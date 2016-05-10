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
#include "remoteif.h"
#include "openssl/ssl.h"
#include "openssl/bio.h"
#include "openssl/err.h"
#include "kglobal.h"
#include "kutil.h"
#include "kjson.h"
#include "krpc_const.h"
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>
#include <semaphore.h>
#include <time.h>
#include <string>
#include <utility>
#include <functional>
#include <thread>
#include "connections.h"
#include "notification.h"
#include "restful.h"
#include "apsdb.h"
#include "widget_manager.hpp"
#define KSERVER "cloud.kapparock.com:32900"

namespace
{
	char hwAddr[13];
	sem_t *sslChannelSem;

	int getMACAddr()
	{
		struct ifreq ifr;
		char iface[] = "wlan0";
		int fd = socket(AF_INET, SOCK_DGRAM, 0);
		ifr.ifr_addr.sa_family = AF_INET;
		strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
		ioctl(fd, SIOCGIFHWADDR, &ifr);
		close(fd);
		unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
		sprintf(hwAddr, "%02x%02x%02x%02x%02x%02x",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		return 0;
	}

	SSL_CTX *ctx;

	int CTXInit()
	{
		/* Set up the SSL context */
		ctx = SSL_CTX_new(TLSv1_2_client_method());
		/* Load the trust store */
		if(! SSL_CTX_load_verify_locations(ctx, "/usr/lib/kappaio/files/kcert.pem", NULL))
		{
			fprintf(stderr, "Error loading trust store\n");
			ERR_print_errors_fp(stderr);
			SSL_CTX_free(ctx);
			return -1;
		}

		if (ctx == NULL) {
			ERR_print_errors_fp(stderr);
			LOG_ERROR_MESSAGE("failed to obtain ctx");
			return -1;
		}
		return 0;
	}
}

//void showCerts(SSL* ssl)
//{
//	X509 *cert;
//	char *line;
//	cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
//	if ( cert != NULL ) {
//		LOG_MESSAGE("Server certificates:");
//		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
//		LOG_MESSAGE("Subject: %s", line);
//		free(line);       /* free the malloc'ed string */
//		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
//		LOG_MESSAGE("Issuer: %s", line);
//		free(line);       /* free the malloc'ed string */
//		X509_free(cert);     /* free the malloc'ed certificate copy */
//	}	else {
//		LOG_MESSAGE("No certificates.");
//	}
//}

namespace remote
{
	class cbContext {
	public:
		virtual void response(const string&) = 0;
		virtual const string& args() = 0;
		virtual ~cbContext() {}
	};
	using cbContextRef = cbContext&;

	namespace credentials {
		using JSON = kapi::JSON;
		using namespace remote::client_files;
		using namespace remote::tags;
		using namespace std;
		bool remoteAccessIsEnabled() {
			return JSON(deviceData)["connect"].toInteger() == 1;
		}
		string 	deviceToken() {
			return JSON(deviceData)[payload::devTkn].toString();
		}
	} // namespace credentials

	namespace message_payloads
	{
		using JSON = kapi::JSON;
		using namespace credentials;
		using namespace KappaProtocol::ver2;
		using namespace KappaProtocol::ver2::uri_labels;
		struct cbCtx : public cbContext
		{
			string msg_;
			cbCtx(const string& msg):msg_{msg} {}
			cbCtx(string&& msg):msg_{move(msg)}{}
			const string& args() 	{return msg_;}
			void  response(const string& s) {}
			~cbCtx(){}
		};
		using cbCtxRef = cbCtx&;
		struct onePing_ {
			int lastReq_{0};
			int lastRsp_{0};
			int minReqIntv_{};
			int minRspIntv_{};
			int bii_{}; // broken link indicator
			int seq_{0};
			void operator()(UDPConn& conn) {
				if ((time(0) - lastReq_) < minReqIntv_)
				{
					return;
				}

				JSON pl_ = req_struct(pingreq);
				pl_[ARG][token] = deviceToken();
				pl_[ARG][wantrsp] = ((time(0) - lastRsp_) > minRspIntv_);

				if (((time(0) - lastRsp_) > minRspIntv_))
				{
					LOG_MESSAGE("sending ping..., last good : %d secs ago", time(0) - lastRsp_);
				}

				lastReq_ = time(0);
				conn.sendto(pl_.stringify());
			}
			onePing_(int minReqIntv_, int minRspIntv_, int bii)
			:minReqIntv_{minReqIntv_},minRspIntv_{minRspIntv_},bii_{bii}
			{
				kapi::notify::simpleHandler(pingrsp, [this](cbCtxRef x) {
					lastRsp_ = time(0);
				});
			}
			bool brokenLink() {return (time(NULL) - lastRsp_ - minRspIntv_> bii_);}
		}; // struct onePing_

