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
#ifndef APS_OBJECT_MODELS_IMPLE_HPP_
#define	APS_OBJECT_MODELS_IMPLE_HPP_
#include "kjson.h"
using namespace kapi;
namespace aps {
namespace aps_label_
	{
		const char ASSOCS[] = "assocs";
		const char NWKADDR[] = "id";
		const char IEEEADDR[] = "ieee_id";
		const char CAPABILITY[] = "capability";
		const char ENDPOINTS[] = "endpoints";
		const char ENDPOINTID[] = "id";
		const char PAIRED[] = "paired";
		const char LOCALENDPOINT[] = "localEndpointOfContact";
		namespace SIMPLE_DESC
		{
			const char LABEL[] = "simpleDescriptor";
			const char PROFILEID[] = "profileId";
			const char DEVICDEID[] = "deviceId";
			const char DEVICEVERSION[] = "deviceVersion";
			const char ENDPOINT[] = "endpoint";
			const char INCLUSTERCOUNT[]	= "inClusterCount";
			const char INCLUSTERLIST[] 	= "inClusterList";
			const char OUTCLUSTERCOUNT[]= "outClusterCount";
			const char OUTCLUSTERLIST[] = "outClusterList";
		}
		const char CLUSTERS[] = "clusters";
		const char ATTRS[] = "attributes";
	}
JSON nodeSnapshot(const Attribute& a)
{
	JSON zom(kapi::JSONType::JSON_OBJECT);
	zom["id"] = IntToHexStr(a.id());
	zom["value"] = a.value().toString();
	return zom;
}

JSON nodeSnapshot(Cluster& c)
{
	JSON zom(kapi::JSONType::JSON_OBJECT);
	zom["id"] = IntToHexStr(c.id());
	zom[aps_label_::ATTRS] = JSON(kapi::JSONType::JSON_ARRAY);
	for (auto& x : c.attributes())
	{
		zom[aps_label_::ATTRS].newElement() = x.toJSON();
	}
	return zom;
}

JSON nodeSnapshot(Endpoint& e__)
{
	JSON zom(kapi::JSONType::JSON_OBJECT);

	SimpleDescriptor& simpleDesc = e__.simpleDescriptor();
	zom[aps_label_::ENDPOINTID] = IntToHexStr(e__.id());
	bool paired = (e__.localPeer() != nullptr);
	zom[aps_label_::PAIRED] = paired;
	if (paired) { zom[aps_label_::LOCALENDPOINT] = IntToHexStr(e__.localPeer()->id()); }
	if (e__.getWidgetRoot()) { zom["WidgetRoot"] =  e__.getWidgetRoot(); }
	if (e__.getWidgetIndexFile()) { zom["WidgetIndexFile"] = e__.getWidgetIndexFile(); }
	//
	// simple descriptor -> JSON format
	//
	zom[aps_label_::SIMPLE_DESC::LABEL] = { kapi::JSONType::JSON_OBJECT };
	JSON& simpleDescJSON = zom[aps_label_::SIMPLE_DESC::LABEL];
	simpleDescJSON[aps_label_::SIMPLE_DESC::ENDPOINT] = IntToHexStr(simpleDesc.endpointAddr);
	simpleDescJSON[aps_label_::SIMPLE_DESC::PROFILEID] = IntToHexStr(simpleDesc.profileId);
	simpleDescJSON[aps_label_::SIMPLE_DESC::DEVICDEID] = IntToHexStr(simpleDesc.deviceId);
	simpleDescJSON[aps_label_::SIMPLE_DESC::DEVICEVERSION] = IntToHexStr(simpleDesc.deviceVersion);
	simpleDescJSON[aps_label_::SIMPLE_DESC::INCLUSTERCOUNT] = IntToHexStr(simpleDesc.inclusterCount());
	simpleDescJSON[aps_label_::SIMPLE_DESC::INCLUSTERLIST] = JSON(kapi::JSONType::JSON_ARRAY);

	for (struct {JSON& json; size_t i;} v = {simpleDescJSON[aps_label_::SIMPLE_DESC::INCLUSTERLIST], 0}; v.i < simpleDesc.inclusterCount();	v.i++) {
		v.json.newElement() = IntToHexStr(simpleDesc.inclusterList[v.i]);
	}

	simpleDescJSON[aps_label_::SIMPLE_DESC::OUTCLUSTERCOUNT] = IntToHexStr(simpleDesc.inclusterCount());
	simpleDescJSON[aps_label_::SIMPLE_DESC::OUTCLUSTERLIST] = JSON(kapi::JsonType::JSON_ARRAY);
	for (struct {JSON& json; size_t i;} v = {simpleDescJSON[aps_label_::SIMPLE_DESC::OUTCLUSTERLIST], 0}; v.i < simpleDesc.outclusterCount(); v.i++) 		{
		v.json.newElement() = IntToHexStr(simpleDesc.outclusterList[v.i]);
	}

	zom["data"] = JSON(kapi::JSONType::JSON_OBJECT);

	for (auto& x : e__.data()) { zom["data"][x.first] = x.second; }
	zom[aps_label_::CLUSTERS] = JSON(kapi::JSONType::JSON_ARRAY);
	for (auto& x : e__.clusters()) 	{	zom[aps_label_::CLUSTERS].newElement() = nodeSnapshot(x);	}
	return zom;
}

JSON nodeSnapshot(Device& d__)
{
	JSON zom(kapi::JSONType::JSON_OBJECT);
	d__.shLockAcquire();
	zom[aps_label_::NWKADDR] = IntToHexStr(d__.id());
	zom[aps_label_::IEEEADDR] = IntToHexStr(d__.ieee_id());
	zom[aps_label_::CAPABILITY] = IntToHexStr(d__.capability());
	zom[aps_label_::ASSOCS] = JSON(kapi::JSONType::JSON_ARRAY);

	for (auto& x : d__.assocs()) { zom[aps_label_::ASSOCS].newElement()	= nodeSnapshot(x);	}

	zom[aps_label_::ENDPOINTS] = JSON(kapi::JsonType::JSON_ARRAY);

	for (auto& x : d__.endpoints()) { zom[aps_label_::ENDPOINTS].newElement() = nodeSnapshot(x);	}
	d__.shLockRelease();

	return zom;
}
	template <typename T, typename F, typename... ARGS>
	T& find_if(forward_list<T>& list, F pred, ARGS... Args)
	{
		for (T& x : list) {
			if (pred(x))
				return x;
		}
		return *list.emplace_after(list.before_begin(),Args...);
	}
	template <typename T, typename F>
	bool exist(forward_list<T>& list, F pred) {
		for (T& x: list) {
			if (pred(x)) return true;
		}
		return false;
	}

