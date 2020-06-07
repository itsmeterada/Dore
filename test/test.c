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

	dyn_test
    
    The display group that is created depends on the command line flag.
    It will be one of a triangle list, triangle mesh, triangle strip,
    line list, connected line list, point list, text, or annotation text.  
    This module is primarily intended for the individual testing of the
    different primitive geometry types using the Dynamic Renderer.
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

DtObject make_triangle_list();
DtObject make_triangle_mesh();
DtObject make_triangle_strip();
DtObject make_line_list();
DtObject make_cline_list();
DtObject make_point_list();
DtObject make_text();
DtObject make_anntxt();

extern DtInt testcase;

/*
  ======================================================================
 */
DtObject makeobject()
{
   DtObject object;
   DtReal color[4];
   DtReal pos[3];
   DtObject testobject;

   switch (testcase) {
   	case 0:
		testobject = make_triangle_list();
		break;
	case 1:
		testobject = make_triangle_mesh();
		break;
	case 2:
		testobject = make_triangle_strip();
		break;
   	case 3:
		testobject = make_line_list();
		break;
   	case 4:
		testobject = make_cline_list();
		break;
   	case 5:
		testobject = make_point_list();
		break;
   	case 6:
		testobject = make_text();
		break;
   	case 7:
		testobject = make_anntxt();
		break;
   	default:
		fprintf(stderr, "Unknown testcase %d\n", testcase);
		break;
   }
		
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

	DgAddObj(DoCallback(subdivspec_callback, DcNullObject));

	DgAddObj(DoCallback(translate_callback, DcNullObject));

	DgAddObj(DoCallback(rotate_callback, DcNullObject));

	DgAddObj(DoCallback(scale_callback, DcNullObject));

	DgAddObj(DoCallback(specular_callback, DcNullObject));

	DgAddObj(DoPickID (1));

	DgAddObj(testobject);

   DgClose();

   return(object);
}

/***********************************************************************
 **  Triangle List
 **/

DtObject
make_triangle_list()

