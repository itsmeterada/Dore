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
	dor_doresys_initialize (processors)
	dor_doresys_initialize_drivers ()
	dor_doresys_initialize_renderers ()
	DtInt dor_doresys_titan_count_processors ()
	dor_doresys_terminate ()

  ======================================================================
 */
#include "dore/internal/dogen.h"
#include <sys/sysmips.h>
#include <thread.h>

/*
 ======================================================================
 */

dor_doresys_initialize (processors)
DtInt processors;
{
    DtInt bootprocessor;
    DtInt actualprocessors;
    DtInt dor_doresys_titan_count_processors();
    extern DtInt doe_system_main_processors;

    if (processors < 0 || processors > 4) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_doresys_initialize",
		    "processors number must be between 0 and 4");
	doe_system_main_processors = 0;
	return;
    }

    if (processors > 1) {
	actualprocessors = dor_doresys_titan_count_processors();
	processors = min(processors,actualprocessors);
    } else {
	bootprocessor = sysmips(SBOOT_CPU,0,0,0);
	sysmips(SFORCE_NORUN,bootprocessor,0,0);
    }

    if (processors > 0) {
	_set_thread_number(processors);
    }
    doe_system_main_processors = processors;
}
/*
 ======================================================================
 */

dor_doresys_initialize_drivers ()
{
    ddr_rasterfile_install_driver(DcNullPtr);

    ddr_x11_titang2_install_driver(DcNullPtr);
}
/*
 ======================================================================
 */

dor_doresys_initialize_renderers ()
{
    drr_dynrnd_install_renderer(DcRealTime);

    drr_glbrnd_install_renderer(DcProductionTime);
}
/*
 ======================================================================
 */

DtInt dor_doresys_titan_count_processors()
{
    int processormask;
    int bitno;
    int processorcount;

    processormask = sysmips(PROCPRESENT,0,0,0);

    processorcount = 0;

    for (bitno=0;bitno<32;bitno++) {
	processorcount += ((processormask>>bitno)&1);
    }

    return(processorcount);
}
/*
 ======================================================================
 */

extern _procno;

dor_doresys_titan_setup_threads(int n)
{
//	This is called with an argument that is >1, and represents the
//	number of processors desired
//	This number is assumed to not exceed the number of processors
//	actually available
//	It is assumed that the process has gone parallel, but
//	nothing has been done (e.g., with tigr) that depends on
//	the specific processor

//	The aim is to ensure:
//		All threads are running on separate processors
//		if n < nproc, no thread is on the boot processor
//		if n == nproc, the distinguished thread is not the boot proc
//	The way the routine works is:
//		determine the number of processors and the boot processor
//		determine the association between threads and processors
//		decide what processor the threads SHOULD be on
//		make each thread switch to the target processor, if needed.

//	NOTE: it is tricky to force the OS to dispatch all processors
//	since the standard thread code will redispatch any free processor
//	to continue an iteration.  The trick is to use a semiphore to make
//	sure that no iteration completes without all having been started
//	(e.g., all processors have been dispatched)

//		determine the number of processors and the boot processor

	int nproc = thread( THREAD_NPROC, 0 );
	int bootproc = sysmips( SBOOT_CPU );
	int distinguished;
	threadlocal i;
	volatile semi;
	static curproc[4];
	static newproc[4];

//	Determine the association between threads and processors

	if( n < nproc )
		_set_thread_number( n );

	semi = -n;
	parbegin( n );
	while( (i = parnext()) > 0 ){
	    LOAD_SYNCH( semi );	// bump semiphore
		    THREAD_SYS( curproc[_procno] = sysmips( WHICHPROC); );
	    while( semi ) { };
	}

//	At this point we are thready, and in the distinguished thread

	distinguished = curproc[0];

	for( i=0; i<4; ++i )
		newproc[i] = curproc[i];

	if( n == 1 || n > nproc ){
	    fprintf( stderr,"ERROR: CALL with n=%d, nproc=%d\n", n, nproc );
	}

	if( n == nproc ){
	    /*
	     * we want to run the distinguished thread on
	     * the boot processor (on which the X server also runs)
	     * since the distinguished thread is essentially serial
	     * with the X server
	     */
	    if( distinguished != bootproc ) {
		/* we swap the first and second threads */

		for (i = 1; i < n; i++)
			if (curproc[i] == bootproc)
				break;
		if (i >= n)
			fprintf(stderr, "ERROR: no thread on boot proc\n");
		else {
		    newproc[i] = curproc[0];
		    newproc[0] = curproc[i];
		}
	    }
	}
	else{
	    int i, j, free;

	    //	n < nproc
		    //	is any equal to the boot processor?

			    for( i=0; i<n; ++i ){
				if( newproc[i] != bootproc )
					continue;

				// find another processor

					free = sysmips( PROCPRESENT);

				for( j=0; j<n; ++j )
					free &= ~(1<<newproc[j]);

				for( j=0; j<4; ++j )
					if( free & (1<<j) )
						break;

				if( j >= 4 )
					fprintf( stderr,
						"BIG TROUBLE in thread city %d\n", j );

				// set thread i to j

					newproc[i] = j;
				break;  // no more work to do...
				    }
	}

//	now, set the threads that change processors

	semi = -n;
	parbegin( n );
	while( (i = parnext()) > 0 ){
	    LOAD_SYNCH( semi );	// bump semiphore
		    /*		if( curproc[_procno] != newproc[_procno] ) */
		    THREAD_SYS( sysmips( SFORCE_RUN, newproc[_procno] ) );
	    while( semi ) { };
	}
}
/*
 ======================================================================
 */

dor_doresys_terminate ()
{
}
