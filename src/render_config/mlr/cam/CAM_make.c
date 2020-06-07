/* vi:set ts=8 sw=4 ai:
 *[
 *$     CAM_make.c
 *
 *      Steve Gray - Mon Jul 17 17:12:20 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	set up camera info
 *
 *      Usage:
 *		CAM_make()		camera method routine
 *		CAM_matrix()		set the camera matrix
 *		CAM_perspective()	set the perspective projection info
 *		CAM_projection()	set the projection type
 *
 *		CAM_get_matrix()	get viewing matrix
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *	01/17/90	fixed ortho camera size				gray
 *
 *]
 */


/* #define	DEBUG */

/* system includes */
#include <stdio.h>
#include <math.h>

/* dore includes */
#include "dore.h"
#include "dore/dore_develop/private/permat.h"
#include "dore/dore_develop/private/prjmat.h"
#include "dore/dore_develop/private/parmat.h"

/* Render includes */
#include "primitives.h"

/* MLR Dore interface includes */
#include "global.h"
#include "cam.h"


/* current world coordinate to pixel coordinate matrix */
Matrix_4	cam_current_wcs_to_pcs;


/*[
 *$     CAM_make
 *
 *      Steve Gray - Mon Jul 17 17:14:11 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    camera method routine
 *
 *      Usage:
 *		CAM_make()
 *
 *      Notes:  
 *		write the camera parameters to the camera file
 *
 *?
 *!     Changes History
 *
 *	01/17/90	fixed ortho camera size				gray
 *
 *]
 */


