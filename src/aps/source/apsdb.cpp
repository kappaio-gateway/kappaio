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
#include "apsdb.h"
#include <atomic>
#include <errno.h>
#include <forward_list>
#include <future>
#include "hal.h"
#include <iterator>
#include "kglobal.h"
#include "kjson.h"
#include "kutil.h"
//#include <mutex>
#include "notification.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <thread>
#include <tuple>
#include <syslog.h>
#include "restful.h"
#include "aps_data_service_impl.hpp"
#include "aps_data_service_ext_impl.hpp"
#include "aps_object_models_impl.hpp"

namespace
{

} //namespace apsdbPrivate
namespace {
	static aps::Device* self_cpp = nullptr;
	void apsdbinit()
	{
		aps::data_service::init();
		aps::data_service_EXT::init();
		return;
	}
	void SelfInit(void)
	{
		using namespace aps;
		self_cpp = new Device{0};
		selfptr()->ieee_id(get_ieee_id());

		//===== 09072014 ====
	//	Device *x = selfptr();
		using namespace ApplicationInterface;
		using namespace kapi;
		using namespace kapi::notify;
		using JSON = kapi::JSON;

		handler(EventTag,  thisDevice().uri(),	[](Context C)
		{
			C.response(thisDevice().toJSON().stringify());
		});

		handler(EventTag,  thisDevice().uri() + "/endpoints", [](Context C)
		{
			JSON ids(JSONType::JSON_ARRAY);
			for (auto& y : thisDevice().endpoints())
			{
				ids.newElement() = IntToHexStr(y.id());
			}
			C.response(ids.stringify());
		});

		handler(EventTag, "devices", [](Context C)
		{
			JSON ids(JSONType::JSON_ARRAY);
			ids.newElement() = IntToHexStr(selfptr()->id());
			for (auto& y : thisDevice().assocs())
			{
				ids.newElement() = IntToHexStr(y.id());
			}
			C.response(ids.stringify());
		});

		handler(EventTag, "thisdevice", [](Context C)
		{
			C.response(thisDevice().toJSON().stringify());
		});
		//bindingTable = new bindingTable_c();
		return;
	}
}
namespace aps {

	void init()
	{
		SelfInit();
		apsdbinit();
	}

	uint8_t cntr()
	{
		static atomic<uint8_t> cntr_;
		return cntr_++;
	}

	int dataRequest(AFMessage& msg)
	{
		sendAPSMessageStruct(&msg);
		return 0;
	}

} // namespace aps


int establish_profile(aps::Endpoint& e_)
{
	using namespace aps;
	if (e_.localPeer() == nullptr)
		return -1;

	JSON clusLib{e_.localPeer()->getClusterLookupPath()};
	JSON dtLib  {e_.localPeer()->getDatatypeLookupPath()};

	for (auto cId : e_.simpleDescriptor().inclusterList)
	{
		if (!clusLib.exist( IntToHexStr( cId )) )
			continue;

		Cluster& cluster = e_.clusters( cId );
		JSON& attrList = clusLib[ IntToHexStr( cId ) ][2];

		attrList.forEachInArr([&cluster, &dtLib](size_t index, JSON& el) {
			if (!dtLib.exist(el[2].toString())) {
				LOG_ERROR_MESSAGE("unknown attr val type");
				return;
			}

			JSON& dataType 	= dtLib[el[2].toString()];
			Attribute& attr = cluster.attributes( el[(size_t)0].toInteger()  );
			attr.value(new AttributeValueDefault(dataType[(size_t)0].toInteger(), dataType[1].toInteger()));
		});
	}
	return 0;
}

