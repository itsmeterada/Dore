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
	ddr_g4vll_prom_write_rectangle_byte_rgb
	ddr_g4vll_return_PROM_fcns

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/develop.h>
#include "../dcm/dcm.h"
#include "prom.h"

static DDt_PROM	*dde_g4vll_PROM_fcns = 0;


/* ====================================================================== */

void ddr_g4vll_prom_write_rectangle_byte_rgb (
    DtInt    xstart,
    DtInt    ystart,
    DtInt    length,
    DtInt    height,
    DtUChar *pixarr)
{
    DtInt yend;
    DtInt scanlineno;
    DtUChar *pixeladdr;

    pixeladdr = pixarr;
    yend = ystart + height - 1;

    for (scanlineno=ystart;  scanlineno <= yend;  ++scanlineno) {
	ddr_g4vll_dcm_write_scanline_byte (xstart,scanlineno,length,pixeladdr);
	pixeladdr += 3*length;
    }
}

/* ====================================================================== */

DtPtr ddr_g4vll_return_PROM_fcns (void)
{
    DDt_PROM *fcns;

    if (!dde_g4vll_PROM_fcns) {
	fcns = DDdevice_CreatePROMStruct ();

	if (!fcns) {
	    DDerror (ERR_CANT_ALLOC_MEM,"dde_g4vll_return_PROM_fcns",DcNullPtr);
	}

	fcns->write_rectangle_byte_rgb =ddr_g4vll_prom_write_rectangle_byte_rgb;
	dde_g4vll_PROM_fcns = fcns;
    }

    return (DtPtr) dde_g4vll_PROM_fcns;
}
