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
#ifndef MTPROC_H
#define MTPROC_H
#include <stdint.h>
#include <stddef.h>
#include "zdpconst.h"
/***************************************************************************************************
 * SYS COMMANDS
 ***************************************************************************************************/

/* AREQ from host */
#define MT_SYS_RESET_REQ                     0x00

/* SREQ/SRSP */
#define MT_SYS_PING                          0x01
#define MT_SYS_VERSION                       0x02
#define MT_SYS_SET_EXTADDR                   0x03
#define MT_SYS_GET_EXTADDR                   0x04
#define MT_SYS_RAM_READ                      0x05
#define MT_SYS_RAM_WRITE                     0x06
#define MT_SYS_OSAL_NV_ITEM_INIT             0x07
#define MT_SYS_OSAL_NV_READ                  0x08
#define MT_SYS_OSAL_NV_WRITE                 0x09
#define MT_SYS_OSAL_START_TIMER              0x0A
#define MT_SYS_OSAL_STOP_TIMER               0x0B
#define MT_SYS_RANDOM                        0x0C
#define MT_SYS_ADC_READ                      0x0D
#define MT_SYS_GPIO                          0x0E
#define MT_SYS_STACK_TUNE                    0x0F
//-***** changed from 0x14 -> 0x44 to avoid collision with SET_TX_POWER in 2.5.1a -*****
#define MT_SYS_WDT_KICK                      0x44

/* AREQ to host */
#define MT_SYS_RESET_IND                     0x80
#define MT_SYS_OSAL_TIMER_EXPIRED            0x81

/***************************************************************************************************
 * MAC COMMANDS
 ***************************************************************************************************/
/* SREQ/SRSP */
#define MT_MAC_RESET_REQ                     0x01
#define MT_MAC_INIT                          0x02
#define MT_MAC_START_REQ                     0x03
#define MT_MAC_SYNC_REQ                      0x04
#define MT_MAC_DATA_REQ                      0x05
#define MT_MAC_ASSOCIATE_REQ                 0x06
#define MT_MAC_DISASSOCIATE_REQ              0x07
#define MT_MAC_GET_REQ                       0x08
#define MT_MAC_SET_REQ                       0x09
#define MT_MAC_GTS_REQ                       0x0a
#define MT_MAC_RX_ENABLE_REQ                 0x0b
#define MT_MAC_SCAN_REQ                      0x0c
#define MT_MAC_POLL_REQ                      0x0d
#define MT_MAC_PURGE_REQ                     0x0e
#define MT_MAC_SET_RX_GAIN_REQ               0x0f

/* AREQ from Host */
#define MT_MAC_ASSOCIATE_RSP                 0x50
#define MT_MAC_ORPHAN_RSP                    0x51

/* AREQ to host */
#define MT_MAC_SYNC_LOSS_IND                 0x80
#define MT_MAC_ASSOCIATE_IND                 0x81
#define MT_MAC_ASSOCIATE_CNF                 0x82
#define MT_MAC_BEACON_NOTIFY_IND             0x83
#define MT_MAC_DATA_CNF                      0x84
#define MT_MAC_DATA_IND                      0x85
#define MT_MAC_DISASSOCIATE_IND              0x86
#define MT_MAC_DISASSOCIATE_CNF              0x87
#define MT_MAC_GTS_CNF                       0x88
#define MT_MAC_GTS_IND                       0x89
#define MT_MAC_ORPHAN_IND                    0x8a
#define MT_MAC_POLL_CNF                      0x8b
#define MT_MAC_SCAN_CNF                      0x8c
#define MT_MAC_COMM_STATUS_IND               0x8d
#define MT_MAC_START_CNF                     0x8e
#define MT_MAC_RX_ENABLE_CNF                 0x8f
#define MT_MAC_PURGE_CNF                     0x90

/***************************************************************************************************
 * NWK COMMANDS
 ***************************************************************************************************/

/* AREQ from host */
#define MT_NWK_INIT                          0x00

