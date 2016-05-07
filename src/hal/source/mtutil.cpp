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
#include "mtutil.h"
#include "mt_helper.h"
//#include "rssys.h"
#include <semaphore.h>
//#include "apsdb.h"
#include <string.h>
#include "kutil.h"
#include "kdev.h"
#include "kglobal.h"
#include <stdlib.h>

#define INVALID_NODE_ADDR 0xFFFE

static void mt_util_get_device_info_rsp_proc(uint8_t *pBuf);

sem_t UtilCommand;

struct UtilAddrLookUp_s {
	sem_t CommandLock;
	uint8_t InProgress;
	uint16_t NwkAddr;
	uint64_t IEEEAddr;
} UtilAddrLookUp;

struct utlmessage_s {
	volatile int incoming;
	sem_t sem;
	uint8_t *buffer;
};
static struct utlmessage_s msg;
int get_utlmessage(uint8_t *pBuf, int timeout_ms);

int get_utlmessage(uint8_t *pBuf, int timeout_ms) {
	int status = 0;
	int dlyCnt_ms = 0;
	while (dlyCnt_ms < timeout_ms) {

		sem_wait(&(msg.sem));
		if (msg.incoming == 1) {
			memcpy((void *)pBuf, (void *)(msg.buffer), msg.buffer[MT_RPC_POS_LEN]+3);
			status = 1;
			free(msg.buffer);
			msg.incoming = 0;
		} else {
			status = 0;
		}
		sem_post(&(msg.sem));
		if (status == 1) {
			break;
		}

//		kNanosleep(1000000);
		dlyCnt_ms++;
	}
	if (dlyCnt_ms == timeout_ms) {
		return -1;
	}
	return 0;
}


//uint8_t MT_UtilInit() {
//	sem_init(&UtilAddrLookUp.CommandLock, 0, 1);
//	UtilAddrLookUp.InProgress = FALSE;
//	//=========
//	sem_init(&(msg.sem), 0, 1);
//	msg.incoming = 0;
//	//=========
//
//	return 0;
//}

uint8_t MT_UtilCommandProcessing(uint8_t* pBuf)
{
	uint8_t status = MT_RPC_SUCCESS;
	uint8_t rspBuf[MT_BUFFER_MAX];

	#ifdef MT_DEBUG_FUNC
	char str0[] = "Unknown Util Msg : 0x";
	#endif


	if (mtRecv(pBuf) < 0)
	{
		LOG_ERROR_MESSAGE("hello from MT_UTIL fallback callback: %02x",pBuf[MT_RPC_POS_CMD1]);
		switch (pBuf[MT_RPC_POS_CMD1])
		{
			case MT_UTIL_ASSOC_GET_WITH_ADDRESS:
				UTIL_ASSOC_GET_WITH_ADDRESS_Rsp_Proc(pBuf);
				break;
			case MT_UTIL_ADDRMGR_EXT_ADDR_LOOKUP:
				UTIL_ADDRMGR_EXT_ADDR_LOOKUP_Rsp_Proc(pBuf);
				break;
			case MT_UTIL_GET_DEVICE_INFO:
				mt_util_get_device_info_rsp_proc(pBuf);
				break;
			default:
				status = MT_RPC_ERR_COMMAND_ID;
//				#ifdef MT_DEBUG_FUNC
//				if (pBuf[MT_RPC_POS_CMD0] == 0x67)
//					SendDebugMsgUint8(str0, strlen(str0), pBuf, (size_t)(pBuf[0]+3));
//				#endif
				status = MT_RPC_ERR_COMMAND_ID;
			break;
		}
	}
  return status;
}

uint8_t UTIL_ASSOC_GET_WITH_ADDRESS_Req(uint64_t IEEEAddr, uint16_t NwkAddr) {

//	uint8 pData[10];
//	uint64touint8(IEEEAddr, pData);
//	uint16touint8(NwkAddr, pData+8);
//	MTBuildAndSendCmd(0x27, MT_UTIL_ASSOC_GET_WITH_ADDRESS, 10, pData);
	return 0;
}

uint8_t UTIL_ASSOC_GET_WITH_ADDRESS_Rsp_Proc(uint8_t* pBuf) {
	//************** TI associated_devices_t structure **************
	//typedef struct
	//{
	//  UINT16 shortAddr;                 // Short address of associated device
	//  uint16 addrIdx;                   // Index from the address manager
	//  byte nodeRelation;
	//  byte devStatus;                   // bitmap of various status values
	//  byte assocCnt;
	//  byte age;
	//  linkInfo_t linkInfo;
	//} associated_devices_t;
	//**************************************************************

	return SUCCESS;
}
//uint8_t UTIL_ADDRMGR_EXT_ADDR_LOOKUP_Req(uint16_t NwkAddr) {
//	uint8 pData[10];
//
//	sem_wait(&UtilAddrLookUp.CommandLock);
//	UtilAddrLookUp.InProgress = TRUE;
//	UtilAddrLookUp.NwkAddr = NwkAddr;
//
//	uint64touint8(0, pData);
//	uint16touint8(NwkAddr, pData+8);
//	MTBuildAndSendCmd(0x27, MT_UTIL_ADDRMGR_EXT_ADDR_LOOKUP, 10, pData);
//	return 0;
//}
uint8_t UTIL_ADDRMGR_EXT_ADDR_LOOKUP_Rsp_Proc(uint8_t *pBuf) {
//	uint8 *temp = pBuf+MT_RPC_POS_DAT0;
//	uint64_t IEEEAddr = build_uint64(temp);
//
//	UtilAddrLookUp.IEEEAddr = IEEEAddr;
//	UtilAddrLookUp.InProgress = FALSE;
//	sem_post(&UtilAddrLookUp.CommandLock);
	return SUCCESS;

}
//uint64_t GetAssocIEEEAddr(uint16_t NwkAddr) {
//	int TVal = 0;
//
//	UTIL_ADDRMGR_EXT_ADDR_LOOKUP_Req(NwkAddr);
//	while (UtilAddrLookUp.InProgress == TRUE) {
//		sleep(1);
//	}
//	return UtilAddrLookUp.IEEEAddr;
//}


