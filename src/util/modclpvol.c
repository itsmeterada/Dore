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
	dor_modclpvol_create
	dor_modclpvol_destroy
	dor_modclpvol_print
	dor_modclpvol_copy
	dor_modclpvol_apply_volume
	dor_modclpvol_get_value
	dor_modclpvol_set_value
	dor_modclpvol_plane_equation

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/matrix.h>
#include <dore/internal/modclpvol.h>

/*
 ======================================================================
 */

dot_modclpvol *dor_modclpvol_create (
    DtMatrix4x4    lcstowcs,
    DtClipOperator operator,
    DtInt          halfspacecount,
    DtHalfSpace   *halfspaces)
{
    DtMatrix4x4 tspwcstolcsmat;
    dot_modclpvol *modclpvol;
    DtInt halfspaceno;

    dor_matrix_load (tspwcstolcsmat,lcstowcs);
    dor_matrix_invert(tspwcstolcsmat);
    dor_matrix_transpose(tspwcstolcsmat);

    modclpvol = (dot_modclpvol *)dor_space_allocate(sizeof *modclpvol);

    modclpvol->halfspacecount = halfspacecount;

    if (halfspacecount > 0){
	modclpvol->halfspaces = (DtHalfSpace *)dor_space_allocate
					(halfspacecount*(sizeof (DtHalfSpace)));
	modclpvol->plneqns = (DtPoint4 *)dor_space_allocate
					(halfspacecount*(sizeof (DtPoint4)));

	for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++){
	    modclpvol->halfspaces[halfspaceno].point[0] =
		    halfspaces[halfspaceno].point[0];
	    modclpvol->halfspaces[halfspaceno].point[1] =
		    halfspaces[halfspaceno].point[1];
	    modclpvol->halfspaces[halfspaceno].point[2] =
		    halfspaces[halfspaceno].point[2];

	    modclpvol->halfspaces[halfspaceno].vector[0] =
		    halfspaces[halfspaceno].vector[0];
	    modclpvol->halfspaces[halfspaceno].vector[1] =
		    halfspaces[halfspaceno].vector[1];
	    modclpvol->halfspaces[halfspaceno].vector[2] =
		    halfspaces[halfspaceno].vector[2];

	    dor_modclpvol_plane_equation(modclpvol,halfspaceno,tspwcstolcsmat);
	}
    }
    else{
	modclpvol->halfspaces = NULL;
	modclpvol->plneqns = NULL;
    }

    return(modclpvol);
}
/*
 ======================================================================
 */

void dor_modclpvol_destroy (dot_modclpvol *modclpvol)
{
    if (modclpvol->halfspaces != NULL){
	dor_space_deallocate(modclpvol->halfspaces);
	dor_space_deallocate(modclpvol->plneqns);
    }

    dor_space_deallocate(modclpvol);
}
/*
 ======================================================================
 */

