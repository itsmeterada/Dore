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
 
/*
  ======================================================================
  Functions:
	DtPtr dor_render_obj_data_crt (object)
	void dor_render_obj_data_del (object, data)
	void dor_render_obj_data_prt (object, data)

  ======================================================================
 */

#include <dore/internal/dogen.h>
#include "render.h"
/*
 ======================================================================
 */
DtPtr dor_render_obj_data_crt (
    DtObject object)
{
    dot_render_obj_data *data;

    if ((data = (dot_render_obj_data *)
	 DDspace_Allocate(sizeof(dot_render_obj_data)))
	== (dot_render_obj_data *)DcNullPtr) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE,
		    "dor_render_obj_data_crt",
		    "sizeof (dot_render_obj_data)");
	return DcNullPtr;
    }

    data->seq_num = -1;

    return (DtPtr)data;
}
/*
 ======================================================================
 */
void dor_render_obj_data_del (
    DtObject object,
    dot_render_obj_data *data)
{
    DDspace_Deallocate(data);
}
/*
 ======================================================================
 */
void dor_render_obj_data_prt (
    DtObject object,
    dot_render_obj_data *data)
{
    dor_print_output("RenderData:");
    dor_print_indent_up();
    sprintf (dor_print_get_line(), "last sequence number: %d", 
	     data->seq_num);
    dor_print_output (dor_print_get_line());
    dor_print_indent_down();
}
