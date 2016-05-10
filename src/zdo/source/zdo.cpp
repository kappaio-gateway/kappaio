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
#include "zdo.h"
#include "zdpconst.h"
#include "kglobal.h"
#include "kutil.h"
#include "hal.h"
#include "kjson.h"
#include <string.h>
#include "zdo_archive.h"
#include "restful.h"
#include "kjson.h"
#include "notification.h"
#include "mtzdo.h"

using namespace aps;
using namespace data_service;
using namespace std;

int node_desc_req(Device *pDevice);
int active_ep_req(Device *pDevice);
int simple_desc_req(Endpoint *pEndpoint);
int Mgmt_Permit_Joining_req(uint8_t PermitDuration, uint8_t TC_Significance, uint8_t& status);
//int Mgmt_Nwk_Update_Req(uint32_t ScanChannels, uint8_t ScanDuration, uint8_t ScanCount, uint8_t nwkUpdateId, uint16_t nwkManagerAddr);
//================================================================================================================
namespace {

	namespace zdp_frame_pos
	{
		const int RSPSTAT = 1;
		const int NADDROI = 2;
		const int MSGBODY = 4;
	}
	using namespace zdp_frame_pos;

	int zdpHdr_with_NwkaddrOfInterest(uint8_t* buf, uint16_t nwkaddr)
	{
		int i = 0;
		buf[i++] = cntr();
		i += hostIntToANSI(buf+i, nwkaddr);
		return i;
	}
	struct generic_zdp_frm  {
		uint16_t dstAddr_;
		generic_zdp_frm(uint16_t dstAddr):dstAddr_{dstAddr}
		{}
		int operator()(uint8_t* buf)
		{
			return zdpHdr_with_NwkaddrOfInterest(buf, dstAddr_);
		}
	};
	using node_desc_frm = generic_zdp_frm;
	using active_ep_frm = generic_zdp_frm;

	struct simple_desc_frm  {
		uint16_t dstAddr_;
		uint8_t epId_;
		simple_desc_frm(uint16_t dstAddr, uint8_t epId):dstAddr_{dstAddr}, epId_{epId}
		{}
		int operator()(uint8_t* buf)
		{
			int i = zdpHdr_with_NwkaddrOfInterest(buf, dstAddr_);
			buf[i++] = epId_;
			return i;
		}
	};
	struct Mgmt_Permit_Joining_req_frm {
		uint8_t PermitDuration_;
		uint8_t TC_Significance_;
		Mgmt_Permit_Joining_req_frm(uint8_t PermitDuration, uint8_t TC_Significance)
		:PermitDuration_{PermitDuration}, TC_Significance_{TC_Significance}
		{}
		int operator()(uint8_t* buf)
		{
			int i = 0;
			buf[i++] = cntr();
			buf[i++] = PermitDuration_;
			buf[i++] = TC_Significance_;
			return i;
		}
	};

	struct Mgmt_Leave_req_frm {
		uint64_t ieeeAddr_;
		Mgmt_Leave_req_frm(uint64_t ieeeAddr):ieeeAddr_{ieeeAddr}{}
		int operator()(uint8_t* buf)
		{
			int i = 0;
			buf[i++] = cntr();
			i += hostIntToANSI(buf+i, ieeeAddr_);
			i += hostIntToANSI(buf+i, (uint64_t)0);
			return i;
		}
	};
	struct IEEE_addr_req_frm {
		uint16_t NWKAddrOfInterest_;
		uint8_t RequestType_;
		uint8_t StartIndex_;
		IEEE_addr_req_frm(uint16_t NWKAddrOfInterest,uint8_t RequestType,uint8_t StartIndex)
		:NWKAddrOfInterest_{NWKAddrOfInterest},RequestType_{RequestType},StartIndex_{StartIndex}
		{}
		int operator()(uint8_t* buf)
		{
			int i = zdpHdr_with_NwkaddrOfInterest(buf, NWKAddrOfInterest_);
			i += hostIntToANSI(buf+i, NWKAddrOfInterest_);
			i += hostIntToANSI(buf+i, RequestType_);
			i += hostIntToANSI(buf+i, StartIndex_);
			return i;
		}
	};
	using APDU = ZDO::APDU;
}

