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
**  This file contains generic routines associated with method and class
**  operations.
*****************************************************************************/

#include "../pex_driver.h"



    /*************************/
    /***  Data Structures  ***/
    /*************************/

ddt_pex_drom_class_data  dde_pex_drom_class_data[] =
{
    {   "DoPointList", -1,
	(DtMethodPtr) ddr_pex_drom_pointlist_create,
	(DtMethodPtr) ddr_pex_drom_pointlist_print,
	(DtMethodPtr) ddr_pex_drom_pointlist_delete,
	-1
    },
    {   "DoVarPointList", -1,
	(DtMethodPtr) ddr_pex_drom_pointlist_create,
	(DtMethodPtr) ddr_pex_drom_pointlist_print,
	(DtMethodPtr) ddr_pex_drom_pointlist_delete,
	-1
    },
    {   "DoLineList", -1,
	(DtMethodPtr) ddr_pex_drom_lines_create,
	(DtMethodPtr) ddr_pex_drom_lines_print,
	(DtMethodPtr) ddr_pex_drom_lines_delete,
	-1
    },
    {   "DoVarLineList", -1,
	(DtMethodPtr) ddr_pex_drom_lines_create,
	(DtMethodPtr) ddr_pex_drom_lines_print,
	(DtMethodPtr) ddr_pex_drom_lines_delete,
	-1
    },
    {   "DoPolyline", -1,
	(DtMethodPtr) ddr_pex_drom_lines_create,
	(DtMethodPtr) ddr_pex_drom_lines_print,
	(DtMethodPtr) ddr_pex_drom_lines_delete,
	-1
    },
    {   "DoTriangleList", -1,
	(DtMethodPtr) ddr_pex_drom_triangles_create,
	(DtMethodPtr) ddr_pex_drom_triangles_print,
	(DtMethodPtr) ddr_pex_drom_triangles_delete,
	-1
    },
    {   "DoTriangleMesh", -1,
	(DtMethodPtr) ddr_pex_drom_triangles_create,
	(DtMethodPtr) ddr_pex_drom_triangles_print,
	(DtMethodPtr) ddr_pex_drom_triangles_delete,
	-1
    },
    {   "DoVarTriangleMesh", -1,
	(DtMethodPtr) ddr_pex_drom_triangles_create,
	(DtMethodPtr) ddr_pex_drom_triangles_print,
	(DtMethodPtr) ddr_pex_drom_triangles_delete,
	-1
    },
    {   "DoTriangleStrip", -1,
	(DtMethodPtr) ddr_pex_drom_triangles_create,
	(DtMethodPtr) ddr_pex_drom_triangles_print,
	(DtMethodPtr) ddr_pex_drom_triangles_delete,
	-1
    },
    {   "DoVarTriangleStrip", -1,
	(DtMethodPtr) ddr_pex_drom_triangles_create,
	(DtMethodPtr) ddr_pex_drom_triangles_print,
	(DtMethodPtr) ddr_pex_drom_triangles_delete,
	-1
    },
    { 0, -1, 0,0,0, -1 }
};

	/* The following renderer's class methods are overridden here. */

static DDt_DROMMethod dde_pex_DROM_methods[] =
{
    { "DoPointList",       (DtMethodPtr) ddr_pex_drom_pointlist_render    },
    { "DoVarPointList",    (DtMethodPtr) ddr_pex_drom_varpointlist_render },
    { "DoLineList",        (DtMethodPtr) ddr_pex_drom_linelist_render     },
    { "DoVarLineList",     (DtMethodPtr) ddr_pex_drom_varlinelist_render  },
    { "DoPolyline",        (DtMethodPtr) ddr_pex_drom_polyline_render     },
    { "DoTriangleList",    (DtMethodPtr) ddr_pex_drom_trilist_render      },
    { "DoTriangleMesh",    (DtMethodPtr) ddr_pex_drom_trimesh_render      },
    { "DoVarTriangleMesh", (DtMethodPtr) ddr_pex_drom_vartrimesh_render   },
    { "DoTriangleStrip",   (DtMethodPtr) ddr_pex_drom_tristrip_render     },
    { "DoVarTriangleStrip",(DtMethodPtr) ddr_pex_drom_vartristrip_render  },
    { 0,                   (DtMethodPtr) 0                                }
};



/*****************************************************************************
**  This routine adds the class data to the renderer for primitives that will
**  be rendered directly.
*****************************************************************************/

void ddr_pex_drom_new_class (
    char  *class_name,	/* Name of the Object Class */
    DtInt  class_id)	/* Class Identifier */
{
    register DtInt ii;	/* Class Data Index */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_new_class (\"%s\", %d)\n", class_name, class_id);
	fflush (stdout);
#   endif

    for (ii=0;  dde_pex_drom_class_data[ii].name;  ++ii)
    {
        if (0 != strcmp (dde_pex_drom_class_data[ii].name, class_name))
            continue;

        dde_pex_drom_class_data[ii].id = class_id;
        dde_pex_drom_class_data[ii].index =
            DDclass_AddObjectData
            (   class_id,
                dde_pex_drom_class_data[ii].routine_create,
                dde_pex_drom_class_data[ii].routine_print,
                dde_pex_drom_class_data[ii].routine_delete
            );
    }
}



/*****************************************************************************
**  This routine returns a pointer to the named DROM method routines
**  structure.
*****************************************************************************/

DtPtr ddr_pex_return_DROM_methods (void)
{
#   if DEBUG_CALL
	print ("ddr_pex_return_DROM_methods ()\n");
	fflush (stdout);
#   endif

    return (DtPtr) dde_pex_DROM_methods;
}



/*****************************************************************************
**  This routine gets the class ID and class additional-data index if either
**  of these is not yet valid.  If the class is not currently renderable,
**  the function returns 0.  If the function is unable to find a valid
**  additional-data index, it returns 0.  On successful completion, the
**  classid is set to the object class ID, the dataindex is set to the
**  additional-data index, and the function returns 1.
*****************************************************************************/

DtInt GetClassInfo (
    DtInt *classid,	/* Object Class Method Index */
    char  *classname,	/* Object Class Name */
    DtInt *dataindex)	/* Object Class Additional Data Index */
{
    register int ii;	/* Loop Index */

#   if DEBUG_CALL
	printf ("GetClassInfo (0x%lx, \"%s\", 0x%lx)\n",
	    classid, classname, dataindex);
	fflush (stdout);
#   endif

    /* If we don't know what the class ID for the object is yet, get it. */

    if (*classid == -1)
	*classid = DsInqClassId (classname);

    /* Determine if the object class is currently renderable. */

    if (!DDcondex_QueryRender (*classid))
	return 0;

    /* Get the additional_data field index for the object.  If the data index
    ** is not -1, then we've already found it.  */

    if (*dataindex == -1)
    {   for (ii=0;  dde_pex_drom_class_data[ii].name;  ++ii)
	{   if (dde_pex_drom_class_data[ii].id == *classid)
	    {   *dataindex = dde_pex_drom_class_data[ii].index;
		break;
	    }
	}
    }

    return (*dataindex != -1);
}
