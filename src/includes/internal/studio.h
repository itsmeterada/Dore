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
 

#ifndef DORE_STUDIO_H
#define DORE_STUDIO_H
 
/*****************************************************************************
**  This file contains function prototypes for Dore' studio functions.
*****************************************************************************/

    /* Studio Function Prototypes */

dot_object* dor_camera_create  (void);
void        dor_camera_initialize  (void);
void        dor_camera_initialize_pick  (dot_object *);
void        dor_camera_print  (dot_object *);
dot_object* dor_cammat_create  (DtMatrix4x4);
void        dor_cammat_destroy  (dot_object *);
void        dor_cammat_execute  (dot_object *);
void        dor_cammat_initialize  (void);
void        dor_cammat_initialize_pick  (dot_object *);
void        dor_cammat_print  (dot_object *);
dot_object* dor_glbrndmaxobjs_create  (DtInt);
void        dor_glbrndmaxobjs_execute  (dot_object *);
void        dor_glbrndmaxobjs_initialize  (void);
void        dor_glbrndmaxobjs_print  (dot_object *);
dot_object* dor_glbrndmaxsub_create  (DtInt);
void        dor_glbrndmaxsub_execute  (dot_object *);
void        dor_glbrndmaxsub_initialize  (void);
void        dor_glbrndmaxsub_print  (dot_object *);
dot_object* dor_glbrndraylvl_create  (DtInt);
void        dor_glbrndraylvl_execute  (dot_object *);
void        dor_glbrndraylvl_initialize  (void);
void        dor_glbrndraylvl_print  (dot_object *);
dot_object* dor_intermdisptype_create  (DtIntermediateDispType, DtInt *);
void        dor_intermdisptype_destroy  (dot_object *);
void        dor_intermdisptype_execute  (dot_object *);
void        dor_intermdisptype_initialize  (void);
void        dor_intermdisptype_print  (dot_object *);
dot_object* dor_lgtatn_create  (DtReal, DtReal);
void        dor_lgtatn_destroy  (dot_object *);
void        dor_lgtatn_execute  (dot_object *);
void        dor_lgtatn_initialize  (void);
void        dor_lgtatn_print  (dot_object *);
dot_object* dor_lgtclr_create  (DtColorModel, DtReal []);
void        dor_lgtclr_destroy  (dot_object *);
void        dor_lgtclr_execute  (dot_object *);
void        dor_lgtclr_initialize  (void);
void        dor_lgtclr_print  (dot_object *);
dot_object* dor_lgtint_create  (DtReal);
void        dor_lgtint_destroy  (dot_object *);
void        dor_lgtint_execute  (dot_object *);
void        dor_lgtint_initialize  (void);
void        dor_lgtint_print  (dot_object *);
dot_object* dor_lgtspdang_create  (DtReal, DtReal);
void        dor_lgtspdang_destroy  (dot_object *);
void        dor_lgtspdang_execute  (dot_object *);
void        dor_lgtspdang_initialize  (void);
void        dor_lgtspdang_print  (dot_object *);
dot_object* dor_lgtspdexp_create  (DtReal);
void        dor_lgtspdexp_destroy  (dot_object *);
void        dor_lgtspdexp_execute  (dot_object *);
void        dor_lgtspdexp_initialize  (void);
void        dor_lgtspdexp_print  (dot_object *);
dot_object* dor_lgttyp_create  (DtObject);
void        dor_lgttyp_destroy  (dot_object *);
void        dor_lgttyp_execute  (dot_object *);
void        dor_lgttyp_initialize  (void);
void        dor_lgttyp_initialize_types  (void);
void        dor_lgttyp_print  (dot_object *);
dot_object* dor_light_create  (void);
void        dor_light_initialize  (void);
void        dor_light_print  (dot_object *);
dot_object* dor_parallel_create  (DtReal, DtReal, DtReal);
void        dor_parallel_destroy  (dot_object *);
void        dor_parallel_execute  (dot_object *);
void        dor_parallel_initialize  (void);
void        dor_parallel_initialize_pick  (dot_object *);
void        dor_parallel_print  (dot_object *);
dot_object* dor_perspective_create  (DtReal, DtReal, DtReal);
void        dor_perspective_destroy  (dot_object *);
void        dor_perspective_execute  (dot_object *);
void        dor_perspective_initialize  (void);
void        dor_perspective_initialize_pick  (dot_object *);
void        dor_perspective_print  (dot_object *);
dot_object* dor_projection_create  (DtArea *, DtProjectionType, DtPoint3,
				    DtReal, DtReal, DtReal);
void        dor_projection_destroy  (dot_object *);
void        dor_projection_execute  (dot_object *);
void        dor_projection_initialize  (void);
void        dor_projection_initialize_pick  (dot_object *);
void        dor_projection_print  (dot_object *);
dot_object* dor_smpadapt_create  (DtReal);
void        dor_smpadapt_destroy  (dot_object *);
void        dor_smpadapt_execute  (dot_object *);
void        dor_smpadapt_initialize  (void);
void        dor_smpadapt_print  (dot_object *);
dot_object* dor_smpadaptswi_create  (DtSwitch);
void        dor_smpadaptswi_destroy  (dot_object *);
void        dor_smpadaptswi_execute  (dot_object *);
void        dor_smpadaptswi_initialize  (void);
void        dor_smpadaptswi_print  (dot_object *);
dot_object* dor_smpfilter_create  (DtObject, DtReal, DtReal);
void        dor_smpfilter_destroy  (dot_object *);
void        dor_smpfilter_execute  (dot_object *);
void        dor_smpfilter_initialize  (void);
void        dor_smpfilter_initialize_filters  (void);
void        dor_smpfilter_print  (dot_object *);
dot_object* dor_smpjitter_create  (DtReal);
void        dor_smpjitter_destroy  (dot_object *);
void        dor_smpjitter_execute  (dot_object *);
void        dor_smpjitter_initialize  (void);
void        dor_smpjitter_print  (dot_object *);
dot_object* dor_smpjitterswi_create  (DtSwitch);
void        dor_smpjitterswi_destroy  (dot_object *);
void        dor_smpjitterswi_execute  (dot_object *);
void        dor_smpjitterswi_initialize  (void);
void        dor_smpjitterswi_print  (dot_object *);
dot_object* dor_smpsuper_create  (DtInt, DtInt);
void        dor_smpsuper_destroy  (dot_object *);
void        dor_smpsuper_execute  (dot_object *);
void        dor_smpsuper_initialize  (void);
void        dor_smpsuper_print  (dot_object *);
dot_object* dor_smpsuperswi_create  (DtSwitch);
void        dor_smpsuperswi_destroy  (dot_object *);
void        dor_smpsuperswi_execute  (dot_object *);
void        dor_smpsuperswi_initialize  (void);
void        dor_smpsuperswi_print  (dot_object *);
dot_object* dor_stereo_create  (DtReal, DtReal);
void        dor_stereo_destroy  (dot_object *);
void        dor_stereo_execute  (dot_object *);
void        dor_stereo_initialize  (void);
void        dor_stereo_print  (dot_object *);
dot_object* dor_stereoswi_create  (DtSwitch);
void        dor_stereoswi_execute  (dot_object *);
void        dor_stereoswi_initialize  (void);
void        dor_stereoswi_print  (dot_object *);

#endif
