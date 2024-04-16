#pragma once
#include <loki/Visitor.h>

#include <h264p/seiimpl.h>

#include <boost/json.hpp>

#include <memory.h>
#include <vector>

#include "MDPMParser.h"

namespace avc2json
{
	/// @brief SEIParser parse the SEI implementation classes
	class SEIParser :
		public Loki::BaseVisitor,
		public Loki::Visitor<ThetaStream::SEIPicTiming>,
		public Loki::Visitor<ThetaStream::SEIUserDataUnreg>,
		public Loki::Visitor<ThetaStream::SEIDefault>
	{
	public:
		typedef std::vector<avc2json::MDPMElement> ElementCollection;
		typedef ElementCollection::iterator iterator;
	public:
		SEIParser(void);
		~SEIParser(void);

		void Visit(ThetaStream::SEIPicTiming& sei);
		void Visit(ThetaStream::SEIUserDataUnreg& sei);
		void Visit(ThetaStream::SEIDefault& sei);

		boost::json::array& getSEI() { return seipayloads_; }

	private:
		void parseMDPMData(ThetaStream::SEIUserDataUnreg& sei);
		void parseMIDPmicrosectime(ThetaStream::SEIUserDataUnreg& sei);

	private:
		ElementCollection elements_;
		boost::json::array seipayloads_;
	};

}