/* SREQ/SRSP */
#define MT_NLDE_DATA_REQ                     0x01
#define MT_NLME_NETWORK_FORMATION_REQ        0x02
#define MT_NLME_PERMIT_JOINING_REQ           0x03
#define MT_NLME_JOIN_REQ                     0x04
#define MT_NLME_LEAVE_REQ                    0x05
#define MT_NLME_RESET_REQ                    0x06
#define MT_NLME_GET_REQ                      0x07
#define MT_NLME_SET_REQ                      0x08
#define MT_NLME_NETWORK_DISCOVERY_REQ        0x09
#define MT_NLME_ROUTE_DISCOVERY_REQ          0x0A
#define MT_NLME_DIRECT_JOIN_REQ              0x0B
#define MT_NLME_ORPHAN_JOIN_REQ              0x0C
#define MT_NLME_START_ROUTER_REQ             0x0D

/* AREQ to host */
#define MT_NLDE_DATA_CONF                    0x80
#define MT_NLDE_DATA_IND                     0x81
#define MT_NLME_NETWORK_FORMATION_CONF       0x82
#define MT_NLME_JOIN_CONF                    0x83
#define MT_NLME_JOIN_IND                     0x84
#define MT_NLME_LEAVE_CONF                   0x85
#define MT_NLME_LEAVE_IND                    0x86
#define MT_NLME_POLL_CONF                    0x87
#define MT_NLME_SYNC_IND                     0x88
#define MT_NLME_NETWORK_DISCOVERY_CONF       0x89
#define MT_NLME_START_ROUTER_CONF            0x8A

/***************************************************************************************************
 * AF COMMANDS
 ***************************************************************************************************/

/* SREQ/SRSP */
#define MT_AF_REGISTER                       0x00
#define MT_AF_DATA_REQUEST                   0x01  /* AREQ optional, but no AREQ response. */
#define MT_AF_DATA_REQUEST_EXT               0x02  /* AREQ optional, but no AREQ response. */
#define MT_AF_DATA_REQUEST_SRCRTG            0x03

#define MT_AF_INTER_PAN_CTL                  0x10
#define MT_AF_DATA_STORE                     0x11
#define MT_AF_DATA_RETRIEVE                  0x12

/* AREQ to host */
#define MT_AF_DATA_CONFIRM                   0x80
#define MT_AF_INCOMING_MSG                   0x81
#define MT_AF_INCOMING_MSG_EXT               0x82

/***************************************************************************************************
 * ZDO COMMANDS
 ***************************************************************************************************/

/* SREQ/SRSP */
#define MT_ZDO_NWK_ADDR_REQ                  0x00
#define MT_ZDO_IEEE_ADDR_REQ                 0x01
#define MT_ZDO_NODE_DESC_REQ                 0x02
#define MT_ZDO_POWER_DESC_REQ                0x03
#define MT_ZDO_SIMPLE_DESC_REQ               0x04
#define MT_ZDO_ACTIVE_EP_REQ                 0x05
#define MT_ZDO_MATCH_DESC_REQ                0x06
#define MT_ZDO_COMPLEX_DESC_REQ              0x07
#define MT_ZDO_USER_DESC_REQ                 0x08
#define MT_ZDO_END_DEV_ANNCE                 0x0A
#define MT_ZDO_USER_DESC_SET                 0x0B
#define MT_ZDO_SERVICE_DISC_REQ              0x0C
#define MT_ZDO_END_DEV_BIND_REQ              0x20
#define MT_ZDO_BIND_REQ                      0x21
#define MT_ZDO_UNBIND_REQ                    0x22
  
#define MT_ZDO_SET_LINK_KEY                  0x23
#define MT_ZDO_REMOVE_LINK_KEY               0x24
#define MT_ZDO_GET_LINK_KEY                  0x25
#define MT_ZDO_NWK_DISCOVERY_REQ             0x26
#define MT_ZDO_JOIN_REQ                      0x27

