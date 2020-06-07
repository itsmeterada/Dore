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
    char        *name;
    DtMethodPtr  render_rtn;
    DtMethodPtr  del_rtn;
    DtMethodPtr  inq_wcstofcs_rtn;
    dot_object  *device;
    dot_object  *view;
    Dt32Bits     device_data_id;
    Dt32Bits     view_data_id;
} dot_render_data;

typedef struct {
    Dt32Bits seq_num;
} dot_render_obj_data;

extern Dt32Bits 	doe_render_current_renderer_id;
extern dot_object *	doe_render_current_device;
extern dot_object *	doe_render_current_view;

extern Dt32Bits 	doe_render_next_renderer_id;
extern dot_render_data *doe_render_renderer;


    /***  Function Prototypes  ***/

	/* The following functions are local to the files in this directory.
	** All other functions are visible elsewhere and are prototyped in
	** dogen.h.  */

DtPtr dor_render_obj_data_crt  (DtObject);
void  dor_render_obj_data_del  (DtObject, dot_render_obj_data *);
void  dor_render_obj_data_prt  (DtObject, dot_render_obj_data *);
void  dor_render_print         (void);
