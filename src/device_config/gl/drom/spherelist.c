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
**  This file contains method functions for spherelist objects.
*****************************************************************************/

#include <math.h>
#include "../gl_driver.h"
#include <gl/sphere.h>
#include <dore/dore_develop/private/sphlst.h>

#define  DEBUG_CALL    0
#define  DEBUG_SPHERE  0


    /*******************************/
    /***  Function Declarations  ***/
    /*******************************/

static DtInt ddr_gl_drom_render_GL_spherelist (struct object *,struct sphlst *);


    /*************************/
    /***  Data Structures  ***/
    /*************************/

	/* GL SphereList Data */

typedef struct
{   int    tessprimtype;	/* Tesselation Primitive Type */
    int    tessdepth;		/* Tesselation Depth */
    Object objid;		/* Object Identifier */
} SLData;

static DtInt classid   = -1;	/* Spherelist Class Identifier */
static DtInt dataindex = -1;	/* Spherelist Additional Data Field Index */



/*****************************************************************************
**  This routine allocates and initializes the GL-specific data for the GL
**  spherelist used for the spherelist object.  It returns the address of
**  the allocated and initialized data.  This data can later be accessed from
**  the additional_data field of the object.
*****************************************************************************/

DtPtr ddr_gl_drom_spherelist_create (DtObjectStructure *object)
{
    register SLData *sldata;	/* New Sphere-List Data */

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_spherelist_create (object %p)\n", object);
#   endif

    sldata = DDspace_Allocate (sizeof(*sldata));

    sldata->objid = -1;

    return sldata;
}



/*****************************************************************************
**  This routine is called to print the additional_data field of the sphere-
**  list object.
*****************************************************************************/

void ddr_gl_drom_spherelist_print (
    DtObjectStructure *object,
    DtPtr data)
{
    /* No-op */
}



/*****************************************************************************
**  This routine is called to cleanup and free allocated space for the 
**  additional data associated with a spherelist object.  In this case (for
**  GL spheres), we need to free up the spherelist object and de-allocate the
**  additional_data field.
*****************************************************************************/

void ddr_gl_drom_spherelist_delete (
    DtObjectStructure *object,		/* Spherelist Object */
    DtPtr sldata_parm)			/* Spherelist Data */
{
    auto SLData *sldata = (SLData*)(sldata_parm);

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_spherelist_delete (object %p, sldata %p)\n",
	    object, sldata);
#   endif

    if (!sldata) return;

    if (sldata->objid != -1)
	delobj (sldata->objid);

    DDspace_Deallocate (sldata);
}



/*****************************************************************************
**  This routine is called whenever a spherelist object is to be rendered.
*****************************************************************************/

void ddr_gl_drom_spherelist_render (dot_object *object)
{
    static DtMethodPtr    altobj_query = 0;	/* Alt. Object Routine Query */
    auto   struct sphlst *spherelist;		/* Sphere List Data */
    auto   DtObject       alt_obj;		/* Alt. Object */

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_spherelist_render (object %p)\n", object);
#   endif

    /* If we don't know what the class ID for the spherelist is yet, get it. */

    if (classid == -1) 
	classid = DsInqClassId ("DoSphereList");

    /* Determine if the spherelist class is currently renderable. */

    if (!DDcondex_QueryRender(classid))
	return;

    /* Handle the spherelist according the the spherelist type. */

    spherelist = (struct sphlst *) (object->data);

    /* Try to render the sphere-list using GL spheres.  If we can't do this,
    ** then use the standard alternate object. */

    if (!ddr_gl_drom_render_GL_spherelist (object, spherelist))
    {
	/* Update and use standard alternate representation (triangles) */

	if (!altobj_query)
	    altobj_query = DDobject_InqMethod (object,DcMethodUpdStdAltObj);

	if (altobj_query)
	{
	    alt_obj = (DtObject) (*altobj_query)(object);

	    if (alt_obj)
		DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	    else
	        DDerror (ERR_CANT_CREATE_ALTOBJ,
			 "ddr_gl_drom_spherelist_render", "");
	}
    }
}