//================================================================================================================
namespace zdo_private
{
	uint8_t *message_preprocess_(AFMessage * msg, size_t asduLen_exp, uint16_t * nwkaddr_of_interest)
	{
		uint8_t *pasdu = msg->asdu();
		if (msg->asduLen() != asduLen_exp)
		{
			LOG_ERROR_MESSAGE("bad msg length: %d", msg->asduLen());
			return NULL;
		}

		if (msg->asdu() == NULL)
		{
			LOG_ERROR_MESSAGE("ASDU is NULL");
			return NULL;
		}

		uint8_t transeqnum = *pasdu++;

		if (msg->clusterId() & ZDO_RESPONSE_BIT)
		{
			uint8_t status = *pasdu++;
			if (status != ZDP_SUCCESS)
			{
				LOG_ERROR_MESSAGE("bad rsp status: %02x, cid: %02x", status,msg->clusterId());
				return NULL;
			}
		}
		if (nwkaddr_of_interest)
		{
			pasdu += ANSITohostInt(*nwkaddr_of_interest, pasdu);
		}

		return pasdu;
	}

	void device_annce_proc(AFMessage *msg);
	void match_Desc_req_proc(AFMessage *msg);
	int zdo_callback(AFMessage *msg);
	int active_ep_req_apdu_process(Device *pDevice, uint8_t *apdu)
	{
		uint8_t *pasdu = apdu;
		size_t ActiveEPCount = *pasdu++;
		while (ActiveEPCount--) { pDevice->endpointPtr(*pasdu++);  }
		return 0;
	}

	int node_desc_req_apdu_process(NodeDescriptor_t *node_desc, uint8_t *apdu)
	{
		uint8_t *pasdu = apdu;
		node_desc->LogicalType = *pasdu & 0x07;
		node_desc->ComplexDescAvail = (*pasdu & 0x08) >> 3;
		node_desc->UserDescAvail = (*pasdu & 0x10) >> 4;
		node_desc->Reserved = (*pasdu & 0xE0) >> 5;
		node_desc->APSFlags = *++pasdu & 0x07;
		node_desc->FrequencyBand = (*pasdu & 0xF8) >> 3;
		node_desc->CapabilityFlags = *++pasdu;
		pasdu += ANSITohostInt(node_desc->ManufacturerCode, ++pasdu);
		node_desc->MaxBufferSize = *pasdu++;
		pasdu += ANSITohostInt(node_desc->MaxInTransferSize, pasdu);
		pasdu += ANSITohostInt(node_desc->ServerMask, pasdu);
		pasdu += ANSITohostInt(node_desc->MaxOutTransferSize, pasdu);
		node_desc->DescriptorCapability = *pasdu;
		return 0;
	}

	int simple_desc_req_apdu_process(SimpleDescriptor& simpleDesc, uint8_t * apdu)
	{
		uint8_t *pasdu = apdu;
		simpleDesc.endpointAddr = *pasdu++;

		pasdu += ANSITohostInt(simpleDesc.profileId,pasdu);
		pasdu += ANSITohostInt(simpleDesc.deviceId, pasdu);
		simpleDesc.deviceVersion = *pasdu++;
		simpleDesc.inclusterCount(*pasdu++);
		for (auto& i: simpleDesc.inclusterList) {
			pasdu += ANSITohostInt(i, pasdu);
		}
		simpleDesc.outclusterCount(*pasdu++);

		for (auto& i: simpleDesc.outclusterList) {
			pasdu += ANSITohostInt(i, pasdu);
		}

		for (auto i : simpleDesc.inclusterList) {
			LOG_MESSAGE("detected incluster  : %04x", i);
		}

		for (auto i : simpleDesc.outclusterList) {
			LOG_MESSAGE("detected outcluster : %04x",i);
		}


		return 0;
	}

