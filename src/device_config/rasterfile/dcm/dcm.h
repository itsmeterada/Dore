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
 
#include <dore/internal/dogen.h>
#include <dore/dore_develop/develop.h>

typedef struct {
    DtInt		width;
    DtInt		height;
    DtInt		depth;
    DtRasterType	type;
    FILE *		fp;
    DtPtr		data;
    long		bindata;
} ddt_rasterfile_data;

extern ddt_rasterfile_data *dde_rasterfile_current_device_data;


    /***  Function Prototypes  ***/

void   ddr_rasterfile_dcm_become_current_driver  (DtPtr);
void   ddr_rasterfile_dcm_close_device  (void);
DtPtr  ddr_rasterfile_dcm_create_local_data  (DtObject);
void   ddr_rasterfile_dcm_flush  (void);
DtFlag ddr_rasterfile_dcm_initialize_device  (DtObject, DtPtr, DtPtr, DtPtr);
void   ddr_rasterfile_dcm_inquire_auto_size  (DtFlag *);
void   ddr_rasterfile_dcm_inquire_clip_list(DtUInt*,DtReal**,DtReal[4],DtFlag*);
void   ddr_rasterfile_dcm_inquire_color_entries  (DtColorModel,int,int,DtReal*);
void   ddr_rasterfile_dcm_inquire_device_extent  (DtVolume *);
void   ddr_rasterfile_dcm_inquire_ncolors  (DtInt *);
DtFlag ddr_rasterfile_dcm_inquire_pixel_data  (
		DtRasterType, DtInt*, DtInt*, DtRasterType*, DtPtr*, DtFlag*);
void   ddr_rasterfile_dcm_inquire_resolution  (DtReal*, DtReal*);
void   ddr_rasterfile_dcm_inquire_visual_type  (DtVisualType*);
void   ddr_rasterfile_dcm_write_scanline_byte  (DtInt, DtInt, DtInt, DtUChar*);
DtPtr  ddr_rasterfile_return_DCM_fcns  (void);
