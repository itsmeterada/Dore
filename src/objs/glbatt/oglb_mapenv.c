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
	dor_global_mapenv_initialize 
	dor_global_mapenv_set_value 
	dor_global_mapenv_push_value 
	dor_global_mapenv_pop_value 
	dor_global_mapenv_pop_group 
	dor_global_mapenv_get_value 
	dor_global_mapenv_get_data_pointer 
	dor_global_mapenv_update_execute 
	dor_global_mapenv_destroy_element 

  ======================================================================
 */
#include "global.h"

struct global_mapenv_texture {
   DtObject mapping;
   DtObject raster;
   DtPtr attributes;
   DtInt attributes_size;
};   

struct global_mapenv_data {
   DtInt size;
   struct global_mapenv_texture *texture;
};

dot_object *doe_global_mapenv_object;
DtInt doe_global_mapenv_class_id;
DtInt doe_global_mapenv_init;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_mapenv_get_value }
};

void dor_global_mapenv_initialize (void)
{
    struct gas *gas;

    doe_global_mapenv_class_id = 
	    dor_class_add ("AttTextureMapEnviron", 1, methods,DcNullPtr);

    gas = dor_gas_create(dor_global_mapenv_destroy_element);

    doe_global_mapenv_object = 
	    dor_object_create(doe_global_mapenv_class_id,gas);

    doe_global_mapenv_init = DcFalse;
    dor_global_mapenv_push_value(DcMapReplace, DcStdTableLookup, DcNullObject, 0, 0);
}
/*
 ======================================================================
 */

