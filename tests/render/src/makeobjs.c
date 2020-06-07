/*************************************************************************
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

/*************************************************************************
**    This file contains object creation callback routines for the render
**  program.  In most cases, the user is prompted for parameters for each
**  object, which may include, tesselation, vertex types, texture maps,
**  and possibly surface smoothing.
*************************************************************************/
 
#include <stdio.h>
#include "dore.h"
#include <math.h>


   /***************************************/
   /***  Program Parameters & Constants ***/
   /***************************************/

#define TEX2D   (1<<0)		/* Include 2D Texture */
#define TEX3D   (1<<1)		/* Include 3D Texture */


   /***************************/
   /***  Macro Definitions  ***/
   /***************************/

#define print(s)   fputs(s,stdout)

      /* This macro returns the Z value of a particular functional
      // surface when given the X and Y coordinates.  wave_surf_dx
      // and wave_surf_dy are the partial derivatives of this function
      // with respect to x and y, and are used to find the true normal
      // vector to the surface Z = wave_surf(X,Y).  
      //
      // NOTE: Normals determined using these macros must be normalised
      // before they are passed to Dore. */

#define HEIGHT  0.25		/* Height of the Function Field */

#define wave_surf(x,y) \
   (HEIGHT * sin (M_PI * (2.5*(x) + (y) + 1)))

#define wave_surf_dx(x,y) \
   (HEIGHT * 2.5 * M_PI * cos (M_PI * (2.5*(x) + (y) + 1)))

#define wave_surf_dy(x,y) \
   (HEIGHT * M_PI * cos (M_PI * (2.5*(x) + (y) + 1)))

#define ADDVDATA(ptr,a,b,c) \
(  *ptr++ = (a), \
   *ptr++ = (b), \
   *ptr++ = (c)  \
)


   /**************************************/
   /***  Type & Structure Definitions  ***/
   /**************************************/

typedef struct {
   DtPtr      name;
   DtObject   object;
   DtObject (*create_rtn)();
} CallbackData;

typedef CallbackData  *CallbackDataPtr;


   /*******************************/
   /***  Function Declarations  ***/
   /*******************************/

extern  DtPtr  malloc ARGS((unsigned));

DtObject make_triangle_strip      ARGS(());
int      updvarlinlst_callback    ARGS(());
int      updvarpntlst_callback    ARGS(());
int      updvarsimplymsh_callback ARGS(());
int      updvartrimsh_callback    ARGS(());
int      updvartristrip_callback  ARGS(());
int      updvarquadgrid_callback  ARGS(());

int      execute_callback_geom   ARGS((CallbackDataPtr));
DtObject make_triangle_mesh      ARGS(());
DtObject make_triangle_list      ARGS(());
DtObject make_cline_list         ARGS(());
DtObject make_line_list          ARGS(());
DtObject make_point_list         ARGS(());
DtObject make_quad_list          ARGS(());
DtObject make_quad_mesh          ARGS(());
DtObject make_quad_grid          ARGS(());
DtObject make_simply             ARGS(());
DtObject make_simplymsh          ARGS(());
DtObject make_ply                ARGS(());
DtObject make_plymsh             ARGS(());
DtObject make_uvtext             ARGS(());
DtObject make_plymkr             ARGS(());
DtObject make_patch              ARGS(());
DtObject make_bvoltest           ARGS(());
DtObject make_vartrimsh          ARGS(());
DtObject make_varlinlst          ARGS(());
DtObject make_varpntlst          ARGS(());
DtObject make_varsimplymsh       ARGS(());
DtObject make_vartristrip        ARGS(());
DtObject make_varquadgrid        ARGS(());
void     Prompt                  ARGS((char*, DtInt, DtInt*));
DtPtr    space_allocate          ARGS((DtInt));
void     space_deallocate        ARGS((DtPtr));
void     normalize_vector        ARGS((DtReal, DtReal, DtReal, DtRealTriple));
DtObject obsolete_callback_name();

   /* These function declarations cannot be prototyped, since internally
   // Dore' has not declared the callback function using prototypes.
   // Therefore, real numbers will promoted to doubles while the callback
   // function expects floats on the parameter stack.  */

int uv_callback1  (Dt32Bits, DtReal, DtReal, DtReal, DtReal, DtReal,
		   DtReal, DtReal*, DtReal*);
int uv_callback2  (Dt32Bits, DtReal, DtReal, DtReal, DtReal, DtReal,
		   DtReal, DtReal*, DtReal*);
int uvw_callback1 (Dt32Bits, DtReal, DtReal, DtReal, DtReal, DtReal,
		   DtReal, DtReal*, DtReal*, DtReal*);
int uvw_callback2 (Dt32Bits, DtReal, DtReal, DtReal, DtReal, DtReal,
		   DtReal, DtReal*, DtReal*, DtReal*);


   /**************************************/
   /***  Global Variable Declarations  ***/
   /**************************************/

DtObject vartristrip;

extern DtFlag   updvar1;
extern DtFlag   updvar2;

       DtObject varlinlst;
       DtObject vartrimsh;
       DtObject varpntlst;
       DtObject varsimplymsh;
       DtObject varquadgrid;

static DtInt    object_count = -1;
static DtFlag   varlock = DcFalse;	/* Variable Geometry Lock */
static DtReal	vertloc[2700], vertclr[2700];
static DtReal 	*locdata, *clrdata, *nrmdata; /* variable data arrays */
static DtInt	vgridcount = 0;		/* variable geom gridcount */
static DtReal   vertuv  [1800];
static DtReal   vertuvw [2700];
static DtInt    texflags[] = { 0, TEX2D|TEX3D, TEX2D, TEX3D };

CallbackData routines[] = {
   { "tristrip",     DcNullPtr, make_triangle_strip },
   { "vtrimsh",      DcNullPtr, obsolete_callback_name },
   { "vtrilst",      DcNullPtr, obsolete_callback_name },
   { "vpollin",      DcNullPtr, obsolete_callback_name },
   { "vlinlst",      DcNullPtr, obsolete_callback_name },
   { "vpntlst",      DcNullPtr, obsolete_callback_name },

   { "trimsh",      DcNullPtr, make_triangle_mesh },
   { "trilst",      DcNullPtr, make_triangle_list },
   { "pollin",      DcNullPtr, make_cline_list },
   { "linlst",      DcNullPtr, make_line_list },
   { "pntlst",      DcNullPtr, make_point_list },

   { "simply",       DcNullPtr, make_simply },
   { "simplymsh",    DcNullPtr, make_simplymsh },
   { "ply",          DcNullPtr, make_ply },
   { "plymsh",       DcNullPtr, make_plymsh },
   { "uvtext",       DcNullPtr, make_uvtext },
   { "plymkr",       DcNullPtr, make_plymkr },
   { "patch",        DcNullPtr, make_patch },
   { "bvoltest",     DcNullPtr, make_bvoltest },
   { "vartrimsh",    DcNullPtr, make_vartrimsh },
   { "varlinlst",    DcNullPtr, make_varlinlst },
   { "varpntlst",    DcNullPtr, make_varpntlst },
   { "varsimplymsh", DcNullPtr, make_varsimplymsh },
   { "vartristrip",  DcNullPtr, make_vartristrip },
   { "varquadgrid",  DcNullPtr, make_varquadgrid },
   { "quadlst",      DcNullPtr, make_quad_list },
   { "quadmsh",      DcNullPtr, make_quad_mesh },
   { "quadgrid",     DcNullPtr, make_quad_grid },
   { 0,              DcNullPtr, 0}
};



/*==========================================================================
==
==========================================================================*/

int execute_callback_geom (
    CallbackDataPtr data)
{
   auto int ii;

   if (object_count == -1) {
      Prompt ("Number of times to execute object [1]", 1, &object_count);
      object_count = (object_count < 1) ? 1 : object_count;
   }

   if (data->object == DcNullObject) {
      data->object = DsHoldObj ((*(data->create_rtn))());
   }

   for (ii=0;  ii < object_count;  ++ii) {
      DsExecuteObj (data->object);
   }

   return 0;
}



/*==========================================================================
==
==========================================================================*/

DtObject make_bvoltest (void)
{
   static DtVolume bndvol = {{-1.,-1.,-1.},{1.,1.,1.,}};
   DtObject boxobj;

   boxobj = DoGroup (DcTrue);
        DgAddObj (DoPrimSurf(DcBox));
   DgClose();

   DoGroup (DcTrue);
      DgAddObj (DoMinBoundingVolExt (50.0));
      DgAddObj (DoBoundingVol (&bndvol, boxobj));
      DgAddObj (DoPrimSurf (DcSphere));
   return DgClose();
}



/*==========================================================================
==  This routine generates a connected line list.  The line list is a wire
==  mesh of the good ol' wave surface.
==========================================================================*/
 
DtObject make_cline_list (void)
{
   auto DtReal        color[3];
   auto DtInt         i;
   auto DtInt         ele_per_vtx;
   auto DtInt         gridcount;
   auto DtReal        gridincr;
   auto DtReal        loc[3];
   auto DtRealTriple  nvector;
   auto DtInt         xgridloc, ygridloc;
   auto DtReal       *vertexdata;
   auto DtReal       *vertexptr;
   auto DtVertexType  vtype;		/* Vertex Information Type */

   print ("\nCLine List --\n");

   Prompt ("Gridcount [10]", 10, &gridcount);
   if (gridcount < 1) return DcNullObject;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &i);
   switch (i) {
      default:
      case 0:  vtype = DcLoc;        ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;     ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;     ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr;  ele_per_vtx = 9;  break;
   }

   if (gridcount > 1) {
      gridincr = 2. / (DtReal)(gridcount-1);

      vertexdata = (DtReal*) space_allocate ((DtInt)(gridcount * gridcount
		                            * ele_per_vtx * sizeof(DtReal)));
   } else {
      vertexdata = (DtReal*) space_allocate ((DtInt)(2 * ele_per_vtx
		                            * sizeof(DtReal)));
   }

   /* Iterate over the wave surface's grid intersection points and
   // generate the line endpoints that mesh the surface. */

   vertexptr = vertexdata;

   for (xgridloc=0;  xgridloc < gridcount;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < gridcount;  ++ygridloc) {
	 loc[0] = -1 + ((DtReal)xgridloc*gridincr);
	 loc[1] = -1 + ((DtReal)ygridloc*gridincr);
	 loc[2] = 2.0 * wave_surf (loc[0], loc[1]);

	 color[0] = (1.0 + loc[0]) * 0.5;
	 color[1] = (1.0 + loc[1]) * 0.5;
	 color[2] = (1.0 + loc[2]) * 0.5;

	 ADDVDATA (vertexptr, loc[0], loc[1], loc[2]);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	    normalize_vector(-wave_surf_dx(loc[0],loc[1]),
                                 -wave_surf_dy(loc[0],loc[1]), 1, nvector);
	    ADDVDATA (vertexptr, nvector[0], nvector[1], nvector[2]);
	 }
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr))
	    ADDVDATA (vertexptr, color[0], color[1], color[2]);
      }
   }

   /* If the user specified a gridcount of only 1, then just generate a
   // single line segment. */

   if (gridcount == 1) {
      ADDVDATA (vertexptr, 0,0,0);
      if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr))
	 ADDVDATA (vertexptr, 0,0,1);
      if ((vtype == DcLocClr) || (vtype == DcLocNrmClr))
	 ADDVDATA (vertexptr, 0.5, 0.5, 0.5);

      ADDVDATA (vertexptr, 1,0,0);
      if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr))
	 ADDVDATA (vertexptr, 0,0,1);
      if ((vtype == DcLocClr) || (vtype == DcLocNrmClr))
	 ADDVDATA (vertexptr, 1.0, 0.5, 0.5);
   }

   return DoPolyline (DcRGB, vtype, gridcount*gridcount, vertexdata);
}



/*==========================================================================
==  This function generates a list of independent lines.  The generated list
==  meshes the surface of the wave function defined by the wave_surf macro.
==========================================================================*/

