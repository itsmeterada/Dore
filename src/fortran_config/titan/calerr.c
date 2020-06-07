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

void fortran_call_error_handler (handler, fd, err_number, funcname, string)
    DtPFV handler;
    DtInt fd;
    DtInt err_number;
    char *funcname;
    char *string;
{
   struct {
      char *cp;
      int len;
   } f_funcname, f_string;
   int local_fd = fd;
   int local_err_number = err_number;

   f_funcname.len = strlen (funcname);
   f_funcname.cp = funcname;

   f_string.len = strlen (string);
   f_string.cp  = string;

   (*handler)(&local_fd, &local_err_number, &f_funcname, 
		&f_funcname.len, &f_string, &f_string.len);
}
