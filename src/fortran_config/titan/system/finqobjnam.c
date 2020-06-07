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

DtInt DSQONT (object)
    DtObject *object;
{
    DtNameType type;
    DtPtr nam;

    dor_object_inq_name(*object, &type, &nam);
    return (DtInt)type;
}

DtInt DSQONI (object)
    DtObject *object;
{
    DtNameType type;
    DtPtr nam;

    dor_object_inq_name(*object, &type, &nam);
    if (type != DcNameInteger)
	return 0;
    return (DtInt)nam;
}

void DSQONS (object, fname, length)
    DtObject *object;
    struct {
	char *fp;
	int   fn;
    } *fname;
    DtInt *length;
{
    DtNameType type;
    int i;
    char *nam;

    dor_object_inq_name(*object, &type, &nam);
    if(type != DcNameString) {
	    *length = 0;
	    return;
    }
    if(fname->fn < (i = strlen(nam))) {
	    *length = 0;
	    return;
    }
    strncpy(fname->fp, nam, i);
    *length = i;
}
