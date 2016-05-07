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
#include "kzdef.h"
#include <stdint.h>
#include <semaphore.h>
#include "mtaf.h"
#include "mtutil.h"
#include "mtsys.h"
#ifndef HAL_H
#define HAL_H

namespace HAL_STRUCTURE
{
	typedef struct
	{
		uint8_t  txCounter;    // Counter of transmission success/failures
		uint8_t txCost;       // Average of sending rssi values if link staus is enabled
	                       // i.e. NWK_LINK_STATUS_PERIOD is defined as non zero
		uint8_t  rxLqi;        // average of received rssi values
	                       // needs to be converted to link cost (1-7) before used
		uint8_t  inKeySeqNum;  // security key sequence number
		uint32_t inFrmCntr;    // security frame counter..
		uint16_t txFailure;    // higher values indicate more failures
	} linkInfo_t;

	typedef struct
	{
		uint16_t shortAddr;                 // Short address of associated device
		uint16_t addrIdx;                   // Index from the address manager
		uint8_t nodeRelation;
		uint8_t devStatus;                   // bitmap of various status values
		uint8_t assocCnt;
		uint8_t age;
	  linkInfo_t linkInfo;
	} associated_devices_t;
}

typedef struct
{
	uint8_t status;
	uint64_t IEEEAddr;
	uint16_t NwkAddr;
	uint8_t DeviceType;
	uint8_t DeviceState;
	uint8_t NumAssocDevices;
} mt_device_info_t;

typedef struct
{
	uint16_t capability;
	uint8_t operationState;
	uint8_t info;
} processorStatus_t;

void hal_init();
int halSetAPSMessageHandler(int (*fn)(AFMessage*));
uint64_t get_ieee_id();
uint16_t get_nwk_id();
int system_reset();
int ProcessorReset();
int StartSystem();
int system_ping(uint8_t*result);
int systemPing(processorStatus_t *status);
int cc2530_wdt_kick();
int start_zdo();
int getAssocCount();
int getAssocInfo(HAL_STRUCTURE::associated_devices_t *_dev, uint8_t index);
int getDeviceInfo(mt_device_info_t *deviceInfo);
uint16_t getAssocNwkAddr(uint64_t extAddr);
uint64_t getAssocExtAddr(uint16_t nwkAddr);
int sendAPSMessage(uint16_t dstAddr, uint8_t dstEndpoint, uint16_t clusterId,
					uint8_t srcEndpoint, uint8_t seqNum, uint8_t txOption, uint8_t radius,
					uint8_t *asdu, uint8_t asduLength);
int sendAPSMessageStruct(AFMessage *		message);
int sendAPSMessageStruct(AFMessage_EXT *message);
void registerEndpoint(uint8_t endpoint_id, uint16_t profile_id,uint16_t device_id, uint8_t device_ver);
void registerInterPanCB(uint8_t ep);
//void KickOffWatchDog();
void StartSystemMonitor();

namespace HAL
{
	using NvInfo_t = MT::UTIL::NvInfo_t;
	inline int getNvInfo(NvInfo_t &nvInfo)
	{
		return MT::UTIL::GetNvInfo(nvInfo);
	}

	using NvItem_t = MT::SYS::NvItem_t;
	inline int OsalNvRead(NvItem_t &nvItem)
	{
		return MT::SYS::OsalNvRead(nvItem);
	}
	inline int OsalNvWrite(NvItem_t &nvItem)
	{
		return MT::SYS::OsalNvWrite(nvItem);
	}
	int getChannel(uint8 &channel);
//	int setChannel(uint8 channel);
	int getNwkUpdateId(uint8_t &nwkUpdateId);
	int setNwkUpdateId(uint8_t nwkUpdateId);
	int zcdStartupOptions(uint8_t options);
	int delayRestart(int sec);

}
#endif
