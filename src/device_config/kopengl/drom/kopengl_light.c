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
   ddr_kopengl_drom_light 
   ddr_kopengl_drom_set_att_lgtatn 
   ddr_kopengl_drom_set_att_lgtclr 
   ddr_kopengl_drom_set_att_lgtint 
   ddr_kopengl_drom_set_att_lgtspdang 
   ddr_kopengl_drom_set_att_lgtspdexp
   ddr_kopengl_drom_set_att_lgtswi 
   ddr_kopengl_drom_set_att_lgttyp 
   ddr_kopengl_drom_update_display_light 
   ddr_kopengl_drom_update_display_lights 

 **************************************************************************/

#include "dore/internal/matrix.h"
#include "drom.h"



/*****************************************************************************
 *  This routine executes a light object.  It saves the current values of the
 *  light attributes along with the handle to the light object.  The
 *  attributes include the local-to-world transformation, light color, type
 *  intensity, spread angles, exponent, and attenuation.  The current local-
 *  to-world transformation matrix defines the position and direction of the
 *  light.  The position and direction are determined by transforming the
 *  point <0,0,0> and the vector <0,0,-1> by the current local-to-world
 *  transformation matrix. This is done in drom.update_display_light. 
 ****************************************************************************/

void 
ddr_kopengl_drom_light (DtObject light)
{
 int             lightno;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.light\n");
#endif

 /*
  * If the total number of lights exceeds that supported by
  * OpenGL, lets skip this excercise. VIEW->max_lights is 
  * set to GL_MAX_LIGHTS in drom.initialize_view
  */
 if (VIEW->total_lights + 1 > VIEW->max_lights)
   {
    DDerror (ERR_VALUE_OUT_OF_RANGE, 
             "ddr_kopengl_drom_light", 
             "Reached OpenGL limit on lights");
    return;
   }


 lightno = (VIEW->total_lights)++;

 /*
  * Get the current model view matrix. This is the world to light matrix
  * transform. Invert it to get the light_to_world matrix transform. 
  * Save this for later use in drom.update_lights.
  */
 {
  glReal      temp1[16];
  DtMatrix4x4 temp2;

  GET_GL_MATRIX (GL_MODELVIEW_MATRIX, temp1); 
  CONVERT_OPENGL_TO_DORE (temp2, temp1);
  dor_matrix_invert (temp2);
  CONVERT_DORE_TO_OPENGL (VIEW->light[lightno].light_to_world_matrix, temp2);
 }
                
#ifdef _DEBUG_KOPENGL_
 ddr_kopengl_print_opengl_matrix ("light_to_world_matrix", 
                                  VIEW->light[lightno].light_to_world_matrix);
#endif

 VIEW->light[lightno].light        = light;
 VIEW->light[lightno].type         = DROM.light_type;

 VIEW->light[lightno].position[0]  = 0.0;
 VIEW->light[lightno].position[1]  = 0.0;
 VIEW->light[lightno].position[2]  = 0.0;

 VIEW->light[lightno].direction[0] =  0.0;
 VIEW->light[lightno].direction[1] =  0.0;
 VIEW->light[lightno].direction[2] = -1.0;

 VIEW->light[lightno].color[0]     = DROM.light_color[0]*DROM.light_intensity; 
 VIEW->light[lightno].color[1]     = DROM.light_color[1]*DROM.light_intensity; 
 VIEW->light[lightno].color[2]     = DROM.light_color[2]*DROM.light_intensity; 

 VIEW->light[lightno].angle_total  = DROM.light_spread_angle_total;
 VIEW->light[lightno].angle_delta  = DROM.light_spread_angle_delta;
 VIEW->light[lightno].exponent     = DROM.light_spread_exponent;

 VIEW->light[lightno].c1           = DROM.light_att_c1;
 VIEW->light[lightno].c2           = DROM.light_att_c2;

 VIEW->light[lightno].status       = DcOn;
}


/*****************************************************************************
 *  This routine sets the light attenuation values lights that have an
 *  associated position (currently spot and positional lights).  The light
 *  intensity attenuation is given by the formula (1 / [c1 + c2*d]), where
 *  c1 & c2 are the light attenuation factors, and d is the distance from the
 *  illuminated surface point to the light position.  For lights with no
 *  light attenuation, c1=1 and c2=0.
 ****************************************************************************/

void 
ddr_kopengl_drom_set_att_lgtatn (DtReal c1, DtReal c2)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_lgtatn\n");
#endif

 DROM.light_att_c1 = c1;
 DROM.light_att_c2 = c2;
}


