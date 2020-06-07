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

#include <stdio.h>
#include <dore/dore.h>
#include <math.h>

/*
 * This example program shows how to use radiosity in Dore.
 * It creates a 5-sided "Cornell" box with an area light source
 * and two simple objects inside the box. Once the convergence
 * criterion is met, the camera is moved around without a need
 * for recomputing the radiosity solution. Note the color bleeding
 * where the sides of the box meet, and also the soft shadows
 * projected by the objects onto the "floor" of the box.
 */

DtReal white[3]     = {1.0, 1.0, 1.0};
DtReal black[3]     = {0.0, 0.0, 0.0};
DtReal redish[3]    = {0.9, 0.0, 0.7};
DtReal bluish[3]    = {0.0, 0.7, 0.9};
DtReal yellowish[3] = {0.7, 0.7, 0.2};

DtRepType current_reptype;
DtObject defgroup;
DtPoint3 at_cam      = { 0.0, 0.0, 0.0 };
DtPoint3 from_cam    = { 0.0, 0.0, 3.7 };
DtVector3 up         = { 0.0, 1.0, 0.0 };

/* function prototypes */
DtObject make_triangle_mesh(DtInt);
DtObject make_simply_mesh(DtInt);
void set_reptype();
void change_camera_pos(DtReal, DtReal, DtReal);

