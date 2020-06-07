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
	dor_condex_initialize
	dor_condex_query_execute
	dor_condex_query_render
	dor_condex_query_pick
	dor_condex_set_disable_prim_exec_flag
	dor_condex_query_disable_prim_exec_flag

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/exeset.h>
#include <dore/internal/bitfield.h>

DtFlag doe_condex_disable_primitive_exec_flag;



/* ====================================================================== */

void dor_condex_initialize (void)
{
    doe_condex_disable_primitive_exec_flag = DcFalse;
}

/* ====================================================================== */

    /* Returns DcTrue if the given object type is currently */
    /* executable. Returns DcFalse otherwise.               */

DtFlag dor_condex_query_execute (DtInt objecttype)
{
    dot_exeset *curset;
    DtFlag executability;

    curset = (dot_exeset *)dor_global_exeset_get_data_pointer();

    if (curset == (dot_exeset *)DcNullPtr){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_condex_query_execute", DcNullPtr);
	return(DcFalse);
    }

    executability =
    dor_bitfield_test_membership(curset->bitset, objecttype);

    return(executability);
}

/* ====================================================================== */

    /* Returns DcTrue if the given object type (assumed to be
    a drawing primitive type) is free to render itself.  This
    happens if that object type is currently executable and
    invisibility is currently DcOff. */

DtFlag dor_condex_query_render (DtInt objecttype)
{
    DtSwitch invisibility;

    if (doe_condex_disable_primitive_exec_flag){
	return(DcFalse);
    }

    dor_global_invswi_get_value(&invisibility);

    if (invisibility){
	return(DcFalse);
    }

    return(dor_condex_query_execute(objecttype));
}

/* ====================================================================== */

    /* Returns DcTrue if the given object type (assumed to be a    */
    /* drawable primitive) is elegible to be picked. This occures  */
    /* if it is renderable and the pickability switch is currently */
    /* DcOn. Returns DcFalse otherwise.                            */


DtFlag dor_condex_query_pick (DtInt objecttype)
{
    DtInt renderable;
    DtSwitch pickability;

    if(! dor_global_pckswi_get_value(&pickability)){
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_condex_query_pick", DcNullPtr);
	pickability = 0;
    }

    renderable = dor_condex_query_render(objecttype);

    return((DtFlag) (renderable && pickability));
}

/* ====================================================================== */

void dor_condex_set_disable_prim_exec_flag (DtFlag switchvalue)
{
    doe_condex_disable_primitive_exec_flag = switchvalue;
}

/* ====================================================================== */

DtFlag dor_condex_query_disable_prim_exec_flag (void)
{
    return(doe_condex_disable_primitive_exec_flag);
}
