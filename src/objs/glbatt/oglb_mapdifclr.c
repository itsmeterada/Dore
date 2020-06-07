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
	dor_global_mapdifclr_initialize 
	dor_global_mapdifclr_set_value 
	dor_global_mapdifclr_push_value 
	dor_global_mapdifclr_pop_value 
	dor_global_mapdifclr_pop_group 
	dor_global_mapdifclr_get_value 
	dor_global_mapdifclr_get_data_pointer 
	dor_global_mapdifclr_update_execute 
	dor_global_mapdifclr_destroy_element 

  ======================================================================
 */
#include "global.h"

struct global_mapdifclr_texture {
   DtObject mapping;
   DtObject raster;
   DtPtr attributes;
   DtInt attributes_size;
};   

struct global_mapdifclr_data {
   DtInt size;
   struct global_mapdifclr_texture *texture;
};

dot_object *doe_global_mapdifclr_object;
DtInt doe_global_mapdifclr_class_id;
DtInt doe_global_mapdifclr_init;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_mapdifclr_get_value }
};

void dor_global_mapdifclr_initialize (void)
{
    struct gas *gas;

    doe_global_mapdifclr_class_id = 
	    dor_class_add ("AttTextureMapDiffuseColor", 1, methods,DcNullPtr);

    gas = dor_gas_create(dor_global_mapdifclr_destroy_element);

    doe_global_mapdifclr_object = 
	    dor_object_create(doe_global_mapdifclr_class_id,gas);

    doe_global_mapdifclr_init = DcFalse;
    dor_global_mapdifclr_push_value(DcMapReplace, DcStdTableLookup, DcNullObject, 0, 0);
}
/*
 ======================================================================
 */