main()
{

    DtObject view, device, frame;
    DtObject all_walls, all_box;
    DtObject wallgroup, lightgroup, roomgroup, boxgroup;
    static      DtPoint3 at_light1   = { 0.0,-1.0, 0.0 };
    static      DtPoint3 from_light1 = { 0.5, 0.5, 1.0 };
    static      DtPoint3 at_light2   = { 0.0, 1.0, 0.0 };
    static      DtPoint3 from_light2 = {-0.5,-0.5, 1.0 };
    DtReal      emis_factor;
    DtReal      emis_scale;
    DtInt       gridsize;
    DtReal   convergence[1], box_scale;
    DtReal   cres[1];
    DtInt i, j, steps;
    DtReal x, y, z, xinc, yinc, zinc;
        
    emis_factor = 11.0;
    emis_scale = 0.4;
    gridsize = 4;

    DsInitialize(0);
    DsRadiosityInitialize();

    device = DoDevice("display", "-geometry =400x400+512+0");
    frame = DoFrame();
    DdSetFrame(device, frame);
    view = DoView();
    DgAddObjToGroup(DfInqViewGroup(frame), view);
    DvSetBackgroundColor(view, DcRGB, black);

    defgroup = DoGroup(DcTrue);
        DgAddObj(DoPerspective(40.,-1., -50.0 ));
        DgAddObj(DoLookAtFrom(at_cam, from_cam, up));
        DgAddObj(DoCamera());

        DgAddObj(DoLightType(DcLightAmbient));
        DgAddObj(DoLight());
    DgClose();
    
    lightgroup = DoGroup(DcTrue);
        /* tesselate light a little less than other objects */
        DgAddObj(make_triangle_mesh(gridsize-1));
    DgClose();

    wallgroup = DoGroup(DcTrue);
        DgAddObj(make_triangle_mesh(gridsize));
    DgClose();

        box_scale = 0.3;
    boxgroup = DoGroup(DcTrue);
        DgAddObj(DoScale(box_scale, box_scale, box_scale));
        DgAddObj(make_simply_mesh(gridsize));
    DgClose();

    all_walls = DoGroup(DcTrue);
        DgAddObj(DoRadiositySwitch(DcOn));

        DgAddObj(DoEmissionSwitch(DcOn));
        DgAddObj(DoEmissionFactor(emis_factor));

        DgAddObj(DoDiffuseColor(DcRGB, white));

        /* light */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(0., .99, 0.));
            DgAddObj(DoRotate(DcXAxis, M_PI/2.));
            DgAddObj(DoScale(emis_scale, emis_scale, emis_scale));
            DgAddObj(lightgroup);
        DgAddObj(DoPopMatrix());

        DgAddObj(DoEmissionSwitch(DcOff));

        /* ceiling */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(0., 1., 0.));
            DgAddObj(DoRotate(DcXAxis, M_PI/2.));
            DgAddObj(wallgroup);
        DgAddObj(DoPopMatrix());

        /* back wall */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(0.0, 0.0, -1.0));
            DgAddObj(wallgroup);
        DgAddObj(DoPopMatrix());

        /* floor */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(0.0, -1.0, 0.0));
            DgAddObj(DoRotate(DcXAxis, -M_PI/2.));
            /* we double the resolution of the grid
               on the floor to get more accurate soft shadows */
            DgAddObj(make_triangle_mesh(gridsize*2));
        DgAddObj(DoPopMatrix());

        /* redish right wall */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoDiffuseColor(DcRGB, redish));
            DgAddObj(DoTranslate(1.0, 0.0, 0.0));
            DgAddObj(DoRotate(DcYAxis, -M_PI/2.));
            DgAddObj(wallgroup);
        DgAddObj(DoPopMatrix());

        /* bluish left wall */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoDiffuseColor(DcRGB, bluish));
            DgAddObj(DoTranslate(-1.0, 0.0, 0.0));
            DgAddObj(DoRotate(DcYAxis, M_PI/2.));
            DgAddObj(wallgroup);
        DgAddObj(DoPopMatrix());
    DgClose();

    all_box = DoGroup(DcTrue);
        /* top of box */
        DgAddObj(DoRadiositySwitch(DcOn));
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(0., box_scale, 0.));
            DgAddObj(DoRotate(DcXAxis, -M_PI/2.));
            DgAddObj(boxgroup);
        DgAddObj(DoPopMatrix());

        /* back of box */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(0., 0., -box_scale));
            DgAddObj(DoRotate(DcYAxis, -M_PI));
            DgAddObj(boxgroup);
        DgAddObj(DoPopMatrix());

        /* front of box */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(0., 0., box_scale));
            DgAddObj(boxgroup);
        DgAddObj(DoPopMatrix());

        /* right side of box */
        /* note: box has no bottom */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(box_scale, 0., 0.));
            DgAddObj(DoRotate(DcYAxis, M_PI/2.));
            DgAddObj(boxgroup);
        DgAddObj(DoPopMatrix());

        /* left side of box */
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(-box_scale, 0., 0.));
            DgAddObj(DoRotate(DcYAxis, -M_PI/2.));
            DgAddObj(boxgroup);
        DgAddObj(DoPopMatrix());
    DgClose();

    roomgroup = DoGroup(DcTrue);
        DgAddObj(DoRepType(DcSurface));
        /* this is just for first outline display; radiosity
           will ignore this attribute */
        DgAddObj(DoSurfaceShade(DcShaderConstant));
        DgAddObj(DoCallback((DtPFI) set_reptype, DcNullPtr));
        /* room walls */
        DgAddObj(all_walls);
        /* objects in room */
        DgAddObj(DoDiffuseColor(DcRGB, yellowish));
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(-.4, box_scale-1.0, 0.0));
            DgAddObj(DoRotate(DcYAxis, M_PI/6.0));
            DgAddObj(all_box);
        DgAddObj(DoPopMatrix());
        DgAddObj(DoPushMatrix());
            DgAddObj(DoTranslate(0.5, 1.2*box_scale-1.0, 0.0));
            DgAddObj(DoScale(0.5, 1.2, .5));
            DgAddObj(DoRotate(DcYAxis, M_PI/3.0));
            DgAddObj(all_box);
        DgAddObj(DoPopMatrix());
    DgClose();

    DgAddObjToGroup(DvInqDefinitionGroup(view), defgroup);
    DgAddObjToGroup(DvInqDisplayGroup(view), roomgroup);

    /* we want to stop when 30% of unshot energy remains */
    convergence[0] = 0.30;
    DvSetRadiosityConvergence(view,  DcConvGlobalRel, convergence);

    /* request a 80 x 80 hemicube device */
    cres[0] = 80;
    DvSetFormFactorCompType(view,  DcFormFactorHemicube, cres);

    DvSetRadiosityUpdateType(view,  DcRadiosityNone);
    current_reptype = DcOutlines;
    DdUpdate(device);
    printf("Showing geometry of scene. Hit return to continue\n");
    getchar();

    printf("Convergence criterion set to 30 percent unshot energy\n");
    printf("Computing progressive radiosity....\n");
    DvSetRadiosityUpdateType(view,  DcRadiosityComputeDisp);
    current_reptype = DcSurface;
    DdUpdate(device);

    DvSetRadiosityUpdateType(view,  DcRadiosityDisp);
    printf("\nHit return to move camera around.\n");
    getchar();

    for (j=0; j<2; j++) {
        steps = 25;
        x = from_cam[0];
        y = from_cam[1];
        z = from_cam[2];

        xinc = 0;
        yinc = 1.3/steps;
        zinc = -(3.7-1.7)/steps;
        for (i=0; i<steps; i++) {
            x+= xinc; y+= yinc; z+= zinc;
            change_camera_pos(x, y, z);
            DdUpdate(device);
        }
        DdUpdate(device);

        yinc = -1.3/steps;
        zinc = (5.5-1.7)/steps;
        for (i=0; i<steps; i++) {
            x+= xinc; y+= yinc; z+= zinc;
            change_camera_pos(x, y, z);
            DdUpdate(device);
        }
        DdUpdate(device);

        zinc = -(5.5-4.5)/steps;
        xinc = yinc = 2.0/steps;
        for (i=0; i<steps; i++) {
            x+= xinc; y+= yinc; z+= zinc;
            change_camera_pos(x, y, z);
            DdUpdate(device);
        }
        DdUpdate(device);
        DdUpdate(device);

        zinc = (6.5-4.5)/steps;
        xinc = -4.0/steps;
        yinc = -2.0/steps;
        for (i=0; i<steps; i++) {
            x+= xinc; y+= yinc; z+= zinc;
            change_camera_pos(x, y, z);
            DdUpdate(device);
        }
        DdUpdate(device);
        DdUpdate(device);

        xinc = 2.0/steps;
        yinc = 0;
        zinc = (3.7-6.5)/steps;
        for (i=0; i<steps; i++) {
            x+= xinc; y+= yinc; z+= zinc;
            change_camera_pos(x, y, z);
            DdUpdate(device);
        }
        DdUpdate(device);

        if (j==0) {
            printf("Want to see it again?\n");
            getchar();
        }
    }

    printf("All done!\n");
    getchar();
    DsTerminate();
}

