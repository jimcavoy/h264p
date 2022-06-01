#include "nalu.h"

#include "naluimpl.h"
#include "sps.h"
#include "pps.h"
#include "slice.h"
#include "util.h"

#include <iterator>
#include <iostream>

std::shared_ptr<ThetaStream::NALUnitSPS> g_sps;
std::shared_ptr<ThetaStream::NALUnitPPS> g_pps;

using namespace std;
using namespace ThetaStream;

/////////////////////////////////////////////////////////////////////////////
// NALUnit
NALUnit::NALUnit(unsigned short len)
	:startcodeprefix_len_(len)
	, nal_ref_idc_(0)
{

}

NALUnit::~NALUnit()
{

}

NALUnit::NALUnit(const NALUnit &rhs)
	:startcodeprefix_len_(rhs.startcodeprefix_len())
	, nal_ref_idc_(rhs.nal_ref_idc_)
{
	NALUnit& temp = const_cast<NALUnit&>(rhs);
	pimpl_ = temp.pimpl_;
	for (size_t i = 0; i < rhs.rbsp_.size(); i++)
		rbsp_.push_back(rhs.rbsp_[i]);
}

NALUnit& NALUnit::operator =(const NALUnit& rhs)
{
	NALUnit temp(rhs);
	swap(temp);
	return *this;
}

void NALUnit::swap(NALUnit& src)
{
	std::swap(pimpl_, src.pimpl_);
	NALUnit::RawByteStreamPayload tmp;

	std::swap(rbsp_, src.rbsp_);
	std::swap(startcodeprefix_len_, src.startcodeprefix_len_);
	std::swap(nal_ref_idc_, src.nal_ref_idc_);
}

unsigned int NALUnit::size() const
{
	return (unsigned int)rbsp_.size();
}

void NALUnit::push_back(char c)
{
	rbsp_.push_back(c);
}

void NALUnit::parse()
{
	if (!rbsp_.empty())
	{
		unsigned char t = rbsp_[0] & 0x1F;
		nal_ref_idc_ = rbsp_[0] & 0x60;

		switch (t)
		{
		case 0x01: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitSlice);	break;
		case 0x02: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitDPA);	break;
		case 0x03: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitDPB);	break;
		case 0x04: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitDPC);	break;
		case 0x05: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitIDR);	break;
		case 0x06: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitSEI);	break;
		case 0x07: {
			if (g_sps != NULL)
				g_sps.reset();
			g_sps = std::shared_ptr<NALUnitSPS>(new NALUnitSPS);
			pimpl_ = std::shared_ptr<NALUnitImpl>(g_sps);
		}break;
		case 0x08: {
			if (g_pps != NULL)
				g_pps.reset();
			g_pps = std::shared_ptr<NALUnitPPS>(new NALUnitPPS);
			pimpl_ = std::shared_ptr<NALUnitImpl>(g_pps);
		}break;
		case 0x09: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitAUD);	break;
		case 0x0A: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitEOSEQ);	break;
		case 0x0B: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitEOStream);	break;
		case 0x0C: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitFill);	break;
		case 0x0D: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitSPSE);	break;
		case 0x0E: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitPrefixNALU);	break;
		case 0x0F: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitSubsetSPS);	break;
		case 0x13: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitSliceWithoutPartitioning); break;
		case 0x14: pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitSliceExtension); break;
		case 0x10: case 0x11: case 0x12: case 0x15: case 0x16: case 0x17:
			pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitReserved(t)); break;
		case 0x00: case 0x18: case 0x19: case 0x1A:
		case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
			pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitUnspecified(t)); break;
		default:
			pimpl_ = std::shared_ptr<NALUnitImpl>(new NALUnitUnknown(t));
			std::cerr << "Unknown NAL Unit type=" << (int)t << endl;
		}
	}

	if (pimpl_.get() != NULL)
	{
		pimpl_->parse(rbsp_.begin(), rbsp_.end());
	}
}

unsigned short NALUnit::startcodeprefix_len() const
{
	return startcodeprefix_len_;
}

char NALUnit::nal_ref_idc() const
{
	return nal_ref_idc_;
}

void NALUnit::Accept(Loki::BaseVisitor& visitor)
{
	if (pimpl_.get() == NULL)
		return;
	pimpl_->Accept(visitor);
}