CAM_make( object)
DtObject	object;
{
Float		focal_lth, width;
DtObject 	dore_cam;
DtPFI 		routine;
DtMatrix4x4	lcs_matrix, matrix;
DtInt		res_x, res_y;
DtVolume	*view_port;
DtNColor3	bkg_color;
DtInt		ray_depth;
DtSwitch	aa_switch;
DtReal		xsamp, ysamp;
DtObject	filter;
Matrix_4	mlr_matrix;
Int		i, j, k;



/* no counting to be done here */
    if ( mlr_mode == MLR_MODE_DIRECT_COUNT )
    {
	return;
    }
   
    dore_cam = DvInqActiveCamera (mlr_control.view);

    if (dore_cam == DcNullObject || dore_cam == object) 
    {
	if ( mlr_mode == MLR_MODE_FILE ) {
             /* close and reopen camera file. This is to avoid having several
                "res", "scam", "outfile", etc.. definitions in the case where several
                calls to DoCamera are made in the user program  */
             fclose( mlr_files.camera_fp);
             if ((mlr_files.camera_fp = fopen( mlr_files.camera_file, "w")) == NULL)
                 {
                 printf( "file name: %s\n", mlr_files.camera_file);
                 DDerror( ERR_FATAL_INTERNAL_ERROR, "CAM_make",
                        "error creating camera control file");
                 }
	}

        routine = DDclass_InqMethod ( DcTypeAttLcstowcsmat,
				       DcMethodInqGlbAttVal);

        (*routine)( lcs_matrix);

/* pre-concatinate right_to_left_mat with lcs_matrix, note that this turns
 * normals inside out, which is why the "reverse on" appears in the geom
 * file
 */
	for ( i = 0; i < 4; i++ )
	{
	    for ( j = 0; j < 4; j++ )
	    {
		matrix[j][i] = 0.0;

		for ( k = 0; k < 4; k++ )
		{
		    matrix[j][i] += (right_to_left_mat[k][i] * 
				     lcs_matrix[j][k]);
		}
	    }
	}


/* get the screen viewport */
	view_port = DDdevice_InqActualViewport(mlr_control.device);

	res_x = (view_port->fur[0] - view_port->bll[0]);
	res_y = (view_port->fur[1] - view_port->bll[1]);

	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf(mlr_files.camera_fp, 
		    "cammat %f %f %f %f %f %f %f %f %f %f %f %f\n",
	             matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
	             matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
	             matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
	}
	else
	{
	    for ( i = 0; i < 4; i++ )
	    {
		for ( j = 0; j < 4; j++ )
		{
		    mlr_matrix[j][i] = matrix[i][j];
		}
	    }

#ifdef	RENDER_DIRECT
	    MLR_cammat( mlr_matrix);
#endif
	}
    
/* if the image is wider than hight, we have to adjust the projection plane
 * width to compensate for the way Dore thinks cameras work
 */
	if ( res_y < res_x )
	{
	    width = 0.5 * ((Float) res_y / (Float) res_x);
	}
	else
	{
	    width = 0.5;
	}

/* figure out what kind of camera we are dealing with */
	switch ( camera.type ) {
	case DORE_CAMERA_PERSPECTIVE:
            focal_lth = width / tan( (double) (camera.fov / 2.0));

	    if ( mlr_mode == MLR_MODE_FILE )
	    {
                fprintf( mlr_files.camera_fp, "res %d %d\n", res_x, res_y);
                fprintf( mlr_files.camera_fp, "scam %f\n", focal_lth);
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_res( (Float) res_x, (Float) res_y);
	        MLR_scam( (Float) focal_lth);
#endif
	    }

/* set wcs to pcs matrix */
	    cam_set_persp_matrix( focal_lth, res_x, res_y, matrix);

	    break;
	
	case DORE_CAMERA_PROJECTION:
	    focal_lth = camera.focal_lth;

	    if ( mlr_mode == MLR_MODE_FILE )
	    {
                fprintf( mlr_files.camera_fp, "res %d %d\n", res_x, res_y);
                fprintf( mlr_files.camera_fp, "scam %f\n", focal_lth);
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_res( (Float) res_x, (Float) res_y);
	        MLR_scam( (Float) focal_lth);
#endif
	    }

/* set wcs to pcs matrix */
	    cam_set_persp_matrix( focal_lth, res_x, res_y, matrix);
	    break;
	
	case DORE_CAMERA_PARALLEL:
	    if ( mlr_mode == MLR_MODE_FILE )
	    {
                fprintf( mlr_files.camera_fp, "res %d %d\n", res_x, res_y);
                fprintf( mlr_files.camera_fp, "ortho %f\n", camera.size);
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_res( (Float) res_x, (Float) res_y);
	        MLR_ortho( (Float) camera.size);
#endif
	    }

/* set wcs to pcs matrix */
	    cam_set_ortho_matrix( camera.size, res_x, res_y, matrix);
	    break;
	
	default:
	    DDerror( ERR_BAD_OBJECT, "CAM_make", "unknown camera type");
	}

/* get the background color */
	DvInqBackgroundColor( mlr_control.view, DcRGB, &bkg_color);

	if ( mlr_mode == MLR_MODE_FILE )
	{
            fprintf( mlr_files.camera_fp, "bkg %f %f %f\n", bkg_color.r, 
		     bkg_color.g, bkg_color.b);

	    if ( mlr_auto_render )
	    {
                fprintf( mlr_files.camera_fp, "outfile stdout rle\n");
	    }
	    else
	    {
                fprintf( mlr_files.camera_fp, "outfile %s rle\n", 
					      mlr_files.rle_file);
	    }
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_bkg( (Float) bkg_color.r, (Float) bkg_color.g,
		     (Float) bkg_color.b);
	    
	    MLR_display( FALSE, 0, 0);

	    /*
	    MLR_outfile( "dore.rle", FILE_TYPE_ART_RLE);
	    */
#endif
	}

/* get ray tracing depth */
        routine = DDclass_InqMethod( DsInqClassId( "AttGlbRendRayLevel"),
				        DcMethodInqGlbAttVal);

        (*routine)( &ray_depth);

	if ( mlr_mode == MLR_MODE_FILE )
	{
	    fprintf( mlr_files.camera_fp, "raylim %d\n", ray_depth);
	}
	else
	{
#ifdef	RENDER_DIRECT
	    MLR_raylim( ray_depth);
#endif
	}

	if ( ray_depth > 1 )
	{
	    mlr_limits.vox = 8;
	}

/* get super sampling data */
        routine = DDclass_InqMethod( DsInqClassId( "AttSampleSuperSwitch"),
				        DcMethodInqGlbAttVal);

        (*routine)( &aa_switch);

	if ( aa_switch )
	{
            routine = DDclass_InqMethod( DsInqClassId( "AttSampleSuper"),
				          DcMethodInqGlbAttVal);

            (*routine)( &xsamp, &ysamp);

	    if ( ysamp > xsamp )
		xsamp = ysamp;

	    if ( mlr_mode == MLR_MODE_FILE )
	    {
	        fprintf( mlr_files.camera_fp, "samp %d\n", (Int) xsamp);
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_samp( (Int) xsamp);
#endif
	    }
	}

/* get adaptive antialiasing status */
        routine = DDclass_InqMethod( DsInqClassId( "AttSampleAdaptiveSwitch"),
				      DcMethodInqGlbAttVal);

        (*routine)( &aa_switch);

	if ( aa_switch )
	{
	    if ( mlr_mode == MLR_MODE_FILE )
	    {
	        fprintf( mlr_files.camera_fp, "adaptive on\n");
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_adaptive( TRUE);
#endif
	    }
	}

/* get jitter sampling info */
        routine = DDclass_InqMethod( DsInqClassId( "AttSampleJitterSwitch"),
				      DcMethodInqGlbAttVal);

        (*routine)( &aa_switch);

	if ( aa_switch )
	{
            routine = DDclass_InqMethod( DsInqClassId( "AttSampleJitter"),
				          DcMethodInqGlbAttVal);

            (*routine)( &xsamp);

	    if ( mlr_mode == MLR_MODE_FILE )
	    {
	        fprintf( mlr_files.camera_fp, "jitter %f\n", xsamp);
	    }
	    else
	    {
#ifdef	RENDER_DIRECT
	        MLR_jitter( (Float) xsamp);
#endif
	    }
	}
    }
}


