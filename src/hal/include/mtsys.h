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
#ifndef MTSYS_H
#define MTSYS_H
#include <stdint.h>
#include <stddef.h>
#define SYSPINGRSP_POS 3
#define SYSPINGRSP_LEN 2
#ifdef __cplusplus
extern "C" {
#endif
uint8_t MT_SysCommandProcessing(uint8_t *pBuf);
// === meant to replace the above functions
void mtsys_init();
int mt_sysping_req(uint8_t *returnBuffer, size_t bufferSize);
int mt_sys_wdtkick_req(uint8_t *returnBuffer, size_t bufferSize);
int mt_getextaddr_req(uint64_t *res);
int mt_sysreset_req();

namespace MT {
	namespace SYS {
		typedef struct
		{
			uint8_t status;
			uint16_t id;
			uint8_t offset;
			uint8_t len;
			uint8_t value[256];
		} NvItem_t ;

		int OsalNvWrite(NvItem_t &nvItem);
		int OsalNvRead(NvItem_t &nvItem);
	}
}
#ifdef __cplusplus
}
#endif
#endif
