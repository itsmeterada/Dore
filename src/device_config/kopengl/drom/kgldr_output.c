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
 
/**************************************************************************

  Functions:
    ddr_kopengl_drom_render_point_list 
    ddr_kopengl_drom_render_line_list 
    ddr_kopengl_drom_render_connected_line_list 
    ddr_kopengl_drom_render_triangle_list
    ddr_kopengl_drom_render_triangle_mesh 

**************************************************************************/


#include "drom.h"



void ddr_kopengl_drom_render_point_list (DtColorModel   colormodel,
                                         DtRealTriple   bndboxpts[8],
                                         DtInt          pointcount,
                                         DtRealTriple  *vtxlocs,
                                         DtRealTriple  *vtxnrms,
                                         DtRealTriple  *vtxclrs,
                                         DtInt          uv_count,
                                         DtRealCouple **uv_list,
                                         DtInt          uvw_count,
                                         DtRealTriple **uvw_list)
 {
  int i;
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ...in _drom_render_point_list\n");
  ddr_kopengl_drom_print_point_list (colormodel, bndboxpts, pointcount,
				     vtxlocs, vtxnrms, vtxclrs,
				     uv_count, uv_list, uvw_count, uvw_list);
#endif

#ifdef NO_BUFFERED_EXTENSIONS

  glBegin (GL_POINTS);

    if (vtxnrms)
      {
       if (vtxclrs)
         {
          for (i=0; i<pointcount; i++)
            {
             GL_VCOLOR (vtxclrs[i]);
             GL_NORMAL (vtxnrms[i]);
             GL_VERTEX (vtxlocs[i]);
            }
         }
       else
         {
          for (i=0; i<pointcount; i++)
            {
             GL_NORMAL (vtxnrms[i]);
             GL_VERTEX (vtxlocs[i]);
            }
         }
      }
    else
      {
       if (vtxclrs)
         {
          for (i=0; i<pointcount; i++)
            {
             GL_VCOLOR (vtxclrs[i]);
             GL_VERTEX (vtxlocs[i]);
            }
         }
       else
         {
          for (i=0; i<pointcount; i++)
            {
             GL_VERTEX (vtxlocs[i]);
            }
         }
      }

  glEnd ();

#else
  kglBeginBuf (GL_POINTS, pointcount); 

    if (vtxclrs)
       KGL_COLOR_BUF (vtxclrs, 3*sizeof (glReal));

    if (vtxnrms)
       KGL_NORMAL_BUF (vtxnrms, 3*sizeof (glReal));

    if (vtxlocs)
       KGL_VERTEX_BUF (vtxlocs, 3*sizeof (glReal));

  kglEndBuf ();
#endif
 }



