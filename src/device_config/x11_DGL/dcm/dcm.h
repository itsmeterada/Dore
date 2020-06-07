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
#include <X11/Xd.h>

typedef struct {
    DtObject		device;
    Display *		display;
    Window		window;
    GC			gc;
    XdDrawablePtr	pDraw;
    XdGCPtr		pGC;
    XBoxRec		extent;
    unsigned long	nrects;
    DtReal *		clipRects;
    int 		drawBuffer;
    int 		userWindow;
    int			userDisplay;
    int			userDrawable;
    int			doubleBuffered;
    XWindowAttributes	window_attributes;
    unsigned long	sequence_number;
    int			expansion_board;
    int			db_mode;
    int			stereoswitch;
    DtReal		background_color[3];
    DtVisualType	visualtype;
    DtFlag		auto_size;
    DtFlag		bad_geom;
    DtFlag		stereoscreen;
    /* for Pseudo Color */
    DtInt		shade_index;
    XdReal		cbtselpsweight[3];
    int			cbtselpsmask[3];
    XdReal		cwtavgpsweight[3];
    DtReal		cwtavgpslim[3];
    DtShadeMode		shade_mode;
    short		bg_color_index;
    DtObject		current_view;
    DtObject		previous_view;
    DtFlag		partially_obscure;
    unsigned char	eye;
} ddt_x11_DGL_data;

extern ddt_x11_DGL_data *dde_x11_DGL_current_dcm_data;

extern int dde_x11_DGL_user_initialized;
extern int dde_x11_DGL_open_count;

extern struct list *dde_x11_DGL_dcm_list;

#define XDISPLAY(x)		((x)->display)
#define XWINDOW(x)		((x)->window)
#define XClipRects(x)		((x)->clipRects)
#define XExtent(x)		((x)->extent)
#define XDrawBuffer(x)		((x)->drawBuffer)
#define XUserWindow(x)		((x)->userWindow)
#define XUserDisplay(x)		((x)->userDisplay)
#define XDoubleBuffered(x)	((x)->doubleBuffered)
#define XDisplayBuffer(x)	((XDrawBuffer(x))?0:1)
#define XWINDOWATTRIBUTES(x)	((x)->window_attributes)
#define XNRECTS(x)		((x)->nrects)
#define XSEQUENCENO(x)		((x)->sequence_number)
#define ExpansionBoard(x)	((x)->expansion_board)
#define DBMODE(x)		((x)->db_mode)
#define XpDRAW(x)		((x)->pDraw)
#define XUserDrawable(x)	((x)->userDrawable)
#define XpGC(x)			((x)->pGC)
#define XGC(x)			((x)->gc)



