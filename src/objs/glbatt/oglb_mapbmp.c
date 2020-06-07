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
	dor_global_mapbmp_initialize 
	dor_global_mapbmp_set_value 
	dor_global_mapbmp_push_value 
	dor_global_mapbmp_pop_value 
	dor_global_mapbmp_pop_group 
	dor_global_mapbmp_get_value 
	dor_global_mapbmp_get_data_pointer 
	dor_global_mapbmp_update_execute 
	dor_global_mapbmp_destroy_element 

  ======================================================================
 */
#include "global.h"

struct global_mapbmp_texture {
   DtObject mapping;
   DtObject raster;
   DtPtr attributes;
   DtInt attributes_size;
};   

struct global_mapbmp_data {
   DtInt size;
   struct global_mapbmp_texture *texture;
};

dot_object *doe_global_mapbmp_object;
DtInt doe_global_mapbmp_class_id;
DtInt doe_global_mapbmp_init;
/*
 ======================================================================
 */


static DtMethodEntry methods[] = {
    { DcMethodInqGlbAttVal, (DtMethodPtr)dor_global_mapbmp_get_value }
};

void dor_global_mapbmp_initialize (void)
{
    struct gas *gas;

    doe_global_mapbmp_class_id = 
	    dor_class_add ("AttTextureMapBump", 1, methods,DcNullPtr);

    gas = dor_gas_create(dor_global_mapbmp_destroy_element);

    doe_global_mapbmp_object = 
	    dor_object_create(doe_global_mapbmp_class_id,gas);

    doe_global_mapbmp_init = DcFalse;
    dor_global_mapbmp_push_value(DcMapReplace, DcStdTableLookup, DcNullObject, 0, 0);
}
/*
 ======================================================================
 */