{
   DtVertexType colortype;
   DtInt i;
   DtInt ele_per_vtx;
   DtInt gridcount;
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal xgridval[4], ygridval[4], zgridval[4];
   DtReal rclrval[4], gclrval[4], bclrval[4];
   DtReal *vertices;
   DtReal *vertexptr;
   static DtInt vtx[6] = { 0, 2, 1, 2, 3, 1};

   printf("\nenter gridcount for triangle list:  ");
   scanf("%d",&gridcount);

   if (gridcount < 1) return DcNullObject;
 
   printf ("enter vertex type for lines (0 Loc, 1 LocNrm, 2 LocClr, 3 LocNrmClr): ");
   scanf("%d", &i);
         
   i = (i < 0) ? 0 : (i > 3) ? 3 : i;
   switch (i) {
   case 0:
      colortype = DcLoc;
      ele_per_vtx = 3; 
      break;
   case 1:
      colortype = DcLocNrm;
      ele_per_vtx = 6;
      break;
   case 2:
      colortype = DcLocClr;
      ele_per_vtx = 6;
      break;
   case 3:
      colortype = DcLocNrmClr;
      ele_per_vtx = 9;
      break;
   }

   if (gridcount > 1) {
        gridincr = 2. / (DtReal)(gridcount-1);
   	vertices = (DtReal *)malloc(
		3*2*(gridcount-1)*(gridcount-1)*ele_per_vtx*(sizeof (DtReal)));
   } else if (gridcount == 1) {
   	vertices = (DtReal *)malloc(3*2*ele_per_vtx*(sizeof (DtReal)));
   }

   vertexptr = vertices;

   for (xgridloc=0;xgridloc<gridcount-1;xgridloc++) {
      for (ygridloc=0;ygridloc<gridcount-1;ygridloc++) {
         xgridval[0] = -1. + ((DtReal)xgridloc*gridincr);
         ygridval[0] = -1. + ((DtReal)ygridloc*gridincr);
         zgridval[0] = .25 * sin(7*(xgridval[0]+ygridval[0]));
         rclrval[0] = (1.+xgridval[0]) * .5;
         gclrval[0] = (1.+ygridval[0]) * .5;
         bclrval[0] = (1.+zgridval[0]) * .5;

         xgridval[1] = -1. + ((DtReal)(xgridloc+1)*gridincr);
         ygridval[1] = -1. + ((DtReal)ygridloc*gridincr);
         zgridval[1] = .25 * sin(7*(xgridval[1]+ygridval[1]));
         rclrval[1] = (1.+xgridval[1]) * .5;
         gclrval[1] = (1.+ygridval[1]) * .5;
         bclrval[1] = (1.+zgridval[1]) * .5;

         xgridval[2] = -1. + ((DtReal)xgridloc*gridincr);
         ygridval[2] = -1. + ((DtReal)(ygridloc+1)*gridincr);
         zgridval[2] = .25 * sin(7*(xgridval[2]+ygridval[2]));
         rclrval[2] = (1.+xgridval[2]) * .5;
         gclrval[2] = (1.+ygridval[2]) * .5;
         bclrval[2] = (1.+zgridval[2]) * .5;

         xgridval[3] = -1. + ((DtReal)(xgridloc+1)*gridincr);
         ygridval[3] = -1. + ((DtReal)(ygridloc+1)*gridincr);
         zgridval[3] = .25 * sin(7*(xgridval[3]+ygridval[3]));
         rclrval[3] = (1.+xgridval[3]) * .5;
         gclrval[3] = (1.+ygridval[3]) * .5;
         bclrval[3] = (1.+zgridval[3]) * .5;

         /* vertex 1 */
         
         /* Setup the siz vertices for the two triangles in this square */
           
         for (i=0; i<6; i++) {
            vertexptr[0] = xgridval[vtx[i]];
            vertexptr[1] = ygridval[vtx[i]];
            vertexptr[2] = zgridval[vtx[i]];

            switch (colortype) {
            case DcLoc:
               vertexptr += 3;
               break;

            case DcLocNrm:
               vertexptr[3] = 0.;
               vertexptr[4] = 0.;
               vertexptr[5] = -1.;
               vertexptr += 6;
               break;

            case DcLocClr:
               vertexptr[3] = rclrval[vtx[i]];
               vertexptr[4] = gclrval[vtx[i]];
               vertexptr[5] = bclrval[vtx[i]];
               vertexptr += 6;
               break;

            case DcLocNrmClr:
               vertexptr[3] = 0.;
               vertexptr[4] = 0.;
               vertexptr[5] = -1.;

               vertexptr[6] = rclrval[vtx[i]];
               vertexptr[7] = gclrval[vtx[i]];
               vertexptr[8] = bclrval[vtx[i]];

               vertexptr +=9;
            }
         }
      }
   }

   if (gridcount == 1) {
      switch (colortype) {
      case DcLoc:
         vertices[0] = 0.;
         vertices[1] = 0.;
         vertices[2] = 0.;

         vertices[3] = 0.;
         vertices[4] = 1.;
         vertices[5] = 0.;
         
         vertices[6] = 1.;
         vertices[7] = 0.;
         vertices[7] = 0.;
         break;

      case DcLocNrm:
         vertices[0] = 0.;
         vertices[1] = 0.;
         vertices[2] = 0.;
         vertices[3] = 0.;
         vertices[4] = 0.;
         vertices[5] = -1.;

         vertices[6] = 0.;
         vertices[7] = 1.;
         vertices[8] = 0.;
         vertices[9] = 0.;
         vertices[10] = 0.;
         vertices[11] = -1.;
         
         vertices[12] = 1.;
         vertices[13] = 0.;
         vertices[14] = 0.;
         vertices[15] = 0.;
         vertices[16] = 0.;
         vertices[17] = -1.;
         break;

      case DcLocClr:
         vertices[0] = 0.;
         vertices[1] = 0.;
         vertices[2] = 0.;
         vertices[3] = (1.+vertices[0]) * .5;
         vertices[4] = (1.+vertices[1]) * .5;
         vertices[5] = (1.+vertices[2]) * .5;

         vertices[6] = 0.;
         vertices[7] = 1.;
         vertices[8] = 0.;
         vertices[9] = (1.+vertices[6]) * .5;
         vertices[10] = (1.+vertices[7]) * .5;
         vertices[11] = (1.+vertices[8]) * .5;

         vertices[12] = 1.;
         vertices[13] = 0.;
         vertices[14] = 0.;
         vertices[15] = (1.+vertices[12]) * .5;
         vertices[16] = (1.+vertices[13]) * .5;
         vertices[17] = (1.+vertices[14]) * .5;
         break;
 
      case DcLocNrmClr:
         vertices[0] = 0.;
         vertices[1] = 0.;
         vertices[2] = 0.;
         vertices[3] = 0.;
         vertices[4] = 0.;
         vertices[5] = -1.;
         vertices[6] = (1.+vertices[0]) * .5;
         vertices[7] = (1.+vertices[1]) * .5;
         vertices[8] = (1.+vertices[2]) * .5;

         vertices[9] = 0.;
         vertices[10] = 1.;
         vertices[11] = 0.;
         vertices[12] = 0.;
         vertices[13] = 0.;
         vertices[14] = -1.;
         vertices[15] = (1.+vertices[9]) * .5;
         vertices[16] = (1.+vertices[10]) * .5;
         vertices[17] = (1.+vertices[11]) * .5;
 
         vertices[18] = 1.;
         vertices[19] = 0.;
         vertices[20] = 0.;
         vertices[21] = 0.;
         vertices[22] = 0.;
         vertices[23] = -1.;
         vertices[24] = (1.+vertices[18]) * .5;
         vertices[25] = (1.+vertices[19]) * .5;
         vertices[26] = (1.+vertices[20]) * .5;
      }
   }

   if (gridcount == 1) {
      return (DoTriangleList(DcRGB,colortype,1,vertices));
   } else {
      return (DoTriangleList(DcRGB,colortype,2*(gridcount-1)*(gridcount-1),
                             vertices));
   }
}


