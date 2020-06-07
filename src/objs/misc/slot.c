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
	int dor_slot_initialize ()
	dot_object * dor_slot_create ()
	int dor_slot_destroy (object)
	int dor_slot_print (object)
	int dor_slot_activate (slot_object,value)
	int dor_slot_enqueue_input (slot_object, value)
	int dor_slot_flush_input ()
	int dor_slot_input_value (slot_object, value)
	dot_object * dor_slot_inq_valuator_group (slot_object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>

#define dod_slot_queue_max_length 1000

struct slot_data {
    dot_object *valuator_group;
};

typedef struct SLOT_ELEMENT {
    dot_object *slot;
    DtReal value;
    struct SLOT_ELEMENT *next;
} dot_slot_queue_element;

DtObject
	DcTransXSlot,
	DcTransYSlot,
	DcTransZSlot,
	DcScaleXSlot,
	DcScaleYSlot,
	DcScaleZSlot,
	DcRotXSlot,
	DcRotYSlot,
	DcRotZSlot,
	DcUndoSlot,
	DcUpdateSlot;

dot_slot_queue_element *doe_slot_queue_head;
dot_slot_queue_element *doe_slot_queue_tail;
DtInt doe_slot_queue_length;
DtInt doe_slot_class_id;

/* ====================================================================== */

static DtMethodEntry slot_methods[] = {
    { DcMethodPrint,   (DtMethodPtr) dor_slot_print },
    { DcMethodDestroy, (DtMethodPtr) dor_slot_destroy },
};



/* ====================================================================== */

void dor_slot_initialize (void)
{
    doe_slot_class_id = dor_class_install (DcTypeInputSlot, "DoInputSlot", 2, 
					   slot_methods, DcNullPtr);

    /* create the standard input slots */
    DcTransXSlot = (DtObject) dor_slot_create ();
    DcTransYSlot = (DtObject) dor_slot_create ();
    DcTransZSlot = (DtObject) dor_slot_create ();
    DcScaleXSlot = (DtObject) dor_slot_create ();
    DcScaleYSlot = (DtObject) dor_slot_create ();
    DcScaleZSlot = (DtObject) dor_slot_create ();
    DcRotXSlot   = (DtObject) dor_slot_create ();
    DcRotYSlot   = (DtObject) dor_slot_create ();
    DcRotZSlot   = (DtObject) dor_slot_create ();
    DcUndoSlot   = (DtObject) dor_slot_create ();
    DcUpdateSlot = (DtObject) dor_slot_create ();

    /* Create dummy references to these objects in the event that
     * they are dereferenced such that their reference count goes to
     * zero: this keeps them from being deleted entirely.
     */
    dor_object_add_reference (DcTransXSlot, DcNullObject);
    dor_object_add_reference (DcTransYSlot, DcNullObject);
    dor_object_add_reference (DcTransZSlot, DcNullObject);
    dor_object_add_reference (DcScaleXSlot, DcNullObject);
    dor_object_add_reference (DcScaleYSlot, DcNullObject);
    dor_object_add_reference (DcScaleZSlot, DcNullObject);
    dor_object_add_reference (DcRotXSlot,   DcNullObject);
    dor_object_add_reference (DcRotYSlot,   DcNullObject);
    dor_object_add_reference (DcRotZSlot,   DcNullObject);
    dor_object_add_reference (DcUndoSlot,   DcNullObject);
    dor_object_add_reference (DcUpdateSlot, DcNullObject);

    /* initialize global slot data */
    doe_slot_queue_head = (dot_slot_queue_element *)0;
    doe_slot_queue_tail = (dot_slot_queue_element *)0;
    doe_slot_queue_length = 0;
}



/* ====================================================================== */

dot_object *dor_slot_create (void)
{
    struct slot_data *slotdata;

    slotdata = dor_space_allocate (sizeof(*slotdata));

    slotdata->valuator_group = dor_group_create (DcFalse, DcFalse);

    return dor_object_create(doe_slot_class_id,slotdata);
}



/* ====================================================================== */

void dor_slot_destroy (dot_object *object)
{
    struct slot_data *slotdata = (struct slot_data *)object->data;

    dor_group_empty(slotdata->valuator_group);
    dor_space_deallocate(slotdata);
}



/* ====================================================================== */

void dor_slot_print (dot_object *object)
{
    struct slot_data *slotdata = (struct slot_data *)object->data;
	
    dor_print_output("valuator group");
    dor_print_indent_up();
    dor_object_print(slotdata->valuator_group);
    dor_print_indent_down();
}



/* ====================================================================== */

void dor_slot_activate (
    dot_object *slot_object,
    DtReal      value)
{
    struct slot_data *slotdata = (struct slot_data *)slot_object->data;
    dot_object *valuator_group = slotdata->valuator_group;
    DtInt group_size, i;
    dot_object *element;
    DtPFI function;
    DtObject dataobject;
    DtPtr datptr;
    Dt32Bits datval;
    DtFlag pass_by_ref;
    DtInt type;
    DtReal local_value = value;
    dot_object *local_slot = slot_object;
    DtInt calbak_class_id = dor_class_inq_id("DoCallback");
    DtInt datval_class_id = dor_class_inq_id("DoDataVal");
    DtInt datptr_class_id = dor_class_inq_id("DoDataPtr");

    group_size = dor_group_inq_size(valuator_group);

    for (i=0;i<group_size;i++) {
	element = dor_group_inquire_object_at_position(valuator_group,i,
						       DcBeginning);

	if (calbak_class_id == dor_object_inq_type(element)) {
	    function = dor_calbak_get_function(element);
	    dataobject = dor_calbak_get_dataobject(element);
	    pass_by_ref = dor_calbak_get_pass_by_ref(element);

	    dor_system_set_edit_source(dod_system_edit_source_valuator);

	    if (dataobject == DcNullObject) {
		datval = 0;
		if (pass_by_ref) {
		    (*function)(&datval,&local_slot,&local_value);
		} else {
		    (*function)(datval,local_slot,local_value);
		}
	    } else if ((DcObjectValid == 
			dor_object_validate(dataobject)) &&
		       (((type = dor_object_inq_type(dataobject))
			 == datptr_class_id) ||
			(type == datval_class_id))) {

		if (type == datptr_class_id) {
		    datptr = dor_datptr_get_value(dataobject);
		    if (pass_by_ref) {
			(*function) (&datptr, &local_slot, &local_value);
		    } else {
			(*function) (datptr, local_slot, local_value);
		    }
		} else if (type == datval_class_id) {
		    datval = dor_datval_get_value(dataobject);
		    if (pass_by_ref) {
			(*function) (&datval, &local_slot, &local_value);
		    } else {
			(*function) (datval, local_slot, local_value);
		    }
		}
	    } else {
		DDerror(ERR_INVALID_DATAOBJECT, 
			     "dor_slot_activate",
			     DcNullPtr);
	    }
	    dor_system_set_edit_source(dod_system_edit_source_others);
	}
    }
}



/* ====================================================================== */

void dor_slot_enqueue_input (
    dot_object *slot_object,
    DtReal      value)
{
    dot_slot_queue_element *newelement;

    newelement = dor_space_allocate(sizeof(*newelement));

    newelement->slot  = slot_object;
    newelement->value = value;
    newelement->next  = (dot_slot_queue_element *)0;

    if(doe_slot_queue_length >= dod_slot_queue_max_length){
	if(doe_slot_queue_length == dod_slot_queue_max_length){
	    DDerror (ERR_SLOT_QUEUE_OVERFLOW,
			"dor_slot_enqueue_input", DcNullPtr);
	}
	return;
    }

    if(doe_slot_queue_length++ == 0){
	doe_slot_queue_tail = doe_slot_queue_head = newelement;
    }
    else{
	doe_slot_queue_tail->next = newelement;
	doe_slot_queue_tail = newelement;
    }
}



/* ====================================================================== */

void dor_slot_flush_input (void)
{
    dot_slot_queue_element *p, *pre;

    for(p = doe_slot_queue_head; p != (dot_slot_queue_element *)0; ){
	dor_slot_activate(p->slot, p->value);
	pre = p;
	p = p->next;
	dor_space_deallocate(pre);
    }

    doe_slot_queue_head = (dot_slot_queue_element *)0;
    doe_slot_queue_tail = (dot_slot_queue_element *)0;
    doe_slot_queue_length = 0;
}



/* ====================================================================== */

void dor_slot_input_value (
    dot_object *slot_object,
    DtReal      value)
{
#ifdef ERRCHK
    if( ! dor_object_confirm_type(slot_object, doe_slot_class_id)){
	DDerror(ERR_INVALID_SLOT,
		     "DsInputValue", DcNullPtr);
	return;
    }
#endif

    if(doe_system_valuator_switch) /* valuators enabled */{
	dor_slot_activate(slot_object, value);
    }
    else			/* valuators are disabled */{
	dor_slot_enqueue_input(slot_object, value);
    }
}



/* ====================================================================== */

dot_object * dor_slot_inq_valuator_group (dot_object *slot_object)
{
    struct slot_data *slotdata;

#ifdef ERRCHK
    if( ! dor_object_confirm_type(slot_object, doe_slot_class_id)){
	DDerror(ERR_INVALID_SLOT,
		     "DsInqValuatorGroup", DcNullPtr);
	return(DcNullObject);
    }
#endif

    slotdata = (struct slot_data *)slot_object->data;

    return(slotdata->valuator_group);
}
