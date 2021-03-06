
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


/**************************************************************************

 Functions:
   ddr_kopengl_drom_print_point_list 
   ddr_kopengl_drom_print_line_list 
   ddr_kopengl_drom_print_connected_line_list 
   ddr_kopengl_drom_print_triangle_list
   ddr_kopengl_drom_print_triangle_mesh 
   ddr_kopengl_drom_print_matrix 
   ddr_kopengl_drom_print_studio 
   ddr_kopengl_drom_print_atts 

 **************************************************************************/

#include <stdio.h>
#include "drom.h"



/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_point_list (DtColorModel     colormodel,
				   DtRealTriple     bndboxpts[8],
				   DtInt            pointcount,
				   DtRealTriple    *vtxlocs,
				   DtRealTriple    *vtxnrms,
				   DtRealTriple    *vtxclrs,
				   DtInt            uv_count,
				   DtRealCouple   **uv_list,
				   DtInt            uvw_count,
				   DtRealTriple   **uvw_list)
{
 DtInt           i, j;
 DtFlag          nrms, clrs;

 fprintf (stdout, "[KOpenGL Driver] ...in _drom_print_point_list\n");

#ifdef NOTYET
 ddr_kopengl_drom_print_atts ();
#endif

 if (bndboxpts != (DtRealTriple *) 0)
   {
    printf ("\tbndboxpts\n");
    for (i = 0; i < 8; i++)
      {
       printf ("\t    (%g %g %g)\n",
	       bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
      }
   }
 else
   {
    printf ("\tbndboxpts:\t<NULL>\n");
   }

 nrms = vtxnrms != (DtRealTriple *) 0;
 clrs = vtxclrs != (DtRealTriple *) 0;
 printf ("\t%d points with locations%s", pointcount,
	 nrms ? ", normals" : "");
 if (clrs)
   {
    switch (colormodel)
       {
	case DcRGB:
	 printf (", (DcRGB) colors");
	 break;
	default:
	 printf (", <%d> colors", colormodel);
	 break;
       }
   }
 if (uv_count > 0)
  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
 if (uvw_count > 0)
  printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
 printf ("\n");

 for (i = 0; i < pointcount; i++)
   {
    printf ("\t    %d: (%g %g %g)", i,
	    vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
    if (nrms)
      {
       printf (" (%g %g %g)",
	       vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
      }
    if (clrs)
      {
       printf (" (%g %g %g)",
	       vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
      }
    for (j = 0; j < uv_count; j++)
      {
       printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
      }
    for (j = 0; j < uvw_count; j++)
      {
       printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
      }
    printf ("\n");
   }
}


/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_line_list (DtColorModel    colormodel,
				  DtRealTriple    bndboxpts[8],
				  DtInt           linecount,
				  DtRealTriple   *elenrms,
				  DtRealTriple   *vtxlocs,
				  DtRealTriple   *vtxnrms,
				  DtRealTriple   *vtxclrs,
				  DtInt           uv_count,
				  DtRealCouple  **uv_list,
				  DtInt           uvw_count,
				  DtRealTriple  **uvw_list)
{
 DtInt           i, j;
 DtFlag          nrms, clrs;

 fprintf (stdout, "[KOpenGL Driver] ...in _drom_print_line_list\n");

#ifdef NOTYET
 ddr_kopengl_drom_print_atts ();
#endif

 if (bndboxpts != (DtRealTriple *) 0)
   {
    printf ("\tbndboxpts\n");
    for (i = 0; i < 8; i++)
      {
       printf ("\t    (%g %g %g)\n",
	       bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
      }
   }
 else
   {
    printf ("\tbndboxpts:\t<NULL>\n");
   }

 if (elenrms != (DtRealTriple *) 0)
   {
    printf ("\t%d elenrms\n", linecount);
    for (i = 0; i < linecount; i++)
      {
       printf ("\t    %d: (%g %g %g)\n", i,
	       elenrms[i][0], elenrms[i][1], elenrms[i][2]);
      }
   }
 nrms = vtxnrms != (DtRealTriple *) 0;
 clrs = vtxclrs != (DtRealTriple *) 0;
 printf ("\t%d lines with locations%s", linecount,
	 nrms ? ", normals" : "");
 if (clrs)
   {
    switch (colormodel)
       {
	case DcRGB:
	 printf (", (DcRGB) colors");
	 break;
	default:
	 printf (", <%d> colors", colormodel);
	 break;
       }
   }
 if (uv_count > 0)
  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
 if (uvw_count > 0)
  printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
 printf ("\n");

 for (i = 0; i < linecount * 2; i++)
   {
    printf ("\t    %d-%d: (%g %g %g)", i / 2, i % 2,
	    vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
    if (nrms)
      {
       printf (" (%g %g %g)",
	       vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
      }
    if (clrs)
      {
       printf (" (%g %g %g)",
	       vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
      }
    for (j = 0; j < uv_count; j++)
      {
       printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
      }
    for (j = 0; j < uvw_count; j++)
      {
       printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
      }
    printf ("\n");

   }
}



/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_connected_line_list (DtColorModel    colormodel,
					    DtRealTriple    bndboxpts[8],
					    DtInt           vertexcount,
					    DtRealTriple   *elenrms,
					    DtRealTriple   *vtxlocs,
					    DtRealTriple   *vtxnrms,
					    DtRealTriple   *vtxclrs,
					    DtInt           uv_count,
					    DtRealCouple  **uv_list,
					    DtInt           uvw_count,
					    DtRealTriple  **uvw_list)
{
 DtInt           i, j;
 DtFlag          nrms, clrs;

 fprintf (stdout, "[KOpenGL Driver] ...in _drom_print_connected_line_list\n");

#ifdef NOTYET
 ddr_kopengl_drom_print_atts ();
#endif

 if (bndboxpts != (DtRealTriple *) 0)
   {
    printf ("\tbndboxpts\n");
    for (i = 0; i < 8; i++)
      {
       printf ("\t    (%g %g %g)\n",
	       bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
      }
   }
 else
   {
    printf ("\tbndboxpts:\t<NULL>\n");
   }

 if (elenrms != (DtRealTriple *) 0)
   {
    printf ("\t%d elenrms\n", vertexcount - 1);
    for (i = 0; i < vertexcount - 1; i++)
      {
       printf ("\t    %d: (%g %g %g)\n", i,
	       elenrms[i][0], elenrms[i][1], elenrms[i][2]);
      }
   }
 nrms = vtxnrms != (DtRealTriple *) 0;
 clrs = vtxclrs != (DtRealTriple *) 0;
 printf ("\t%d vertices with locations%s", vertexcount,
	 nrms ? ", normals" : "");
 if (clrs)
   {
    switch (colormodel)
       {
	case DcRGB:
	 printf (", (DcRGB) colors");
	 break;
	default:
	 printf (", <%d> colors", colormodel);
	 break;
       }
   }
 if (uv_count > 0)
   printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
 if (uvw_count > 0)
   printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
 printf ("\n");

 for (i = 0; i < vertexcount; i++)
   {
    printf ("\t    %d: (%g %g %g)", i,
	    vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
    if (nrms)
      {
       printf (" (%g %g %g)",
	       vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
      }
    if (clrs)
      {
       printf (" (%g %g %g)",
	       vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
      }
    for (j = 0; j < uv_count; j++)
      {
       printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
      }
    for (j = 0; j < uvw_count; j++)
      {
       printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
      }
    printf ("\n");
   }
}



/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_triangle_list (DtColorModel    colormodel,
				      DtRealTriple    bndboxpts[8],
				      DtInt           trianglecount,
				      DtRealTriple   *elenrms,
				      DtRealTriple   *vtxlocs,
				      DtRealTriple   *vtxnrms,
				      DtRealTriple   *vtxclrs,
				      DtInt           uv_count,
				      DtRealCouple  **uv_list,
				      DtInt           uvw_count,
				      DtRealTriple  **uvw_list)
{
 DtInt           i, j;
 DtFlag          nrms, clrs;

 fprintf (stdout, "[KOpenGL Driver] ...in _drom_print_triangle_list\n");

 ddr_kopengl_drom_print_atts ();

 if (bndboxpts != (DtRealTriple *) 0)
   {
    printf ("\tbndboxpts\n");
    for (i = 0; i < 8; i++)
      {
       printf ("\t    (%g %g %g)\n",
	       bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
      }
   }

 if (elenrms != (DtRealTriple *) 0)
   {
    printf ("\t%d elenrms\n", trianglecount);
    for (i = 0; i < trianglecount; i++)
      {
       printf ("\t    %d: (%g %g %g)\n", i,
	       elenrms[i][0], elenrms[i][1], elenrms[i][2]);
      }
   }
 nrms = vtxnrms != (DtRealTriple *) 0;
 clrs = vtxclrs != (DtRealTriple *) 0;
 printf ("\t%d triangles with locations%s", trianglecount,
	 nrms ? ", normals" : "");
 if (clrs)
   {
    switch (colormodel)
       {
	case DcRGB:
	 printf (", (DcRGB) colors");
	 break;
	default:
	 printf (", <%d> colors", colormodel);
	 break;
       }
   }
 if (uv_count > 0)
  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
 if (uvw_count > 0)
  printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
 printf ("\n");

 for (i = 0; i < trianglecount * 3; i++)
   {
    printf ("\t    %d-%d: (%g %g %g)", i / 3, i % 3,
	    vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
    if (nrms)
      {
       printf (" (%g %g %g)",
	       vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
      }
    if (clrs)
      {
       printf (" (%g %g %g)",
	       vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
      }
    for (j = 0; j < uv_count; j++)
      {
       printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
      }
    for (j = 0; j < uvw_count; j++)
      {
       printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
      }
    printf ("\n");
   }
}



/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_triangle_mesh (DtColorModel    colormodel,
				      DtRealTriple    bndboxpts[8],
				      DtInt           trianglecount,
				      DtRealTriple   *elenrms,
				      DtInt           vertexcount,
				      DtRealTriple   *vtxlocs,
				      DtRealTriple   *vtxnrms,
				      DtRealTriple   *vtxclrs,
				      DtInt           vertexlist[][3],
				      DtInt           compiledvertexlist[][3],
				      DtInt           uv_count,
				      DtRealCouple  **uv_list,
				      DtInt           uvw_count,
				      DtRealTriple  **uvw_list)
{
 DtInt           i, j;
 DtFlag          nrms, clrs;

 fprintf (stdout, "[KOpenGL Driver] ...in _drom_print_triangle_mesh\n");

#ifdef NOTYET
 ddr_kopengl_drom_print_atts ();
#endif

 if (bndboxpts != (DtRealTriple *) 0)
   {
    printf ("\tbndboxpts\n");
    for (i = 0; i < 8; i++)
      {
       printf ("\t    (%g %g %g)\n",
	       bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
      }
   }

 if (elenrms != (DtRealTriple *) 0)
   {
    printf ("\t%d elenrms\n", trianglecount);
    for (i = 0; i < trianglecount; i++)
      {
       printf ("\t    %d: (%g %g %g)\n", i,
	       elenrms[i][0], elenrms[i][1], elenrms[i][2]);
      }
   }
 nrms = vtxnrms != (DtRealTriple *) 0;
 clrs = vtxclrs != (DtRealTriple *) 0;
 printf ("\t%d vertices with locations%s", vertexcount,
	 nrms ? ", normals" : "");
 if (clrs)
   {
    switch (colormodel)
       {
	case DcRGB:
	 printf (", (DcRGB) colors");
	 break;
	default:
	 printf (", <%d> colors", colormodel);
	 break;
       }
   }
 if (uv_count > 0)
  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
 if (uvw_count > 0)
  printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
 printf ("\n");

 for (i = 0; i < vertexcount; i++)
   {
    printf ("\t    %d: (%g %g %g)", i,
	    vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
    if (nrms)
      {
       printf (" (%g %g %g)",
	       vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
      }
    if (clrs)
      {
       printf (" (%g %g %g)",
	       vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
      }
    for (j = 0; j < uv_count; j++)
      {
       printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
      }
    for (j = 0; j < uvw_count; j++)
      {
       printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
      }
    printf ("\n");
   }

 printf ("\t%d triangles\n", trianglecount);
 for (i = 0; i < trianglecount; i++)
   {
    printf ("\t    %d: (%d %d %d)\n", i, vertexlist[i][0],
	    vertexlist[i][1], vertexlist[i][2]);
   }
}



/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_matrix (matrix)
 DtMatrix4x4     matrix;
{
 DtInt           i;

 for (i = 0; i < 4; i++)
   {
    printf ("\t\t\t%e %e %e %e\n",
	    matrix[i][0], matrix[i][1], matrix[i][2], matrix[i][3]);
   }
}



/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_studio (void)
{
 DtInt           i;

 printf ("    device_data:\n");

 printf ("\tvisual_type	");
 switch (dde_kopengl_drom_curdevdat->visual_type)
    {
     case DcStaticGrey:
      printf ("DcStaticGrey\n");
      break;
     case DcGreyScale:
      printf ("DcGreyScale\n");
      break;
     case DcStaticColor:
      printf ("DcStaticColor\n");
      break;
     case DcPseudoColor:
      printf ("DcPseudoColor\n");
      break;
     case DcTrueColor:
      printf ("DcTrueColor\n");
      break;
     case DcDirectColor:
      printf ("DcDirectColor\n");
      break;
     default:
      printf ("<%d>\n", dde_kopengl_drom_curdevdat->visual_type);
    }

 printf ("\tshade_mode	");
 switch (dde_kopengl_drom_curdevdat->shade_mode)
    {
     case DcRange:
      printf ("DcRange\n");
      break;
     case DcComponent:
      printf ("DcComponent\n");
      break;
     default:
      printf ("<%d>\n", dde_kopengl_drom_curdevdat->shade_mode);
    }

 printf ("\tcamera_to_device_trans	(%g %g %g)\n",
	 dde_kopengl_drom_curdevdat->camera_to_device_trans[0],
	 dde_kopengl_drom_curdevdat->camera_to_device_trans[1],
	 dde_kopengl_drom_curdevdat->camera_to_device_trans[2]);
 printf ("\tcamera_to_device_scale	(%g %g %g)\n",
	 dde_kopengl_drom_curdevdat->camera_to_device_scale[0],
	 dde_kopengl_drom_curdevdat->camera_to_device_scale[1],
	 dde_kopengl_drom_curdevdat->camera_to_device_scale[2]);
 printf ("\tframe_to_device_scale	%g %g %g)\n",
	 dde_kopengl_drom_curdevdat->frame_to_device_scale[0],
	 dde_kopengl_drom_curdevdat->frame_to_device_scale[1],
	 dde_kopengl_drom_curdevdat->frame_to_device_scale[2]);
 printf ("\tannoscale	(%g %g %g)\n",
	 dde_kopengl_drom_curdevdat->annoscale[0],
	 dde_kopengl_drom_curdevdat->annoscale[1],
	 dde_kopengl_drom_curdevdat->annoscale[2]);
 printf ("\tstereoflag	%s\n",
	 dde_kopengl_drom_curdevdat->stereoflag ? "DcOn" : "DcOff");

 printf ("    view_data:\n");
 printf ("\tclear_flag:	%s\n",
	 dde_kopengl_drom_curviwdat->clear_flag ? "DcOn" : "DcOff");
 printf ("\tshade_index:	%d\n",
	 dde_kopengl_drom_curviwdat->shade_index);
 printf ("\tbackground:	(%g %g %g)\n",
	 dde_kopengl_drom_curviwdat->background_color[0],
	 dde_kopengl_drom_curviwdat->background_color[1],
	 dde_kopengl_drom_curviwdat->background_color[2]);
 printf ("\tbackground_just	(%g %g)\n",
	 dde_kopengl_drom_curviwdat->background_just[0],
	 dde_kopengl_drom_curviwdat->background_just[1]);

 printf ("\tcamera:\n");
 printf ("\t   type		");
 switch (dde_kopengl_drom_curcamdat->camera_type)
    {
     case DcCameraArbitrary:
      printf ("DcCameraArbitrary\n");
      break;
     case DcCameraParallel:
      printf ("DcCameraParallel\n");
      break;
     case DcCameraPerspective:
      printf ("DcCameraPerspective\n");
      break;
     case DcCameraProjection:
      printf ("DcCameraProjection\n");
      break;
     default:
      printf ("<%d>\n",
	      dde_kopengl_drom_curcamdat->camera_type);
    }
 printf ("\t   stereoswitch	%s\n",
	 dde_kopengl_drom_curcamdat->stereoswitch ? "DcOn" : "DcOff");
 printf ("\t   eye_sep	%g\n",
	 dde_kopengl_drom_curcamdat->stereo_eyeseparation);
 printf ("\t   focal_dist	%g\n",
	 dde_kopengl_drom_curcamdat->stereo_distance);

 ddr_kopengl_print_opengl_matrix ("Projection Matrix", CAMERA->projection_matrix);
 ddr_kopengl_print_opengl_matrix ("Camera to World", CAMERA->cam_to_world_matrix);
 ddr_kopengl_print_opengl_matrix ("World to Camera", CAMERA->world_to_cam_matrix);
 ddr_kopengl_print_opengl_matrix ("Camera to Frustum", CAMERA->cam_to_frustum_matrix);

 {
  glReal          tmp[16];

  GET_GL_MATRIX (GL_PROJECTION_MATRIX, tmp);
  ddr_kopengl_print_opengl_matrix ("OpenGL Projection Matrix", tmp);

  GET_GL_MATRIX (GL_MODELVIEW_MATRIX, tmp);
  ddr_kopengl_print_opengl_matrix ("OpenGL Modelview Matrix", tmp);
 }


 printf ("\tlights (%d)\n", dde_kopengl_drom_curviwdat->total_lights);
 for (i = 0; i < dde_kopengl_drom_curviwdat->total_lights; i++)
   {
    printf ("\t   %d: (0x%x) %s ", i,
	    dde_kopengl_drom_curviwdat->light[i].light,
	    dde_kopengl_drom_curviwdat->light[i].status ? "DcOn " : "DcOff");
    if (dde_kopengl_drom_curviwdat->light[i].type == DcLightAmbient)
      {
       printf ("DcLightAmbient\n");
       printf ("\t\tcolor\t\t(%g %g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].color[0],
	       dde_kopengl_drom_curviwdat->light[i].color[1],
	       dde_kopengl_drom_curviwdat->light[i].color[2]);
      }
    else if (dde_kopengl_drom_curviwdat->light[i].type == DcLightInfinite)
      {
       printf ("DcLightInfinite\n");
       printf ("\t\tcolor\t\t(%g %g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].color[0],
	       dde_kopengl_drom_curviwdat->light[i].color[1],
	       dde_kopengl_drom_curviwdat->light[i].color[2]);
       printf ("\t\tdirection\t(%g %g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].direction[0],
	       dde_kopengl_drom_curviwdat->light[i].direction[1],
	       dde_kopengl_drom_curviwdat->light[i].direction[2]);
      }
    else if (dde_kopengl_drom_curviwdat->light[i].type == DcLightPoint ||
	     dde_kopengl_drom_curviwdat->light[i].type == DcLightPointAttn)
      {
       printf ("DcLightPoint%s\n",
	       dde_kopengl_drom_curviwdat->light[i].type == DcLightPointAttn
	       ? "Attn" : "");
       printf ("\t\tcolor\t\t(%g %g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].color[0],
	       dde_kopengl_drom_curviwdat->light[i].color[1],
	       dde_kopengl_drom_curviwdat->light[i].color[2]);
       printf ("\t\tposition\t(%g %g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].position[0],
	       dde_kopengl_drom_curviwdat->light[i].position[1],
	       dde_kopengl_drom_curviwdat->light[i].position[2]);
       if (dde_kopengl_drom_curviwdat->light[i].type == DcLightPointAttn)
	 {
	  printf ("\t\tattenuation\t(%g %g)\n",
		  dde_kopengl_drom_curviwdat->light[i].c1,
		  dde_kopengl_drom_curviwdat->light[i].c2);
	 }
      }
    else if (dde_kopengl_drom_curviwdat->light[i].type == DcLightSpot ||
	     dde_kopengl_drom_curviwdat->light[i].type == DcLightSpotAttn)
      {
       printf ("DcLightSpot%s\n",
	       dde_kopengl_drom_curviwdat->light[i].type == DcLightSpotAttn
	       ? "Attn" : "");
       printf ("\t\tcolor\t\t(%g %g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].color[0],
	       dde_kopengl_drom_curviwdat->light[i].color[1],
	       dde_kopengl_drom_curviwdat->light[i].color[2]);
       printf ("\t\tposition\t(%g %g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].position[0],
	       dde_kopengl_drom_curviwdat->light[i].position[1],
	       dde_kopengl_drom_curviwdat->light[i].position[2]);
       printf ("\t\tdirection\t(%g %g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].direction[0],
	       dde_kopengl_drom_curviwdat->light[i].direction[1],
	       dde_kopengl_drom_curviwdat->light[i].direction[2]);
       printf ("\t\tangles\t\t(%g %g)\n",
	       dde_kopengl_drom_curviwdat->light[i].angle_total,
	       dde_kopengl_drom_curviwdat->light[i].angle_delta);
       printf ("\t\texponent\t%g\n",
	       dde_kopengl_drom_curviwdat->light[i].exponent);
       if (dde_kopengl_drom_curviwdat->light[i].type == DcLightSpotAttn)
	 {
	  printf ("\t\tattenuation\t(%g %g)\n",
		  dde_kopengl_drom_curviwdat->light[i].c1,
		  dde_kopengl_drom_curviwdat->light[i].c2);
	 }
      }
   }

 printf ("   view_data\n");
 printf ("\twindow		(%g %g) (%g %g)\n",
	 dde_kopengl_drom_curwindat->actviewxmin,
	 dde_kopengl_drom_curwindat->actviewymin,
	 dde_kopengl_drom_curwindat->actviewxmax,
	 dde_kopengl_drom_curwindat->actviewymax);
}



/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_mapping (ddt_kopengl_drom_texmap * texmap,
				char *texname)
{
 DtPtr           name;
 DtNameType      type;
 DtInt           i, j;

 printf ("\t    %-10s\t\t%s\n", texname, texmap->status ?
	 "DcOn" : "DcOff");
 if (!texmap->status)
  return;

 for (i = 0; i < texmap->count; i++)
   {
    printf ("\t\t%d: mapping (", i);
    if (texmap->mapping[i] == DcStd3dTableLookup)
      {
       printf ("DcStd3dTableLookup");
      }
    else if (texmap->mapping[i] == DcStdBumpMap)
      {
       printf ("DcStdBumpMap");
      }
    else if (texmap->mapping[i] == DcStdCubeEnvironMap)
      {
       printf ("DcStdCubeEnvironMap");
      }
    else if (texmap->mapping[i] == DcStdSphereEnvironMap)
      {
       printf ("DcStdSphereEnvironMap");
      }
    else if (texmap->mapping[i] == DcStdTableLookup)
      {
       printf ("DcStdTableLookup");
      }
    else
      {
       printf ("<0x%x>", texmap->mapping[i]);
      }
    printf (") raster (0x%x)\n", texmap->raster[i]);
    printf ("\t\t    aamode\t");
    switch (texmap->attributes[i].aamode)
       {
	case DcTextureAANone:
	 printf ("DcTextureAANone\n");
	 break;
	case DcTextureAAMIPPoint:
	 printf ("DcTextureAAMIPPoint\n");
	 break;
	case DcTextureAAMIP2Point:
	 printf ("DcTextureAAMIP2Point\n");
	 break;
	case DcTextureAAMIPBilinear:
	 printf ("DcTextureAAMIPBilinear\n");
	 break;
	case DcTextureAAMIP2Bilinear:
	 printf ("DcTextureAAMIP2Bilinear\n");
	 break;
	case DcTextureAAAdaptive:
	 printf ("DcTextureAAAdaptive\n");
	 break;
	case DcTextureAABilinear:
	 printf ("DcTextureAABilinear\n");
	 break;
	case DcTextureAATrilinear:
	 printf ("DcTextureAATriilinear\n");
	 break;
	case DcTextureAASumArea:
	 printf ("DcTextureAASumArea\n");
	 break;
	default:
	 printf ("<%d>\n", texmap->attributes[i].aamode);
       }
    printf ("\t\t    operator\t");
    switch (texmap->attributes[i].operator)
       {
	case DcTextureReplace:
	 printf ("DcTextureReplace\n");
	 break;
	case DcTextureMultiply:
	 printf ("DcTextureMultiply\n");
	 break;
	case DcTextureBlend:
	 printf ("DcTextureBlend\n");
	 break;
	case DcTextureAdd:
	 printf ("DcTextureAdd\n");
	 break;
	default:
	 printf ("<%d>\n", texmap->attributes[i].operator);
       }
    printf ("\t\t    surface_object\t0x%x",
	    texmap->attributes[i].surface_object);
    if (DsInqObjStatus (texmap->attributes[i].surface_object) == DcObjectValid)
      {

       DsInqObjName (texmap->attributes[i].surface_object, &type, &name);
       switch (type)
	  {
	   case DcNameInteger:
	    printf ("\t(%d)\n", *(int *) name);
	    break;
	   case DcNameString:
	    printf ("\t(%s)\n", name);
	    break;
	   default:
	    printf ("\n");
	  }
      }
    printf ("\t\t    projection_type\t");
    switch (texmap->attributes[i].projection_type)
       {
	case DcObjCentroid:
	 printf ("DcObjCentroid\n");
	 break;
	case DcISN:
	 printf ("DcISN\n");
	 break;
	default:
	 printf ("<%d>\n", texmap->attributes[i].projection_type);
       }
    printf ("\t\t    uv idx\t%d\n", texmap->attributes[i].uv.idx);
    printf ("\t\t    uv extend\t(");
    for (j = 0; j < 2; j++)
      {
       switch (texmap->attributes[i].uv.extend[j])
	  {
	   case DcExtendNone:
	    printf ("DcExtendNone");
	    break;
	   case DcExtendBlack:
	    printf ("DcExtendBlack");
	    break;
	   case DcExtendClamp:
	    printf ("DcExtendClamp");
	    break;
	   case DcExtendRepeat:
	    printf ("DcExtendRepeat");
	    break;
	   default:
	    printf ("<%d>", texmap->attributes[i].uv.extend[j]);
	  }
       if (j == 0)
	printf (", ");
       else
	printf (")\n");
      }
    printf ("\t\t    uv matrix\n");
    printf ("\t\t\t(%g %g %g)\n",
	    texmap->attributes[i].uv.matrix[0][0],
	    texmap->attributes[i].uv.matrix[0][1],
	    texmap->attributes[i].uv.matrix[0][2]);
    printf ("\t\t\t(%g %g %g)\n",
	    texmap->attributes[i].uv.matrix[1][0],
	    texmap->attributes[i].uv.matrix[1][1],
	    texmap->attributes[i].uv.matrix[1][2]);
    printf ("\t\t\t(%g %g %g)\n",
	    texmap->attributes[i].uv.matrix[2][0],
	    texmap->attributes[i].uv.matrix[2][1],
	    texmap->attributes[i].uv.matrix[2][2]);
    printf ("\t\t    uvw idx\t%d\n",
	    texmap->attributes[i].uvw.idx);
    printf ("\t\t    uvw extend\t(");
    for (j = 0; j < 3; j++)
      {
       switch (texmap->attributes[i].uvw.extend[j])
	  {
	   case DcExtendNone:
	    printf ("DcExtendNone");
	    break;
	   case DcExtendBlack:
	    printf ("DcExtendBlack");
	    break;
	   case DcExtendClamp:
	    printf ("DcExtendClamp");
	    break;
	   case DcExtendRepeat:
	    printf ("DcExtendRepeat");
	    break;
	   default:
	    printf ("<%d>", texmap->attributes[i].uvw.extend[j]);
	  }
       if (j == 2)
	printf (")\n");
       else
	printf (", ");
      }
    printf ("\t\t    uvw matrix\n");
    printf ("\t\t\t(%g %g %g %g)\n",
	    texmap->attributes[i].uvw.matrix[0][0],
	    texmap->attributes[i].uvw.matrix[0][1],
	    texmap->attributes[i].uvw.matrix[0][2],
	    texmap->attributes[i].uvw.matrix[0][3]);
    printf ("\t\t\t(%g %g %g %g)\n",
	    texmap->attributes[i].uvw.matrix[1][0],
	    texmap->attributes[i].uvw.matrix[1][1],
	    texmap->attributes[i].uvw.matrix[1][2],
	    texmap->attributes[i].uvw.matrix[1][3]);
    printf ("\t\t\t(%g %g %g %g)\n",
	    texmap->attributes[i].uvw.matrix[2][0],
	    texmap->attributes[i].uvw.matrix[2][1],
	    texmap->attributes[i].uvw.matrix[2][2],
	    texmap->attributes[i].uvw.matrix[2][3]);
    printf ("\t\t\t(%g %g %g %g)\n",
	    texmap->attributes[i].uvw.matrix[3][0],
	    texmap->attributes[i].uvw.matrix[3][1],
	    texmap->attributes[i].uvw.matrix[3][2],
	    texmap->attributes[i].uvw.matrix[3][3]);
   }
}


/**************************************************************************
 **************************************************************************/

void
ddr_kopengl_drom_print_atts (void)
{
 DtInt i;

 printf ("\tAttributes\n");
 printf ("\t    Enabled lights: ");
 for (i = 0; i < dde_kopengl_drom_curviwdat->total_lights; i++)
   {
    if (dde_kopengl_drom_curviwdat->light[i].status)
     printf ("%d ", i);
   }
 printf ("\n");

 ddr_kopengl_print_opengl_matrix ("Projection Matrix", CAMERA->projection_matrix);
 ddr_kopengl_print_opengl_matrix ("Camera to World", CAMERA->cam_to_world_matrix);
 ddr_kopengl_print_opengl_matrix ("World to Camera", CAMERA->world_to_cam_matrix);
 ddr_kopengl_print_opengl_matrix ("Camera to Frustum", CAMERA->cam_to_frustum_matrix);

 printf ("\t    ambint\t\t%g\n", DROM.ambint);
 printf ("\t    ambswi\t\t%s\n", DROM.ambswi ? "DcOn" : "DcOff");
 printf ("\t    bacfacculble\t%s\n", DROM.bacfacculble ? "DcOn" : "DcOff");
 printf ("\t    bacfacculswi\t%s\n", DROM.bacfacculswi ? "DcOn" : "DcOff");
 printf ("\t    depcue_zfront\t%g\n", DROM.depcue_zfront);
 printf ("\t    depcue_zback\t%g\n", DROM.depcue_zback);
 printf ("\t    depcue_sfront\t%g\n", DROM.depcue_sfront);
 printf ("\t    depcue_sback\t%g\n", DROM.depcue_sback);
 printf ("\t    depcue_color\t(%g %g %g)\n",
	 DROM.depcue_color[0],
	 DROM.depcue_color[1],
	 DROM.depcue_color[2]);
 printf ("\t    depcueswi\t\t%s\n", DROM.depcueswi ? "DcOn" : "DcOff");
 printf ("\t    difclr\t\t(%g %g %g)\n",
	 DROM.difclr[0],
	 DROM.difclr[1],
	 DROM.difclr[2]);
 printf ("\t    difint\t\t%g\n", DROM.difint);
 printf ("\t    difswi\t\t%s\n", DROM.difswi ? "DcOn" : "DcOff");
 printf ("\t    hidsrfswi\t\t%s\n", DROM.hidsrfswi ? "DcOn" : "DcOff");
 printf ("\t    inttyp\t\t");
 switch (DROM.inttyp)
    {
     case DcConstantShade:
      printf ("DcConstantShade\n");
      break;
     case DcVertexShade:
      printf ("DcVertexShade\n");
      break;
     case DcSurfaceShade:
      printf ("DcSurfaceShade\n");
      break;
     default:
      printf ("<%d>\n", DROM.inttyp);
    }
 printf ("\t    line_type\t\t");
 switch (DROM.line_type)
    {
     case DcLineTypeSolid:
      printf ("DcLineTypeSolid\n");
      break;
     case DcLineTypeDash:
      printf ("DcLineTypeDash\n");
      break;
     case DcLineTypeDot:
      printf ("DcLineTypeDot\n");
      break;
     case DcLineTypeDotDash:
      printf ("DcLineTypeDotDash\n");
      break;
     default:
      printf ("<%d>\n", DROM.line_type);
    }
 printf ("\t    line_width\t\t%g\n", DROM.line_width);
 printf ("\t    localaasty\t\t");
 switch (DROM.localaasty)
    {
     case DcLocalAntiAliasFast:
      printf ("DcLocalAntiAliasFast\n");
      break;
     case DcLocalAntiAliasIntermediate:
      printf ("DcLocalAntiAliasIntermediate\n");
      break;
     case DcLocalAntiAliasBest:
      printf ("DcLocalAntiAliasBest\n");
      break;
     default:
      printf ("<%d>\n", DROM.localaasty);
    }
 printf ("\t    localaaswi\t\t%s\n", DROM.localaaswi ? "DcOn" : "DcOff");
 printf ("\t    refswi\t\t%s\n", DROM.refswi ? "DcOn" : "DcOff");
 printf ("\t    reptyp\t\t");
 switch (DROM.reptyp)
    {
     case DcPoints:
      printf ("DcPoints\n");
      break;
     case DcWireframe:
      printf ("DcWireframe\n");
      break;
     case DcSurface:
      printf ("DcSurface\n");
      break;
     case DcOutlines:
      printf ("DcOutlines\n");
      break;
     case DcSurfaceWireframeEdges:
      printf ("DcSurfaceWireframeEdges\n");
      break;
     case DcSurfaceOutlineEdges:
      printf ("DcSurfaceOutlineEdges\n");
      break;
     default:
      printf ("<%d>\n", DROM.reptyp);
    }
 printf ("\t    shaswi\t\t%s\n", DROM.shaswi ? "DcOn" : "DcOff");
 printf ("\t    shdidx\t\t%d\n", DROM.shdidx);
 printf ("\t    spcclr\t\t(%g %g %g)\n",
	 DROM.spcclr[0],
	 DROM.spcclr[1],
	 DROM.spcclr[2]);
 printf ("\t    spcfct\t\t%g\n", DROM.spcfct);
 printf ("\t    spcint\t\t%g\n", DROM.spcint);
 printf ("\t    spcswi\t\t%s\n", DROM.spcswi ? "DcOn" : "DcOff");
 printf ("\t    srdedgclr\t\t(%g %g %g)\n",
	 DROM.srfedgclr[0],
	 DROM.srfedgclr[1],
	 DROM.srfedgclr[2]);
 printf ("\t    srfshd\t\t");
 if (DROM.srfshd == DcShaderConstant)
   {
    printf ("DcShaderConstant\n");
   }
 else if (DROM.srfshd == DcShaderLightSource)
   {
    printf ("DcShaderLightSource\n");
   }
 else
   {
    printf ("0x%x\n", DROM.srfshd);
   }
 printf ("\t    transpclr\t\t(%g %g %g)\n",
	 DROM.transpclr[0],
	 DROM.transpclr[1],
	 DROM.transpclr[2]);
 printf ("\t    transpint\t\t%g\n", DROM.transpint);
 printf ("\t    transpswi\t\t%s\n", DROM.transpswi ? "DcOn" : "DcOff");
 printf ("\n");

 ddr_kopengl_drom_print_mapping (&(DROM.mapbmp), "mapbmp");
 ddr_kopengl_drom_print_mapping (&(DROM.mapdifclr), "mapdifclr");
 ddr_kopengl_drom_print_mapping (&(DROM.mapenv), "mapenv");
 ddr_kopengl_drom_print_mapping (&(DROM.maptrnint), "maptrnint");

}

/**************************************************************************
 **************************************************************************/

void 
ddr_kopengl_print_dore_matrix (char *title, DtMatrix4x4 matrix)
{
 int             i, j;

 printf ("\n** %s **\n", title);

 for (i = 0; i < 4; ++i)
   {
    for (j = 0; j < 4; ++j)
     printf ("%f ", matrix[i][j]);
    printf ("\n");
   }
}


/**************************************************************************
 **************************************************************************/

void 
ddr_kopengl_print_opengl_matrix (char *title, glReal *matrix)
{
 printf ("\n** %s **\n", title);
 printf ("%f %f %f %f\n", matrix[0], matrix[4], matrix[8], matrix[12]);
 printf ("%f %f %f %f\n", matrix[1], matrix[5], matrix[9], matrix[13]);
 printf ("%f %f %f %f\n", matrix[2], matrix[6], matrix[10], matrix[14]);
 printf ("%f %f %f %f\n", matrix[3], matrix[7], matrix[11], matrix[15]);

}
