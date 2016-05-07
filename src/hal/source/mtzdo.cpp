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
#include "mtzdo.h"
#include "mt_helper.h"
#include "mtproc.h"
#include "kglobal.h"
#include <string.h>

namespace MT {
	namespace ZDO {
//		typedef enum {
//			DEV_HOLD,
//			DEV_INIT,
//			DEV_NWK_DISC,
//			DEV_NWK_JOINING,
//			DEV_NWK_REJOIN,
//			DEV_END_DEVICE_UNAUTH,
//			DEV_END_DEVICE,
//			DEV_ROUTER,
//			DEV_COORD_STARTING,
//			DEV_ZB_COORD,
//			DEV_NWK_ORPHAN
//		} devStates_t;
		volatile uint8_t devState = 0x00;
		uint8_t zdoState()
		{
			return static_cast<uint8_t>(devState);
		}
		bool zdoCoordReady()
		{
			return (devState == 0x09);
		}
	}
}
//static uint8_t MT_ZDO_STARTUP_FROM_APP_Rsp(uint8_t *pBuf, uint8_t *returnBuffer);

static uint8_t MTZDOStateChangeProc(uint8_t *pBuf, uint8_t *returnBuffer)
{
	MT::ZDO::devState = pBuf[MT_RPC_POS_DAT0];
	LOG_ERROR_MESSAGE("zdo state: %02x", MT::ZDO::devState);
	return SUCCESS;
}

uint8_t MT_ZdoCommandProcessing(uint8_t* pBuf)
{
	uint8_t status = MT_RPC_SUCCESS;
	uint8_t (*callback)(uint8_t*,uint8_t*) =NULL;

	if (mtRecv(pBuf) < 0)
	{
		switch (pBuf[MT_RPC_POS_CMD1])
		{
			case MT_ZDO_STATE_CHANGE_IND:
	//			callback=MTZDOStateChangeProc;
	//			mt_recv(pBuf[MT_RPC_POS_CMD0],pBuf[MT_RPC_POS_CMD1], pBuf, callback);
				MTZDOStateChangeProc(pBuf, NULL);
			break;
			case MT_ZDO_END_DEVICE_ANNCE_IND:
	//			MTZDOEndDeviceAnnceProc(pBuf);
			break;
			case MT_ZDO_NODE_DESC_RSP:
				//MT_ZDO_Node_Desc_Rsp_Proc(pBuf);
			break;
			case MT_ZDO_ACTIVE_EP_RSP:
				//Replaced by Active_EP_rsp_proc(afmessage_t *msg)
				//MT_ZDO_Active_EP_Rsp_Proc(pBuf);
			break;
			case MT_ZDO_SIMPLE_DESC_RSP:
				//Replaced by Simple_Desc_rsp_proc(afmessage_t *msg)
				//MT_ZDO_Simple_Desc_Rsp_Proc(pBuf);
			break;
//			case MT_ZDO_STARTUP_FROM_APP:
	//			callback = MT_ZDO_STARTUP_FROM_APP_Rsp;
	//			MT_ZDO_STARTUP_FROM_APP_Rsp(pBuf);
//				break;
			case MT_ZDO_MGMT_DIRECT_JOIN_RSP:
	//			callback = mtZDOMGMTDirectJoinRspProc;
	//			mt_recv(pBuf[MT_RPC_POS_CMD0],pBuf[MT_RPC_POS_CMD1], pBuf, callback);
				break;
			case MT_ZDO_MGMT_PERMIT_JOIN_RSP:
				//LOG_MESSAGE("got rsp");
				break;
			default:
	//			#ifdef MT_DEBUG_FUNC
	//			if (pBuf[MT_RPC_POS_CMD0] != 0x65)
	//				SendDebugMsgUint8(str0, strlen(str0), pBuf, (size_t)(pBuf[0]+3));
	//			#endif
				status = MT_RPC_ERR_COMMAND_ID;
			break;
		}
	}
  return status;
}

