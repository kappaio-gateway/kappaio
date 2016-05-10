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
#include "kutil.h"
#include <string.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <sys/un.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <fstream>

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#error Byte order (endianness) is not defined
#endif

#if defined(__LITTLE_ENDIAN__) && defined(__BIG_ENDIAN__)
#error big endian and little endian are both
#endif

int kNanosleep(struct timespec *_interval)
{
	struct timespec interval, remaining;

	interval = *_interval;
	remaining = *_interval;
	size_t count = 0;

	while(nanosleep(&interval,&remaining)!=0 && count < 0xffffffff)
	{
		interval = remaining;
		count++;
	}

	return count<0xffffffff?0:-1;
}

int kSleep(size_t interval)
{
	struct timespec _interval = {static_cast<__time_t>(interval),0};
	return kNanosleep(&_interval);
}

int kPrintMem(char *buffer, void *mem, size_t memLen)
{
	char *_buffer = buffer;

#if defined(__BIG_ENDIAN__)
	for (size_t i=memLen-1;i>=0;i--)
	{
		_buffer += sprintf(_buffer, "%02x", *(uint8_t*)(mem + i));
	}

#elif defined(__LITTLE_ENDIAN__)

	for (size_t i=0;i<memLen;i++)
	{
		_buffer += sprintf(_buffer, "%02x", *(uint8_t*)(mem + i));
	}
#endif

	return memLen << 1;
}

int kByteToHexString(char *outputString, uint8_t *byte, size_t byteLength)
{
	char *buffer = outputString;
	for (size_t i = 0 ; i < byteLength ; i++)
	{
		buffer += sprintf(buffer, "%02x", *(byte + i));
	}
	return byteLength << 1;
}

int kHexStringToByte(uint8_t *outputByte, const char *hexString, size_t stringLength)
{
	uint8_t *pdst = outputByte;
	size_t asrclen = stringLength;
	char *psrc = (char *)hexString;
	while (asrclen--)
	{
		*pdst = kCharToInt(*psrc++)<<4;
		*pdst++ |= kCharToInt(*psrc++);
	}

	return stringLength >> 1;
}

int kMemToInt(void* destination, void* source, size_t size)
{
#if defined(__BIG_ENDIAN__)
	memcpyr(destination, source, size);

#elif defined(__LITTLE_ENDIAN__)
	memcpy(destination, source, size);

#endif
	return size;
}

int kIntToMem(void* destination, void* source, size_t size)
{
#if defined(__BIG_ENDIAN__)
		memcpyr(destination, source, size);
#elif defined(__LITTLE_ENDIAN__)
		memcpy((void*)destination, (void*)source, size);
#endif
		return size;
}

void *memcpyr(void * destination, const void * source, size_t num )
{
	char *pdst = (char *)destination;
	char const *psrc = (char const *) source;
	size_t i = 0;
	size_t numc = num;
	while (numc--)
	{
		*pdst++ = *(psrc + numc);
	}
	return destination;
}
/////////////////////////////////////////////////////////////////////////////////////////
//
// int kStrToInt(void* destination, const char* source, size_t destination_size)
//
// Converts the a valid ASCII hex string to a uint8_t array
//
// return :
// destination_size * 2 : conversion performed successfully
// -1                   : source contains at least one invalid character and operation is abort

int kStrToInt(void* destination, const char* source, size_t destination_size)
{
	uint8_t *pdst = static_cast<uint8_t*>(destination);
	size_t asrclen = destination_size;
	int returnStatus = destination_size << 1;

	#if defined(__BIG_ENDIAN__)
		char *psrc = (char*)source;
		while (asrclen--)
		{
			uint8_t x;
			if ((x = kCharToInt(*psrc++)) > 0x0f) { returnStatus = -1; break; }
			else *pdst = x << 4;
			if ((x = kCharToInt(*psrc++)) > 0x0f) { returnStatus = -1; break; }
			else *pdst++ |= x;
		}

	#elif defined(__LITTLE_ENDIAN__)
		char *psrc = (char*)(source + asrclen *2 -1);
		while (asrclen--)
		{
			uint8_t x;
			if ((x = kCharToInt(*psrc--)) > 0x0f) { returnStatus = -1; break; }
			else *pdst = x;
			if ((x = kCharToInt(*psrc--)) > 0x0f) { returnStatus = -1; break; }
			else *pdst++ |= (x << 4);
		}
	#endif

	return returnStatus;
}