#define MT_ZDO_MGMT_NWKDISC_REQ              0x30
#define MT_ZDO_MGMT_LQI_REQ                  0x31
#define MT_ZDO_MGMT_RTG_REQ                  0x32
#define MT_ZDO_MGMT_BIND_REQ                 0x33
#define MT_ZDO_MGMT_LEAVE_REQ                0x34
#define MT_ZDO_MGMT_DIRECT_JOIN_REQ          0x35
#define MT_ZDO_MGMT_PERMIT_JOIN_REQ          0x36
#define MT_ZDO_MGMT_NWK_UPDATE_REQ           0x37

/* AREQ optional, but no AREQ response. */
#define MT_ZDO_MSG_CB_REGISTER               0x3E
#define MT_ZDO_MSG_CB_REMOVE                 0x3F
#define MT_ZDO_STARTUP_FROM_APP              0x40

/* AREQ from host */
#define MT_ZDO_AUTO_FIND_DESTINATION_REQ     0x41

/* AREQ to host */
#define MT_ZDO_AREQ_TO_HOST                  0x80 /* Mark the start of the ZDO CId AREQs to host. */
#define MT_ZDO_NWK_ADDR_RSP               /* 0x80 */ ((uint8_t)NWK_addr_req | 0x80)
#define MT_ZDO_IEEE_ADDR_RSP                 0x81  //((uint8)IEEE_addr_req | 0x80)
#define MT_ZDO_NODE_DESC_RSP                 0x82 // ((uint8)Node_Desc_req | 0x80)
#define MT_ZDO_POWER_DESC_RSP             /* 0x83 */ ((uint8_t)Power_Desc_req | 0x80)
#define MT_ZDO_SIMPLE_DESC_RSP               0x84  // ((uint8)Simple_Desc_req | 0x80)
#define MT_ZDO_ACTIVE_EP_RSP                 0x85  // ((uint8)Active_EP_req | 0x80)
#define MT_ZDO_MATCH_DESC_RSP                0x86  // ((uint8)Match_Desc_req | 0x80)

#define MT_ZDO_COMPLEX_DESC_RSP              0x87
#define MT_ZDO_USER_DESC_RSP                 0x88
//                                        /* 0x92 */ ((uint8)Discovery_Cache_req | 0x80)
#define MT_ZDO_END_DEVICE_ANNCE_IND          0xC1
#define MT_ZDO_USER_DESC_CONF                0x89
#define MT_ZDO_SERVER_DISC_RSP               0x8A

#define MT_ZDO_END_DEVICE_BIND_RSP        /* 0xA0 */ ((uint8_t)End_Device_Bind_req | 0x80)
#define MT_ZDO_BIND_RSP                   /* 0xA1 */ ((uint8_t)Bind_req | 0x80)
#define MT_ZDO_UNBIND_RSP                 /* 0xA2 */ ((uint8_t)Unbind_req | 0x80)

#define MT_ZDO_MGMT_NWK_DISC_RSP          /* 0xB0 */ ((uint8_t)Mgmt_NWK_Disc_req | 0x80)
#define MT_ZDO_MGMT_LQI_RSP               /* 0xB1 */ ((uint8_t)Mgmt_Lqi_req | 0x80)
#define MT_ZDO_MGMT_RTG_RSP               /* 0xB2 */ ((uint8_t)Mgmt_Rtg_req | 0x80)
#define MT_ZDO_MGMT_BIND_RSP              /* 0xB3 */ ((uint8_t)Mgmt_Bind_req | 0x80)
#define MT_ZDO_MGMT_LEAVE_RSP             /* 0xB4 */ ((uint8_t)Mgmt_Leave_req | 0x80)
#define MT_ZDO_MGMT_DIRECT_JOIN_RSP       /* 0xB5 */ ((uint8_t)Mgmt_Direct_Join_req | 0x80)
#define MT_ZDO_MGMT_PERMIT_JOIN_RSP       /* 0xB6 */ ((uint8_t)Mgmt_Permit_Join_req | 0x80)

//                                        /* 0xB8 */ ((uint8)Mgmt_NWK_Update_req | 0x80)

