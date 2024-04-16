#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <cstdint>
#include <iterator>

namespace ThetaStream
{
    /// @brief Remove all emulation_preventation_three_byte (0x03) symbol from input bitstream
    /// that is passed in using Iter @p first and @p last, and output a new bitstream into
    /// BackIter &p bit parameter.  In short, this function converts binary symbol 
    /// 0x000003 to 0x0000.
    /// Ref: IEC 14496-10 (H.264 standard) section 7.4.1
    /// @tparam BackIter Back inserter iterator type to where the output bitstream will be inserted.
    /// @tparam Iter Iterator type for the input bitstream
    /// @param bit The back inserter iterator for the output bitstream.
    /// @param first The position in the input bitstream.
    /// @param last One position past the last byte in the input bitstream.
    template<class BackIter, class Iter>
    void removeEmulationPrevention3Bytes(BackIter bit, Iter first, Iter last)
    {
        std::vector<char> v;
        Iter iter;

        // temporary copy
        std::copy(first, last, std::back_inserter(v));

        std::vector<char>::iterator it;
        it = v.begin();
        char c0, c1, c2;
        c0 = *it; c1 = *it++; c2 = *it++;

        // remove 0x03 symbol
        while (it != v.end())
        {
            if (c0 == 0x00 && c1 == 0x00 && c2 == 0x03)
                it = v.erase(--it);
            c0 = c1; c1 = c2; c2 = *it++;
        }

        // output new bitstream
        std::copy(v.begin(), v.end(), bit);
    }

    uint64_t ntohll(uint64_t n);

} // namespace ThetaStream

#endif 