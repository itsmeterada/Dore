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
	int ddr_x11_titang2_drom_render_point_list (colormodel,bndboxpts,pointcount, vtxlocs,vtxnrms,vtxclrs)
	int ddr_x11_titang2_drom_render_line_list (colormodel,bndboxpts,linecount,elenrms, vtxlocs,vtxnrms,vtxclrs)
	int ddr_x11_titang2_drom_render_connected_line_list (colormodel,bndboxpts,vertexcount, elenrms,vtxlocs, vtxnrms,vtxclrs)
	int ddr_x11_titang2_drom_render_triangle_list (colormodel,bndboxpts,trianglecount,elenrms, vtxlocs,vtxnrms,vtxclrs)
	int ddr_x11_titang2_drom_render_triangle_mesh (colormodelbndboxpts,trianglecount,elenrms, vertexcount,vtxlocs,vtxnrms, vtxclrs,vertexlist, compiledvertexlist)
	int ddr_x11_titang2_drom_parallel_render_point_list (bndboxpts,pointcount, vtxlocs,vtxnrms,vtxclrs)
	void ddr_titan_par_rndpntlst ()
	int ddr_x11_titang2_drom_parallel_render_line_list (bndboxpts,linecount,elenrms, vtxlocs,vtxnrms,vtxclrs)
	void ddr_titan_par_rndlinlst ()
	int ddr_x11_titang2_drom_parallel_render_connected_line_list (bndboxpts,linecount, elenrms,vertexcount, vtxlocs,vtxnrms,vtxclrs)
	void ddr_titan_par_rndclnlst ()
	int ddr_x11_titang2_drom_parallel_render_triangle_list (bndboxpts,trianglecount, elenrms,vtxlocs,vtxnrms,vtxclrs)
	void ddr_titan_par_rndtrilst ()
	int ddr_x11_titang2_drom_parallel_render_triangle_mesh (bndboxpts,trianglecount,elenrms, vertexcount,vtxlocs,vtxnrms, vtxclrs,compiledvertexlist)
	void ddr_titan_par_rndtrimsh ()

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
extern DtInt	_taskno;
DtInt		 globalpointcount;
DtInt		 globallinecount;
DtInt		 globalvertexcount;
DtInt		 globaltrianglecount;
DtRealTriple	*globalelenrms;
DtRealTriple	*globalvtxlocs;
DtRealTriple	*globalvtxnrms;
DtRealTriple	*globalvtxclrs;
DtIntTriple	*globalcompiledvertexlist;
DtFlag		 directdrawflag;

/*
 ======================================================================
 */