DtObject make_line_list (void)
{
   auto DtVertexType  vtype;
   auto DtInt         i;
   auto DtInt         ele_per_vtx;
   auto DtInt         gridcount;
   auto DtReal        gridincr;
   auto DtInt         xgridloc, ygridloc;
   auto DtReal        xgridval1, ygridval1, zgridval1;
   auto DtReal        xgridval2, ygridval2, zgridval2;
   auto DtReal       *vertexdata;
   auto DtReal       *vertexptr;
   auto DtRealTriple nvector;

   print ("\nLine List --\n");

   Prompt ("Gridcount [10]", 10, &gridcount);
   if (gridcount < 1) return DcNullObject;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &i);
   switch (i) {
      default:
      case 0:  vtype = DcLoc;        ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;     ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;     ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr;  ele_per_vtx = 9;  break;
   }

   if (gridcount > 1)
	 gridincr = 2. / (DtReal)(gridcount-1);

   vertexdata = (DtReal*) space_allocate ((DtInt)(2 * gridcount * gridcount
		                         * ele_per_vtx * sizeof(DtReal)));

   vertexptr = vertexdata;

   /* Iterate over the surface of the function and generate mesh segments.
   // Note that this loop fails immediately if gridcount is one; this
   // condition is handled below the loop. */

   for (xgridloc=0;  xgridloc < gridcount-1;  ++xgridloc) {
      xgridval1 = -1 + ((DtReal)xgridloc*gridincr);
      xgridval2 = -1 + ((DtReal)(xgridloc+1)*gridincr);
      for (ygridloc=0;  ygridloc < gridcount;  ++ygridloc) {
	 ygridval1 = -1 + ((DtReal)ygridloc*gridincr);
	 zgridval1 = wave_surf (xgridval1, ygridval1);

	 ygridval2 = -1. + ((DtReal)ygridloc*gridincr);
	 zgridval2 = wave_surf (xgridval2, ygridval2);

	 /* vertex 1 */

	 ADDVDATA (vertexptr, xgridval1, ygridval1, zgridval1);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	    normalize_vector(-wave_surf_dx(xgridval1,ygridval1),
                                 -wave_surf_dy(xgridval1,ygridval1), 1, nvector);
	    ADDVDATA (vertexptr, nvector[0], nvector[1], nvector[2]);
	 }
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    *vertexptr++ = (1.0 + xgridval1) * 0.5;
	    *vertexptr++ = (1.0 + ygridval1) * 0.5;
	    *vertexptr++ = (1.0 + zgridval1) * 0.5;
	 }

	 /* vertex 2 */

	 ADDVDATA (vertexptr, xgridval2, ygridval2, zgridval2);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	    normalize_vector(-wave_surf_dx(xgridval2,ygridval2),
				 -wave_surf_dy(xgridval2,ygridval2), 1, nvector);
	    ADDVDATA (vertexptr, nvector[0], nvector[1], nvector[2]);
	 }
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    *vertexptr++ = (1.0 + xgridval2) * 0.5;
	    *vertexptr++ = (1.0 + ygridval2) * 0.5;
	    *vertexptr++ = (1.0 + zgridval2) * 0.5;
	 }
      }
   }

   /* If the user specified a gridcount of only one, then just generate
   // a single line segment. */

   if (gridcount == 1) {
      static DtReal loc[2][3] =		/* Gridcount 1 Vertex Locations */
      {  {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0} };

      vertexptr = vertexdata;

      for (i=0;  i < 2;  ++i)
      {  ADDVDATA (vertexptr, loc[i][0], loc[i][1], loc[i][2]);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr))
	    ADDVDATA (vertexptr, 0,0,1);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	    *vertexptr++ = (1.0 + loc[i][0]) * 0.5;
	    *vertexptr++ = (1.0 + loc[i][1]) * 0.5;
	    *vertexptr++ = (1.0 + loc[i][2]) * 0.5;
	 }
      }
   }

   if (gridcount == 1) {
      return DoLineList (DcRGB, vtype, 1, vertexdata);
   } else {
      return DoLineList (DcRGB, vtype, (gridcount-1)*gridcount, vertexdata);
   }
}



/*==========================================================================
==
==========================================================================*/

DtObject make_patch (void)
{
   extern DtObject DcBezier4;
   extern DtObject DcHermite4;

   static DtReal clrctlpts[] =
   {  1.0, 0.5, 0.0,   1,1,1,
      1.5,-0.5,-0.5,   1,1,1,
      1.5, 0.0,-1.0,   1,1,1,
      2.5, 0.0, 0.0,   1,1,1,
      1.5,-0.5, 0.5,   1,0,0,
      2.0, 0.5, 0.0,   1,0,0,
      2.5, 0.0, 0.0,   1,0,0,
      1.5, 0.0,-1.0,   1,0,0,
      1.5, 0.0, 1.0,   1,0,0,
      2.5, 0.0, 0.0,   1,0,0,
      1.5, 0.0, 0.0,   1,0,0,
      1.5, 0.0, 0.0,   1,0,0,
      2.5, 0.0, 0.0,   0,0,1,
      1.5, 0.0, 1.0,   0,0,1,
      1.5, 0.0, 0.0,   0,0,1,
      1.5, 0.0, 0.0,   0,0,1
   };

   static DtReal ctlpts[] =
   { -2.0, 0.5, 1.0,
     -1.5,-0.5, 0.5,
     -1.5, 0.0, 0.0,
     -0.5, 0.0, 1.0,

     -1.5,-0.5, 1.5,
     -1.0, 0.5, 1.0,
     -0.5, 0.0, 1.0,
     -1.5, 0.0, 0.0,

     -1.5, 0.0, 2.0,
     -0.5, 0.0, 1.0,
     -1.5, 0.0, 1.0,
     -1.5, 0.0, 1.0,

     -0.5, 0.0, 1.0,
     -1.5, 0.0, 2.0,
     -1.5, 0.0, 1.0,
     -1.5, 0.0, 1.0
   };



   DoGroup(DcTrue);
	DgAddObj(DoPatch(DcRGB, DcLocClr, DcHermite4, clrctlpts, DcHermite4));
        DgAddObj(DoPatch(DcRGB, DcLoc, DcHermite4, ctlpts, DcHermite4));
   return DgClose();
}



/*==========================================================================
==  This routine generates a multi-contour polygon.  The actual generated
==  polygon, however only has a single contour.
==========================================================================*/

DtObject make_ply (void)
{
   static DtReal        color[10][3] =	/* Vertex Colors */
   {  {1,0,0}, {0,0,1}, {1,0,0}, {0,0,1}, {1,0,0},
      {0,0,1}, {1,0,0}, {0,0,1}, {1,0,0}, {0,0,1}
   };
   auto   DtInt         contours[1];	/* Vertices per Contour */
   auto   DtInt         ele_per_vtx;	/* Data Values Per Vertex */
   auto   DtInt         ii;		/* Scratch Integer */
   static DtReal        loc[10][3] =	/* Vertex Locations */
   {  { 0, 0, 0}, { 3, 3, 0}, {-3, 3, 0}, { 2,-4, 0}, { 0, 2, 0},
      { 0, 6, 0}, { 3, 0, 0}, { 3, 6, 0}, { 6, 6, 0}, { 6, 0, 0}
   };
   auto   DtInt         texture;	/* Texture Type (none/both/2D/3D) */
   auto   DtReal       *vertexdata;	/* Vertex Data Array */
   auto   DtReal       *vertexptr;	/* Vertex Array Traversal Pointer */
   auto   DtVertexType  vtype;		/* Vertex Type */

   contours[0] = 10;

   print ("\nPolygon --\n");

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;       ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;    ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;    ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr; ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2;
   if (texture & TEX3D)  ele_per_vtx += 3;

   vertexdata = (DtReal*) space_allocate ((DtInt)
					  (10 * ele_per_vtx * sizeof(DtReal)));
   vertexptr  = vertexdata;

   /* Cycle through each vertex and add data as needed. */

   for (ii=0;  ii < 10;  ++ii)
   {
      ADDVDATA (vertexptr, loc[ii][0], loc[ii][1], loc[ii][2]);
      if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr))
	 ADDVDATA (vertexptr, 0,0,1);
      if ((vtype == DcLocClr) || (vtype == DcLocNrmClr))
	 ADDVDATA (vertexptr, color[ii][0], color[ii][1], color[ii][2]);
      if (texture & TEX2D) {
	 *vertexptr++ =     (loc[ii][0] + 4) / 10;
	 *vertexptr++ = 1 - (loc[ii][1] + 4) / 10;
      }
      if (texture & TEX3D)
	 ADDVDATA (vertexptr,
	        (loc[ii][0] + 4) / 10,
	    1 - (loc[ii][1] + 4) / 10,
	    0.5);
   }

   if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
   if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);

   return DoPolygon (DcRGB, vtype, 1, contours, vertexdata, DcComplex);
}



/*==========================================================================
==
==========================================================================*/

DtObject make_plymkr (void)
{
   static DtReal locs1[][3] = {   { -2.0, -2.0, -2.0 },
				  {  0.0, -2.0, -2.0 },
				  {  2.0, -2.0, -2.0 },
				  {  2.0,  2.0, -2.0 },
				  { -2.0,  2.0, -2.0 }  };

   static DtReal locs2[][3] = {   { -4.0, -4.0, -4.0 },
				  {  0.0, -4.0, -4.0 },
				  {  4.0, -4.0, -4.0 },
				  {  4.0,  4.0, -4.0 },
			  	  { -4.0,  4.0, -4.0 } };

   DoGroup(DcTrue);

   	DgAddObj (DoMarkerGlyph((DtInt)'*'));
   	DgAddObj (DoPolymarker(5,locs1));

   	DgAddObj (DoMarkerGlyph((DtInt)'&'));
   	DgAddObj (DoPolymarker(5,locs2));

   return DgClose();
}



/*==========================================================================
==  This function generates a multi-contour-polygon mesh.  The object
==  generated is a box with square holes in each face.
==========================================================================*/

DtObject make_plymsh (void)
{
   static DtInt         contours[] =	/* Vertices Per Contour */
   { 4,4,4,4,4,4,4,4,4,4,4,4 };
   auto   DtInt         ele_per_vtx;	/* Data Values Per Vertex */
   auto   DtInt         ii;		/* Scratch Integer */
   static DtReal        loc[32][3] =	/* Vertex Locations */
   {  /* Corners */
      {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0},
      {0.0, 1.0,-1.0}, {0.0, 0.0,-1.0}, {1.0, 0.0,-1.0}, {1.0, 1.0,-1.0},
      /* Holes (front, left, back, right, bottom, top) */
      {0.8, 0.2, 0.0}, {0.8, 0.8, 0.0}, {0.2, 0.8, 0.0}, {0.2, 0.2, 0.0},
      {0.0, 0.2,-0.2}, {0.0, 0.8,-0.2}, {0.0, 0.8,-0.8}, {0.0, 0.2,-0.8},
      {0.2, 0.2,-1.0}, {0.2, 0.8,-1.0}, {0.8, 0.8,-1.0}, {0.8, 0.2,-1.0},
      {1.0, 0.2,-0.8}, {1.0, 0.8,-0.8}, {1.0, 0.8,-0.2}, {1.0, 0.2,-0.2},
      {0.2, 0.0,-0.2}, {0.2, 0.0,-0.8}, {0.8, 0.0,-0.8}, {0.8, 0.0,-0.2},
      {0.8, 1.0,-0.2}, {0.8, 1.0,-0.8}, {0.2, 1.0,-0.8}, {0.2, 1.0,-0.2}
   };
   static DtInt         polygons[] =	/* Contours Per Polygon */
   { 2,2,2,2,2,2 };
   auto   DtFlag        smooth;		/* Normal Smooth Flag */
   auto   DtInt         texture;	/* Texture Type (none/both/2D/3D) */
   auto   DtReal       *vertexdata;	/* Vertex Data Array */
   auto   DtReal       *vertexptr;	/* Vertex Array Traversal Pointer */
   static DtInt         vlist[] =	/* Contour List */
   {  0,1,2,3,  8, 9,10,11,	/* front */
      3,2,4,5, 12,13,14,15,	/* left */
      5,4,7,6, 16,17,18,19,	/* back */
      6,7,1,0, 20,21,22,23,	/* right */
      0,3,5,6, 24,25,26,27,	/* bottom */
      1,7,4,2, 28,29,30,31 	/* top */
   };
   auto   DtVertexType  vtype;		/* Vertex Type */

   print ("\nPolygon Mesh --\n");

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;       ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;    ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;    ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr; ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2;
   if (texture & TEX3D)  ele_per_vtx += 3;

   Prompt ("Smooth surface? (0=no, 1=yes)", 0, &ii);
   smooth = (ii != 0);

   vertexdata = (DtReal*) space_allocate ((DtInt)
					  (32 * ele_per_vtx * sizeof(DtReal)));
   vertexptr  = vertexdata;

   /* Iterate over each vertex and add vertex data according to the vertex
   // type and the type of texture mapping to be applied. */

   for (ii=0;  ii < 32;  ++ii)
   {
      ADDVDATA (vertexptr, loc[ii][0], loc[ii][1], loc[ii][2]);
      if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	 auto  DtReal  norm;	/* Vector Norm */
	 auto  DtReal  Nx, Nz;	/* Normal Components */

	 /* Each vertex is given a cylinder-like normal vector, relative
	 // to the Y axis. */

	 Nx = 2.0 * loc[ii][0] - 1.0;
	 Nz = 2.0 * loc[ii][2] + 1.0;
	 norm = sqrt ((double)(Nx*Nx + Nz*Nz));
	 ADDVDATA (vertexptr, Nx/norm, 0.0, Nx/norm);
      }
      if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	 if (loc[ii][1] > 0.5)
	    ADDVDATA (vertexptr, 0.0314, 0.4000, 0.1333);
	 else
	    ADDVDATA (vertexptr, 0.7333, 0.0667, 0.4000);
      }
      if (texture & TEX2D) {
	 *vertexptr++ =     loc[ii][0];
	 *vertexptr++ = 1 - loc[ii][1];
      }
      if (texture & TEX3D)
	 ADDVDATA (vertexptr, loc[ii][0], 1-loc[ii][1], -loc[ii][2]);
   }

   if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
   if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);

   return DoPolygonMesh (DcRGB, vtype, 32, vertexdata, 6, polygons,
			 contours, vlist, DcConcave, smooth);
}



