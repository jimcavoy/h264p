#include "seiimpl.h"

#include "util.h"

#include <iterator>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

using namespace std;
using namespace ThetaStream;

/////////////////////////////////////////////////////////////////////////////
// SEIimpl
SEIimpl::SEIimpl()
{

}

SEIimpl::~SEIimpl()
{

}


/////////////////////////////////////////////////////////////////////////////
// SEIDefault

SEIDefault::SEIDefault()
{

}

SEIDefault::~SEIDefault()
{

}

void SEIDefault::parse(SEIimpl::iterator first, SEIimpl::iterator last)
{
	std::copy(first, last, std::back_inserter(rbsp_));
}

unsigned char SEIDefault::type() const
{
	return rbsp_[0];
}

size_t SEIDefault::payloadSize() const
{
	size_t ret = 0;
	int i = 1;
	unsigned char sz = 0;
	while ((sz = rbsp_[i++]) == 0xff)
	{
		ret += 0xff;
	}
	ret += sz;
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// SEIPicTiming

SEIPicTiming::SEIPicTiming()
{

}

SEIPicTiming::~SEIPicTiming()
{

}

void SEIPicTiming::parse(SEIimpl::iterator first, SEIimpl::iterator last)
{
	std::copy(first, last, std::back_inserter(rbsp_));
}

size_t SEIPicTiming::payloadSize() const
{
	size_t ret = 0;
	int i = 1;
	unsigned char sz = 0;
	while ((sz = rbsp_[i++]) == 0xff)
	{
		ret += 0xff;
	}
	ret += sz;
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// SEIUserDataUnreg

SEIUserDataUnreg::SEIUserDataUnreg()
{

}

SEIUserDataUnreg::~SEIUserDataUnreg()
{

}

size_t SEIUserDataUnreg::payloadSize() const
{
	size_t ret = 0;
	int i = 1;
	unsigned char sz = 0;
	while ((sz = rbsp_[i++]) == 0xff)
	{
		ret += 0xff;
	}
	ret += sz;
	return ret;
}

void SEIUserDataUnreg::parse(SEIimpl::iterator first, SEIimpl::iterator last)
{
	std::copy(first, last, std::back_inserter(rbsp_));

	size_t payloadSz = 0;
	size_t i = 1;
	int offset = 1;
	unsigned char sz = 0;

	while ((sz = rbsp_[i++]) == 0xff)
	{
		payloadSz += 0xff;
		offset++;
	}
	payloadSz += sz;

	size_t pos = i;
	for (; i < pos + 16; i++)
	{
		uuid_.push_back(rbsp_[i]);
	}

	payload_.insert(payload_.begin(), rbsp_.begin() + i, rbsp_.end());
}