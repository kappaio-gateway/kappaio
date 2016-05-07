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
#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <vector>
#include <functional>
#include "kutil.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <memory>
#include "kjson.h"

namespace remote {
	std::string recvStr(int fd);

	class Conn {
	public:
		virtual int fd() const = 0;
		virtual void cb() 		= 0;
		virtual ~Conn() {}
	};
	class UDPConn// : public Conn
	{
	private:
		int fd_ {-1};
		addrinfo *ai_ {NULL};
		sockaddr_in cliaddr {};
		socklen_t len_{};
		std::string name_;
		std::string service_;
		int	rt_{-1};
		time_t initTime {time(NULL)};
		void freeResrc();
		void acqrResrc();
	public:
		UDPConn()= delete;
		UDPConn(const char* ip, const char* port):UDPConn{ip,port,-1}
		{}
		UDPConn(const char* ip, const char* port, int rt)
		:name_{ip},service_{port},rt_{rt}
		{
			acqrResrc();
		}
		~UDPConn() {
			freeResrc();
		}
		void reset() {
			freeResrc(); acqrResrc();
		}
		int	socketFd() const {
			return fd_;
		}
		int	sendto(const char* b, size_t s) {
			return ::sendto(fd_, b, s, 0, ai_->ai_addr, ai_->ai_addrlen);
		}
		int	sendto(const std::string& str) {
			return sendto(str.c_str(),str.size());
		}
		int	recvfrom(char* b, size_t s) {
			return ::recvfrom(fd_,	b, s, 0 , (sockaddr *)&cliaddr, &len_);
		}
		int	uptime() const {
			return time(NULL) - initTime;
		}
		int	fd() const {
			return fd_;
		}
		void cb() {}
	};// class UDPConn

	class selects
	{
	private:
		using pair_ = std::pair<int,std::function<void()>>;
		fd_set 	rdset_{};
		timeval tv{};
		std::vector<pair_> conns{};
		int fdMax{0};
	public:
		selects()= delete;
		selects(__time_t sec, __suseconds_t usec = 0):tv{sec,usec}
		{}
		void reset();
		void rFD_SET(int fd, std::function<void()> cb);
		void listenOnce();
		~selects(){}
	}; // class selects

	class selectws
	{
	private:
		using pair_ = std::pair<int,std::function<void()>>;
		fd_set 	rdset_{};
		timeval tv{};
		std::vector<pair_> conns{};
		int fdMax{0};
	public:
		selectws() = delete;
		selectws(int sec, int usec = 0):tv{sec,usec}
		{}
		void reset();
		void wFD_SET(int fd, std::function<void()> cb);
		void listenOnce();
		~selectws(){}
	}; // class selects


	class sslCliConn
	{
	private:
		SSL* ssl {nullptr};
		std::string buf_{};
	public:
		sslCliConn(const char* IP, const char* port, int rt, SSL_CTX *ctx);
		~sslCliConn();
		int send(const char* b, size_t s);
		int send(const std::string& s) { return send(s.c_str(), s.size());}
		int recv(char* b, size_t s);
		std::string& recvStr();
		int fd() const;
	};

	struct UnixSocketServelet
	{
		sockaddr_in cliaddr_{};
		socklen_t 	cliAddrLen_	{};
		std::string args_{};
		int fd_	{-1};
		UnixSocketServelet(int svrfd) {
			fd_ = accept(svrfd, (struct sockaddr *)&cliaddr_, &cliAddrLen_);
		}
		UnixSocketServelet()
		{}
		~UnixSocketServelet() {
			if (fd_ > 0) close(fd_);
		}

		int send(const std::string s)
		{
			return ::send(fd_, s.c_str(), s.size(), 0);
		}

		int recv(char* b, size_t s)
		{
			return ::recv(fd_ ,b, s, 0);
		}

		int fd() const
		{
			return fd_;
		}

		std::string& args()
		{
			return args_;
		}
		void args(std::string&& a) {
			args_ = move(a);
		}
	};
	using spUnixSocketServelet = std::shared_ptr<UnixSocketServelet>;
} // namespace remote
#endif
