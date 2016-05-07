//====================================================================================
//     The MIT License (MIT)
//
//     Copyright (c) 2011 Kapparock LLC
//
//     Permission is hereby granted, free of charge, to any person obtaining a copy
//     of this software and associated documentation files (the "Software"), to deal
//     in the Software without restriction, including without limitation the rights
//     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//     copies of the Software, and to permit persons to whom the Software is
//     furnished to do so, subject to the following conditions:
//
//     The above copyright notice and this permission notice shall be included in
//     all copies or substantial portions of the Software.
//
//     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//     THE SOFTWARE.
//====================================================================================
#include "mtintf.h"
#include "kdev.h"
#include "kglobal.h"
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <termios.h>
#include <fcntl.h>
#include <pthread.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

//#define INBOUND 1
//#define OUTBOUND 0
#define SERIAL_BUFFER_MAX 512
#define SERIAL_BUFFER_MAX_MASK 511
#define PACKET_BUFFER_MAX 256
#define MIN_PACKET_LEN 5

static uint8_t SerBufIn[SERIAL_BUFFER_MAX];//local serial buffer
static int SerBufInHead, SerBufInTail, SerBufInLen;
pthread_mutex_t SerBufInWrME,SerBufInRdME;
pthread_mutexattr_t SerBufInWrMEAttr, SerBufInRdMEAttr;  
pthread_mutex_t SerBufInLenME;
pthread_mutexattr_t SerBufInLenMEAttr;  

static int serial_file_desc;
static void SerialInit();
static struct termios old_attribs;
static void SetSerialHandler();
//static void SerialSend();
static void SerialRcv(int sig, siginfo_t *siginfo, void *context);
static struct sigaction newact, oldact;
static void *MessageAssembler(void *message);
static void (*ProcessIncommingMessage)(uint8_t *pBuf)=NULL;
static sem_t serial_send_sem;

#define MT_INTF_SIG SIGRTMIN + 14

void SetSerialHandler()
{
	int oflags;
	newact.sa_sigaction = &SerialRcv;
	newact.sa_flags = SA_SIGINFO;

	// 2013-11-12
	fcntl(serial_file_desc, F_SETSIG, MT_INTF_SIG);
	// 2013-11-12
	sigaction(MT_INTF_SIG, &newact, &oldact);
	fcntl(serial_file_desc, F_SETOWN, getpid()); // set owner to the current process
	oflags = fcntl(serial_file_desc, F_GETFL);
	fcntl(serial_file_desc, F_SETFL, oflags | FASYNC);
}

void MTIntfInit(void (*callback)(uint8_t*))
{
	//****************************************************
	SerBufInHead = 0;
	SerBufInTail = 0;
	SerBufInLen = 0;
	pthread_mutexattr_init(&SerBufInWrMEAttr);
	pthread_mutexattr_init(&SerBufInRdMEAttr);
	pthread_mutex_init(&SerBufInWrME, &SerBufInWrMEAttr);
	pthread_mutex_init(&SerBufInRdME, &SerBufInRdMEAttr);
	pthread_mutexattr_init(&SerBufInLenMEAttr);
	pthread_mutex_init(&SerBufInLenME, &SerBufInLenMEAttr);
	//****************************************************
	SerialInit();
	SetSerialHandler();	

	ProcessIncommingMessage = callback;
	sem_init(&serial_send_sem, 0, 1);
}
void MTIntfClose()
{
	if(tcsetattr(serial_file_desc, TCSAFLUSH, &old_attribs) < 0) {
		syslog(LOG_INFO, "restore serial port failed : %s", strerror(errno));
	}
	close(serial_file_desc);
	sigaction(SIGIO, &oldact, NULL);
	pthread_mutexattr_destroy(&SerBufInWrMEAttr);
	pthread_mutex_destroy(&SerBufInWrME);
}

void SerialRcv(int sig, siginfo_t *siginfo, void *context) {
	int len, SerBufAva, SerTailPtr;
	pthread_t thread;
	pthread_mutex_lock(&SerBufInLenME);

	do {
		// compute the first postion in the local buffer that is available for writing
		SerTailPtr = SERIAL_BUFFER_MAX_MASK & (SerBufInHead + SerBufInLen);  
		// compute the buffer size available for the writing in one shot
		SerBufAva = SERIAL_BUFFER_MAX - SerTailPtr;
		// copy kernel serial buffer into the local serial buffer
		len = read(serial_file_desc, SerBufIn + SerTailPtr, SerBufAva);
		// update buffer information 
		if (len >= 0) {
			SerBufInLen += len;
		}
		// Determine whether to read more
	} while (len > 0);
	pthread_mutex_unlock(&SerBufInLenME);
	pthread_create(&thread, NULL, MessageAssembler, (void *)NULL);
	pthread_detach(thread);
}