/*****************************************************************************
 *  This function sets the color of the new light source.  If the colormodel
 *  is other than RGB, then this function has no effect.
 ****************************************************************************/

void 
ddr_kopengl_drom_set_att_lgtclr (DtColorModel colormodel, DtReal color[])
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_lgtclr\n");
#endif

 if (colormodel == DcRGB)
   {
    DROM.light_color[0] = color[0];
    DROM.light_color[1] = color[1];
    DROM.light_color[2] = color[2];
   }
}

/*****************************************************************************
 *  This routine defines the intensity of subsequently defined lights. Each
 *  time drom.light is called, the current light intensity attribute value
 *  is stored with the other light attributes. 
 ****************************************************************************/

void 
ddr_kopengl_drom_set_att_lgtint (DtReal intensity)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_lgtint\n");
#endif

 DROM.light_intensity = intensity;
}


/*****************************************************************************
 *  This routine sets the spotlight spread angle for subsequent spotlights.
 ****************************************************************************/

void 
ddr_kopengl_drom_set_att_lgtspdang (DtReal total, DtReal delta)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_lgtspdang\n");
#endif

 DROM.light_spread_angle_total = total;
 DROM.light_spread_angle_delta = delta;
}


/*****************************************************************************
**  This procedure sets the spotlight cosine exponent for future spotlights.
*****************************************************************************/

void 
ddr_kopengl_drom_set_att_lgtspdexp (DtReal exp)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_lgtspdexp\n");
#endif

 DROM.light_spread_exponent = exp;
}


/*****************************************************************************
 *  This subroutine turns off all lights given in the list of lights.  The
 *  `count' parameter gives the number of lights to turn off, and the `lights'
 *  paramter is an array of light object handles for lights that are to be
 *  disabled.  This function is called each time the list of disabled lights
 *  changes and is usually called during the display traversal -- hence, the
 *  effect needs to be immediate.
 ****************************************************************************/

void 
ddr_kopengl_drom_set_att_lgtswi (DtInt count, DtObject * lights)
{
 DtInt           i;
 DtInt           lightidx;
 DtInt           enabled;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_lgtswi\n");
#endif

 /* Do nothing if lights have not yet been initialized */ 
 if ( dde_kopengl_drom_pre_init || (!VIEW) )
   return;

 if (!VIEW->total_lights)
   DROM.lighting = 0;
 else 
   {
    DROM.lighting = 1;
    glEnable (GL_LIGHTING);
   }

 /* For all possible lights .... */
 for (lightidx = 0; lightidx < VIEW->total_lights; lightidx++)
   {
    /* Let the default status be True  (i.e. light is on ) */
    enabled = DcTrue;

    /* 
     * If the current light is one of the lights passed 
     * to this function (i.e. a light to be turned off)
     * set enabled to False.
     */
    for (i = 0; i < count; i++)
      {
       if (VIEW->light[lightidx].light == lights[i])
	 {
	  enabled = DcFalse;
	  break;
	 }
      }

    /*
     * Set the light state in the appropriate structure 
     */ 
    VIEW->light[lightidx].status = enabled;

    /*
     * Enable/Disable the light for OpenGL accordingly 
     */
    if (enabled)
      glEnable (GL_LIGHT0 + lightidx); 
    else
      glDisable (GL_LIGHT0 + lightidx); 
   }
}




/*****************************************************************************
 *  This procedure sets the light type for future light sources.  
 ****************************************************************************/

void 
ddr_kopengl_drom_set_att_lgttyp (DtObject type)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_lgttyp\n");
#endif

 DROM.light_type = type;
}


#define RadToDegrees(rad) ((rad)*57.2958)

