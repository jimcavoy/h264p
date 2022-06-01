#ifndef UTIL_H
#define UTIL_H

#include <vector>

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
	std::vector<char> v;
	Iter iter;

	// temporary copy
	for(iter = first; iter != last; ++iter)
		v.push_back(*iter);

	std::vector<char>::iterator it;
	it = v.begin();
	char c0,c1,c2;
	c0 = *it; c1 = *it++; c2 = *it++;

	// remove 0x03 symbol
	while(it != v.end())
	{
		if(c0 == 0x00 && c1 == 0x00 && c2 == 0x03)
			it = v.erase(--it);
		c0 = c1; c1 = c2; c2 = *it++;
	}

	// output new bitstream
	for(it = v.begin(); it != v.end(); ++it)
		*bit++ = *it;
}

unsigned __int64 ntohll(unsigned __int64 n);

} // namespace lcss

#endif 