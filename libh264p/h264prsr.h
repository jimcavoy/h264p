#ifndef H264PRSR_H
#define H264PRSR_H

#include <memory>

namespace ThetaStream
{
	class NALUnit;

/////////////////////////////////////////////////////////////////////////////
// H264Parser
class H264Parser
{
public:
	H264Parser();
	virtual ~H264Parser();

	virtual void parse(const char* buf, unsigned int size);
	virtual void onNALUnit( NALUnit& nalu);

private:
	std::unique_ptr<NALUnit> _nalu;
	char _start_code_prefix[4];
};

}

#endif