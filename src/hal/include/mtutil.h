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
#ifndef MTUTIL_H
#define MTUTIL_H
#include <stdint.h>
#include "mtproc.h"
#include "MT_RPC.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace MT
{
	namespace UTIL
	{
		typedef struct
		{
			uint8_t status;
			uint64_t IEEEAddr;
			uint32_t ScanChannels;
			uint16_t PanId;
			uint8_t SecurityLevel;
			uint8_t PreConfigKey[16];
		} NvInfo_t;
		int GetNvInfo(NvInfo_t& nvInfo);
		int SetChannels(uint32_t channels);
	}
}

uint8_t MT_UtilCommandProcessing(uint8_t* pBuf);
uint8_t UTIL_ASSOC_GET_WITH_ADDRESS_Req(uint64_t IEEEAddr, uint16_t NwkAddr);
uint8_t UTIL_ASSOC_GET_WITH_ADDRESS_Rsp_Proc(uint8_t* pBuf);
uint8_t UTIL_ADDRMGR_EXT_ADDR_LOOKUP_Rsp_Proc(uint8_t *pBuf);
int mt_util_get_device_info(uint8_t *res);
int mtUtilAssocGetWithAddress(uint64_t extAddr, void *returnBuffer);
int mtUtilGetAssocCount(uint16_t *returnCount);
int mtUtilAssocFindDevice(uint8_t *returnBuffer, uint8_t index);
int mtUtilGetDeviceInfo(uint8_t *returnBuffer, size_t bufferSize);
int mtUtilAddrmgrNwkAddrLookup(uint16_t nwkAddr, uint64_t *returnBuffer);


//void set_default_channel();
#ifdef __cplusplus
}
#endif

#endif