int MT_ZDO_MGMT_PERMIT_JOIN_REQ_FUNC(uint8_t PermitDuration, uint8_t TC_Significance)
{
	mtMessage_t req;
	uint8_t status;
	req.cmdType = 0x25;
	req.cmdId = 0x36;
	req.dataLength = 4;
	req.pData[0] = 0;
	req.pData[1] = 0;
	req.pData[2] = PermitDuration;
	req.pData[3] = TC_Significance;

	if (__MTSEND__(&req,&status,1,NULL) < 0)
	{
		LOG_ERROR_MESSAGE("send zdo msg failed");
		return -1;
	} else
	{
		LOG_ERROR_MESSAGE("ZDO start response: %02x", status);
	}

	return 0;
}

int MT_ZDO_STARTUP_FROM_APP_Req(uint16_t *StartDelay)
{
	uint16_t cow=0x0000;
	mtMessage_t req;
	uint8_t status;
	req.cmdType = 0x25;
	req.cmdId = MT_ZDO_STARTUP_FROM_APP;
	req.dataLength = 2;
	req.pData[0] = 0;
	req.pData[1] = 0;

	if (__MTSEND__(&req,&status,1,NULL) < 0)
	{
		LOG_ERROR_MESSAGE("send zdo msg failed");
		return -1;
	} else
	{
		LOG_ERROR_MESSAGE("ZDO start response: %02x", status);
	}


//	MT_ZDO_MGMT_PERMIT_JOIN_REQ_FUNC();
	return 0;
}


uint8_t MT_ZDO_STARTUP_FROM_APP_Rsp(uint8_t *pBuf, uint8_t *returnBuffer)
{
//	LOG_VAR(pBuf[MT_RPC_POS_DAT0], %02x);
//	release_channel();
//	mtMessage_t req,rsp;
//	req.cmdType =
	return 0;
}
//uint8_t MT_ZDO_Simple_Desc_Req(endpoint_t *ep)
//{
//	uint8_t cmd0 = 0x25;
//	uint8_t cmd1 = 0x04;
//	uint8_t dataLen = 5;
//	uint8_t pData[255];
//	uint8_t *ppData = pData;
//	if (ep == NULL) {
//		printf("no EP \n");
//		return 0;
//	}
//
//	timemcpy(ppData, ep->pNwkAddr, 2);
//	ppData += 2;
//	timemcpy(ppData, ep->pNwkAddr, 2);
//	ppData += 2;
//	*ppData = ep->EndpointAddr;
//	MTBuildAndSendCmd(cmd0, cmd1, dataLen, pData);
//	return 0;
//}
//
//uint8_t MT_ZDO_Simple_Desc_Req_All(device_t *device) {
//	endpoint_t *x;
//	x = GetFirstActiveEndpoint(device);
//	while (x!=NULL) {
////		Simple_Desc_Req(x);
//		x = GetNextActiveEndpoint(x);
//		//=====Yuming Liang 04/10/2012=====
////		sleep(3);
//		//=====Yuming Liang 04/10/2012=====
//	}
//	return 0;
//}
//
//
//uint8_t MT_ZDO_Active_EP_Req(device_t *device)
//{
//	uint8_t cmd0=0x25;
//	uint8_t cmd1=0x05;
//	uint8_t dataLen = 4;
//	uint8_t pData[4];
//	timemcpy(pData, &(device->NwkAddr), 2);
//	timemcpy(pData+2,&(device->NwkAddr), 2);
//	MTBuildAndSendCmd(cmd0, cmd1, dataLen, pData);
//	return 0;
//}
//
//uint8_t MT_ZDO_Active_EP_Rsp_Proc(uint8_t *pBuf)
//{
//	uint8_t *temp = pBuf+MT_RPC_POS_DAT0;
//	uint16_t SrcAddr, NwkAddr;
//	uint8_t status;
//	uint8_t ActiveEPCount;
//	uint8_t *ActiveEPList;
//	device_t *x;
//	endpoint_t *y;
//
//	//timemcpy(SrcAddr,temp,2);
//	SrcAddr = build_uint16(temp[0], temp[1]);
//	temp += 2;
//	status = *temp;
//	temp++;
//	//timemcpy(NwkAddr,temp,2);
//	NwkAddr = build_uint16(temp[0], temp[1]);
//	temp += 2;
//	ActiveEPCount = *temp;
//	temp++;
//	x = GetAssocByNwkAddr(NwkAddr);
//	if (x == NULL) {
//		return 1;
//	}
//	while (ActiveEPCount--) {
////		addep(x,*temp++);
//	}
//	return 0;
//}
//
//uint8_t MT_ZDO_Node_Desc_Rsp_Proc(uint8_t *pBuf)
//{
//	//syslog(LOG_INFO, "Node_Desc_rsp received");
//	return 0;
//}
//
//uint8_t MT_ZDO_Node_Desc_Req(device_t *device)
//{
//	uint8_t dataLen = 4;
//	uint8_t cmd0 = 0x25;
//	uint8_t cmd1 = 0x02;
//	uint8_t pData[4];
//	timemcpy(pData, &device->NwkAddr, 2);
//	timemcpy(pData+2, &device->NwkAddr, 2);
//	MTBuildAndSendCmd(cmd0, cmd1, dataLen, pData);
//	return 0;
//}
//
//uint8_t MT_ZDO_Msg_CB_Register(uint8_t *CID)
//{
//	uint8_t dataLen = 2;
//	uint8_t cmd0 = 0x25;
//	uint8_t cmd1 = 0x3e;
//	uint8_t pData[2];
//
//	timemcpy(pData, CID, dataLen);
//	PrintHex(pData,2);
//	MTBuildAndSendCmd(cmd0, cmd1, dataLen, pData);
//	return 0;
//}

