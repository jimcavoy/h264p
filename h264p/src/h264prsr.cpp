#include "h264prsr.h"
#include "nalu.h"

#include <cstring>

using namespace std;

namespace
{
    int hasStartCode(const uint8_t* buf)
    {
        if (buf[0] == 0x00
            && buf[1] == 0x00
            && buf[2] == 0x01)
            return 3;

        if (buf[0] == 0x00
            && buf[1] == 0x00
            && buf[2] == 0x00
            && buf[3] == 0x01)
            return 4;

        return 0;
    }
}


/// @brief H264Parser Implementation Class
class ThetaStream::H264Parser::Impl
{
public:
    Impl()
    {
        memset(_start_code_prefix, 0xFF, 4);
    }
    ~Impl() {}
    Impl(const Impl& other)
        :_nalu(other._nalu)
    {
        memcpy(_start_code_prefix, other._start_code_prefix, 4);
    }

public:
    NALUnit _nalu;
    char _start_code_prefix[4];
};

/////////////////////////////////////////////////////////////////////////////
// H264Parser
ThetaStream::H264Parser::H264Parser()
    :_pimpl(std::make_unique<ThetaStream::H264Parser::Impl>())
{

}

ThetaStream::H264Parser::~H264Parser()
{

}

/// @brief Call this function to parse a H.264/AVC bitstream.
/// @param buf [in] Buffer to the H.264/AVC bitstream.
/// @param size [in] The size of the buffer as a number of bytes.
void ThetaStream::H264Parser::parse(const uint8_t* buf, size_t size)
{
    unsigned short startcodeprefix_len = 0;

    if (size == 0)
    {
        _pimpl->_nalu.parse();
        onNALUnit(_pimpl->_nalu);
        _pimpl->_nalu = NALUnit();
    }

    for (unsigned int i = 0; i < size; i++)
    {
        int len = hasStartCode(buf + i);
        if (len == 0)
        {
            _pimpl->_nalu.push_back(buf[i]);
        }
        else
        {
            startcodeprefix_len = len;
            i += len;
            if (_pimpl->_nalu.size() > 0)
            {
                _pimpl->_nalu.parse();
                onNALUnit(_pimpl->_nalu);
            }
            _pimpl->_nalu = NALUnit(startcodeprefix_len);
            if (i < size)
            {
                _pimpl->_nalu.push_back(buf[i]);
            }
        }
    }
}

/// @brief The H264Parser instance calls this function when it completes parsing a NALUnit.
/// Override this function in your code to handle this event.
/// @param nalu [in] The current NALUnit instance that was parsed.
void ThetaStream::H264Parser::onNALUnit(ThetaStream::NALUnit& nalu)
{

}