void dor_modclpvol_print (dot_modclpvol *modclpvol)
{
    DtInt halfspaceno;

    sprintf(dor_print_get_line(),"modclpvol at [%lx]",modclpvol);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"halfspacecount = %d",
	    modclpvol->halfspacecount);
    dor_print_output(dor_print_get_line());

    for (halfspaceno=0;halfspaceno<modclpvol->halfspacecount;halfspaceno++){
	sprintf(dor_print_get_line(),
		"  halfspace %d point, vector = (%lf,%lf,%lf) (%lf,%lf,%lf)",
		halfspaceno,
		modclpvol->halfspaces[halfspaceno].point[0],
		modclpvol->halfspaces[halfspaceno].point[1],
		modclpvol->halfspaces[halfspaceno].point[2],
		modclpvol->halfspaces[halfspaceno].vector[0],
		modclpvol->halfspaces[halfspaceno].vector[1],
		modclpvol->halfspaces[halfspaceno].vector[2]);
	dor_print_output(dor_print_get_line());

	sprintf(dor_print_get_line(),
		"  halfspace %d plane equation = (%lf,%lf,%lf,%lf)",
		halfspaceno,
		modclpvol->plneqns[halfspaceno][0],
		modclpvol->plneqns[halfspaceno][1],
		modclpvol->plneqns[halfspaceno][2],
		modclpvol->plneqns[halfspaceno][3]);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

dot_modclpvol *dor_modclpvol_copy (dot_modclpvol *oldmodclpvol)
{
    dot_modclpvol *newmodclpvol;
    DtInt halfspacecount;
    DtInt halfspaceno;

    newmodclpvol = (dot_modclpvol *)dor_space_allocate(sizeof *newmodclpvol);

    halfspacecount = oldmodclpvol->halfspacecount;

    if (halfspacecount > 0){
	newmodclpvol->halfspaces = (DtHalfSpace *)dor_space_allocate
					(halfspacecount*(sizeof (DtHalfSpace)));
	newmodclpvol->plneqns = (DtPoint4 *)dor_space_allocate
					(halfspacecount*(sizeof (DtPoint4)));

	for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++){
	    newmodclpvol->halfspaces[halfspaceno].point[0] =
		    oldmodclpvol->halfspaces[halfspaceno].point[0];
	    newmodclpvol->halfspaces[halfspaceno].point[1] =
		    oldmodclpvol->halfspaces[halfspaceno].point[1];
	    newmodclpvol->halfspaces[halfspaceno].point[2] =
		    oldmodclpvol->halfspaces[halfspaceno].point[2];

	    newmodclpvol->halfspaces[halfspaceno].vector[0] =
		    oldmodclpvol->halfspaces[halfspaceno].vector[0];
	    newmodclpvol->halfspaces[halfspaceno].vector[1] =
		    oldmodclpvol->halfspaces[halfspaceno].vector[1];
	    newmodclpvol->halfspaces[halfspaceno].vector[2] =
		    oldmodclpvol->halfspaces[halfspaceno].vector[2];

	    newmodclpvol->plneqns[halfspaceno][0] =
		    oldmodclpvol->plneqns[halfspaceno][0];
	    newmodclpvol->plneqns[halfspaceno][1] =
		    oldmodclpvol->plneqns[halfspaceno][1];
	    newmodclpvol->plneqns[halfspaceno][2] =
		    oldmodclpvol->plneqns[halfspaceno][2];
	    newmodclpvol->plneqns[halfspaceno][3] =
		    oldmodclpvol->plneqns[halfspaceno][3];
	}
    }
    else{
	newmodclpvol->halfspaces = NULL;
	newmodclpvol->plneqns = NULL;
    }

    newmodclpvol->halfspacecount = halfspacecount;

    return(newmodclpvol);
}
/*
 ======================================================================
 */

