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
 
/*  ======================================================================
    This file contains the display and studio objects for the executable:

	dyn_texture
    
    The display group is a textured triangle mesh.  
    ====================================================================== */

#include <stdio.h>
#include <math.h>
#include <dore/dore.h>

int light_ambient_callback();
int light_infinite_callback();
int ambient_intensity_callback();
int diffuse_intensity_callback();
int interptype_callback();
int reptype_callback();
int specular_callback();
int backface_callback();
int rotate_callback();
int scale_callback();
int subdivspec_callback();
int translate_callback();
int depthcue_callback();
int texture_extend_callback();
int texture_scale_callback();
int texture_translate_callback();
int texture_antialias_callback();
int texture_op_callback();

DtObject make_texture_triangle_mesh();


/*
  ======================================================================
 */
DtObject makeobject()
{
   DtObject object;
   DtReal color[4];
   DtReal pos[3];
   DtObject testobject;
   DtObject texture;
   extern char texture_raster_file[];

   testobject = make_texture_triangle_mesh();
   texture = DoFileRaster(texture_raster_file, DcNullPtr);

   object = DoGroup(DcTrue);
   
	DgAddObj(DoPickSwitch(DcOn));

   	color[0] = 0.;
	color[1] = 0.;
	color[2] = 0.;
	DgAddObj(DoDepthCue(1.,0.,1.,0.,DcRGB,color));

	DgAddObj(DoCallback(depthcue_callback, DcNullObject));   

	DgAddObj(DoCallback(backface_callback, DcNullObject));
	DgAddObj(DoBackfaceCullable(DcOn));

	DgAddObj(DoCallback(reptype_callback, DcNullObject));
	DgAddObj(DoCallback(interptype_callback, DcNullObject));

	color[0] = .99;
	DgAddObj(DoSubDivSpec(DcSubDivFixed, color));

	DgAddObj(DoCallback(translate_callback, DcNullObject));

	DgAddObj(DoCallback(rotate_callback, DcNullObject));

	DgAddObj(DoCallback(scale_callback, DcNullObject));

	DgAddObj(DoCallback(specular_callback, DcNullObject));

	DgAddObj(DoTextureMapDiffuseColorSwitch(DcOn));
	DgAddObj(DoCallback(texture_extend_callback, DcNullObject));
	DgAddObj(DoCallback(texture_scale_callback, DcNullObject));
	DgAddObj(DoCallback(texture_translate_callback, DcNullObject));
	DgAddObj(DoCallback(texture_antialias_callback, DcNullObject));
	DgAddObj(DoCallback(texture_op_callback, DcNullObject));
	DgAddObj(DoTextureMapDiffuseColor(DcMapReplace, DcStdTableLookup,
		texture));
	DgAddObj(DoTextureMapDiffuseColorSwitch(DcOn));

	DgAddObj(DoPickID (1));

	DgAddObj(testobject);

   DgClose();

   return(object);
}

/****************************************************************************
 **  Triangle Mesh
 **/