ddr_x11_titang2_drom_render_point_list(colormodel,bndboxpts,pointcount,
				       vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt pointcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
    if (doe_system_main_processors > 0 && (pointcount > 32)) {
	ddr_x11_titang2_drom_parallel_render_point_list(bndboxpts,pointcount,
							vtxlocs,vtxnrms,vtxclrs);
    } else {
	ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_pntlst,bndboxpts,
					       (DtRealTriple *)0,vtxnrms,vtxclrs);

	ddr_x11_titang2_drom_setup_variable_parms(0,pointcount,
						  vtxlocs,(DtRealTriple *)0,vtxnrms,vtxclrs);

#ifdef DEBUG
	ddr_x11_titang2_drom_print_fixed_parms();
	ddr_x11_titang2_drom_print_variable_parms(0);
#endif

	_cg_point_draw(&dde_x11_titang2_drom_cg_tlparms[0]);
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_render_line_list(colormodel,bndboxpts,linecount,elenrms,
				      vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt linecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
    DtFlag ddr_x11_titang2_drom_check_direct_line();

    if (doe_system_main_processors > 0 && (linecount > 32)) {
	ddr_x11_titang2_drom_parallel_render_line_list(bndboxpts,linecount,
						       elenrms,vtxlocs,
						       vtxnrms,vtxclrs);
    } else {
	ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_linlst,bndboxpts,
					       elenrms,vtxnrms,vtxclrs);

	ddr_x11_titang2_drom_setup_variable_parms(0,linecount,vtxlocs,
						  elenrms,vtxnrms,vtxclrs);

/*
	if (ddr_x11_titang2_drom_check_direct_line()) {
	    DDdevice_SetForegroundColor(dde_x11_titang2_drom_data.current_device,
	    				   dde_x11_titang2_drom_data.diffuse_reflectance);
	  
	    ddr_x11_titang2_dcm_set_depth_buffering(DcOff);
	  
	    _cg_direct_line_draw(&dde_x11_titang2_drom_cg_tlparms[0]);
	  
	    ddr_x11_titang2_dcm_set_depth_buffering(DcOn);
	} else {
	    _cg_line_draw(&dde_x11_titang2_drom_cg_tlparms[0]);
	}
*/
#ifdef DEBUG
	ddr_x11_titang2_drom_print_fixed_parms();
	ddr_x11_titang2_drom_print_variable_parms(0);
#endif

	_cg_line_draw(&dde_x11_titang2_drom_cg_tlparms[0]);
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_render_connected_line_list(colormodel,bndboxpts,vertexcount,
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
    ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_conlinlst,
					   bndboxpts, elenrms,vtxnrms,vtxclrs);

    ddr_x11_titang2_drom_setup_variable_parms(0,vertexcount-1,
					      vtxlocs,elenrms,vtxnrms,vtxclrs);

#ifdef DEBUG
    ddr_x11_titang2_drom_print_fixed_parms();
    ddr_x11_titang2_drom_print_variable_parms(0);
#endif

    _cg_polyline_draw(&dde_x11_titang2_drom_cg_tlparms[0]);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_render_triangle_list(colormodel,bndboxpts,trianglecount,elenrms,
					  vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
    if ((doe_system_main_processors > 0) && (trianglecount > 32)) {
	ddr_x11_titang2_drom_parallel_render_triangle_list(bndboxpts,trianglecount,
							   elenrms,vtxlocs,vtxnrms,
							   vtxclrs);
    } else {
	ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_trilst,
					       bndboxpts,elenrms,vtxnrms,vtxclrs);

	ddr_x11_titang2_drom_setup_variable_parms(0,trianglecount,
						  vtxlocs,elenrms,vtxnrms,vtxclrs);

#ifdef DEBUG
	ddr_x11_titang2_drom_print_fixed_parms();
	ddr_x11_titang2_drom_print_variable_parms(0);
#endif

	_cg_tri_draw(&dde_x11_titang2_drom_cg_tlparms[0]);
    }
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_render_triangle_mesh(colormodel,bndboxpts,trianglecount,elenrms,
					  vertexcount,vtxlocs,vtxnrms,
					  vtxclrs,vertexlist,
					  compiledvertexlist)
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
    if ((doe_system_main_processors > 0) && (trianglecount > 32)) {
	ddr_x11_titang2_drom_parallel_render_triangle_mesh(bndboxpts,trianglecount,
							   elenrms,vertexcount,
							   vtxlocs,vtxnrms,vtxclrs,
							   compiledvertexlist);
    } else {
	ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_trimsh,
					       bndboxpts, elenrms,vtxnrms,vtxclrs);

	ddr_x11_titang2_drom_setup_variable_parms(0,trianglecount,
						  vtxlocs,elenrms,vtxnrms,vtxclrs);

	ddr_x11_titang2_drom_setup_mesh_space(vertexcount,1);

	ddr_x11_titang2_drom_setup_variable_mesh_parms(0,vertexcount,0,
						       compiledvertexlist);

#ifdef DEBUG
	ddr_x11_titang2_drom_print_fixed_parms();
	ddr_x11_titang2_drom_print_variable_parms(0);
#endif

	_cg_trimesh_draw(&dde_x11_titang2_drom_cg_tlparms[0]);
    }
}

/*
 ======================================================================
 */

ddr_x11_titang2_drom_parallel_render_point_list(bndboxpts,pointcount,
						vtxlocs,vtxnrms,vtxclrs)
	DtRealTriple bndboxpts[8];
	DtInt pointcount;
	DtRealTriple *vtxlocs;
	DtRealTriple *vtxnrms;
	DtRealTriple *vtxclrs;
{
    extern void ddr_titan_par_rndpntlst();

    globalpointcount = pointcount;
    globalvtxlocs = vtxlocs;
    globalvtxnrms = vtxnrms;
    globalvtxclrs = vtxclrs;

    ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_pntlst,
					   bndboxpts,
					   elenrms,vtxnrms,vtxclrs);

    parstack(ddr_titan_par_rndpntlst,2000,doe_system_main_processors);
}
/*
 ======================================================================
 */

