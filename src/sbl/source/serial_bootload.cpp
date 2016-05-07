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
#include "serial_bootload.h"
#include <semaphore.h>
#include <string.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "MT_RPC.h"
#include "hal.h"
#include "kglobal.h"
#include "mt_helper.h"
#include "kutil.h"

struct sbmessage_s {
	volatile int incoming;
	sem_t sem;
	uint8_t *buffer;
};

static struct sbmessage_s msg;
char *SB_FIRMWARE_PERM_PATH;

//int get_sbmessage(uint8_t *pBuf, int timeout_ms);
int sb_image_write();
int sb_image_read();
int sb_handshake();
int sb_image_check();
//int sb_image_enable();
uint8_t sb_callback(uint8_t *pBuf); //
//int sb_BootLoad_state();
uint16_t calcCRC(uint8_t *firmwareBuffer);
uint16_t runPoly(uint16_t crc, uint8_t val);

int flashImage(const char* imageFile)
{
	printf("starting flashing %s to TI chip \n", imageFile);
	if (registerSubsystemCb(0x0d,sb_callback) <0)
	{
		LOG_MESSAGE("serial boot subsystem failed to register, exiting...");
		return -1;
	}

	sem_init(&(msg.sem), 0, 1);
	msg.incoming = 0;

	SB_FIRMWARE_PERM_PATH = (char*)imageFile;

	printf("checking image ... \n");
	if (sb_image_check() < 0)
	{
		printf("firmware image check failed, exiting... \n");
		exit(-1);
	}

	printf("image ok start flashing ... \n");

	while (sb_handshake() < 0)
	{
		kSleep(5);
	}

	if (sb_image_write() < 0)
	{
		syslog(LOG_INFO, "firmware image write failed");
		return -1;
	}

//	if (sb_image_read() < 0){
//		syslog(LOG_INFO, "firmware image verification failed");
//		return -1;
//	}

	return 0;
}

int sb_image_check()
{
	int fd;
	int rtnval = 0;
//	off_t offset = HAL_SB_CRC_ADDR - HAL_SB_IMG_ADDR;
	uint16_t *crc;
	uint8_t firmwareBuffer[HAL_SB_IMG_SIZE];

	fd = open(SB_FIRMWARE_PERM_PATH, O_RDWR);
	if (fd < 0)
	{
		LOG_ERROR_MESSAGE("image file open failed");
		return -1;
	}

	if (read(fd, (void *)firmwareBuffer, HAL_SB_IMG_SIZE) != HAL_SB_IMG_SIZE)
	{
		LOG_ERROR_MESSAGE("bad image size");
		rtnval = -1;
		goto exit_sb_image_check;
	}

	// basic check crc first
	crc = (uint16_t *)(firmwareBuffer + HAL_SB_CRC_ADDR - HAL_SB_IMG_ADDR);
	if (crc[0] == 0xFFFF || crc[0] == 0x0000 || crc[0] != calcCRC(firmwareBuffer))
	{
		LOG_ERROR_MESSAGE("bad image crc");
		rtnval = -1;
		goto exit_sb_image_check;
	}


	exit_sb_image_check:
	close(fd);
	return rtnval;
}

int sb_handshake()
{
	mtMessage_t req, rsp;
	req.cmdType = SB_RPC_SYS_BOOT;
	req.cmdId = SB_HANDSHAKE_CMD;
	req.dataLength = 0;

	kSleep(1);

	int rtn = mtSend(&req,SB_HANDSHAKE_RSP,(void*)&rsp,sizeof(rsp),NULL);

	if (rtn<0)
	{
		printf("handshake failed \n");
	} else
	{
		printf("handshake status: %02x \n", rsp.pData[0]);
	}

	return rtn;
}

