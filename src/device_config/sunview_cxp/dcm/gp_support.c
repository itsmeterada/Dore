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
	void noop ()
	void GP_clear_Zbuffer (gp)
	void GP_clear_window (gp, color, db)
	void GP_clear_Z_and_color (gp, color, xmin, ymin, xmax, ymax)
	void GP_rop_ff (gp, xsrc, ysrc, w, h, xdst, ydst)
	void GP_flush (gp)
	void GP_double_buffer (gp, flag)
	void GP_Zbuffer (gp, flag)
	void GP_switch_buffer (gp)
	void GP_matrix_concat (gp, mata, matb, matc)
	void GP_load_matrix (gp, mat3d, num)
	Rect * GP_get_cliplist (gp, count)
	void GP_set_cliplist (gp, count, rectlist)
	void allocbuf (gp)
	void reset_buf (gp)
	void GP_set_winview (gp, scale, translate)
	void loadcliplist (gp)
	void initcmds (gp)
	void GP_draw_3D_point_list (gp, pointcount, vtxlocs, colors, disp_type, shade_type)
	void GP_draw_3D_line_list (gp, linecount, vtxlocs, colors, disp_type, shade_type)
	void GP_draw_3D_connline_list (gp, vertexcount, vtxlocs, colors, disp_type, shade_type)
	void GP_draw_3D_triangle (gp, vtxlocs, clist, disp_type, shade_type)
	void GP_compute_3D_triangle (gp, vlist, clist, points, xform)
	void GP_draw_3D_polygon (gp, nbnds, npnts, vlist, clist, disp_type, shade_type)
	Notify_value GP_SIGXCPU_handler ()

  ======================================================================
 */
#include "dcm.h"

/*
 * Function proto-types for external routines
 */

/*
 * NOTE: defined in gp.h
 *
 * extern GP_data_ptr GP_init	ARGS( (Pixwin *, int, int, int, int) );
 * extern void GP_clear_Zbuffer	ARGS( (GP_data_ptr) );
 * extern void GP_clear_window 	ARGS( (GP_data_ptr, int, int) );
 * extern void GP_flush		ARGS( (GP_data_ptr) );
 * extern void GP_double_buffer	ARGS( (GP_data_ptr, int) );
 * extern void GP_Zbuffer	ARGS( (GP_data_ptr, int) );
 * extern void GP_switch_buffer	ARGS( (GP_data_ptr) );
 * extern void GP_draw_3D_polygon	ARGS( (GP_data_ptr, int, int[], 
 * 				       GP_point_3D *, GP_color *, 
 * 				       GP_display_type, GP_shade_type) );
 * extern void GP_matrix_concat	ARGS( (GP_data_ptr, int, int, int) );
 * extern void GP_load_matrix	ARGS( (GP_data_ptr, Matrix *, short) );
 * extern Rect *GP_get_cliplist	ARGS( (GP_data_ptr, int *) );
 * extern void GP_set_cliplist	ARGS( (GP_data_ptr, DtInt, DtReal[][4]) );
 * 
 */

/*
 * Function proto-types for internal routines
 */

static void allocbuf ARGS( (GP_data_ptr) );
static void reset_buf ARGS( (GP_data_ptr) );
static void winview ARGS( (GP_data_ptr) );
static void loadcliplist ARGS( (GP_data_ptr) );
static void initcmds ARGS( (GP_data_ptr) );

#define GPMAXCLIP	60	/* maximum number of clip rectangles */
#define GP_BYTESPERBLOCK 1024	/* # bytes in GP buffer block */

#define DPRINTF if (GP_debug) fprintf

int GP_debug = 0;

static int GP_restart_count = 0;
static int GP_setup_complete = 0;
static int GP_ioctl_fd = 0;
static int *GP_signal_client = &GP_restart_count;

static void noop() {};
/*
  ======================================================================
 */
GP_data_ptr
GP_init (pw, n_buffers, Zbuffer, double_buffer, n_color_planes, func)
     Pixwin *pw;
     int n_buffers;
     int Zbuffer;
     int double_buffer;
     int n_color_planes;
     void (*func)();
{
   GP_data_ptr gp;

   /*
    * If the pro_rop field of the pixrect associated with the pixwin is 
    * "gp1_rop" we have a GP and can access it.
    */

   if (pw->pw_pixrect->pr_ops->pro_rop != gp1_rop) {
      fprintf (stderr, "Cannot access GP - is there one ?\n");
      return NULL;
   }

   if ((gp = (GP_data_ptr) malloc (sizeof (GP_data_type))) == NULL) {
      fprintf (stderr, "Could not allocate memory\n");
      return NULL;
   }

   gp->pw = pw;
   gp->ioctl_fd = gp1_d(pw->pw_pixrect)->ioctl_fd;

   if (ioctl (gp->ioctl_fd, GP1IO_GET_TRUMINORDEV, &gp->mindev) < 0) {
      perror ("Could not get minor device for GP2");
      free ((char *)gp);
      return NULL;
   }

   /*
    * Find out where the GP shared memory is: this is where the GP buffers
    * are and gp1_alloc/gp1_post refer to it.  Allocate the GP buffer and
    * initialize viewing, clipping.
    */

   gp->shmem = (short *) gp1_d(pw->pw_clipdata->pwcd_prmulti)->gp_shmem;

   /*
    * The operations in each GP buffer are associated with a static block
    * (graphics context) withc remembers things like color, raster-op, etc.
    * We allocate a static block to use.  The GP buffer will start with a 
    * USE_CONTEXT command specifying this static block.
    */

   if ((gp->statblk = gp1_get_static_block (gp->ioctl_fd)) < 0) {
      perror ("Could not get static block for GP2");
      free ((char *)gp);
      return NULL;
   }

   if (!GP_setup_complete) {
      extern Notify_value GP_SIGXCPU_handler();

      GP_ioctl_fd = gp->ioctl_fd;

      notify_set_signal_func (GP_signal_client, GP_SIGXCPU_handler, 
			      SIGXCPU, NOTIFY_ASYNC);
      GP_restart_count = gp1_get_restart_count (GP_ioctl_fd);
      GP_setup_complete = 1;
   }

   gp->n_buffers = n_buffers;

   gp->cmd_offset = 0;
   gp->cmd_bitvec = 0;
   gp->cmd_ptr = NULL;
   gp->cmd_left = 0;

   gp->clipid = 0;
   if (func == NULL) {
      gp->func = noop;
   } else {
      gp->func = func;
   }

   gp->current_matrix = 0;
   gp->Zbuffer_on = Zbuffer;
   gp->double_buffer_on = 0;
   gp->double_buffer_pending = double_buffer;
   gp->color_planes = n_color_planes;
   gp->setup_pending = 1;
   gp->restart_count = GP_restart_count;
   allocbuf(gp);
   initcmds(gp);
   
   return gp;

} /* GP_init */
/*
  ======================================================================
 */
