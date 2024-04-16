#pragma once
#include <h264p/h264prsr.h>
#include <functional>
#include <memory>

namespace avc2json
{
    /// @brief TestH264Parser
    class TestH264Parser :
        public ThetaStream::H264Parser
    {
    public:
        TestH264Parser(const char* filename);
        virtual ~TestH264Parser(void);

        /**
         * @brief Handle the event when parser finished parsing a NALUnit
         * @param nalu [in] The NALUnit that was parsed.
         */
        virtual void onNALUnit(ThetaStream::NALUnit& nalu) override;

    private:

        class Impl;
        std::unique_ptr<Impl> _pimpl;
    };
}

