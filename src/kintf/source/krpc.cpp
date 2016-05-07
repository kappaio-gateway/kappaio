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
//#include "krpc.h"
//#include "krpc_helper.h"
//#include <sys/time.h>
//#include <syslog.h>
//#include "mysql_conn.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <iostream>
//#include <sys/stat.h>
//#include <sys/socket.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/un.h>
//#include <sys/stat.h>
//#include <fstream>
//#include <sstream>
//#include "kutil.h"
//
//namespace krpcPrivate
//{
//int sendLocalUnixRequest(const char *input, size_t inputLen, char *output, size_t maxOutputLen, const char *socketPath)
//{
//	int rtn = -1;
//
//	int unixClientSock = openUnixClientSock(NULL, socketPath);
//
//	if (unixClientSock < 0)
//	{
//		printf("failed to open rsserialusock");
//		return rtn;
//	}
//
//	send(unixClientSock, input, inputLen, 0);
//
//	struct timeval tv;
//	fd_set fdset;
//	int selectRetVal;
//	FD_ZERO(&fdset);
//	FD_SET(unixClientSock, 	&fdset);
//	tv = {5, 0};
//
//	do {
//		selectRetVal = select(unixClientSock+1, &fdset, NULL, NULL, &tv);
//	} while (selectRetVal < 0 && errno == EINTR && tv.tv_usec != 0 && tv.tv_sec!=0);
//
//	if (selectRetVal > 0)
//	{
//		size_t bufLen = recv(unixClientSock,output, maxOutputLen, 0);
//		rtn = bufLen;
//	}
//
//	close(unixClientSock);
//	return rtn;
//}
//	int createResponseHeader(node_json *request, node_json *response)
//	{
//		if (request==NULL || response==NULL)
//		{
//			return -1;
//		}
//
//		int seqNum = request->getElement(RPC_HEADER_SEQ_TAG)->getInteger(NULL);
//		int type = request->getElement(RPC_HEADER_CMD_TAG)->getInteger(NULL);
//
//		newQueryConstructHeader(response,type + RSP_OFFSET, seqNum);
//
//		return 0;
//	}
//
//	int createResponseFrame(node_json *request, node_json *response)
//	{
//		if (request==NULL || response==NULL)
//		{
//			return -1;
//		}
//		int seqNum = request->getElement(RPC_HEADER_SEQ_TAG)->getInteger(NULL);
//		int type = request->getElement(RPC_HEADER_CMD_TAG)->getInteger(NULL);
//
//		return newQueryFormatFrame(response, type + RSP_OFFSET, seqNum);
//	}
//
//	int serverPingRequestHandle(node_json *request, node_json *response)
//	{
//		if (createResponseHeader(request,response)==0)
//		{
//			response->newObject("data")->newInteger("status", 0);
//		}
//		return 0;
//	}
//
//	int signIn(node_json *request, node_json *response)
//	{
//		int rtn=0;
//		char authKey[128], devToken[128];
//
//		const char *email = request->getElement("data")->getElement("email")->getString();
//		const char *password = request->getElement("data")->getElement("password")->getString();
//		const char *macAddr = request->getElement(RPC_HEADER_MAC_TAG)->getString();
//
//		int auth = authenticateUser(email, password);
//
//		if (auth == 0)
//		{
//			registerUserDevicePair(email,macAddr, authKey);
//			registerDevice(macAddr, devToken);
//		}
//
//		if (createResponseHeader(request, response) == 0)
//		{
//			node_json *data = response->newObject("data");
//
//			data->newInteger("status", auth);
//			if (auth == 0)
//			{
//				data->newString(RPC_AUTH_KEY_TAG,authKey);
//				data->newString(RPC_DEVICE_TOKEN_TAG, devToken);
//			}
//		}
//		return rtn;
//	}
//
//	int clientHeartBeat(node_json *request, node_json *response)
//	{
//		int rtn = 0;
////		request->dumpFile("/tmp/clientHeartBeat");
//		const char *token = request->getElement(RPC_DEVICE_TOKEN_TAG)->getString();
//		if (token == NULL)
//		{
//			rtn = -1;
//		} else {
//			const char *addr = request->getElement("sourceIP")->getString();
//			int port = request->getElement("sourcePortInt")->getInteger(NULL);
//			rtn = setDeviceHeartBeatAddress(token, addr, port);
//		}
//		return rtn;
//	}
//
//	int clientPing(node_json *request, node_json *response)
//	{
//		int rtn = 0;
////		request->dumpFile("/tmp/clientHeartBeat");
//		const char *token = request->getElement(RPC_DEVICE_TOKEN_TAG)->getString();
//		if (token == NULL)
//		{
//			rtn = -1;
//		} else {
//			const char *addr = request->getElement("sourceIP")->getString();
//			int port = request->getElement("sourcePort")->getInteger(NULL);
//			setDeviceHeartBeatAddress(token, addr, port);
//
//			response->setString("destinationIP", request->getElement("sourceIP")->getString());
//			response->setString("destinationPort", request->getElement("sourcePort")->getString());
//			response->newObject("data")->setInteger("type", RPC_OP_CLIENT_PING_RSP);
//			response->dumpFile("/tmp/ClientPing");
//			rtn = SEND_RESPONSE;
//		}
//		return rtn;
//	}
//
//	int clientStatusReport(node_json *request, node_json *response)
//	{
//		int rtn = -1;
//		char file[128];
//
//		const char *mac = request->getElement(RPC_HEADER_MAC_TAG)->getString();
//		if (createResponseFrame(request,response) < 0)
//		{
//			return rtn;
//		}
//		if (mac)
//		{
//			int offset = sprintf(file, "/tmp/");
//			getDeviceSnapshotFile(mac,NULL,file+offset);
//			node_json *devStat = request->getElement(RPC_HEADER_DAT_TAG)->getElement(RPC_DEVICE_STATUS_TAG);
//			if (devStat)
//			{
//				devStat->dumpFile(file);
//			}
//			response->getElement(RPC_HEADER_DAT_TAG)->setInteger(RPC_DAT_STATUS_TAG,0);
//			rtn = 0;
//		}
//		request->dumpFile("/tmp/clientStatusReport");
//		return rtn;
//	}
//
//	int notifyClient(node_json *request, node_json *response)
//	{
//		int rtn = RPC_REQUEST_ERROR;
//		if (request == NULL || response == NULL)
//		{
//			return rtn;
//		}
//
//		response->setString("destinationIP", request->getElement("data")->getElement("IPAddress")->getString());
//		response->setString("destinationPort", request->getElement("data")->getElement("Port")->getString());
//		response->newObject("data")->setInteger("type", RPC_OP_NEW_MESSAGE);
//		response->dumpFile("/tmp/notifyClient");
//		rtn = SEND_RESPONSE;
//		return rtn;
//	}
//
//	int getRPCMessage(node_json *request, node_json *response)
//	{
//		using namespace std;
//		int rtn = RPC_REQUEST_ERROR;
//
//		if (request == NULL || response == NULL)
//		{
//			return rtn;
//		}
//
//		if (createResponseFrame(request,response) < 0)
//		{
//			return rtn;
//		}
//
//		const char *macAddr = request->getElement(RPC_HEADER_MAC_TAG)->getString();
//		char rpcFile[64];
//		string deviceEphemeralData;
//		if (macAddr)
//		{
//			deviceEphemeralData = EPHEMERAL_PATH_ROOT + string("/dev") + string(macAddr) + string("/"DEVICE_MESSAGE_FILE);
//		} else {
//			return rtn;
//		}
//
//		int status;
//		node_json *rpcMessage =	new node_json(deviceEphemeralData.c_str(),'f',&status);
//
//		if (status != 0)
//		{
//			delete rpcMessage;
//			return rtn;
//		}
//
//		response->getElement(RPC_HEADER_DAT_TAG)->setElement(RPC_DAT_MESSAGE_TAG, rpcMessage->getElement("data")->clone());
//		response->getElement(RPC_HEADER_DAT_TAG)->setInteger(RPC_DAT_STATUS_TAG,0);
//		response->dumpFile("/tmp/getRPCMessage");
//		rtn = SEND_RESPONSE;
//		delete rpcMessage;
//		return rtn;
//	}
//
//	int rpcResponse(node_json *request, node_json *response)
//	{
//		using namespace std;
//		int rtn = RPC_REQUEST_ERROR;
//
//		if (request == NULL || response == NULL)
//		{
//			return rtn;
//		}
//
//		if (createResponseFrame(request,response) < 0)
//		{
//			return rtn;
//		}
//
//		/* EPHEMERAL_PATH_ROOT/dev<MAC Address>/rspsock<messageId>*/
//		string socketPathName =EPHEMERAL_PATH_ROOT
//				+ string("/dev") + string(request->getElement(RPC_HEADER_MAC_TAG)->getString()) \
//				+ string("/rspsock") + string(request->getElement("data")->getElement("messageId")->getString());
//
//		char *JsonString = request->getElement("data")->dumpString();
//		struct stat st;
//		stat(socketPathName.c_str(), &st);
//
//		if (S_ISSOCK(st.st_mode)) {
//
//			char output[MAX_SOCKET_BUFFER];
//			int outputLen = sendLocalUnixRequest(JsonString, strlen(JsonString), output, MAX_SOCKET_BUFFER, socketPathName.c_str());
//		}
//
//		request->dumpFile("/tmp/rpcResponse");
//		free(JsonString);
//		return 0;
//	}
//
//	int rpcRspWidgetUploadProcess(node_json *request, node_json *response)
//		{
//			using namespace std;
//			int rtn = RPC_REQUEST_ERROR;
//
//			if (request == NULL || response == NULL)
//			{
//				return rtn;
//			}
//
//			if (createResponseFrame(request,response) < 0)
//			{
//				return rtn;
//			}
//
//			/* EPHEMERAL_PATH_ROOT/dev<MAC Address>/rspsock<messageId>*/
//			string devEphemeralPath = EPHEMERAL_PATH_ROOT + string("/dev") + string(request->getElement(RPC_HEADER_MAC_TAG)->getString());
//			string socketPathName =EPHEMERAL_PATH_ROOT
//					+ string("/dev") + string(request->getElement(RPC_HEADER_MAC_TAG)->getString()) \
//					+ string("/rspsock") + string(request->getElement("data")->getElement("messageId")->getString());
//
////			char *JsonString = request->getElement("data")->dumpString();
//
//			const char *code = request->getElement("data")->getElement("data")->getElement("data")->getString();
//			string codeStr = string(code);
//			// == delete this for product ====
////			node_json *x = new node_json(JSON_OBJECT);
////			x->setString("code",code);
////			x->dumpFile("/tmp/rpcRspWidgetUploadProcess");
////			delete x;
//			// == delete this for product ====
//
//
//			string widgetRootPath = devEphemeralPath + string("/widget");
//			char commandStr[256];
//
//			// this will be the widget root
//			sprintf(commandStr, "mkdir -m 0744 -p %s", widgetRootPath.c_str());
//			system(commandStr);
//			// unpack to widget root
//			DecodeToDir((const char*)(widgetRootPath.c_str()), (char*)codeStr.c_str());
//
//			string widgetIndexFile = widgetRootPath + "/" + request->getElement("data")->getElement("data")->getElement("WidgetIndexFile")->getString();
//
//			char *codeString;
//			if (1){
//				struct stat st;
//				stat(widgetIndexFile.c_str(), &st);
//				size_t size = st.st_size;
//				if (size>0)
//				{
//					codeString = (char*)malloc(size+1);
//
//					int fd = open(widgetIndexFile.c_str(),O_RDONLY);
//					size_t fsize = read(fd, codeString,size+1);
//					close(fd);
//
//				}
//			}
//			char *JsonString;
//			node_json *output = new node_json(JSON_OBJECT);
//			output->setString("WidgetRoot",widgetRootPath.c_str());
//			output->setString("WidgetIndexFile", codeString);
//			output->setInteger("status",0);
//			JsonString = output->dumpString();
//			delete output;
//
//			struct stat st;
//			stat(socketPathName.c_str(), &st);
//
//			if (S_ISSOCK(st.st_mode)) {
//
//				char output[MAX_SOCKET_BUFFER];
//				int outputLen = sendLocalUnixRequest(JsonString, strlen(JsonString), output, MAX_SOCKET_BUFFER, socketPathName.c_str());
//			}
//
////			request->dumpFile("/tmp/rpcResponse");
//			free(JsonString);
//			free(codeString);
//			return 0;
//		}
//}
//
//int rpcHandle(node_json *req, node_json *rsp)
//{
//	int cmd = req->getElement(RPC_HEADER_CMD_TAG)->getInteger(NULL);
//	int rtn = -1;
//
//	switch (cmd)
//	{
//	case SERVER_PING:
//		rtn = krpcPrivate::serverPingRequestHandle(req, rsp);
//		break;
//	case SIGN_IN:
//		rtn = krpcPrivate::signIn(req,rsp);
//		break;
//	case CLIENT_HEARTBEAT:
//		rtn = krpcPrivate::clientHeartBeat(req,rsp);
//		break;
//	case RPC_OP_CLIENT_PING:
//		rtn = krpcPrivate::clientPing(req,rsp);
//		break;
//	case RPC_OP_CLIENT_REPORT:
//		rtn = krpcPrivate::clientStatusReport(req,rsp);
//		break;
//	case RPC_OP_NOTIFY_CLIENT:
//		rtn = krpcPrivate::notifyClient(req,rsp);
//		break;
//	case RPC_OP_GET_MESSAGE:
//		rtn = krpcPrivate::getRPCMessage(req,rsp);
//		break;
//	case RPC_OP_RSP_DEFAULT:
//		rtn = krpcPrivate::rpcResponse(req,rsp);
//		break;
//	case RPC_OP_RSP_UPLOAD_WIDGET:
//		rtn = krpcPrivate::rpcRspWidgetUploadProcess(req,rsp);
//		break;
//	default:
//		krpcPrivate::createResponseHeader(req,rsp);
//		rsp->getElement("data")->newInteger("status", -1);
//		rtn = -1;
//		break;
//	}
//	return rtn;
//}
