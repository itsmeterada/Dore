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
 

/*****************************************************************************
**  This header file contains declarations and definitions for the global
**  attribute object functions.
*****************************************************************************/

    /***  Include Files  ***/

#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>


    /***  Local Function Prototypes  ***/

struct gas* dor_gas_create           (DtPFV);
DtFlag      dor_gas_get_value        (struct gas*, DtPtr*);
void        dor_gas_initialize       (void);
DtInt       dor_gas_pop_group        (struct gas*);
DtFlag      dor_gas_pop_value        (struct gas*, DtPtr*);
void        dor_gas_print            (struct gas*);
DtFlag      dor_gas_push_value       (struct gas*, DtPtr);
void        dor_gas_set_debug_switch (DtFlag);
DtFlag      dor_gas_set_value        (struct gas*, DtPtr);
DtFlag      dor_gas_test_set_value   (struct gas*);
