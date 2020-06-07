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
	dor_sphlst_initialize
	dor_sphlst_create
	dor_sphlst_destroy
	dor_sphlst_print
	dor_sphlst_update_alternate
	dor_sphlst_pick
	dor_sphlst_render_standard
	dor_sphlst_create_alternate_geometry
	dor_sphlst_gen_octahedra
	dor_sphlst_sphere_point
	dor_sphlst_gen_sphere
	dor_sphlst_check_subdiv
	dor_sphlst_compute_bounding_volume

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/prisrf.h>
#include <dore/dore_develop/private/sphlst.h>
#include <math.h>

#define ATOR(a)  (((a)/180.0)*M_PI)

DtInt doe_sphlst_class_id;
DtFlag doe_sphlst_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry sphlst_methods[] = {
    {DcMethodDestroy,          (DtMethodPtr)dor_sphlst_destroy},
    {DcMethodStdRenderDisplay, (DtMethodPtr)dor_sphlst_render_standard},
    {DcMethodCmpBndVolume,     (DtMethodPtr)dor_sphlst_compute_bounding_volume},
    {DcMethodPrint,            (DtMethodPtr)dor_sphlst_print},
    {DcMethodPick,             (DtMethodPtr)dor_sphlst_pick},
    {DcMethodUpdStdAltObj,     (DtMethodPtr)dor_sphlst_update_alternate}
};

void dor_sphlst_initialize (void)
{
    doe_sphlst_class_id = dor_class_add("DoSphereList",6,sphlst_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_sphlst_create (
    DtColorModel colormodel,
    DtInt spherecount,
    DtReal *locations,
    DtReal *radii,
    DtReal *colors)
{
    struct sphlst *sphlst;
    DtInt sphereno;
    dot_object *sphlstobject;

    if (!doe_sphlst_initialized) {
    dor_sphlst_initialize();
    doe_sphlst_initialized = DcTrue;
    }

    sphlst = (struct sphlst *)dor_space_allocate(sizeof *sphlst);

    sphlst->sphere_count = spherecount;

    sphlst->locations = (DtReal *)dor_space_allocate(
			3 * spherecount*(sizeof (DtReal)));

    for (sphereno=0;sphereno<spherecount*3;sphereno+=3){
        sphlst->locations[sphereno] = locations[sphereno];
	sphlst->locations[sphereno+1] = locations[sphereno+1];
	sphlst->locations[sphereno+2] = locations[sphereno+2];
    }

    if (radii != NULL){
	sphlst->radii = (DtReal *)dor_space_allocate(
			spherecount*(sizeof (DtReal)));

	for (sphereno=0;sphereno<spherecount;sphereno++){
	    sphlst->radii[sphereno] = radii[sphereno];
	}
    }
    else
	sphlst->radii = NULL;

    if (colors != NULL){
        sphlst->colors = (DtReal *)dor_space_allocate(
			3 * spherecount*(sizeof (DtReal)));

	for (sphereno=0;sphereno<spherecount*3;sphereno+=3){
	    sphlst->colors[sphereno] = colors[sphereno];
	    sphlst->colors[sphereno+1] = colors[sphereno+1];
	    sphlst->colors[sphereno+2] = colors[sphereno+2];
	}
    }
    else
	sphlst->colors = NULL;
	
    sphlst->alternate_object = DcNullPtr;

    sphlstobject = dor_object_create(doe_sphlst_class_id,sphlst);

    return(sphlstobject);
}
/*
 ======================================================================
 */

void dor_sphlst_destroy (dot_object *object)
{
    struct sphlst *sphlst;

    sphlst = (struct sphlst *)(object->data);

    if (sphlst->alternate_object != NULL){
        dor_object_check_deletion(sphlst->alternate_object);
    }

    if (sphlst->locations != NULL){
        dor_space_deallocate(sphlst->locations);
    }

    if (sphlst->radii != NULL){
        dor_space_deallocate(sphlst->radii);
    }

    if (sphlst->colors != NULL){
        dor_space_deallocate(sphlst->colors);
    }

    dor_space_deallocate(sphlst);
}
/*
 ======================================================================
 */

void dor_sphlst_print (dot_object *object)
{
    struct sphlst *sphlst;

    sphlst = (struct sphlst *)(object->data);
    
    if (sphlst == NULL) {
        dor_print_output("sphere list is NULL");
        return;
    }

    sprintf(dor_print_get_line(), "Sphere List at [%lx]\n", object);
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(), "# of spheres = %d\n",sphlst->sphere_count);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

dot_object *dor_sphlst_update_alternate (dot_object *object)
{
    struct sphlst *sphlst;
    DtFlag changed;
    dot_object *alt;

    sphlst = (struct sphlst *)(object->data);

    changed = dor_sphlst_check_subdiv(sphlst);

    if ((sphlst->alternate_object != DcNullPtr) && !changed) {
	alt = sphlst->alternate_object;

    } else {
	if (sphlst->alternate_object != DcNullPtr) {
            dor_object_get_method(sphlst->alternate_object,
                               DcMethodDestroy)(sphlst->alternate_object);

            sphlst->alternate_object = DcNullPtr;
        }

	sphlst->alternate_object = dor_sphlst_create_alternate_geometry(sphlst);
        alt = sphlst->alternate_object;

        if (sphlst->alternate_object == DcNullPtr) {
            DDerror(ERR_CANT_CREATE_ALTOBJ, "dor_sphlst_update_alternate", "");
	}
    }
    return (alt);
}	
/*
 ======================================================================
 */

void dor_sphlst_pick (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_pick(doe_sphlst_class_id))
        return;

    dor_pick_initialize_object(object);

    alt = dor_sphlst_update_alternate(object);
    if (alt != DcNullPtr) {
        doe_system_alternate_object_execution_flag = DcTrue;

    dor_object_get_current_method(alt)(alt);

    doe_system_alternate_object_execution_flag = DcFalse;
    }
}
/*
 ======================================================================
 */

void dor_sphlst_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_sphlst_class_id))
        return;

    alt = dor_sphlst_update_alternate(object);
    if (alt != DcNullPtr) {
        dor_object_get_current_method(alt)(alt);
    }
}
/*
 ======================================================================
 */

