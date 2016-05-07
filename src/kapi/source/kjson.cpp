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
#include "kjson.h"
#include "kutil.h"
#include <string.h>
#include <syslog.h>
#include <algorithm>
#include <string>
#include <iostream>
#include "jansson.h"

using namespace std;
namespace kapi {
	JSON::JSON(const JSON& J)
	:JSON()
	{
		json_error_t error;
		json_t * __json =json_deep_copy(J.json_); // make a fresh copy of json
		if (__json) reset(__json);
		else { setFailBit(std::string(const_cast<char*>(error.text))); type_ = JSONType::JSON_FAULT;}
	}

	JSON::JSON(const char* source, JsonSourceType type)	:JSON(source)
	{ }

	JSON::JSON(const char* source):JSON()
	{
		json_error_t error;
		json_t * __json = json_loads( source, JSON_DISABLE_EOF_CHECK, &error);
		if ( __json == NULL )
			if ( (__json = json_load_file(source,JSON_DISABLE_EOF_CHECK, &error)) == NULL )
			{
				setFailBit("Unable to parse JSON");
				type_=JSONType::JSON_FAULT;
				return;
			}
		reset(__json);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	// JSON::JSON(JSONType type)
	//
	// creates an empty JSON of type "type", only used to propagate error
	//
	JSON::JSON(JSONType type):JSON()
	{
		switch (type) {
		case JSONType::JSON_OBJECT: reset(static_cast<json_t*>(json_object())); break;
		case JSONType::JSON_ARRAY:	reset(static_cast<json_t*>(json_array()))  ; break;
		case JSONType::JSON_STRING:	reset(json_string( "" ));break;
		case JSONType::JSON_INTEGER:reset(json_integer( 0 ));break;
		case JSONType::JSON_NULL:   reset(static_cast<json_t*>(json_null()));break;
		case JSONType::JSON_FAULT:  type_= type;setFailBit();break;
		default: break;
		}
	}

	JSON::JSON()
	:fault_(nullptr),
	 type_(JSONType::JSON_NULL),
	 json_(json_null()),
	 parent_(NULL),
	 id(),
	 childList_(),
	 errorStr_(""),
	 warningStr_(""),
	 stringified_(""),
	 state_(goodbit),
	 jsonPtrIsBorrowed_(false)
	{	}

	JSON::JSON(JSON&& J)
	:fault_(J.fault_),
	 type_(J.type_),
	 json_(J.json_),
	 parent_(NULL),
	 id(J.id),
	 childList_(),
	 errorStr_(J.errorStr_),
	 warningStr_(""),
	 stringified_(""),
	 state_(J.state_),
	 jsonPtrIsBorrowed_(false)
	{
		// if the object owns the json, then take the ownership
		// if the object borrows the json, then shared the ownership
		if (!J.jsonPtrIsBorrowed_) J.json_=NULL;
		else json_incref(json_);
	}
	JSON::JSON(const string& s):JSON{s.c_str()}
	{}
	JSON::~JSON()
	{
		if (!jsonPtrIsBorrowed() && json_!= NULL)
		{
			json_decref(json_);
		}
		delete fault_;
	}

	JSON& JSON::fault()
	{
		if (fault_ == nullptr) fault_ = new JSON(JSONType::JSON_FAULT);
		return *fault_;
	}
	JSON& JSON::newElement(const std::string& key)
	{
		return operator[](key);
	}
	JSON& JSON::newElement()
	{
		return operator[](size());
	}
	int JSON::erase(const std::string& key) {
		if (!exist(key) || type() != JSONType::JSON_OBJECT)
			return 0;
		JSON& x = operator[](key); // get the element
		if (x.jsonPtrIsBorrowed()) json_object_del(json_, key.c_str());
		childList_.remove_if([&key](JSON& x){ return key == x.id.key; });
		return 1;
	}
	int	JSON::erase(size_t i) {
		if (!exist(i) || type() != JSONType::JSON_ARRAY) {
			return 0;
		}

		JSON& x = operator[](i); // get the element
		if (x.jsonPtrIsBorrowed()) {
			json_array_remove(json_, i);
		} else {syslog(LOG_INFO, "not removed");}
		childList_.remove_if([i](JSON& x){ return x.id.index == i; });
		return 1;
	}
	JSON& JSON::operator[](const std::string& key)
	{
		if (type() != JSONType::JSON_OBJECT) {
			if (type() == JSONType::JSON_FAULT) {
				return *this;
			}
			else if (type() ==  JSONType::JSON_NULL) {
				reset(json_object(  ));
				updateParent();
			}
			else {
				setFailBit(std::string("Attempt to access \"") + key + "\" of non-JSON_OBJECT");
				return fault();
			}
		}
		for (JSON& J : childList()) if (J.id.key == key) return J;
		return wrapper(key);
	}
	JSON& JSON::operator[](size_t index)
	{
		std::stringstream ss;
		if (type() != JSONType::JSON_ARRAY) {
			if (type() == JSONType::JSON_FAULT) return *this;
			ss << "Attempt to access [" << index << "] of non-JSON_ARRAY";
			setFailBit(ss.str());
			return fault();
		}
		for (JSON& J : childList()) if (J.id.index == index) return J;

		return wrapper(index);
	}

	void  JSON::operator=(const char* str) {
		if (type() == JSONType::JSON_FAULT) return;
		if (type() != JSONType::JSON_STRING) {
			reset(json_string( str ));
			updateParent();
		} else json_string_set(json_, str);
		return;
	}
//	void JSON::operator=(bool b) {
//		if (type() == JSONType::JSON_FAULT) return;
//
//		if (b==true) reset(json_true());
//		else reset(json_false());
//		updateParent();
//	  return;
//	}
	void  JSON::operator=(const std::string& str)
	{
		operator=(str.c_str());
	}

	void JSON::operator=(std::string&& str)
	{
		operator=(str.c_str());
	}

	size_t JSON::size()
	{
		if (type() ==  JSONType::JSON_ARRAY) return json_array_size(json_);
		if (type() ==  JSONType::JSON_OBJECT) return json_object_size(json_);
		return 0;
	}

	void JSON::operator=(JsonInt val)
	{
		if (type() == JSONType::JSON_FAULT) return;
		if (type() != JSONType::JSON_INTEGER)
		{
			reset(json_integer( static_cast<json_int_t>(val)));
			updateParent();
		} else json_integer_set(json_, static_cast<json_int_t>(val));

		return;
	}
	void JSON::operator=(unsigned val)
	{
		operator=(static_cast<JsonInt>(val));
	}
	void JSON::operator=(int val)
	{
		operator=(static_cast<JsonInt>(val));
	}
	void JSON::operator=(const JSON& J)
	{
		//if (type() == JSONType::JSON_FAULT || J.type() == JSONType::JSON_FAULT) return;
		json_error_t error;
		json_t * __json =json_deep_copy(J.json_); // make a fresh copy of json
		if (__json)
		{
			reset(__json);
			updateParent();
		}
		else { setFailBit(std::string(const_cast<char*>(error.text))); type_ = JSONType::JSON_FAULT;}
		return;
	}

	void JSON::operator=(JSON&& J) noexcept
	{
		//if (type() == JSONType::JSON_FAULT || J.type() == JSONType::JSON_FAULT) return;
		//if (type() == JSONType::JSON_FAULT || J.type() == JSONType::JSON_FAULT) return;
		reset(J.json_);
		J.json_=NULL;
		updateParent();

		return;
	}

	void JSON::operator=(bool boolean) {
		if (type() == JSONType::JSON_FAULT) return;
		boolean ?  reset(json_true()) : reset(json_false());
		updateParent();
		return;
	}
	bool JSON::exist(const string& k) const {
		if (type() == JSONType::JSON_OBJECT) {
			return (json_object_get(json_, k.c_str()) != NULL);
		}
		return false;
	}

	bool JSON::exist(size_t i) const {
		if (type() == JSONType::JSON_ARRAY) {
			return (json_array_get(json_, i) != NULL);
		}
		return false;
	}

	void JSON::forEachInObj(std::function<void(const char* key,JSON&)> f){
		if (type() != JSONType::JSON_OBJECT) return;
		for (void *iter = json_object_iter(json_); iter; iter = json_object_iter_next(json_, iter)) {
			const char* key = json_object_iter_key(iter);
			f(key, operator[](key));
		}
	}
	void JSON::forEachInArr(std::function<void(unsigned,JSON&)> f) {
		if (type() != JSONType::JSON_ARRAY) return;
		for(size_t index_ = 0; index_ < size(); index_++)
			f(index_, operator[](index_));
	}
	void JSON::updateParent() {
		if (parent_ != NULL) {
			jsonPtrIsBorrowed_=true;
			if (parent_->type() == JSONType::JSON_OBJECT) json_object_set_new(parent_->json_, id.key.c_str(), json_);
			else if (parent_->type() == JSONType::JSON_ARRAY) json_array_set_new(parent_->json_, id.index, json_);
			else setFailBit("Invalid type of parent");
		}
	}
	JsonList& JSON::childList() {return childList_;}
	JsonString JSON::stringify() const {
		JsonString tempStr("");
		char *temp = json_dumps(json_,JSON_ENSURE_ASCII|JSON_COMPACT|JSON_INDENT(0));
		if (temp)
		{
			tempStr = temp;
			free(temp);
		}
		return tempStr;
	}

	JsonString JSON::toString() {
		JsonString tempStr("");
		if (type() != JSONType::JSON_STRING) {/* report error */ tempStr= "";}
		else tempStr = json_string_value(json_);
		return tempStr;
	}
	bool JSON::toBool()
	{
		return type() == JSONType::JSON_TRUE;
	}
	JsonInt JSON::toInteger() {
		JsonInt rtn = 0;
		switch (type())
		{
		case JSONType::JSON_INTEGER:
			rtn = static_cast<JsonInt>(json_integer_value(json_)); break;
		case JSONType::JSON_STRING:
			if (1) {
				std::string temp="";
				size_t a = strlen(json_string_value(json_));
				if (a & static_cast<size_t>(1) == 1) { temp += "0"; a++;}
				temp += json_string_value(json_);
				kStrToInt((void*)&rtn, temp.c_str(), std::min(a>>1, sizeof(JsonInt)));
			}
			break;
		default:setFailBit("Invalid conversion to integer");break;
		}
		return rtn;
	}
	void JSON::toFile(const char* path){json_dump_file(json_,path, JSON_INDENT(0)|JSON_COMPACT);}
	JSONType  JSON::type() const { return type_; }
	JsonState JSON::state() { return state_; }
	JsonState JSON::stateAll() {
		JsonState x = state();
		for (JsonList::iterator it = childList().begin(); it != childList().end(); ++it)
			x |= (*it).stateAll();
		return x;
	}
	void JSON::setFailBit() { state_ |= failbit;}
	void JSON::setFailBit(std::string message) {setFailBit(); errorStr_ += message; errorStr_ += "; "; }
	void JSON::setFailBit(std::stringstream message) {setFailBit(message.str());}

	bool JSON::jsonPtrIsBorrowed()
	{
		return jsonPtrIsBorrowed_;
	}
	void JSON::reset(json_t* j)
	{
		if (json_) json_decref(json_);
		json_ = j;
		type_ = static_cast<JSONType>json_typeof(json_);
		childList().clear();
	}
	bool JSON::fail() { return ( state() & failbit );}
	bool JSON::good() { return ( state() == goodbit );}
	const JsonString& JSON::errorStr() {return errorStr_;}
	JsonString JSON::errorStrAll(){
		JsonString x = errorStr();
		for (JsonList::iterator it = childList().begin(); it != childList().end(); ++it) x += (*it).errorStrAll();
		return x;
	}

	JSON& JSON::wrapper(const std::string& key)
	{
		JSON& x = *(childList().emplace(childList().end()));
		x.parent_ = this;
		x.id.key = key;

		if (json_t * json = json_object_get(json_, key.c_str()))
		{
			x.reset(json);
		} else
		{
			x.updateParent();
		}
		x.jsonPtrIsBorrowed_ = true;

		return x;
	}
	JSON& JSON::wrapper(size_t index)
	{
		JSON& x = *(childList().emplace(childList().end()));
		x.parent_ = this;
		x.id.index = index;
		for (size_t i = size(); i <= index; i++)
		{
			json_array_append_new(json_, static_cast<json_t*>(json_null()));
		}

		if (json_t * json = json_array_get(json_, index))
		{
			x.reset(json);
		} else
		{
			x.updateParent();
		}
		x.jsonPtrIsBorrowed_ = true;

		return x;
	}
	} // namespace kapi

