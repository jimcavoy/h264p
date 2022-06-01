#include "slice.h"

#include "bitstream.h"
#include "vlc.h"
#include "util.h"
#include "sps.h"
#include "pps.h"

#include <iterator>

extern std::shared_ptr<ThetaStream::NALUnitSPS> g_sps;
extern std::shared_ptr<ThetaStream::NALUnitPPS> g_pps;

using namespace std;
using namespace ThetaStream;


/////////////////////////////////////////////////////////////////////////////
// NALUnitSliceHeaderImpl

NALUnitSliceHeaderImpl::NALUnitSliceHeaderImpl(unsigned char nut)
	: NALUnitImpl(nut)
	, first_mb_in_slice(0)
	, slice_type(0)
	, pic_parameter_set_id(0)
	, colour_plane_id(0)
	, frame_num(0)
	, field_pic_flag(false)
	, bottom_field_flag(false)
	, idr_pic_id(0)
	, pic_order_cnt_lsb(0)
	, delta_pic_order_cnt_bottom(0)
	, redundant_pic_cnt(0)
	, direct_spatial_mv_pred_flag(false)
	, num_ref_idx_active_override_flag(false)
	, num_ref_idx_10_active_minus1(0)
	, num_ref_idx_11_active_minus1(0)
	, no_output_of_prior_pics_flag(false)
	, long_term_reference_flag(false)
	, slice_qp_delta(0)
	, disable_deblocking_filter_idc(0)
	, slice_alpha_c0_offset_div2(0)
	, slice_beta_offset_div2(0)
{
	delta_pic_order_cnt[0] = 0;
	delta_pic_order_cnt[1] = 0;
}

NALUnitSliceHeaderImpl::~NALUnitSliceHeaderImpl()
{

}

void NALUnitSliceHeaderImpl::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{
	std::vector<char> sodb;
	removeEmulationPrevention3Bytes(std::back_inserter(sodb), first, last);

	Bitstream bitstream(++(sodb.begin()), sodb.end(), sodb.size() - 1);
	Interpret(&bitstream);
}

void NALUnitSliceHeaderImpl::Interpret(Bitstream* bits)
{
	if (g_sps == NULL)
		return;

	if (g_pps == NULL)
		return;

	first_mb_in_slice = ue_v(bits);
	slice_type = ue_v(bits);
	pic_parameter_set_id = ue_v(bits);
	if (g_sps->separate_colour_plane_flag)
		colour_plane_id = u_v(2, bits);
	frame_num = u_v(g_sps->log2_max_frame_num_minus4 + 4, bits);
	if (!g_sps->frame_mbs_only_flag) {
		field_pic_flag = u_1(bits);
		if (field_pic_flag)
			bottom_field_flag = u_1(bits);
	}
	if (nal_unit_type == 0x05) {
		idr_pic_id = ue_v(bits);
	}
	if (g_sps->pic_order_cnt_type == 0) {
		pic_order_cnt_lsb = u_v(g_sps->log2_max_pic_order_cnt_lsb_minus4 + 4, bits);
		if (g_pps->bottom_field_pic_order_in_frame_present_flag && !field_pic_flag)
			delta_pic_order_cnt_bottom = se_v(bits);
	}
	if (g_sps->pic_order_cnt_type == 1 && !g_sps->delta_pic_order_always_zero_flag) {
		delta_pic_order_cnt[0] = se_v(bits);
		if (g_pps->bottom_field_pic_order_in_frame_present_flag && !field_pic_flag)
			delta_pic_order_cnt[1] = se_v(bits);
	}
	if (g_pps->redundant_pic_cnt_present_flag)
		redundant_pic_cnt = ue_v(bits);
	// slice_type == B
	if (slice_type == 1 || slice_type == 6) {
		direct_spatial_mv_pred_flag = u_1(bits);
	}
	// slice_type == P || slice_type == B
	if (slice_type == 0 || slice_type == 5 || slice_type == 1 || slice_type == 6) {
		num_ref_idx_active_override_flag = u_1(bits);
		if (num_ref_idx_active_override_flag) {
			num_ref_idx_10_active_minus1 = ue_v(bits);
			if (slice_type == 1 || slice_type == 5) {
				num_ref_idx_11_active_minus1 = ue_v(bits);
			}
		}
	}
	// for now stop parsing
	/*if (nal_unit_type == 0x05) {
		no_output_of_prior_pics_flag = u_1(bits);
		long_term_reference_flag = u_1(bits);
	}
	slice_qp_delta = se_v(bits);
	if (g_pps->deblocking_filter_control_present_flag) {
		disable_deblocking_filter_idc = ue_v(bits);
		if (disable_deblocking_filter_idc != 1) {
			slice_alpha_c0_offset_div2 = se_v(bits);
			slice_beta_offset_div2 = se_v(bits);
		}
	}*/
}

/////////////////////////////////////////////////////////////////////////////
// NALUnitSlice

NALUnitSlice::NALUnitSlice()
	:NALUnitSliceHeaderImpl(0x01)
{
}

NALUnitSlice::~NALUnitSlice()
{
}

/////////////////////////////////////////////////////////////////////////////
// NALUnitIDR

ThetaStream::NALUnitIDR::NALUnitIDR()
	:NALUnitSliceHeaderImpl(0x05)
{
}

ThetaStream::NALUnitIDR::~NALUnitIDR()
{
}