	Device::Device(uint16_t i)
	:capability_{},
	 nodeDescriptor_{},
	 assocs_{},
	 endpoints_{},
	 id_{i},
	 ieee_id_{},
	 active{0}
	 {}

	Device::~Device()
	{}

	void Device::id(uint16_t i) {id_= i;}
	uint16_t Device::id() const { return id_;}
	void Device::ieee_id(uint64_t i) {ieee_id_ = i;}
	uint64_t Device::ieee_id() const { return ieee_id_;}

	NodeDescriptor_t* Device::node_descPtr() {return &nodeDescriptor_;}
	NodeDescriptor_t& Device::nodeDescriptor() 	{return nodeDescriptor_;}
	uint8_t Device::capability() const { return capability_; }
	void Device::capability(uint8_t c) { capability_ = c; }
	Device& Device::assocs(uint16_t i) { return find_if(assocs_, [i](Device& x){ return (x.id() == i);},  i);}
	Device& Device::assocs(uint64_t i) { return find_if(assocs_, [i](Device& x){ return (x.ieee_id() == i);},  i); }
	bool Device::assocExist(uint16_t i) { return exist(assocs_, [i](Device& x){ return (x.id() == i);});}
	bool Device::assocExist(uint64_t i) { return exist(assocs_, [i](Device& x){ return (x.ieee_id() == i);}); }
	Devices& Device::assocs() {	return assocs_;}

	Endpoint* Device::endpointPtr(uint8_t i) {	return  &find_if(endpoints_, [i](Endpoint& x){ return (x.id() == i);},  i, this);}
	Endpoint& Device::newEndpoint(uint8_t i) {
		if (endpointExist(i)) {
			for (uint8_t j = 1; j < 240 ; j++) {
				if (!endpointExist(j)) return endpoints(j);
			}
			return endpoints(241);
		} else {
			return endpoints(i);
		}
	}

	Endpoint& 	Device::endpoints(uint8_t i) { return find_if(endpoints(), [i](Endpoint& x){ return (x.id() == i);}, i, this); }
	void Device::removeAssoc(uint16_t i) {	assocs_.remove_if([i](Device& x){return x.id()==i;}); }
	bool Device::endpointExist(uint8_t i){ return exist(endpoints_, [i](Endpoint& x){ return (x.id() == i);});}
	Endpoints& Device::endpoints() { return endpoints_;}
	JSON Device::toJSON() { return aps::nodeSnapshot(*this); }
	string Device::uri() { return (std::string("devices/")+IntToHexStr(id()));};

	// Device definitions
	Endpoint::Endpoint(uint8_t i):Endpoint(i,nullptr)
	{}
	Endpoint::Endpoint(uint8_t i, Device* p)
	:simpleDescriptor_{},
	 messageHandler{0},
	 localPeer_(nullptr),
	 id_{i},
	 parent_{p},
	 data_{}
	{
		setClusterLookupPath(JSON_CLUSTERS_FILE);
		setDatatypeLookupPath(JSON_ZCL_DATATYPES);
	}
	Endpoint::~Endpoint()
	{}

