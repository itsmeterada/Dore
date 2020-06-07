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
 
      program main
      include '/usr/include/fortran/DORE'

      external errhnd
      integer*4 dev
      character*80 version
      integer*4 ver_len
      data ver_len /80/
      integer*4 deferr
      integer*4 deffd

C***********************************************************

      call dsinit (0)

      call dsqver(version, ver_len)
      print *, 'Version ', version

      call dsqev (deffd, deferr)

      print *, 'Error from DoDevice - using standard error handler'

      dev = dod('junk', 4, ' ', 1)

      print *, 'Installing Fortran error handler'

      call dssev(6, errhnd)

      print *, 'Error from DoDevice - using Fortran Error handler'

      dev = dod('junk', 4, ' ', 1)

      print *, 'Resetting Error handler to default'

      call dssev(deffd, DCNULL)

      print *, 'Error from DdUpdate'

      call ddu(dev)

      call dsterm

      end

C**********************************************************************
C
C
      SUBROUTINE ERRHND(ERRFIL, ERRNUM, FCNNAM, FCNNML,
     x ERRSTR, ERRSTL)

      INCLUDE '/usr/include/fortran/DORE'

      INTEGER*4 ERRFIL
      INTEGER*4 ERRNUM
      INTEGER*4 FCNNML
      INTEGER*4 ERRSTL
      CHARACTER*(*) FCNNAM
      CHARACTER*(*) ERRSTR
C
      INTEGER*4 INERR
      DATA INERR /0/
      COMMON /INERR/ INERR
C
      INTEGER*4 BUFSIZ
      PARAMETER (BUFSIZ = 1024)
      CHARACTER*1024 BUF
      INTEGER*4 STATUS
      INTEGER*4 SEVERT
      CHARACTER*8 SEVSTR
      INTEGER*4 SLEN
      INTEGER*4 TLEN
C
 10   FORMAT (' Error #',I4,' - in routine ', A)
 20   FORMAT (' Error #',I4)
 30   FORMAT (' Error message truncated')
 40   FORMAT (' Error message #',I4,' does not exist')
 50   FORMAT (' ***** ',A,' ERROR ***** (#',I4,
     x     ') in routine ',A,' - ')
 60   FORMAT (' ***** ',A,' ERROR ***** (#',I4,')')
 70   FORMAT ('       ',A,' - ',A)
 80   FORMAT ('       ',A)
C
      IF (INERR .EQ. 1) THEN
C
C Error handler has recursed, just output simple message
C
         IF (FCNNML .GT. 0) THEN
            WRITE(ERRFIL, 10) ERRNUM, FCNNAM
         ELSE
            WRITE(ERRFIL, 20) ERRNUM
         ENDIF

         INERR = 0
         RETURN
      ENDIF

      INERR = 1

      SEVERT = -1
      STATUS = DSQEM(ERRNUM, BUFSIZ, BUF, SEVERT)

      IF (STATUS .EQ. DCRTRN) THEN
         WRITE(ERRFIL, 30)
      ENDIF

      IF (STATUS .EQ. DCRFAL) THEN
         WRITE(ERRFIL, 40)
         INERR = 0
         RETURN
      ENDIF

      DO 100 SLEN = BUFSIZ - 1, 1, -1
         IF (BUF(SLEN:SLEN+1) .NE. ' ') GOTO 101
 100  CONTINUE 
 101  CONTINUE

      IF (SEVERT .EQ. DCEMNR) THEN
         SEVSTR = 'MINOR'
         TLEN = 5
      ELSE IF (SEVERT .EQ. DCESEV) THEN
         SEVSTR = 'SEVERE'
         TLEN = 6
      ELSE IF (SEVERT .EQ. DCEFAT) THEN
         SEVSTR = 'FATAL'
         TLEN = 5
      ELSE
         SEVSTR = 'UNKNOWN'
         TLEN = 7
      ENDIF

      IF (FCNNML .GT. 0) THEN
         WRITE(ERRFIL,50) SEVSTR(1:TLEN), ERRNUM, FCNNAM
      ELSE
         WRITE(ERRFIL,60) SEVSTR(1:TLEN), ERRNUM
      ENDIF
      
      IF (ERRSTL .GT. 0) THEN
         WRITE(ERRFIL, 70) BUF(1:SLEN), ERRSTR
      ELSE
         WRITE(ERRFIL, 80) BUF(1:SLEN)
      ENDIF

      INERR = 0

      IF (SEVERT .EQ. DCEFAT) THEN
C
C Fatal error so exit
C
         CALL EXIT(1)
      ENDIF

      RETURN
      END

