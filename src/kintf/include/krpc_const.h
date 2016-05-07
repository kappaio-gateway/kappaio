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
#ifndef _KRPC_CONST_H
#define _KRPC_CONST_H
#include "kjson.h"
#include <sys/time.h>
// For cloudside
#define EPHEMERAL_PATH_ROOT "/tmp/kappad-ephemeral"
#define DEVICE_MESSAGE_FILE "ephemeral-message"
//
#define RSP_OFFSET 128

#define RPC_HEADER_TOD_TAG "tod"
#define RPC_HEADER_SEQ_TAG "seqNum"
#define RPC_HEADER_CMD_TAG "type"
#define RPC_HEADER_MAC_TAG "mac"
#define RPC_HEADER_DAT_TAG "data"
#define RPC_DAT_STATUS_TAG "status"
#define RPC_AUTH_KEY_TAG "authKey"
#define RPC_DEVICE_TOKEN_TAG "deviceToken"
#define RPC_DEVICE_STATUS_TAG "deviceStatus"
#define RPC_DAT_MESSAGE_TAG "rpcMessage"
#define RPC_DAT_RPC_MESSAGE_TAG "rpcMessage"

namespace KappaProtocol
{
	namespace ver2 {
		using namespace std;
		using namespace kapi;

		const char VER[] = "ver";
		const char URI[] = "uri";
		const char ARG[] = "arg";
		namespace uri_labels
		{
			const char pingreq[] = "ping/req";
			const char pingrsp[] = "ping/rsp";
			const char msgInd[] = "msg/ind";
			const char msgReq[] = "msg/req";
			const char wantrsp[] = "rsp";
			const char token[] = "tkn";
			const char SEQ[] = "seq";
			const char STAT[] = "sta";
		}
		inline JSON req_struct(const string& uri)
		{
			JSON x{JSONType::JSON_OBJECT};
			x["ver"] = 2;
			x["uri"] = uri;
			x["tod"] = (int)time(NULL);
			return x;
		}
		inline JSON req_struct(const string& uri, string&& pl)
		{
			JSON x = req_struct(uri);
			x[ARG] = pl;
			return x;
		}
		inline JSON req_struct(const string& uri, const string& pl)
		{
			JSON x = req_struct(uri);
			x[ARG] = pl;
			return x;
		}
	}
}

namespace remote
{
	const	char serverURL[] = "cloud.kapparock.com:32900";

	namespace client_files
	{
		const char deviceData[] = "/usr/lib/rsserial/device_data";
		const char certifcate[]	= "/usr/lib/rsserial/kcert.pem";
	}

	namespace tags
	{
		namespace header
		{
			const char tod[] = "tod";
			const char seq[] = "seqNum";
			const char cmd[] = "type";
			const char mac[] = "mac";
			const char dat[] = "data";
		}
		namespace payload
		{
			const char status[] = "status";
			const char authKey[] = "authKey";
			const char devTkn[]	= "deviceToken";
			const char devSta[]	= "deviceStatus";
			const char message[] = "rpcMessage";
			const char rpcMsg[]	= "rpcMessage";
		}
	}
}
//enum {
//	RPC_NULL,    //  	0
//	SERVER_PING, //		1
//	SIGN_IN,		//		2
//	CLIENT_HEARTBEAT // 3
//};
//#define RPC_OPCODE_RSP_OFFSET 		128
//#define RPC_OP_SIGN_IN        		2
//#define RPC_OP_CLIENT_HEARTBEAT 	3
//#define RPC_OP_CLIENT_REPORT 		4
//#define RPC_OP_NOTIFY_CLIENT 		5
//#define RPC_OP_NEW_MESSAGE  		6
//#define RPC_OP_GET_MESSAGE 			7
//#define RPC_OP_CLIENT_PING   		8
//#define RPC_OP_REQ_DEFAULT       	9
//#define RPC_OP_REQ_UPLOAD_WIDGET 	10
//#define RPC_OP_RSP_DEFAULT      	RPC_OP_REQ_DEFAULT + RPC_OPCODE_RSP_OFFSET
//#define RPC_OP_RSP_UPLOAD_WIDGET 	RPC_OP_REQ_UPLOAD_WIDGET + RPC_OPCODE_RSP_OFFSET

//

#define MAX_SOCKET_BUFFER 262144
#define RPC_OP_CLIENT_PING_RSP   136 // 128+8
#define SEND_RESPONSE 1
#ifndef SUCCESS
#define SUCCESS 0
#endif
#define RPC_REQUEST_ERROR -1
#endif
