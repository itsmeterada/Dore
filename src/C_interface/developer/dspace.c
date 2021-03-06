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

/* ====================================================================== */

DtPtr DDspace_Allocate (DtInt size)
{
    return(dor_space_allocate(size));
}

/* ====================================================================== */

DtPtr DDspace_Reallocate (
    DtPtr    in_ptr,
    unsigned size)
{
    return(dor_space_reallocate(in_ptr, size));
}

/* ====================================================================== */

void DDspace_Deallocate (DtPtr spaceptr)
{
    dor_space_deallocate (spaceptr);
}
