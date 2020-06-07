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
**  This file contains print routines used for debugging.
*****************************************************************************/

#include "../pex_driver.h"



/*****************************************************************************
**  This procedure prints the contents of the matrix passed in `matrix'.
*****************************************************************************/

void  PrintMatrix  (DtMatrix4x4 matrix)
{
    printf ("        % #12.5f % #12.5f % #12.5f % #12.5f\n",
	    matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3]);
    printf ("        % #12.5f % #12.5f % #12.5f % #12.5f\n",
	    matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3]);
    printf ("        % #12.5f % #12.5f % #12.5f % #12.5f\n",
	    matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
    printf ("        % #12.5f % #12.5f % #12.5f % #12.5f\n",
	    matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]);
}



/*****************************************************************************
*****************************************************************************/

void PrintStudio (void)
{
    print  ("=== Studio Attributes ===\n\n");
    printf ("    device_data:\n");

    printf ("    visual_type      ");
    switch (DEVICE->visual_type) {
        case DcStaticGrey:
	    printf ("DcStaticGrey\n");	break;
        case DcGreyScale:
	    printf ("DcGreyScale\n");	break;
        case DcStaticColor:
	    printf ("DcStaticColor\n");	break;
        case DcPseudoColor:
	    printf ("DcPseudoColor\n");	break;
        case DcTrueColor:
	    printf ("DcTrueColor\n");	break;
        case DcDirectColor:
	    printf ("DcDirectColor\n");	break;
        default:
	    printf ("<%d>\n", DEVICE->visual_type);
    }

    printf ("    shade_mode      ");
    switch (DEVICE->shade_mode) {
        case DcRange:
	    printf ("DcRange\n");	break;
        case DcComponent:
	    printf ("DcComponent\n");	break;
        default:
	    printf ("<%d>\n", DEVICE->shade_mode);
    }

    printf ("    ctodtrans       (%g %g %g)\n",
	    DEVICE->ctodtrans[0],
	    DEVICE->ctodtrans[1],
	    DEVICE->ctodtrans[2]);
    printf ("    ctodscale       (%g %g %g)\n",
	    DEVICE->ctodscale[0],
	    DEVICE->ctodscale[1],
	    DEVICE->ctodscale[2]);
    printf ("    ftodscale       (%g %g %g)\n",
	    DEVICE->ftodscale[0],
	    DEVICE->ftodscale[1],
	    DEVICE->ftodscale[2]);
    printf ("    annoscale       (%g %g %g)\n",
	    DEVICE->annoscale[0],
	    DEVICE->annoscale[1],
	    DEVICE->annoscale[2]);

    printf ("    view_data:\n");
    printf ("    background:     (%g %g %g)\n",
	    VIEW->background_color[0],
	    VIEW->background_color[1],
	    VIEW->background_color[2]);
    printf ("    background_just (%g %g)\n",
	    VIEW->background_just[0],
	    VIEW->background_just[1]);

    printf ("    camera:\n");
    printf ("       type         ");
    switch (DROM.camera_type) {
    case DcCameraArbitrary:	printf ("DcCameraArbitrary\n");		break;
    case DcCameraParallel:	printf ("DcCameraParallel\n");		break;
    case DcCameraPerspective:	printf ("DcCameraPerspective\n");	break;
    case DcCameraProjection:	printf ("DcCameraProjection\n");	break;
    default:			printf ("<%d>\n", DROM.camera_type);
    }
    printf ("       M_projection\n");     PrintMatrix (DROM.M_projection);
    printf ("       M_camera_world\n");   PrintMatrix (DROM.M_camera_world);
    printf ("       M_world_camera\n");   PrintMatrix (DROM.M_world_camera);
    printf ("       M_world_frustum\n");  PrintMatrix (DROM.M_world_frustum);

    printf ("   view_data\n");
    printf ("    window		(%g %g) (%g %g)\n",
	    WINDOW->actviewxmin,
	    WINDOW->actviewymin,
	    WINDOW->actviewxmax,
	    WINDOW->actviewymax);
}



/*****************************************************************************
*****************************************************************************/

#define ON_OFF(flag)   ((flag) ? "On" : "Off")
#define COLORS(C)  (C)[0], (C)[1], (C)[2]

void PrintAttributes (void)
{
    printf ("=== Rendering Attributes ===\n");

    printf ("    M_local_world\n");    PrintMatrix (DROM.M_local_world);
    printf ("    M_local_frustum\n");  PrintMatrix (DROM.M_local_frustum);
    printf ("\n");

    printf ("    ambient switch   : %s\n", ON_OFF(DROM.ambient_switch));
    printf ("    diffuse switch   : %s\n", ON_OFF (DROM.diffuse_switch));
    printf ("    specular switch  : %s\n", ON_OFF (DROM.specular_switch));
    printf ("    transp. switch   : %s\n", ON_OFF (DROM.transp_switch));
    printf ("    bfcull switch    : %s\n", ON_OFF(DROM.bfcull_switch));

    printf ("\n");

    printf ("    ambient intensity: %g\n", DROM.ambient_intens);
    printf ("    bf cullable      : %s\n", ON_OFF(DROM.bfcullable));
    printf ("    depthcue switch  : %s\n", ON_OFF (DROM.depthcue.mode));
    printf ("    depthcue zfront  : %g\n", DROM.depthcue.front_plane);
    printf ("    depthcue zback   : %g\n", DROM.depthcue.back_plane);
    printf ("    depthcue sfront  : %g\n", DROM.depthcue.front_scaling);
    printf ("    depthcue sback   : %g\n", DROM.depthcue.back_scaling);
    printf ("    depthcue color   : (%g %g %g)\n",
	DROM.depthcue.color.value.rgb.red,
	DROM.depthcue.color.value.rgb.green,
	DROM.depthcue.color.value.rgb.blue);
    printf ("    diffuse intens   : (%g)\n", DROM.diffuse_intens);
    printf ("    object rep type  : ");
    switch (DROM.rep_type)
    {   case DcPoints:                printf ("DcPoints\n");              break;
        case DcWireframe:             printf ("DcWireframe\n");           break;
        case DcSurface:               printf ("DcSurface\n");             break;
        case DcOutlines:              printf ("DcOutlines\n");            break;
        case DcSurfaceWireframeEdges: printf("DcSurfaceWireframeEdges\n");break;
        case DcSurfaceOutlineEdges:   printf ("DcSurfaceOutlineEdges\n"); break;
        default:                      printf ("<%d>\n", DROM.rep_type);   break;
    }
    printf ("    specular intens. : %g\n", DROM.specular_intens);
    printf ("    surface shader   : ");
    if (DROM.shading_model == PEXReflectionNone)
	printf ("DcShaderConstant\n");
    else if (DROM.shading_model == PEXReflectionSpecular)
	printf ("DcShaderLightSource\n");
    else
	printf ("0x%x\n", DROM.shading_model);

    printf ("    transp. intensity: %g\n", DROM.transp_intens);
}
