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
#include "mtaf.h"
#include <string.h>
#include <time.h>
#include <syslog.h>
#include "kutil.h"
#include "zbconst.h"
#include "MT_RPC.h"
#include "kglobal.h"
#include "mt_helper.h"
#include "kdev.h"
#include "mtproc.h"
#include "mtaf_impl.h"

//#define MT_AF_INCOMING_MSG_IND 0x81
//#define MT_AF_INCOMING_MSG_EXT_IND 0x82

namespace {
const static uint8_t AFMsg_Ind = 0x81;
const static uint8_t AFMsg_EXT_Ind 	= 0x82;

int (*incommingMessageHandle)(AFMessage *message)=0;
int (*incommingMessageHandle_EXT)(AFMessage_EXT *)=0;

int MTAFIncomingMsgProc(uint8_t *pBuf)
{
	mt_afmessage temp;
	temp.loadStr(pBuf + MT_RPC_POS_DAT0, pBuf[MT_RPC_POS_LEN]);

	#ifdef MT_DEBUG_FUNC
	log_af_message(&temp);
	#endif
	incommingMessageHandle(&temp);
	return SUCCESS;
}

int MTAFIncomingMsgEXTProc(uint8_t *pBuf)
{
	mt_afmessage_EXT temp;
	temp.loadStr(pBuf + MT_RPC_POS_DAT0, pBuf[MT_RPC_POS_LEN]);

	incommingMessageHandle_EXT(&temp);
	return SUCCESS;
}

uint8_t MTAfDataRequestConfirm(uint8_t *pBuf)
{
	release_channel();
	return SUCCESS;
}
} // namespace


namespace kapi
{
	namespace mtaf
	{

		void register_AFMessage_cb (int (*messageHandle)(AFMessage *))
		{
			incommingMessageHandle = messageHandle;
		}

		void register_AFMessage_EXT_cb ( int (*messageHandle_EXT)(AFMessage_EXT *))
		{
			incommingMessageHandle_EXT = messageHandle_EXT;
		}

		uint8_t dataRequest(AFMessage_EXT * x)
		{
			mtMessage_t msg;
			msg.cmdId = 0x02;
			msg.cmdType = 0x24;
			msg.dataLength = 20 + x->asduLen() + 1;

			uint8_t *MTDataPtr = msg.pData;
			*MTDataPtr++ = x->addrMode();
			MTDataPtr += hostIntToANSI(MTDataPtr, x->dstAddr());
			*MTDataPtr++ = x->dstEndpoint();
			//=====
			MTDataPtr += hostIntToANSI(MTDataPtr, x->dstPanId());
			//=====
			*MTDataPtr++ = x->srcEndpoint();
			MTDataPtr += hostIntToANSI(MTDataPtr, x->clusterId());
			*MTDataPtr++ = x->tranSeqNum();
			*MTDataPtr++ = x->options();
			*MTDataPtr++ = x->radius();
			//=====
			MTDataPtr += hostIntToANSI(MTDataPtr, x->asduLen()); // 16-bit integer to store the APDU length
			//=====

			memcpy((void *)MTDataPtr, (void *)x->asdu(), (size_t)x->asduLen());
			mtBuildAndSendCmd(&msg);
			return SUCCESS;
		}
	}
}// namespace kapi

uint8_t MT_AfCommandProcessing(uint8_t *pBuf)
{
	uint8_t status = MT_RPC_SUCCESS;
	switch (pBuf[MT_RPC_POS_CMD1])
	{
		case AFMsg_Ind:
			MTAFIncomingMsgProc(pBuf);
		break;

		case AFMsg_EXT_Ind:
			MTAFIncomingMsgEXTProc(pBuf);
		break;

		case MT_AF_DATA_REQUEST:
			MTAfDataRequestConfirm(pBuf);
		break;
		default:
		{

		}
			status = MT_RPC_ERR_COMMAND_ID;
		break;
	}
	return status;
}

uint8_t MT_AfDataRequest(AFMessage * x,uint8_t *result)
{
	mtMessage_t msg;
	msg.cmdId = 0x01;
	msg.cmdType = 0x24;
	msg.dataLength = ZBNWKADDRLEN + ZB_EP_LEN + ZB_EP_LEN + ZB_CLUSTERID_LEN + ZB_TRANSEQNUM_LEN + 1 + 1 + x->asduLen()+1;

	uint8_t *MTDataPtr = msg.pData;
	 MTDataPtr += hostIntToANSI(MTDataPtr, x->dstAddr(), 256);
	*MTDataPtr++ = x->dstEndpoint();
	*MTDataPtr++ = x->srcEndpoint();
	 MTDataPtr += hostIntToANSI(MTDataPtr, x->clusterId(), 256);
	*MTDataPtr++ = x->tranSeqNum();
	*MTDataPtr++ = x->options();
	*MTDataPtr++ = x->radius();
	*MTDataPtr++ = x->asduLen();												// 8-bit integer to store the APDU length

	memcpy((void *)MTDataPtr, (void *)x->asdu(), (size_t)x->asduLen());
	mtBuildAndSendCmd(&msg);
	return SUCCESS;
}




uint8_t AfDataRequest(AFMessage *x)
{
	return MT_AfDataRequest(x);
}

void register_endpoint_TI(uint8_t endpoint_id, uint16_t profile_id,uint16_t device_id, uint8_t device_ver)
{
	uint8_t px[64];
	uint8_t len =0;
	px[len++] = endpoint_id;

	kIntToMem((void*)(px+len),(void*)&profile_id,2);
	len += 2;

	kIntToMem((void*)(px+len),(void*)&device_id,2);
	len += 2;

	px[len++] = device_ver;
	px[len++] = 0; //LatencyReq;
	px[len++] = 0; //AppNumInClusters
	px[len++] = 0; //AppNumOutClusters
	syslog(LOG_INFO, "registering endpoint: %02X", endpoint_id);
	MTBuildAndSendCmd(0x24, 00, len, px); //register on TI Zigbee Chip
	return;
}

void register_interpan_cb_TI(uint8_t ep)
{
	mtMessage_t msg;
	msg.cmdId = 0x10;
	msg.cmdType = 0x24;
	msg.dataLength = 0;
	msg.pData[msg.dataLength++] = 0x02;
	msg.pData[msg.dataLength++] = ep;
	mtBuildAndSendCmd(&msg);
}
void interPanSet(uint8_t ch)
{
	mtMessage_t msg;
	msg.cmdId = 0x10;
	msg.cmdType = 0x24;
	msg.dataLength = 0;
	msg.pData[msg.dataLength++] = 0x01;
	msg.pData[msg.dataLength++] = ch;
	mtBuildAndSendCmd(&msg);
}

void intraPanSet()
{
	mtMessage_t msg;
	msg.cmdId = 0x10;
	msg.cmdType = 0x24;
	msg.dataLength = 0;
	msg.pData[msg.dataLength++] = 0x00;
	mtBuildAndSendCmd(&msg);
}
