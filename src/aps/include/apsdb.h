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
#ifndef APSDB_H
#define APSDB_H

#include <vector>
#include <iterator>
#include <map>
#include <functional>
#include <forward_list>
#include <memory>
#include "hal.h"
#include "kglobal.h"
#include "kjson.h"
#include "aps_afmessage_impl.h"

#define JSON_ZCLPROFILES   "/usr/lib/rsserial/zclprofiles.json"
#define JSON_CLUSTERS_FILE "/usr/lib/rsserial/zclclusters.json"
#define JSON_ZCL_DATATYPES "/usr/lib/rsserial/zcldatatypes.json"
#define JSON_WIDGET_LIB	   "/usr/lib/rsserial/widget.json"

namespace aps
{
	struct NodeDescriptor_t
	{
		uint8_t LogicalType:3;
		uint8_t ComplexDescAvail:1;  /* AF_V1_SUPPORT - reserved bit. */
		uint8_t UserDescAvail:1;     /* AF_V1_SUPPORT - reserved bit. */
		uint8_t Reserved:3;
		uint8_t APSFlags:3;
		uint8_t FrequencyBand:5;
		uint8_t CapabilityFlags;
		uint16_t ManufacturerCode;
		uint8_t MaxBufferSize;
		uint16_t MaxInTransferSize;
		uint16_t ServerMask;
		uint16_t MaxOutTransferSize;
		uint8_t DescriptorCapability;
	} ;

	struct SimpleDescriptor {
		using ClusterList = std::vector<uint16_t>;
		ClusterList outclusterList;
		ClusterList inclusterList;
		uint8_t	inclusterCount() {return inclusterList.size();}
		uint8_t outclusterCount() { return outclusterList.size();}
		void inclusterCount(uint8_t s) { inclusterList.resize(s);	inclusterList.shrink_to_fit();}
		void outclusterCount(uint8_t s) { outclusterList.resize(s);	outclusterList.shrink_to_fit();}
		uint8_t endpointAddr;
		uint16_t profileId;
		uint16_t deviceId;
		uint8_t deviceVersion;
	};

	class Device;
	class Endpoint;
	class Cluster;
	class Attribute;
	using string = std::string;
	using Devices = std::forward_list<Device>;
	using Endpoints = std::forward_list<Endpoint>;
	using Clusters 	= std::forward_list<Cluster>;
	using Attributes= std::forward_list<Attribute>;
	using JSON = kapi::JSON;

	class Device : public klock_c
	{
	private:
		uint8_t capability_;
		NodeDescriptor_t nodeDescriptor_;
		Devices assocs_;
		Endpoints endpoints_;
		uint16_t id_;
		uint64_t ieee_id_;
	public:
		Device(uint16_t);
		~Device();
		void id(uint16_t);
		uint16_t id() const;
		void ieee_id(uint64_t);
		uint64_t ieee_id() const;
		NodeDescriptor_t* node_descPtr();
		NodeDescriptor_t& nodeDescriptor();
		uint8_t capability() const;
		void capability(uint8_t);
		Device&	assocs(uint16_t);
		Device&	assocs(uint64_t);
		bool assocExist(uint16_t);
		bool assocExist(uint64_t);
		Devices& assocs();
		void removeAssoc(uint16_t);
		Endpoint* endpointPtr(uint8_t);
		Endpoint& newEndpoint(uint8_t);
		Endpoint& endpoints(uint8_t);
		Endpoints& endpoints();
		bool endpointExist(uint8_t);
		string uri();
		kapi::JSON toJSON();
		int active;
	};