void
GP_clear_Zbuffer(gp)
     GP_data_ptr gp;
{
   register int wfd;
   register struct pixwin_prlist *prl;
   register struct gp1pr *dmd;

   wfd = gp->pw->pw_clipdata->pwcd_windowfd;
   win_getscreenposition(wfd, &(gp->win_org.x), &(gp->win_org.y));
   win_getsize (wfd, &(gp->win_size));

   if ((gp->cmd_left -= 6 * sizeof (short)) <= 0) {
      GP_flush (gp);
      gp->cmd_left -= 6 * sizeof (short);
   }

   DPRINTF(stderr,"GP1_set_ZBUF\n");
   *(gp->cmd_ptr)++ = GP1_SET_ZBUF;
   
   *(gp->cmd_ptr) ++ = 65535;
   *(gp->cmd_ptr) ++ = gp->win_org.x;
   *(gp->cmd_ptr) ++ = gp->win_org.y;
   *(gp->cmd_ptr) ++ = gp->win_size.r_width;
   *(gp->cmd_ptr) ++ = gp->win_size.r_height;

} /* GP_clear_Zbuffer */
/*
  ======================================================================
 * Clears the entire GP window to the given color.
 *
 * Notes:
 * To clear the window, the GP1_PR_ROP_NF command is used.  This command is 
 * designed to not require a static block (graphics contex) so all of the
 * operands must be specified.  It also does not clip to the clip list (which is
 * in the static block) so it must be performed for each rectangle in
 * clip list.
 *
 * The coordinates of the box are given with respect to the window.
 * The coordinates of each clip list rectangle are with respect to
 * the entire screen (device coordinates).  PRROPNF wants the box coordinates
 * to be with respect to the clip rectangle.
 *
 * It has been observed that the GP1_PR_xxx commands do not co-exist with the
 * GP1_XF_xxx commands in the smae buffer.  Therefore, we flush the
 * buffer before and after the clear.  If this is ever fixed, we can remove
 * the redundant flushes.
 */
void
GP_clear_window(gp, color, db)
     GP_data_ptr gp;
     int color;
     int db;
{
   register int wfd;
   register struct pixwin_prlist *prl;
   register struct gp1pr *dmd;
   int which_buffer;

   if (gp->Zbuffer_on) {
      GP_clear_Zbuffer (gp);
   }

   if (gp->double_buffer_on && db) {
      gp1_sync(gp->shmem, gp->ioctl_fd);
      which_buffer = pw_dbl_get (gp->pw, PW_DBL_WRITE);
      pw_dbl_set (gp->pw, PW_DBL_WRITE, PW_DBL_BOTH, 0);
   }

   pw_lock (gp->pw, &(gp->win_size));
      
   dmd = gp1_d (gp->pw->pw_clipdata->pwcd_prmulti);
   prl = gp->pw->pw_clipdata->pwcd_prl;
   while (prl) {
      if ((gp->cmd_left -= 11 * sizeof(short)) <= 0) {
	 GP_flush (gp);
	 gp->cmd_left -= 11 * sizeof (short);
      }
      DPRINTF(stderr, "GP1_PR_ROP_NF | %d\n", gp->color_planes);
      *(gp->cmd_ptr) ++ = GP1_PR_ROP_NF | gp->color_planes;
      *(gp->cmd_ptr) ++ = dmd->cg2_index;
      *(gp->cmd_ptr) ++ = PIX_SRC | PIX_COLOR(color);
      dmd = gp1_d(prl->prl_pixrect);
      *(gp->cmd_ptr) ++ = dmd->cgpr_offset.x;
      *(gp->cmd_ptr) ++ = dmd->cgpr_offset.y;
      *(gp->cmd_ptr) ++ = prl->prl_pixrect->pr_size.x;
      *(gp->cmd_ptr) ++ = prl->prl_pixrect->pr_size.y;
      *(gp->cmd_ptr) ++ = 0;
      *(gp->cmd_ptr) ++ = 0;
      *(gp->cmd_ptr) ++ = prl->prl_pixrect->pr_size.x;
      *(gp->cmd_ptr) ++ = prl->prl_pixrect->pr_size.y;
      prl = prl->prl_next;
   }
   GP_flush(gp);
   pw_unlock(gp->pw);

   if (gp->double_buffer_on && db) {
      gp1_sync(gp->shmem, gp->ioctl_fd);
      pw_dbl_set (gp->pw, PW_DBL_WRITE, which_buffer, 0);
   }

} /* GP_clear_window */
/*
  ======================================================================
 */
void
GP_clear_Z_and_color(gp, color, xmin, ymin, xmax, ymax)
     GP_data_ptr gp;
     short color;
     short xmin;
     short ymin;
     short xmax;
     short ymax;
{
   if ((gp->cmd_left -= 17 * sizeof (short)) <= 0) {
      GP_flush (gp);
      gp->cmd_left -= 17 * sizeof (short);
   }

   DPRINTF(stderr,"GP1_SET_ZBUF\n");
   *(gp->cmd_ptr)++ = GP1_SET_ZBUF;
   
   *(gp->cmd_ptr) ++ = 65535;
   *(gp->cmd_ptr) ++ = xmin;
   *(gp->cmd_ptr) ++ = ymin;
   *(gp->cmd_ptr) ++ = xmax - xmin;
   *(gp->cmd_ptr) ++ = ymax - ymin;

   DPRINTF(stderr,"GP1_PR_ROP_NF\n");
   *(gp->cmd_ptr) ++ = GP1_PR_ROP_NF | gp->color_planes;
   *(gp->cmd_ptr) ++ = gp1_d (gp->pw->pw_clipdata->pwcd_prmulti)->cg2_index;
   *(gp->cmd_ptr) ++ = PIX_SRC | PIX_COLOR(color);
   *(gp->cmd_ptr) ++ = xmin;
   *(gp->cmd_ptr) ++ = ymin;
   *(gp->cmd_ptr) ++ = xmax - xmin;
   *(gp->cmd_ptr) ++ = ymax - ymin;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = xmax - xmin;
   *(gp->cmd_ptr) ++ = ymax - ymin;

} /* GP_clear_Z_and_color */
/*
  ======================================================================
 */
void
GP_rop_ff(gp, xsrc, ysrc, w, h, xdst, ydst)
     GP_data_ptr gp;
     short xsrc;
     short ysrc;
     short w;
     short h;
     short xdst;
     short ydst;
{
   if ((gp->cmd_left -= 17 * sizeof (short)) <= 0) {
      GP_flush (gp);
      gp->cmd_left -= 17 * sizeof (short);
   }

   DPRINTF(stderr,"GP1_PR_ROP_FF\n");
   *(gp->cmd_ptr) ++ = GP1_PR_ROP_FF | gp->color_planes;
   *(gp->cmd_ptr) ++ = gp1_d (gp->pw->pw_clipdata->pwcd_prmulti)->cg2_index;
   *(gp->cmd_ptr) ++ = PIX_SRC;
   *(gp->cmd_ptr) ++ = xdst;
   *(gp->cmd_ptr) ++ = ydst;
   *(gp->cmd_ptr) ++ = w;
   *(gp->cmd_ptr) ++ = h;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = w;
   *(gp->cmd_ptr) ++ = h;
   *(gp->cmd_ptr) ++ = xsrc;
   *(gp->cmd_ptr) ++ = ysrc;
   *(gp->cmd_ptr) ++ = w;
   *(gp->cmd_ptr) ++ = h;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = 0;

} /* GP_rop_ff */
/*
  ======================================================================
 * Flush GP buffer commands.  An EOCL command is appended to the end
 * of  the buffer with the appropriate bit vectors (saved by allocbuf)
 * to insure that the GP will free the buffer when it has finished.
 * The screen area to be affedted by the GP must be locked before
 * posting and unlocked afterwards.
 *
 * To handle window size and/or position changes, we examine the clip ID
 * in the pixwin before a GP buffer is posted.  If the window has changed,
 * the clip ID will be different.  This signals us to reload the clip list
 * and recompute viewing parameters.
 */
