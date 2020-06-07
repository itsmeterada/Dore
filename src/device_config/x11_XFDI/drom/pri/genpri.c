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
	DtPtr ddr_x11_XFDI_drom_cvtclrs_create (object)
	int ddr_x11_XFDI_drom_cvtclrs_print (object, data)
	int ddr_x11_XFDI_drom_cvtclrs_delete (object, data)

  ======================================================================
 */
#include "../drom.h"
/*
 ======================================================================
 */
DtPtr
ddr_x11_XFDI_drom_cvtclrs_create(object)
     DtObject object;
{
    ddt_x11_XFDI_drom_cvtclrs_data *data;

    if ((data = (ddt_x11_XFDI_drom_cvtclrs_data *)
	 DDspace_Allocate(sizeof(ddt_x11_XFDI_drom_cvtclrs_data)))
	== (ddt_x11_XFDI_drom_cvtclrs_data *)0) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE,
		    "ddr_x11_XFDI_drom_cvtclrs_create",
		    "sizeof (ddt_x11_XFDI_drom_cvtclrs_data)");
	return DcNullPtr;
    }

    data->converted_clrs = (unsigned long *)0;
    data->sequence_number = 0;

    return (DtPtr)data;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_cvtclrs_print(object, data)
     DtObject object;
     ddt_x11_XFDI_drom_cvtclrs_data *data;
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_cvtclrs_delete(object, data)
     DtObject object;
     ddt_x11_XFDI_drom_cvtclrs_data *data;
{
    if (data->converted_clrs != NULL)
	    DDspace_Deallocate(data->converted_clrs);
    DDspace_Deallocate(data);
}

     
