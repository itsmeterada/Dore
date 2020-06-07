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
#include <dore/dore_develop/private/device.h>

void dsqdi_ (count, name, namlen, description, deslen, namfstrlen, desfstrlen)
    DtInt *count;
    char *name;
    DtInt *namlen;
    char *description;
    DtInt *deslen;
    int namfstrlen;
    int desfstrlen;
{
    char **c_name;
    char **c_desc;
    DtInt i, j;
    int len;
    char *n;
    char *d;

    c_name = dor_space_allocate (*count * sizeof (char*));
    c_desc = dor_space_allocate (*count * sizeof (char*));

    if (!c_name || !c_desc) return;

    dor_device_inq_driver_info (*count, c_name, c_desc);

    for (i=0, n=name, d=description; i<*count; i++, n+= namfstrlen, d+= desfstrlen) {
	if ((len = strlen(c_name[i])) < namfstrlen) {
	    for (j=0; j < len; j++)
		n[j] = c_name[i][j];
	    for (;j < namfstrlen; j++) 
		n[j] = '\0';
	} else {
	    for (j=0; j < namfstrlen; j++) 
		n[j] = c_name[i][j];
	}
	if ((len = strlen(c_desc[i])) < desfstrlen) {
	    for (j=0; j < len; j++)
		d[j] = c_desc[i][j];
	    for (;j < desfstrlen; j++) 
		d[j] = '\0';
	} else {
	    for (j=0; j < desfstrlen; j++) 
		d[j] = c_desc[i][j];
	}
    }
    dor_space_deallocate (c_name);
    dor_space_deallocate (c_desc);
}
