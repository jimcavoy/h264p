#include "TestH264Parser.h"

#include <h264p/nalu.h>
#include <h264p/naluimpl.h>
#include "SEIParser.h"

#include <loki/Visitor.h>

#include <iostream>
#include <string>

using namespace ThetaStream;
using namespace std;

#ifdef _WIN32
#define sprintf sprintf_s
#endif

namespace
{
    string sliceType(unsigned int type)
    {
        char stype[255]{};

        switch (type)
        {
        case 0: case 5:
            sprintf(stype, "P Slice = %d", type);
            break;
        case 1: case 6:
            sprintf(stype, "B Slice = %d", type);
            break;
        case 2: case 7:
            sprintf(stype, "I Slice = %d", type);
            break;
        }

        return string(stype);
    }
}

/// <summary>
/// Visitor class that prints out a JSON represent of a H.264 
/// NAL unit.
/// </summary>
/// <seealso cref="Loki::BaseVisitor" />
/// <seealso cref="Loki::Visitor{NALUnitSlice}" />
/// <seealso cref="Loki::Visitor{NALUnitDPA}" />
/// <seealso cref="Loki::Visitor{NALUnitDPB}" />
/// <seealso cref="Loki::Visitor{NALUnitDPC}" />
/// <seealso cref="Loki::Visitor{NALUnitIDR}" />
/// <seealso cref="Loki::Visitor{NALUnitSEI}" />
/// <seealso cref="Loki::Visitor{NALUnitSPS}" />
/// <seealso cref="Loki::Visitor{NALUnitPPS}" />
/// <seealso cref="Loki::Visitor{NALUnitAUD}" />
/// <seealso cref="Loki::Visitor{NALUnitEOSEQ}" />
/// <seealso cref="Loki::Visitor{NALUnitEOStream}" />
/// <seealso cref="Loki::Visitor{NALUnitFill}" />
/// <seealso cref="Loki::Visitor{NALUnitUnspecified}" />
class NALUnitPrintVisitor
    : public Loki::BaseVisitor,
    public Loki::Visitor<NALUnitSlice>,
    public Loki::Visitor<NALUnitDPA>,
    public Loki::Visitor<NALUnitDPB>,
    public Loki::Visitor<NALUnitDPC>,
    public Loki::Visitor<NALUnitIDR>,
    public Loki::Visitor<NALUnitSEI>,
    public Loki::Visitor<NALUnitSPS>,
    public Loki::Visitor<NALUnitPPS>,
    public Loki::Visitor<NALUnitAUD>,
    public Loki::Visitor<NALUnitEOSEQ>,
    public Loki::Visitor<NALUnitEOStream>,
    public Loki::Visitor<NALUnitFill>,
    public Loki::Visitor<NALUnitUnspecified>
{
public:
    NALUnitPrintVisitor(boost::json::object& nalu)
        :nalu_(nalu)
    {

    }
    void Visit(NALUnitSlice& unit)
    {
        boost::json::object header;
        nalu_["nal_unit_type"] = "Slice";

        header["first_mb_in_slice"] = unit.first_mb_in_slice;
        header["slice_type"] = sliceType(unit.slice_type).c_str();
        header["pic_parameter_set_id"] = unit.pic_parameter_set_id;
        header["frame_num"] = unit.frame_num;

        nalu_["slice_header"] = header;
    }

    void Visit(NALUnitDPA& unit)
    {
        nalu_["nal_unit_type"] = "DPA";
    }

    void Visit(NALUnitDPB& unit)
    {
        nalu_["nal_unit_type"] = "DPB";
    }

    void Visit(NALUnitDPC& unit)
    {
        nalu_["nal_unit_type"] = "DPC";
    }

    void Visit(NALUnitIDR& unit)
    {
        boost::json::object header;
        nalu_["nal_unit_type"] = "IDR";

        header["first_mb_in_slice"] = unit.first_mb_in_slice;
        header["slice_type"] = sliceType(unit.slice_type).c_str();
        header["pic_parameter_set_id"] = unit.pic_parameter_set_id;
        header["frame_num"] = unit.frame_num;
        header["idr_pic_id"] = unit.idr_pic_id;

        nalu_["slice_header"] = header;
    }

    void Visit(NALUnitSEI& unit)
    {
        nalu_["nal_unit_type"] = "SEI";

        SEIParser vis;
        NALUnitSEI::sei_payloads_type::iterator it;
        for (it = unit.begin(); it != unit.end(); ++it)
        {
            it->Accept(vis);
        }
        nalu_["SEI"] = vis.getSEI();
    }

    void Visit(NALUnitSPS& unit)
    {
        boost::json::object sps;
        std::string profile_idc;
        nalu_["nal_unit_type"] = "SPS";

        switch (unit.profile_idc)
        {
        case 66: profile_idc = "baseline"; break;
        case 77: profile_idc = "main"; break;
        case 88: profile_idc = "extended"; break;
        case 100: profile_idc = "high"; break;
        }
        sps["profile_idc"] = profile_idc;

        sps["constraint_set0_flag"] = unit.constrainted_set0_flag;
        sps["constraint_set1_flag"] = unit.constrainted_set1_flag;
        sps["constraint_set2_flag"] = unit.constrainted_set2_flag;
        sps["constraint_set3_flag"] = unit.constrainted_set3_flag;
        sps["constraint_set4_flag"] = unit.constrainted_set4_flag;
        sps["constraint_set5_flag"] = unit.constrainted_set5_flag;

        sps["level_idc"] = (int)unit.level_idc;
        sps["seq_parameter_set_id"] = unit.seq_parameter_set_id;
        sps["log2_max_frame_num_minus4"] = unit.log2_max_frame_num_minus4;
        sps["pic_order_cnt_type"] = unit.pic_order_cnt_type;
        sps["num_ref_frames"] = unit.num_ref_frames;
        sps["gaps_in_frame_num_value_allowed_flag"] = unit.gaps_in_frame_num_value_allowed_flag;
        sps["pic_width_in_mbs_minus1"] = unit.pic_width_in_mbs_minus1;
        sps["pic_height_in_map_units_minus1"] = unit.pic_height_in_map_units_minus1;
        sps["frame_mbs_only_flag"] = unit.frame_mbs_only_flag;
        sps["direct_8x8_inference_flag"] = unit.direct_8x8_inference_flag;
        sps["frame_cropping_flag"] = unit.frame_cropping_flag;
        sps["vui_parameters_present_flag"] = unit.vui_parameters_present_flag;

        if (unit.vui_parameters_present_flag)
        {
            boost::json::object vui;
            vui["aspect_ratio_info_present_flag"] = unit.vui_seq_parameters.aspect_ratio_info_present_flag;
            if (unit.vui_seq_parameters.aspect_ratio_info_present_flag)
            {
                vui["aspect_ratio_idc"] = unit.vui_seq_parameters.aspect_ratio_idc;
                if (unit.vui_seq_parameters.aspect_ratio_idc == 255)
                {
                    vui["sar_width"] = unit.vui_seq_parameters.sar_width;
                    vui["sar_height"] = unit.vui_seq_parameters.sar_height;
                }
            }
            vui["overscan_info_present_flag"] = unit.vui_seq_parameters.overscan_info_present_flag;
            vui["video_signal_type_present_flag"] = unit.vui_seq_parameters.video_signal_type_present_flag;
            vui["chroma_location_info_present_flag"] = unit.vui_seq_parameters.chroma_location_info_present_flag;
            vui["timing_info_present_flag"] = unit.vui_seq_parameters.timing_info_present_flag;
            if (unit.vui_seq_parameters.timing_info_present_flag)
            {
                vui["num_units_in_tick"] = unit.vui_seq_parameters.num_units_in_tick;
                vui["time_scale"] = unit.vui_seq_parameters.time_scale;
                vui["fixed_frame_rate_flag"] = unit.vui_seq_parameters.fixed_frame_rate_flag;
            }

            vui["nal_hrd_parameters_present_flag"] = unit.vui_seq_parameters.nal_hrd_parameters_present_flag;
            vui["nal_vcl_parameters_present_flag"] = unit.vui_seq_parameters.vcl_hrd_parameters_present_flag;

            vui["pic_struct_present_flag"] = unit.vui_seq_parameters.pic_struct_present_flag;
            vui["bitstream_restriction_flag"] = unit.vui_seq_parameters.bitstream_restriction_flag;

            if (unit.vui_seq_parameters.bitstream_restriction_flag)
            {
                vui["motion_vectors_over_pic_boundaries_flag"] = unit.vui_seq_parameters.motion_vectors_over_pic_boundaries_flag;
                vui["max_bytes_per_pic_denom"] = unit.vui_seq_parameters.max_bytes_per_pic_denom;
                vui["max_bits_per_mb_denom"] = unit.vui_seq_parameters.max_bits_per_mb_denom;
                vui["log2_max_mv_length_horizontal"] = unit.vui_seq_parameters.log2_max_mv_length_horizontal;
                vui["log2_max_mv_length_vertical"] = unit.vui_seq_parameters.log2_max_mv_length_vertical;
                vui["num_reorder_frames"] = unit.vui_seq_parameters.num_reorder_frames;
                vui["max_dec_frame_buffering"] = unit.vui_seq_parameters.max_dec_frame_buffering;
            }
            sps["VUI"] = vui;
        }
        nalu_["SPS"] = sps;
    }

    void Visit(NALUnitPPS& unit)
    {
        boost::json::object pps;
        nalu_["nal_unit_type"] = "PPS";
        pps["pic_parameter_set_id"] = unit.pic_parameter_set_id;
        pps["seq_parameter_set_id"] = unit.seq_parameter_set_id;
        if (unit.entropy_coding_mode_flag)
            pps["entropy_coding_mode_flag"] = "CABAC";
        else
            pps["entropy_coding_mode_flag"] = "CAVLC";
        pps["bottom_field_pic_order_in_frame_present_flag"] = unit.bottom_field_pic_order_in_frame_present_flag;
        pps["num_slice_groups_minus1"] = unit.num_slice_groups_minus1;
        pps["num_ref_idx_10_default_active_minus1"] = unit.num_ref_idx_10_default_active_minus1;
        pps["num_ref_idx_11_default_active_minus1"] = unit.num_ref_idx_11_default_active_minus1;
        pps["weighted_pred_flag"] = unit.weighted_pred_flag;
        pps["weighted_bipred_idc"] = unit.weighted_bipred_idc;
        pps["pic_init_qp_minus26"] = unit.pic_init_qp_minus26;
        pps["pic_init_qs_minus26"] = unit.pic_init_qs_minus26;
        pps["chroma_qp_index_offset"] = unit.chroma_qp_index_offset;
        pps["deblocking_filter_control_present_flag"] = unit.deblocking_filter_control_present_flag;
        pps["constrained_intra_pred_flag"] = unit.constrained_intra_pred_flag;
        pps["redundant_pic_cnt_present_flag"] = unit.redundant_pic_cnt_present_flag;

        nalu_["PPS"] = pps;
    }

    void Visit(NALUnitAUD& unit)
    {
        boost::json::object aud;
        nalu_["nal_unit_type"] = "AUD";
        aud["primary_pic_type"] = unit.primary_pic_type();
        nalu_["AUD"] = aud;
    }

    void Visit(NALUnitEOSEQ& unit)
    {
        nalu_["nal_unit_type"] = "EOSEQ";
    }

    void Visit(NALUnitEOStream& unit)
    {
        nalu_["nal_unit_type"] = "EOStream";
    }

    void Visit(NALUnitFill& unit)
    {
        nalu_["nal_unit_type"] = "Fill";
    }

    void Visit(NALUnitUnspecified& unit)
    {
        nalu_["nal_unit_type"] = "Unspecified";
    }
private:
    boost::json::object& nalu_;
};


