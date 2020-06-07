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
 
#ifdef DORE_REAL_SINGLE
#define DORE_REAL real*4
#define DORE_RCONST(val) val
#else
#define DORE_REAL real*8
#define DORE_RCONST(val) val/**/d0
#endif
      program main
Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 object1, object2, object3, object4
      integer*4 camera_group, display_group
      integer*4 undisplay_grp1, undisplay_grp2
      integer*4 grp, grp1, grp2, grp3, grp4

      DORE_REAL origin(3), camera(3), up(3), light(3)
      
      data origin /   DORE_RCONST(0.0), DORE_RCONST(0.0),  DORE_RCONST(0.0)/
      data camera /   DORE_RCONST(0.0), DORE_RCONST(0.0),  DORE_RCONST(10.0)/
      data up     /   DORE_RCONST(0.0), DORE_RCONST(1.0),  DORE_RCONST(0.0)/
      data light  / DORE_RCONST(-10.0), DORE_RCONST(10.0), DORE_RCONST(10.0)/

      DORE_REAL green(3), red(3)

      data green  / DORE_RCONST(0.0), DORE_RCONST(1.0), DORE_RCONST(0.0)/
      data red    / DORE_RCONST(1.0), DORE_RCONST(0.0), DORE_RCONST(0.0)/

      character*80 version
      integer*4 ver_len
      data ver_len /80/

      character*80 message
      integer*4 rtnval

C ======================================================================

      call system ('cat README')

      call dsinit(0)

      call dsqver(version, ver_len)
 100  format (1x, 'Version ', a)
      write (6, 100) version

      if (dgqo() .ne. dcnull) then
         call ExitWithMessage('incorrect open group', 1)
      endif

C create a typical definition group checking element pointer

      camera_group = dog(dctrue)
      if (camera_group .eq. dcnull) then
         call ExitWithMessage('cannot create group', 2)
      endif

      if (dgqo() .ne. camera_group) then
         call ExitWithMessage('incorrect open group', 3)
      endif

C add light objects

      call dgao (dopumx())
         call dgao (doli (DORE_RCONST(1.0)))
         call dgao (dolaf (origin, light, up))
         call dgao (dolt())
      call dgao (doppmx())

C add camera objects

      call dgao (dopar (DORE_RCONST(10.0), DORE_RCONST(0.0), DORE_RCONST(-100.0)))
      call dgao (dolaf (origin, camera, up))
      call dgao (docm())

C check the current position of ptr

      if (dgqep() .ne. 8) then
         call BadElePtrMessage(4)
      endif

C set pointer and check the position

      call dgsep(0, dcbeg)
      if (dgqep() .ne. 0) then
         call BadElePtrMessage(5)
      endif

      call dgsep(0, dcend)
      if (dgqep() .ne. 8) then
         call BadElePtrMessage(6)
      endif

      call dgsep(2,dcbeg)
      if (dgqep() .ne. 2) then
         call BadElePtrMessage(7)
      endif

      call dgsep(-2,dccur)
      if (dgqep() .ne. 0) then
         call BadElePtrMessage(8)
      endif

      if (camera_group .ne. dgcs()) then
         call ExitWithMessage(
     x        'incorrect return value of group_close', 9)
      endif

      if (dcnull .ne. dgcs()) then
         call ExitWithMessage(
     x        'incorrect return value of group_close', 10)
      endif

C check the size of closed group

      if (dgqs(camera_group) .ne. 8) then
         call ExitWithMessage('Incorrect group size', 11)
      endif

      if (dgck(camera_group) .ne. dcgok) then
         call ExitWithMessage(
     x        'wrong value from dgck (DgCheck)', 12)
      endif

