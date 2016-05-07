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
#ifndef ZDO_H
#define ZDO_H
#include "apsdb.h"
#include "kglobal.h"
#include "zdo_archive.h"

void startZDO();
aps::Device *deviceQuery(uint16_t shortAddr);
int	refreshDevice(uint16_t shortAddr);

namespace ZDO
{
	using namespace aps;
	using namespace data_service;
	struct zdpPred
	{
		uint8_t zdpCount;
		uint16_t cId_;
		zdpPred( AFMessage& o):zdpCount{o.asdu()[0]}, cId_{0x8000 | o.clusterId()}
		{}
		~zdpPred()
		{};
		bool operator()( AFMessage& i )
		{
			return ((i.asdu()[0] == zdpCount) && (i.clusterId() == cId_) && (i.dstEndpoint() == 0));
		}
	};

	class APDU : public kr_afmessage
	{
		private:
			uint8_t* asdu_{kr_afmessage::asdu()};
			uint8_t& asduLen_{kr_afmessage::data[asduLenIdx]};
		public:
			~APDU() {}
			APDU() = delete;
			APDU(uint16_t nwkaddr, uint16_t cId)
			{
				kr_afmessage::clusterId(cId);
				kr_afmessage::dstAddr(nwkaddr);
				kr_afmessage::dstEndpoint(0);
				kr_afmessage::srcEndpoint(0);
				kr_afmessage::options(0);
				kr_afmessage::tranSeqNum(0);
				kr_afmessage::radius(3);
				kr_afmessage::asduLen(0);
			}
			APDU(uint16_t nwkaddr, uint16_t cId, std::function<int(uint8_t*)> zclFrame):APDU(nwkaddr, cId)
			{
				asduLen_ += zclFrame(asdu_+asduLen_);
			}
			template <typename F>
			void send(F cb, bool block=true)
			{
				data_service::request(*this, zdpPred(*this), cb);
			}
	};

	struct Mgmt_NWK_Update_req_frm
	{
		uint32_t ScanChannels_;
		uint8_t ScanDuration_;
		uint8_t ScanCount_;
		uint8_t nwkUpdateId_;
		uint16_t nwkManagerAddr_;
		uint16_t clusterId_;
		Mgmt_NWK_Update_req_frm(uint32_t ScanChannels, uint8_t ScanDuration, uint8_t ScanCount,	uint8_t nwkUpdateId, uint16_t nwkManagerAddr)
		:ScanChannels_{ScanChannels},
		 ScanDuration_{ScanDuration},
		 ScanCount_{ScanCount},
		 nwkUpdateId_{nwkUpdateId},
		 nwkManagerAddr_{nwkManagerAddr},
		 clusterId_{0x0038}
		{}
		int operator()(uint8_t* buf)
		{
			int i = 0;
			buf[i++] = cntr();
			i += hostIntToANSI(buf+i, ScanChannels_);
			buf[i++] = ScanDuration_;
			if (ScanDuration_ >= 0 && ScanDuration_ <= 5)
			{
				buf[i++] = ScanCount_;
			}
			if (ScanDuration_ == 0xfe || ScanDuration_ == 0xff)
			{
				buf[i++] = nwkUpdateId_;
			}
			if (ScanDuration_ == 0xff)
			{
				i += hostIntToANSI(buf+i, nwkManagerAddr_);
			}
			return i;
		}
		uint16_t clusteId() const {
			return clusterId_;
		}
	};
}
#endif
