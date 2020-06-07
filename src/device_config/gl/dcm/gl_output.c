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
	ddr_gl_dcm_clear_rectangle_depth
	ddr_gl_dcm_clear_rectangle_depth_and_color
	ddr_gl_dcm_clear_both_buffers
	ddr_gl_dcm_write_scanline_byte
	ddr_gl_dcm_dith_merge_rgb
	ddr_gl_dcm_dith_expand
	ddr_gl_dcm_dith_compress
	ddr_gl_dcm_dith_convert_3_to_1
	ddr_gl_dcm_dith_make_color_table

  ======================================================================
 */
#include "../gl_driver.h"



/* ======================================================================== */

void ddr_gl_dcm_clear_rectangle_depth (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
#   ifdef DEBUGP
	printf ("(DEVICE)Clear_rectangle_depth called\n");
#   endif

    zclear();
}



/* ======================================================================== */

void ddr_gl_dcm_clear_rectangle_depth_and_color (
    DtShort xmin,
    DtShort ymin,
    DtShort xmax,
    DtShort ymax)
{
#   ifdef DEBUGP
	printf ("(DEVICE)Clear_rect_depth_and_color called\n");
#   endif

    zfunction (ZF_LESS);

    if (CURDEV->visualtype==DcPseudoColor)
	czclear (CURDEV->backgroundcolor, getgdesc (GD_ZMAX));
    else
    {   czclear
	(   RGBpack
	    (   CURDEV->backgroundRGB[0],
		CURDEV->backgroundRGB[1],
		CURDEV->backgroundRGB[2]
	    ),
	    getgdesc (GD_ZMAX)
	);
    }
}



/* ======================================================================== */

void ddr_gl_dcm_clear_both_buffers (void)
{
    long sx,sy;

    getsize(&sx,&sy);
    viewport(0,sx-1,0,sy-1);

    if (CURDEV->visualtype==DcPseudoColor)
	color(CURDEV->backgroundcolor);
    else
    {   RGBcolor((int)((CURDEV->backgroundRGB[0])*255.),
		 (int)((CURDEV->backgroundRGB[1])*255.),
		 (int)((CURDEV->backgroundRGB[2])*255.));
    }

    clear ();
    swapbuffers ();
    clear ();
}



/* ======================================================================== */

void ddr_gl_dcm_write_scanline_byte (
    DtInt xstart,
    DtInt ystart,
    DtInt length,
    DtUChar *data)
{
    Colorindex colors[1280];	/* width must never be greater! */
    unsigned long lcolors[1280];
    unsigned char *lptr;
    int i,i3;

    if ((ystart < 0) || (ystart >= CURDEV->height) || (xstart >= CURDEV->width))
	return;

    if (xstart < 0) {
	length += xstart;
	xstart = 0;
    }

    if (length < 0) return;

    if (xstart+length > CURDEV->width)
	length = CURDEV->width-xstart;

    if (CURDEV->visualtype == DcPseudoColor) {
	for (i=0, i3=0; i<length; i++, i3+=3)
	    colors[i] = ddr_gl_dcm_dith_convert_3_to_1
			(xstart+i,ystart,data[i3],data[i3+1],data[i3+2]);

	rectwrite (xstart, ystart, xstart + length - 1, ystart + 1, colors);
    } else {

	lptr = (unsigned char *)lcolors;
	for (i=0; i<length*3; i+=3, lptr+=4) {
	    lptr[0] = 0xff;
	    lptr[1] = data[i+2];
	    lptr[2] = data[i+1];
	    lptr[3] = data[i+0];
	}

	lrectwrite (xstart, ystart, xstart + length - 1, ystart + 1, lcolors);
    }
}



/* ======================================================================== */

