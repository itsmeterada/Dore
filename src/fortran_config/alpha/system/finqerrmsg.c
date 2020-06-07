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
 
#include <string.h>
#include "../fortran.h"
#include <dore/internal/error.h>
#include <dore/internal/system.h>

DtReadStatus dsqem_ (errornumber, bufsiz, buf, severity, fstrlen)
    DtInt *errornumber;
    DtInt *bufsiz;
    DtPtr *buf;
    DtInt *severity;
    int fstrlen;
{
    DtReadStatus res;
    DtInt i;
    DtErrorStatus errstat;
    char *local_buf;

    local_buf = (char *)buf;

    res = dor_error_inquire_message(*errornumber,fstrlen,local_buf,&errstat);

    *severity = errstat;

    /* Fortran does not use NULL termination so just pad the rest of
    ** the string with spaces */

    for (i = strlen(local_buf); i < fstrlen; i++) local_buf[i] = ' ';
    return res;
}