DtObject
make_texture_triangle_mesh()
{
   DtVertexType colortype;
   DtInt i;
   DtInt ele_per_vtx;
   DtInt gridcount;
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal *vertices;
   DtReal *vertexptr;
   DtInt *triangles;
   DtInt *triptr;

   printf("\nenter gridcount for triangle mesh:  ");
   scanf("%d",&gridcount);

   if (gridcount < 1) return DcNullObject;

   printf ("enter vertex type for points (0 Loc, 1 LocNrm, 2 LocClr, 3 LocNrmClr): ");
   scanf("%d", &i);
         
   i = (i < 0) ? 0 : (i > 3) ? 3 : i;
   switch (i) {
   case 0:
      colortype = DcLoc;
      ele_per_vtx = 5; 
      break;
   case 1:
      colortype = DcLocNrm;
      ele_per_vtx = 8;
      break;
   case 2:
      colortype = DcLocClr;
      ele_per_vtx = 8;
      break;
   case 3:
      colortype = DcLocNrmClr;
      ele_per_vtx = 11;
      break;
   }
   if (gridcount > 1) {
      gridincr = 2. / (DtReal)(gridcount-1);

      vertices = (DtReal *)malloc(
                	gridcount*gridcount*ele_per_vtx*(sizeof (DtReal)));

      triangles = (DtInt *)malloc(
			2*3*(gridcount-1)*(gridcount-1)*(sizeof (DtInt)));
   } else if (gridcount == 1) {
      vertices = (DtReal *)malloc(3*ele_per_vtx*(sizeof (DtReal)));

      triangles = (DtInt *)malloc(3*(sizeof (DtInt)));
   }

   vertexptr = vertices;

   if (gridcount > 1) {
      for (xgridloc=0;xgridloc<gridcount;xgridloc++) {
         for (ygridloc=0;ygridloc<gridcount;ygridloc++) {
            vertexptr[0] = -1. + ((DtReal)xgridloc*gridincr);
            vertexptr[1] = -1. + ((DtReal)ygridloc*gridincr);
            vertexptr[2] = .25 * sin(7*(vertexptr[0]+vertexptr[1]));
 
            switch (colortype) {
               case DcLocNrm:
                  vertexptr[3] = 0.;
                  vertexptr[4] = 0.;
                  vertexptr[5] = 1.;
               break;

               case DcLocClr:
                  vertexptr[3] = (1.+vertexptr[0]) * .5;
                  vertexptr[4] = (1.+vertexptr[1]) * .5;
                  vertexptr[5] = (1.+vertexptr[2]) * .5;
               break;

               case DcLocNrmClr:
                  vertexptr[3] = 0.;
                  vertexptr[4] = 0.;
                  vertexptr[5] = 1.;

                  vertexptr[6] = (1.+vertexptr[0]) * .5;
                  vertexptr[7] = (1.+vertexptr[1]) * .5;
                  vertexptr[8] = (1.+vertexptr[2]) * .5;
               break;
            }
	    vertexptr[ele_per_vtx-2] = (DtReal)xgridloc / (DtReal)(gridcount-1);
	    vertexptr[ele_per_vtx-1] = (DtReal)(gridcount-1-ygridloc) / 
			(DtReal)(gridcount-1);
            vertexptr += ele_per_vtx;
         }
      }
      triptr = triangles;

      for (xgridloc=0;xgridloc<gridcount-1;xgridloc++) {
         for (ygridloc=0;ygridloc<gridcount-1;ygridloc++) {
            triptr[0] = ygridloc + xgridloc*gridcount;
            triptr[1] = ygridloc + (xgridloc+1)*gridcount;
            triptr[2] = (ygridloc+1) + xgridloc*gridcount;

            triptr[3] = ygridloc + (xgridloc+1)*gridcount;
            triptr[4] = (ygridloc+1) + (xgridloc+1)*gridcount;
            triptr[5] = (ygridloc+1) + xgridloc*gridcount;

            triptr += 6;
         }
      }
   } else {
      vertexptr = vertices;
      switch (colortype) {
         case DcLoc:
            vertices[0] = 0.;
            vertices[1] = 0.;
            vertices[2] = 0.;
            vertexptr[3] = 0;
            vertexptr[4] = 1;
            vertexptr += ele_per_vtx;
   
            vertices[0] = 0.;
            vertices[1] = 1.;
            vertices[2] = 0.;
            vertexptr[3] = 0;
            vertexptr[4] = 0;
            vertexptr += ele_per_vtx;
            
            vertices[0] = 1.;
            vertices[1] = 0.;
            vertices[2] = 0.;
            vertexptr[3] = 1;
            vertexptr[4] = 1;
            vertexptr += ele_per_vtx;
            break;
   
         case DcLocNrm:
            vertices[0] = 0.;
            vertices[1] = 0.;
            vertices[2] = 0.;
            vertices[3] = 0.;
            vertices[4] = 0.;
            vertices[5] = 1.;
            vertexptr[6] = 0;
            vertexptr[7] = 1;
            vertexptr += ele_per_vtx;
   
            vertices[0] = 0.;
            vertices[1] = 1.;
            vertices[2] = 0.;
            vertices[3] = 0.;
            vertices[4] = 0.;
            vertices[5] = 1.;
            vertexptr[6] = 0;
            vertexptr[7] = 0;
            vertexptr += ele_per_vtx;
            
            vertices[0] = 1.;
            vertices[1] = 0.;
            vertices[2] = 0.;
            vertices[3] = 0.;
            vertices[4] = 0.;
            vertices[5] = 1.;
            vertexptr[6] = 1;
            vertexptr[7] = 1;
            vertexptr += ele_per_vtx;
            break;
   
         case DcLocClr:
            vertices[0] = 0.;
            vertices[1] = 0.;
            vertices[2] = 0.;
            vertices[3] = (1.+vertices[0]) * .5;
            vertices[4] = (1.+vertices[1]) * .5;
            vertices[5] = (1.+vertices[2]) * .5;
            vertexptr[6] = 0;
            vertexptr[7] = 1;
            vertexptr += ele_per_vtx;
   
            vertices[0] = 0.;
            vertices[1] = 1.;
            vertices[2] = 0.;
            vertices[3] = (1.+vertices[0]) * .5;
            vertices[4] = (1.+vertices[1]) * .5;
            vertices[5] = (1.+vertices[2]) * .5;
            vertexptr[6] = 0;
            vertexptr[7] = 0;
            vertexptr += ele_per_vtx;
   
            vertices[0] = 1.;
            vertices[1] = 0.;
            vertices[2] = 0.;
            vertices[3] = (1.+vertices[0]) * .5;
            vertices[4] = (1.+vertices[1]) * .5;
            vertices[5] = (1.+vertices[2]) * .5;
            vertexptr[6] = 1;
            vertexptr[7] = 1;
            vertexptr += ele_per_vtx;
            break;
   
         case DcLocNrmClr:
            vertices[0] = 0.;
            vertices[1] = 0.;
            vertices[2] = 0.;
            vertices[3] = 0.;
            vertices[4] = 0.;
            vertices[5] = 1.;
            vertices[6] = (1.+vertices[0]) * .5;
            vertices[7] = (1.+vertices[1]) * .5;
            vertices[8] = (1.+vertices[2]) * .5;
            vertexptr[9] = 0;
            vertexptr[10] = 1;
            vertexptr += ele_per_vtx;
   
            vertices[0] = 0.;
            vertices[1] = 1.;
            vertices[2] = 0.;
            vertices[3] = 0.;
            vertices[4] = 0.;
            vertices[5] = 1.;
            vertices[6] = (1.+vertices[0]) * .5;
            vertices[7] = (1.+vertices[1]) * .5;
            vertices[8] = (1.+vertices[2]) * .5;
            vertexptr[9] = 0;
            vertexptr[10] = 0;
            vertexptr += ele_per_vtx;
   
            vertices[0] = 1.;
            vertices[1] = 0.;
            vertices[2] = 0.;
            vertices[3] = 0.;
            vertices[4] = 0.;
            vertices[5] = 1.;
            vertices[6] = (1.+vertices[0]) * .5;
            vertices[7] = (1.+vertices[1]) * .5;
            vertices[8] = (1.+vertices[2]) * .5;
            vertexptr[9] = 1;
            vertexptr[10] = 1;
            vertexptr += ele_per_vtx;
	    break;
      }
   
      triangles[0] = 0;
      triangles[1] = 1;
      triangles[2] = 2;
   }

   if (gridcount == 1) {
      return (DoTriangleMesh(DcRGB, colortype | DsTextureUVCount(1), 3, 
			vertices, 1, triangles, DcTrue));
   } else {
      return (DoTriangleMesh(DcRGB, colortype | DsTextureUVCount(1),
			gridcount*gridcount,vertices,
                        2*(gridcount-1)*(gridcount-1),triangles, DcTrue));
   }
}
/*
  ======================================================================
 */
