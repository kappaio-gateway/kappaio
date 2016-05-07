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
#include "mtsys.h"
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <stddef.h>
#include "mt_helper.h"
#include "mtdebug.h"
#include "kglobal.h"
#include "kutil.h"
#include "MT_RPC.h"
#include "mtproc.h"

int mt_sysping_req(uint8_t *returnBuffer, size_t bufferSize)
{
	mtMessage_t req;
	req.cmdType = 0x21;
	req.cmdId = MT_SYS_PING;
	req.dataLength = 0;
	return __MTSEND__(&req, returnBuffer,bufferSize,NULL);
}

int mt_sys_wdtkick_req(uint8_t *returnBuffer, size_t bufferSize)
{
	mtMessage_t req;
	req.cmdType = 0x21;
	req.cmdId = MT_SYS_WDT_KICK;
	req.dataLength = 0;
	return __MT_OOB_SEND__(&req, returnBuffer,bufferSize,NULL);
}

uint8_t MTSysPingProc(uint8_t *pBuf, uint8_t *returnBuffer)
{
	uint8_t SysPingRsp[SYSPINGRSP_LEN];
	ByteReverse(pBuf+SYSPINGRSP_POS, SYSPINGRSP_LEN, SysPingRsp);
	LOG_ERROR_MESSAGE("Sys ping response: %02x%02x",SysPingRsp[1],SysPingRsp[0]);
	*returnBuffer = SysPingRsp[0];
	*(returnBuffer+1) = SysPingRsp[1];
	return 0;
}


int mt_getextaddr_req(uint64_t *res)
{
	mtMessage_t out,rtn;
	out.cmdType = 0x21;
	out.cmdId = 0x04;
	out.dataLength = 0;
	uint8_t result[8];
	mtSend(&out, MT_SYS_GET_EXTADDR,(void*)&rtn,sizeof(rtn),NULL);
	bytes_to_uint((void*)res, (void*)(rtn.pData),8);
	return 0;
}
uint8_t MTSysGetExtAddrProc(uint8_t *pBuf, uint8_t *returnBuffer)
{
	bytes_to_uint((void*)returnBuffer, (void*)(pBuf+MT_RPC_POS_DAT0),8);
	return 0;
}

int mt_sysreset_req()
{
	uint8_t status;
	mtMessage_t msg;
	uint8_t result[32];
	msg.cmdType=0x21;
	msg.cmdId =0;
	msg.dataLength=0;
	return __MTSEND__(&msg, NULL, 0, NULL);
}

static uint8_t MTSysResetProc(uint8_t *pBuf,uint8_t *returnBuffer)
{
	return 0;
}

namespace MT {
	namespace SYS {
		int OsalNvRead(NvItem_t &nvItem) {
			mtMessage_t req;

			req.cmdType = 0x21;
			req.cmdId = MT_SYS_OSAL_NV_READ;
			req.dataLength = 3;
			_INT_TO_BYTES_(*(req.pData), nvItem.id, 2);
			req.pData[2] = nvItem.offset;

			uint8_t result[256];
			uint8_t *rtn = result;

			if (__MTSEND__(&req,result,256,NULL) < 0)
			{
				LOG_ERROR_MESSAGE("OsalNvRead failed");
				return -1;
			}

			nvItem.status = *(rtn++);
			nvItem.len = *(rtn++);
			for (int i=0;i<nvItem.len; i++) {
				nvItem.value[i] = *(rtn++);
			}
			return 0;
		}
		int OsalNvWrite(NvItem_t &nvItem) {
			if (nvItem.len == 0 || nvItem.len > 246)
			{
				return -1;
			}

			mtMessage_t req;

			req.cmdType = 0x21;
			req.cmdId = MT_SYS_OSAL_NV_WRITE;
			req.dataLength = 4 + nvItem.len;
			_INT_TO_BYTES_(*(req.pData), nvItem.id, 2);
			req.pData[2] = nvItem.offset;
			req.pData[3] = nvItem.len;

			for (int i=0;i<nvItem.len;i++)
			{
				req.pData[4+i] = nvItem.value[i];
			}

			uint8_t result[256];
			uint8_t *rtn = result;

			if (__MTSEND__(&req,result,256,NULL) < 0)
			{
				LOG_ERROR_MESSAGE("OsalNvWrite failed");
				return -1;
			}

			nvItem.status = *(rtn++);

			return 0;
		}
	}
}
uint8_t MT_SysCommandProcessing(uint8_t *pBuf) {
  uint8_t status = MT_RPC_SUCCESS;
  uint8_t (*callback)(uint8_t*,uint8_t*) =NULL;
  if (mtRecv(pBuf) < 0)
//  if (1)
  {
	  LOG_ERROR_MESSAGE("hello from MT sys fallback callback : %02x", pBuf[2]);
  switch (pBuf[MT_RPC_POS_CMD1])
  {
    case MT_SYS_RESET_IND:
    	callback = MTSysResetProc;
      break;
    case MT_SYS_PING:
      callback = MTSysPingProc;
      break;
    case MT_SYS_GET_EXTADDR:
    	callback = MTSysGetExtAddrProc;
      break;
/*
    case MT_SYS_VERSION:
      MT_SysVersion();
      break;

    case MT_SYS_SET_EXTADDR:
      MT_SysSetExtAddr(pBuf);
      break;



// CC253X MAC Network Processor does not have NV support
#if !defined(CC253X_MACNP)
    case MT_SYS_OSAL_NV_READ:
      MT_SysOsalNVRead(pBuf);
      break;

    case MT_SYS_OSAL_NV_WRITE:
      MT_SysOsalNVWrite(pBuf);
      break;
#endif

    case MT_SYS_OSAL_START_TIMER:
      MT_SysOsalStartTimer(pBuf);
      break;

    case MT_SYS_OSAL_STOP_TIMER:
      MT_SysOsalStopTimer(pBuf);
      break;

    case MT_SYS_RANDOM:
      MT_SysRandom();
      break;

    case MT_SYS_ADC_READ:
      MT_SysAdcRead(pBuf);
      break;

    case MT_SYS_GPIO:
      MT_SysGpio(pBuf);
      break;

    case MT_SYS_STACK_TUNE:
      MT_SysStackTune(pBuf);
      break;
*/
    default:
      status = MT_RPC_ERR_COMMAND_ID;
      break;
  }
  }

	#ifdef MT_DEBUG_FUNC
	if (status != MT_RPC_SUCCESS) {
		uint8_t test_str[] = "Unknown SYS message....";
		//SendDebugMsg(test_str, sizeof(test_str));
		SendDebugUnknownMsg(pBuf, (size_t)(pBuf[0] + 3));
	}
	#endif
  return status;
}