void
GP_flush(gp)
     GP_data_ptr gp;
{
   if (gp->double_buffer_pending) {
      GP_double_buffer(gp, 1);
      gp->double_buffer_pending = 0;
   }

   /*
    * End current block and allocate new one.  Tell GP to free this buffer
    * when it is finished executing the commands.
    */
   DPRINTF (stderr, "GP1_EOCL | GP1_FREEBLKS\n");
   *(gp->cmd_ptr) ++ = GP1_EOCL | GP1_FREEBLKS;
   GP1_PUT_I ((gp->cmd_ptr), gp->cmd_bitvec);

   if (gp->restart_count != GP_restart_count) {
      int oldofs;
      int bitvec;
      short *cmdptr;
      int left;

      oldofs = gp->cmd_offset;
      bitvec = gp->cmd_bitvec;
      cmdptr = gp->cmd_ptr;
      left = gp->cmd_left;

      allocbuf (gp);
      initcmds (gp);
      
      DPRINTF (stderr, "GP1_EOCL | GP1_FREEBLKS\n");
      *(gp->cmd_ptr) ++ = GP1_EOCL | GP1_FREEBLKS;
      GP1_PUT_I ((gp->cmd_ptr), gp->cmd_bitvec);
      gp1_post (gp->shmem, gp->cmd_offset, gp->ioctl_fd);

      gp->cmd_offset = oldofs;
      gp->cmd_bitvec = bitvec;
      gp->cmd_ptr = cmdptr;
      gp->cmd_left = left;

      gp->restart_count = GP_restart_count;
      gp->setup_pending = 0;

      /* make sure that the clip list gets updated */
      gp->clipid = 0;
      (*(gp->func))(gp); /* notify of clip list change */
   }

   /*
    * Check the clip Id to see if it has changed.  If so, the clip list has
    * to be reloaded because the window position and/or size has changed and
    * we don't want to draw outside of our window.
    */

   pw_lock (gp->pw, &(gp->win_size));
   if (gp->clipid != gp->pw->pw_clipdata->pwcd_clipid) {
      if (!gp->setup_pending) {
	 reset_buf(gp);
	 pw_unlock (gp->pw);
	 (*(gp->func))(gp); /* notify of clip list change */
	 return;
      } else {
	 (*(gp->func))(gp); /* notify of clip list change */
	 gp->setup_pending = 0;
      }
   }
   DPRINTF (stderr,"gp1_post\n");
   gp1_post (gp->shmem, gp->cmd_offset, gp->ioctl_fd);
   pw_unlock(gp->pw);
   allocbuf(gp);

} /* GP_flush */
/*
  ======================================================================
 */
void
GP_double_buffer(gp, flag)
     GP_data_ptr gp;
     int flag;
{
   if (flag && !gp->double_buffer_on) {
      gp->double_buffer_on = 1;
      DPRINTF(stderr,"pw_dbl_access\n");
      pw_dbl_access (gp->pw);
   } else if (!flag && gp->double_buffer_on) {
      gp->double_buffer_on = 0;
      DPRINTF(stderr,"pw_dbl_release\n");
      pw_dbl_release (gp->pw);
   }
}
/*
  ======================================================================
 */
void
GP_Zbuffer (gp, flag)
     GP_data_ptr gp;
     int flag;
{
   if (flag && !gp->Zbuffer_on) {
      gp->Zbuffer_on = 1;
      if ((gp->cmd_left -= sizeof (short)) <= 0) {
	 GP_flush (gp);
	 gp->cmd_left -= sizeof (short);
      }
      DPRINTF (stderr, "GP1_SET_HIDDEN_SURF | GP1_ZBHIDDENSURF\n");
      *(gp->cmd_ptr) ++ = GP1_SET_HIDDEN_SURF | GP1_ZBHIDDENSURF;

   } else if (!flag && gp->Zbuffer_on) {
      gp->Zbuffer_on = 0;
      if ((gp->cmd_left -= sizeof (short)) <= 0) {
	 GP_flush (gp);
	 gp->cmd_left -= sizeof (short);
      }
      DPRINTF(stderr, "GP1_SET_HIDDEN_SURF | GP1_NOHIDDENSURF\n");
      *(gp->cmd_ptr) ++ = GP1_SET_HIDDEN_SURF | GP1_NOHIDDENSURF;
   }
}
/*
  ======================================================================
 */
void
GP_switch_buffer (gp)
     GP_data_ptr gp;
{
   if (gp->double_buffer_on) {
      GP_flush (gp);
      gp1_sync(gp->shmem, gp->ioctl_fd);
      DPRINTF(stderr, "pw_dbl_flip\n");
      pw_dbl_flip(gp->pw);
   }
}
/*
  ======================================================================
 */
void
GP_matrix_concat(gp, mata, matb, matc)
     GP_data_ptr gp;
     int mata, matb, matc;
{
   if ((gp->cmd_left -= 4 * sizeof (short)) <= 0) {
      GP_flush(gp);
      gp->cmd_left -= 4 * sizeof (short);
   }

   DPRINTF (stderr, "GP1_MUL_MAT_3D\n");
   *(gp->cmd_ptr)++ = GP1_MUL_MAT_3D;
   *(gp->cmd_ptr)++ = (short)mata;
   *(gp->cmd_ptr)++ = (short)matb;
   *(gp->cmd_ptr)++ = (short)matc;

} /* GP_matrix_concat */
/*
  ======================================================================
 */
void
GP_load_matrix (gp, mat3d, num)
     GP_data_ptr gp;
     dot_matrix_4x4 mat3d;
     DtShort num;
{
   register int i;
   register int j;
   float t;

   if ((gp->cmd_left -= 2 * sizeof (short) + 16 * sizeof (float)) <= 0) {
      GP_flush(gp);
      gp->cmd_left -= 2 * sizeof (short) + 16 * sizeof (float);
   }

   DPRINTF (stderr, "GP1_SET_MAT_3D | %d\n", num);
   *(gp->cmd_ptr)++ = GP1_SET_MAT_3D | (short)num;
   for (i=0; i<4;i++) {
      for (j=0; j<4;j++) {
	 t = (float) (mat3d)[i][j];
	 GP1_PUT_F ((gp->cmd_ptr), t);
      }
   }
} /* GP_load_matrix */
/*
  ======================================================================
 */
