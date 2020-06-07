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
	int dor_exeset_initialize ()
	dot_object * dor_exeset_create (listsize, list, setop)
	int dor_exeset_destroy (object)
	int dor_exeset_print (object)
	int dor_exeset_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/exeset.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/bitfield.h>

struct exeset_data {
    dot_exeset *exeset;
    DtSetOperation setop;
};

dot_exeset *doe_exeset_temp_exeset;
DtInt doe_exeset_class_id;
/*
 ======================================================================
 */

static DtMethodEntry exeset_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_exeset_print   },
    { DcMethodDestroy,          (DtMethodPtr) dor_exeset_destroy },
    { DcMethodIniPick,          (DtMethodPtr) dor_exeset_execute },
    { DcMethodPick,             (DtMethodPtr) dor_exeset_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_exeset_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_exeset_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_exeset_execute },
};

void dor_exeset_initialize (void)
{
    doe_exeset_class_id = dor_class_install (DcTypeExecSet, "DoExecSet", 7,
					     exeset_methods, DcNullPtr);
	
    /* use as scratch space in exe.c */

    doe_exeset_temp_exeset = dor_exeset_create_exeset(dor_object_inq_ntypes());
}
/*
 ======================================================================
 */

dot_object *dor_exeset_create (
    DtInt listsize,
    DtInt list[],
    DtSetOperation setop)
{
    static DtFlag initialized = DcFalse;
    dot_object *exesetobject;
    struct exeset_data *exesetdata;
    int ntypes;

    if (!initialized) {
	dor_exeset_initialize();
	initialized = DcTrue;
    }

    exesetdata = (struct exeset_data *)
	    dor_space_allocate(sizeof *exesetdata);
	
    ntypes = dor_object_inq_ntypes();

    exesetdata->exeset = dor_exeset_create_exeset(ntypes);

    dor_bitfield_load(exesetdata->exeset->bitset, listsize,
		      list, exesetdata->exeset->ints, ntypes);

    exesetdata->setop = setop;

    exesetobject = dor_object_create(doe_exeset_class_id, exesetdata);

    return(exesetobject);
}
/*
 ======================================================================
 */

void dor_exeset_destroy (
    dot_object *object)
{
    struct exeset_data *exesetdata;

    exesetdata = (struct exeset_data *)object->data;
    dor_exeset_destroy_exeset(exesetdata->exeset);
    dor_space_deallocate(exesetdata);
}
/*
 ======================================================================
 */

void dor_exeset_print (
    dot_object *object)
{
    struct exeset_data *exesetdata;
    int N, i;
    Dt32Bits *set;

    exesetdata = (struct exeset_data *)object->data;

    if (exesetdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    N = exesetdata->exeset->objs;
    set = exesetdata->exeset->bitset;

    sprintf(dor_print_get_line(),"N = %d", N);
    dor_print_output(dor_print_get_line());

    dor_print_output("list = ");
    for(i=0; i<N; i++)
	    if(dor_bitfield_test_membership(set, i)) {
		sprintf(dor_print_get_line(),"      %d ", i);
		dor_print_output(dor_print_get_line());
	    }

    switch(exesetdata->setop) {
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

void dor_exeset_execute (
    dot_object *object)
{
    struct exeset_data *data;
    Dt32Bits last_word, *wordptr, bit;
    int i, overlap;

    if( ! dor_condex_query_execute(doe_exeset_class_id))
	    return;

    data = (struct exeset_data *)object->data;

    dor_global_exeset_get_value(doe_exeset_temp_exeset);

    /*
      NOTE: It is assumed that the unused bits of the last word of 
      "data->exeset->bitset" are all zeros. This is important when
      the current exeset is larger than a given one because the 
      following bitfield calls operate on whole words.
      For "add", "delete", and "invert", the bitfield calls will then
      do the correct things. For "replace", the bits of the old 
      exeset that are overwritten by those extra zeros durring the copy
      call are restored.
      */

    switch(data->setop){
    case DcSetAdd:
	dor_bitfield_add(doe_exeset_temp_exeset->bitset, 
			 data->exeset->bitset, data->exeset->ints);
	break;

    case DcSetDelete:
	dor_bitfield_delete(doe_exeset_temp_exeset->bitset,
			    data->exeset->bitset, data->exeset->ints);
	break;

    case DcSetInvert:
	dor_bitfield_invert(doe_exeset_temp_exeset->bitset,
			    data->exeset->bitset, data->exeset->ints);
	break;

    case DcSetReplace:
	last_word = doe_exeset_temp_exeset->
		bitset[data->exeset->ints -1];
	overlap = data->exeset->objs % 32;
	dor_bitfield_copy(doe_exeset_temp_exeset->bitset,
			  data->exeset->bitset, data->exeset->ints);
	if(overlap){
	    wordptr = &doe_exeset_temp_exeset->
		    bitset[data->exeset->ints -1];
	    for(i=overlap; i<32; i++){
		bit = (1 << i);
		if(bit & last_word){
		    *wordptr |= bit;
		}
	    }
	}
	break;

    default:
	DDerror (ERR_INVALID_SET_OP,
		    "dor_exeset_execute", DcNullPtr);
	return;
    }

    dor_global_exeset_set_value(doe_exeset_temp_exeset);
}
