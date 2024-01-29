#ifndef SEIIMPL_H
#define SEIIMPL_H

#include <vector>
#include <cstdint>
#include <cstddef>

#include <loki/Visitor.h>

class Bitstream;

namespace ThetaStream
{

/////////////////////////////////////////////////////////////////////////////
// SEIimpl
class SEIimpl
	: public Loki::BaseVisitable<>
{
public:
	typedef std::vector<uint8_t> RawByteStreamPayload;
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

	double timestampInSeconds() const;

	uint64_t timestamp() const;

private:
	void Interpret(Bitstream* bitstream);

public:
	// if (CpbDpbDelaysPresentFlag ) {
	int cpb_removal_delay{ 0 };					// u(v)
	int dpb_output_delay{ 0 };					// u(v)
	// }
	// if (pic_struct_present_flag) {
	int pic_struct{ 0 };						// u(4)
	// for (i=0; i < NumClockTS; i++) {
	int clock_timestamp_flag{ 0 };				// u(1)
	// if (clock_timestamp_flag[i]) {
	int ct_type{ 0 };							// u(2)
	int nuit_field_based_flag{ 0 };				// u(1)
	int counting_type{ 0 };						// u(5)
	int full_timestamp_flag{ 0 };				// u(1)
	int discontinuity_flag{ 0 };				// u(1)
	int cnt_dropped_flag{ 0 };					// u(1)
	int n_frames{ 0 };							// u(8)
	// if (full_timestamp_flag) {
	int seconds_value{ 0 };						// u(6)
	int minutes_value{ 0 };						// u(6)
	int hours_value{ 0 };						// u(5)
	// } else {
	int seconds_flag{ 0 };						// u(1)
	// if (seconds_flag) {
	//		seconds_value
	int minutes_flag{ 0 };						// u(1)
	// if (minutes_flag ) {
	//		minutes_value
	int hours_flag{ 0 };						// u(1)
	// if (hours_flag) {
	//		hours_value
	// }
	// }
	// }
	// if(time_offset_length > 0)
	int time_offset{ 0 };						// i(v)
	// }
	// }
	// }
	// }

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