C create another (display) group

      display_group = dog(dctrue)

      if (display_group .eq. dcnull) then
         call ExitWithMessage('cannot create group', 13)
      endif

      call dgao (dopumx())
         call dgao (doll(0))
         call dgao (DOSC(DORE_RCONST(0.5), DORE_RCONST(0.5), DORE_RCONST(0.5)))
         call dgao (DODIFC(DCRGB, green))
         call dgao (DOPMS(DCSPHR))
      call dgao (DOPPMX())
      object1 = DOXLT(DORE_RCONST(1.0), DORE_RCONST(0.0), DORE_RCONST(0.0))
      call dgao (object1)
      call dgao (DOXLT (DORE_RCONST(0.0), DORE_RCONST(1.0), DORE_RCONST(0.0)))
      call dgao (DOSC (DORE_RCONST(0.5), DORE_RCONST(0.5), DORE_RCONST(0.5)))
      object2 = DOLL(50)
      call dgao (object2)
      call dgao (DODIFC(DCRGB, red))
      call dgao (DOROT(DCXAX, DORE_RCONST(0.3)))
      call dgao (DOROT(DCYAX, DORE_RCONST(0.3)))
      call dgao (DOLL(100))
      call dgao (DOPMS(DCSPHR))
C----------------------------------------------------------------------
C		0	PushMatrix
C		1	DoLabel(0) 
C		2	Scale(0.5) 
C		3	DiffuseColor(green) 
C		4	PrimitiveSurface(Sphere) 
C		5	PopMatrix	
C		6	object1 = Translate(x) 
C		7	Translate(y) 
C		8	Scale(0.5) 
C		9	object2 = Label(50) 
C	2   --> 10	DiffuseColor(red) 
C	1   --> 11	Rotate(x) 
C		12	rotate(y) 
C		13	Label(100) 
C	4,5 --> 14	PrimitiveSurface(Sphere)
C	0,3 --> 15
C
C----------------------------------------------------------------------

      call DGSEP(11, DCBEG)
      if (DGQEP() .ne. 11) then
         call BadElePtrMessage(14)
      endif

      call DGSEP(-5, DCEND)
      if (DGQEP() .ne. 10) then
         call BadElePtrMessage(15)
      endif

      call DGSEP(11,DCCUR)
      if (DGQEP() .ne. 15) then
         call BadElePtrMessage(16)
      endif

C check the pointer that is set using the label

      call DGSEP(0,DCBEG)
      if (dctrue .ne. DGSEPL(100,1)) then
         call ExitWithMessage(
     x        'wrong value from DGSEPL (DgSetElePtrRelLabel)', 17)
      endif

      if (DGQEP() .ne. 14) then
         call BadElePtrMessage(18)
      endif

C following label 100 should not be found

      if (DCFALS .ne. DGSEPL(100, 1)) then
         call ExitWithMessage(
     x        'wrong value from DGSEPL (DgSetElePtrRelLabel)', 18)
      endif

      if (DGQEP() .ne. 14) then
         call BadElePtrMessage(19)
      endif

      call DGRO(DOPMS(DCBOX))

C ----------------------------------------------------------------------
C		0	PushMatrix
C		1	DoLabel(0) 
C		2	Scale(0.5) 
C		3	DiffuseColor(green) 
C		4	PrimitiveSurface(Sphere) 
C		5	PopMatrix	
C		6	object1 = Translate(x) 
C	6,7 --> 7	Translate(y) 
C		8	Scale(0.5) 
C		9	object2 = Label(50) 
C	        10	DiffuseColor(red) 
C                11	Tranlate(x) 
C		12	rotate(y) 
C		13	Label(100) 
C	        14	PrimitiveSurface(Box)---replaced
C	        15
C ----------------------------------------------------------------------

      call DGSEP(0,DCBEG)
      rtnval = DGSEPL(50,-2)
      if (DGQEP() .ne. 7) then
         call BadElePtrMessage(20)
      endif

C should do nothing
      call DGDE(-3)
      if (DGQS(display_group) .ne. 15) then
 10      format (1x, 'Incorrect group size ', i7)
         write (6,10) DGQS(display_group)
         call ExitWithMessage(message,21)
      endif

      if (DGQEP() .ne. 7) then
         call BadElePtrMessage(22)
      endif

      call DGDE(1)
C ---------------------------------------------------------------------- 
C		0	PushMatrix
C		1	DoLabel(0)
C		2	Scale(0.5)
C		3	DiffuseColor(green)
C		4	PrimitiveSurface(Sphere)
C		5	PopMatrix	
C		6	object1 = Translate(x)
C			(Translate(y))--deleted
C	8   --> 7	Scale(0.5)
C		8	object2 = Label(50)
C		9 	DiffuseColor(red)
C		10	Rotate(x)
C		11	rotate(y)
C		12	Label(100)
C		13	PrimitiveSurface(Box)
C		14
C ----------------------------------------------------------------------
      if (DGQEP() .ne. 7) then
         call BadElePtrMessage(23)
      endif

      grp = DGCS()