//uint8_t zdo_cb(afmessage_t *msg)
//{
//	int rv;
//	if (msg->DstEndPoint != ZDO_EP) return 1;
//
//	switch (msg->ClusterID) {
//		case Node_Desc_rsp:
//			rv = Node_Desc_rsp_proc(msg);
//		break;
//		case Active_EP_rsp:
//			rv = Active_EP_rsp_proc(msg);
//		break;
//		case Simple_Desc_rsp:
//			rv = Simple_Desc_rsp_proc(msg);
//		break;
//		default:
//			rv = -1;
//		break;
//	}
//	return rv;
//}


//uint8_t Simple_Desc_rsp_proc(afmessage_t *msg) {
//	uint8_t transeqnum, status; // transaction sequence number of ZDP
//	uint8_t *pasdu = msg->asdu;
//	uint16_t NwkAddrOfInterest;
//	uint8_t EndpointAddr;
//	endpoint_t *pEndPoint, *y;
//	endpoint_t EndPoint;
//	SimpleDescriptor_t *SimpleDesc;
//	int i;
//	int SimpleDescLength;
//	char hdr[] = "Simple_Desc_rsp";
//	SimpleDescriptor_t *pDesc;
//
//	// check for consistencies
//	if (msg->asdu == NULL) {
//		syslog(LOG_INFO, "%s error: ASDU is NULL", hdr);
//		return 1;
//	}
//	// check to see if the message is a request to a previous response
//	if (ZDPMessageCheckIn(msg) != SUCCESS) {return EEXIST;}
//
//	// de-construct the message
//	SimpleDescLength = pasdu[4];
//
//	if (msg->asduLen != 5 + SimpleDescLength) {
//		//TranSeqnum(1 byte) + status(1 byte) + NwkAddr(2 bytes) + ep_count(1 byte) + ep_list(var bytes)
//		syslog(LOG_INFO, "%s error: bad length: %d", hdr, msg->asduLen);
//		return 1;
//	}
//
//	transeqnum = *pasdu++;
//	status = *pasdu++;
//	if (status != SUCCESS) {
//		syslog(LOG_INFO, "%s error: bad status -- %02X", hdr, status);
//		return 1;
//	}
//
//	NwkAddrOfInterest = build_uint16(pasdu[0], pasdu[1]);
//	pasdu += sizeof(NwkAddrOfInterest);
//	pasdu++;
////	EndpointAddr = *pasdu++;
//
//	// retrieve the intended endpoint
//	pDesc = NewSimpleDescriptor();
//	pEndPoint = GetEndpointByAddr(NwkAddrOfInterest, *pasdu);
//	if (pDesc == NULL || pEndPoint == NULL) {
//		#ifdef MT_DEBUG_FUNC
//		if (pDesc == NULL){
//			vTraceLog("Simple_Desc_rsp_proc(), pDesc = NULL");
//		}
//		if (pEndPoint == NULL){
//			vTraceLog("Simple_Desc_rsp_proc(), pEndPoint = NULL");
//		}
//		#endif
//		return 1;
//	};
//
//
//	AddSimpleDescriptor(pEndPoint, pDesc);
//	SetSimpleDescriptor(pEndPoint,pasdu);
//
//	//Yuming 03-02-2012
//	pEndPoint->Active = TRUE;
//
//	//match profile id, establish data structure
//	APSNewPeerHandle(pEndPoint);
//
//	#ifdef MT_DEBUG_FUNC
//	vTraceLog("EndPoint=%02X, ProfileID=%04X, DeviceID=%04X", pEndPoint->EndpointAddr, pDesc->ProfileID, pDesc->DeviceID);
//	for (i=0;i<(int)pDesc->InclusterCount; i++) {
//		vTraceLog("Incluster%d = %04X", i+1, pDesc->InclusterList[i]);
//	}
//	for (i=0;i<(int)pDesc->OutclusterCount; i++) {
//		vTraceLog("Outcluster%d = %04X", i+1, pDesc->OutclusterList[i]);
//	}
//	#endif
//
//	return SUCCESS;
//
//}
//
//uint8_t Active_EP_rsp_proc(afmessage_t *msg) {
//	uint8_t transeqnum, status; // transaction sequence number of ZDP
//	uint8_t *pasdu = msg->asdu;
//	uint16_t NwkAddrOfInterest;
//	device_t *x;
//	endpoint_t *y;
//	int ActiveEPCount;
//	char hdr[] = "Active_EP_rsp";
//
//	char debugbuf[256];
//	uint16_t debugnwkaddr;
//	endpoint_t *debugdev;
//
//	//Check for consistency
//	if (msg->asdu == NULL) {
//		syslog(LOG_INFO, "%s error: ASDU is NULL", hdr);
//		return 1;
//	}
//	//Check for Duplicate
//	if (ZDPMessageCheckIn(msg) != SUCCESS) {
//		sprintf(debugbuf, "Active_EP_rsp: previous message has checked in");
//		SendDebugMsg(debugbuf, strlen(debugbuf));
//		return EEXIST;
//	}
//
//	ActiveEPCount = pasdu[4];
//	if (msg->asduLen != 5 + ActiveEPCount) {
//		syslog(LOG_INFO, "%s error: bad node descriptor length: %d", hdr, msg->asduLen);
//		return 1;
//	}
//
//	transeqnum = *pasdu++;
//	status = *pasdu++;
//
//	if (status != SUCCESS) {
//		syslog(LOG_INFO, "%s error: bad status -- %02X", hdr, status);
//		return 1;
//	}
//
//	NwkAddrOfInterest = build_uint16(pasdu[0], pasdu[1]);
//	pasdu += sizeof(NwkAddrOfInterest);
////	x = GetAssocByNwkAddr(NwkAddrOfInterest);
//	if (x == NULL) {
//		syslog(LOG_INFO, "%s error: assoc not exist", hdr);
//		return 1;
//	}
//
//	while (ActiveEPCount--) {
//		//addep(x,*++pasdu);
//		AddEndpoint(NwkAddrOfInterest, *++pasdu);
//	}
//
//	return SUCCESS;
//}

