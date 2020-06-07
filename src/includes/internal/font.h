#ifndef DORE_FONT_H
#define DORE_FONT_H
 
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
 

    /***************************/
    /***  Defined Constants  ***/
    /***************************/

#define dod_font_magic_number   2242
#define dod_font_version        1

#define dod_font_type_vector    0
#define dod_font_type_polygonal 1

#define dod_font_last_font	DcHelvetica 


    /********************************************/
    /***  Data Structures & Type Definitions  ***/
    /********************************************/

typedef struct  
{
    int    type;           /* vector or polygonal */
    int    numchar;        /* number of characters in file */
    int    firstchar;      /* first character in sequence */
    int    char_width;     /* width of widest character */
    int    baseline;       /* bottom of capital letters */
    int    capline;        /* top of capital letters */
    int    topline;        /* highest point of any letter in font */
    int    bottomline;     /* lowest point of any letter in font */
    int    nsegs;          /* total number of segments */
    int    npoints;        /* total number of points in font */
    int   *widths;         /* width of each character */
    int   *nsegments;      /* number of segments in each character */
    int   *first_segment;  /* first segment in each character */
    int   *first_point;    /* first point for each segment */
    int   *x_coord;        /* x coordinate of each point */
    int   *y_coord;        /* y coordinate of each point */
} dot_font;

typedef struct 
{   DtReal x;
    DtReal y;
} dot_point2d ;

typedef struct 
{
  char         *string;   	/* text to be drawn/queried */
  DtInt         length;  	/* number of characters in string */
  dot_point2d   position; 	/* position to display text at */
  DtReal   	height;   	/* height of character */
  DtReal   	charexp;  	/* expansion factor */
  DtTextPath  	path;     	/* character path */
  DtReal   	xup;		/* x component of up vector */
  DtReal   	yup; 		/* y component of up vector */
  DtReal   	spacing;   	/* extra spacing; fraction of character height*/
  DtFont    	font_index;
  DtTextAlignHorizontal halign;	/* horizontal alignment */
  DtTextAlignVertical 	valign;	/* vertical alignment */
  char         *user_data; 	/* state information for user */
  DtPFV         polyline; 	/* users polyline function */
  DtPFV         polygon; 	/* users polygon function */
} dot_font_state ;

#endif
