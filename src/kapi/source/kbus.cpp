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
#include "kbus.h"
#include <semaphore.h>
#include <string.h>
#include "kutil.h"
#include <pthread.h>
#include <syslog.h>
#include <jansson.h>

typedef struct kbData_s kbData_t;
typedef struct kbFile_s kbFile_t;

struct kbData_s
{
	json_t *data;
	unsigned char type;
	unsigned char autoRefresh;
	void *(*refresher)(void*);
	kbFile_t *kbFile;
	kbData_t *next;
};

struct kbFile_s
{
	unsigned int seqNum;
	char *path;
	struct timespec refreshIntval;
	sem_t semaphore;
	json_t *kbDataBus;
	kbData_t *kbData;
};

namespace kbus
{
	kbData_t *createKbData()
	{
		kbData_t *x = (kbData_t *)malloc(sizeof(kbData_t));
		if (x != NULL)
		{
			x->data = NULL;
			x->next = NULL;
			x->autoRefresh = 0;
			x->refresher = NULL;
		}
		return x;
	}

	int destroyKbData(kbData_t *kbData)
	{
		if (kbData == NULL)
		{
			return -1;
		}

		json_decref(kbData->data);

		return 0;
	}

	int resetKbData(kbData_t *kbData)
	{
		if (kbData->type == KBSTRUC)
		{
			json_object_clear(kbData->data);
		}

		if (kbData->type == KBARRAY)
		{
			json_array_clear(kbData->data);
		}

		return 0;
	}

	void kbCommit(kbFile_t *kbFile)
	{
		kbData_t *x = kbFile->kbData;

		while (x != NULL)
		{
			if (x->autoRefresh)
			{
				x->refresher((void*)x);
			}

			x = x->next;
		}

		sem_wait(&(kbFile->semaphore));
		// Insert sequence number
		json_object_set_new(kbFile->kbDataBus,"seqnum",json_integer(kbFile->seqNum++));

		// Insert time stamp
		json_object_set_new(kbFile->kbDataBus,"unixtime", json_integer(time(NULL)));

		// Write json to write
		json_dump_file(kbFile->kbDataBus, kbFile->path, JSON_INDENT(1));

		// clear all data for next write
		kbData_t *iter = kbFile->kbData;

		while (iter != NULL)
		{
			kbus::resetKbData(iter);
			iter = iter->next;
		}

		sem_post(&(kbFile->semaphore));

	}

	void *kbSchdule(void *td)
	{
		kbFile_t *x = (kbFile_t*)td;

		while (1)
		{
			kbCommit(x);
			kNanosleep(&(x->refreshIntval));
		}

		return NULL;
	}

}

int kbRun(void *fileHandle)
{
	pthread_t thread;

	int rtnval = pthread_create(&thread, NULL, kbus::kbSchdule, (void *)fileHandle);

	return rtnval;
}

void *kbOpen(const char *path, int mode)
{
	kbFile_t *file = (kbFile_t *)malloc(sizeof(kbFile_t));
	if (file != NULL)
	{
		file->seqNum = 0;

		sem_init(&(file->semaphore), 0, 1);

		file->kbData = NULL;

		file->path = (char *)malloc(strlen(path)+1);

		sprintf(file->path, path, "%s");

		// default refreshInterval is 2 sec's
		file->refreshIntval = {2,0};

		file->kbDataBus = json_object();
	}
	return (void *)file;
}

int kbClose(void *handle)
{
	kbFile_t *y = (kbFile_t*)handle;
	kbData_t *x = y->kbData;

	while (x != NULL)
	{
		kbData_t *z = x;
		x = x->next;
		kbus::destroyKbData(z);
	}

	sem_destroy(&(y->semaphore));

	json_decref(y->kbDataBus);

	free(y->path);

	free(y);

	return 0;
}

void *kbAddObj(void *handle, const char *name, unsigned char type)
{
	if ((type != KBSTRUC && type != KBARRAY) || handle == NULL)
	{
		return NULL;
	}

	kbData_t *x = kbus::createKbData();

	if (x != NULL)
	{
		x->type = type;

		if (type == KBSTRUC)
		{
			x->data = json_object();
		}

		if (type == KBARRAY)
		{
			x->data = json_array();
		}

		kbFile_t *y = (kbFile_t *)handle;

		if (y->kbData == NULL)
		{
			y->kbData = x;
		} else
		{
			kbData_t *z = y->kbData;

			while (z->next != NULL)
			{
				z = z->next;
			}

			z->next = x;
		}

		x->kbFile = y;

		json_object_set_new(y->kbDataBus, name, x->data);

	}
	return (void*)x;
}

int kbAddInt(void *objHandle, const char *name, const int integer)
{
	kbData_t *x = (kbData_t *)objHandle;

	sem_wait(&(x->kbFile->semaphore));

	if (x->type == KBSTRUC)
	{
		json_object_set_new(x->data, name, json_integer(integer));
	}

	if (x->type == KBARRAY)
	{
		json_array_append_new(x->data,json_integer(integer));
	}

	sem_post(&(x->kbFile->semaphore));

	return 0;
}

int kbAddStr(void *objHandle, const char *name, const char *str)
{
	kbData_t *x = (kbData_t *)objHandle;

	sem_wait(&(x->kbFile->semaphore));

	if (x->type == KBSTRUC)
	{
		json_object_set_new(x->data, name, json_string(str));
	}

	if (x->type == KBARRAY)
	{
		json_array_append_new(x->data,json_string(str));
	}

	sem_post(&(x->kbFile->semaphore));

	return 0;
}

int kbAddJSON(void *objHandle, const char *name, json_t *json)
{
	kbData_t *x = (kbData_t *)objHandle;

	sem_wait(&(x->kbFile->semaphore));

	if (x->type == KBSTRUC)
	{
		json_object_set_new(x->data, name, json);
	}

	if (x->type == KBARRAY)
	{
		json_array_append_new(x->data,json);
	}

	sem_post(&(x->kbFile->semaphore));

	return 0;
}

int kbRefreshRate(void *fileHandle, size_t sec, long nsec)
{
	if (fileHandle == NULL)
	{
		return -1;
	}

	((kbFile_t*)fileHandle)->refreshIntval = {sec,nsec};

	return 0;
}

int kbSetRefresher(void *objHandle, void *(*refresher)(void *))
{
	kbData_t *x = (kbData_t *)objHandle;

	sem_wait(&(x->kbFile->semaphore));

	x->autoRefresh = 1;

	x->refresher = refresher;

	sem_post(&(x->kbFile->semaphore));

	return 0;
}
