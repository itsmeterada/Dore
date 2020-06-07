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
	int ddr_x11_titang2_drom_print_fixed_parms ()
	int ddr_x11_titang2_drom_print_variable_parms (bufferno)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_x11_titang2_drom_print_fixed_parms()
{
    struct cg_sparms *sparms;
    DtInt bndboxptno;
    DtInt rectangleno;
    DtReal *realptr;
    DtReal *windowvalue;

    sparms = &dde_x11_titang2_drom_cg_sparms;

    printf("sparms data structure at [%lx]:\n",sparms);

    printf("	ctl = [%lx]\n",sparms->cg_ctl);

    printf("	matrix:\n");
    dor_matrix_print(sparms->cg_matrix);
    printf("	scale = (%lf,%lf,%lf)\n",
	   sparms->cg_scale[0],sparms->cg_scale[1],sparms->cg_scale[2]);
    printf("	translate = (%lf,%lf,%lf)\n",
	   sparms->cg_xlate[0],sparms->cg_xlate[1],sparms->cg_xlate[2]);

    printf("	frustum xy clipping window at [%lx]\n",sparms->cg_window);
    if (sparms->cg_window != NULL){
	windowvalue = sparms->cg_window;
	printf("		window = (%lf,%lf) (%lf,%lf)\n",
	       windowvalue[0],windowvalue[1],windowvalue[2],windowvalue[3]);
    }

    printf("	bounding box:\n");
    if (sparms->cg_bdbox != NULL) {
	for (bndboxptno=0;bndboxptno<8;bndboxptno++){
	    printf("		point #%d (%lf,%lf,%lf)\n",bndboxptno,
		   sparms->cg_bdbox[3*bndboxptno+0],
		   sparms->cg_bdbox[3*bndboxptno+1],
		   sparms->cg_bdbox[3*bndboxptno+2]);
	}
    }
    else{
	printf ("               NULL\n");
    }

    printf("	drefl at [%lx]\n",sparms->cg_drefl);
    if (sparms->cg_drefl != NULL){
	printf("		color value = (%lf,%lf,%lf)\n",
	       sparms->cg_drefl[0],sparms->cg_drefl[1],sparms->cg_drefl[2]);
    }

    printf("	arefl at [%lx]\n",sparms->cg_arefl);
    if (sparms->cg_arefl != NULL){
	printf("		color value = (%lf,%lf,%lf)\n",
	       sparms->cg_arefl[0],sparms->cg_arefl[1],sparms->cg_arefl[2]);
    }

    printf("	srefl at [%lx]\n",sparms->cg_srefl);
    if (sparms->cg_srefl != NULL){
	printf("		color value = (%lf,%lf,%lf)\n",
	       sparms->cg_srefl[0],sparms->cg_srefl[1],sparms->cg_srefl[2]);
    }

    printf("	nrects = %d\n",sparms->cg_nrects);
    if (sparms->cg_nrects > 0){
	realptr = sparms->cg_rects;

	for (rectangleno=0;rectangleno<sparms->cg_nrects;rectangleno++){
	    printf("	rectangleno %d = (%lf,%lf) (%lf,%lf)\n",rectangleno,
		   realptr[0],realptr[1],realptr[2],realptr[3]);

	    realptr += 4;
	}
    }

    printf("	nmplanes = %d\n",sparms->cg_nmplanes);

    printf("	nlights = %d\n",sparms->cg_nlights);
    printf("	lnorms at [%lx]\n",sparms->cg_lnorms);
    printf("	hnorms at [%lx]\n",sparms->cg_hnorms);
    printf("	lcolors at [%lx]\n",sparms->cg_lcolors);
    printf("	sptbl at [%lx]\n",sparms->cg_sptbl);
    /*
      for (index=0;index<256;index++)
      printf("sptbl[%d] = %lf\n",index,sparms->cg_sptbl[index]);
      */

    printf("	dprange at [%lx] (%lg, %lg, %lg)\n",sparms->cg_dprange,
	   sparms->cg_dprange[0], sparms->cg_dprange[1], sparms->cg_dprange[2] );
    printf("	dpfracs at [%lx] (%lg, %lg)\n",sparms->cg_dpfracs,
	   sparms->cg_dpfracs[0], sparms->cg_dpfracs[1] );
    printf("	dpcolor at [%lx] (%lg, %lg, %lg)\n",sparms->cg_dpcolor,
	   sparms->cg_dpcolor[0], sparms->cg_dpcolor[1], sparms->cg_dpcolor[2]);

    printf("	psweight at [%lx]\n",sparms->cg_psweight);
    printf("	pslim|psmask at [%lx]\n",sparms->cg_pscol.cg_pslim);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_print_variable_parms(bufferno)
	DtInt bufferno;
{
    struct cg_tlparms *tlparms;

    tlparms = &dde_x11_titang2_drom_cg_tlparms[bufferno];

    printf("tlparms data structure at [%lx]:\n",tlparms);

    printf("	spb -> [%lx]\n",tlparms->cg_spb);
    printf("	nelts = %d\n",tlparms->cg_nelts);

    printf("	verts at [%lx]\n",tlparms->cg_verts);
    printf("	vcolors at [%lx]\n",tlparms->cg_vcolors);
    printf("	norms at [%lx]\n",tlparms->cg_norms);

    printf("	nverts = %d\n",tlparms->cg_nverts);
    printf("	tclist at [%lx]\n",tlparms->cg_tclist);
    printf("	xfverts at [%lx]\n",tlparms->cg_xfverts);
    printf("	xfpersp at [%lx]\n",tlparms->cg_xfpersp);
}
