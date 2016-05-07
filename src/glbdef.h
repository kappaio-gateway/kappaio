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
#ifndef GLBDEF_H
#define GLBDEF_H
#include "kglobal.h"
#include <features.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <syslog.h>
#include <netinet/in.h>
#include "zbconst.h"
#include "MT_RPC.h"


#define MQNAME "/mqq"
#define MTQI_MAXMSG 10
#define MTQI_MSGSIZE 256
#define MTQO_MAXMSG 10
#define MTQO_MSGSIZE 256
#define DBGQ 12

#define LQI "/etc/rssq/lqi"
#define LQO 14
#define MTQI "/etc/rssq/mtqi"
#define MTQO "/etc/rssq/mtqo"
#define CMQI "/etc/rssq/cmqi"
#define CMQO "/etc/rssq/cmqo"
#define PIDF "/var/run/rsserial.pid"
#define PMODE 0666 // everyone has access to the queue
#define PPMODE S_IRGRP|S_IROTH|S_IRUSR|S_IWGRP|S_IWOTH|S_IWUSR
#define CLOCKID CLOCK_REALTIME
#define CLOCKSIG SIGRTMIN
#define SIG CLOCKSIG
#define MAXQNUM 32 
#define MAXMSGSZ 258
#define MTQIWFLG IPC_CREAT|S_IWUSR|S_IRUSR
#define MTQIRFLG S_IRGRP|S_IROTH
#define MTQIPRIO 1
#define MTQOWFLG IPC_CREAT|S_IWUSR|S_IRUSR
#define MTQORFLG S_IRGRP|S_IROTH
#define MTQOPRIO 1
#define DBGQWFLG IPC_CREAT|S_IWUSR|S_IRUSR|S_IROTH
#define DBGQRFLG S_IRGRP|S_IROTH|S_IRUSR
#define LQIFLG IPC_CREAT|S_IWUSR|S_IRUSR|S_IROTH|S_IWOTH
#define LQIFLGW S_IWOTH
#define LQOFLG IPC_CREAT|S_IWUSR|S_IRUSR|S_IROTH

//OpenWrt backfire does NOT support CONFIG_POSIX_MQUEUE, use SV4 message queue for now
/*
#define S_IRUSR 0x00000100   //Allow the owner of the message queue to read from it.
#define S_IWUSR 0x00000080   //Allow the owner of the message queue to write to it.
#define S_IRGRP 0x00000020   //Allow the group of the message queue to read from it.
#define S_IWGRP 0x00000010   //Allow the group of the message queue to write to it.
#define S_IROTH 0x00000004   //Allow others to read from the message queue.
#define S_IWOTH 0x00000002  //Allow others to write to the message queue.
#define IPC_CREAT 0x00000200 //Create the message queue if it does not exist.
#define IPC_EXCL 0x00000400  //Return an error if the IPC_CREAT flag is set and the message queue already exists.
*/



/*typedef uint8_t uint8;
typedef unsigned int unsigned short;
typedef unsigned int word16;*/

typedef struct {
	long int mtype;
	char msg[MAXMSGSZ];
} message_t;
typedef struct {
		int thread_id;
		uint8_t *buf;
} thread_data_t;

#endif
//#define szmessage size_t(sizeof(message_t) - sizeof(long int))
//Enable/disable modules



