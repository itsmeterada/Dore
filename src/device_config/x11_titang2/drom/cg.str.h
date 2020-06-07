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
 
/*  This file defines the interface to the low level graphics
    clipping, drawing, and shading pipeline.

    Two structures are defined.  The first construct, 'cg_tlparms',
    is a data structure containing the thread local parameters
    required for defining the working area and set of elements to be
    clipped, shaded, and drawn during a single call running under a 
    single thread.  The second construct,'cg_sparms', is a data 
    structure containing the shared flags, parameters, and data 
    pointers required for specifying clipping, shading, and drawing 
    of any of the defined objects.  When operating on a multiprocessor 
    system, the 'cg_sparms' parameter block is read-only and can be 
    shared among all threads.  The 'cg_tlparms' parameter block must
    be specified uniquely for each stream.  Neither parameter block 
    is modified by the low level pipeline.  

    Symbolic definitions for the fields and values contained in the
    general control parameter are defined in cg.con.h.  Both
    the data structures and the symbolic constants are defined 
    both in C language and in assembly language formats.  The C 
    language definitions use identifiers prefixed by 'cg_' and 
    symbolic constants prefixed by 'CG_'.  This reduces the chances
    for conflict with other defined symbols in C language code.
     
    Conditional definition is under control of the symbol 'ASSEMBLER'. 
    When this symbol is defined to the pre-processor, the assembler 
    formats are defined.  Otherwise, the C language formats are 
    defined.

    The clipping, shading, and drawing pipeline is always invoked 
    by a call to the main fetch and shade routine, with a single 
    formal parameter, e.g.

	_tri_draw( &(thread_local_parm_block) );


    The main fetch and shade routines are:

	file		entry point		input structure
	----		-----------		---------------
	fstrimesh.s	_cg_trimesh_draw	triangle mesh
	fstri.s		_cg_tri_draw		triangle list
	fspolylin.s	_cg_polyline_draw	polyline
	fsdirlin.s	_cg_direct_line_draw	direct line list
	fslin.s		_cg_line_draw		line list
	fspoint.s	_cg_point_draw		point list
*/

/*	thread local clipping, shading, and drawing parameter block	*/
#ifndef	ASSEMBLER

struct	cg_tlparms {

/*	C language definitions	*/

	struct	 cg_sparms  *cg_spb;  /* -> cg_sparms parameter block */
	int	 cg_nelts;	/* number of elements */
	
	double	*cg_verts;	/* -> vertex coordinates */
	double	*cg_vcolors;	/* -> vertex colors */
	double	*cg_norms;	/* -> vertex or element normals */
	int	*cg_vismask;	/* -> visibility masks */

	/*	last six parameters used for meshes only	*/
	int	*cg_meshsync;	/* -> synchronization location */
				/* preset to - number of threads by caller */	
	int	 cg_nverts;	/* number of vertices to process */
	int	 cg_fstvert;	/* index of first vertex to process */
	int	*cg_tclist;	/* -> mesh connectivity list, scaled by 3 */
	double	*cg_xfverts;	/* -> xformed vertex area: X,Y,Z */
	double	*cg_xfpersp;	/* -> xformed perspective data W */
	
};

#else

/*	assembler language definitions	*/

#define	SPB	0(tlpb)		/* -> spb parameter block */
#define	NELTS	4(tlpb)		/* number of elements */

#define	VERTS	8(tlpb)		/* -> vertex coordinates */
#define	VCOLORS	12(tlpb)	/* -> vertex colors */
#define	NORMS	16(tlpb)	/* -> vertex or element normals */
#define	VISMASK	20(tlpb)	/* -> visibility mask */

	/*	last six parameters used for meshes only	*/
#define	MSYNC	24(tlpb)	/* -> synchronization location */
#define	NVERTS	28(tlpb)	/* number of vertices to process */
#define	FSTVERT	32(tlpb)	/* index of first vertex to process */	
#define	TCLIST	36(tlpb)	/* -> mesh connectivity list, scaled by 3 */
#define	XFVERTS	40(tlpb)	/* -> transformed vertex area: X,Y,Z */
#define	XFPERSP	44(tlpb)	/* -> transformed perspective data W */


#endif

