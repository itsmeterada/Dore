#ifndef DD_VIEW_H
#define DD_VIEW_H
 
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
 
#include <dore/internal/dogen.h>

typedef struct {
   DtInt       width;
   DtInt       height;
   dot_object *raster;
} dot_resized_raster;

typedef struct view {
   dot_object		*next;		/* next frame object */
   dot_object		*active_camera;
   DtColorModel		color_type;
   DtReal		background_color[3];
   DtInt		shade_index;
   DtFlag		clear_flag;
   dot_object		*display_group;
   dot_object		*definition_group;
   DtRenderStyle	render_style;
   DtUpdateType		update_type;
   DtVolume		boundary; 
   dot_object_reference	*references;

   DtInt		modification_sequence_number;

   DtReal		background_left_just;
   DtReal		background_bottom_just;
   dot_object		*background_raster;
   dot_object		*background_callback;
   DtInt		background_raster_count;
   dot_resized_raster	*background_resized_raster;

   DtPtr                *render_data[3];

   DtFormFactorCompType  formfactor_comptype;
   DtReal                *formfactor_comptype_parms;
   DtRadiosityConvType   rad_convergence_type;
   DtReal                *rad_convergence_parms;
   DtRadiosityUpdateType rad_update_type;

} dot_view;

extern dot_object *doe_view_list;
extern DtInt doe_view_class_id;

#endif