//char *toHexStr(char *buffer, void *number, size_t size)
int kIntToStr(char* destination, const void* source, size_t size)
{
	char *_buffer = destination;

#if defined(__BIG_ENDIAN__)
	for (size_t i=0;i<size;i++)
	{
		_buffer += sprintf(_buffer, "%02x", *(uint8_t*)(source + i));
	}

#elif defined(__LITTLE_ENDIAN__)
	for (int i=size-1;i>=0;i--)
	{
		_buffer += sprintf(_buffer, "%02x", *(uint8_t*)(source + i));
	}
#endif
	return 0;
}

char *kToHexStr(char* destination, const void* source, size_t size)
{
	kIntToStr(destination, source, size);
	return destination;
}

int bytes_to_uint(void* destination, void* source, size_t size)
{
	return kMemToInt(destination, source, size);
}
/////////////////////////////////////////////////////////////////////////////////////////
//
// uint8_t kCharToInt(char c)
//
// Perform conversion between a single ASCII representation of hex integer to uint8_t
//
// Return :
// static_cast<uint8_t>(0) - static_cast<uint8_t>(15)
// Any other value should be considered as invalid input
uint8_t kCharToInt(char c)
{
	if (c >= 0x30 && c <= 0x39) return static_cast<uint8_t>(c - 0x30);
	if (c >= 0x41 && c <= 0x46) return static_cast<uint8_t>(c - 55);
	if (c >= 0x61 && c <= 0x66) return static_cast<uint8_t>(c - 87);
	return 16;
}

klock_c::klock_c()
:shLock(NULL),exLock(NULL), shLockCnt(0)
{
	this->shLock = (sem_t*)malloc(sizeof(sem_t));
	this->exLock = (sem_t*)malloc(sizeof(sem_t));
	this->shLockPriv = (sem_t*)malloc(sizeof(sem_t));

	sem_init(this->shLock,0,1);
	sem_init(this->shLockPriv,0,1);
	sem_init(this->exLock,0,1);

}

klock_c::~klock_c()
{
	sem_destroy(this->shLock);
	sem_destroy(this->shLockPriv);
	sem_destroy(this->exLock);

	free((void*)this->shLock);
	free((void*)this->shLockPriv);
	free((void*)this->exLock);
}

int klock_c::shLockAcquire()
{
	sem_wait(this->shLockPriv);
	if (sem_trywait(this->exLock) <0)
	{
		sem_wait(this->shLock);
		if (this->shLockCnt <= 0)
		{
			sem_post(this->shLock);
			sem_wait(this->exLock);
			sem_wait(this->shLock);
			this->shLockCnt++;
			sem_post(this->shLock);
		} else
		{	this->shLockCnt++;
			sem_post(this->shLock);
		}
	} else
	{
		sem_wait(this->shLock);
		this->shLockCnt++;
		sem_post(this->shLock);
	}
	sem_post(this->shLockPriv);
	return 0;
}

int klock_c::shLockRelease()
{
	int rtn = -1;
	sem_wait(this->shLock);
	if (this->shLockCnt == 0)
	{
		rtn = -1;
	}

	if (this->shLockCnt ==1)
	{
		this->shLockCnt = 0;
		sem_post(this->exLock);
		rtn = 0;
	}

	if (this->shLockCnt >1)
	{
		this->shLockCnt--;
		rtn =  0;
	}

	sem_post(this->shLock);
	return rtn;
}

int klock_c::exLockAcquire()
{
	sem_wait(this->exLock);
	return 0;
}

int klock_c::exLockRelease()
{
	int rtn = -1;
	sem_wait(this->shLock);
	if (this->shLockCnt == 0)
	{
		sem_post(this->exLock);
		rtn = 0;
	}
	sem_post(this->shLock);
	return rtn;
}

int klock_c::lockAcquire()
{
	return this->exLockAcquire();
}

int klock_c::lockRelease()
{
	return this->exLockRelease();
}