/* 
 * Sphere list representation using subdivision.
*/

extern DtReal *h1pts;
extern DtInt *h1cl;
extern DtInt npnts, ntris;

dot_object *dor_sphlst_create_alternate_geometry (struct sphlst *sphlst)
{
    DtInt number_spheres;
    DtReal *locations;
    DtReal *radii;
    DtReal *colors;
    DtInt i, j, k;
    DtInt divs;  
    DtObject trimshobj;
    DtReal subdivlevel;
    DtInt subdivtype;
    DtReal *vertices;
    DtIntTriple *sphtris;
    DtInt sphere_num,verts_per_sphere,triangles_per_sphere;
    DtInt total_verts,total_triangles;
    DtInt voff,sphoff;
    DtSwitch cmptexuvswi, cmptexuvwswi;
    DtVertexType vertextype;
    DtInt vertexsize;
    DtReal x, y, z;
    DtReal uval[3], diff;
    DtInt tri[3], modify[3];
    DtInt addvertices, endofvector, uoffset;
    DtReal morethanhalf = 0.51;
    dot_object *uv_dataobject, *uvw_dataobject;
    DtPFI uv_function, uvw_function;
    dot_object *cmptexuvcbk, *cmptexuvwcbk;

    number_spheres = sphlst->sphere_count;
    locations = sphlst->locations;
    radii = sphlst->radii;
    colors = sphlst->colors;

    dor_global_subdivspc_get_value(&subdivtype,&subdivlevel);

    if (subdivtype != DcSubDivSegments)
    	divs = 2 + (subdivlevel*10.);
    else
    	divs = subdivlevel/4.;

    if (divs <= 0){
	divs = 1;  /* must have at least 1! (or else segment fault) */
    }

    if (colors == NULL) {
        vertextype = DcLocNrm;
        vertexsize = 6;
    } else {
	vertextype = DcLocNrmClr;
        vertexsize = 9;
    }
    addvertices = 0;
    dor_global_cmptexuvswi_get_value(&cmptexuvswi);
    if (cmptexuvswi) {
        vertexsize += 2;
        vertextype |= dor_system_texture_uv_count(1);
        dor_global_cmptexuvcbk_get_value(&cmptexuvcbk);
        if (cmptexuvcbk != DcStdUVCallback) {
            uv_function = dor_calbak_get_function(cmptexuvcbk);
            uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);
        }
        /* compute how many vertices we'll need to add when we fix
           the vertex list to make sure the uv's are ok at the seam */
        addvertices = ((divs==1)?1:divs-1)*6+2;
    }

    dor_global_cmptexuvwswi_get_value(&cmptexuvwswi);
    if (cmptexuvwswi) {
        vertexsize += 3;
        vertextype |= dor_system_texture_uvw_count(1);
        dor_global_cmptexuvwcbk_get_value(&cmptexuvwcbk);
        if (cmptexuvwcbk != DcStdUVWCallback) {
            uvw_function = dor_calbak_get_function(cmptexuvwcbk);
            uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);
        }
    }

    /* now generate the vertices and connection list. */
    dor_sphlst_gen_sphere(divs);  

    verts_per_sphere = npnts;
    triangles_per_sphere = ntris;


    /*Allocate the vertices array */
    vertices = (DtReal *)dor_space_allocate((sizeof(DtReal)) * vertexsize *
                (verts_per_sphere+addvertices) * number_spheres);

    /* Allocate the connections array */
    sphtris = (DtIntTriple *)dor_space_allocate(
    		triangles_per_sphere*(sizeof(DtIntTriple)) * number_spheres);

    voff = 0;
    sphoff = 0;
    for (sphere_num = 0; sphere_num < number_spheres; sphere_num++){
	for (i=0; i <verts_per_sphere; i++){
            /* scale & translate points by radii & location */
            x = h1pts[i*3];
            y = h1pts[i*3+1];
            z = h1pts[i*3+2];
            if (radii == NULL) {
                vertices[voff++] = x + locations[sphere_num*3+0];
		vertices[voff++] = y + locations[sphere_num*3+1];
                vertices[voff++] = z + locations[sphere_num*3+2];
            } else {
                vertices[voff++] = x*radii[sphere_num] 
					+ locations[sphere_num*3+0];
		vertices[voff++] = y*radii[sphere_num] 
					+ locations[sphere_num*3+1];
		vertices[voff++] = z*radii[sphere_num] 
					+ locations[sphere_num*3+2];
            }
            /* copy the normal information */
            vertices[voff++] = x;
            vertices[voff++] = y;
            vertices[voff++] = z;
               
            if (colors != NULL){
                /* copy the color information */
                vertices[voff++] = colors[sphere_num*3];
                vertices[voff++] = colors[sphere_num*3+1];
                vertices[voff++] = colors[sphere_num*3+2];
            }

            if (cmptexuvswi) {
                if (cmptexuvcbk == DcStdUVCallback) {
		    vertices[voff++] = dor_prisrf_sphere_ucomp(y, x);
		    vertices[voff++] = dor_prisrf_sphere_vcomp(z); 
		} else {
		    dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
		          uv_dataobject, x, y, z,  x, y, z,
			  &(vertices[voff]), &(vertices[voff+1]));
		    voff+=2;
                }
            }
            if (cmptexuvwswi) {
		if (cmptexuvwcbk == DcStdUVWCallback) {
                    vertices[voff++] = x/2. + .5;
                    vertices[voff++] = .5 - y/2.;
                    vertices[voff++] = .5 - z/2.;
		} else {
		    dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
			uvw_dataobject, x, y, z, x, y, z, &(vertices[voff]),
			&(vertices[voff+1]), &(vertices[voff+2]));
		    voff+=3;
		}
            }
        }

        /* now copy the connection information, with offset,
	   for this sphere */
        for (i=0; i<triangles_per_sphere; i++){
            sphtris[sphoff][0] = h1cl[i*3] + 
                    sphere_num * verts_per_sphere;
            sphtris[sphoff][1] = h1cl[i*3+1] + 
                    sphere_num * verts_per_sphere;
            sphtris[sphoff][2] = h1cl[i*3+2] + 
                    sphere_num * verts_per_sphere;
            sphoff++;
        }
    }

    total_verts = number_spheres * verts_per_sphere;
    total_triangles = number_spheres * triangles_per_sphere;

    if (cmptexuvswi && (cmptexuvcbk == DcStdUVCallback)) {
        /* At this point, the triangles on BOTH sides of the seam have some
           vertices with u=1.0. Here we spot the triangles which should have
           u=0.0 instead, add these new vertices to the list and update the
           contour list. */
           
        if (colors == NULL)
            uoffset = 6;
        else
            uoffset = 9;
	endofvector = total_verts; 
	for (i=0; i<total_triangles; i++) {
            for (j=0; j<3; j++) {
                tri[j] = sphtris[i][j];
                uval[j] = vertices[tri[j]*vertexsize+uoffset];
                modify[j] = DcFalse;
            }
            /* a difference in u greater than .5 for any two vertices
               indicates that we have a triangle which u's need to be fixed */
            diff = uval[0]-uval[1];
            if (dor_math_fabs(diff) > morethanhalf)
                if (diff > 0)
                    modify[0] = DcTrue;
                else
                    modify[1] = DcTrue;
            diff = uval[1]-uval[2];
            if (dor_math_fabs(diff) > morethanhalf)
                if (diff > 0)
                    modify[1] = DcTrue;
                else
                    modify[2] = DcTrue;
            diff = uval[2]-uval[0];
            if (dor_math_fabs(diff) > morethanhalf)
                if (diff > 0)
                    modify[2] = DcTrue;
                else
                    modify[0] = DcTrue;
             for (k=0; k<3; k++) {
                 if ((modify[k] == DcTrue) && (uval[k] >= 1.000)) {
                 /* copy vertex values to end of array */
                     for (j=0; j<vertexsize; j++) {
                         vertices[endofvector*vertexsize+j] =
                              vertices[tri[k]*vertexsize+j];
                     }
                     /* overwrite old u value (1.0) with new (0.0) */
                     vertices[endofvector*vertexsize+uoffset] = 0.0;
                     sphtris[i][k] = endofvector;
                     endofvector++;
                }
            }
			
        }
    total_verts = endofvector;
    }
		
    trimshobj = DoTriangleMesh(DcRGB,vertextype, total_verts, vertices, 
                           total_triangles, (DtInt *)sphtris, DcFalse);
    dor_space_deallocate(vertices);
    dor_space_deallocate(sphtris);

    return(trimshobj);
}


