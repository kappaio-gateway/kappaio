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
#include "mtdebug.h"
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <semaphore.h>
#include <stdarg.h>
#include <jansson.h>
#include "kglobal.h"
#include <string.h>
#include "MT_RPC.h"
#include <unistd.h>
#include "kutil.h"
#include <syslog.h>
#define DBGQ_MAXMSG 10
#define DBGQ_MSGSIZE 256

#define UPDATE_INTERVAL_US 1500000

//static int SendDebugMsgBeforeInit(uint8_t *pBuf, size_t BufLen) {return SUCCESS;}
//static int (*pSendDebugMsg)(uint8_t *pBuf, size_t BufLen) = SendDebugMsgBeforeInit;

//typedef struct
//{
//	long long int pid;
//	char tcomm[PATH_MAX];
//	char state;
//	long long int ppid;
//	long long int pgid;
//	long long int sid;
//	long long int tty_nr;
//	long long int tty_pgrp;
//	long long int flags;
//	long long int min_flt;
//	long long int cmin_flt;
//	long long int maj_flt;
//	long long int cmaj_flt;
//	long long int utime;
//	long long int stimev;
//	long long int cutime;
//	long long int cstime;
//	long long int priority;
//	long long int nicev;
//	long long int num_threads;
//	long long int it_real_value;
//	unsigned long long start_time;
//	long long int vsize;
//	long long int rss;
//	long long int rsslim;
//	long long int start_code;
//	long long int end_code;
//	long long int start_stack;
//	long long int esp;
//	long long int eip;
//	long long int pending;
//	long long int blocked;
//	long long int sigign;
//	long long int sigcatch;
//	long long int wchan;
//	long long int zero1;
//	long long int zero2;
//	long long int exit_signal;
//	long long int cpu;
//	long long int rt_priority;
//	long long int policy;
//} procstat_t;
//static int getprocstat(procstat_t *procstat);

//struct json_debug_log_s
//{
//	unsigned int seqnum;
//	sem_t *semaphore;
//	json_t *debug_log;
//	json_t *proc;
//	json_t *af_message;
//	json_t *system_message;
//	json_t *tod;
//} json_debug_log;
// private functions

//static void json_debug_proc();
//static inline void json_debug_dump();
//static void json_debug_dump_thread(void *td);
//
//void json_debug_dump()
//{
//	// dump section
//	sem_wait(json_debug_log.semaphore);
//	json_dump_file(json_debug_log.debug_log, JSON_DEBUG_PATH, JSON_INDENT(1));
//	json_array_clear(json_debug_log.af_message);
//	json_array_clear(json_debug_log.system_message);
//	sem_post(json_debug_log.semaphore);
//	// update section
//	json_object_set_new(json_debug_log.debug_log,"seqnum",json_integer(json_debug_log.seqnum++));
//	json_object_set_new(json_debug_log.debug_log,"unixtime", json_integer(time(NULL)));
////	json_debug_proc();
//}
//void json_debug_init()
//{
//	json_debug_log.seqnum = 0;
//	json_debug_log.debug_log = json_object();
//	json_debug_log.proc = json_object();
//	json_debug_log.af_message = json_array();
//	json_debug_log.system_message = json_array();
//	json_debug_log.tod = json_array();
//	json_object_set_new(json_debug_log.debug_log,"unixtime", json_integer(time(NULL)));
//	json_object_set_new(json_debug_log.debug_log,"seqnum",json_integer(0));
//	json_object_set_new(json_debug_log.debug_log,"af_message",json_debug_log.af_message);
//	json_object_set_new(json_debug_log.debug_log,"system_message",json_debug_log.system_message);
//	json_object_set_new(json_debug_log.debug_log,"process_status",json_debug_log.proc);
//	// use semaphore to ensure thread safety
//	json_debug_log.semaphore = (sem_t *)malloc(sizeof(sem_t));
//	sem_init(json_debug_log.semaphore, 0, 1);
//}

