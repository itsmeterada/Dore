
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

 Functions:
   ddr_kopengl_drom_set_att_mapbmp 
   ddr_kopengl_drom_set_att_mapbmpswi 
   ddr_kopengl_drom_set_att_mapdifclr 
   ddr_kopengl_drom_set_att_mapdifclrswi 
   ddr_kopengl_drom_set_att_mapenv 
   ddr_kopengl_drom_set_att_mapenvswi 
   ddr_kopengl_drom_set_att_maptrnint 
   ddr_kopengl_drom_set_att_maptrnintswi 

 ****************************************************************************/


#include "drom.h"

/*****************************************************************************
 ****************************************************************************/

void
ddr_kopengl_drom_set_att_mapbmp (DtInt         count, 
                                 DtObject      mapping[], 
                                 DtObject      raster[], 
                                 dot_stdtexatt attributes[])

{
 DtInt           i;

 if (count > DROM.mapbmp.max_count)
   {
    DROM.mapbmp.mapping = (DtObject *)
     DDspace_Reallocate (DROM.mapbmp.mapping,
			 count * sizeof (DtObject));
    DROM.mapbmp.raster = (DtObject *)
     DDspace_Reallocate (DROM.mapbmp.raster,
			 count * sizeof (DtObject));
    DROM.mapbmp.attributes = (dot_stdtexatt *)
     DDspace_Reallocate (DROM.mapbmp.attributes,
			 count * sizeof (dot_stdtexatt));
    if (DROM.mapbmp.mapping == (DtObject *) 0 ||
	DROM.mapbmp.raster == (DtObject *) 0 ||
	DROM.mapbmp.attributes == (dot_stdtexatt *) 0)
     return;
    DROM.mapbmp.max_count = count;
   }
 DROM.mapbmp.count = count;

 for (i = 0; i < count; i++)
   {
    DROM.mapbmp.mapping[i] = mapping[i];
    DROM.mapbmp.raster[i] = raster[i];
    DROM.mapbmp.attributes[i] = attributes[i];
   }
}


/*****************************************************************************
 ****************************************************************************/

void
ddr_kopengl_drom_set_att_mapbmpswi (DtSwitch switchvalue)
{
 DROM.mapbmp.status = switchvalue;
}



/*****************************************************************************
 ****************************************************************************/

void
ddr_kopengl_drom_set_att_mapdifclr (DtInt         count, 
                                    DtObject      mapping[], 
                                    DtObject      raster[], 
                                    dot_stdtexatt attributes[])
{
 DtInt           i;

 if (count > DROM.mapdifclr.max_count)
   {
    DROM.mapdifclr.mapping = (DtObject *)
     DDspace_Reallocate (DROM.mapdifclr.mapping,
			 count * sizeof (DtObject));
    DROM.mapdifclr.raster = (DtObject *)
     DDspace_Reallocate (DROM.mapdifclr.raster,
			 count * sizeof (DtObject));
    DROM.mapdifclr.attributes = (dot_stdtexatt *)
     DDspace_Reallocate (DROM.mapdifclr.attributes,
			 count * sizeof (dot_stdtexatt));
    if (DROM.mapdifclr.mapping == (DtObject *) 0 ||
	DROM.mapdifclr.raster == (DtObject *) 0 ||
	DROM.mapdifclr.attributes == (dot_stdtexatt *) 0)
     return;
    DROM.mapdifclr.max_count = count;
   }
 DROM.mapdifclr.count = count;

 for (i = 0; i < count; i++)
   {
    DROM.mapdifclr.mapping[i] = mapping[i];
    DROM.mapdifclr.raster[i] = raster[i];
    DROM.mapdifclr.attributes[i] = attributes[i];
   }
}



/*****************************************************************************
 ****************************************************************************/

void
ddr_kopengl_drom_set_att_mapdifclrswi (DtSwitch switchvalue)
{
 DROM.mapdifclr.status = switchvalue;
}



/*****************************************************************************
 ****************************************************************************/

void
ddr_kopengl_drom_set_att_mapenv (DtInt         count, 
                                 DtObject      mapping[], 
                                 DtObject      raster[], 
                                 dot_stdtexatt attributes[])
{
 DtInt           i;

 if (count > DROM.mapenv.max_count)
   {
    DROM.mapenv.mapping = (DtObject *)
     DDspace_Reallocate (DROM.mapenv.mapping,
			 count * sizeof (DtObject));
    DROM.mapenv.raster = (DtObject *)
     DDspace_Reallocate (DROM.mapenv.raster,
			 count * sizeof (DtObject));
    DROM.mapenv.attributes = (dot_stdtexatt *)
     DDspace_Reallocate (DROM.mapenv.attributes,
			 count * sizeof (dot_stdtexatt));
    if (DROM.mapenv.mapping == (DtObject *) 0 ||
	DROM.mapenv.raster == (DtObject *) 0 ||
	DROM.mapenv.attributes == (dot_stdtexatt *) 0)
     return;
    DROM.mapenv.max_count = count;
   }
 DROM.mapenv.count = count;

 for (i = 0; i < count; i++)
   {
    DROM.mapenv.mapping[i] = mapping[i];
    DROM.mapenv.raster[i] = raster[i];
    DROM.mapenv.attributes[i] = attributes[i];
   }
}



/*****************************************************************************
 ****************************************************************************/

void
ddr_kopengl_drom_set_att_mapenvswi (DtSwitch switchvalue)
{
 DROM.mapenv.status = switchvalue;
}



/*****************************************************************************
 ****************************************************************************/

void
ddr_kopengl_drom_set_att_maptrnint (DtInt         count,
                                    DtObject      mapping[], 
                                    DtObject      raster[], 
                                    dot_stdtexatt attributes[])
{
 DtInt           i;

 if (count > DROM.maptrnint.max_count)
   {
    DROM.maptrnint.mapping = (DtObject *)
     DDspace_Reallocate (DROM.maptrnint.mapping,
			 count * sizeof (DtObject));
    DROM.maptrnint.raster = (DtObject *)
     DDspace_Reallocate (DROM.maptrnint.raster,
			 count * sizeof (DtObject));
    DROM.maptrnint.attributes = (dot_stdtexatt *)
     DDspace_Reallocate (DROM.maptrnint.attributes,
			 count * sizeof (dot_stdtexatt));
    if (DROM.maptrnint.mapping == (DtObject *) 0 ||
	DROM.maptrnint.raster == (DtObject *) 0 ||
	DROM.maptrnint.attributes == (dot_stdtexatt *) 0)
     return;
    DROM.maptrnint.max_count = count;
   }
 DROM.maptrnint.count = count;

 for (i = 0; i < count; i++)
   {
    DROM.maptrnint.mapping[i] = mapping[i];
    DROM.maptrnint.raster[i] = raster[i];
    DROM.maptrnint.attributes[i] = attributes[i];
   }
}



/*****************************************************************************
 ****************************************************************************/

void
ddr_kopengl_drom_set_att_maptrnintswi (DtSwitch switchvalue)
{
 DROM.maptrnint.status = switchvalue;
}