C close the group, relplace 1 object, add 1 object
C and verify the objects at guessed position

      call DGROG(display_group, DOSC(DORE_RCONST(1.0), DORE_RCONST(1.0), DORE_RCONST(1.0)))
      object3 = DOREPT(DCSURF)
      call DGAOG(display_group, object3)
C ----------------------------------------------------------------------
C		0	PushMatrix
C		1	DoLabel(0)
C		2	Scale(0.5)
C		3	DiffuseColor(green)
C		4	PrimitiveSurface(Sphere)
C		5	PopMatrix	
C		6	object1 = Translate(x)
C		7	object3 = RepType(Surface)--added
C	9   --> 8	Scale(1.)--replaced
C		9	object2 = Label(50)
C		10 	DiffuseColor(red)
C		11	Rotate(x)
C		12	rotate(y)
C		13	Label(100)
C		14	PrimitiveSurface(Box)
C	10  --> 15
C ----------------------------------------------------------------------

      if (object1 .ne. DGQOP(display_group, 6, DCBEG)) then
         call ExitWithMessage(
     x        'incorrect object from DGQOP (DgInqObjAtPos)', 24)
      endif

      if (object1 .ne. DGQOP(display_group, -9, DCEND)) then
         call ExitWithMessage(
     x        'incorrect object from DGQOP (DgInqObjAtPos)', 25)
      endif

      if (object1 .ne. DGQOP(display_group, -2, DCCUR)) then
         call ExitWithMessage(
     x        'incorrect object from DGQOP (DgInqObjAtPos)', 26)
      endif

      if (object2 .ne. DGQOP(display_group, 9, DCBEG)) then
         call ExitWithMessage(
     x        'incorrect object from DGQOP (DgInqObjAtPos)', 27)
      endif

      if (DGQS(display_group) .ne. 15) then
         call ExitWithMessage('Incorrect group size', 28)
      endif

      if (object3 .ne. DGQOP(display_group, 7, DCBEG)) then
         call ExitWithMessage(
     x        'incorrect object from DGQOP (DgInqObjAtPos)', 29)
      endif

      call DGO(display_group, DCFALS)
      if (DGQEP() .ne. 8) then
         call BadElePtrMessage(30)
      endif

      rtnval = DGCS()

      call DGO(display_group, DCTRUE)
      if (DGQEP() .ne. 15) then
         call BadElePtrMessage(31)
      endif

      rtnval = DGCS()

C create another (undisplay) group

      undisplay_grp1 = DOG(DCTRUE)
      if (undisplay_grp1 .eq. dcnull) then
         call ExitWithMessage('cannot create group', 32)
      endif

      call DGAO(DOLL(0))
      call DGAO(DOXLT(DORE_RCONST(2.0), DORE_RCONST(0.0), DORE_RCONST(0.0)))
      call DGAO(DOLL(150))
      call DGAO(DOXLT(DORE_RCONST(0.0), DORE_RCONST(2.0), DORE_RCONST(0.0)))
      call DGAO(DOLL(150))
      call DGAO(DOROT(DCXAX, DORE_RCONST(0.3)))
      call DGAO(DOROT(DCYAX, DORE_RCONST(0.3)))
      call DGAO(DOLL(200))
      call DGAO(DOSC(DORE_RCONST(0.5), DORE_RCONST(0.5), DORE_RCONST(0.5)))
      call DGAO(DOLL(210))
      call DGAO(DOLL(220))
      call DGAO(DOLL(200))
      call DGAO(DODIFC(DCRGB, green))
      call DGAO(DOPMS(DCSPHR))

C delete some elements by various delete commands
C and check the group size

      call DGDER(5,6)
      if (DGQS(undisplay_grp1) .ne. 12) then
         call ExitWithMessage('Incorrect group size', 33)
      endif

      if (DGQEP() .ne. 5) then
         call BadElePtrMessage(34)
      endif

