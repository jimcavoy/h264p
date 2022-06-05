#include "TestH264Parser.h"

#include "nalu.h"
#include "naluimpl.h"
#include "seiimpl.h"
#include "SEIParser.h"

#include "loki/Visitor.h"

#include "json/writer.h"

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
	NALUnitPrintVisitor(json::Object& nalu)
		:nalu_(nalu)
	{

	}
	void Visit(NALUnitSlice& unit)
	{
		json::Object header;
		nalu_["nal_unit_type"] = json::String("Slice");

		header["first_mb_in_slice"] = json::Number(unit.first_mb_in_slice);
		header["slice_type"] = json::String(sliceType(unit.slice_type).c_str());
		header["pic_parameter_set_id"] = json::Number(unit.pic_parameter_set_id);
		header["frame_num"] = json::Number(unit.frame_num);

		nalu_["slice_header"] = header;
	}

	void Visit(NALUnitDPA& unit)
	{
		nalu_["nal_unit_type"] = json::String("DPA");
	}

	void Visit(NALUnitDPB& unit)
	{
		nalu_["nal_unit_type"] = json::String("DPB");
	}

	void Visit(NALUnitDPC& unit)
	{
		nalu_["nal_unit_type"] = json::String("DPC");
	}

	void Visit(NALUnitIDR& unit)
	{
		json::Object header;
		nalu_["nal_unit_type"] = json::String("IDR");

		header["first_mb_in_slice"] = json::Number(unit.first_mb_in_slice);
		header["slice_type"] = json::String(sliceType(unit.slice_type).c_str());
		header["pic_parameter_set_id"] = json::Number(unit.pic_parameter_set_id);
		header["frame_num"] = json::Number(unit.frame_num);
		header["idr_pic_id"] = json::Number(unit.idr_pic_id);

		nalu_["slice_header"] = header;
	}

	void Visit(NALUnitSEI& unit)
	{
		nalu_["nal_unit_type"] = json::String("SEI");

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
		json::Object sps;
		std::string profile_idc;
		nalu_["nal_unit_type"] = json::String("SPS");

		switch(unit.profile_idc)
		{
		case 66: profile_idc = "baseline"; break;
		case 77: profile_idc = "main"; break;
		case 88: profile_idc = "extended"; break;
		case 100: profile_idc = "high"; break;
		}
		sps["profile_idc"] = json::String(profile_idc);

		sps["constraint_set0_flag"] = json::Boolean(unit.constrainted_set0_flag);
		sps["constraint_set1_flag"] = json::Boolean(unit.constrainted_set1_flag);
		sps["constraint_set2_flag"] = json::Boolean(unit.constrainted_set2_flag);
		sps["constraint_set3_flag"] = json::Boolean(unit.constrainted_set3_flag);
		sps["constraint_set4_flag"] = json::Boolean(unit.constrainted_set4_flag);
		sps["constraint_set5_flag"] = json::Boolean(unit.constrainted_set5_flag);

		sps["level_idc"] = json::Number((int) unit.level_idc);
		sps["seq_parameter_set_id"] = json::Number(unit.seq_parameter_set_id);
		sps["log2_max_frame_num_minus4"] = json::Number(unit.log2_max_frame_num_minus4);
		sps["pic_order_cnt_type"] = json::Number(unit.pic_order_cnt_type);
		sps["num_ref_frames"] = json::Number(unit.num_ref_frames);
		sps["gaps_in_frame_num_value_allowed_flag"] = json::Boolean(unit.gaps_in_frame_num_value_allowed_flag);
		sps["pic_width_in_mbs_minus1"] = json::Number(unit.pic_width_in_mbs_minus1);
		sps["pic_height_in_map_units_minus1"] = json::Number(unit.pic_height_in_map_units_minus1);
		sps["frame_mbs_only_flag"] = json::Boolean(unit.frame_mbs_only_flag);
		sps["direct_8x8_inference_flag"] = json::Boolean(unit.direct_8x8_inference_flag);
		sps["frame_cropping_flag"] = json::Boolean(unit.frame_cropping_flag);
		sps["vui_parameters_present_flag"] = json::Boolean(unit.vui_parameters_present_flag);

		if(unit.vui_parameters_present_flag)
		{
			json::Object vui;
			vui["aspect_ratio_info_present_flag"] = json::Boolean(unit.vui_seq_parameters.aspect_ratio_info_present_flag);
			if(unit.vui_seq_parameters.aspect_ratio_info_present_flag)
			{
				vui["aspect_ratio_idc"] = json::Number(unit.vui_seq_parameters.aspect_ratio_idc);
				if(unit.vui_seq_parameters.aspect_ratio_idc == 255)
				{
					vui["sar_width"] = json::Number(unit.vui_seq_parameters.sar_width);
					vui["sar_height"] = json::Number(unit.vui_seq_parameters.sar_height);
				}
			}
			vui["overscan_info_present_flag"] = json::Boolean(unit.vui_seq_parameters.overscan_info_present_flag);
			vui["video_signal_type_present_flag"] = json::Boolean(unit.vui_seq_parameters.video_signal_type_present_flag);
			vui["chroma_location_info_present_flag"] = json::Boolean(unit.vui_seq_parameters.chroma_location_info_present_flag);
			vui["timing_info_present_flag"] = json::Boolean(unit.vui_seq_parameters.timing_info_present_flag);
			if(unit.vui_seq_parameters.timing_info_present_flag)
			{
				vui["num_units_in_tick"] = json::Number(unit.vui_seq_parameters.num_units_in_tick);
				vui["time_scale"] = json::Number(unit.vui_seq_parameters.time_scale);
				vui["fixed_frame_rate_flag"] = json::Boolean(unit.vui_seq_parameters.fixed_frame_rate_flag);
			}

			vui["nal_hrd_parameters_present_flag"] = json::Boolean(unit.vui_seq_parameters.nal_hrd_parameters_present_flag);
			vui["nal_vcl_parameters_present_flag"] = json::Boolean(unit.vui_seq_parameters.vcl_hrd_parameters_present_flag);

			vui["pic_struct_present_flag"] = json::Boolean(unit.vui_seq_parameters.pic_struct_present_flag);
			vui["bitstream_restriction_flag"] = json::Boolean(unit.vui_seq_parameters.bitstream_restriction_flag);

			if(unit.vui_seq_parameters.bitstream_restriction_flag)
			{
				vui["motion_vectors_over_pic_boundaries_flag"] = json::Boolean(unit.vui_seq_parameters.motion_vectors_over_pic_boundaries_flag);
				vui["max_bytes_per_pic_denom"] = json::Number(unit.vui_seq_parameters.max_bytes_per_pic_denom);
				vui["max_bits_per_mb_denom"] = json::Number(unit.vui_seq_parameters.max_bits_per_mb_denom);
				vui["log2_max_mv_length_horizontal"] = json::Number(unit.vui_seq_parameters.log2_max_mv_length_horizontal);
				vui["log2_max_mv_length_vertical"] = json::Number(unit.vui_seq_parameters.log2_max_mv_length_vertical);
				vui["num_reorder_frames"] = json::Number(unit.vui_seq_parameters.num_reorder_frames);
				vui["max_dec_frame_buffering"] = json::Number(unit.vui_seq_parameters.max_dec_frame_buffering);
			}
			sps["VUI"] = vui;
		}
		nalu_["SPS"] = sps;
	}

	void Visit(NALUnitPPS& unit)
	{
		json::Object pps;
		nalu_["nal_unit_type"] = json::String("PPS");
		pps["pic_parameter_set_id"] = json::Number(unit.pic_parameter_set_id);
		pps["seq_parameter_set_id"] =  json::Number(unit.seq_parameter_set_id);
		if(unit.entropy_coding_mode_flag)
			pps["entropy_coding_mode_flag"] = json::String("CABAC");
		else
			pps["entropy_coding_mode_flag"] = json::String("CAVLC");
		pps["bottom_field_pic_order_in_frame_present_flag"] = json::Boolean(unit.bottom_field_pic_order_in_frame_present_flag);
		pps["num_slice_groups_minus1"] =  json::Number(unit.num_slice_groups_minus1);
		pps["num_ref_idx_10_default_active_minus1"] = json::Number(unit.num_ref_idx_10_default_active_minus1);
		pps["num_ref_idx_11_default_active_minus1"] = json::Number(unit.num_ref_idx_11_default_active_minus1);
		pps["weighted_pred_flag"] = json::Boolean(unit.weighted_pred_flag);
		pps["weighted_bipred_idc"] = json::Number(unit.weighted_bipred_idc);
		pps["pic_init_qp_minus26"] = json::Number(unit.pic_init_qp_minus26);
		pps["pic_init_qs_minus26"] = json::Number(unit.pic_init_qs_minus26);
		pps["chroma_qp_index_offset"] = json::Number(unit.chroma_qp_index_offset);
		pps["deblocking_filter_control_present_flag"] = json::Boolean(unit.deblocking_filter_control_present_flag);
		pps["constrained_intra_pred_flag"] = json::Boolean(unit.constrained_intra_pred_flag);
		pps["redundant_pic_cnt_present_flag"] = json::Boolean(unit.redundant_pic_cnt_present_flag);

		nalu_["PPS"] = pps;
	}

	void Visit(NALUnitAUD& unit)
	{
		json::Object aud;
		nalu_["nal_unit_type"] = json::String("AUD");
		aud["primary_pic_type"] = json::Number(unit.primary_pic_type());
		nalu_["AUD"] = aud;
	}

	void Visit(NALUnitEOSEQ& unit)
	{
		nalu_["nal_unit_type"] = json::String("EOSEQ");
	}

	void Visit(NALUnitEOStream& unit)
	{
		nalu_["nal_unit_type"] = json::String("EOStream");
	}

	void Visit(NALUnitFill& unit)
	{
		nalu_["nal_unit_type"] = json::String("Fill");
	}

	void Visit(NALUnitUnspecified& unit)
	{
		nalu_["nal_unit_type"] = json::String("Unspecified");
	}
private:
	json::Object& nalu_;
};

/////////////////////////////////////////////////////////////////////////////
// TestH264Parser

TestH264Parser::TestH264Parser(const char* filename)
	:count_(0)
{
	root_["filename"] =json::String(filename);
}


TestH264Parser::~TestH264Parser(void)
{
	root_["H.264 NAL Units"] = nalus_;
	json::Writer::Write(root_,cout);
}

void TestH264Parser::onNALUnit( ThetaStream::NALUnit& nalu)
{
	string nri;
	count_++;

	json::Object naluObj;

	switch(nalu.nal_ref_idc())
	{
	case 0x00: nri = "00"; break;
	case 0x20: nri = "01"; break;
	case 0x40: nri = "10"; break;
	case 0x60: nri = "11"; break;
	}

	if(nalu.size() > 0)
	{
		naluObj["nal_ref_idc"] = json::String(nri);
		naluObj["NumBytesInRBSP"] = json::Number(nalu.size());

		NALUnitPrintVisitor vis(naluObj);
		nalu.Accept(vis);

		nalus_.Insert(naluObj);
	}
}