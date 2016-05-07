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
#ifndef KGLOBAL_H
#define KGLOBAL_H
#include <endian.h>
#define SMALL_ENDIAN LITTLE_ENDIAN
#define ENDIAN SMALL_ENDIAN

#define MT_DEBUG_FUNC
#define MT_SYS_FUNC
#define MT_ZDO_FUNC
#define MT_AF_FUNC
#define MT_UTIL_FUNC
#ifdef MT_DEBUG_FUNC
#include "kdev.h"
#define LOG_ERROR_MESSAGE(...) vTraceLog(__VA_ARGS__)
#define LOG_MESSAGE(...) vTraceLog(__VA_ARGS__)
#define LOG_VAR(x,y) vTraceLog(#x " = " #y , x)
#else
#define LOG_ERROR_MESSAGE(...)
#define LOG_MESSAGE(...)
#define LOG_VAR(x,y)
#endif

#define NULL_GUARD do {if (this==NULL) return NULL;} while(0)

#define SUCCESS 0
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif
