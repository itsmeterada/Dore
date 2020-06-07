
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
   ddr_kopengl_drom_set_att_ambint
   ddr_kopengl_drom_set_att_ambswi
   ddr_kopengl_drom_set_att_bacfacculble 
   ddr_kopengl_drom_set_att_bacfacculswi
   ddr_kopengl_drom_set_att_depcue
   ddr_kopengl_drom_set_att_depcueswi
   ddr_kopengl_drom_set_att_difclr
   ddr_kopengl_drom_set_att_difint 
   ddr_kopengl_drom_set_att_difswi 
   ddr_kopengl_drom_set_att_hidsrfswi
   ddr_kopengl_drom_set_att_inttyp 
   ddr_kopengl_drom_set_att_lintyp
   ddr_kopengl_drom_set_att_linwid 
   ddr_kopengl_drom_set_att_localaaswi
   ddr_kopengl_drom_set_att_localaasty 
   ddr_kopengl_drom_set_att_refswi 
   ddr_kopengl_drom_set_att_reptyp
   ddr_kopengl_drom_set_att_shaswi 
   ddr_kopengl_drom_set_att_shdidx
   ddr_kopengl_drom_set_att_spcclr 
   ddr_kopengl_drom_set_att_spcfct 
   ddr_kopengl_drom_set_att_spcint 
   ddr_kopengl_drom_set_att_spcswi 
   ddr_kopengl_drom_set_att_srfedgclr 
   ddr_kopengl_drom_set_att_srfshd 
   ddr_kopengl_drom_set_att_transpclr 
   ddr_kopengl_drom_set_att_transpint 
   ddr_kopengl_drom_set_att_transpswi 

 **************************************************************************/


#include "drom.h"


/*  Ambient Intensity  */
void
ddr_kopengl_drom_set_att_ambint (DtReal intensity)
{
 GLfloat param[4];

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_ambint\n");
#endif

 DROM.ambint = intensity;
 
 param[0] = param[1] = param[2] = (DROM.ambswi) ? DROM.ambint : 0;
 param[3] = 1.0;
 glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, param);
}




/*  Ambient switch */
void
ddr_kopengl_drom_set_att_ambswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_ambswi\n");
#endif

 DROM.ambswi = switchvalue;

 ddr_kopengl_drom_set_att_ambint (DROM.ambint);
}



/* Backface Cullable */
void 
ddr_kopengl_drom_set_att_bacfacculble (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_bacfacculble\n");
#endif

 if ((DROM.bacfacculble = switchvalue) && DROM.bacfacculswi)
   {
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);
   }
 else
   {
    glDisable (GL_CULL_FACE);
   }
}




/* Backface Cull Switch */
void 
ddr_kopengl_drom_set_att_bacfacculswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_bacfacculswi\n");
#endif

 if ((DROM.bacfacculswi = switchvalue) && DROM.bacfacculble)
   {
    glCullFace (GL_BACK);
    glEnable (GL_CULL_FACE);
   }
 else
   {
    glDisable (GL_CULL_FACE);
   }
}




/* Depth Cue */
void 
ddr_kopengl_drom_set_att_depcue (DtReal       zfront, 
                                 DtReal       zback, 
                                 DtReal       sfront, 
                                 DtReal       sback, 
                                 DtColorModel colormodel, 
                                 DtReal       color[])
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_depcue\n");
#endif

 DROM.depcue_zfront = zfront;
 DROM.depcue_zback = zback;
 DROM.depcue_sfront = sfront;
 DROM.depcue_sback = sback;
 DROM.depcue_colormodel = colormodel;
 if (colormodel == DcRGB)
   {
    DROM.depcue_color[0] = color[0];
    DROM.depcue_color[1] = color[1];
    DROM.depcue_color[2] = color[2];
   }
}





/* Depth Cue Switch */
void 
ddr_kopengl_drom_set_att_depcueswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_depcueswi\n");
#endif

 DROM.depcueswi = switchvalue;
}



/*  Diffuse Color */
void 
ddr_kopengl_drom_set_att_difclr (DtColorModel colormodel, DtReal color[])
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_difclr\n");
#endif

 DROM.difclr[0] = color[0];
 DROM.difclr[1] = color[1];
 DROM.difclr[2] = color[2];

 ddr_kopengl_restore_diffuse_color ();
}



/*  Diffuse Intensity */
void 
ddr_kopengl_drom_set_att_difint (DtReal intensity)
{
 GLfloat param[4];

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_difint\n");
#endif

 DROM.difint = intensity;

 param[0] = param[1] = param[2] = (DROM.difswi) ? DROM.difint : 0; 
 param[3] = 1.0;
 glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, param);
}



/*  Diffuse Switch */
void 
ddr_kopengl_drom_set_att_difswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_difswi\n");
#endif

 DROM.difswi = switchvalue;

 ddr_kopengl_drom_set_att_difint (DROM.difint);
}



/*  Hidden Surface Switch */
void 
ddr_kopengl_drom_set_att_hidsrfswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_hidsrfswi\n");
#endif

 DROM.hidsrfswi = switchvalue;
 DDdevice_SetDepthBufferEnable (DROM.device, switchvalue);
}



/* Interpolation Type */
void 
ddr_kopengl_drom_set_att_inttyp (DtInterpType interptype)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_inttyp\n");
#endif

 DROM.inttyp = interptype;
 switch (DROM.inttyp)
   {
    /* Flat Shading */
    case DcConstantShade:
      glShadeModel (GL_FLAT);
      break;

    /* Gouraud Shading */
    case DcVertexShade:

    /* Phong Shading */
    case DcSurfaceShade:
      glShadeModel (GL_SMOOTH);
      break;
   }
}


/* Line Type */
void 
ddr_kopengl_drom_set_att_lintyp (DtLineType type)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_lintyp\n");
#endif

 DROM.line_type = type;

 if (type == DcLineTypeSolid) 
   {
    glDisable (GL_LINE_STIPPLE);
    return;
   }

 
 glEnable (GL_LINE_STIPPLE);
 switch (type) 
   {
    case DcLineTypeDash:
      glLineStipple (1, 0x3F00);
      break;

    case DcLineTypeDot:
      glLineStipple (1, 0x0303);
      break;

    case DcLineTypeDotDash:
      glLineStipple (1, 0xF060);
      break;

    default:
      fprintf (stderr, "[KOpenGL Driver] ...invalid line type\n");
      glDisable (GL_LINE_STIPPLE);
    }

 return;
}



/* Line Width */
void 
ddr_kopengl_drom_set_att_linwid (DtReal width)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_linwid\n");
#endif

 if (width < 1.0) 
   width = 1.0;
 DROM.line_width = width;

 glLineWidth (width);
}



/* Local Antialias Style */
void 
ddr_kopengl_drom_set_att_localaasty (DtLocalAntiAliasStyle style)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_localaasty\n");
#endif

 DROM.localaasty = style;
}



/* Local Antialg Switch */
void 
ddr_kopengl_drom_set_att_localaaswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_localaaswi\n");
#endif

 DROM.localaaswi = switchvalue;
}



/* Reflection Switch */
void 
ddr_kopengl_drom_set_att_refswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_refswi\n");
#endif

 DROM.refswi = switchvalue;
}


/* RepType */
void 
ddr_kopengl_drom_set_att_reptyp (DtRepType reptype)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_reptyp\n");
#endif


 DROM.reptyp = reptype;
}



/* Shadow Switch */
void 
ddr_kopengl_drom_set_att_shaswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_shaswi\n");
#endif

 DROM.shaswi = switchvalue;
}




/* Shade Index */
void 
ddr_kopengl_drom_set_att_shdidx (DtInt shdex)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_shdidx\n");
#endif

 DROM.shdidx = shdex;
}


/*  Specular Color  */
void 
ddr_kopengl_drom_set_att_spcclr (DtColorModel colormodel, DtReal color[])
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_spcclr\n");
#endif

 DROM.spcclr[0] = color[0];
 DROM.spcclr[1] = color[1];
 DROM.spcclr[2] = color[2];

 ddr_kopengl_restore_specular_color (); 
}



/*  Specular Factor */
void 
ddr_kopengl_drom_set_att_spcfct (DtReal factor)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_spcfct\n");
#endif

 DROM.spcfct = factor;

 ddr_kopengl_restore_specular_factor ();
}



/*  Specular Intensity */
void 
ddr_kopengl_drom_set_att_spcint (DtReal intensity)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_spcint\n");
#endif

 DROM.spcint = intensity;
}



/*  Specular Switch */
void 
ddr_kopengl_drom_set_att_spcswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_spcswi\n");
#endif

 DROM.spcswi = switchvalue;

 ddr_kopengl_restore_specular_factor ();
}



void 
ddr_kopengl_drom_set_att_srfedgclr (DtColorModel colormodel, DtReal color[])
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_srfedgclr\n");
#endif

 if (colormodel == DcRGB)
   {
    DROM.srfedgclr[0] = color[0];
    DROM.srfedgclr[1] = color[1];
    DROM.srfedgclr[2] = color[2];
   /* XXX */
   }
}




/*  Surface Shader Callback Object  */
void 
ddr_kopengl_drom_set_att_srfshd (DtObject callbackobj)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_srfshd\n");

    {   auto char buff[40];
        printf
        (   "ddr_kopengl_drom_set_att_srfshd (%s)\n",
            (callbackobj == DcShaderConstant)    ? "DcShaderConstant"    :
            (callbackobj == DcShaderLightSource) ? "DcShaderLightSource" :
            (sprintf(buff,"%lx",callbackobj), buff)
        );
        fflush (stdout);
    }

#endif

 DROM.srfshd = callbackobj;

 /* XXX */
 if (DROM.srfshd == DcShaderConstant)
   {
    glDisable (GL_LIGHTING);
    DROM.lighting = 0;
    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL);
   }
 else
   {
    /* 
       Turn on lighting calculations only if after 
       pre_init and if some lights have been defined 
    */
    if (!dde_kopengl_drom_pre_init && VIEW->total_lights)
      {
       glEnable (GL_LIGHTING);
       DROM.lighting = 1;
      }

    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL);
   }
}




/*  Transparency color */
void 
ddr_kopengl_drom_set_att_transpclr (DtColorModel colormodel, DtReal color[])
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_transpclr\n");
#endif

 if (colormodel == DcRGB)
   {
    DROM.transpclr[0] = color[0];
    DROM.transpclr[1] = color[1];
    DROM.transpclr[2] = color[2];
   }
}



/*  Transparency intensity */
void 
ddr_kopengl_drom_set_att_transpint (DtReal value)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_transpint\n");
#endif

 DROM.transpint = value;
}



/*  Transparency switch */
void 
ddr_kopengl_drom_set_att_transpswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_transpswi\n");
#endif

 DROM.transpswi = switchvalue;
}
