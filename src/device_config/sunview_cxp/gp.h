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
 
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include <sys/ioctl.h>
#include <sun/gpio.h>
#include <pixrect/gp1cmds.h>

/*
 * Typedefs
 */
typedef struct {
   Pixwin *pw;			/* -> pixwin assoicated with GP */
   int ioctl_fd;		/* GP file descriptor */
   int mindev;			/* minor device number for GP */
   short *shmem;		/* -> GP shared memory */
   int statblk;			/* static block offset */
   int n_buffers;		/* # of buffers to allocate */
   int cmd_offset;		/* offset for command buffer */
   int cmd_bitvec;		/* bit vector describing our buffer */
   short *cmd_ptr;		/* -> after last command in GP buffer */
   int cmd_left;		/* # bytes left in command buffer */
   int clipid;			/* pixwin clip id */
   Rect win_size;		/* dimensions of pixwin */
   struct {
      int x;
      int y;
   } win_org;			/* origin of pixwin in FB */
   int current_matrix;		/* # current xform matrix */
   int Zbuffer_on;		/* Flag for Z buffer use */
   int double_buffer_on;	/* double buffering enabled */
   int double_buffer_pending;	/* set if want double buffering but not on yet */
   int color_planes;		/* color planes mask */
   void (*func)();		/* function to call when the clip list changes */
   int setup_pending;	
   int restart_count;
} GP_data_type, *GP_data_ptr;

typedef struct {
   float x, y, z;
} GP_point_3D;

typedef int GP_color;

typedef struct {
   float e[4][4];
} Matrix;

/*
 * Function proto types
 */

extern GP_data_ptr GP_init	ARGS( (Pixwin *, int, int, int, int) );
extern void GP_clear_Zbuffer	ARGS( (GP_data_ptr) );
extern void GP_clear_window 	ARGS( (GP_data_ptr, int, int) );
extern void GP_clear_Z_and_color ARGS( (GP_data_ptr, short, short, short, short) );
extern void GP_flush		ARGS( (GP_data_ptr) );
extern void GP_double_buffer	ARGS( (GP_data_ptr, int) );
extern void GP_Zbuffer		ARGS( (GP_data_ptr, int) );
extern void GP_switch_buffer	ARGS( (GP_data_ptr) );
extern void GP_draw_3D_polygon	ARGS( (GP_data_ptr, int, int[], 
				       GP_point_3D *, GP_color *, 
				       DtRepType, DtInterpType) );
extern void GP_matrix_concat	ARGS( (GP_data_ptr, int, int, int) );
extern void GP_load_matrix	ARGS( (GP_data_ptr, DtMatrix4x4 *, DtShort) );
extern Rect *GP_get_cliplist	ARGS( (GP_data_ptr, int *) );
extern void GP_set_cliplist	ARGS( (GP_data_ptr, DtInt, DtReal[][4]) );