C ----------------------------------------------------------------------
C		0       DoLabel(0)
C		1       Translate(x)
C		2       Label(150)
C		3       Translate(y)
C		4       Label(150)
C			(Rotate(x))--deleted
C			(Rotate(y))--deleted
C           -->	5	Label(200)
C		6       Scale(0.5)
C		7	Label(210)
C		8	Label(220)
C		9	label(200)
C		10	DiffuseColor(green)
C		11      PrimitiveSurface(Sphere)
C		12
C ----------------------------------------------------------------------
      call DGDER(10,15)
      if (DGQS(undisplay_grp1) .ne. 10) then
         call ExitWithMessage('Incorrect group size', 35)
      endif
      if (DGQEP() .ne. 10) then
         call BadElePtrMessage(36)
      endif
C ----------------------------------------------------------------------
C		0       DoLabel(0)
C		1       Translate(x)
C		2       Label(150)
C		3       Translate(y)
C		4       Label(150)
C           	5	Label(200)
C		6       Scale(0.5)
C		7	Label(210)
C		8	Label(220)
C		9	label(200)
C           -->	10	(DiffuseColor(green))--deleted
C		        (PrimitiveSurface(Sphere))--deleted
C ----------------------------------------------------------------------

      call DGSEP(0,DCBEG)
      write(6,20)
 20   format (1x,
     x	'vvv should cause -unable to find second label- error: vvv')
      if (DCFALS .ne. DGDEL(150,175)) then
         call ExitWithMessage(
     x        'incorrect value form DGDEL (DgDelEleBetweenLabels)', 37)
      endif

      write(6,21)
 21   format(1x, '^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^')

      if (DCTRUE .ne. DGDEL(150,200)) then
         call ExitWithMessage(
     x        'incorrect value from DGDEL (DgDelEleBetweenLabels)', 38)
      endif

      if (DGQS(undisplay_grp1) .ne. 8) then
         call ExitWithMessage('Incorrect goup size', 39)
      endif

      if (DGQEP() .ne. 3) then
         call BadElePtrMessage(40)
      endif

C ----------------------------------------------------------------------
C		0       DoLabel(0)
C		1       Translate(x)
C		2       Label(150)
C		        (Translate(y))--deleted
C		        (Label(150))--deleted
C           --> 3	Label(200)
C		4       Scale(0.5)
C		5	Label(210)
C		6	Label(220)
C		7	label(200)
C		8
C ----------------------------------------------------------------------
      call DGAO(DOLL(250))
      call DGSEP(0,DCBEG)
C ----------------------------------------------------------------------
C	        0       DoLabel(0)
C		1       Translate(x)
C		2       Label(150)
C		3	Label(250)
C            	4	Label(200)
C		5       Scale(0.5)
C		6	Label(210)
C	    -->	7	Label(220)
C		8	label(200)
C		9
C ----------------------------------------------------------------------

C delete the elemetn between two labels and check the group size

      rtnval = DGDEL(250,220)
      if (DGQS(undisplay_grp1) .ne. 6) then
         call ExitWithMessage('Incorrect goup size', 41)
      endif

      if (DGQEP() .ne. 4) then
         call BadElePtrMessage(42)
      endif

C ----------------------------------------------------------------------
C	        0       DoLabel(0)
C		1       Translate(x)
C		2       Label(150)
C		3	Label(250)
C            		Label(200)---------|
C			Scale(0.5)    deleted
C			Label(210)---------|
C	    -->	4	Label(220)
C		5	label(200)
C		6
C ----------------------------------------------------------------------
      call DGE(undisplay_grp1)

      if (DGQS(undisplay_grp1) .ne. 0) then
         call ExitWithMessage('Incorrect group size', 43)
      endif

      rtnval = DGCS()

      undisplay_grp2 = DOG(DCTRUE)
      if (undisplay_grp2 .eq. dcnull) then
         call ExitWithMessage('cannot create group', 44)
      endif

      call DGAO(DOLL(300))
      call DGAO(DOSC(DORE_RCONST(0.5), DORE_RCONST(0.5), DORE_RCONST(0.5)))
      call DGAO(DOLL(350))
      call DGAO(DODIFC(DCRGB, green))
      call DGAO(DOPMS(DCSPHR))
      call DGAO(undisplay_grp1)
      rtnval = DGCS()

      call DGO(undisplay_grp1, DCTRUE)
      call DGAO(DOLL(400))
      call DGAO(DOSC(DORE_RCONST(0.5), DORE_RCONST(0.5), DORE_RCONST(0.5)))
      call DGAO(DOLL(450))
      call DGAO(undisplay_grp2)
      rtnval = dgcs()

      if (DGCK(undisplay_grp1) .ne. DCGBAD) then
         call ExitWithMessage(
     x        'wrong value from DGCK (DgCheck)', 45)
      endif

