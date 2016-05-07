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
#ifndef KJSON_H
#define KJSON_H
#include "jansson.h"
#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <functional>

#ifdef __cplusplus

namespace kapi {
	//using namespace std;
	typedef long long int JsonInt;
	typedef long long unsigned JsonUInt;
	typedef std::string JsonString;
	typedef uint16_t JsonState;
	enum class JSONType
	{
		JSON_OBJECT = JSON_OBJECT,
		JSON_ARRAY  = JSON_ARRAY,
		JSON_STRING = JSON_STRING,
		JSON_INTEGER = JSON_INTEGER,
		JSON_REAL = JSON_REAL,
		JSON_TRUE = JSON_TRUE,
		JSON_FALSE = JSON_FALSE,
		JSON_NULL = JSON_NULL,
		JSON_EMPTY,
		JSON_FAULT
	};
	using JsonType = JSONType ;
	enum class JsonSourceType {FILE, STRING};
	class JSON;

	static const JsonState failbit = 0x0001;
	static const JsonState badbit  = 0x0002;
	static const JsonState warnbit = 0x0004;
	static const JsonState goodbit = 0x0000;

	typedef std::list<JSON> JsonList;

	class JSON
	{
	protected:
		bool jsonPtrIsBorrowed();
		bool jsonPtrIsBorrowed_;
		void reset(json_t* j);

		struct { std::string key; size_t index; } id;
	private:
		json_t* json_;
		JSON* parent_;
		JSON* fault_;
		JSONType type_;
		JsonList childList_;
		JsonString errorStr_;
		JsonString warningStr_;
		JsonString stringified_;
		JsonState state_;

		JSON& fault();
		void updateParent();
		JSON& wrapper(const std::string&);
		JSON& wrapper(size_t);
	public:

		JSON();
		JSON(JSONType);
		JSON(const char* source, JsonSourceType type);
		JSON(const char* source);
		JSON(const JSON& J);
		JSON(JSON&& J);
		JSON(const std::string& s);
		~JSON();
		JSONType type() const;
		JSON& newElement(const std::string& key);
		JSON& newElement();
		int 	erase(const std::string& key);
		int		erase(size_t i);
		JSON& operator[](const std::string& key);
		JSON& operator[](size_t index);
		size_t size();
		void operator=(const std::string& str);
		void operator=(std::string&& str);
		void operator=(const char* str);
		void operator=(JsonInt val);
		void operator=(unsigned val);
		void operator=(int val);
		void operator=(const JSON& J);
		void operator=(JSON&& J) noexcept;
		void operator=(bool boolean);
		bool exist(const std::string&) const;
		bool exist(size_t) const;
		void forEachInObj(std::function<void(const char*,JSON&)> f);
		void forEachInArr(std::function<void(unsigned,JSON&)> f);
		JsonInt toInteger();
		JsonString toString();
		bool toBool();
		JsonString stringify() const;
		JsonList& childList();
		void toFile(const char* path);
		JsonState state();
		JsonState stateAll();
		void setFailBit();
		void setFailBit(std::string);
		void setFailBit(std::stringstream);
		const JsonString& errorStr();
		JsonString errorStrAll();
		bool fail();
		bool good();
	};
} // namespace kapi
#endif
#endif