	int recoverEndpoint(Device *self)
	{
		using JSON = kapi::JSON;

		JSON recvr(ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION);
		if (!recvr.good())
		{
			LOG_MESSAGE("Bad/nonexistent archive file, now re-initialize...");
			ZDO::ARCHIVE::clear();
			HAL::zcdStartupOptions(0x03);
			LOG_MESSAGE("kappaio going down for restart...");
			HAL::delayRestart(1);
			return -1;
		}

		recvr.forEachInObj([](const char* key,JSON& dev) {
			Device& assoc = thisDevice().assocs( static_cast<uint16_t>(dev["nwkAddr"].toInteger()) );
			assoc.ieee_id( dev["ieeeAddr"].toInteger() );
			assoc.active = 1;
			node_desc_req_apdu_process( &assoc.nodeDescriptor() , hexStrToByteVec(dev["NodeDescApduString"].toString()).data());
			active_ep_req_apdu_process( &assoc, hexStrToByteVec(dev["ActiveEndpointApduString"].toString()).data());

			for (auto& ep : assoc.endpoints()) {
				if ( ep.id() != 0 )	{
					JSON& epSimpleDescJson = dev["endpointList"][IntToHexStr(ep.id())]["SimpleDescApdu"];
					if (epSimpleDescJson.good()) {
						if (epSimpleDescJson.toString().size() > 0) {
							simple_desc_req_apdu_process(ep.simpleDescriptor(), hexStrToByteVec(epSimpleDescJson.toString()).data());
							aps_newpeer_handle(ep);
						}
					}
				}
			}
		});
		return 0;
	}

	int getAllEndpointSimpleDesc(Device *assoc)
	{
		int rtn = 0;
		timespec waitTimeBetweenCommands = {0,100000000};
		if (assoc==NULL)
		{
			return -1;
		}
		for (auto& x_ : assoc->endpoints())
		{
			Endpoint *x = &x_;
			if (x->id()!=0)
			{
				if (simple_desc_req(x) < 0)
				{
					LOG_VAR(x->id(), %02x);
					rtn = -1;
				}
				/* pause after each request 06-05-2014 */
				kNanosleep(&waitTimeBetweenCommands);
				/* pause after each request 06-05-2014 */
			}
		}
		return rtn;
	}
}

void startZDO()
{
	selfptr()->endpointPtr(0)->messageHandler = zdo_private::zdo_callback;

	/* change the number of simultaneous ZDO from 2 to 1, 06-09-2014 */
	//zdo_private::messageService = new messageService_c(0,1, "ep0");
	/* change the number of simultaneous ZDO from 2 to 1, 06-09-2014 */

	mt_device_info_t deviceInfo;
	if (start_zdo() == 0) {
		LOG_MESSAGE("ZDO is now in Coord mode");
	}
	zdo_private::recoverEndpoint(selfptr());
	using namespace kapi;
	using namespace kapi::notify;
	using Context =  ApplicationInterface::Context;

	// close the network as default
	{
		uint8_t status=0xff;
		Mgmt_Permit_Joining_req(0x00,0x00,status);
	}

	handler(ApplicationInterface::EventTag, selfptr()->endpointPtr(0)->uri() + "/Mgmt_Permit_Joining_req", [](Context C) {
		JSON arg(C.parameter().c_str());
		uint8_t pd 	=  arg["PermitDuration"].toInteger();
		uint8_t tcs =  arg["TC_Significance"].toInteger();
		JSON rsp{JSONType::JSON_OBJECT};
		uint8_t status=0xff;
		Mgmt_Permit_Joining_req(pd,tcs,status);
		rsp["status"] = status;
		rsp["pd"] = pd;
		rsp["tcs"] = tcs;
		C.response(rsp.stringify());
	});

	handler(ApplicationInterface::EventTag, selfptr()->endpointPtr(0)->uri() + "/Mgmt_Leave_req", [](Context C) {
		JSON arg(C.parameter().c_str());
		JSON rsp{JSONType::JSON_OBJECT};
		int status = -1;
		uint64_t ieeeAddr = arg["ieeeAddr"].toInteger();

		thisDevice().removeAssoc(thisDevice().assocs(ieeeAddr).id());
		removeDev(ieeeAddr);

		APDU{0, 0x0034, Mgmt_Leave_req_frm(ieeeAddr)}.send([&status](AFMessage& x)
		{
			status = x.asdu()[1];
		});

		rsp["status"] = status;
		rsp["ieeeAddr"] = IntToHexStr(ieeeAddr);
		C.response(rsp.stringify());
	});
	/*
	handler(ApplicationInterface::EventTag, selfptr()->endpointPtr(0)->uri() + "/IEEE_addr_req", [](Context C) {
		JSON arg(C.parameter().c_str());
		JSON rsp{JSONType::JSON_OBJECT};
		int status = -1;
		uint16_t NWKAddrOfInterest = arg["NWKAddrOfInterest"].toInteger();
		uint8_t RequestType = arg["RequestType"].toInteger();
		uint8_t StartIndex = arg["StartIndex"].toInteger();
		APDU{NWKAddrOfInterest, 0x0001, IEEE_addr_req_frm(NWKAddrOfInterest, RequestType, StartIndex)}.send([&status](AFMessage& x)
		{
			LOG_MESSAGE("got it! %02x", x.asdu()[0]);
			//uint8_t* msg_body = x.asdu() + MSGBODY;
			//status = *msg_body;
		});

		rsp["status"] = status;
		rsp["NWKAddrOfInterest"] = IntToHexStr(NWKAddrOfInterest);
		C.response(rsp.stringify());
	});

	handler(ApplicationInterface::EventTag, selfptr()->endpointPtr(0)->uri() + "/active_ep_req", [](Context C) {
			JSON arg(C.parameter().c_str());
			JSON rsp{JSONType::JSON_OBJECT};
			int status = -1;
			//uint16_t NWKAddrOfInterest = arg["NWKAddrOfInterest"].toInteger();
			//uint8_t RequestType = arg["RequestType"].toInteger();
			//uint8_t StartIndex = arg["StartIndex"].toInteger();

			APDU{0x0000, 0x0005, active_ep_frm(0x0000)}.send([](AFMessage& x)
			{
				LOG_MESSAGE("got it!");
			});
			rsp["status"] = status;
			//rsp["NWKAddrOfInterest"] = IntToHexStr(NWKAddrOfInterest);
			C.response(rsp.stringify());
	});
	*/
	return;
}

