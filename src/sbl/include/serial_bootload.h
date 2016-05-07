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
#ifndef SERIAL_BOOTLOAD_H
#define SERIAL_BOOTLOAD_H
#include <stdint.h>
//#include "glbdef.h"
//#define SB_FIRMWARE_TEMP_PATH "/tmp/krfirm.bin"

#define HAL_SB_IMG_ADDR 0x2000
#define HAL_SB_CRC_ADDR 0x2090
// Size of internal flash less 4 pages for boot loader, 6 pages for NV, & 1 page for lock bits.
#define HAL_SB_IMG_SIZE      (0x40000 - 0x2000 - 0x3000 - 0x0800)

#define HAL_FLASH_WORD_SIZE 4
#define SB_IMG_OSET                 (HAL_SB_IMG_ADDR / HAL_FLASH_WORD_SIZE)
// The write-page boundary since the write-address is passed as "actual address / flash word size".
#define SB_WPG_SIZE                 (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE)

#define SB_BUF_SIZE                 128
#define SB_SOF                      0xFE
#define SB_HANDSHAKE                0xFE

#define SB_FORCE_BOOT               0xF8
#define SB_FORCE_RUN               (SB_FORCE_BOOT ^ 0xFF)

#define SB_RW_BUF_LEN               64

// Bootloader Serial Interface Subsystem
#define SB_RPC_SYS_BOOT             0x4D

// Commands to Bootloader
#define SB_WRITE_CMD                0x01
#define SB_READ_CMD                 0x02
#define SB_ENABLE_CMD               0x03
#define SB_HANDSHAKE_CMD            0x04

#define SB_WRITE_RSP                0x81
#define SB_READ_RSP                 0x82
#define SB_ENABLE_RSP               0x83
#define SB_HANDSHAKE_RSP            0x84

// Status codes
#define SB_SUCCESS                  0
#define SB_FAILURE                  1
#define SB_INVALID_FCS              2
#define SB_INVALID_FILE             3
#define SB_FILESYSTEM_ERROR         4
#define SB_ALREADY_STARTED          5
#define SB_NO_RESPOSNE              6
#define SB_VALIDATE_FAILED          7
#define SB_CANCELED                 8

//// Serial RX States
//#define SB_SOF_STATE                0
//#define SB_LEN_STATE                1
//#define SB_CMD1_STATE               2
//#define SB_CMD2_STATE               3
//#define SB_DATA_STATE               4
//#define SB_FCS_STATE                5
#ifdef __cplusplus
extern "C" {
#endif
//int run_bootload();
int flashImage(const char* imageFile);
#ifdef __cplusplus
}
#endif
#endif
