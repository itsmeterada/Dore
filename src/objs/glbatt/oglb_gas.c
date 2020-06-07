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
	dor_gas_initialize 
	dor_gas_create 
	dor_gas_test_set_value 
	dor_gas_set_value 
	dor_gas_get_value 
	dor_gas_push_value 
	dor_gas_pop_value 
	dor_gas_pop_group 
	dor_gas_print 
	dor_gas_set_debug_switch 

  ======================================================================
 */
#include "global.h"

#define dod_gas_element_chunk	200

struct gas_data {
    DtFlag debugswitch;
};

struct gas_element {
    dot_object *group;
    DtPtr       value;
};

struct gas {
    DtPFV destroy_routine;
    DtInt current_element;
    struct gas_element *elements;
    DtInt last_element;
};

struct gas_data doe_gas_data;

/*
 ======================================================================
 */

void dor_gas_initialize (void)
{
    doe_gas_data.debugswitch = DcFalse;
}
/*
 ======================================================================
 */

struct gas *dor_gas_create (DtPFV destroyroutine)
{
    struct gas *gas;

    gas = (struct gas *)dor_space_allocate(sizeof *gas);

    gas->destroy_routine = destroyroutine;

    gas->elements = (struct gas_element *)dor_space_allocate(
		dod_gas_element_chunk*(sizeof (struct gas_element)));

    gas->elements[0].group = NULL;
    gas->elements[0].value = (DtPtr)-1;
    gas->current_element = 0;

    gas->last_element = dod_gas_element_chunk-1;

    return(gas);
}
/*
 ======================================================================
 */

DtFlag dor_gas_test_set_value (
    struct gas *gas)
{
    dot_object *currentgroup;

    currentgroup = dor_group_get_current_group();

    if (gas->elements[gas->current_element].group == currentgroup){
	return(DcFalse);
    } else{
	return(DcTrue);
    }
}
/*
 ======================================================================
 */

DtFlag dor_gas_set_value (
    struct gas *gas,
    DtPtr       value)
{
    dot_object *currentgroup;

    currentgroup = dor_group_get_current_group();

    if (gas->elements[gas->current_element].group == currentgroup){
	gas->elements[gas->current_element].value = value;

	return(DcFalse);
    } else{
	if (gas->current_element == gas->last_element){
	    gas->elements = (struct gas_element *)dor_space_reallocate(
								       gas->elements,((gas->last_element+1)+dod_gas_element_chunk)*
								       (sizeof (struct gas_element)));

	    gas->last_element += dod_gas_element_chunk;
	}

	gas->current_element++;
	gas->elements[gas->current_element].group = currentgroup;
	gas->elements[gas->current_element].value = value;

	return(DcTrue);
    }
}
/*
 ======================================================================
 */

DtFlag dor_gas_get_value (
    struct gas *gas,
    DtPtr      *value)
{
    if (gas->current_element <= 0){
	DDerror (ERR_NO_CURRENT_VALUE, "dor_gas_get_value", DcNullPtr);
	return(DcFalse);
    }

    *value = gas->elements[gas->current_element].value;

    return(DcTrue);
}
/*
 ======================================================================
 */

DtFlag dor_gas_push_value (
    struct gas *gas,
    DtPtr       value)
{
    dot_object *currentgroup;

    currentgroup = dor_group_get_current_group();

    if (gas->current_element == gas->last_element){
	gas->elements = dor_space_reallocate
			(   gas->elements,
			    ((gas->last_element+1)+dod_gas_element_chunk)
				* (sizeof (struct gas_element)));

	gas->last_element += dod_gas_element_chunk;
    }
		
    gas->current_element++;
    gas->elements[gas->current_element].group = currentgroup;
    gas->elements[gas->current_element].value = value;

    if (gas->elements[(gas->current_element)-1].group == currentgroup){
	return(DcFalse);
    } else{
	return(DcTrue);
    }
}
/*
 ======================================================================
 */

DtFlag dor_gas_pop_value (
    struct gas *gas,
    DtPtr      *value)
{
    dot_object *currentgroup;

    currentgroup = dor_group_get_current_group();

    if (gas->elements[gas->current_element].group == currentgroup){
	*value = gas->elements[gas->current_element].value;

	gas->current_element--;

	return(DcTrue);
    } else{
	return(DcFalse);
    }
}
/*
 ======================================================================
 */

DtInt dor_gas_pop_group (
    struct gas *gas)
{
    dot_object *currentgroup;
    DtInt count = 0;

    currentgroup = dor_group_get_current_group();

    if (currentgroup == NULL){
	return count;
    }

    while (gas->elements[gas->current_element].group == currentgroup){
	(*(gas->destroy_routine))(gas->elements[gas->current_element].value);

	gas->current_element--;
	count++;

	if (gas->current_element < 0){
	    DDerror (ERR_CANT_POP_STACK, "dor_gas_pop_group", DcNullPtr);
	    return count;
	}
    }
    return count;
}
/*
 ======================================================================
 */

void dor_gas_print (
    struct gas *gas)
{
    DtInt elementno;

    printf ("gas at [%lx]\n", gas);

    for (elementno=0;elementno<=gas->current_element;elementno++){
	printf("	element %d:	[%lx], [%lx]\n",elementno,
	    gas->elements[elementno].group, gas->elements[elementno].value);
    }

    printf("	current element = %d\n",gas->current_element);
    printf("\n");
}
/*
 ======================================================================
 */

void dor_gas_set_debug_switch (
    DtFlag value)
{
    doe_gas_data.debugswitch = value;
}
