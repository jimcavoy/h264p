#pragma once
#include "h264prsr.h"

#include "json/elements.h"

/// <summary>
/// 
/// </summary>
/// <seealso cref="ThetaStream::H264Parser" />
class TestH264Parser :
	public ThetaStream::H264Parser
{
public:
	TestH264Parser(const char* filename);
	virtual ~TestH264Parser(void);

	virtual void onNALUnit( ThetaStream::NALUnit& nalu);

private:
	unsigned int count_;
	json::Object	root_;
	json::Array		nalus_;
};

