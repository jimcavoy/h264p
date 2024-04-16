#include "sps.h"

#include "bitstream.h"
#include "vlc.h"
#include "util.h"

#include <iterator>

using namespace std;
using namespace ThetaStream;

typedef unsigned char byte;

//AVC Profile IDC definitions
typedef enum {
    FREXT_CAVLC444 = 44,       //!< YUV 4:4:4/14 "CAVLC 4:4:4"
    BASELINE = 66,       //!< YUV 4:2:0/8  "Baseline"
    MAIN = 77,       //!< YUV 4:2:0/8  "Main"
    EXTENDED = 88,       //!< YUV 4:2:0/8  "Extended"
    FREXT_HP = 100,      //!< YUV 4:2:0/8  "High"
    FREXT_Hi10P = 110,      //!< YUV 4:2:0/10 "High 10"
    FREXT_Hi422 = 122,      //!< YUV 4:2:2/10 "High 4:2:2"
    FREXT_Hi444 = 244,      //!< YUV 4:4:4/14 "High 4:4:4"
    MVC_HIGH = 118,      //!< YUV 4:2:0/8  "Multiview High"
    STEREO_HIGH = 128       //!< YUV 4:2:0/8  "Stereo High"
} ProfileIDC;

typedef enum {
    CF_UNKNOWN = -1,     //!< Unknown color format
    YUV400 = 0,     //!< Monochrome
    YUV420 = 1,     //!< 4:2:0
    YUV422 = 2,     //!< 4:2:2
    YUV444 = 3      //!< 4:4:4
} ColorFormat;

static const uint8_t ZZ_SCAN[16] =
{ 0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

static const uint8_t ZZ_SCAN8[64] =
{ 0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
   12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
   35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
   58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};


// syntax for scaling list matrix values
static void Scaling_List(int* scalingList, int sizeOfScalingList, bool* UseDefaultScalingMatrix, ThetaStream::Bitstream* s)
{
    int j, scanj;
    int delta_scale, lastScale, nextScale;

    lastScale = 8;
    nextScale = 8;

    for (j = 0; j < sizeOfScalingList; j++)
    {
        scanj = (sizeOfScalingList == 16) ? ZZ_SCAN[j] : ZZ_SCAN8[j];

        if (nextScale != 0)
        {
            delta_scale = se_v(s);
            nextScale = (lastScale + delta_scale + 256) % 256;
            *UseDefaultScalingMatrix = (bool)(scanj == 0 && nextScale == 0);
        }

        scalingList[scanj] = (nextScale == 0) ? lastScale : nextScale;
        lastScale = scalingList[scanj];
    }
}

/////////////////////////////////////////////////////////////////////////////
// NALUnitSPS

NALUnitSPS::NALUnitSPS()
    :NALUnitImpl(0x07)
    , profile_idc(0)
    , constrainted_set0_flag(false)
    , constrainted_set1_flag(false)
    , constrainted_set2_flag(false)
    , constrainted_set3_flag(false)
    , constrainted_set4_flag(false)
    , constrainted_set5_flag(false)
    , reserved_zero_2bits(0)
    , level_idc(0)
    , seq_parameter_set_id(0)
    , chroma_format_idc(0)
    , separate_colour_plane_flag(false)
    , bit_depth_luma_minus8(0)
    , bit_depth_chroma_minus8(0)
    , seq_scaling_matrix_present_flag(false)
    , log2_max_frame_num_minus4(0)
    , pic_order_cnt_type(0)
    , log2_max_pic_order_cnt_lsb_minus4(0)
    , delta_pic_order_always_zero_flag(false)
    , offset_for_non_ref_pic(0)
    , offset_for_top_to_bottom_field(0)
    , num_ref_frames_in_pic_order_cnt_cycle(0)
    , num_ref_frames(0)
    , gaps_in_frame_num_value_allowed_flag(false)
    , pic_width_in_mbs_minus1(0)
    , frame_mbs_only_flag(false)
    , mb_adaptive_frame_field_flag(false)
    , direct_8x8_inference_flag(false)
    , frame_cropping_flag(false)
    , frame_cropping_rect_left_offset(0)
    , frame_cropping_rect_right_offset(0)
    , frame_cropping_rect_top_offset(0)
    , frame_cropping_rect_bottom_offset(0)
    , vui_parameters_present_flag(false)
{
    vui_seq_parameters.matrix_coefficients = 2;
}

NALUnitSPS::~NALUnitSPS()
{

}

void NALUnitSPS::parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last)
{
    std::vector<uint8_t> sodb;
    removeEmulationPrevention3Bytes(std::back_inserter(sodb), first, last);

    Bitstream bitstream(++(sodb.begin()), sodb.end(), sodb.size() - 1);
    Interpret(&bitstream);
}

