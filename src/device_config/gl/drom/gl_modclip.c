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
	ddr_gl_drom_apply_att_clpvol
	ddr_gl_drom_pop_att_clpvol
	ddr_gl_drom_push_att_clpvol
	ddr_gl_drom_set_att_clpswi
	ddr_gl_drom_set_att_clpvol

  ======================================================================
 */
#include "../gl_driver.h"




/* ======================================================================== */

void ddr_gl_drom_set_att_clpswi (
    DtSwitch switchvalue)
{
}



/* ======================================================================== */

void ddr_gl_drom_apply_att_clpvol (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
#   ifdef DEBUGP
	printf("Apply clip volume NOT IMPLEMENTED\n");
#   endif
}



/* ======================================================================== */

void ddr_gl_drom_push_att_clpvol (void)
{
#   ifdef DEBUGP
	printf("Push clip volume NOT IMPLEMENTED\n");
#   endif
}



/* ======================================================================== */

void ddr_gl_drom_pop_att_clpvol (void)
{
#   ifdef DEBUGP
	printf("Pop clip volume NOT IMPLEMENTED\n");
#   endif
}



/* ======================================================================== */

void ddr_gl_drom_set_att_clpvol (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
}