void gen_random(char *s, const int len) {

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	srand (time(NULL));
	for (int i = 0; i < len; ++i) { s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];}

	s[len] = 0;
}

int Base64Encode(const unsigned char* message, size_t messageSize, char** buffer) { //Encodes a string to base64
  BIO *bio, *b64;
  FILE* stream;
  //~ int encodedSize = 4*ceil((double)strlen(message)/3);
  int encodedSize = 4*ceil((double)messageSize/3);
  *buffer = (char *)malloc(encodedSize+1);

  stream = fmemopen(*buffer, encodedSize+1, "w");
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  bio = BIO_push(b64, bio);
  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line

  BIO_write(bio, message, messageSize);
  BIO_flush(bio);
  BIO_free_all(bio);
  fclose(stream);

  //~ return (0); //success
  return encodedSize; //success
}

#define WIDGETZIPBUFFER "/tmp/kappaio-temp-files/widgetbuf.tgz"
#define RESULTANTZIP    "/tmp/kappaio-temp-files/resultantzip.tgz"

int EncodeDir(const char *fileName, char **pbuffer)
{
	char command[256];
	size_t commandPos = 0;

	commandPos += sprintf(command, "mkdir -p /tmp/kappaio-temp-files/ && tar -cz -f %s -C %s . > /tmp/errorlog 2>&1", WIDGETZIPBUFFER, fileName);

	FILE *tgzCommand = popen(command, "r");
	pclose(tgzCommand);

	int fd = open(WIDGETZIPBUFFER,O_RDONLY);
	struct stat st;
	stat(WIDGETZIPBUFFER, &st);
	size_t size = st.st_size;
	unsigned char *buffer = (unsigned char*)malloc(size+1);
	read(fd, buffer, size);

	int encodedSize = Base64Encode(buffer, size, pbuffer);

	free(buffer);
	close(fd);

	return encodedSize;
}

int calcDecodeLength(const char* b64input) { //Calculates the length of a decoded base64 string
  int len = strlen(b64input);
  int padding = 0;

  if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len-1] == '=') //last char is =
    padding = 1;

  return (int)len*0.75 - padding;
}

int Base64Decode(char* b64message, unsigned char** buffer) { //Decodes a base64 encoded string
  BIO *bio, *b64;
  int decodeLen = calcDecodeLength(b64message), len = 0;

  *buffer = (unsigned char*)malloc(decodeLen+1);
  FILE* stream = fmemopen(b64message, strlen(b64message), "r");

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new_fp(stream, BIO_NOCLOSE);
  bio = BIO_push(b64, bio);
  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
  len = BIO_read(bio, *buffer, strlen(b64message));
    //Can test here if len == decodeLen - if not, then return an error
  (*buffer)[len] = '\0';

  BIO_free_all(bio);
  fclose(stream);

  //~ return (0); //success
  return decodeLen; //success
}

int DecodeToDir(const char *dir, char *buffer)
{
	using namespace std;
	unsigned char* base64DecodeOutput;
	int decodedSize = Base64Decode(buffer, &base64DecodeOutput);

	//char tempZip[128];
	string tempZip = string{dir} + "/tempZfjf80as98f09u3r380fdsf9spoifdsjip.tgz";

	//sprintf(tempZip, "%s/tempZfjf80as98f09u3r380fdsf9spoifdsjip.tgz",dir);
	int fd = open(tempZip.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	write(fd,base64DecodeOutput, decodedSize);
	close(fd);
	free(base64DecodeOutput);

	//char commandStr[256];
	//size_t commandStrLen = 0;

	//==========================================
	string cmd = string{"tar -xzf "} + tempZip + " -C " + dir;
	//ofstream{"/tmp/cmd.txt", ofstream::out} << cmd;
	//==========================================
	//return 0;
	//commandStrLen = sprintf(commandStr,"tar -xzf %s -C %s", tempZip, dir);
	system(cmd.c_str());

	cmd = string{"rm "} + tempZip;
	//commandStrLen = sprintf(commandStr,"rm %s", tempZip);
	system(cmd.c_str());
	return 0;
}

int openUnixServerSock(const char *name, const char *service)
{
	struct sockaddr_un local;
	int len;
	int on = 1;

	int socketfd = socket(PF_UNIX,SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		syslog(LOG_INFO,"socket() open failed: %s", strerror(errno));
		close(socketfd);
		return -1;
	}
	memset(&local, 0, sizeof(struct sockaddr_un));
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, service);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	int flags = fcntl(socketfd, F_GETFL, 0);
	fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);

	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	if (bind(socketfd, (struct sockaddr *)&local, sizeof(struct sockaddr_un)) <0)
	{
		syslog(LOG_INFO,"bind() failed with service  : %s , for reason : %s", service, strerror(errno));
		close(socketfd);
		return -1;
	}

	return socketfd;
}


