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
#include "zdo_archive.h"
#include "kjson.h"
#include "kglobal.h"
#include <memory>

using Device = aps::Device;
using Endpoint = aps::Endpoint;
using Cluster = aps::Cluster;
using Attribute	= aps::Attribute;
using NodeDescriptor_t = aps::NodeDescriptor_t;
using SimpleDescriptor = aps::SimpleDescriptor;
using namespace kapi;
using namespace std;

int saveDevAddresses(Device *pDevice)
{
	int rtn = 0;
	char ieeeAddrStr[17], nwkAddrStr[5];//, label[20];
	uint64_t ieeeAddr = pDevice->ieee_id();
	uint16_t nwkAddr  = pDevice->id();
	kIntToStr(ieeeAddrStr,(void*)(&ieeeAddr), 8);
	kIntToStr(nwkAddrStr, (void*)(&nwkAddr),2);

	JSON zar{ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION};

	if (!zar.good())
	{
		zar = JSON{JSONType::JSON_OBJECT};
	}

	JSON& newDev = zar[ieeeAddrStr];

	newDev["ieeeAddr"] = ieeeAddrStr;
	newDev["nwkAddr"] = nwkAddrStr;
	zar.toFile(ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION);
	return 0;
}

int saveIds(Device& d)
{
	JSON zar{ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION};
	string ieeeAddrStr = IntToHexStr(d.ieee_id());
	string nwkAddrStr  = IntToHexStr(d.id());
	if (!zar.good())
	{
		zar = JSON{JSONType::JSON_OBJECT};
	}

	JSON& newDev = zar[ieeeAddrStr];
	newDev["ieeeAddr"] = ieeeAddrStr;
	newDev["nwkAddr"] = nwkAddrStr;

	zar.toFile(ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION);

	return 0;
}

int saveIds(Endpoint& e)
{
	saveIds(*(e.parent()));
	string ieeeAddrStr = IntToHexStr(e.parent()->ieee_id());
	string epStr = IntToHexStr(e.id());

	JSON zar{ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION};
	if (!zar.good()) {
		zar = JSON{JSONType::JSON_OBJECT};
	}

	JSON& endpointList = zar[ieeeAddrStr]["endpointList"];
	JSON& newDev = endpointList[epStr];
	newDev["Id"] = epStr;
	zar.toFile(ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION);

	return 0;
}
int saveEndpointApdu(Endpoint *pEndpoint, const char *label, uint8_t *apdu, size_t apduLength)
{
	saveIds(*pEndpoint);

	string ieeeAddrStr = IntToHexStr(pEndpoint->parent()->ieee_id());
	string epIdStr = IntToHexStr(pEndpoint->id());
	char apduString[apduLength << 1 + 1];
//	std::unique_ptr<char[]> apduString_p(new char(apduLength << 1 + 1));
//	char* apduString = apduString_p.get();
	size_t stringLen = kByteToHexString(apduString, apdu,apduLength);

	JSON zar{ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION};

	if (!zar.good()) zar = JSON{JSONType::JSON_OBJECT};

	JSON& ep = zar[ieeeAddrStr]["endpointList"][epIdStr];

	ep[label] = apduString;
	zar.toFile(ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION);

	return 0;
}
int saveDevApdu(Device *pDevice, const char* label, uint8_t *apdu, size_t apduLength)
{
	saveDevAddresses(pDevice);
	char apduString[apduLength << 1 + 1];
	size_t stringLen = kByteToHexString(apduString, apdu,apduLength);

	JSON zar{ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION};
	if (!zar.good()) zar = JSON{JSONType::JSON_OBJECT};

	JSON& dev = zar[IntToHexStr(pDevice->ieee_id())];

	dev[label] = apduString;
	zar.toFile(ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION);
	return 0;
}

int removeDev(uint64_t ieeeAddr)
{
	JSON zar{ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION};
	if (!zar.good())
		return -1;

	zar.erase(IntToHexStr(ieeeAddr));
	zar.toFile(ZDO::ARCHIVE::_ZDO_ARCHIVE_LOCATION);
	return 0;
}


