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
#include "restful.h"
#include "kjson.h"
#include "notification.h"
#include "kglobal.h"
#include <vector>
#include <map>
namespace ApplicationInterface
{
	using namespace kapi;
	using namespace std;
	std::vector<__Context> __Contexts;
	__Context::__Context(std::string uri):uri_(uri)	{}
	__Context::~__Context(){}
	std::string& __Context::URI() {return uri_;}
	std::string& __Context::parameter() {return parameter_;}
	void __Context::parameter(string P) {parameter_ = P;}
	void __Context::response(string&& R) {response_ = R;}
	void __Context::response(const string& R) {response_ = R;}
	string& __Context::response() {return response_;}
	void __Context::request(const string& S) { request_ = S;}
	string& __Context::request() {return request_;}

	Context getContext(const std::string& uri)
	{
		for (Context x : __Contexts) {if (x.URI() ==  uri) return x;}
		__Contexts.emplace_back(uri);
		return getContext(uri);
	}

	void handleRESTful(const string& req, std::string& rsp) {
		JSON req_(req);
		JSON& path  = req_["path"];
		JSON& param = req_["parameter"];
		std::string uri;

		path.forEachInArr([&](size_t index, JSON& json){uri += json.toString() + "/";	});

		uri.pop_back();
		Context c = getContext(uri);
		c.parameter(param.stringify());
		c.request(req);
		kapi::notify::trigger("Application", uri,  c);
		rsp = c.response();

		//LOG_MESSAGE("req : %s", req_.stringify().c_str());

	}
//	void handleRESTful(string&& req, std::string& rsp)
//	{
//
//	}
//	void handleRESTfulArch(const char* req, std::string& rsp) {
//		enum RESTfulNodes {
//			root,   		/* 0 */
//			han,			/* 1 */
//			devices,		/* 2 */
//			thisdevice,		/* 3 */
//			device,			/* 4 */
//			endpoints,		/* 5 */
//			endpoint,		/* 6 */
//			widget,			/* 7 */
//			clusters,		/* 8 */
//			attributes,
//			endparsing,
//			unknown
//		};
//		const char hanTag[] 		= "han";
//		const char thisdeviceTag[]	= "thisdevice";
//		const char devicesTag[] 	= "devices";
//		const char endpointsTag[] 	= "endpoints";
//		const char widgetTag[] 		= "widget";
//		const char * const rootTagGroup[] 		= {hanTag};
//		const RESTfulNodes rootenumTagGroup[] 	= {han};
//		const char * const hanTagGroup[] = {thisdeviceTag,devicesTag};
//		const RESTfulNodes hanenumTagGroup[] 	= {device, devices};
//		const char * const deviceTagGroup[]     = {endpointsTag};
//		const RESTfulNodes deviceenumTagGroup[] 	= {endpoints};
//
//		uint16_t nwkaddr=0, clusterId;
//		uint8_t endpointId;
//
//		struct Addrs {
//			uint16_t nwkaddr;
//			uint16_t clusterId;
//			uint8_t endpointId;
//		};
//		Addrs context;
//		struct StateMap {
//			StateMap(const char ** const tagGroup, const RESTfulNodes* tagenumGroup, size_t size, Addrs& context)
//			:tagGroup_(tagGroup),
//			 tagenumGroup_(tagenumGroup),
//			 size_(size),
//			 context_(context)
//			 {};
//			char** tagGroup_;
//			RESTfulNodes* tagenumGroup_;
//			size_t size_;
//			Addrs& context_;
//			RESTfulNodes defaultNode = unknown;
//			RESTfulNodes operator()(std::string& in) {
//				for (size_t i=0;i<size_;i++) { if (in == tagGroup_[i]) return tagenumGroup_[i];	}
//				return defaultNode;
//			};
//		};
//
//		StateMap rootBeer(rootTagGroup,rootenumTagGroup, 1, context);
//		StateMap hanBeer(hanTagGroup, hanenumTagGroup,2, context);
//		StateMap deviceBeer(deviceTagGroup, deviceenumTagGroup,1, context);
//		JSON req_(req);
//		JSON& path  = req_["path"];
//		JSON& param = req_["parameter"];
//		rsp = "{}";
//
//		JSON rspJSON(JsonType::JSON_OBJECT);
//		func<void> resource[unknown+1] = {};
//		resource[root] = [&](){rspJSON.newElement("helloMessage") = "hello from bo";};
//		resource[han]  = [&](){rspJSON.newElement("han") = thisDevice().toJSON();};
//		resource[devices] = [&](){
//			JSON& x = rspJSON.newElement("DeviceList");
//			x = JSON(JsonType::JSON_ARRAY);
//			x.newElement() = JSON((std::string("{\"nwkadd\":\"")+IntToHexStr(thisDevice().id()) + std::string("\", \"ieeeadd\":\"")+IntToHexStr(thisDevice().ieee_id()) + std::string("\"}")).c_str());
//			for (node_c* y : thisDevice().peerList()) {
//				zdevice_c& z = static_cast<zdevice_c&>(*y);
//				x.newElement() = JSON((std::string("{\"nwkadd\":\"")+IntToHexStr(z.id()) + std::string("\", \"ieeeadd\":\"")+IntToHexStr(z.ieee_id()) + std::string("\"}")).c_str());
//			}
//		};
//		resource[device] = [&](){
//			zdevice_c * x = devicePtr(nwkaddr);
//			if (x) rspJSON.newElement("device")= x->toJSON();
//		};
//
//		resource[endpoint] = [&](){
//			endpoint_c * x = devicePtr(nwkaddr)->endpointPtr(endpointId);
//			if (x) {
//				if (param.size() == 0) rspJSON.newElement("device")= x->toJSON();
//				else rspJSON.newElement("something") =  param;
//				std::string uri;
//				path.forEachInArr([&](size_t index, kapi::JSON& json){uri += json.toString() + "/";	});
//				uri += "ApplicationMessage";
//				Context& c = getContext(uri);
//				c.parameter(param.stringify());
//				Context y = getContext(uri);
//
//				kapi::Notification::trigger(uri);
//
//				LOG_MESSAGE("rsp = %s", c.response().c_str());
////				LOG_MESSAGE("req.strinify : %s, %s", req, y.parameter().c_str());
//
//			}
//		};
//		resource[endpoints] = [&](){
//			JSON& x = rspJSON.newElement("EndpointList");
//			x = JSON(JsonType::JSON_ARRAY);
//			for (node_c* y : devicePtr(nwkaddr)->childList()) x.newElement() = JSON((std::string("{\"id\":\"")+IntToHexStr((static_cast<endpoint_c&>(*y)).id()) + std::string("\"}")).c_str());
//		};
//		func<void , size_t, RESTfulNodes> x([&](size_t index, RESTfulNodes current) {
//			if(index >= path.size()) { 	resource[current](); return; }
//			std::string node = path[index++].toString();
//			RESTfulNodes next;
//			switch (current) {
//			case root:
//				next = rootBeer(node);
//				break;
//			case han:
//				next = hanBeer(node);
//				break;
//			case devices:
//				if (node.size() != 4) next = unknown;
//				else { kStrToInt((void*)&nwkaddr, node.c_str(), 2); next = device;	}
//				break;
//			case device:
//				next = deviceBeer(node);
//				break;
//			case endpoints:
//				if (node.size() != 2) next = unknown;
//				else { kStrToInt((void*)&endpointId, node.c_str(), 1); next = endpoint;	}
//				break;
//			case unknown:
//				return;
//				break;
//			default:
//				return;
//				break;
//			}
//			x(index,next);
//		});
//		x(0,root);
//		rsp = rspJSON.stringify();
//	}
} /* namespace interface0001 */
