/**************************************************************************
# Copyright (C) 1994 Kubota Graphics Corp.
# 
# Permission to use, copy, modify, and distribute this material for
# any purpose and without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies, and that the name of Kubota Graphics not be used in
# advertising or publicity pertaining to this material.  Kubota
# Graphics Corporation MAKES NO REPRESENTATIONS ABOUT THE ACCURACY
# OR SUITABILITY OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED
# "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE AND KUBOTA GRAPHICS CORPORATION DISCLAIMS ALL WARRANTIES,
# EXPRESS OR IMPLIED.
**************************************************************************/
 
 
#ifndef DORE_DRIVER_H
#define DORE_DRIVER_H
 
/*****************************************************************************
**  This file contains function prototypes for the Dore' driver functions.
*****************************************************************************/

#include <dore/dore_develop/private/stdtexatt.h>

    /***  Driver Installation Routines  ***/

void ddr_g4vll_install_driver (char*);
void ddr_gl_install_driver (char*);
void ddr_kopengl_install_driver (char*);
void ddr_pex_install_driver (char*);
void ddr_ps_install_driver (char*);
void ddr_rasterfile_install_driver (char*);
void ddr_sampledev_install_driver (char*);
void ddr_stdx11_install_driver (char*);
void ddr_sunview_cxp_install_driver (char*);
void ddr_sunview_gen_install_driver (char*);
void ddr_template_install_driver (char*);
void ddr_vll_install_driver (char*);
void ddr_x11_DGL_install_driver (char*);
void ddr_x11_XFDI_install_driver (char*);
void ddr_x11_titang2_install_driver (char*);

    /***  Soft Tools Functions  ***/

struct dynout_device_data;
struct dynout_view_data;
struct dynout_window_data;

void    ddr_softpipe_drom_apply_att_clpvol (DtClipOperator,DtInt,DtHalfSpace*);
void    ddr_softpipe_drom_camera  (void);
DtInt   ddr_softpipe_drom_check_backface_triangle  (DtRealTriple[3]);
void    ddr_softpipe_drom_check_shade_buffers  (DtInt, DtInt, DtInt);
DtInt   ddr_softpipe_drom_clip2d_code_vertex  (DtRealTriple);
void    ddr_softpipe_drom_clip2d_edge_boundary  (
		DtRealTriple, DtRealTriple, DtColorRGB, DtColorRGB, DtInt,
		DtRealTriple, DtColorRGB);
void    ddr_softpipe_drom_clip2d_triangle  (
		DtRealTriple, DtRealTriple, DtRealTriple, DtColorRGB,
		DtColorRGB, DtColorRGB);
void    ddr_softpipe_drom_clip2d_triangle_boundary  (
		DtRealTriple, DtRealTriple, DtRealTriple, DtColorRGB,
		DtColorRGB, DtColorRGB, DtInt);
void    ddr_softpipe_drom_clip2d_vector  (
		DtRealTriple, DtRealTriple, DtColorRGB, DtColorRGB);
void    ddr_softpipe_drom_clip2d_vector_boundary  (
		DtRealTriple, DtRealTriple, DtColorRGB, DtColorRGB, DtInt);
DtInt   ddr_softpipe_drom_clip3d_code_vertex  (DtRealTriple);
void    ddr_softpipe_drom_clip3d_edge_boundary  (
		DtRealTriple, DtRealTriple, DtColorRGB, DtColorRGB,
		DtInt, DtRealTriple, DtColorRGB);
void    ddr_softpipe_drom_clip3d_triangle_boundary  (
		DtRealTriple, DtRealTriple, DtRealTriple, DtColorRGB,
		DtColorRGB, DtColorRGB, DtInt);
void    ddr_softpipe_drom_clip3d_vector_boundary  (
		DtRealTriple, DtRealTriple, DtColorRGB, DtColorRGB, DtInt);
void    ddr_softpipe_drom_clip_box  (DtRealTriple[8], DtFlag *, DtFlag *);
void    ddr_softpipe_drom_compute_wcstofcs  (void);
DtPtr   ddr_softpipe_drom_create_local_device_data  (DtObject);
DtPtr   ddr_softpipe_drom_create_local_view_data  (DtObject);
DtPtr   ddr_softpipe_drom_create_local_window_data  (DtObject, DtObject);
void    ddr_softpipe_drom_delete_local_device_data (DtPtr);
void    ddr_softpipe_drom_delete_local_view_data  (DtPtr);
void    ddr_softpipe_drom_delete_local_window_data (DtPtr);
void    ddr_softpipe_drom_get_lcstowcsmat  (DtMatrix4x4);
void    ddr_softpipe_drom_get_normal_type  (
		DtRealTriple *, DtRealTriple *, DtInt *, DtRealTriple **);
