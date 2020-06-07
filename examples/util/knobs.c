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
 * ??? ??, 1988  GVW  Created
 * May ??, 1988  MEC  Signal driven
 * May ??, 1988  GVW  More Fortran interface stuff
 * Jul 29, 1988  MEC  Fix some reentrancy problems
 * Aug 19, 1988  DRG  remapped function names, added C names, removed
 *	initialization routine, and modified documentation.
 *	All changes being backwards compatable.
 * Nov 22, 1988  DRG  removed shadowing of Dore valuator switch functions
 * Dec 21, 1988  MEC  Split PollKnob() from SigPollKnob()
 * Jan 11, 1989  DRG  made start_knobs() public and added knobs modes and
 *	knob initialization options.
 *
 *
 *
 * KNOBS.C
 * This file contains utilities to handle knob box input.
 * Public functions are:
 *
 * TO BIND KNOBS TO DORE INPUT SLOTS:
 *	C       -> set_knobs()
 *	Fortran -> CALL SETKNB
 *
 * TO START THE KNOBS MECHANISM:
 *	C       -> start_knobs(mode, vtime, vmin)
 *	           int mode, vtime, vmin;
 *	Fortran -> STKNB(MODE, VTIME, VMIN)
 *	           INTEGER*4 MODE, VTIME, VMIN
 *
 * TO ENABLE AND DISABLE KNOBBING  IN SIGNAL MODE:
 *	C       -> enable_knobs() and disable_knobs() 
 *	Fortran -> CALL ENBKNB and DISKNB 
 *
 * TO DISCARD PENDING KNOBS INPUT:
 *	C       -> flush_knobs()
 *	Fortran -> CALL FLSKNB()
 *
 * TO READ AND PROCESS KNOB INPUT SYNCHRONOUSLY:
 *	C       -> poll_knobs()
 *	Fortran	-> CALL PLLKNB
 *
 *
 * The knobs utility code can be used in three different modes:
 * 	1-- non-blocking reads (synchronous)
 * 	2-- blocking reads (synchronous)
 * 	3-- signal driven (asynchronous)
 * 
 * To do blocking reads of the knobs: call start_knobs with the value
 * "KnobBlock" (defined in dore_util.h).  Subsequent calls to poll_knobs will
 * block until there is knob input.  poll_knobs returns with the number
 * of different knobs with values.
 * 
 * To do Non-blocking reads of the knobs : call start_knobs with the value
 * "KnobNonBlock" to start_knobs.  If there is no knob input waiting
 * durring a subsequent call to poll_knobs, the value 0 is returned. 
 * 
 * To do signal driven reads of the knobs: call start_knobs with the value
 * "KnobSig".  When signal are later enabled via enable_knobs (see
 * below) knob events are handled asynchronously.
 * 
 * The second argument to start_knobs is an integer that specifies the
 * ammount of time in tenths of seconds for the knobs to wait until
 * flushing knob input that has not exceeded the minimum value to cause
 * a knob event.  The third argument is an integer that specifies the
 * minimum number of knob ticks to receive before the knobs will cause
 * a knob event.  The values 1 and 2, respectively, are reasonable values.
 * 
 * It is important to realize that signals are the software
 * equivalent of interrupts.  Signal handlers have the same
 * restrictions as interrupt handlers.
 *
 * Signal handlers must be disabled whenever the process is in
 * non-interruptible code.  The most important case of this
 * is non-reentrant library functions.  Almost all KPC
 * library functions are non-reentrant.  This includes the
 * Dore library, Fortran I/O, C Standard I/O, and any
 * function that has static variables or does memory allocation
 * with malloc or sbrk.  Quite a list!
 *
 * This particular signal handler calls Dore library functions.
 * Therefore, EVERY call to a Dore library function in the
 * main process after knobs are enabled must be protected with
 * disable_knobs().  Knobs may be bound to slots at any time
 * ie, even before knobs are enabled, but once knobs are enabled,
 * reentrent code must be protected with disable_knobs().
 *
 * disable_knobs() and enable_knobs() nest.
 *
 * disable_knobs() and enable_knobs() cannot be used within the signal handler
 * itself because of race conditions.  Remember that Unix
 * prevents the signal handler from being re-entered.
 *
 *
 *
 * KNOB DATA FORMAT
 * The knob interface is described in a Hewlett-Packard
 * document not included here.  Each of the eight knobs
 * has two configurable parameters: a minimum tick count and
 * a minimum pause value.  A knob will transmit a three-byte
 * packet if it has moved more than its minimum tick count and
 * it has been quiet for longer than its minimum pause value.
 * This code uses a tick count of 2 and a pause value of 0.
 */

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dore/dore.h>
#include "dore_util.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stropts.h>