/*[
 *$     CAM_make_matrix
 *
 *      Steve Gray - Wed Jul 19 11:16:39 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set camera matrix
 *
 *      Usage:
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

CAM_make_matrix()
{
    DDerror( ERR_INVALID_CAMERA, "CAM_make_matrix", 
	        "arbitrary camera matricies are not yet supported");
}


/*[
 *$     CAM_make_perspective
 *
 *      Steve Gray - Wed Jul 19 11:17:20 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    
 *
 *      Usage:
 *		CAM_make_perspective( object)
 *		DtObject	*object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

CAM_make_perspective( object)
DtObjectStructure	*object;
{
struct perspective_data *per_data;


    per_data = (struct perspective_data *) (object->data);

    camera.type = DORE_CAMERA_PERSPECTIVE;

    camera.fov = per_data->fov_degree * (3.141592 / 180.0);
    camera.hither = -(per_data->hither);
    camera.yon = -(per_data->yon);

#ifdef	DEBUG
printf( "CAM_make_perspective %f %f %f\n", camera.fov, camera.hither,
	camera.yon);
#endif
}


/*[
 *$     CAM_make_projection
 *
 *      Steve Gray - Wed Jul 19 11:17:46 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    make an arbitrary projection camera
 *
 *      Usage:
 *		CAM_make_projection( object)
 *		DtObjectStructure	*object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *]
 */

CAM_make_projection( object)
DtObjectStructure	*object;
{
struct projection_data	*proj_data;


/* get the projection data */
    proj_data = (struct projection_data *) (object->data);


    if ( proj_data->ptype == DcParallel )
    {
	camera.type = DORE_CAMERA_PARALLEL;

	camera.size = proj_data->window.ur[0] - proj_data->window.ll[0];
    }
    else
    {
        camera.type = DORE_CAMERA_PROJECTION;

        camera.left = proj_data->window.ll[0];
        camera.bottom = proj_data->window.ll[1];

	camera.right = proj_data->window.ur[0];
        camera.top = proj_data->window.ur[1];

	camera.focal_lth = proj_data->view_plane;
    }

    camera.hither = -(proj_data->hither);
    camera.yon = -(proj_data->yon);
}