//void json_debug_proc()
//{
//	procstat_t proc;
//	getprocstat(&proc);
//
//	json_object_set_new(json_debug_log.proc, "pid", json_integer(proc.pid)); // 0
//	json_object_set_new(json_debug_log.proc, "tcomm", json_string(proc.tcomm));// 0
//	json_object_set_new(json_debug_log.proc, "state", json_string(&(proc.state)));// 0
//	json_object_set_new(json_debug_log.proc, "ppid", json_integer(proc.ppid));// 0
//	json_object_set_new(json_debug_log.proc, "pgid", json_integer(proc.pgid));// 0
//	json_object_set_new(json_debug_log.proc, "sid", json_integer(proc.sid));// 0
//	json_object_set_new(json_debug_log.proc, "tty_nr", json_integer(proc.tty_nr));// 0
//	json_object_set_new(json_debug_log.proc, "tty_pgrp", json_integer(proc.tty_pgrp));// 0
//	json_object_set_new(json_debug_log.proc, "flags", json_integer(proc.flags));// 0
//	json_object_set_new(json_debug_log.proc, "min_flt", json_integer(proc.min_flt));// 0
//	json_object_set_new(json_debug_log.proc, "cmin_flt", json_integer(proc.cmin_flt));// 0
//	json_object_set_new(json_debug_log.proc, "maj_flt", json_integer(proc.maj_flt));// 0
//	json_object_set_new(json_debug_log.proc, "cmaj_flt", json_integer(proc.cmaj_flt));// 0
//	json_object_set_new(json_debug_log.proc, "utime", json_integer(proc.utime));// 0
//	json_object_set_new(json_debug_log.proc, "stimev", json_integer(proc.stimev));// 0
//	json_object_set_new(json_debug_log.proc, "cutime", json_integer(proc.cutime));// 0
//	json_object_set_new(json_debug_log.proc, "cstime", json_integer(proc.cstime));// 0
//	json_object_set_new(json_debug_log.proc, "priority", json_integer(proc.priority));// 0
//	json_object_set_new(json_debug_log.proc, "nicev", json_integer(proc.nicev));// 0
//	json_object_set_new(json_debug_log.proc, "num_threads", json_integer(proc.num_threads));// 0
//	json_object_set_new(json_debug_log.proc, "it_real_value", json_integer(proc.it_real_value));// 0
//	json_object_set_new(json_debug_log.proc, "start_time", json_integer(proc.start_time));// 0
//	json_object_set_new(json_debug_log.proc, "vsize", json_integer(proc.vsize));// 0
//	json_object_set_new(json_debug_log.proc, "rss", json_integer(proc.rss));// 0
//	json_object_set_new(json_debug_log.proc, "rsslim", json_integer(proc.rsslim));// 0
//	json_object_set_new(json_debug_log.proc, "start_code", json_integer(proc.start_code));// 0
//	json_object_set_new(json_debug_log.proc, "end_code", json_integer(proc.end_code));// 0
//	json_object_set_new(json_debug_log.proc, "start_stack", json_integer(proc.start_stack));// 0
//	json_object_set_new(json_debug_log.proc, "esp", json_integer(proc.esp));// 0
//	json_object_set_new(json_debug_log.proc, "eip", json_integer(proc.eip));// 0
//	json_object_set_new(json_debug_log.proc, "pending", json_integer(proc.pending));
//	json_object_set_new(json_debug_log.proc, "blocked", json_integer(proc.blocked));
//	json_object_set_new(json_debug_log.proc, "sigign", json_integer(proc.sigign));
//	json_object_set_new(json_debug_log.proc, "sigcatch", json_integer(proc.sigcatch));
//	json_object_set_new(json_debug_log.proc, "wchan", json_integer(proc.wchan));
//	json_object_set_new(json_debug_log.proc, "zero1", json_integer(proc.zero1));
//	json_object_set_new(json_debug_log.proc, "zero2", json_integer(proc.zero2));
//	json_object_set_new(json_debug_log.proc, "exit_signal", json_integer(proc.exit_signal));
//	json_object_set_new(json_debug_log.proc, "cpu", json_integer(proc.cpu));
//	json_object_set_new(json_debug_log.proc, "rt_priority", json_integer(proc.rt_priority));
//	json_object_set_new(json_debug_log.proc, "policy", json_integer(proc.policy));
//}
//int log_af_message(afmessage_t *msg)
//{
//	char temp[512];
//	json_t *json_temp = json_array();
//	// AF message format
//	// [time stamp; SrcAddr; GroupID; ClusterID; SrcEndpoint; DstEndpoint; TranSeqNum; ASDU]
//	// time stamp in unix time
//	json_array_append_new(json_temp, json_integer(time(NULL)));
//
//	sprintf(temp, "%04x", msg->SrcAddr);
//	json_array_append_new(json_temp,json_string(temp));
//
//	sprintf(temp, "%04x", msg->GroupID);
//	json_array_append_new(json_temp,json_string(temp));
//
//	sprintf(temp, "%04x", msg->ClusterID);
//	json_array_append_new(json_temp,json_string(temp));
//
//	sprintf(temp, "%02x", msg->SrcEndPoint);
//	json_array_append_new(json_temp,json_string(temp));
//
//	sprintf(temp, "%02x", msg->DstEndPoint);
//	json_array_append_new(json_temp,json_string(temp));
//
//	sprintf(temp, "%02x", msg->TranSeqNum);
//	json_array_append_new(json_temp,json_string(temp));
//
//	size_t templen=0;
//	for (int i=0;i<(size_t)msg->asduLen;i++) {
//		templen += sprintf(temp+templen, "%02X", msg->asdu[i]);
//	}
//	temp[templen] = 0;
//	json_array_append_new(json_temp,json_string(temp));
//
//	sem_wait(json_debug_log.semaphore);
//	json_array_append_new(json_debug_log.af_message,json_temp);
//	sem_post(json_debug_log.semaphore);
//	return 0;
//}
//int log_system_messsage(char *msg)
//{
//	json_t *json_temp = json_array();
//
//	json_array_append_new(json_temp, json_integer(time(NULL)));
//	json_array_append_new(json_temp, json_string(msg));
//
//	sem_wait(json_debug_log.semaphore);
//	json_array_append_new(json_debug_log.system_message,json_temp);
//	sem_post(json_debug_log.semaphore);
//
//	return 0;
//}

