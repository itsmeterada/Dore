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
	dor_font_initialize
	dor_font_get_font
	dor_get_font_name
	dor_font_get_type
	dor_font_load
	dor_font_inquire_bounds
	dor_font_draw_text
	static getid
	static do_step
	static build_char
	static map_halignment
	static map_valignment
	static basic_width
	static compute_text_rectangle
	static init_text

  ======================================================================
 */
#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/internal/font.h>

    /**************************************/
    /***  Static Function Declarations  ***/
    /**************************************/

static int  getid                  (dot_font_state *st, int);
static void do_step                (dot_font_state *st);
static void build_char             (dot_font_state *st);
static void map_halignment         (dot_font_state *st);
static void map_valignment         (dot_font_state *st);
static int  basic_width		   (dot_font_state *st);
static void compute_text_rectangle (dot_font_state *st);
static void init_text              (dot_font_state *st);


    /**************************/
    /***  Global Variables  ***/
    /**************************/

char      *doe_font_library_name;
DtInt      doe_text_max_font;
dot_font **doe_text_font_table;

/*
 ======================================================================
 */

void dor_font_initialize (void)
{
    if ( ! (doe_font_library_name = DDgetenv("DORE_FONTDIR"))) {
	    doe_font_library_name = DORE_FONTDIR;
    }

    doe_text_max_font = 0;	/* maximum font index in use */

    doe_text_font_table = (dot_font **)dor_space_allocate(sizeof(dot_font *));

    if ( doe_text_font_table == NULL )
	    DDerror (ERR_CANT_ALLOC_MEM, "dor_font_initialize", "font table");

    doe_text_font_table[0] = dor_font_load(DcPlainRoman);

    if ( doe_text_font_table[0] == NULL )
	    DDerror (ERR_CANT_LOAD_DEF_FONT, "dor_font_initialize", DcNullPtr);
}
/*
 ======================================================================
 */

dot_font *dor_font_get_font (DtInt fontid)
{
    int i;
    dot_font **temp = doe_text_font_table;
    dot_font *new_font;

    /* if font index out of range display warning and use default font */

    if ( (fontid < 0 ) || ( fontid > dod_font_last_font) ){
	sprintf(DDerror_GetLine(),
		"font index %d",fontid);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_font_get_font", DDerror_GetLine());
	return doe_text_font_table[0];
    }

    /* if font already loaded, return it */

    if ( ( fontid <= doe_text_max_font ) &&
	( doe_text_font_table[fontid] != NULL )) {
	return doe_text_font_table[fontid];
    }

    /* allocate more space for font table if necessary */

    if ( fontid > doe_text_max_font ) {
	temp = dor_space_reallocate (temp, (fontid+1) * sizeof(dot_font*));

	if ( temp == NULL ) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dor_font_get_font", "font table");
	    return doe_text_font_table[0];
	}

	doe_text_font_table = temp;

	for ( i = doe_text_max_font+1; i <= fontid; i	++){
	    doe_text_font_table[i] = NULL;
	}

	doe_text_max_font = fontid;
    }

    /* load new font */

    if ( !(new_font = dor_font_load(fontid)))
	    new_font = doe_text_font_table[0];

    doe_text_font_table[fontid] = new_font;

    return new_font;
}
/*
 ======================================================================
 */

