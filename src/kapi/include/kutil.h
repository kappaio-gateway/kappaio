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
#ifndef KUTIL_H
#define KUTIL_H
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <semaphore.h>
#include <string.h>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////
// for backward compatibility only
#define _BYTES_TO_INT_(a,b,c) 			kMemToInt((void*)&a,(void*)&b,c)
#define _MEM_TO_HEXSTR_(a,b,c) 			memToHexStr((char*)a,(void*)b,(size_t)c)
#define _INT_TO_BYTES_(a,b,c) 			kIntToMem((void*)&a,(void*)&b,c)
#define memToHexStr(textBuffer, mem, memSize) kPrintMem(textBuffer, mem, memSize)
//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////
// for backward compatibility only
int bytes_to_uint(void* destination, void* source, size_t size);
//////////////////////////////////////////////////////////////////////
int kNanosleep(struct timespec *interval);
int kSleep(size_t interval);
int kPrintMem(char *buffer, void *mem, size_t memLen);
int kMemToInt(void* destination, void* source, size_t size);
int kIntToMem(void* destination, void* source, size_t size);
// converts valid ASCII hex string to an integer of 8*size bits
int kStrToInt(void* destination, const char* source, size_t destination_size);
// converts an integer of 8*size bits to an ASCII hex string
int kIntToStr(char* destination, const void* source, size_t size);
// temporarily API to wraps kIntToStr for functions that need char* as return
char *kToHexStr(char* destination, const void* source, size_t size);
uint8_t kCharToInt(char c);
void *memcpyr(void * destination, const void * source, size_t num );
void gen_random(char *s, const int len);
int EncodeDir(const char *fileName, char **pbuffer); // tar+gzip the dir "fileName" and base64 encode, pbuffer must be free after use
int DecodeToDir(const char *dir, char *buffer);
int kByteToHexString(char *outputString, uint8_t *byte, size_t byteLength);
int kHexStringToByte(uint8_t *outputByte, const char *hexString, size_t stringLength);
int openUnixServerSock(const char *name, const char *service);
int openUnixClientSock(const char *name, const char *service);
//int sockAcceptWait(int socketfd, int timeoutSec);
//int sendLocalUnixRequest(const char *input, size_t inputLen, char *output, size_t maxOutputLen, const char *socketPath);
//int recvWait(int socketfd, void* buffer, size_t expectLen, int flags, int timeoutSec );

#ifdef __cplusplus
}
#endif
//===========================================================================
template<typename T>
std::string IntToHexStr(T t)
{
	char temp[sizeof(T) << 1 + 1];
	kIntToStr(temp, static_cast<const void*>(&t), sizeof(T));
	return std::string(temp);
}
//===========================================================================
template<typename T>
int hostIntToANSI(uint8_t* buf, T in, size_t bufLen)
{
	if (sizeof(T) > bufLen) return 0;
	memcpy((void*)buf, (void*)&in, sizeof(T));
	return sizeof(T);
}
//===========================================================================
template<typename T>
int hostIntToANSI(uint8_t* buf, T in)
{
	memcpy((void*)buf, (void*)&in, sizeof(T));
	return sizeof(T);
}
//===========================================================================
template<typename T>
int ANSITohostInt(T& in, uint8_t* buf, size_t bufLen)
{
	if (sizeof(T) > bufLen) return 0;
	memcpy((void*)&in, (void*)buf, sizeof(T));
	return sizeof(T);
}
//===========================================================================
template<typename T>
int ANSITohostInt(T& in, uint8_t* buf)
{
	memcpy((void*)&in, (void*)buf, sizeof(T));
	return sizeof(T);
}
//===========================================================================

inline std::vector<uint8_t> hexStrToByteVec(const std::string& str)
{
	std::vector<uint8_t> v;
	size_t asrclen = str.size();
	if (asrclen & 1) {
		return v;
	}
	const char * psrc = str.c_str();
	while (asrclen--) {
		v.push_back(kCharToInt(*psrc++) << 4 | kCharToInt(*psrc++));
	}
	return v;
}
//===========================================================================
class klock_c
{
private:
	sem_t *shLockPriv;
	sem_t *shLock;
	int shLockCnt;
	sem_t *exLock;
public:
	klock_c();
	~klock_c();
	int lockAcquire();
	int lockRelease();
	int shLockAcquire();
	int shLockRelease();
	int exLockAcquire();
	int exLockRelease();
};

#endif
