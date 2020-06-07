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
	dor_nurbsurf_initialize
	dor_nurbsurf_create
	dor_nurbsurf_destroy
	dor_nurbsurf_print
	dor_nurbsurf_update_alternate
	dor_nurbsurf_pick
	dor_nurbsurf_render_standard
	dor_nurbsurf_subdivide_control_along_u
	dor_nurbsurf_subdivide_control_along_v
	dor_nurb_subdivide_knot
	dor_nurb_subdivide_knot_bezier
	dor_nurb_subknot
	dor_nurbsurf_subdivide_along_u
	dor_nurbsurf_dumpnurb
	dor_nurbsurf_dumpvec
	dor_nurbsurf_subdivide_along_v
	dor_nurbsurf_subdivide_along_u_and_v
	dor_nurbsurf_Point4_same
	dor_nurbsurf_test_linear
	dor_nurbsurf_test_u_boundaries
	dor_nurbsurf_test_v_boundaries
	dor_nurbsurf_create_alternate_geometry
	dor_nurbsurf_create_alternate_geometry_triangle
	dor_nurbsurf_check_subdiv
	dor_nurbsurf_check_triangle
	dor_nurbsurf_compute_bounding_volume
	dor_nurb_4to3

  ======================================================================
 */
#include <math.h>
#include <memory.h>
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/nurbsurf.h>
#include <dore/dore_develop/private/trilst.h>

#ifdef NOTDEF
#define dor_space_allocate(x) jfr_space_allocate(x,0,__FILE__,__LINE__)
#define dor_space_allocate2(x,y) jfr_space_allocate(x,0,__FILE__,__LINE__)
#define dor_space_deallocate(x) jfr_space_deallocate(x,0,__FILE__,__LINE__)
#define dor_space_deallocate2(x,y) jfr_space_deallocate(x,0,__FILE__,__LINE__)
#endif NOTDEF

#define dor_space_allocate2(x,y) dor_space_allocate(x)
#define dor_space_deallocate2(x,y) dor_space_deallocate(x)

#define dod_nurbsurf_type_undefined             0
#define dod_nurbsurf_type_nurbsurf              1

#define dod_nurbsurf_tessellatetype_nonmesh     0
#define dod_nurbsurf_tessellatetype_mesh        1

/*
 ======================================================================
 */

DtInt doe_nurbsurf_class_id;

    /***  Method Routines  ***/

static DtMethodEntry nurbsurf_methods[] = {
   {DcMethodCmpBndVolume,    (DtMethodPtr)dor_nurbsurf_compute_bounding_volume},
   {DcMethodDestroy,         (DtMethodPtr)dor_nurbsurf_destroy},
   {DcMethodPick,            (DtMethodPtr)dor_nurbsurf_pick},
   {DcMethodPrint,           (DtMethodPtr)dor_nurbsurf_print},
   {DcMethodUpdStdAltObj,    (DtMethodPtr)dor_nurbsurf_update_alternate},
   {DcMethodStdRenderDisplay,(DtMethodPtr)dor_nurbsurf_render_standard}
};


    /***  Function Prototypes  ***/

void        dor_nurbsurf_initialize (void);
dot_object *dor_nurbsurf_create
		(DtColorModel, DtCtrlPointType, DtNArea, DtInt, DtInt,
		 DtReal[], DtInt, DtInt, DtReal[], DtInt, DtInt, DtReal[]);
void        dor_nurbsurf_destroy (dot_object*);
void        dor_nurbsurf_print (struct object*);
dot_object *dor_nurbsurf_update_alternate (dot_object*);
void        dor_nurbsurf_pick (dot_object*);
void        dor_nurbsurf_render_standard (dot_object*);
void        dor_nurbsurf_subdivide_adaptive
		(struct nurbsurf*, DtShort, DtPFV, DtPtr);
DtNPoint4 **dor_nurbsurf_subdivide_control_along_u
		(struct nurbsurf *, struct nurbsurf *, DtNPoint4 **);
DtNPoint4 **dor_nurbsurf_subdivide_control_along_v
		(struct nurbsurf *, struct nurbsurf *, DtNPoint4 **);
void        dor_nurbsurf_subdivide_fixed
		(struct nurbsurf *, DtInt, DtShort, DtPFV, DtPtr);
DtReal     *dor_nurb_subdivide_knot (DtReal*, int, int, int, int);
struct newknot dor_nurb_subdivide_knot_bezier (DtReal*, int, int);
DtReal     *dor_nurb_subknot (DtReal*, DtReal*, int, int, int);
void        dor_nurbsurf_subdivide_along_u
		(struct nurbsurf *, struct nurbsurf *, struct nurbsurf *);
void        dor_nurbsurf_dumpnurb (char*, struct nurbsurf *);
void        dor_nurbsurf_dumpvec (char*, DtReal*, int);
void        dor_nurbsurf_subdivide_along_v
		(struct nurbsurf *, struct nurbsurf *, struct nurbsurf *);
void        dor_nurbsurf_subdivide_along_u_and_v
		(struct nurbsurf *, struct nurbsurf *, struct nurbsurf *,
		 struct nurbsurf *, struct nurbsurf *);
void        dor_nurbsurf_tessellate (struct nurbsurf*, DtShort, DtPFV, DtPtr);
void        dor_nurbsurf_tessellate_nurbsurf
		(struct nurbsurf *, DtShort, DtPFV, char*);
static DtFlag dor_nurbsurf_Point4_same (DtNPoint4 *, DtNPoint4 *);
DtFlag      dor_nurbsurf_test_linear (int, DtNPoint4*, DtInt, DtReal*);
DtFlag      dor_nurbsurf_test_u_boundaries (struct nurbsurf *);
DtFlag      dor_nurbsurf_test_v_boundaries (struct nurbsurf *);
dot_object *dor_nurbsurf_create_alternate_geometry (struct nurbsurf *);
void        dor_nurbsurf_create_alternate_geometry_triangle
		(dot_object*, DtNPoint3*, DtNPoint3*, DtNPoint3*, DtNPoint3*,
	         DtNPoint3*, DtNPoint3*);
DtFlag      dor_nurbsurf_check_subdiv (struct nurbsurf *);
DtFlag      dor_nurbsurf_check_triangle (DtNPoint3*, DtNPoint3*, DtNPoint3*);
void        dor_nurbsurf_compute_bounding_volume (dot_object*);
DtNPoint3   dor_nurb_4to3 (DtNPoint4 *);
DtNPoint4   dor_nurb_svr4 (DtReal, DtReal*, DtNPoint4*, int);
DtNPoint4   dor_nurb_dvr4 (DtReal, DtReal*, DtNPoint4*, int, int);
DtNPoint4   dor_nurb_cvr4 (DtReal*, DtReal*, DtNPoint4*, int);



/* ======================================================================== */

