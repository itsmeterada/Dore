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
 
/*
  ======================================================================
  Functions:
	void drr_dynrnd_clpswi ()
	void drr_dynrnd_clpvol (object)
	void drr_dynrnd_global_clpvol (exe_type, pop_cnt)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/clpvol.h>
#include "dynrnd.h"
/*
 ======================================================================
 */
void drr_dynrnd_clpswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttClipSwitch"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_clpswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_clpvol (
    dot_object *object)
{
    struct clpvol_data *clpvoldat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoClipVol");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    clpvoldat = (struct clpvol_data *)object->data;

    dor_global_clpvol_notify_change();

    (*dre_dynrnd_DROM_fcns->apply_att_clpvol) (clpvoldat->operator,
					       clpvoldat->halfspacecount,
					       clpvoldat->halfspaces);
}
/*
 ======================================================================
 */
void drr_dynrnd_global_clpvol (
    DtGlobalCall exe_type,
    DtInt pop_cnt)
{
    DtInt i;

    switch (exe_type) {
	case DcGlobalCallPush:
	    (*dre_dynrnd_DROM_fcns->push_att_clpvol) ();
	    break;

	case DcGlobalCallPop:
	    for (i=0; i<pop_cnt; i++) {
		(*dre_dynrnd_DROM_fcns->pop_att_clpvol) ();
	    }
	    break;
	
	default:
	    break;
    }
}
/*
 ======================================================================
 */
