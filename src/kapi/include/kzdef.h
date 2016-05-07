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
#ifndef KZDEF_H
#define KZDEF_H
#include <stdint.h>
#include "kutil.h"

class AFMessage
{
public:
	virtual 					~AFMessage()					   {};
	virtual uint16_t 	dstAddr() 			const 	= 0;
	virtual void 			dstAddr(uint16_t) 			= 0;
	virtual uint8_t 	dstEndpoint() 	const 	= 0;
	virtual void			dstEndpoint(uint8_t) 		= 0;
	virtual uint16_t 	groupId() 			const		= 0;
	virtual void 			groupId(uint16_t)				= 0;
	virtual uint16_t 	clusterId() 		const 	= 0;
	virtual void 			clusterId(uint16_t)			= 0;
	virtual uint8_t 	srcEndpoint() 	const 	= 0;
	virtual void		 	srcEndpoint(uint8_t)		= 0;
	virtual uint16_t 	srcAddr()				const 	= 0;
	virtual void 			srcAddr(uint16_t)				= 0;
	virtual uint8_t 	wasBroadCast() 	const 	= 0;
	virtual void 			wasBroadCast(uint8_t)		= 0;
	virtual uint8_t 	linkQuality()		const 	= 0;
	virtual void		 	linkQuality(uint8_t)		= 0;
	virtual uint8_t 	securityUse()		const 	= 0;
	virtual void 			securityUse(uint8_t)		= 0;
	virtual uint32_t 	timeStamp()			const 	= 0;
	virtual void 			timeStamp(uint32_t)			= 0;
	virtual uint8_t	 	tranSeqNum()		const 	= 0;
	virtual void	 		tranSeqNum(uint8_t)			= 0;
	virtual uint8_t 	asduLen() 			const 	= 0;
	virtual void 			asduLen(uint8_t)				= 0;
	virtual uint8_t*	asdu() 									= 0;
	virtual uint8_t 	radius() 				const 	= 0;
	virtual void 			radius(uint8_t)					= 0;
	virtual uint8_t 	options() 			const 	= 0;
	virtual void 			options(uint8_t)				= 0;
	virtual uint8_t 	dstAddrMode() 	const 	= 0;
	virtual void 			dstAddrMode(uint8_t)		= 0;
};

class AFMessage_EXT
{
public:
	virtual 					~AFMessage_EXT()				  {};
	virtual uint64_t 	dstAddr() 			const 	= 0;
	virtual void 			dstAddr(uint64_t) 			= 0;
	virtual uint8_t 	dstEndpoint() 	const 	= 0;
	virtual void			dstEndpoint(uint8_t) 		= 0;
	virtual uint8_t 	addrMode() 	const 			= 0;
	virtual void 			addrMode(uint8_t)				= 0;

	virtual uint16_t 	srcPanId() 			const 		= 0;
	virtual void			srcPanId(uint16_t) 				= 0;
	virtual uint16_t 	dstPanId() 			const 		= 0;
	virtual void			dstPanId(uint16_t) 				= 0;

	virtual uint16_t 	groupId() 			const		= 0;
	virtual void 			groupId(uint16_t)				= 0;
	virtual uint16_t 	clusterId() 		const 	= 0;
	virtual void 			clusterId(uint16_t)			= 0;
	virtual uint8_t 	srcEndpoint() 	const 	= 0;
	virtual void		 	srcEndpoint(uint8_t)		= 0;
	virtual uint64_t 	srcAddr()				const 	= 0;
	virtual void 			srcAddr(uint64_t)				= 0;
	virtual uint8_t 	wasBroadCast() 	const 	= 0;
	virtual void 			wasBroadCast(uint8_t)		= 0;
	virtual uint8_t 	linkQuality()		const 	= 0;
	virtual void		 	linkQuality(uint8_t)		= 0;
	virtual uint8_t 	securityUse()		const 	= 0;
	virtual void 			securityUse(uint8_t)		= 0;
	virtual uint32_t 	timeStamp()			const 	= 0;
	virtual void 			timeStamp(uint32_t)			= 0;
	virtual uint8_t	 	tranSeqNum()		const 	= 0;
	virtual void	 		tranSeqNum(uint8_t)			= 0;
	virtual uint16_t 	asduLen() 			const 	= 0;
	virtual void 			asduLen(uint16_t)				= 0;
	virtual uint8_t*	asdu() 									= 0;
	virtual uint8_t 	radius() 				const 	= 0;
	virtual void 			radius(uint8_t)					= 0;
	virtual uint8_t 	options() 			const 	= 0;
	virtual void 			options(uint8_t)				= 0;
};