using namespace zdo_private;

int zdo_private::zdo_callback(AFMessage * msg)
{
	switch (msg->clusterId())
	{
	case Device_annce:
		device_annce_proc(msg);
	break;
	case Match_Desc_req:
		match_Desc_req_proc(msg);
		break;
	default:
		{
		LOG_ERROR_MESSAGE("zdo fallback callback, cId: %04x, nwkAddr: %04x", msg->clusterId(), msg->srcAddr());

		if ((msg->clusterId() & 0x7fff) == 0x0036) {
			char apduString[128];
			size_t stringLen = kByteToHexString(apduString, msg->asdu(),msg->asduLen());
			LOG_MESSAGE("apdu = %s",apduString );
		}
		}
	break;
	}
	return 0;
}
void zdo_private::match_Desc_req_proc(AFMessage *msg)
{
	uint16_t NwkAddrOfInterest;
	size_t messageLength = msg->asdu()[4];
	LOG_MESSAGE("Match Desc Req, Nwk Addr: %04x", NwkAddrOfInterest);
	return;
}
void zdo_private::device_annce_proc(AFMessage *msg)
{
	LOG_MESSAGE("got annce");
	uint16_t NwkAddrOfInterest;
	uint8_t *pasdu = zdo_private::message_preprocess_(msg, 12,&NwkAddrOfInterest);

	if (pasdu == NULL)
	{
		return;
	}

	uint64_t ieee_addr;
	pasdu += kMemToInt((void*)&ieee_addr,(void*)pasdu, 8);
	uint8_t capability = *pasdu;

	Device *assoc = &(selfptr()->assocs(ieee_addr));

	assoc->id(NwkAddrOfInterest);
	assoc->ieee_id(ieee_addr);
	assoc->capability(capability);
	if (node_desc_req(assoc) < 0) {
		return;
	}
	LOG_MESSAGE("Got node desc");
	if (active_ep_req(assoc) < 0) {
		return;
	}
	LOG_MESSAGE("Got active endpoint");
	if (zdo_private::getAllEndpointSimpleDesc(assoc) < 0) {
		return;
	}
	assoc->active=1;
}