void ddr_titan_par_rndpntlst()
{
    threadlocal DtInt chunksize;
    threadlocal DtInt thread;
    threadlocal DtInt firstpointno;
    threadlocal DtInt pointcount;
    threadlocal DtInt firstvertexno;
    threadlocal DtRealTriple *vtxlocs, *vtxnrms, *vtxclrs;

    if (doe_system_main_processors == 1){
	thread = 0;

	firstpointno = 0;
	pointcount = globalpointcount;
    }
    else{
	thread = _taskno;

	chunksize = globalpointcount / doe_system_main_processors;

	firstpointno = thread * chunksize;

	if (thread == (doe_system_main_processors-1)){
	    pointcount = globalpointcount - firstpointno;
	}
	else{
	    pointcount = chunksize;
	}
    }

    firstvertexno = firstpointno;

    vtxlocs = globalvtxlocs + firstvertexno;

    if (globalvtxnrms == NULL)
	    vtxnrms = NULL;
    else
	    vtxnrms = globalvtxnrms + firstvertexno;

    if (globalvtxclrs == NULL)
	    vtxclrs = NULL;
    else
	    vtxclrs = globalvtxclrs + firstvertexno;

    ddr_x11_titang2_drom_setup_variable_parms(thread,pointcount,
					      vtxlocs,NULL,vtxnrms,vtxclrs);

    _cg_point_draw(&dde_x11_titang2_drom_cg_tlparms[thread]);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_parallel_render_line_list(bndboxpts,linecount,elenrms,
					       vtxlocs,vtxnrms,vtxclrs)
	DtRealTriple bndboxpts[8];
	DtInt linecount;
	DtRealTriple *elenrms;
	DtRealTriple *vtxlocs;
	DtRealTriple *vtxnrms;
	DtRealTriple *vtxclrs;
{
    DtFlag ddr_x11_titang2_drom_check_direct_line();
    extern void ddr_titan_par_rndlinlst();

    globallinecount = linecount;
    globalelenrms = elenrms;
    globalvtxlocs = vtxlocs;
    globalvtxnrms = vtxnrms;
    globalvtxclrs = vtxclrs;

    ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_linlst,
					   bndboxpts,elenrms,vtxnrms,vtxclrs);

    directdrawflag = ddr_x11_titang2_drom_check_direct_line();

    parstack(ddr_titan_par_rndlinlst,2000,doe_system_main_processors);
}
/*
 ======================================================================
 */
void ddr_titan_par_rndlinlst()
{
    threadlocal DtInt chunksize;
    threadlocal DtInt firstvertexno;
    threadlocal DtInt thread;
    threadlocal DtInt firstlineno;
    threadlocal DtInt linecount;
    threadlocal DtRealTriple *vtxlocs, *elenrms, *vtxnrms, *vtxclrs;

    if (doe_system_main_processors == 1){
	thread = 0;

	firstlineno = 0;
	linecount = globallinecount;
    }
    else{
	thread = _taskno;

	chunksize = globallinecount / doe_system_main_processors;

	firstlineno = thread * chunksize;

	if (thread == (doe_system_main_processors-1)){
	    linecount = globallinecount - firstlineno;
	}
	else{
	    linecount = chunksize;
	}
    }

    firstvertexno = 2 * firstlineno;

    vtxlocs = globalvtxlocs + firstvertexno;
    elenrms = globalelenrms + firstlineno;
    vtxnrms = globalvtxnrms + firstvertexno;
    vtxclrs = globalvtxclrs + firstvertexno;

    ddr_x11_titang2_drom_setup_variable_parms(thread,linecount,
					      vtxlocs,elenrms,vtxnrms,vtxclrs);

    /*  If we are not performing hidden-surface or vertex shading
	operations on the line list, then we can use bill's "special
	super high-speed line list drawing routine".  */

    /*
      if (directdrawflag){
      DDdevice_SetForegroundColor(dde_x11_titang2_drom_data.current_device,
      				     dde_x11_titang2_drom_data.diffuse_reflectance);
      
      ddr_x11_titang2_dcm_set_depth_buffering(DcOff);
      
      _cg_direct_line_draw(&dde_x11_titang2_drom_cg_tlparms[thread]);
      
      ddr_x11_titang2_dcm_set_depth_buffering(DcOn);
      }
      else{
      _cg_line_draw(&dde_x11_titang2_drom_cg_tlparms[thread]);
      }
      */
    _cg_line_draw(&dde_x11_titang2_drom_cg_tlparms[thread]);
}
/*
 ======================================================================
 */