#undef	EWOULDBLOCK
#define	EWOULDBLOCK	11
#define KTOR(knob)	( DTOR( knob * ( 360.0 / 256.0 ) ) )
#define	MAX_KNOB	8
#define	param
#define	public



static	int		fdKnob		= -1;
static	int		modeKnob	= KnobVirgin;
static	int		nKnobDisable	= 1;
public	int		nKnobPoll	= 0;
static	DtObject	rgSlot[MAX_KNOB];



public	int		poll_knobs( );
static	void		SigPollKnob( );






/***************************************************************************
 * Bind knob motions to variable values.
 ***************************************************************************/
	public void set_knobs (
    DtObject Slot0,
    DtObject Slot1,
    DtObject Slot2,
    DtObject Slot3,
    DtObject Slot4,
    DtObject Slot5,
    DtObject Slot6,
    DtObject Slot7)
{
	rgSlot[0]	= Slot0;
	rgSlot[1]	= Slot1;
	rgSlot[2]	= Slot2;
	rgSlot[3]	= Slot3;
	rgSlot[4]	= Slot4;
	rgSlot[5]	= Slot5;
	rgSlot[6]	= Slot6;
	rgSlot[7]	= Slot7;
}

/*
 * Fortran binding
 */
	public void SETKNB (
    param DtObject *pSlot0,
    param DtObject *pSlot1,
    param DtObject *pSlot2,
    param DtObject *pSlot3,
    param DtObject *pSlot4,
    param DtObject *pSlot5,
    param DtObject *pSlot6,
    param DtObject *pSlot7)
{
	set_knobs(
		*pSlot0, *pSlot1, *pSlot2, *pSlot3,
		*pSlot4, *pSlot5, *pSlot6, *pSlot7 );
}




/***************************************************************************
 * Disable knob polling signals.
 ***************************************************************************/
	public void disable_knobs (void)
{
	if ( nKnobDisable++ == 0 )
	{
		if ( sighold( SIGPOLL ) == -1 )
		{
			fprintf( stderr,
				"disable_knobs failure: errno %d\n", errno );
			return;
		}
	}
	if ( nKnobDisable % 10 == 0 )
	{
		fprintf( stderr,
			"disable_knobs warning: excess calls to KNBDIS\n" );
	}
}

/*
 * Fortran binding
 */
	public void DISKNB (void)
{
	disable_knobs( );
}

/*
 * for backwards compatibility
 */
	public void KNBDIS (void)
{
	disable_knobs( );
}






/***************************************************************************
 * Enable knob polling signals.
 ***************************************************************************/
	public void enable_knobs (void)
{
	if ( modeKnob == KnobVirgin )
		start_knobs( KnobSig, 1, 2 );

	if ( --nKnobDisable < 0 )
	{
		fprintf( stderr,
			"enable_knobs warning: excess call to enable_knobs\n" );
		nKnobDisable = 0;
	}

	if ( nKnobDisable == 0 )
	{
		if ( sigrelse( SIGPOLL ) == -1 )
			fprintf( stderr,
				"enable_knobs failure; errno %d\n", errno );
	}
}

/*
 * Fortran binding
 */
	public void ENBKNB (void)
{
	enable_knobs( );
}

/*
 * for backwards compatibility
 */
	public void KNBENB (void)
{
	printf( "KNBENB: Obselete interface -- please use ENBKNB\n" );
	enable_knobs( );
}





/***************************************************************************
 * Discard any pending input from the knobs.
 ***************************************************************************/
	public void flush_knobs (void)
{
	auto	unsigned char	dummy	[3];
	errno = 0;
	while ( read( fdKnob, &dummy[0], 1 ) != -1 )
		;
	if ( errno != EWOULDBLOCK )
	{
		fprintf( stderr, "flush_knobs failure: errno %d\n", errno );
	}
}

/*
 * Fortran binding
 */
	public void FLSKNB (void)
{
	flush_knobs();
}