static int ddr_gl_dcm_dith_table[16][16] = {
    {  0,192, 48,240, 12,204, 60,252,  3,195, 51,243, 15,207, 63,255},
    {128, 64,176,112,140, 76,188,124,131, 67,179,115,143, 79,191,127},
    { 32,224, 16,208, 44,236, 28,220, 35,227, 19,211, 47,239, 31,223},
    {160, 96,144, 80,172,108,156, 92,163, 99,147, 83,175,111,159, 95},
    {  8,200, 56,248,  4,196, 52,244, 11,203, 59,251,  7,199, 55,247},
    {136, 72,184,120,132, 68,180,116,139, 75,187,123,135, 71,183,119},
    { 40,232, 24,216, 36,228, 20,212, 43,235, 27,219, 39,231, 23,215},
    {168,104,152, 88,164,100,148, 84,171,107,155, 91,167,103,151, 87},
    {  2,194, 50,242, 14,206, 62,254,  1,193, 49,241, 13,205, 61,253},
    {130, 66,178,114,142, 78,190,126,129, 65,177,113,141, 77,189,125},
    { 34,226, 18,210, 46,238, 30,222, 33,225, 17,209, 45,237, 29,221},
    {162, 98,146, 82,174,110,158, 94,161, 97,145, 81,173,109,157, 93},
    { 10,202, 58,250,  6,198, 54,246,  9,201, 57,249,  5,197, 53,245},
    {138, 74,186,122,134, 70,182,118,137, 73,185,121,133, 69,181,117},
    { 42,234, 26,218, 38,230, 22,214, 41,233, 25,217, 37,229, 21,213},
    {170,106,154, 90,166,102,150, 86,169,105,153, 89,165,101,149, 85}
};

/*
 * Dith_Number of squares in dither.
 * Log2 of that.
 * All bits set to mask for 0..Dith_NumD-1
 */

#define Dith_NumD 256
#define Dith_NumDb 8
#define MaskD (Dith_NumD-1)

/*=====================================================================
** Merges r, g, and b (less than Dith_NumRed,Dith_NumGreen,Dith_NumBlue)
** into a single value.
**===================================================================== */

int ddr_gl_dcm_dith_merge_rgb (
    int r,
    int g,
    int b)
{
    return (r)*(Dith_NumGreen*Dith_NumBlue)+(g)*Dith_NumBlue+(b);
}



/* ===================================================================
** Expands a value v to it's full resolution when
** numc is the number of shades (including black).
** ==================================================================== */

int ddr_gl_dcm_dith_expand (
    int v,
    int numc)
{
    return ((v)*((((numc)-1)<<Dith_NumDb)+1))>>Dith_NumVinB;
}



/* ====================================================================
** Compresses a single value v to a number
** from 0 to numc-1, using dither value d.
** ==================================================================== */

int ddr_gl_dcm_dith_compress (
    int v,
    int numc,
    int d)
{
    register int ddr_gl_dcm_dith_cmpr;

    ddr_gl_dcm_dith_cmpr = ddr_gl_dcm_dith_expand(v,numc);

    return((ddr_gl_dcm_dith_cmpr>>Dith_NumDb)
	   +(((MaskD&ddr_gl_dcm_dith_cmpr)>d)?1:0));
}



/* =======================================================================
** Returns a color index from x,y,r,g,b
** ==================================================================== */

int ddr_gl_dcm_dith_convert_3_to_1 (
    int x,
    int y,
    int r,
    int g,
    int b)
{
    register int ddr_gl_dcm_dith_conv;

    ddr_gl_dcm_dith_conv=ddr_gl_dcm_dith_table[(x)&15][(y)&15];

    return(ddr_gl_dcm_dith_merge_rgb
	   (ddr_gl_dcm_dith_compress(r,Dith_NumRed,   ddr_gl_dcm_dith_conv),
	    ddr_gl_dcm_dith_compress(g,Dith_NumGreen, ddr_gl_dcm_dith_conv),
	    ddr_gl_dcm_dith_compress(b,Dith_NumBlue , ddr_gl_dcm_dith_conv)));
}



/* ====================================================================== */

ddr_gl_dcm_dith_color_table *ddr_gl_dcm_dith_make_color_table (void)
{
    int i,r,g,b;
    ddr_gl_dcm_dith_color_table *ctp;

    ctp = (ddr_gl_dcm_dith_color_table *)malloc
	    (sizeof(ddr_gl_dcm_dith_color_table));

    for (i=0; i<Dith_NumColors; i++) {

	r = i/(Dith_NumBlue*Dith_NumGreen);
	g = (i/Dith_NumBlue)%Dith_NumGreen;
	b = i%Dith_NumBlue;

	ctp->red[i] = (r*(Dith_NumVout-1))/(Dith_NumRed-1);
	ctp->green[i] = (g*(Dith_NumVout-1))/(Dith_NumGreen-1);
	ctp->blue[i] = (b*(Dith_NumVout-1))/(Dith_NumBlue-1);
    }

    return(ctp);
}
