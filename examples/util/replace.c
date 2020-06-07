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
 
#include <dore/dore.h>
#include "dore_util.h"


/*
Recursively examine all elements of a group and substitute all
occurrences of elements of a given type with a new object.
The number of substitutions made is returned.
*/

DtInt replace_type_with (
    DtObject group,
    DtInt search_type,
    DtObject with)
{
	DtInt type, group_size, i, substitutions=0;
	DtObject cur_ele, DgInqObjAtPos();

	type = DsInqObjType(group);
	if(! ((type == DcTypeGroup) || (type == DcTypeInlineGroup))) {
		printf("replace_type_with: not passed a group\n");
		return;
	}
	group_size = DgInqSize(group);
	DgOpen(group, DcFalse);
		for(i=0; i<group_size; i++) {
			cur_ele = DgInqObjAtPos(group, i, DcBeginning);
			type = DsInqObjType(cur_ele);
			if(type == search_type) {
				substitutions++;
				DgSetElePtr(i, DcBeginning);
				DgReplaceObj(with);
			}
			else if((type == DcTypeGroup) ||
			        (type == DcTypeInlineGroup)) {
				substitutions += replace_type_with(cur_ele,
					search_type, with);
			}
		}
	DgClose();
	return(substitutions);
} /* end of replace.c */
