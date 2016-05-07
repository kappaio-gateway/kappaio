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
#include "aps_afmessage_impl.h"
#include "kutil.h"
namespace aps
{
	template<typename T>
	T proxyRd(const uint8_t * in) { T out; ANSITohostInt(out, const_cast<uint8_t *>(in)); return out;}
	uint16_t kr_afmessage_hide::dstAddr() const { return proxyRd<uint16_t>(data+dstAddrIdx);}
	void kr_afmessage_hide::dstAddr(uint16_t i)	{ hostIntToANSI(data+dstAddrIdx, i);}
	uint8_t kr_afmessage_hide::dstEndpoint() const { return data[dstEpIdx];}
	void kr_afmessage_hide::dstEndpoint(uint8_t i) { data[dstEpIdx] = i; }
	uint16_t kr_afmessage_hide::groupId() const	{return 0;}
	void kr_afmessage_hide::groupId(uint16_t i)	{ }
	uint16_t kr_afmessage_hide::clusterId()	const { return proxyRd<uint16_t>(data+clusIdx);}
	void kr_afmessage_hide::clusterId(uint16_t i) { hostIntToANSI(data+clusIdx, i);}
	uint8_t	kr_afmessage_hide::srcEndpoint() const { return data[srcEpIdx];}
	void kr_afmessage_hide::srcEndpoint(uint8_t i) { data[srcEpIdx] = i;}
	uint16_t kr_afmessage_hide::srcAddr() const { return 0;}
	void kr_afmessage_hide::srcAddr(uint16_t i ) { }
	uint8_t kr_afmessage_hide::wasBroadCast() const { return 0;}
	void kr_afmessage_hide::wasBroadCast(uint8_t i) { }
	uint8_t kr_afmessage_hide::kr_afmessage_hide::linkQuality() const { return 0;}
	void kr_afmessage_hide::linkQuality(uint8_t i) { }
	uint8_t	kr_afmessage_hide::securityUse() const { return 0;}
	void kr_afmessage_hide::securityUse(uint8_t i) { }
	uint32_t kr_afmessage_hide::timeStamp()	const { return 0;}
	void kr_afmessage_hide::timeStamp(uint32_t i) { }
	uint8_t	kr_afmessage_hide::kr_afmessage_hide::tranSeqNum() const { return data[transIdx];}
	void kr_afmessage_hide::tranSeqNum(uint8_t i) { data[transIdx] = i;}
	uint8_t kr_afmessage_hide::kr_afmessage_hide::asduLen() const { return data[asduLenIdx];}
	void kr_afmessage_hide::asduLen(uint8_t i) { data[asduLenIdx] = i;}
	uint8_t* kr_afmessage_hide::asdu() { return data+asduIdx;}
	uint8_t kr_afmessage_hide::radius() const { return data[radiuIdx];}
	void kr_afmessage_hide::radius(uint8_t i) { data[radiuIdx] = i;}
	uint8_t kr_afmessage_hide::options() const { return data[optnsIdx];}
	void kr_afmessage_hide::options(uint8_t i) { data[optnsIdx] = i;}
	uint8_t	kr_afmessage_hide::dstAddrMode() const { return 0x02;}
	void kr_afmessage_hide::dstAddrMode(uint8_t i) {}
	//=====================================================================================
	//=====================================================================================
	//=====================================================================================
	//=====================================================================================
	uint64_t kr_afmessage_EXT_hide::dstAddr() const	{ return proxyRd<uint64_t>(data+dstAddrIdx); }
	void kr_afmessage_EXT_hide::dstAddr(uint64_t i) { hostIntToANSI(data+dstAddrIdx, i);}
	uint8_t	kr_afmessage_EXT_hide::dstEndpoint() const { return data[dstEpIdx];}
	void kr_afmessage_EXT_hide::dstEndpoint(uint8_t i) { data[dstEpIdx] = i; }
	uint16_t kr_afmessage_EXT_hide::groupId() const	{return 0;}
	void kr_afmessage_EXT_hide::groupId(uint16_t i)	{ }
	uint16_t kr_afmessage_EXT_hide::clusterId() const { return proxyRd<uint16_t>(data+clusIdx);}
	void kr_afmessage_EXT_hide::clusterId(uint16_t i) { hostIntToANSI(data+clusIdx, i);}
	uint8_t kr_afmessage_EXT_hide::srcEndpoint() const { return data[srcEpIdx];}
	void kr_afmessage_EXT_hide::srcEndpoint(uint8_t i) { data[srcEpIdx] = i;}
	uint64_t kr_afmessage_EXT_hide::srcAddr() const { return 0;}
	void kr_afmessage_EXT_hide::srcAddr(uint64_t i ) { }
	uint8_t kr_afmessage_EXT_hide::wasBroadCast() const	{ return 0;}
	void kr_afmessage_EXT_hide::wasBroadCast(uint8_t i)	{ }
	uint8_t kr_afmessage_EXT_hide::linkQuality() const { return 0;}
	void kr_afmessage_EXT_hide::linkQuality(uint8_t i) { }
	uint8_t kr_afmessage_EXT_hide::securityUse() const { return 0;}
	void kr_afmessage_EXT_hide::securityUse(uint8_t i) { }
	uint32_t kr_afmessage_EXT_hide::timeStamp()	const { return 0;}
	void kr_afmessage_EXT_hide::timeStamp(uint32_t i) { }
	uint8_t kr_afmessage_EXT_hide::tranSeqNum()	const { return data[transIdx];}
	void kr_afmessage_EXT_hide::tranSeqNum(uint8_t i) { data[transIdx] = i;}
	uint16_t kr_afmessage_EXT_hide::asduLen() const { return proxyRd<uint16_t>(data+asduLenIdx);}
	void kr_afmessage_EXT_hide::asduLen(uint16_t i)	{ hostIntToANSI(data+asduLenIdx, i);}
	uint8_t* kr_afmessage_EXT_hide::asdu() {return data+asduIdx;}
	uint8_t kr_afmessage_EXT_hide::radius() const { return data[radiuIdx];}
	void kr_afmessage_EXT_hide::radius(uint8_t i) { data[radiuIdx] = i;}
	uint8_t kr_afmessage_EXT_hide::options() const { return data[optnsIdx];}
	void kr_afmessage_EXT_hide::options(uint8_t i) { data[optnsIdx] = i;}
	uint8_t kr_afmessage_EXT_hide::addrMode() const { return data[dstAddrMdIdx];}
	void kr_afmessage_EXT_hide::addrMode(uint8_t i)	{ data[dstAddrMdIdx] = i;}
	uint16_t kr_afmessage_EXT_hide::srcPanId() const { return 0;}
	void kr_afmessage_EXT_hide::srcPanId(uint16_t i){ }
	uint16_t kr_afmessage_EXT_hide::dstPanId() const { return proxyRd<uint16_t>(data+dstPanIdIdx);}
	void kr_afmessage_EXT_hide::dstPanId(uint16_t i) { hostIntToANSI(data+dstPanIdIdx, i);}
	void kr_afmessage_EXT_hide::loadStr(const uint8_t* in, size_t len)	{memcpy(data,in, len);}
}