/****************************************************************************
 **  Triangle Mesh
 **/
DtObject
make_triangle_mesh()
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
      ele_per_vtx = 3; 
      break;
   case 1:
      colortype = DcLocNrm;
      ele_per_vtx = 6;
      break;
   case 2:
      colortype = DcLocClr;
      ele_per_vtx = 6;
      break;
   case 3:
      colortype = DcLocNrmClr;
      ele_per_vtx = 9;
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

   for (xgridloc=0;xgridloc<gridcount;xgridloc++) {
      for (ygridloc=0;ygridloc<gridcount;ygridloc++) {
         vertexptr[0] = -1. + ((DtReal)xgridloc*gridincr);
         vertexptr[1] = -1. + ((DtReal)ygridloc*gridincr);
         vertexptr[2] = .25 * sin(7*(vertexptr[0]+vertexptr[1]));
 
         switch (colortype) {
         case DcLoc:
            vertexptr += 3;
            break;

         case DcLocNrm:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;
            vertexptr += 6;
            break;

         case DcLocClr:
            vertexptr[3] = (1.+vertexptr[0]) * .5;
            vertexptr[4] = (1.+vertexptr[1]) * .5;
            vertexptr[5] = (1.+vertexptr[2]) * .5;
            vertexptr += 6;
            break;

         case DcLocNrmClr:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;

            vertexptr[6] = (1.+vertexptr[0]) * .5;
            vertexptr[7] = (1.+vertexptr[1]) * .5;
            vertexptr[8] = (1.+vertexptr[2]) * .5;

            vertexptr +=9;
         }
         
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

   if (gridcount == 1) {
      switch (colortype) {
      case DcLoc:
         vertices[0] = 0.;
         vertices[1] = 0.;
         vertices[2] = 0.;

         vertices[3] = 0.;
         vertices[4] = 1.;
         vertices[5] = 0.;
         
         vertices[6] = 1.;
         vertices[7] = 0.;
         vertices[7] = 0.;
         break;

      case DcLocNrm:
         vertices[0] = 0.;
         vertices[1] = 0.;
         vertices[2] = 0.;
         vertices[3] = 0.;
         vertices[4] = 0.;
         vertices[5] = 1.;

         vertices[6] = 0.;
         vertices[7] = 1.;
         vertices[8] = 0.;
         vertices[9] = 0.;
         vertices[10] = 0.;
         vertices[11] = 1.;
         
         vertices[12] = 1.;
         vertices[13] = 0.;
         vertices[14] = 0.;
         vertices[15] = 0.;
         vertices[16] = 0.;
         vertices[17] = 1.;
         break;

      case DcLocClr:
         vertices[0] = 0.;
         vertices[1] = 0.;
         vertices[2] = 0.;
         vertices[3] = (1.+vertices[0]) * .5;
         vertices[4] = (1.+vertices[1]) * .5;
         vertices[5] = (1.+vertices[2]) * .5;

         vertices[6] = 0.;
         vertices[7] = 1.;
         vertices[8] = 0.;
         vertices[9] = (1.+vertices[6]) * .5;
         vertices[10] = (1.+vertices[7]) * .5;
         vertices[11] = (1.+vertices[8]) * .5;

         vertices[12] = 1.;
         vertices[13] = 0.;
         vertices[14] = 0.;
         vertices[15] = (1.+vertices[12]) * .5;
         vertices[16] = (1.+vertices[13]) * .5;
         vertices[17] = (1.+vertices[14]) * .5;
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

         vertices[9] = 0.;
         vertices[10] = 1.;
         vertices[11] = 0.;
         vertices[12] = 0.;
         vertices[13] = 0.;
         vertices[14] = 1.;
         vertices[15] = (1.+vertices[9]) * .5;
         vertices[16] = (1.+vertices[10]) * .5;
         vertices[17] = (1.+vertices[11]) * .5;

         vertices[18] = 1.;
         vertices[19] = 0.;
         vertices[20] = 0.;
         vertices[21] = 0.;
         vertices[22] = 0.;
         vertices[23] = 1.;
         vertices[24] = (1.+vertices[18]) * .5;
         vertices[25] = (1.+vertices[19]) * .5;
         vertices[26] = (1.+vertices[20]) * .5;
      }

      triangles[0] = 0;
      triangles[1] = 1;
      triangles[2] = 2;
   }

   if (gridcount == 1) {
      return (DoTriangleMesh(DcRGB,colortype, 3, vertices, 1, 
                             triangles, DcTrue));
   } else {
      return (DoTriangleMesh(DcRGB,colortype,gridcount*gridcount,vertices,
                             2*(gridcount-1)*(gridcount-1),triangles, 
                             DcTrue));
   }
}

/****************************************************************************
 **  Triangle Strip
 **/
DtObject
make_triangle_strip()
{
   DtVertexType colortype;
   DtInt i;
   DtInt ele_per_vtx;
   DtInt gridcount;
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal xgridval[4], ygridval[4], zgridval[4];
   DtReal rclrval[4], gclrval[4], bclrval[4];
   DtReal uval[4];
   DtReal vval[4];
   DtReal *vertices;
   DtReal *vertexptr;
   static DtInt vtx[4] = { 0, 0, 1, 1 };
   DtInt start, end;

   printf("enter gridcount for triangle strip:  ");
   scanf("%d",&gridcount);

   if (gridcount < 1) return DcNullObject;

   printf ("enter color type for lines (0 Loc, 1 LocNrm, 2 LocClr, 3 LocNrmClr): ");
   scanf("%d", &i);
	
   i = (i < 0) ? 0 : (i > 3) ? 3 : i;
   switch (i) {
   case 0:
      colortype = DcLoc;
      ele_per_vtx = 3; 
      break;
   case 1:
      colortype = DcLocNrm;
      ele_per_vtx = 6;
      break;
   case 2:
      colortype = DcLocClr;
      ele_per_vtx = 6;
      break;
   case 3:
      colortype = DcLocNrmClr;
      ele_per_vtx = 9;
      break;
   }

   printf ("include uv coordinates? (0 no, 1 yes): ");
   scanf ("%d", &i);

   if (gridcount > 1) {
      gridincr = 2. / (DtReal)(gridcount-1);

      vertices = (DtReal *)malloc((2*(gridcount)+2)*(gridcount-1)*ele_per_vtx*(
			sizeof (DtReal)));
   } else {
      vertices = (DtReal *)malloc(3*2*ele_per_vtx*(sizeof (DtReal)));
   }

   vertexptr = vertices;

   for (xgridloc=0;xgridloc<gridcount-1;xgridloc++) {
      for (ygridloc=0;ygridloc<gridcount;ygridloc++) {
	 xgridval[0] = -1. + ((DtReal)xgridloc*gridincr);
	 ygridval[0] = -1. + ((DtReal)ygridloc*gridincr);
	 zgridval[0] = .25 * sin(7*(xgridval[0]+ygridval[0]));
	 rclrval[0] = (1.+xgridval[0]) * .5;
	 gclrval[0] = (1.+ygridval[0]) * .5;
	 bclrval[0] = (1.+zgridval[0]) * .5;

	 xgridval[1] = -1. + ((DtReal)(xgridloc+1)*gridincr);
	 ygridval[1] = -1. + ((DtReal)ygridloc*gridincr);
	 zgridval[1] = .25 * sin(7*(xgridval[1]+ygridval[1]));
	 rclrval[1] = (1.+xgridval[1]) * .5;
	 gclrval[1] = (1.+ygridval[1]) * .5;
	 bclrval[1] = (1.+zgridval[1]) * .5;

	 /* vertex 1 */
	 
	 /* Setup the siz vertices for the two triangles in this square */
	   
	 if (ygridloc==0) {
	    start = 0;
	    end = 2;
	 } else if (ygridloc == gridcount - 1) {
	    start = 1;
	    end = 3;
	 } else {
	    start = 1;
	    end = 2;
	 }

	 for (i=start; i<=end; i++) {
	    vertexptr[0] = xgridval[vtx[i]];
	    vertexptr[1] = ygridval[vtx[i]];
	    vertexptr[2] = zgridval[vtx[i]];

	    switch (colortype) {
	    case DcLocNrm:
	       vertexptr[3] = 0.;
	       vertexptr[4] = 0.;
	       vertexptr[5] = -1.;
	       break;

	    case DcLocClr:
	       vertexptr[3] = rclrval[vtx[i]];
	       vertexptr[4] = gclrval[vtx[i]];
	       vertexptr[5] = bclrval[vtx[i]];
	       break;

	    case DcLocNrmClr:
	       vertexptr[3] = 0.;
	       vertexptr[4] = 0.;
	       vertexptr[5] = -1.;

	       vertexptr[6] = rclrval[vtx[i]];
	       vertexptr[7] = gclrval[vtx[i]];
	       vertexptr[8] = bclrval[vtx[i]];
	       break;
	    }
	    vertexptr += ele_per_vtx;
	 }
      }
   }

   if (gridcount == 1) {
      switch (colortype) {
      case DcLoc:
	 vertices[0] = 0.;
	 vertices[1] = 0.;
	 vertices[2] = 0.;

	 vertices[3] = 0.;
	 vertices[4] = 1.;
	 vertices[5] = 0.;
	 
	 vertices[6] = 1.;
	 vertices[7] = 0.;
	 vertices[7] = 0.;
	 break;

      case DcLocNrm:
	 vertices[0] = 0.;
	 vertices[1] = 0.;
	 vertices[2] = 0.;
	 vertices[3] = 0.;
	 vertices[4] = 0.;
	 vertices[5] = -1.;

	 vertices[6] = 0.;
	 vertices[7] = 1.;
	 vertices[8] = 0.;
	 vertices[9] = 0.;
	 vertices[10] = 0.;
	 vertices[11] = -1.;
	 
	 vertices[12] = 1.;
	 vertices[13] = 0.;
	 vertices[14] = 0.;
	 vertices[15] = 0.;
	 vertices[16] = 0.;
	 vertices[17] = -1.;
	 break;

      case DcLocClr:
	 vertices[0] = 0.;
	 vertices[1] = 0.;
	 vertices[2] = 0.;
	 vertices[3] = (1.+vertices[0]) * .5;
	 vertices[4] = (1.+vertices[1]) * .5;
	 vertices[5] = (1.+vertices[2]) * .5;

	 vertices[6] = 0.;
	 vertices[7] = 1.;
	 vertices[8] = 0.;
	 vertices[9] = (1.+vertices[6]) * .5;
	 vertices[10] = (1.+vertices[7]) * .5;
	 vertices[11] = (1.+vertices[8]) * .5;

	 vertices[12] = 1.;
	 vertices[13] = 0.;
	 vertices[14] = 0.;
	 vertices[15] = (1.+vertices[12]) * .5;
	 vertices[16] = (1.+vertices[13]) * .5;
	 vertices[17] = (1.+vertices[14]) * .5;
	 break;

      case DcLocNrmClr:
	 vertices[0] = 0.;
	 vertices[1] = 0.;
	 vertices[2] = 0.;
	 vertices[3] = 0.;
	 vertices[4] = 0.;
	 vertices[5] = -1.;
	 vertices[6] = (1.+vertices[0]) * .5;
	 vertices[7] = (1.+vertices[1]) * .5;
	 vertices[8] = (1.+vertices[2]) * .5;

	 vertices[9] = 0.;
	 vertices[10] = 1.;
	 vertices[11] = 0.;
	 vertices[12] = 0.;
	 vertices[13] = 0.;
	 vertices[14] = -1.;
	 vertices[15] = (1.+vertices[9]) * .5;
	 vertices[16] = (1.+vertices[10]) * .5;
	 vertices[17] = (1.+vertices[11]) * .5;

	 vertices[18] = 1.;
	 vertices[19] = 0.;
	 vertices[20] = 0.;
	 vertices[21] = 0.;
	 vertices[22] = 0.;
	 vertices[23] = -1.;
	 vertices[24] = (1.+vertices[18]) * .5;
	 vertices[25] = (1.+vertices[19]) * .5;
	 vertices[26] = (1.+vertices[20]) * .5;
      }
   }

   if (gridcount == 1) {
      return (DoTriangleStrip(DcRGB,colortype,3,vertices));
   } else {
      return(DoTriangleStrip(DcRGB,colortype,
			     (2*(gridcount)+2)*(gridcount-1), vertices));
   }
}


/****************************************************************************
 **  Line List
 **/
DtObject
make_line_list()
{
   DtVertexType colortype;
   DtInt i;
   DtInt ele_per_vtx;
   DtInt gridcount;
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal xgridval1, ygridval1, zgridval1;
   DtReal xgridval2, ygridval2, zgridval2;
   DtReal *vertices;
   DtReal *vertexptr;

   printf("\nenter gridcount for lines:  ");
   scanf("%d",&gridcount);

   if (gridcount < 1) return DcNullObject;

   printf ("enter vertex type for lines (0 Loc, 1 LocNrm, 2 LocClr, 3 LocNrmClr): ");
   scanf("%d", &i);
         
   i = (i < 0) ? 0 : (i > 3) ? 3 : i;
   switch (i) {
   case 0:
      colortype = DcLoc;
      ele_per_vtx = 3; 
      break;
   case 1:
      colortype = DcLocNrm;
      ele_per_vtx = 6;
      break;
   case 2:
      colortype = DcLocClr;
      ele_per_vtx = 6;
      break;
   case 3:
      colortype = DcLocNrmClr;
      ele_per_vtx = 9;
      break;
   }

   if (gridcount >= 1) 
         gridincr = 2. / (DtReal)(gridcount-1);

   vertices = (DtReal *)malloc(
                2*gridcount*gridcount*ele_per_vtx*(sizeof (DtReal)));

   vertexptr = vertices;

   for (xgridloc=0;xgridloc<gridcount-1;xgridloc++) {
      for (ygridloc=0;ygridloc<gridcount;ygridloc++) {
         xgridval1 = -1. + ((DtReal)xgridloc*gridincr);
         ygridval1 = -1. + ((DtReal)ygridloc*gridincr);
         zgridval1 = .25 * sin(7*(xgridval1+ygridval1));

         xgridval2 = -1. + ((DtReal)(xgridloc+1)*gridincr);
         ygridval2 = -1. + ((DtReal)ygridloc*gridincr);
         zgridval2 = .25 * sin(7*(xgridval2+ygridval2));

         /* vertex 1 */

         vertexptr[0] = xgridval1;
         vertexptr[1] = ygridval1;
         vertexptr[2] = zgridval1;

         switch (colortype) {
         case DcLoc:
            vertexptr += 3;
            break;

         case DcLocNrm:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;
            vertexptr += 6;
            break;

         case DcLocClr:
            vertexptr[3] = (1.+vertexptr[0]) * .5;
            vertexptr[4] = (1.+vertexptr[1]) * .5;
            vertexptr[5] = (1.+vertexptr[2]) * .5;
            vertexptr += 6;
            break;

         case DcLocNrmClr:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;

            vertexptr[6] = (1.+vertexptr[0]) * .5;
            vertexptr[7] = (1.+vertexptr[1]) * .5;
            vertexptr[8] = (1.+vertexptr[2]) * .5;
 
 
            vertexptr +=9;
         }
 
         /* vertex 2 */

         vertexptr[0] = xgridval2;
         vertexptr[1] = ygridval2;
         vertexptr[2] = zgridval2;

         switch (colortype) {
         case DcLoc:
            vertexptr += 3;
            break;

         case DcLocNrm:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;
            vertexptr += 6;
            break;

         case DcLocClr:
            vertexptr[3] = (1.+vertexptr[0]) * .5;
            vertexptr[4] = (1.+vertexptr[1]) * .5;
            vertexptr[5] = (1.+vertexptr[2]) * .5;
            vertexptr += 6;
            break;

         case DcLocNrmClr:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;
                              
            vertexptr[6] = (1.+vertexptr[0]) * .5;
            vertexptr[7] = (1.+vertexptr[1]) * .5;
            vertexptr[8] = (1.+vertexptr[2]) * .5;
                              
            vertexptr +=9;
         }

      }
   }

   if (gridcount == 1) {
      vertices[0] = 0.;
      vertices[1] = 0.;
      vertices[2] = 0.;

      switch (colortype) {
      case DcLocNrm:
         vertices[3] = 0.;
         vertices[4] = 0.;
         vertices[5] = 1.;
         break;

      case DcLocClr:
         vertices[3] = (1.+vertices[0]) * .5;
         vertices[4] = (1.+vertices[1]) * .5;
         vertices[5] = (1.+vertices[2]) * .5;
         break;

      case DcLocNrmClr:
         vertices[3] = 0.;
         vertices[4] = 0.;
         vertices[5] = 1.;
                              
         vertices[6] = (1.+vertices[0]) * .5;
         vertices[7] = (1.+vertices[1]) * .5;
         vertices[8] = (1.+vertices[2]) * .5;
      }
           
      switch (colortype) {
      case DcLoc:
         vertices[3] = 1.;
         vertices[4] = 0.;
         vertices[5] = 0.;
         break;
                 
      case DcLocNrm:
         vertices[6] = 1.;
         vertices[7] = 0.;
         vertices[8] = 0.;

         vertices[9] = 0.;
         vertices[10] = 0.;
         vertices[11] = 1.;
         break;

      case DcLocClr:
         vertices[6] = 1.;
         vertices[7] = 0.;
         vertices[8] = 0.;
 
         vertices[9] = (1.+vertices[6]) * .5;
         vertices[10] = (1.+vertices[7]) * .5;
         vertices[11] = (1.+vertices[8]) * .5;
      }
   }

   if (gridcount == 1) {
      return (DoLineList(DcRGB,colortype,1,vertices));
   } else {
      return (DoLineList(DcRGB,colortype,(gridcount-1)*gridcount,vertices));
   }
}


/***************************************************************************
 **   Connected Line List
 **/

DtObject
make_cline_list()

{
   DtVertexType colortype;
   DtInt i;
   DtInt ele_per_vtx;
   DtInt gridcount;
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal *vertices;
   DtReal *vertexptr;
 
   printf("\nenter gridcount for connected lines:  ");
   scanf("%d",&gridcount);

   if (gridcount < 1) return DcNullObject;

   printf ("enter vertex type for lines (0 Loc, 1 LocNrm, 2 LocClr, 3 LocNrmClr): ");
   scanf("%d", &i);
         
   i = (i < 0) ? 0 : (i > 3) ? 3 : i;
   switch (i) {
   case 0:
      colortype = DcLoc;
      ele_per_vtx = 3; 
      break;
   case 1:
      colortype = DcLocNrm;
      ele_per_vtx = 6;
      break;
   case 2:
      colortype = DcLocClr;
      ele_per_vtx = 6;
      break;
   case 3:
      colortype = DcLocNrmClr;
      ele_per_vtx = 9;
      break;
   }

   if (gridcount > 1) {
      gridincr = 2. / (DtReal)(gridcount-1);

      vertices = (DtReal *)malloc(
                	gridcount*gridcount*ele_per_vtx*(sizeof (DtReal)));
   } else if (gridcount == 1) {
      vertices = (DtReal *)malloc(2*ele_per_vtx*(sizeof (DtReal)));
   }

   vertexptr = vertices;


   for (xgridloc=0;xgridloc<gridcount;xgridloc++) {
      for (ygridloc=0;ygridloc<gridcount;ygridloc++) {
         vertexptr[0] = -1. + ((DtReal)xgridloc*gridincr);
         vertexptr[1] = -1. + ((DtReal)ygridloc*gridincr);
         vertexptr[2] = .5 * sin(7*(vertexptr[0]+vertexptr[1]));

         switch (colortype) {
         case DcLoc:
            vertexptr += 3;
            break;
 
         case DcLocNrm:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;
            vertexptr += 6;
            break;

         case DcLocClr:
            vertexptr[3] = (1.+vertexptr[0]) * .5;
            vertexptr[4] = (1.+vertexptr[1]) * .5;
            vertexptr[5] = (1.+vertexptr[2]) * .5;
            vertexptr += 6;
            break;

         case DcLocNrmClr:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;

            vertexptr[6] = (1.+vertexptr[0]) * .5;
            vertexptr[7] = (1.+vertexptr[1]) * .5;
            vertexptr[8] = (1.+vertexptr[2]) * .5;

            vertexptr +=9;
         }
      }
   }

   if (gridcount == 1) {
      vertices[0] = 0.;
      vertices[1] = 0.;
      vertices[2] = 0.;
 
      switch (colortype) {
      case DcLocNrm:
         vertices[3] = 0.;
         vertices[4] = 0.;
         vertices[5] = 1.;
         break;

      case DcLocClr:
         vertices[3] = (1.+vertices[0]) * .5;
         vertices[4] = (1.+vertices[1]) * .5;
         vertices[5] = (1.+vertices[2]) * .5;
         break;

      case DcLocNrmClr:
         vertices[3] = 0.;
         vertices[4] = 0.;
         vertices[5] = 1.;
                              
         vertices[6] = (1.+vertices[0]) * .5;
         vertices[7] = (1.+vertices[1]) * .5;
         vertices[8] = (1.+vertices[2]) * .5;
      }
           
      switch (colortype) {
      case DcLoc:
         vertices[3] = 1.;
         vertices[4] = 0.;
         vertices[5] = 0.;
         break;
                 
      case DcLocNrm:
         vertices[6] = 1.;
         vertices[7] = 0.;
         vertices[8] = 0.;

         vertices[9] = 0.;
         vertices[10] = 0.;
         vertices[11] = 1.;
         break;

      case DcLocClr:
         vertices[6] = 1.;
         vertices[7] = 0.;
         vertices[8] = 0.;

         vertices[9] = (1.+vertices[6]) * .5;
         vertices[10] = (1.+vertices[7]) * .5;
         vertices[11] = (1.+vertices[8]) * .5;
         break;

      case DcLocNrmClr:
         vertices[9] = 1.;
         vertices[10] = 0.;
         vertices[11] = 0.;
         
         vertices[12] = 0.;
         vertices[13] = 0.;
         vertices[14] = 1.;
                              
         vertices[15] = (1.+vertices[9]) * .5;
         vertices[16] = (1.+vertices[10]) * .5;
         vertices[17] = (1.+vertices[11]) * .5;
      }
   }

   return(DoPolyline(DcRGB,colortype,gridcount*gridcount,vertices));

}


/***************************************************************************
 **   Point List
 **/


DtObject
make_point_list()

{
   DtVertexType colortype;
   DtInt i;
   DtInt ele_per_vtx;
   DtInt gridcount;
   DtReal gridincr;
   DtInt xgridloc, ygridloc;
   DtReal *vertices;
   DtReal *vertexptr;

   printf("\nenter gridcount for points:  ");
   scanf("%d",&gridcount);

   if (gridcount < 1) return DcNullObject;

   printf ("enter vertex type for points (0 Loc, 1 LocNrm, 2 LocClr, 3 LocNrmClr): ");
   scanf("%d", &i);
         
   i = (i < 0) ? 0 : (i > 3) ? 3 : i;
   switch (i) {
   case 0:
      colortype = DcLoc;
      ele_per_vtx = 3; 
      break;
   case 1:
      colortype = DcLocNrm;
      ele_per_vtx = 6;
   case 2:
      colortype = DcLocClr;
      ele_per_vtx = 6;
      break;
   case 3:
      colortype = DcLocNrmClr;
      ele_per_vtx = 9;
      break;
   }

   if (gridcount > 1)
         gridincr = 2. / (DtReal)(gridcount-1);

   vertices = (DtReal *)malloc(
        		gridcount*gridcount*ele_per_vtx*(sizeof (DtReal)));

   vertexptr = vertices;

   for (xgridloc=0;xgridloc<gridcount;xgridloc++) {
      for (ygridloc=0;ygridloc<gridcount;ygridloc++) {
         vertexptr[0] = -1. + ((DtReal)xgridloc*gridincr);
         vertexptr[1] = -1. + ((DtReal)ygridloc*gridincr);
         vertexptr[2] = .25 * sin(7*(vertexptr[0]+vertexptr[1]));

         switch (colortype) {
         case DcLoc:
            vertexptr += 3;
            break;

         case DcLocNrm:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;
            vertexptr += 6;
            break;

         case DcLocClr:
            vertexptr[3] = (1.+vertexptr[0]) * .5;
            vertexptr[4] = (1.+vertexptr[1]) * .5;
            vertexptr[5] = (1.+vertexptr[2]) * .5;
            vertexptr += 6;
            break;

         case DcLocNrmClr:
            vertexptr[3] = 0.;
            vertexptr[4] = 0.;
            vertexptr[5] = 1.;

            vertexptr[6] = (1.+vertexptr[0]) * .5;
            vertexptr[7] = (1.+vertexptr[1]) * .5;
            vertexptr[8] = (1.+vertexptr[2]) * .5;

            vertexptr +=9;
         }
         
      }
   }

   return(DoPointList(DcRGB,colortype,gridcount*gridcount,vertices));
}



/**************************************************************************
 **  Text
 **/

DtObject
make_text()
{
   DtReal pos[3];
   DtVector3 u,v;
   DtReal color[3];

   DoGroup(DcTrue);

  	DgAddObj(DoTextFont(DcHelvetica));

        pos[0] = -2.5;
        pos[1] = 2.;
        pos[2] = 0.;
        u[0] = 1.;
        u[1] = 0.;
        u[2] = 0.;
        v[0] = 0.;
        v[1] = 1.;
        v[2] = 0.;
	color[0] = 0.0;
 	color[1] = 1.0;
	color[2] = 0.5;
	DgAddObj(DoDiffuseColor(DcRGB, color));
	DgAddObj(DoTextSpace(0.05));
        DgAddObj(DoText(pos,u,v,"Different"));

        DgAddObj(DoTextFont(DcComplexItalic));

        pos[0] = -2.5;
        pos[1] = -1.;
        pos[2] = 0.;
        u[0] = 1.;
        u[1] = 0.;
        u[2] = 0.;
        v[0] = 0.;
        v[1] = 1.;
        v[2] = 0.;
        DgAddObj(DoText(pos,u,v,"Fonts"));

	color[0] = 1.0;
 	color[1] = 0.0;
	color[2] = 1.0;
	DgAddObj(DoDiffuseColor(DcRGB, color));
	DgAddObj(DoScale(0.7, 0.7, 0.7));
	DgAddObj(DoRotate(DcZAxis, 0.785));
        DgAddObj(DoPrimSurf(DcBox));

   return(DgClose());
}


/**************************************************************************
 **  Annotation Text
 **/

DtObject 
make_anntxt()
{
   DtReal pos[3];
   DtReal color[3];

   DoGroup(DcTrue);
	DgAddObj(DoTextFont(DcHelvetica));

	DgAddObj(DoTextHeight(80.));

	pos[0] = -2.5;
	pos[1] = 2.;
	pos[2] = -0;
	color[0] = 0.0;
 	color[1] = 1.0;
	color[2] = 0.5;
	DgAddObj(DoDiffuseColor(DcRGB, color));
	DgAddObj(DoTextSpace(0.05));
	DgAddObj(DoAnnoText(pos,"Different"));

	DgAddObj(DoTextFont(DcComplexItalic));

	pos[0] = -2.5;
	pos[1] = -1.;
	pos[2] = -0;
	DgAddObj(DoAnnoText(pos,"Fonts"));

	color[0] = 1.0;
 	color[1] = 0.0;
	color[2] = 1.0;
	DgAddObj(DoDiffuseColor(DcRGB, color));
	DgAddObj(DoScale(0.7, 0.7, 0.7));
	DgAddObj(DoRotate(DcZAxis, 0.785));
	DgAddObj(DoPrimSurf(DcBox));
   return(DgClose());
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

	DgAddObj(DoPerspective(30.,-5.,-25.));

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
/*
  ======================================================================
 */
