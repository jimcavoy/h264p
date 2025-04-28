#ifndef H264PRSR_H
#define H264PRSR_H

#include <memory>
#include <cstdint>

namespace ThetaStream
{
    class NALUnit;

    /// @brief H.265/AVC Parser Class
    /// Create a class that inherits H264Parser and override the H264Parser::onNALUnit. 
    /// Call the H264Parser::parse with the H.264/AVC bitstream.  The parser will call 
    /// H264Parser::onNALUnit when completes parsing a NAL unit.
    class H264Parser
    {
    public:
        H264Parser();
        virtual ~H264Parser();

        virtual void parse(const uint8_t* buf, size_t size);
        virtual void onNALUnit(ThetaStream::NALUnit& nalu);

    private:
        class Impl; 
        std::unique_ptr<Impl> _pimpl; ///< smart point to the parser implementation class
    };

}

#endif
