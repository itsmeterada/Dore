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
	void ddr_vll_drom_set_att_clpswi (switchvalue)
	void ddr_vll_drom_apply_att_clpvol (operator,halfspacecount,halfspaces)
	void ddr_vll_drom_push_att_clpvol ()
	void ddr_vll_drom_pop_att_clpvol ()
	void ddr_vll_drom_set_att_clpvol (operator,halfspacecount,halfspaces)
	void ddr_vll_drom_update_clpvol (modclpvol)

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_clpswi(switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.modelclip_switch = switchvalue;
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_apply_att_clpvol(operator,halfspacecount,halfspaces)
	DtClipOperator operator;
	DtInt halfspacecount;
	DtHalfSpace *halfspaces;
{
    ddt_vll_drom_clpvol_stack *cs;
    DtInt i, j;
    DtMatrix4x4 objectmatrix;
    float mat[4][4];

    cs = &(dde_vll_drom_data.clpvol_stack);

    VLLquery_matrix(VLL_OBJECT_MATRIX, mat);

    if (sizeof(float) != sizeof(DtReal)) {
	for (i=0; i<4; i++) {
	    for (j=0; j<4; j++) {
		objectmatrix[i][j] = mat[i][j];
	    }
	}
	dor_modclpvol_apply_volume(cs->elements[cs->current_element].modclpvol,
				   objectmatrix,
				   operator,halfspacecount,halfspaces);

    } else {
	dor_modclpvol_apply_volume(cs->elements[cs->current_element].modclpvol,
				   mat, operator,halfspacecount,halfspaces);
    }

    ddr_vll_drom_update_clpvol(cs->elements[cs->current_element]);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_push_att_clpvol()
{
    struct modclpvol *dor_modclpvol_copy();
    ddt_vll_drom_clpvol_stack *cs;

    cs = &(dde_vll_drom_data.clpvol_stack);

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
void 
ddr_vll_drom_pop_att_clpvol ()
{
    ddt_vll_drom_clpvol_stack *cs;

    cs = &(dde_vll_drom_data.clpvol_stack);

    if (cs->current_element >= 0) {
	dor_modclpvol_destroy(cs->elements[cs->current_element].modclpvol);
	cs->elements[cs->current_element].modclpvol = NULL;
	cs->current_element--;

	ddr_vll_drom_update_clpvol(cs->elements[cs->current_element]);
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_clpvol(operator,halfspacecount,halfspaces)
	DtClipOperator operator;
	DtInt halfspacecount;
	DtHalfSpace *halfspaces;
{
    ddt_vll_drom_clpvol_stack *cs;
    DtInt i, j;
    DtMatrix4x4 objectmatrix;
    float mat[4][4];

    if (dde_vll_drom_pre_init) return;

    cs = &(dde_vll_drom_data.clpvol_stack);
   
    VLLquery_matrix(VLL_OBJECT_MATRIX, mat);

    if (sizeof(float) != sizeof(DtReal)) {
	for (i=0; i<4; i++) {
	    for (j=0; j<4; j++) {
		objectmatrix[i][j] = mat[i][j];
	    }
	}
	dor_modclpvol_set_value(cs->elements[cs->current_element].modclpvol,
				objectmatrix,
				operator,halfspacecount,halfspaces);
    } else {
	dor_modclpvol_set_value(cs->elements[cs->current_element].modclpvol,
				mat, operator,halfspacecount,halfspaces);
    }
    ddr_vll_drom_update_clpvol(cs->elements[cs->current_element]);
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_update_clpvol(modclpvol)
	struct modclpvol *modclpvol;
{
    DtInt halfspaceno;
    DtInt halfspacecount;
    DtPoint3 point[50];
    DtVector3 vector[50];
    DtPoint4 plneqn[50];

    if (dde_vll_drom_pre_init) return;

    dor_modclpvol_get_value(modclpvol,&halfspacecount,point,vector,plneqn);

    if (halfspacecount > 8) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "ddr_vll_drom_update_clpvol",
		    "Only the first 8 Clipping planes used\n");
	halfspacecount = 8;
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

	if (sizeof(float) != sizeof(DtReal)) {
	    float plane[4];
	    int i;

	    for (i=0; i<4; i++) {
		plane[i] = plneqn[halfspaceno][i];
	    }
	    VLLset_clip_plane(halfspaceno, 0, plane);
	} else {
	    VLLset_clip_plane(halfspaceno, 0, (float *)plneqn[halfspaceno]);
	}
    }

    VLLset_number_of_clip_planes(halfspacecount);
}
/*
 ======================================================================
 */
