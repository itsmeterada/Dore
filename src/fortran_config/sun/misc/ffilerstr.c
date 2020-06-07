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

DtObject dofrs_ (filename, flen, specialstring, slen)
    char *filename;
    DtInt *flen;
    char *specialstring;
    DtInt *slen;
{
    static int   nfilename;
    static int   nspcstr;
    static char *sfilename;
    static char *sspcstr;
    static char *nullstr = "DCNULL";

    nfilename = fstr(&sfilename, nfilename, filename, *flen);
    nspcstr = fstr(&sspcstr, nspcstr, specialstring, *slen);

    if (!(strcmp(specialstring,nullstr)))
	return (dor_filerstr_create(sfilename, sspcstr));
    else
	return (dor_filerstr_create(sfilename, DcNullPtr));
}