/* 
============================================================================
*/

void set_reptype()
{
    DsExecuteObj(DoRepType(current_reptype));
}

/* 
============================================================================
*/

void change_camera_pos(
  DtReal x,
  DtReal y,
  DtReal z)
{
  /* modifies the "from" position for the camera */
   from_cam[0] = x;
   from_cam[1] = y;
   from_cam[2] = z;
   DgOpen(defgroup, DcFalse);
       DgSetElePtr(1, DcBeginning);
       DgReplaceObj(DoLookAtFrom(at_cam, from_cam, up));
   DgClose();

}

/* 
============================================================================
*/

DtObject make_triangle_mesh (DtInt gridsize)
{
   auto DtInt        i;
   auto DtReal       gridincr;         /* Surface Grid Increment */
   auto DtInt        xgridloc,         /* Grid Location X */
                     ygridloc;         /* Grid Location Y */
   auto DtReal       *vertexdata;      /* Vertex Data Array Pointer */
   auto DtReal       *vertexptr;       /* Vertex Array Traversal Pointer */
   auto DtInt        *triangles;
   auto DtInt        *triptr;

   if (gridsize < 1) return DcNullObject;

   if (gridsize > 1) {
      gridincr = 2.0 / (DtReal)(gridsize-1);
      vertexdata = (DtReal*) DDspace_Allocate ((DtInt)(gridsize * gridsize
                                           * 6 * sizeof(DtReal)));
      triangles = (DtInt*) DDspace_Allocate ((DtInt)(6 * (gridsize-1)
                                           * (gridsize-1) * sizeof(DtInt)));
   } else {
      vertexdata = (DtReal*) DDspace_Allocate ((DtInt)
                               (18 * sizeof(DtReal)));
      triangles = (DtInt*) DDspace_Allocate ((DtInt) (3 * sizeof(DtInt)));
   }

   vertexptr = vertexdata;

   for (xgridloc=0;  xgridloc < gridsize;  ++xgridloc) {
      auto DtReal loc[3];               /* Vertex Location */

      loc[0] = -1 + ((DtReal)xgridloc*gridincr);

      for (ygridloc=0;  ygridloc < gridsize;  ++ygridloc) {

            loc[1] = -1 + ((DtReal)ygridloc*gridincr);
            loc[2] = 0.;

        *vertexptr++ = loc[0];
        *vertexptr++ = loc[1];
        *vertexptr++ = loc[2];
      }
   }

   triptr = triangles;
   for (xgridloc=0;  xgridloc < gridsize-1;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < gridsize-1;  ++ygridloc) {
         *triptr++ = (ygridloc+0) + ((xgridloc+0) * gridsize);
         *triptr++ = (ygridloc+0) + ((xgridloc+1) * gridsize);
         *triptr++ = (ygridloc+1) + ((xgridloc+0) * gridsize);

         *triptr++ = (ygridloc+0) + ((xgridloc+1) * gridsize);
         *triptr++ = (ygridloc+1) + ((xgridloc+1) * gridsize);
         *triptr++ = (ygridloc+1) + ((xgridloc+0) * gridsize);
      }
   }

   if (gridsize == 1) {
      static DtReal loc[3][3] = 
      { {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0} };

      vertexptr = vertexdata;
      for (i=0;  i < 3;  ++i) {  
        *vertexptr++ = loc[i][0]; 
        *vertexptr++ = loc[i][1];
        *vertexptr++ = loc[i][2];
      }
      triangles[0] = 0;
      triangles[1] = 1;
      triangles[2] = 2;
   }

   if (gridsize == 1) {
      return DoTriangleMesh (DcRGB, DcLoc, 3, vertexdata, 1, triangles, DcTrue);
   } else {
      return DoTriangleMesh (DcRGB, DcLoc, gridsize*gridsize, vertexdata,
                             2*(gridsize-1)*(gridsize-1), triangles, DcTrue);
   }

}

