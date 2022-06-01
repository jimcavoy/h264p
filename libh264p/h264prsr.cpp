#include "h264prsr.h"

#include "nalu.h"

using namespace std;

namespace
{
	int hasStartCode(const char* buf)
	{
		if(buf[0] == 0x00
			&& buf[1] == 0x00
			&& buf[2] == 0x01)
			return 3;

		if(buf[0] == 0x00
			&& buf[1] == 0x00
			&& buf[2] == 0x00
			&& buf[3] == 0x01)
			return 4;

		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// H264Parser
ThetaStream::H264Parser::H264Parser()
{
	memset(_start_code_prefix,0xFF,4);
	_nalu = std::make_unique<NALUnit>();
}

ThetaStream::H264Parser::~H264Parser()
{

}

void ThetaStream::H264Parser::parse(const char* buf, unsigned int size)
{
	unsigned short startcodeprefix_len = 0;

	if(size == 0)
	{
		_nalu->parse();
		onNALUnit(*_nalu);
		_nalu = std::make_unique<NALUnit>();
	}

	for(unsigned int i = 0; i < size; i++)
	{
		int len = hasStartCode(buf+i);
		if(len == 0)
		{
			_nalu->push_back(buf[i]);
		}
		else
		{
			startcodeprefix_len = len;
			i += len;
			if(_nalu->size() > 0)
			{
				_nalu->parse();
				onNALUnit(*_nalu);
			}
			_nalu = std::make_unique<NALUnit>(startcodeprefix_len);
			if(i < size)
			{
				_nalu->push_back(buf[i]);
			}
		}
	}
}

void ThetaStream::H264Parser::onNALUnit( NALUnit& nalu)
{

}
