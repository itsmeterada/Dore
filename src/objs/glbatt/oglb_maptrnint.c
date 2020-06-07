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
	dor_global_maptrnint_initialize 
	dor_global_maptrnint_set_value 
	dor_global_maptrnint_push_value 
	dor_global_maptrnint_pop_value 
	dor_global_maptrnint_pop_group 
	dor_global_maptrnint_get_value 
	dor_global_maptrnint_get_data_pointer 
	dor_global_maptrnint_update_execute 
	dor_global_maptrnint_destroy_element 

  ======================================================================
 */
#include "global.h"

struct global_maptrnint_texture {
   DtObject mapping;
   DtObject raster;
   DtPtr attributes;
   DtInt attributes_size;
};   

struct global_maptrnint_data {
   DtInt size;
   struct global_maptrnint_texture *texture;
};

dot_object *doe_global_maptrnint_object;
DtInt doe_global_maptrnint_class_id;
DtInt doe_global_maptrnint_init;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_maptrnint_get_value }
};

void dor_global_maptrnint_initialize (void)
{
    struct gas *gas;

    doe_global_maptrnint_class_id = 
	    dor_class_add ("AttTextureMapTranspIntens", 1, methods,DcNullPtr);

    gas = dor_gas_create(dor_global_maptrnint_destroy_element);

    doe_global_maptrnint_object = 
	    dor_object_create(doe_global_maptrnint_class_id,gas);

    doe_global_maptrnint_init = DcFalse;
    dor_global_maptrnint_push_value(DcMapReplace, DcStdTableLookup, DcNullObject, 0, 0);
}
/*
 ======================================================================
 */

