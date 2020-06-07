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
	int ddr_x11_DGL_drom_set_att_clpswi (switchvalue)
	int ddr_x11_DGL_drom_apply_att_clpvol (operator,halfspacecount,halfspaces)
	int ddr_x11_DGL_drom_push_att_clpvol ()
	ddr_x11_DGL_drom_pop_att_clpvol ddr_x11_DGL_drom_pop_att_clpvol ()
	int ddr_x11_DGL_drom_set_att_clpvol (operator,halfspacecount,halfspaces)
	int ddr_x11_DGL_drom_update_clpvol (modclpvol)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

	/*  Modelling Clip Switch.  */

ddr_x11_DGL_drom_set_att_clpswi(switchvalue)
     DtSwitch switchvalue;

{
    dde_x11_DGL_drom_data.object_modelclip_switch = switchvalue;

    XdSetClipSwitch(dde_x11_DGL_drom_curdevdat->pGC,
		    (XdSwitch)switchvalue);
}
/*
 ======================================================================
 */

	/*  Modelling Clipping Volume.  */
PRAGMA(OPT_LEVEL 2)

ddr_x11_DGL_drom_apply_att_clpvol(operator,halfspacecount,halfspaces)
     DtClipOperator operator;
     DtInt halfspacecount;
     DtHalfSpace *halfspaces;
{
    struct clpvol_stack *cs;
    DtInt i, j;
    DtMatrix4x4 newmatrix;

    cs = &(dde_x11_DGL_drom_data.clpvol_stack);

    if (sizeof (DtReal) != sizeof(XdReal)) {
	for (i=0; i<4; i++) {
PRAGMA(IVDEP)
	    for (j=0; j<4; j++) {
		newmatrix[i][j] = dde_x11_DGL_drom_data.object_lcstowcsmat[i][j];
	    }
	}
	dor_modclpvol_apply_volume(cs->elements[cs->current_element].modclpvol,
				   newmatrix,operator,halfspacecount,halfspaces);
    } else {
	dor_modclpvol_apply_volume(cs->elements[cs->current_element].modclpvol,
				   dde_x11_DGL_drom_data.object_lcstowcsmat,
				   operator,halfspacecount,halfspaces);
    }
    ddr_x11_DGL_drom_update_clpvol(cs->elements[cs->current_element]);
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_push_att_clpvol()
{
    struct modclpvol *dor_modclpvol_copy();
    struct clpvol_stack *cs;

    cs = &(dde_x11_DGL_drom_data.clpvol_stack);

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

ddr_x11_DGL_drom_pop_att_clpvol ()
{
    struct clpvol_stack *cs;

    cs = &(dde_x11_DGL_drom_data.clpvol_stack);

    if (cs->current_element >= 0) {
	dor_modclpvol_destroy(cs->elements[cs->current_element].modclpvol);
	cs->elements[cs->current_element].modclpvol = NULL;
	cs->current_element--;

	ddr_x11_DGL_drom_update_clpvol(cs->elements[cs->current_element]);
    }
}
/*
 ======================================================================
 */

	/*  Modelling Clipping Volume.  */
PRAGMA(OPT_LEVEL 2)

ddr_x11_DGL_drom_set_att_clpvol(operator,halfspacecount,halfspaces)
     DtClipOperator operator;
     DtInt halfspacecount;
     DtHalfSpace *halfspaces;
{
    struct clpvol_stack *cs;
    DtInt i, j;
    DtMatrix4x4 newmatrix;

    cs = &(dde_x11_DGL_drom_data.clpvol_stack);


    if (sizeof (DtReal) != sizeof(XdReal)) {
	for (i=0; i<4; i++) {
PRAGMA(IVDEP)
	    for (j=0; j<4; j++) {
		newmatrix[i][j] = dde_x11_DGL_drom_data.object_lcstowcsmat[i][j];
	    }
	}
	dor_modclpvol_set_value(cs->elements[cs->current_element].modclpvol,
				newmatrix, operator,halfspacecount,halfspaces);
    } else {
	dor_modclpvol_set_value(cs->elements[cs->current_element].modclpvol,
				dde_x11_DGL_drom_data.object_lcstowcsmat,
				operator,halfspacecount,halfspaces);
    }
    ddr_x11_DGL_drom_update_clpvol(cs->elements[cs->current_element]);
}

/*
 ======================================================================
 */

ddr_x11_DGL_drom_update_clpvol(modclpvol)
     struct modclpvol *modclpvol;
{
    DtInt halfspaceno;
    DtInt halfspacecount;
    DtPoint3 point[50];
    DtVector3 vector[50];
    DtPoint4 plneqn[50];
    int nplanes[2];

    dor_modclpvol_get_value(modclpvol,&halfspacecount,point,vector,plneqn);

    nplanes[0] = halfspacecount;

    dde_x11_DGL_drom_data.modelclip_halfspacecount = halfspacecount;

    if (dde_x11_DGL_drom_pre_init) {
	for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++) {
	    dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][0] =
		    plneqn[halfspaceno][0];
	    dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][1] =
		    plneqn[halfspaceno][1];
	    dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][2] =
		    plneqn[halfspaceno][2];
	}
	return;
    }

    for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++) {
#ifdef DEBUG
	printf("halfspace %d point = (%lf,%lf,%lf)\n",
	       halfspaceno,
	       point[halfspaceno][0],point[halfspaceno][1],point[halfspaceno][2]);

	printf("halfspace %d vector = (%lf,%lf,%lf)\n",
	       halfspaceno,
	       vector[halfspaceno][0],vector[halfspaceno][1],vector[halfspaceno][2]);

	printf("halfspace %d wcs plane equation = (%lf,%lf,%lf,%lf)\n",
	       halfspaceno,
	       plneqn[halfspaceno][0],
	       plneqn[halfspaceno][1],
	       plneqn[halfspaceno][2],
	       plneqn[halfspaceno][3]);
#endif

	XdMathHomoTrns((XdReal)plneqn[halfspaceno][0],
		       (XdReal)plneqn[halfspaceno][1],
		       (XdReal)plneqn[halfspaceno][2],
		       (XdReal)plneqn[halfspaceno][3],
		       dde_x11_DGL_drom_curcamdat->tspfcstowcsmat,
		       &dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][0],
		       &dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][1],
		       &dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][2],
		       &dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][3]);

#ifdef DEBUG
	printf("halfspace %d fcs scaled plane equation = (%lf,%lf,%lf,%lf)\n",
	       halfspaceno,
	       dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][0],
	       dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][1],
	       dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][2],
	       dde_x11_DGL_drom_data.modelclip_plneqns[halfspaceno][3]);
#endif
    }

    XdSetModelingPlanes(dde_x11_DGL_drom_curdevdat->pGC,
			1,
			nplanes,
			dde_x11_DGL_drom_data.modelclip_plneqns);
}
