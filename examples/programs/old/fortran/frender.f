Cident "%W%" %G%
 
C**************************************************************************
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
C**************************************************************************
 
C
      PROGRAM MAIN
C
C
      IMPLICIT NONE
      INCLUDE '/usr/include/fortran/DORE'
      INCLUDE '/usr/include/fortran/DOREMETHODS'
      INCLUDE '/usr/include/fortran/DORETEXT'
      INCLUDE '/usr/include/fortran/DORETYPES'
C
      INTEGER*8 DEVICE, FRAME, VIEW
      INTEGER*8 POST, BASE, DEF_GROUP, OBJ_GROUP
#ifdef DORE_REAL_DOUBLE
      REAL*8 ORIGIN(3)
      REAL*8 EYE_POINT(3)
      REAL*8 LIGHT(3)
      REAL*8 UP(3)
      REAL*8 MAGENTA(3)
      REAL*8 YELLOW(3)
      REAL*8 SKY_BLUE(3)
      REAL*8 SDS(1)
#else
      REAL*4 ORIGIN(3)
      REAL*4 EYE_POINT(3)
      REAL*4 LIGHT(3)
      REAL*4 UP(3)
      REAL*4 MAGENTA(3)
      REAL*4 YELLOW(3)
      REAL*4 SKY_BLUE(3)
      REAL*4 SDS(1)
#endif
      CHARACTER DUMMY
C
C
#ifdef DORE_REAL_DOUBLE
      ORIGIN(1) = 0.0D0
      ORIGIN(2) = 0.0D0
      ORIGIN(3) = 0.0D0
C
      EYE_POINT(1) =  0.0D0
      EYE_POINT(2) =  0.0D0
      EYE_POINT(3) = 10.0D0
C
      LIGHT(1) =  1.0D0
      LIGHT(2) =  1.0D0
      LIGHT(3) =  0.5D0
C
      UP(1) = 0.0D0
      UP(2) = 1.0D0
      UP(3) = 0.0D0
C
      MAGENTA(1) = 1.0D0
      MAGENTA(2) = 0.0D0
      MAGENTA(3) = 1.0D0
C
      YELLOW(1) = 0.8D0
      YELLOW(2) = 0.8D0
      YELLOW(3) = 0.0D0
C
      SKY_BLUE(1) = 0.3D0
      SKY_BLUE(2) = 0.3D0
      SKY_BLUE(3) = 1.0D0
C
      SDS(1) = 0.8D0
#else
      ORIGIN(1) = 0.0E0
      ORIGIN(2) = 0.0E0
      ORIGIN(3) = 0.0E0
C
      EYE_POINT(1) =  0.0E0
      EYE_POINT(2) =  0.0E0
      EYE_POINT(3) = 10.0E0
C
      LIGHT(1) =  1.0E0
      LIGHT(2) =  1.0E0
      LIGHT(3) =  0.5E0
C
      UP(1) = 0.0E0
      UP(2) = 1.0E0
      UP(3) = 0.0E0
C
      MAGENTA(1) = 1.0E0
      MAGENTA(2) = 0.0E0
      MAGENTA(3) = 1.0E0
C
      YELLOW(1) = 0.8E0
      YELLOW(2) = 0.8E0
      YELLOW(3) = 0.0E0
C
      SKY_BLUE(1) = 0.3E0
      SKY_BLUE(2) = 0.3E0
      SKY_BLUE(3) = 1.0E0
C
      SDS(1) = 0.8E0
#endif
C
C
      CALL DSINIT (0)
      DEVICE = DOD ('display', 9, '-singlebuffered', 15)
      FRAME = DOFR ()
      CALL DDSF (DEVICE, FRAME)
      VIEW = DOVW ()
      CALL DVSBC (VIEW, DCRGB, SKY_BLUE)
      CALL DGAOG (DFQVG(FRAME), VIEW)
C
      DEF_GROUP = DOG(DCTRUE)
#ifdef DORE_REAL_DOUBLE
              CALL DGAO (DOPAR (10.0D0, -1.0D0, -20.0D0))
#else
              CALL DGAO (DOPAR (10.0E0, -1.0E0, -20.0E0))
#endif
              CALL DGAO (DOPUMX ())
                      CALL DGAO (DOLAF (ORIGIN, EYE_POINT, UP))
                      CALL DGAO (DOCM ())
              CALL DGAO (DOPPMX ())
              CALL DGAO (DOPUMX ())
                      CALL DGAO (DOLAF (ORIGIN, LIGHT, UP))
#ifdef DORE_REAL_DOUBLE
                      CALL DGAO (DOLI (1.0D0))
#else
                      CALL DGAO (DOLI (1.0E0))
#endif
                      CALL DGAO (DOLT ())
              CALL DGAO (DOPPMX ())
      CALL DGCS ()
C
      POST = DOG(DCTRUE)
              CALL DGAO (DODIFC (DCRGB, YELLOW))
              CALL DGAO (DOSRFS (DCSHCN))
              CALL DGAO (DOREPT (DCWIRE))
#ifdef DORE_REAL_DOUBLE
              CALL DGAO (DOROT (DCXAX, 1.5707963D0))
              CALL DGAO (DOSC (0.4D0, 0.4D0, 2.1D0))
#else
              CALL DGAO (DOROT (DCXAX, 1.5707963E0))
              CALL DGAO (DOSC (0.4E0, 0.4E0, 2.1E0))
#endif
              CALL DGAO (DOPMS (DCCYL))
      CALL DGCS ()
C
      BASE = DOG(DCTRUE)
#ifdef DORE_REAL_DOUBLE
              CALL DGAO (DOXLT (0.0D0, -3.0D0, 0.0D0))
              CALL DGAO (DOSC (2.0D0, 2.0D0, 2.0D0))
              CALL DGAO (DOXLT (-0.5D0, -0.5D0, -0.5D0))
#else
              CALL DGAO (DOXLT (0.0E0, -3.0E0, 0.0E0))
              CALL DGAO (DOSC (2.0E0, 2.0E0, 2.0E0))
              CALL DGAO (DOXLT (-0.5E0, -0.5E0, -0.5E0))
#endif
              CALL DGAO (DOPMS (DCBOX))
      CALL DGCS ()
C
      OBJ_GROUP = DOG(DCTRUE)
              CALL DGAO (DOREPT (DCSURF))
              CALL DGAO (DOSDS (DCSDRL, SDS))
              CALL DGAO (DODIFC (DCRGB, MAGENTA))
	      CALL DGAO (post)
	      CALL DGAO (base)
              CALL DGAO (DOPMS (DCSPHR))
      CALL DGCS ()
C
C
      CALL DGAOG (DVQDG(VIEW), DEF_GROUP)
      CALL DGAOG (DVQIG(VIEW), OBJ_GROUP)
      CALL DDU (DEVICE)
      WRITE (6, '(''Hit return to exit.'')')
      READ (5, '(A)') DUMMY
      CALL DSRO (DEVICE)
      CALL DSTERM
C
      END
