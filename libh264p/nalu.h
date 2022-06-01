#ifndef NALU_H
#define NALU_H

#include <vector>
#include <memory>

#include "loki/Visitor.h"

namespace ThetaStream
{

class NALUnitImpl;

/////////////////////////////////////////////////////////////////////////////
// NALUint

class NALUnit 
	: public Loki::BaseVisitable<>
{
public:
	typedef std::vector<char> RawByteStreamPayload;
	typedef RawByteStreamPayload::iterator iterator;
	typedef RawByteStreamPayload::const_iterator const_iterator;
public:
	NALUnit(unsigned short startcodeprefix_len=4);
	~NALUnit();
	NALUnit& operator=(const NALUnit& rhs);
	NALUnit(const NALUnit& orig);

	void swap(NALUnit& src);

	unsigned short startcodeprefix_len() const;
	char nal_ref_idc() const;

	unsigned int size() const;
	void push_back(char c);
	void parse();

	iterator begin() { return rbsp_.begin(); }
	iterator end() { return rbsp_.end(); }

	void Accept(Loki::BaseVisitor& visitor); 

private:
	std::shared_ptr<NALUnitImpl> pimpl_;
	RawByteStreamPayload rbsp_;
	unsigned short startcodeprefix_len_;
	char nal_ref_idc_;
};

}

#endif