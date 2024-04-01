#pragma once
#include <loki/Visitor.h>

#include <h264p/seiimpl.h>

#include <boost/json.hpp>

#include <memory.h>
#include <vector>


/////////////////////////////////////////////////////////////////////////////
// This class represents a Modified Digital Video Pack Metadata (MDPM) element
// of the unregistered user data with UUID 17ee8c60f84d11d98cd60800200c9a66 in 
// the H.264 Supplemental Enhancement Information (SEI).

class MDPMElement
{
public:
	MDPMElement(char t = (char)0xFF, char* value = NULL) 
		:tag_(t){
		memcpy(value_,value,4);
	}
	~MDPMElement() {}
	MDPMElement(const MDPMElement& orig)
	{
		tag_ = orig.tag();
		memcpy(value_,orig.value(),4);
	}

	MDPMElement& operator=(const MDPMElement& rhs)
	{
		tag_ = rhs.tag();
		memcpy(value_,rhs.value(),4);
		return *this;
	}

	char tag() const { return tag_;}
	const char* value() const {return value_;}

private:
	char tag_;
	char value_[4];
};

/////////////////////////////////////////////////////////////////////////////
// This class represents SEI parser

class SEIParser :
	public Loki::BaseVisitor,
	public Loki::Visitor<ThetaStream::SEIPicTiming>,
	public Loki::Visitor<ThetaStream::SEIUserDataUnreg>,
	public Loki::Visitor<ThetaStream::SEIDefault>
{
public:
	typedef std::vector<MDPMElement> ElementCollection;
	typedef ElementCollection::iterator iterator;
public:
	SEIParser(void);
	~SEIParser(void);

	void Visit(ThetaStream::SEIPicTiming& sei);
	void Visit(ThetaStream::SEIUserDataUnreg& sei);
	void Visit(ThetaStream::SEIDefault& sei);

	boost::json::array& getSEI() {return seipayloads_;}

private:
	void parseMDPMData(ThetaStream::SEIUserDataUnreg& sei);
	void parseMIDPmicrosectime(ThetaStream::SEIUserDataUnreg& sei);

private:
	ElementCollection elements_;
	boost::json::array seipayloads_;
};

