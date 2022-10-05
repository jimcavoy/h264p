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

	H264Parser(const H264Parser& other);
	H264Parser& operator=(const H264Parser& rhs);

	H264Parser(H264Parser&& other) noexcept;
	H264Parser& operator=(H264Parser&& rhs) noexcept;

	virtual void parse(const char* buf, unsigned int size);
	virtual void onNALUnit( ThetaStream::NALUnit& nalu);

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;
};

}

#endif