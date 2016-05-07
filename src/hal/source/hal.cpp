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
#include "hal.h"
#include "kglobal.h"
#include "kutil.h"
#include <string.h>
#include <pthread.h>
#include "mtsys.h"
#include "mtproc.h"
#include "mtutil.h"
#include "mtzdo.h"
#include "mtaf.h"
#include "mtdebug.h"
#include "mt_helper.h"
#include "mtintf.h"
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define RSSERIAL_LAST_FAILED_PING "/rstmp/rsserial-last-failed-ping"
#define MINIUM_FAILED_PING_WAIT_SEC 40

namespace halPrivate
{
	int consecutiveFailedPing = 0;
}

namespace
{
	void *WatchDogThread(void *td)
	{
		timespec interval = {0,250000000}; //250ms
		while (1)
		{
			//if (cnt++ < 4) {
			cc2530_wdt_kick();
			kNanosleep(&interval);
			//}
		}
		return nullptr;
	}
	void *StateMonitor(void *td)
	{
		processorStatus_t status;
		//LOG_MESSAGE("StateMonitor is in test mode which poll the system every 500ms instead of the normal rate of once every 5 sec");
		//timespec interval = {0,500000000};
		kSleep(1);
		while(1)
		{
//			kSleep(5);
			systemPing(&status);
			if (status.capability != 0x0059)
			{
				LOG_MESSAGE("systemPing state: %04x, exiting...", status.capability);
				//kSleep(2);
				exit(0);
			}
			kSleep(5);
			//kNanosleep(&interval);
		}
		return nullptr;
	}
}

void StartSystemMonitor()
{
	pthread_t thread;

	syslog(LOG_INFO, "Starting Watch-Dog Timer");
	pthread_create(&thread, NULL, WatchDogThread, (void *)NULL);
	pthread_detach(thread);

	syslog(LOG_INFO, "Starting State Monitor");
	pthread_create(&thread, NULL, StateMonitor, (void *)NULL);
	pthread_detach(thread);

	return;
}

void hal_init()
{
	//set_default_channel();
	mtHelperInit();
	MTProcInit();
//	mtsys_init();
#ifdef MT_DEBUG_FUNC
	MTDebugInit();
#endif
//	mtAFInit(halPrivate::apsMessageIndicate);
	//set_default_channel();
}

int halSetAPSMessageHandler(int (*fn)(AFMessage*))
{

	kapi::mtaf::register_AFMessage_cb(fn);
	return 0;
}

uint64_t get_ieee_id()
{
	uint64_t ieee_id;
	mt_getextaddr_req(&ieee_id);
	return ieee_id;
}

uint16_t get_nwk_id()
{
	uint8_t res[64];
	mt_util_get_device_info(res);
	return 0;
}

int system_reset()
{
	uint8_t res=0xff;
	mt_sysreset_req();
	return (int)res;
}
int ProcessorReset()
{
	// 3 consecutive resets, each on is 4 seconds apart
	// the 4 sec is the time it takes for processor to reach a state where it can process serial command

	// reduced resets from 3 to 1, the reason for 3 reset being that it was suspected that the reset feature of 2530 is buggy
	// however, some observations suggested (not sure yet) that it was the improper restart of the application on the router side
	// so a 2 sec delay to will be added to the restart script between stop and start
	mt_sysreset_req();
	/* changed from 4 sec to 2 sec, 06-06-2014 */
vTraceLog("Reseted USB wait 4 secs...");	
	kSleep(4);
	/* changed from 4 sec to 2 sec, 06-06-2014 */
	
	/* Rehook usb, 04-21-2015 */
vTraceLog("Reset interface");
	resetInterface();
	/* Rehook usb, 04-21-2015 */
	return 0;
}

int StartSystem()
{
	processorStatus_t status;
	mtMessage_t req,rsp;

	while (1)
	{
		systemPing(&status);

		if (status.capability == 0x3275)
		{
			// to be fixed, the SB_ENABLE_RSP command will NOT be recognized
			// because the callback for SBL subsystem (0x0D) is NOT registeexit(0);red
			// in the mtproc callback array. The SBL subsystem call back is only
			// register when it update new firmware. As such, the following command
			// will generate an warning message:
			//                "UNKNOW MESSAGE :01 4D 83 00"
			// but this is not harmful.

			req.cmdType = 0x21;
			req.cmdId = 0x4b;
			req.dataLength = 0;
			__MTSEND__(&req,NULL,0,NULL);
			vTraceLog("cc2530 image ok, booting the system...");
		} else if (status.capability == 0x32b5)
		{
			LOG_ERROR_MESSAGE("no valid firmware inside");

		} else if (status.capability == 0x0059 && status.operationState == 0x02)
		{
			vTraceLog("cc2530 is in normal operating state");
			break;
		} else
		{
//			mt_sysreset_req();
		}

		kSleep(3);
	}
	return 0;
}