void    ddr_softpipe_drom_get_wcstofcsmat  (DtMatrix4x4);
void    ddr_softpipe_drom_h_normal  (DtReal[3], DtReal[3], DtReal[3]);
void    ddr_softpipe_drom_initialize_specular_table  (void);
void    ddr_softpipe_drom_light  (DtObject);
void    ddr_softpipe_drom_output_line  (
		DtRealTriple, DtRealTriple, DtColorRGB, DtColorRGB);
void    ddr_softpipe_drom_output_point  (DtRealTriple, DtColorRGB);
void    ddr_softpipe_drom_output_triangle  (
		DtRealTriple, DtRealTriple, DtRealTriple, DtColorRGB,
		DtColorRGB, DtColorRGB);
void    ddr_softpipe_drom_pop_att_clpvol  (void);
void    ddr_softpipe_drom_pop_lcstofcsmat  (void);
void    ddr_softpipe_drom_post_initialization  (void);
void    ddr_softpipe_drom_pre_initialization  (void);
void    ddr_softpipe_drom_push_att_clpvol  (void);
void    ddr_softpipe_drom_push_lcstofcsmat  (DtMatrix4x4);
void    ddr_softpipe_drom_render_clip_line  (
		DtRealTriple, DtRealTriple, DtColorRGB, DtColorRGB);
void    ddr_softpipe_drom_render_clip_line2d  (
		DtRealTriple, DtRealTriple, DtColorRGB, DtColorRGB);
void    ddr_softpipe_drom_render_clip_triangle  (
		DtRealTriple, DtRealTriple, DtRealTriple, DtColorRGB,
		DtColorRGB, DtColorRGB);
void    ddr_softpipe_drom_render_clip_triangle2d  (
		DtRealTriple, DtRealTriple, DtRealTriple, DtColorRGB,
		DtColorRGB, DtColorRGB);
