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
	void ddr_sampledev_drom_set_att_mapbmp (count, mapping, raster, attributes)
	void ddr_sampledev_drom_set_att_mapbmpswi (switchvalue)
	void ddr_sampledev_drom_set_att_mapdifclr (count, mapping, raster, attributes)
	void ddr_sampledev_drom_set_att_mapdifclrswi (switchvalue)
	void ddr_sampledev_drom_set_att_mapenv (count, mapping, raster, attributes)
	void ddr_sampledev_drom_set_att_mapenvswi (switchvalue)
	void ddr_sampledev_drom_set_att_maptrnint (count, mapping, raster, attributes)
	void ddr_sampledev_drom_set_att_maptrnintswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_mapbmp (count, mapping, raster, attributes)
DtInt count;
DtObject mapping[];
DtObject raster[];
dot_stdtexatt attributes[];
{
    DtInt i;

    if (count > dde_sampledev_drom_data.mapbmp.max_count) {
	dde_sampledev_drom_data.mapbmp.mapping = (DtObject *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapbmp.mapping,
				   count*sizeof(DtObject));
	dde_sampledev_drom_data.mapbmp.raster = (DtObject *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapbmp.raster,
				   count*sizeof(DtObject));
	dde_sampledev_drom_data.mapbmp.attributes = (dot_stdtexatt *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapbmp.attributes,
				   count*sizeof(dot_stdtexatt));
	if (dde_sampledev_drom_data.mapbmp.mapping    == (DtObject *)0 ||
	    dde_sampledev_drom_data.mapbmp.raster     == (DtObject *)0 ||
	    dde_sampledev_drom_data.mapbmp.attributes == (dot_stdtexatt *)0) 
		return;
	dde_sampledev_drom_data.mapbmp.max_count = count;
    }
    dde_sampledev_drom_data.mapbmp.count = count;

    for (i=0; i<count; i++) {
	dde_sampledev_drom_data.mapbmp.mapping[i]    = mapping[i];
	dde_sampledev_drom_data.mapbmp.raster[i]     = raster[i];
	dde_sampledev_drom_data.mapbmp.attributes[i] = attributes[i];
    }
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_mapbmpswi (switchvalue)
DtSwitch switchvalue;
{
    dde_sampledev_drom_data.mapbmp.status = switchvalue;
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_mapdifclr (count, mapping, raster, attributes)
DtInt count;
DtObject mapping[];
DtObject raster[];
dot_stdtexatt attributes[];
{
    DtInt i;

    if (count > dde_sampledev_drom_data.mapdifclr.max_count) {
	dde_sampledev_drom_data.mapdifclr.mapping = (DtObject *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapdifclr.mapping,
				   count*sizeof(DtObject));
	dde_sampledev_drom_data.mapdifclr.raster = (DtObject *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapdifclr.raster,
				   count*sizeof(DtObject));
	dde_sampledev_drom_data.mapdifclr.attributes = (dot_stdtexatt *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapdifclr.attributes,
				   count*sizeof(dot_stdtexatt));
	if (dde_sampledev_drom_data.mapdifclr.mapping    == (DtObject *)0 ||
	    dde_sampledev_drom_data.mapdifclr.raster     == (DtObject *)0 ||
	    dde_sampledev_drom_data.mapdifclr.attributes == (dot_stdtexatt *)0) 
		return;
	dde_sampledev_drom_data.mapdifclr.max_count = count;
    }
    dde_sampledev_drom_data.mapdifclr.count = count;

    for (i=0; i<count; i++) {
	dde_sampledev_drom_data.mapdifclr.mapping[i]    = mapping[i];
	dde_sampledev_drom_data.mapdifclr.raster[i]     = raster[i];
	dde_sampledev_drom_data.mapdifclr.attributes[i] = attributes[i];
    }
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_mapdifclrswi (switchvalue)
DtSwitch switchvalue;
{
    dde_sampledev_drom_data.mapdifclr.status = switchvalue;
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_mapenv (count, mapping, raster, attributes)
DtInt count;
DtObject mapping[];
DtObject raster[];
dot_stdtexatt attributes[];
{
    DtInt i;

    if (count > dde_sampledev_drom_data.mapenv.max_count) {
	dde_sampledev_drom_data.mapenv.mapping = (DtObject *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapenv.mapping,
				   count*sizeof(DtObject));
	dde_sampledev_drom_data.mapenv.raster = (DtObject *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapenv.raster,
				   count*sizeof(DtObject));
	dde_sampledev_drom_data.mapenv.attributes = (dot_stdtexatt *)
		DDspace_Reallocate(dde_sampledev_drom_data.mapenv.attributes,
				   count*sizeof(dot_stdtexatt));
	if (dde_sampledev_drom_data.mapenv.mapping    == (DtObject *)0 ||
	    dde_sampledev_drom_data.mapenv.raster     == (DtObject *)0 ||
	    dde_sampledev_drom_data.mapenv.attributes == (dot_stdtexatt *)0) 
		return;
	dde_sampledev_drom_data.mapenv.max_count = count;
    }
    dde_sampledev_drom_data.mapenv.count = count;

    for (i=0; i<count; i++) {
	dde_sampledev_drom_data.mapenv.mapping[i]    = mapping[i];
	dde_sampledev_drom_data.mapenv.raster[i]     = raster[i];
	dde_sampledev_drom_data.mapenv.attributes[i] = attributes[i];
    }
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_mapenvswi (switchvalue)
DtSwitch switchvalue;
{
    dde_sampledev_drom_data.mapenv.status = switchvalue;
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_maptrnint (count, mapping, raster, attributes)
DtInt count;
DtObject mapping[];
DtObject raster[];
dot_stdtexatt attributes[];
{
    DtInt i;

    if (count > dde_sampledev_drom_data.maptrnint.max_count) {
	dde_sampledev_drom_data.maptrnint.mapping = (DtObject *)
		DDspace_Reallocate(dde_sampledev_drom_data.maptrnint.mapping,
				   count*sizeof(DtObject));
	dde_sampledev_drom_data.maptrnint.raster = (DtObject *)
		DDspace_Reallocate(dde_sampledev_drom_data.maptrnint.raster,
				   count*sizeof(DtObject));
	dde_sampledev_drom_data.maptrnint.attributes = (dot_stdtexatt *)
		DDspace_Reallocate(dde_sampledev_drom_data.maptrnint.attributes,
				   count*sizeof(dot_stdtexatt));
	if (dde_sampledev_drom_data.maptrnint.mapping    == (DtObject *)0 ||
	    dde_sampledev_drom_data.maptrnint.raster     == (DtObject *)0 ||
	    dde_sampledev_drom_data.maptrnint.attributes == (dot_stdtexatt *)0) 
		return;
	dde_sampledev_drom_data.maptrnint.max_count = count;
    }
    dde_sampledev_drom_data.maptrnint.count = count;

    for (i=0; i<count; i++) {
	dde_sampledev_drom_data.maptrnint.mapping[i]    = mapping[i];
	dde_sampledev_drom_data.maptrnint.raster[i]     = raster[i];
	dde_sampledev_drom_data.maptrnint.attributes[i] = attributes[i];
    }
}
/*
 ======================================================================
 */
void
ddr_sampledev_drom_set_att_maptrnintswi (switchvalue)
DtSwitch switchvalue;
{
    dde_sampledev_drom_data.maptrnint.status = switchvalue;
}
/*
 ======================================================================
 */
