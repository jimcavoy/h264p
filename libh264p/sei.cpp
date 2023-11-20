#include "sei.h"

#include "seiimpl.h"

#include <iterator>

using namespace std;
using namespace ThetaStream;

/////////////////////////////////////////////////////////////////////////////
// NALUnit
SEI::SEI()
{

}

SEI::~SEI()
{

}

SEI::SEI(const SEI &rhs)
{
	pimpl_ = rhs.pimpl_;
	std::copy(rhs.rbsp_.begin(), rhs.rbsp_.end(), back_inserter(rbsp_));
}

SEI& SEI::operator =(const SEI& rhs)
{
	SEI temp(rhs);
	swap(temp);
	return *this;
}

SEI::SEI(SEI&& src) noexcept
{
	*this = std::move(src);
}

SEI& SEI::operator=(SEI&& rhs) noexcept
{
	if (this != &rhs)
	{
		rbsp_ = std::move(rhs.rbsp_);
		pimpl_ = rhs.pimpl_;
		rhs.pimpl_.reset();
	}
	return *this;
}

void SEI::swap(SEI& src)
{
	pimpl_.swap(src.pimpl_);
	rbsp_.swap(src.rbsp_);
}

unsigned int SEI::size() const
{
	return (unsigned int)rbsp_.size();
}

void SEI::push_back(uint8_t c)
{
	rbsp_.push_back(c);
}

void SEI::parse()
{
	if (!rbsp_.empty())
	{
		switch (rbsp_[0]) // payloadType
		{
		case 0x01: pimpl_ = std::shared_ptr<SEIimpl>(new SEIPicTiming);		break;
		case 0x05: pimpl_ = std::shared_ptr<SEIimpl>(new SEIUserDataUnreg);	break;
		default: pimpl_ = std::shared_ptr<SEIimpl>(new SEIDefault);
		}
	}

	if (pimpl_.get() != NULL)
		pimpl_->parse(rbsp_.begin(), rbsp_.end());
}

void SEI::Accept(Loki::BaseVisitor& visitor)
{
	if (pimpl_.get() == NULL)
		return;
	pimpl_->Accept(visitor);
}