Rect *
GP_get_cliplist (gp, count)
     GP_data_ptr gp;
     int *count;
{
   struct pixwin_prlist *prl;
   Rect *list;
   Rect *ptr;
   int n;
   struct gp1pr *dmd;
   int wfd;
   int id;

   wfd = gp->pw->pw_clipdata->pwcd_windowfd;
   win_getscreenposition(wfd, &(gp->win_org.x), &(gp->win_org.y));
   win_getsize (wfd, &(gp->win_size));

   pw_lock (gp->pw, &(gp->win_size));
   if (gp->clipid != (id = gp->pw->pw_clipdata->pwcd_clipid)) {
      if (gp->setup_pending) {
	    GP_flush (gp);
	 } else {
	    reset_buf(gp);
	 }
      loadcliplist(gp);
      DPRINTF (stderr, "GP1_EOCL | GP1_FREEBLKS\n");
      *(gp->cmd_ptr)++ = GP1_EOCL | GP1_FREEBLKS;
      GP1_PUT_I ((gp->cmd_ptr), gp->cmd_bitvec);
      gp1_post (gp->shmem, gp->cmd_offset, gp->ioctl_fd);
      gp->clipid = id;
      allocbuf(gp);
   }

   for (n=0, prl = gp->pw->pw_clipdata->pwcd_prl;
	prl;
	n++, prl = prl->prl_next);
   *count = n;

   list = (Rect *)malloc(n*sizeof(Rect));
   if (list == NULL) {
      fprintf (stderr, "Could not allocate space for cliplist\n:");
      *count = 0;
      return NULL;
   }

   for (ptr = list, prl = gp->pw->pw_clipdata->pwcd_prl; 
	prl; 
	ptr++, prl = prl->prl_next) {
      dmd = gp1_d(prl->prl_pixrect);
      ptr->r_left = dmd->cgpr_offset.x;
      ptr->r_top = dmd->cgpr_offset.y;
      ptr->r_width = prl->prl_pixrect->pr_size.x;
      ptr->r_height = prl->prl_pixrect->pr_size.y;
   }
   pw_unlock(gp->pw);

   return list;
}
/*
  ======================================================================
 */
void
GP_set_cliplist (gp, count, rectlist)
     GP_data_ptr gp;
     DtInt count;
     DtReal rectlist[][4];
{
   int i;

   if ((gp->cmd_left -= (2 + (4*count)) * sizeof (short)) <= 0) {
      GP_flush(gp);
      gp->cmd_left -= (2 + (4*count)) * sizeof (short);
   }

   DPRINTF (stderr, "GP1_SET_CLIP_LIST\n");
   *(gp->cmd_ptr) ++ = GP1_SET_CLIP_LIST;
   *(gp->cmd_ptr)++ = (short)count;

   for (i=0; i<count; i++) {
      *(gp->cmd_ptr) ++ = (short) (rectlist[i][0]);
      *(gp->cmd_ptr) ++ = (short) (rectlist[i][1]);
      *(gp->cmd_ptr) ++ = (short) (rectlist[i][2] - rectlist[i][0]);
      *(gp->cmd_ptr) ++ = (short) (rectlist[i][3] - rectlist[i][1]);
   }
}
/*
  ======================================================================
 * allocbuf()
 * Allocates a new GP buffer of size GP_blocks.  An initial USE_CONTEXT
 * command that reference static block GP_statblk is put at the beginning
 * of the buffer.
 *
 * returns:
 *  (gp->cmd_ptr) -> next available slot in new buffer
 *  GP_left = # bytes left in buffer.
 *
 * Buffers look like:
 * 2 bytes GP1_USE_CONTEXT | static block index
 *       ...
 * 2 bytes GL1_EOCL | GP1_FREEBLKS
 * -------
 * 8 bytes
 */

static void
allocbuf(gp)
     GP_data_ptr gp;
{
   static int num_allocs = 0;
   static int sleep_cnt = 0;
   int cnt = 0;

   while ((gp->cmd_offset = gp1_alloc (gp->shmem, gp->n_buffers, 
				       &(gp->cmd_bitvec), 
				       gp->mindev, gp->ioctl_fd)) == 0) {
      if (++cnt > 1000) {
	 sleep_cnt++;
	 fprintf (stderr, "Dore_GP: could not allocate a buffer a 1000 tries\n");
	 fprintf (stderr, "Trying to restart GP2\n");
	 gp1_kern_gp_restart (gp->ioctl_fd, 1);
	 cnt = 0;
      }
   }
   (gp->cmd_ptr) = &((short *) gp->shmem)[gp->cmd_offset];
   DPRINTF (stderr, "GP1_USE_CONTEXT | %d\n", gp->statblk);
   *(gp->cmd_ptr)++ = GP1_USE_CONTEXT | gp->statblk;
   gp->cmd_left = gp->n_buffers * GP_BYTESPERBLOCK - 8;

   num_allocs++;

} /* allocbuf */
/*
  ======================================================================
 */
static void
reset_buf(gp)
     GP_data_ptr gp;
{
   DPRINTF (stderr, "--- RESET BUFFER ---\n");
   (gp->cmd_ptr) = &((short *) gp->shmem)[gp->cmd_offset];
   DPRINTF (stderr, "GP1_USE_CONTEXT | %d\n", gp->statblk);
   *(gp->cmd_ptr)++ = GP1_USE_CONTEXT | gp->statblk;
   gp->cmd_left = gp->n_buffers * GP_BYTESPERBLOCK - 8;
}
/*
  ======================================================================
 * winview()
 * Set window viewing for GP.  We assume our coordinate space has 
 * (-1,1,0) in the upper left corner, (0,0,0) in the center and (1,-1,1)
 * in the lower right corner.  This describes the GP clipping area.  We
 * want to map thsi space onto a pixwin with the 0,0 as the upper left
 * corner and WIDTH, HEIGHT as the lower right corner.  The SEP_VWP
 * command allows us to specify X,Y,Z offsets and scale factors.
 *
 * If U is a point in user psace and W is a point in window space, we
 * use the following transformation:
 *   W = U * SF + OFS
 * where SF are the X,Y,Z scale factors and OFS represents the offsets.
 * We wand (0,0) from user space to be mapped to (WIDTH / 2, HIEGHT / 2)
 * within the window and (-1,1) from user space to be mapped to (0,0).
 * This means our scale factors must be:
 *   SFx = WIDTH / 2, SFy = -HIEGHT / 2, SFz = 1 (no transform)
 * and our offsets must be:
 *   OFSx = WIDTH / 2, OFSy = HIEGHT / 2, OFSz = 0
 * Because the GP maps to phusical screen coordinates and not window
 * coordinates, we have to add in the position of the upper left corner
 * of the window to OFSx and OFSy.
 *
 * This routine assumes it will be called with an empty buffer - it
 * doews not check for overflow.
 */