char *dor_get_font_name (DtInt font_id)
{
    switch (font_id) {
	case DcPlainRoman:
	    return "romanp";
	case DcSimplexRoman:
	    return "romans";
	case DcDuplexRoman:
	    return "romand";
	case DcComplexSmallRoman:
	    return "romancs";
	case DcComplexRoman:
	    return "romanc";
	case DcTriplexRoman:
	    return "romant";
	case DcComplexSmallItalic:
	    return "italiccs";
	case DcComplexItalic:
	    return "italicc";
	case DcTriplexItalic:
	    return "italict";
	case DcSimplexScript:
	    return "scripts";
	case DcComplexScript:
	    return "scriptc";
	case DcGothicGerman:
	    return "gothgrt";
	case DcGothicEnglish:
	    return "gothgbt";
	case DcGothicItalian:
	    return "gothitt";
	case DcPlainGreek:
	    return "greekp";
	case DcSimplexGreek:
	    return "greeks";
	case DcComplexSmallGreek:
	    return "greekcs";
	case DcComplexGreek:
	    return "greekc";
	case DcComplexCyrillic:
	    return "cyrillic";
	case DcUpperCaseMathematics:
	    return "ucmath";
	case DcLowerCaseMathematics:
	    return "lcmath";
	case DcMusic:
	    return "music";
	case DcMeteorology:
	    return "meteorology";
	case DcSymbols:
	    return "symbols";
	case DcAstrology:
	    return "astrology";
	case DcHelvetica:
	    return "helvetica";
	default:
	    break;
    }

    return(NULL);
}
/*
 ======================================================================
 */

int dor_font_get_type (dot_font_state *st)
{
    dot_font *font;

    font = dor_font_get_font(st->font_index);
    return font->type;
}
/*
 ======================================================================
 */