int mt_util_get_device_info(uint8_t *res)
{
//	mtMessage_t x, rtn;
//	x.cmdType = 0x27;
//	x.cmdId = 0;
//	x.dataLength =0;
	return 0;


}
void mt_util_get_device_info_rsp_proc(uint8_t *pBuf)
{
	LOG_VAR(pBuf[0],%02x);
	release_channel();
}

int mtUtilAssocGetWithAddress(uint64_t extAddr, void *returnBuffer)
{
	mtMessage_t req,rsp;
	req.cmdType = 0x27;
	req.cmdId = 0x4a;
	req.dataLength = 0x0a;
	memset(req.pData,0,32);

	_INT_TO_BYTES_(req.pData,extAddr,8);
	return mtSend(&req,0x4a,returnBuffer,sizeof(mtMessage_t),NULL);
}

int mtUtilGetAssocCount(uint16_t *returnCount)
{
	//#define PARENT 				0
	//#define CHILD_RFD 			1
	//#define CHILD_RFD_RX_IDLE 	2
	//#define CHILD_FFD 			3
	//#define CHILD_FFD_RX_IDLE 	4
	//#define NEIGHBOR 			5
	//#define OTHER 				6
	mtMessage_t req;
	req.cmdType = 0x27;
	req.cmdId = MT_UTIL_ASSOC_COUNT;
	req.dataLength = 2;
	req.pData[0] = 0;
	req.pData[2] = 6;
	uint8_t res[2];
	if (__MTSEND__(&req,res,2,NULL) < 0)
	{
		LOG_ERROR_MESSAGE("util get assoc count failed");
		return -1;
	}
	_BYTES_TO_INT_(*returnCount,*res,2);
	return 0;
}

int mtUtilAssocFindDevice(uint8_t *returnBuffer, uint8_t index)
{
	mtMessage_t req;
	req.cmdType = 0x27;
	req.cmdId = MT_UTIL_ASSOC_FIND_DEVICE;
	req.dataLength = 1;
	req.pData[0] = index;

	if (__MTSEND__(&req,returnBuffer,18,NULL) < 0)
	{
		LOG_ERROR_MESSAGE("mtUtilAssocFindDevice failed");
		return -1;
	}

	return 0;
}

int mtUtilGetDeviceInfo(uint8_t *returnBuffer,size_t bufferSize)
{
	mtMessage_t req;
	req.cmdType = 0x27;
	req.cmdId = 0;
	req.dataLength = 0;

	return __MTSEND__(&req,returnBuffer, bufferSize, NULL);
}

int mtUtilAddrmgrNwkAddrLookup(uint16_t nwkAddr, uint64_t *returnBuffer)
{
	mtMessage_t req;

	req.cmdType = 0x27;
	req.cmdId = MT_UTIL_ADDRMGR_NWK_ADDR_LOOKUP;
	req.dataLength = 2;
	_INT_TO_BYTES_(*(req.pData), nwkAddr,2);

	uint8_t result[8];

	if (__MTSEND__(&req,result,8,NULL) < 0)
	{
		LOG_ERROR_MESSAGE("mtUtilGetDeviceInfo failed");
		return -1;
	}

	_BYTES_TO_INT_(*returnBuffer,*result,8);
	return 0;
}

namespace MT{
	namespace UTIL{
		int GetNvInfo(NvInfo_t &nvInfo)
		{
			mtMessage_t req;

			req.cmdType = 0x27;
			req.cmdId = MT_UTIL_GET_NV_INFO;
			req.dataLength = 0;

			uint8_t result[256];
			uint8_t *rtn = result;

			if (__MTSEND__(&req,result,256,NULL) < 0)
			{
				LOG_ERROR_MESSAGE("mtUtilGetDeviceInfo failed");
				return -1;
			}

			rtn += _BYTES_TO_INT_(nvInfo.status, *rtn, 1);
			rtn += _BYTES_TO_INT_(nvInfo.IEEEAddr, *rtn, 8);
			rtn += _BYTES_TO_INT_(nvInfo.ScanChannels, *rtn, 4);
			rtn += _BYTES_TO_INT_(nvInfo.PanId, *rtn, 2);
			rtn += _BYTES_TO_INT_(nvInfo.SecurityLevel, *rtn, 1);

			return 0;
		}
	}
}
//void set_default_channel()
//{
//	mtMessage_t msg;
//	msg.cmdId = 0x03;
//	msg.cmdType = 0x27;
//	msg.dataLength = 4;
//	msg.pData[msg.dataLength++] = 0x00;
//	msg.pData[msg.dataLength++] = 0x00;
//	msg.pData[msg.dataLength++] = 0x88;
//	msg.pData[msg.dataLength++] = 0x00;
//	mtBuildAndSendCmd(&msg);
//}