ddr_x11_titang2_drom_parallel_render_connected_line_list(bndboxpts,linecount,
							 elenrms,vertexcount,
							 vtxlocs,vtxnrms,vtxclrs)
	DtRealTriple bndboxpts[8];
	DtInt linecount;
	DtRealTriple *elenrms;
	DtInt vertexcount;
	DtRealTriple *vtxlocs;
	DtRealTriple *vtxnrms;
	DtRealTriple *vtxclrs;
{
    extern void ddr_titan_par_rndclnlst();

    globalvertexcount = vertexcount;
    globallinecount = linecount;
    globalelenrms = elenrms;
    globalvtxlocs = vtxlocs;
    globalvtxnrms = vtxnrms;
    globalvtxclrs = vtxclrs;

    ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_conlinlst,
					   bndboxpts,elenrms,vtxnrms,vtxclrs);

    parstack(ddr_titan_par_rndclnlst,2000,doe_system_main_processors);
}
/*
 ======================================================================
 */
void ddr_titan_par_rndclnlst()
{
    threadlocal DtInt firstvertexno;
    threadlocal DtInt thread;
    threadlocal DtInt firstlineno;
    threadlocal DtInt linecount;
    threadlocal DtRealTriple *vtxlocs, *elenrms, *vtxnrms, *vtxclrs;

    if (doe_system_main_processors == 1){
	thread = 0;

	firstvertexno = 0;

	firstlineno = 0;
	linecount = globallinecount;
    }
    else{
	thread = _taskno;

	if (thread == 0){
	    firstvertexno = 0;

	    firstlineno = 0;
	    linecount = globallinecount / doe_system_main_processors;
	}
	else if (thread == 1){
	    firstvertexno = globalvertexcount / doe_system_main_processors;

	    firstlineno = globallinecount / doe_system_main_processors;
	    linecount = globallinecount - firstlineno;
	}
	else{
	    return;
	}
    }

    firstvertexno = 2 * firstlineno;

    vtxlocs = globalvtxlocs + firstvertexno;
    elenrms = globalelenrms + firstlineno;
    vtxnrms = globalvtxnrms + firstvertexno;
    vtxclrs = globalvtxclrs + firstvertexno;

    ddr_x11_titang2_drom_setup_variable_parms(thread,linecount,
					      vtxlocs,elenrms,vtxnrms,vtxclrs);

    /*
      _cg_polyline_draw(&dde_x11_titang2_drom_cg_tlparms[thread]);
      */
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_parallel_render_triangle_list(bndboxpts,trianglecount,
		elenrms,vtxlocs,vtxnrms,vtxclrs)
	DtRealTriple bndboxpts[8];
	DtInt trianglecount;
	DtRealTriple *elenrms;
	DtRealTriple *vtxlocs;
	DtRealTriple *vtxnrms;
	DtRealTriple *vtxclrs;
{
    extern void ddr_titan_par_rndtrilst();

    ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_trilst,
					   bndboxpts, elenrms,vtxnrms,vtxclrs);

    globaltrianglecount = trianglecount;
    globalelenrms = elenrms;
    globalvtxlocs = vtxlocs;
    globalvtxnrms = vtxnrms;
    globalvtxclrs = vtxclrs;

    parstack(ddr_titan_par_rndtrilst,2000,doe_system_main_processors);
}
/*
 ======================================================================
 */
