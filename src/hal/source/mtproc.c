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
#include "mtproc.h"
#include "mtintf.h"
#include "MT_RPC.h"
//#include "glbdef.h"
#include <string.h>
#include "mtdebug.h"
#include <syslog.h>
#include <stdlib.h>
#include "kglobal.h"

#ifdef MT_UTIL_FUNC
	#include "mtutil.h"
#endif
#ifdef MT_SYS_FUNC
	#include "mtsys.h"
#endif
#ifdef MT_ZDO_FUNC
	#include "mtzdo.h"
#endif
#ifdef MT_AF_FUNC
	#include "mtaf.h"
#endif
#ifndef UINT8
	#define UINT8
	typedef uint8_t uint8;
#endif

static mtProcessMsg_t mtProcessIncoming[] =
{
	NULL, // [0]

	#ifdef MT_SYS_FUNC  // [1]
		MT_SysCommandProcessing,
	#else
		NULL,
	#endif

	#ifdef MT_MAC_FUNC // [2]
		MT_MacCommandProcessing,
	#else
		NULL,
	#endif

	#ifdef MT_NWK_FUNC// [3]
		MT_NwkCommandProcessing,
	#else
		NULL,
	#endif

	// mtProcessIncoming[4] is set to NULL during initialization to prevent incoming AF messages trigger execution NULL callbacks
	// It will be set to MT_F
//	NULL,
	#ifdef MT_AF_FUNC // [4]
		MT_AfCommandProcessing,
	#else
	NULL,
	#endif

	#ifdef MT_ZDO_FUNC // [5]
		MT_ZdoCommandProcessing,
	#else
	NULL,
	#endif

	#ifdef MT_SAPI_FUNC // [6]
		MT_SapiCommandProcessing,
	#else
	NULL,
	#endif

	#ifdef MT_UTIL_FUNC // [7]
		MT_UtilCommandProcessing,
	#else
	NULL,
	#endif

	#ifdef MT_DEBUG_FUNC // [8]
		MT_DebugCommandProcessing,
	#else
	NULL,
	#endif

	#ifdef MT_APP_FUNC   // 0x09
		MT_AppCommandProcessing,
	#else
	NULL,
	#endif

	#ifdef MT_OTA_FUNC // 0x0A
		MT_OtaCommandProcessing,
	#else
	NULL,
	#endif

	NULL,// 0x0B
	NULL,// 0x0C
	NULL,
	NULL,
//	sb_callback, 0x0D
	//sblproc,// 0x0D
};

#define MT_ERROR_SRSP_LEN 3
static void MT_ProcessIncoming(uint8_t *pBuf);
static uint8_t CmdFCSCal(uint8_t *pBuf, uint8_t dataLen);
static int (*OutOfBandMessageProcess)(uint8_t *pBuf);

void setOutOfBandMessageCallBack(int (*CallBack)(uint8_t *))
{
	OutOfBandMessageProcess = CallBack;
}

void MTProcInit()
{
//	#ifdef MT_UTIL_FUNC
//	MT_UtilInit();
//	#endif
	syslog(LOG_INFO, "MTProcInit ok");

	// Initialize the low level serial interface with TI-CC2530
	MTIntfInit(MT_ProcessIncoming);

//	image_write();
}

void StartMT_Af()
{
	// enable AF message processing
	mtProcessIncoming[4] = MT_AfCommandProcessing;
	return;
}

void MTProcClose()  
{
	//MsgQClose(mdi);
//	MsgQClosec(mdo);
	//MTDebugClose();
}

void MT_ProcessIncoming(uint8_t *pBuf)
{
	mtProcessMsg_t  func;
	uint8_t rsp[MT_ERROR_SRSP_LEN];

	/* pre-build response message:  | status | cmd0 | cmd1 | */
	rsp[1] = pBuf[MT_RPC_POS_CMD0];
	rsp[2] = pBuf[MT_RPC_POS_CMD1];

	/* check length */
	if (pBuf[MT_RPC_POS_LEN] > MT_RPC_DATA_MAX)
	{
		rsp[0] = MT_RPC_ERR_LENGTH;
	} else if ((rsp[1] & MT_RPC_SUBSYSTEM_MASK) < MT_RPC_SYS_MAX)
	{
		if (OutOfBandMessageProcess(pBuf) == 0)
		{
			/* out of band message has the highest priority */

//			syslog(LOG_INFO, "Out of band MT message caught");
		} else
			{
			/* check subsystem range, look up processing function */
			func = mtProcessIncoming[rsp[1] & MT_RPC_SUBSYSTEM_MASK];
			if (func)
			{
				/* execute processing function */
				rsp[0] = (*func)(pBuf);
			} else
			{
				SendDebugUnknownMsg(pBuf, (size_t)(pBuf[0] + 3));
				rsp[0] = MT_RPC_ERR_SUBSYSTEM;
			}
		}
	} else
	{
		rsp[0] = MT_RPC_ERR_SUBSYSTEM;
	}

//	return NULL;
  //if (rsp[0] != MT_RPC_SUCCESS)
}
int registerSubsystemCb(uint8_t subsystem, uint8_t (*callback)(uint8_t*))
{
	if (mtProcessIncoming[subsystem] == NULL)
	{
		mtProcessIncoming[subsystem] = callback;
		return 0;
	}
	return -1;
}


void mtBuildAndSendCmd(mtMessage_t *message)
{
	MTBuildAndSendCmd(message->cmdType, message->cmdId, message->dataLength, message->pData);
}
void MTBuildAndSendCmd(uint8_t cmdType, uint8_t cmdId, uint8_t dataLen, uint8_t *pData)
{
	uint8_t *cmd_ptr, *msg;
	cmd_ptr = (uint8_t *) malloc(dataLen + SPI_0DATA_MSG_LEN);
	if (cmd_ptr == NULL) {
		syslog(LOG_INFO, "failed to allocate msg buffer");
		return;
	}
	*cmd_ptr = MT_SOF;
	msg = cmd_ptr+1;
	*msg++ = dataLen;
	*msg++ = cmdType;
	*msg++ = cmdId;
	if (pData) memcpy(msg, pData, (size_t)dataLen);
	cmd_ptr[dataLen + SPI_0DATA_MSG_LEN -1] = CmdFCSCal(cmd_ptr + 1, dataLen + 3);
	SerialSend(cmd_ptr, (size_t)dataLen + SPI_0DATA_MSG_LEN);
	free((void *)cmd_ptr);	
}
uint8_t CmdFCSCal(uint8_t *pBuf, uint8_t dataLen)
{
	uint8_t fcs = 0;
	for (int i = 0; i < dataLen; i++)
	{
		fcs ^= pBuf[i]; 
	}
	return fcs;
}
void ByteReverse(uint8 *pBuf, size_t BufLen, uint8 *dest)
{
	size_t i;
	for (i = 0; i < BufLen; i++)
	{
		dest[i] = pBuf[BufLen - 1 - i];
	}
}