int node_desc_req(Device *pDevice)
{
	using namespace zdp_frame_pos;
	APDU{pDevice->id(), 0x0002, node_desc_frm(pDevice->id())}.send([pDevice](AFMessage& x)
	{
		uint8_t* msg_body = x.asdu() + MSGBODY;
		saveDevApdu(pDevice,"NodeDescApduString",msg_body, 13);
		node_desc_req_apdu_process(pDevice->node_descPtr(), msg_body);
	});
	return 0 ;
}

int active_ep_req(Device *pDevice)
{
	using namespace zdp_frame_pos;
	APDU{pDevice->id(), 0x0005, active_ep_frm(pDevice->id())}.send([pDevice](AFMessage& x)
	{
		uint8_t* msg_body = x.asdu() + MSGBODY;
		size_t ActiveEPCount = x.asdu()[4];
		saveDevApdu(pDevice,"ActiveEndpointApduString",msg_body, ActiveEPCount+1);
		active_ep_req_apdu_process(pDevice, msg_body);
	});
	return 0 ;
}

int simple_desc_req(Endpoint *pEndpoint)
{
	using namespace zdp_frame_pos;
	Device *pDevice = pEndpoint->parent();

	APDU{pDevice->id(), 0x0004, simple_desc_frm(pDevice->id(), pEndpoint->id())}.send([pEndpoint](AFMessage& x)
	{
		uint8_t* msg_body = x.asdu() + MSGBODY + 1;
		size_t SimpleDescLength = x.asdu()[4];
		saveEndpointApdu(pEndpoint,"SimpleDescApdu",msg_body, SimpleDescLength );
		SimpleDescriptor& desc = pEndpoint->simpleDescriptor();
		simple_desc_req_apdu_process(desc, msg_body);
		aps_newpeer_handle(*pEndpoint);
	});

	return 0 ;
}

int Mgmt_Permit_Joining_req(uint8_t PermitDuration, uint8_t TC_Significance, uint8_t& status)
{
	using namespace zdp_frame_pos;
	APDU{0, 0x0036, Mgmt_Permit_Joining_req_frm(PermitDuration,TC_Significance)}.send([&status](AFMessage& x)
	{
		status = x.asdu()[1];
	});

	return 0 ;
}

//int Mgmt_Nwk_Update_Req(uint32_t ScanChannels, uint8_t ScanDuration, uint8_t ScanCount, uint8_t nwkUpdateId, uint16_t nwkManagerAddr)
//{
//	using namespace zdp_frame_pos;
//
//	APDU{0, 0x0038, Mgmt_NWK_Update_req_frm(ScanChannels,ScanDuration, ScanCount,nwkUpdateId, nwkManagerAddr)}.send([&status](AFMessage& x)
//	{
//		status = x.asdu()[1];
//	});
//
//	return 0 ;
//}

Device *deviceQuery(uint16_t shortAddr)
{
	Device* assoc = &(thisDevice().assocs(shortAddr));

	uint64_t extAddr=getAssocExtAddr(shortAddr);
	assoc->id(shortAddr);
	assoc->ieee_id(extAddr);

	if (node_desc_req(assoc) < 0)
	{
		LOG_MESSAGE("node_desc_req failed: %04x", shortAddr);
		delete assoc;
		return NULL;
	}

	if (active_ep_req(assoc) < 0)
	{
		LOG_MESSAGE("active_ep_req failed: %04x", shortAddr);
		delete assoc;
		return NULL;
	}

	if ((zdo_private::getAllEndpointSimpleDesc(assoc)) < 0)
	{
		LOG_MESSAGE("getAllEndpointSimpleDesc failed: %04x", shortAddr);
		delete assoc;
		return NULL;
	}
	assoc->active = 1;

	LOG_MESSAGE("device query: %04x", shortAddr);
	return assoc;
}

int refreshDevice(uint16_t shortAddr)
{
	LOG_MESSAGE("start refreshing...");
	int rtn = -1;
	selfptr()->removeAssoc(shortAddr);
	Device *assoc = deviceQuery(shortAddr);
	rtn = 0;
	return rtn;
}