/*==========================================================================
==
==========================================================================*/

DtObject make_point_list (void)
{
   DtVertexType vtype;
   DtInt i;
   DtInt ele_per_vtx;
   DtInt gridcount;
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal *vertexdata;
   DtReal *vertexptr;
   auto DtRealTriple nvector;

   print ("\nPoint List --\n");

   Prompt ("Gridcount [10]", 10, &gridcount);
   if (gridcount < 1) return DcNullObject;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &i);
   switch (i) {
      default:
      case 0:  vtype = DcLoc;        ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;     ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;     ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr;  ele_per_vtx = 9;  break;
   }

   if (gridcount > 1)
	 gridincr = 2. / (DtReal)(gridcount-1);

   vertexdata = (DtReal*) space_allocate ((DtInt)(gridcount * gridcount
	                                * ele_per_vtx * sizeof(DtReal)));

   vertexptr = vertexdata;

   for (xgridloc=0;xgridloc<gridcount;xgridloc++) {
      for (ygridloc=0;ygridloc<gridcount;ygridloc++) {
	 auto DtReal  Lx, Ly, Lz;	/* Location Values */

	 Lx = -1 + ((DtReal)xgridloc*gridincr);
	 Ly = -1 + ((DtReal)ygridloc*gridincr);
	 Lz = wave_surf (Lx, Ly);

	 ADDVDATA (vertexptr, Lx, Ly, Lz);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	    normalize_vector(-wave_surf_dx(Lx,Ly),-wave_surf_dy(Lx,Ly),1,nvector);
	    ADDVDATA (vertexptr, nvector[0], nvector[1], nvector[2]);
	 }
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    *vertexptr++ = (1 + Lx) * 0.5;
	    *vertexptr++ = (1 + Ly) * 0.5;
	    *vertexptr++ = (1 + Lz) * 0.5;
	 }
      }
   }

   return DoPointList (DcRGB, vtype, gridcount*gridcount, vertexdata);
}



/*==========================================================================
==  This routine generates a quadrilateral list.  The generated surface is
==  a function of X and Y and ``waves'' in the Z direction such that the
==  quadrilaterals are not twisted.
==========================================================================*/

DtObject make_quad_list (void)
{
   auto   DtInt         ele_per_vtx;	/* # Data Values Per Vertex */
   auto   DtInt         gridcount;	/* # Grid Lines on Surface */
   auto   DtInt         ii;	        /* Scratch Integer */
   auto   DtReal        rclrval[4],	/* Grid Intersection Color */
			gclrval[4],
			bclrval[4];
   auto   DtInt         texture;	/* Texture Type (none/both/2D/3D) */
   auto   DtReal       *vertexdata;	/* Vertex Data Array */
   auto   DtReal       *vertexptr;	/* Vertex Array Traversal Pointer */
   static DtInt         vtx[6]={0,2,1,2,3,1};
   auto   DtVertexType  vtype;		/* Vertex Type */
   auto   DtInt         xgridloc;	/* Grid X Line Number */

   print ("\nQuad List --\n");

   Prompt ("Gridcount [10]", 10, &gridcount);
   if (gridcount < 1) return DcNullObject;
   if (gridcount < 2) gridcount = 2;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;        ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;     ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;     ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr;  ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2;
   if (texture & TEX3D)  ele_per_vtx += 3;

   vertexdata = (DtReal*) space_allocate ((DtInt)(3*2 * (gridcount-1)
			       * (gridcount-1) * ele_per_vtx * sizeof(DtReal)));

   /* Generate the quadrilateral patches over the ruled surface.  Note that
   // we're doing this surface because each quadrilateral must be planar.
   // Each quadrilateral patch looks like this:
   //
   //  ^  2----------------------------1     X       There are gridcount-1
   //  |  |       quadrilateral        |     |       patches across the
   //  u  3----------------------------0  Y--Z       ruled surface.  u
   //     v=1                        v=0             increases by gridline.
   //
   //  If vertex nomals are included, then they are set to unit vectors
   //  that are exactly perpendicular to the wave function.
   */

   vertexptr = vertexdata;

   for (xgridloc=0;  xgridloc < gridcount-1;  ++xgridloc) {
      auto DtReal gridloc[4][3];	/* Grid Intersection Position */
      auto DtReal griduv[4][2];		/* Surface UV Parameters */

      griduv[0][0] = griduv[3][0] = (DtReal)(xgridloc  )/(DtReal)(gridcount-1);
      griduv[1][0] = griduv[2][0] = (DtReal)(xgridloc+1)/(DtReal)(gridcount-1);
      griduv[0][1] = griduv[1][1] =  0;
      griduv[2][1] = griduv[3][1] =  1;

      gridloc[0][0] = gridloc[3][0] = (2 * griduv[0][0]) - 1;
      gridloc[1][0] = gridloc[2][0] = (2 * griduv[1][0]) - 1;
      gridloc[0][1] = gridloc[1][1] = -1;
      gridloc[2][1] = gridloc[3][1] =  1;
      gridloc[0][2] = gridloc[3][2]
		    = 0.25 * sin (griduv[0][0] * 5 * M_PI);
      gridloc[1][2] = gridloc[2][2]
		    = 0.25 * sin (griduv[1][0] * 5 * M_PI);

      for (ii=0;  ii < 4;  ++ii) {
	 ADDVDATA (vertexptr, gridloc[ii][0],gridloc[ii][1],gridloc[ii][2]);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {

	    auto DtReal norm;		/* Normal Vector Magnitude */
	    auto DtReal slope;		/* XZ Gradiant of the Wave Surface */

	    /* Find normal to the wave curve at the vertex.  Note:
	    //
	    //     S (x,y) = <x, y, F(x)>            Surface Function
	    //     F (x)   = 0.25 sin (5 pi x)       Height Function
	    //     F'(x)   = 1.25 pi cos (5 pi x)
	    //     N (x,y) = <-F'(x), 0, 1>          Normal Function
            */

	    slope = 1.25 * M_PI * cos (5 * M_PI * griduv[ii][0]);
	    norm = sqrt ((slope * slope) + 1);

	    ADDVDATA (vertexptr, - (slope / norm), 0, 1 / norm);
	 }
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    *vertexptr++ = griduv[ii][0];
	    *vertexptr++ = griduv[ii][1];
	    *vertexptr++ = (gridloc[ii][2] * 2.0) + 0.5;
	 }
	 if (texture & TEX2D)
	 {  *vertexptr++ = griduv[ii][0];
	    *vertexptr++ = 1 - griduv[ii][1];
	 }
	 if (texture & TEX3D)
	 {  *vertexptr++ = griduv[ii][0];
	    *vertexptr++ = 1 - griduv[ii][1];
	    *vertexptr++ = (gridloc[ii][2] * 2.0) + 0.5;
	 }
      }
   }

   if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
   if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);

   return DoQuadList (DcRGB, vtype, gridcount-1, vertexdata);
}



/*==========================================================================
==  This function generates a quad mesh that covers the surface of a wave
==  function.  The user is prompted for the number of grid lines, the line
==  type, patch smoothing, and the texture-mapping type.
==========================================================================*/

DtObject make_quad_mesh (void)
{
   auto DtInt         ii;		/* Scratch Integer */
   auto DtInt         ele_per_vtx;	/* # Parameters / Vertex */
   auto DtInt         gridcount;	/* Number of Grid Lines */
   auto DtReal        gridincr;		/* Increment Across Grid */
   auto DtInt         npatches;		/* Number of Quadrilateral Patches */
   auto DtInt         nverts;		/* Number of Vertices */
   auto DtInt        *quads;		/* Quad List */
   auto DtInt        *quadptr;		/* Quad List Traversal Pointer */
   auto DtFlag        smooth;		/* If true, then smooth. */
   auto DtInt         texture;		/* Texture Type (none/both/2D/3D) */
   auto DtReal       *vertexdata;	/* Vertex Data Array */
   auto DtReal       *vertexptr;	/* Vertex List Traversal Pointer */
   auto DtVertexType  vtype;		/* Vertex Information Type */
   auto DtInt         xgridloc,		/* Grid Vertex Position */
		      ygridloc;

   print ("\nQuad Mesh --\n");

   Prompt ("Gridcount [10]", 10, &gridcount);
   if (gridcount < 1) return DcNullObject;
   if (gridcount < 2) gridcount = 2;

   npatches = (gridcount-1) * (gridcount-1);
   nverts   = gridcount * gridcount;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;        ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;     ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;     ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr;  ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2;
   if (texture & TEX3D)  ele_per_vtx += 3;

   Prompt ("Smooth surface? (0=no, 1=yes)", 0, &ii);
   smooth = (ii != 0);

   gridincr = 2.0 / (DtReal) (gridcount-1);
   vertexdata = (DtReal*) space_allocate ((DtInt)
		                          (nverts*ele_per_vtx*sizeof(DtReal)));
   quads = (DtInt*) space_allocate ((DtInt) (4 * npatches * sizeof(DtInt)));

   /* The following loop spits out the vertices for each of the mesh
   // intersection points.  It adds vertex data as needed. */

   vertexptr = vertexdata;

   for (xgridloc=0;  xgridloc < gridcount;  ++xgridloc) {
      auto DtReal Lx, Ly, Lz;	/* Vertex Locations */
      auto DtReal Lu, Lv;	/* Location, UV Space */

      Lx = -1 + ((DtReal) xgridloc * gridincr);
      Lu = (Lx + 1) / 2;
      Lz = .25 * sin (Lu * 5 * M_PI);

      for (ygridloc=0;  ygridloc < gridcount;  ++ygridloc) {

	 Ly = -1 + ((DtReal) ygridloc * gridincr);
         Lv = (Ly + 1) / 2;

	 ADDVDATA (vertexptr, Lx, Ly, Lz);

	 /* If vertex normals are requested, then we add normals that are
	 // truly perpendicular to the wave surface. */

	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {

	    auto DtReal norm;		/* Normal Vector Magnitude */
	    auto DtReal slope;		/* XZ Gradiant of the Wave Surface */

	    /* Find normal to the wave curve at the vertex.  Note:
	    //
	    //     S (x,y) = <x, y, F(x)>            Surface Function
	    //     F (x)   = 0.25 sin (5 pi x)       Height Function
	    //     F'(x)   = 1.25 pi cos (5 pi x)
	    //     N (x,y) = <-F'(x), 0, 1>          Normal Function
            */

	    slope = 1.25 * M_PI * cos (5 * M_PI * Lu);
	    norm = sqrt ((slope * slope) + 1);

	    ADDVDATA (vertexptr, - (slope / norm), 0, 1 / norm);
	 }
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    ADDVDATA (vertexptr, (Lx+1)/2, (Ly+1)/2, (Lz+1)/2); 
	 }
	 if (texture & TEX2D) {
	    *vertexptr++ = Lu;
	    *vertexptr++ = 1 - Lv;
	 }
	 if (texture & TEX3D)
	    ADDVDATA (vertexptr, Lu, 1 - Lv, (2*Lz) + .5);
      }
   }

   /* Now that we've generated the vertex list, we need to generate the
   // connectivity list for the quadrilaterals. */

   quadptr = quads;

   for (xgridloc=0;  xgridloc < gridcount-1;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < gridcount-1;  ++ygridloc) {
	 *quadptr++ = (ygridloc+0) + ((xgridloc+0) * gridcount);
	 *quadptr++ = (ygridloc+0) + ((xgridloc+1) * gridcount);
	 *quadptr++ = (ygridloc+1) + ((xgridloc+1) * gridcount);
	 *quadptr++ = (ygridloc+1) + ((xgridloc+0) * gridcount);
      }
   }

   if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
   if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);

   return DoQuadMesh (DcRGB, vtype, nverts,vertexdata, npatches,quads, smooth);
}

/*==========================================================================
==  This function generates a quad grid that covers the surface of a wave
==  function.  The user is prompted for the width and height of the grid
== (in number of vertices), the vertex type, patch smoothing, and the
== texture-mapping type.
==========================================================================*/

