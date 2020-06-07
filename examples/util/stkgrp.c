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
 

/*****************************************************************************
				STACKED GROUPS

Previous to the 2.1 release, it was an error to attempt to open a group when
another was currently open.  The stacked group utility functions allowed
users to get around this restriction.  Now the restriction has been removed
from Dore so this utility is obsoleted.  The stubs are here included for one
more release so that users can make an easy transition.

*****************************************************************************/

#include <dore/dore.h>
#include "dore_util.h"




DtObject stacked_group (
    DtFlag open_flag,
    DtFlag inline_flag)
{
	if(inline_flag) {
		printf("stacked_group utility obsolete. Use DoInLineGroup\n");
		return(DoInLineGroup(open_flag));
	}
	else {
		printf("stacked_group utility obsolete. Use DoGroup\n");
		return(DoGroup(open_flag));
	}
}


DtObject close_stacked_group (void)
{
	printf("close_stacked_group utility obsolete. Use DgClose\n");
	return(DgClose());
}


void open_stacked_group (
    DtObject the_group)
{
	printf("open_stacked_group utility obsolete. Use DgOpen\n");
	DgOpen(the_group, DcFalse);
}
