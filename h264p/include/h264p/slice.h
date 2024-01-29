#ifndef SLICE_H
#define SLICE_H

#include <h264p/naluimpl.h>
#include <loki/Visitor.h>

class Bitstream;

namespace ThetaStream
{
	/////////////////////////////////////////////////////////////////////////////
	// NALUnitSliceHeaderImpl
	class NALUnitSliceHeaderImpl
		: public NALUnitImpl
	{
	public:
		NALUnitSliceHeaderImpl(unsigned char nut);
		virtual ~NALUnitSliceHeaderImpl();

		void parse(NALUnitImpl::iterator first, NALUnitImpl::iterator last);

		LOKI_DEFINE_VISITABLE()

	public:
		unsigned int	first_mb_in_slice;				// ue(v)
		unsigned int	slice_type;						// ue(v)
		unsigned int	pic_parameter_set_id;			// ue(v)
		unsigned int	colour_plane_id;				// u(2)
		unsigned int	frame_num;						// ue(v)
		bool			field_pic_flag;					// u(1)
		bool			bottom_field_flag;				// u(1)
		unsigned int	idr_pic_id;						// ue(v)
		unsigned int	pic_order_cnt_lsb;				// u(v)
		int				delta_pic_order_cnt_bottom;		// se(v)
		int				delta_pic_order_cnt[2];			// se(v)
		unsigned int	redundant_pic_cnt;				// ue(v)
		bool			direct_spatial_mv_pred_flag;	// u(1)
		bool			num_ref_idx_active_override_flag; //u(1)
		unsigned int	num_ref_idx_10_active_minus1;	// ue(v)
		unsigned int	num_ref_idx_11_active_minus1;	// ue(v)
		bool			no_output_of_prior_pics_flag;	// u(1)
		bool			long_term_reference_flag;		// u(1)
		int				slice_qp_delta;					// se(v)
		unsigned int	disable_deblocking_filter_idc;	// ue(v)
		int				slice_alpha_c0_offset_div2;		// se(v)
		int				slice_beta_offset_div2;			// se(v)

	private:
		void Interpret(Bitstream* bits);
	};
	
	/////////////////////////////////////////////////////////////////////////////
	// NALUnitSlice - Coded slice of a non-IDR picture
	class NALUnitSlice
		: public NALUnitSliceHeaderImpl
	{
	public:
		NALUnitSlice();
		virtual ~NALUnitSlice();

		LOKI_DEFINE_VISITABLE()
	};

	/////////////////////////////////////////////////////////////////////////////
	// NALUnitIDR - Coded slice of an IDR picture
	class NALUnitIDR
		: public NALUnitSliceHeaderImpl
	{
	public:
		NALUnitIDR();
		virtual ~NALUnitIDR();

		LOKI_DEFINE_VISITABLE()
	};

};

#endif