void ddr_kopengl_drom_render_line_list (DtColorModel   colormodel,
                                        DtRealTriple   bndboxpts[8],
                                        DtInt          linecount,
                                        DtRealTriple  *elenrms,
                                        DtRealTriple  *vtxlocs,
                                        DtRealTriple  *vtxnrms,
                                        DtRealTriple  *vtxclrs,
                                        DtInt          uv_count,
                                        DtRealCouple **uv_list,
                                        DtInt          uvw_count,
                                        DtRealTriple **uvw_list)
 {
  int i;
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ...in _drom_render_line_list\n");
  ddr_kopengl_drom_print_line_list (colormodel, bndboxpts, linecount, elenrms,
				    vtxlocs, vtxnrms, vtxclrs,
				    uv_count, uv_list, uvw_count, uvw_list);
#endif

#ifdef NO_BUFFERED_EXTENSIONS

  glBegin (GL_LINES);

    if (vtxnrms)
      {
       if (vtxclrs)
         {
          for (i=0; i<2*linecount; i++)
            {
             GL_NORMAL (vtxnrms[i]);
             GL_VCOLOR (vtxclrs[i]);
             GL_VERTEX (vtxlocs[i]);
            }
         }
       else
         {
          for (i=0; i<2*linecount; i++)
            {
             GL_NORMAL (vtxnrms[i]);
             GL_VERTEX (vtxlocs[i]);
            }
         }
      }
    else
      {
       if (elenrms)
         {
          if (vtxclrs)
            {
             for (i=0; i<2*linecount; i++)
               {
                if (!(i%2))
                  GL_NORMAL (elenrms[i/2]);
                GL_VCOLOR (vtxclrs[i]);
                GL_VERTEX (vtxlocs[i]);
               }
            }
          else
            {
             for (i=0; i<2*linecount; i++)
               {
                if (!(i%2))
                  GL_NORMAL (elenrms[i/2]);
                GL_VERTEX (vtxlocs[i]);
               }
            }
         }
       else
         {
          if (vtxclrs)
            {
             for (i=0; i<2*linecount; i++)
               {
                GL_VCOLOR (vtxclrs[i]);
                GL_VERTEX (vtxlocs[i]);
               }
            }
          else
            {
             for (i=0; i<2*linecount; i++)
               {
                GL_VERTEX (vtxlocs[i]);
               }
            }
         }
      }

  glEnd ();

#else /* If no buffered extensions */

  kglBeginBuf (GL_LINES, linecount);

    if (vtxclrs)
       KGL_COLOR_BUF (vtxclrs, 3 * sizeof (glReal));

    if (vtxnrms)
       KGL_NORMAL_BUF (vtxnrms, 3 * sizeof (glReal));

    KGL_VERTEX_BUF (vtxlocs, 3 * sizeof (glReal));

  kglEndBuf ();

#endif
 }



void ddr_kopengl_drom_render_connected_line_list (DtColorModel   colormodel,
                                                  DtRealTriple   bndboxpts[8],
                                                  DtInt          vertexcount,
                                                  DtRealTriple  *elenrms,
                                                  DtRealTriple  *vtxlocs,
                                                  DtRealTriple  *vtxnrms,
                                                  DtRealTriple  *vtxclrs,
                                                  DtInt          uv_count,
                                                  DtRealCouple **uv_list,
                                                  DtInt          uvw_count,
                                                  DtRealTriple **uvw_list)
 {
  int i;
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ...in _drom_render_connected_line_list\n");
  ddr_kopengl_drom_print_connected_line_list (colormodel, bndboxpts,vertexcount,
					      elenrms,vtxlocs,vtxnrms,
					      vtxclrs, uv_count, uv_list, 
					      uvw_count, uvw_list);
#endif

#ifdef NO_BUFFERED_EXTENSIONS

  glBegin (GL_LINE_STRIP);

    if (vtxnrms)
      {
       if (vtxclrs)
         {
          for (i=0; i<vertexcount; i++)
            {
             GL_NORMAL (vtxnrms[i]);
             GL_VCOLOR (vtxclrs[i]);
             GL_VERTEX (vtxlocs[i]);
            }
         }
       else
         {
          for (i=0; i<vertexcount; i++)
            {
             GL_NORMAL (vtxnrms[i]);
             GL_VERTEX (vtxlocs[i]);
            }
         }
      }
    else
      {
       if (elenrms)
         {
          if (vtxclrs)
            {
             for (i=0; i<vertexcount; i++)
               {
                /* XXX Elem normals here */
                GL_VCOLOR (vtxclrs[i]);
                GL_VERTEX (vtxlocs[i]);
               }
            }
          else
            {
             for (i=0; i<vertexcount; i++)
               {
                /* XXX Elem normals here */
                GL_VERTEX (vtxlocs[i]);
               }
            }
         }
       else
         {
          if (vtxclrs)
            {
             for (i=0; i<vertexcount; i++)
               {
                GL_VCOLOR (vtxclrs[i]);
                GL_VERTEX (vtxlocs[i]);
               }
            }
          else
            {
             for (i=0; i<vertexcount; i++)
                GL_VERTEX (vtxlocs[i]);
            }
         }
      }

  glEnd ();


#else
  kglBeginBuf (GL_LINE_STRIP, vertexcount);

    if (vtxclrs)
       KGL_COLOR_BUF (vtxclrs, 3 * sizeof (glReal));

    if (vtxnrms)
       KGL_NORMAL_BUF (vtxnrms, 3 * sizeof (glReal));

    if (vtxlocs)
       KGL_VERTEX_BUF (vtxlocs, 3 * sizeof (glReal));

  kglEndBuf ();
#endif
 }



