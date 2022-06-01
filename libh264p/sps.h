#ifndef SPS_H
#define SPS_H

#include "naluimpl.h"

#include "loki/Visitor.h"

#define MAXnum_ref_frames_in_pic_order_cnt_cycle  256
#define MAXIMUMVALUEOFcpb_cnt   32

class Bitstream;

/////////////////////////////////////////////////////////////////////////////
// NALUnitSPS

namespace ThetaStream
{

typedef struct
{
  unsigned int cpb_cnt_minus1;                                   // ue(v)
  unsigned int bit_rate_scale;                                   // u(4)
  unsigned int cpb_size_scale;                                   // u(4)
  unsigned int bit_rate_value_minus1 [MAXIMUMVALUEOFcpb_cnt];    // ue(v)
  unsigned int cpb_size_value_minus1 [MAXIMUMVALUEOFcpb_cnt];    // ue(v)
  unsigned int cbr_flag              [MAXIMUMVALUEOFcpb_cnt];    // u(1)
  unsigned int initial_cpb_removal_delay_length_minus1;          // u(5)
  unsigned int cpb_removal_delay_length_minus1;                  // u(5)
  unsigned int dpb_output_delay_length_minus1;                   // u(5)
  unsigned int time_offset_length;                               // u(5)
} hrd_parameters_t;


typedef struct
{
  bool      aspect_ratio_info_present_flag;                   // u(1)
  unsigned int aspect_ratio_idc;                              // u(8)
  unsigned short sar_width;                                   // u(16)
  unsigned short sar_height;                                  // u(16)
  bool      overscan_info_present_flag;                       // u(1)
  bool      overscan_appropriate_flag;                        // u(1)
  bool      video_signal_type_present_flag;                   // u(1)
  unsigned int video_format;                                  // u(3)
  bool      video_full_range_flag;                            // u(1)
  bool      colour_description_present_flag;                  // u(1)
  unsigned int colour_primaries;                              // u(8)
  unsigned int transfer_characteristics;                      // u(8)
  unsigned int matrix_coefficients;                           // u(8)
  bool      chroma_location_info_present_flag;                // u(1)
  unsigned int  chroma_sample_loc_type_top_field;             // ue(v)
  unsigned int  chroma_sample_loc_type_bottom_field;          // ue(v)
  bool      timing_info_present_flag;                         // u(1)
  unsigned int num_units_in_tick;                             // u(32)
  unsigned int time_scale;                                    // u(32)
  bool      fixed_frame_rate_flag;                            // u(1)
  bool      nal_hrd_parameters_present_flag;                  // u(1)
  hrd_parameters_t nal_hrd_parameters;                        // hrd_paramters_t
  bool      vcl_hrd_parameters_present_flag;                  // u(1)
  hrd_parameters_t vcl_hrd_parameters;                           // hrd_paramters_t
  // if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag))
  bool      low_delay_hrd_flag;                               // u(1)
  bool      pic_struct_present_flag;                          // u(1)
  bool      bitstream_restriction_flag;                       // u(1)
  bool      motion_vectors_over_pic_boundaries_flag;          // u(1)
  unsigned int max_bytes_per_pic_denom;                       // ue(v)
  unsigned int max_bits_per_mb_denom;                         // ue(v)
  unsigned int log2_max_mv_length_vertical;                   // ue(v)
  unsigned int log2_max_mv_length_horizontal;                 // ue(v)
  unsigned int num_reorder_frames;                            // ue(v)
  unsigned int max_dec_frame_buffering;                       // ue(v)
} vui_seq_parameters_t;


/////////////////////////////////////////////////////////////////////////////
// NALUnitSPS

class NALUnitSPS
	: public NALUnitImpl
{
public:
	NALUnitSPS();
	virtual ~NALUnitSPS();

	void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

	LOKI_DEFINE_VISITABLE()

public:
	unsigned char	profile_idc;							// u(8)
	bool			constrainted_set0_flag;					// u(1)
	bool			constrainted_set1_flag;					// u(1)
	bool			constrainted_set2_flag;					// u(1)
	bool			constrainted_set3_flag;					// u(1)
	bool			constrainted_set4_flag;					// u(1)
	bool			constrainted_set5_flag;					// u(1)
	unsigned int	reserved_zero_2bits;					// u(2)
	unsigned char	level_idc;								// u(8)
	unsigned int	seq_parameter_set_id;					// ue(v)
	unsigned int	chroma_format_idc;						// ue(v)
	// if(chroma_format_idc == 3)
		bool  separate_colour_plane_flag;					// u(1)

	unsigned int	bit_depth_luma_minus8;					// ue(v)
	unsigned int	bit_depth_chroma_minus8;				// ue(v)

	bool			seq_scaling_matrix_present_flag;		// u(1)
	// if(seq_scaling_matrix_present_flag) {
		bool			seq_scaling_list_present_flag[12];	// u(1)
		int				ScalingList4x4[6][16];				// se(v)
		int				ScalingList8x8[6][64];				// se(v)
		bool			UseDefaultScalingMatrix4x4Flag[6];
		bool			UseDefaultScalingMatrix8x8Flag[6];
	//}

	unsigned int	log2_max_frame_num_minus4;				// ue(v)
	unsigned int	pic_order_cnt_type;						// ue(v)
	// if(pic_order_cnt_type == 0)
	unsigned int	log2_max_pic_order_cnt_lsb_minus4;		// ue(v)
	// else if(pic_order_cnt_type == 1) {
	bool			delta_pic_order_always_zero_flag;		// u(1)
	int				offset_for_non_ref_pic;					// se(v)
	int				offset_for_top_to_bottom_field;			// se(v)
	unsigned int	num_ref_frames_in_pic_order_cnt_cycle;	// ue(v)
	//}
	// for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
		int   offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   // se(v)
	unsigned int num_ref_frames;							// ue(v)
	bool   gaps_in_frame_num_value_allowed_flag;			// u(1)
	unsigned int pic_width_in_mbs_minus1;					// ue(v)
	unsigned int pic_height_in_map_units_minus1;			// ue(v)
	bool   frame_mbs_only_flag;								// u(1)
	// if( !frame_mbs_only_flag )
		bool   mb_adaptive_frame_field_flag;				// u(1)
	bool   direct_8x8_inference_flag;						// u(1)
	bool   frame_cropping_flag;								// u(1)
	// if(frame_cropping_flag) {
		unsigned int frame_cropping_rect_left_offset;                // ue(v)
		unsigned int frame_cropping_rect_right_offset;               // ue(v)
		unsigned int frame_cropping_rect_top_offset;                 // ue(v)
		unsigned int frame_cropping_rect_bottom_offset;              // ue(v)
	//}
	bool   vui_parameters_present_flag;                      // u(1)
	// if(vui_parameters_present_flag) {
		vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
	//}
private:
	void Interpret(Bitstream* bits);
	void ReadVUI(Bitstream* bits);
	void ReadHRDParameters(Bitstream* bits,hrd_parameters_t* hrd);
};

};
#endif