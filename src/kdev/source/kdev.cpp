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
#include "kdev.h"
#include "kbus.h"
#include "kutil.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <jansson.h>

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#error Byte order (endianness) is not defined
#endif

#if defined(__LITTLE_ENDIAN__) && defined(__BIG_ENDIAN__)
#error big endian and little endian are both
#endif

namespace kdev
{
	void *kdFile = NULL;

	void *messagerLogger = NULL;

	void *afmsgLogger = NULL;

	void *procLogger = NULL;

//	typedef struct
//	{
//		long long int pid;
//		char tcomm[PATH_MAX];
//		char state;
//		long long int ppid;
//		long long int pgid;
//		long long int sid;
//		long long int tty_nr;
//		long long int tty_pgrp;
//		long long int flags;
//		long long int min_flt;
//		long long int cmin_flt;
//		long long int maj_flt;
//		long long int cmaj_flt;
//		long long int utime;
//		long long int stimev;
//		long long int cutime;
//		long long int cstime;
//		long long int priority;
//		long long int nicev;
//		long long int num_threads;
//		long long int it_real_value;
//		unsigned long long start_time;
//		long long int vsize;
//		long long int rss;
//		long long int rsslim;
//		long long int start_code;
//		long long int end_code;
//		long long int start_stack;
//		long long int esp;
//		long long int eip;
//		long long int pending;
//		long long int blocked;
//		long long int sigign;
//		long long int sigcatch;
//		long long int wchan;
//		long long int zero1;
//		long long int zero2;
//		long long int exit_signal;
//		long long int cpu;
//		long long int rt_priority;
//		long long int policy;
//	} procstat_t;

//	int getprocstat(procstat_t *procstat)
//	{
//		// long tickspersec = sysconf(_SC_CLK_TCK);
//		char filename[32];
//		FILE *input = NULL;
//
//		sprintf(filename, "/proc/%d/stat", getpid());
//		input = fopen(filename, "r");
//		if(!input)
//		{
//			syslog(LOG_INFO, "open");
//			return 1;
//		}
//
//		fscanf(input, "%lld ", &procstat->pid);
//		fscanf(input, "%s ", procstat->tcomm);
//		fscanf(input, "%s ", &procstat->state);
//		fscanf(input, "%lld ", &procstat->ppid);
//		fscanf(input, "%lld ", &procstat->pgid);
//		fscanf(input, "%lld ", &procstat->sid);
//		fscanf(input, "%lld ", &procstat->tty_nr);
//		fscanf(input, "%lld ", &procstat->tty_pgrp);
//		fscanf(input, "%lld ", &procstat->flags);
//		fscanf(input, "%lld ", &procstat->min_flt);
//		fscanf(input, "%lld ", &procstat->cmin_flt);
//		fscanf(input, "%lld ", &procstat->maj_flt);
//		fscanf(input, "%lld ", &procstat->cmaj_flt);
//		fscanf(input, "%lld ", &procstat->utime);
//		fscanf(input, "%lld ", &procstat->stimev);
//		fscanf(input, "%lld ", &procstat->cutime);
//		fscanf(input, "%lld ", &procstat->cstime);
//		fscanf(input, "%lld ", &procstat->priority);
//		fscanf(input, "%lld ", &procstat->nicev);
//		fscanf(input, "%lld ", &procstat->num_threads);
//		fscanf(input, "%lld ", &procstat->it_real_value);
//		fscanf(input, "%llu ", &procstat->start_time);
//		fscanf(input, "%lld ", &procstat->vsize);
//		fscanf(input, "%lld ", &procstat->rss);
//		fscanf(input, "%lld ", &procstat->rsslim);
//		fscanf(input, "%lld ", &procstat->start_code);
//		fscanf(input, "%lld ", &procstat->end_code);
//		fscanf(input, "%lld ", &procstat->start_stack);
//		fscanf(input, "%lld ", &procstat->esp);
//		fscanf(input, "%lld ", &procstat->eip);
//		fscanf(input, "%lld ", &procstat->pending);
//		fscanf(input, "%lld ", &procstat->blocked);
//		fscanf(input, "%lld ", &procstat->sigign);
//		fscanf(input, "%lld ", &procstat->sigcatch);
//		fscanf(input, "%lld ", &procstat->wchan);
//		fscanf(input, "%lld ", &procstat->zero1);
//		fscanf(input, "%lld ", &procstat->zero2);
//		fscanf(input, "%lld ", &procstat->exit_signal);
//		fscanf(input, "%lld ", &procstat->cpu);
//		fscanf(input, "%lld ", &procstat->rt_priority);
//		fscanf(input, "%lld ", &procstat->policy);
//		fclose(input);
//		return 0;
//	}
//
//	void *procstatRefresher(void *logger)
//	{
//		procstat_t proc;
//		getprocstat(&proc);
//
//		kbAddInt(logger,"pid", 	proc.pid);
//		kbAddStr(logger,"tcomm", proc.tcomm);// 0
//		kbAddStr(logger,"state", &(proc.state));// 0
//		kbAddInt(logger, "ppid", proc.ppid);// 0
//		kbAddInt(logger, "pgid", proc.pgid);// 0
//		kbAddInt(logger, "sid", proc.sid);// 0
//		kbAddInt(logger, "tty_nr", proc.tty_nr);// 0
//		kbAddInt(logger, "tty_pgrp", proc.tty_pgrp);// 0
//		kbAddInt(logger, "flags", proc.flags);// 0
//		kbAddInt(logger, "min_flt", proc.min_flt);// 0
//		kbAddInt(logger, "cmin_flt", proc.cmin_flt);// 0
//		kbAddInt(logger, "maj_flt", proc.maj_flt);// 0
//		kbAddInt(logger, "cmaj_flt", proc.cmaj_flt);// 0
//		kbAddInt(logger, "utime", proc.utime);// 0
//		kbAddInt(logger, "stimev", proc.stimev);// 0
//		kbAddInt(logger, "cutime", proc.cutime);// 0
//		kbAddInt(logger, "cstime", proc.cstime);// 0
//		kbAddInt(logger, "priority", proc.priority);// 0
//		kbAddInt(logger, "nicev", proc.nicev);// 0
//		kbAddInt(logger, "num_threads", proc.num_threads);// 0
//		kbAddInt(logger, "it_real_value", proc.it_real_value);// 0
//		kbAddInt(logger, "start_time", proc.start_time);// 0
//		kbAddInt(logger, "vsize", proc.vsize);// 0
//		kbAddInt(logger, "rss", proc.rss);// 0
//		kbAddInt(logger, "rsslim", proc.rsslim);// 0
//		kbAddInt(logger, "start_code", proc.start_code);// 0
//		kbAddInt(logger, "end_code", proc.end_code);// 0
//		kbAddInt(logger, "start_stack", proc.start_stack);// 0
//		kbAddInt(logger, "esp", proc.esp);// 0
//		kbAddInt(logger, "eip", proc.eip);// 0
//		kbAddInt(logger, "pending", proc.pending);
//		kbAddInt(logger, "blocked", proc.blocked);
//		kbAddInt(logger, "sigign", proc.sigign);
//		kbAddInt(logger, "sigcatch", proc.sigcatch);
//		kbAddInt(logger, "wchan", proc.wchan);
//		kbAddInt(logger, "zero1", proc.zero1);
//		kbAddInt(logger, "zero2", proc.zero2);
//		kbAddInt(logger, "exit_signal", proc.exit_signal);
//		kbAddInt(logger, "cpu", proc.cpu);
//		kbAddInt(logger, "rt_priority", proc.rt_priority);
//		kbAddInt(logger, "policy", proc.policy);
//
//		return NULL;
//	}