		void getMsg(cbCtxRef x)
		{
			using JSON = kapi::JSON;
			using namespace KappaProtocol::ver2;
			using namespace ApplicationInterface;
			using namespace kapi::notify;

			JSON pkt = req_struct("msg/req");
			pkt[ARG] = x.args();
			sslCliConn cli{"cloud.kapparock.com", "32900", 5,ctx };
			cli.send(pkt.stringify());
			selects S{2};
			S.rFD_SET(cli.fd(),[&cli](){
				JSON msg{cli.recvStr()};
				class forwardCtx_t : public cbContext
				{
				public:
					string arg_;
					sslCliConn* pCli_;
					forwardCtx_t(string&& a, sslCliConn* pCli) : arg_{a}, pCli_{pCli} {}
					void response(const string& s) override {
						JSON x = req_struct("msg/rsp", s);
						pCli_->send(x.stringify());
					}
					const string& args() override { return arg_;}
					~forwardCtx_t() {}
				};
				forwardCtx_t fx{msg["arg"].toString(), &cli};
				simpleTrigger(msg[ "uri" ].toString(), static_cast<cbContextRef>(fx));
			});
			S.listenOnce();
		}
		void handleRestful(cbContextRef ctx_) {
			using namespace ApplicationInterface;
			string rsp;
			handleRESTful(ctx_.args(), rsp);
			ctx_.response(rsp);
		}

		void widgetHandle(cbContextRef ctx_)
		{
			uint8_t epId = JSON{ctx_.args()}["path"][1].toInteger();
			JSON rsp{JSONType::JSON_OBJECT};
			if (! thisDevice().endpointExist(epId) ) {
				rsp["status"] = -1;
			} else {
				const char* codeFile = thisDevice().endpoints(epId).getWidgetRoot();
				char* base64EncodeOutput;
				int encodedSize = EncodeDir(codeFile, &base64EncodeOutput);
				if (encodedSize > 0)
				{
					rsp["data"] = base64EncodeOutput;
					rsp["WidgetRoot"] = thisDevice().endpoints(epId).getWidgetRoot();
					rsp["WidgetIndexFile"] = thisDevice().endpoints(epId).getWidgetIndexFile();
					rsp["status"] = 0;
				}
				free(base64EncodeOutput);
			}
			ctx_.response(rsp.stringify());
		}
		void widget_loader_handle(cbContextRef ctx_)
		{
			JSON widgetList{"/usr/lib/kappaio/files/widgets.json"};
			JSON rsp{JSONType::JSON_OBJECT};
			if (!widgetList.good())
			{
				rsp["status"] = -1;
			} else {
				rsp["status"] = 0;

			}
			ctx_.response(rsp.stringify());
		}
		using namespace kapi;
		using namespace kapi::notify;
		using namespace KappaProtocol::ver2::uri_labels;
		using namespace ApplicationInterface;
		void sign_in_handle(Context C)
		{
			sslCliConn cli{"cloud.kapparock.com", "32900", 5,ctx };
			JSON pl{C.parameter()};
			JSON req{C.request()};
			JSON response{JSONType::JSON_OBJECT};
			response["status"] = -1;

			if (req["method"].toString() == "POST") {
				pl["mac"] = hwAddr;
				cli.send(req_struct("sign_in/req", pl.stringify()).stringify());

				// TODO: move timeout value to a seperate file
				selects s(2);

				s.rFD_SET(cli.fd(),[&cli, &pl, &response](){
					JSON rsp{JSON{cli.recvStr()}["arg"].toString()};
					if (rsp["status"].toInteger() !=0)
					{
						return;
					}
					JSON task{JSONType::JSON_OBJECT};
					task["email"] = pl["email"];
					task["authkey"] = rsp["authKey"];
					JSON devData{JSONType::JSON_OBJECT};
					devData["deviceToken"] = rsp["deviceToken"];
					devData["authorizedUser"] = move(task);
					devData["connect"] = 1;
					devData.toFile("/usr/lib/kappaio/files/device_data");
					JSON cred{"/usr/lib/kappaio/files/device_data"};
					response["authorized"] = cred["authorizedUser"];
					response["status"] = 0;

				});
				s.listenOnce();
			} else if (req["method"].toString() == "DELETE") {
				unlink("/usr/lib/kappaio/files/device_data");
				response["status"] = 0;
			} else if (req["method"].toString() == "GET") {
				response["status"] = 0;
				JSON cred{"/usr/lib/kappaio/files/device_data"};
				response["authorized"] = cred["authorizedUser"];
			}
			C.response(response.stringify());
		}

