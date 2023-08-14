#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <cstdint>

namespace ThetaStream
{
// Remove all emulation_preventation_three_byte (0x03) symbol from input bitstream
// that is passed in using Iter first and last, and output a new bitstream into
// BackIter bit parameter.  In short, this function converts binary symbol 
// 0x000003 to 0x0000.
// Ref: IEC 14496-10 (H.264 standard) section 7.4.1
template<class BackIter, class Iter>
void removeEmulationPrevention3Bytes( BackIter bit, Iter first, Iter last)
{
	Iter it;

	it = first;
	char c0, c1, c2;
	c0 = *it; c1 = *it++; c2 = *it++;
	int flag = -1;

	// remove 0x03 symbol
	while (it != last)
	{
		if (c0 == 0x00 && c1 == 0x00 && c2 == 0x03)
		{
			flag = 2;
		}
		if (flag < 0)
		{
			*bit++ = c0;
		}
		else if (flag == 2 || flag == 1)
		{
			*bit++ = c0;
			flag--;
		}
		else
			flag--;
		c0 = c1; c1 = c2; c2 = *it++;
	}
}

uint64_t ntohll(uint64_t n);

} // namespace lcss

#endif 