#define MT_ZDO_STATUS_ERROR_RSP              0xC3
#define MT_ZDO_STATE_CHANGE_IND              0xC0
#define MT_ZDO_MATCH_DESC_RSP_SENT           0xC2
#define MT_ZDO_SRC_RTG_IND                   0xC4
#define MT_ZDO_BEACON_NOTIFY_IND             0xC5
#define MT_ZDO_JOIN_CNF                      0xC6
#define MT_ZDO_NWK_DISCOVERY_CNF             0xC7

#define MT_ZDO_MSG_CB_INCOMING               0xFF

// Some arbitrarily chosen value for a default error status msg.
#define MtZdoDef_rsp                         0x0040

/***************************************************************************************************
 * SAPI COMMANDS
 ***************************************************************************************************/

// SAPI MT Command Identifiers
/* AREQ from Host */
#define MT_SAPI_SYS_RESET                   0x09

/* SREQ/SRSP */
#define MT_SAPI_START_REQ                   0x00
#define MT_SAPI_BIND_DEVICE_REQ             0x01
#define MT_SAPI_ALLOW_BIND_REQ              0x02
#define MT_SAPI_SEND_DATA_REQ               0x03
#define MT_SAPI_READ_CFG_REQ                0x04
#define MT_SAPI_WRITE_CFG_REQ               0x05
#define MT_SAPI_GET_DEV_INFO_REQ            0x06
#define MT_SAPI_FIND_DEV_REQ                0x07
#define MT_SAPI_PMT_JOIN_REQ                0x08
#define MT_SAPI_APP_REGISTER_REQ            0x0a

/* AREQ to host */
#define MT_SAPI_START_CNF                   0x80
#define MT_SAPI_BIND_CNF                    0x81
#define MT_SAPI_ALLOW_BIND_CNF              0x82
#define MT_SAPI_SEND_DATA_CNF               0x83
#define MT_SAPI_READ_CFG_RSP                0x84
#define MT_SAPI_FIND_DEV_CNF                0x85
#define MT_SAPI_DEV_INFO_RSP                0x86
#define MT_SAPI_RCV_DATA_IND                0x87


/***************************************************************************************************
 * UTIL COMMANDS
 ***************************************************************************************************/

/* SREQ/SRSP: */
#define MT_UTIL_GET_DEVICE_INFO              0x00
#define MT_UTIL_GET_NV_INFO                  0x01
#define MT_UTIL_SET_PANID                    0x02
#define MT_UTIL_SET_CHANNELS                 0x03
#define MT_UTIL_SET_SECLEVEL                 0x04
#define MT_UTIL_SET_PRECFGKEY                0x05
#define MT_UTIL_CALLBACK_SUB_CMD             0x06
#define MT_UTIL_KEY_EVENT                    0x07
#define MT_UTIL_TIME_ALIVE                   0x09
#define MT_UTIL_LED_CONTROL                  0x0A

#define MT_UTIL_TEST_LOOPBACK                0x10
#define MT_UTIL_DATA_REQ                     0x11
  
#define MT_UTIL_SRC_MATCH_ENABLE             0x20
#define MT_UTIL_SRC_MATCH_ADD_ENTRY          0x21
#define MT_UTIL_SRC_MATCH_DEL_ENTRY          0x22
#define MT_UTIL_SRC_MATCH_CHECK_SRC_ADDR     0x23
#define MT_UTIL_SRC_MATCH_ACK_ALL_PENDING    0x24
#define MT_UTIL_SRC_MATCH_CHECK_ALL_PENDING  0x25

#define MT_UTIL_ADDRMGR_EXT_ADDR_LOOKUP      0x40
#define MT_UTIL_ADDRMGR_NWK_ADDR_LOOKUP      0x41
#define MT_UTIL_APSME_LINK_KEY_DATA_GET      0x44
#define MT_UTIL_APSME_LINK_KEY_NV_ID_GET     0x45
#define MT_UTIL_ASSOC_COUNT                  0x48
#define MT_UTIL_ASSOC_FIND_DEVICE            0x49
#define MT_UTIL_ASSOC_GET_WITH_ADDRESS       0x4A

#define MT_UTIL_ZCL_KEY_EST_INIT_EST         0x80
#define MT_UTIL_ZCL_KEY_EST_SIGN             0x81