C a second set of tests for DgSetElePtrRelLabel to
C prove that an old bug is fixed.  The buf occurred when
C the label was the first element in a group

      object4 = DOG(DCTRUE)
      call DGAO(DOLL(1))
      call DGAO(DOLL(2))
      call DGSEP(0, DCBEG)
      if (DCTRUE .ne. DGSEPL(1,1)) then
         call ExitWithMessage(
     x        'Incorrect value from DGSEPL (DgSetElePtrRelLabel)', 46)
      endif

      if (DGQEP() .ne. 1) then
         call BadElePtrMessage(47)
      endif

C remove the currently open group (leavingt no open group

      call DSRO(object4)

C now purposely generate som minor group editing errors

 30   format (1x, 
     x 'vvv should cause -a group is not currently open- error: vvv')
      write (6,30)

      rtnval = DGSEPL(20, 1)
      write (6,21)

 40   format (1x,
     x 'vvv should cause -invalid or deleted object- error: vvv')
      write (6,40)

      call DGAOG(DOG(DCFALS), 200000)
      write (6, 21)

C test open group nesting

      grp1 = DOG(DCTRUE)
      if (grp1 .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 48)
      endif

      call DGAO(DOLL(10))
      grp2 = DOG(DCTRUE)
      if (grp2 .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 49)
      endif

      call DGAO(DOLL(20))
      call DGAO(DOLL(21))
      call DGAO(DGCS())
      if (grp1 .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 50)
      endif
      call DGAO(DOLL(11))
      grp3 = DOG(DCTRUE)
      if (grp3 .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 51)
      endif
      
      call DGAO(DOLL(20))
      grp4 = DOG(DCTRUE)
      if (grp4 .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 52)
      endif
      
      call DGAO(DOLL(30))
      call DGAO(DGCS())
      if (grp3 .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 53)
      endif
      
      call DGAO(DOLL(21))
C reset ele ptr
      call DGSEP(0, DCBEG)
C point ot last goup
      rtnval = DGSEPL(20, 1)

      call DGAO (DGCS())
      if (grp1 .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 54)
      endif

      call DGAO(DOLL(12))
      grp = DGCS()

      if (dcnull .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 55)
      endif

 50   format (1x, 'group before: ')
 51   format (1x, 'replacing this element: ')
 52   format (1x, 'group after: ')

      write (6, 50)
      call DSPO(grp)

      write (6, 51)
      call DSPO(DGQOP(grp3, 0, DCCUR))
      call DGO(grp3, DCFALS)
      
      if (grp3 .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 56)
      endif
      
      call DGRO(DOLL(3))
      rtnval = DGCS()

      if (dcnull .ne. DGQO()) then
         call ExitWithMessage(
     x        'incorrect open group from DGQO (DgInqOpen)', 57)
      endif

      write (6,52)
      call DSPO(grp)

 60   format (1x, 'ALL GROUP TESTS PASSED')
      write (6, 60)

      call DSTERM()

      end

C ======================================================================
      subroutine ExitWithMessage (str, line)

      include '/usr/include/fortran/DORE'

      character*80 str
      integer*4 line

 10   format (1x,'*** test failed at test point ', i3, ' ***', a)
      
      write (6, 10) line, str

      return
      end

C ======================================================================
      subroutine BadElePtrMessage (line)

      include '/usr/include/fortran/DORE'

      integer*4 line
      character*80 str

 10   format (1x, '--- internal error no group currently open !!! ---')
 20   format (1x, 'incorrect element pointer = ', i7)

      if (DGQO() .eq. dcnull) then
         write (6, 10)
         call exit(1)
      endif

      write (str, 20) DGQEP()

      call ExitWithMessage (str, line)

      return
      end
C ======================================================================
