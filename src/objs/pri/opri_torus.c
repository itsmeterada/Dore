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
	dor_torus_initialize
	dor_torus_create
	dor_torus_destroy
	dor_torus_print
	dor_torus_update_alternate
	dor_torus_check_subdiv
	dor_torus_compute_bounding_volume
	dor_torus_pick
	dor_torus_render_standard
	dor_torus_create_alternate_geometry
	dor_torus_ucomp
	dor_torus_vcomp
	dor_torus_compuv_usercallback
	dor_torus_compuvw_usercallback

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/torus.h>
#include <dore/dore_develop/private/trilst.h>
#include <math.h>

/*
 ======================================================================
 */

DtInt doe_torus_class_id;

static DtMethodEntry torus_methods[] = {
    { DcMethodDestroy,         (DtMethodPtr)dor_torus_destroy },
    { DcMethodCmpBndVolume,    (DtMethodPtr)dor_torus_compute_bounding_volume },
    { DcMethodPick,            (DtMethodPtr)dor_torus_pick },
    { DcMethodPrint,           (DtMethodPtr)dor_torus_print },
    { DcMethodUpdStdAltObj,    (DtMethodPtr)dor_torus_update_alternate },
    { DcMethodStdRenderDisplay,(DtMethodPtr)dor_torus_render_standard }
};

/*
 ======================================================================
 */