/* AREQ from/to host */
#define MT_UTIL_SYNC_REQ                     0xE0
#define MT_UTIL_ZCL_KEY_ESTABLISH_IND        0xE1

/***************************************************************************************************
 * DEBUG COMMANDS
 ***************************************************************************************************/

/* SREQ/SRSP: */
#define MT_DEBUG_SET_THRESHOLD               0x00

/* AREQ */
#define MT_DEBUG_MSG                         0x80

/***************************************************************************************************
 * APP COMMANDS
 ***************************************************************************************************/

/* SREQ/SRSP: */
#define MT_APP_MSG                           0x00
#define MT_APP_USER_TEST                     0x01

/* SRSP */
#define MT_APP_RSP                           0x80

/***************************************************************************************************
* FILE SYSTEM COMMANDS
***************************************************************************************************/
#define MT_OTA_FILE_READ_REQ                  0x00
#define MT_OTA_NEXT_IMG_REQ                   0x01

#define MT_OTA_FILE_READ_RSP                  0x80
#define MT_OTA_NEXT_IMG_RSP                   0x81
#define MT_OTA_STATUS_IND                     0x82
  
/*
 *  Definitions to allow conditional compiling -
 *  To use these in an embedded environment include them as a compiler
 *  option (ex. "-DMT_NWK_FUNC" )
 */

/* Task Event IDs - bit masks */
#define MT_ZTOOL_SERIAL_RCV_CHAR        0x0001
#define MT_ZTOOL_SERIAL_RCV_BUFFER_FULL 0x0002
#define MT_SERIAL_ZTOOL_XMT_READY       0x0004
#define MT_ZAPP_SERIAL_RCV_CHAR         MT_ZTOOL_SERIAL_RCV_CHAR
#define MT_ZAPP_SERIAL_RCV_BUFFER_FULL  MT_ZTOOL_SERIAL_RCV_BUFFER_FULL
#define MT_SERIAL_ZAPP_XMT_READY        MT_SERIAL_ZTOOL_XMT_READY
#define MT_AF_EXEC_EVT                  0x0008

#define MT_MSG_SEQUENCE_EVT             0x0040
#define MT_KEYPRESS_POLL_EVT            0x0080

/* SYS_OSAL_EVENT ID's */
#define MT_SYS_OSAL_EVENT_0             0x0800
#define MT_SYS_OSAL_EVENT_1             0x0400
#define MT_SYS_OSAL_EVENT_2             0x0200
#define MT_SYS_OSAL_EVENT_3             0x0100
#define MT_SYS_OSAL_EVENT_MASK (MT_SYS_OSAL_EVENT_0 | MT_SYS_OSAL_EVENT_1 | \
                                MT_SYS_OSAL_EVENT_2 | MT_SYS_OSAL_EVENT_3)

/* Message Command IDs */
#define CMD_SERIAL_MSG                  0x01
#define CMD_DEBUG_MSG                   0x02
#define CMD_TX_MSG                      0x03
#define CB_FUNC                         0x04
#define CMD_SEQUENCE_MSG                0x05
#define CMD_DEBUG_STR                   0x06
#define AF_INCOMING_MSG_FOR_MT          0x0F

/* Error Response IDs */
#define UNRECOGNIZED_COMMAND            0x00
#define UNSUPPORTED_COMMAND             0x01
#define RECEIVE_BUFFER_FULL             0x02

/* Serial Message Command IDs */
#define SPI_CMD_DEBUG_MSG               0x4003
#define SPI_CMD_TRACE_SUB               0x0004
#define SPI_CMD_TRACE_MSG               0x4004

/* User CMD IDs */
#define SPI_CMD_USER0                   0x000A
#define SPI_CMD_USER1                   0x000B
#define SPI_CMD_USER2                   0x000C
#define SPI_CMD_USER3                   0x000D
#define SPI_CMD_USER4                   0x000E
#define SPI_CMD_USER5                   0x000F


#define SPI_SYS_STRING_MSG              0x0015
#define SPI_CMD_SYS_NETWORK_START       0x0020
#define SPI_CMD_ZIGNET_DATA             0x0022