void
GP_set_winview(gp, scale, translate)
     GP_data_ptr gp;
     DtReal scale[3];
     DtReal translate[3];
{
   int wfd;
   int x, y;
   float xsf, ysf, wsf;
   float tmp;
   register int i;

   DPRINTF (stderr, "GP1_set_VWP_3D\n");
   *(gp->cmd_ptr) ++ = GP1_SET_VWP_3D;
   for (i=0; i<3; i++) {
      tmp = scale[i];
      GP1_PUT_F ((gp->cmd_ptr), tmp);
      tmp = translate[i];
      GP1_PUT_F ((gp->cmd_ptr), tmp);
   }
   gp->cmd_left -= sizeof (short) + 6 * sizeof (float);

} /* winview */
/*
  ======================================================================
 * loadcliplist()
 * Loads the clip list for the pixwin into the GP.  This routine assumes
 * we start with an empty buffer and all the clip rectangles will fit
 * so it doesn't check for overflow.  The clip list is obtained from
 * internal pixwin data structures.  This clip rectangles are in PHYSICAL
 * SCREEN COORDINATES.
 *
 * It is assumed that this routine will be called with a new buffer,
 * we do not check for buffer overflow.
 */
static void
loadcliplist(gp)
     GP_data_ptr gp;
{
   struct pixwin_prlist *prl;
   short *rectptr;
   int n;
   struct gp1pr *dmd;

   DPRINTF (stderr, "GP1_SET_CLIP_LIST\n");
   *(gp->cmd_ptr) ++ = GP1_SET_CLIP_LIST;
   rectptr = (gp->cmd_ptr)++;
   prl = gp->pw->pw_clipdata->pwcd_prl;
   n = 0;
   while (prl && (n < GPMAXCLIP)) {
      ++n;
      dmd = gp1_d(prl->prl_pixrect);
      *(gp->cmd_ptr) ++ = dmd->cgpr_offset.x;
      *(gp->cmd_ptr) ++ = dmd->cgpr_offset.y;
      *(gp->cmd_ptr) ++ = prl->prl_pixrect->pr_size.x;
      *(gp->cmd_ptr) ++ = prl->prl_pixrect->pr_size.y;
      prl = prl->prl_next;
   }
   *rectptr = n;
   gp->cmd_left -= (4 * n + 2) * sizeof (short);

} /* loadcliplist */
/*
  ======================================================================
 * initcmds()
 * Initialize GP parameters.  The GP WILL NOT DRAW if you don't
 * initialize everything properly.  This routine properly handles
 * buffering (updates (gp->cmd_ptr) and gp->cmd_left).
 */
static void
initcmds(gp)
     GP_data_ptr gp;
{
   register float *mat;
   register int i;

   static float identity3d[4][4] = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0 };

   DPRINTF (stderr, "GP1_SET_FB_NUM | %d\n", 
	    gp1_d (gp->pw->pw_clipdata->pwcd_prmulti)->cg2_index);
   *(gp->cmd_ptr) ++ = GP1_SET_FB_NUM | 
	 gp1_d (gp->pw->pw_clipdata->pwcd_prmulti)->cg2_index;
   DPRINTF (stderr, "GP1_SET_FB_PLANES | %d\n", gp->color_planes);
   *(gp->cmd_ptr) ++ = GP1_SET_FB_PLANES | gp->color_planes;
   DPRINTF (stderr, "GP1_SET_COLOR | 1\n");
   *(gp->cmd_ptr) ++ = GP1_SET_COLOR | 1;
   DPRINTF (stderr, "GP1_SET_ROP\n");
   *(gp->cmd_ptr) ++ = GP1_SET_ROP;
   *(gp->cmd_ptr) ++ = PIX_SRC;
   DPRINTF (stderr, "GP1_SET_CLIP_PLANES | GP1_CLIP_PLANE_HITHER | GP1_CLIP_YON\n");
/*   *(gp->cmd_ptr) ++ = GP1_SET_CLIP_PLANES; */
   *(gp->cmd_ptr) ++ = GP1_SET_CLIP_PLANES | GP1_CLIP_PLANE_HITHER | GP1_CLIP_PLANE_YON; 
   if (gp->Zbuffer_on) {
      DPRINTF (stderr, "GP1_SET_HIDDEN_SURF | GP1_ZBHIDDENSURF\n");
      *(gp->cmd_ptr) ++ = GP1_SET_HIDDEN_SURF | GP1_ZBHIDDENSURF;
   } else {
      DPRINTF (stderr, "GP1_SET_HIDDEN_SURF | GP1_NOHIDDENSURF\n");
      *(gp->cmd_ptr) ++ = GP1_SET_HIDDEN_SURF | GP1_NOHIDDENSURF;
   }
   DPRINTF (stderr, "GP1_SET_MAT_NUM | %d\n", gp->current_matrix);
   *(gp->cmd_ptr) ++ = GP1_SET_MAT_NUM | gp->current_matrix;
   DPRINTF (stderr, "GP1_SET_MAT_3D | %d\n", gp->current_matrix);
   *(gp->cmd_ptr) ++ = GP1_SET_MAT_3D | gp->current_matrix;
   mat = &identity3d[0][0];
   i = sizeof (identity3d) / sizeof(float);
   while (--i >= 0) { GP1_PUT_F((gp->cmd_ptr), *mat); mat++; }
   DPRINTF (stderr, "GP1_SET_LINE_WIDTH\n");
   *(gp->cmd_ptr) ++ = GP1_SET_LINE_WIDTH;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = 0;
   DPRINTF (stderr, "GP1_SET_LINE_TEX\n");
   *(gp->cmd_ptr) ++ = GP1_SET_LINE_TEX;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = 0;
   DPRINTF (stderr, "GP1_SET_PGON_TEX_ORG_SCR\n");
   *(gp->cmd_ptr) ++ = GP1_SET_PGON_TEX_ORG_SCR;
   *(gp->cmd_ptr) ++ = 0;
   *(gp->cmd_ptr) ++ = 0;
   gp->cmd_left -= 20 * sizeof (short) + sizeof (identity3d);

} /* initcmds */
/*
  ======================================================================
 */
void
GP_draw_3D_point_list (gp, pointcount, vtxlocs, colors, disp_type, shade_type)
     GP_data_ptr gp;
     DtInt pointcount;
     DtRealTriple *vtxlocs;
     DtInt *colors;
     DtRepType disp_type;
     DtInterpType shade_type;
{
   int i, j, k, start;
   int cnt;
   float tmp;

   switch (disp_type) {
   case DcPoints:
   case DcWireframe:
   case DcSurface: 
   case DcSurfaceWireframeEdges: 
   case DcSurfaceOutlineEdges: 
   case DcOutlines:
      for (i=0; i<pointcount; i++) {
	 if ((gp->cmd_left -= (4)*sizeof(short) + (6)*sizeof(float))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (4)*sizeof(short) + (6)*sizeof(float);
	 }

	 switch (shade_type) {
	 case DcConstantShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i];
	    break;
	 case DcVertexShade:
	 case DcSurfaceShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i];
	    break;
	 }
	 DPRINTF (stderr, "GP1_XF_LINE_FLT_3D\n");
	 *(gp->cmd_ptr)++ = GP1_XF_LINE_FLT_3D;

	 *(gp->cmd_ptr)++ = 1;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[i][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
	 *(gp->cmd_ptr)++ = 0x8000;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[i][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
      }
      break;
   }
} /* GP_draw_3D_point_list */
/*
  ======================================================================
 */
