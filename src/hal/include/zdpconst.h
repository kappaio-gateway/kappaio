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
#ifndef ZDPCONST_H
#define ZDPCONST_H
#define ZDO_EP 0   // Endpoint of ZDO
#define ZDO_PROFILE_ID 0

// IEEE_addr_req request types
#define ZDP_ADDR_REQTYPE_SINGLE     0
#define ZDP_ADDR_REQTYPE_EXTENDED   1

// ZDO Status Values
#define ZDP_SUCCESS            0x00  // Operation completed successfully
#define ZDP_INVALID_REQTYPE    0x80  // The supplied request type was invalid
#define ZDP_DEVICE_NOT_FOUND   0x81  // Reserved
#define ZDP_INVALID_EP         0x82  // Invalid endpoint value
#define ZDP_NOT_ACTIVE         0x83  // Endpoint not described by a simple desc.
#define ZDP_NOT_SUPPORTED      0x84  // Optional feature not supported
#define ZDP_TIMEOUT            0x85  // Operation has timed out
#define ZDP_NO_MATCH           0x86  // No match for end device bind
#define ZDP_NO_ENTRY           0x88  // Unbind request failed, no entry
#define ZDP_NO_DESCRIPTOR      0x89  // Child descriptor not available
#define ZDP_INSUFFICIENT_SPACE 0x8a  // Insufficient space to support operation
#define ZDP_NOT_PERMITTED      0x8b  // Not in proper state to support operation
#define ZDP_TABLE_FULL         0x8c  // No table space to support operation
#define ZDP_NOT_AUTHORIZED     0x8d  // Permissions indicate request not authorized
#define ZDP_BINDING_TABLE_FULL 0x8e  // No binding table space to support operation

#define ZDP_NETWORK_DISCRIPTOR_SIZE  8
#define ZDP_NETWORK_EXTENDED_DISCRIPTOR_SIZE  14
#define ZDP_RTG_DISCRIPTOR_SIZE      5
#define ZDP_BIND_DISCRIPTOR_SIZE  19

// Mgmt_Permit_Join_req fields
#define ZDP_MGMT_PERMIT_JOIN_REQ_DURATION 0
#define ZDP_MGMT_PERMIT_JOIN_REQ_TC_SIG   1
#define ZDP_MGMT_PERMIT_JOIN_REQ_SIZE     2

// Mgmt_Leave_req fields
#define ZDP_MGMT_LEAVE_REQ_REJOIN      1 << 7
#define ZDP_MGMT_LEAVE_REQ_RC          1 << 6   // Remove Children

// Mgmt_Lqi_rsp - (neighbor table) device type
#define ZDP_MGMT_DT_COORD  0x0
#define ZDP_MGMT_DT_ROUTER 0x1
#define ZDP_MGMT_DT_ENDDEV 0x2

// Mgmt_Lqi_rsp - (neighbor table) relationship
#define ZDP_MGMT_REL_PARENT  0x0
#define ZDP_MGMT_REL_CHILD   0x1
#define ZDP_MGMT_REL_SIBLING 0x2
#define ZDP_MGMT_REL_UNKNOWN 0x3

// Mgmt_Lqi_rsp - (neighbor table) unknown boolean value
#define ZDP_MGMT_BOOL_UNKNOWN 0x02

/*********************************************************************
 * Message/Cluster IDS
 */

// ZDO Cluster IDs
#define ZDO_RESPONSE_BIT_V1_0   ((uint8_t)0x80)
#define ZDO_RESPONSE_BIT        ((unsigned short)0x8000)

#define NWK_addr_req            ((unsigned short)0x0000)
#define IEEE_addr_req           ((unsigned short)0x0001)
#define Node_Desc_req           ((unsigned short)0x0002)
#define Power_Desc_req          ((unsigned short)0x0003)
#define Simple_Desc_req         ((unsigned short)0x0004)
#define Active_EP_req           ((unsigned short)0x0005)
#define Match_Desc_req          ((unsigned short)0x0006)
#define NWK_addr_rsp            (NWK_addr_req | ZDO_RESPONSE_BIT)
#define IEEE_addr_rsp           (IEEE_addr_req | ZDO_RESPONSE_BIT)
#define Node_Desc_rsp           (Node_Desc_req | ZDO_RESPONSE_BIT)
#define Power_Desc_rsp          (Power_Desc_req | ZDO_RESPONSE_BIT)
#define Simple_Desc_rsp         (Simple_Desc_req | ZDO_RESPONSE_BIT)
#define Active_EP_rsp           (Active_EP_req | ZDO_RESPONSE_BIT)
#define Match_Desc_rsp          (Match_Desc_req | ZDO_RESPONSE_BIT)

#define Complex_Desc_req        ((unsigned short)0x0010)
#define User_Desc_req           ((unsigned short)0x0011)
#define Discovery_Cache_req     ((unsigned short)0x0012)
#define Device_annce            ((unsigned short)0x0013)
#define User_Desc_set           ((unsigned short)0x0014)
#define Server_Discovery_req    ((unsigned short)0x0015)
#define Complex_Desc_rsp        (Complex_Desc_req | ZDO_RESPONSE_BIT)
#define User_Desc_rsp           (User_Desc_req | ZDO_RESPONSE_BIT)
#define Discovery_Cache_rsp     (Discovery_Cache_req | ZDO_RESPONSE_BIT)
#define User_Desc_conf          (User_Desc_set | ZDO_RESPONSE_BIT)
#define Server_Discovery_rsp    (Server_Discovery_req | ZDO_RESPONSE_BIT)

#define End_Device_Bind_req     ((unsigned short)0x0020)
#define Bind_req                ((unsigned short)0x0021)
#define Unbind_req              ((unsigned short)0x0022)
#define Bind_rsp                (Bind_req | ZDO_RESPONSE_BIT)
#define End_Device_Bind_rsp     (End_Device_Bind_req | ZDO_RESPONSE_BIT)
#define Unbind_rsp              (Unbind_req | ZDO_RESPONSE_BIT)

#define Mgmt_NWK_Disc_req       ((unsigned short)0x0030)
#define Mgmt_Lqi_req            ((unsigned short)0x0031)
#define Mgmt_Rtg_req            ((unsigned short)0x0032)
#define Mgmt_Bind_req           ((unsigned short)0x0033)
#define Mgmt_Leave_req          ((unsigned short)0x0034)
#define Mgmt_Direct_Join_req    ((unsigned short)0x0035)
#define Mgmt_Permit_Join_req    ((unsigned short)0x0036)
#define Mgmt_NWK_Update_req     ((unsigned short)0x0038)
#define Mgmt_NWK_Disc_rsp       (Mgmt_NWK_Disc_req | ZDO_RESPONSE_BIT)
#define Mgmt_Lqi_rsp            (Mgmt_Lqi_req | ZDO_RESPONSE_BIT)
#define Mgmt_Rtg_rsp            (Mgmt_Rtg_req | ZDO_RESPONSE_BIT)
#define Mgmt_Bind_rsp           (Mgmt_Bind_req | ZDO_RESPONSE_BIT)
#define Mgmt_Leave_rsp          (Mgmt_Leave_req | ZDO_RESPONSE_BIT)
#define Mgmt_Direct_Join_rsp    (Mgmt_Direct_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_Permit_Join_rsp    (Mgmt_Permit_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_NWK_Update_notify  (Mgmt_NWK_Update_req | ZDO_RESPONSE_BIT)

#endif
