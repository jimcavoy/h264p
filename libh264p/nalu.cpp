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

class NALUnit::Impl
{
public:
	Impl(unsigned short startcodeprefix_len=4)
		:_startcodeprefix_len(startcodeprefix_len)
	{

	}
	Impl(const Impl& other)
		:_startcodeprefix_len(other._startcodeprefix_len)
		, _nal_ref_idc(other._nal_ref_idc)
	{
		NALUnit::Impl& temp = const_cast<NALUnit::Impl&>(other);
		_pimpl = temp._pimpl;
		std::copy(other._rbsp.begin(), other._rbsp.end(), std::back_inserter(_rbsp));
	}
	~Impl() {}

	void parse()
	{
		if (!_rbsp.empty())
		{
			uint8_t t = _rbsp[0] & 0x1F;
			_nal_ref_idc = _rbsp[0] & 0x60;

			switch (t)
			{
			case 0x01: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitSlice);	break;
			case 0x02: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitDPA);	break;
			case 0x03: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitDPB);	break;
			case 0x04: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitDPC);	break;
			case 0x05: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitIDR);	break;
			case 0x06: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitSEI);	break;
			case 0x07: {
				if (g_sps != NULL)
					g_sps.reset();
				g_sps = std::shared_ptr<NALUnitSPS>(new NALUnitSPS);
				_pimpl = std::shared_ptr<NALUnitImpl>(g_sps);
			}break;
			case 0x08: {
				if (g_pps != NULL)
					g_pps.reset();
				g_pps = std::shared_ptr<NALUnitPPS>(new NALUnitPPS);
				_pimpl = std::shared_ptr<NALUnitImpl>(g_pps);
			}break;
			case 0x09: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitAUD);	break;
			case 0x0A: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitEOSEQ);	break;
			case 0x0B: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitEOStream);	break;
			case 0x0C: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitFill);	break;
			case 0x0D: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitSPSE);	break;
			case 0x0E: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitPrefixNALU);	break;
			case 0x0F: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitSubsetSPS);	break;
			case 0x13: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitSliceWithoutPartitioning); break;
			case 0x14: _pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitSliceExtension); break;
			case 0x10: case 0x11: case 0x12: case 0x15: case 0x16: case 0x17:
				_pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitReserved(t)); break;
			case 0x00: case 0x18: case 0x19: case 0x1A:
			case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
				_pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitUnspecified(t)); break;
			default:
				_pimpl = std::shared_ptr<NALUnitImpl>(new NALUnitUnknown(t));
				std::cerr << "Unknown NAL Unit type=" << (int)t << endl;
			}
		}

		if (_pimpl.get() != NULL)
		{
			_pimpl->parse(_rbsp.begin(), _rbsp.end());
		}
	}

public:
	std::shared_ptr<NALUnitImpl> _pimpl;
	NALUnit::RawByteStreamPayload _rbsp;
	unsigned short _startcodeprefix_len;
	uint8_t _nal_ref_idc{};
};

/////////////////////////////////////////////////////////////////////////////
// NALUnit
NALUnit::NALUnit(unsigned short len)
	:_pimpl(std::make_unique<NALUnit::Impl>(len))
{
	
}

NALUnit::~NALUnit()
{

}

NALUnit::NALUnit(const NALUnit &other)
	:_pimpl(std::make_unique<NALUnit::Impl>(*other._pimpl))
{
	
}

NALUnit& ThetaStream::NALUnit::operator=(NALUnit&& rhs) noexcept
{
	if (this != &rhs)
	{
		_pimpl = std::move(rhs._pimpl);
	}
	return *this;
}

ThetaStream::NALUnit::NALUnit(NALUnit&& other) noexcept
{
	*this = std::move(other);
}

NALUnit& NALUnit::operator =(const NALUnit& rhs)
{
	if (this != &rhs)
	{
		_pimpl.reset(new NALUnit::Impl(*rhs._pimpl));
	}
	return *this;
}

unsigned int NALUnit::size() const
{
	return (unsigned int)_pimpl->_rbsp.size();
}

void NALUnit::push_back(uint8_t c)
{
	_pimpl->_rbsp.push_back(c);
}

void NALUnit::parse()
{
	_pimpl->parse();
}

NALUnit::iterator ThetaStream::NALUnit::begin()
{
	return _pimpl->_rbsp.begin();
}

NALUnit::iterator ThetaStream::NALUnit::end()
{
	return _pimpl->_rbsp.end();
}

const uint8_t* ThetaStream::NALUnit::data() const
{
	return _pimpl->_rbsp.data();
}

unsigned short NALUnit::startcodeprefix_len() const
{
	return _pimpl->_startcodeprefix_len;
}

uint8_t NALUnit::nal_ref_idc() const
{
	return _pimpl->_nal_ref_idc;
}

void NALUnit::Accept(Loki::BaseVisitor& visitor)
{
	if (_pimpl->_pimpl.get() == nullptr)
		return;
	_pimpl->_pimpl->Accept(visitor);
}