void dor_modclpvol_apply_volume (
    dot_modclpvol *modclpvol,
    DtMatrix4x4    lcstowcs,
    DtClipOperator operator,
    DtInt          halfspacecount,
    DtHalfSpace   *halfspaces)
{
    DtMatrix4x4 tspwcstolcsmat;
    DtInt oldhalfspacecount;
    DtHalfSpace *newhalfspaces;
    DtPoint4 *newplneqns;
    DtInt halfspaceno;

    dor_matrix_load (tspwcstolcsmat,lcstowcs);
    dor_matrix_invert(tspwcstolcsmat);
    dor_matrix_transpose(tspwcstolcsmat);

    oldhalfspacecount = modclpvol->halfspacecount;

    if (halfspacecount > 0){
	/*  Allocate space for the new halfspaces list, including space for
	    the old list of halfspaces.  */

	newhalfspaces = (DtHalfSpace *)dor_space_allocate
		((oldhalfspacecount+halfspacecount)*(sizeof (DtHalfSpace)));
	newplneqns = (DtPoint4 *)dor_space_allocate
		((oldhalfspacecount+halfspacecount)*(sizeof (DtPoint4)));

	/*  Copy the old halfspaces and plane equations into the new list.  */

	for (halfspaceno=0;halfspaceno<oldhalfspacecount;halfspaceno++){
	    newhalfspaces[halfspaceno].point[0] =
		    modclpvol->halfspaces[halfspaceno].point[0];
	    newhalfspaces[halfspaceno].point[1] =
		    modclpvol->halfspaces[halfspaceno].point[1];
	    newhalfspaces[halfspaceno].point[2] =
		    modclpvol->halfspaces[halfspaceno].point[2];

	    newhalfspaces[halfspaceno].vector[0] =
		    modclpvol->halfspaces[halfspaceno].vector[0];
	    newhalfspaces[halfspaceno].vector[1] =
		    modclpvol->halfspaces[halfspaceno].vector[1];
	    newhalfspaces[halfspaceno].vector[2] =
		    modclpvol->halfspaces[halfspaceno].vector[2];

	    newplneqns[halfspaceno][0] = modclpvol->plneqns[halfspaceno][0];
	    newplneqns[halfspaceno][1] = modclpvol->plneqns[halfspaceno][1];
	    newplneqns[halfspaceno][2] = modclpvol->plneqns[halfspaceno][2];
	    newplneqns[halfspaceno][3] = modclpvol->plneqns[halfspaceno][3];
	}

	/*  Copy the new halfspaces into the new list and compute their plane
	    equations.  */

	for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++){
	    newhalfspaces[oldhalfspacecount+halfspaceno].point[0] =
		    halfspaces[halfspaceno].point[0];
	    newhalfspaces[oldhalfspacecount+halfspaceno].point[1] =
		    halfspaces[halfspaceno].point[1];
	    newhalfspaces[oldhalfspacecount+halfspaceno].point[2] =
		    halfspaces[halfspaceno].point[2];

	    newhalfspaces[oldhalfspacecount+halfspaceno].vector[0] =
		    halfspaces[halfspaceno].vector[0];
	    newhalfspaces[oldhalfspacecount+halfspaceno].vector[1] =
		    halfspaces[halfspaceno].vector[1];
	    newhalfspaces[oldhalfspacecount+halfspaceno].vector[2] =
		    halfspaces[halfspaceno].vector[2];
	}

	/*  Delete the old halfspaces and plane equations lists.  */

	if (oldhalfspacecount > 0){
	    dor_space_deallocate(modclpvol->halfspaces);
	    dor_space_deallocate(modclpvol->plneqns);
	}

	/*  Make the new halfspace list the list for the modclpvol.  */

	modclpvol->halfspacecount += halfspacecount;

	modclpvol->halfspaces = newhalfspaces;
	modclpvol->plneqns = newplneqns;

	for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++){
	    dor_modclpvol_plane_equation(modclpvol, oldhalfspacecount+halfspaceno,
					 tspwcstolcsmat);
	}
    }
}
/*
 ======================================================================
 */

void dor_modclpvol_get_value (
    dot_modclpvol *modclpvol,
    DtInt         *halfspacecount,
    DtPoint3      *point,
    DtVector3     *vector,
    DtPoint4      *plneqn)
{
    DtInt halfspaceno;

    *halfspacecount = modclpvol->halfspacecount;

    for (halfspaceno=0;halfspaceno<modclpvol->halfspacecount;halfspaceno++){
	point[halfspaceno][0] = modclpvol->halfspaces[halfspaceno].point[0];
	point[halfspaceno][1] = modclpvol->halfspaces[halfspaceno].point[1];
	point[halfspaceno][2] = modclpvol->halfspaces[halfspaceno].point[2];

	vector[halfspaceno][0] = modclpvol->halfspaces[halfspaceno].vector[0];
	vector[halfspaceno][1] = modclpvol->halfspaces[halfspaceno].vector[1];
	vector[halfspaceno][2] = modclpvol->halfspaces[halfspaceno].vector[2];

	plneqn[halfspaceno][0] = modclpvol->plneqns[halfspaceno][0];
	plneqn[halfspaceno][1] = modclpvol->plneqns[halfspaceno][1];
	plneqn[halfspaceno][2] = modclpvol->plneqns[halfspaceno][2];
	plneqn[halfspaceno][3] = modclpvol->plneqns[halfspaceno][3];
    }
}
/*
 ======================================================================
 */

