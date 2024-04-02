#pragma once
#include <h264p/h264prsr.h>
#include <functional>
#include <memory>

/**
 * @brief 
 */
class TestH264Parser :
    public ThetaStream::H264Parser
{
public:
    TestH264Parser(const char* filename);
    virtual ~TestH264Parser(void);

    /**
     * @brief 
     * @param nalu [in] 
     */
    virtual void onNALUnit(ThetaStream::NALUnit& nalu) override;

private:

    class Impl;
    std::unique_ptr<Impl> _pimpl;
};

