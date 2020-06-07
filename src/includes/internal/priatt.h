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
 

#ifndef DORE_PRIATT_H
#define DORE_PRIATT_H
 
/*****************************************************************************
**  This file contains function prototypes for Dore' primitive attributes.
*****************************************************************************/

#include <dore/internal/clip3d.h>
#include <dore/internal/glbatt.h>

    /* Primitive Attribute Function Prototypes */

dot_object* dor_ambint_create  (DtReal);
void        dor_ambint_destroy  (dot_object *);
void        dor_ambint_execute  (dot_object *);
void        dor_ambint_initialize  (void);
void        dor_ambint_print  (dot_object *);
dot_object* dor_ambswi_create  (DtSwitch);
void        dor_ambswi_execute  (dot_object *);
void        dor_ambswi_initialize  (void);
void        dor_ambswi_print  (dot_object *);
dot_object* dor_bacfacculble_create  (DtSwitch);
void        dor_bacfacculble_execute  (dot_object *);
void        dor_bacfacculble_initialize  (void);
void        dor_bacfacculble_print  (dot_object *);
dot_object* dor_bacfacculswi_create  (DtSwitch);
void        dor_bacfacculswi_execute  (dot_object *);
void        dor_bacfacculswi_initialize  (void);
void        dor_bacfacculswi_print  (dot_object *);
DtReal      dor_bndvol_compute_current_extension2  (DtVolume *);
dot_object* dor_bndvol_create  (DtVolume *, DtObject);
void        dor_bndvol_destroy  (dot_object *);
void        dor_bndvol_device_coord_compare  (dot_clip3d_vertex *, dot_clip3d_vertex *, dot_clip3d_vertex *, DtInt, DtPtr);
void        dor_bndvol_execute  (dot_object *);
void        dor_bndvol_initialize  (void);
void        dor_bndvol_print  (dot_object *);
dot_object* dor_bndvolswi_create  (DtSwitch);
void        dor_bndvolswi_execute  (dot_object *);
void        dor_bndvolswi_initialize  (void);
void        dor_bndvolswi_print  (dot_object *);
void        dor_clpswi_initialize  (void);
dot_object *dor_clpswi_create  (DtSwitch);
void        dor_clpswi_print  (dot_object *);
void        dor_clpswi_execute  (dot_object *);
void        dor_clpvol_initialize  (void);
dot_object *dor_clpvol_create  (DtClipOperator, DtInt, DtHalfSpace *);
void        dor_clpvol_destroy  (dot_object *);
void        dor_clpvol_print  (dot_object *);
void        dor_clpvol_execute  (dot_object *);
dot_object* dor_cmptexuvcbk_create  (DtObject);
void        dor_cmptexuvcbk_destroy  (DtObject);
void        dor_cmptexuvcbk_execute  (DtObject);
void        dor_cmptexuvcbk_initialize  (void);
void        dor_cmptexuvcbk_initialize_types  (void);
void        dor_cmptexuvcbk_print  (DtObject);
dot_object* dor_cmptexuvswi_create  (DtSwitch);
void        dor_cmptexuvswi_execute  (dot_object *);
void        dor_cmptexuvswi_initialize  (void);
void        dor_cmptexuvswi_print  (dot_object *);
dot_object* dor_cmptexuvwcbk_create  (DtObject);
void        dor_cmptexuvwcbk_destroy  (DtObject);
void        dor_cmptexuvwcbk_execute  (DtObject);
void        dor_cmptexuvwcbk_initialize  (void);
void        dor_cmptexuvwcbk_initialize_types  (void);
void        dor_cmptexuvwcbk_print  (DtObject);
dot_object* dor_cmptexuvwswi_create  (DtSwitch);
void        dor_cmptexuvwswi_execute  (dot_object *);
void        dor_cmptexuvwswi_initialize  (void);
void        dor_cmptexuvwswi_print  (dot_object *);
dot_object* dor_depcue_create
		(DtReal, DtReal, DtReal, DtReal, DtColorModel, DtReal[]);