//uint8_t Node_Desc_rsp_proc(afmessage_t *msg)
//{
//	uint8_t transeqnum, status; // transaction sequence number of ZDP
//	uint8_t *pasdu = msg->asdu;
//	uint16_t NwkAddrOfInterest;
//	device_t x;
//
//	char hdr[] = "Node_Desc_rsp";
//
//	if (ZDPMessageCheckIn(msg) != SUCCESS) {return EEXIST;}
//
//	if (msg->asduLen != 17) {
//		//1+1+2+13
//		syslog(LOG_INFO, "%s error: bad node descriptor length: %d", hdr, msg->asduLen);
//		return 1;
//	}
//	if (msg->asdu == NULL) {
//		syslog(LOG_INFO, "%s error: ASDU is NULL", hdr);
//		return 1;
//	}
//
//	transeqnum = *pasdu++;
//	status = *pasdu++;
//	if (status != ZDP_SUCCESS) {
//		syslog(LOG_INFO, "%s error: bad status -- %02X", hdr, status);
//		return 1;
//	}
//	NwkAddrOfInterest = build_uint16(pasdu[0], pasdu[1]);
//	pasdu += sizeof(NwkAddrOfInterest);
//
//	x.NwkAddr = NwkAddrOfInterest;
//	getassoc(&x);
//	x.NodeDescriptor.LogicalType = *pasdu & 0x07;
//	//syslog(LOG_INFO, "%s logicaltype : %02X", hdr,x->NodeDescriptor.LogicalType);
//	x.NodeDescriptor.ComplexDescAvail = (*pasdu & 0x08) >> 3;
//	x.NodeDescriptor.UserDescAvail = (*pasdu & 0x10) >> 4;
//	x.NodeDescriptor.Reserved = (*pasdu & 0xE0) >> 5;
//	pasdu++;
//	x.NodeDescriptor.APSFlags = *pasdu & 0x07;
//	x.NodeDescriptor.FrequencyBand = (*pasdu & 0xF8) >> 3;
//	//syslog(LOG_INFO, "%s FrequencyBand : %02X", hdr,x->NodeDescriptor.FrequencyBand);
//	pasdu++;
//	x.NodeDescriptor.CapabilityFlags = *pasdu;
//	pasdu++;
//	x.NodeDescriptor.ManufacturerCode = build_uint16(pasdu[0], pasdu[1]);
//	pasdu += 2;
//	x.NodeDescriptor.MaxBufferSize = *pasdu++;
//	x.NodeDescriptor.MaxInTransferSize = build_uint16(pasdu[0], pasdu[1]);
//	pasdu += 2;
//	x.NodeDescriptor.ServerMask = build_uint16(pasdu[0], pasdu[1]);
//	pasdu += 2;
//	x.NodeDescriptor.MaxOutTransferSize = build_uint16(pasdu[0], pasdu[1]);
//	pasdu += 2;
//	x.NodeDescriptor.DescriptorCapability = *pasdu;
////	setassoc(&x);
//	//syslog(LOG_INFO, "IEEE Address: %08X%08X", (unsigned)(x.IEEEAddr>>32), (unsigned)(x.IEEEAddr));
//	//assocunlock(x);
//	return 0;
//}
//uint8_t Node_Desc_Req(device_t *pDevice) {
//	uint8_t ret;
//	afmessage_t *x = NewAFMessage();
//	x->ClusterID = 0x0002;
//	x->DstAddr = pDevice->NwkAddr;
//	x->DstEndPoint = 0x00;
//	x->SrcEndPoint = 0x00;
//	x->TranSeqNum = GetAPSCounter();
//	x->Options = 0x00;
//	x->Radius = 0x01;
//	x->asduLen = 3;
//	x->asdu[0] = GetZDPSeqNum();
//	uint16touint8(x->DstAddr, x->asdu+1);
//	ret = AfDataRequestWait(x);
//	FreeAFMessage(x);
//	return ret;
//}
//uint8_t Active_EP_Req(device_t *pDevice) {
//	uint8_t ret;
//	afmessage_t *x = NewAFMessage();
//	x->ClusterID = 0x0005;
//	x->DstAddr = pDevice->NwkAddr;
//	x->DstEndPoint = 0x00;
//	x->SrcEndPoint = 0x00;
//	x->TranSeqNum = GetAPSCounter();
//	x->Options = 0x00;
//	x->Radius = 0x01;
//	x->asduLen = 3;
//	x->asdu[0] = GetZDPSeqNum();
//	uint16touint8(x->DstAddr, x->asdu+1);
//	ret = AfDataRequestWait(x);
//	FreeAFMessage(x);
//	return ret;
//}
//uint8_t Simple_Desc_Req(endpoint_t *pEndpoint) {
//	uint8_t ret;
//
//	afmessage_t *x = NewAFMessage();
//	x->ClusterID = 0x0004;
//	x->DstAddr = GetEndpointNwkAddr(pEndpoint);
//	x->DstEndPoint = 0x00;
//	x->SrcEndPoint = 0x00;
//	x->TranSeqNum = GetAPSCounter();
//	x->Options = 0x00;
//	x->Radius = 0x01;
//	x->asduLen = 4;
//	x->asdu[0] = GetZDPSeqNum();
//	uint16touint8(x->DstAddr, x->asdu+1);
//	x->asdu[3] = pEndpoint->EndpointAddr;
//	ret = AfDataRequestWait(x);
//	FreeAFMessage(x);
//	return ret;
//}
//uint8_t ZDPMessageCheckIn(afmessage_t *Incoming) {
//	afmessage_t *MessageSent=NULL;
//	uint8_t ZDPSeqNumTemp = Incoming->asdu[0];
//	int ret;
//	MessageSent = GetAFZDPMessage(ZDPSeqNumTemp);
//	if (MessageSent == NULL) {return -1;}
//	ret = AFMessageRspCheckIn(MessageSent);
//	if (ret != SUCCESS) {
//		vTraceLog("Unexpected Message,  SA: %04X, cId: %04X", Incoming->SrcAddr, Incoming->ClusterID);
//	}
//	return ret;
//}
//uint8_t RequestNodeInfoByNwkAddr(uint16_t NwkAddr) {
//	device_t *x;
//	uint64_t IEEEAddr;
//
//	UTIL_ASSOC_GET_WITH_ADDRESS_Req(0, NwkAddr);
//	// wait for the TI chip to respond, the message format dose not have
//	// the sequence number or anything similar to identify the response the message
//	sleep(1);
//	x = GetAssocByNwkAddr(NwkAddr);
//	if (x == NULL) {
//		vTraceLog("Assoc not found");
//		return 1;
//	}
//	IEEEAddr = GetAssocIEEEAddr(NwkAddr);
//	SetAssocIEEEAddr(x,IEEEAddr);
//	vTraceLog("Assoc Found, IEEEAddr: %08X%08X", (uint32_t)(x->IEEEAddr>>32),(uint32_t)x->IEEEAddr);
////	GetAssocInfo(x);
//	return SUCCESS;
//}
//uint8_t MTZDOEndDeviceAnnceProc(uint8_t *pBuf) {
//	device_t *x;
//	uint16_t SrcAddr, NwkAddr;
//	uint64_t IEEEAddr;
//	uint8_t capability;
//
//
//	SrcAddr = build_uint16(pBuf[MT_ZDO_SRCADDR_POS],pBuf[MT_ZDO_SRCADDR_POS+1]);
//	NwkAddr = build_uint16(pBuf[MT_ZDO_NWKADDR_POS],pBuf[MT_ZDO_NWKADDR_POS+1]);
//	IEEEAddr = build_uint64(pBuf+MT_ZDO_IEEEADDR_POS);
//	capability = pBuf[MT_ZDO_CAP_POS];
//	x = GetAssocByIEEEAddr(IEEEAddr);
//	if (x == NULL) {
//		x = NewAssoc();
//		AddAssoc(x);
//	}
//	SetAssocIEEEAddr(x, IEEEAddr);
//	SetAssocNwkAddr(x, NwkAddr);
//	SetAssocCapability(x, capability);
//	GetAssocInfo(x);
//
//	return 0;
//}
/*uint8 MT_ZDO_Simple_Desc_Rsp_Proc(uint8 *pBuf)
{
	uint16_t SrcAddr, NwkAddr;
	uint8 status;
	uint8 len;
	uint8 ep;
	uint8 profileid[2];
	uint8 deviceid[2];
	uint8 deviceversion;
	uint8 numinclusters;
	uint8 inclusterlist[255];
	uint8 numoutclusters;
	uint8 outclusterlist[255];
	uint8 *temp = pBuf + MT_RPC_POS_DAT0;;
	endpoint_t *x;
	
	//timemcpy(srcaddr, temp, 2);
	SrcAddr = build_uint16(temp[0], temp[1]);
	temp += 2;
	status = *temp++;
	if (status) return 1;
	//timemcpy(wkaddr, temp, 2);
	NwkAddr = build_uint16(temp[0], temp[1]);
	temp += 2;
	len = *temp++;
	ep = *temp++;
	if ((x = getep(NwkAddr, ep)) == NULL) return 1;
	
	timemcpy(x->ProfileID, temp, 2);
	temp += 2;
	timemcpy(x->DeviceID, temp, 2);
	temp += 2;
	x->DeviceVersion = *temp++;
	x->numinclusters = *temp++;
	x->inclusterlist = (uint8_t *)malloc(ZBNWKADDRLEN * x->numinclusters);
	timemcpy(x->inclusterlist, temp, (size_t)(x->numinclusters*2));
	temp += (int)(x->numinclusters*2);
	x->numoutclusters = *temp++;
	x->outclusterlist = (uint8_t *)malloc(ZBNWKADDRLEN * x->numoutclusters);
	timemcpy(x->outclusterlist, temp, (size_t)(x->numoutclusters*2));
	
	return 0;
}*/