/*
 * Open and initialize the knob channel.
 * The knob has two configurable parameters,
 * which are analagous to VTIME and VMIN.
 *
 * The parameter "mode" can be one of "KnobBlock",
 * "KnobNonBlock", or "KnobSig" for blocking, non-blocking
 * and signal driven reads of the knobs.
 *
 * The knob box will send bytes to the host computer
 * if vtime tenths of seconds have elapsed
 * since the last knob motion, or if a single knob
 * has moved more than vmin ticks.
 *
 * Note: vtime is a parameter for the whole knob box;
 * each individual knob has its own vmin but this code
 * sets them all to vmin.
 */

	public void start_knobs (
    int mode,
    int vtime,
    int vmin)
{
	auto	unsigned char	rgbKnob [3];

	if ( modeKnob != KnobVirgin )
		return;

	modeKnob = mode;

	fdKnob = open( "/dev/tty3", O_RDWR );
	if ( fdKnob < 0 )
		goto LError;

	rgbKnob[0] = 0x90;
	rgbKnob[1] = 0x00;
	rgbKnob[2] = vtime;
	if ( write( fdKnob, &rgbKnob[0], 3 ) != 3 )
		goto LError;

	rgbKnob[1] = 0x00;
	rgbKnob[2] = vmin;
	for ( rgbKnob[0] = 0x80; rgbKnob[0] < 0x88; rgbKnob[0]++ )
	{
		if ( write( fdKnob, &rgbKnob[0], 3 ) != 3 )
			goto LError;
	}

	if ( (KnobSig == mode) || (KnobNonBlock == mode) )
	{
		if ( fcntl( fdKnob, F_SETFL, FNDELAY ) == -1 )
			goto LError;
	}

	if ( (KnobSig == mode) )
	{
		sigset( SIGPOLL, NULL );
		if ( ioctl( fdKnob, I_SETSIG, 1 ) == -1 )
			goto LError;
	}

	return;

 LError:
	fprintf( stderr, "start_knobs failure: errno %d\n", errno );
	return;
}

/*
 * Fortran binding
 */
	public void STKNB (
    int *mode,
    int *vtime,
    int *vmin)
{
	start_knobs(*mode, *vtime, *vmin);
}


/*
 * for backwards compatability
 */
	public void INIKNB (void)
{
	printf( "INIKNB: Obselete interface -- please use STKNB(KnobSig)\n" );
	start_knobs(KnobSig, 1, 2);
}






/*
 * Polling procedure for knob input.
 *
 * Returns number of knobs with non-zero input since last call,
 * or -1 on error.
 */

	public int poll_knobs (void)
{
	auto	int		iKnob;
	auto	int		nKnobNonZero;
	auto	unsigned char	rgbKnob [3];
	auto	DtReal		rgdKnob	[MAX_KNOB];



	for ( iKnob = 0; iKnob < MAX_KNOB; iKnob++ )
		rgdKnob[iKnob] = 0.0;



	/*
	 * Snarf three-byte sequence in rgbKnob.
	 */
	for ( ; ; )
	{
 LRead0:
		errno = 0;
		if ( read( fdKnob, &rgbKnob[0], 1 ) == -1 )
		{
			if ( errno == EWOULDBLOCK )
				goto LReadEnd;
			else
				goto LError;
		}


 LRead1:
		errno = 0;
		if ( read( fdKnob, &rgbKnob[1], 1 ) == -1 )
		{
			if ( errno == EWOULDBLOCK )
				goto LRead1;
			else
				goto LError;
		}

 LRead2:
		errno = 0;
		if ( read( fdKnob, &rgbKnob[2], 1 ) == -1 )
		{
			if ( errno == EWOULDBLOCK )
				goto LRead2;
			else
				goto LError;
		}

#if		defined(DEBUG)
		printf( "Knobs: %2.2X %2.2X %2.2X\n",
			rgbKnob[0], rgbKnob[1], rgbKnob[2] );
#endif

		iKnob		=  rgbKnob[0] - 0x80;
		if ( iKnob >= sizeof(rgdKnob)/sizeof(rgdKnob[0]) )
			return(-1);
		rgdKnob[iKnob]	+= (DtReal)
			( ( (int) (char) rgbKnob[1] ) * 256
			+ (int) rgbKnob[2] );

		if ( modeKnob == KnobBlock )
			break;
	}
 LReadEnd:
	;



	/*
	 * Send new values of knobs off to valuator functions.
	 */
	nKnobNonZero = 0;
	for ( iKnob = 0; iKnob < MAX_KNOB; iKnob++ )
	{
		if ( rgdKnob[iKnob] != 0.0 && rgSlot[iKnob] != NULL )
		{
			nKnobNonZero++;
			DsInputValue( rgSlot[iKnob], KTOR( rgdKnob[iKnob] ) );
		}
	}



	/*
	 * That's all, folks.
	 */
	return nKnobNonZero;



 LError:
	fprintf( stderr, "SigPollKnob failure: errno %d\n", errno );
	return(-1);
}


/*
 * Fortran binding
 */
	public int PLLKNB (void)
{
	return poll_knobs( );
}