int openUnixClientSock(const char *name, const char *service)
{
	struct sockaddr_un local;
	int len;
	int on = 1;

	int socketfd = socket(PF_UNIX,SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		syslog(LOG_INFO,"socket() open failed: %s", strerror(errno));
		close(socketfd);
		return -1;
	}
	memset(&local, 0, sizeof(struct sockaddr_un));
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, service);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	int flags = fcntl(socketfd, F_GETFL, 0);
	fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	int status = connect(socketfd,(struct sockaddr *)&local, len);

	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(socketfd, &fdset);

	struct timeval tv;
	tv.tv_sec = 1;

	int retVal;

	do {
			retVal = select(socketfd+1, NULL, &fdset, NULL, &tv);
	} while (retVal < 0 && errno == EINTR && tv.tv_usec != 0 && tv.tv_sec!=0);

	if (retVal == 1)
	{
		int so_error;
		socklen_t slen = sizeof so_error;
		getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &so_error, &slen);
		if (so_error == 0)
		{
			return socketfd;
		} else
		{
			syslog(LOG_INFO,"connection failed");
			close(socketfd);
			return -1;
		}
	} else {
		syslog(LOG_INFO,"no socket, return from select() = %d", retVal);
		close(socketfd);
		return -1;
	}

	return -1;
}
//int sockAcceptWait(int socketfd, int timeoutSec)
//{
//	fd_set fdset;
//	FD_ZERO(&fdset);
//	FD_SET(socketfd, &fdset);
//
//	struct timeval tv;
//	//tv.tv_sec = 1800;
//	tv.tv_sec = (__time_t)timeoutSec;
//	tv.tv_usec = 0;
//
//	int retVal;
//	int incoming;
//	struct sockaddr_in CliAddr;
//	socklen_t CliAddrLen;
//
//	if (listen(socketfd, 128) < 0) {
//		syslog(LOG_INFO, "listen() failed in __portListen");
//	}
//
//	do {
//		retVal = select(socketfd + 1, &fdset, NULL, NULL, &tv);
//	} while (retVal < 0 && errno == EINTR && tv.tv_usec != 0 && tv.tv_sec!=0);
//
//	if (retVal == 1 && FD_ISSET(socketfd, &fdset))
//	{
//		incoming = accept(socketfd,(struct sockaddr *) &CliAddr, &CliAddrLen);
//		return incoming;
//	} else {
//		syslog(LOG_INFO,"select() returned with code : %d, errno: %d, message %s", retVal, errno,strerror(errno));
//		return -1;
//	}
//}
//
//int recvWait(int socketfd, void* buffer, size_t expectLen, int flags, int timeoutSec )
//{
//	fd_set fdset;
//	FD_ZERO(&fdset);
//	FD_SET(socketfd, &fdset);
//
//	struct timeval tv;
//	//tv.tv_sec = 1800;
//	tv.tv_sec = (__time_t)timeoutSec;
//	tv.tv_usec = 0;
//
//	int retVal;
//	int incoming;
//	struct sockaddr_in CliAddr;
//	socklen_t CliAddrLen;
//
//	do {
//		retVal = select(socketfd + 1, &fdset, NULL, NULL, &tv);
//	} while (retVal < 0 && errno == EINTR && tv.tv_usec != 0 && tv.tv_sec!=0);
//
//	if (retVal == 1 && FD_ISSET(socketfd, &fdset))
//	{
//		size_t recvLen = recv(socketfd, buffer, expectLen, flags);
//		return recvLen;
//	} else {
//		syslog(LOG_INFO,"select() returned with code : %d, errno: %d, message %s", retVal, errno,strerror(errno));
//		return -1;
//	}
//}
