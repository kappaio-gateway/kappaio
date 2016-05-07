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
#ifndef RESTFUL_H_
#define RESTFUL_H_
#include <string>
#include <memory>
using namespace std;
namespace ApplicationInterface
{
	const char EventTag[] = "Application";
	class __Context
	{
	public:
		__Context(string uri);
		~__Context();
		string& URI();
		string& parameter();
		string& response();
		string& request();
		void parameter(string P);
		void response(string&& R);
		void response(const string& R);
		void request(const string& S);
	private:
		string uri_;
		string parameter_;
		string response_;
		string request_;
	};
	typedef __Context& Context;

	Context getContext(const std::string& uri);
void handleRESTful(const string& req, std::string& rsp);
//void handleRESTful(string&& req, std::string& rsp);

//namespace __RESTFul__
//{
//	void bind(const string& uri, function<void(Context)> handler);
//	void trigger(const string& uri, Context C);
//}
} // ApplicationInterface
#endif