void dor_modclpvol_set_value (
    dot_modclpvol *modclpvol,
    DtMatrix4x4    lcstowcs,
    DtClipOperator operator,
    DtInt          halfspacecount,
    DtHalfSpace   *halfspaces)
{
    DtMatrix4x4 tspwcstolcsmat;
    DtInt halfspaceno;

    dor_matrix_load (tspwcstolcsmat,lcstowcs);
    dor_matrix_invert(tspwcstolcsmat);
    dor_matrix_transpose(tspwcstolcsmat);

    /*  Delete the old halfspaces list.  */

    if (modclpvol->halfspacecount > 0){
	dor_space_deallocate(modclpvol->halfspaces);
	dor_space_deallocate(modclpvol->plneqns);
    }

    if (halfspacecount > 0){
	/*  Allocate space for the new halfspaces list, including space for
	    the old list of halfspaces.  */

	modclpvol->halfspaces = (DtHalfSpace *)dor_space_allocate
			(halfspacecount*(sizeof (DtHalfSpace)));
	modclpvol->plneqns = (DtPoint4 *)dor_space_allocate
			(halfspacecount*(sizeof (DtPoint4)));

	/*  Copy the new halfspaces into the new list.  */

	for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++){
	    modclpvol->halfspaces[halfspaceno].point[0] =
		    halfspaces[halfspaceno].point[0];
	    modclpvol->halfspaces[halfspaceno].point[1] =
		    halfspaces[halfspaceno].point[1];
	    modclpvol->halfspaces[halfspaceno].point[2] =
		    halfspaces[halfspaceno].point[2];

	    modclpvol->halfspaces[halfspaceno].vector[0] =
		    halfspaces[halfspaceno].vector[0];
	    modclpvol->halfspaces[halfspaceno].vector[1] =
		    halfspaces[halfspaceno].vector[1];
	    modclpvol->halfspaces[halfspaceno].vector[2] =
		    halfspaces[halfspaceno].vector[2];
	}

	/*  Make the new halfspace list the list for the modclpvol.  */

	modclpvol->halfspacecount += halfspacecount;

	for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++){
	    dor_modclpvol_plane_equation(modclpvol,halfspaceno,tspwcstolcsmat);
	}
    }
    else{
	modclpvol->halfspaces = NULL;
	modclpvol->plneqns = NULL;
    }
}
/*
 ======================================================================
 */

void dor_modclpvol_plane_equation (
    dot_modclpvol *modclpvol,
    DtInt          halfspaceno,
    DtMatrix4x4    matrix)
{
    DtReal a, b, c;
    DtReal x, y, z;
    DtReal origplneqn[4];

    a = modclpvol->halfspaces[halfspaceno].vector[0];
    b = modclpvol->halfspaces[halfspaceno].vector[1];
    c = modclpvol->halfspaces[halfspaceno].vector[2];

    x = modclpvol->halfspaces[halfspaceno].point[0];
    y = modclpvol->halfspaces[halfspaceno].point[1];
    z = modclpvol->halfspaces[halfspaceno].point[2];

    origplneqn[0] = a;
    origplneqn[1] = b;
    origplneqn[2] = c;
    origplneqn[3] = -(a*x+b*y+c*z);

    dor_math_homo_trns_fast(origplneqn,matrix,modclpvol->plneqns[halfspaceno]);
}