dot_font *dor_font_load (DtInt font_id)
{
    int i, j, k;
    char *font_name;
    char filename[256];
    FILE *fontfile;
    dot_font *hfont;
    int this_segment = 0, this_point = 0;
    int npoints, file_version;

    font_name = dor_get_font_name(font_id);

    if ( !font_name ){
	sprintf(DDerror_GetLine(),
		"font id %d",font_id);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_font_load", DDerror_GetLine());
	return NULL;
    }

    /* open font file	*/

    strcpy(filename, doe_font_library_name);
    strcat(filename, "/");
    strcat(filename, font_name);
    strcat(filename, ".df");

    if ((fontfile = fopen(filename, "r")) == NULL ) {
	sprintf(DDerror_GetLine(),
		"Can't open font file '%s'",filename);
	DDerror (ERR_IO_ERROR,
		    "dor_font_load", DDerror_GetLine());
	return NULL;
    }

#ifdef debug
    printf("reading font file %s\n", filename);
#endif
    /* check magic number */

    if ( getw(fontfile) != dod_font_magic_number ) {
	sprintf(DDerror_GetLine(),
		"bad magic number font '%s'",filename);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_font_load", DDerror_GetLine());
	return NULL;
    }

    /* check font format version number */

    if ( (file_version = getw(fontfile)) != dod_font_version ) {
	sprintf(DDerror_GetLine(),
		"incompatible version'%s'\nfile version %d, required version %d",
		filename, file_version, dod_font_version);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_font_load", DDerror_GetLine());
	return NULL;
    }

    if ((hfont = (dot_font *)dor_space_allocate(sizeof(dot_font))) == NULL ){
	DDerror (ERR_CANT_ALLOC_MEM, "dor_font_load", "hfont");
	return NULL;
    }

    /* check font type */

    hfont->type = getw(fontfile);

    if (( hfont->type != dod_font_type_vector ) &&
	( hfont->type != dod_font_type_polygonal )){
	sprintf(DDerror_GetLine(),
		"unknown font type %d'",hfont->type);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_font_load", DDerror_GetLine());
	return NULL;
    }

    /* number of charcters in file */

    hfont->numchar = getw(fontfile);

#ifdef debug
    printf("number of charecters in font %d\n", hfont->numchar);
#endif

    /* first character in sequence */

    hfont->firstchar = getw(fontfile);

#ifdef debug
    printf("first char %d\n", hfont->firstchar);
#endif

    /* character width */

    hfont->char_width = getw(fontfile);

#ifdef debug
    printf("char width%d\n", hfont->char_width);
#endif

    /* baseline */

    hfont->baseline = getw(fontfile);

#ifdef debug
    printf("baseline %d\n", hfont->baseline);
#endif

    /* capline */

    hfont->capline = getw(fontfile);

#ifdef debug
    printf("capline %d\n", hfont->capline);
#endif

    /* topline */

    hfont->topline = getw(fontfile);

#ifdef debug
    printf("topline %d\n", hfont->topline);
#endif

    /* bottom line */

    hfont->bottomline = getw(fontfile);

#ifdef debug
    printf("bottom line %d\n", hfont->bottomline);
#endif

    /* number of segments */

    hfont->nsegs = getw(fontfile);

#ifdef debug
    printf("number of segments %d\n", hfont->nsegs);
#endif

    /* number of points */

    hfont->npoints = getw(fontfile);

#ifdef debug
    printf("number of points %d\n", hfont->npoints);
    fflush(stdout);
#endif

    /* add one extra character for displaying characters out of range */

    if (!(hfont->widths = dor_space_allocate(sizeof(int)*(hfont->numchar+1)))){
	DDerror (ERR_CANT_ALLOC_MEM, "dor_font_load", "widths");
	return NULL;
    }

    hfont->widths[hfont->numchar] = hfont->char_width;

    if ((hfont->nsegments = dor_space_allocate(sizeof(int)*(hfont->numchar+1))) == NULL ){
	DDerror (ERR_CANT_ALLOC_MEM, "dor_font_load", "nsegments");
	return NULL;
    }

    hfont->nsegments[hfont->numchar] = 1;

    if ((hfont->first_segment = (int *)dor_space_allocate(sizeof(int)*(hfont->numchar+1))) == NULL ){
	DDerror (ERR_CANT_ALLOC_MEM, "dor_font_load", "first_segment");
	return NULL;
    }

    hfont->first_segment[hfont->numchar] = hfont->nsegs;

    /* need extra entry to specify last point of last (the error) character */

    if ((hfont->first_point = (int *)dor_space_allocate(sizeof(int)*(hfont->nsegs+2))) == NULL ){
	DDerror (ERR_CANT_ALLOC_MEM, "dor_font_load", "first_point");
	return NULL;
    }

    hfont->first_point[hfont->nsegs] = hfont->npoints;
    hfont->first_point[hfont->nsegs+1] = hfont->npoints+5;

    if ((hfont->x_coord = (int *)dor_space_allocate(sizeof(int)*(hfont->npoints+5))) == NULL ){
	DDerror (ERR_CANT_ALLOC_MEM, "dor_font_load", "x_coord");
	return NULL;
    }

    if ((hfont->y_coord = (int *)dor_space_allocate(sizeof(int)*(hfont->npoints+5))) == NULL ){
	DDerror (ERR_CANT_ALLOC_MEM, "dor_font_load", "y_coord");
	return NULL;
    }

    /* define points for error character */

    hfont->x_coord[hfont->npoints] = 0;
    hfont->y_coord[hfont->npoints] = hfont->bottomline;

    hfont->x_coord[hfont->npoints+1] = 0;
    hfont->y_coord[hfont->npoints+1] = hfont->topline;

    hfont->x_coord[hfont->npoints+2] = hfont->char_width-1;
    hfont->y_coord[hfont->npoints+2] = hfont->topline;

    hfont->x_coord[hfont->npoints+3] = hfont->char_width-1;
    hfont->y_coord[hfont->npoints+3] = hfont->bottomline;

    hfont->x_coord[hfont->npoints+4] = 0;
    hfont->y_coord[hfont->npoints+4] = hfont->bottomline;

    /* input character information */

    for ( i = 0; i < hfont->numchar; i++ ) {
	/* for each character */

	/* input character width */

	hfont->widths[i] = getw(fontfile);

	/* input number of segments in character */

	hfont->nsegments[i] = getw(fontfile);
	hfont->first_segment[i] = this_segment;

	for ( j = 1; j <= hfont->nsegments[i]; j++)
		{		/* for each segment */
		    npoints = getw(fontfile);
		    hfont->first_point[this_segment++] = this_point;

		    for ( k = 1; k <= npoints; k++){
			/* for each point in segment */
			hfont->x_coord[this_point] = getw(fontfile);
			hfont->y_coord[this_point++] = getw(fontfile);
		    }		/* each point */
		}		/* each segment */

    }				/* each character */

    fclose(fontfile);
    return hfont;
}
/*
 ======================================================================
 */

