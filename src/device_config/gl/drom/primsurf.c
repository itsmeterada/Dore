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
 
/*****************************************************************************
**  This file contains method functions for primsurf objects.
*****************************************************************************/

#include <math.h>
#include "../gl_driver.h"
#include <dore/dore_develop/private/prisrf.h>

#define  DEBUG_CALL    0
#define  DEBUG_SPHERE  0


    /*******************************/
    /***  Function Declarations  ***/
    /*******************************/

static DtInt ddr_gl_drom_render_GL_primsurf_sphere    (struct object *);
static DtInt ddr_gl_drom_render_GL_primsurf_cylinder  (struct object *);


    /*************************/
    /***  Data Structures  ***/
    /*************************/

	/* GL Sphere Data */

typedef struct
{   int    tessprimtype;	/* Tesselation Primitive Type */
    int    tessdepth;		/* Tesselation Depth */
    Object objid;		/* Object Identifier */
} PSData;

static DtInt classid   = -1;	/* Primitive Surface Class Identifier */
static DtInt dataindex = -1;	/* PrimSurf Additional Data Field Index */



/*****************************************************************************
**  This routine allocates and initializes the GL-specific data for the GL
**  sphere primitive used for the primsurf object.  It returns the address of
**  the allocated and initialized data.  This data can later be accessed from
**  the additional_data field of the object.
*****************************************************************************/

DtPtr ddr_gl_drom_primsurf_create (DtObjectStructure *object)
{
    register PSData *psdata;	/* New Primsurf Data */

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_primsurf_create (object %p)\n", object);
#   endif

    psdata = DDspace_Allocate (sizeof(*psdata));

    psdata->objid = -1;

    return psdata;
}



/*****************************************************************************
**  This routine is called to print the additional_data field of the primsurf
**  object.
*****************************************************************************/

void ddr_gl_drom_primsurf_print (
    DtObjectStructure *object,
    DtPtr data)
{
    auto PSData *psdata = data;		/* Primsurf Data Pointer */

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_primsurf_print (object %p, data %p)\n",
	    object, data);
#   endif

    printf ("Primsurf Data (object %p):\n", object);
    printf ("    Additional data at %p\n", psdata);
    printf ("    GL Object Identifier %ld\n", (long)(psdata->objid));
}



/*****************************************************************************
**  This routine is called to cleanup and free allocated space for the 
**  additional data associated with a primsurf object.  In this case (for
**  GL spheres), we need to free up the sphere object and de-allocate the
**  additional_data field.
*****************************************************************************/

void ddr_gl_drom_primsurf_delete (
    DtObjectStructure *object,	/* PrimSurf Object */
    DtPtr data)			/* Additional Data Pointer */
{
    auto PSData *psdata = data;		/* PrimSurf Data Pointer */
#   if DEBUG_CALL
	printf ("* ddr_gl_drom_primsurf_delete (object %p, psdata %p)\n",
	    object, psdata);
#   endif

    if (!psdata) return;

    delobj (((PSData*)(psdata)) -> objid);

    DDspace_Deallocate (psdata);
}



/*****************************************************************************
**  This routine is called whenever a primsurf object is to be rendered.  If
**  the object is a sphere and can be done via GL spheres, then a GL sphere
**  is used, otherwise the standard alternate representation is used.
*****************************************************************************/

void ddr_gl_drom_primsurf_render (dot_object *object)
{
    static DtMethodPtr    altobj_query = 0;	/* Alt. Object Routine Query */
    auto   struct prisrf *primsurf;
    auto   DtObject       alt_obj;		/* Alt. Object */

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_primsurf_render (object %p)\n", object);
#   endif

    /* If we don't know what the class ID for the primsurf is yet, get it. */

    if (classid == -1) 
	classid = DsInqClassId ("DoPrimSurf");

    /* Determine if the primsurf class is currently renderable. */

    if (!DDcondex_QueryRender(classid))
	return;

    /* Handle the primsurf according the the primsurf type. */

    primsurf = (struct prisrf *) (object->data);

    switch (primsurf->surface_type)
    {
	case DcSphere:
	    if (ddr_gl_drom_render_GL_primsurf_sphere (object))
		return;
	    break;

	case DcCylinder:
	    if (ddr_gl_drom_render_GL_primsurf_cylinder (object))
		return;
	    break;

	case DcBox:
	case DcCone:
	    /* Use standard alternate representation. */
	    break;
    }

    /* If we don't support the primsurf requested, or if the primsurf
    ** couldn't be rendered, then just execute the standard alternate
    ** representation (triangles).  */

    if (!altobj_query)
	altobj_query = DDobject_InqMethod (object,DcMethodUpdStdAltObj);

    if (altobj_query)
    {
	alt_obj = (DtObject) (*altobj_query)(object);

	if (alt_obj)
	    DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	else
	{   DDerror (ERR_CANT_CREATE_ALTOBJ,
		     "ddr_gl_drom_primsurf_render", "");
	    return;
	}
    }
}



