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
 
#include "dore/internal/dogen.h"
#include "dore/dore_develop/develop.h"

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/XTitan.h>
#include <X11/XB.h>
#include "machine/tigr.h"
#include "machine/rasterizer.h"

typedef struct {
   DtObject device;
   Display *display;
   Window window;
   GC gc;
   XBoxRec extent;
   unsigned long nrects;
   DtReal *clipRects;
   DtReal *clearClipRects;
   int drawBuffer;
   int userWindow, userDisplay;
   int doubleBuffered;
   XImage *image;
   XWindowAttributes window_attributes;
   unsigned long sequence_number;
   int expansion_board;
   int db_mode;
   int stereoswitch;
   DtReal background_color[3];
   DtVisualType visualtype;
   DtFlag auto_size;
   DtFlag bad_geom;
   DtFlag stereoscreen;
   /* for Pseudo Color */
   DtInt shade_index;
   DtReal cbtselpsweight[3];
   DtInt cbtselpsmask[3];
   DtReal cwtavgpsweight[3];
   DtReal cwtavgpslim[3];
   DtShadeMode shade_mode;
   short bg_color_index;
   DtObject current_view;
   DtObject previous_view;
   DtFlag partially_obscure;
} ddt_x11_titang2_data;


typedef struct {
   int color_index;
   short z_function;
   int control;
   int pixel_function;
   int pixel_clip;
   int vector_pattern;
   unsigned char fg_r;
   unsigned char fg_g;
   unsigned char fg_b;
   unsigned char bg_r;
   unsigned char bg_g;
   unsigned char bg_b;
   unsigned short mask_r;
   unsigned short mask_g;
   unsigned short mask_b;
   int select;
} ddt_x11_titang2_dcm_tigr_state;


#undef cw

typedef char byte;
typedef long seventeen_fifteen;
typedef short twelve_four;
typedef long twelve_sixteen;
typedef long twelve_twenty;
typedef short nine_three_four;
typedef long nine_three_sixteen;

#define make_wc_op(wc,op) ((wc)<<16)|(op)
#define hi(x) ((x)>>16)
#define lo(x) (0xffff&(x))

typedef struct {
	int wc_op,cw;
	twelve_four x1;
	twelve_four ytop;
	twelve_twenty dx1dy;
	twelve_four x2;
	twelve_four yvertex;
	twelve_twenty dx2dy;
	twelve_four x3;
	twelve_four ybottom;
	twelve_twenty dx3dy;
	struct {
		seventeen_fifteen fl1;
		seventeen_fifteen dfl1dedge;
		seventeen_fifteen fl2;
		seventeen_fifteen dfl2dedge;
		seventeen_fifteen dfdx;
	} tfs[4];
} triangle_data;

typedef struct {
	int wc_op,cw;
	twelve_four x1;
	twelve_four y1;
	twelve_four x2;
	twelve_four y2;
	struct {
		seventeen_fifteen f;
		seventeen_fifteen dfdx;
	} fs[4];
} vector_data;

/*
/*	tg_prep_tri(p), tg_prep_tri_z(p),
/*	tg_prep_vec(p), tg_prep_vec_z(p), tg_prep_vec_sh(p), tg_prep_vec_sh_z(p)
/*	tg_prep_rectvf(p)
/*	all return values in *p which tell where to put mass-produced objects.
*/
typedef struct {
	int wc_op;		/* the opcode for this kind of object */
	unsigned short n;	/* how many the buffer can hold (>=1) */
	unsigned short cw;	/* control-word settings */
	int *ptr;		/* 1st object goes here */
	int (*finish)();	/* (*p->finish)(p, next_ptr);  */
} tg_prep;




extern ddt_x11_titang2_data *dde_x11_titang2_current_dcm_data;
extern int dde_x11_titang2_thread_lock;
extern int dde_x11_titang2_tg_fd;

extern int dde_x11_titang2_tg_open_count;

extern struct list *dde_x11_titang2_dcm_list;

extern ddt_x11_titang2_dcm_tigr_state dde_x11_titang2_tigr_state;

/*  Dore driver for mp fullcolor dynamic windows on titan local db */

#define XDISPLAY(x)		((x)->display)
#define XWINDOW(x)		((x)->window)
#define XGC(x)			((x)->gc)
#define XClipRects(x)		((x)->clipRects)
#define XExtent(x)		((x)->extent)
#define XDrawBuffer(x)		((x)->drawBuffer)
#define XUserWindow(x)		((x)->userWindow)
#define XUserDisplay(x)		((x)->userDisplay)
#define XDoubleBuffered(x)	((x)->doubleBuffered)
#define XDisplayBuffer(x)	((XDrawBuffer(x))?0:1)
#define XBLACKPIXEL(x)		BlackPixel(XDISPLAY(x),DefaultScreen(XDISPLAY(x)))
#define XWHITEPIXEL(x)		WhitePixel(XDISPLAY(x),DefaultScreen(XDISPLAY(x)))
#define XIMAGE(x)		((x)->image)
#define XWINDOWATTRIBUTES(x)	((x)->window_attributes)
#define XNRECTS(x)		((x)->nrects)
#define XSEQUENCENO(x)		((x)->sequence_number)
#define ExpansionBoard(x)	((x)->expansion_board)
#define DBMODE(x)		((x)->db_mode)
#ifdef notdef
#define ForePixel(x) 		XBLACKPIXEL(x)
#define BackPixel(x) 		XWHITEPIXEL(x)
#endif
#define ForePixel(x) 		XWHITEPIXEL(x)
#define BackPixel(x) 		XBLACKPIXEL(x)

