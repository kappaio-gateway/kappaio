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
#ifndef MTINTF_H
#define MTINTF_H
#include <stdint.h>
#include <stddef.h>

#define MT_SOF 0xFE
#define POLL 0x00
#define SREQ 0x20
#define AREQ 0x40
#define SRSP 0x60
#define MAX_MTCMD_LEN 256
#define MIN_MTCMD_LEN 4
#define MAX_MTDAT_LEN 253
#define MAX_MTMSG_Q_LEN 32
#define MAX_RX_LEN 255
//#define SERIAL_DEVICE "/dev/ttyS0"
#define SERIAL_DEVICE "/dev/tty2531"
#ifdef __cplusplus
extern "C" {
#endif
void MTIntfInit(void (*callback)(uint8_t*));
void MTIntfClose();
void SerialSend(uint8_t *pbuf, size_t len);
void resetInterface();
#ifdef __cplusplus
}
#endif
#endif
