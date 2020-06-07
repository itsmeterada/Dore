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
	Returns a copy of an object. If the object is a group hierarchy,
	then a new (different) group hierarchy is returned.
	Note that leaf nodes of the hierarchy (non groups) are not copied
	but a new reference is created to them. For all practical purposes
	however, the object returned by copy_object should be treated
	as a completely new copy and can be edited and released without
	affecting the original copy. The only time that users need to be
	aware of this distinction is when their hierarchies contain
	callback objects; in which case they should look carefully at
	their callback functions.
*/

DtObject copy_object (
    DtObject obj)
{
	DtInt type, group_size, i;
	DtObject new_group;

	type = DsInqObjType(obj);
	switch(type) {
		case DcTypeGroup:
			new_group = DoGroup(DcTrue);
			break;
		case DcTypeInlineGroup:
			new_group = DoInLineGroup(DcTrue);
			break;
		default:
			return(obj);
	}
	group_size = DgInqSize(obj);
	for(i=0; i<group_size; i++)
		DgAddObj(copy_object(DgInqObjAtPos(obj, i, DcBeginning)));
	DgClose();
	return(new_group);
} /* end of copy.c */
