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
#ifndef MTZDO_H
#define MTZDO_H
#include <stdint.h>
#include "mtproc.h"
#include "kzdef.h"
#include "MT_RPC.h"
#include "hal.h"
//#ifdef MT_DEBUG_FUNC
//	#include "mtdebug.h"
//#endif
/**************************************************************************************************
 * CONSTANTS
 **************************************************************************************************/
#define MT_ZDO_SRCADDR_POS 3
#define MT_ZDO_NWKADDR_POS 5
#define MT_ZDO_IEEEADDR_POS 7
#define MT_ZDO_CAP_POS 15
#define MT_ZDO_END_DEVICE_ANNCE_IND_LEN   0x0D
#define MT_ZDO_ADDR_RSP_LEN               0x0D
#define MT_ZDO_BIND_UNBIND_RSP_LEN        0x03
#define MT_ZDO_BEACON_IND_LEN             21
#define MT_ZDO_BEACON_IND_PACK_LEN        (MT_UART_TX_BUFF_MAX - SPI_0DATA_MSG_LEN)
#define MT_ZDO_JOIN_CNF_LEN               5

#define MTZDO_RESPONSE_BUFFER_LEN   100
#define MTZDO_MAX_MATCH_CLUSTERS    16
#define MTZDO_MAX_ED_BIND_CLUSTERS  15

// Conversion from ZDO Cluster Id to the RPC AREQ Id is direct as follows:
#define MT_ZDO_CID_TO_AREQ_ID(CId)  ((uint8_t)(CId) | 0x80)

#define MT_ZDO_STATUS_LEN 1
uint8_t MT_ZdoCommandProcessing(uint8_t *pBuf);
int MT_ZDO_STARTUP_FROM_APP_Req(uint16_t *StartDelay);

uint8_t MT_ZDO_Msg_CB_Register(uint8_t *CID);
//uint8_t zdo_cb(afmessage_t *msg);
//uint8_t MTZDOEndDeviceAnnceProc(uint8_t *pBuf);
//uint8_t Node_Desc_Req(device_t *pDevice);
//uint8_t Node_Desc_rsp_proc(afmessage_t *msg);
//uint8_t Active_EP_Req(device_t *pDevice);
//uint8_t Active_EP_rsp_proc(afmessage_t *msg);
//uint8_t Simple_Desc_Req(endpoint_t *pEndpoint);
//uint8_t Simple_Desc_rsp_proc(afmessage_t *msg);
//uint8_t ZDPMessageCheckIn(afmessage_t *Incoming);
//uint8_t GetAssocInfo(device_t *pDevice);
uint8_t RequestNodeInfoByNwkAddr(uint16_t NwkAddr);
int MT_ZDO_MGMT_PERMIT_JOIN_REQ_FUNC(uint8_t PermitDuration, uint8_t TC_Significance);
uint8_t epcbreg();
namespace MT {
	namespace ZDO {
		uint8_t zdoState();
		bool zdoCoordReady();
	}
}
#endif

