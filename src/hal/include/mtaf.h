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
#ifndef MTAF_H
#define MTAF_H
#include "kzdef.h"

namespace kapi
{
	namespace mtaf
	{
		void register_AFMessage_EXT_cb	(int (*messageHandle_EXT)(AFMessage_EXT *));
		void register_AFMessage_cb (int (*messageHandle)(AFMessage *));
		uint8_t dataRequest(AFMessage_EXT * x);
	}
}
uint8_t MT_AfDataRequest(AFMessage * x, uint8_t * result =  nullptr);

uint8_t MT_AfCommandProcessing(uint8_t * pBuf);
uint8_t AfDataRequest(AFMessage * x);
void register_endpoint_TI(uint8_t endpoint_id, uint16_t profile_id,uint16_t device_id, uint8_t device_ver);
void register_interpan_cb_TI(uint8_t ep);
void interPanSet(uint8_t ch);
void intraPanSet();

#endif
