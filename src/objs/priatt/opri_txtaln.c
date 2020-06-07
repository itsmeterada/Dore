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
	int dor_txtaln_initialize ()
	struct object * dor_txtaln_create (halign, valign)
	int dor_txtaln_destroy (object)
	int dor_txtaln_print (object)
	int dor_txtaln_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_txtaln_class_id;

static DtMethodEntry txtaln_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_txtaln_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_txtaln_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_txtaln_execute },
    { DcMethodDestroy,          (DtMethodPtr) dor_txtaln_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_txtaln_execute },
};

void dor_txtaln_initialize (void)
{
    doe_txtaln_class_id = dor_class_install (DcTypeTextAlign, "DoTextAlign", 5,
					     txtaln_methods,DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_txtaln_create (
    DtTextAlignHorizontal halign,
    DtTextAlignVertical valign)
{
    static DtFlag initialized = DcFalse;
    struct txtaln *txtaln;
    struct object *txtalnobject;

    if (!initialized) {
	dor_txtaln_initialize();
	initialized = DcTrue;
    }

    txtaln = (struct txtaln *)
	    dor_space_allocate(sizeof *txtaln);

    txtaln->halign = halign;
    txtaln->valign = valign;

    txtalnobject = dor_object_create(doe_txtaln_class_id,txtaln);

    return(txtalnobject);
}
/*
 ======================================================================
 */

void dor_txtaln_destroy (
    dot_object *object)
{
    struct txtaln_data *txtaln;

    txtaln = (struct txtaln_data *)(object->data);

    dor_space_deallocate(txtaln);
}
/*
 ======================================================================
 */

void dor_txtaln_print (
    dot_object *object)
{
    dot_txtaln *txtaln;

    txtaln = (dot_txtaln *)object->data;

    switch ( txtaln->halign ) {
    case DcTextHAlignNormal:
	dor_print_output("horizontal = DcTextHAlignNormal") ;
	break ;
    case DcTextHAlignLeft:
	dor_print_output("horizontal = DcTextHAlignLeft") ;
	break ;
    case DcTextHAlignCenter:
	dor_print_output("horizontal = DcTextHAlignCenter") ;
	break ;
    case DcTextHAlignRight:
	dor_print_output("horizontal = DcTextHAlignRight") ;
	break ;
    }

    switch ( txtaln->valign ) {
    case DcTextVAlignNormal:
	dor_print_output("vertical = DcTextVAlignNormal\n") ;
	break ;
    case DcTextVAlignTop:
	dor_print_output("vertical = DcTextVAlignTop\n") ;
	break ;
    case DcTextVAlignCap:
	dor_print_output("vertical = DcTextVAlignCap\n") ;
	break ;
    case DcTextVAlignHalf:
	dor_print_output("vertical = DcTextVAlignHalf\n") ;
	break ;
    case DcTextVAlignBase:
	dor_print_output("vertical = DcTextVAlignBase\n") ;
	break ;
    case DcTextVAlignBottom:
	dor_print_output("vertical = DcTextVAlignBottom\n") ;
	break ;
    }
}
/*
 ======================================================================
 */

void dor_txtaln_execute (
    struct object *object)
{
    dot_txtaln *txtaln;

    if( ! dor_condex_query_execute(doe_txtaln_class_id))
	    return;

    txtaln = (dot_txtaln *)object->data;

    dor_global_txtaln_set_value(txtaln);
}
