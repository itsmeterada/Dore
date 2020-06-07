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
	int dor_fil_initialize ()
	dot_object * dor_fil_create (filter, n, list, setop)
	int dor_fil_print (object)
	int dor_fil_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/filters.h>
#include <dore/internal/glbatt.h>

struct fil_data {
    DtInt fil;
    dot_bitfield bitfield;
    DtSetOperation setop;
};
/*
 ======================================================================
 */

DtInt doe_fil_class_id;

static DtMethodEntry fil_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_fil_print   },
    { DcMethodIniPick,          (DtMethodPtr) dor_fil_execute },
    { DcMethodPick,             (DtMethodPtr) dor_fil_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_fil_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_fil_execute },
};

void dor_fil_initialize (void)
{
    doe_fil_class_id = dor_class_install (DcTypeFilter, "DoFilter", 5,
					  fil_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_fil_create (
    DtFilter filter,
    DtInt n,
    DtInt list[],
    DtSetOperation setop)
{
    static DtFlag initialized = DcFalse;
    dot_object *filobject;
    struct fil_data *fildata;

    if (!initialized) {
	dor_fil_initialize();
	initialized = DcTrue;
    }

    fildata = (struct fil_data *)
	    dor_space_allocate(sizeof *fildata);

    fildata->fil = filter;

    dor_bitfield_load(fildata->bitfield, n, list,
		      dod_bitfield_ints, dod_bitfield_bits);

    fildata->setop = setop;

    filobject = dor_object_create(doe_fil_class_id, fildata);

    return(filobject);
}
/*
 ======================================================================
 */

void dor_fil_print (
    dot_object *object)
{
    struct fil_data *fildata;
    DtInt List[dod_bitfield_bits];
    int i, N;

    fildata = (struct fil_data *)object->data;

    if (fildata == NULL) {
	dor_print_output("bad values");
	return;
    }

    dor_bitfield_unload(fildata->bitfield, &N, List);

    switch(fildata->fil) {
    case DcInvisibilityInclusion:
	dor_print_output(
			 "filter = DcInvisibilityInclusion");
	break;
    case DcInvisibilityExclusion:
	dor_print_output(
			 "filter = DcInvisibilityExclusion");
	break;
    case DcPickabilityInclusion:
	dor_print_output(
			 "filter = DcPickabilityInclusion");
	break;
    case DcPickabilityExclusion:
	dor_print_output(
			 "filter = DcPickabilityExclusion");
	break;
    }

    sprintf(dor_print_get_line(),"N = %d", N);
    dor_print_output(dor_print_get_line());

    dor_print_output("list = ");
    for(i=0; i<N; i++) {
	sprintf(dor_print_get_line(),"      %d ", List[i]);
	dor_print_output(dor_print_get_line());
    }

    switch(fildata->setop) {
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

void dor_fil_execute (
    dot_object *object)
{
    struct fil_data *data;
    dot_bitfield tempbitfield;
    dot_bitfield *curnamset;
    dot_filters *curfils;
    DtInt fil;
    /****	DtFlag invis, pick, high;   ****/
    DtFlag invis, pick;

    if( ! dor_condex_query_execute(doe_fil_class_id))
	    return;

    data = (struct fil_data *)object->data;

    curfils = (dot_filters *)dor_global_fil_get_data_pointer();

    fil = data->fil;

    /* get the current filter in question */
    dor_bitfield_copy(tempbitfield, (*curfils)[fil], dod_bitfield_ints);

    /* modify it to get the new filter */
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
	DDerror (ERR_INVALID_SET_OP, "dor_fil_execute", DcNullPtr);
	return;
    }

    curnamset = (dot_bitfield *)dor_global_namset_get_data_pointer();

    /* update the global flag affected by this type of filter */
    switch(fil) {
    case DcInvisibilityInclusion:
	invis = dor_filters_test_intersections(*curnamset, tempbitfield, 
					       (*curfils)[(int)DcInvisibilityExclusion]);
	dor_global_invswi_set_value((DtSwitch)invis);
	break;

    case DcInvisibilityExclusion:
	invis = dor_filters_test_intersections(*curnamset, 
					       (*curfils)[(int)DcInvisibilityInclusion],
					       tempbitfield);
	dor_global_invswi_set_value((DtSwitch)invis);
	break;

    case DcPickabilityInclusion:
	pick = dor_filters_test_intersections(*curnamset, tempbitfield,
					      (*curfils)[(int)DcPickabilityExclusion]);
	dor_global_pckswi_set_value((DtSwitch)pick);
	break;

    case DcPickabilityExclusion:
	pick = dor_filters_test_intersections(*curnamset, 
					      (*curfils)[(int)DcPickabilityInclusion],
					      tempbitfield);
	dor_global_pckswi_set_value((DtSwitch)pick);
	break;

    default:
	break;
    }

    /* make the new filter the current one */
    dor_global_fil_set_value(fil, tempbitfield);
}
