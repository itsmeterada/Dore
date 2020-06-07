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

DSQDI (count, name, namlength, desc, desclength)
    DtInt *count;
    struct {
	char *name;
	int len;
    } *name, *desc;
    DtInt *namlength;
    DtInt *desclength;
{
    char **c_name;
    char **c_desc;
    DtInt i, j;
    int len;
    char *n;
    char *d;

    c_name = (char **)dor_space_allocate(*count*sizeof(char *));
    c_desc = (char **)dor_space_allocate(*count*sizeof(char *));

    if (c_name == (char **)0 || c_desc == (char **)0) 
	   return;

    dor_device_inq_driver_info(*count, c_name, c_desc);

    for (i=0, n=name->name, d=desc->name;
	i<*count;
	i++, n+= name->len, d+= desc->len) {
       if ((len = strlen(c_name[i])) < name->len) {
	   for (j=0; j < len; j++)
		   n[j] = c_name[i][j];
	   for (;j < name->len; j++) 
		   n[j] = '\0';
       } else {
	   for (j=0; j < name->len; j++) 
		   n[j] = c_name[i][j];
       }
       if ((len = strlen(c_desc[i])) < desc->len) {
	   for (j=0; j < len; j++)
		   d[j] = c_desc[i][j];
	   for (;j < desc->len; j++) 
		   d[j] = '\0';
       } else {
	   for (j=0; j < desc->len; j++) 
		   d[j] = c_desc[i][j];
       }
    }
    dor_space_deallocate (c_name);
    dor_space_deallocate (c_desc);
}
