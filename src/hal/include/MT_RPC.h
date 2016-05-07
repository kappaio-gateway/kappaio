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
#ifndef MT_RPC_H
#define MT_RPC_H
//#include "glbdef.h"
#ifdef __cplusplus
extern "C" {
#endif
 
#ifndef UINT8
	#define UINT8
	typedef uint8_t uint8;
#endif
//typedef unsigned int uint16;
typedef unsigned int word16;
//typedef uint8_t byte;
//#define byte uint8_t
/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/

//#include "hal_types.h"

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/

/* 1st byte is the length of the data field, 2nd/3rd bytes are command field. */
#define MT_RPC_FRAME_HDR_SZ   3

/* Maximum length of data in the general frame format. The upper limit is 255 because of the
 * 1-byte length protocol. But the operation limit is lower for code size and ram savings so that
 * the uart driver can use 256 byte rx/tx queues and so
 * (MT_RPC_DATA_MAX + MT_RPC_FRAME_HDR_SZ + MT_UART_FRAME_OVHD) < 256
 */
#define MT_RPC_DATA_MAX       250
#define MT_BUFFER_MAX         256
/* The 3 MSB's of the 1st command field byte are for command type. */
#define MT_RPC_CMD_TYPE_MASK  0xE0

/* The 5 LSB's of the 1st command field byte are for the subsystem. */
#define MT_RPC_SUBSYSTEM_MASK 0x1F

/* position of fields in the general format frame */
#define MT_RPC_POS_LEN        0
#define MT_RPC_POS_CMD0       1
#define MT_RPC_POS_CMD1       2
#define MT_RPC_POS_DAT0       3

/* Error codes */
#define MT_RPC_SUCCESS        0     /* success */
#define MT_RPC_ERR_SUBSYSTEM  1     /* invalid subsystem */
#define MT_RPC_ERR_COMMAND_ID 2     /* invalid command ID */
#define MT_RPC_ERR_PARAMETER  3     /* invalid parameter */
#define MT_RPC_ERR_LENGTH     4     /* invalid length */

/***************************************************************************************************
 * TYPEDEF
 ***************************************************************************************************/

typedef enum {
  MT_RPC_CMD_POLL = 0x00,
  MT_RPC_CMD_SREQ = 0x20,
  MT_RPC_CMD_AREQ = 0x40,
  MT_RPC_CMD_SRSP = 0x60,
  MT_RPC_CMD_RES4 = 0x80,
  MT_RPC_CMD_RES5 = 0xA0,
  MT_RPC_CMD_RES6 = 0xC0,
  MT_RPC_CMD_RES7 = 0xE0
} mtRpcCmdType_t;

typedef enum {
  MT_RPC_SYS_RES0,   /* Reserved. */
  MT_RPC_SYS_SYS,
  MT_RPC_SYS_MAC,
  MT_RPC_SYS_NWK,
  MT_RPC_SYS_AF,
  MT_RPC_SYS_ZDO,
  MT_RPC_SYS_SAPI,   /* Simple API. */
  MT_RPC_SYS_UTIL,
  MT_RPC_SYS_DBG,
  MT_RPC_SYS_APP,
  MT_RPC_SYS_OTA, // 0x0A
  RESERVED0, // 0x0B
  RESERVED1, //0x0C
  SBLr, //0x0D
  MT_RPC_SYS_MAX     /* Maximum value, must be last */
  /* 10-32 available, not yet assigned. */
} mtRpcSysType_t;

typedef struct
{
  uint8 *(*alloc)(mtRpcCmdType_t type, uint8 len);
  void (*send)(uint8 *pBuf);
} mtTransport_t;

typedef uint8 (*mtProcessMsg_t)(uint8 *pBuf);

/***************************************************************************************************
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MT_RPC_H */