/*****************************************************************************
**  This routine determines if GL spheres can be used (according to the
**  current attributes).  If GL spheres cannot be used, the function returns
**  0.  If GL spheres _can_ be used, the sphere list is used, or created &
**  used, or destroyed, recreated & used.  After the spherelist object is
**  called successfully , the function returns 1.
*****************************************************************************/

static DtInt ddr_gl_drom_render_GL_spherelist (
    struct object *object,
    struct sphlst *spherelist)
{
    static   DtMethodPtr gentexuv_query = 0;	/* 2D Tex Mapping ? */
    register int         ii;			/* Loop Index */
    auto     SLData     *sldata;		/* Sphere List Data */
    auto     DtSwitch    uvswitchvalue;		/* 2D Tex Switch */

#   if DEBUG_CALL
	printf ("* ddr_gl_drom_render_GL_spherelist (object %p)\n", object);
#   endif

    /* If the sphere type is Dore' spheres, then return 0 to use the standard
    ** alternate representation. */

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
    ** instead of GL spheres.  */

    if (uvswitchvalue && DevStatus.texture_2D)
	return 0;

    /* Get the additional_data field index for spherelist objects, if we
    ** haven't already.  */

    if (dataindex == -1)
    {   for (ii=0;  dde_gl_drom_class_data[ii].name;  ++ii)
	{   if (dde_gl_drom_class_data[ii].id == classid)
	    {   dataindex = dde_gl_drom_class_data[ii].index;
		break;
	    }
	}
    }

    /* Grab the additional-data field of the spherelist. */

    sldata = (SLData*) (object->additional_data [dataindex]);

    /* Call MakeGLSphereList() to create or re-create the spherelist if
    ** necessary (if either the subdivision or surface rep parameters
    ** have changed since the last create). */

    sldata->objid = MakeGLSphereList
		    (   sldata->objid,
			&(sldata->tessprimtype), &(sldata->tessdepth),
			spherelist->sphere_count, spherelist->locations,
			spherelist->radii, spherelist->colors
		    );

    /* Render the GL spherelist. */

#   if DEBUG_SPHERE
	printf ("- Calling callobj (%08lX)\n", sldata->objid);
#   endif

    callobj (sldata->objid);

    return 1;
}



/*****************************************************************************
**  This procedure generates a GL object composed of GL spheres.  If an error
**  occurs such that the sphere cannot be generated, then this routine returns
**  a -1 for the object identifier.  If the `object_id' parameter is -1, then
**  a new object identifier is generated, otherwise the given value is used.
**  On successful completion, this routine returns the object identifier.
*****************************************************************************/