void ddr_kopengl_drom_render_triangle_list (DtColorModel   colormodel,
                                            DtRealTriple   bndboxpts[8],
                                            DtInt          trianglecount,
                                            DtRealTriple  *elenrms,
                                            DtRealTriple  *vtxlocs,
                                            DtRealTriple  *vtxnrms,
                                            DtRealTriple  *vtxclrs,
                                            DtInt          uv_count,
                                            DtRealCouple **uv_list,
                                            DtInt          uvw_count,
                                            DtRealTriple **uvw_list)
 {
  int i;
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ...in _drom_render_triangle_line_list\n");
  ddr_kopengl_drom_print_triangle_list (colormodel,bndboxpts,trianglecount,
                                        elenrms,
					vtxlocs,vtxnrms,vtxclrs,
					uv_count, uv_list, uvw_count, uvw_list);
#endif

  
  switch (DROM.reptyp)
    {
     case DcPoints:
       glPolygonMode (GL_FRONT_AND_BACK, GL_POINT);
       break;
     case DcWireframe:
     case DcOutlines:
       glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
       break;
     case DcSurface:
       glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
       break;
     case DcSurfaceWireframeEdges:
     case DcSurfaceOutlineEdges:
       glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
       break;
     default:
       break;
    }

#ifdef NO_BUFFERED_EXTENSIONS

  glBegin (GL_TRIANGLES);


    if (vtxnrms)
      {
       if (vtxclrs) 
         {  
          for (i=0; i<trianglecount*3; i++)
            {
             GL_NORMAL (vtxnrms[i]); 
             GL_VCOLOR (vtxclrs[i]);
             GL_VERTEX (vtxlocs[i]); 
            }
         }
       else
         {
          for (i=0; i<trianglecount*3; i++)
            {
             GL_NORMAL (vtxnrms[i]); 
             GL_VERTEX (vtxlocs[i]); 
            }
         }
      }
    else
      {
       if (elenrms)
         {
          if (vtxclrs)
            {
             for (i=0; i<trianglecount*3; i++)
               {
                if (!(i%3))
                  GL_NORMAL (elenrms[i/3]);
                GL_VCOLOR (vtxclrs[i]);
                GL_VERTEX (vtxlocs[i]); 
               }
            }
          else
            {
             for (i=0; i<trianglecount*3; i++)
               {
                if (!(i%3))
                  GL_NORMAL (elenrms[i/3]);
                GL_VERTEX (vtxlocs[i]); 
               }
            }
         }
       else
         {
          if (vtxclrs)
            {
             for (i=0; i<trianglecount*3; i++)
               {
                GL_VCOLOR (vtxclrs[i]);
                GL_VERTEX (vtxlocs[i]); 
               }
            }
          else
            {
             for (i=0; i<trianglecount*3; i++)
               {
                GL_VERTEX (vtxlocs[i]); 
               }
            }
         }
      }


      
    for (i=0; i<trianglecount*3; i++)
      {
       if (vtxnrms)
         GL_NORMAL (vtxnrms[i]); 
       if (vtxclrs)
         GL_VCOLOR (vtxclrs[i]);
       if (vtxlocs)
         GL_VERTEX (vtxlocs[i]); 
      }
   
  glEnd ();
  
#else
  kglBeginBuf (GL_TRIANGLES, 3 * trianglecount);

    if (vtxnrms)
      KGL_NORMAL_BUF (vtxnrms, 3 * sizeof (glReal));
/* XXX 
    else
      {
       if (elenrms)
         kglFacetNormalBuf (elenrms, 3 * sizeof (GLfloat));
      }
*/

    if (vtxclrs)
       KGL_COLOR_BUF (vtxclrs, 3 * sizeof (glReal));

    KGL_VERTEX_BUF (vtxlocs, 3 * sizeof (glReal));

  kglEndBuf ();
#endif
 }