/*[
 *$     CAM_make_parallel
 *
 *      Steve Gray - Fri Dec 15 15:18:20 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    make a parallel projection
 *
 *      Usage:
 *		CAM_make_parallel( object)
 *		DtObjectStructure	*object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

CAM_make_parallel( object)
DtObjectStructure	*object;
{
struct parallel_data	*par_data;


/* get the dore data */
    par_data = (struct parallel_data *) (object->data);

/* set the camera data */
    camera.type = DORE_CAMERA_PARALLEL;

    camera.size = par_data->fov_degree;

    camera.hither = -(par_data->hither);
    camera.yon = -(par_data->yon);
}



/*[
 *$     CAM_get_matrix
 *
 *      Steve Gray - Thu May  3 08:06:30 PDT 1990
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    return viewing matrix
 *
 *      Usage:
 *		CAM_get_matrix( mat)
 *		DtMatrix4x4	mat	(out) viewing matrix
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

CAM_get_matrix( mat)
DtMatrix4x4	mat;
{
Int	i, j;


    for ( i = 0; i < 4; i++ )
	for ( j = 0; j < 4; j++ )
	    mat[i][j] = cam_current_wcs_to_pcs[j][i];
}


/*[
 *$     cam_set_persp_matrix
 *
 *      Steve Gray - Fri May  4 13:30:39 PDT 1990
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set current screen projection matrix for perspective
 *			camera
 *
 *      Usage:
 *		cam_set_persp_matrix( focal_length, res_x, res_y, cam_mat)
 *		Float		focal_length	(in)  camera focal length
 *		Int		res_x, res_y	(in)  screen resolution
 *		Matrix_4	cam_mat		(in)  camera matrix
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

cam_set_persp_matrix( focal_length, res_x, res_y, cam_mat)
Float		focal_length;
Int		res_x, res_y;
Matrix_4	cam_mat;
{
Float		aspect;
Matrix_4	projection;
Matrix_4	icam;
Int		i, j, k;


    aspect = ((Float) res_x) / ((Float) res_y);

    projection[0][0] = ((Float) res_x) / 2.0;
    projection[1][0] = 0.0;
    projection[2][0] = 0.0;
    projection[3][0] = ((Float) res_x) / 2.0;

    projection[0][1] = 0.0;
    projection[1][1] = (1.0 / aspect) * (((Float) res_y) / 2.0);
    projection[2][1] = 0.0;
    projection[3][1] = (1.0 / aspect) * (((Float) res_y) / 2.0);

    projection[0][2] = 0.0;
    projection[1][2] = 0.0;
    projection[2][2] = 0.5 / focal_length;
    projection[3][2] = -0.5;

    projection[0][3] = 0.0;
    projection[1][3] = 0.0;
    projection[2][3] = 0.5 / focal_length;
    projection[3][3] = 0.0;

/* invert the camera matrix */
    inv_cam_mat( cam_mat, icam);

/* pre-multiply the projection matrix by the inverse camera matrix */
    for ( i = 0; i < 4; i++ )
    {
	for( j = 0; j < 4; j++ )
	{
    	    cam_current_wcs_to_pcs[i][j] = 0.0;

	    for ( k = 0; k < 4; k++ )
	    {
		cam_current_wcs_to_pcs[i][j] += (icam[i][k] * projection[k][j]);
	    }
	}
    }
}


