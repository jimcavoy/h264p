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

	NALUnit& operator=(NALUnit&& rhs) noexcept;
	NALUnit(NALUnit&& other) noexcept;

	unsigned short startcodeprefix_len() const;
	char nal_ref_idc() const;

	unsigned int size() const;
	void push_back(char c);
	void parse();

	iterator begin();
	iterator end();

	void Accept(Loki::BaseVisitor& visitor); 

private:
	class Impl;
	std::unique_ptr<Impl> _pimpl;
};

}

#endif