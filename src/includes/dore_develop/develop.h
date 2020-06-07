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

#ifndef DORE_DEVELOP
#define DORE_DEVELOP

#include <dore/dore_develop/private/stdtexatt.h>

typedef DtInt DDt_DeviceInterface;
#define DDc_DCM			1
#define DDc_PROM		2
#define DDc_DROM		3
#define DDc_DROMMethods		4


    /*************************/
    /***  Data Structures  ***/
    /*************************/

typedef struct {	/* DDt_DCM */
    DtInt version_number;

    DtPtr  (*create_local_data) (DtObject);
    DtFlag (*initialize_device) (DtObject, DtPtr, DtPtr, DtPtr);
    void   (*become_current_driver) (DtPtr);
    void   (*close_device) (void);
    void   (*flush) (void);
    void   (*swap_buffers) (void);
    void   (*update_geometry) (void);
    void   (*inquire_device_extent) (DtVolume *);
    void   (*inquire_stereo) (DtFlag *);
    void   (*inquire_ncolors) (DtInt *);
    void   (*inquire_resolution) (DtReal *, DtReal *);
    void   (*inquire_visual_type) (DtVisualType *);
    void   (*inquire_auto_size) (DtFlag*);
    void   (*inquire_color_entries) (DtColorModel, DtInt, DtInt, DtReal[]);
    void   (*set_color_entries) (DtColorModel, DtInt, DtInt, DtReal[]);
    void   (*set_foreground_color) (DtColorModel, DtReal[]);
    void   (*set_background_color) (DtColorModel, DtReal[]);
    void   (*set_shade_index) (DtInt);
    void   (*inquire_clip_list) (DtUInt*, DtReal**, DtReal[4], DtFlag*);
    void   (*clear_rectangle_depth) (DtShort, DtShort, DtShort, DtShort);
    void   (*clear_rectangle_depth_and_color) (DtShort,DtShort,DtShort,DtShort);
    void   (*set_depth_buffer_enable) (DtFlag);
    void   (*set_depth_buffer_write) (DtFlag);
    void   (*write_scanline_byte) (DtInt, DtInt, DtInt, DtUChar *);
    void   (*set_current_view) (DtObject);
    DtFlag (*inquire_pixel_data) (DtRasterType, DtInt*, DtInt*, DtRasterType*,
				  DtPtr*, DtFlag*);

    /* following fields only in version 2 and up */

    void   (*set_options) (DtPtr);

} DDt_DCM;


typedef struct {	/* DDt_PROM */
    DtInt version_number;
    void (*write_rectangle_byte_rgb) (DtInt, DtInt, DtInt, DtInt, DtUChar*);
} DDt_PROM;


