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
	dor_render_get_wcstofcsmat
	dor_render_inq_num_renderers
	dor_render_inq_renderer_id
	dor_render_inq_renderer_names

  ======================================================================
 */

#include <dore/internal/dogen.h>
#include "render.h"



/* ====================================================================== */

void dor_render_get_wcstofcsmat (DtMatrix4x4 wcstofcsmat)
{
    if (doe_render_current_renderer_id >= 0 &&
	doe_render_current_renderer_id < doe_render_next_renderer_id) {

	(*(doe_render_renderer[doe_render_current_renderer_id].inq_wcstofcs_rtn))
		(wcstofcsmat);
    }
}



/* ====================================================================== */

DtInt dor_render_inq_num_renderers (void)
{
    Dt32Bits cnt;
    Dt32Bits i;

    for (i=0, cnt=0; i<doe_render_next_renderer_id; i++) {
	if (doe_render_renderer[i].name)
	    cnt++;
    }

    return cnt;
}



/* ====================================================================== */

DtInt dor_render_inq_renderer_id (char *renderer_name)
{
    Dt32Bits i;

    if (!renderer_name) 
	return -1;

    for (i=0; i<doe_render_next_renderer_id; i++) {
	if ((doe_render_renderer[i].name) &&
	    (strcmp(doe_render_renderer[i].name, renderer_name) == 0)) {
	    /*
	     * found renderer_name
	     */
	    return i;
	}
    }
    return -1;
}



/* ====================================================================== */

void dor_render_inq_renderer_names (char *names[])
{
    Dt32Bits rndid;
    Dt32Bits i;

    for (rndid=0, i=0; rndid<doe_render_next_renderer_id; rndid++) {
	if (doe_render_renderer[rndid].name) 
		names[i++] = doe_render_renderer[rndid].name;
    }
}
