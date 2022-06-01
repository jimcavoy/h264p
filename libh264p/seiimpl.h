#ifndef SEIIMPL_H
#define SEIIMPL_H

#include <vector>

#include "loki/Visitor.h"


namespace ThetaStream
{

/////////////////////////////////////////////////////////////////////////////
// SEIimpl
class SEIimpl
	: public Loki::BaseVisitable<>
{
public:
	typedef std::vector<char> RawByteStreamPayload;
	typedef RawByteStreamPayload::iterator iterator;
public:
	SEIimpl();
	virtual ~SEIimpl();

	virtual void parse(SEIimpl::iterator first
		,SEIimpl::iterator last) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// SEIDefault
class SEIDefault
	: public SEIimpl
{
public:
	SEIDefault();
	virtual ~SEIDefault();

	void parse(SEIimpl::iterator first, SEIimpl::iterator last);

	LOKI_DEFINE_VISITABLE()

	unsigned char type() const;

	size_t payloadSize() const;

	template<class BackInsertIter>
	void payload(BackInsertIter backit)
	{
		for (int i = 2; i < payloadSize(); i++)
		{
			*backit++ = rbsp_[i];
		}
	}

private:
	RawByteStreamPayload rbsp_;
};

/////////////////////////////////////////////////////////////////////////////
// SEIPicTiming
class SEIPicTiming
	: public SEIimpl
{
public:
	SEIPicTiming();
	virtual ~SEIPicTiming();

	void parse(SEIimpl::iterator first, SEIimpl::iterator last);

	LOKI_DEFINE_VISITABLE()

	size_t payloadSize() const;

	template<class BackInsertIter>
	void payload(BackInsertIter backit)
	{
		for (int i = 2; i < payloadSize(); i++)
		{
			*backit++ = rbsp_[i];
		}
	}

private:
	RawByteStreamPayload rbsp_;
};

/////////////////////////////////////////////////////////////////////////////
// SEIUserDataUnreg
class SEIUserDataUnreg
	: public SEIimpl
{
public:
	SEIUserDataUnreg();
	virtual ~SEIUserDataUnreg();

	void parse(SEIimpl::iterator first, SEIimpl::iterator last);

	LOKI_DEFINE_VISITABLE()

	template<class BackInsertIter>
	void uuid(BackInsertIter backit)
	{
		if(uuid_.size() > 15)
		{
			std::copy(uuid_.begin(),uuid_.end(),backit);
		}
	}

	size_t payloadSize() const;

	template<class BackInsertIter>
	void payload(BackInsertIter backit)
	{
		std::copy(payload_.begin(),payload_.end(),backit);
	}

private:
	RawByteStreamPayload rbsp_;
	RawByteStreamPayload payload_;
	RawByteStreamPayload uuid_;
};

};

#endif