void dor_nurbsurf_initialize (void)
{
    doe_nurbsurf_class_id = dor_class_install (DcTypeNURBSurface, 
				"DoNURBSurf", 6, nurbsurf_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_nurbsurf_create (
    DtColorModel colormodel,
    DtCtrlPointType ctrlpointtype,
    DtNArea uv_area,
    DtInt order_u,
    DtInt n_knot_u,
    DtReal knot_u[],
    DtInt order_v,
    DtInt n_knot_v,
    DtReal knot_v[],
    DtInt n_ctrl_u,
    DtInt n_ctrl_v,
    DtReal ctrl_vertices[])
{
    static DtFlag initialized = DcFalse;
    register int k;
    struct nurbsurf *new;

    if (!initialized) {
	dor_nurbsurf_initialize();
	initialized = DcTrue;
    }

    new=(struct nurbsurf *)dor_space_allocate(sizeof(*new));
    if (n_knot_u!=(order_u+n_ctrl_u)) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoNURBSurface", 
		    "n_knot_u !=(order_u+n_ctrl_u)");
	return((DtObject)dor_null_create(DcNullPtr));
    }
    if (n_knot_v!=(order_v+n_ctrl_v)) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoNURBSurface", 
		    "n_knot_v !=(order_v+n_ctrl_v)");
	return((DtObject)dor_null_create(DcNullPtr));
    }
    dor_global_subdivspc_get_value(&new->subdivtype, 
				   &new->subdivlvl[0]);
    for (k=4; --k>=0; ) {
	new->edgeflat[k]=0;
	new->cornerflat[k]=0;
	new->corner[k].x=new->corner[k].y=new->corner[k].z=0.0;
    }
    new->u_closed=0;
    new->u_period=0;
    new->v_closed=0;
    new->v_period=0;
    new->rational=1;
    new->u_owner=1;
    new->v_owner=1;
    new->cp_owner=1;
    new->u_order=order_u;
    new->v_order=order_v;
    new->u_nknot=n_knot_u;
    new->v_nknot=n_knot_v;
    new->u_nctrl=n_ctrl_u;
    new->v_nctrl=n_ctrl_v;

    new->u=(DtReal *)dor_space_allocate(k=n_knot_u*sizeof(*new->u));
    bcopy(knot_u,new->u,k);

    new->v=(DtReal *)dor_space_allocate(k=n_knot_v*sizeof(*new->v));
    bcopy(knot_v,new->v,k);{
	register DtNPoint4 **pp;
	register DtNPoint4 *p;

	pp=(DtNPoint4 **)dor_space_allocate(n_ctrl_v*sizeof(*pp));
	new->cptp=pp;
	p=(DtNPoint4 *)dor_space_allocate(k=n_ctrl_u*n_ctrl_v*sizeof(*p));
	bcopy(ctrl_vertices,p,k);
	for (k=n_ctrl_v; --k>=0; p+=n_ctrl_u)
		*pp++=p;
    }
    new->uv_area=uv_area;
    new->alternate_object = NULL;
    return(dor_object_create(doe_nurbsurf_class_id, new));
}
/*
 ======================================================================
 */

void dor_nurbsurf_destroy (dot_object *object)
{
    struct nurbsurf *nurbsurf;

    nurbsurf = (struct nurbsurf *)(object->data);

    if (nurbsurf->alternate_object != NULL) {
	dor_object_check_deletion(nurbsurf->alternate_object);
    }
    if (nurbsurf->cp_owner) {
	nurbsurf->cp_owner=0;
	dor_space_deallocate(nurbsurf->cptp);
    }
    if (nurbsurf->v_owner) {
	nurbsurf->v_owner=0;
	dor_space_deallocate(nurbsurf->v);
    }
    if (nurbsurf->u_owner) {
	nurbsurf->u_owner=0;
	dor_space_deallocate(nurbsurf->u);
    }

    dor_space_deallocate(nurbsurf);
}
/*
 ======================================================================
 */