void NALUnitSPS::Interpret(Bitstream* bitstream)
{
    unsigned n_ScalingList;
    int lossless_qpprime_flag;

    profile_idc = u_v(8, bitstream);
    constrainted_set0_flag = u_1(bitstream);
    constrainted_set1_flag = u_1(bitstream);
    constrainted_set2_flag = u_1(bitstream);
    constrainted_set3_flag = u_1(bitstream);
    constrainted_set4_flag = u_1(bitstream);
    constrainted_set5_flag = u_1(bitstream);
    reserved_zero_2bits = u_v(2, bitstream);

    level_idc = u_v(8, bitstream);
    seq_parameter_set_id = ue_v(bitstream);

    if (profile_idc == FREXT_HP ||
        profile_idc == FREXT_Hi10P ||
        profile_idc == FREXT_Hi422 ||
        profile_idc == FREXT_Hi444 ||
        profile_idc == FREXT_CAVLC444)
    {
        chroma_format_idc = ue_v(bitstream);
        if (chroma_format_idc == YUV444)
        {
            separate_colour_plane_flag = u_1(bitstream);
        }

        bit_depth_luma_minus8 = ue_v(bitstream);
        bit_depth_chroma_minus8 = ue_v(bitstream);
        lossless_qpprime_flag = u_1(bitstream);
        seq_scaling_matrix_present_flag = u_1(bitstream);

        if (seq_scaling_matrix_present_flag)
        {
            n_ScalingList = chroma_format_idc != YUV444 ? 8 : 12;
            for (unsigned i = 0; i < n_ScalingList; i++)
            {
                seq_scaling_list_present_flag[i] = u_1(bitstream);
                if (seq_scaling_list_present_flag[i])
                {
                    if (i < 6)
                        Scaling_List(ScalingList4x4[i], 16, &UseDefaultScalingMatrix4x4Flag[i], bitstream);
                    else
                        Scaling_List(ScalingList8x8[i], 64, &UseDefaultScalingMatrix8x8Flag[i], bitstream);
                }
            }
        }
    } // if high profile

    log2_max_frame_num_minus4 = ue_v(bitstream);
    pic_order_cnt_type = ue_v(bitstream);

    if (pic_order_cnt_type == 0)
        log2_max_pic_order_cnt_lsb_minus4 = ue_v(bitstream);
    else if (pic_order_cnt_type == 1)
    {
        delta_pic_order_always_zero_flag = u_1(bitstream);
        offset_for_non_ref_pic = se_v(bitstream);
        offset_for_top_to_bottom_field = se_v(bitstream);
        num_ref_frames_in_pic_order_cnt_cycle = ue_v(bitstream);
        for (unsigned i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
            offset_for_ref_frame[i] = se_v(bitstream);
    }

    num_ref_frames = ue_v(bitstream);
    gaps_in_frame_num_value_allowed_flag = u_1(bitstream);
    pic_width_in_mbs_minus1 = ue_v(bitstream);
    pic_height_in_map_units_minus1 = ue_v(bitstream);
    frame_mbs_only_flag = u_1(bitstream);
    if (!frame_mbs_only_flag)
        mb_adaptive_frame_field_flag = u_1(bitstream);

    direct_8x8_inference_flag = u_1(bitstream);
    frame_cropping_flag = u_1(bitstream);

    if (frame_cropping_flag)
    {
        frame_cropping_rect_left_offset = ue_v(bitstream);
        frame_cropping_rect_right_offset = ue_v(bitstream);
        frame_cropping_rect_top_offset = ue_v(bitstream);
        frame_cropping_rect_bottom_offset = ue_v(bitstream);
    }

    vui_parameters_present_flag = u_1(bitstream);
    if (vui_parameters_present_flag)
        ReadVUI(bitstream);
}

void NALUnitSPS::ReadVUI(Bitstream* s)
{
    vui_seq_parameters.aspect_ratio_info_present_flag = u_1(s);
    if (vui_seq_parameters.aspect_ratio_info_present_flag)
    {
        vui_seq_parameters.aspect_ratio_idc = u_v(8, s);
        if (255 == vui_seq_parameters.aspect_ratio_idc)
        {
            vui_seq_parameters.sar_width = u_v(16, s);
            vui_seq_parameters.sar_height = u_v(16, s);
        }
    }

    vui_seq_parameters.overscan_info_present_flag = u_1(s);
    if (vui_seq_parameters.overscan_info_present_flag)
        vui_seq_parameters.overscan_appropriate_flag = u_1(s);

    vui_seq_parameters.video_signal_type_present_flag = u_1(s);
    if (vui_seq_parameters.video_signal_type_present_flag)
    {
        vui_seq_parameters.video_format = u_v(3, s);
        vui_seq_parameters.video_full_range_flag = u_1(s);
        vui_seq_parameters.colour_description_present_flag = u_1(s);
        if (vui_seq_parameters.colour_description_present_flag)
        {
            vui_seq_parameters.colour_primaries = u_v(8, s);
            vui_seq_parameters.transfer_characteristics = u_v(8, s);
            vui_seq_parameters.matrix_coefficients = u_v(8, s);
        }
    }

    vui_seq_parameters.chroma_location_info_present_flag = u_1(s);
    if (vui_seq_parameters.chroma_location_info_present_flag)
    {
        vui_seq_parameters.chroma_sample_loc_type_top_field = ue_v(s);
        vui_seq_parameters.chroma_sample_loc_type_bottom_field = ue_v(s);
    }

    vui_seq_parameters.timing_info_present_flag = u_1(s);
    if (vui_seq_parameters.timing_info_present_flag)
    {
        vui_seq_parameters.num_units_in_tick = u_v(32, s);
        vui_seq_parameters.time_scale = u_v(32, s);
        vui_seq_parameters.fixed_frame_rate_flag = u_1(s);
    }

    vui_seq_parameters.nal_hrd_parameters_present_flag = u_1(s);
    if (vui_seq_parameters.nal_hrd_parameters_present_flag)
    {
        ReadHRDParameters(s, &vui_seq_parameters.nal_hrd_parameters);
    }

    vui_seq_parameters.vcl_hrd_parameters_present_flag = u_1(s);
    if (vui_seq_parameters.vcl_hrd_parameters_present_flag)
    {
        ReadHRDParameters(s, &vui_seq_parameters.vcl_hrd_parameters);
    }

    if (vui_seq_parameters.nal_hrd_parameters_present_flag || vui_seq_parameters.vcl_hrd_parameters_present_flag)
    {
        vui_seq_parameters.low_delay_hrd_flag = u_1(s);
    }

    vui_seq_parameters.pic_struct_present_flag = u_1(s);
    vui_seq_parameters.bitstream_restriction_flag = u_1(s);
    if (vui_seq_parameters.bitstream_restriction_flag)
    {
        vui_seq_parameters.motion_vectors_over_pic_boundaries_flag = u_1(s);
        vui_seq_parameters.max_bytes_per_pic_denom = ue_v(s);
        vui_seq_parameters.max_bits_per_mb_denom = ue_v(s);
        vui_seq_parameters.log2_max_mv_length_horizontal = ue_v(s);
        vui_seq_parameters.log2_max_mv_length_vertical = ue_v(s);
        vui_seq_parameters.num_reorder_frames = ue_v(s);
        vui_seq_parameters.max_dec_frame_buffering = ue_v(s);
    }
}

void NALUnitSPS::ReadHRDParameters(Bitstream* s, hrd_parameters_t* hrd)
{
    hrd->cpb_cnt_minus1 = ue_v(s);
    hrd->bit_rate_scale = u_v(4, s);
    hrd->cpb_size_scale = u_v(4, s);

    for (unsigned i = 0; i <= hrd->cpb_cnt_minus1; i++)
    {
        hrd->bit_rate_value_minus1[i] = ue_v(s);
        hrd->cpb_size_value_minus1[i] = ue_v(s);
        hrd->cbr_flag[i] = u_1(s);
    }

    hrd->initial_cpb_removal_delay_length_minus1 = u_v(5, s);
    hrd->cpb_removal_delay_length_minus1 = u_v(5, s);
    hrd->dpb_output_delay_length_minus1 = u_v(5, s);
    hrd->time_offset_length = u_v(5, s);
}