	uint8_t Endpoint::id() const { return id_;}
	void Endpoint::id(uint8_t i) { id_=i;}
	Device* Endpoint::parent() { return parent_;}
	void Endpoint::parent(Device* p) { parent_ = p;}
	Cluster* Endpoint::clusterPtr(uint16_t i) {	return &aps::find_if(clusters_, [i](Cluster& x){ return (x.id() == i);}, i, this);}
	Cluster& Endpoint::clusters(uint16_t i) { return aps::find_if(clusters_, [i](Cluster& x){ return (x.id() == i);}, i, this);}
	Clusters& Endpoint::clusters() { return clusters_;}
	Endpoint* 	Endpoint::localPeer() { return localPeer_;}
	void Endpoint::localPeer(Endpoint* e) {	localPeer_ = e;}

	SimpleDescriptor& Endpoint::simpleDescriptor(){	return simpleDescriptor_;}

	void Endpoint::setClusterLookupPath(const char* filePath) { data()["ClusterLookupPath"] = filePath; }
	const char* Endpoint::getClusterLookupPath() { return const_cast<char*>(data()["ClusterLookupPath"].c_str());}
	void Endpoint::setDatatypeLookupPath(const char* filePath) { data()["datatypeLookupPath"] = filePath; }
	const char* Endpoint::getDatatypeLookupPath() { return const_cast<char*>(data()["datatypeLookupPath"].c_str());}
	void Endpoint::setWidgetLookupPath(const char* filePath) {	data()["widgetLookupPath"] = filePath;}
	const char*	Endpoint::getWidgetLookupPath()	{ return const_cast<char*>(data()["widgetLookupPath"].c_str());}
	void Endpoint::setWidgetRoot(const char* filePath) { data()["WidgetRoot"] = filePath;}
	const char* Endpoint::getWidgetRoot() { return const_cast<char*>(data()["WidgetRoot"].c_str());}
	void Endpoint::setWidgetIndexFile(const char* filePath) {	data()["WidgetIndexFile"] = filePath;}
	const char* Endpoint::getWidgetIndexFile() { return const_cast<char*>(data()["WidgetIndexFile"].c_str());}

	JSON Endpoint::toJSON() { return aps::nodeSnapshot(*this);}
	std::string Endpoint::uri()
	{
		std:: string __uri;
		if (parent()) __uri = parent()->uri();
		__uri +=std::string("/endpoints/") + IntToHexStr(id());
		return __uri;
	}

	Endpoint::Data& Endpoint::data() {return data_;}

	/* Cluster */

	Cluster::Cluster(uint16_t i) : Cluster{i,nullptr}
	{}

	Cluster::Cluster(uint16_t i,Endpoint* p)
	:id_{i},
	 parent_{p},
	 attributes_{}
	{}
	Cluster::~Cluster()
	{}

	uint16_t Cluster::id() const { return id_;}
	void Cluster::id(uint16_t i) {id_ = i;}
	Endpoint* Cluster::parent() { return parent_;}
	void Cluster::parent(Endpoint* p) {parent_ = p;}

	Attribute& Cluster::attributes(uint16_t i) { return  aps::find_if(attributes_, [i](Attribute& x){ return (x.id() == i);}, i);}
	Attributes& Cluster::attributes() { return attributes_;}
	JSON Cluster::toJSON() { return aps::nodeSnapshot(*this);}

	AttributeValueDefault::AttributeValueDefault(uint8_t typeId, size_t size)
	:data_{new uint8_t[size]}, size_{size}, typeId_{typeId} {}
	AttributeValueDefault::~AttributeValueDefault() {}

	string AttributeValueDefault::toString() const {
		char buf[size_ << 1 + 1];
		char* buf_ = buf;
		for (int i=size_-1;i>=0;i--)
			buf_ += sprintf(buf_, "%02x", data_[i]);
		return string(buf);
	}

	int AttributeValueDefault::setValue(const uint8_t* d)
	{
		memcpy(data_.get(), d, size_);
		return size_;
	}

	Attribute::Attribute(uint16_t i):Attribute(i,new AttributeValueDefault(0,1))
	{}

	Attribute::Attribute(uint16_t i, AttributeValue* p)
	:id_{i},
	 value_{p}
	{}

	Attribute::~Attribute()	{}
	uint16_t Attribute::id() const {return id_;}
	void Attribute::id(uint16_t i) {id_ = i;}
	AttributeValue& Attribute::value() const { return *value_;}
	void Attribute::value(AttributeValue* v){ value_.reset(v); }
	JSON Attribute::toJSON()  {return aps::nodeSnapshot(*this);}
}// namespace aps
#endif