void ddr_kopengl_drom_render_triangle_mesh (DtColorModel   colormodel,
                                            DtRealTriple   bndboxpts[8],
                                            DtInt          trianglecount,
                                            DtRealTriple  *elenrms,
                                            DtInt          vertexcount,
                                            DtRealTriple  *vtxlocs,
                                            DtRealTriple  *vtxnrms,
                                            DtRealTriple  *vtxclrs,
                                            DtInt          vertexlist[][3],
                                            DtInt    compiledvertexlist[][3],
                                            DtInt          uv_count,
                                            DtRealCouple **uv_list,
                                            DtInt          uvw_count,
                                            DtRealTriple **uvw_list)
 {
  int     i, j, k;
  glReal *datav, *vptr; 
  glReal *datan, *nptr;     
  glReal *datac, *cptr;     
  glReal *datae, *eptr; 
  DtInt   idx;


#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] ...in _drom_render_triangle_mesh_list\n");
  ddr_kopengl_drom_print_triangle_mesh (colormodel, bndboxpts, trianglecount, 
					elenrms, vertexcount,vtxlocs,vtxnrms,
					vtxclrs,vertexlist,
					compiledvertexlist,
					uv_count, uv_list, uvw_count, uvw_list);
#endif    

  /* Allocate temporary for data that will be walked */ 
  if (vtxlocs)
    vptr = datav = (glReal *) malloc (3*3*trianglecount*sizeof(glReal));
  if (vtxnrms)
    nptr = datan = (glReal *) malloc (3*3*trianglecount*sizeof(glReal));
  if (vtxclrs)
    cptr = datac = (glReal *) malloc (3*3*trianglecount*sizeof(glReal));
  if (elenrms)
    eptr = datae = (glReal *) malloc (3*trianglecount*sizeof(glReal));

  /* Set representation type */
  switch (DROM.reptyp)
    {
     case DcPoints:
       glPolygonMode (GL_FRONT_AND_BACK, GL_POINT);
       break;
     case DcWireframe:
     case DcOutlines:
       glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
       break;
     case DcSurface:
       glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
       break;
     case DcSurfaceWireframeEdges:
     case DcSurfaceOutlineEdges:
       glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
       break;
     default:
       break;
    }


  /* 
   * Convert the data from a mesh to a format for GL_TRIANGLES 
   */
  if (vtxnrms) /* If vertex normals specified check some more */
    {
     if (vtxclrs) /* If vertex normals && colors */
       {
        for (i=0; i<trianglecount; i++)
          for (j=0; j<3; j++)
            {
             idx = vertexlist[i][j];
             for (k=0; k<3; k++)
               {
                *datav++ = (vtxlocs)[idx][k];
                *datan++ = (vtxnrms)[idx][k];
                *datac++ = (vtxclrs)[idx][k];
               }
            }
       }
     else /* If vertex normals only (&& !colors) */
       {
        for (i=0; i<trianglecount; i++)
          for (j=0; j<3; j++)
            {
             idx = vertexlist[i][j];
             for (k=0; k<3; k++)
               {
                *datav++ = (vtxlocs)[idx][k];
                *datan++ = (vtxnrms)[idx][k];
               }
            }
       }
    }
  else /* If no vertex normals */
    {
     if (vtxclrs) /* If !vertex normals && colors */
       {
        if (elenrms) /* If elem normals specified */ 
          {
	   for (i=0; i<trianglecount; i++)
	     for (j=0; j<3; j++)
	       {
	        *datae++ = elenrms[i][j];
		idx = vertexlist[i][j];
		for (k=0; k<3; k++)
		  {
		   *datav++ = (vtxlocs)[idx][k];
		   *datac++ = (vtxclrs)[idx][k];
		  }
	       }
          }
        else
          {
	   for (i=0; i<trianglecount; i++)
	     for (j=0; j<3; j++)
	       {
		idx = vertexlist[i][j];
		for (k=0; k<3; k++)
		  {
		   *datav++ = (vtxlocs)[idx][k];
		   *datac++ = (vtxclrs)[idx][k];
		  }
	       }
          }
       } 
     else /* If !colors */
       {
        if (elenrms) /* If elem normals specified */ 
          {
	   for (i=0; i<trianglecount; i++)
	     for (j=0; j<3; j++)
	       {
	        *datae++ = elenrms[i][j];
		idx = vertexlist[i][j];
		for (k=0; k<3; k++)
		  *datav++ = (vtxlocs)[idx][k];
	       }
          }
        else /* If !elem normals */
          {
	   for (i=0; i<trianglecount; i++)
	     for (j=0; j<3; j++)
	       {
	        idx = vertexlist[i][j];
	        for (k=0; k<3; k++)
	          *datav++ = (vtxlocs)[idx][k];
	       }
          }
       }
    }
  /* Phew! Done walking the data !! */




#ifdef NO_BUFFERED_EXTENSIONS

  glBegin (GL_TRIANGLES);

    if (vtxnrms)
      {
       if (vtxclrs)
         {
          for (i=0; i<3*trianglecount; i++)
            {
             GL_NORMAL (&nptr[i*3]); 
             GL_VCOLOR (&cptr[i*3]);
             GL_VERTEX (&vptr[i*3]); 
            }
         }
       else
         {
          for (i=0; i<3*trianglecount; i++)
            {
             GL_NORMAL (&nptr[i*3]); 
             GL_VERTEX (&vptr[i*3]); 
            }
         }
      }
    else
      {
       if (elenrms)
         {
          if (vtxclrs)
            {
             for (i=0; i<3*trianglecount; i++)
               {
                if (!(i%3))
                  GL_NORMAL (&eptr[i]);
                GL_VCOLOR (&cptr[i*3]);
                GL_VERTEX (&vptr[i*3]); 
               }
            }
          else
            {
             for (i=0; i<3*trianglecount; i++)
               {
                if (!(i%3))
                  GL_NORMAL (&eptr[i]);
                GL_VERTEX (&vptr[i*3]); 
               }
            }
         }
       else
         {
          if (vtxclrs)
            {
             for (i=0; i<3*trianglecount; i++)
               {
                GL_VCOLOR (&cptr[i*3]);
                GL_VERTEX (&vptr[i*3]); 
               }
            }
          else
            {
             for (i=0; i<3*trianglecount; i++)
               GL_VERTEX (&vptr[i*3]); 
            }
         }
      }

  glEnd ();


#else

  /* Draw the triangles */
  kglBeginBuf (GL_TRIANGLES, 3*trianglecount);

    if (vtxclrs)
       KGL_COLOR_BUF (cptr, 3*sizeof (glReal));

    if (vtxnrms)
       KGL_NORMAL_BUF (nptr, 3*sizeof (glReal));
    else
      if (elenrms)
        kglFacetNormalBuf (eptr, 3*sizeof (glReal));

    if (vtxlocs)
      KGL_VERTEX_BUF (vptr, 3*sizeof (glReal));

  kglEndBuf ();

#endif


  /* Free memory that we allocated */
  if (vtxlocs)
    free (vptr);
  if (vtxnrms)
    free (nptr);
  if (vtxclrs)
    free (cptr);
  if (elenrms)
    free (eptr);
 }
