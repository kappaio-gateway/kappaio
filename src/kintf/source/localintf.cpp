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
#include "connections.h"
#include "localintf.h"
#include "kjson.h"
#include <time.h>
#include <pthread.h>
#include "kglobal.h"
#include "kutil.h"
#include "restful.h"

using namespace remote;
using namespace ApplicationInterface;
using namespace kapi;

namespace {

	void* hostUNIXSocketServer(void *td)
	{
		int fd = openUnixServerSock(NULL, "/tmp/rsserialusock");
		if (fd < 0)
		{
			LOG_ERROR_MESSAGE("openUnixServerSock failed");
			return nullptr;
		}

		if (listen(fd, 128) < 0)
		{
			LOG_ERROR_MESSAGE("listen() failed in UNIXServerSock");
			return nullptr;
		}

		selects s{60};

		s.rFD_SET(fd, [fd]() {
			UnixSocketServelet svr(fd);
			if (svr.fd() < 0)
			{
				return;
			}

			JSON req{ recvStr(svr.fd()) };
			if (!req.good())
			{
				return;
			}

			if (req.exist("ver") && req["ver"].toString() == "0001") /*RESTful mode*/
			{
				string rsp{"[\"success\"]"};
				handleRESTful(req["request"].toString(), rsp);
				svr.send(rsp);
			}
		});

		while (1) {
			s.listenOnce();
		}
		close(fd);
		return nullptr;
	}

}

void localIntfInit()
{
	pthread_t thread;
	pthread_create(&thread, NULL, &hostUNIXSocketServer, NULL);
	pthread_detach(thread);
}

string localRpcRequest(const string& in)
{
	string out{};
	int fd = openUnixClientSock(NULL, "/tmp/rsserialusock");
	if (fd < 0)
	{
		out.append("failed to open rsserialusock");
		return out;
	}
	send(fd, in.c_str(), in.size(), 0);
	selects s{5};
	s.rFD_SET(fd,[fd, &out]() {
		out = move(recvStr(fd));
	});
	s.listenOnce();

	close(fd);
	return out;
}
