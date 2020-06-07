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
	void drr_dynrnd_mapbmp ()
	void drr_dynrnd_mapbmpswi ()
	void drr_dynrnd_mapdifclr ()
	void drr_dynrnd_mapdifclrswi ()
	void drr_dynrnd_mapenv ()
	void drr_dynrnd_mapenvswi ()
	void drr_dynrnd_maptrnint ()
	void drr_dynrnd_maptrnintswi ()

  ======================================================================
 */
#include "dynrnd.h"
/*
 ======================================================================
 */
void drr_dynrnd_mapbmp (void)
{
    DtInt count;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTextureMapBump"),
				  DcMethodInqGlbAttVal);
    }

    (*query)(dre_dynrnd_data.bmp_max_textures,
	     &count, 
	     dre_dynrnd_data.bmp_mapping,
	     dre_dynrnd_data.bmp_raster,sizeof(dot_stdtexatt),
	     dre_dynrnd_data.bmp_attributes, 
	     dre_dynrnd_data.bmp_size);
   
    if (count > dre_dynrnd_data.bmp_max_textures) {
	dre_dynrnd_data.bmp_mapping = (DtObject *)
		DDspace_Reallocate (dre_dynrnd_data.bmp_mapping,
				    count*sizeof(DtObject));
	dre_dynrnd_data.bmp_raster = (DtObject *)
		DDspace_Reallocate (dre_dynrnd_data.bmp_raster,
				    count*sizeof(DtObject));
	dre_dynrnd_data.bmp_attributes = (dot_stdtexatt *)
		DDspace_Reallocate (dre_dynrnd_data.bmp_attributes,
				    count*sizeof(dot_stdtexatt));
	dre_dynrnd_data.bmp_size = (DtInt *)
		DDspace_Reallocate (dre_dynrnd_data.bmp_size,
				    count*sizeof(DtInt));
	dre_dynrnd_data.bmp_max_textures = count;
	(*query)(dre_dynrnd_data.bmp_max_textures,
		 &count, 
		 dre_dynrnd_data.bmp_mapping,
		 dre_dynrnd_data.bmp_raster,sizeof(dot_stdtexatt),
		 dre_dynrnd_data.bmp_attributes, 
		 dre_dynrnd_data.bmp_size);
    }

    (*dre_dynrnd_DROM_fcns->set_att_mapbmp) (count, 
					     dre_dynrnd_data.bmp_mapping, 
					     dre_dynrnd_data.bmp_raster, 
					     dre_dynrnd_data.bmp_attributes);
}
/*
 ======================================================================
 */
void drr_dynrnd_mapbmpswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTextureMapBumpSwitch"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_mapbmpswi)(switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_mapdifclr (void)
{
    DtInt count;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTextureMapDiffuseColor"),
				  DcMethodInqGlbAttVal);
    }

    (*query)(dre_dynrnd_data.difclr_max_textures,
	     &count, 
	     dre_dynrnd_data.difclr_mapping,
	     dre_dynrnd_data.difclr_raster,sizeof(dot_stdtexatt),
	     dre_dynrnd_data.difclr_attributes, 
	     dre_dynrnd_data.difclr_size);
   
    if (count > dre_dynrnd_data.difclr_max_textures) {
	dre_dynrnd_data.difclr_mapping = (DtObject *)
		DDspace_Reallocate (dre_dynrnd_data.difclr_mapping,
				    count*sizeof(DtObject));
	dre_dynrnd_data.difclr_raster = (DtObject *)
		DDspace_Reallocate (dre_dynrnd_data.difclr_raster,
				    count*sizeof(DtObject));
	dre_dynrnd_data.difclr_attributes = (dot_stdtexatt *)
		DDspace_Reallocate (dre_dynrnd_data.difclr_attributes,
				    count*sizeof(dot_stdtexatt));
	dre_dynrnd_data.difclr_size = (DtInt *)
		DDspace_Reallocate (dre_dynrnd_data.difclr_size,
				    count*sizeof(DtInt));
	dre_dynrnd_data.difclr_max_textures = count;
	(*query)(dre_dynrnd_data.difclr_max_textures,
		 &count, 
		 dre_dynrnd_data.difclr_mapping,
		 dre_dynrnd_data.difclr_raster,sizeof(dot_stdtexatt),
		 dre_dynrnd_data.difclr_attributes, 
		 dre_dynrnd_data.difclr_size);
    }

    (*dre_dynrnd_DROM_fcns->set_att_mapdifclr) (
				count, 
				dre_dynrnd_data.difclr_mapping, 
				dre_dynrnd_data.difclr_raster, 
				dre_dynrnd_data.difclr_attributes);
}
/*
 ======================================================================
 */
