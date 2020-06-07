#ifndef DD_NURBSURF_H
#define DD_NURBSURF_H
 
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

struct newknot {int n; DtReal *p;};

struct nurbsurf {
    DtInt subdivtype;
    DtReal subdivlvl[5];
    short
      u_closed:1,  /* 1==> closed in 1st parametric variable u */
      u_period:1,  /* 1==>periodic in u */
      v_closed:1,  /* 1==> closed in 2nd parametric variable v */
      v_period:1,  /* 1==>periodic in v */
      rational:1,  /* 1==>rational; 0==>polynomial */
      u_owner:1,   /* 1==> this struct allocated *u */
      v_owner:1,   /* 1==> this struct allocated *v */
      cp_owner:1;  /* 1==> this struct allocated *cptp */
    char u_order;	  /* 1+degree in u */
    char v_order;	  /* 1+degree in v */
    DtShort u_nknot;  /* number of knots in u */
    DtShort v_nknot;  /* number of knots in v */
    DtShort u_nctrl;  /* number of control points along u */
    DtShort v_nctrl;  /* number of control points along v */
	    /* u_nknot = u_nctrl + u_order;  and similarly for v */
    DtReal *u;	  /* 1st knot sequence */
    DtReal *v;	  /* 2nd knot sequence */
    DtNPoint4 **cptp;   /* -> v_nctrl ptrs to row of u_nctrl 4D pts */
    DtObjectStructure *alternate_object;
    DtNArea uv_area;  /* bounds of parametric intervals */
    char cornerflat[4];
    char edgeflat[4];
    DtNPoint3 corner[4];  /* crack prevention */
};

struct nurbcurv {
    DtInt subdivtype;
    DtReal subdivlvl[5];
    char
      closed:1,
      period:1,
      rational:1,  /* 1==>rational; 0==>polynomial */
      u_owner:1,   /* 1==> this struct allocated *u */
      cp_owner:1;  /* 1==> this struct allocated *cpt */
    char order;	  /* 1+degree */
    DtShort nknot;
    DtShort nctrl;
	    /* nknot = nctrl + order */
    DtReal *u;	  /* knot sequence */
    DtNPoint4 *cpt;   /* control points */
    DtInterval interval;  /* bounds of parametric interval */
    DtObjectStructure *alternate_object;
};

    /***  Function Prototypes  ***/

void        dor_nurbsurf_initialize  (void);
dot_object *dor_nurbsurf_create
		(DtColorModel, DtCtrlPointType, DtNArea, DtInt, DtInt,
		DtReal [], DtInt, DtInt, DtReal [], DtInt, DtInt, DtReal []);
void        dor_nurbsurf_destroy  (dot_object *);
void        dor_nurbsurf_print  (struct object *);
dot_object *dor_nurbsurf_update_alternate  (dot_object *);
void        dor_nurbsurf_pick  (dot_object *);
void        dor_nurbsurf_render_standard  (dot_object *);
void        dor_nurbsurf_subdivide_adaptive 
		(struct nurbsurf *, DtShort, DtPFV, DtPtr);
dot_object *dor_nurbsurf_create_alternate_geometry  (struct nurbsurf *);
void        dor_nurbsurf_compute_bounding_volume  (dot_object *);

#endif
