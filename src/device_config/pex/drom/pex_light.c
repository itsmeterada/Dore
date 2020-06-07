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
**  This file contains the routines and procedures that relate to lights
**  objects and light attributes.
*****************************************************************************/

#include "../pex_driver.h"



/*****************************************************************************
**  This routine executes a light object.  It saves the current values of the
**  light attributes along with the handle to the light object.  The
**  attributes include the local-to-world transformation, light color, type
**  intensity, spread angles, exponent, and attenuation.  The current local-
**  to-world transformation matrix defines the position and direction of the
**  light.  The position and direction are determined by transforming the
**  point <0,0,0> and the vector <0,0,-1> by the current local-to-world
**  transformation matrix.
*****************************************************************************/

void  ddr_pex_drom_light  (DtObject objid)
{
    auto     DtReal              dir[4];	/* Light Direction */
    static   DtReal              light_pos[4] = { 0,0, 0,1};	/* Position */
    static   DtReal              light_dir[4] = { 0,0,-1,1};	/* Direction */
    register ddt_pex_drom_light *new_light;	/* Pointer to New Light */
    auto     DtReal              pos[4];	/* Light Position */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_light (%lx)\n", objid);
	fflush (stdout);
#   endif

    /* If the PEX server does not support any more lights, skip this one. */

    if (DROM.light_index_pex > pex_status.light_max)
	return;
    
    /* If there are no more available slots in the light heap, then grow the
    ** heap to make more room.  The light heap never decreases in size. */

    if (DROM.light_heap_next >= DROM.light_heap_size)
    {
	DROM.light_heap_size += LIGHT_HEAP_GROW_SIZE;
	DROM.light_heap =
	    DDspace_Reallocate
	    (   DROM.light_heap,
		DROM.light_heap_size * sizeof(*(DROM.light_heap))
	    );
    }

    /* Set up the new light on the heap.  */

    new_light = DROM.light_heap + DROM.light_heap_next;

    new_light->pex_index = DROM.light_index_pex;

    ++DROM.light_heap_next;
    ++DROM.light_index_pex;

    /* Load the new light parameters into the new light on the heap. */

    new_light->objid     = objid;
    new_light->dore_type = DROM.newlight.dore_type;
    new_light->intensity = DROM.newlight.intensity;
    new_light->status    = DcOn;
    new_light->pex_entry = DROM.newlight.pex_entry;

    dor_math_homo_trns_fast (light_pos, DROM.M_local_world, pos);

    if (pos[3] != 0.0)
    {   pos[0] /= pos[3];
        pos[1] /= pos[3];
        pos[2] /= pos[3];
    }

    dor_math_homo_trns_fast (light_dir, DROM.M_local_world, dir);

    if (dir[3] != 0.0)
    {   dir[0] /= dir[3];
	dir[1] /= dir[3];
	dir[2] /= dir[3];
    }

    dir[0] -= pos[0];
    dir[1] -= pos[1];
    dir[2] -= pos[2];

    dor_math_renormalize_vector (dir);

    /* Set the light color scaled by the light intensity. */

#   define LIGHT_COLOR  new_light->pex_entry.color.value.rgb

    LIGHT_COLOR.red   *= new_light->intensity;
    LIGHT_COLOR.green *= new_light->intensity;
    LIGHT_COLOR.blue  *= new_light->intensity;

    /* Do the light source according to the capabilities of the PEX server.
    ** If the requested light type is not available, then approximate it with
    ** the nearest supported PEX light type.  */

    if (new_light->dore_type == DcLightSpot)
        new_light->pex_entry.type = pex_modes.light_spot;

    else if (new_light->dore_type == DcLightSpotAttn)
    {   new_light->pex_entry.type         = pex_modes.light_spot;
        new_light->pex_entry.attenuation1 = 1;
	new_light->pex_entry.attenuation2 = 0;
    }

    else if (new_light->dore_type == DcLightPointAttn)
    {
	new_light->pex_entry.type = pex_modes.light_point;
	if (pex_modes.light_point == PEXLightWCVector)
	{   dir[0] = -pos[0];
	    dir[1] = -pos[1];
	    dir[2] = -pos[2];
	    if (1e-6 > (dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]))
		dir[2] = -1;
	}
    }

    else if (new_light->dore_type == DcLightPoint)
    {
	new_light->pex_entry.type         = pex_modes.light_point;
	new_light->pex_entry.attenuation1 = 1;
	new_light->pex_entry.attenuation2 = 0;
	if (pex_modes.light_point == PEXLightWCVector)
	{   dir[0] = -pos[0];
	    dir[1] = -pos[1];
	    dir[2] = -pos[2];
	    if (1e-6 > (dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]))
		dir[2] = -1;
	}
    }

    else if (new_light->dore_type == DcLightInfinite)
	new_light->pex_entry.type = pex_modes.light_vector;

    else if (new_light->dore_type == DcLightAmbient)
	new_light->pex_entry.type = PEXLightAmbient;

    new_light->pex_entry.point.x = pos[0];
    new_light->pex_entry.point.y = pos[1];
    new_light->pex_entry.point.z = pos[2];

    new_light->pex_entry.direction.x = dir[0];
    new_light->pex_entry.direction.y = dir[1];
    new_light->pex_entry.direction.z = dir[2];

    /* Now add the light entry to the PEX light lookup-table. */

    PEXSetTableEntries
    (   DCM->display, DROM.light_LUT,
	new_light->pex_index, 1,
	PEXLUTLight,
	(PEXPointer)(&new_light->pex_entry)
    );

    /* Update the current light source state to include the new light. */

    UpdateLightState ();
}



