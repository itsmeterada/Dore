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
 
/* ======================================================================
  Functions:
	ddr_iris_to_dore_matrix
	ddr_dore_to_iris_matrix
	ddr_iris_loadmatrix_id
	ddr_iris_id_matrix
	ddr_print_current_matrix
	ddr_set_iris_material_color_range
	ddr_set_iris_material_color_RGB
	ddr_iris_set_diffuse_color
	ddr_iris_restore_diffuse_color
	ddr_iris_restore_depcue
	ddr_iris_set_depcue_color
	ddr_iris_restore_specular_factor
	ddr_iris_restore_specular_color
	ddr_iris_init_patterns
	ddr_iris_restore_transparency

  ====================================================================== */

#include <stdio.h>
#include "../gl_driver.h"



/* ====================================================================== */

void ddr_iris_to_dore_matrix (
    Matrix      iris_matrix,
    DtMatrix4x4 dore_matrix)
{
    register int i,j;

    for (i=0;  i < 4;  ++i)
	for (j=0;  j < 4;  ++j)
	    dore_matrix[i][j] = iris_matrix[j][i];
}



/* ====================================================================== */

void ddr_dore_to_iris_matrix (
    DtMatrix4x4 dore_matrix,
    Matrix      iris_matrix)
{
    register int i,j;

    for (i=0; i<4; i++)
	for (j=0; j<4; j++)
	    iris_matrix[i][j] = dore_matrix[j][i];
}



/* ====================================================================== */

void ddr_iris_loadmatrix_id (void)
{
    static Matrix iris_identity = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
    };

    loadmatrix (iris_identity);
}



/* ====================================================================== */

void ddr_iris_id_matrix (Matrix matrix)
{
    register int i,j;

    for (i=0;  i < 4;  ++i)
	for (j=0; j < 4;  ++j)
	    matrix[i][j] = (i==j) ? 1.0 : 0.0;
}



/* ====================================================================== */

void ddr_print_current_matrix (void)
{
    Matrix matrix;
    int    x, y;

    getmatrix (matrix);

    printf ("Current matrix:\n");

    for (y=0;  y < 4;  ++y) {
	printf ("\t");
	for (x=0;  x < 4;  ++x)
	    printf ("%14g ", matrix[y][x]);
	printf ("\n");
    }
}



/* ====================================================================== */

void ddr_set_iris_material_color_range (int from, int to)
{
    auto float props[10];
    auto int   prop = 0;

    props[prop++] = COLORINDEXES;
    props[prop++] = from;
    props[prop++] = to;
    props[prop++] = to;
    props[prop++] = LMNULL;

    lmdef (DEFMATERIAL, 1, prop, props);
}



/*=====================================================================
**  Does the actual Iris calls to set the color index/RGBcolor to r,g,b.
**
**  Looks at DevStatus.lighting, DevStatus.visualtype,
**  DevStatus.diffuseswitch, and DefStatus.ambientswitch to
**  determine what actions to take.
**===================================================================*/

void ddr_iris_set_diffuse_color (DtReal diffuse_color[3])
{
    /* Depth-cueing */

    if (DevStatus.depcueswitch) {
	ddr_iris_set_depcue_color (diffuse_color);
	return;
    }

    /* Pseudo-Color */

    if (DevStatus.visualtype == DcPseudoColor) {
	auto DtReal amp;

	amp = RGBshade (diffuse_color[0], diffuse_color[1], diffuse_color[2]);

	/* PseudoColor with lighting model: */

	if (DevStatus.lighting) {
	    ddr_set_iris_material_color_range
	    (   DevStatus.rampmin,
		DevStatus.rampmin + 
		    (int)((DevStatus.rampmax - DevStatus.rampmin) * amp)
	    );
	    return;
	}

	/* PseudoColor, no lighting.  */

	color (DevStatus.rampmin
	      + (int)((DevStatus.rampmax - DevStatus.rampmin) * amp));
	return;
    }

    /* True Color */

    SET_COLOR (diffuse_color);
}



/* ====================================================================
** Tells the iris to use the diffuse color as specified in DevStatus.
** ==================================================================== */

void ddr_iris_restore_diffuse_color (void)
{
    ddr_iris_set_diffuse_color (DevStatus.diffusecolor);
}



/* ====================================================================== */

void ddr_iris_restore_depcue (void)
{
    ddr_iris_set_depcue_color (DevStatus.diffusecolor);
}



/* ====================================================================== */

