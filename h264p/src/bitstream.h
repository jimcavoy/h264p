#ifndef BITSTREAM_H
#define BITSTREAM_H

#include "naluimpl.h"

namespace ThetaStream
{
	/// @brief Converts a Raw Byte Sequence into a bit stream.
	class Bitstream
	{
	public:
		Bitstream(ThetaStream::NALUnitImpl::iterator first, ThetaStream::NALUnitImpl::iterator last, size_t len);
		~Bitstream();
	public:
		// CABAC Decoding
		int           read_len;           ///< actual position in the codebuffer, CABAC only
		int           code_len;           ///< overall codebuffer length, CABAC only
		// CAVLC Decoding
		int           frame_bitoffset;    ///< actual position in the codebuffer, bit-oriented, CAVLC only
		int           bitstream_length;   ///< over codebuffer lnegth, byte oriented, CAVLC only
		// ErrorConcealment
		unsigned char* streamBuffer;      ///< actual codebuffer for read bytes
		int           ei_flag;            ///< error indication, 0: no error, else unspecified error
	};
}

#endif