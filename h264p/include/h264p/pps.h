#ifndef PPS_H
#define PPS_H

#include "naluimpl.h"
#include <loki/Visitor.h>
#include <vector>

namespace ThetaStream
{
    class Bitstream;
}

namespace ThetaStream
{

    /// @brief NALUnitPPS represents a NAL Unit Picture Parameter Set RBSP syntax as defined in 
    /// ISO/IEC 14496-10, Section 7.3.2.2, page 44.
    class NALUnitPPS
        : public NALUnitImpl
    {
    public:
        NALUnitPPS();
        virtual ~NALUnitPPS();

        void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

        LOKI_DEFINE_VISITABLE()

    public:
        unsigned int	pic_parameter_set_id;		// ue(v)
        unsigned int	seq_parameter_set_id;		// ue(v)
        bool			entropy_coding_mode_flag;	// u(1)
        bool			bottom_field_pic_order_in_frame_present_flag; // u(1)
        unsigned int	num_slice_groups_minus1;	// ue(v)
        //if(num_slice_groups_minus1 > 0) {
        unsigned int slice_group_map_type;		// ue(v)
        //if(slice_group_map_type ==  0)
        //	for(iGroup = 0;iGroup <= num_slice_groups_minus1; iGroup++)
        std::vector<unsigned int> run_length_minus1;	// ue(v)
        //else if(slice_group_map_type == 2)
            //for(iGroup=0;iGroup<num_slice_groups_minus1;iGroup++){
        std::vector<unsigned int> top_left;		// ue(v)
        std::vector<unsigned int> bottom_right;	// ue(v)
        //}
    //else if(slice_group_map_type == 3 ||
    //		  slice_group_map_type == 4 ||
    //		..slice_group_map_type == 5) {
        bool slice_group_change_direction_flag;		// u(1)
        unsigned int slice_group_change_rate_minus1;// ue(v)
        //} else if (slice_group_map_type == 6) {
        unsigned int pic_size_in_map_units_minus1;	// ue(v)
        //for(i=0;i<=pic_size_in_map_units_minus1;i++)
        std::vector<bool> slice_group_id;		// u(1)
        //}
    //}
        unsigned int num_ref_idx_10_default_active_minus1;	// ue(v)
        unsigned int num_ref_idx_11_default_active_minus1;	// ue(v)
        bool		 weighted_pred_flag;					// u(1)
        unsigned int weighted_bipred_idc;					// u(2)
        int			pic_init_qp_minus26;					// se(v)
        int			pic_init_qs_minus26;					// se(v)
        int			chroma_qp_index_offset;					// se(v)
        bool		deblocking_filter_control_present_flag;	// u(1)
        bool		constrained_intra_pred_flag;			// u(1)
        bool		redundant_pic_cnt_present_flag;			// u(1)
        // if(more_rbsp_data()){
        bool	transform_8x8_mode_flag;				// u(1)
        bool	pic_scaling_matrix_present_flag;		// u(1)
        // if(pic_scaling_matrix_present_flag)
            // for(i=0;i<6+
                //((chroma_format_idc != 3)?2:6)*transform_8x8_mode_flag;
                //i++){
        std::vector<bool> pic_scaling_list_present_flag; // u(1)
        int second_chroma_qp_index_offset;					 // se(v)

    private:
        void Interpret(ThetaStream::Bitstream* bits);
    };

};

#endif