void dor_font_inquire_bounds (
    DtInt  font_index,
    DtInt *first_char,
    DtInt *last_char)
{
    dot_font *font;

    font = dor_font_get_font(font_index);

    *first_char = font->firstchar;
    *last_char = font->firstchar + font->numchar-1;
}
/*
 ======================================================================
 */

/****
*     output text string
****/


static  dot_font   *font;		/* font to display text in */
static  DtInt       current;		/* current character */
static  DtReal      xscale, yscale;	/* scaling factors for text */
static  DtReal      xorign, yorign;	/* point to draw character at */
static  DtReal      xdelta, ydelta;	/* amount to translate by */
static  dot_point2d unit_up;		/* unit up vector */
static  dot_point2d unit_base;		/* unit vector along baseline */
static  DtReal      space_inc;		/* amount of extra space */
static  DtPFV       outfunc;		/* function to output points */
static  DtInt       char_id;
static  DtInt       ahalign;		/* actual horizontal alignment */
static  DtInt       avalign;		/* actual vertical alignment */


void dor_font_draw_text (dot_font_state *st)
{
    init_text(st);

    for ( current = 0; current < st->length; ) {
#ifdef debug
	printf("outputing character '%c'\n", st->string[current]);
	fflush(stdout);
#endif
	build_char(st);
	do_step(st);
    }
}

/* the following routines are all tools used directly or indirectly
	by dor_font_draw_text */

static int getid (
    dot_font_state *st,
    int             i)
{
    register int j = st->string[i];

    if ( ( j >= font->firstchar ) && ( j < font->firstchar+font->numchar ) )
	    return j-font->firstchar;
    else return font->numchar;
}


static void do_step (dot_font_state *st)
{
    switch (st->path) {
    case DcTextPathRight:
	xorign += xscale*font->widths[getid(st, current)]+space_inc;
	break;
    case DcTextPathLeft:
	xorign -= xscale*font->widths[getid(st, current+1)]+space_inc;
	break;
    case DcTextPathUp:
	yorign += yscale*(font->topline-font->bottomline)+space_inc;
	break;
    case DcTextPathDown:
	yorign -= yscale*(font->topline-font->bottomline)+space_inc;
	break;
    }

    current++;
}


static void build_char (dot_font_state *st)
{
    DtPoint3 pts[200];
    int j, k, nextpt, segment;
    DtReal xadj;

    char_id = getid(st, current);
    segment = font->first_segment[char_id];

    /* centre vertical text */

    if ( (st->path == DcTextPathUp) || (st->path == DcTextPathDown ) )
	    xadj = (font->char_width-font->widths[char_id])*xscale/2;
    else xadj = 0;

    for ( j = 1; j <= font->nsegments[getid(st, current)]; j++ ) {
	nextpt = 0;
	for ( k = font->first_point[segment];
	     k < font->first_point[segment+1];
	     k++ ) {
	    pts[nextpt][0] = (xscale*font->x_coord[k]+xorign+xadj)*unit_base.x+
		    (yscale*font->y_coord[k]+yorign)*unit_up.x + xdelta;
	    pts[nextpt][1] = (xscale*font->x_coord[k]+xorign+xadj)*unit_base.y+
		    (yscale*font->y_coord[k]+yorign)*unit_up.y +ydelta;
	    pts[nextpt++][2] = 0;
	}

	(outfunc)(st->user_data,nextpt, pts);
	segment++;
    }
}


static void map_halignment (dot_font_state *st)
{
    if ( st->halign != DcTextHAlignNormal ) {
	ahalign = st->halign;
	return;
    }

    switch ( st->path ){
    case DcTextPathRight:
	ahalign = DcTextHAlignLeft;
	break;
    case DcTextPathLeft:
	ahalign = DcTextHAlignRight;
	break;
    case DcTextPathUp:
	ahalign = DcTextHAlignCenter;
	break;
    case DcTextPathDown:
	ahalign = DcTextHAlignCenter;
	break;
    }
}


