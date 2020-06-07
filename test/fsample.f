#ident "%W%" %G%
 
C /**************************************************************************
C # Copyright (C) 1994 Kubota Graphics Corp.
C # 
C # Permission to use, copy, modify, and distribute this material for
C # any purpose and without fee is hereby granted, provided that the
C # above copyright notice and this permission notice appear in all
C # copies, and that the name of Kubota Graphics not be used in
C # advertising or publicity pertaining to this material.  Kubota
C # Graphics Corporation MAKES NO REPRESENTATIONS ABOUT THE ACCURACY
C # OR SUITABILITY OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED
C # "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING THE
C # IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
C # PURPOSE AND KUBOTA GRAPHICS CORPORATION DISCLAIMS ALL WARRANTIES,
C # EXPRESS OR IMPLIED.
C **************************************************************************/
 
C ===========================================================
C This file contains the source code for the sample program 
C from Chapter 2 of the Dore Programmer's Guide.  The 
C generated image contains a blue cylinder, a magenta box, 
C and a wireframe magenta sphere.
C
C Note : In Fortran, the include file paths have to be hard-
C coded (here we have /usr/include/fortran/DORE). Therefore,
C Dore has to be installed before this program is compiled.
C ===========================================================
C
      PROGRAM MAIN
C
      IMPLICIT NONE

      INCLUDE '/usr/include/fortran/DORE'

C
      INTEGER*4 DEVICE, FRAME, VIEW
      INTEGER*4 POST, BASE, DEFGRP, OBJGRP
      REAL*4 ORIGIN(3)
      REAL*4 EYEPT(3)
      REAL*4 LIGHT(3)
      REAL*4 UP(3)
      REAL*4 MAGENT(3)
      REAL*4 GREY(3)
      REAL*4 SKYBLU(3)
      REAL*4 SDS(1)
      CHARACTER DUMMY
C
      DATA ORIGIN /  0.0E0,  0.0E0,  0.0E0 /
      DATA EYEPT  / 10.0E0, 10.0E0, 10.0E0 /
      DATA LIGHT  /  1.0E0,  1.0E0,  0.5E0 /
      DATA UP     /  0.0E0,  1.0E0,  0.0E0 /
      DATA MAGENT /  1.0E0,  0.0E0,  1.0E0 /
      DATA GREY   /  0.4E0,  0.4E0,  0.4E0 /
      DATA SKYBLU /  0.3E0,  0.3E0,  1.0E0 /
      DATA SDS    / 28.0E0 /
C
      CALL DSINIT (0)  ! initialize Dore !
C
      ! create device
#if defined(sun) && !defined(NO_SUNVIEW)
      DEVICE = DOD('sunview',7,
     1             '-visualtype DcPseudoColor -zbuffer',34)
#else
#ifdef sun
      DEVICE = DOD('stdx11',6,
     1             '-visualtype DcPseudoColor -zbuffer',34)
#else
#ifdef sgi
      DEVICE = DOD('gl',2,
     1             '-visualtype DcTrueColor',23)
#else
#if defined (titan) && defined (__P3__)
      DEVICE = DOD('stdx11',6,
     1             '-zbuffer -visualtype DcTrueColor',32)
#else
      DEVICE = DOD('stdx11',6,
     1             '-visualtype DcPseudoColor -zbuffer',34)
#endif
#endif
#endif
#endif

C
      ! create frame and add to device
      FRAME = DOFR()                 
      CALL DDSF (DEVICE,FRAME)      
C
      ! create view and add to frame
      VIEW = DOVW()                      
      CALL DVSBC (VIEW, DCRGB, GREY)  
      CALL DGAOG(DFQVG(FRAME), VIEW)   
C	
      ! use degrees for all angles
      CALL DSSAU(DCAD)		
C
      ! create a group for parallel camera and light
      DEFGRP = DOG(DCTRUE)  	
           CALL DGAO(DOPAR(10.0E0, -0.1E0, -20.0E0))  
           CALL DGAO(DOPUMX())
                CALL DGAO (DOLAF(ORIGIN, EYEPT, UP)) 
                CALL DGAO (DOCM())       
           CALL DGAO(DOPPMX())
           CALL DGAO(DOPUMX())
                CALL DGAO(DOLAF(ORIGIN, LIGHT, UP))  
                CALL DGAO(DOLI(1.0E0))   
                CALL DGAO(DOLT())       
           CALL DGAO(DOPPMX())
      CALL DGCS()
C
      ! define POST group
      POST = DOG(DCTRUE)
           CALL DGAO(DODIFC(DCRGB, SKYBLU))  
           CALL DGAO(DOXLT(0.0E0, 1.5E0, 0.0E0)) 
           CALL DGAO(DOROT(DCXAX, 90.0E0))  
           CALL DGAO(DOSC(1.2E0, 1.2E0, 2.4E0))  
           CALL DGAO(DOPMS(DCCYL))       
      CALL DGCS()
C
      ! define BASE group
      BASE = DOG(DCTRUE)
           CALL DGAO(DOXLT(0.0E0, -2.2E0, 0.0E0))  
           CALL DGAO(DOSC(2.5E0, 2.5E0, 2.5E0))  
           CALL DGAO(DOXLT(-0.5E0, -0.5E0, -0.5E0)) 
           CALL DGAO(DOPMS(DCBOX))       
      CALL DGCS()
C
      ! OBJGRP includes POST, BASE, and sphere object
      OBJGRP = DOG(DCTRUE)
           CALL DGAO(DOREPT(DCSURF))
           CALL DGAO(DOSDS(DCSDSG, SDS))
           CALL DGAO(DODIFC(DCRGB, MAGENT)) 
           CALL DGAO(POST)               
           CALL DGAO(BASE)              
           CALL DGAO(DOXLT(0.0E0, 3.2E0, 0.0E0)) 
           CALL DGAO(DOSC(1.5E0, 1.5E0, 1.5E0))  
           CALL DGAO(DOREPT(DCWIRE))    
           CALL DGAO(DOPMS(DCSPHR))     
      CALL DGCS()
C
      ! add objects to view
      CALL DGAOG(DVQDG(VIEW), DEFGRP)    
      CALL DGAOG(DVQIG(VIEW),OBJGRP)    
C
      ! render the image
      CALL DDU(DEVICE)                   
      WRITE(6,'(''Hit return to continue.'')')
      READ(5, '(A)') DUMMY
C
      ! clean up
      CALL DSRO(DEVICE)                  
C
      ! shut down
      CALL DSTERM                       
C
      END