void *MessageAssembler(void *message)
{
	int SerBufHeadTemp, Offset = 0;
	int FrameLen;// TempLen;
	uint8_t PacketBuffer[PACKET_BUFFER_MAX]; 
	
	while (SerBufInLen >= MIN_PACKET_LEN) {
		pthread_mutex_lock(&SerBufInLenME);
		SerBufHeadTemp = SerBufInHead;
		while (SerBufIn[SerBufHeadTemp] != MT_SOF) {
			Offset++;
			SerBufHeadTemp = (SerBufInHead + Offset) & SERIAL_BUFFER_MAX_MASK;
			if (SerBufInHead == SerBufHeadTemp || SerBufInLen < Offset + MIN_PACKET_LEN) {
				pthread_mutex_unlock(&SerBufInLenME);
				return NULL;
			}
		}

		SerBufHeadTemp = (SerBufHeadTemp+1) & SERIAL_BUFFER_MAX_MASK;
		FrameLen = (int)SerBufIn[SerBufHeadTemp] + 4;

		//check the expected packet length against the current local buffer size
		if (SerBufInLen < FrameLen + 1 + Offset) {
			pthread_mutex_unlock(&SerBufInLenME);
			return NULL;
		}

		//start of packet checking processing
		int TempLen = SERIAL_BUFFER_MAX - SerBufHeadTemp;
		if (FrameLen <= TempLen) {
			memcpy(PacketBuffer, SerBufIn+SerBufHeadTemp, FrameLen);
		} else {
			memcpy(PacketBuffer, SerBufIn+SerBufHeadTemp, TempLen);
			// Wrap back
			memcpy(PacketBuffer + TempLen, SerBufIn, FrameLen - TempLen);
		}

		SerBufInLen -= (FrameLen + 1 + Offset);
		SerBufInHead = (SerBufHeadTemp + FrameLen) & SERIAL_BUFFER_MAX_MASK;

		pthread_mutex_unlock(&SerBufInLenME);

		uint8_t chk=0;
		for (int i=0;i<FrameLen; i++) {chk ^= PacketBuffer[i];}

		if (chk == 0) {
			ProcessIncommingMessage(PacketBuffer);
		}
	}
	return NULL;
}

void SerialSend(uint8_t *pbuf, size_t len) {
	sem_wait(&serial_send_sem);
	write(serial_file_desc, (void *)pbuf, (size_t)len);
	sem_post(&serial_send_sem);
}

void SerialInit() {
	struct termios stdio;
	
	serial_file_desc  = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY); // <- don't block the IO port

	if (serial_file_desc < 0 ) {return;}
	
	if(!isatty(serial_file_desc)) {
		syslog(LOG_INFO, "%s Not a serial port", SERIAL_DEVICE);
		return;}
	
	if(tcgetattr(serial_file_desc, &stdio) < 0) {return;}
	old_attribs = stdio;
	
//	if(cfsetispeed(&stdio, B38400) < 0 || cfsetospeed(&stdio, B38400) < 0)
	if(cfsetispeed(&stdio, B115200) < 0 || cfsetospeed(&stdio, B115200) < 0)
	{return;}
 
	stdio.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
	stdio.c_oflag &= ~OPOST;
	stdio.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	stdio.c_cflag &= ~(CSIZE | PARENB);
//	stdio.c_cflag |= CS8 | CREAD | CLOCAL; // CRTSCTS <- absolutely NO CRTSTCS, otherwise it won't work
	stdio.c_cflag |= CS8 | CREAD; // CRTSCTS <- absolutely NO CRTSTCS, otherwise it won't work
	stdio.c_cc[VMIN]  = 1;
	stdio.c_cc[VTIME] = 0;
	stdio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
	stdio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	stdio.c_cc[VERASE]   = 0;     /* del */
	stdio.c_cc[VKILL]    = 0;     /* @ */
	stdio.c_cc[VEOF]     = 0;     /* Ctrl-d */
	stdio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	stdio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
	stdio.c_cc[VSWTC]    = 0;     /* '\0' */
	stdio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
	stdio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	stdio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	stdio.c_cc[VEOL]     = 0;     /* '\0' */
	stdio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	stdio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	stdio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	stdio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	stdio.c_cc[VEOL2]    = 0;     /* '\0' */

//	ioctl(serial_file_desc, TIOCEXCL);
	if (tcflush(serial_file_desc, TCIOFLUSH) <0)
	{
		return;
	}
	if(tcsetattr(serial_file_desc, TCSAFLUSH, &stdio) < 0) {return;}

}
void resetInterface()
{
	close(serial_file_desc);
	SerialInit();
	SetSerialHandler();
}

