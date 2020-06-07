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
#include "dore/fortran.h"

void fortran_call_error_handler (
    DtPFV  handler,
    DtInt  fd,
    DtInt  err_number,
    char  *funcname,
    char  *string)
{
    int funcname_len;
    int string_len;
    int local_fd = fd;
    int local_err_number = err_number;

    funcname_len = strlen (funcname);
    string_len = strlen(string);

    (*handler) (&local_fd, &local_err_number, funcname, 
		&funcname_len, string, &string_len, 
		funcname_len, string_len);
}
