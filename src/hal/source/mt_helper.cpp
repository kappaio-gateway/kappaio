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
#include "mt_helper.h"
#include "kglobal.h"
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <syslog.h>
#include "mtintf.h"

#define NUM_SUBSYSTEMS 16

static struct sys_channel_s
{
	sem_t sem;
	uint8_t command;
	uint8_t *return_buffer;
} sys_channel;

struct mtMessageResource_s
{
	uint8_t cmdType;
	uint8_t cmdId;
	uint8_t *pData;
	uint8_t dataLength;
	uint8_t rspCmdId;
	mtMessage_t *request;
	mtMessage_t *response;
	void *returnBuffer;
	size_t bufferSize;
	sem_t sem;
	void (*callback)(mtMessage_t*, void*, size_t);
	pthread_mutex_t mutex;
	pthread_cond_t conditionVar;
	pthread_attr_t attr;
	pthread_t sendThread;
	pthread_t recvThread;
	// send and receive thread has separate return status to prevent race condition
	int *recvStatus;
	int *sendStatus;
};

static struct mtMessageResource_s mtMessageResource[NUM_SUBSYSTEMS];
//static struct mtMessageResource_s OutOfBandmtMessageResource;
static void *sendThread(void *td);
static void *recvThread(void *td);
int mtOOBSend(mtMessage_t *mtMessage, uint8_t rspCmdId, void *returnBuffer, size_t bufferSize, void (*callback)(mtMessage_t*,void*,size_t));
int mtOOBRecv(uint8_t *incoming);
static sem_t OutOfBandSem;

void mtHelperInit()
{
	for (int i=0;i<NUM_SUBSYSTEMS-1;i++)
	{
		pthread_mutex_init(&(mtMessageResource[i].mutex), NULL);
		pthread_cond_init(&(mtMessageResource[i].conditionVar), NULL);
		sem_init(&(mtMessageResource[i].sem), 0, 1);
		pthread_attr_init(&(mtMessageResource[i].attr));
		pthread_attr_setdetachstate(&(mtMessageResource[i].attr), PTHREAD_CREATE_JOINABLE);
		mtMessageResource[i].callback=NULL;
	}
	setOutOfBandMessageCallBack(mtOOBRecv);
	sem_init(&OutOfBandSem, 0, 1);
}

/* Out of band data */
int __MT_OOB_SEND__(mtMessage_t *mtMessage, uint8_t *returnBuffer, size_t bufferSize, void (*callback)(mtMessage_t*,void*,size_t))
{
	mtMessage_t rsp;

	mtOOBSend(mtMessage, mtMessage->cmdId, (void*)&rsp, sizeof(rsp), callback);
	return 0;
}

int mtOOBSend(mtMessage_t *mtMessage, uint8_t rspCmdId, void *returnBuffer, size_t bufferSize, void (*callback)(mtMessage_t*,void*,size_t))
{
	sem_wait(&OutOfBandSem);
	mtBuildAndSendCmd(mtMessage);
	return 0;
}

int mtOOBRecv(uint8_t *incoming)
{
	mtMessage_t *x = (mtMessage_t*)(incoming -1);
	if (x->cmdType ==0x61 && x->cmdId == MT_SYS_WDT_KICK )
	{
		sem_post(&OutOfBandSem);
		return 0;
	} else
	{
		return -1;
	}
}
/* Out of band data */










int __MTSEND__(mtMessage_t *mtMessage, uint8_t *returnBuffer, size_t bufferSize, void (*callback)(mtMessage_t*,void*,size_t))
{
	mtMessage_t rsp;

	if (returnBuffer!=NULL)
	{
		if (mtSend(mtMessage, mtMessage->cmdId, (void*)&rsp, sizeof(rsp), callback) <0)
		{
			return -1;
		}
		memcpy(returnBuffer, (void*)&(rsp.pData), bufferSize);
	} else
	{
		return mtSend(mtMessage, mtMessage->cmdId, NULL, 0, callback);
	}
	return 0;
}