DtObject make_quad_grid (void)
{
   auto DtInt         ii;		/* Scratch Integer */
   auto DtInt         width;		/* width of grid (in vertices)*/
   auto DtInt         height;		/* height of grid (in vertices)*/
   auto DtInt         ele_per_vtx;	/* # Parameters / Vertex */
   auto DtReal        xgridincr;	/* Increment Across Grid's width */
   auto DtReal        ygridincr;	/* Increment Across Grid's height */
   auto DtInt         nverts;		/* Number of Vertices */
   auto DtFlag        smooth;		/* If true, then smooth. */
   auto DtInt         texture;		/* Texture Type (none/both/2D/3D) */
   auto DtReal       *vertexdata;	/* Vertex Data Array */
   auto DtReal       *vertexptr;	/* Vertex List Traversal Pointer */
   auto DtVertexType  vtype;		/* Vertex Information Type */
   auto DtInt         xgridloc,		/* Grid Vertex Position */
		      ygridloc;

   print ("\nQuad Grid --\n");

   Prompt ("Width [10]", 10, &width);
   Prompt ("Height [10]", 10, &height);
   if (width < 1 || height < 1) return DcNullObject;
   if (width < 2) width = 2;
   if (height < 2) height = 2;

   nverts   = width * height;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;        ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;     ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;     ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr;  ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2;
   if (texture & TEX3D)  ele_per_vtx += 3;

   Prompt ("Smooth surface? (0=no, 1=yes)", 0, &ii);
   smooth = (ii != 0);

   xgridincr = 2.0 / (DtReal) (width-1);
   ygridincr = 2.0 / (DtReal) (height-1);
   vertexdata = (DtReal*) space_allocate ((DtInt)
		                          (nverts*ele_per_vtx*sizeof(DtReal)));

   /* The following loop spits out the vertices for each of the mesh
      intersection points.  It adds vertex data as needed. */

   vertexptr = vertexdata;

   for (ygridloc=0;  ygridloc < height;  ++ygridloc) {
      auto DtReal Lx, Ly, Lz;	/* Vertex Locations */
      auto DtReal Lu, Lv;	/* Location, UV Space */

      Ly = -1 + ((DtReal) ygridloc * ygridincr);
      Lv = (Ly + 1) / 2;

      for (xgridloc=0;  xgridloc < width;  ++xgridloc) {

	 Lx = -1 + ((DtReal) xgridloc * xgridincr);
         Lu = (Lx + 1) / 2.;
         Lz = .25 * sin (Ly * 5 * M_PI);

         printf("%d %d --> %g %g %g\n", xgridloc, ygridloc, Lx, Ly, Lz);
	 ADDVDATA (vertexptr, Lx, Ly, Lz);

	 /* If vertex normals are requested, then we add normals that are
	    truly perpendicular to the wave surface. */

	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {

	    auto DtReal norm;		/* Normal Vector Magnitude */
	    auto DtReal slope;		/* XZ Gradiant of the Wave Surface */

	    /* Find normal to the wave curve at the vertex.  Note:
	    //
	    //     S (x,y) = <x, y, F(x)>            Surface Function
	    //     F (x)   = 0.25 sin (5 pi x)       Height Function
	    //     F'(x)   = 1.25 pi cos (5 pi x)
	    //     N (x,y) = <-F'(x), 0, 1>          Normal Function
            */

	    slope = 1.25 * M_PI * cos (5 * M_PI * Lu);
	    norm = sqrt ((slope * slope) + 1);

	    ADDVDATA (vertexptr, - (slope / norm), 0, 1 / norm);
	 }
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    ADDVDATA (vertexptr, (Lx+1)/2, (Ly+1)/2, (Lz+1)/2); 
	 }
	 if (texture & TEX2D) {
	    *vertexptr++ = Lu;
	    *vertexptr++ = 1 - Lv;
	 }
	 if (texture & TEX3D)
	    ADDVDATA (vertexptr, Lu, 1 - Lv, (2*Lz) + .5);
      }
   }

   if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
   if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);

   return DoQuadGrid (DcRGB, vtype, width, height, vertexdata, smooth);
}



/*==========================================================================
==  This routine creates a simple polygon.  The polygon shape is that of a
==  solid-filled 'M' (with a line on the bottom).  The colors are set to
==  red at the top three vertices and blue on the bottom ones.  The normals
==  are +X and -X away from the polygon (the center one is normal to the
==  plane of the polygon).
==========================================================================*/

DtObject make_simply (void)
{
   static DtReal  clr[5][3] =		/* Vertex Colors */
   {  { 1.0, 0.0, 0.0}, { 1.0, 0.0, 0.0}, { 1.0, 0.0, 0.0},
      { 0.0, 0.0, 1.0}, { 0.0, 0.0, 1.0}
   };
   auto   int     ele_per_vtx;		/* # Elements Per Vertex */
   auto   int     ii;			/* Scratch Integer */
   static DtReal  loc[5][3] =		/* Vertex Locations */
   {  { 1.0, 1.0,-1.0}, { 0.0, 0.0,-1.0}, {-1.0, 1.0,-1.0},
      {-1.0,-1.0,-1.0}, { 1.0,-1.0,-1.0}
   };
   static DtReal  nrm[5][3] =		/* Vertex Normals */
   {  { 1.0, 0.0, 0.0}, { 0.0, 0.0, 1.0}, {-1.0, 0.0, 0.0},
      {-1.0, 0.0, 0.0}, { 1.0, 0.0, 0.0}
   };
   auto   DtInt   texture;		/* Texture (none/both/2D/3D) */
   auto   DtReal *vertexdata;		/* Vertex Data Array */
   auto   DtReal *vertexptr;		/* Vertex Array Traversal Pointer */
   auto   DtInt   vtype;		/* Vertex Type */

   print ("\nSimple Polygon --\n");

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;       ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;    ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;    ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr; ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2; 
   if (texture & TEX3D)  ele_per_vtx += 3; 

   vertexdata = (DtReal*) space_allocate ((DtInt)sizeof(DtReal)*5*ele_per_vtx);
   vertexptr  = vertexdata;

   for (ii=0;  ii < 5;  ++ii)
   {  ADDVDATA (vertexptr, loc[ii][0], loc[ii][1], loc[ii][2]);
      if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr))
	 ADDVDATA (vertexptr, nrm[ii][0], nrm[ii][1], nrm[ii][2]);
      if ((vtype == DcLocClr) || (vtype == DcLocNrmClr))
	 ADDVDATA (vertexptr, clr[ii][0], clr[ii][1], clr[ii][2]);
      if (texture & TEX2D) {
	 *vertexptr++ = (loc[ii][0] + 1.0) * 0.5;
	 *vertexptr++ = 1 - ((loc[ii][1] < -0.1) ? 0.0 : 1.0);
      }
      if (texture & TEX3D) {
	 *vertexptr++ = (loc[ii][0] + 1.0) * 0.5;
	 *vertexptr++ = 1 - ((loc[ii][1] + 1.0) * 0.5);
	 *vertexptr++ = 0.5;
      }
   }

   if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
   if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);

   return DoSimplePolygon (DcRGB, vtype, 5, vertexdata, DcConcave);
}



/*==========================================================================
==  This routine generates a simple polygon mesh.  The generated object is
==  a cube with corners at <0,0,0> and <1,1,-1>.  The normals are set
==  cylinder-like around the Y axis.
==========================================================================*/

DtObject make_simplymsh (void)
{
   static DtReal  clr[8][3] =		/* Vertex Colors */
   {  {.07, .00, .70}, {.40, .40, .40}, {.80, .00, .40}, {.03, .40, .13},
      {.00, .00, .40}, {.40, .50, .00}, {.80, .80, .10}, {.00, .70, .40}
   };
   auto   int     ele_per_vtx;		/* # Elements Per Vertex */
   auto   int     ii;			/* Scratch Integer */
   static DtReal  loc[8][3] =		/* Vertex Locations */
   {  {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0},
      {0.0, 1.0,-1.0}, {0.0, 0.0,-1.0}, {1.0, 0.0,-1.0}, {1.0, 1.0,-1.0}
   };
   auto   DtFlag        smooth;		/* If true, then smooth. */
   auto   DtInt         texture;	/* Texture (none/both/2D/3D) */
   auto   DtReal       *vertexdata;	/* Vertex Data Array */
   auto   DtReal       *vertexptr;	/* Vertex Array Traversal Pointer */
   auto   DtVertexType  vtype;		/* Vertex Type */
   static DtInt         contours[] =	/* # Vertices per Contour */
   { 4,4,4,4,4,4 };
   static DtInt         vlist[] =	/* Contour Vertex List */
   { 0,1,2,3, 3,2,4,5, 5,4,7,6, 6,7,1,0, 0,3,5,6, 1,7,4,2 };

   print ("\nSimple Polygon Mesh --\n");

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;       ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;    ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;    ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr; ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2; 
   if (texture & TEX3D)  ele_per_vtx += 3; 

   Prompt ("Smooth surface? (0=no, 1=yes)", 0, &ii);
   smooth = (ii != 0);

   vertexdata = (DtReal*) space_allocate ((DtInt)sizeof(DtReal)*8*ele_per_vtx);
   vertexptr  = vertexdata;

   for (ii=0;  ii < 8;  ++ii)
   {  ADDVDATA (vertexptr, loc[ii][0], loc[ii][1], loc[ii][2]);
      if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	 auto DtReal norm;	/* Magnitude of Normal */
	 auto DtReal Nx,Nz;	/* Normal X and Z components */

	 Nx = (loc[ii][0] * 2.0) - 1.0;
	 Nz = (loc[ii][2] * 2.0) + 1.0;
	 norm = sqrt ((double) Nx*Nx + Nz*Nz);

	 ADDVDATA (vertexptr, Nx/norm, 0.0, Nz/norm);
      }
      if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	 ADDVDATA (vertexptr, clr[ii][0], clr[ii][1], clr[ii][2]);
      }
      if (texture & TEX2D) {
	 *vertexptr++ = loc[ii][0];
	 *vertexptr++ = 1 - loc[ii][1];
      }
      if (texture & TEX3D) {
	 *vertexptr++ =  loc[ii][0];
	 *vertexptr++ = 1 -  loc[ii][1];
	 *vertexptr++ = -loc[ii][2];
      }
   }

   if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
   if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);

   return DoSimplePolygonMesh (DcRGB, vtype, 8, vertexdata, 6, contours,
			       vlist, DcConcave, smooth);
}



/*==========================================================================
==  This function generates a triangle list surface.
==========================================================================*/

DtObject make_triangle_list (void)
{
   auto   DtInt         ii;		/* Scratch Integer */
   auto   DtInt         ele_per_vtx;	/* # Data Values Per Vertex */
   auto   DtInt         gridcount;	/* # Grid Lines */
   auto   DtReal        gridincr;	/* Surface Grid Increment */
   auto   DtInt         xgridloc,	/* X Grid Line */
			ygridloc;	/* Y Grid Line */
   auto   DtReal        xgridval[4],
			ygridval[4],
			zgridval[4];
   auto   DtReal        rclrval[4],	/* Vertex Color */
			gclrval[4],
			bclrval[4];
   auto   DtInt         texture;	/* Texture Type (none/both/2D/3D) */
   auto   DtReal       *vertexdata;	/* Vertex Data Array */
   auto   DtReal       *vertexptr;	/* Vertex Array Traversal Pointer */
   auto   DtRealTriple nvector;
   static DtInt         vtx[6] =
   { 0, 1, 2, 1, 3, 2 };
   auto   DtVertexType  vtype;		/* Vertex Type */

   print ("\nTriangle List --\n");

   Prompt ("Gridcount [10]", 10, &gridcount);
   if (gridcount < 1) return DcNullObject;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;        ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;     ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;     ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr;  ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2;
   if (texture & TEX3D)  ele_per_vtx += 3;

   if (gridcount > 1) {
      gridincr = 2.0 / (DtReal)(gridcount-1);
      vertexdata = (DtReal*) space_allocate ((DtInt)(3*2 * (gridcount-1)
	       * (gridcount-1) * ele_per_vtx * sizeof(DtReal)));
   } else {
      vertexdata = (DtReal*) space_allocate ((DtInt)(3*2 * ele_per_vtx
	       * sizeof(DtReal)));
   }

   /* The following loop generates the triangle patches by doing a square
   // subsection at a time.  Four vertices are calculated and then two
   // triangles are generated for the square section:
   //
   // 2------3          2       2------1      The vertex indexing is
   // |      |          |\       \     |      accomplished via the "vtx"
   // |      |    is    | \       \    |      array, which gives the six
   // |      |  output  |  \  and  \   |      vertex indices from the
   // |      |    as    |   \       \  |      original four rectangle
   // |      |          |    \       \ |      vertices.
   // |      |          |     \       \|
   // 0------1          0------1       0
   */

   vertexptr = vertexdata;

   for (xgridloc=0;  xgridloc < gridcount-1;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < gridcount-1;  ++ygridloc) {

	 xgridval[0] = xgridval[2] = -1 + ((DtReal)(xgridloc+0) * gridincr);
	 xgridval[1] = xgridval[3] = -1 + ((DtReal)(xgridloc+1) * gridincr);
	 ygridval[0] = ygridval[1] = -1 + ((DtReal)(ygridloc+0) * gridincr);
	 ygridval[2] = ygridval[3] = -1 + ((DtReal)(ygridloc+1) * gridincr);

	 for (ii=0;  ii < 4;  ++ii) {
	    zgridval[ii] = wave_surf (xgridval[ii], ygridval[ii]);
	    rclrval [ii] = (1.0 + xgridval[ii]) * 0.5;
	    gclrval [ii] = (1.0 + ygridval[ii]) * 0.5;
	    bclrval [ii] = 0.1;/*(HEIGHT + zgridval[ii]) / (2 * HEIGHT);*/
	 }

	 /* Setup the six vertices for the two triangles in this square */

	 for (ii=0;  ii < 6;  ++ii) {
	    auto DtReal Lx, Ly, Lz;	/* Vertex Location */

	    Lx = xgridval[vtx[ii]];
	    Ly = ygridval[vtx[ii]];
	    Lz = zgridval[vtx[ii]];

	    ADDVDATA (vertexptr, Lx, Ly, Lz);
	    if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	    	normalize_vector(-wave_surf_dx(Lx,Ly),-wave_surf_dy(Lx,Ly),
			1,nvector);
	    	ADDVDATA (vertexptr, nvector[0], nvector[1], nvector[2]);
	    }
	    if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	       *vertexptr++ = rclrval[vtx[ii]];
	       *vertexptr++ = gclrval[vtx[ii]];
	       *vertexptr++ = bclrval[vtx[ii]];
	    }
	    if (texture & TEX2D) {
	       *vertexptr++ = (Lx + 1) * 0.5;
	       *vertexptr++ = 1 - ((Ly + 1) * 0.5);
	    }
	    if (texture & TEX3D) {
	       *vertexptr++ = (Lx + 1) * 0.5;
	       *vertexptr++ = 1 - ((Ly + 1) * 0.5);
	       *vertexptr++ = (Lz + HEIGHT) / (2 * HEIGHT);
	    }
	 }
      }
   }

   /* If the user selected a gridcount of one, then a single triangle will
   // be output. */

   if (gridcount == 1) {
      static DtReal loc[3][3] =		/* Gridcount 1 Vertex Locations */
      { {0,0,0}, {0,1,0}, {1,0,0} };

      vertexptr = vertexdata;
      for (ii=0;  ii < 3;  ++ii) {
	 ADDVDATA (vertexptr, loc[ii][0], loc[ii][1], loc[ii][2]);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr))
	    ADDVDATA (vertexptr, 0,0,-1);
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    *vertexptr++ = (1.0 + loc[ii][0]) * 0.5;
	    *vertexptr++ = (1.0 + loc[ii][1]) * 0.5;
	    *vertexptr++ = (1.0 + loc[ii][2]) * 0.5;
	 }
	 if (texture & TEX2D) {
	    *vertexptr++ = (xgridval[vtx[ii]] + 1.0) * 0.5;
	    *vertexptr++ = (ygridval[vtx[ii]] + 1.0) * 0.5;
	 }
	 if (texture & TEX3D) {
	    *vertexptr++ = (xgridval[vtx[ii]] + 1.0) * 0.5;
	    *vertexptr++ = (ygridval[vtx[ii]] + 1.0) * 0.5;
	    *vertexptr++ = (zgridval[vtx[ii]] + HEIGHT) / (2 * HEIGHT);
	 }
      }
   }

   if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
   if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);

   if (gridcount == 1)
      return DoTriangleList (DcRGB, vtype, 1, vertexdata);
   else
      return DoTriangleList (DcRGB, vtype, 2*(gridcount-1)*(gridcount-1),
			     vertexdata);
}