void
GP_draw_3D_line_list (gp, linecount, vtxlocs, colors, disp_type, shade_type)
     GP_data_ptr gp;
     DtInt linecount;
     DtRealTriple *vtxlocs;
     DtInt *colors;
     DtRepType disp_type;
     DtInterpType shade_type;
{
   int i, j, k, start;
   int cnt;
   float tmp;

   switch (disp_type) {
   case DcPoints:
      for (i=0; i<linecount; i++) {
	 if ((gp->cmd_left -= (6)*sizeof(short) + (12)*sizeof(float))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (6)*sizeof(short) + (12)*sizeof(float);
	 }

	 switch (shade_type) {
	 case DcConstantShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i];
	    break;
	 case DcVertexShade:
	 case DcSurfaceShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i*2]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i*2];
	    break;
	 }
	 DPRINTF (stderr, "GP1_XF_LINE_FLT_3D\n");
	 *(gp->cmd_ptr)++ = GP1_XF_LINE_FLT_3D;

	 for (j=0; j<2; j++) {
	    *(gp->cmd_ptr)++ = 1;
	    for (k=0; k<3; k++) {
	       tmp = vtxlocs[i*2+j][k];
	       GP1_PUT_F ((gp->cmd_ptr), tmp);
	    }
	    if (j == 1) {
	       *(gp->cmd_ptr)++ = 0x8000;
	    } else {
	       *(gp->cmd_ptr)++ = 0;
	    }
	    for (k=0; k<3; k++) {
	       tmp = vtxlocs[i*2+j][k];
	       GP1_PUT_F ((gp->cmd_ptr), tmp);
	    }
	 }
      }
      break;

   case DcWireframe:
   case DcOutlines:
   case DcSurface: 
   case DcSurfaceWireframeEdges: 
   case DcSurfaceOutlineEdges: 
      for (i=0; i<linecount; i++) {
	 if ((gp->cmd_left -= (4)*sizeof(short) + (6)*sizeof(float))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (4)*sizeof(short) + (6)*sizeof(float);
	 }

	 switch (shade_type) {
	 case DcConstantShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i];
	    break;
	 case DcVertexShade:
	 case DcSurfaceShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i*2]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i*2];
	    break;
	 }
	 DPRINTF (stderr, "GP1_XF_LINE_FLT_3D\n");
	 *(gp->cmd_ptr)++ = GP1_XF_LINE_FLT_3D;

	 *(gp->cmd_ptr)++ = 1;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[i*2][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
	 *(gp->cmd_ptr)++ = 0x8000;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[i*2+1][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
      }
      break;
   }

} /* GP_draw_3D_line_list */
/*
  ======================================================================
 */
void
GP_draw_3D_connline_list (gp, vertexcount, vtxlocs, colors, disp_type, shade_type)
     GP_data_ptr gp;
     DtInt vertexcount;
     DtRealTriple *vtxlocs;
     DtInt *colors;
     DtRepType disp_type;
     DtInterpType shade_type;
{
   int i, j, k, start;
   int cnt;
   float tmp;

   switch (disp_type) {
   case DcPoints:
      for (i=0; i<vertexcount; i++) {
	 if ((gp->cmd_left -= (4)*sizeof(short) + (6)*sizeof(float))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (4)*sizeof(short) + (6)*sizeof(float);
	 }

	 switch (shade_type) {
	 case DcConstantShade:
	    if (i == vertexcount-1) {
	       DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i-1]);
	       *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i-1];
	    } else {
	       DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i]);
	       *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i];
	    }
	    break;
	 case DcVertexShade:
	 case DcSurfaceShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i];
	    break;
	 }
	 DPRINTF (stderr, "GP1_XF_LINE_FLT_3D\n");
	 *(gp->cmd_ptr)++ = GP1_XF_LINE_FLT_3D;

	 *(gp->cmd_ptr)++ = 1;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[i][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
	 *(gp->cmd_ptr)++ = 0x8000;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[i][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
      }
      break;

   case DcWireframe:
   case DcOutlines:
   case DcSurface: 
   case DcSurfaceWireframeEdges: 
   case DcSurfaceOutlineEdges: 
      for (i=0; i<vertexcount-1; i++) {
	 if ((gp->cmd_left -= (4)*sizeof(short) + (6)*sizeof(float))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (4)*sizeof(short) + (6)*sizeof(float);
	 }

	 switch (shade_type) {
	 case DcConstantShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i];
	    break;
	 case DcVertexShade:
	 case DcSurfaceShade:
	    DPRINTF (stderr, "GP1_SET_COLOR | %d\n", colors[i]);
	    *(gp->cmd_ptr)++ = GP1_SET_COLOR | colors[i];
	    break;
	 }
	 DPRINTF (stderr, "GP1_XF_LINE_FLT_3D\n");
	 *(gp->cmd_ptr)++ = GP1_XF_LINE_FLT_3D;

	 *(gp->cmd_ptr)++ = 1;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[i][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
	 *(gp->cmd_ptr)++ = 0x8000;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[i+1][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
      }
      break;
   }

} /* GP_draw_3D_connline_list */
/*
  ======================================================================
 */