/* system command response */
#define SPI_CB_SYS_CALLBACK_SUB_RSP     0x1006
#define SPI_CB_SYS_PING_RSP             0x1007
#define SPI_CB_SYS_GET_DEVICE_INFO_RSP  0x1014
#define SPI_CB_SYS_KEY_EVENT_RSP        0x1016
#define SPI_CB_SYS_HEARTBEAT_RSP        0x1017
#define SPI_CB_SYS_LED_CONTROL_RSP      0x1019

/* Message Sequence definitions */
#define SPI_CMD_SEQ_START               0x0600
#define SPI_CMD_SEQ_WAIT                0x0601
#define SPI_CMD_SEQ_END                 0x0602
#define SPI_CMD_SEQ_RESET               0x0603
#define DEFAULT_WAIT_INTERVAL           5000      //5 seconds

/* Serial Message Command Routing Bits */
#define SPI_RESPONSE_BIT                0x1000
#define SPI_SUBSCRIPTION_BIT            0x2000
#define SPI_DEBUGTRACE_BIT              0x4000

#define SPI_0DATA_MSG_LEN                5
#define SPI_RESP_MSG_LEN_DEFAULT         6

#define LEN_MAC_BEACON_MSDU             15
#define LEN_MAC_COORDEXTND_ADDR          8
#define LEN_MAC_ATTR_BYTE                1
#define LEN_MAC_ATTR_INT                 2

#define SOP_FIELD                        0
#define CMD_FIELD_HI                     1
#define CMD_FIELD_LO                     2
#define DATALEN_FIELD                    3
#define DATA_BEGIN                       4

/* MT PACKET (For Test Tool): FIELD IDENTIFIERS */
#define MT_MAC_CB_ID                0
#define MT_OFFSET                   1
#define MT_SOP_FIELD                MT_OFFSET + SOP_FIELD
#define MT_CMD_FIELD_HI             MT_OFFSET + CMD_FIELD_HI
#define MT_CMD_FIELD_LO             MT_OFFSET + CMD_FIELD_LO
#define MT_DATALEN_FIELD            MT_OFFSET + DATALEN_FIELD
#define MT_DATA_BEGIN               MT_OFFSET + DATA_BEGIN

#define MT_INFO_HEADER_LEN         1
#define MT_RAM_READ_RESP_LEN       0x02
#define MT_RAM_WRITE_RESP_LEN      0x01

/* Defines for the fields in the AF structures */
#define AF_INTERFACE_BITS          0x07
#define AF_INTERFACE_OFFSET        0x05
#define AF_APP_DEV_VER_MASK        0x0F
#define AF_APP_FLAGS_MASK          0x0F
#define AF_TRANSTYPE_MASK          0x0F
#define AF_TRANSDATATYPE_MASK      0x0F

/* Defines for the semi-precision structure */
#define AF_SEMI_PREC_SIGN          0x8000
#define AF_SEMI_PREC_EXPONENT      0x7C00
#define AF_SEMI_PREC_MANTISSA      0x03FF
#define AF_SEMI_PREC_SIGN_OFFSET   0x0F
#define AF_SEMI_PREC_EXP_OFFSET    0x0A

typedef struct mtMessage_s
{
	uint8_t sof;
	uint8_t dataLength;
	uint8_t cmdType;
	uint8_t cmdId;
	uint8_t pData[256];
} mtMessage_t;

#ifdef __cplusplus
extern "C" {
#endif

void MTProcInit();
void StartMT_Af();
void MTProcClose();
void MTBuildAndSendCmd(uint8_t cmdType, uint8_t cmdId, uint8_t dataLen, uint8_t *pData);
void mtBuildAndSendCmd(mtMessage_t *message);
void ByteReverse(uint8_t *pBuf, size_t BufLen, uint8_t *dest);
int registerSubsystemCb(uint8_t subsystem, uint8_t (*callback)(uint8_t*));
void setOutOfBandMessageCallBack(int (*CallBack)(uint8_t *));

#ifdef __cplusplus
}
#endif

#endif