		void init()
		{
			simpleHandler("msg/ind" , getMsg);
			simpleHandler("restful/req" , handleRestful);
			simpleHandler("widget/req" , widgetHandle);
			handler(EventTag, "sign_in" , sign_in_handle);
			handler(EventTag, "widget_registry"	, widget_manager::widgetRegistry);
			handler(EventTag, "widget_load" , widget_manager::widgetLoad);
//			handler(EventTag, "sign_in", [](Context C) {
//				sslCliConn cli{"cloud.kapparock.com", "32900", 5,ctx };
//				JSON pl{C.parameter()};
//				pl["mac"] = hwAddr;
//				cli.send(req_struct("sign_in/req", pl.stringify()).stringify());
//				selects s(2);
//				s.rFD_SET(cli.fd(),[&cli, &pl](){
//					JSON rsp{JSON{cli.recvStr()}["arg"].toString()};
//					if (rsp["status"].toInteger() !=0) { return; }
//					JSON task{JSONType::JSON_OBJECT};
//					task["email"] 	= pl["email"];
//					task["authkey"] = rsp["authKey"];
//					JSON devData{JSONType::JSON_OBJECT};
//					devData["deviceToken"] 		= rsp["deviceToken"];
//					devData["authorizedUser"] = move(task);
//					devData["connect"] = 1;
//					devData.toFile("/usr/lib/kappaio/files/device_data");
//				});
//				s.listenOnce();
//				C.response(C.parameter());
//			});
		}
	}// namespace message_payloads

	namespace internal {

	}//namespace system_task
} // namespace remote

namespace {
	void *startUDPServer000(void *td)
	{
		using namespace remote::credentials;
		using namespace remote::message_payloads;
		using namespace remote::internal;
		using namespace remote;
		using namespace kapi::notify;

		// it will be better if were loaded from a JSON file
		UDPConn server{"cloud.kapparock.com", "32901", 10};
		selects S{10};
		onePing_ onePing{45, 3600, 900};
		while(1)
		{
			// *		*			*  WARNINIG		*			*			*
			// this check of enable should be moved somewhere else
			// what if it was disabled when the connection is still open??
			if (!remoteAccessIsEnabled()) {
				kSleep(5); continue;
			}

			if (onePing.brokenLink()) {
				server.reset();
				S.reset();
				S.rFD_SET(server.socketFd(), [&server]() {
					char b[MAX_SOCKET_BUFFER];
					if (server.recvfrom(b, MAX_SOCKET_BUFFER) > 0) {
						JSON x{b};
						x.toFile("/tmp/justarrive");
						cbCtx f{x[ ARG ].toString()};
						simpleTrigger(x[ URI ].toString(),f);
					}
					return;
				});
			}
			onePing(server);
			S.listenOnce();
		}
		return nullptr;
	}
}

void remoteIntfInit()
{
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	SSL_load_error_strings();

	CTXInit();
	getMACAddr();
	sslChannelSem = (sem_t *)malloc(sizeof(sem_t));
	sem_init(sslChannelSem, 0, 1);

	pthread_t thread;
	pthread_create(&thread, NULL, startUDPServer000, (void *)NULL);
	pthread_detach(thread);

	remote::message_payloads::init();
	return;
}
