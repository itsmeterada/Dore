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
 
#include "dore.h"
#include "dore/dore_develop/error.h"
#include "dore/dore_develop/develop.h"

/* 
 * Data stuctures for sample renderer
 */

/*
===============================================================================
*/

/*
 * Structure to keep correspondence between classes and method routines
 * for each traversal method
 */

typedef struct {
   DtPtr  name;
   DtPFI  routine;
} drt_samplernd_methods;

/*
 * Structure for light list to be kept with view
 */

typedef struct {
   DtObject	type;
   DtColorRGB	color;
   DtPoint3	pos;
   DtVector3	dir;
} drt_samplernd_light_data, *drt_samplernd_light_data_ptr;

/*
 * Structure for additional data for view object
 */

typedef struct {
   DtCameraMatrixType  camera_type;
   DtMatrix4x4  projmat;
   DtMatrix4x4  ccstowcsmat; /* camera-to-world matrix */
   DtMatrix4x4  wcstoccsmat; /* world-to-camera matrix */
   DtMatrix4x4  wcstofcsmat; /* world-to-frustum matrix */
   DtInt        max_lights;
   DtInt        light_cnt;
   drt_samplernd_light_data_ptr  lights;
   DtFlag       clear_flag;
   DtColorModel background_color_model;
   DtColorRGB   background_color;
   DtInt        background_shade_index;
} drt_samplernd_view_data, *drt_samplernd_view_data_ptr;

/*
 * Structure for additional data for device object
 */

typedef struct {
   DtReal	extent[4];
   DtReal	scale[3];
   DtReal	trans[3];
   DtShadeMode	shade_mode;
   DtVisualType	visual_type;
} drt_samplernd_device_data, *drt_samplernd_device_data_ptr;

/*
 * Structure for sample renderer's private data
 */

typedef struct {
   DtInt        studio_traversal;
   DtInt        display_traversal;
   DtObject     view_handle;
   DtObject     device_handle;
   DtInt        view_index;
   DtInt        device_index;
   DtInt        pntlst_index;
   DDt_PROM     *output;
   drt_samplernd_view_data_ptr    viwdat;
   drt_samplernd_device_data_ptr  devdat;
   DtRepType    reptyp;
   DtColorRGB   difclr;
   DtReal       difint;
   DtSwitch     difswi;
   DtMatrix4x4  lcstowcsmat;
} drt_samplernd_render_data, *drt_samplernd_render_data_ptr;

extern drt_samplernd_render_data_ptr dre_samplernd_render_private;

extern drt_samplernd_methods dre_samplernd_studio_method[];
extern drt_samplernd_methods dre_samplernd_display_method[];

extern DtShort  drr_samplernd_matrix_solve();
extern DtFlag   drr_samplernd_vector_normalize();

