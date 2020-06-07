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

int DSQRNS (names, length)
    struct {
	char *name;
	int len;
    } *names;
    DtInt *length;
{
    char **c_names;
    DtInt count;
    DtInt i, j;
    int len;
    char *n;

    count = dor_render_inq_num_renderers();
    c_names = (char **)dor_space_allocate(count*sizeof(char *));

    dor_render_inq_renderer_names(c_names);

    for (i=0, n=names->name; i<count; i++, n+= names->len) {
      if ((len = strlen(c_names[i])) < names->len) {
	 for (j=0; j < len; j++)
	       n[j] = c_names[i][j];
	 for (;j < names->len; j++) 
	       n[j] = '\0';
      } else {
	 for (j=0; j < names->len; j++) 
	       n[j] = c_names[i][j];
      }
    }
    dor_space_deallocate (c_names);
}
