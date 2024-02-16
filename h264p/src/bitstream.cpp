#include "bitstream.h"

#include <iterator>

/*!
 ************************************************************************
 * \brief
 *    Converts RBSP to string of data bits
 * \param streamBuffer
 *          pointer to buffer containing data
 *  \param last_byte_pos
 *          position of the last byte containing data.
 * \return last_byte_pos
 *          position of the last byte pos. If the last-byte was entirely a stuffing byte,
 *          it is removed, and the last_byte_pos is updated.
 *
************************************************************************/

static int RBSPtoSODB(u_char *streamBuffer, int last_byte_pos)
{
	int ctr_bit, bitoffset;

	bitoffset = 0;
	//find trailing 1
	ctr_bit = (streamBuffer[last_byte_pos - 1] & (0x01 << bitoffset));   // set up control bit

	while (ctr_bit == 0)
	{   // find trailing 1 bit
		++bitoffset;
		if (bitoffset == 8)
		{
			if (last_byte_pos == 0)
				printf(" Panic: All zero data sequence in RBSP \n");
			//assert(last_byte_pos != 0);
			--last_byte_pos;
			bitoffset = 0;
		}
		ctr_bit = streamBuffer[last_byte_pos - 1] & (0x01 << (bitoffset));
	}
	return(last_byte_pos);
}

/////////////////////////////////////////////////////////////////////////////
// Bitstream
Bitstream::Bitstream(ThetaStream::NALUnitImpl::iterator first, ThetaStream::NALUnitImpl::iterator last, size_t len)
	: read_len(0)
	, frame_bitoffset(0)
	, ei_flag(0)
{
	using namespace std;
	streamBuffer = new u_char[len];
#ifdef _WIN32
	std::copy(first, last,
		stdext::checked_array_iterator<byte*>(streamBuffer, len));
#else
	ThetaStream::NALUnitImpl::iterator it;
	int i = 0;
	for (it = first; it != last; ++it)
	{
		if (i < len)
		{
			streamBuffer[i++] = *it;
		}
	}
#endif
	code_len = bitstream_length = RBSPtoSODB(streamBuffer, (int)len);
}

Bitstream::~Bitstream()
{
	delete[] streamBuffer;
}