void        dor_depcue_destroy  (dot_object *);
void        dor_depcue_execute  (dot_object *);
void        dor_depcue_initialize  (void);
void        dor_depcue_print  (dot_object *);
dot_object* dor_depcueswi_create  (DtSwitch);
void        dor_depcueswi_execute  (dot_object *);
void        dor_depcueswi_initialize  (void);
void        dor_depcueswi_print  (dot_object *);
dot_object* dor_difclr_create  (DtColorModel, DtReal []);
void        dor_difclr_destroy  (dot_object *);
void        dor_difclr_execute  (dot_object *);
void        dor_difclr_initialize  (void);
void        dor_difclr_print  (dot_object *);
dot_object* dor_difint_create  (DtReal);
void        dor_difint_destroy  (dot_object *);
void        dor_difint_execute  (dot_object *);
void        dor_difint_initialize  (void);
void        dor_difint_print  (dot_object *);
dot_object* dor_difswi_create  (DtSwitch);
void        dor_difswi_execute  (dot_object *);
void        dor_difswi_initialize  (void);
void        dor_difswi_print  (dot_object *);
dot_object* dor_emisclr_create  (DtColorModel, DtReal[]);
void        dor_emisclr_destroy  (dot_object *);
void        dor_emisclr_execute  (dot_object *);
void        dor_emisclr_initialize  (void);
void        dor_emisclr_print  (dot_object *);
dot_object* dor_emisfct_create  (DtReal);
void        dor_emisfct_execute  (dot_object *);
void        dor_emisfct_initialize  (void);
void        dor_emisfct_print  (dot_object *);
dot_object* dor_emisswi_create  (DtSwitch);
void        dor_emisswi_execute  (dot_object *);
void        dor_emisswi_initialize  (void);
void        dor_emisswi_print  (dot_object *);
dot_object* dor_hidsrfswi_create  (DtSwitch);
void        dor_hidsrfswi_execute  (dot_object *);
void        dor_hidsrfswi_initialize  (void);
void        dor_hidsrfswi_print  (dot_object *);
dot_object* dor_inttyp_create  (DtInterpType);
void        dor_inttyp_execute  (dot_object *);
void        dor_inttyp_initialize  (void);
void        dor_inttyp_print  (dot_object *);
dot_object* dor_invswi_create  (DtSwitch);
void        dor_invswi_execute  (dot_object *);
void        dor_invswi_initialize  (void);
void        dor_invswi_print  (dot_object *);
dot_object* dor_lgtswi_create  (DtObject, DtSwitch);
void        dor_lgtswi_destroy  (dot_object *);
void        dor_lgtswi_execute  (dot_object *);
void        dor_lgtswi_initialize  (void);
void        dor_lgtswi_print  (dot_object *);
dot_object* dor_lintyp_create  (DtLineType);
void        dor_lintyp_execute  (dot_object *);
void        dor_lintyp_initialize  (void);
void        dor_lintyp_print  (dot_object *);
struct object* dor_linwid_create  (DtReal);
void        dor_linwid_destroy  (dot_object *);
void        dor_linwid_execute  (struct object *);
void        dor_linwid_initialize  (void);
void        dor_linwid_print  (dot_object *);
dot_object* dor_localaasty_create  (DtLocalAntiAliasStyle);
void        dor_localaasty_destroy  (dot_object *);
void        dor_localaasty_execute  (dot_object *);
void        dor_localaasty_initialize  (void);
void        dor_localaasty_print  (dot_object *);
dot_object* dor_localaaswi_create  (DtSwitch);
void        dor_localaaswi_destroy  (dot_object *);
void        dor_localaaswi_execute  (dot_object *);
void        dor_localaaswi_initialize  (void);
void        dor_localaaswi_print  (dot_object *);
struct object* dor_minbndext_create  (DtReal);
void        dor_minbndext_destroy  (dot_object *);
void        dor_minbndext_execute  (struct object *);
void        dor_minbndext_initialize  (void);
void        dor_minbndext_print  (dot_object *);
dot_object* dor_mkrfnt_create  (DtFont);
void        dor_mkrfnt_execute  (dot_object *);
void        dor_mkrfnt_initialize  (void);
void        dor_mkrfnt_print  (dot_object *);
dot_object* dor_mkrgly_create  (DtInt);
void        dor_mkrgly_execute  (dot_object *);
void        dor_mkrgly_initialize  (void);
void        dor_mkrgly_print  (dot_object *);
struct object* dor_mkrscl_create  (DtReal);
void        dor_mkrscl_execute  (struct object *);
void        dor_mkrscl_initialize  (void);
void        dor_mkrscl_print  (dot_object *);
dot_object* dor_pckswi_create  (DtSwitch);
void        dor_pckswi_execute  (dot_object *);
void        dor_pckswi_initialize  (void);
void        dor_pckswi_print  (dot_object *);
dot_object* dor_radambswi_create  (DtSwitch);
void        dor_radambswi_execute  (dot_object *);
void        dor_radambswi_initialize  (void);
void        dor_radambswi_print  (dot_object *);
dot_object* dor_radswi_create  (DtSwitch);
void        dor_radswi_execute  (dot_object *);
void        dor_radswi_initialize  (void);
void        dor_radswi_print  (dot_object *);
dot_object* dor_reflecswi_create  (DtSwitch);
void        dor_reflecswi_execute  (dot_object *);
void        dor_reflecswi_initialize  (void);
void        dor_reflecswi_print  (dot_object *);
dot_object* dor_refractidx_create  (DtReal);
void        dor_refractidx_destroy  (dot_object *);
void        dor_refractidx_execute  (dot_object *);
void        dor_refractidx_initialize  (void);
void        dor_refractidx_print  (dot_object *);
dot_object* dor_refractswi_create  (DtSwitch);
void        dor_refractswi_execute  (dot_object *);
void        dor_refractswi_initialize  (void);
void        dor_refractswi_print  (dot_object *);
dot_object* dor_reptyp_create  (DtRepType);
void        dor_reptyp_execute  (dot_object *);
void        dor_reptyp_initialize  (void);
void        dor_reptyp_print  (dot_object *);
dot_object* dor_shadowswi_create  (DtSwitch);
void        dor_shadowswi_execute  (dot_object *);
void        dor_shadowswi_initialize  (void);
void        dor_shadowswi_print  (dot_object *);
dot_object* dor_shdidx_create  (DtInt);
void        dor_shdidx_execute  (dot_object *);
void        dor_shdidx_initialize  (void);
void        dor_shdidx_print  (dot_object *);
dot_object* dor_spcclr_create  (DtColorModel, DtReal []);
void        dor_spcclr_destroy  (dot_object *);
void        dor_spcclr_execute  (dot_object *);
void        dor_spcclr_initialize  (void);
void        dor_spcclr_print  (dot_object *);
dot_object* dor_spcfct_create  (DtReal);
void        dor_spcfct_execute  (dot_object *);
void        dor_spcfct_initialize  (void);
void        dor_spcfct_print  (dot_object *);
dot_object* dor_spcint_create  (DtReal);
void        dor_spcint_destroy  (dot_object *);
void        dor_spcint_execute  (dot_object *);
void        dor_spcint_initialize  (void);
void        dor_spcint_print  (dot_object *);
dot_object* dor_spcswi_create  (DtSwitch);
void        dor_spcswi_execute  (dot_object *);
void        dor_spcswi_initialize  (void);
void        dor_spcswi_print  (dot_object *);
dot_object* dor_srfedgclr_create  (DtColorModel, DtReal []);
void        dor_srfedgclr_destroy  (dot_object *);
void        dor_srfedgclr_execute  (dot_object *);
void        dor_srfedgclr_initialize  (void);
void        dor_srfedgclr_print  (dot_object *);
void        dor_srfshd_constant_handler  (void);
dot_object* dor_srfshd_create  (DtObject);
void        dor_srfshd_destroy  (dot_object *);
void        dor_srfshd_execute  (dot_object *);
void        dor_srfshd_initialize  (void);
void        dor_srfshd_initialize_types  (void);
void        dor_srfshd_lightsource_handler  (void);
void        dor_srfshd_print  (dot_object *);
dot_object* dor_subdivspc_create  (DtInt, DtReal *);
void        dor_subdivspc_destroy  (dot_object *);
void        dor_subdivspc_execute  (dot_object *);
void        dor_subdivspc_initialize  (void);
void        dor_subdivspc_print  (dot_object *);
dot_object* dor_transpclr_create  (DtColorModel, DtReal []);
void        dor_transpclr_destroy  (dot_object *);
void        dor_transpclr_execute  (dot_object *);
void        dor_transpclr_initialize  (void);
void        dor_transpclr_print  (dot_object *);
dot_object* dor_transpint_create  (DtReal);
void        dor_transpint_destroy  (dot_object *);
void        dor_transpint_execute  (dot_object *);
void        dor_transpint_initialize  (void);
void        dor_transpint_print  (dot_object *);
dot_object* dor_transporientclr_create  (DtColorModel, DtReal []);
void        dor_transporientclr_destroy  (dot_object *);
void        dor_transporientclr_execute  (dot_object *);
void        dor_transporientclr_initialize  (void);
void        dor_transporientclr_print  (dot_object *);
dot_object* dor_transporientexp_create  (DtReal);
void        dor_transporientexp_destroy  (dot_object *);
void        dor_transporientexp_execute  (dot_object *);
void        dor_transporientexp_initialize  (void);
void        dor_transporientexp_print  (dot_object *);
dot_object* dor_transporientint_create  (DtReal);
void        dor_transporientint_destroy  (dot_object *);
void        dor_transporientint_execute  (dot_object *);
void        dor_transporientint_initialize  (void);
void        dor_transporientint_print  (dot_object *);
dot_object* dor_transporientswi_create  (DtSwitch);
void        dor_transporientswi_execute  (dot_object *);
void        dor_transporientswi_initialize  (void);
void        dor_transporientswi_print  (dot_object *);
dot_object* dor_transpswi_create  (DtSwitch);
void        dor_transpswi_execute  (dot_object *);
void        dor_transpswi_initialize  (void);
void        dor_transpswi_print  (dot_object *);
struct object* dor_txtaln_create  (DtTextAlignHorizontal, DtTextAlignVertical);
void        dor_txtaln_destroy  (dot_object *);
void        dor_txtaln_execute  (struct object *);
void        dor_txtaln_initialize  (void);
void        dor_txtaln_print  (dot_object *);
struct object* dor_txtexp_create  (DtReal);
void        dor_txtexp_destroy  (dot_object *);
void        dor_txtexp_execute  (struct object *);
void        dor_txtexp_initialize  (void);
void        dor_txtexp_print  (dot_object *);
dot_object* dor_txtfnt_create  (DtFont);
void        dor_txtfnt_execute  (dot_object *);
void        dor_txtfnt_initialize  (void);
void        dor_txtfnt_print  (dot_object *);
struct object* dor_txthgt_create  (DtReal);
void        dor_txthgt_destroy  (dot_object *);
void        dor_txthgt_execute  (struct object *);
void        dor_txthgt_initialize  (void);
void        dor_txthgt_print  (dot_object *);
dot_object* dor_txtpat_create  (DtTextPath);
void        dor_txtpat_execute  (dot_object *);
void        dor_txtpat_initialize  (void);
void        dor_txtpat_print  (dot_object *);
dot_object* dor_txtprc_create  (DtTextPrecision);
void        dor_txtprc_execute  (dot_object *);
void        dor_txtprc_initialize  (void);
void        dor_txtprc_print  (dot_object *);
struct object* dor_txtspc_create  (DtReal);
void        dor_txtspc_execute  (struct object *);
void        dor_txtspc_initialize  (void);
void        dor_txtspc_print  (dot_object *);
struct object* dor_txtupvec_create  (DtReal, DtReal);
void        dor_txtupvec_execute  (struct object *);
void        dor_txtupvec_initialize  (void);
void        dor_txtupvec_print  (dot_object *);

#endif