void ddr_titan_par_rndtrilst()
{
    threadlocal DtInt thread;
    threadlocal DtInt chunksize;
    threadlocal DtInt firsttriangleno;
    threadlocal DtInt trianglecount;
    threadlocal DtInt firstvertexno;
    threadlocal DtRealTriple *vtxlocs, *elenrms, *vtxnrms, *vtxclrs;

    if (doe_system_main_processors == 1){
	thread = 0;

	firsttriangleno = 0;
	trianglecount = globaltrianglecount;
    }
    else{
	thread = _taskno;

	chunksize = globaltrianglecount / doe_system_main_processors;

	firsttriangleno = thread * chunksize;

	if (thread == (doe_system_main_processors-1)){
	    trianglecount = globaltrianglecount - firsttriangleno;
	}
	else{
	    trianglecount = chunksize;
	}
    }

    firstvertexno = 3 * firsttriangleno;

    vtxlocs = globalvtxlocs + firstvertexno;
    elenrms = globalelenrms + firsttriangleno;
    vtxnrms = globalvtxnrms + firstvertexno;
    vtxclrs = globalvtxclrs + firstvertexno;

    ddr_x11_titang2_drom_setup_variable_parms(thread,trianglecount,
					      vtxlocs,elenrms,vtxnrms,vtxclrs);

    _cg_tri_draw(&dde_x11_titang2_drom_cg_tlparms[thread]);
}
/*
 ======================================================================
 */

ddr_x11_titang2_drom_parallel_render_triangle_mesh(bndboxpts,trianglecount,elenrms,
						   vertexcount,vtxlocs,vtxnrms,
						   vtxclrs,compiledvertexlist)
	DtRealTriple bndboxpts[8];
	DtInt trianglecount;
	DtRealTriple *elenrms;
	DtInt vertexcount;
	DtRealTriple *vtxlocs;
	DtRealTriple *vtxnrms;
	DtRealTriple *vtxclrs;
	DtInt compiledvertexlist[][3];
{
    extern void ddr_titan_par_rndtrimsh();

    ddr_x11_titang2_drom_setup_fixed_parms(ddd_x11_titang2_drom_prigeotyp_trimsh,
					   bndboxpts,elenrms,vtxnrms,vtxclrs);

    ddr_x11_titang2_drom_setup_mesh_space(vertexcount,doe_system_main_processors);

    globalvertexcount = vertexcount;
    globaltrianglecount = trianglecount;
    globalelenrms = elenrms;
    globalvtxlocs = vtxlocs;
    globalvtxnrms = vtxnrms;
    globalvtxclrs = vtxclrs;
    globalcompiledvertexlist = compiledvertexlist;

    parstack(ddr_titan_par_rndtrimsh,2000,doe_system_main_processors);
}
/*
 ======================================================================
 */
void ddr_titan_par_rndtrimsh()
{
    threadlocal DtInt chunksize;
    threadlocal DtInt thread;
    threadlocal DtInt firsttriangleno;
    threadlocal DtInt trianglecount;
    threadlocal DtInt firstvertexno;
    threadlocal DtInt vertexcount;
    threadlocal DtRealTriple *elenrms;

    if (doe_system_main_processors == 1){
	thread = 0;

	firsttriangleno = 0;
	trianglecount = globaltrianglecount;

	firstvertexno = 0;
	vertexcount = globalvertexcount;
    }
    else{
	thread = _taskno;

	chunksize = globaltrianglecount / doe_system_main_processors;

	firsttriangleno = thread * chunksize;

	if (thread == (doe_system_main_processors-1)){
	    trianglecount = globaltrianglecount - firsttriangleno;
	}
	else{
	    trianglecount = chunksize;
	}

	chunksize = globalvertexcount / doe_system_main_processors;

	firstvertexno = thread * chunksize;

	if (thread == (doe_system_main_processors-1)){
	    vertexcount = globalvertexcount - firstvertexno;
	}
	else{
	    vertexcount = chunksize;
	}
    }

    elenrms = globalelenrms + firsttriangleno;

    ddr_x11_titang2_drom_setup_variable_parms(thread,trianglecount,
					      globalvtxlocs,elenrms,globalvtxnrms,globalvtxclrs);

    ddr_x11_titang2_drom_setup_variable_mesh_parms(thread,vertexcount,firstvertexno,
						   globalcompiledvertexlist[firsttriangleno]);

    _cg_trimesh_draw(&dde_x11_titang2_drom_cg_tlparms[thread]);
}