void dor_global_mapdifclr_set_value (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster,
    DtPtr attributes,
    DtInt attributes_size)
{
    struct global_mapdifclr_data *mapdifclrdata;
    DtInt size;
    struct global_mapdifclr_texture *texture;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	structure is created and pushed on the stack.   */

    if (dor_gas_test_set_value(doe_global_mapdifclr_object->data)) {
	dor_global_mapdifclr_push_value(operator, mapping, raster, attributes, attributes_size);
    } else {
	mapdifclrdata = dor_global_mapdifclr_get_data_pointer();

	if (mapdifclrdata == (struct global_mapdifclr_data *)DcNullPtr) {
	    return;
	}

	if (operator == DcMapAdd) {
	    size = mapdifclrdata->size + 1;
	} else			/* DcMapReplace */ {
	    size = 1;
	}
	if (size != mapdifclrdata->size) {
	    texture = (struct global_mapdifclr_texture *)dor_space_reallocate
		    (mapdifclrdata->texture, size*sizeof(struct global_mapdifclr_texture));
	    if (size > mapdifclrdata->size) {
		texture[size-1].attributes_size = 0;
		texture[size-1].attributes = 0;
	    }
	    mapdifclrdata->texture = texture;
	    mapdifclrdata->size = size;
	} else {
	    texture = mapdifclrdata->texture;
	}

	if (texture[size-1].attributes_size != 0) {
	    dor_space_deallocate(texture[size-1].attributes);
	}

	texture[size-1].mapping = mapping;
	texture[size-1].raster = raster;
	texture[size-1].attributes_size = attributes_size;


	if (attributes_size > 0) {
	    texture[size-1].attributes = (DtPtr) dor_space_allocate 
		    (attributes_size*sizeof (char));
	    bcopy (attributes, texture[size-1].attributes, attributes_size);
	} else {
	    texture[size-1].attributes = 0;
	}
      
	/*  The value of this attribute may have changed, go execute
	    whatever update method is appropriate at this time.  */

	dor_global_mapdifclr_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_mapdifclr_push_value (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster,
    DtPtr attributes,
    DtInt attributes_size)
{
    struct global_mapdifclr_data *mapdifclrdata;
    struct global_mapdifclr_data *newmapdifclrdata;
    struct object *currentgroup;
    DtInt i;

    newmapdifclrdata = (struct global_mapdifclr_data *) dor_space_allocate
	    (sizeof *newmapdifclrdata);

    if (doe_global_mapdifclr_init) {
	if (!dor_gas_get_value (doe_global_mapdifclr_object->data,
				(DtPtr)(&mapdifclrdata))) {
	    mapdifclrdata = (struct global_mapdifclr_data *)DcNullPtr;
	}
    } else {
	mapdifclrdata = (struct global_mapdifclr_data *)DcNullPtr;
	doe_global_mapdifclr_init = DcTrue;
    }

    newmapdifclrdata->size = 1;
    if ((mapdifclrdata != (struct global_mapdifclr_data *)DcNullPtr) &&
	(operator == DcMapAdd)) {
	newmapdifclrdata->size += mapdifclrdata->size;
    }
    newmapdifclrdata->texture = (struct global_mapdifclr_texture *)
	    dor_space_allocate (newmapdifclrdata->size *sizeof(struct global_mapdifclr_texture));

    for (i=0; i<newmapdifclrdata->size-1; i++) {
	if (mapdifclrdata->texture[i].attributes_size != 0) {
	    newmapdifclrdata->texture[i].attributes = (DtPtr) dor_space_allocate 
		    (mapdifclrdata->texture[i].attributes_size*sizeof (char));
	    bcopy (mapdifclrdata->texture[i].attributes, 
		   newmapdifclrdata->texture[i].attributes, 
		   mapdifclrdata->texture[i].attributes_size);
	} else {
	    newmapdifclrdata->texture[i].attributes = 0;
	}
	newmapdifclrdata->texture[i].mapping = mapdifclrdata->texture[i].mapping;
	newmapdifclrdata->texture[i].raster = mapdifclrdata->texture[i].raster;
	newmapdifclrdata->texture[i].attributes_size = mapdifclrdata->texture[i].attributes_size;
    }

    i = newmapdifclrdata->size - 1;
    if (attributes_size != 0) {
	newmapdifclrdata->texture[i].attributes = (DtPtr) dor_space_allocate 
		(attributes_size*sizeof (char));
	bcopy (attributes, newmapdifclrdata->texture[i].attributes, attributes_size);
    } else {
	newmapdifclrdata->texture[i].attributes = 0;
    }
    newmapdifclrdata->texture[i].mapping = mapping;
    newmapdifclrdata->texture[i].raster = raster;
    newmapdifclrdata->texture[i].attributes_size = attributes_size;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_mapdifclr_object->data,
			   newmapdifclrdata)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,
				   dor_global_mapdifclr_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapdifclr_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_mapdifclr_pop_value (void)
{
    DtFlag popflag;
    struct global_mapdifclr_data *mapdifclrdata;

    popflag = dor_gas_pop_value (doe_global_mapdifclr_object->data,
				 (DtPtr)(&mapdifclrdata));

    dor_global_mapdifclr_destroy_element(mapdifclrdata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapdifclr_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_mapdifclr_pop_group (void)
{
    dor_gas_pop_group(doe_global_mapdifclr_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapdifclr_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_mapdifclr_get_value (
    DtInt max_textures,
    DtInt *num_textures,
    DtObject mapping[],
    DtObject raster[],
    DtInt max_attr_size,
    unsigned char attributes[],
    DtInt attributes_size[])
{
   DtFlag getflag;
   DtInt copy_textures;
   DtInt copy_size;
   struct global_mapdifclr_data *mapdifclrdata;
   DtInt in, out;

#ifdef DEBUG
   printf ("max_textures = %d\nmax_attr_size = 0x%lx\n", max_textures, max_attr_size);
#endif

   getflag = dor_gas_get_value (doe_global_mapdifclr_object->data,
			        (DtPtr)(&mapdifclrdata));

   if (!getflag) {
      DDerror (ERR_NO_CURRENT_VALUE,
		  "dor_global_mapdifclr_get_value", DcNullPtr);
      return(DcFalse);
   }

   *num_textures = mapdifclrdata->size;

   copy_textures = max_textures < mapdifclrdata->size ? max_textures : 
	 mapdifclrdata->size;

   /* NOTE: The list of textures is stored in the order that they are executed but
      they should be applied in the reverse order, so the list returned is reversed so
      that the first texture in the list is the one that should be applied first.
      */

   for (out=0, in=mapdifclrdata->size - 1; out<copy_textures; out++, in--) {
      mapping[out] = mapdifclrdata->texture[in].mapping;
      raster[out]  = mapdifclrdata->texture[in].raster;
      attributes_size[out] = mapdifclrdata->texture[in].attributes_size;

      copy_size = max_attr_size <= mapdifclrdata->texture[in].attributes_size ? max_attr_size : 
	    mapdifclrdata->texture[in].attributes_size;

#ifdef DEBUG
      printf ("copy_size = 0x%lx\nattributes = 0x%lx\nattributes + %d*0x%lx = 0x%lx\n",
	      copy_size, attributes, out, max_attr_size, attributes+out*max_attr_size);
#endif

      bcopy (mapdifclrdata->texture[in].attributes, attributes+out*max_attr_size, copy_size);
   }

   return(DcTrue);
}
/*
 ======================================================================
 */

struct global_mapdifclr_data *dor_global_mapdifclr_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_mapdifclr_object->data,&dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_mapdifclr_get_data_pointer", DcNullPtr);
	return((struct global_mapdifclr_data *)DcNullPtr);
    }

    return((struct global_mapdifclr_data *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_mapdifclr_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_mapdifclr_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_mapdifclr_destroy_element (
    struct global_mapdifclr_data *mapdifclrdata)
{
    DtInt i;

    for (i=0; i<mapdifclrdata->size; i++) {
	if (mapdifclrdata->texture[i].attributes != (DtPtr)0) {
	    dor_space_deallocate(mapdifclrdata->texture[i].attributes);
	}
    }
    if (mapdifclrdata->texture != (struct global_mapdifclr_texture *)0) {
	dor_space_deallocate (mapdifclrdata->texture);
    }

    dor_space_deallocate(mapdifclrdata);
}
