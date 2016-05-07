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
#ifndef _ZDO_ARCHIVE_H
#define _ZDO_ARCHIVE_H
#include "apsdb.h"
#include <stdio.h>
#include "kjson.h"
//#define _ZDO_ARCHIVE_LOCATION "/usr/lib/rsserial/zdo_archive.json"

int saveDevApdu(aps::Device *pDevice, const char *label, uint8_t *apdu, size_t apduLength);
int saveEndpointApdu(aps::Endpoint *pEndpoint, const char *label, uint8_t *apdu, size_t apduLength);
int removeDev(uint64_t ieeeAddr);
namespace ZDO
{
	namespace ARCHIVE
	{
		const char _ZDO_ARCHIVE_LOCATION[] =  "/usr/lib/rsserial/zdo_archive.json";

		inline void clear() {
			//remove(_ZDO_ARCHIVE_LOCATION);
			kapi::JSON zar{kapi::JSONType::JSON_OBJECT};
			zar.toFile(_ZDO_ARCHIVE_LOCATION);
		}
	}
}
#endif
