#include <h264p/seiimpl.h>
#include <h264p/sps.h>

#include "bitstream.h"
#include "vlc.h"
#include "util.h"

extern std::shared_ptr<ThetaStream::NALUnitSPS> g_sps;

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

	Bitstream bitstream((rbsp_.begin()+2), rbsp_.end(), rbsp_.size()-2);
	Interpret(&bitstream);
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

double ThetaStream::SEIPicTiming::timestampInSeconds() const
{
	double ts{ 0.0 };
	if (g_sps != nullptr && g_sps->vui_parameters_present_flag)
	{
		ts = (double) timestamp() / g_sps->vui_seq_parameters.time_scale;
	}
	return ts;
}

uint64_t ThetaStream::SEIPicTiming::timestamp() const
{
	uint64_t ts{ 0 };

	if (g_sps != nullptr && g_sps->vui_parameters_present_flag)
	{
		ts = ((hours_value * 60 + minutes_value) * 60 + seconds_value) * g_sps->vui_seq_parameters.time_scale +
			n_frames * (g_sps->vui_seq_parameters.num_units_in_tick * (1 + nuit_field_based_flag)) + time_offset;
	}

	return ts;
}

void ThetaStream::SEIPicTiming::Interpret(Bitstream* bitstream)
{
	int cpb_removal_len = 24;
	int dpb_output_len = 24;
	int pic_struct_present_flag{ 0 };
	int NumClockTs{ 0 };

	if (g_sps == nullptr)
		return;

	bool CpbDpbDelaysPresentFlag = (g_sps->vui_parameters_present_flag
		&& ((g_sps->vui_seq_parameters.nal_hrd_parameters_present_flag != 0)
			|| (g_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag != 0)));

	if (CpbDpbDelaysPresentFlag)
	{
		if (g_sps->vui_parameters_present_flag)
		{
			if (g_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
			{
				cpb_removal_len = g_sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
				dpb_output_len = g_sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1 + 1;
			}
			else if (g_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
			{
				cpb_removal_len = g_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
				dpb_output_len = g_sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1 + 1;
			}
		}

		if ((g_sps->vui_seq_parameters.nal_hrd_parameters_present_flag) ||
			(g_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag))
		{
			cpb_removal_delay = u_v(cpb_removal_len, bitstream);
			dpb_output_delay = u_v(dpb_output_len, bitstream);
		}
	}

	if (!g_sps->vui_parameters_present_flag)
	{
		pic_struct_present_flag = 0;
	}
	else
	{
		pic_struct_present_flag = g_sps->vui_seq_parameters.pic_struct_present_flag;
	}

	if (pic_struct_present_flag)
	{
		pic_struct = u_v(4, bitstream);
	}

	switch (pic_struct)
	{
	case 0:
	case 1:
	case 2:
		NumClockTs = 1;
		break;
	case 3:
	case 4:
	case 7:
		NumClockTs = 2;
		break;
	case 5:
	case 6:
	case 8:
		NumClockTs = 3;
		break;
	default:
		;
	}

	for (int i = 0; i < NumClockTs; i++)
	{
		clock_timestamp_flag = u_1(bitstream);

		if (clock_timestamp_flag)
		{
			ct_type = u_v(2, bitstream);
			nuit_field_based_flag = u_1(bitstream);
			counting_type = u_v(5, bitstream);
			full_timestamp_flag = u_1(bitstream);
			discontinuity_flag = u_1(bitstream);
			cnt_dropped_flag = u_1(bitstream);
			n_frames = u_v(8, bitstream);

			if (full_timestamp_flag)
			{
				seconds_value = u_v(6, bitstream);
				minutes_value = u_v(6, bitstream);
				hours_value = u_v(5, bitstream);
			}
			else
			{
				seconds_flag = u_1(bitstream);
				if (seconds_flag)
				{
					seconds_value = u_v(6, bitstream);
					minutes_flag = u_1(bitstream);

					if (minutes_flag)
					{
						minutes_value = u_v(6, bitstream);
						hours_flag = u_1(bitstream);

						if (hours_flag)
						{
							hours_value = u_v(5, bitstream);
						}
					}
				}
			}
			{
				int time_offset_length;
				if (g_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
					time_offset_length = g_sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length;
				else if (g_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
					time_offset_length = g_sps->vui_seq_parameters.nal_hrd_parameters.time_offset_length;
				else
					time_offset_length = 24;
				if (time_offset_length)
					time_offset = i_v(time_offset_length, bitstream);
				else
					time_offset = 0;
			}
		}
	}
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