typedef struct {	/* DDt_DROM */
    DtInt version_number;

    void (*pre_initialization)	(void);
    void (*post_initialization)  (void);

    DtPtr (*create_local_device_data)  (DtObject);
    DtPtr (*create_local_view_data)  (DtObject);
    DtPtr (*create_local_window_data)  (DtObject, DtObject);

    void (*start_update)  (DtObject, DtObject, DtPtr, DtPtr, DtPtr);
    void (*update_local_data)  (void);
    void (*update_studio)  (DtPFI);
    void (*update_display)  (DtPFI);

    void (*camera)  (void);
    void (*light)  (DtObject);

    void (*set_camera_matrix)  (DtMatrix4x4);
    void (*set_parallel_matrix)  (DtReal, DtReal, DtReal);
    void (*set_perspective_matrix)  (DtReal, DtReal, DtReal);
    void (*set_projection_matrix)  (
	     DtArea*, DtProjectionType, DtPoint3, DtReal, DtReal, DtReal);

    void (*push_att_clpvol)  (void);
    void (*pop_att_clpvol)  (void);
    void (*set_att_clpvol)  (DtClipOperator, DtInt, DtHalfSpace*);
    void (*apply_att_clpvol)  (DtClipOperator, DtInt, DtHalfSpace*);

    void   (*get_wcstofcsmat)  (DtMatrix4x4);
    void   (*pop_lcstofcsmat)  (void);
    void   (*push_lcstofcsmat)  (DtMatrix4x4);
    DtFlag (*transform_clip_z_point)  (DtPoint3, DtMatrix4x4);

    void (*render_point_list) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	     DtRealTriple*, DtInt, DtRealCouple**, DtInt, DtRealTriple**,
	     DtReal*);
    void (*render_line_list) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	     DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	     DtRealTriple**, DtReal*);
    void (*render_connected_line_list) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*,
	     DtRealTriple*, DtRealTriple*, DtRealTriple*, DtInt,
	     DtRealCouple**, DtInt, DtRealTriple**, DtReal*);
    void (*render_triangle_list) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	     DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	     DtRealTriple**, DtReal*);
    void (*render_triangle_mesh) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtInt,
	     DtRealTriple*, DtRealTriple*, DtRealTriple*, DtInt[][3],
	     DtInt[][3], DtInt, DtRealCouple**, DtInt, DtRealTriple**, DtReal*);

    void (*set_att_ambint)	(DtReal);
    void (*set_att_ambswi)	(DtSwitch);
    void (*set_att_bacfacculble)(DtSwitch);
    void (*set_att_bacfacculswi)(DtSwitch);
    void (*set_att_clpswi)	(DtSwitch);
    void (*set_att_depcue)	(DtReal,DtReal, DtReal,DtReal, DtColorModel,
				 DtReal[]);
    void (*set_att_depcueswi)	(DtSwitch);
    void (*set_att_difclr)	(DtColorModel, DtReal[]);
    void (*set_att_difint)	(DtReal);
    void (*set_att_difswi)	(DtSwitch);
    void (*set_att_hidsrfswi)	(DtSwitch);
    void (*set_att_inttyp)	(DtInterpType);
    void (*set_att_lgtclr)	(DtColorModel, DtReal[]);
    void (*set_att_lgtint)	(DtReal);
    void (*set_att_lgttyp)	(DtObject);
    void (*set_att_lintyp)	(DtLineType);
    void (*set_att_linwid)	(DtReal);
    void (*set_att_refswi)	(DtSwitch);
    void (*set_att_reptyp)	(DtRepType);
    void (*set_att_shaswi)	(DtSwitch);
    void (*set_att_shdidx)	(DtInt);
    void (*set_att_spcclr)	(DtColorModel, DtReal[]);
    void (*set_att_spcfct)	(DtReal);
    void (*set_att_spcint)	(DtReal);
    void (*set_att_spcswi)	(DtSwitch);
    void (*set_att_srfshd)	(DtObject);
    void (*set_att_stereo)	(DtReal, DtReal);
    void (*set_att_stereoswi)	(DtSwitch);
    void (*set_att_transpclr)	(DtColorModel, DtReal[]);
    void (*set_att_transpint)	(DtReal);
    void (*set_att_transpswi)	(DtSwitch);

    void (*update_lcstowcsmat_lokatfrm)(DtPoint3,DtPoint3,DtVector3,DtCompType);
    void (*update_lcstowcsmat_pop)     (void);
    void (*update_lcstowcsmat_push)    (void);
    void (*update_lcstowcsmat_rotate)  (DtAxis, DtReal, DtCompType);
    void (*update_lcstowcsmat_scale)   (DtReal, DtReal, DtReal, DtCompType);
    void (*update_lcstowcsmat_shear)   (DtMajorPlane,DtReal,DtReal,DtCompType);
    void (*update_lcstowcsmat_tfmmat)  (DtMatrix4x4, DtCompType);
    void (*update_lcstowcsmat_transl)  (DtReal, DtReal, DtReal, DtCompType);

    void (*set_att_lgtswi)	(DtInt, DtObject*);
    void (*set_att_lgtspdexp)	(DtReal);
    void (*set_att_lgtspdang)	(DtReal, DtReal);
    void (*set_att_lgtatn)	(DtReal, DtReal);

    void (*set_att_mapbmpswi)	(DtSwitch);
    void (*set_att_mapbmp)	(DtInt, DtObject[],DtObject[], dot_stdtexatt[]);
    void (*set_att_mapdifclrswi)(DtSwitch);
    void (*set_att_mapdifclr)	(DtInt, DtObject[],DtObject[], dot_stdtexatt[]);
    void (*set_att_mapenvswi)	(DtSwitch);
    void (*set_att_mapenv)	(DtInt, DtObject[],DtObject[], dot_stdtexatt[]);
    void (*set_att_maptrnintswi)(DtSwitch);
    void (*set_att_maptrnint)	(DtInt, DtObject[],DtObject[], dot_stdtexatt[]);
    void (*get_lcstowcsmat)	(DtMatrix4x4);

    void (*delete_local_device_data) (DtPtr);
    void (*delete_local_view_data)   (DtPtr);
    void (*delete_local_window_data) (DtPtr);

    DtFlag use_separate_wireframe;

    /* following fields only in version 2 and up */
 
    void (*set_att_srfedgclr)  (DtColorModel, DtReal[]);
    void (*set_att_localaaswi) (DtSwitch);
    void (*set_att_localaasty) (DtLocalAntiAliasStyle);
 
    void (*set_att_transporientclr) (DtColorModel, DtReal[]);
    void (*set_att_transporientexp) (DtReal);
    void (*set_att_transporientint) (DtReal);
    void (*set_att_transporientswi) (DtSwitch);

    void (*set_att_smpadapt) (DtReal);
    void (*set_att_smpadaptswi) (DtSwitch);
    void (*set_att_smpfilter) (DtObject, DtReal, DtReal);
    void (*set_att_smpjitter) (DtReal);
    void (*set_att_smpjitterswi) (DtSwitch);
    void (*set_att_smpsuper) (DtInt, DtInt);
    void (*set_att_smpsuperswi) (DtSwitch);
} DDt_DROM;


