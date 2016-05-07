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
using namespace std;
namespace remote
{
	string recvStr(int fd) {
		string out;
		static const int maxlen = 4096;
		char x[maxlen];
		int l = 0;

		struct pred {
			int maxLen_;
			int fd_;
			pred(int m, int fd):maxLen_{m},fd_{fd}{}
			bool operator()(int l) {
				if (l <= 0 || l < maxLen_) return false;
				char b[1];
				return recv(fd_, b, 1, MSG_PEEK|MSG_DONTWAIT) > 0;
			}
		};

		pred fn{maxlen, fd};
		do {
			l = recv(fd, x, maxlen,0);
			out.append(x,l);
		} while(fn(l));
		return out;
	}

	int open_sock_(const char* name, const char* service, addrinfo **result, int fm, int st, int fl)
	{
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = fm;
		hints.ai_socktype = st;
		hints.ai_flags = fl;
		if (getaddrinfo(name,service, &hints, result) < 0) {
			syslog(LOG_INFO,"getaddrinfo failed %s", strerror(errno));
			return -1;
		}
		int socketfd = socket((*result)->ai_family, (*result)->ai_socktype, (*result)->ai_protocol);
		if (socketfd < 0) {
			syslog(LOG_INFO,"socket() open failed: %s", strerror(errno));
			close(socketfd);
			return -1;
		}
		int flags = fcntl(socketfd, F_GETFL, 0);
		fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
		int on=1;
		setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
		return socketfd;
	}

	inline int openUDPSock(const char* name, const char* service, addrinfo **result)
	{
		return open_sock_(name, service, result,AF_INET,SOCK_DGRAM,AI_PASSIVE );
	}

	void UDPConn::freeResrc() {
		if (fd_ > 0) close(fd_);
		if (ai_!=NULL) freeaddrinfo(ai_);
	}
	void UDPConn::acqrResrc() {
		const char* cn = name_.size() == 0 ? NULL : name_.c_str();
		const char* cs = service_.size() == 0 ? NULL : service_.c_str();
		while ((fd_=openUDPSock(cn, cs, &ai_)) < 0 && rt_ > 0)
		{
			kSleep(rt_);
			syslog(LOG_INFO, "open UDP port failed, retry in %d secs...", rt_);
		}
	}

	void selects::reset() {
		FD_ZERO(&rdset_);
		conns.clear();
		fdMax=0;
	}
	void selects::rFD_SET(int fd, function<void()> cb) {
		FD_SET(fd, &rdset_);
		conns.emplace_back(fd,move(cb));
		fdMax = fd > fdMax ? fd : fdMax;
	}
	void selects::listenOnce(){
		int retVal;
		timeval tv_ = tv;
		fd_set rdset__ = rdset_;

		do {
			retVal = select(fdMax+1, &rdset__, NULL, NULL, &tv_);
		} while (retVal < 0 && errno == EINTR && tv_.tv_usec != 0 && tv_.tv_sec!=0);

		if (retVal > 0)	{
			for (auto& x : conns) {
				if (FD_ISSET(x.first,&rdset_)) x.second();}
		}
	}

