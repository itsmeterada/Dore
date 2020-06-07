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
	int ddr_sunview_cxp_drom_set_att_clpswi (switchvalue)
	int ddr_sunview_cxp_drom_apply_att_clpvol (operator,halfspacecount,halfspaces)
	int ddr_sunview_cxp_drom_push_att_clpvol ()
	ddr_sunview_cxp_drom_pop_att_clpvol ddr_sunview_cxp_drom_pop_att_clpvol ()
	int ddr_sunview_cxp_drom_set_att_clpvol (operator,halfspacecount,halfspaces)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

	/*  Modelling Clip Switch.  */

ddr_sunview_cxp_drom_set_att_clpswi(switchvalue)
     DtSwitch switchvalue;

{
    dde_sunview_cxp_drom_data.object_modelclip_switch = switchvalue;

    ddr_sunview_cxp_drom_update_state();
}
/*
 ======================================================================
 */

	/*  Modelling Clipping Volume.  */

ddr_sunview_cxp_drom_apply_att_clpvol(operator,halfspacecount,halfspaces)
     DtClipOperator operator;
     DtInt halfspacecount;
     DtHalfSpace *halfspaces;
{
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_push_att_clpvol()
{
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_pop_att_clpvol ()
{
}
/*
 ======================================================================
 */

	/*  Modelling Clipping Volume.  */

ddr_sunview_cxp_drom_set_att_clpvol(operator,halfspacecount,halfspaces)
     DtClipOperator operator;
     DtInt halfspacecount;
     DtHalfSpace *halfspaces;
{
}