uint8_t sb_callback(uint8_t *pBuf)
{
	sem_wait(&(msg.sem));

	mtRecv(pBuf);
	msg.incoming = 1;
	msg.buffer = (uint8_t *)malloc(pBuf[MT_RPC_POS_LEN]+3);
	memcpy((void *)(msg.buffer), (void *)pBuf, pBuf[MT_RPC_POS_LEN]+3);
	sem_post(&(msg.sem));
	return 0;
}

int sb_image_read()
{
	int fd;
	uint8_t FileBuf[SB_RW_BUF_LEN + 3];
//	uint8_t cmdBuf[SB_RW_BUF_LEN * 2];
//	uint8_t rspBuf[SB_RW_BUF_LEN*2];
	int FileBufLen;
	uint16_t addr = 0;
//	int i;
	int rtnval = 0;

	fd = open(SB_FIRMWARE_PERM_PATH, O_RDONLY);
	if (fd < 0)
	{
		LOG_ERROR_MESSAGE("firmware open failed");
		return -1;
	}

	addr = 0;
	mtMessage_t req,rsp;
	req.cmdType = SB_RPC_SYS_BOOT;
	req.cmdId = SB_READ_CMD;
	req.dataLength = SB_RW_BUF_LEN+3;

	do {
		FileBufLen = read(fd, (void*)FileBuf, SB_RW_BUF_LEN);
		if (FileBufLen > 0)
		{

			req.pData[0] = (uint8_t)(addr & 0x00FF);
			req.pData[1] = (uint8_t)(addr >> 8);

			if (mtSend(&req,SB_READ_RSP,(void*)&rsp,sizeof(rsp),NULL) <0)
			{
				LOG_ERROR_MESSAGE("send read failed, addr = %04x",addr);
			} else
			{
				if (rsp.pData[0]!=SB_SUCCESS)
				{
					LOG_ERROR_MESSAGE("read status failed, addr : %04x, status : %02x",addr,rsp.pData[0]);
				}
			}

//			char buffer[256];
			if (memcmp(FileBuf, rsp.pData+3, SB_RW_BUF_LEN) != 0)
			{
				rtnval = -1;
				LOG_ERROR_MESSAGE("ADDR %02x%02x not match", rsp.pData[2],rsp.pData[1]);
				break;
			} else
			{
				if ((addr & 0x0FFF) == 0)
				{
					LOG_ERROR_MESSAGE("read-back address %04x success", addr);
				}
			}
			addr += SB_RW_BUF_LEN / HAL_FLASH_WORD_SIZE;
		}

	} while (FileBufLen > 0);

	close(fd);
	return rtnval;
}