void dor_global_mapbmp_set_value (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster,
    DtPtr attributes,
    DtInt attributes_size)
{
    struct global_mapbmp_data *mapbmpdata;
    DtInt size;
    struct global_mapbmp_texture *texture;

    /*  If setting the value of the global attribute would
	result in an implicit push (this would occur when the
	value is the first on the current group stack), then a new
	structure is created and pushed on the stack.   */

    if (dor_gas_test_set_value(doe_global_mapbmp_object->data)) {
	dor_global_mapbmp_push_value(operator, mapping, raster, attributes, attributes_size);
    } else {
	mapbmpdata = dor_global_mapbmp_get_data_pointer();

	if (mapbmpdata == (struct global_mapbmp_data *)DcNullPtr) {
	    return;
	}

	if (operator == DcMapAdd) {
	    size = mapbmpdata->size + 1;
	} else			/* DcMapReplace */ {
	    size = 1;
	}
	if (size != mapbmpdata->size) {
	    texture = (struct global_mapbmp_texture *)dor_space_reallocate
		    (mapbmpdata->texture, size*sizeof(struct global_mapbmp_texture));
	    if (size > mapbmpdata->size) {
		texture[size-1].attributes_size = 0;
		texture[size-1].attributes = 0;
	    }
	    mapbmpdata->texture = texture;
	    mapbmpdata->size = size;
	} else {
	    texture = mapbmpdata->texture;
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

	dor_global_mapbmp_update_execute();
    }
}
/*
 ======================================================================
 */

void dor_global_mapbmp_push_value (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster,
    DtPtr attributes,
    DtInt attributes_size)
{
    struct global_mapbmp_data *mapbmpdata;
    struct global_mapbmp_data *newmapbmpdata;
    struct object *currentgroup;
    DtInt i;

    newmapbmpdata = (struct global_mapbmp_data *) dor_space_allocate
	    (sizeof *newmapbmpdata);

    if (doe_global_mapbmp_init) {
	if (!dor_gas_get_value (doe_global_mapbmp_object->data,
				(DtPtr)(&mapbmpdata))) {
	    mapbmpdata = (struct global_mapbmp_data *)DcNullPtr;
	}
    } else {
	mapbmpdata = DcNullPtr;
	doe_global_mapbmp_init = DcTrue;
    }

    newmapbmpdata->size = 1;
    if ((mapbmpdata != (struct global_mapbmp_data *)DcNullPtr) &&
	(operator == DcMapAdd)) {
	newmapbmpdata->size += mapbmpdata->size;
    }
    newmapbmpdata->texture = (struct global_mapbmp_texture *)
	    dor_space_allocate (newmapbmpdata->size *sizeof(struct global_mapbmp_texture));

    for (i=0; i<newmapbmpdata->size-1; i++) {
	if (mapbmpdata->texture[i].attributes_size != 0) {
	    newmapbmpdata->texture[i].attributes = (DtPtr) dor_space_allocate 
		    (mapbmpdata->texture[i].attributes_size*sizeof (char));
	    bcopy (mapbmpdata->texture[i].attributes, 
		   newmapbmpdata->texture[i].attributes, 
		   mapbmpdata->texture[i].attributes_size);
	} else {
	    newmapbmpdata->texture[i].attributes = 0;
	}
	newmapbmpdata->texture[i].mapping = mapbmpdata->texture[i].mapping;
	newmapbmpdata->texture[i].raster = mapbmpdata->texture[i].raster;
	newmapbmpdata->texture[i].attributes_size = mapbmpdata->texture[i].attributes_size;
    }

    i = newmapbmpdata->size - 1;
    if (attributes_size != 0) {
	newmapbmpdata->texture[i].attributes = (DtPtr) dor_space_allocate 
		(attributes_size*sizeof (char));
	bcopy (attributes, newmapbmpdata->texture[i].attributes, attributes_size);
    } else {
	newmapbmpdata->texture[i].attributes = 0;
    }
    newmapbmpdata->texture[i].mapping = mapping;
    newmapbmpdata->texture[i].raster = raster;
    newmapbmpdata->texture[i].attributes_size = attributes_size;

    /*  If the gas utility returns DcTrue, then this was the first
	value pushed for this attribute for the current stacking
	level.  Therefore, we must notify the group mechanism that this
	attribute must be "unwound" when the group terminates.  We pass
	the group handler the actual "unwind" entry point for this
	attribute.  */

    if (dor_gas_push_value(doe_global_mapbmp_object->data,
			   newmapbmpdata)) {
	currentgroup = dor_group_get_current_group();

	dor_group_record_attribute(currentgroup,
				   dor_global_mapbmp_pop_group);
    }

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapbmp_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_mapbmp_pop_value (void)
{
    DtFlag popflag;
    struct global_mapbmp_data *mapbmpdata;

    popflag = dor_gas_pop_value(doe_global_mapbmp_object->data,
				(DtPtr)(&mapbmpdata));

    dor_global_mapbmp_destroy_element(mapbmpdata);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapbmp_update_execute();

    return(popflag);
}
/*
 ======================================================================
 */

/*  Pop all of the attribute values stored for this global attribute for 
    the current group.  The gas subroutine will ask the attribute object 
    to destroy any instance data being popped.  */

void dor_global_mapbmp_pop_group (void)
{
    dor_gas_pop_group(doe_global_mapbmp_object->data);

    /*  The value of this attribute may have changed, go execute
	whatever update method is appropriate at this time.  */

    dor_global_mapbmp_update_execute();
}
/*
 ======================================================================
 */

DtFlag dor_global_mapbmp_get_value (
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
   struct global_mapbmp_data *mapbmpdata;
   DtInt in, out;

#   ifdef DEBUG
	printf ("max_textures = %d\nmax_attr_size = 0x%x\n",
	    max_textures, max_attr_size);
#   endif

   getflag = dor_gas_get_value(doe_global_mapbmp_object->data,
			       (DtPtr)(&mapbmpdata));

   if (!getflag) {
      DDerror (ERR_NO_CURRENT_VALUE,
		  "dor_global_mapbmp_get_value", DcNullPtr);
      return(DcFalse);
   }

   *num_textures = mapbmpdata->size;

   copy_textures = max_textures < mapbmpdata->size ? max_textures : 
	 mapbmpdata->size;

   /* NOTE: The list of textures is stored in the order that they are executed but
      they should be applied in the reverse order, so the list returned is reversed so
      that the first texture in the list is the one that should be applied first.
      */

   for (out=0, in=mapbmpdata->size - 1; out<copy_textures; out++, in--) {
      mapping[out] = mapbmpdata->texture[in].mapping;
      raster[out]  = mapbmpdata->texture[in].raster;
      attributes_size[out] = mapbmpdata->texture[in].attributes_size;

      copy_size = max_attr_size <= mapbmpdata->texture[in].attributes_size ? max_attr_size : 
	    mapbmpdata->texture[in].attributes_size;

#     ifdef DEBUG
	  printf ("copy_size  = 0x%x\n",  copy_size);
	  printf ("attributes = 0x%lx\n", attributes);
	  printf ("attributes + %d*0x%lx = 0x%lx\n",
	      out, max_attr_size, attributes+out*max_attr_size);
#     endif

      bcopy (mapbmpdata->texture[in].attributes, attributes+out*max_attr_size, copy_size);
   }

   return(DcTrue);
}
/*
 ======================================================================
 */

struct global_mapbmp_data *dor_global_mapbmp_get_data_pointer (void)
{
    DtFlag getflag;
    DtPtr dataptr;

    getflag = dor_gas_get_value(doe_global_mapbmp_object->data,&dataptr);

    if (!getflag) {
	DDerror (ERR_NO_CURRENT_VALUE,
		    "dor_global_mapbmp_get_data_pointer", DcNullPtr);
	return((struct global_mapbmp_data *)DcNullPtr);
    }

    return((struct global_mapbmp_data *)dataptr);
}
/*
 ======================================================================
 */

/*  The attribute (may have) changed value.  Execute the currently active 
    method (like rendering, rendering initialization, etc.).  */

void dor_global_mapbmp_update_execute (void)
{
    DtMethodPtr executemethod;

    executemethod = dor_object_get_current_method(doe_global_mapbmp_object);

    (*executemethod)();
}
/*
 ======================================================================
 */

void dor_global_mapbmp_destroy_element (
    struct global_mapbmp_data *mapbmpdata)
{
    DtInt i;

    for (i=0; i<mapbmpdata->size; i++) {
	if (mapbmpdata->texture[i].attributes != (DtPtr)0) {
	    dor_space_deallocate(mapbmpdata->texture[i].attributes);
	}
    }
    if (mapbmpdata->texture != (struct global_mapbmp_texture *)0) {
	dor_space_deallocate (mapbmpdata->texture);
    }

    dor_space_deallocate(mapbmpdata);
}
