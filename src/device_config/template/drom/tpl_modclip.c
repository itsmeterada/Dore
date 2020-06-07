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
	void ddr_template_drom_set_att_clpswi (switchvalue)
	void ddr_template_drom_apply_att_clpvol (operator,halfspacecount,halfspaces)
	void ddr_template_drom_push_att_clpvol ()
	void ddr_template_drom_pop_att_clpvol ()
	void ddr_template_drom_set_att_clpvol (operator,halfspacecount,halfspaces)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
	/*  Modelling Clip Switch */
void ddr_template_drom_set_att_clpswi (
    DtSwitch switchvalue)
{
}
/*
 ======================================================================
 */
	/*  Modelling Clip Volume */
void ddr_template_drom_apply_att_clpvol (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
}
/*
 ======================================================================
 */
	/* Push Modelling Clip Volume */
void ddr_template_drom_push_att_clpvol (void)
{
}
/*
 ======================================================================
 */
	/* Pop Modelling Clip Volume */
void ddr_template_drom_pop_att_clpvol (void)
{
}
/*
 ======================================================================
 */
	/*  Modelling Clipping Volume */
void ddr_template_drom_set_att_clpvol (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
}
/*
 ======================================================================
 */