int sb_image_write()
{
	int fd;
//	uint8_t FileBuf[SB_RW_BUF_LEN + 3],cmdBuf[SB_RW_BUF_LEN * 2], rspBuf[SB_RW_BUF_LEN*2];
	int FileBufLen;
	uint16_t addr = 0;
//	int i;
	int rtnval = 0;

	fd = open(SB_FIRMWARE_PERM_PATH, O_RDONLY);
	if (fd < 0)
	{
		LOG_ERROR_MESSAGE("firmware open failed");
		return -1;
	}

	addr = 0;
	mtMessage_t wrReq,wrRsp;
	wrReq.cmdType = SB_RPC_SYS_BOOT;
	wrReq.cmdId = SB_WRITE_CMD;
	wrReq.dataLength = SB_RW_BUF_LEN+2;

	mtMessage_t rdReq,rdRsp;
	rdReq.cmdType = SB_RPC_SYS_BOOT;
	rdReq.cmdId = SB_READ_CMD;
	rdReq.dataLength = SB_RW_BUF_LEN+3;

	do {
		uint8_t returnStatus = 0xff;
		int wrRetry = 0, rdRetry=0, rwRetry = 0, readbackStatus=-1;
		/* read out SB_RW_BUF_LEN=64 bytes */

		FileBufLen = read(fd, (void*)((uint8_t*)wrReq.pData+2), SB_RW_BUF_LEN);

		/* write the chunk to flash, retry if needed */
		if (FileBufLen > 0)
		{
			rwRetry = 0;
			readbackStatus=-1;
			while (readbackStatus != 0 && rwRetry < 3)
			{
				wrRetry = 0;
				rdRetry = 0;

				while (returnStatus != SB_SUCCESS && wrRetry < 3 )
				{
					wrReq.pData[0] = (uint8_t)(addr & 0x00FF);
					wrReq.pData[1] = (uint8_t)(addr >> 8);

					/* write to flash */
					if (mtSend(&wrReq,SB_WRITE_RSP,(void*)&wrRsp,sizeof(wrRsp),NULL) <0)
					{
						LOG_ERROR_MESSAGE("send write failed, addr = %04x",addr);
					} else
					{
						returnStatus = wrRsp.pData[0];

						if (returnStatus!=SB_SUCCESS)
						{
							wrRetry++;
							LOG_ERROR_MESSAGE("write status failed, addr = %04x",addr);
						}
						else
						{
							if ((addr & 0x0FFF) == 0)
							{
								printf("write address block %04x success\n", addr);
							}
						}
					}

				}

				if (returnStatus!=SB_SUCCESS)
				{
					LOG_ERROR_MESSAGE("write at addr = %04x failed after 3 retries",addr);
					rwRetry++;
					continue;
				}

				readbackStatus = -1;
				while(readbackStatus !=0 && rdRetry < 3)
				{
					rdReq.pData[0] = (uint8_t)(addr & 0x00FF);
					rdReq.pData[1] = (uint8_t)(addr >> 8);

					if (mtSend(&rdReq,SB_READ_RSP,(void*)&rdRsp,sizeof(rdRsp),NULL) <0)
					{
						LOG_ERROR_MESSAGE("send read failed, addr = %04x",addr);
					} else
					{
						if (rdRsp.pData[0]!=SB_SUCCESS)
						{
							LOG_ERROR_MESSAGE("read status failed, addr : %04x, status : %02x",addr,rdRsp.pData[0]);
							rdRetry++;
							continue;
						}
					}

					if (memcmp(wrReq.pData+2, rdRsp.pData+3, SB_RW_BUF_LEN) != 0)
					{
						rtnval = -1;
						LOG_ERROR_MESSAGE("ADDR %02x%02x not match", rdRsp.pData[2],rdRsp.pData[1]);
						break;
					} else
					{
						readbackStatus = 0;
						if ((addr & 0x0FFF) == 0)
						{
							printf("read-back address %04x success\n", addr);
						}
					}
				}


				rwRetry++;
			}

			if (readbackStatus!=0)
			{
				LOG_ERROR_MESSAGE("readback at addr = %04x failed after 3 retries",addr);
				exit(-1);
			}

			addr += SB_RW_BUF_LEN / HAL_FLASH_WORD_SIZE;
		}

	} while(FileBufLen > 0);

	close(fd);
	return rtnval;
}

uint16_t calcCRC(uint8_t *firmwareBuffer)
{
  uint32_t addr;
  uint16_t crc = 0;

  // Run the CRC calculation over the active body of code.
  for (addr = 0; addr < HAL_SB_IMG_SIZE; addr++)
  {
	  if (addr == HAL_SB_CRC_ADDR - HAL_SB_IMG_ADDR)
	  {
		  addr += 3;
	  }
	  else
	  {
		  crc = runPoly(crc, firmwareBuffer[addr]);
	  }
  }
  // IAR note explains that poly must be run with value zero for each byte of crc.
  crc = runPoly(crc, 0);
  crc = runPoly(crc, 0);

  return crc;
}

uint16_t runPoly(uint16_t crc, uint8_t val)
{
	const uint16_t poly = 0x1021;

	uint8_t cnt;

	for (cnt = 0; cnt < 8; cnt++, val <<= 1)
	{
		uint8_t msb = (crc & 0x8000) ? 1 : 0;

		crc <<= 1;
		if (val & 0x80)  crc |= 0x0001;
		if (msb)         crc ^= poly;
	}
	return crc;
}