/*==========================================================================
==  This routine generates a triangle mesh.
==========================================================================*/

DtObject make_triangle_mesh (void)
{
   auto DtVertexType  vtype;		/* Vertex Type */
   auto DtInt         ii;		/* Scratch Integer */
   auto DtInt         ele_per_vtx;	/* Real-Values Per Vertex */
   auto DtInt         gridcount=0;	/* # Grid Lines */
   auto DtReal        gridincr;		/* Surface Grid Increment */
   auto DtInt         xgridloc,		/* Grid Location X */
		      ygridloc;		/* Grid Location Y */
   auto DtReal       *vertexdata;	/* Vertex Data Array Pointer */
   auto DtReal       *vertexptr;	/* Vertex Array Traversal Pointer */
   auto DtInt         texture;		/* Texture Type (none/both/2D/3D) */
   auto DtInt        *triangles;
   auto DtInt        *triptr;
   auto DtFlag        smooth;		/* Normal Smooth Flag */
   auto DtRealTriple  nvector;

   print ("\nTriangle Mesh --\n");

   Prompt ("Gridcount [10]", 10, &gridcount);
   if (gridcount < 1) return DcNullObject;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;        ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;     ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;     ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr;  ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2;
   if (texture & TEX3D)  ele_per_vtx += 3;

   Prompt ("Smooth surface? (0=no, 1=yes)", 0, &ii);
   smooth = (ii != 0);

   if (gridcount > 1) {
      gridincr = 2.0 / (DtReal)(gridcount-1);
      vertexdata = (DtReal*) space_allocate ((DtInt)(gridcount * gridcount
					   * ele_per_vtx * sizeof(DtReal)));
      triangles = (DtInt*) space_allocate ((DtInt)(2*3 * (gridcount-1)
					   * (gridcount-1) * sizeof(DtInt)));
   } else {
      vertexdata = (DtReal*) space_allocate ((DtInt)
		               (3 * ele_per_vtx * sizeof(DtReal)));
      triangles = (DtInt*) space_allocate ((DtInt) (3 * sizeof(DtInt)));
   }

   vertexptr = vertexdata;

   for (xgridloc=0;  xgridloc < gridcount;  ++xgridloc) {
      auto DtReal loc[3];		/* Vertex Location */

      loc[0] = -1 + ((DtReal)xgridloc*gridincr);

      for (ygridloc=0;  ygridloc < gridcount;  ++ygridloc) {

	 loc[1] = -1 + ((DtReal)ygridloc*gridincr);
	 loc[2] = wave_surf (loc[0], loc[1]);

	 ADDVDATA (vertexptr, loc[0],loc[1],loc[2]);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	    normalize_vector(-wave_surf_dx(loc[0],loc[1]),
                                 -wave_surf_dy(loc[0],loc[1]), 1, nvector);
	    ADDVDATA (vertexptr, nvector[0], nvector[1], nvector[2]);
	 }
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    *vertexptr++ = (1 + loc[0]) / 2;
	    *vertexptr++ = (1 + loc[1]) / 2;
	    *vertexptr++ = (HEIGHT + loc[2]) / (2 * HEIGHT);
	 }
	 if (texture & TEX2D) {
	    *vertexptr++ = (1 + loc[0]) / 2;
	    *vertexptr++ = 1 - ((1 + loc[1]) / 2);
	 }
	 if (texture & TEX3D) {
	    *vertexptr++ = (loc[0] + 1) / 2;
	    *vertexptr++ = 1 - ((loc[1] + 1) / 2);
	    *vertexptr++ = (loc[2] + HEIGHT) / (2 * HEIGHT);
	}
      }
   }

   triptr = triangles;
   for (xgridloc=0;  xgridloc < gridcount-1;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < gridcount-1;  ++ygridloc) {
	 *triptr++ = (ygridloc+0) + ((xgridloc+0) * gridcount);
	 *triptr++ = (ygridloc+0) + ((xgridloc+1) * gridcount);
	 *triptr++ = (ygridloc+1) + ((xgridloc+0) * gridcount);

	 *triptr++ = (ygridloc+0) + ((xgridloc+1) * gridcount);
	 *triptr++ = (ygridloc+1) + ((xgridloc+1) * gridcount);
	 *triptr++ = (ygridloc+1) + ((xgridloc+0) * gridcount);
      }
   }

   if (gridcount == 1) {
      static DtReal loc[3][3] =		/* Gridcount 1 Vertex Locations */
      { {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0} };

      vertexptr = vertexdata;
      for (ii=0;  ii < 3;  ++ii)
      {  ADDVDATA (vertexptr, loc[ii][0], loc[ii][1], loc[ii][2]);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr))
	    ADDVDATA (vertexptr, 0,0,1);
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    *vertexptr++ = (1.0 + loc[ii][0]) * 0.5;
	    *vertexptr++ = (1.0 + loc[ii][1]) * 0.5;
	    *vertexptr++ = (1.0 + loc[ii][2]) * 0.5;
	 }
      }
      triangles[0] = 0;
      triangles[1] = 1;
      triangles[2] = 2;
   }

   if (gridcount == 1) {
      return DoTriangleMesh (DcRGB, vtype, 3, vertexdata, 1, triangles, smooth);
   } else {
      if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
      if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);
      return DoTriangleMesh (DcRGB, vtype, gridcount*gridcount, vertexdata,
			     2*(gridcount-1)*(gridcount-1), triangles, smooth);
   }
}



/*==========================================================================
==  This function generates a triangle strip surface.  The generated surface
==  is a sine function of X and Y.  The user is prompted for the number of
==  grid lines, the line type, and the texture-mapping type.
==========================================================================*/

DtObject make_triangle_strip (void)
{
   auto   DtInt         ele_per_vtx;	/* # Elements / Vertex */
   auto   DtInt         gridcount;	/* # TriStrip Grids */
   auto   DtReal        gridincr;
   auto   DtInt         ii;		/* Loop Counter */
   auto   DtInt         start, end;
   auto   DtInt         texture;	/* Texture Type (none/both/2D/3D) */
   auto   DtReal        uvw[2][3];	/* UVW Texture Indices */
   auto   DtReal       *vertexdata;	/* Vertex Data Values Array */
   auto   DtReal       *vertexptr;	/* Vertex Array Traversal Pointer */
   static DtInt         vtx[4] = {1,1,0,0};
   auto   DtVertexType  vtype;		/* Vertex Information Type */
   auto   DtInt         xgridloc,	/* Surface X & Y Location */
   			ygridloc;
   auto   DtReal        xgridval[2],	/* Grid Vertex Positions */
			ygridval,
			zgridval[2];
   auto   DtRealTriple  nvector;

   print ("\nTriangle Strip --\n");

   Prompt ("Gridcount [10]", 10, &gridcount);
   if (gridcount < 1) return DcNullObject;

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &ii);
   switch (ii) {
      default:
      case 0:  vtype = DcLoc;       ele_per_vtx = 3;  break;
      case 1:  vtype = DcLocNrm;    ele_per_vtx = 6;  break;
      case 2:  vtype = DcLocClr;    ele_per_vtx = 6;  break;
      case 3:  vtype = DcLocNrmClr; ele_per_vtx = 9;  break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];
   if (texture & TEX2D)  ele_per_vtx += 2;
   if (texture & TEX3D)  ele_per_vtx += 3;

   if (gridcount > 1) {
      gridincr = 2.0 / (DtReal)(gridcount-1);
      vertexdata = (DtReal*) space_allocate ((DtInt)((gridcount+1)*(gridcount-1)
					   * 2 * ele_per_vtx * sizeof(DtReal)));
   } else {
      vertexdata = (DtReal*) space_allocate ((DtInt)(3*2 * ele_per_vtx
		                           * sizeof(DtReal)));
   }

   vertexptr = vertexdata;

   for (xgridloc=0;  xgridloc < gridcount-1;  ++xgridloc) {
      xgridval[0] = -1  + ((DtReal)(xgridloc  ) * gridincr);
      xgridval[1] = -1  + ((DtReal)(xgridloc+1) * gridincr);

      for (ygridloc=0;  ygridloc < gridcount;  ++ygridloc) {

	 ygridval = -1  + ((DtReal)ygridloc * gridincr);

	 zgridval[0] = wave_surf (xgridval[0], ygridval);
	 zgridval[1] = wave_surf (xgridval[1], ygridval);

	 if (texture & (TEX2D | TEX3D)) {
	    uvw[0][0] = (xgridval[0] + 1) / 2;			/* U0 */
	    uvw[1][0] = (xgridval[1] + 1) / 2;			/* U1 */
	    uvw[0][1] = uvw[1][1] = 1 - ((ygridval + 1) / 2);	/* V0 & V1 */
	 }
	 if (texture & TEX3D) {
	    uvw[0][2] = (DtReal) (zgridval[0] + HEIGHT) / (2 * HEIGHT);
	    uvw[1][2] = (DtReal) (zgridval[1] + HEIGHT) / (2 * HEIGHT);
	 }

	 /* Load in the vertices for the current patch in the current
	 // vertical strip.  A vertical strip looks like this:
	 //     .   .
	 //     :/  :        Note that the first and last vertices of the
	 //   6 +---+ 5      strip are loaded twice.  The result is that we
	 //     |R /|        get a degenerate triangle for each edge segment
	 //     | / |        in the X direction, and a degenerate triangle
	 //     |/ L|        to the beginning of the next vertical strip.
	 //   4 +---+ 3      This allows us to do the entire surface in a
	 //     |R /|        single triangle strip.  Note the ``handedness''
	 //     | / |        of the triangles (marked L for left-hand and R
	 //     |/ L|        for right-hand).
	 //   2 +===+ 0,1
	 */

	 start = (ygridloc == 0) ? 0 : 1;
	 end   = (ygridloc == gridcount-1) ? 3 : 2;

	 for (ii=start;  ii <= end;  ++ii) {
	    auto DtReal Lx, Ly, Lz;		/* Vertex Location */

	    Lx = xgridval[vtx[ii]];
	    Ly = ygridval;
	    Lz = zgridval[vtx[ii]];


	    ADDVDATA (vertexptr, Lx, Ly, Lz);
	    if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr)) {
	    	normalize_vector(-wave_surf_dx(Lx,Ly),
				-wave_surf_dy(Lx,Ly),1,nvector);
	    	ADDVDATA (vertexptr, nvector[0], nvector[1], nvector[2]);
	    }
	    if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	       *vertexptr++ = (1 + Lx) / 2;
	       *vertexptr++ = (1 + Ly) / 2;
	       *vertexptr++ = (HEIGHT + Lz) / (2 * HEIGHT);
	    }
	    if (texture & TEX2D) {
	       *vertexptr++ = uvw[vtx[ii]][0];
	       *vertexptr++ = uvw[vtx[ii]][1];
	    }
	    if (texture & TEX3D) {
	       *vertexptr++ = uvw[vtx[ii]][0];
	       *vertexptr++ = uvw[vtx[ii]][1];
	       *vertexptr++ = uvw[vtx[ii]][2];
	    }
	 }
      }
   }

   if (gridcount == 1) {
      static DtReal loc[3][3] =		/* Gridcount 1 Vertex Locations */
      { {0,0,0}, {0,1,0}, {1,0,0} };

      vertexptr = vertexdata;
      for (ii=0;  ii < 3;  ++ii) {
	 ADDVDATA (vertexptr, loc[ii][0], loc[ii][1], loc[ii][2]);
	 if ((vtype == DcLocNrm) || (vtype == DcLocNrmClr))
	    ADDVDATA (vertexptr, 0,0,-1);
	 if ((vtype == DcLocClr) || (vtype == DcLocNrmClr)) {
	    *vertexptr++  = (1 + loc[ii][0]) / 2;
	    *vertexptr++  = (1 + loc[ii][1]) / 2;
	    *vertexptr++  = (1 + loc[ii][2]) / 2;
	 }
      }
   }

   if (gridcount == 1) {
      return DoTriangleStrip (DcRGB, vtype, 3, vertexdata);
   } else {
      if (texture & TEX2D)  vtype |= DsTextureUVCount (1);
      if (texture & TEX3D)  vtype |= DsTextureUVWCount (1);
      return DoTriangleStrip (DcRGB, vtype,
			     (2*(gridcount)+2) * (gridcount-1), vertexdata);
   }
}



