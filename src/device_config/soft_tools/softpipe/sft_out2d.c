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
	void ddr_softpipe_drom_output_point (vertex,color)
	void ddr_softpipe_drom_output_line (vertex1,vertex2,color1,color2)
	void ddr_softpipe_drom_output_triangle (vertex1,vertex2,vertex3, color1,color2,color3)

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
void ddr_softpipe_drom_output_point (
    DtRealTriple vertex,
    DtColorRGB color)
{
   DtInt point[3];

#ifdef DEBUG
   printf("...processing dynout output point \n");
   fflush(stdout);
#endif

   point[0] = (int)(vertex[0] * 
		    dde_softpipe_drom_curdevdat->ctodscale[0] + 
		    dde_softpipe_drom_curdevdat->ctodtranslate[0]);
   point[1] = (int)(vertex[1] * 
		    dde_softpipe_drom_curdevdat->ctodscale[1] + 
		    dde_softpipe_drom_curdevdat->ctodtranslate[1]);
   point[2] = (int)(vertex[2] * 
		    dde_softpipe_drom_curdevdat->ctodscale[2] + 
		    dde_softpipe_drom_curdevdat->ctodtranslate[2]);

   (dde_softpipe_drom_data.draw_point_dc)(point,color);
}

/*
 ======================================================================
 */
void ddr_softpipe_drom_output_line (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtColorRGB color1,
    DtColorRGB color2)
{
   DtInt point1[3],point2[3];

#ifdef DEBUG
   printf("...processing dynout output line \n");
   fflush(stdout);
#endif
   point1[0] = (int)(vertex1[0] * 
		     dde_softpipe_drom_curdevdat->ctodscale[0] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[0]);
   point1[1] = (int)(vertex1[1] * 
		     dde_softpipe_drom_curdevdat->ctodscale[1] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[1]);
   point1[2] = (int)(vertex1[2] * 
		     dde_softpipe_drom_curdevdat->ctodscale[2] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[2]);
   point2[0] = (int)(vertex2[0] * 
		     dde_softpipe_drom_curdevdat->ctodscale[0] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[0]);
   point2[1] = (int)(vertex2[1] * 
		     dde_softpipe_drom_curdevdat->ctodscale[1] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[1]);
   point2[2] = (int)(vertex2[2] * 
		     dde_softpipe_drom_curdevdat->ctodscale[2] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[2]);

#ifdef DEBUG
   if((vertex1[1] == 1.)||(vertex2[1] == 1.)||
      (vertex1[1] == -1.)||(vertex2[1] == -1.)) {
      printf("vertex: (%lf,%lf)-(%lf,%lf)\n",
	     vertex1[0],vertex1[1],vertex2[0],vertex2[1]);
      printf("points: (%d,%d)-(%d,%d)\n",
	     point1[0],point1[1],point2[0],point2[1]);
      printf("pause!!!!!\n");
      getchar();
   }
#endif

   (dde_softpipe_drom_data.draw_line_dc)(point1,point2,color1,color2);
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_output_triangle (
    DtRealTriple vertex1,
    DtRealTriple vertex2,
    DtRealTriple vertex3,
    DtColorRGB color1,
    DtColorRGB color2,
    DtColorRGB color3)
{
   DtInt point1[3],point2[3],point3[3];

#ifdef DEBUG
   printf("...processing dynout output triangle\n");
   fflush(stdout);
#endif

#ifdef DEBUG
   printf("vertex: (%lf,%lf)-(%lf,%lf)-(%lf,%lf)\n",
	  vertex1[0],vertex1[1],vertex2[0],vertex2[1],vertex3[0],vertex3[1]);
#endif

   point1[0] = (int)(vertex1[0] * 
		     dde_softpipe_drom_curdevdat->ctodscale[0] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[0]);
   point1[1] = (int)(vertex1[1] * 
		     dde_softpipe_drom_curdevdat->ctodscale[1] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[1]);
   point1[2] = (int)(vertex1[2] * 
		     dde_softpipe_drom_curdevdat->ctodscale[2] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[2]);
   point2[0] = (int)(vertex2[0] * 
		     dde_softpipe_drom_curdevdat->ctodscale[0] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[0]);
   point2[1] = (int)(vertex2[1] * 
		     dde_softpipe_drom_curdevdat->ctodscale[1] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[1]);
   point2[2] = (int)(vertex2[2] * 
		     dde_softpipe_drom_curdevdat->ctodscale[2] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[2]);
   point3[0] = (int)(vertex3[0] * 
		     dde_softpipe_drom_curdevdat->ctodscale[0] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[0]);
   point3[1] = (int)(vertex3[1] * 
		     dde_softpipe_drom_curdevdat->ctodscale[1] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[1]);
   point3[2] = (int)(vertex3[2] * 
		     dde_softpipe_drom_curdevdat->ctodscale[2] + 
		     dde_softpipe_drom_curdevdat->ctodtranslate[2]);

#ifdef DEBUG
   printf("points: (%d,%d,%d)-(%d,%d,%d)-(%d,%d,%d)\n",
	  point1[0],point1[1],point1[2],
	  point2[0],point2[1],point2[2],
	  point3[0],point3[1],point3[2]);
#endif
   (dde_softpipe_drom_data.draw_triangle_dc)(point1,point2,point3,
					      color1,color2,color3);
}
