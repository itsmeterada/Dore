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
        ddt_vll_drom_torus * ddr_vll_drom_torus_create
        int ddr_vll_drom_torus_print (object, data)
        int ddr_vll_drom_torus_delete (object, data)
	int ddr_vll_drom_torus_render(object)
	dot_object *ddr_vll_drom_torus_update_alternate(object)
	dot_object *ddr_vll_drom_torus_create_alternate_geometry(t)

  ======================================================================
 */

#include "drom.h"
#include "dore/internal/dogen.h"
#include "dore/dore_develop/private/torus.h"
#include <math.h>

/*
 ======================================================================
 */
typedef struct {
    DtObjectStructure *quad_altobj;
    DtInt quad_subdivtype;
    DtReal quad_subdivlevel[1];
} ddt_vll_drom_torus;

static DtInt classid = -1;
static DtInt dataindex = -1;

/*
 ======================================================================
 */
ddt_vll_drom_torus *
ddr_vll_drom_torus_create(object)
        DtObjectStructure *object;
{
    ddt_vll_drom_torus *dynout_torus;

    dynout_torus =  (ddt_vll_drom_torus *)
            DDspace_Allocate(sizeof(ddt_vll_drom_torus));

    dynout_torus->quad_altobj = (DtObjectStructure *)0;

    dynout_torus->quad_subdivtype = -1;

    return dynout_torus;
}

/*
  ======================================================================
 */

ddr_vll_drom_torus_print(object, data)
        DtObjectStructure *object;
        ddt_vll_drom_torus *data;
{
}

/*
  ======================================================================
 */

ddr_vll_drom_torus_delete(object, data)
        DtObjectStructure *object;
        ddt_vll_drom_torus *data;
{
    if (data->quad_altobj != (DtObjectStructure *)0) {
        DeDeleteObject(data->quad_altobj);
    }
    DDspace_Deallocate(data);
}

/*
 ======================================================================
 */

int ddr_vll_drom_torus_render(object)
dot_object *object;
{
    dot_object *ddr_vll_drom_torus_update_alternate();
    dot_object *alt;
    dot_pfi dor_object_get_current_method();

    if (classid == -1)
        classid = DsInqClassId("DoTorus");

    if (!dor_condex_query_render(classid))
	    return;

    alt = ddr_vll_drom_torus_update_alternate (object);
    if (alt != dod_object_null) {
	dor_object_get_current_method(alt) (alt);
    }
}
/*
 ======================================================================
 */

dot_object *ddr_vll_drom_torus_update_alternate(object)
dot_object *object;
{
    struct torus *torus;
    DtFlag changed;
    dot_object *ddr_vll_drom_torus_create_alternate_geometry();
    dot_pfi dor_object_get_method();
    DtFlag dor_torus_check_subdiv();
    dot_object *alt;
    DtInt i, quad_subdivtype;
    DtReal quad_subdivlevel[1];
    DtPFI routine_subdiv = DcNullPtr;
    ddt_vll_drom_torus *dynout_torus;

    torus = (struct torus *)(object->data);

    if (dataindex == -1) {
        for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
            if (dde_vll_drom_class_data[i].id == classid) {
                dataindex = dde_vll_drom_class_data[i].index;
                break;
            }
        }
    }

    dynout_torus = (ddt_vll_drom_torus *)object->additional_data[dataindex];

    /* check if subdivision spec has changed since last update of the quad
       alternate representation. Note that the subdivision of the
       stored quad and triangle alt. representations may differ */

    if (routine_subdiv == (DtPFI)DcNullPtr) {
      routine_subdiv = DDclass_InqMethod(DsInqClassId("AttSubDivSpec"),
                                 DsInqMethodId("InqGlobalValue"));
    }
    (*routine_subdiv) (&quad_subdivtype, quad_subdivlevel);

    if (dynout_torus->quad_subdivtype != quad_subdivtype) {
         dynout_torus->quad_subdivtype = quad_subdivtype;
         changed = DcTrue;
    }

    if (dynout_torus->quad_subdivlevel[0] != quad_subdivlevel[0]) {
         dynout_torus->quad_subdivlevel[0] = quad_subdivlevel[0];
         changed = DcTrue;
    }


    if ((dynout_torus->quad_altobj != dod_object_null) && !changed) {
	alt = dynout_torus->quad_altobj;

    } else {
	if (dynout_torus->quad_altobj != dod_object_null) {
	    dor_object_get_method(dynout_torus->quad_altobj,
				  DcMethodDestroy)(dynout_torus->quad_altobj);

	    dynout_torus->quad_altobj = dod_object_null;
	}

	dynout_torus->quad_altobj = 
		ddr_vll_drom_torus_create_alternate_geometry(torus);
	alt = dynout_torus->quad_altobj;

	if (dynout_torus->quad_altobj == dod_object_null) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "ddr_vll_drom_torus_update_alternate", "");
	}
    }
    return alt;
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

dot_object *ddr_vll_drom_torus_create_alternate_geometry(t)
struct torus *t;
{
    DtInt subdivtype;
    DtReal subdivlevel[1];
    DtSwitch cmptexuvswi;
    DtSwitch cmptexuvwswi;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    static DtInt space=0;
    static DtReal *vertices;
    DtReal *vptr;
    DtInt needspace;
    DtInt vtxsize;
    DtInt divisions;
    DtInt quadcount;
    DtReal quad[4][11];
    DtReal dor_math_cos();
    DtReal dor_math_sin();
    DtInt i, j, v, qd;
    DtReal alpha, beta;
    DtReal alpha_inc, beta_inc;
    DtReal cos_alpha, sin_alpha, cos_alpha_plus_inc, sin_alpha_plus_inc;
    DtReal cos_beta,  sin_beta,  cos_beta_plus_inc,  sin_beta_plus_inc;
    DtReal x, x_inc;
    DtReal center1[3], center2[3];
    dot_object *quadlstobj;
    dot_object *dor_quadlst_create();
    DtReal dor_torus_ucomp(), dor_torus_vcomp();
    DtReal smallr;
    DtVertexType vertextype;
    DtReal totw, toth;
    dot_pfi uv_function;
    dot_pfi uvw_function;
    dot_pfi dor_calbak_get_function();
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;
    dot_object *dor_calbak_get_dataobject();

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
    quadcount = 0;
    if (cmptexuvwswi) {
        /* compute total width and height of torus */
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

            for (qd=0; qd<4; qd++)  {
                /* store xyz and normal */
	        for (v=0; v<6; v++)
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
	    quadcount ++;
   	}
   	
   	alpha += alpha_inc;
    }

    quadlstobj = dor_quadlst_create(DcRGB, vertextype, quadcount, vertices);
					
    return(quadlstobj);
}

/*
 ======================================================================
 */