//class afmessage_t : public AFMessage
//{
//private:
//	uint16_t 	DstAddr_						{};
//	uint8_t 	DstEndPoint_				{};
//	uint16_t 	GroupID_						{};
//	uint16_t 	ClusterID_					{};
//	uint8_t 	SrcEndPoint_				{};
//	uint16_t 	SrcAddr_						{};
//	uint8_t 	WasBroadCast_				{};
//	uint8_t 	LinkQuality_				{};
//	uint8_t 	SecurityUse_				{};
//	uint32_t 	TimeStamp_					{};
//	uint8_t	 	TranSeqNum_					{};
//	uint8_t 	asduLen_						{};
//	uint8_t 	asdu_[255]					{};
//	uint8_t 	Radius_							{};
//	uint8_t 	Options_						{};
//	uint8_t 	dstAddrMode_				{};
//public:
//						~afmessage_t()										{}
//	uint16_t 	dstAddr() 			const 		override { 	return DstAddr_;}
//	void 			dstAddr(uint16_t i) 			override { 	DstAddr_ = i;}
//	uint8_t 	dstEndpoint() 	const 		override { 	return DstEndPoint_;}
//	void			dstEndpoint(uint8_t i) 		override { 	DstEndPoint_ = i; }
//	uint16_t 	groupId() 			const			override { 	return GroupID_;}
//	void 			groupId(uint16_t i)				override { 	GroupID_ = i;}
//	uint16_t 	clusterId() 		const 		override { 	return ClusterID_;}
//	void 			clusterId(uint16_t i)			override { 	ClusterID_ = i;}
//	uint8_t 	srcEndpoint() 	const 		override { 	return SrcEndPoint_;}
//	void		 	srcEndpoint(uint8_t i)		override { 	SrcEndPoint_ = i;}
//	uint16_t 	srcAddr()				const 		override { 	return SrcAddr_;}
//	void 			srcAddr(uint16_t i )			override { 	SrcAddr_ = i;}
//	uint8_t 	wasBroadCast() 	const 		override { 	return WasBroadCast_;}
//	void 			wasBroadCast(uint8_t i)		override { 	WasBroadCast_ = i;}
//	uint8_t 	linkQuality()		const 		override { 	return LinkQuality_;}
//	void		 	linkQuality(uint8_t i)		override { 	LinkQuality_ = i;}
//	uint8_t 	securityUse()		const 		override {	return SecurityUse_;}
//	void 			securityUse(uint8_t i)		override {	SecurityUse_ = i;}
//	uint32_t 	timeStamp()			const 		override {	return TimeStamp_;}
//	void 			timeStamp(uint32_t i)			override {	TimeStamp_ = i;}
//	uint8_t	 	tranSeqNum()		const 		override {	return TranSeqNum_;}
//	void	 		tranSeqNum(uint8_t i)			override {	TranSeqNum_ = i;}
//	uint8_t 	asduLen() 			const 		override {	return asduLen_;}
//	void 			asduLen(uint8_t i)				override {	asduLen_ = i;}
//	uint8_t*	asdu()	 									override {	return asdu_;}
//	uint8_t 	radius() 				const 		override {	return Radius_;}
//	void 			radius(uint8_t i)					override {	Radius_ = i;}
//	uint8_t 	options() 			const 		override {	return Options_;}
//	void 			options(uint8_t i)				override {	Options_ = i;}
//	uint8_t 	dstAddrMode() 	const 		override {	return dstAddrMode_;}
//	void 			dstAddrMode(uint8_t i)		override {	dstAddrMode_ = i;}
//};

#endif