/*==========================================================================
==
==========================================================================*/

DtObject make_uvtext (void)
{
   static DtPoint3 origin = {0.,0.,0.};

   static DtVector3 X = { 1.,0.,0.},
                    Y = { 0.,1.,0.},
                    Z = { 0.,0.,1.},
                    NegX = { -1.,0.,0.},
                    NegY = { 0.,-1.,0.},
                    NegZ = { 0.,0.,-1.},
                    NegYZ = { 0.,-1.,1.};
   static DtReal    red[]   = {1.,0.,0.},
                    green[] = {0.,1.,0.},
                    blue[]  = {0.,0.,1.},
                    cyan[]  = {0.,1.,1.};
   static DtReal    xline[] = {0.,0.,0.,    1.,0.,0.},
                    yline[] = {0.,0.,0.,    0.,1.,0.},
                    zline[] = {0.,0.,0.,    0.,0.,1.};

   DoGroup(DcTrue);
	 DgAddObj(DoText(origin, X, Y, "---> XYplane"));
         DgAddObj(DoText(origin, X, Z, "---> XZplane"));
         DgAddObj(DoText(origin, Y, Z, "---> YZplane"));

         DgAddObj(DoText(origin, NegX, Y, "---> -XYplane"));
         DgAddObj(DoText(origin, NegX, Z, "---> -XZplane"));
         DgAddObj(DoText(origin, NegY, Z, "---> -YZplane"));

         DgAddObj(DoText(origin, X, NegY, "---> X-Yplane"));
         DgAddObj(DoText(origin, X, NegZ, "---> X-Zplane"));
         DgAddObj(DoText(origin, Y, NegZ, "---> Y-Zplane"));

         DgAddObj(DoText(origin, Y, X, "---> YXplane"));
         DgAddObj(DoText(origin, Z, X, "---> ZXplane"));
         DgAddObj(DoText(origin, Z, Y, "---> ZYplane"));

	 DgAddObj(DoDiffuseColor(DcRGB, red));
         DgAddObj(DoLineList(DcRGB, DcLoc, 1, xline));
         DgAddObj(DoDiffuseColor(DcRGB, green));
         DgAddObj(DoLineList(DcRGB, DcLoc, 1, yline));
         DgAddObj(DoDiffuseColor(DcRGB, blue));
         DgAddObj(DoLineList(DcRGB, DcLoc, 1, zline));
   return DgClose();
}



/*==========================================================================
==  This routine generates a variable geometry line list.
==========================================================================*/

DtObject make_varlinlst (void)
{
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal xgridval1, ygridval1, zgridval1;
   DtReal xgridval2, ygridval2, zgridval2;
   DtReal *locptr, *clrptr, *nrmptr;
   DtFlag colors, normals;
   DtInt i;

   print ("\nVariable Line List --\n");

   Prompt ("Gridcount [10]", 10, &vgridcount);
   if (vgridcount < 1) 
	return DcNullObject;
   else if (vgridcount > 1)
	 gridincr = 2. / (DtReal)(vgridcount-1);

   if (varlock == DcFalse)
      varlock = DcTrue;
   else {
      print ("ERROR:  Variable geometry already in use.\n");
      return DcNullObject;
   }

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &i);
   switch (i) {
      default:
      case 0:  colors = DcFalse; 	normals = DcFalse; 	break;
      case 1:  colors = DcFalse;	normals = DcTrue;     	break;
      case 2:  colors = DcTrue;     	normals = DcFalse;	break;
      case 3:  colors = DcTrue;		normals = DcTrue;	break;
   }

   locdata = (DtReal *) space_allocate ((DtInt)(2 * vgridcount * vgridcount
					      * 3 * sizeof(DtReal)));

   if (colors == DcTrue) 
	clrdata = (DtReal *) space_allocate ((DtInt)(2 * vgridcount * 
				vgridcount * 3 * sizeof(DtReal)));
   else
	clrdata = DcNullPtr;

   if (normals == DcTrue)
	nrmdata = (DtReal *) space_allocate ((DtInt)(2 * vgridcount * 
				vgridcount * 3 * sizeof(DtReal)));
   else
	nrmdata = DcNullPtr;


   locptr = locdata;
   clrptr = clrdata;
   nrmptr = nrmdata;

   for (xgridloc=0;  xgridloc<vgridcount-1;  xgridloc++) {
      for (ygridloc=0;  ygridloc<vgridcount;  ygridloc++) {
	 xgridval1 = -1 + ((DtReal)xgridloc*gridincr);
	 ygridval1 = -1 + ((DtReal)ygridloc*gridincr);
	 zgridval1 = wave_surf (xgridval1, ygridval1);

	 xgridval2 = -1 + ((DtReal)(xgridloc+1)*gridincr);
	 ygridval2 = -1 + ((DtReal)ygridloc*gridincr);
	 zgridval2 = wave_surf (xgridval2, ygridval2);

	 /* vertex 1 */

	 ADDVDATA (locptr, xgridval1, ygridval1, zgridval1);
	 ADDVDATA (locptr, xgridval2, ygridval2, zgridval2);

	 if (colors == DcTrue) {
	    *clrptr++ = (1 + xgridval1) * 0.5;
	    *clrptr++ = (1 + ygridval1) * 0.5;
	    *clrptr++ = (1 + zgridval1) * 0.5;

	    *clrptr++ = (1 + xgridval2) * 0.5;
	    *clrptr++ = (1 + ygridval2) * 0.5;
	    *clrptr++ = (1 + zgridval2) * 0.5;
	 }

	 if (normals == DcTrue) {
	    *nrmptr++ = 0;
	    *nrmptr++ = 0;
	    *nrmptr++ = 1;

	    *nrmptr++ = 0;
	    *nrmptr++ = 0;
	    *nrmptr++ = 1;
	 }
      }
   }

   if (vgridcount == 1) {
      ADDVDATA (locptr, 0,0,0);
      ADDVDATA (locptr, 1,0,0);
      ADDVDATA (clrptr, 0.5,0.5,0.5);
      ADDVDATA (clrptr, 1.0,0.5,0.5);
      nrmdata = DcNullPtr;
   }

   varlinlst = DoVarLineList (DcRGB, (vgridcount-1)*vgridcount, locdata,
			      nrmdata, clrdata);
   DsHoldObj (varlinlst);
   return varlinlst;
}



/*==========================================================================
==
==========================================================================*/

DtObject make_varpntlst (void)
{
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal *locptr, *clrptr, *nrmptr;
   DtFlag colors, normals;
   DtInt i;

   print ("\nVariable Point List --\n");

   Prompt ("Gridcount [10]", 10, &vgridcount);
   if (vgridcount < 1)
        return DcNullObject;
   else if (vgridcount > 1)
         gridincr = 2. / (DtReal)(vgridcount-1);

   if (varlock == DcFalse)
      varlock = DcTrue;
   else {
      print ("ERROR:  Variable geometry already in use.\n");
      return DcNullObject;
   }

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &i);
   switch (i) {
      default:
      case 0:  colors = DcFalse;        normals = DcFalse;      break;
      case 1:  colors = DcFalse;        normals = DcTrue;       break;
      case 2:  colors = DcTrue;         normals = DcFalse;      break;
      case 3:  colors = DcTrue;         normals = DcTrue;       break;
   }

   locdata = (DtReal *) space_allocate ((DtInt)(vgridcount * vgridcount
                                              * 3 * sizeof(DtReal)));

   if (colors == DcTrue)
        clrdata = (DtReal *) space_allocate ((DtInt)(vgridcount *
                                vgridcount * 3 * sizeof(DtReal)));
   else
        clrdata = DcNullPtr;

   if (normals == DcTrue)
        nrmdata = (DtReal *) space_allocate ((DtInt)(vgridcount *
                                vgridcount * 3 * sizeof(DtReal)));
   else
        nrmdata = DcNullPtr;

   locptr = locdata;
   clrptr = clrdata;
   nrmptr = nrmdata;

   for (xgridloc=0;xgridloc<vgridcount;xgridloc++) {
      for (ygridloc=0;ygridloc<vgridcount;ygridloc++) {
	 locptr[0] = -1 + ((DtReal)xgridloc*gridincr);
	 locptr[1] = -1 + ((DtReal)ygridloc*gridincr);
	 locptr[2] = wave_surf (locptr[0], locptr[1]);

	 if (colors == DcTrue) {
	    *clrptr++ = (1 + locptr[0]) * .5;
            *clrptr++ = (1 + locptr[1]) * .5;
            *clrptr++ = (1 + locptr[2]) * .5;
	 }

         if (normals == DcTrue) {
            *nrmptr++ = 0;
            *nrmptr++ = 0;
            *nrmptr++ = 1;
         }

         locptr += 3;
      }
   }

   varpntlst = DoVarPointList(DcRGB,vgridcount*vgridcount,locdata,nrmdata,clrdata);

   DsHoldObj(varpntlst);
   return (varpntlst);
}



/*==========================================================================
==  This function generates a variable simple polgon mesh surface (the user
==  is able to interactively alter the geometry of the mesh).  The test
==  object is a cube constructed of six square polygons; the cube extends
==  from the <-1,-1,-1> to <1,1,1>.  The user is prompted for the texture
==  mapping mode, which may be none, 2D or 3D.
==========================================================================*/

DtObject make_varsimplymsh (void)
{
   static DtReal  clr[8][3] =		/* Vertex Colors */
   {  {.00, .00, .60}, {.73, .67, .40}, {.03, .60, .30}, {.70, .40, .01},
      {.60, .00, .00}, {.00, .50, .05}, {.67, .25, .25}, {.00, .30, .60} 
   };
   auto   DtReal *clrptr;		/* Vertex Color Value Array Pointer */
   static DtInt   contours[] =		/* # of Vertices per Contour */
   { 4, 4, 4, 4, 4, 4 };
   auto   DtInt   ii;			/* Scratch Integer */
   static DtReal  loc[8][3] =		/* Vertex Locations */
   {  { 1.,-1., 1.}, { 1., 1., 1.}, {-1., 1., 1.}, {-1.,-1., 1.},
      {-1., 1.,-1.}, {-1.,-1.,-1.}, { 1.,-1.,-1.}, { 1., 1.,-1.}
   };
   auto   DtReal *locptr;		/* Vertex Location Array Pointer */
   auto   DtFlag  smooth;		/* Normal Smooth Flag */
   auto   DtInt   texture;		/* Texture Mode (none/both/2D/3D) */
   auto   DtReal *uvptr;		/* Vertex UV Value Array Pointer */
   auto   DtReal *uvwptr;		/* Vertex UVW Value Array Pointer */
   static DtReal  uv[8][2] =		/* UV Texture Parameters */
   {  {1., 0.}, {1., 1.}, {0., 1.}, {0., 0.},
      {0., 1.}, {0., 0.}, {1., 0.}, {1., 1.}
   };
   static DtReal  uvw[8][3] =		/* UVW Texture Parameters */
   {  {1.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0},
      {0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 0.0, 1.0}
   };
   static DtInt   vlist[] =		/* Contour Vertex List */
   { 0,1,2,3, 3,2,4,5, 5,4,7,6, 6,7,1,0, 0,3,5,6, 1,7,4,2 };

   print ("\nVariable Simple Polygon Mesh --\n");

   if (varlock == DcFalse)
      varlock = DcTrue;
   else {
      print ("ERROR:  Variable geometry already in use.\n");
      return DcNullObject;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];

   Prompt ("Smooth surface? (0=no, 1=yes)", 0, &ii);
   smooth = (ii != 0);

   locptr = vertloc;
   clrptr = vertclr;
   uvptr  = vertuv;
   uvwptr = vertuvw;

   for (ii=0;  ii < 8;  ++ii)
   {   ADDVDATA (locptr, loc[ii][0], loc[ii][1], loc[ii][2]);
       ADDVDATA (clrptr, clr[ii][0], clr[ii][1], clr[ii][2]);
       if (texture & TEX2D) {
	  *uvptr++ = uv[ii][0];
	  *uvptr++ = uv[ii][1];
       }
       if (texture & TEX3D)
          ADDVDATA (uvwptr, uvw[ii][0], uvw[ii][1], uvw[ii][2]);
   }

   varsimplymsh = DoVarSimplePolygonMesh
		  (  DcRGB, 8, vertloc, DcNullPtr, vertclr, 6, contours,
		     vlist, DcConvex, smooth
		  );

   DsHoldObj (varsimplymsh);

   {  if (texture & TEX2D)
	 DpUpdVarSimplePolygonMeshUV (varsimplymsh, 0, vertuv);
      if (texture & TEX3D)
	 DpUpdVarSimplePolygonMeshUVW (varsimplymsh, 0, vertuvw);
   }

   return varsimplymsh;
}