int aps_newpeer_handle(aps::Endpoint& remote_endpoint)
{
	using namespace ApplicationInterface;
	using namespace kapi;
	using namespace aps;
	int rtn = -1;

	for (auto& local_ : thisDevice().endpoints())
	{
		if (remote_endpoint.simpleDescriptor().profileId == local_.simpleDescriptor().profileId)
		{
			remote_endpoint.localPeer(&local_);
			for (auto local_cId : local_.simpleDescriptor().outclusterList)
			{
				for (auto remote_cId : remote_endpoint.simpleDescriptor().inclusterList) {
					/* if any match is found, create a binding entry */
					if (local_cId == remote_cId) {
						//remote_endpoint.data()["localEndpointOfContact"] = IntToHexStr(local_.id());
						uint64_t ieee_id = remote_endpoint.parent()->ieee_id();
						//==========================================================================================
						//******************************************************************************************
						// Zigbee Binding tables, NOT USED for now, maybe useful later??
						//******************************************************************************************
						//						getBindingTable()->bindRequest(&(local_.parent()->ieee_id()), 		/* source IEEE address 	*/
						//								local_.id(),  												/* source endpoint		*/
						//								local_SimpleDesc.outclusterList()[i], 									/* source out cluster */
						//								0x03, 																/* address mode */
						//								(uint8_t*)&ieee_id, 												/* destination address */
						//								remote_endpoint.id()); 											/* destination endpoint */
						//==========================================================================================
						rtn = 0;
			}}}

			if (rtn==0) {
				/* moved from user space 06-06-2014 */
				establish_profile(remote_endpoint);

				////////////////////////////////

				/* added 09-07-2014 */
				// A default URI handler for the newly joined endpoint which
				// prints out its data structure
				Device* x = remote_endpoint.parent();
				notify::handler("Application",  x->uri(),
				[x](Context C)
				{
					C.response(x->toJSON().stringify());
				});

				notify::handler("Application", x->uri() +"/endpoints",
						[x](Context C) {
							JSON ids(kapi::JSONType::JSON_ARRAY);
							for (auto& y : x->endpoints())
							{
								ids.newElement() = IntToHexStr(y.id());
							}
							C.response(ids.stringify());
				});

				/* added peer list 07-20-2014 */
				// local_endpoint->handleNewpeer(*remote_endpoint);
				/* added peer list 07-20-2014 */

				/* 09-19-2014 */
				// triggers an event signal to the local endpoint
				// that is registered to handle the this profieId
				notify::trigger("NewJoin", local_.uri(), remote_endpoint);
				/* 09-19-2014 */

				break;
			}
		}
	}
	return rtn;
} // int aps_newpeer_handle(Endpoint* remote_endpoint)


// =================== Device definitions ===================
	aps::Device* selfptr() {return self_cpp;}
	aps::Device& thisDevice() {return *selfptr();}


//
//#define TOJSONHEXSTR(x,y,z) json_string(kToHexStr((char*)x, (void*)&y, z))
//#define TOHEXSTR(x,y,z) kToHexStr(x, (void*)&y,z)
//
///* This implementation will be phased out */
//json_t *nodeList_to_jsonArray(json_t *parent, node_c *first_node,const char *label, char type, json_t *nodeProcessor(node_c*,char))
//{
//	json_t *list = json_array();
//	node_c *node = first_node;
//	json_object_set_new(parent, label, list);
//
//	while (node)
//	{
//		json_array_append_new(list, nodeProcessor(node, type));
//		node = node->next();
//	}
//	return list;
//}
//json_t *nodeList_to_jsonArray(json_t *parent, sandbox1::NodeList &nodeList,const char *label, char type, json_t *nodeProcessor(node_c*,char))
//{
//	json_t *list = json_array();
//	json_object_set_new(parent, label, list);
//	for (sandbox1::NodeList::iterator it = nodeList.begin(); it != nodeList.end();++it)
//		json_array_append_new(list, nodeProcessor(*it, type));
//
//	return list;
//}
//int dataRequest(uint8_t dstAddrMode,
//								uint64_t dstAddr,
//								uint8_t dstEndpoint,
//								uint16_t profileId,
//								uint16_t clusterId,
//								uint8_t srcEndpoint,
//								size_t asduLength,
//								uint8_t *asdu,
//								uint8_t txOption,
//								size_t radius)
//{
//	bindingEntry_c *entry=bindingTable->findSourceEntry(selfptr()->ieee_id(),srcEndpoint, clusterId);
//
//	if ( entry == NULL && dstEndpoint!=0 && srcEndpoint !=0 ) { return -1; }
//
//	if ( dstAddrMode == 0x00 )
//	{
//		bindingEntry_c *dst = entry->destination();
//		while (dst)
//		{
//			if (*(dst->addrModePtr())==0x03)
//			{
//				sendAPSMessage(selfptr()->assocs(*(uint64_t*)(dst->idPtr())).id(),
//											*(dst->endpointIdPtr()),
//											clusterId,
//											srcEndpoint,
//											GetAPSCounter(),
//											txOption,
//											(uint8_t)radius,
//											asdu,
//											(uint8_t)asduLength);
//			}
//			dst = dst->next();
//		}
//	}
//
//	//16-bit address for dstAddr and dstEndpoint present
//	if (dstAddrMode==0x02)
//	{
//		sendAPSMessage(static_cast<uint16_t>(dstAddr), dstEndpoint, clusterId, srcEndpoint, GetAPSCounter(),txOption,(uint8_t)radius,asdu, (uint8_t)asduLength);
//	}
//	//64-bit extended address for dstAddr and dstEndpoint present
//	if (dstAddrMode==0x03)
//	{
//		sendAPSMessage(selfptr()->assocs(dstAddr).id(), dstEndpoint, clusterId, srcEndpoint,GetAPSCounter(),txOption,(uint8_t)radius,asdu, (uint8_t)asduLength);
//	}
//
//	return 0;
//}