//uint8_t Simple_Desc_rsp_proc(afmessage_t *msg)
//{
//	uint8_t transeqnum, status; // transaction sequence number of ZDP
//	uint8_t *pasdu = msg->asdu;
//	uint16_t NwkAddrOfInterest;
//	uint8_t EndPointID;
//	device_t x;
//	endpoint_t *pEndPoint;
//	endpoint_t EndPoint;
//	SimpleDescriptor_t *SimpleDesc;
//	cluster_t Cluster;
//	int i;
//	int SimpleDescLength;
//	char hdr[] = "Simple_Desc_rsp";
//	char debugbuf[256];
//	// check for consistencies
//	if (msg->asdu == NULL) {
//		syslog(LOG_INFO, "%s error: ASDU is NULL", hdr);
//		return 1;
//	}
//	if (ZDPMessageCheckIn(msg) != SUCCESS) {return EEXIST;}
//
//	SimpleDescLength = pasdu[4];
//
//	if (msg->asduLen != 5 + SimpleDescLength) {
//		//TranSeqnum(1 byte) + status(1 byte) + NwkAddr(2 bytes) + ep_count(1 byte) + ep_list(var bytes)
//		syslog(LOG_INFO, "%s error: bad length: %d", hdr, msg->asduLen);
//		return 1;
//	}
//
//	transeqnum = *pasdu++;
//	status = *pasdu++;
//	if (status != SUCCESS) {
//		syslog(LOG_INFO, "%s error: bad status -- %02X", hdr, status);
//		return 1;
//	}
//	NwkAddrOfInterest = build_uint16(pasdu[0], pasdu[1]);
//	pasdu += sizeof(NwkAddrOfInterest);
//	pasdu++;
//	EndPointID = *pasdu++;
//
//	EndPoint.NwkAddr = NwkAddrOfInterest;
//	EndPoint.EndpointAddr = EndPointID;
//	getendpoint(&EndPoint);
//	EndPoint.ProfileID = build_uint16(pasdu[0], pasdu[1]);
//	pasdu +=sizeof(EndPoint.ProfileID);
//	EndPoint.DeviceID = build_uint16(pasdu[0], pasdu[1]);
//	pasdu +=sizeof(EndPoint.DeviceID);
//
//	EndPoint.DeviceVersion = *pasdu++;
//	EndPoint.numinclusters = *pasdu++;
//	setendpoint(&EndPoint);
//
//
//	for (i=0;i<(int)EndPoint.numinclusters; i++) {
//		Cluster.inout = INCLUSTER;
//		Cluster.ClusterID = build_uint16(pasdu[0], pasdu[1]);
//		addcluster(&EndPoint, &Cluster);
//		pasdu += sizeof(uint16_t);
//	}
//	EndPoint.numoutclusters = *pasdu++;
//	for (i=0;i<(int)EndPoint.numoutclusters; i++) {
//		Cluster.inout = OUTCLUSTER;
//		Cluster.ClusterID = build_uint16(pasdu[0], pasdu[1]);
//		addcluster(&EndPoint, &Cluster);
//		pasdu += sizeof(uint16_t);
//	}
//
//	//getendpoint(&EndPoint);
//	#ifdef MT_DEBUG_FUNC
//	sprintf(debugbuf, "EndPoint=%02X, ProfileID=%04X, DeviceID=%04X", EndPoint.EndpointAddr, EndPoint.ProfileID, EndPoint.DeviceID);
//	SendDebugMsg(debugbuf, strlen(debugbuf));
//	#endif
//	//assocunlock(x);
//	return SUCCESS;
//
//}
