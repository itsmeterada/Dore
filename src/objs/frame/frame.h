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
 
#include "dore/internal/dogen.h"

typedef struct frame {
	dot_object *next;	/* next frame object */
	DtReal left_justification;
	DtReal bottom_justification;
	DtVolume boundary;
	dot_object_reference *references;
	dot_object *view_group;
	DtInt modification_sequence_number;
} dot_frame;


#define dod_frame_null	(struct frame *)0


extern dot_object *doe_frame_list ;
extern DtInt doe_frame_class_id;
