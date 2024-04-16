#include "pps.h"

#include "bitstream.h"
#include "vlc.h"
#include "util.h"

#include <iterator>

using namespace std;
using namespace ThetaStream;

/////////////////////////////////////////////////////////////////////////////
// NALUnitPPS

NALUnitPPS::NALUnitPPS()
    :NALUnitImpl(0x08)
    , pic_parameter_set_id(0)
    , seq_parameter_set_id(0)
    , entropy_coding_mode_flag(false)
    , bottom_field_pic_order_in_frame_present_flag(false)
    , num_slice_groups_minus1(0)
    , slice_group_map_type(0)
    , slice_group_change_direction_flag(false)
    , slice_group_change_rate_minus1(0)
    , pic_size_in_map_units_minus1(0)
    , num_ref_idx_10_default_active_minus1(0)
    , num_ref_idx_11_default_active_minus1(0)
    , weighted_pred_flag(false)
    , weighted_bipred_idc(0)
    , pic_init_qp_minus26(0)
    , pic_init_qs_minus26(0)
    , chroma_qp_index_offset(0)
    , deblocking_filter_control_present_flag(false)
    , constrained_intra_pred_flag(false)
    , redundant_pic_cnt_present_flag(false)
    , transform_8x8_mode_flag(false)
    , pic_scaling_matrix_present_flag(false)
    , second_chroma_qp_index_offset(0)
{

}

NALUnitPPS::~NALUnitPPS()
{

}

void NALUnitPPS::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{
    std::vector<uint8_t> sodb;
    removeEmulationPrevention3Bytes(std::back_inserter(sodb), first, last);

    Bitstream bitstream(++(sodb.begin()), sodb.end(), sodb.size() - 1);
    Interpret(&bitstream);
}

void NALUnitPPS::Interpret(ThetaStream::Bitstream* bits)
{
    pic_parameter_set_id = ue_v(bits);
    seq_parameter_set_id = ue_v(bits);
    entropy_coding_mode_flag = u_1(bits);
    bottom_field_pic_order_in_frame_present_flag = u_1(bits);
    num_slice_groups_minus1 = ue_v(bits);
    if (num_slice_groups_minus1 > 0) {
        slice_group_map_type = ue_v(bits);
        if (slice_group_map_type == 0)
            for (unsigned iGroup = 0; iGroup <= num_slice_groups_minus1; iGroup++)
                run_length_minus1.push_back(ue_v(bits));
        else if (slice_group_map_type == 2)
            for (unsigned iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++) {
                top_left.push_back(ue_v(bits));
                bottom_right.push_back(ue_v(bits));
            }
        else if (slice_group_map_type == 3 ||
            slice_group_map_type == 4 ||
            slice_group_map_type == 5) {
            slice_group_change_direction_flag = u_1(bits);
            slice_group_change_rate_minus1 = ue_v(bits);
        }
        else if (slice_group_map_type == 6) {
            pic_size_in_map_units_minus1 = ue_v(bits);
            for (unsigned i = 0; i <= pic_size_in_map_units_minus1; i++)
                slice_group_id.push_back(u_1(bits));
        }
    }
    num_ref_idx_10_default_active_minus1 = ue_v(bits);
    num_ref_idx_11_default_active_minus1 = ue_v(bits);
    weighted_pred_flag = u_1(bits);
    weighted_bipred_idc = u_v(2, bits);
    pic_init_qp_minus26 = se_v(bits);
    pic_init_qs_minus26 = se_v(bits);
    chroma_qp_index_offset = se_v(bits);
    deblocking_filter_control_present_flag = u_1(bits);
    constrained_intra_pred_flag = u_1(bits);
    redundant_pic_cnt_present_flag = u_1(bits);
}