/*==========================================================================
==  This function generates a variable triangle mesh surface (the user is
==  to able interactively alter the geometry of the mesh).  The generated
==  surface is a sine function of X and Y.  The user is prompted for the
==  texture mapping mode and vertex color.
==========================================================================*/

DtObject make_vartrimsh (void)
{
   auto DtFlag  color_verts;	/* If nonzero, then use vertex color. */
   auto DtReal  gridincr;	/* Surface Grid Increment */
   auto DtInt   ii;		/* Scratch Integer */
   auto DtReal *locptr;		/* Pointer to Vertex Location Array */
   auto DtReal *clrptr;		/* Pointer to Vertex Color Array */
   auto DtReal *nrmptr;		/* Pointer to Vertex Normal Array */
   auto DtInt   texture;	/* Texture (0=none, 1=both, 2=2D, 3=3D) */
   auto DtInt  *triangles;	/* Triangle Array Traversal Pointer */
   auto DtInt  *triptr;
   auto DtReal *uvptr;		/* UV Vertex Values Array */
   auto DtReal *uvwptr;		/* UVW Vertex Values Array */
   auto DtInt   xgridloc,	/* X & Y Grid Location */
		ygridloc;
   DtFlag colors, normals;
   DtInt i;
   DtRealTriple nvector;

   print ("\nVariable Triangle Mesh --\n");

   if (varlock == DcFalse)
      varlock = DcTrue;
   else {
      print ("ERROR:  Variable geometry already in use.\n");
      return DcNullObject;
   }

   Prompt ("Gridcount [10]", 10, &vgridcount);
   if (vgridcount < 1)
        return DcNullObject;
   else if (vgridcount > 1)
         gridincr = 2. / (DtReal)(vgridcount-1);

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &i);
   switch (i) {
      default:
      case 0:  colors = DcFalse;        normals = DcFalse;      break;
      case 1:  colors = DcFalse;        normals = DcTrue;       break;
      case 2:  colors = DcTrue;         normals = DcFalse;      break;
      case 3:  colors = DcTrue;         normals = DcTrue;       break;
   }

   triangles = (DtInt*) space_allocate ((DtInt)(2 * 3 * (vgridcount-1)
					* (vgridcount-1) * sizeof(DtInt)));

   locdata = (DtReal *) space_allocate ((DtInt)(vgridcount * vgridcount
                                              * 3 * sizeof(DtReal)));

   if (normals == DcTrue)
        nrmdata = (DtReal *) space_allocate ((DtInt)(vgridcount *
                                vgridcount * 3 * sizeof(DtReal)));
   else
        nrmdata = DcNullPtr;

   if (colors == DcTrue)
        clrdata = (DtReal *) space_allocate ((DtInt)(vgridcount *
                                vgridcount * 3 * sizeof(DtReal)));
   else
        clrdata = DcNullPtr;

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];

   /* Loop over all grid vertices and set the location, color and (if
   // appropriate) the texture for each grid point.  */

   locptr = locdata;
   clrptr = clrdata;
   nrmptr = nrmdata;
   uvptr  = vertuv;
   uvwptr = vertuvw;

   for (xgridloc=0;  xgridloc < vgridcount;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < vgridcount;  ++ygridloc) {
	 auto DtReal Lx, Ly, Lz;

	 Lx = -1 + ((DtReal) xgridloc * gridincr);
	 Ly = -1 + ((DtReal) ygridloc * gridincr);
	 Lz = wave_surf (Lx, Ly);

	 ADDVDATA (locptr, Lx, Ly, Lz);

	 if (colors == DcTrue)
	     ADDVDATA (clrptr, 0.2, (Lx+1)/2, (Ly+1)/2);

	 if (normals == DcTrue) {
	     normalize_vector(-wave_surf_dx(Lx, Ly),
                              -wave_surf_dy(Lx, Ly), 1, nvector);
	     ADDVDATA (nrmptr, nvector[0], nvector[1], nvector[2]);
	 }

	 if (texture & TEX2D) {
	    *uvptr++ = (DtReal) xgridloc / (DtReal) (vgridcount-1);
	    *uvptr++ = 1.0 - ((DtReal) ygridloc / (DtReal) (vgridcount-1));
	 }
	 if (texture & TEX3D) {
	    *uvwptr++ = (DtReal) xgridloc / (DtReal) (vgridcount-1);
	    *uvwptr++ = 1.0 - ((DtReal) ygridloc / (DtReal) (vgridcount-1));
	    *uvwptr++ = (Lz + HEIGHT) / (2 * HEIGHT);
         }
      }
   }

   for (triptr=triangles, xgridloc=0;  xgridloc < vgridcount-1;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < vgridcount-1;  ++ygridloc) {
	 *triptr++ = (ygridloc  ) + (xgridloc  ) * vgridcount;
	 *triptr++ = (ygridloc  ) + (xgridloc+1) * vgridcount;
	 *triptr++ = (ygridloc+1) + (xgridloc  ) * vgridcount;

	 *triptr++ = (ygridloc  ) + (xgridloc+1) * vgridcount;
	 *triptr++ = (ygridloc+1) + (xgridloc+1) * vgridcount;
	 *triptr++ = (ygridloc+1) + (xgridloc  ) * vgridcount;
      }
   }

   vartrimsh = DoVarTriangleMesh
	       (  DcRGB, vgridcount * vgridcount, locdata, nrmdata, clrdata, 
		   2 * (vgridcount-1) * (vgridcount-1), triangles, DcFalse
	       );

   {  if (texture & TEX2D)
	 DpUpdVarTriangleMeshUV (vartrimsh, 0, vertuv);
      if (texture & TEX3D)
	 DpUpdVarTriangleMeshUVW (vartrimsh, 0, vertuvw);
   }

   return vartrimsh;
}



/*==========================================================================
==  This function generates a variable quad grid surface (the user is
==  to able interactively alter the geometry of the grid).  The generated
==  surface is a sine function of X and Y.  The user is prompted for the
==  texture mapping mode, vertex color, and smooth parameter.
==========================================================================*/

DtObject make_varquadgrid (void)
{
   auto DtReal *clrptr;		/* Pointer to Vertex Color Array */
   auto DtFlag  color_verts;	/* If nonzero, then use vertex color. */
   auto DtFlag  smooth;		/* Smoot flag*/
   auto DtReal  gridincr;	/* Surface Grid Increment */
   auto DtInt   gridcount;	/* Number of Grid Lines on Surface */
   auto DtInt   ii;		/* Scratch Integer */
   auto DtReal *locptr;		/* Pointer to Vertex Location Array */
   auto DtInt   texture;	/* Texture (0=none, 1=both, 2=2D, 3=3D) */
   auto DtReal *uvptr;		/* UV Vertex Values Array */
   auto DtReal *uvwptr;		/* UVW Vertex Values Array */
   auto DtInt   xgridloc,	/* X & Y Grid Location */
		ygridloc;

   print ("\nVariable Quad Grid --\n");

   if (varlock == DcFalse)
      varlock = DcTrue;
   else {
      print ("ERROR:  Variable geometry already in use.\n");
      return DcNullObject;
   }

   /* we want 20 x20 quads, i.e 21 x 21 vertices */
   gridcount = 21;
   gridincr  = 0.1;

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];

   Prompt ("Include vertex color? (0=no, 1=yes)", 0, &ii);
   color_verts = (ii != 0);

   Prompt ("Smooth surface? (0=no, 1=yes)", 0, &ii);
   smooth = (ii != 0);

   /* Loop over all grid vertices and set the location, color and (if
   // appropriate) the texture for each grid point.  */

   locptr = vertloc;
   clrptr = vertclr;
   uvptr  = vertuv;
   uvwptr = vertuvw;

   for (xgridloc=0;  xgridloc < gridcount;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < gridcount;  ++ygridloc) {
	 auto DtReal Lx, Ly, Lz;

	 Lx = -1 + ((DtReal) xgridloc * gridincr);
	 Ly = -1 + ((DtReal) ygridloc * gridincr);
	 Lz = wave_surf (Lx, Ly);

	 ADDVDATA (locptr, Lx, Ly, Lz);
	 if (color_verts)
	     ADDVDATA (clrptr, 0.2, (Lx+1)/2, (Ly+1)/2);

	 if (texture & TEX2D) {
	    *uvptr++ = (DtReal) xgridloc / (DtReal) (gridcount-1);
	    *uvptr++ = 1.0 - ((DtReal) ygridloc / (DtReal) (gridcount-1));
	 }
	 if (texture & TEX3D) {
	    *uvwptr++ = (DtReal) xgridloc / (DtReal) (gridcount-1);
	    *uvwptr++ = 1.0 - ((DtReal) ygridloc / (DtReal) (gridcount-1));
	    *uvwptr++ = (Lz + HEIGHT) / (2 * HEIGHT);
         }
      }
   }

   varquadgrid = DoVarQuadGrid
	       (  DcRGB, gridcount, gridcount, vertloc, DcNullPtr,
		  (color_verts ? vertclr : DcNullPtr), smooth
	       );

   {  if (texture & TEX2D)
	 DpUpdVarQuadGridUV (varquadgrid, 0, vertuv);
      if (texture & TEX3D)
	 DpUpdVarQuadGridUVW (varquadgrid, 0, vertuvw);
   }

   return varquadgrid;
}



/*==========================================================================
==  This function generates a variable triangle strip surface (the user is
==  able to interactively alter the geometry of the mesh).  The generated
==  surface is a sine function of X and Y defined by the macro "wave_surf".
==  The user is prompted for the texture mapping mode.
==========================================================================*/