void
GP_draw_3D_triangle (gp, vtxlocs, clist, disp_type, shade_type)
     GP_data_ptr gp;
     DtRealTriple *vtxlocs;
     DtInt *clist;
     DtRepType disp_type;
     DtInterpType shade_type;
{
   int i, j, k, start;
   int cnt;
   int color;
   float tmp;

   switch (disp_type) {
   case DcPoints:
      if ((gp->cmd_left -= (8)*sizeof(short) + (18)*sizeof(float))
	  <= 0) {
	 GP_flush(gp);
	 gp->cmd_left -= (8)*sizeof(short) + (18)*sizeof(float);
      }

      DPRINTF (stderr, "GP1_SET_COLOR | %d\n", clist[0]);
      *(gp->cmd_ptr)++ = GP1_SET_COLOR | clist[0];
      DPRINTF (stderr, "GP1_XF_LINE_FLT_3D\n");
      *(gp->cmd_ptr)++ = GP1_XF_LINE_FLT_3D;

      for (j=0; j<3; j++) {
	 *(gp->cmd_ptr)++ = 1;
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[j][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
	 if (j == 2) {
	    *(gp->cmd_ptr)++ = 0x8000;
	 } else {
	    *(gp->cmd_ptr)++ = 0;
	 }
	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[j][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
      }
      break;

   case DcWireframe:
   case DcOutlines:
      if ((gp->cmd_left -= (6)*sizeof(short) + (12)*sizeof(float))
	  <= 0) {
	 GP_flush(gp);
	 gp->cmd_left -= (6)*sizeof(short) + (12)*sizeof(float);
      }

      DPRINTF (stderr, "GP1_SET_COLOR | %d\n", clist[0]);
      *(gp->cmd_ptr)++ = GP1_SET_COLOR | clist[0];
      DPRINTF (stderr, "GP1_XF_LINE_FLT_3D\n");
      *(gp->cmd_ptr)++ = GP1_XF_LINE_FLT_3D;

      for (j=0; j<3; j++) {
	 if (j == 0) {
	    *(gp->cmd_ptr)++ = 1;
	 } else {
	    *(gp->cmd_ptr)++ = 0;
	 }

	 for (k=0; k<3; k++) {
	    tmp = vtxlocs[j][k];
	    GP1_PUT_F ((gp->cmd_ptr), tmp);
	 }
      }
      *(gp->cmd_ptr)++ = 0x8000;
      
      for (k=0; k<3; k++) {
	 tmp = vtxlocs[0][k];
	 GP1_PUT_F ((gp->cmd_ptr), tmp);
      }
      break;

   case DcSurface: 
   case DcSurfaceWireframeEdges: 
   case DcSurfaceOutlineEdges: 
      switch (shade_type) {
      case DcConstantShade:
	 if ((gp->cmd_left -= (4)*sizeof(short) + (9)*sizeof(float))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (4)*sizeof(short) + (9)*sizeof(float);
	 }

	 DPRINTF (stderr, "GP1_SET_COLOR | %d\n", clist[0]);
	 *(gp->cmd_ptr)++ = GP1_SET_COLOR | clist[0];
	 DPRINTF (stderr, "GP1_XF_PGON_FLT_3D | GP1_SHADE_CONSTANT\n");
	 *(gp->cmd_ptr)++ = GP1_XF_PGON_FLT_3D | GP1_SHADE_CONSTANT;
	 *(gp->cmd_ptr)++ = 1; /* nbnds */

	 *(gp->cmd_ptr)++ = 3; /* npnts */

	 for (j=0; j<3; j++) {
	    for (k=0; k<3; k++) {
	       tmp = vtxlocs[j][k];
	       GP1_PUT_F ((gp->cmd_ptr), tmp);
	    }
	 }
	 break;
	 
      case DcVertexShade:
      case DcSurfaceShade:
	 if ((gp->cmd_left -= (3)*sizeof(short) + (9)*sizeof(float)
	      + 3*sizeof(int))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (3)*sizeof(short) + (9)*sizeof(float)
		  + 3*sizeof(int);
	 }

	 DPRINTF (stderr, "GP1_XF_PGON_FLT_3D | GP1_SHADE_GOURAUD\n");
	 *(gp->cmd_ptr)++ = GP1_XF_PGON_FLT_3D | GP1_SHADE_GOURAUD;
	 *(gp->cmd_ptr)++ = 1; /* nbnds */
	 *(gp->cmd_ptr)++ = 3; /* npnts */

	 for (j=0; j<3; j++) {
	    for (k=0; k<3; k++) {
	       tmp = vtxlocs[j][k];
	       GP1_PUT_F ((gp->cmd_ptr), tmp);
	    }
	    color = clist[j] << 16;
	    GP1_PUT_I ((gp->cmd_ptr), color);
	 }
	 break;
      }
      break;
   }
} /* GP_draw_3D_triangle */
/*
  ======================================================================
 */
void
GP_compute_3D_triangle (gp, vlist, clist, points, xform)
     GP_data_ptr gp;
     GP_point_3D vlist[3];
     GP_color clist[3];
     int points;
     int xform;
{
   int i, j, k, start;
   int cnt;
   float tmp = 1.0;
   short *flags;
   float x, y, z, w;
   int ix, iy, iz;
   short *data;
   static int nbnds;
   static int npnts[100];

   GP_flush(gp);

   if (xform) {
      gp->cmd_left -= (3)*sizeof(short) + (16)*sizeof(float);

      DPRINTF(stderr, "GP1_GET_MAT_3D | %d\n", gp->current_matrix);
      *(gp->cmd_ptr)++ = GP1_GET_MAT_3D | gp->current_matrix;
      data = gp->cmd_ptr;
      *(gp->cmd_ptr)++ = 1;
      gp->cmd_ptr += 16*sizeof(float);
   
      DPRINTF (stderr, "GP1_EOCL\n");
      *(gp->cmd_ptr) ++ = GP1_EOCL;
      DPRINTF (stderr, "gp1_post\n");
      gp1_post (gp->shmem, gp->cmd_offset, gp->ioctl_fd);

      while (*data) {
	 sleep (1);
      }

      data++;
      printf ("mat[%d]\n", gp->current_matrix);
      for (i=0; i<4; i++) {
	 printf ("\t");
	 for (j=0; j<4; j++) {
	    GP1_GET_F(data, x);
	    printf ("%13g ", x);
	 }
	 printf ("\n");
      }
      reset_buf(gp);
   }

   if (points) {
      gp->cmd_left -= (5)*sizeof(short) + (12)*sizeof(float);

      DPRINTF (stderr, "GP1_MUL_POINT_FLT_3D\n");
      *(gp->cmd_ptr)++ = GP1_MUL_POINT_FLT_3D;
      *(gp->cmd_ptr)++ = 3;
	      
      data = gp->cmd_ptr;

      for (j=0; j<3; j++) {
	 GP1_PUT_F ((gp->cmd_ptr), vlist[j].x);
	 GP1_PUT_F ((gp->cmd_ptr), vlist[j].y);
	 GP1_PUT_F ((gp->cmd_ptr), vlist[j].z);
	 GP1_PUT_F ((gp->cmd_ptr), tmp);
	 flags = gp->cmd_ptr;
	 *(gp->cmd_ptr)++ = 1;
      }
      
      DPRINTF (stderr, "GP1_EOCL\n");
      *(gp->cmd_ptr) ++ = GP1_EOCL;
      DPRINTF (stderr, "gp1_post\n");
      gp1_post (gp->shmem, gp->cmd_offset, gp->ioctl_fd);

      while (*flags) {
	 sleep (1);
      }

      for (j=0; j<3; j++) {
	 printf ("pnt[%d]: (%g,%g,%g,%g)\n",  j, 
		 vlist[j].x, vlist[j].y, vlist[j].z, 1.0);

	 GP1_GET_F(data, x);
	 GP1_GET_F(data, y);
	 GP1_GET_F(data, z);
	 GP1_GET_F(data, w);
	 data++;
	 printf ("xformed: (%g,%g,%g,%g)\n",  x, y, z, w);
      }

      reset_buf(gp);

   } else {

      gp->cmd_left -= (5)*sizeof(short) + (9)*sizeof(float) +
	    (1)*sizeof(int);

      DPRINTF (stderr, "GP1_PROC_PGON_FLT_3D | GP1_SHADE_CONSTANT\n");
      *(gp->cmd_ptr)++ = GP1_PROC_PGON_FLT_3D | GP1_SHADE_CONSTANT;
      *(gp->cmd_ptr)++ = 1; /* nbnds */
      
      *(gp->cmd_ptr)++ = 3; /* npnts */

      for (j=0; j<3; j++) {
	 GP1_PUT_F ((gp->cmd_ptr), vlist[j].x);
	 GP1_PUT_F ((gp->cmd_ptr), vlist[j].y);
	 GP1_PUT_F ((gp->cmd_ptr), vlist[j].z);
	 printf ("tri[%d]: (%g,%g,%g)\n", j, vlist[j].x, vlist[j].y, 
		 vlist[j].z);
      }

      data = gp->cmd_ptr;
      *(gp->cmd_ptr) ++ = gp->cmd_left;
      DPRINTF (stderr, "GP1_EOCL\n");
      *(gp->cmd_ptr) ++ = GP1_EOCL;
      DPRINTF (stderr, "gp1_post\n");
      gp1_post (gp->shmem, gp->cmd_offset, gp->ioctl_fd);

      while (*data) {
	 sleep(1);
      }

      nbnds = *(gp->cmd_ptr)++;
      for (i=0; i<nbnds; i++) {
	 npnts[i] = *(gp->cmd_ptr)++;
      }
      for (i=0; i<nbnds; i++) {
	 for (j=0; j<npnts[i]; j++) {
	    GP1_GET_I(gp->cmd_ptr, ix);
	    GP1_GET_I (gp->cmd_ptr, iy);
	    GP1_GET_I (gp->cmd_ptr, iz);
	    printf ("b%d p%d (%d,%d,%d)\n", i+1, j+1, ix, iy, iz);
	 }
      }
      reset_buf(gp);
   }
}
/*
  ======================================================================
 */
void
GP_draw_3D_polygon (gp, nbnds, npnts, vlist, clist, disp_type, shade_type)
     GP_data_ptr gp;
     int nbnds;
     int npnts[];
     GP_point_3D *vlist;
     GP_color *clist;
     DtRepType disp_type;
     DtInterpType shade_type;
{
   extern void GP_draw_3D_triangle();
   int i, j, k, start;
   int cnt;

   if (nbnds == 1 && npnts[0] == 3) {
      GP_draw_3D_triangle(gp, vlist, clist, disp_type, shade_type);
      return;
   }

   switch (disp_type) {
   case DcWireframe:
   case DcOutlines:
      for (i=0, cnt=0; i<nbnds; i++) {
	 cnt += npnts[i] + 1;
      }
      if ((gp->cmd_left -= (cnt+2)*sizeof(short) + (cnt*3)*sizeof(float))
	  <= 0) {
	 GP_flush(gp);
	 gp->cmd_left -= (cnt+2)*sizeof(short) + (cnt*3)*sizeof(float);
      }

      DPRINTF (stderr, "GP1_SET_COLOR | %d\n", clist[0]);
      *(gp->cmd_ptr)++ = GP1_SET_COLOR | clist[0];
      DPRINTF (stderr, "GP1_XF_LINE_FLT_3D\n");
      *(gp->cmd_ptr)++ = GP1_XF_LINE_FLT_3D;

      for (i=0, k=0; i<nbnds; i++) {
	 for (j=0; j<npnts[i]; j++, k++) {
	    if (j == 0) {
	       start = k;
	       *(gp->cmd_ptr)++ = 1;
	    } else {
	       *(gp->cmd_ptr)++ = 0;
	    }

	    GP1_PUT_F ((gp->cmd_ptr), vlist[k].x);
	    GP1_PUT_F ((gp->cmd_ptr), vlist[k].y);
	    GP1_PUT_F ((gp->cmd_ptr), vlist[k].z);
	 }
	 if (i == (nbnds - 1)) {
	    *(gp->cmd_ptr)++ = 0x8000;
	 } else {
	    *(gp->cmd_ptr)++ = 0;
	 }
	 GP1_PUT_F ((gp->cmd_ptr), vlist[start].x);
	 GP1_PUT_F ((gp->cmd_ptr), vlist[start].y);
	 GP1_PUT_F ((gp->cmd_ptr), vlist[start].z);
      }
      break;

   case DcSurface: 
   case DcSurfaceWireframeEdges: 
   case DcSurfaceOutlineEdges: 
      switch (shade_type) {
      case DcConstantShade:
	 for (i=0, cnt=0; i<nbnds; i++) {
	    cnt += npnts[i];
	 }
	 if ((gp->cmd_left -= (nbnds+3)*sizeof(short) + (cnt*3)*sizeof(float))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (nbnds+3)*sizeof(short) + (cnt*3)*sizeof(float);
	 }

	 DPRINTF (stderr, "GP1_SET_COLOR | %d\n", clist[0]);
	 *(gp->cmd_ptr)++ = GP1_SET_COLOR | clist[0];
	 DPRINTF (stderr, "GP1_XF_PGON_FLT_3D | GP1_SHADE_CONSTANT\n");
	 *(gp->cmd_ptr)++ = GP1_XF_PGON_FLT_3D | GP1_SHADE_CONSTANT;
	 *(gp->cmd_ptr)++ = nbnds;

	 for (i=0; i<nbnds; i++) {
	    *(gp->cmd_ptr)++ = npnts[i];
	 }

	 for (i=0, k=0; i<nbnds; i++) {
	    for (j=0; j<npnts[i]; j++, k++) {
	       GP1_PUT_F ((gp->cmd_ptr), vlist[k].x);
	       GP1_PUT_F ((gp->cmd_ptr), vlist[k].y);
	       GP1_PUT_F ((gp->cmd_ptr), vlist[k].z);
	    }
	 }
	 break;

      case DcVertexShade:
      case DcSurfaceShade:
	 for (i=0, cnt=0; i<nbnds; i++) {
	    cnt += npnts[i];
	 }
	 if ((gp->cmd_left -= (nbnds+2)*sizeof(short) + (cnt*3)*sizeof(float)
	      + cnt*sizeof(int))
	     <= 0) {
	    GP_flush(gp);
	    gp->cmd_left -= (nbnds+2)*sizeof(short) + (cnt*3)*sizeof(float)
		  + cnt*sizeof(int);
	 }

	 DPRINTF (stderr, "GP1_XF_PGON_FLT_3D | GP1_SHADE_GOURAUD\n");
	 *(gp->cmd_ptr)++ = GP1_XF_PGON_FLT_3D | GP1_SHADE_GOURAUD;
	 *(gp->cmd_ptr)++ = nbnds;

	 for (i=0; i<nbnds; i++) {
	    *(gp->cmd_ptr)++ = npnts[i];
	 }

	 for (i=0, k=0; i<nbnds; i++) {
	    for (j=0; j<npnts[i]; j++, k++) {
	       GP1_PUT_F ((gp->cmd_ptr), vlist[k].x);
	       GP1_PUT_F ((gp->cmd_ptr), vlist[k].y);
	       GP1_PUT_F ((gp->cmd_ptr), vlist[k].z);
	       GP1_PUT_I ((gp->cmd_ptr), clist[k]);
	    }
	 }
	 break;
      }
      break;
   }

} /* GP_draw_3D_polygon */
/*
  ======================================================================
 */
static
Notify_value
GP_SIGXCPU_handler ()
{
   int restart_count;

   restart_count = gp1_get_restart_count (GP_ioctl_fd);

   if (GP_restart_count != restart_count) {
      GP_restart_count = restart_count;
   }
   return (NOTIFY_DONE);
}