/*****************************************************************************/

DtReal *h1pts;
DtReal *h2pts;
DtInt *h1cl;
DtInt *h2cl;
DtInt ntris, npnts;
static DtInt number_of_triangles = 0;

/* vertices and normals for an octhedra...
 * the normals are the same as the vertex points.
*/

DtReal octahedra_pts[] = {
	1.,0.,0.,
	0.,1.,0.,
	-1.,0.,0.,
	0.,-1.,0.,
	0.,0.,-1.,
	0.,0.,1.
};

DtInt octahedra_cl[] = {
	0,4,1,
	4,2,1,
	2,5,1,
	5,0,1,
	4,0,3,
	2,4,3,
	5,2,3,
	0,5,3
};


void dor_sphlst_gen_octahedra (void)
{
    DtInt i;

    npnts = 6;
    ntris = 8;
    for (i = 0; i < npnts * 3; i++)
	h1pts[i] = octahedra_pts[i];
    for (i = 0; i < ntris*3; i++)
	h1cl[i] = octahedra_cl[i];
}


/*****************************************************************************/
void dor_sphlst_sphere_point (
    DtReal a1,
    DtReal a2,
    DtInt  pi)
{
    h1pts[pi  ] = sin((double)ATOR(a2))*cos((double)ATOR(a1));
    h1pts[pi+1] = sin((double)ATOR(a2))*sin((double)ATOR(a1));
    h1pts[pi+2] = cos((double)ATOR(a2));
}

