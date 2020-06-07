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
	int ddr_x11_DGL_drom_render_point_list (colormodel,bndboxpts,
		pointcount, vtxlocs,vtxnrms,vtxclrs)
	int ddr_x11_DGL_drom_render_line_list (colormodel,bndboxpts,
		linecount,elenrms, vtxlocs,vtxnrms,vtxclrs)
	int ddr_x11_DGL_drom_render_connected_line_list (colormodel,bndboxpts,
		vertexcount, elenrms,vtxlocs, vtxnrms,vtxclrs)
	int ddr_x11_DGL_drom_render_triangle_list (colormodel,bndboxpts,
		trianglecount,elenrms, vtxlocs,vtxnrms,vtxclrs)
	int ddr_x11_DGL_drom_render_triangle_mesh (colormodel,bndboxpts,
		trianglecount,elenrms, vertexcount,vtxlocs,vtxnrms,vtxclrs,
		vertexlist, compiledvertexlist)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_x11_DGL_drom_render_point_list(colormodel,bndboxpts,pointcount,
				   vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt pointcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
   XdElementRec data;

   if (sizeof(DtReal) == sizeof(double))
	 data.format = XdDataDouble;
   else
	 data.format = XdDataFloat;

   data.elecount = pointcount;
   data.vtxlocs = (XdPoint *)vtxlocs;

   data.bndbox  = (XdPoint *)bndboxpts;
   data.vtxnrms = (XdNormal *)vtxnrms;
   data.elenrms = (XdNormal *)0;
   data.vtxclrs = (XdColor *)vtxclrs;
   data.objnrm  = (XdNormal *)0;
   data.edges   = (int *)0;

   if (dde_x11_DGL_drom_data.transparent_surface_pass &&
      dde_x11_DGL_drom_data.object_transparent_switch) {

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_transparent_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_transparent_ambient_intensity);

      XdPointList3(dde_x11_DGL_drom_curdevdat->pDraw,
		dde_x11_DGL_drom_curdevdat->pGC, &data);

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_ambient_intensity);
    } else {

      XdPointList3(dde_x11_DGL_drom_curdevdat->pDraw,
		dde_x11_DGL_drom_curdevdat->pGC, &data);
    }
}

/*
 ======================================================================
 */