DtObject make_vartristrip (void)
{
   auto   DtInt   end;
   auto   DtInt   ii;			/* Scratch Integer */
   auto   DtReal  gridincr;		/* Surface Grid Traversal Increment */
   auto   DtReal *locptr;		/* Ptr to Vertex Location Storage */
   auto   DtReal *nrmptr;		/* Ptr to Vertex Normals Storage */
   auto   DtReal *clrptr;		/* Ptr to Vertex Colors Storage */
   auto   DtInt   start;
   auto   DtInt   texture;		/* Texture Type (none/both/2D/3D) */
   auto   DtReal *uvptr;		/* Vertex UV Value Array Pointer */
   auto   DtReal *uvwptr;		/* Vertex UVW Value Array Pointer */
   static DtInt   vtx[4] = { 0,0,1,1 };
   auto   DtInt   xgridloc;		/* X Surface Grid Location */
   auto   DtReal  xgridval[4];
   auto   DtInt   ygridloc;		/* Y Surface Grid Location */
   auto   DtReal  ygridval[4];
   auto   DtReal  zgridval[4];
   DtFlag colors, normals;
   DtInt i;
   DtRealTriple nvector;

   print ("\nVariable Triangle Strip --\n");

   if (varlock == DcFalse)
      varlock = DcTrue;
   else {
      print ("ERROR:  Variable geometry already in use.\n");
      return DcNullObject;
   }

   Prompt ("Gridcount [10]", 10, &vgridcount);
   if (vgridcount < 1)
        return DcNullObject;
   else if (vgridcount > 1)
         gridincr = 2. / (DtReal)(vgridcount-1);

   Prompt ("Vertex type (0=Loc, 1=LocNrm, 2=LocClr, 3=LocNrmClr)", 0, &i);
   switch (i) {
      default:
      case 0:  colors = DcFalse;        normals = DcFalse;      break;
      case 1:  colors = DcFalse;        normals = DcTrue;       break;
      case 2:  colors = DcTrue;         normals = DcFalse;      break;
      case 3:  colors = DcTrue;         normals = DcTrue;       break;
   }

   Prompt ("Texture mapping (0=none, 1=both, 2=2D, 3=3D)", 0, &ii);
   texture = texflags[ii];

   locdata = (DtReal *) space_allocate ((DtInt)(2 * (vgridcount+1) * 
				(vgridcount-1) * 3 * sizeof(DtReal)));

   if (normals == DcTrue)
        nrmdata = (DtReal *) space_allocate ((DtInt)(2 * (vgridcount+1) * 
                                (vgridcount-1) * 3 * sizeof(DtReal)));
   else
        nrmdata = DcNullPtr;

   if (colors == DcTrue)
        clrdata = (DtReal *) space_allocate ((DtInt)(2 * (vgridcount+1) * 
                                (vgridcount-1) * 3 * sizeof(DtReal)));
   else
        clrdata = DcNullPtr;


   locptr = locdata;
   nrmptr = nrmdata;
   clrptr = clrdata;
   uvptr  = vertuv;
   uvwptr = vertuvw;

   /* Loop over all of the grid vertices on the surface. */

   for (xgridloc=0;  xgridloc < vgridcount-1;  ++xgridloc) {
      xgridval[0] = -1 + ((DtReal)(xgridloc  )*gridincr);
      xgridval[1] = -1 + ((DtReal)(xgridloc+1)*gridincr);

      for (ygridloc=0;  ygridloc < vgridcount;  ++ygridloc) {
	 ygridval[0] = -1 + ((DtReal)ygridloc*gridincr);
	 ygridval[1] = -1 + ((DtReal)ygridloc*gridincr);
	 zgridval[0] = wave_surf (xgridval[0], ygridval[0]);
	 zgridval[1] = wave_surf (xgridval[1], ygridval[1]);

	 /* vertex 1 */

	 /* Setup the six vertices for the two triangles in this square */

	 start = (ygridloc == 0) ? 0 : 1;
	 end   = (ygridloc == (vgridcount-1)) ? 3 : 2;

	 for (ii=start;  ii <= end;  ii++) {
	    auto DtReal Lx, Ly, Lz;

	    Lx = xgridval [vtx[ii]];
	    Ly = ygridval [vtx[ii]];
	    Lz = zgridval [vtx[ii]];

	    locptr[0] = Lx;
	    locptr[1] = Ly;
	    locptr[2] = Lz;

	    if (normals == DcTrue) {
	       normalize_vector(-wave_surf_dx(Lx,Ly),
				-wave_surf_dy(Lx,Ly),
				-1, nvector);
	       *nrmptr++ = nvector[0];
	       *nrmptr++ = nvector[1];
	       *nrmptr++ = nvector[2];
	    }

	    if (colors == DcTrue) {
	       *clrptr++ = (1 + locptr[0]) * .5;
	       *clrptr++ = (1 + locptr[1]) * .5;
	       *clrptr++ = (1 + locptr[2]) * .5;
	    }

	    if (texture & TEX2D) {
	       *uvptr++ = (locptr[0] + 1) / 2;
	       *uvptr++ = 1 - ((locptr[1] + 1) / 2);
	    }
	    else if (texture & TEX3D) {
	       *uvwptr++ = (locptr[0] + 1) / 2;
	       *uvwptr++ = 1 - ((locptr[1] + 1) / 2);
	       *uvwptr++ = (locptr[2] + HEIGHT) / (2 * HEIGHT);
            }
	    locptr += 3;
	 }
      }
   }

   vartristrip = DoVarTriangleStrip
		 (  DcRGB, (2 * (vgridcount + 1)) * (vgridcount-1),
		    locdata, nrmdata, clrdata
		 );

   DsHoldObj (vartristrip);

   if (texture & TEX2D)
        DpUpdVarTriangleStripUV (vartristrip, 0, vertuv);
   if (texture & TEX3D)
        DpUpdVarTriangleStripUVW (vartristrip, 0, vertuvw);

   return vartristrip;
}



/*==========================================================================
==
==========================================================================*/

void makeobjects (void)
{
   CallbackDataPtr p;

   for (p=routines; p->name != 0; p++) {
      DsSetObjNameString
      (  DsHoldObj (DoCallback (execute_callback_geom,DoDataPtr(p))),
	 p->name,
	 DcFalse
      );
   }

   DsSetObjNameString (
      DsHoldObj (DoCallback (updvartrimsh_callback, DcNullObject)),
      "updvartrimsh", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback (updvarpntlst_callback, DcNullObject)),
      "updvarpntlst", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback (updvarlinlst_callback, DcNullObject)),
      "updvarlinlst", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback (updvartristrip_callback, DcNullObject)),
      "updvartristrip", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback (updvarsimplymsh_callback, DcNullObject)),
      "updvarsimplymsh", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback (updvarquadgrid_callback, DcNullObject)),
      "updvarquadgrid", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback ((DtPFI)uv_callback1, DcNullObject)),
      "uv1", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback ((DtPFI)uv_callback2, DcNullObject)),
      "uv2", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback ((DtPFI)uvw_callback1, DcNullObject)),
      "uvw1", DcFalse);
   DsSetObjNameString (
      DsHoldObj (DoCallback ((DtPFI)uvw_callback2, DcNullObject)),
      "uvw2", DcFalse);
}



/*==========================================================================
==  This routine prompts the user for numeric input and stores the response
==  in the result parameter.  If the user hits return without entering a
=== number, prompt() stores the default value in the result.
==========================================================================*/

void 
Prompt (
    char *string,	    /* Prompt String */
    DtInt default_value,    /* Default Value for Carriage-Return Entry */
    DtInt *result	    /* Resulting Value */
)
{
   static char   buff[81];	/* Input Buffer */

   printf ("    %s:  ", string);
   *result = default_value;
   fgets (buff, sizeof(buff), stdin);
   sscanf (buff, "%d", result);
}



/*==========================================================================
==
==========================================================================*/

void reset_objects (void)
{
   CallbackDataPtr p;

   object_count = -1;

   for (p=routines; p->name != 0; p++) {
      if (p->object != DcNullObject) {
	 DsReleaseObj(p->object);
	 p->object = DcNullObject;
      }
   }
}



/*==========================================================================
==  This function allocates memory for the routines in this file.  If the
==  memory allocation fails for any reason, it prints an error message and
==  halts execution, otherwise it returns a pointer to the memory block.
==========================================================================*/

DtPtr space_allocate (
    DtInt size)
{
   auto DtPtr ptr;

   if (size < 1) return NULL;

   if (!(ptr = malloc ((unsigned)size))) {
      fputs ("malloc() failed.\n", stderr);
      exit (1);
   }
   return ptr;
}



/*==========================================================================
==  This routine deallocates memory blocks returned from space_allocate().
==========================================================================*/

void space_deallocate (
    DtPtr ptr)
{
   if (ptr) free (ptr);
}



/*==========================================================================
==
==========================================================================*/

int updvarlinlst_callback (void)
{
   DtInt i;
   DtInt count;

   count = (DtInt)(vgridcount*vgridcount/2);
   if (updvar1) {
      for (i=0; i<count; i++) {
	 locdata[3*i+2] += .1;
      }
      DpUpdVarLineList(varlinlst, 0, locdata, nrmdata, clrdata);
      updvar1 = DcFalse;
   } else if (updvar2) {
      for (i=0; i<count; i++) {
	 locdata[3*i+2] -= .1;
      }
      DpUpdVarLineList(varlinlst, 0, locdata, nrmdata, clrdata);
      updvar2 = DcFalse;
   }
   return 0;
}



/*==========================================================================
==
==========================================================================*/

int updvarpntlst_callback (void)
{
   DtInt i;
   DtInt count;

   count = (DtInt)(vgridcount*vgridcount/2);

   if (updvar1) {
      for (i=0; i<count; i++) {
	 locdata[3*i+2] += .1;
      }
      DpUpdVarPointList(varpntlst, 0, locdata, nrmdata, locdata);
      updvar1 = DcFalse;
   }
   else if (updvar2) {
      for (i=0; i<count; i++) {
	 locdata[3*i+2] -= .1;
      }
      DpUpdVarPointList(varpntlst, 0, locdata, nrmdata, locdata);
      updvar2 = DcFalse;
   }
   return 0;
}



/*==========================================================================
==
==========================================================================*/

int updvarsimplymsh_callback (void)
{
   auto DtInt ii;

   if (updvar1) {
      for (ii=0;  ii < 8;  ++ii) {
	 vertloc[3*ii] *= 1.1;
      }
      DpUpdVarSimplePolygonMesh (varsimplymsh, vertloc,
				 (DtReal *)DcNullPtr, vertclr,
      				 DcConcave, DcFalse, DcFalse);
      updvar1 = DcFalse;
   } else if (updvar2) {
      for (ii=0;  ii < 8;  ++ii) {
	 vertloc[3*ii] /= 1.1;
      }
      DpUpdVarSimplePolygonMesh (varsimplymsh, vertloc,
				 (DtReal *)DcNullPtr, vertclr,
      				 DcConcave, DcTrue, DcFalse);
      updvar2 = DcFalse;
   }
   return 0;
}

int updvarquadgrid_callback (void)
{
   auto DtInt i;

   if (updvar1) {
      for (i=0; i<150; i++) {
         vertloc[3*i+2] += .1;
      }
      DpUpdVarQuadGrid (varquadgrid, vertloc,
			(DtReal *)DcNullPtr, vertclr,
      			DcFalse);
      updvar1 = DcFalse;
   } else if (updvar2) {
      for (i=0; i<150; i++) {
         vertloc[3*i+2] += .1;
      }
      DpUpdVarQuadGrid (varsimplymsh, vertloc,
			(DtReal *)DcNullPtr, vertclr,
      			DcFalse);
      updvar2 = DcFalse;
   }
   return 0;
}



/*==========================================================================
==
==========================================================================*/

int updvartrimsh_callback (void)
{
   DtInt i;
   DtInt count;

   count = (DtInt)(vgridcount*vgridcount/2);

   if (updvar1) {
      for (i=0; i<count; i++) {
         locdata[3*i+2] += .1;
      }
      DpUpdVarTriangleMesh (vartrimsh, locdata, nrmdata, clrdata, DcFalse);
      updvar1 = DcFalse;
   }
   else if (updvar2) {
      for (i=0; i<count; i++) {
         locdata[3*i+2] -= .1;
      }
      DpUpdVarTriangleMesh (vartrimsh, locdata, nrmdata, clrdata, DcFalse);
      updvar2 = DcFalse;
   }
   return 0;
}



/*==========================================================================
==
==========================================================================*/

int updvartristrip_callback (void)
{
   DtInt i;
   DtInt count;

   count = (DtInt)(vgridcount*vgridcount/2);

   if (updvar1) {
      for (i=0; i<count; i++) {
	 locdata[3*i+2] += .1;
      }
      DpUpdVarTriangleStrip (vartristrip, 0, locdata, nrmdata, clrdata);
      updvar1 = DcFalse;
   }
   else if (updvar2) {
      for (i=0; i<count; i++) {
	 locdata[3*i+2] -= .1;
      }
      DpUpdVarTriangleStrip (vartristrip, 0, locdata, nrmdata, clrdata);
      updvar2 = DcFalse;
   }
   return 0;
}



/*==========================================================================
==
==========================================================================*/

int uv_callback1 (
    Dt32Bits data,
    DtReal x,
    DtReal y,
    DtReal z,
    DtReal nx,
    DtReal ny,
    DtReal nz,
    DtReal *u,
    DtReal *v)
{
   *u = (DtReal) fabs (nx);
   *v = (DtReal) fabs (ny);
   return 0;
}



/*==========================================================================
==
==========================================================================*/

int uv_callback2 (
    Dt32Bits data,
    DtReal x,
    DtReal y,
    DtReal z,
    DtReal nx,
    DtReal ny,
    DtReal nz,
    DtReal *u,
    DtReal *v)
{
   *u = (DtReal) fabs ((ny + nz) / 2);
   *v = (DtReal) fabs ((nx + nz) / 2);
   return 0;
}



/*==========================================================================
==
==========================================================================*/

int uvw_callback1 (
    Dt32Bits data,
    DtReal x,
    DtReal y,
    DtReal z,
    DtReal nx,
    DtReal ny,
    DtReal nz,
    DtReal *u,
    DtReal *v,
    DtReal *w)
{
   *u = (DtReal) fabs (nx);
   *v = (DtReal) fabs (ny);
   *w = (DtReal) fabs (nz);
   return 0;
}



/*==========================================================================
==
==========================================================================*/

int uvw_callback2 (
    Dt32Bits data,
    DtReal x,
    DtReal y,
    DtReal z,
    DtReal nx,
    DtReal ny,
    DtReal nz,
    DtReal *u,
    DtReal *v,
    DtReal *w)
{
   *u = (DtReal) fabs ((ny + nz) / 2);
   *v = (DtReal) fabs ((nx + nz) / 2);
   *w = (DtReal) fabs ((nx + ny) / 2);
   return 0;
}

/*==========================================================================
==
==========================================================================*/
void normalize_vector (
    DtReal x,
    DtReal y,
    DtReal z,
    DtRealTriple nvector)
{
    auto DtReal       length;

    length = (x*x) + (y*y) + (z*z);

    if (length == 0.) {
	nvector[0] = nvector[1] = nvector[2] = 0.;
    } else {
	length = 1. / sqrt((double)length);
	nvector[0] = x * length;
	nvector[1] = y * length;
	nvector[2] = z * length;
    }
}

/*==========================================================================
==
==========================================================================*/

DtObject obsolete_callback_name (void)
{
   printf("\nWARNING: obsolete callback name:\n");
   printf("Old name       New name\n");
   printf("-----------------------\n");
   printf("vtrimsh  -->   trimsh\n");
   printf("vtrilst  -->   trilst\n");
   printf("vpollin  -->   pollin\n");
   printf("vlinlst  -->   linlst\n\n");

   return(DcNullObject);
}