/*****************************************************************************
**  This routine sets the light attenuation values lights that have an
**  associated position (currently spot and positional lights).  The light
**  intensity attenuation is given by the formula (1 / [c1 + c2*d]), where
**  c1 & c2 are the light attenuation factors, and d is the distance from the
**  illuminated surface point to the light position.  For lights with no
**  light attenuation, c1=1 and c2=0.
*****************************************************************************/

void  ddr_pex_drom_set_att_lgtatn  (
    DtReal c1,		/* Light Attenuation Factor 1 */
    DtReal c2)		/* Light Attenuation Factor 2 */
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_lgtatn (%f, %f)\n", c1, c2);
	fflush (stdout);
#   endif

    DROM.newlight.pex_entry.attenuation1 = c1;
    DROM.newlight.pex_entry.attenuation2 = c2;
}



/*****************************************************************************
**  This function sets the color of the new light source.  If the colormodel
**  is other than RGB, then this function has no effect.
*****************************************************************************/

void  ddr_pex_drom_set_att_lgtclr  (
    DtColorModel colormodel,	/* Should be DcRGB */
    DtReal       color[])	/* Light RGB Color */
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_lgtclr (%d, [%f,%f,%f])\n",
	    colormodel, color[0], color[1], color[2]);
	fflush (stdout);
#   endif

    if (colormodel != DcRGB) return;

    SETCOLOR (DROM.newlight.pex_entry.color, color[0], color[1], color[2]);
}



/*****************************************************************************
**  This routine defines the intensity of subsequently defined lights.  Since
**  this is not a paramter of PEX light sources, we'll multiply this value by
**  the light color when we define the light source.
*****************************************************************************/

void  ddr_pex_drom_set_att_lgtint  (DtReal intensity)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_lgtint (%f)\n", intensity);
	fflush (stdout);
#   endif

    DROM.newlight.intensity = intensity;
}



/*****************************************************************************
**  This routine sets the spotlight spread angle for subsequent spotlights.
**  Since the PEX spotlight uses only a spread angle and a cosine-exponent
**  falloff, we ignore the delta parameter.
*****************************************************************************/

void  ddr_pex_drom_set_att_lgtspdang  (
    DtReal total,	/* Maximum Spotlight Lighting Angle */
    DtReal delta)	/* Maximum Spotlight Full Intensity Angle */
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_lgtspdang (%f, %f)\n", total, delta);
	fflush (stdout);
#   endif

    DROM.newlight.pex_entry.spread_angle = total;
}



/*****************************************************************************
**  This procedure sets the spotlight cosine exponent for future spotlights.
*****************************************************************************/