void MTDebugInit()
{
//	json_debug_init();
//	pthread_t thread = pthread_create(&thread, NULL, &json_debug_dump_thread, (void *)NULL);
//	pSendDebugMsg = SendDebugMsg;
	kdInit(0);
}

//void json_debug_dump_thread(void *td)
//{
//	struct timespec interval = {1,0};
//	while (1)
//	{
//		json_debug_dump();
//		kNanosleep(&interval);
//	}
//}

uint8 MT_DebugCommandProcessing(uint8 *pBuf)
{
	char dest[DBGQ_MSGSIZE];
	memcpy((void*)dest, (void*)pBuf, DBGQ_MSGSIZE);
	dest[(size_t)dest[MT_RPC_POS_LEN] + MT_RPC_FRAME_HDR_SZ] = 0;
	
	return (uint8)SendDebugMsg(dest+MT_RPC_FRAME_HDR_SZ, (size_t)(dest[MT_RPC_POS_LEN] + 1));
}

//int SendDebugUnknownMsg(uint8 *pBuf, size_t BufLen)
//{
//	int status = 0;
//	uint8 tempBuf[512];
//	uint8 hdrstr[] = "UNKNOW MESSAGE : ";
//	const int spacing = 3;
//	int i;
//	sprintf(tempBuf, hdrstr, strlen(hdrstr));
//
//	for (i = 0; i< BufLen; i++)
//	{
//		sprintf(tempBuf + strlen(hdrstr) - 1 + i*spacing, "%02X ", pBuf[i]);
//	}
//
//	tempBuf[strlen(hdrstr) + BufLen * spacing] = 0;
//	return SendDebugMsg(tempBuf, strlen(hdrstr) + BufLen * spacing);
//}
//int SendDebugUnknownMsg(uint8 *pBuf, size_t BufLen)
//{
//	return kdBinTraceLog((void*)pBuf, BufLen);
//}

void SendDebugMsgUint8(char *pMsg, size_t MsgLen, uint8 *pVar, size_t VarLen)
{
	char *pVarStr;
	int i;

	pVarStr = (char *) malloc(MsgLen + 2 * VarLen + 2);
	if (pMsg != NULL) memcpy(pVarStr, pMsg, MsgLen);
	for (i = 0; i < VarLen; i++) {
		sprintf(pVarStr+2*i + MsgLen, "%02X", pVar[i]);
	}
	pVarStr[MsgLen + 2 * VarLen] = 0;
	SendDebugMsg((uint8 *)pVarStr, MsgLen + 2 * VarLen + 1); // "+1" is for NULL char
	free((void *)pVarStr);
}
 
