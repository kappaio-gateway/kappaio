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
#ifndef MTDEBUG_H
#define MTDEBUG_H
//#include "glbdef.h"
//#include "apsdb.h"
#include <linux/limits.h>
#include "kdev.h"

//========== json ====================
#define JSON_DEBUG_PATH "/rstmp/debuglog.json"
#define JSON_DEBUG_UNIXTIME				0
#define JSON_DEBUG_SEQNUM            	1
#define JSON_DEBUG_AF_MESSAGE        	2
#define JSON_DEBUG_SYSTEM_MESSAGE    	3
#define JSON_DEBUG_PROC              	4
#define JSON_DEBUG_TOD               	5

// **** Format within AF messages ******
#define JSON_DEBUG_AF_UNIXTIME		 	0
#define JSON_DEBUG_AF_SRCADDR			1
#define JSON_DEBUG_AF_GROUPID			2
#define JSON_DEBUG_AF_CLUSTERID			3
#define JSON_DEBUG_AF_SRCENDPOINT		4
#define JSON_DEBUG_AF_DSTENDPOINT		5
#define JSON_DEBUG_AF_SEQNUM			6
//***************************************


#ifdef __cplusplus
extern "C" {
#endif

void MTDebugInit();
void MTDebugClose();
uint8_t MT_DebugCommandProcessing(uint8_t *pBuf);
void SendDebugMsgUint8(char *pMsg, size_t MsgLen, uint8_t *pVar, size_t VarLen);

#ifdef __cplusplus
}
#endif

#endif