void drr_dynrnd_mapdifclrswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTextureMapDiffuseColorSwitch"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_mapdifclrswi)(switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_mapenv (void)
{
    DtInt count;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTextureMapEnviron"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(dre_dynrnd_data.env_max_textures,
	     &count, 
	     dre_dynrnd_data.env_mapping,
	     dre_dynrnd_data.env_raster,sizeof(dot_stdtexatt),
	     dre_dynrnd_data.env_attributes, 
	     dre_dynrnd_data.env_size);
   
    if (count > dre_dynrnd_data.env_max_textures) {
	dre_dynrnd_data.env_mapping = (DtObject *)
		DDspace_Reallocate (dre_dynrnd_data.env_mapping,
				    count*sizeof(DtObject));
	dre_dynrnd_data.env_raster = (DtObject *)
		DDspace_Reallocate (dre_dynrnd_data.env_raster,
				    count*sizeof(DtObject));
	dre_dynrnd_data.env_attributes = (dot_stdtexatt *)
		DDspace_Reallocate (dre_dynrnd_data.env_attributes,
				    count*sizeof(dot_stdtexatt));
	dre_dynrnd_data.env_size = (DtInt *)
		DDspace_Reallocate (dre_dynrnd_data.env_size,
				    count*sizeof(DtInt));
	dre_dynrnd_data.env_max_textures = count;
	(*query)(dre_dynrnd_data.env_max_textures,
		 &count, 
		 dre_dynrnd_data.env_mapping,
		 dre_dynrnd_data.env_raster,sizeof(dot_stdtexatt),
		 dre_dynrnd_data.env_attributes, 
		 dre_dynrnd_data.env_size);
    }

    (*dre_dynrnd_DROM_fcns->set_att_mapenv) (
				count, 
				dre_dynrnd_data.env_mapping, 
				dre_dynrnd_data.env_raster, 
				dre_dynrnd_data.env_attributes);
}
/*
 ======================================================================
 */
void drr_dynrnd_mapenvswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(
			DsInqClassId("AttTextureMapEnvironSwitch"),
			DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_mapenvswi)(switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_maptrnint (void)
{
    DtInt count;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTextureMapTranspIntens"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(dre_dynrnd_data.trnint_max_textures,
	     &count, 
	     dre_dynrnd_data.trnint_mapping,
	     dre_dynrnd_data.trnint_raster,sizeof(dot_stdtexatt),
	     dre_dynrnd_data.trnint_attributes, 
	     dre_dynrnd_data.trnint_size);
   
    if (count > dre_dynrnd_data.trnint_max_textures) {
	dre_dynrnd_data.trnint_mapping = (DtObject *)
		DDspace_Reallocate (dre_dynrnd_data.trnint_mapping,
				    count*sizeof(DtObject));
	dre_dynrnd_data.trnint_raster = (DtObject *)
		DDspace_Reallocate (dre_dynrnd_data.trnint_raster,
				    count*sizeof(DtObject));
	dre_dynrnd_data.trnint_attributes = (dot_stdtexatt *)
		DDspace_Reallocate (dre_dynrnd_data.trnint_attributes,
				    count*sizeof(dot_stdtexatt));
	dre_dynrnd_data.trnint_size = (DtInt *)
		DDspace_Reallocate (dre_dynrnd_data.trnint_size,
				    count*sizeof(DtInt));
	dre_dynrnd_data.trnint_max_textures = count;
	(*query)(dre_dynrnd_data.trnint_max_textures,
		 &count, 
		 dre_dynrnd_data.trnint_mapping,
		 dre_dynrnd_data.trnint_raster,sizeof(dot_stdtexatt),
		 dre_dynrnd_data.trnint_attributes, 
		 dre_dynrnd_data.trnint_size);
    }

    (*dre_dynrnd_DROM_fcns->set_att_maptrnint) (
				count, 
				dre_dynrnd_data.trnint_mapping, 
				dre_dynrnd_data.trnint_raster, 
				dre_dynrnd_data.trnint_attributes);
}
/*
 ======================================================================
 */
void drr_dynrnd_maptrnintswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTextureMapTranspIntensSwitch"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_maptrnintswi)(switchvalue);
}
/*
 ======================================================================
 */