DtObject makestudio()
{
   DtObject studio;
   static DtPoint3	at		= {0., 0., 0.};
   static DtPoint3	camfrom		= {0., 0., 10.};
   static DtPoint3	lightfrom	= {-1., 1., 1.};
   static DtVector3	up		= {0., 1., 0.};
   static DtReal	white[3]	= {1., 1., 1.};

   studio = DoGroup(DcTrue);

	DgAddObj(DoPerspective(30.,-5.,-15.));

	DgAddObj(DoGlbRendMaxObjs(1));
	DgAddObj(DoGlbRendMaxSub(5));

	DgAddObj(DoPushMatrix());
		DgAddObj(DoLookAtFrom(at, camfrom, up));
		DgAddObj(DoCamera());
	DgAddObj(DoPopMatrix());

	DgAddObj(DoLightColor(DcRGB, white));
	DgAddObj(DoCallback(ambient_intensity_callback, DcNullObject));
	DgAddObj(DoLightType(DcLightAmbient));
	DgAddObj(DoLight());

	DgAddObj(DoPushMatrix());
		DgAddObj(DoLookAtFrom(at, lightfrom, up));
		DgAddObj(DoLightColor(DcRGB, white));
		DgAddObj(DoCallback(diffuse_intensity_callback, DcNullObject));
		DgAddObj(DoLightType(DcLightInfinite));
		DgAddObj(DoLight());
	DgAddObj(DoPopMatrix());

   DgClose();

   return(studio);
}