void dor_nurbsurf_print (struct object *object)
{
    struct nurbsurf *nurbsurf;

    nurbsurf = (struct nurbsurf *)(object->data);

    if (nurbsurf == NULL){
	dor_print_output("nurb surface is NULL");
	return;
    }

    printf(dor_print_get_line(),"u_nknot = %d, v_nknot = %d",
	   nurbsurf->u_nknot, nurbsurf->v_nknot);
    dor_print_output(dor_print_get_line());
	
    printf(dor_print_get_line(),"u_nctrl = %d, v_nctrl = %d",
	   nurbsurf->u_nctrl, nurbsurf->v_nctrl);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

dot_object *dor_nurbsurf_update_alternate (dot_object *object)
{
    struct nurbsurf *nurbsurf;
    DtFlag changed;
    dot_object *alt;

    nurbsurf = (struct nurbsurf *)(object->data);

    changed = dor_nurbsurf_check_subdiv(nurbsurf);

    if ((nurbsurf->alternate_object != DcNullPtr) && !changed) {
	alt = nurbsurf->alternate_object;
    } else {
	if (nurbsurf->alternate_object != DcNullPtr) {
	    dor_object_get_method(nurbsurf->alternate_object,
				  DcMethodDestroy)(nurbsurf->alternate_object);

	    nurbsurf->alternate_object = DcNullPtr;
	}

	nurbsurf->alternate_object = 
		dor_nurbsurf_create_alternate_geometry(nurbsurf);
	alt = nurbsurf->alternate_object;

	if (nurbsurf->alternate_object == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_nurbsurf_update_alternate", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

void dor_nurbsurf_pick (dot_object *object)
{
    struct nurbsurf *nurbsurf;
    DtFlag changed;

    if (!dor_condex_query_pick(doe_nurbsurf_class_id))
	    return;

    nurbsurf = (struct nurbsurf *)(object->data);

    changed = dor_nurbsurf_check_subdiv(nurbsurf);

    if ((nurbsurf->alternate_object != DcNullPtr) && !changed) {
	dor_object_get_method(nurbsurf->alternate_object,
			      DcMethodPick)(nurbsurf->alternate_object);
	return;
    } else {
	if (nurbsurf->alternate_object != NULL) {
	    dor_object_get_method(nurbsurf->alternate_object,
				  DcMethodDestroy)(nurbsurf->alternate_object);

	    nurbsurf->alternate_object = NULL;
	}

	nurbsurf->alternate_object = 
		dor_nurbsurf_create_alternate_geometry(nurbsurf);

	if (nurbsurf->alternate_object == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_nurbsurf_pick", "");
	    return;
	}

	dor_object_get_method(nurbsurf->alternate_object,
			      DcMethodPick)(nurbsurf->alternate_object);

	return;
    }	
}
/*
 ======================================================================
 */

void dor_nurbsurf_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_nurbsurf_class_id))
	    return;

    alt = dor_nurbsurf_update_alternate(object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
/*
 ======================================================================
 */

void dor_nurbsurf_subdivide_adaptive (
    struct nurbsurf *nurbsurf,
    DtShort tessellatetype,
    DtPFV action,
    DtPtr actionparm)
{
    void (*f)()=0;

    if (!dor_nurbsurf_test_u_boundaries(nurbsurf))
	f=dor_nurbsurf_subdivide_along_u;
    else if (!dor_nurbsurf_test_v_boundaries(nurbsurf))
	f=dor_nurbsurf_subdivide_along_v;

    if (f) {
	struct nurbsurf sub[2];
	register int k; register struct nurbsurf *np;
	(*f)(nurbsurf,&sub[0],&sub[1]);
	for (np= &sub[0], k=2; --k>=0; ++np) {
	    dor_nurbsurf_subdivide_adaptive(np,
		tessellatetype,action,actionparm);
	}
	for (np= &sub[0], k=2; --k>=0; ++np) {
	    if (np->u_owner)  dor_space_deallocate2(np->u,np);
	    if (np->v_owner)  dor_space_deallocate2(np->v,np);
	    if (np->cp_owner) dor_space_deallocate2(np->cptp,np);
	}
    } else {
	dor_nurbsurf_tessellate_nurbsurf
	    (nurbsurf,tessellatetype,action,actionparm);
    }
}
/*
 ======================================================================
 */

DtNPoint4 **dor_nurbsurf_subdivide_control_along_u (
    struct nurbsurf *new,
    struct nurbsurf *orig,
    register DtNPoint4 **ocpp)
{
    register int k=new->u_nctrl,j=new->v_nctrl,j2=(1+j)&~1;
    register DtNPoint4 **ncpp=(DtNPoint4 **)dor_space_allocate(
						 j2*sizeof(**new->cptp));
    register DtNPoint4 *ncp=(DtNPoint4 *)dor_space_allocate(
						 j*k*sizeof(*new->cptp));
    DtNPoint4 **value=ncpp;

    new->cp_owner=1;
    for (j=new->v_nctrl; --j>=0; ) { /* each row */
	register DtReal *up=orig->u,*wp=new->u;
	register DtNPoint4 *cp= *ocpp++;
	*ncpp++=ncp;
	for (k=new->u_nctrl; --k>=0; ++wp) {
	    while (*wp>=up[1]) {++up; ++cp;}
	    *ncp++=dor_nurb_cvr4(wp,up,cp,new->u_order);
	}
    }
    return(value);
}
/*
 ======================================================================
 */

DtNPoint4 **dor_nurbsurf_subdivide_control_along_v (
    struct nurbsurf *new,
    struct nurbsurf *orig,
    DtNPoint4 **ocpp)
{
    register int k=new->u_nctrl,j=new->v_nctrl,j2=(1+j)&~1;
    register DtNPoint4 **ncpp=(DtNPoint4 **)dor_space_allocate(
				j2*sizeof(*new->cptp)+j*k*sizeof(**new->cptp));
    register DtNPoint4 *ncp=(DtNPoint4 *)(ncpp+j2);
    DtNPoint4 **value=ncpp;

    new->cp_owner=1;
    while (--j>=0) { *ncpp++=ncp; ncp+=new->u_nctrl; }
    for (j=0; j<new->u_nctrl; ++j) { /* each col */
	register DtNPoint4 **cpp;
	register DtReal *vp=orig->v,*wp=new->v;
	register DtNPoint4 *cp; DtNPoint4 temp[100];
	for (cp=temp, cpp=ocpp, k=orig->v_nctrl; --k>=0; )
		*cp++= (*cpp++)[j];
	for (cp=temp, ncpp=value, k=new->v_nctrl; --k>=0; ++wp) {
	    while (*wp>=vp[1]) {++vp; ++cp;}
	    (*ncpp++)[j]=dor_nurb_cvr4(wp,vp,cp,new->v_order);
	}
    }
    return(value);
}
/*
 ======================================================================
 */

void dor_nurbsurf_subdivide_fixed (
    struct nurbsurf *nurbsurf,
    DtInt divisionsremaining,
    DtShort tessellatetype,
    DtPFV action,
    DtPtr actionparm)
{
	register int k,savk; register struct nurbsurf *np;
	struct nurbsurf sub[4];
	if (--divisionsremaining <= 0) {
	    dor_nurbsurf_tessellate_nurbsurf
		(nurbsurf, tessellatetype, action, actionparm);
	    return;
	}
	/* if there are lots of knots, then the nurb is "long"
	** and should just be cut into two pieces at the same level,
	** without introducing new knots.
	*/
	if ((nurbsurf->u_order<<2)<nurbsurf->u_nknot) {
		/* lots of u knots */
		++divisionsremaining; k=2;
		dor_nurbsurf_subdivide_along_u(nurbsurf,&sub[0],&sub[1]);
	} else if ((nurbsurf->v_order<<2)<nurbsurf->v_nknot) {
		/* lots of v knots */
		++divisionsremaining; k=2;
		dor_nurbsurf_subdivide_along_v(nurbsurf,&sub[0],&sub[1]);
	} else {
		k=4;
		dor_nurbsurf_subdivide_along_u_and_v(nurbsurf,
			&sub[0],&sub[1],&sub[2],&sub[3]);
	}
	savk=k;
	for (np= &sub[0], k=savk; --k>=0; ++np) {
		dor_nurbsurf_subdivide_fixed(np,divisionsremaining,
			tessellatetype,action,actionparm);
	}
	for (np= &sub[0], k=savk; --k>=0; ++np) {
		if (np->u_owner)  dor_space_deallocate2(np->u,np);
		if (np->v_owner)  dor_space_deallocate2(np->v,np);
		if (np->cp_owner) dor_space_deallocate2(np->cptp,np);
	}
}
/*
 ======================================================================
 */

/* add nnew more knots around the median of the knots;
** allocate the new knot vector, and return a pointer to it.
** do not increase the multiplicity of an existing knot value.
** if old knots near the median have multiplicity greater than
** one, then some of the new knots may also have multiplicity
** greater than one; but try for new singleton knots.
*/

DtReal *dor_nurb_subdivide_knot (
    DtReal *vec,
    int nvec,
    int order,
    int nnew,
    int np)
{
    register int k;
    /* space for new knot vector */
    DtReal *vec2=(DtReal *)dor_space_allocate2((nvec+nnew)*sizeof(*vec2),np);
    register DtReal t,*vp2,*vp3;
    /* new knots to left of median [in reverse order]. */
    /* high end of vec2 holds reversed left-of-median knots */
    for (vp2=vec, vp3=vec2+(nvec+nnew), k=nvec>>1; --k>=0; )
	    *--vp3= *vp2++;
    vp3[-1]=vp2[0];		/* subknot() needs this for boundary condition */
    /* insert nnew>>1 knots */
    vp3=dor_nurb_subknot(vp3,vec2,nnew>>1,nvec>>1,order);
    /* copy old median when nvec is odd */
    if (1&nvec) *vp3=vec[nvec>>1];
    /* reverse, so that new left of median is ascending */
    for (vp2=vec2; vp2<vp3; ) { t= *--vp3; *vp3= *vp2; *vp2++=t; }
    /* new knots to right of median */
    k=(1+nvec)>>1;
    dor_nurb_subknot(vec+k,vec2+k+(nnew>>1),nnew-(nnew>>1),nvec-k,order);
    return(vec2);
}

/* increase the multiplicity of the median knot to <order>.
** if the knot vector is already a single bezier interval,
** then bisect with a knot of multiplicity <order>.
*/

struct newknot dor_nurb_subdivide_knot_bezier (
    register DtReal *vec,
    int nvec,
    int order)
{
    struct newknot retval;
    register int k=nvec>>1;
    DtReal *vec2; register DtReal *vp,m;
    if (vec[k]==vec[nvec-1])	/* already bezier */
	    m=0.5*(vec[0]+vec[k]);
    else {
	m=vec[k]; --order;
	while (vec[k]==vec[k-1]) --k;
	while (vec[k]==vec[k+1]) {--order; ++k;}
	++k;
    }
    if (order>0) {
	vec2=(DtReal *)dor_space_allocate((retval.n=nvec+order)*sizeof(*vec2));
	bcopy(vec,vec2,k*sizeof(*vec2));
	for (vp= &vec2[k]; --order>=0; ) *vp++=m;
	bcopy(&vec[k],vp,(nvec-k)*sizeof(*vp));
	retval.p=vec2;
    } else {
	retval.n=nvec;
	retval.p=vec;
    }
    return(retval);
}

DtReal *dor_nurb_subknot (
    register DtReal *vp,	/* old knot vector */
    register DtReal *v2,	/* new knot vector */
    int new,			/* number of new knots to insert */
    int len,			/* number of knots in *vp */
    int ord)			/* order of polynomial */
{
	register DtReal *vm=vp,*ve=vp+len;
	while (vp[-1]==vp[0]) { ++vm; --len; *v2++= *vp++; }
	while (0<new) {
		if (vp[-1]==vp[0]) {
			v2[0]=v2[-1]; ++v2;
		} else {
			while (vm<vp) *v2++= *vm++;
			*v2++=0.5*(vp[-1]+vp[0]);
		}
		--new; ++vp;
		if (--len<ord) while (--new>=0) {v2[0]=v2[-1]; ++v2;}
	}
	while (vm<ve) *v2++= *vm++;  /* stragglers */
	return(v2);
}

#ifdef NOTDEF
DtReal old[50];

void main (void)
{
    int nknot,order,new;
    register int k;
    DtReal *rp,*np;
    struct newknot nk;
    for (;;) {
	printf("\n\nnknot order new:\n");
	scanf(" %d %d %d",&nknot,&order,&new);
	printf("knot values:\n");
	for (rp=old, k=nknot; --k>=0; ++rp) scanf(" %lf",rp);
	printf("new knots:\n");
	nk=dor_nurb_subdivide_knot_bezier(old,nknot,order,new);
	np=nk.p;
	for (k=nk.n; --k>=0; ++np) {
	    printf("  %g",*np);
	    if (0==(7&k)) printf("\n");
	}
    }
}
#endif NOTDEF
/*
 ======================================================================
 */
void dor_nurbsurf_subdivide_along_u (
    register struct nurbsurf *orig,
    register struct nurbsurf *lu,
    register struct nurbsurf *ru)
{
    register int k,j;
    struct nurbsurf new; struct newknot nk;

    dor_nurbsurf_dumpnurb("subu orig",orig);
    *lu= *orig; *ru= *orig;
    lu->u_closed=0; lu->v_closed=0; lu->u_owner=0; lu->v_owner=0; lu->cp_owner=0;
    ru->u_closed=0; ru->v_closed=0; ru->u_owner=0; ru->v_owner=0; ru->cp_owner=0;
    lu->edgeflat[3]=0; ru->edgeflat[2]=0;
    if (!orig->edgeflat[0]) { lu->cornerflat[2]=0; ru->cornerflat[0]=0; }
    else                    { lu->cornerflat[2]=1; ru->cornerflat[0]=1;
			      ru->corner[0].x = lu->corner[2].x =
					0.5*(lu->corner[0].x+lu->corner[2].x);
			      ru->corner[0].y = lu->corner[2].y = 
					0.5*(lu->corner[0].y+lu->corner[2].y);
			      ru->corner[0].z = lu->corner[2].z = 
					0.5*(lu->corner[0].z+lu->corner[2].z);
			  }
    if (!orig->edgeflat[1]) { lu->cornerflat[3] = 0; ru->cornerflat[1] = 0; }
    else                    { lu->cornerflat[3] = 1; ru->cornerflat[1] = 1;
			      ru->corner[1].x = lu->corner[3].x = 
					0.5*(lu->corner[1].x+lu->corner[3].x);
			      ru->corner[1].y = lu->corner[3].y = 
					0.5*(lu->corner[1].y+lu->corner[3].y);
			      ru->corner[1].z = lu->corner[3].z = 
					0.5*(lu->corner[1].z+lu->corner[3].z);
			  }

    nk=dor_nurb_subdivide_knot_bezier(orig->u,orig->u_nknot,orig->u_order);
    if (k=(nk.n-orig->u_nknot)) {
	new= *orig; new.u_owner=1;
	new.u_nknot+=k; lu->u_nknot+=k; ru->u_nknot+=k;
	new.u_nctrl+=k; lu->u_nctrl+=k; ru->u_nctrl+=k;
	lu->u=ru->u=new.u=nk.p;
	lu->u_owner=1;
	/* new control points [Oslo alg] */
	lu->cptp=ru->cptp=new.cptp=dor_nurbsurf_subdivide_control_along_u(
						&new,orig,orig->cptp);
	lu->cp_owner=1;
	orig= &new;
	dor_nurbsurf_dumpnurb("subu new",&new);
    }
    {				/* cut in half */
	register DtNPoint4 **cp0=orig->cptp,**cpr; register DtReal d;
	k=lu->u_nknot; k>>=1; d=lu->u[k];
	lu->uv_area.ur.x=ru->uv_area.ll.x=d;
	while (lu->u[--k]==d) ; ++k;
	lu->u_nctrl =k; lu->u_nknot =k+lu->u_order;
	while (ru->u[++k]==d) ; k-=ru->u_order;
	ru->u_nctrl-=k; ru->u_nknot-=k; ru->u+=k;
	j=ru->v_nctrl;
	ru->cptp=cpr=(DtNPoint4 **)dor_space_allocate2(j*sizeof(*ru->cptp),ru);
	while (--j>=0) *cpr++=k+*cp0++;
	ru->cp_owner=1;
    }
    dor_nurbsurf_test_v_boundaries(lu);
    dor_nurbsurf_test_v_boundaries(ru);
    dor_nurbsurf_dumpnurb("subu lu",lu);
    dor_nurbsurf_dumpnurb("subu ru",ru);
}

void dor_nurbsurf_dumpnurb (char *msg, register struct nurbsurf *np)
{
#ifdef NOTDEF
    printf("\n%s @%lx\n",msg,np);
    printf("flags %x  order %d %d",*(short *)np,np->u_order,np->v_order);
    printf("  nknot %d %d  nctrl %d %d  cornerflat %d %d %d %d  edgeflat %d %d %d %d\n",
	   np->u_nknot,np->v_nknot,np->u_nctrl,np->v_nctrl,
	   np->cornerflat[0],np->cornerflat[1],np->cornerflat[2],np->cornerflat[3],
	   np->edgeflat[0],np->edgeflat[1],np->edgeflat[2],np->edgeflat[3]);
    printf("corners\n");
    for (k=0; k<4; ++k) {
	printf("  (%g %g %g)\n",
	       np->corner[k].x, np->corner[k].y, np->corner[k].z);
    }
    dor_nurbsurf_dumpvec("u",np->u,np->u_nknot);
    dor_nurbsurf_dumpvec("v",np->v,np->v_nknot);
    printf("area (%g %g) (%g %g)\n",np->uv_area.ll,np->uv_area.ur);
    printf("cp %x\n",np->cptp);
    printf("cp\n",np->cptp);
    for (k=0; k<np->v_nctrl; ++k) {
	register DtNPoint4 *cp;
	for (cp=np->cptp[k], j=np->u_nctrl; --j>=0; ++cp) {
	    printf("  (%g %g %g %g)",*cp);
	    if (0==(3&j)) printf("\n");
	}
	printf("\n");
    }
#endif NOTDEF
}

void dor_nurbsurf_dumpvec (char *msg, DtReal *p, int n)
{
    /*printf("%s @%lx (",msg,p);*/
    printf("%s (",msg);
    while (--n>=0) {
	printf(" %g",*p++);
	if (0==n) printf(")");
	if (0==(7&n)) printf("\n");
    }
}
/*
 ======================================================================
 */
void dor_nurbsurf_subdivide_along_v (
    register struct nurbsurf *orig,
    register struct nurbsurf *lv,
    register struct nurbsurf *rv)
{
    register int k;
    struct nurbsurf new; struct newknot nk;

    dor_nurbsurf_dumpnurb("subv orig",orig);
    *lv= *orig; *rv= *orig;
    lv->v_closed=0; lv->v_closed=0; lv->u_owner=0; lv->v_owner=0; lv->cp_owner=0;
    rv->v_closed=0; rv->v_closed=0; rv->u_owner=0; rv->v_owner=0; rv->cp_owner=0;
    lv->edgeflat[1]=0; rv->edgeflat[0]=0;
    if (!orig->edgeflat[2]) { lv->cornerflat[1] = 0; rv->cornerflat[0] = 0; }
    else                    { lv->cornerflat[1] = 1; rv->cornerflat[0] = 1;
			      rv->corner[0].x = lv->corner[1].x = 
					0.5*(lv->corner[0].x+lv->corner[1].x);
			      rv->corner[0].y = lv->corner[1].y = 
					0.5*(lv->corner[0].y+lv->corner[1].y);
			      rv->corner[0].z = lv->corner[1].z = 
					0.5*(lv->corner[0].z+lv->corner[1].z);
			  }
    if (!orig->edgeflat[3]) { lv->cornerflat[3] = 0; rv->cornerflat[2] = 0; }
    else                    { lv->cornerflat[3] = 1; rv->cornerflat[2] = 1;
			      rv->corner[2].x = lv->corner[3].x = 
					0.5*(lv->corner[2].x+lv->corner[3].x);
			      rv->corner[2].y = lv->corner[3].y = 
					0.5*(lv->corner[2].y+lv->corner[3].y);
			      rv->corner[2].z = lv->corner[3].z = 
					0.5*(lv->corner[2].z+lv->corner[3].z);
			  }

    nk=dor_nurb_subdivide_knot_bezier(orig->v,orig->v_nknot,orig->v_order);
    if (k=(nk.n-orig->v_nknot)) {
	new= *orig; new.v_owner=1;
	new.v_nknot+=k; lv->v_nknot+=k; rv->v_nknot+=k;
	new.v_nctrl+=k; lv->v_nctrl+=k; rv->v_nctrl+=k;
	lv->v=rv->v=new.v=nk.p;
	lv->v_owner=1;
	/* new control points [Oslo alg] */
	lv->cptp=rv->cptp=new.cptp=dor_nurbsurf_subdivide_control_along_v(
						&new,orig,orig->cptp);
	lv->cp_owner=1;
	orig= &new;
	dor_nurbsurf_dumpnurb("subv new",&new);
    }
    {				/* cut in half */
	register DtReal d;
	k=lv->v_nknot; k>>=1; d=lv->v[k];
	lv->uv_area.ur.y=rv->uv_area.ll.y=d;
	while (lv->v[--k]==d) ; ++k;
	lv->v_nctrl =k; lv->v_nknot =k+lv->v_order;
	while (rv->v[++k]==d) ; k-=rv->v_order;
	rv->v_nctrl-=k; rv->v_nknot-=k; rv->v+=k;
	rv->cptp+=k;
    }
    dor_nurbsurf_test_u_boundaries(lv);
    dor_nurbsurf_test_u_boundaries(rv);
    dor_nurbsurf_dumpnurb("subv lv",lv);
    dor_nurbsurf_dumpnurb("subv rv",rv);
}
/*
 ======================================================================
 */

void dor_nurbsurf_subdivide_along_u_and_v (
    struct nurbsurf *orig,
    struct nurbsurf *lulv,
    struct nurbsurf *lurv,
    struct nurbsurf *rulv,
    struct nurbsurf *rurv)
{
    struct nurbsurf sub[2];
    dor_nurbsurf_subdivide_along_u(orig,&sub[0],&sub[1]);
    dor_nurbsurf_subdivide_along_v(&sub[0],lulv,lurv);
    dor_nurbsurf_subdivide_along_v(&sub[1],rulv,rurv);
}
/*
 ======================================================================
 */

void dor_nurbsurf_tessellate (
    struct nurbsurf *nurbsurf,
    DtShort tessellatetype,
    DtPFV action,
    DtPtr actionparm)
{
   DtInt lo, hi;
   DtReal mid;
   DtReal logvalue;
   register int k;

   if (nurbsurf->subdivlvl[0]<=0.0) {
	DDerror (ERR_ZERO_SUBDIVSPEC,
		    "dor_nurbsurf_tessellate", DcNullPtr);
   }

   for (k=4; --k>=0; ) { 
	nurbsurf->edgeflat[k]=0; 
	nurbsurf->cornerflat[k]=0; 
   }

   switch(nurbsurf->subdivtype) {
   case DcSubDivSegments:
	/* Will use fixed subdivision with k that results in
	   the number of segments closest to specified level. */
	if (0>=(k=nurbsurf->subdivlvl[0])) {
		k=3;
	} else {
		logvalue = dor_math_log(nurbsurf->subdivlvl[0])/M_LN2;
		lo = dor_math_floor(logvalue);
		if ((logvalue - lo) < .000001) {
			k = lo+1;
		} else {
			hi = dor_math_ceil(logvalue);
			mid = (dor_math_pow(2.,(DtReal)lo) +
				dor_math_pow(2.,(DtReal)hi)) / 2.;
			if (nurbsurf->subdivlvl[0] > mid)
				k = hi+1;
			else
				k = lo+1;
		}
	}
	
		dor_nurbsurf_subdivide_fixed(nurbsurf,k,
			tessellatetype,action,actionparm);
		break;
   case DcSubDivFixed:
	if (0>=(k=nurbsurf->subdivlvl[0]))
		k=3;
	dor_nurbsurf_subdivide_fixed(nurbsurf,k,
		tessellatetype,action,actionparm);
	break;
   case DcSubDivAbsolute:
   case DcSubDivRelative:
	if (nurbsurf->subdivlvl[0]<=0.0)
		nurbsurf->subdivlvl[0]=0.03;
	dor_nurbsurf_subdivide_adaptive(nurbsurf,
		tessellatetype,action,actionparm);
	break;
   }
}
/*
 ======================================================================
 */

void dor_nurbsurf_tessellate_nurbsurf (
    register struct nurbsurf *np,
    DtShort  tessellatetype,
    DtPFV    action,
    char    *actionparm)
{
	static char ac=1;
	DtNPoint3 p,pu,pv;
	DtNPoint3 v00,v00cos;
	DtNPoint3 v30,v30cos;
	DtNPoint3 v33,v33cos;
	DtNPoint3 v03,v03cos;

	dor_nurbsurf_dumpnurb("tessellate",np);
	/*  Assign the v locations and calculate the v cosines.   */
	p=dor_nurb_4to3(&np->cptp[0][0]);
	if (ac && np->cornerflat[0])
		v00=np->corner[0];
	else	v00=p;
	if (tessellatetype == dod_nurbsurf_tessellatetype_mesh) {
	    pu=dor_nurb_4to3(&np->cptp[0][1]);
	    pv=dor_nurb_4to3(&np->cptp[1][0]);
	    dor_math_cross_cosines(
		    pu.x, pu.y, pu.z, p.x, p.y, p.z, pv.x, pv.y, pv.z,
		    &v00cos.x, &v00cos.y, &v00cos.z);
	}

	p=dor_nurb_4to3(&np->cptp[0][np->u_nctrl-1]);
	if (ac && np->cornerflat[2])
	    v30=np->corner[2];
	else
	    v30=p;
	if (tessellatetype == dod_nurbsurf_tessellatetype_mesh) {
	    pu=dor_nurb_4to3(&np->cptp[0][np->u_nctrl-2]);
	    pv=dor_nurb_4to3(&np->cptp[1][np->u_nctrl-1]);
	    dor_math_cross_cosines(
		    pv.x, pv.y, pv.z, p.x, p.y, p.z, pu.x, pu.y, pu.z,
		    &v30cos.x, &v30cos.y, &v30cos.z);
	}

	p=dor_nurb_4to3(&np->cptp[np->v_nctrl-1][np->u_nctrl-1]);
	if (ac && np->cornerflat[3])
	    v33=np->corner[3];
	else
	    v33=p;
	if (tessellatetype == dod_nurbsurf_tessellatetype_mesh) {
	    pu=dor_nurb_4to3(&np->cptp[np->v_nctrl-1][np->u_nctrl-2]);
	    pv=dor_nurb_4to3(&np->cptp[np->v_nctrl-2][np->u_nctrl-1]);
	    dor_math_cross_cosines(
		    pu.x,pu.y,pu.z, p.x, p.y, p.z, pv.x,pv.y,pv.z,
		    &v33cos.x,&v33cos.y,&v33cos.z);
	}

	p=dor_nurb_4to3(&np->cptp[np->v_nctrl-1][0]);
	if (ac && np->cornerflat[1])
	    v03=np->corner[1];
	else
	    v03=p;
	if (tessellatetype == dod_nurbsurf_tessellatetype_mesh) {
	    pu=dor_nurb_4to3(&np->cptp[np->v_nctrl-1][1]);
	    pv=dor_nurb_4to3(&np->cptp[np->v_nctrl-2][0]);
	    dor_math_cross_cosines(
		    pv.x,pv.y,pv.z, p.x, p.y, p.z, pu.x,pu.y,pu.z,
		    &v03cos.x,&v03cos.y,&v03cos.z);
	}

	if (tessellatetype == dod_nurbsurf_tessellatetype_mesh) {
	    static int count=0;
	    ++count;
	    if (count<1) {
		printf("tespat %d\n",count);
		printf(" (%g %g %g)  (%g %g %g)\n",
			v00.x,v00.y,v00.z,v00cos.x,v00cos.y,v00cos.z);
		printf(" (%g %g %g)  (%g %g %g)\n",
			v03.x,v03.y,v03.z,v03cos.x,v03cos.y,v03cos.z);
		printf(" (%g %g %g)  (%g %g %g)\n",
			v30.x,v30.y,v30.z,v30cos.x,v30cos.y,v30cos.z);
		printf(" (%g %g %g)  (%g %g %g)\n",
			v33.x,v33.y,v33.z,v33cos.x,v33cos.y,v33cos.z);
	    }
	    (*action)(actionparm,&v00,&v03,&v33, &v00cos,&v03cos,&v33cos);
	    (*action)(actionparm,&v00,&v33,&v30, &v00cos,&v33cos,&v30cos);
	} else {
	    (*action)(actionparm,&v00,&v03,&v33);
	    (*action)(actionparm,&v00,&v33,&v30);
	}
	/*dor_device_flush();*/
}
/*
 ======================================================================
 */

static DtFlag dor_nurbsurf_Point4_same (
    DtNPoint4 *p0,
    DtNPoint4 *p1)
{
    DtNPoint3 q0, q1; 
    DtReal d,z;
	
    q0=dor_nurb_4to3(p0);
    q1=dor_nurb_4to3(p1);

    d=dor_math_distance_squared(q0.x,q0.y,q0.z, q1.x,q1.y,q1.z);
    z=dor_math_distance_squared(q0.x,q0.y,q0.z, 0.,0.,0.);
#ifdef NOTDEF
    if (jfrdebug) {
	printf("Point4_same (%g %g %g %g) (%g %g %g %g) %g %g\n",
	       p0->x,p0->y,p0->z,p0->w,
	       p1->x,p1->y,p1->z,p1->w,
	       d,z);
    }
#endif NOTDEF
    if ((1.0e6*d)<=z)
	    return(DcTrue);
    else
	    return(DcFalse);
}

/*  Test if the linear sequence of control vertices is flat.  */

DtFlag dor_nurbsurf_test_linear (
    int n,
    register DtNPoint4 *p,
    DtInt subdivtype,
    DtReal *subdivlvl)
{
    register DtReal w,d;
    DtNPoint3 p0, pn;
    register DtReal (*func)() = dor_math_point_line_distance;

    /* trim identical endpoints */
    while (2<n) {
	if (dor_nurbsurf_Point4_same(p+0,p+1)) {
	    --n; ++p;
	} else
	    break;
    }
    while (2<n) {
	if (dor_nurbsurf_Point4_same(p+n-1,p+n-2)) {
	    --n;
	} else
	    break;
    }

    p0=dor_nurb_4to3(p+0);
    pn=dor_nurb_4to3(p+n-1);
    d=subdivlvl[0];
    w=dor_math_distance(p0.x,p0.y,p0.z,pn.x,pn.y,pn.z);
    if (n<=2) {			/* degenerate case: no interior points */
	if (w<=d)
		return(DcTrue);
	else
		return(DcFalse);
    }
    if (DcSubDivRelative==subdivtype)
	    d*=w;
    if (0.0>=d) func=dor_math_distance;
#ifdef NOTDEF
    if (jfrdebug) {
	printf("dor_nurbsurf_test_linear (%g %g %g) (%g %g %g)\n",
	       p0.x,p0.y,p0.z, pn.x,pn.y,pn.z);
    }
#endif NOTDEF

    /*  Compute the distance between the non-end points and
	the line drawn between the endpoints of the curve.  */

    for (n-=2, ++p; --n>=0; ++p) {
	register DtReal t;
	w=1.0/p->w;
	t=func(p->x*w,p->y*w,p->z*w, p0.x,p0.y,p0.z, pn.x,pn.y,pn.z);
#ifdef NOTDEF
	if (jfrdebug) {
	    printf("  %g (%g %g %g)\n",t,p->x*w,p->y*w,p->z*w);
	}
#endif NOTDEF
	if (d<t)
		return(DcFalse);
	if (t<=(0.001*d)) {
	    /* the sequence of points   a b a b   is not linear
	       unless point a is close enough to point b.
	       If an intermediate point p1 .. pn-1 is exactly
	       on the line p0-pn, then substitute the intermediate
	       point for the endpoint p0.  If this causes the end
	       points to coincide, then change the function
	       accordingly.
	       */
	    p0.x=p->x*w;
	    p0.y=p->y*w;
	    p0.z=p->z*w;
	    if (p0.x==pn.x && p0.y==pn.y && p0.z==pn.z)
		func=dor_math_distance;
	}
    }
#ifdef NOTDEF
    if (jfrdebug)
	    printf(" linear\n");
#endif NOTDEF
    return(DcTrue);
}
/*
 ======================================================================
 */

/* Test the nurbsurf for flatness of its u parameter boundaries.  If it
** is flat in both of its boundaries, return true, otherwise return false.
** In addition, as part of "cracking" prevention, mark boundaries wch
** are flat.  If a boundary has already been marked as flat, do not test
** it again.
*/

DtFlag dor_nurbsurf_test_u_boundaries (struct nurbsurf *np)
{
    register int k=np->u_nctrl;

    if (!np->edgeflat[0]) {
	if (dor_nurbsurf_test_linear(k,np->cptp[0],np->subdivtype,np->subdivlvl)) {
	    np->edgeflat[0]=1;
	    if (!np->cornerflat[0]) {
		np->cornerflat[0]=1;
		np->corner[0]=dor_nurb_4to3(&np->cptp[0][0]);
	    }
	    if (!np->cornerflat[2]) {
		np->cornerflat[2]=1;
		np->corner[2]=dor_nurb_4to3(&np->cptp[0][np->u_nctrl-1]);
	    }
	}
    }
    if (!np->edgeflat[1]) {
	if (dor_nurbsurf_test_linear(k,np->cptp[np->v_nctrl-1],np->subdivtype,
		np->subdivlvl)) {
	    np->edgeflat[1]=1;
	    if (!np->cornerflat[1]) {
		np->cornerflat[1]=1;
		np->corner[1]=dor_nurb_4to3(&np->cptp[np->v_nctrl-1][0]);
	    }
	    if (!np->cornerflat[3]) {
		np->cornerflat[3]=1;
		np->corner[3]=dor_nurb_4to3(&np->cptp[np->v_nctrl-1][np->u_nctrl-1]);
	    }
	}
    }
    return(np->edgeflat[0] && np->edgeflat[1]);
}
/*
 ======================================================================
 */

/* Test the nurbsurf for flatness of its v parameter boundaries.  If it
** is flat in both of its boundaries, return true, otherwise return false.
** In addition, as part of "cracking" prevention, mark boundaries which
** are flat.  If a boundary has already been marked as flat, do not test
** it again.
*/

DtFlag dor_nurbsurf_test_v_boundaries (struct nurbsurf *np)
{
    register int k;
    DtNPoint4 tmp[100],*tp;

    /*printf("tstvbnd %d\n",np->v_nctrl);*/
    if (!np->edgeflat[2]) {
	for (tp=tmp, k=np->v_nctrl; --k>=0; )
		*tp++=np->cptp[k][0];
	if (dor_nurbsurf_test_linear(np->v_nctrl,tmp,np->subdivtype,np->subdivlvl)) {
	    np->edgeflat[2]=1;
	    if (!np->cornerflat[0]) {
		np->cornerflat[0]=1;
		np->corner[0]=dor_nurb_4to3(&np->cptp[0][0]);
	    }
	    if (!np->cornerflat[1]) {
		np->cornerflat[1]=1;
		np->corner[1]=dor_nurb_4to3(&np->cptp[np->v_nctrl-1][0]);
	    }
	}
    }
    if (!np->edgeflat[3]) {
	register int j=np->u_nctrl-1;
	for (tp=tmp, k=np->v_nctrl; --k>=0; )
		*tp++=np->cptp[k][j];
	if (dor_nurbsurf_test_linear(np->v_nctrl,tmp,np->subdivtype,np->subdivlvl)) {
	    np->edgeflat[3]=1;
	    if (!np->cornerflat[2]) {
		np->cornerflat[2]=1;
		np->corner[2]=dor_nurb_4to3(&np->cptp[0][np->u_nctrl-1]);
	    }
	    if (!np->cornerflat[3]) {
		np->cornerflat[3]=1;
		np->corner[3]=dor_nurb_4to3(&np->cptp[np->v_nctrl-1][np->u_nctrl-1]);
	    }
	}
    }
    return(np->edgeflat[2] && np->edgeflat[3]);
}
/*
 ======================================================================
 */

dot_object *dor_nurbsurf_create_alternate_geometry (struct nurbsurf *nurbsurf)
{
    dot_object *trilstobj;

    trilstobj = dor_trilst_open (DcRGB, DcLocNrm, 500);

    dor_nurbsurf_tessellate
    (   nurbsurf,
	(DtShort) dod_nurbsurf_tessellatetype_mesh,
	dor_nurbsurf_create_alternate_geometry_triangle,
	trilstobj
    );

    dor_trilst_close (trilstobj);

    return(trilstobj);
}
/*
 ======================================================================
 */

void dor_nurbsurf_create_alternate_geometry_triangle (
    dot_object *trilstobj,
    DtNPoint3 *vtx1,
    DtNPoint3 *vtx2,
    DtNPoint3 *vtx3,
    DtNPoint3 *vtxnrm1,
    DtNPoint3 *vtxnrm2,
    DtNPoint3 *vtxnrm3)
{
	typedef struct {DtNPoint3 p,n;} DtNPointNormal;
	DtNPointNormal vertices[3];

	if (!dor_nurbsurf_check_triangle(vtx1,vtx2,vtx3))
	    return;

	vertices[0].p= *vtx1;
	vertices[1].p= *vtx2;
	vertices[2].p= *vtx3;

	dor_math_renormalize_fast ((DtReal*)vtxnrm1);
	dor_math_renormalize_fast ((DtReal*)vtxnrm2);
	dor_math_renormalize_fast ((DtReal*)vtxnrm3);

	vertices[0].n.x = vtxnrm1->x;
	vertices[0].n.y = vtxnrm1->y;
	vertices[0].n.z = vtxnrm1->z;
	vertices[1].n.x = vtxnrm2->x;
	vertices[1].n.y = vtxnrm2->y;
	vertices[1].n.z = vtxnrm2->z;
	vertices[2].n.x = vtxnrm3->x;
	vertices[2].n.y = vtxnrm3->y;
	vertices[2].n.z = vtxnrm3->z;

	dor_trilst_add_triangle (trilstobj, (DtReal*) vertices);
}
/*
 ======================================================================
 */

DtFlag dor_nurbsurf_check_subdiv (struct nurbsurf *nurbsurf)
{
    DtFlag returnflag;
    DtInt subdivtype;
    DtReal subdivlvl[5];

    returnflag = DcFalse;

    dor_global_subdivspc_get_value(&subdivtype, subdivlvl);

    if (nurbsurf->subdivtype != subdivtype) {
	nurbsurf->subdivtype = subdivtype;
	returnflag = DcTrue;
    }

    if (nurbsurf->subdivlvl[0] != subdivlvl[0]) {
	nurbsurf->subdivlvl[0] = subdivlvl[0];
	returnflag = DcTrue;
    }

    return (returnflag);
}
/*
 ======================================================================
 */

DtFlag dor_nurbsurf_check_triangle (
    DtNPoint3 *vertex1,
    DtNPoint3 *vertex2,
    DtNPoint3 *vertex3)
{
    if (vertex1->x==vertex2->x && vertex1->y==vertex2->y &&
	vertex1->z==vertex2->z)
	    return(DcFalse);

    if (vertex1->x==vertex3->x && vertex1->y==vertex3->y &&
	vertex1->z==vertex3->z)
	    return(DcFalse);

    if (vertex2->x==vertex3->x && vertex2->y==vertex3->y &&
	vertex2->z==vertex3->z)
	    return(DcFalse);

    return(DcTrue);
}
/*
 ======================================================================
 */

void dor_nurbsurf_compute_bounding_volume (dot_object *object)
{
    struct nurbsurf *nurbsurf;

    nurbsurf = (struct nurbsurf *)(object->data);

    if (nurbsurf->alternate_object != DcNullPtr) {
   	dor_object_get_method(nurbsurf->alternate_object,
			      DcMethodCmpBndVolume) (nurbsurf->alternate_object);

   	return;
    } else {
   	nurbsurf->alternate_object = 
   		dor_nurbsurf_create_alternate_geometry(nurbsurf);

   	if (nurbsurf->alternate_object == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_nurbsurf_compute_bounding_volume", "");
	    return;
   	}

   	dor_object_get_method(nurbsurf->alternate_object,
			      DcMethodCmpBndVolume) (nurbsurf->alternate_object);

   	return;
    }
}
/*
 ======================================================================
 */

DtNPoint3 dor_nurb_4to3 (DtNPoint4 *p4)
{
    DtNPoint3 p3;
    register DtReal w=1.0/p4->w;
    p3.x=p4->x*w;
    p3.y=p4->y*w;
    p3.z=p4->z*w;
    return(p3);
}

#define OMAX 25
/* spline value recurrence with DtNPoint4 coefficients */
DtNPoint4 dor_nurb_svr4 (
    DtReal x,		/* point at which to evaluate */
    DtReal *u,		/* knot vector with u[0] <= x < u[1] */
    DtNPoint4 *cin,	/* coefficients -(k-1)..0 */
    register int k)	/* order; also 1+degree */
{
	DtNPoint4 c[OMAX];  /* scratch area */
	register int i,j;
	if (k<=0 || OMAX<k)
		DDerror (ERR_BAD_CURVE_ORDER,
			    "DoNURB", "srv4");
	if (x<u[0] || x>=u[1])
		DDerror (ERR_BAD_CURVE_INTERVAL,
			    "DoNURB", "svr4");
	bcopy(&cin[1-k],&c[0],k*sizeof(*cin));
	for (j=k-1; --j>=0; ) {/* elevate degree */
		register DtReal *ui; register DtNPoint4 *cki;
		/*printf("j=%d  x=%g\n",j,x);*/
		for (ui=u, cki=c+k-1, i= -j; i<=0; --ui,--cki,++i) {
				/* sweep coeff */
			register DtReal d=ui[j+1]-ui[0];
			/*printf("i=%d  ui[j+1]=%g  ui[0]=%g\n",i,ui[j+1],ui[0]);*/
			/*printf("  cki[0]=(%g %g %g %g)\n",cki[0]);*/
			/*printf("  cki[-1]=(%g %g %g %g)\n",cki[-1]);*/
			if (0.0==d) {
				cki[0].x=cki[0].y=cki[0].z=0.0; cki[0].w=1.0;
				/*printf("  zero denominator\n");*/
			} else {
				register DtReal f=(x-ui[0])/d, g=1.0-f;
				/*printf("f=%g  g=%g\n",f,g);*/
				cki[0].x=cki[0].x*f+cki[-1].x*g;
				cki[0].y=cki[0].y*f+cki[-1].y*g;
				cki[0].z=cki[0].z*f+cki[-1].z*g;
				cki[0].w=cki[0].w*f+cki[-1].w*g;
				/*printf("   (%g %g %g %g)\n",cki[0]);*/
			}
		}
	}
	/*printf("svr4=(%g %g %g %g)\n",c[k-1]);*/
	return(c[k-1]);
}

/* derivative value recurrence with DtNPoint4 coefficients */

DtNPoint4 dor_nurb_dvr4 (
    DtReal x,		/* point at which to evaluate */
    DtReal *u,		/* knot vector with u[0] <= x < u[1] */
    DtNPoint4 *cin,	/* coefficients -(k-1)..0 */
    register int k,	/* order; also 1+degree */
    int r)		/* number of derivatives to take */
{
	DtNPoint4 c[OMAX];  /* scratch area */
	register int i,j;
	/*printf("r=%d\n",r);*/
	c[0].x=c[0].y=c[0].z=c[0].w=0.0;
	if (r>=k) return(c[0]);
	if (k<=0 || OMAX<k)
		DDerror (ERR_BAD_CURVE_ORDER,
			    "DoNURB", "dvr4");
	if (x<u[0] || x>=u[1])
		DDerror (ERR_BAD_CURVE_INTERVAL,
			    "DoNURB", "dvr4");
	bcopy(&cin[1-k],&c[0],k*sizeof(*cin));
	for (j=1; j<=r; ++j) {/* elevate degree */
		register DtReal *ui; register DtNPoint4 *ci;
		for (ui=u, ci=c+k-1, i=k-j; --i>=0; --ui,--ci) {
				/* sweep coeff */
			register DtReal d=ui[k-j]-ui[0];
			/*printf("i=%d  j=%d  k=%d  ui[k-j]=%g  ui[0]=%g\n",
				i,j,k,ui[k-j],ui[0]);*/
			if (0.0==d) {
				ci[0].x=ci[0].y=ci[0].z=ci[0].w=0.0;
			} else {
				d=(k-j)/d;
				/*printf("d=%g  ci[0]=(%g %g %g %g) ci[-1]=(%g %g %g %g)\n",
					d,ci[0],ci[-1]);*/
				ci[0].x=d*(ci[0].x - ci[-1].x);
				ci[0].y=d*(ci[0].y - ci[-1].y);
				ci[0].z=d*(ci[0].z - ci[-1].z);
				ci[0].w=d*(ci[0].w - ci[-1].w);
			}
		}
	}
	return(dor_nurb_svr4(x,u,&c[k-1],k-r));
}

/* control-vertex recurrence with DtNPoint4 coefficients */

DtNPoint4 dor_nurb_cvr4 (
    DtReal *w,		/* new knot vector */
    DtReal *u,		/* old knot vector with u[0] <= w[0] < u[1] */
    DtNPoint4 *vin,	/* coefficients -(k-1)..0 */
    register int k)	/* order; also 1+degree */
{
	DtNPoint4 v[OMAX];  /* scratch area */
	register int i,r;
	if (k<=0 || OMAX<k)
		DDerror (ERR_BAD_CURVE_ORDER,
			    "DoNURB", "cvr4");
	if (w[0]<u[0] || w[0]>=u[1])
		DDerror (ERR_BAD_CURVE_INTERVAL,
			    "DoNURB", "cvr4");
	bcopy(&vin[1-k],&v[0],k*sizeof(*vin));
	for (r=k; --r> 0; ) {/* elevate degree */
		register DtReal *ui,wj=w[r]; register DtNPoint4 *vi;
		/*printf("r=%d  k=%d  w[r]=%g\n",r,k,wj);*/
		for (ui=u, vi=v+k-1, i=r; --i>=0; --ui,--vi) if (wj!=ui[r]) {
			/* sweep coeff */
			register DtReal d=ui[r]-ui[0];
			/*printf("i=%d  ui[r]=%g  ui[0]=%g\n",i,ui[r],ui[0]);*/
			if (0.0==d) {
				vi[0].x=vi[0].y=vi[0].z=vi[0].w=0.0;
				/*printf("zero denominator\n");*/
			} else {
				register DtReal f=(wj-ui[0])/d, g=1.0-f;
				/*printf("f=%g  g=%g  vi[0]=(%g %g %g %g)  vi[-1]=(%g %g %g %g)\n",
					f,g,vi[0].x,vi[0].y,vi[0].z,vi[0].w,
					vi[-1].x,vi[-1].y,vi[-1].z,vi[-1].w);*/
				vi[0].x=vi[0].x*f + vi[-1].x*g;
				vi[0].y=vi[0].y*f + vi[-1].y*g;
				vi[0].z=vi[0].z*f + vi[-1].z*g;
				vi[0].w=vi[0].w*f + vi[-1].w*g;
			}
		}
	}
	return(v[k-1]);
}