	class Endpoint
	{
	public:
		using Data = std::map<string,string> ;
	private:
		SimpleDescriptor simpleDescriptor_;
		Data data_;
		Device* parent_;
		Endpoint* localPeer_;
		Clusters clusters_;
		uint8_t id_;
	public:
		Endpoint(uint8_t);
		Endpoint(uint8_t, Device*);
		~Endpoint();
		uint8_t id() const;
		void id(uint8_t);
		Device* parent();
		void parent(Device*);
		Endpoint* localPeer();
		void localPeer(Endpoint*);
		Cluster* clusterPtr(uint16_t);
		Cluster& clusters(uint16_t);
		Clusters& clusters();
		SimpleDescriptor& simpleDescriptor();
		int (*messageHandler)(AFMessage*);
		int (*rpcHandler)(void *_data){};
		kapi::JSON toJSON();
		string uri();
		void setClusterLookupPath(const char* filePath);
		void setDatatypeLookupPath(const char* filePath);
		void setWidgetLookupPath(const char* filePath);
		void setWidgetRoot(const char* filePath);
		void setWidgetIndexFile(const char* filePath);
		const char* getClusterLookupPath();
		const char* getDatatypeLookupPath();
		const char* getWidgetLookupPath();
		const char* getWidgetRoot();
		const char* getWidgetIndexFile();
		Data& data();
	};

	class Cluster
	{
	private:
		uint16_t id_;
		Endpoint* parent_;
		Attributes attributes_;
	public:
		Cluster(uint16_t);
		Cluster(uint16_t,Endpoint*);
		~Cluster();
		uint16_t id() const;
		void id(uint16_t);
		Endpoint* parent();
		void parent(Endpoint*);
		Attribute& attributes(uint16_t);
		Attributes& attributes();
		kapi::JSON toJSON() ;
	};

	class AttributeValue
	{
	public:
		virtual string toString() const = 0;
		virtual int setValue(const uint8_t*) = 0;
		virtual ~AttributeValue() {};
	};

	class AttributeValueDefault: public AttributeValue
	{
	private:
		std::unique_ptr<uint8_t[]> data_;
		size_t size_;
		uint8_t	typeId_;
	public:
		AttributeValueDefault(uint8_t typeId, size_t size);
		~AttributeValueDefault() override;
		string toString() const override;
		int setValue(const uint8_t*) override;
	};

	class Attribute
	{
	private:
		uint16_t id_;
		std::unique_ptr<AttributeValue> value_;
	public:
		Attribute(uint16_t);
		Attribute(uint16_t, AttributeValue*);
		~Attribute();
		uint16_t id() const;
		void id(uint16_t);
		AttributeValue& value() const;
		void value(AttributeValue*);
		kapi::JSON toJSON() ;
	};
} // namespace aps


aps::Device* selfptr();
aps::Device& thisDevice();
int aps_newpeer_handle(aps::Endpoint& remote_endpoint);

namespace aps
{
	void init();
	uint8_t cntr();
	namespace data_service
	{
		using PredWrapper =	std::function<bool(AFMessage&)>;
		using CbWrapper = std::function<void(AFMessage&)>;
		void request(const AFMessage& msg);
		void request(AFMessage& msg, PredWrapper&& pred, CbWrapper&& cb, int64_t wait_time_us = 1000000);
	}
	namespace data_service_EXT
	{
		using PredWrapper_EXT =	std::function<bool(AFMessage_EXT&)>;
		using CbWrapper_EXT = std::function<void(AFMessage_EXT&)>;

		class TxHandle
		{
		private:
			struct Data;
			Data* data;
		public:
			TxHandle(PredWrapper_EXT&& pred,CbWrapper_EXT&& cb);
			void arm();
			bool wait_for(int64_t micro_sec);
			void remove();
			~TxHandle();
		};
		void request(const AFMessage_EXT& msg);
		void request(AFMessage_EXT& msg, PredWrapper_EXT&& pred, CbWrapper_EXT&& cb, int64_t wait_time_us = 1000000);
		TxHandle registerHandlers(PredWrapper_EXT&& pred, CbWrapper_EXT&& cb);
		void removeHandler(TxHandle& h);
	}

} //namespace aps
inline uint8_t GetAPSCounter() {return aps::cntr();}
inline uint8_t GetZDPSeqNum() { return aps::cntr();}
#endif

