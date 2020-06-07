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
**  This file contains routines for rendering annotation text.  Annotation
**  text is text that is perpendicular to the line of sight.
**
**  NOTE:  This file is currently unused in order to use Dore's alternate
**         methods.  This buys us the capability of using Dore's fonts and
**         other textual attributes consistently.
*****************************************************************************/

#include "drom.h"
#include <dore/dore_develop/develop.h>
#include <dore/dore_develop/private/anntxt.h>

#define DEBUG 0



    /***  Data Structures  ***/

typedef struct
{   pxlStructure  pex_struct;	/* PEX Annotation Text Structure */
    DtFlag        ready;	/* True:  PEX Structure ready for Execution */
} AnnoData;

static pxlStructure BuildAnnoText  ARGS((struct anntxt *));



/*****************************************************************************
**  The annotext_create routine allocates memory for the annotation text data
**  structure on object creation.  This is also where preliminary
**  initialization takes place.
*****************************************************************************/

DtPtr ddr_pex_drom_annotext_create (object)
    DtObjectStructure *object;
{
    register AnnoData  *annodata;	/* Annotation Text Data */

#   if DEBUG_CALL
	printf ("* ddr_pex_drom_annotext_create (object %p)\n", object);
#   endif

    annodata = DDspace_Allocate (sizeof(*annodata));
    annodata->ready = DcFalse;

    return annodata;
}



/*****************************************************************************
**  The annodata_print routine prints the contents of the annotation text data
**  structure to the standard output stream.
*****************************************************************************/

void ddr_pex_drom_annotext_print (object, annodata_parm)
    DtObjectStructure *object;
    DtPtr              annodata_parm;
{
    /* No-op */
}



/*****************************************************************************
**  The annotext_delete routine deletes PEX structures and memory allocated
**  for a given annotation text object.
*****************************************************************************/

void ddr_pex_drom_annotext_delete (object, annodata_parm)
    DtObjectStructure *object;
    DtPtr              annodata_parm;
{
    auto AnnoData *annodata = annodata_parm;

#   if DEBUG_CALL
	print  ("* ddr_pex_drom_annotext_delete (\n");
	printf ("*     object %p,\n", object);
	printf ("*     annodata_parm %p)\n", annodata_parm);
#   endif

    if (!annodata) return;

    if (annodata->ready)
        PEXDestroyStructures (DCM->display, &(annodata->pex_struct), 1);

    DDspace_Deallocate (annodata);
}



/*****************************************************************************
**  This routine is called to render annotation text.  If this routine is
**  called for the first time, then we also need to bundle up and initialize
**  everything for PEX.
*****************************************************************************/

void ddr_pex_drom_annotext_render (object)
    dot_object *object;
{
    static DtInt     classid = -1;	/* Annotation Text Class ID */
    static DtInt     dataindex = -1;	/* Annotation Text Add'l Data Field */
    auto   AnnoData *annodata;		/* Annotation Text Additional Data */

#   if DEBUG_CALL
	printf ("* ddr_pex_drom_annotext_render (object %p)\n", object);
#   endif

    /* Get the class ID and additional-data index. */

    if (!GetClassInfo (&classid, "DoAnnoText", &dataindex))
	return;

    /* Get the additional_data field of the annotation text structure. */

    annodata = (AnnoData*) object->additional_data [dataindex];

    /* If the annotation text is not yet ready for PEX rendering, then we need
    // to build the PEX structure for execution. */

    if (!annodata->ready)
    {   auto struct anntxt *annotext = (struct anntxt *) object->data;

	annodata->pex_struct = BuildAnnoText (annotext);
	annodata->ready = DcTrue;
    }

    /* Now execute the annotation text PEX structure. */

    PEXExecuteStructure (DROM.ocbuffer, annodata->pex_struct);
}



/*****************************************************************************
**  This routine builds a PEX annotation text structure.  It returns the PEX
**  structure ID of the resulting PEX structure.
*****************************************************************************/

static pxlStructure  BuildAnnoText  (annotext)
    struct anntxt *annotext;	/* Dore' Annotation Text Structure */
{
    register int          ii;		/* Loop Index */
    auto     pxlOCBuf    *ocbuffer;	/* PEX Output Command Buffer */
    auto     pxlCoord3D  *pex_coords;	/* PEX Coordinate Array */
    auto     pxlStructure pex_struct;	/* PEX Structure */

#   if DEBUG_CALL
	printf ("* BuildAnnoText (annotext %p)\n", annotext);
#   endif

    /* Create the PEX structure. */

    pex_struct = PEXCreateStructure (DCM->display);

    /* Create the temporary PEX structure-create output-command buffer. */

    ocbuffer = PEXAllocateTransientOCBuffer (DCM->display, pxlAddToStructure,
					     pex_struct, PEXDefaultOCError, 0);

    PEXSetATextAlignment
    (   ocbuffer,
	(annotext->halign == DcTextHAlignLeft)   ? PEXHalignLeft   :
	(annotext->halign == DcTextHAlignCenter) ? PEXHalignCenter :
	(annotext->halign == DcTextHAlignRight)  ? PEXHalignRight  :
						   PEXHalignNormal ,
	(annotext->valign == DcTextVAlignTop)    ? PEXValignTop    :
	(annotext->valign == DcTextVAlignCap)    ? PEXValignCap    :
	(annotext->valign == DcTextVAlignHalf)   ? PEXValignHalf   :
	(annotext->valign == DcTextVAlignBase)   ? PEXValignBase   :
	(annotext->valign == DcTextVAlignBottom) ? PEXValignBottom :
						   PEXValignNormal
    );

    PEXSetATextPath
    (   ocbuffer,
	(annotext->textpath == DcTextPathLeft) ? PEXPathLeft :
	(annotext->textpath == DcTextPathUp)   ? PEXPathUp   :
	(annotext->textpath == DcTextPathDown) ? PEXPathDown :
	                                         PEXPathRight
    );

    PEXSetTextPrecision
    (   ocbuffer,
	(annotext->precision == DcStringPrecision)    ? PEXStringPrecision :
	(annotext->precision == DcCharacterPrecision) ? PEXCharPrecision   :
							PEXStrokePrecision
    );

    PEXSetCharExpansion (ocbuffer, (float)(annotext->textexp));
    PEXSetATextHeight   (ocbuffer, (float)(annotext->textheight / 10.0));
    PEXSetCharSpacing   (ocbuffer, (float)(annotext->textspace));
    PEXSetATextUpVector (ocbuffer, (float)(annotext->xup),
				   (float)(annotext->yup));

    {   auto pxlCoord3D origin, offset;

	origin.x = annotext->position[0];
	origin.y = annotext->position[1];
	origin.z = annotext->position[2];

	offset.x = offset.y = offset.z = 0;
	
	PEXAnnotationText (ocbuffer, &origin, &offset, (char*)annotext->string,
			   strlen((char*)annotext->string));
    }

    PEXFlushOCBuffer (ocbuffer);
    PEXDeallocateOCBuffer (ocbuffer);

    /* Return the PEX structure ID of the annotation text. */

    return pex_struct;
}
