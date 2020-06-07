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
	int dor_namset_initialize ()
	dot_object * dor_namset_create (n, list, setop)
	int dor_namset_print (object)
	int dor_namset_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/filters.h>
#include <dore/internal/glbatt.h>

struct namset_data {
   dot_bitfield bitfield;
   DtSetOperation setop;
};
/*
 ======================================================================
 */

DtInt doe_namset_class_id;

static DtMethodEntry namset_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_namset_print   },
    { DcMethodIniPick,          (DtMethodPtr) dor_namset_execute },
    { DcMethodPick,             (DtMethodPtr) dor_namset_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_namset_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_namset_execute },
};

void dor_namset_initialize (void)
{
    doe_namset_class_id = dor_class_install (DcTypeNameSet, "DoNameSet", 5,
					     namset_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_namset_create (
    DtInt n,
    DtInt list[],
    DtSetOperation setop)
{
    static DtFlag initialized = DcFalse;
    dot_object *namsetobject;
    struct namset_data *namsetdata;

    if (!initialized) {
	dor_namset_initialize();
	initialized = DcTrue;
    }

    namsetdata = (struct namset_data *)
	    dor_space_allocate(sizeof *namsetdata);

    dor_bitfield_load(namsetdata->bitfield, n, list,
		      dod_bitfield_ints, dod_bitfield_bits);

    namsetdata->setop = setop;

    namsetobject = dor_object_create(doe_namset_class_id, namsetdata);

    return(namsetobject);
}
/*
 ======================================================================
 */

void dor_namset_print (
    dot_object *object)
{
    struct namset_data *namsetdata;
    DtInt List[dod_bitfield_bits];
    int i, N;


    namsetdata = (struct namset_data *)object->data;

    if (namsetdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    dor_bitfield_unload(namsetdata->bitfield, &N, List);

    sprintf(dor_print_get_line(),"N = %d", N);
    dor_print_output(dor_print_get_line());

    dor_print_output("list = ");
    for(i=0; i<N; i++) {
	sprintf(dor_print_get_line(),"      %d ", List[i]);
	dor_print_output(dor_print_get_line());
    }

    switch(namsetdata->setop) {
    case DcSetAdd:
	dor_print_output("setop = DcSetAdd");
	break;
    case DcSetDelete:
	dor_print_output("setop = DcSetDelete");
	break;
    case DcSetInvert:
	dor_print_output("setop = DcSetInvert");
	break;
    case DcSetReplace:
	dor_print_output("setop = DcSetReplace");
	break;
    }
}
/*
 ======================================================================
 */

void dor_namset_execute (
    dot_object *object)
{
    struct namset_data *data;
    dot_bitfield       tempbitfield;
    dot_filters        *curfils;
    /****	DtFlag             invis, pick, high;  *****/
    DtFlag             invis, pick;

    if( ! dor_condex_query_execute(doe_namset_class_id))
	    return;

    data = (struct namset_data *)object->data;

    dor_global_namset_get_value(tempbitfield);

    /* modify it to get the new namset */
    switch(data->setop) {
    case DcSetAdd:
	dor_bitfield_add(tempbitfield, data->bitfield, dod_bitfield_ints);
	break;

    case DcSetDelete:
	dor_bitfield_delete(tempbitfield, data->bitfield, dod_bitfield_ints);
	break;

    case DcSetInvert:
	dor_bitfield_invert(tempbitfield, data->bitfield, dod_bitfield_ints);
	break;

    case DcSetReplace:
	dor_bitfield_copy(tempbitfield, data->bitfield, dod_bitfield_ints);
	break;

    default:
	DDerror (ERR_INVALID_SET_OP, "dor_namset_execute", DcNullPtr);
	return;
    }



    curfils = (dot_filters *)dor_global_fil_get_data_pointer();


    /* update global invisibilility, pickability, and highlightling flags */

    invis = dor_filters_test_intersections(tempbitfield, 
					   (*curfils)[(DtInt)DcInvisibilityInclusion],
					   (*curfils)[(DtInt)DcInvisibilityExclusion]);
    dor_global_invswi_set_value((DtSwitch)invis);

    pick = dor_filters_test_intersections(tempbitfield,
					  (*curfils)[(DtInt)DcPickabilityInclusion],
					  (*curfils)[(DtInt)DcPickabilityExclusion]);
    dor_global_pckswi_set_value((DtSwitch)pick);

    /*
      high = dor_filters_test_intersections(tempbitfield,
      					 (*curfils)[(DtInt)DcHighlightingInclusion],
      					 (*curfils)[(DtInt)DcHighlightingExclusion]);
      dor_global_hltswi_set_value((DtSwitch)high);
      */


    /* make the new namset the current one */
    dor_global_namset_set_value(tempbitfield);
}
