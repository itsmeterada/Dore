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
	int ddr_x11_XFDI_drom_set_att_clpswi (switchvalue)
	int ddr_x11_XFDI_drom_apply_att_clpvol (operator,halfspacecount,halfspaces)
	int ddr_x11_XFDI_drom_push_att_clpvol ()
	ddr_x11_XFDI_drom_pop_att_clpvol ddr_x11_XFDI_drom_pop_att_clpvol ()
	int ddr_x11_XFDI_drom_set_att_clpvol (operator,halfspacecount,halfspaces)
	int ddr_x11_XFDI_drom_update_clpvol (modclpvol)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_clpswi(switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.modelclip_switch = switchvalue;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_apply_att_clpvol(operator,halfspacecount,halfspaces)
     DtClipOperator operator;
     DtInt halfspacecount;
     DtHalfSpace *halfspaces;
{
    ddt_x11_XFDI_drom_clpvol_stack *cs;
    DtInt i, j;
    DtMatrix4x4 objectmatrix;
    int matrixflag;

    cs = &(dde_x11_XFDI_drom_data.clpvol_stack);

    XFDIQueryMatrix(dde_x11_XFDI_drom_curdevdat->display,
		    dde_x11_XFDI_drom_curdevdat->gc,
		    XFDIObjectMatrix,
		    objectmatrix,
		    &matrixflag);
    dor_matrix_transpose(objectmatrix);

    dor_modclpvol_apply_volume(cs->elements[cs->current_element].modclpvol,
			       objectmatrix,
			       operator,halfspacecount,halfspaces);

    ddr_x11_XFDI_drom_update_clpvol(cs->elements[cs->current_element]);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_push_att_clpvol()
{
    struct modclpvol *dor_modclpvol_copy();
    ddt_x11_XFDI_drom_clpvol_stack *cs;

    cs = &(dde_x11_XFDI_drom_data.clpvol_stack);

    if (cs->current_element == cs->last_element) {
	cs->elements = (struct clpvol_element *)DDspace_Reallocate
		(cs->elements,((cs->last_element+1)+10)*
		 (sizeof (struct clpvol_element)));

	cs->last_element += 10;
    }
		
    cs->elements[cs->current_element+1].modclpvol = 
	    dor_modclpvol_copy(cs->elements[cs->current_element].modclpvol);

    cs->current_element++;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_pop_att_clpvol ()
{
    ddt_x11_XFDI_drom_clpvol_stack *cs;

    cs = &(dde_x11_XFDI_drom_data.clpvol_stack);

    if (cs->current_element >= 0) {
	dor_modclpvol_destroy(cs->elements[cs->current_element].modclpvol);
	cs->elements[cs->current_element].modclpvol = NULL;
	cs->current_element--;

	ddr_x11_XFDI_drom_update_clpvol(cs->elements[cs->current_element]);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_clpvol(operator,halfspacecount,halfspaces)
     DtClipOperator operator;
     DtInt halfspacecount;
     DtHalfSpace *halfspaces;
{
    ddt_x11_XFDI_drom_clpvol_stack *cs;
    DtInt i, j;
    DtMatrix4x4 objectmatrix;
    int matrixflag;

    if (dde_x11_XFDI_drom_pre_init) return;

    cs = &(dde_x11_XFDI_drom_data.clpvol_stack);
   
    XFDIQueryMatrix(dde_x11_XFDI_drom_curdevdat->display,
		    dde_x11_XFDI_drom_curdevdat->gc,
		    XFDIObjectMatrix,
		    objectmatrix,
		    &matrixflag);
    dor_matrix_transpose(objectmatrix);

    dor_modclpvol_set_value(cs->elements[cs->current_element].modclpvol,
			    objectmatrix,
			    operator,halfspacecount,halfspaces);
    ddr_x11_XFDI_drom_update_clpvol(cs->elements[cs->current_element]);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_update_clpvol(modclpvol)
     struct modclpvol *modclpvol;
{
    DtInt halfspaceno;
    DtInt halfspacecount;
    DtPoint3 point[50];
    DtVector3 vector[50];
    DtPoint4 plneqn[50];
    DtInt clip_code;

    if (dde_x11_XFDI_drom_pre_init) return;

    dor_modclpvol_get_value(modclpvol,&halfspacecount,point,vector,plneqn);

    if (halfspacecount > 16) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_x11_XFDI_drom_update_clpvol",
		    "Only the first 16 Clipping planes used\n");
	halfspacecount = 16;
    }

    for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++) {
#ifdef DEBUG
	printf("halfspace %d point = (%lf,%lf,%lf)\n",
	       halfspaceno,
	       point[halfspaceno][0],point[halfspaceno][1],point[halfspaceno][2]);

	printf("halfspace %d vector = (%lf,%lf,%lf)\n",
	       halfspaceno,
	       vector[halfspaceno][0],vector[halfspaceno][1],vector[halfspaceno][2]);

	printf("halfspace %d fcs plane equation = (%lf,%lf,%lf,%lf)\n",
	       halfspaceno,
	       plneqn[halfspaceno][0],
	       plneqn[halfspaceno][1],
	       plneqn[halfspaceno][2],
	       plneqn[halfspaceno][3]);
#endif

	XFDISetClipPlane(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 halfspaceno, XFDIClipAfterTransform,
			 plneqn[halfspaceno][0],
			 plneqn[halfspaceno][1],
			 plneqn[halfspaceno][2],
			 plneqn[halfspaceno][3],
			 0.0);
    }

    clip_code = XFDIClipAll;

    switch (halfspacecount) {
    case 0:	clip_code |= XFDIClipPlane0;
    case 1:	clip_code |= XFDIClipPlane1;
    case 2:	clip_code |= XFDIClipPlane2;
    case 3:	clip_code |= XFDIClipPlane3;
    case 4:	clip_code |= XFDIClipPlane4;
    case 5:	clip_code |= XFDIClipPlane5;
    case 6:	clip_code |= XFDIClipPlane6;
    case 7:	clip_code |= XFDIClipPlane7;
    case 8:	clip_code |= XFDIClipPlane8;
    case 9:	clip_code |= XFDIClipPlane9;
    case 10:	clip_code |= XFDIClipPlane10;
    case 11:	clip_code |= XFDIClipPlane11;
    case 12:	clip_code |= XFDIClipPlane12;
    case 13:	clip_code |= XFDIClipPlane13;
    case 14:	clip_code |= XFDIClipPlane14;
    case 15:	clip_code |= XFDIClipPlane15;
    }

    XFDISetClipLevel (dde_x11_XFDI_drom_curdevdat->display,
		      dde_x11_XFDI_drom_curdevdat->gc,
		      clip_code);
}
