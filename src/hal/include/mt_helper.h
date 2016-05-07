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
#ifndef MT_HELPER_H
#define MT_HELPER_H
#include <stdint.h>
#include "mtproc.h"
#ifdef __cplusplus
extern "C" {
#endif
void mtHelperInit();
//int 	request_channel(uint8_t command);
//uint8_t *getChannelBuffer(uint8_t command);
void 	release_channel();

int mtSend(mtMessage_t *mtMessage, uint8_t rspCmdId, void *returnBuffer, size_t bufferSize, void (*callback)(mtMessage_t*,void*,size_t));
int mtRecv(uint8_t *incoming);
// this mean to replace the mtSend API
//int __MTSEND__(mtMessage_t *mtMessage, uint8_t rspCmdId, void *returnBuffer, size_t bufferSize, void (*callback)(uint8_t*,void*,size_t));
int __MTSEND__(mtMessage_t *mtMessage, uint8_t *returnBuffer, size_t bufferSize, void (*callback)(mtMessage_t*,void*,size_t));
int __MT_OOB_SEND__(mtMessage_t *mtMessage, uint8_t *returnBuffer, size_t bufferSize, void (*callback)(mtMessage_t*,void*,size_t));

#ifdef __cplusplus
}
#endif
#endif
