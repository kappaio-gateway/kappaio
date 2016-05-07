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
#ifndef WIDGET_MANAGER_HPP_
#define WIDGET_MANAGER_HPP_
#include <string>
#include "notification.h"
#include "restful.h"
#include "kjson.h"
namespace kapi
{
	namespace widget_manager
	{
		using Context = ApplicationInterface::Context;
		static const char widget_registry_json[] = "/usr/lib/rsserial/widget_registry.json";
		// assumptions:
		// 1. bl_ is of type JSON_OBJECT
		// 2. every element in bl_ is of type JSON_STRING
		struct pred
		{
			JSON& bl_;
			pred(JSON& bl) : bl_(bl){}
			bool operator()(JSON& cmp)
			{
				bool result = true;
				bl_.forEachInObj([&result, &cmp](const char* key, JSON& val) {
					if (cmp.exist(key))  {
						result &= cmp[key].toString() == val.toString();
					}
				});
				return result;
			}
		};

		void widgetRegistry(Context C)
		{
			JSON param{C.parameter()};
			JSON reg{widget_registry_json};
			JSON request{C.request()};
			JSON rsp{JSONType::JSON_OBJECT};
			int status = 0;

			if (!reg.good()) {
				reg = JSON{JSONType::JSON_OBJECT};
			}

			JSON& list = reg["list"];

			if (list.type() != JSONType::JSON_ARRAY) {
				list = JSON{JSONType::JSON_ARRAY};
			}

			string method = request["method"].toString();
			if (method ==  "DELETE")	{
				if (param.size() == 0) {
					list = JSON{JSONType::JSON_ARRAY};
				} else {
					pred p{param};
					for (int i = 0; i < list.size(); i++)
					{
						if (p(list[i])) {
							list.erase(i);
						}
					}
				}

			} else if (method ==  "POST") {
				pred p{param};
				bool exist = false;
				for (int i = 0; i < list.size(); i++)
				{
					if (p(list[i])) {
						exist = true;
					}
				}
				if (!exist) {
					list.newElement() = JSON{C.parameter()};
				}
			} else if (method == "GET") {
				rsp["result"] = reg["list"];
			}	else {
				status = -1;
				rsp["err"] = method + " not supported";
			}
			reg.toFile(widget_registry_json);
			rsp["status"] = status;
			C.response(rsp.stringify());
		}

		void widgetLoad(Context C)
		{
			JSON param{C.parameter()};
			JSON reg{widget_registry_json};
			JSON rsp{JSONType::JSON_OBJECT};
			JSON& list =  reg["list"];
			pred p{param};
			int status = -1;
			for (int i = 0; i < list.size(); i++)
			{
				JSON& entry = list[i];
				if (p(entry))
				{
					JSON& data = rsp["result"];
					data = entry;
					const char* codeFile = entry["root"].toString().c_str();
					char* base64EncodeOutput;
					int encodedSize = EncodeDir(codeFile, &base64EncodeOutput);
					if (encodedSize > 0)
					{
						data["content"]	= base64EncodeOutput;
					}
					free(base64EncodeOutput);
					status = 1;
				}
			}
			rsp["status"] = status;
			C.response(rsp.stringify());
		}
	}
}
#endif