int system_ping(uint8_t*result)
{
	return mt_sysping_req(result,4);
}

int systemPing(processorStatus_t *status)
{
	FILE *file;
	time_t lastFailedPing, timeLapse;

	/* check for last failed ping, if file doesn't exist, create one */

	if (access(RSSERIAL_LAST_FAILED_PING, F_OK) <0)
	{
		file = fopen (RSSERIAL_LAST_FAILED_PING, "w+");
		fprintf(file, "0");
		fclose(file);
	}

	file = fopen (RSSERIAL_LAST_FAILED_PING, "r");
	fscanf(file, "%d", &lastFailedPing);
	fclose(file);

	timeLapse = time(NULL) - lastFailedPing;
	if (timeLapse < MINIUM_FAILED_PING_WAIT_SEC)
	{
		syslog(LOG_INFO, "last failed ping is %d, wait %d sec", lastFailedPing, MINIUM_FAILED_PING_WAIT_SEC-timeLapse);
		kSleep(MINIUM_FAILED_PING_WAIT_SEC- timeLapse);
	}

	uint8_t result[4];
	int rtnval = mt_sysping_req(result,4);

	if (rtnval == 0)
	{
		status->capability = *(uint16_t*)result;
		status->operationState = result[2];
		halPrivate::consecutiveFailedPing = 0;
	} else {
		if (halPrivate::consecutiveFailedPing > 3)
		{
			file = fopen (RSSERIAL_LAST_FAILED_PING, "w+");
			fprintf(file, "%ld", time(NULL));
			fclose(file);
		} else {
			halPrivate::consecutiveFailedPing++;
		}
	}

	return rtnval;
}

int cc2530_wdt_kick()
{
	uint8_t result[1];
	int rtnval = mt_sys_wdtkick_req(result,1);
	return rtnval;
}
int start_zdo()
{
	MT_ZDO_STARTUP_FROM_APP_Req(0);

	int t = 0;
	int status = -1;
	while (t < 30)
	{
		if (MT::ZDO::zdoCoordReady()) {
			status = 0;
			break;
		}
		kSleep(1);
		t++;
	}
	return status;
}

int getAssocCount()
{
	uint16_t returnBuffer;
	if (mtUtilGetAssocCount(&returnBuffer) < 0)
	{
		return -1;
	}
	return (int)returnBuffer;
}

int getAssocInfo(HAL_STRUCTURE::associated_devices_t *_dev, uint8_t index)
{
	uint8_t returnBuffer[18];
	uint8_t *rtn = returnBuffer;
//	HAL_STRUCTURE::associated_devices_t dev = *_dev;

	if (mtUtilAssocFindDevice(returnBuffer, index) < 0)
	{
		return -1;
	}

	// 0xfffe from shortaddr means INVALID_NODE_ADDR, indicating failure
	rtn += _BYTES_TO_INT_(_dev->shortAddr,*rtn,2);
	rtn += _BYTES_TO_INT_(_dev->addrIdx,*rtn,2);
	rtn += _BYTES_TO_INT_(_dev->nodeRelation,*rtn,1);
	rtn += _BYTES_TO_INT_(_dev->devStatus,*rtn,1);
	rtn += _BYTES_TO_INT_(_dev->assocCnt,*rtn,1);
	rtn += _BYTES_TO_INT_(_dev->age,*rtn,1);
	rtn += _BYTES_TO_INT_(_dev->linkInfo.txCounter,*rtn,1);
	rtn += _BYTES_TO_INT_(_dev->linkInfo.txCost,*rtn,1);
	rtn += _BYTES_TO_INT_(_dev->linkInfo.rxLqi,*rtn,1);
	rtn += _BYTES_TO_INT_(_dev->linkInfo.inKeySeqNum,*rtn,1);
	rtn += _BYTES_TO_INT_(_dev->linkInfo.inFrmCntr,*rtn,4);
	rtn += _BYTES_TO_INT_(_dev->linkInfo.txFailure,*rtn,2);
	return 0;

}