void dor_global_mapenv_set_value (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster,
    DtPtr attributes,
    DtInt attributes_size)
{
    struct global_mapenv_data *mapenvdata;
    DtInt size;
    struct global_mapenv_texture *texture;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	structure is created and pushed on the stack.   */

    if (dor_gas_test_set_value(doe_global_mapenv_object->data)) {
	dor_global_mapenv_push_value(operator, mapping, raster, attributes, attributes_size);
    } else {
	mapenvdata = dor_global_mapenv_get_data_pointer();

	if (mapenvdata == (struct global_mapenv_data *)DcNullPtr) {
	    return;
	}

	if (operator == DcMapAdd) {
	    size = mapenvdata->size + 1;
	} else			/* DcMapReplace */ {
	    size = 1;
	}
	if (size != mapenvdata->size) {
	    texture = (struct global_mapenv_texture *)dor_space_reallocate
		    (mapenvdata->texture, size*sizeof(struct global_mapenv_texture));
	    if (size > mapenvdata->size) {
		texture[size-1].attributes_size = 0;
		texture[size-1].attributes = 0;
	    }
	    mapenvdata->texture = texture;
	    mapenvdata->size = size;
	} else {
	    texture = mapenvdata->texture;
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

	dor_global_mapenv_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_mapenv_push_value (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster,
    DtPtr attributes,
    DtInt attributes_size)
{
    struct global_mapenv_data *mapenvdata;
    struct global_mapenv_data *newmapenvdata;
    struct object *currentgroup;
    DtInt i;

    newmapenvdata = (struct global_mapenv_data *) dor_space_allocate
	    (sizeof *newmapenvdata);

    if (doe_global_mapenv_init) {
	if (!dor_gas_get_value (doe_global_mapenv_object->data,
				(DtPtr)(&mapenvdata))) {
	    mapenvdata = (struct global_mapenv_data *)DcNullPtr;
	}
    } else {
	mapenvdata = (struct global_mapenv_data *)DcNullPtr;
	doe_global_mapenv_init = DcTrue;
    }

    newmapenvdata->size = 1;
    if ((mapenvdata != (struct global_mapenv_data *)DcNullPtr) &&
	(operator == DcMapAdd)) {
	newmapenvdata->size += mapenvdata->size;
    }
    newmapenvdata->texture = (struct global_mapenv_texture *)
	    dor_space_allocate (newmapenvdata->size *sizeof(struct global_mapenv_texture));

    for (i=0; i<newmapenvdata->size-1; i++) {
	if (mapenvdata->texture[i].attributes_size != 0) {
	    newmapenvdata->texture[i].attributes = (DtPtr) dor_space_allocate 
		    (mapenvdata->texture[i].attributes_size*sizeof (char));
	    bcopy (mapenvdata->texture[i].attributes, 
		   newmapenvdata->texture[i].attributes, 
		   mapenvdata->texture[i].attributes_size);
	} else {
	    newmapenvdata->texture[i].attributes = 0;
	}
	newmapenvdata->texture[i].mapping = mapenvdata->texture[i].mapping;
	newmapenvdata->texture[i].raster = mapenvdata->texture[i].raster;
	newmapenvdata->texture[i].attributes_size = mapenvdata->texture[i].attributes_size;
    }

    i = newmapenvdata->size - 1;
    if (attributes_size != 0) {
	newmapenvdata->texture[i].attributes = (DtPtr) dor_space_allocate 
		(attributes_size*sizeof (char));
	bcopy (attributes, newmapenvdata->texture[i].attributes, attributes_size);
    } else {
	newmapenvdata->texture[i].attributes = 0;
    }
    newmapenvdata->texture[i].mapping = mapping;
    newmapenvdata->texture[i].raster = raster;
    newmapenvdata->texture[i].attributes_size = attributes_size;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_mapenv_object->data,
			   newmapenvdata)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,
				   dor_global_mapenv_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapenv_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_mapenv_pop_value (void)
{
    DtFlag popflag;
    struct global_mapenv_data *mapenvdata;

    popflag = dor_gas_pop_value (doe_global_mapenv_object->data,
				 (DtPtr)(&mapenvdata));

    dor_global_mapenv_destroy_element(mapenvdata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapenv_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_mapenv_pop_group (void)
{
    dor_gas_pop_group(doe_global_mapenv_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapenv_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_mapenv_get_value (
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
   struct global_mapenv_data *mapenvdata;
   DtInt in, out;

#ifdef DEBUG
   printf ("max_textures = %d\nmax_attr_size = 0x%lx\n", max_textures, max_attr_size);
#endif

   getflag = dor_gas_get_value (doe_global_mapenv_object->data,
			        (DtPtr)(&mapenvdata));

   if (!getflag) {
      DDerror (ERR_NO_CURRENT_VALUE,
		  "dor_global_mapenv_get_value", DcNullPtr);
      return(DcFalse);
   }

   *num_textures = mapenvdata->size;

   copy_textures = max_textures < mapenvdata->size ? max_textures : 
	 mapenvdata->size;

   /* NOTE: The list of textures is stored in the order that they are executed but
      they should be applied in the reverse order, so the list returned is reversed so
      that the first texture in the list is the one that should be applied first.
      */

   for (out=0, in=mapenvdata->size - 1; out<copy_textures; out++, in--) {
      mapping[out] = mapenvdata->texture[in].mapping;
      raster[out]  = mapenvdata->texture[in].raster;
      attributes_size[out] = mapenvdata->texture[in].attributes_size;

      copy_size = max_attr_size <= mapenvdata->texture[in].attributes_size ? max_attr_size : 
	    mapenvdata->texture[in].attributes_size;

#ifdef DEBUG
      printf ("copy_size = 0x%lx\nattributes = 0x%lx\nattributes + %d*0x%lx = 0x%lx\n",
	      copy_size, attributes, out, max_attr_size, attributes+out*max_attr_size);
#endif

      bcopy (mapenvdata->texture[in].attributes, attributes+out*max_attr_size, copy_size);
   }

   return(DcTrue);
}
/*
 ======================================================================
 */

struct global_mapenv_data *dor_global_mapenv_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_mapenv_object->data,&dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_mapenv_get_data_pointer", DcNullPtr);
	return((struct global_mapenv_data *)DcNullPtr);
    }

    return((struct global_mapenv_data *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_mapenv_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_mapenv_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_mapenv_destroy_element (
    struct global_mapenv_data *mapenvdata)
{
    DtInt i;

    for (i=0; i<mapenvdata->size; i++) {
	if (mapenvdata->texture[i].attributes != (DtPtr)0) {
	    dor_space_deallocate(mapenvdata->texture[i].attributes);
	}
    }
    if (mapenvdata->texture != (struct global_mapenv_texture *)0) {
	dor_space_deallocate (mapenvdata->texture);
    }

    dor_space_deallocate(mapenvdata);
}
