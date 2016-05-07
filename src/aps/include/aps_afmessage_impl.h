//====================================================================================
// The MIT License (MIT)
//
// Copyright (c) 2011 Kapparock LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//====================================================================================
#ifndef APS_AFMESSAGE_IMPL_H_
#define APS_AFMESSAGE_IMPL_H_

#include "kzdef.h"
namespace aps {

class kr_afmessage_hide: public AFMessage {
protected:
	uint8_t data[255] { };
	static constexpr int dstAddrIdx = 0;
	static constexpr uint8_t dstEpIdx = dstAddrIdx + sizeof(uint16_t);
	static constexpr uint8_t srcEpIdx = dstEpIdx + sizeof(uint8_t);
	static constexpr uint8_t clusIdx = srcEpIdx + sizeof(uint8_t);
	static constexpr uint8_t transIdx = clusIdx + sizeof(uint16_t);
	static constexpr uint8_t optnsIdx = transIdx + sizeof(uint8_t);
	static constexpr uint8_t radiuIdx = optnsIdx + sizeof(uint8_t);
	static constexpr uint8_t asduLenIdx = radiuIdx + sizeof(uint8_t);
	static constexpr uint8_t asduIdx = asduLenIdx + sizeof(uint8_t);
public:
	~kr_afmessage_hide() {
	}
	uint16_t dstAddr() const override;
	void dstAddr(uint16_t i) override;
	uint8_t dstEndpoint() const override;
	void dstEndpoint(uint8_t i) override;
	uint16_t groupId() const override;
	void groupId(uint16_t i) override;
	uint16_t clusterId() const override;
	void clusterId(uint16_t i) override;
	uint8_t srcEndpoint() const override;
	void srcEndpoint(uint8_t i) override;
	uint16_t srcAddr() const override;
	void srcAddr(uint16_t i) override;
	uint8_t wasBroadCast() const override;
	void wasBroadCast(uint8_t i) override;
	uint8_t linkQuality() const override;
	void linkQuality(uint8_t i) override;
	uint8_t securityUse() const override;
	void securityUse(uint8_t i) override;
	uint32_t timeStamp() const override;
	void timeStamp(uint32_t i) override;
	uint8_t tranSeqNum() const override;
	void tranSeqNum(uint8_t i) override;
	uint8_t asduLen() const override;
	void asduLen(uint8_t i) override;
	uint8_t* asdu() override;
	uint8_t radius() const override;
	void radius(uint8_t i) override;
	uint8_t options() const override;
	void options(uint8_t i) override;
	uint8_t dstAddrMode() const override;
	void dstAddrMode(uint8_t i) override;
};

using kr_afmessage = kr_afmessage_hide;

class kr_afmessage_EXT_hide: public AFMessage_EXT {
public:
	uint8_t data[255] { };
	static constexpr uint8_t dstAddrMdIdx = 0;
	static constexpr uint8_t dstAddrIdx = dstAddrMdIdx + sizeof(uint8_t); // 1
	static constexpr uint8_t dstEpIdx = dstAddrIdx + sizeof(uint64_t);	// 9
	static constexpr uint8_t dstPanIdIdx = dstEpIdx + sizeof(uint8_t);	// 10
	static constexpr uint8_t srcEpIdx = dstPanIdIdx + sizeof(uint16_t);	// 12
	static constexpr uint8_t clusIdx = srcEpIdx + sizeof(uint8_t); // 13
	static constexpr uint8_t transIdx = clusIdx + sizeof(uint16_t);	// 15
	static constexpr uint8_t optnsIdx = transIdx + sizeof(uint8_t);	//	16
	static constexpr uint8_t radiuIdx = optnsIdx + sizeof(uint8_t);	//	17
	static constexpr uint8_t asduLenIdx = radiuIdx + sizeof(uint8_t); // 18
	static constexpr uint8_t asduIdx = asduLenIdx + sizeof(uint16_t); // 20
public:
	~kr_afmessage_EXT_hide() {
	}
	uint64_t dstAddr() const override; // {return proxyRd<uint64_t>(data+dstAddrIdx);}
	void dstAddr(uint64_t i) override; //	{ hostIntToANSI(data+dstAddrIdx, i);}
	uint8_t dstEndpoint() const override; // 	{ return data[dstEpIdx];}
	void dstEndpoint(uint8_t i) override; // 	{	data[dstEpIdx] = i; }
	uint16_t groupId() const override; // 	{	return 0;}
	void groupId(uint16_t i) override; // 	{ }
	uint16_t clusterId() const override; //	{	return proxyRd<uint16_t>(data+clusIdx);}
	void clusterId(uint16_t i) override; // 	{	hostIntToANSI(data+clusIdx, i);}
	uint8_t srcEndpoint() const override; // 	{ return data[srcEpIdx];}
	void srcEndpoint(uint8_t i) override; // 	{	data[srcEpIdx] = i;}
	uint64_t srcAddr() const override; // 	{ return 0;}
	void srcAddr(uint64_t i) override; // 	{ }
	uint8_t wasBroadCast() const override; //	{ return 0;}
	void wasBroadCast(uint8_t i) override; // 	{ }
	uint8_t linkQuality() const override; //	{ return 0;}
	void linkQuality(uint8_t i) override; //	{ }
	uint8_t securityUse() const override; // 	{	return 0;}
	void securityUse(uint8_t i) override; // 	{	}
	uint32_t timeStamp() const override; // 	{	return 0;}
	void timeStamp(uint32_t i) override; //	{	}
	uint8_t tranSeqNum() const override; // 	{	return data[transIdx];}
	void tranSeqNum(uint8_t i) override; // 	{	data[transIdx] = i;}
	uint16_t asduLen() const override; // 	{	return proxyRd<uint16_t>(data+asduLenIdx);}
	void asduLen(uint16_t i) override; //	{	hostIntToANSI(data+asduLenIdx, i);}
	uint8_t* asdu() override; //	{	return data+asduIdx;}
	uint8_t radius() const override; //	{	return data[radiuIdx];}
	void radius(uint8_t i) override; //	{	data[radiuIdx] = i;}
	uint8_t options() const override; // 	{	return data[optnsIdx];}
	void options(uint8_t i) override; // 	{	data[optnsIdx] = i;}
	uint8_t addrMode() const override; //	{	return data[dstAddrMdIdx];}
	void addrMode(uint8_t i) override; //	{	data[dstAddrMdIdx] = i;}
	uint16_t srcPanId() const override; //	{	return 0;}
	void srcPanId(uint16_t i) override; //	{ }
	uint16_t dstPanId() const override; //{	return proxyRd<uint16_t>(data+dstPanIdIdx);}
	void dstPanId(uint16_t i) override; //		{hostIntToANSI(data+dstPanIdIdx, i);}
	void loadStr(const uint8_t* in, size_t len); //	{memcpy(data,in, len);}
};
}
#endif