/*	shared clipping, shading, and drawing parameter block	*/
#ifndef	ASSEMBLER

/*	C language definitions	*/

struct  cg_sparms {

	int	 cg_ctl;	/* control vector, defined in cg.con.h */
	int	 cg_lhsense;	/* left handed directional sense flag */
	
	double	*cg_matrix;	/* -> transformation matrix */
	double	*cg_scale;	/* -> scaling vector */
	double	*cg_xlate;	/* -> translation vector */

	double	*cg_bdbox;	/* -> bounding box */
	double	*cg_window;	/* -> clipping planes: XL,YT,XR,YB */

	int	 cg_nrects;	/* number of obscuring rectangles */
	int	 cg_nmplanes;	/* number of modeling planes */
	double	*cg_rects;	/* -> obscuring rectangle list */
	double	*cg_mplanes;	/* -> modeling plane list, each:
				     (A/SCx, B/SCy, C/SCz, D) */
	
	double	*cg_drefl;	/* -> diffuse reflectance or total color */
	double	*cg_arefl;	/* -> ambient reflectance */
	double	*cg_srefl;	/* -> specular reflectance */

	int	 cg_nlights;	/* number of lights */
	double	*cg_lnorms;	/* -> light normals */
	double	*cg_hnorms;	/* -> specular normals */
	double	*cg_lcolors;	/* -> light colors */
	double	*cg_sptbl;	/* -> specular power table */

	double	*cg_dprange;	/* -> 1/(zfront-zback), zfront, zback */
	double	*cg_dpfracs;	/* -> sfront, sback */
	double	*cg_dpcolor;	/* -> depth cueing background color */

	double	*cg_psweight;	/* -> pseudo color weights
					( R wt, G wt, B wt )
				     or ( 256./256., R<0>/256., G<0>/256. */
	union {
	  double *cg_pslim;	/* -> pseudo color limits
					( low, high, (high-low)/255 )  */
	  int	 *cg_psmask;	/* -> pseudo color bit masks for R,G,B,
					each as: int  0,mask;	*/
	} cg_pscol;
};

#else

/*	assembler language definitions	*/

#define	CTL	0(spb)		/* control vector, defined in cg.con.h */
#define	LHSENSE	4(spb)		/* left handed directonal sense flag */

#define	MATRIX	8(spb)		/* -> transformation matrix */
#define	SCALEV	12(spb)		/* -> scaling vector */
#define	XLATEV	16(spb)		/* -> translation vector */

#define	BDBOXC	20(spb)		/* -> bounding box coordinates */
#define	WINDOW	24(spb)		/* -> clipping planes: XL,YT,XR,YB */

#define	NRECTS	 28(spb)	/* number of obscuring rectangles */
#define	NMPLANES 32(spb)	/* number of modeling planes */
#define	RECTS	 36(spb)	/* -> obscuring rectangle list */
#define	MPLANES	 40(spb)	/* -> modeling plane list, each:
				     (A/SCx, B/SCy, C/SCz, D) */
	
#define	DREFL	44(spb)		/* -> diffuse reflectance or total color */
#define	AREFL	48(spb)		/* -> ambient reflectance */
#define	SREFL	52(spb)		/* -> specular reflectance */

#define	NLIGHTS	56(spb)		/* number of lights */
#define	LNORMS	60(spb)		/* -> light normals */
#define	HNORMS	64(spb)		/* -> specular normals */
#define	LCOLORS	68(spb)		/* -> light colors */
#define	SPTBL	72(spb)		/* -> specular power table */

#define	DPRANGE	76(spb)		/* -> 1/(zfront-zback), zfront, zback */
#define	DPFRACS	80(spb)		/* -> sfront, sback */
#define	DPCOLOR	84(spb)		/* -> depth cueing background color */

#define	PSWEIGHT 88(spb)	/* -> pseudo color weights
					( R wt, G wt, B wt )
				     or ( 256./256., R<0>/256., G<0>/256. */
#define	PSLIM	92(spb)		/* -> pseudo color limits
					( low, high, (high-low)/255 )  */
#define	PSMASK	92(spb)		/* -> pseudo color bit masks for R,G,B,
					each as: .word  0,mask  */
					
#endif