	inline int logSystemMessage(char *str)
	{
		return kbAddStr(messagerLogger, NULL, str);
	}
}

int kdTraceLog(const char *format, ...)
{
	va_list arg;
	char temp[4096];

	va_start(arg, format);

	int done = vsprintf(temp, format, arg);

	va_end(arg);

	if (kdev::messagerLogger != NULL)
	{
		kdev::logSystemMessage(temp);
	}

	syslog(LOG_INFO, "%s", temp);

	return done;
}

int kdBinTraceLog(void *mem, size_t size)
{
	char temp[512];

	kPrintMem(temp,mem,size);

	return kdTraceLog("%s", temp);

}

int kdAPSMessageLog(AFMessage *msg)
{
	char temp[512];
	json_t *tempJSON = json_object();

	json_object_set_new(tempJSON, "UNIXTimeStamp", json_integer(time(NULL)));

	sprintf(temp, "%04x", msg->srcAddr());
	json_object_set_new(tempJSON, "SrcAddr", json_string(temp));

	sprintf(temp, "%04x", msg->groupId());
	json_object_set_new(tempJSON, "GroupId", json_string(temp));

	sprintf(temp, "%04x", msg->clusterId());
	json_object_set_new(tempJSON, "ClusterId", json_string(temp));

	sprintf(temp, "%02x", msg->srcEndpoint());
	json_object_set_new(tempJSON, "SrcEndpoint", json_string(temp));

	sprintf(temp, "%02x", msg->dstEndpoint());
	json_object_set_new(tempJSON, "DstEndpoint", json_string(temp));

	sprintf(temp, "%02x", msg->tranSeqNum());
	json_object_set_new(tempJSON, "TranSeqNumber", json_string(temp));

	sprintf(temp, "%02x", msg->wasBroadCast());
	json_object_set_new(tempJSON, "WasBroadCast", json_string(temp));

	sprintf(temp, "%02x", msg->linkQuality());
	json_object_set_new(tempJSON, "LinkQuality", json_string(temp));

	sprintf(temp, "%02x", msg->securityUse());
	json_object_set_new(tempJSON, "SecurityUse", json_string(temp));

	size_t templen=0;
	for (int i=0;i<(size_t)msg->asduLen();i++) {
		templen += sprintf(temp+templen, "%02X", msg->asdu()[i]);
	}
	temp[templen] = 0;
	json_object_set_new(tempJSON, "Data", json_string(temp));

	kbAddJSON(kdev::afmsgLogger, NULL, tempJSON);
	return 0;
}
void kdInit(int mode)
{
	kdev::kdFile = kbOpen("/tmp/kdFile.json", 0);

	kdev::messagerLogger = kbAddObj(kdev::kdFile,"system_message", KBARRAY);

	kdev::afmsgLogger = kbAddObj(kdev::kdFile,"af_message", KBARRAY);

	//kdev::procLogger = kbAddObj(kdev::kdFile, "process_status", KBSTRUC);

	//kbSetRefresher(kdev::procLogger, kdev::procstatRefresher);

	kbRefreshRate(kdev::kdFile,2,0);

	kbRun(kdev::kdFile);
}

int kdClose()
{
	return kbClose(kdev::kdFile);
}

int SendDebugUnknownMsg(uint8_t *pBuf, size_t BufLen)
{
	return kdBinTraceLog((void*)pBuf, BufLen);
}

int bytes_to_uint(void* destination, void* source, size_t size)
{

#if defined(__BIG_ENDIAN__)
	timemcpy(destination, source, size);
#elif defined(__LITTLE_ENDIAN__)
	memcpy(destination, source, size);
#endif
	return size;
}
