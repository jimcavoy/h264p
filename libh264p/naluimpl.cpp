#include "naluimpl.h"

#include "util.h"

#include <iterator>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

using namespace std;
using namespace ThetaStream;



NALUnitImpl::NALUnitImpl(uint8_t nut)
	:nal_unit_type(nut)
{

}

NALUnitImpl::~NALUnitImpl()
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitDPA

NALUnitDPA::NALUnitDPA()
	:NALUnitImpl(0x02)
{

}

NALUnitDPA::~NALUnitDPA()
{

}

void NALUnitDPA::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitDPB

NALUnitDPB::NALUnitDPB()
	:NALUnitImpl(0x03)
{

}

NALUnitDPB::~NALUnitDPB()
{

}

void NALUnitDPB::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitDPC

NALUnitDPC::NALUnitDPC()
	:NALUnitImpl(0x04)
{

}

NALUnitDPC::~NALUnitDPC()
{

}

void NALUnitDPC::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}


/////////////////////////////////////////////////////////////////////////////
// NALUnitSEI

NALUnitSEI::NALUnitSEI()
	:NALUnitImpl(0x06)
{

}

NALUnitSEI::~NALUnitSEI()
{

}

void NALUnitSEI::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{
	NALUnitImpl::RawByteStreamPayload rbsp;
	NALUnitImpl::RawByteStreamPayload payload;
	removeEmulationPrevention3Bytes(std::back_inserter(rbsp), first, last);
	size_t payloadSz = 0;
	int offset = 1;
	int state = 0; // 0 = type, 1 = size, 2 payload

	// start at index 1 to skip over NAL Unit type.
	for (size_t i = 1; i < rbsp.size(); i++)
	{
		switch (state)
		{
		case 0: 
		{
			unsigned char type = rbsp[i];
			if (type >= 0 && type < 47)
			{
				payload.push_back(rbsp[i]);
				state = 1;
			}
			else
				return;
		}
		break;
		case 1: 
		{
			unsigned char sz = rbsp[i];
			if (sz == 0xff)
			{
				payloadSz += 0xff;
			}
			else
			{
				payloadSz += sz;
				state = 2;
			}
			payload.push_back(rbsp[i]);
			offset++;

			if (payloadSz > rbsp.size())
				return;
		}
		break;
		case 2:
		{
			state = 0;
			SEI sei;
			payload.insert(payload.begin() + offset, &rbsp[i], &rbsp[i + payloadSz]);
			std::copy(payload.begin(), payload.end(), back_inserter(sei.rbsp_));
			sei.parse();
			_sei_payloads.push_back(std::forward<SEI>(sei));
			i += (payloadSz - 1);
			payload.clear();
			payloadSz = 0;
			offset = 1;
		}
		break;
		}
	}
}

NALUnitSEI::sei_payloads_type::iterator NALUnitSEI::begin()
{
	return _sei_payloads.begin();
}

NALUnitSEI::sei_payloads_type::iterator NALUnitSEI::end()
{
	return _sei_payloads.end();
}

/////////////////////////////////////////////////////////////////////////////
// NALUnitAUD

NALUnitAUD::NALUnitAUD()
	:NALUnitImpl(0x09)
	,primary_pic_type_(0)
{

}

NALUnitAUD::~NALUnitAUD()
{

}

void NALUnitAUD::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{
	NALUnitImpl::iterator it = first;
	it++;
	if (it != last)
		primary_pic_type_ = *it;
}

unsigned short NALUnitAUD::primary_pic_type() const
{
	unsigned short r = 0;

	switch (primary_pic_type_) {
	case 0x10: r = 0; break;
	case 0x30: r = 1; break;
	case 0x50: r = 2; break;
	case 0x70: r = 3; break;
	case 0x90: r = 4; break;
	case 0xb0: r = 5; break;
	case 0xd0: r = 6; break;
	case 0xf0: r = 7; break;
	}
	return r;
}

/////////////////////////////////////////////////////////////////////////////
// NALUnitEOSEQ

NALUnitEOSEQ::NALUnitEOSEQ()
	:NALUnitImpl(0x0A)
{

}

NALUnitEOSEQ::~NALUnitEOSEQ()
{

}

void NALUnitEOSEQ::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitEOStream

NALUnitEOStream::NALUnitEOStream()
	:NALUnitImpl(0x0B)
{

}

NALUnitEOStream::~NALUnitEOStream()
{

}

void NALUnitEOStream::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitFill

NALUnitFill::NALUnitFill()
	:NALUnitImpl(0x0C)
{

}

NALUnitFill::~NALUnitFill()
{

}

void NALUnitFill::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitSPSE

NALUnitSPSE::NALUnitSPSE()
	:NALUnitImpl(0x0D)
{

}

NALUnitSPSE::~NALUnitSPSE()
{

}

void NALUnitSPSE::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitPrefixNALU

NALUnitPrefixNALU::NALUnitPrefixNALU()
	:NALUnitImpl(0x0E)
{

}

NALUnitPrefixNALU::~NALUnitPrefixNALU()
{

}

void NALUnitPrefixNALU::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitSubsetSPS

NALUnitSubsetSPS::NALUnitSubsetSPS()
	:NALUnitImpl(0x0F)
{

}

NALUnitSubsetSPS::~NALUnitSubsetSPS()
{

}

void NALUnitSubsetSPS::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitSliceWithoutPartitioning

NALUnitSliceWithoutPartitioning::NALUnitSliceWithoutPartitioning()
	:NALUnitImpl(0x13)
{

}

NALUnitSliceWithoutPartitioning::~NALUnitSliceWithoutPartitioning()
{

}

void NALUnitSliceWithoutPartitioning::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitSliceExtension

NALUnitSliceExtension::NALUnitSliceExtension()
	:NALUnitImpl(0x14)
{

}

NALUnitSliceExtension::~NALUnitSliceExtension()
{

}

void NALUnitSliceExtension::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

/////////////////////////////////////////////////////////////////////////////
// NALUnitUnspecified

NALUnitUnspecified::NALUnitUnspecified(unsigned char type)
	:NALUnitImpl(type)
{

}

NALUnitUnspecified::~NALUnitUnspecified()
{

}

void NALUnitUnspecified::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

unsigned char NALUnitUnspecified::type() const
{
	return nal_unit_type;
}

/////////////////////////////////////////////////////////////////////////////
// NALUnitReserved

NALUnitReserved::NALUnitReserved(unsigned char type)
	:NALUnitImpl(type)
{

}

NALUnitReserved::~NALUnitReserved()
{

}

void NALUnitReserved::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

unsigned char NALUnitReserved::type() const
{
	return nal_unit_type;
}
/////////////////////////////////////////////////////////////////////////////
// NALUnitUnknown

NALUnitUnknown::NALUnitUnknown(unsigned char type)
	:NALUnitImpl(type)
{

}

NALUnitUnknown::~NALUnitUnknown()
{

}

void NALUnitUnknown::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{

}

unsigned char NALUnitUnknown::type() const
{
	return nal_unit_type;
}