/* 
============================================================================
*/

DtObject make_simply_mesh (DtInt gridsize)
{
   auto DtInt        i;
   auto DtReal       gridincr;         /* Surface Grid Increment */
   auto DtInt        xgridloc,         /* Grid Location X */
                     ygridloc;         /* Grid Location Y */
   auto DtReal       *vertexdata;      /* Vertex Data Array Pointer */
   auto DtReal       *vertexptr;       /* Vertex Array Traversal Pointer */
   auto DtInt        *contours;
   auto DtInt        *vtxlist;
   auto DtInt        *vtxlstptr;
   auto DtInt        plycnt;

   if (gridsize < 1) return DcNullObject;

   if (gridsize > 1) {
      gridincr = 2.0 / (DtReal)(gridsize-1);
      vertexdata = (DtReal*) DDspace_Allocate ((DtInt)(gridsize * gridsize
                                           * 6 * sizeof(DtReal)));
      plycnt = (gridsize-1) * (gridsize-1);
      contours = (DtInt*) DDspace_Allocate ((DtInt)(plycnt * sizeof(DtInt)));
      vtxlist = (DtInt*) DDspace_Allocate ((DtInt)( 4* plycnt * sizeof(DtInt)));
   } else {
        return DcNullObject;
   }

   vertexptr = vertexdata;

   for (xgridloc=0;  xgridloc < gridsize;  ++xgridloc) {
      auto DtReal loc[3];               /* Vertex Location */

      loc[0] = -1 + ((DtReal)xgridloc*gridincr);
     for (ygridloc=0;  ygridloc < gridsize;  ++ygridloc) {

            loc[1] = -1 + ((DtReal)ygridloc*gridincr);
            loc[2] = 0.;

            *vertexptr++ = loc[0];
            *vertexptr++ = loc[1];
            *vertexptr++ = loc[2];
      }
   }

   for (i=0; i<plycnt; i++) {
        contours[i] = 4;
   }

   vtxlstptr = vtxlist;
   for (xgridloc=0;  xgridloc < gridsize-1;  ++xgridloc) {
      for (ygridloc=0;  ygridloc < gridsize-1;  ++ygridloc) {
         *vtxlstptr++ = (ygridloc+0) + ((xgridloc+0) * gridsize);
         *vtxlstptr++ = (ygridloc+0) + ((xgridloc+1) * gridsize);
         *vtxlstptr++ = (ygridloc+1) + ((xgridloc+1) * gridsize);
         *vtxlstptr++ = (ygridloc+1) + ((xgridloc+0) * gridsize);
      }
   }

   return DoSimplePolygonMesh (DcRGB, DcLoc, gridsize*gridsize, vertexdata,
                             plycnt, contours, vtxlist, DcConvex, DcTrue);

}
/* 
============================================================================
*/