int getDeviceInfo(mt_device_info_t *deviceInfo)
{
	uint8_t rtn[256];
	int rtnval = mtUtilGetDeviceInfo(rtn,256);

	if (rtnval == 0)
	{
		deviceInfo->status = rtn[0];
		deviceInfo->IEEEAddr = *(uint64_t *)(rtn+1);
		deviceInfo->NwkAddr = *(uint16_t *)(rtn+9);
		deviceInfo->DeviceType = rtn[11];
		deviceInfo->DeviceState = rtn[12];
		deviceInfo->NumAssocDevices = rtn[13];
	}

	return rtnval;
}

int sendAPSMessageStruct(AFMessage *message)
{
	return MT_AfDataRequest(message);
}

int sendAPSMessageStruct(AFMessage_EXT *message)
{
	return kapi::mtaf::dataRequest(message);
}

uint16_t getAssocNwkAddr(uint64_t extAddr)
{
	mtMessage_t rtn;
	mtUtilAssocGetWithAddress(extAddr, &rtn);
	uint16_t nwkAddr;
	bytes_to_uint((void*)&nwkAddr,(void*)rtn.pData,2);
	return nwkAddr;
}

uint64_t getAssocExtAddr(uint16_t nwkAddr)
{
	uint64_t extAddr;

	if (mtUtilAddrmgrNwkAddrLookup(nwkAddr, &extAddr)<0)
	{
		return 0xffffffffffffffff;
	}
	return extAddr;
}
void registerEndpoint(uint8_t endpoint_id, uint16_t profile_id,uint16_t device_id, uint8_t device_ver)
{
	register_endpoint_TI(endpoint_id,profile_id,device_id, device_ver);
}
void registerInterPanCB(uint8_t ep)
{
	register_interpan_cb_TI(ep);
}
namespace HAL {
	int getChannel(uint8_t &channel)
	{
		NvItem_t nvItem;

		nvItem.id = 0x0021;
		nvItem.offset = 0;
		nvItem.len = 0;
		int status = HAL::OsalNvRead(nvItem);

		if (status == 0 && nvItem.len != 0)
		{
			channel = nvItem.value[22];
		}
		return status;
	}
	int getNwkUpdateId(uint8_t &nwkUpdateId)
	{
		NvItem_t nvItem;

		nvItem.id = 0x0021;
		nvItem.offset = 0;
		nvItem.len = 0;
		int status = HAL::OsalNvRead(nvItem);

		if (status == 0 && nvItem.len != 0)
		{
			nwkUpdateId = nvItem.value[109];
		}
		return status;
	}
	int setNwkUpdateId(uint8_t nwkUpdateId)
	{
		NvItem_t nvItem;

		nvItem.id = 0x0021;
		nvItem.offset = 109;
		nvItem.len = 1;
		nvItem.value[0] = nwkUpdateId;
		return HAL::OsalNvWrite(nvItem);

	}
	void *delayRestart_(void *td)
	{
		int sec = *( (int*) td);
		syslog(LOG_INFO, "reset in %d sec", sec);
		kSleep(sec);
		system("/etc/init.d/rsserial-watch restart");
	}
	int delayRestart(int sec)
	{
		pthread_t thread;
		// sec_ must be static, otherwise sec_ will be destroyed before *delayResart
		// have a chance to grab its values
		static int sec_ = sec;

		pthread_create(&thread, NULL, delayRestart_, (void*)&sec_ );
		pthread_detach(thread);

	}
	int zcdStartupOptions(uint8_t options)
	{
		NvItem_t nvItem;

		nvItem.id = 0x0003;
		nvItem.offset = 0;
		nvItem.len = 1;
		nvItem.value[0] = options;
		return HAL::OsalNvWrite(nvItem);
	}
//	int setChannel(uint8 channel)
//	{
//		if (channel < 11 || channel > 26)
//		{
//			return -1;
//		}
//
//		NvItem_t nvItem;
//
//		nvItem.id = 0x0021;
//		nvItem.offset = 22;
//		nvItem.len = 1;
//		nvItem.value[0] = channel;
//		int status = HAL::OsalNvWrite(nvItem);
//
//		if (status == 0 && nvItem.status == 0)
//		{
//			pthread_t thread;
//			pthread_create(&thread, NULL, delayRestart_, (void *)NULL);
//			pthread_detach(thread);
//			syslog(LOG_INFO, "Set channel successful, now restart rsserial...");
//			interPanSet(channel);
//			return 0;
//		} else {
//			syslog(LOG_INFO, "Set channel failed");
//		}
//
//		return 0;
//	}
}