static void map_valignment (dot_font_state *st)
{
    if ( st->valign != DcTextVAlignNormal ){
	avalign = st->valign;
	return;
    }

    switch ( st->path ){
    case DcTextPathRight:
	avalign = DcTextVAlignBase;
	break;
    case DcTextPathLeft:
	avalign = DcTextVAlignBase;
	break;
    case DcTextPathUp:
	avalign = DcTextVAlignBase;
	break;
    case DcTextPathDown:
	avalign = DcTextVAlignTop;
	break;
    }
}


static int basic_width (dot_font_state *st)
{
    int width = 0, i;

    for (i = 0; i< st->length; i++)
	    width += font->widths[getid(st, i)];

    return width;
}


static void compute_text_rectangle (dot_font_state *st)
{
    DtReal xmin, xmax, top_base, bottom_base;
    DtReal xalign, yalign;
    DtReal x1align, y1align;

    switch ( st->path ) {
    case DcTextPathRight:
	xorign = 0;
	yorign = 0;
	xmin = 0;
	xmax = basic_width(st)*xscale+(st->length-1)*space_inc;
	top_base = bottom_base = 0;
	break;
    case DcTextPathLeft:
	xorign = -xscale*font->widths[getid(st, 0)];
	yorign = 0;
	xmin = -basic_width(st)*xscale-(st->length-1)*space_inc;
	xmax = 0;
	top_base = bottom_base = 0;
	break;
    case DcTextPathUp:
	xorign = 0;
	yorign = 0;
	xmin = 0;
	xmax = font->char_width*xscale;
	bottom_base = 0;
	top_base = (st->length-1)*(font->topline-font->bottomline)*yscale;
	break;
    case DcTextPathDown:
	xorign = 0;
	yorign = 0;
	xmin = 0;
	xmax = font->char_width*xscale;
	bottom_base = -(st->length-1)*(font->topline-font->bottomline)*yscale;
	top_base = 0;
	break;
    }

    map_halignment(st);
    map_valignment(st);

    switch ( ahalign ){
    case DcTextHAlignLeft:
	xalign = xmin;
	break;
    case DcTextHAlignCenter:
	xalign = 0.5*(xmin+xmax);
	break;
    case DcTextHAlignRight:
	xalign = xmax;
	break;
    }

    switch ( avalign ){
    case DcTextVAlignTop:
	yalign = top_base+font->topline*yscale;
	break;
    case DcTextVAlignHalf:
	yalign = 0.5 * ( (top_base+font->topline*yscale) +
			(bottom_base+font->bottomline*yscale));
	break;
    case DcTextVAlignBase:
	yalign = bottom_base+font->baseline*yscale;
	break;
    case DcTextVAlignBottom:
	yalign = bottom_base+font->bottomline*yscale;
	break;
    case DcTextVAlignCap:
	yalign = top_base+font->capline*yscale;
	break;
    }

    x1align = xalign*unit_base.x+yalign*unit_up.x;
    y1align = xalign*unit_base.y+yalign*unit_up.y;

    xdelta = st->position.x-x1align;
    ydelta = st->position.y-y1align;
}


/* initialize state variables for text output */

static void init_text (dot_font_state *st)
{
    DtReal length;

    font = dor_font_get_font(st->font_index);

    /* compute scaling factors */

    yscale = st->height/((DtReal)(font->capline-font->baseline));
    xscale = yscale*st->charexp;

    /* compute any additional spacing required */

    space_inc = st->spacing*st->height;

    /* normalized up and base vectors */

    length = dor_math_sqrt (st->xup*st->xup+st->yup*st->yup);
    if ( length == 0.0 ) {
	unit_up.x = 0;
	unit_up.y = 1;
    }
    else {
	unit_up.x = st->xup/length;
	unit_up.y = st->yup/length;
    }

    unit_base.x = unit_up.y;
    unit_base.y = -unit_up.x;

    /* select output function for font type */

    outfunc = (font->type==dod_font_type_vector) ? st->polyline : st->polygon;

    compute_text_rectangle(st);
}
