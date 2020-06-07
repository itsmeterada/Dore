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

#ifndef DORE_ERROR_H
#define DORE_ERROR_H

    /***  Function Prototypes  ***/

void         dor_error_initialize        (void);
void         dor_error                   (DtInt, char*, char*);
char*        dor_error_get_line          (void);
DtReadStatus dor_error_inquire_message   (DtInt, DtInt, char[], DtErrorStatus*);
void         dor_error_inquire_variables (DtInt*, DtPFV*);
void         dor_error_set_variables     (DtInt, DtPFV, DtFlag);

#endif
