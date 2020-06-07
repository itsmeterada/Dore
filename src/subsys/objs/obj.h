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

/* mask for object hold field (1 bit) */
#define  dod_object_hold_mask	0x8000

/* mask for object type field (15 bits) */
#define  dod_object_type_mask	0x7fff

/* number of object headers in object block */
#define  dod_object_block_size	1023

/* first method not defined by object manager 
	object manager defines:

		0	-	DcMethodDestroy
		1	-	DcMethodAddReference
		2	-	DcMethodRemoveReference
		3	-	DcMethodPrint
		4	-	DcMethodCheckGroup
		5	-	DcMethodCmpBndVolume
		6	-	DcMethodIniPick
		7	-	DcMethodPick
		8	-	DcMethodInqGlbAttVal
		9	-	DcMethodUpdStdAltObj
		10	-	DcMethodStdRenderStudio
		11	-	DcMethodStdRenderDisplay

		12	-	DcMethodDynIniRender
		13	-	DcMethodDynRender
		14	-	DcMethodGlbrndIniRender
		15	-	DcMethodGlbrndIniObjs
		16	-	DcMethodGlbrndSpcboxOvr
		17	-	DcMethodGlbrndRayint
		18	-	DcMethodGlbrndUsrdat
		19	-	DcMethodGlbrndWcsloc
		20	-	DcMethodGlbrndWcsnrm
		21	-	DcMethodGlbrndWldbnd
 
		22      -       DcMethodUpdStdAltLineObj
*/
#define dod_object_first_defined_method	23

#define dod_InqGlbAttValName		"InqGlobalValue"
#define dod_UpdStdAltObjName		"UpdStdAlternateObj"
#define dod_UpdStdAltLineObjName	"UpdStdAlternateLineObj"
#define dod_StdRenderStudioName		"StdRenderStudio"
#define dod_StdRenderDisplayName	"StdRenderDisplay"


typedef struct object_type {
    struct object *class_object;
    DtMethodPtr    default_method;
    DtMethodPtr   *methods; 
} dot_object_type;

typedef struct object_block {
    struct object_block *previous;
    struct object_block *next;
    dot_object objects[dod_object_block_size];
} dot_object_block;

typedef struct {
   DtInt type;
   DtInt count;
   struct list *additional_data_info;
} dot_class;

typedef struct {
   DtInt       offset;
   DtMethodPtr crt_rtn;
   DtMethodPtr prt_rtn;
   DtMethodPtr del_rtn;
} dot_additional_data;


/* doubly linked chain of object header blocks */
extern dot_object_block *doe_object_blocks;

/* linked list of free object slots */
extern dot_object *doe_object_free_list;

/* linked list of deleted, but not yet reclaimed objects */
extern dot_object *doe_object_deleted_list;

/* next available method number */
extern DtInt doe_object_next_method;

/* next available object type number */
extern Dt32Bits doe_object_next_type;

/* object dictionary */
extern struct dictionary *doe_object_dictionary;

/* Dore dispatch table */
extern dot_object_type *doe_object_types;

/* list of methods derived from other ones */
extern Dt32Bits  *doe_object_derived_method;
extern char     **doe_object_method_name;

/* list of functions to call when new class added */
extern DtPFV   *doe_object_notify;
extern Dt32Bits doe_object_notify_count;
extern Dt32Bits doe_object_notify_max;

/* variables used for inq first, inq next */
extern dot_object_block * dor_object_scan_block;
extern Dt32Bits doe_object_scan_item;
extern Dt32Bits doe_object_scan_type;
