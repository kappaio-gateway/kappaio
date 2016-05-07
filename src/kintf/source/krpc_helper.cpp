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
//#include "krpc_helper.h"
//#include <sys/select.h>
//#include <sys/types.h>
//#include "time.h"
//#include <net/if.h>
//#include <sys/socket.h>
//#include <sys/ioctl.h>
//#include <sys/types.h>
//#include <sys/select.h>
//#include <unistd.h>
//#include <netdb.h>
//#include "kutil.h"
//#include <syslog.h>
//#include <fcntl.h>
//#include <string.h>
//#include <errno.h>
//
//#define K_SERVER_URL "cloud.kapparock.com"
//#define K_TCP_PORT "32900"
//#define K_UDP_PORT "32901"
//
//int openConnection(const char *hostname, const char *port)
//{
//	struct addrinfo hints, *result;
//	memset(&hints, 0, sizeof(hints));
//	hints.ai_family = AF_INET;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_flags = AI_PASSIVE;
//
//	if (getaddrinfo(hostname, port, &hints, &result) < 0)
//	{
//		syslog(LOG_INFO,"getaddrinfo failed %s", strerror(errno));
//		return -1;
//	}
//
//	int socketfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
//	if (socketfd < 0)
//	{
//		freeaddrinfo(result);
//		syslog(LOG_INFO,"socket() open failed: %s", strerror(errno));
//		close(socketfd);
//		return -1;
//	}
//
//	int flags = fcntl(socketfd, F_GETFL, 0);
//	fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
//
//	fd_set fdset;
//	FD_ZERO(&fdset);
//	FD_SET(socketfd, &fdset);
//
//	struct timeval tv;
//	tv.tv_sec = 15;
//
//	int status = connect(socketfd,result->ai_addr,result->ai_addrlen);
//
//	int retVal;
//
//	do {
//		retVal = select(socketfd+1, NULL, &fdset, NULL, &tv);
//	} while (retVal < 0 && errno == EINTR && tv.tv_usec != 0 && tv.tv_sec!=0);
//
//	if (retVal == 1)
//	{
//		int so_error;
//		socklen_t slen = sizeof so_error;
//		getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &so_error, &slen);
//		if (so_error == 0)
//		{	freeaddrinfo(result);
//			return socketfd;
//		} else
//		{	freeaddrinfo(result);
//			syslog(LOG_INFO,"connection failed");
//			close(socketfd);
//			return -1;
//		}
//	} else {
//		freeaddrinfo(result);
//		syslog(LOG_INFO,"no socket, return from select() = %d", retVal);
//		close(socketfd);
//		return -1;
//	}
//}
//
//int newQueryConstructHeader(node_json *queryJSON, int command, int sequenceNumber)
//{
//	if (queryJSON == NULL)
//	{
//		return -1;
//	}
//
//	if (queryJSON->type != JSON_OBJECT)
//	{
//		return -1;
//	}
//	queryJSON->newInteger(RPC_HEADER_CMD_TAG,command);
//	queryJSON->newInteger(RPC_HEADER_SEQ_TAG,sequenceNumber);
//	queryJSON->newInteger(RPC_HEADER_TOD_TAG,time(NULL));
//	queryJSON->newObject(RPC_HEADER_DAT_TAG);
//
//	return 0;
//}
//
//int newQueryFormatFrame(node_json *queryJSON, int command, int sequenceNumber)
//{
//	if (queryJSON == NULL)
//	{
//		return -1;
//	}
//
//	if (queryJSON->type != JSON_OBJECT)
//	{
//		return -1;
//	}
//	queryJSON->newInteger(RPC_HEADER_CMD_TAG,command);
//	queryJSON->newInteger(RPC_HEADER_SEQ_TAG,sequenceNumber);
//	queryJSON->newInteger(RPC_HEADER_TOD_TAG,time(NULL));
//	queryJSON->newObject(RPC_HEADER_DAT_TAG)->setInteger(RPC_DAT_STATUS_TAG, -1);
//
//	return 0;
//}
//
//int waitOnSSLConnect(SSL *ssl, int timeoutSec)
//{
//	int status = SSL_connect(ssl);
//	int sslerr = 0;
//
//	if (status < 0)
//	{
//		sslerr = SSL_get_error(ssl, status);
//	}
//
//	if (status >= 0 || (sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE))
//	{
//		return status;
//	}
//
//	int timeoutuSec = timeoutSec * 1000000;
//	int timeuSecAcc = 0;
//	struct timespec tv = {0, 10000000};
//
//	while (status < 0 && timeuSecAcc < timeoutuSec)
//	{
//		status = SSL_connect(ssl);
//		timeuSecAcc += 10000;
//		kNanosleep(&tv);
//	}
//
//	if (status < 0)
//	{
//		sslerr = SSL_get_error(ssl, status);
//		syslog(LOG_INFO,"sslerr %d, timeAcc %d", sslerr, timeuSecAcc);
//	}
//	return status;
//}
//
//
//int waitOnSSLRead(SSL* ssl, void *buf, int num, int timeoutSec)
//{
//	int retVal, sslerr;
//	int socketfd = SSL_get_fd(ssl);
//	fd_set fdset;
//	FD_ZERO(&fdset);
//	FD_SET(socketfd, &fdset);
//	struct timeval timeout;
//	timeout.tv_sec = (__time_t)timeoutSec;
//
//	do {
//		retVal = select(socketfd+1, &fdset, NULL, NULL, &timeout);
//	} while (retVal < 0 && errno == EINTR && timeout.tv_usec != 0 && timeout.tv_sec!=0);
//
//	if (retVal == 1)
//	{
//		return SSL_read(ssl, buf, num);
//	} else
//	{
//		syslog(LOG_INFO, "not read, remaining %d sec %d usec, retVal %d, errno %d", timeout.tv_sec, timeout.tv_usec, retVal, errno);
//		return -1;
//	}
//}
//
//int waitOnSSLHandshake(SSL *ssl, int timeoutSec)
//{
//	int status = SSL_do_handshake(ssl);
//	int sslerr = 0;
//
//	if (status < 0)
//	{
//		sslerr = SSL_get_error(ssl, status);
//	}
//
//	if (status >= 0 || (sslerr != SSL_ERROR_WANT_READ && sslerr != SSL_ERROR_WANT_WRITE))
//	{
//		return status;
//	}
//
//	int timeoutuSec = timeoutSec * 1000000;
//	int timeuSecAcc = 0;
//	struct timespec tv = {0, 10000000};
//
//	while (status < 0 && timeuSecAcc < timeoutuSec)
//	{
//		status = SSL_do_handshake(ssl);
//		timeuSecAcc += 10000;
//		kNanosleep(&tv);
//	}
//
//	if (status < 0)
//	{
//		sslerr = SSL_get_error(ssl, status);
//		syslog(LOG_INFO,"sslerr %d, timeAcc %d", sslerr, timeuSecAcc);
//	}
//	return status;
//}
//
//SSL *sslOpenConnection(SSL_CTX *ctx)
//{
//	int server = openConnection(K_SERVER_URL, K_TCP_PORT);
//
//	if (server < 0)
//	{
//		close(server);
//		return NULL;
//	}
//
//	SSL *ssl = SSL_new(ctx);
//	SSL_set_fd(ssl,server);
//	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
//
//	int status;
//	if ((status = waitOnSSLConnect(ssl, 15)) < 0)
//	{
//		syslog(LOG_INFO, "giving up this connection...");
//		close(server);
//		SSL_free(ssl);
//		return NULL;
//	}
//
//	if(SSL_get_verify_result(ssl) != X509_V_OK)
//	{
//		syslog(LOG_INFO, "giving up this connection...");
//		close(server);
//		SSL_free(ssl);
//		return NULL;
//	}
//
//	if (waitOnSSLHandshake(ssl, 15)< 0)
//	{
//		syslog(LOG_INFO, "giving up this connection...");
//		close(server);
//		SSL_free(ssl);
//		return NULL;
//	}
//
//	return ssl;
//
//	sslOpenConnectionAbort:
//	close(server);
//	SSL_free(ssl);
//	return NULL;
//}
//
//int sslCloseConnection(SSL *ssl)
//{
//	if (ssl != NULL)
//	{
//		close(SSL_get_fd(ssl));
//		SSL_free(ssl);
//	}
//
//	return 0;
//}