void ddr_iris_set_depcue_color (DtReal col[3])
{
    DtReal backcolor[3];
    DtReal frontcolor[3];
    DtReal zback, zfront;
    int i;

    for (i=0;  i < 3;  i++)
	backcolor[i] = DevStatus.depcuesback * col[i] * DevStatus.diffuseintens
		     + (1.0 - DevStatus.depcuesback) * DevStatus.depcuecolor[i];

    for (i=0; i<3; i++)
	frontcolor[i] = DevStatus.depcuesfront * col[i] *DevStatus.diffuseintens
		      + (1.0-DevStatus.depcuesfront) * DevStatus.depcuecolor[i];

    zback = (1.0 - DevStatus.depcuezback)  * (DtReal)0x7FFFFF;
    zfront= (1.0 - DevStatus.depcuezfront) * (DtReal)0x7FFFFF;

    if (DevStatus.visualtype == DcPseudoColor)
	lshaderange
	(   DevStatus.rampmin + (int)((DevStatus.rampmax-DevStatus.rampmin)*
			  RGBshade(backcolor[0],backcolor[1],backcolor[2])),
	    DevStatus.rampmin + (int)((DevStatus.rampmax-DevStatus.rampmin)*
			  RGBshade(frontcolor[0],frontcolor[1],frontcolor[2])),
	    (long)zfront,
	    (long)zback
	);
    else
	lRGBrange
	(   (short)(255.0 * backcolor[0]),
	    (short)(255.0 * backcolor[1]),
	    (short)(255.0 * backcolor[2]),
	    (short)(255.0 * frontcolor[0]),
	    (short)(255.0 * frontcolor[1]),
	    (short)(255.0 * frontcolor[2]),
	    (long) zfront,
	    (long) zback
	);

#   ifdef DEBUGP
	printf("Set RGBrange(%f,%f,%f:%f,%f,%f:%f,%f)\n",
	       backcolor[0], backcolor[1], backcolor[2],
	       frontcolor[0], frontcolor[1], frontcolor[2],
	       zfront, zback);
#   endif
}



/* ======================================================================
** Restore the specular factor as specified in
** DevStatus.specularfactor AND DevStatus.specularswitch.
**
** (Does nothing in pseudocolor mode)
** ==================================================================== */

void ddr_iris_restore_specular_factor (void)
{
    float props[10];
    int prop;

    /* No specular in pseudocolor! */

    if (DevStatus.visualtype == DcPseudoColor)
	return;

    prop = 0;
    props[prop++] = SHININESS;
    props[prop++] = DevStatus.specularswitch ? DevStatus.specularfactor : 0.0;
    props[prop++] = LMNULL;

    lmdef (DEFMATERIAL, 1, prop, props);
}



/* ======================================================================
** Restores the specular color as specified in
** DevStatus.
**
** (Does nothing in pseudocolor mode)
** ==================================================================== */

void ddr_iris_restore_specular_color (void)
{
    auto float props[10];
    auto int prop;

    /* No specular in pseudocolor! */

    if (DevStatus.visualtype == DcPseudoColor)
	return;

    prop = 0;
    props[prop++] = SPECULAR;
    props[prop++] = DevStatus.specularcolor[0]*DevStatus.specularintens;
    props[prop++] = DevStatus.specularcolor[1]*DevStatus.specularintens;
    props[prop++] = DevStatus.specularcolor[2]*DevStatus.specularintens;
    props[prop++] = LMNULL;

    lmdef (DEFMATERIAL, 1, prop, props);
}



/* ======================================================================
** Pattern 0 is always opaque,
** and Pattern MaxPattern is
** always invisible.
** ==================================================================== */

#define DitherSize 4
#define DitherIndex(i) ((i)&(DitherSize-1))
#define DitherVal(x,y) (dde_iris_pattern_dither[DitherIndex(x)][DitherIndex(y)])
#define MaxPattern (DitherSize*DitherSize)

static int dde_iris_patterns_initialized = 0;

static int dde_iris_pattern_dither[DitherSize][DitherSize] = {
    { 0,  8,  2, 10 },
    {12,  4, 14,  6 },
    { 3, 11,  1,  9 },
    {15,  7, 13,  5 }
};



/* ====================================================================== */

void ddr_iris_init_patterns (void)
{
    unsigned short pattern[16],p;
    int i,j,k;

    for (i=1;  i <= MaxPattern;  ++i) {
	for (j=0;  j < 16;  ++j) {
	    for (p=0, k=0;  k < 16;  ++k) {
		p<<=1;
		if (i<=DitherVal(j,k)) p |= 1;
	    }
	    pattern[j] = p;
	}
	defpattern(i,16,pattern);
    }
    dde_iris_patterns_initialized = 1;
}



/* ======================================================================
** Restores the transparency to the
** parameters defined in DevStatus.
** ==================================================================== */

void ddr_iris_restore_transparency (void)
{
    auto DtReal t;
    auto short pat;

    if (!dde_iris_patterns_initialized)
	ddr_iris_init_patterns();

    t = (DevStatus.transpswitch) ? DevStatus.transpintens : 0.0;

    pat = t * (MaxPattern+1);

    setpattern (CLAMP (pat, 0, MaxPattern));
}