void    ddr_softpipe_drom_render_connected_line_list  (
		DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*,
		DtRealTriple *, DtRealTriple *, DtRealTriple *,
		DtInt, DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
void    ddr_softpipe_drom_render_line_list  (
		DtColorModel, DtRealTriple[8], DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *, DtRealTriple *,
		DtInt, DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
void    ddr_softpipe_drom_render_point_clip2d  (DtRealTriple, DtColorRGB);
void    ddr_softpipe_drom_render_point_clipped  (DtRealTriple, DtColorRGB);
void    ddr_softpipe_drom_render_point_list  (
		DtColorModel, DtRealTriple[8], DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *,
		DtInt, DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
void    ddr_softpipe_drom_render_triangle_list  (
		DtColorModel, DtRealTriple[8], DtInt, DtRealTriple *,
		DtRealTriple *, DtRealTriple *, DtRealTriple *,
		DtInt, DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
void    ddr_softpipe_drom_render_triangle_mesh  (
		DtColorModel, DtRealTriple[8], DtInt, DtRealTriple *,
		DtInt, DtRealTriple *, DtRealTriple *, DtRealTriple *,
		DtInt[][3], DtInt[][3],
		DtInt, DtRealCouple *[], DtInt, DtRealTriple *[], DtReal*);
DtFlag  ddr_softpipe_drom_renormalize  (DtReal[3]);
void    ddr_softpipe_drom_set_att_ambint  (DtReal);
void    ddr_softpipe_drom_set_att_ambswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_bacfacculble  (DtSwitch);
void    ddr_softpipe_drom_set_att_bacfacculswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_clpswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_clpvol  (DtClipOperator, DtInt, DtHalfSpace*);
void    ddr_softpipe_drom_set_att_depcue  (
		DtReal, DtReal, DtReal, DtReal, DtColorModel, DtReal *);
void    ddr_softpipe_drom_set_att_depcueswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_difclr  (DtColorModel, DtReal[]);
void    ddr_softpipe_drom_set_att_difint  (DtReal);
void    ddr_softpipe_drom_set_att_difswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_hidsrfswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_inttyp  (DtInterpType);
void    ddr_softpipe_drom_set_att_lgtatn  (DtReal, DtReal);
void    ddr_softpipe_drom_set_att_lgtclr  (DtColorModel, DtReal[]);
void    ddr_softpipe_drom_set_att_lgtint  (DtReal);
void    ddr_softpipe_drom_set_att_lgtspdang  (DtReal, DtReal);
void    ddr_softpipe_drom_set_att_lgtspdexp  (DtReal);
void    ddr_softpipe_drom_set_att_lgtswi  (DtInt, DtObject *);
void    ddr_softpipe_drom_set_att_lgttyp  (DtObject);
void    ddr_softpipe_drom_set_att_lintyp  (DtLineType);
void    ddr_softpipe_drom_set_att_linwid  (DtReal);
void    ddr_softpipe_drom_set_att_mapbmp  (
		DtInt, DtObject[], DtObject[], dot_stdtexatt[]);
void    ddr_softpipe_drom_set_att_mapbmpswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_mapdifclr  (
		DtInt, DtObject[], DtObject[], dot_stdtexatt[]);
void    ddr_softpipe_drom_set_att_mapdifclrswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_mapenv  (
		DtInt, DtObject[], DtObject[], dot_stdtexatt[]);
void    ddr_softpipe_drom_set_att_mapenvswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_maptrnint  (
		DtInt, DtObject[], DtObject[], dot_stdtexatt[]);
void    ddr_softpipe_drom_set_att_maptrnintswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_refswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_reptyp  (DtRepType);
void    ddr_softpipe_drom_set_att_shaswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_shdidx  (DtInt);
void    ddr_softpipe_drom_set_att_spcclr  (DtColorModel, DtReal[]);
void    ddr_softpipe_drom_set_att_spcfct  (DtReal);
void    ddr_softpipe_drom_set_att_spcint  (DtReal);
void    ddr_softpipe_drom_set_att_spcswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_srfshd  (DtObject);
void    ddr_softpipe_drom_set_att_stereo  (DtReal, DtReal);
void    ddr_softpipe_drom_set_att_stereoswi  (DtSwitch);
void    ddr_softpipe_drom_set_att_transpclr  (DtColorModel, DtReal[]);
void    ddr_softpipe_drom_set_att_transpint  (DtReal);
void    ddr_softpipe_drom_set_att_transpswi  (DtSwitch);
void    ddr_softpipe_drom_set_camera_matrix  (DtMatrix4x4);
void    ddr_softpipe_drom_set_output_functions  (
		void (*)(), void (*)(), void (*)());
void    ddr_softpipe_drom_set_parallel_matrix  (DtReal, DtReal, DtReal);
void    ddr_softpipe_drom_set_perspective_matrix  (DtReal, DtReal, DtReal);
void    ddr_softpipe_drom_set_projection_matrix  (
		DtArea *, DtProjectionType, DtPoint3, DtReal, DtReal, DtReal);
void    ddr_softpipe_drom_shade_clip_color  (DtInt, DtSwitch, DtInt);
void    ddr_softpipe_drom_shade_connected_line_list  (
		DtInt, DtRealTriple *, DtRealTriple *, DtRealTriple *,
		DtRealTriple *, DtInt *);
void    ddr_softpipe_drom_shade_elements  (
		DtInt, DtRealTriple *, DtInt, DtRealTriple *, DtRealTriple *,
		DtRealTriple *, DtInt *);
void    ddr_softpipe_drom_shade_triangle_list  (
		DtInt, DtRealTriple *, DtInt, DtRealTriple *, DtRealTriple *,
		DtRealTriple *, DtInt *);
void    ddr_softpipe_drom_start_update  (DtObject,DtObject,DtPtr,DtPtr,DtPtr);
DtFlag  ddr_softpipe_drom_transform_clip_z_point  (DtRealTriple, DtMatrix4x4);
void    ddr_softpipe_drom_transform_vertices  (
		DtInt, DtRealTriple *, DtRealTriple **);
void    ddr_softpipe_drom_transform_vertices_ctod  (DtInt, DtRealTriple *);
DtRealTriple*
	ddr_softpipe_drom_unfold_triangle_mesh  (
		DtInt, DtInt, DtRealTriple *, DtInt[][3]);
DtRealTriple*
	ddr_softpipe_drom_unfold_vertex_colors  (
		DtInt, DtInt, DtRealTriple *, DtInt[][3]);
DtRealTriple*
	ddr_softpipe_drom_unfold_vertex_normals  (
		DtInt, DtInt, DtRealTriple *, DtInt[][3]);
void    ddr_softpipe_drom_update_display  (DtPFI);
void    ddr_softpipe_drom_update_lcstowcsmat  (void);
void    ddr_softpipe_drom_update_lcstowcsmat_lokatfrm  (
		DtPoint3, DtPoint3, DtVector3, DtCompType);
void    ddr_softpipe_drom_update_lcstowcsmat_pop  (void);
void    ddr_softpipe_drom_update_lcstowcsmat_push  (void);
void    ddr_softpipe_drom_update_lcstowcsmat_rotate  (DtAxis,DtReal,DtCompType);
void    ddr_softpipe_drom_update_lcstowcsmat_scale  (
		DtReal, DtReal, DtReal, DtCompType);
void    ddr_softpipe_drom_update_lcstowcsmat_shear  (
		DtMajorPlane, DtReal, DtReal, DtCompType);
void    ddr_softpipe_drom_update_lcstowcsmat_tfmmat  (DtMatrix4x4, DtCompType);
void    ddr_softpipe_drom_update_lcstowcsmat_transl  (
		DtReal, DtReal, DtReal, DtCompType);
void    ddr_softpipe_drom_update_lights  (void);
void    ddr_softpipe_drom_update_lights_specular  (void);
void    ddr_softpipe_drom_update_local_data  (void);
void    ddr_softpipe_drom_update_state  (void);
void    ddr_softpipe_drom_update_studio  (DtPFI);
void    ddr_softpipe_return_functions  (DtPtr *);

#endif
