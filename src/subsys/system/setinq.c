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
	int dor_system_set_edit_source (source)
	int dor_system_set_execution_depth (limit)
	int dor_system_set_valuator_switch (switchvalue)
	DtInt dor_system_inq_current_method ()
	DtInt dor_system_inq_edit_source (source)
	DtSwitch dor_system_inq_execution_depth ()
	DtInt dor_system_inq_number_processors ()
	int dor_system_inq_version (version)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include "system.h"

/*
 ======================================================================
 */

void dor_system_set_edit_source (
    DtSwitch source)
{
    doe_system_edit_source = source;
}
/*
 ======================================================================
 */

void dor_system_set_execution_depth (
    DtInt limit)
{

#ifdef ERRCHK
    if(limit <= 0){
	sprintf(dor_error_get_line(), "%d", limit);
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		 "DsSetExeDepthLimit", dor_error_get_line());
    }
#endif

    doe_system_execution_depth = limit ;
}
/*
 ======================================================================
 */

/*#include <signal.h>*/

void dor_system_set_valuator_switch (
    DtSwitch switchvalue)
{
    /*	int oldmask; */

    if(DcOff == doe_system_valuator_switch)
	    if(DcOn == switchvalue){
		/* oldmask = sigblock(1<<SIGALRM); */

		dor_slot_flush_input(); /* process pending inputs */

		/* sigsetmask(oldmask); */
	    }

    doe_system_valuator_switch = switchvalue;
}
/*
 ======================================================================
 */

DtInt dor_system_inq_current_method (void)
{
    return (doe_system_current_method);
}
/*
 ======================================================================
 */

DtInt dor_system_inq_edit_source (
    DtSwitch source)
{
    return(doe_system_edit_source);
}
/*
 ======================================================================
 */

DtSwitch dor_system_inq_execution_depth (void)
{
    return doe_system_execution_depth ;
}
/*
 ======================================================================
 */

DtInt dor_system_inq_number_processors (void)
{
    return (doe_system_main_processors);
}
/*
 ======================================================================
 */

void dor_system_inq_version (
    DtPtr *version)
{
    *version = doe_system_version_string;
}