/*****************************************************************************/
void dor_sphlst_gen_sphere (DtInt step)
{
    DtReal a1,a2;
    DtInt *tcl;
    DtInt q,i,ii,oi;
    DtInt olc,oli,ili;
    DtInt pi;
    DtInt first;
    DtInt numtris;

    numtris = step * step * 8;

    /* need more space for the triangle points? */
    if (numtris > number_of_triangles){
        if (number_of_triangles != 0)  /* not the first time through here? */{
    	    dor_space_deallocate(h1pts);
    	    dor_space_deallocate(h1cl);
        }
    	h1pts = (DtReal *) dor_space_allocate(numtris*sizeof(DtReal)*3);
    	h1cl = (DtInt *) dor_space_allocate(numtris*sizeof(DtInt)*3);
    	number_of_triangles = numtris;
    }

    if (step == 1)  /* generate an octhedra if steps is 1 */{
        dor_sphlst_gen_octahedra();
    	return;
    }

    ntris = 0;
    npnts = 1;
    tcl = &h1cl[0];
    first = 1;
    olc = 4;
    ili = 0;
    oli = 1;
    pi = 0;
    dor_sphlst_sphere_point( 0.0, 180.0, pi);
    pi += 3;
    for (a2 = (180.0 - (90.0/(double)step)); a2 >= 89.9; 
        a2 -= (90.0/(double)step)) {
    	for (i = 0, a1 = 0.0; i < olc; i++, a1 += 360.0/(double)olc) {
    	    dor_sphlst_sphere_point( a1, a2, pi);
    	    pi += 3;
    	    npnts ++;
    	}
    	if (first == 1) {
    	    *tcl++ = (2); *tcl++ = (1); *tcl++ = (0);
    	    *tcl++ = (3); *tcl++ = (2); *tcl++ = (0);
    	    *tcl++ = (4); *tcl++ = (3); *tcl++ = (0);
    	    *tcl++ = (1); *tcl++ = (4); *tcl++ = (0);
    	    ntris += 4;
    	    first = 0;
    	}
    	else {
    	    for (ii = 0, oi = 0, q = 0; q < 4; q++, ii += ((olc/4)-1), oi += (olc/4)) {
    		*tcl++ = (oli+oi+1);
    		*tcl++ = (oli+oi);
    		*tcl++ = (ili+ii);
    		ntris++;
    		for (i = 0; i < ((olc/4)-1); i++) {
    		    if ((ili+ii+i+1) != (oli)) {
    			*tcl++ = (ili+ii+i+1);
    			*tcl++ = (oli+oi+i+1);
    			*tcl++ = (ili+ii+i);

    			*tcl++ = (oli+oi+i+2);
    			*tcl++ = (oli+oi+i+1);
    			*tcl++ = (ili+ii+i+1);
    		    }
    		    else {
    			*tcl++ = (ili);
    			*tcl++ = (oli+oi+i+1);
    			*tcl++ = (ili+ii+i);

    			*tcl++ = (oli);
			*tcl++ = (oli+oi+i+1);
			*tcl++ = (ili);
		    }
		    ntris += 2;
		}
	    }
	}
	ili = oli;
	oli += olc;
	olc += 4;
    }

    /* now copy the hemisphere onto the lower half. */
    /* the total result is stored in h1pts and h1cl. */
    h2pts = &h1pts[3*npnts];
    h2cl = &h1cl[3*ntris];

    for (i = 0; i < (3 * npnts); i += 3) {
	h2pts[i  ] = h1pts[i  ];
	h2pts[i+1] = h1pts[i+1];
	h2pts[i+2] = -1.0 * h1pts[i+2];
    }
    for (i = 0; i < (3 * ntris); i += 3) {
	h2cl[i  ] = h1cl[i+2] + npnts;
	h2cl[i+1] = h1cl[i+1] + npnts;
	h2cl[i+2] = h1cl[i  ] + npnts;
    }
    ntris *= 2;
    npnts *= 2;
}

/*
 ======================================================================
 */

DtFlag dor_sphlst_check_subdiv (struct sphlst *sphlst)
{
    DtFlag returnflag;
    DtInt subdivtype;
    DtReal subdivparms[1];

    returnflag = DcFalse;

    dor_global_subdivspc_get_value(&subdivtype, subdivparms);

    if (sphlst->subdivtype != subdivtype) {
    	sphlst->subdivtype = subdivtype;
    	returnflag = DcTrue;
    }

    if (sphlst->subdivparms[0] != subdivparms[0]) {
    	sphlst->subdivparms[0] = subdivparms[0];
    	returnflag = DcTrue;
    }

    return (returnflag);
}
/*
 ======================================================================
 */

void dor_sphlst_compute_bounding_volume (dot_object *object)
{
    struct sphlst *sphlst;

    sphlst = (struct sphlst *)(object->data);

    dor_system_report_bounding_points(sphlst->sphere_count,sphlst->locations);
}