void  ddr_pex_drom_set_att_lgtspdexp  (DtReal exp)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_lgtspdexp (%f)\n", exp);
	fflush (stdout);
#   endif

    DROM.newlight.pex_entry.concentration = exp;
}



/*****************************************************************************
**  This subroutine turns off all lights given in the list of lights.  The
**  `count' parameter gives the number of lights to turn off, and the `lights'
**  paramter is an array of light object handles for lights that are to be
**  disabled.  This function is called each time the list of disabled lights
**  changes and is usually called during the display traversal -- hence, the
**  effect needs to be immediate.
*****************************************************************************/

void  ddr_pex_drom_set_att_lgtswi  (
    DtInt     count,	/* Number of Lights to Turn Off */
    DtObject *lights)	/* Object Handles of Lights to Turn Off */
{
    register ddt_pex_drom_light *light;		/* Current Light */
    register int                 lightnum;	/* Light Number */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_lgtswi (%d, %lx)\n", count, lights);
	fflush (stdout);
#   endif

    if (dde_pex_drom_pre_init)
	return;

    light = DROM.light_heap;
    lightnum = 0;

    while (lightnum++ < DROM.light_heap_size)
    {
	register int switchnum;		/* Light Switch Array Index */

	/* All lights are set on unless explicitely disabled from the
	** switch array. */

	light->status = DcOn;

	/* For each light in the light heap, scan through the switch array
	** to find a match with the object handle.  If the handle matches,
	** then the light is to be turned off.  */

	for (switchnum = 0;  switchnum < count;  ++switchnum)
	{
	    if (light->objid == lights[switchnum])
	    {   light->status = DcOff;
		break;
	    }
	}

	++ light;
    }

    /* Update the PEX light-source state. */

    UpdateLightState ();
}



/*****************************************************************************
**  This procedure sets the light type for future light sources.  When a
**  new light is actually instantiated, then we'll determine if the current
**  PEX server can actually support the requested light type.  If not, then
**  we'll try to fake it with a different type of light source.
*****************************************************************************/

void  ddr_pex_drom_set_att_lgttyp  (DtObject type)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_set_att_lgttyp (%lx)\n", type);
	fflush (stdout);
#   endif

    DROM.newlight.dore_type = type;
}



/*****************************************************************************
**  This routine notifies PEX of an updated light source state.
*****************************************************************************/

void UpdateLightState (void)
{
    auto     int                 enabled;	/* Lights Enabled  Index */
    auto     int                 disabled;	/* Lights Disabled Index */
    register ddt_pex_drom_light *light;		/* Current Light */
    register int                 lightnum;	/* Light Number */
    auto     int                 num_lights;	/* Number of Current Lights */
    auto     PEXTableIndex      *state;		/* PEX Arrray of Light States */

#   if DEBUG_CALL
	print ("UpdateLightState ()\n");
	fflush (stdout);
#   endif

    /* At the end of this procdure, PEX will expect an array of enabled
    ** light indices, and an array of disabled light indices.  In order to
    ** traverse the light heap only once, I allocate a single array and
    ** load the enabled light indices in the front and the disabled light
    ** indices in the back.  For example, given the light list
    ** 1,(2),3,(4),(5),6,7,8, where the lights with indices in parenthesis
    ** are off, the final array will be 1,3,6,7,8,5,4,2.  Both the `enabled'
    ** and `disabled' values will index the 6th entry (value 5).  */

    num_lights = DROM.light_heap_next;

    state = DDspace_Allocate (num_lights * sizeof(*state));

    light    = DROM.light_heap;
    lightnum = 0;
    enabled  = 0;
    disabled = num_lights;

    while (lightnum++ < num_lights)
    {
	/* Stuff the PEX light index into either the enabled portion
	*  or the disabled portion of the light source array, depending
	*  on the light status. */

	if (light->status == DcOn)
	    state[enabled++]  = light->pex_index;
	else
	    state[--disabled] = light->pex_index;

	++ light;
    }

    PEXSetLightSourceState
    (   DCM->display, PEX.renderer, PEXOCRender,
	enabled, &state[0], num_lights - enabled, &state[disabled]
    );

    DDspace_Deallocate ((char *) state);
}