Object MakeGLSphereList (
    Object  object_id,		/* Existing GL Object Identifier, Else -1 */
    DtInt  *prevprimtype,	/* Previous Tesselation Primitive Type */
    DtInt  *prevdepth,		/* Previous Tesselation Depth */
    DtInt   numspheres,		/* Number of Spheres in List */
    DtReal *center,		/* Array of Sphere Centers (X,Y,Z ordered) */
    DtReal *radius,		/* Array of Sphere Radii */
    DtReal *color)		/* Array of Sphere Colors (R,G,B ordered) */
{
    register DtReal     *centerptr;	/* Sphere Center Array Pointer */
    register DtReal     *colorptr;	/* Sphere Color Array Pointer */
    auto     int         depth;		/* Sphere Tesselation Depth */
    register int         ii;		/* Loop Index */
    auto     int         primtype;	/* Sphere Tesselation Primitive Type */
    register DtReal     *radiusptr;		/* Sphere Radius Array Ptr */
    static   DtMethodPtr routine_subdiv =0;	/* Subdiv Query Routine Ptr */
    auto     DtReal      sphere_color[3];	/* GL Sphere Color */
    auto     DtInt       subdivtype;		/* Current Subdivision Type */
    auto     DtReal      subdivlevel[1];	/* Current Subdivision Level */

#   if DEBUG_CALL
	printf ("* MakeGLSphereList (\n");
	printf (":     object_id %08lX,\n", object_id);
	printf (":     prevprimtype %d, prevdepth %d,\n",
	    *prevprimtype, *prevdepth);
	printf (":     numspheres %d,\n", numspheres);
	printf (":     center %p, radius %p, color %p)\n", center,radius,color);
#   endif

    /* If we haven't yet grabbed the subdivision inquiry routine address,
    ** then do it now (only done once). */

    if (!routine_subdiv)
    {   routine_subdiv = DDclass_InqMethod
			 (   DsInqClassId  ("AttSubDivSpec"),
			     DsInqMethodId ("InqGlobalValue")
			 );
    }

    /* Get the current subdivision parameters. */

    (*routine_subdiv) (&subdivtype, subdivlevel);

    /* Determine the GL sphere tesselation depth, depending on the Dore'
    ** subdivision type and parameter.  Note that for the DcSubDivAbsolute
    ** and DcSubDivRelative subdivision types, we're just sort of faking it. */

    switch (subdivtype)
    {
	case DcSubDivSegments:
	    depth = subdivlevel[0] / 4;
	    break;
	
	case DcSubDivFixed:
	    if (subdivlevel[0] <= 3)
		depth = 1;
	    else
		depth = 1 << ((int)(subdivlevel[0]) - 3);
	    break;
	
	case DcSubDivAbsolute:
	case DcSubDivRelative:
	    depth = (10 * subdivlevel[0]) + 2;
	    break;

	default:
	    depth = 5;
	    break;
    }

    /* Clamp the depth to the allowable GL range of [1,SPH_MAXDEPTH]. */

    depth = CLAMP (depth, 1, SPH_MAXDEPTH);

    /* Select the GL sphere's primitive type (points, lines or polygonal mesh)
    ** depending on the Dore' reptype.  The default value is polygonal mesh. */

    switch (DevStatus.reptype)
    {
	case DcPoints:
            primtype = SPH_POINT;
            break;

        case DcWireframe:
        case DcOutlines:
            primtype = SPH_LINE;
            break;

	/* If the sphere is to be rendered with surfaces, then we select
	** either GL triangle meshes (strips) or GL polygons.  There's some
	** strange lighting problems with non-uniformly scaled GL trimeshes,
	** so for GL spheres we use polygons instead.  However, this brings
	** a bit of a performance hit since GL trimeshes are better for
	** speed.  So, since the GL hemispheres cannot be non-uniformly
	** scaled, we select triangle meshes if the spheretype is GL
	** hemisphere.  */

        default:
        case DcSurface:
        case DcSurfaceWireframeEdges:
        case DcSurfaceOutlineEdges:
            primtype = (CURDEV->sphere_type == SPHTYPE_GLHEMI) ? SPH_MESH
							       : SPH_POLY ;
            break;
    }

    /* If the given object identifier is -1, then we need to create a new
    ** object, instead of re-using a previous object id.  If the object ID is
    ** valid (not -1), and if we're doing regular GL spheres, then we don't
    ** need to re-create the sphere if the tesselation primitive type and
    ** tesselation depth are unchanged.  If the sphere-type is GL hemispheres,
    ** then we need to re-create the sphere, since fixed-orientation spheres
    ** are unaffected by translation matrices.  */

    if (object_id == -1)
	object_id = genobj();
    else if (  (depth == *prevdepth)
	    && (primtype == *prevprimtype)
	    && (CURDEV->sphere_type != SPHTYPE_GLHEMI)
	    )
	return object_id;

    *prevdepth    = depth;
    *prevprimtype = primtype;

    /* Explicitly set all sphere modes. */

    /* If the sphere-type is SPHTYPE_GLHEMI, then we render fixed-orientation
    ** hemi-spheres (for performance), otherwise we'll render regular GL
    ** spheres (full, non-fixed orientation).  */

    if (CURDEV->sphere_type == SPHTYPE_GLHEMI)
    {   sphmode (SPH_HEMI,   TRUE);	/* Render Hemispheres */
	sphmode (SPH_ORIENT, TRUE);	/* Fixed Sphere Orientation */
    }
    else
    {   sphmode (SPH_HEMI,   FALSE);	/* Render Full Spheres */
	sphmode (SPH_ORIENT, FALSE);	/* Non-Fixed Sphere Orientation */
    }

    sphmode (SPH_TESS,  SPH_OCT);	/* Tesselation Base Type = Octahedron */
    sphmode (SPH_PRIM,  primtype);	/* Tesselation Primitive Type */
    sphmode (SPH_DEPTH, depth);		/* Tesselation Subdivision Depth */

    /* Now create the sphere list object. */

    centerptr = center;
    radiusptr = radius;
    colorptr  = color;

    /* There are two object-creation paths possible, depending on whether we're
    ** doing GL spheres or GL hemispheres.  For GL spheres, the basic method is
    ** as follows:
    **
    **     [1]  Create object & close immediately (to allocate object ID).
    **     [2]  Generate sphere object (origin-centered, unit radius).
    **     [3]  Edit sphere-list object.
    **     [4]  Add sphere color, scaling & translation commands to list object.
    **     [5]  Add sphere object call.
    **     [6]  Close sphere-list object.
    **     [7]  If there are more spheres, go to step 2.
    **
    **  GL hemispheres are different in the fact that they are rendered with a
    **  fixed geometric representation, so they can't be rotated (you'll just
    **  see half-shells rotating around in space; they won't be oriented toward
    **  the viewer).  However, since hemispheres are not created as objects, we
    **  don't have to worry about nesting object creation, so the sphere-list
    **  creation is more straightforward:
    **
    **     [1]  Create sphere-list object.
    **     [2]  Set hemisphere color.
    **     [3]  Draw hemisphere.
    **     [4]  If there are more hemispheres, go to step 2.
    **     [5]  Close the sphere-list object.
    */

    makeobj (object_id);

    if (CURDEV->sphere_type == SPHTYPE_GLFULL)
    {   mmode (MVIEWING);
	closeobj ();
    }

    sphere_color[0] = DevStatus.diffusecolor[0];
    sphere_color[1] = DevStatus.diffusecolor[1];
    sphere_color[2] = DevStatus.diffusecolor[2];

    for (ii=0;  ii < numspheres;  ++ii)
    {
	static float parms[4] = { 0,0,0, 1 };    /* GL Sphere Parameters */

	parms[0] = *centerptr++;	/* Center X */
	parms[1] = *centerptr++;	/* Center Y */
	parms[2] = *centerptr++;	/* Center Z */

	if (radius)
	    parms[3] = *radiusptr++;	/* Radius   */

	if (color)
	{   sphere_color[0] = *colorptr++;
	    sphere_color[1] = *colorptr++;
	    sphere_color[2] = *colorptr++;
	}

	/* For hemispheres, just set the color and draw the hemisphere. */

	if (CURDEV->sphere_type == SPHTYPE_GLHEMI)
	{   ddr_iris_set_diffuse_color (sphere_color);
	    sphdraw (parms);
	}
	else
	{   auto Object newobj;		/* GL Object Identifier */

	    /* For full spheres, create sphere object, edit sphere-list,
	    ** transform sphere & set color, call sphere, and close sphere-list
	    ** editing.  */

	    sphobj (newobj = genobj());

	    editobj (object_id);
            {   pushmatrix();
		translate (parms[0], parms[1], parms[2]);
		scale (parms[3], parms[3], parms[3]);
		ddr_iris_set_diffuse_color (sphere_color);
		callobj (newobj);
		popmatrix ();
	    }
	    closeobj ();
	}
    }

    /* If we're doing hemispheres, then close the sphere-list object. */

    if (CURDEV->sphere_type == SPHTYPE_GLHEMI)
	closeobj ();

    /* Return the object identifier of the sphere-list object. */

    return object_id;
}