int mtSend(mtMessage_t *mtMessage, uint8_t rspCmdId, void *returnBuffer, size_t bufferSize, void (*callback)(mtMessage_t*,void*,size_t))
{
	int rtn = -1;
	if (returnBuffer==NULL && callback==NULL)
	{
		mtBuildAndSendCmd(mtMessage);
		rtn = 0;
	}
	else
	{
		uint8_t subsystem = mtMessage->cmdType & 0x1f;
		sem_wait(&(mtMessageResource[subsystem].sem));

		mtMessageResource[subsystem].returnBuffer = returnBuffer;
		mtMessageResource[subsystem].bufferSize = bufferSize;
		mtMessageResource[subsystem].callback= callback;
		mtMessageResource[subsystem].request = mtMessage;
		mtMessageResource[subsystem].rspCmdId =rspCmdId;
		mtMessageResource[subsystem].sendStatus = &rtn;
		pthread_create(&mtMessageResource[subsystem].sendThread, \
						&mtMessageResource[subsystem].attr, \
						&sendThread, \
						(void *)(&mtMessageResource[subsystem]));
		pthread_join(mtMessageResource[subsystem].sendThread, NULL);
		sem_post(&(mtMessageResource[subsystem].sem));
	}
	return rtn;
}

int mtRecv(uint8_t *incoming)
{
	mtMessage_t *x = (mtMessage_t*)(incoming -1);
	uint8_t subsystem = x->cmdType & 0x1f;
	int returnStatus = -1;
	if (mtMessageResource[subsystem].rspCmdId != x->cmdId)
	{
		return -1;
	}
	mtMessageResource[subsystem].response = x;
//	vTraceLog("cmdType: %02x, id: %02x, rspCmdId: %02x", x->cmdType, x->cmdId, mtMessageResource[subsystem].rspCmdId);

	if (sem_trywait(&(mtMessageResource[subsystem].sem)) == 0)
	{
		sem_post(&(mtMessageResource[subsystem].sem));
		return -1;
	} else {
		mtMessageResource[subsystem].recvStatus = &returnStatus;
//		vTraceLog("starting recv thread: %02x", mtMessageResource[subsystem].response->pData[0]);
		pthread_create(&(mtMessageResource[subsystem].recvThread), \
				&(mtMessageResource[subsystem].attr), \
						&recvThread, \
						(void *)(&mtMessageResource[subsystem]));
		pthread_join(mtMessageResource[subsystem].recvThread, NULL);
	}
	return returnStatus;
}

void *recvThread(void *td)
{
	struct mtMessageResource_s *info =(struct mtMessageResource_s*)td;
	pthread_mutex_lock(&(info->mutex));
	if (info->callback==NULL)
	{
		if (info->returnBuffer==NULL)
		{
			*(info->sendStatus)=1; // message is sent and response received... just no buffer to hold the response
			*(info->recvStatus)=1;
		} else
		{
			memcpy(info->returnBuffer, (void*)(info->response), info->bufferSize);
			*(info->sendStatus) = 0;
			*(info->recvStatus) = 0;
		}
	}
	else
	{
		info->callback(info->response,info->returnBuffer, info->bufferSize);
		*(info->sendStatus) = 0;
		*(info->recvStatus) = 0;
	}

	pthread_cond_signal(&(info->conditionVar));
	pthread_mutex_unlock(&(info->mutex));

	return NULL;
}

void *sendThread(void *td)
{
	struct mtMessageResource_s *info =(struct mtMessageResource_s*)td;
	pthread_mutex_lock(&(info->mutex));
	mtBuildAndSendCmd(info->request);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 1;
	int rtn=pthread_cond_timedwait(&(info->conditionVar), &(info->mutex), &ts);

	if (rtn !=0)
	{
		LOG_ERROR_MESSAGE("timewait error in mt : %d", rtn);
		resetInterface();
	}
	pthread_mutex_unlock(&(info->mutex));
	return NULL;
}

//int request_channel(uint8_t command)
//{
//	long t=0;
//	int rtn = -1;
//	struct timespec interval, rem;
//
//	while (t<1000000) // 1 secs
//	{
//		interval.tv_sec = 0;
//		interval.tv_nsec = 1000000;
//		rem = interval;
//
//		if (sem_trywait(&sys_channel.sem)==0)
//		{
//			sys_channel.command = command;
//			rtn = 0;
//			break;
//		}
//		nanosleep(&interval, &rem);
//		t+=1000;
//	}
//	return rtn;
//}

//uint8_t *getChannelBuffer(uint8_t command)
//{
//	return sys_channel.return_buffer;
//}

void release_channel()
{
	sys_channel.command = 0xff;
	sem_post(&sys_channel.sem);
}

//int wait_on_channel(long tous)
//{
//	int rtn =-1;
//	long t=0;
//	struct timespec ts={0,1000}; //1 ms
//	while (t<tous)
//	{
//		if (sem_timedwait(&sys_channel.sem, &ts)==0)
//		{
//			rtn = 0;
//			break;
//		}
//		t+=1;
//	}
//	sys_channel.command = 0xff;
//	sem_post(&sys_channel.sem);
//	return rtn;
//}
