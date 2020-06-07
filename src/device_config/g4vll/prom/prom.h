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
 

#ifndef DORE_G4VLL_PROM_H
#define DORE_G4VLL_PROM_H

/******************************************************************************
**  This header file contains the function prototypes for the production
**  renderer output module for the G4 VLL driver.
******************************************************************************/

void  ddr_g4vll_prom_write_rectangle_byte_rgb(DtInt,DtInt,DtInt,DtInt,DtUChar*);
DtPtr ddr_g4vll_return_PROM_fcns  (void);

#endif
