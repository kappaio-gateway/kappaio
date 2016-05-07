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
#ifndef MTAF_IMPL_H_
#define MTAF_IMPL_H_

//this section is not meant to be directly included in usercode

namespace {
class mt_afmessage : public AFMessage
{
private:
	uint8_t data[255];
	static constexpr uint8_t groupIdIdx = 0;
	static constexpr uint8_t clusIdx = groupIdIdx + sizeof(uint16_t); // 	2
	static constexpr uint8_t srcAddrIdx = clusIdx + sizeof(uint16_t); // 	4
	static constexpr uint8_t srcEpIdx = srcAddrIdx + sizeof(uint16_t); // 	6
	static constexpr uint8_t dstEpIdx = srcEpIdx + sizeof(uint8_t); 	// 	7
	static constexpr uint8_t wasBdctIdx = dstEpIdx + sizeof(uint8_t); 	//	8
	static constexpr uint8_t lnkQltIdx = wasBdctIdx + sizeof(uint8_t); 	//	9
	static constexpr uint8_t secUseIdx = lnkQltIdx + sizeof(uint8_t); 	//	10
	static constexpr uint8_t tmStmpIdx = secUseIdx + sizeof(uint8_t); 	// 	11
	static constexpr uint8_t transIdx = tmStmpIdx + sizeof(uint32_t);	// 	15
	static constexpr uint8_t asduLenIdx = transIdx + sizeof(uint8_t); 	// 	16
	static constexpr uint8_t asduIdx = asduLenIdx + sizeof(uint8_t); 	// 	16 + asduLen
	template<typename T>
	T proxyRd(const uint8_t * in) const { T out; ANSITohostInt(out, const_cast<uint8_t *>(in)); return out;}
public:
	~mt_afmessage() {}
	uint16_t dstAddr() const override { return 0;}
	void dstAddr(uint16_t i) override { }
	uint8_t dstEndpoint() const override { return data[dstEpIdx];}
	void dstEndpoint(uint8_t i) override { data[dstEpIdx] = i; }
	uint16_t groupId() const override { return data[groupIdIdx];}
	void groupId(uint16_t i) override { data[groupIdIdx] = i;}
	uint16_t clusterId() const override { return proxyRd<uint16_t>(data+clusIdx);}
	void clusterId(uint16_t i) override { hostIntToANSI(data+clusIdx, i);}
	uint8_t srcEndpoint() const override { return data[srcEpIdx];}
	void srcEndpoint(uint8_t i) override { data[srcEpIdx] = i;}
	uint16_t srcAddr() const override { return proxyRd<uint16_t>(data+srcAddrIdx);}
	void srcAddr(uint16_t i ) override { hostIntToANSI(data+srcAddrIdx, i);}
	uint8_t wasBroadCast() const override { return 0;}
	void wasBroadCast(uint8_t i) override { }
	uint8_t linkQuality() const override { return data[lnkQltIdx];}
	void linkQuality(uint8_t i) override { }
	uint8_t securityUse() const override { return 0;}
	void securityUse(uint8_t i) override { }
	uint32_t timeStamp() const override { return proxyRd<uint32_t>(data+tmStmpIdx);}
	void timeStamp(uint32_t i) override { hostIntToANSI(data+tmStmpIdx, i);}
	uint8_t	tranSeqNum() const override { return data[transIdx];}
	void tranSeqNum(uint8_t i) override	{ data[transIdx] = i;}
	uint8_t asduLen() const override { return data[asduLenIdx];}
	void asduLen(uint8_t i)	override { data[asduLenIdx] = i;}
	uint8_t* asdu() override { return data+asduIdx;}
	uint8_t radius() const override { return 0;}
	void radius(uint8_t i) override	{ }
	uint8_t options() const override { return 0;}
	void options(uint8_t i)	override { }
	uint8_t dstAddrMode() const override { return 0x02;}
	void dstAddrMode(uint8_t i) override { }
	void loadStr(const uint8_t* in, size_t len)	{memcpy(data,in, len);}
};
//=====================================================================================
//=====================================================================================
//=====================================================================================
//=====================================================================================
class mt_afmessage_EXT : public AFMessage_EXT
{
private:
	uint8_t data[255];
	static constexpr uint8_t groupIdIdx = 0;
	static constexpr uint8_t clusIdx = groupIdIdx + sizeof(uint16_t); // 	2
	static constexpr uint8_t scrAddrMdIdx = clusIdx + sizeof(uint16_t);	//	4
	static constexpr uint8_t srcAddrIdx = scrAddrMdIdx+ sizeof(uint8_t); 	// 	5
	static constexpr uint8_t srcEpIdx = srcAddrIdx + sizeof(uint64_t); // 	13
	static constexpr uint8_t srcPanIdIdx = srcEpIdx + sizeof(uint8_t);	//	14
	static constexpr uint8_t dstEpIdx = srcPanIdIdx + sizeof(uint16_t); // 	16
	static constexpr uint8_t wasBdctIdx = dstEpIdx + sizeof(uint8_t); 	//	17
	static constexpr uint8_t lnkQltIdx = wasBdctIdx + sizeof(uint8_t); 	//	18
	static constexpr uint8_t secUseIdx = lnkQltIdx + sizeof(uint8_t); 	//	19
	static constexpr uint8_t tmStmpIdx = secUseIdx + sizeof(uint8_t); 	// 	20
	static constexpr uint8_t transIdx = tmStmpIdx + sizeof(uint32_t);	// 	24
	static constexpr uint8_t asduLenIdx = transIdx + sizeof(uint16_t); //	26
	static constexpr uint8_t asduIdx = asduLenIdx + sizeof(uint8_t); 	// 	27 + asduLen
	template<typename T>
	T proxyRd(const uint8_t * in) const { T out; ANSITohostInt(out, const_cast<uint8_t *>(in)); return out;}
public:
	~mt_afmessage_EXT()	{}
	uint64_t dstAddr() const override { return 0;}
	void dstAddr(uint64_t i) override { }
	uint8_t dstEndpoint() const override { return data[dstEpIdx];}
	void dstEndpoint(uint8_t i) override { data[dstEpIdx] = i; }
	uint16_t groupId() const override { return data[groupIdIdx];}
	void groupId(uint16_t i) override { data[groupIdIdx] = i;}
	uint16_t clusterId() const override { return proxyRd<uint16_t>(data+clusIdx);}
	void clusterId(uint16_t i) override { hostIntToANSI(data+clusIdx, i);}
	uint8_t srcEndpoint() const override { return data[srcEpIdx];}
	void srcEndpoint(uint8_t i) override { data[srcEpIdx] = i;}
	uint64_t srcAddr() const override { return proxyRd<uint64_t>(data+srcAddrIdx);}
	void srcAddr(uint64_t i ) override { hostIntToANSI(data+srcAddrIdx, i);}
	uint8_t wasBroadCast() const override { return 0;}
	void wasBroadCast(uint8_t i) override { }
	//uint8_t 	linkQuality()		const 		override 	{ return 0;}
	uint8_t linkQuality() const override { return data[lnkQltIdx]; }
	void linkQuality(uint8_t i) override { }
	uint8_t securityUse() const override { return 0;}
	void securityUse(uint8_t i)	override { }
	uint32_t timeStamp() const override { return proxyRd<uint32_t>(data+tmStmpIdx);}
	void timeStamp(uint32_t i) override { hostIntToANSI(data+tmStmpIdx, i);}
	uint8_t	tranSeqNum() const override { return data[transIdx];}
	void tranSeqNum(uint8_t i) override { data[transIdx] = i;}
	uint16_t asduLen() const override { return proxyRd<uint16_t>(data+asduLenIdx);}
	void asduLen(uint16_t i) override { hostIntToANSI(data+asduLenIdx, i);}
	uint8_t* asdu() override { return data+asduIdx;}
	uint8_t radius() const override { return 0;}
	void radius(uint8_t i) override	{ }
	uint8_t options() const override { return 0;}
	void options(uint8_t i) override { }
	uint8_t addrMode() const override { return data[scrAddrMdIdx];}
	void addrMode(uint8_t i) override { data[scrAddrMdIdx] = i;}
	uint16_t srcPanId()	const override { return proxyRd<uint16_t>(data+srcPanIdIdx);}
	void srcPanId(uint16_t i) override { hostIntToANSI(data+srcPanIdIdx, i);}
	uint16_t dstPanId() const override { return 0;}
	void dstPanId(uint16_t i) override {}
	void loadStr(const uint8_t* in, size_t len)	{memcpy(data,in, len);}
};
}
#endif