void MTDebugClose()  
{
//	MsgQClose(md);
//	xmlCleanupParser();
//	xmlmsgdump_true = 0;
//	pthread_mutexattr_destroy(&xmlDocMTattr);
//	pthread_mutex_destroy(&xmlDocMT);
}

//int getprocstat(procstat_t *procstat)
//{
//	long tickspersec = sysconf(_SC_CLK_TCK);
//	char filename[32];
//	FILE *input = NULL;
//
//	sprintf(filename, "/proc/%d/stat", getpid());
//	input = fopen(filename, "r");
//	if(!input)
//	{
//		syslog(LOG_INFO, "open");
//		return 1;
//	}
//
//	fscanf(input, "%lld ", &procstat->pid);
//	fscanf(input, "%s ", procstat->tcomm);
//	fscanf(input, "%s ", &procstat->state);
//	fscanf(input, "%lld ", &procstat->ppid);
//	fscanf(input, "%lld ", &procstat->pgid);
//	fscanf(input, "%lld ", &procstat->sid);
//	fscanf(input, "%lld ", &procstat->tty_nr);
//	fscanf(input, "%lld ", &procstat->tty_pgrp);
//	fscanf(input, "%lld ", &procstat->flags);
//	fscanf(input, "%lld ", &procstat->min_flt);
//	fscanf(input, "%lld ", &procstat->cmin_flt);
//	fscanf(input, "%lld ", &procstat->maj_flt);
//	fscanf(input, "%lld ", &procstat->cmaj_flt);
//	fscanf(input, "%lld ", &procstat->utime);
//	fscanf(input, "%lld ", &procstat->stimev);
//	fscanf(input, "%lld ", &procstat->cutime);
//	fscanf(input, "%lld ", &procstat->cstime);
//	fscanf(input, "%lld ", &procstat->priority);
//	fscanf(input, "%lld ", &procstat->nicev);
//	fscanf(input, "%lld ", &procstat->num_threads);
//	fscanf(input, "%lld ", &procstat->it_real_value);
//	fscanf(input, "%llu ", &procstat->start_time);
//	fscanf(input, "%lld ", &procstat->vsize);
//	fscanf(input, "%lld ", &procstat->rss);
//	fscanf(input, "%lld ", &procstat->rsslim);
//	fscanf(input, "%lld ", &procstat->start_code);
//	fscanf(input, "%lld ", &procstat->end_code);
//	fscanf(input, "%lld ", &procstat->start_stack);
//	fscanf(input, "%lld ", &procstat->esp);
//	fscanf(input, "%lld ", &procstat->eip);
//	fscanf(input, "%lld ", &procstat->pending);
//	fscanf(input, "%lld ", &procstat->blocked);
//	fscanf(input, "%lld ", &procstat->sigign);
//	fscanf(input, "%lld ", &procstat->sigcatch);
//	fscanf(input, "%lld ", &procstat->wchan);
//	fscanf(input, "%lld ", &procstat->zero1);
//	fscanf(input, "%lld ", &procstat->zero2);
//	fscanf(input, "%lld ", &procstat->exit_signal);
//	fscanf(input, "%lld ", &procstat->cpu);
//	fscanf(input, "%lld ", &procstat->rt_priority);
//	fscanf(input, "%lld ", &procstat->policy);
//	fclose(input);
//	return SUCCESS;
//}

//inline int SendDebugMsg(uint8 *pBuf, size_t BufLen)
//{
//	return log_system_messsage((char *)pBuf);
//}

//int vTraceLog(const char *format, ...)
//{
//	va_list arg;
//	char temp[512];
//	int done;
//
//	va_start (arg, format);
//	done = vsprintf(temp, format, arg);
//	va_end (arg);
//	pSendDebugMsg((char *)temp, strlen(temp));
//	syslog(LOG_INFO, "%s", temp);
//	return done;
//}