/*[
 *$     cam_set_ortho_matrix
 *
 *      Steve Gray - Fri May  4 13:33:34 PDT 1990
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set current matrix for orthographic camera
 *
 *      Usage:
 *		cam_set_ortho_matrix( size, res_x, res_y, cam_mat)
 *		Float		size		(in)  projection plane size
 *		Int		res_x, res_y	(in)  screen resolution
 *		Matrix_4	cam_mat		(in)  camera matrix
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

cam_set_ortho_matrix( size, res_x, res_y, cam_mat)
Float		size;
Int		res_x, res_y;
Matrix_4	cam_mat;
{
Float		aspect;
Matrix_4	projection;
Matrix_4	icam;
Int		i, j, k;


    aspect = ((Float) res_x) / ((Float) res_y);

    projection[0][0] = (2.0 / size) * (((Float) res_x) / 2.0);
    projection[1][0] = 0.0;
    projection[2][0] = 0.0;
    projection[3][0] = (2.0 / size) * (((Float) res_x) / 2.0);

    projection[0][1] = 0.0;
    projection[1][1] = (2.0 / (size * aspect)) * (((Float) res_y) / 2.0);
    projection[2][1] = 0.0;
    projection[3][1] = (2.0 / (size * aspect)) * (((Float) res_y) / 2.0);

    projection[0][2] = 0.0;
    projection[1][2] = 0.0;
    projection[2][2] = 1.0;
    projection[3][2] = 0.0;

    projection[0][3] = 0.0;
    projection[1][3] = 0.0;
    projection[2][3] = 0.0;
    projection[3][3] = 1.0;

/* invert the camera matrix */
    inv_cam_mat( cam_mat, icam);

/* pre-multiply the projection matrix by the inverse camera matrix */
    for ( i = 0; i < 4; i++ )
    {
	for( j = 0; j < 4; j++ )
	{
    	    cam_current_wcs_to_pcs[i][j] = 0.0;

	    for ( k = 0; k < 4; k++ )
	    {
		cam_current_wcs_to_pcs[i][j] += (icam[i][k] * projection[k][j]);
	    }
	}
    }
}


/*[
 *$     inv_cam_mat
 *
 *      Steve Gray - Fri May  4 13:37:49 PDT 1990
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    invert camera matrix
 *
 *      Usage:
 *		cam_mat_inv( source, inverse)
 *		Matrix_4	source		(in)  camera matrix
 *		Matrix_4	inverse		(in)  inverse matrix
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

inv_cam_mat (source, inverse)
Float	*source;
Float	*inverse;
{
register int 	i,j;
int		j_mat[16];
int 		m;
Float 		cc,s;
Float 		*t0, *t1;
double 		fabs();


    t0 = source;
    t1 = inverse;
    for (i = 16; i; i--)
	*t1++ = *t0++;

    for (i=0; i<4; i++)
	j_mat[i] = i;

    for (i=0; i<4; i++) {
	cc = 0.0;
	m = i;
	for (j=i; j<4; j++) {
	    t0 = inverse + i*4;
	    if ((fabs(cc)-fabs(t0[j]))<0.0) {
		m = j;
		cc = t0[j];
	    }
	}
	if (i != m) {
	    j = j_mat[m];
	    j_mat[m] = j_mat[i];
	    j_mat[i] = j;
	    for (j=0; j<4; j++) {
		t0 = inverse + j*4;
		s = t0[i];
		t0[i] = t0[m];
		t0[m] = s;
	    }
	}
	t0 = inverse + i*4;
	t0[i] = 1.0;
	for (m=0; m<4; m++)
	    t0[m] /= cc;
	for (m=0; m<4; m++) {
	    if (i!=m) {
		t0 = inverse + i*4;
		t1 = inverse + m*4;
		cc = t1[i];
		if (cc!=0.0) {
		    t1[i] = 0.0;
		    for (j=0; j<4; j++)
			t1[j] -= cc*t0[j];
		}
	    }
	}
    }
    for (i=0; i<4; i++) {
	if (j_mat[i] != i) {
	    m = i;
	    do {
		m++;
		if (j_mat[m]==i)
		    break;
	    } while (4>m);
	    j_mat[m] = j_mat[i];
	    for (j=0; j<4; j++) {
		t0 = inverse + i*4;
		t1 = inverse + m*4;
		cc = t0[j];
		t0[j] = t1[j];
		t1[j] = cc;
	    }
	    j_mat[i] = i;
	}
    }

    return;
}