void dor_global_maptrnint_set_value (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster,
    DtPtr attributes,
    DtInt attributes_size)
{
    struct global_maptrnint_data *maptrnintdata;
    DtInt size;
    struct global_maptrnint_texture *texture;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	structure is created and pushed on the stack.   */

    if (dor_gas_test_set_value(doe_global_maptrnint_object->data)) {
	dor_global_maptrnint_push_value(operator, mapping, raster, attributes, attributes_size);
    } else {
	maptrnintdata = dor_global_maptrnint_get_data_pointer();

	if (maptrnintdata == (struct global_maptrnint_data *)DcNullPtr) {
	    return;
	}

	if (operator == DcMapAdd) {
	    size = maptrnintdata->size + 1;
	} else			/* DcMapReplace */ {
	    size = 1;
	}
	if (size != maptrnintdata->size) {
	    texture = (struct global_maptrnint_texture *)dor_space_reallocate
		    (maptrnintdata->texture, size*sizeof(struct global_maptrnint_texture));
	    if (size > maptrnintdata->size) {
		texture[size-1].attributes_size = 0;
		texture[size-1].attributes = 0;
	    }
	    maptrnintdata->texture = texture;
	    maptrnintdata->size = size;
	} else {
	    texture = maptrnintdata->texture;
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

	dor_global_maptrnint_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_maptrnint_push_value (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster,
    DtPtr attributes,
    DtInt attributes_size)
{
    struct global_maptrnint_data *maptrnintdata;
    struct global_maptrnint_data *newmaptrnintdata;
    struct object *currentgroup;
    DtInt i;

    newmaptrnintdata = (struct global_maptrnint_data *) dor_space_allocate
	    (sizeof *newmaptrnintdata);

    if (doe_global_maptrnint_init) {
	if (!dor_gas_get_value (doe_global_maptrnint_object->data,
				(DtPtr)(&maptrnintdata))) {
	    maptrnintdata = (struct global_maptrnint_data *)DcNullPtr;
	}
    } else {
	maptrnintdata = (struct global_maptrnint_data *)DcNullPtr;
	doe_global_maptrnint_init = DcTrue;
    }

    newmaptrnintdata->size = 1;
    if ((maptrnintdata != (struct global_maptrnint_data *)DcNullPtr) &&
	(operator == DcMapAdd)) {
	newmaptrnintdata->size += maptrnintdata->size;
    }
    newmaptrnintdata->texture = (struct global_maptrnint_texture *)
	    dor_space_allocate (newmaptrnintdata->size *sizeof(struct global_maptrnint_texture));

    for (i=0; i<newmaptrnintdata->size-1; i++) {
	if (maptrnintdata->texture[i].attributes_size != 0) {
	    newmaptrnintdata->texture[i].attributes = (DtPtr) dor_space_allocate 
		    (maptrnintdata->texture[i].attributes_size*sizeof (char));
	    bcopy (maptrnintdata->texture[i].attributes, 
		   newmaptrnintdata->texture[i].attributes, 
		   maptrnintdata->texture[i].attributes_size);
	} else {
	    newmaptrnintdata->texture[i].attributes = 0;
	}
	newmaptrnintdata->texture[i].mapping = maptrnintdata->texture[i].mapping;
	newmaptrnintdata->texture[i].raster = maptrnintdata->texture[i].raster;
	newmaptrnintdata->texture[i].attributes_size = maptrnintdata->texture[i].attributes_size;
    }

    i = newmaptrnintdata->size - 1;
    if (attributes_size != 0) {
	newmaptrnintdata->texture[i].attributes = (DtPtr) dor_space_allocate 
		(attributes_size*sizeof (char));
	bcopy (attributes, newmaptrnintdata->texture[i].attributes, attributes_size);
    } else {
	newmaptrnintdata->texture[i].attributes = 0;
    }
    newmaptrnintdata->texture[i].mapping = mapping;
    newmaptrnintdata->texture[i].raster = raster;
    newmaptrnintdata->texture[i].attributes_size = attributes_size;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_maptrnint_object->data,
			   newmaptrnintdata)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,
				   dor_global_maptrnint_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_maptrnint_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_maptrnint_pop_value (void)
{
    DtFlag popflag;
    struct global_maptrnint_data *maptrnintdata;

    popflag = dor_gas_pop_value (doe_global_maptrnint_object->data,
				 (DtPtr)(&maptrnintdata));

    dor_global_maptrnint_destroy_element(maptrnintdata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_maptrnint_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_maptrnint_pop_group (void)
{
    dor_gas_pop_group(doe_global_maptrnint_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_maptrnint_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_maptrnint_get_value (
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
   struct global_maptrnint_data *maptrnintdata;
   DtInt in, out;

#ifdef DEBUG
   printf ("max_textures = %d\nmax_attr_size = 0x%lx\n", max_textures, max_attr_size);
#endif

   getflag = dor_gas_get_value (doe_global_maptrnint_object->data,
			        (DtPtr)(&maptrnintdata));

   if (!getflag) {
      DDerror (ERR_NO_CURRENT_VALUE,
		  "dor_global_maptrnint_get_value", DcNullPtr);
      return(DcFalse);
   }

   *num_textures = maptrnintdata->size;

   copy_textures = max_textures < maptrnintdata->size ? max_textures : 
	 maptrnintdata->size;

   /* NOTE: The list of textures is stored in the order that they are executed but
      they should be applied in the reverse order, so the list returned is reversed so
      that the first texture in the list is the one that should be applied first.
      */

   for (out=0, in=maptrnintdata->size - 1; out<copy_textures; out++, in--) {
      mapping[out] = maptrnintdata->texture[in].mapping;
      raster[out]  = maptrnintdata->texture[in].raster;
      attributes_size[out] = maptrnintdata->texture[in].attributes_size;

      copy_size = max_attr_size <= maptrnintdata->texture[in].attributes_size ? max_attr_size : 
	    maptrnintdata->texture[in].attributes_size;

#ifdef DEBUG
      printf ("copy_size = 0x%lx\nattributes = 0x%lx\nattributes + %d*0x%lx = 0x%lx\n",
	      copy_size, attributes, out, max_attr_size, attributes+out*max_attr_size);
#endif

      bcopy (maptrnintdata->texture[in].attributes, attributes+out*max_attr_size, copy_size);
   }

   return(DcTrue);
}
/*
 ======================================================================
 */

struct global_maptrnint_data *dor_global_maptrnint_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_maptrnint_object->data,&dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_maptrnint_get_data_pointer", DcNullPtr);
	return((struct global_maptrnint_data *)DcNullPtr);
    }

    return((struct global_maptrnint_data *)dataptr);
}
/*
 ======================================================================
 */

      /*  The attribute (may have) changed value.  Execute the
	  currently active method (like rendering, rendering
	  initialization, etc.).  */

void dor_global_maptrnint_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_maptrnint_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_maptrnint_destroy_element (
    struct global_maptrnint_data *maptrnintdata)
{
    DtInt i;

    for (i=0; i<maptrnintdata->size; i++) {
	if (maptrnintdata->texture[i].attributes != (DtPtr)0) {
	    dor_space_deallocate(maptrnintdata->texture[i].attributes);
	}
    }
    if (maptrnintdata->texture != (struct global_maptrnint_texture *)0) {
	dor_space_deallocate (maptrnintdata->texture);
    }

    dor_space_deallocate(maptrnintdata);
}
