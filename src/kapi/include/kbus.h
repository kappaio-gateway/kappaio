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
#ifndef KBUS_H
#define KBUS_H

#include <stdint.h>
#include "jansson.h"

#ifndef ENDIAN
#define ENDIAN SMALL_ENDIAN // assuming default targer is RAMIPS
#endif

#define KBSTRUC 0x01
#define KBARRAY 0x02

#ifdef __cplusplus
extern "C" {
#endif

void *kbOpen(const char *path, int mode);

int kbClose(void *handle);

int kbRefreshRate(void *fileHandle, size_t sec, long nsec);

int kbSetRefresher(void *objHandle, void *(*refresher)(void *));

int kbRun(void *fileHandle);

void *kbAddObj(void *handle, const char *name, unsigned char type);

int kbAddInt(void *objHandle, const char *name, const int integer);

int kbAddStr(void *objHandle, const char *name, const char *str);

int kbAddJSON(void *objHandle, const char *name, json_t *json);

#ifdef __cplusplus
}
#endif

#endif