void dor_torus_initialize (void)
{
    doe_torus_class_id = dor_class_install (DcTypeTorus, "DoTorus", 6,
					    torus_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_torus_create (
    DtReal R,
    DtReal r)
{
    static DtFlag initialized = DcFalse;
    struct torus *t;

    if (!initialized) {
	dor_torus_initialize();
	initialized = DcTrue;
    }

    t = (struct torus *)dor_space_allocate(sizeof *t);

    t->R = R;
    t->r = r;
    dor_global_subdivspc_get_value(&(t->subdivtype), t->subdivparms);
    t->alternate_object = DcNullPtr;
  
    return(dor_object_create(doe_torus_class_id,t));
}
/*
 ======================================================================
 */

void dor_torus_destroy (dot_object *object)
{
    struct torus *torus;

    torus = (struct torus *)(object->data);

    if (torus->alternate_object != NULL){
	dor_object_check_deletion(torus->alternate_object);
    }

    dor_space_deallocate(torus);
}
/*
 ======================================================================
 */

void dor_torus_print (dot_object *torus_object)
{
    struct torus *t;
  
    t = (struct torus *)torus_object->data;
  
    if (t == NULL){
	dor_print_output("torus is NULL");
	return;  
    }

    sprintf(dor_print_get_line(),"radii = (%f, %f)", t->R, t->r);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

dot_object *dor_torus_update_alternate (dot_object *object)
{
    struct torus *torus;
    DtFlag changed;
    dot_object *alt;

    torus = (struct torus *)(object->data);

    changed = dor_torus_check_subdiv(torus);

    if ((torus->alternate_object != DcNullPtr) && !changed) {
	alt = torus->alternate_object;

    } else {
	if (torus->alternate_object != DcNullPtr) {
	    dor_object_get_method(torus->alternate_object,
				  DcMethodDestroy)(torus->alternate_object);

	    torus->alternate_object = DcNullPtr;
	}

	torus->alternate_object = 
		dor_torus_create_alternate_geometry(torus);
	alt = torus->alternate_object;

	if (torus->alternate_object == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_torus_update_alternate", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

DtFlag dor_torus_check_subdiv (struct torus *torus)
{
    DtFlag returnflag;
    DtInt subdivtype;
    DtReal subdivparms[1];

    returnflag = DcFalse;

    dor_global_subdivspc_get_value(&subdivtype, subdivparms);

    /**** right now only one type of subdision is used for the torus *******/
    /*
      if (torus->subdivtype != subdivtype) {
      torus->subdivtype = subdivtype;
      returnflag = DcTrue;
      }
      */

    if (torus->subdivparms[0] != subdivparms[0]) {
	torus->subdivparms[0] = subdivparms[0];
	returnflag = DcTrue;
    }

    return (returnflag);
}
/*
 ======================================================================
 */

void dor_torus_compute_bounding_volume (dot_object *object)
{
    struct torus *tor;

    DtPoint3 corners[8];
    DtReal x_extent, y_extent;
	

    tor = (struct torus *)object->data;
    y_extent = tor->r;
    x_extent = y_extent + tor->R;

    corners[0][0] = -x_extent;
    corners[0][1] = -y_extent;
    corners[0][2] = -x_extent;

    corners[1][0] =  x_extent;
    corners[1][1] = -y_extent;
    corners[1][2] = -x_extent;

    corners[2][0] = -x_extent;
    corners[2][1] =  y_extent;
    corners[2][2] = -x_extent;

    corners[3][0] =  x_extent;
    corners[3][1] =  y_extent;
    corners[3][2] = -x_extent;

    corners[4][0] = -x_extent;
    corners[4][1] = -y_extent;
    corners[4][2] =  x_extent;

    corners[5][0] =  x_extent;
    corners[5][1] = -y_extent;
    corners[5][2] =  x_extent;

    corners[6][0] = -x_extent;
    corners[6][1] =  y_extent;
    corners[6][2] =  x_extent;

    corners[7][0] =  x_extent;
    corners[7][1] =  y_extent;
    corners[7][2] =  x_extent;

    dor_system_report_bounding_points (8, (DtReal*)corners);
}
/*
 ======================================================================
 */

void dor_torus_pick (dot_object *object)
{
    struct torus *torus;
    DtFlag changed;

    if (!dor_condex_query_pick(doe_torus_class_id))
	    return;

    dor_pick_initialize_object(object);

    torus = (struct torus *)(object->data);

    changed = dor_torus_check_subdiv(torus);

    if ((torus->alternate_object != DcNullPtr) && !changed) {
	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(torus->alternate_object,
			      DcMethodPick) (torus->alternate_object);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    } else {
	if (torus->alternate_object != NULL) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(torus->alternate_object,
				  DcMethodDestroy)(torus->alternate_object);

	    doe_system_alternate_object_execution_flag = DcFalse;
 
	    torus->alternate_object = NULL;
        }

	torus->alternate_object = dor_torus_create_alternate_geometry(torus);

	if (torus->alternate_object == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_torus_pick", "");
	    return;
	}

	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(torus->alternate_object,
			      DcMethodPick)(torus->alternate_object);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    }
}
/*
 ======================================================================
 */

void dor_torus_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_torus_class_id))
	    return;

    alt = dor_torus_update_alternate (object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
/*
 ======================================================================
 */

/*
	Spherical coordinates are used for parameterizing the
        torus for 2D texture mapping:
	Note that the Dore torus lies in the xz-plane.

            atan2(x,z)
        u = ----------- + .5    (atan2 returns (-PI,PI] and we
               2*PI 		 want a value between 0 and 1)


            arccos(y/smallradius)
        v = ---------------------
                    PI

      For 3D texture mapping, the torus is transformed to fit in the
      space [0,1] in x, y, and z. The uvw are computed from these transformed
      coordinates xyz, as: u=x,  v=1-y, and w=1-z.

*/

#define TWOPI 	(M_PI * 2.)

dot_object *dor_torus_create_alternate_geometry (struct torus *t)
{
    DtInt subdivtype;
    DtReal subdivlevel[1];
    DtSwitch cmptexuvswi;
    DtSwitch cmptexuvwswi;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    static DtInt space=0;
    static DtReal *vertices;
    static DtInt trivertindex[] = {0, 1, 2, 0, 2, 3};
    DtReal *vptr;
    DtInt needspace;
    DtInt vtxsize;
    DtInt divisions;
    DtInt tricount;
    DtReal quad[4][11];
    DtInt i, j, k, v, qd;
    DtReal alpha, beta;
    DtReal alpha_inc, beta_inc;
    DtReal cos_alpha, sin_alpha, cos_alpha_plus_inc, sin_alpha_plus_inc;
    DtReal cos_beta,  sin_beta,  cos_beta_plus_inc,  sin_beta_plus_inc;
    DtReal x, x_inc;
    DtReal center1[3], center2[3];
    DtVertexType vertextype;
    DtReal totw, toth;
    dot_object *trilstobj;
    DtReal smallr;
    DtPFI uv_function;
    DtPFI uvw_function;
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;

    dor_global_subdivspc_get_value(&subdivtype, subdivlevel);

    if (subdivtype != DcSubDivSegments) {
	/* old style subdivision spec */
   	subdivlevel[0] = max(0.,subdivlevel[0]);
   	subdivlevel[0] = min(1.,subdivlevel[0]);
   	divisions = 5 + (subdivlevel[0]*60.);
    } else {
   	subdivlevel[0] = max(4.,subdivlevel[0]);
	divisions = subdivlevel[0];
    }

    vtxsize = 6;
    vertextype = DcLocNrm;

    /*  Are texture uvs and/or uvws to be computed?  If so, determine
	whether to use the "callback" provided by Dore or a user-defined
	callback. */

    dor_global_cmptexuvswi_get_value(&cmptexuvswi);
    if (cmptexuvswi) {
	vtxsize += 2;
        vertextype |= dor_system_texture_uv_count(1);
	dor_global_cmptexuvcbk_get_value(&cmptexuvcbk);
	if (cmptexuvcbk != DcStdUVCallback) {
            uv_function = dor_calbak_get_function(cmptexuvcbk);
            uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);
	}
    } 
    dor_global_cmptexuvwswi_get_value(&cmptexuvwswi);
    if (cmptexuvwswi) {
        vtxsize += 3;
        vertextype |= dor_system_texture_uvw_count(1);
        dor_global_cmptexuvwcbk_get_value(&cmptexuvwcbk);
        if (cmptexuvwcbk != DcStdUVWCallback) {
            uvw_function = dor_calbak_get_function(cmptexuvwcbk);
            uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);
        }
    }

    needspace = divisions*divisions*6*vtxsize;
    if (needspace > space) {
        if (space != 0)
        	vertices = (DtReal *)dor_space_reallocate
				(vertices, needspace*(sizeof (DtReal)));
	else
        	vertices = (DtReal *)dor_space_allocate
				(needspace*(sizeof (DtReal)));
        space = needspace;
    }

    smallr = t->r;
    alpha_inc = TWOPI/divisions;
    beta_inc  = alpha_inc;
    alpha = 0;
    tricount = 0;
    if (cmptexuvwswi) {
        /* compute width and height of torus */
        totw = 2 * (t->r + t->R);
        toth = 2 * (t->r);
    }

    vptr = vertices;

    for ( i = 0; i < divisions; i++ ) {
   	sin_alpha =  dor_math_sin(alpha);
   	cos_alpha =  dor_math_cos(alpha);

	if (i == (divisions-1))  {
        /* avoid a crack in the model; make sure last vertex = first vertex */
   	    sin_alpha_plus_inc = 0.;
   	    cos_alpha_plus_inc = 1.;
	} else {
   	    sin_alpha_plus_inc = dor_math_sin(alpha+alpha_inc);
   	    cos_alpha_plus_inc = dor_math_cos(alpha+alpha_inc);
	}

   	center1[0] = t->R*sin_alpha;
   	center1[1] = 0;
   	center1[2] = -t->R*cos_alpha;

   	center2[0] = t->R*sin_alpha_plus_inc;
   	center2[1] = 0;
   	center2[2] = -t->R*cos_alpha_plus_inc;

   	beta = 0;
   	for ( j = 0; j < divisions; j++) {
	    sin_beta = dor_math_sin(beta);
	    cos_beta = dor_math_cos(beta);

	    if (j == (divisions-1)) {
            /* avoid a crack in the model; make sure last vertex = first vertex */
	       sin_beta_plus_inc = 0.;
	       cos_beta_plus_inc = 1.;
	    } else {
	       sin_beta_plus_inc = dor_math_sin(beta+beta_inc);
	       cos_beta_plus_inc = dor_math_cos(beta+beta_inc);
	    }
     
	    x = t->R+t->r*cos_beta;
	    x_inc = t->R+t->r*cos_beta_plus_inc;
     
	    quad[0][0] = sin_alpha*x;
	    quad[0][1] = t->r*sin_beta;
	    quad[0][2] = -cos_alpha*x;
	    dor_math_normal(center1[0], center1[1], center1[2],
			    quad[0][0],quad[0][1],quad[0][2],
			    &(quad[0][3]),&(quad[0][4]),&(quad[0][5]));
	    if (cmptexuvswi) {
		if (cmptexuvcbk == DcStdUVCallback) {
		    quad[0][6] = dor_torus_ucomp(quad[0][0],quad[0][2]);
		    quad[0][7] = dor_torus_vcomp(quad[0][1],smallr );
		} else /* use user-defined callback for uvs */ {
		    dor_torus_compuv_usercallback(cmptexuvcbk, uv_function, uv_dataobject,
	    			quad[0][0],quad[0][1],quad[0][2],
				quad[0][3],quad[0][4],quad[0][5],
				&(quad[0][6]), &(quad[0][7]));
		}
	    }
            if (cmptexuvwswi) {
		if (cmptexuvwcbk == DcStdUVWCallback) {
                    quad[0][8] = quad[0][0]/totw + 0.5;
                    quad[0][9] = .5 - quad[0][1]/toth;
                    quad[0][10] = .5 - quad[0][2]/totw;
		} else /* use user-defined callback for uvws */ {
		    dor_torus_compuvw_usercallback(cmptexuvwcbk, uvw_function, 
				uvw_dataobject, quad[0][0],quad[0][1],quad[0][2],
				quad[0][3],quad[0][4],quad[0][5],
				&(quad[0][8]), &(quad[0][9]), &(quad[0][10]));
		}
            }

	    quad[1][0] = sin_alpha*x_inc;
	    quad[1][1] = t->r*sin_beta_plus_inc;
	    quad[1][2] = -cos_alpha*x_inc;
	    dor_math_normal(center1[0], center1[1], center1[2],
			    quad[1][0],quad[1][1],quad[1][2],
			    &(quad[1][3]),&(quad[1][4]),&(quad[1][5]));
	    if (cmptexuvswi) {
		if (cmptexuvcbk == DcStdUVCallback) {
		    quad[1][6] = dor_torus_ucomp(quad[1][0],quad[1][2]);
		    quad[1][7] = dor_torus_vcomp(quad[1][1],smallr );
		} else /* use user-defined callback for uvs */ {
		    dor_torus_compuv_usercallback(cmptexuvcbk, uv_function, uv_dataobject,
	    			quad[1][0],quad[1][1],quad[1][2],
				quad[1][3],quad[1][4],quad[1][5],
				&(quad[1][6]), &(quad[1][7]));
		}
	    }
            if (cmptexuvwswi) {
		if (cmptexuvwcbk == DcStdUVWCallback) {
                    quad[1][8] = quad[1][0]/totw + 0.5;
                    quad[1][9] = .5 - quad[1][1]/toth;
                    quad[1][10] = .5 - quad[1][2]/totw;
		} else /* use user-defined callback for uvws */ {
		    dor_torus_compuvw_usercallback(cmptexuvwcbk, uvw_function, 
				uvw_dataobject, quad[1][0],quad[1][1],quad[1][2],
				quad[1][3],quad[1][4],quad[1][5],
				&(quad[1][8]), &(quad[1][9]), &(quad[1][10]));
		}
            }

	    quad[2][0] = sin_alpha_plus_inc*x_inc;
	    quad[2][1] = t->r*sin_beta_plus_inc;
	    quad[2][2] = -cos_alpha_plus_inc*x_inc;
	    dor_math_normal(center2[0], center2[1], center2[2],
			    quad[2][0],quad[2][1],quad[2][2],
			    &(quad[2][3]),&(quad[2][4]),&(quad[2][5]));
	    if (cmptexuvswi) {
		if (cmptexuvcbk == DcStdUVCallback) {
	            if (i == (divisions-1))  
                        quad[2][6] = 0.0;
                    else
		        quad[2][6] = dor_torus_ucomp(quad[2][0],quad[2][2]);
		    quad[2][7] = dor_torus_vcomp(quad[2][1],smallr );
		} else /* use user-defined callback for uvs */ {
		    dor_torus_compuv_usercallback(cmptexuvcbk, uv_function, uv_dataobject,
	    			quad[2][0],quad[2][1],quad[2][2],
				quad[2][3],quad[2][4],quad[2][5],
				&(quad[2][6]), &(quad[2][7]));
		}
	    }
            if (cmptexuvwswi) {
		if (cmptexuvwcbk == DcStdUVWCallback) {
                    quad[2][8] = quad[2][0]/totw + 0.5;
                    quad[2][9] = .5 - quad[2][1]/toth;
                    quad[2][10] = .5 - quad[2][2]/totw;
		} else /* use user-defined callback for uvws */ {
		    dor_torus_compuvw_usercallback(cmptexuvwcbk, uvw_function, 
				uvw_dataobject, quad[2][0],quad[2][1],quad[2][2],
				quad[2][3],quad[2][4],quad[2][5],
				&(quad[2][8]), &(quad[2][9]), &(quad[2][10]));
		}
            }

	    quad[3][0] = sin_alpha_plus_inc*x;
	    quad[3][1] = t->r*sin_beta;
	    quad[3][2] = -cos_alpha_plus_inc*x;
	    dor_math_normal(center2[0], center2[1], center2[2],
			    quad[3][0],quad[3][1],quad[3][2],
			    &(quad[3][3]),&(quad[3][4]),&(quad[3][5]));
	    if (cmptexuvswi) {
		if (cmptexuvcbk == DcStdUVCallback) {
	            if (i == (divisions-1))  
                        quad[3][6] = 0.0;
                    else
		        quad[3][6] = dor_torus_ucomp(quad[3][0],quad[3][2]);
		    quad[3][7] = dor_torus_vcomp(quad[3][1],smallr );
		} else /* use user-defined callback */ {
		    dor_torus_compuv_usercallback(cmptexuvcbk, uv_function, uv_dataobject,
	    			quad[3][0],quad[3][1],quad[3][2],
				quad[3][3],quad[3][4],quad[3][5],
				&(quad[3][6]), &(quad[3][7]));
		}
	    }
            if (cmptexuvwswi) {
		if (cmptexuvwcbk == DcStdUVWCallback) {
                    quad[3][8] = quad[3][0]/totw + 0.5;
                    quad[3][9] = .5 - quad[3][1]/toth;
                    quad[3][10] = .5 - quad[3][2]/totw;
		} else /* use user-defined callback for uvws */ {
		    dor_torus_compuvw_usercallback(cmptexuvwcbk, uvw_function, 
				uvw_dataobject, quad[3][0],quad[3][1],quad[3][2],
				quad[3][3],quad[3][4],quad[3][5],
				&(quad[3][8]), &(quad[3][9]), &(quad[3][10]));
		}
            }


	    /* make two triangles out of the quad */
            /* 3 vertex per triangle x 2 triangles = 6 */
	    for (k=0; k<6; k++) {
		/* quad used for the next triangle vertex */
		qd = trivertindex[k];

		for (v=0; v<6; v++)
                     /* store xyz and normal */
                     *vptr++ = quad[qd][v];

                if (cmptexuvswi) {
                     *vptr++ = quad[qd][6];
                     *vptr++ = quad[qd][7];
                }
                if (cmptexuvwswi) {
                     *vptr++ = quad[qd][8];
                     *vptr++ = quad[qd][9];
                     *vptr++ = quad[qd][10];
                }
	    }

	    beta += beta_inc;
	    tricount += 2;
   	}
   	
   	alpha += alpha_inc;
    }


    trilstobj = dor_trilst_create(DcRGB, vertextype, tricount, vertices);

    return(trilstobj);
}
/*
 ======================================================================
 */
DtReal dor_torus_ucomp (
    DtReal x,
    DtReal z)
{
    if (x==0 && z==0) return 0.0;

    return (dor_math_atan2(x,z)/TWOPI + .5);
}


/*
 ======================================================================
 */
DtReal dor_torus_vcomp (
    DtReal y,
    DtReal radius)
{
    return (dor_math_acos(y/radius)/M_PI);
}


/*
 ======================================================================
 */
void dor_torus_compuv_usercallback (
    DtObject callbackobj,
    DtPFI function,
    DtObject dataobject,
    DtReal x,
    DtReal y,
    DtReal z,
    DtReal nx,
    DtReal ny,
    DtReal nz,
    DtReal *u,
    DtReal *v)
{
    DtPtr datptr;
    Dt32Bits datval;
    DtInt type;
    DtInt dataptr_class_id = dor_class_inq_id("DoDataPtr");
    DtInt dataval_class_id = dor_class_inq_id("DoDataVal");

    if (dataobject == DcNullObject) {
	datval = 0;
	if (dor_calbak_get_pass_by_ref(callbackobj)){
	    (*function)(&datval, &x,&y,&z,  &nx,&ny,&nz,  u, v);
	} else {
	    (*function)(datval, x,y,z,  nx,ny,nz,  u, v);
	}
    } else if ((DcObjectValid == dor_object_validate(dataobject)) &&
	      (((type = dor_object_inq_type(dataobject)) == dataptr_class_id) || 
	      (type == dataval_class_id))) {
	if (type == dataptr_class_id) {
            datptr = dor_datptr_get_value(dataobject);
            if (dor_calbak_get_pass_by_ref(callbackobj)) {
                (*function)(&datptr, &x,&y,&z,  &nx,&ny,&nz,  u, v);
            } else {
                (*function)(datptr, x,y,z,  nx,ny,nz,  u, v);
            }
        } else if (type == dataval_class_id) {
            datval = dor_datval_get_value(dataobject);
            if (dor_calbak_get_pass_by_ref(callbackobj)) {
                (*function)(&datval, &x,&y,&z,  &nx,&ny,&nz,  u, v);
            } else {
                (*function)(datval, x,y,z,  nx,ny,nz,  u, v);
            }
        }
    }
}
/*
 ======================================================================
 */
void dor_torus_compuvw_usercallback (
    DtObject callbackobj,
    DtPFI function,
    DtObject dataobject,
    DtReal x,
    DtReal y,
    DtReal z,
    DtReal nx,
    DtReal ny,
    DtReal nz,
    DtReal *u,
    DtReal *v,
    DtReal *w)
{
    DtPtr datptr;
    Dt32Bits datval;
    DtInt type;
    DtInt dataptr_class_id = dor_class_inq_id("DoDataPtr");
    DtInt dataval_class_id = dor_class_inq_id("DoDataVal");

    if (dataobject == DcNullObject) {
	datval = 0;
	if (dor_calbak_get_pass_by_ref(callbackobj)){
	    (*function)(&datval, &x,&y,&z,  &nx,&ny,&nz,  u,v,w);
	} else {
	    (*function)(datval, x,y,z,  nx,ny,nz,  u,v,w);
	}
    } else if ((DcObjectValid == dor_object_validate(dataobject)) &&
	      (((type = dor_object_inq_type(dataobject)) == dataptr_class_id) || 
	      (type == dataval_class_id))) {
	if (type == dataptr_class_id) {
            datptr = dor_datptr_get_value(dataobject);
            if (dor_calbak_get_pass_by_ref(callbackobj)) {
                (*function)(&datptr, &x,&y,&z,  &nx,&ny,&nz,  u,v,w);
            } else {
                (*function)(datptr, x,y,z,  nx,ny,nz,  u,v,w);
            }
        } else if (type == dataval_class_id) {
            datval = dor_datval_get_value(dataobject);
            if (dor_calbak_get_pass_by_ref(callbackobj)) {
                (*function)(&datval, &x,&y,&z,  &nx,&ny,&nz,  u,v,w);
            } else {
                (*function)(datval, x,y,z,  nx,ny,nz,  u,v,w);
            }
        }
    }
}