//
//bindingEntry_c::bindingEntry_c(uint64_t srcAddr, uint8_t srcEndpoint, uint16_t clusterId,
//		uint8_t dstAddrMode, uint8_t *dstAddr, uint8_t dstEndpoint)
//// 8 bytes ieeeAddr, 2 bytes clusterId, 1 byte endpoint
//:node_c(12)
//{
//	//-----------------------------------------------------------------
//	//             1                 					8               1               2
//	//-----------------------------------------------------------------
//	// addMode=0xf3(for source, not in zigbee spec) |   ieeeId   |  endpointId   |   clusterId
//	//-----------------------------------------------------------------
//	uint8_t addMode = 0xf3;
//	this->_id_((void*)(&addMode),0,1,'w');
//	this->_id_((void*)(&srcAddr),1,8,'w');
//	this->_id_((void*)(&srcEndpoint),9,1,'w');
//	this->_id_((void*)(&clusterId),10,2,'w');
//	this->append_destination(dstAddrMode, dstAddr, dstEndpoint);
//}
//
//bindingEntry_c *bindingEntry_c::destination()
//{
//	return static_cast<bindingEntry_c*>(child());
//}
//
//bindingEntry_c *bindingEntry_c::next()
//{
//	return static_cast<bindingEntry_c*>(node_c::next());
//}
//
//int bindingEntry_c::append_destination(uint8_t dstAddrMode, uint8_t *dstAddr, uint8_t dstEndpoint)
//{
//	int rtn=-1;
//	if (dstAddrMode==0x03)
//	{
//		//1 byte dstAddrMode, 8 bytes ieeeAddr, 1 byte endpoint,
//		bindingEntry_c *dst = (bindingEntry_c*)(new node_c(10));
//		dst->_id_((void*)(&dstAddrMode),0,1,'w');
//		dst->_id_((void*)(dstAddr),1,8,'w');
//		dst->_id_((void*)(&dstEndpoint),9,1,'w');
//		node_c::append_node(&(this->childList),dst);
//		rtn = 0;
//	}
//
//	if (dstAddrMode==0x01)
//	{
//		//1 byte dstAddrMode, 2 bytes groupAddr,
//		node_c *dst = new node_c(3);
//		dst->_id_((void*)(&dstAddrMode),0,1,'w');
//		dst->_id_((void*)(dstAddr),1,2,'w');
//		node_c::append_node(&(this->childList),dst);
//		rtn =0;
//	}
//	return rtn;
//}
//
//uint8_t *bindingEntry_c::idPtr()
//{
//	return (this->_id+1);
//}
//
//uint8_t *bindingEntry_c::endpointIdPtr()
//{
//	if (*(this->addrModePtr())==0xf3 || *(this->addrModePtr())==0x03)
//	{
//		return (this->_id+9);
//	}
//	return NULL;
//}
//
//uint16_t *bindingEntry_c::clusterIdPtr()
//{
//	if (*(this->addrModePtr())==0xf3)
//	{
//		return (uint16_t*)(this->_id + 10);
//	}
//	return NULL;
//}
//
//uint8_t *bindingEntry_c::addrModePtr()
//{
//	return (this->_id);
//}
//
//bindingTable_c::bindingTable_c()
//:bindingEntryList({NULL})
//{}
//
//bindingEntry_c *bindingTable_c::findSourceEntry(uint64_t srcAddr, uint8_t srcEndpoint, uint16_t clusterId)
//{
//	uint8_t entryId[11];
//	memcpy((void*)entryId, (void*)&srcAddr,8);
//	memcpy((void*)(entryId+8), (void*)&srcEndpoint,1);
//	memcpy((void*)(entryId+9), (void*)&clusterId,2);
//	if (this->bindingEntryList.firstNode)
//	{
//		return (bindingEntry_c*)this->bindingEntryList.firstNode->find_mem_match(&(this->bindingEntryList), 1,11,entryId);
//	} else
//	{
//		return NULL;
//	}
//}
//
//bindingEntry_c *bindingTable_c::firstEntry()
//{
//	return (bindingEntry_c *)this->bindingEntryList.firstNode;
//}
//
//int bindingTable_c::removeDevice(uint16_t shortAddr)
//{
//	// get the first entry in the list
//	bindingEntry_c *srcIter = this->firstEntry();
//	bindingEntry_c *dstIter, *dstNext, *srcNext;
//	uint16_t ieeeAddr;
//
//	if (selfptr()->id() == shortAddr)
//	{
//		ieeeAddr = selfptr()->ieee_id();
//	}
//	else {
//		ieeeAddr = selfptr()->assocs(shortAddr).ieee_id();
//	}
//
//	this->lockAcquire();
//
//	while (srcIter)
//	{
//		srcNext = srcIter->next();
//		if (*(uint64_t*)(srcIter->idPtr()) == ieeeAddr)
//		{
//			srcIter->unlinkNode(&this->bindingEntryList, srcIter);
//			delete srcIter;
//			srcIter = srcIter->next();
//			continue;
//		}
//		dstIter = (bindingEntry_c*)srcIter->child();
//		while(dstIter)
//		{
//			LOG_VAR(*(uint64_t*)(dstIter->idPtr()), %016llx);
//			dstNext = dstIter->next();
//			if (*(uint16_t*)(dstIter->idPtr()) == ieeeAddr)
//			{
//				srcIter->unlinkChild((node_c*)dstIter);
//				delete dstIter;
//			}
//			dstIter = dstNext;
//		}
//		srcIter = srcIter->next();
//	}
//	this->lockRelease();
//	return 0;
//}
//
//int bindingTable_c::bindRequest(uint64_t srcAddr, uint8_t srcEndpoint, uint16_t clusterId, uint8_t dstAddrMode, uint8_t *dstAddr, uint8_t dstEndpoint)
//{
//	bindingEntry_c *entry=this->findSourceEntry(srcAddr, srcEndpoint, clusterId);
//
//	int rtn = -1;
//
//	this->lockAcquire();
//
//	if (entry)
//	{
//		rtn = entry->append_destination(dstAddrMode, dstAddr, dstEndpoint);
//	}
//	else {
//		entry = new bindingEntry_c(srcAddr, srcEndpoint, clusterId,dstAddrMode, dstAddr, dstEndpoint);
//		entry->append_node(&(this->bindingEntryList),entry);
//		rtn =  0;
//	}
//
//	this->lockRelease();
//
//	return rtn;
//}
//
////outside interface
//bindingTable_c* getBindingTable(){return bindingTable;}

	//	class syncTask_c
	//	{
	//		public:
	//		syncTask_c();
	//		void *(*func)(void*);
	//		void *data;
	//		syncTask_c *next;
	//		syncTask_c *prev;
	//	};
	//
	//	syncTask_c::syncTask_c() : func(NULL), data(NULL), next(NULL), prev(NULL) {}
	//
	//	struct syncTaskList_s
	//	{
	//		syncTask_c *task;
	//		sem_t *sem;
	//		int state;
	//	};
	//	struct syncTaskList_s syncTaskList = {NULL, NULL, -1};
	//
	//	void apsSyncInit()
	//	{
	//		if (syncTaskList.state == -1)
	//		{
	//			syncTaskList.state = 0;
	//			syncTaskList.sem = (sem_t *)malloc(sizeof(sem_t));
	//			syncTaskList.task = new syncTask_c();
	//			sem_init(syncTaskList.sem, 0 ,1);
	//		}
	//		return;
	//	}

	//int apsRegsiterSyncTask(void *(*func)(void*), void *data)
	//{
	//	if (apsdbPrivate::syncTaskList.state < 0)
	//	{
	//		LOG_ERROR_MESSAGE("APS sync task has to be initialized");
	//		return -1;
	//	}
	//
	//	apsdbPrivate::syncTask_c *task = new apsdbPrivate::syncTask_c();
	//	task->func = func;
	//	task->data = data;
	//
	//	apsdbPrivate::syncTask_c *iter = apsdbPrivate::syncTaskList.task;
	//
	//	sem_wait(apsdbPrivate::syncTaskList.sem);
	//	while (iter->next)
	//	{
	//		iter = iter->next;
	//	}
	//
	//	iter->next = task;
	//	task->prev = iter;
	//	sem_post(apsdbPrivate::syncTaskList.sem);
	//
	//	return 0;
	//}

	//int apsSync()
	//{
	//	if (apsdbPrivate::syncTaskList.state <0)
	//	{
	//		return -1;
	//	}
	//
	//	apsdbPrivate::syncTask_c *iter = apsdbPrivate::syncTaskList.task->next;
	//	sem_wait(apsdbPrivate::syncTaskList.sem);
	//	while (iter)
	//	{
	//		iter->func(iter->data);
	//		iter = iter->next;
	//	}
	//	sem_post(apsdbPrivate::syncTaskList.sem);
	//	return 0;
	//}


	//static bindingTable_c* bindingTable;