void
ddr_kopengl_drom_update_display_light (int light_num)
{
 GLfloat param[8];
 GLfloat spread_angle;
 GLfloat spot_exp;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_display_light\n");
#endif


#ifdef _DEBUG_KOPENGL_
 printf ("Updating light number %d\n", light_num);
#endif

 if (VIEW->light[light_num].type == DcLightAmbient)
   {
    param[0] = VIEW->light[light_num].color[0]; 
    param[1] = VIEW->light[light_num].color[1]; 
    param[2] = VIEW->light[light_num].color[2]; 
    param[3] = 1.0; 
    glLightfv (GL_LIGHT0 + light_num, GL_AMBIENT, param); 

    /* Set the diffuse and specular components of light to zero */
    param[0] = 0.0;
    param[1] = 0.0;
    param[2] = 0.0;
    param[3] = 1.0; 
    glLightfv (GL_LIGHT0 + light_num, GL_DIFFUSE, param);

    param[0] = 0.0;
    param[1] = 0.0;
    param[2] = 0.0;
    param[3] = 1.0;
    glLightfv (GL_LIGHT0 + light_num, GL_SPECULAR, param);
   }
 else
   {
    /* Set the ambient intensity of the light to zero */
    param[0] = 0.0; 
    param[1] = 0.0; 
    param[2] = 0.0; 
    param[3] = 1.0;
    glLightfv (GL_LIGHT0 + light_num, GL_AMBIENT, param);

    param[0] = VIEW->light[light_num].color[0];
    param[1] = VIEW->light[light_num].color[1];
    param[2] = VIEW->light[light_num].color[2];
    param[3] = 1.0;
    glLightfv (GL_LIGHT0 + light_num, GL_DIFFUSE, param);

    param[0] = VIEW->light[light_num].color[0];
    param[1] = VIEW->light[light_num].color[1];
    param[2] = VIEW->light[light_num].color[2];
    param[3] = 1.0;
    glLightfv (GL_LIGHT0 + light_num, GL_SPECULAR, param);
   }

 /*
  * Set the unique light properties for each light type 
  */

 if ( (VIEW->light[light_num].type == DcLightSpot) ||
      (VIEW->light[light_num].type == DcLightSpotAttn) )
   {
    param[0] = VIEW->light[light_num].direction[0];
    param[1] = VIEW->light[light_num].direction[1];
    param[2] = VIEW->light[light_num].direction[2];
    glLightfv (GL_LIGHT0 + light_num, GL_SPOT_DIRECTION, param);

    spread_angle = RadToDegrees (VIEW->light[light_num].angle_total);
    if (spread_angle > 90)
      spread_angle = 90;

    glLightf (GL_LIGHT0 + light_num, GL_SPOT_CUTOFF, spread_angle); 

    spot_exp = VIEW->light[light_num].exponent > 128.0 ? 
               128.0 :
               VIEW->light[light_num].exponent;
    glLightf (GL_LIGHT0 + light_num, GL_SPOT_EXPONENT, spot_exp);
   } 

 /*
  * Set common light properties for each light type 
  */
 
 if (VIEW->light[light_num].type == DcLightInfinite ||
     VIEW->light[light_num].type == DcLightAmbient)
   {
    param[0] = -(VIEW->light[light_num].direction[0]);
    param[1] = -(VIEW->light[light_num].direction[1]);
    param[2] = -(VIEW->light[light_num].direction[2]);
    param[3] = 0.0;
   }
 else
   {
    param[0] = VIEW->light[light_num].position[0];
    param[1] = VIEW->light[light_num].position[1];
    param[2] = VIEW->light[light_num].position[2];
    param[3] = 1.0;
   }

 /* Current model view matrix should be the viewing matrix */ 
 glPushMatrix ();
 
 /* 
  * Multiply current model view matrix by light_to_world matrix.
  * This will transform points in the light space to the viewing 
  * space.
  */
 GL_MULT_MATRIX (VIEW->light[light_num].light_to_world_matrix);
 glLightfv (GL_LIGHT0 + light_num, GL_POSITION, param);

 /* Restore viewing matrix */
 glPopMatrix ();

 /* Enable lighting calculations */
 DROM.lighting = 1;
 glEnable (GL_LIGHTING);
 glEnable (GL_LIGHT0 + light_num);

#ifdef _DEBUG_KOPENGL_
 {
  glReal tmp[16];

  GET_GL_MATRIX (GL_MODELVIEW_MATRIX, tmp);
  ddr_kopengl_print_opengl_matrix ("modelview after disp_light", tmp);
 }
#endif
}


void 
ddr_kopengl_drom_update_display_lights (void)
{
 int l;

#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_display_lights\n");
#endif

 glMatrixMode (GL_MODELVIEW);
 glPushMatrix ();
 LOAD_GL_MATRIX (CAMERA->world_to_cam_matrix);

 /* 
  * Turn lights on 
  */
 for (l=0; l < VIEW->total_lights; l++) 
   ddr_kopengl_drom_update_display_light (l);

 /* 
  * Turn the rest of the lights off
  */
 for (; l < VIEW->max_lights; l++)
   glDisable (GL_LIGHT0 + l);

 glPopMatrix();
}