class TestH264Parser::Impl
{
public:
    Impl() {}
    ~Impl() {}

public:
    unsigned int			count_{};
    boost::json::object		root_;
    boost::json::array		nalus_;
};

/////////////////////////////////////////////////////////////////////////////
// TestH264Parser

TestH264Parser::TestH264Parser(const char* filename)
    :_pimpl(std::make_unique<TestH264Parser::Impl>(TestH264Parser::Impl()))
{
    _pimpl->root_["filename"] = filename;
}


TestH264Parser::~TestH264Parser(void)
{
    _pimpl->root_["H.264 NAL Units"] = _pimpl->nalus_;
    std::cout << _pimpl->root_;
}

void TestH264Parser::onNALUnit(ThetaStream::NALUnit& nalu)
{
    string nri;
    _pimpl->count_++;

    boost::json::object naluObj;

    switch (nalu.nal_ref_idc())
    {
    case 0x00: nri = "00"; break;
    case 0x20: nri = "01"; break;
    case 0x40: nri = "10"; break;
    case 0x60: nri = "11"; break;
    }

    if (nalu.size() > 0)
    {
        naluObj["nal_ref_idc"] = nri;
        naluObj["NumBytesInRBSP"] = nalu.size();

        NALUnitPrintVisitor vis(naluObj);
        nalu.Accept(vis);

        _pimpl->nalus_.push_back(naluObj);
    }
}