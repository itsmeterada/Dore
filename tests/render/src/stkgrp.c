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
The stacked group utility functions here are used to allow arbitrary
nesting of open groups not currently provided by the the current
Dore group functions.

Groups may be opened using "open_stacked_group()" which opens a particular
group, or "stacked_group()" which creates a potentially open group.
These functions operate very similarly to "DgOpen()", "DoGroup()" and
"DoInlineGroup()" except that they can be called regardless of wether a
group is currently open. The newly opened group becomes the currently
open group until it is closed with "close_stacked_group()" at which
time, the previously open group is restored.

NOTE: groups opened with one of these functions MUST be closed with
"close_stacked_group()". It is a bad error to use "DgClose()" in this case.

"stacked_group()" takes two flag arguments. The first is the "open" flag
which operates exactly the same as the "open" flag to "DoGroup()".
The second flag is the "inline" flag which causes an inline type group
to be returned if the flag is "DcTrue".

With this facility, arbitrary group hierarchys can be created on the fly.
For example, the following function would create and return a group
containing two sub-groups:
		
		#include "util.h"

		DtObject make_group()
		{
			stacked_group(DcTrue, DcFalse);
				DgAddObj(Do......
				DgAddObj(Do......
				stacked_group(DcTrue, DcFalse);
					DgAddObj(Do......
					DgAddObj(Do......
					DgAddObj(Do......
				DgAddObj(close_stacked_group());
				DgAddObj(Do......
				DgAddObj(Do......
				stacked_group(DcTrue, DcFalse);
					DgAddObj(Do......
					DgAddObj(Do......
					DgAddObj(Do......
				DgAddObj(close_stacked_group());
			return(close_stacked_group());
		}

A function of this type could be called succesfully regardless of whether a
group were currently open. Notice also that it doesn't need any temporrary
group handles.

*****************************************************************************/

#include "dore.h"

#define STACK_SIZE 100



static DtObject group_stack[STACK_SIZE];
static int stack_top = 0;

static GroupPush(Handle)
DtObject Handle;
{
	if(stack_top >= STACK_SIZE)
		{
		printf("group stack overflow!\n");
		exit(1);
		}
	group_stack[stack_top++] = Handle;
}




static DtObject GroupPop()
{
	if(stack_top < 1)
		stack_top == 1;
	return(group_stack[--stack_top]);
}







DtObject stacked_group(open_flag, inline_flag)
DtFlag open_flag, inline_flag;
{
	DtObject cur_group, DgInqOpen(), DgClose(),
		DoGroup(), DoInLineGroup();

	if(DcNullObject != (cur_group = DgInqOpen()))
		{
		GroupPush(cur_group);
		DgClose();
		}
	if(inline_flag)
		return(DoInLineGroup(open_flag));
	else
		return(DoGroup(open_flag));
}







DtObject close_stacked_group()
{
	DtObject closed_group, parent_group, DgClose();

	closed_group = DgClose();
	parent_group = GroupPop();
	if(parent_group != DcNullObject)
		DgOpen(parent_group, DcFalse);
	return(closed_group);
}







void open_stacked_group(the_group)
DtObject the_group;
{
	DtObject cur_group, DgInqOpen(), DgClose();

	if(DcNullObject != (cur_group = DgInqOpen()))
		{
		GroupPush(cur_group);
		DgClose();
		}
	DgOpen(the_group, DcFalse);
}