typedef struct {
    char        *name;
    DtMethodPtr  routine;
} DDt_DROMMethod;



    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

DtInt       DDclass_AddMethod            (char*);
DtInt       DDclass_AddInitializedMethod (char*, DtMethodPtr);
void        DDclass_AddNotify            (DtPFV);
DtInt       DDclass_AddObjectData        (DtInt, DtMethodPtr, DtMethodPtr,
					  DtMethodPtr);
DtInt       DDclass_CopyMethod           (DtInt, char*);
DtMethodPtr DDclass_InqDefaultMethod     (DtInt);
DtMethodPtr DDclass_InqMethod            (DtInt, DtInt);
DtInt       DDclass_ReplaceMethod        (char*, DtInt, DtInt);
void        DDclass_SetMethod            (DtInt, DtInt, DtMethodPtr);

DtFlag      DDcondex_QueryExecute            (DtInt);
DtFlag      DDcondex_QueryRender             (DtInt);
void        DDcondex_SetDisablePrimExecFlag  (DtFlag);
DtFlag      DDcondex_QueryPrimExecFlag       (void);

void        DDdevice_InstallDriver        (char*, char*, DtPFV);
DDt_DCM*    DDdevice_CreateDCMStruct      (void);
DDt_PROM*   DDdevice_CreatePROMStruct     (void);
DDt_DROM*   DDdevice_CreateDROMStruct     (void);
void        DDdevice_ClearRectangleDepth  (DtObject, DtInt,DtInt, DtInt,DtInt);
void        DDdevice_ClearRectangleDepthColor(DtObject,DtInt,DtInt,DtInt,DtInt);
void        DDdevice_Flush                (DtObject);
DtFlag      DDdevice_InqAbortedDraw       (DtObject);
void        DDdevice_InqClipList          (DtObject, DtUInt*, DtReal**,
					   DtReal[4], DtFlag*);
DtVolume*   DDdevice_InqClippingVolume    (DtObject);
void        DDdevice_InqFrameScaleTrans   (DtObject, DtReal[3], DtReal[3]);
void        DDdevice_InqInterfaceRoutines (DtObject,DDt_DeviceInterface,DtPtr*);
void        DDdevice_InqFrustumScaleTrans (DtObject, DtReal[3], DtReal[3]);
void        DDdevice_InqShadeRange        (DtObject, DtInt, DtInt*, DtInt*);
DtFlag      DDdevice_InqStereo            (DtObject);
DtVolume*   DDdevice_InqActualViewport    (DtObject);
void        DDdevice_SetAbortedDraw       (DtObject, DtFlag);
void        DDdevice_SetBackgroundColor   (DtObject, DtColorModel, DtReal[]);
void        DDdevice_SetDepthBufferEnable (DtObject, DtFlag);
void        DDdevice_SetDepthBufferWrite  (DtObject, DtFlag);
void        DDdevice_SetForegroundColor   (DtObject, DtColorModel, DtReal[]);
void        DDdevice_SetShadeIndex        (DtObject, DtInt);

char*       DDgetenv (char*);

void        DDobject_AddReference     (DtObject, DtObject);
void        DDobject_DeleteReference  (DtObject, DtObject);
DtMethodPtr DDobject_InqCurrentMethod (DtObject);
void        DDobject_Delete           (DtObject);
DtMethodPtr DDobject_InqDefaultMethod (DtObject);
DtMethodPtr DDobject_InqMethod        (DtObject, DtInt);

void        DDprint            (char*);
char*       DDprint_GetLine    (void);
void        DDprint_IndentUp   (void);
void        DDprint_IndentDown (void);

DtInt       DDrender_AddRenderer
		(DtInt, char*, DtMethodPtr, DtMethodPtr, DtPFV, DtMethodPtr);

DtPtr       DDspace_Allocate   (DtInt);
DtPtr       DDspace_Reallocate (DtPtr, unsigned);
void        DDspace_Deallocate (DtPtr);

char*       DDstrtok (char*, char*);

DtInt       DDsystem_InqNumberProcessors (void);

DtObject    DDview_ResizeRaster    (DtObject, DtInt, DtInt);
DtFlag      DDview_TraverseDisplay (DtObject, DtInt);
DtFlag      DDview_TraverseStudio  (DtObject, DtInt);

#endif