	int openTCP(const char *hostname, const char *port)
	{
		addrinfo *result;
		int socketfd = open_sock_(hostname, port, &result,AF_INET,SOCK_STREAM,AI_PASSIVE );
		if (socketfd < 0)
		{
			freeaddrinfo(result);
			syslog(LOG_INFO,"socket() open failed: %s", strerror(errno));
			close(socketfd);
			return -1;
		}

		connect(socketfd,result->ai_addr,result->ai_addrlen);

		fd_set wrset;
		timeval tv = {5,0};
		FD_ZERO(&wrset);
		FD_SET(socketfd, &wrset);
		int retVal = 0;
		do {
			retVal = select(socketfd+1, NULL, &wrset, NULL, &tv);
		} while (retVal < 0 && errno == EINTR && tv.tv_usec != 0 && tv.tv_sec!=0);

		if (retVal <= 0)
		{
			return -1;
		}

		int so_error;
		socklen_t slen = sizeof so_error;
		getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &so_error, &slen);
		if (so_error == 0)
		{
			freeaddrinfo(result);
			return socketfd;
		} else {
			freeaddrinfo(result);
			syslog(LOG_INFO,"connection failed");
			close(socketfd);
			return -1;
		}
	}
	int sslWaitOn(SSL *ssl, int timeoutSec, int(*fn)(SSL* ssl))
	{
		int status = fn(ssl);
		int sslerr = 0;
		if (status < 0) {
			sslerr = SSL_get_error(ssl, status);
		}

		if (status >= 0 || (sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE))
		{
			return status;
		}

		int timeoutuSec = timeoutSec * 1000000;
		int timeuSecAcc = 0;
		timespec tv = {0, 10000000};

		while (status < 0 && timeuSecAcc < timeoutuSec)
		{
			status = fn(ssl);
			timeuSecAcc += 10000;
			kNanosleep(&tv);
		}

		if (status < 0)
		{
			sslerr = SSL_get_error(ssl, status);
			syslog(LOG_INFO,"sslerr %d, timeAcc %d", sslerr, timeuSecAcc);
		}
		return status;
	}

	SSL* openSSLConn(const char* IP, const char* port, SSL_CTX *ctx)
	{
		int fd_ = openTCP(IP, port);
		if (fd_ < 0) return NULL;

		SSL* ssl = SSL_new(ctx);
		SSL_set_fd(ssl,fd_);
		SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
		if ( sslWaitOn(ssl, 15, SSL_connect) >= 0)
		{
			long verify = SSL_get_verify_result(ssl);
			//if(SSL_get_verify_result(ssl) == X509_V_OK)
			if(verify == X509_V_OK)
			{
				if (sslWaitOn(ssl, 15, SSL_do_handshake) >= 0)
				{
					return ssl;
				} else {
					syslog(LOG_INFO, "sslWaitOn(ssl, 15, SSL_do_handshake) failed");
				}
			} else {
				syslog(LOG_INFO, "SSL_get_verify_result(ssl) failed, stat = %d", verify);
			}
		} else {
			syslog(LOG_INFO, "sslWaitOn(ssl, 15, SSL_connect)");
		}

		close(fd_);
		SSL_free(ssl);

		return NULL;
	}

	sslCliConn::sslCliConn(const char* IP, const char* port, int rt, SSL_CTX *ctx)
	{
		while ((ssl = openSSLConn(IP, port, ctx)) == NULL && rt > 0) {
			syslog(LOG_INFO, "giving up this connection, retry in %d secs", rt);
			kSleep(rt);
		}
	}

	sslCliConn::~sslCliConn()
	{
		if (ssl != NULL)
		{
			int fd = SSL_get_fd(ssl);
			SSL_shutdown(ssl);
			close(SSL_get_fd(ssl));
			SSL_free(ssl);
		}
	}

	int sslCliConn::send(const char* b, size_t s)
	{
		int err_code = 0;
		int SSL_write_len = -1;
		do {
			SSL_write_len = SSL_write(ssl,b,s);
			if (SSL_write_len < 0 )
			{
				err_code = SSL_get_error(ssl, SSL_write_len);
			}
		} while (err_code==SSL_ERROR_WANT_WRITE && SSL_write_len < 0 );
		return SSL_write_len;
	}

	int sslCliConn::recv(char* b, size_t s)
	{
		SSL_read(ssl, b, s);
	}

	string& sslCliConn::recvStr() {
		const int maxLen = 16384;
		int bufLen = 0,	blkLen = 0,	err_code = 0;
		char b_[maxLen];
		do {
			blkLen = SSL_read(ssl,b_ + bufLen,maxLen);
			if (blkLen <= 0) {
				err_code = SSL_get_error(ssl, blkLen);
			} else {
				b_[blkLen] = 0;
				buf_.append(b_);
			}
		} while (blkLen == maxLen || (blkLen <= 0 && (err_code == SSL_ERROR_WANT_READ || err_code == SSL_ERROR_WANT_WRITE)));
		return buf_;
	}
	int sslCliConn::fd() const
	{
		return SSL_get_fd(ssl);
	}
}