ddr_x11_DGL_drom_render_line_list(colormodel,bndboxpts,linecount,elenrms,
				  vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt linecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
   XdElementRec data;

   if (sizeof(DtReal) == sizeof(double))
	 data.format = XdDataDouble;
   else
	 data.format = XdDataFloat;

   data.elecount = linecount;
   data.vtxlocs = (XdPoint *)vtxlocs;

   data.bndbox  = (XdPoint *)bndboxpts;
   data.vtxnrms = (XdNormal *)vtxnrms;
   data.elenrms = (XdNormal *)elenrms;
   data.vtxclrs = (XdColor *)vtxclrs;
   data.objnrm  = (XdNormal *)0;
   data.edges   = (int *)0;

   if (dde_x11_DGL_drom_data.transparent_surface_pass &&
      dde_x11_DGL_drom_data.object_transparent_switch) {

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_transparent_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_transparent_ambient_intensity);

      XdLineList3(dde_x11_DGL_drom_curdevdat->pDraw,
	       dde_x11_DGL_drom_curdevdat->pGC, &data);

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_ambient_intensity);
    } else {

      XdLineList3(dde_x11_DGL_drom_curdevdat->pDraw,
	       dde_x11_DGL_drom_curdevdat->pGC, &data);
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_render_connected_line_list(colormodel,bndboxpts,vertexcount,
					    elenrms,vtxlocs,
					    vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt vertexcount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
   XdElementRec data;

   if (sizeof(DtReal) == sizeof(double))
	 data.format = XdDataDouble;
   else
	 data.format = XdDataFloat;

   data.elecount = vertexcount;
   data.vtxlocs = (XdPoint *)vtxlocs;

   data.bndbox  = (XdPoint *)bndboxpts;
   data.vtxnrms = (XdNormal *)vtxnrms;
   data.elenrms = (XdNormal *)elenrms;
   data.vtxclrs = (XdColor *)vtxclrs;
   data.objnrm  = (XdNormal *)0;
   data.edges   = (int *)0;

   if (dde_x11_DGL_drom_data.transparent_surface_pass &&
      dde_x11_DGL_drom_data.object_transparent_switch) {

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_transparent_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_transparent_ambient_intensity);

      XdPolyline3(dde_x11_DGL_drom_curdevdat->pDraw,
	       dde_x11_DGL_drom_curdevdat->pGC, &data);

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_ambient_intensity);
    } else {

      XdPolyline3(dde_x11_DGL_drom_curdevdat->pDraw,
	       dde_x11_DGL_drom_curdevdat->pGC, &data);
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_render_triangle_list(colormodel,bndboxpts,trianglecount,elenrms,
				      vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
   XdElementRec data;

   if (sizeof(DtReal) == sizeof(double))
	 data.format = XdDataDouble;
   else
	 data.format = XdDataFloat;

   data.elecount = trianglecount;
   data.vtxlocs = (XdPoint *)vtxlocs;

   data.bndbox  = (XdPoint *)bndboxpts;
   data.vtxnrms = (XdNormal *)vtxnrms;
   data.elenrms = (XdNormal *)elenrms;
   data.vtxclrs = (XdColor *)vtxclrs;
   data.objnrm  = (XdNormal *)0;
   data.edges   = (int *)0;

   if (dde_x11_DGL_drom_data.transparent_surface_pass &&
      dde_x11_DGL_drom_data.object_transparent_switch) {

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_transparent_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_transparent_ambient_intensity);

      XdTriangleList3(dde_x11_DGL_drom_curdevdat->pDraw,
		dde_x11_DGL_drom_curdevdat->pGC, &data);

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC, 
		dde_x11_DGL_drom_data.object_ambient_intensity);
    } else {

      XdTriangleList3(dde_x11_DGL_drom_curdevdat->pDraw,
		dde_x11_DGL_drom_curdevdat->pGC, &data);
    }
}
/*
 ======================================================================
 */

ddr_x11_DGL_drom_render_triangle_mesh(colormodel,bndboxpts,trianglecount,elenrms,
	     vertexcount,vtxlocs,vtxnrms, vtxclrs,vertexlist, compiledvertexlist)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtInt vertexcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
     DtInt vertexlist[][3];
     DtInt compiledvertexlist[][3];
{
   XdElementMeshRec data;

   if (sizeof(DtReal) == sizeof(double))
	 data.format = XdDataDouble;
   else
	 data.format = XdDataFloat;

   data.elecount = trianglecount;
   data.vtxcount = vertexcount;
   data.vtxlocs = (XdPoint *)vtxlocs;
   data.cmpvtxlist = (int *)compiledvertexlist;
   
   data.bndbox  = (XdPoint *)bndboxpts;
   data.vtxnrms = (XdNormal *)vtxnrms;
   data.elenrms = (XdNormal *)elenrms;
   data.vtxclrs = (XdColor *)vtxclrs;
   data.objnrm  = (XdNormal *)0;
   data.eleclrs = (XdColor *)0;
   data.edges   = (int *)0;

   if (dde_x11_DGL_drom_data.transparent_surface_pass &&
      dde_x11_DGL_drom_data.object_transparent_switch) {

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC,
                dde_x11_DGL_drom_data.object_transparent_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC,
                dde_x11_DGL_drom_data.object_transparent_ambient_intensity);

      XdTriangleMesh3(dde_x11_DGL_drom_curdevdat->pDraw,
		   dde_x11_DGL_drom_curdevdat->pGC, &data);

      XdSetDiffuseIntens(dde_x11_DGL_drom_curdevdat->pGC,
                dde_x11_DGL_drom_data.object_diffuse_intensity);
      XdSetAmbientIntens(dde_x11_DGL_drom_curdevdat->pGC,
                dde_x11_DGL_drom_data.object_ambient_intensity);
   } else {

      XdTriangleMesh3(dde_x11_DGL_drom_curdevdat->pDraw,
		   dde_x11_DGL_drom_curdevdat->pGC, &data);
   }
}
