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
#include "dore.h"

void g3create_emap (
   DtObject device,
   DtObject view,
   DtPoint3 frompoint,
   char     outfile[])
{
   DtObject camera_was;
   DtUpdateType updatetype_was;
   DtObject camera[6];
   DtObject camgrp[6];
   DtPoint3 atpoint[6];
   DtVector3 up[6];
   DtInt width[6], height[6];
   DtRasterType type[6];
   DtPtr data[6];
   unsigned char *newdata;
   unsigned char *din, *dout;
   int d, h, w;
   DtInt totalsize;
   DtFlag userdel[6];
   int i;

   fprintf(stderr, "Frompoint: %f %f %f\n", frompoint[0], frompoint[1], frompoint[2]);

   /* save current view params*/
   if (camera_was = DvInqActiveCamera(view)) 
	 DsHoldObj(camera_was);

   updatetype_was = DvInqUpdateType(view);

   /* set up 6 camera directions */
   atpoint[0][0] = frompoint[0];
   atpoint[0][1] = frompoint[1];
   atpoint[0][2] = frompoint[2] + 1.;
   up[0][0] = 0.;
   up[0][1] = 1.;
   up[0][2] = 0.;

   atpoint[1][0] = frompoint[0] - 1.;
   atpoint[1][1] = frompoint[1];
   atpoint[1][2] = frompoint[2];
   up[1][0] = 0.; 
   up[1][1] = 1.;
   up[1][2] = 0.;

   atpoint[2][0] = frompoint[0];
   atpoint[2][1] = frompoint[1];
   atpoint[2][2] = frompoint[2] - 1.;
   up[2][0] = 0.;
   up[2][1] = 1.;
   up[2][2] = 0.;

   atpoint[3][0] = frompoint[0] + 1.;
   atpoint[3][1] = frompoint[1];
   atpoint[3][2] = frompoint[2];
   up[3][0] = 0.;
   up[3][1] = 1.;
   up[3][2] = 0.;

   atpoint[4][0] = frompoint[0];
   atpoint[4][1] = frompoint[1] + 1.;
   atpoint[4][2] = frompoint[2];
   up[4][0] = 0.;
   up[4][1] = 0.;
   up[4][2] = 1.;

   atpoint[5][0] = frompoint[0];
   atpoint[5][1] = frompoint[1] - 1.;
   atpoint[5][2] = frompoint[2];
   up[5][0] = 0.;
   up[5][1] = 0.;
   up[5][2] = -1.;


   /* render and grab 6 faces of the environment box */
   DvSetUpdateType(view, DcUpdateAll);
   for (i=0; i<6; i++) {
      camgrp[i] = DoGroup(DcTrue);
      DgAddObj(DoPushMatrix());
      DgAddObj(DoPerspective(90., -.2, -100.));
      DgAddObj(DoLookAtFrom(atpoint[i], frompoint, up[i]));
      DgAddObj(camera[i] = DoCamera());
      DgAddObj(DoPopMatrix());
      DgClose();
	
      if (i==0) {
	 DgAddObjToGroup(DvInqDefinitionGroup(view), camgrp[i]);
      } else {
	 DgReplaceObjInGroup(DvInqDefinitionGroup(view), camgrp[i]);
      }
      DvSetActiveCamera(view, camera[i]);
      DvUpdate(view);
      getchar();

      if (DdInqPixelData(device, DcRasterRGB, &width[i], &height[i], &type[i], &data[i], &userdel[i]) == -1) {
	 fprintf(stderr, "DdInqPixelData failed\n");
	 exit(-1);
      }
   }
   DgEmpty(camgrp[5]);

   /* put the 6 faces together into an environment map */
   for (i=1; i<6; i++) {
      if (width[i] != width[0]) {
	 fprintf(stderr, "face 0 width = %d, face %d width = %d\n",
		 width[0], i, width[i]);
	 exit(-1);
      }
      if (height[i] != height[0]) {
	 fprintf(stderr, "face 0 height = %d, face %d height = %d\n",
		 height[0], i, height[i]);
	 exit(-1);
      }
      if (type[i] != DcRasterRGB) {
	 fprintf(stderr, "face 0 type != DcRasterRGB\n");
	 exit(-1);
      }
   }

   totalsize = width[0] * height[0] * 6 *4;
   if ((newdata = (unsigned char *)malloc(totalsize)) == NULL) {
      fprintf(stderr, "could not allocate space for data: %d bytes\n",
	      totalsize);
      exit(-1);
   } 

   dout = newdata;

   for (d=0; d<6; d++) {
      fprintf(stderr, ".");
      din = data[d];
      for (h=0; h<height[0]; h++) {
	 for (w=0; w<width[0]; w++) {
	    dout[1] = din[2];
	    dout[2] = din[1];
	    dout[3] = din[0];
	    dout += 4;
	    din += 3;
	 }
      }
   }
   fprintf(stderr, "\n");

   fprintf(stderr, "writing file %s\n", outfile);
   if (write_aif_file (outfile, width[0], height[0], 6, DcRasterABGR, newdata) ==
       -1) {
      fprintf(stderr, "could not write file %s\n", outfile);
      exit(1);
   }
 
   
   /* reset view params and clean up */
   DvSetActiveCamera(view, camera_was);
   DvSetUpdateType(view, updatetype_was);

   for (i=0; i<6; i++) 
	 if (userdel[i]) free(data[i]);
}