//int sbl_serialport_test()
//{
//	system_reset();
//	syslog(LOG_INFO, "reset");
//
//	system_reset();
//	syslog(LOG_INFO, "reset");
//
//	system_reset();
//	syslog(LOG_INFO, "reset");
//
//	system_reset();
//	syslog(LOG_INFO, "reset");
//
//	return 0;
//}

//int sb_BootLoad_state() {
////	struct device_info_s devInfo;
//	int rtn = -1;
//	int dlyCnt;
//	uint8_t rspBuf[MT_BUFFER_MAX];
//
////	MTSysResetReq();
//	while (get_sbmessage(rspBuf, 1000) != 0);
//	syslog(LOG_INFO, "reset");
//
////	MTSysResetReq();
//	while (get_sbmessage(rspBuf, 1000) != 0);
//	syslog(LOG_INFO, "reset");
//
////	MTSysResetReq();
//	while (get_sbmessage(rspBuf, 1000) != 0);
//	syslog(LOG_INFO, "reset");
//
////	MTSysResetReq();
//	while (get_sbmessage(rspBuf, 1000) != 0);
//	syslog(LOG_INFO, "reset");
//
////	MTSysResetReq();
//	while (get_sbmessage(rspBuf, 1000) != 0);
//	if (rspBuf[MT_RPC_POS_CMD1] == 0x05 && rspBuf[MT_RPC_POS_DAT0] == 0x80) {
//		syslog(LOG_INFO, "bootload state");
//		rtn = 0;
//	}
//
//	return rtn;
//}

//int run_bootload()
//{
//	if (registerSubsystemCb(0x0d,sb_callback) <0)
//	{
//		vTraceLog("serial boot subsystem failed to register, exiting...");
//		return -1;
//	}
//
//	sem_init(&(msg.sem), 0, 1);
//	msg.incoming = 0;
//
//	sbl_serialport_test();
//
//
//	if (sb_handshake() < 0)
//	{
//		return -1;
//	}
//
//	if (sb_image_check() < 0)
//	{
//		syslog(LOG_INFO, "firmware image check failed");
//		return -1;
//	}
//
//	if (sb_image_write() < 0)
//	{
//		syslog(LOG_INFO, "firmware image write failed");
//		return -1;
//	}
//
//	if (sb_image_read() < 0){
//		syslog(LOG_INFO, "firmware image verification failed");
//		return -1;
//	}
//
//	sb_image_enable();
//
//	syslog(LOG_INFO, "firmware update successful");
//
//	return 0;
//}
//int sb_image_enable()
//{
////	uint8_t rspBuf[256];
//
//	mtMessage_t req,rsp;
//	req.cmdType = SB_RPC_SYS_BOOT;
//	req.cmdId = SB_ENABLE_CMD;
//	req.dataLength =0;
//
//	if (mtSend(&req,SB_ENABLE_RSP,(void*)&rsp,sizeof(rsp),NULL) <0)
//	{
//		LOG_ERROR_MESSAGE("failed to enable image");
//	}
//	return 0;
//}

//int get_sbmessage(uint8_t *pBuf, int timeout_ms)
//{
//	int status = 0;
//	int dlyCnt_ms = 0;
//	while (dlyCnt_ms < timeout_ms) {
//
//		sem_wait(&(msg.sem));
//		if (msg.incoming == 1) {
//			memcpy((void *)pBuf, (void *)(msg.buffer), msg.buffer[MT_RPC_POS_LEN]+3);
//			status = 1;
//			free(msg.buffer);
//			msg.incoming = 0;
//		} else {
//			status = 0;
//		}
//		sem_post(&(msg.sem));
//		if (status == 1) {
//			break;
//		}
//
//		usleep(1000);
//		dlyCnt_ms++;
//	}
//	if (dlyCnt_ms == timeout_ms) {
//		return -1;
//	}
//	return 0;
//}