/*****************************************************************************
**  This routine determines if a GL sphere can be used (according to the
**  current attributes).  If a GL sphere cannot be used, the function returns
**  0.  If a GL sphere _can_ be used, the sphere is used, or created & used,
**  or destroyed, recreated and used.  After the sphere is called, the
**  function returns 1.
*****************************************************************************/

static DtInt ddr_gl_drom_render_GL_primsurf_sphere (struct object *object)
{
    static   DtReal       center[3] = {0,0,0};	/* Sphere Center */
    static   DtMethodPtr  gentexuv_query = 0;	/* 2D Tex Mapping ? */
    register int          ii;			/* Loop Index */
    auto     PSData      *psdata;		/* PrimSurf Data */
    auto     DtSwitch     uvswitchvalue;	/* 2D Tex Switch */

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_render_GL_primsurf_sphere (object %p)\n",object);
#   endif

    /* If the spheretype is specified as Dore' spheres, then return 0. */

    if (CURDEV->sphere_type == SPHTYPE_DORE)
	return 0;

    if (!gentexuv_query)
    {   gentexuv_query = DDclass_InqMethod
			 (   DsInqClassId ("AttCompTextureUVSwitch"),
			     DcMethodInqGlbAttVal
			 );
    }
    (*gentexuv_query) (&uvswitchvalue);

    /* If the the sphere is texture-mapped, then use the tesselated spheres
    ** instead of a GL sphere.  */

    if (uvswitchvalue && DevStatus.texture_2D)
	return 0;

    /* Get the additional_data field index for the primsurf objects, if we
    ** haven't already.  */

    if (dataindex == -1)
    {   for (ii=0;  dde_gl_drom_class_data[ii].name;  ++ii)
	{   if (dde_gl_drom_class_data[ii].id == classid)
	    {   dataindex = dde_gl_drom_class_data[ii].index;
		break;
	    }
	}
    }

    /* Grab the additional-data field of the primsurf. */

    psdata = (PSData*) (object->additional_data [dataindex]);

    /* Call MakeGLSphereList() to create or re-create the sphere if necessary
    ** (if either the subdivision or surface rep parameters have changed). */

    psdata->objid = MakeGLSphereList
		    (   psdata->objid,
			&(psdata->tessprimtype), &(psdata->tessdepth),
			1, center, DcNullPtr, DcNullPtr
		    );

    /* Render the GL sphere. */

#   if DEBUG_SPHERE
	printf ("- Calling callobj (%08lX)\n", psdata->objid);
#   endif

    callobj (psdata->objid);

    return 1;
}



/*****************************************************************************
**  This routine determines if a GL cylinder can be used (according to the
**  current attributes).  If a GL-strip cylinder cannot be used, the function
**  returns 0.  If a GL cylinder _can_ be used, the cylinder is used, or
**  created & used, or destroyed, recreated and used.  After the cylinder is
**  called, the function returns 1.
*****************************************************************************/

static DtInt ddr_gl_drom_render_GL_primsurf_cylinder (struct object *object)
{
    static   DtMethodPtr gentexuv_query = 0;	/* 2D Tex Mapping ? */
    register int         ii;			/* Loop Index */
    auto     PSData     *psdata;		/* PrimSurf Data */
    auto     DtSwitch    uvswitchvalue;		/* 2D Tex Switch */

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_render_GL_primsurf_cylinder (object %p)\n",
	    object);
#   endif

    if (!gentexuv_query)
    {   gentexuv_query = DDclass_InqMethod
			 (   DsInqClassId ("AttCompTextureUVSwitch"),
			     DcMethodInqGlbAttVal
			 );
    }
    (*gentexuv_query) (&uvswitchvalue);

    /* If the the sphere is texture-mapped, then use the tesselated cylinders
    ** instead of a GL cylinder.  */

    if (uvswitchvalue && DevStatus.texture_2D)
	return 0;

    /* Get the additional_data field index for the primsurf objects, if we
    ** haven't already.  */

    if (dataindex == -1)
    {   for (ii=0;  dde_gl_drom_class_data[ii].name;  ++ii)
	{   if (dde_gl_drom_class_data[ii].id == classid)
	    {   dataindex = dde_gl_drom_class_data[ii].index;
		break;
	    }
	}
    }

    /* Grab the additional-data field of the primsurf. */

    psdata = (PSData*) (object->additional_data [dataindex]);

    /* Call MakeGLCylinder() to create or re-create the cylinder if necessary
    ** (if either the subdivision or surface rep parameters have changed). */

    psdata->objid = MakeGLCylinder
		    (   psdata->objid,
			&(psdata->tessprimtype), &(psdata->tessdepth)
		    );

    /* Render the GL cylinder. */

    ddr_iris_restore_diffuse_color ();
    callobj (psdata->objid);

    return 1;
}
