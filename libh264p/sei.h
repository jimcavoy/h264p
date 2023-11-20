#ifndef SEI_H
#define SEI_H

#include <vector>
#include <memory>

#include "loki/Visitor.h"

namespace ThetaStream
{

class SEIimpl;

/////////////////////////////////////////////////////////////////////////////
// NALUint

class SEI 
	: public Loki::BaseVisitable<>
{
public:
	typedef std::vector<uint8_t> RawByteStreamPayload;
public:
	SEI();
	~SEI();
	SEI& operator=(const SEI& rhs);
	SEI(const SEI& orig);
	SEI(SEI&& src) noexcept;
	SEI& operator=(SEI&& rhs) noexcept;

	void swap( SEI& src);

	unsigned int size() const;
	void push_back(uint8_t c);
	void parse();

	void Accept(Loki::BaseVisitor& visitor); 

private:
	std::shared_ptr<SEIimpl> pimpl_;

public:
	RawByteStreamPayload rbsp_;
};

}

#endif