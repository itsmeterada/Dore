/***************************************************************************
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
***************************************************************************/

/*****************************************************************************
**  This file contains macros and definitions for the FORTRAN / C interface.
*****************************************************************************/

#include <dore/internal/dogen.h>

    /***************************/
    /***  Macro Definitions  ***/
    /***************************/

	/* This macro expands out to the pointer value as long as it's not
	** the address of the fake DCNULL array.  If the program did use the
	** DCNULL address, then it evaluates to the nil pointer value.  */

#define FORTRAN_PTR(x)	(((void*)(x) == (void*)dcnull_) ? DcNullPtr : (x))


    /**************************/
    /***  Global Variables  ***/
    /**************************/

#ifdef GLOBALS
#   define EXTERN   /* Nothing */
#else
#   define EXTERN   extern
#endif

EXTERN long dcmxs_ [3];
EXTERN long dcltps_[6];
EXTERN long dcshs_ [2];
EXTERN long dcpks_ [3];
EXTERN long dciss_ [11];
EXTERN long dcsflt_[1];
EXTERN long dcsmap_[6];
EXTERN long dcsuvc_[1];
EXTERN long dcswc_ [1];
EXTERN long dcdelc_[1];
EXTERN long dctype_[169];
EXTERN long dcnull_[1];


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

int fstr (char**, int, char*, int);
