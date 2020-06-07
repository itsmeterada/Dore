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

      external slot_callback_datptr
      external slot_callback_datval
      external slot_callback_null
      external callback_datptr
      external callback_datval
      external callback_null
      external pick_callback_datptr
      external pick_callback_datval
      external pick_callback_null
      integer*4 pick_callback_datptr
      integer*4 pick_callback_datval
      integer*4 pick_callback_null

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value

      data datval /36/
      data value / DORE_RCONST(12.9876) /

      integer*4 index_size
      integer*4 index(10)
      DORE_REAL z_values(10)
      integer*4 views(10)
      integer*4 list_size
      integer*4 hit_list(20)
      integer*4 e_flag
      DORE_REAL pck_pnt(3)
      integer*4 hit_count

      integer*4 ckstatus

      data index_size /10/
      data list_size /20/
      data pck_pnt /DORE_RCONST(64.0), DORE_RCONST(64.0), DORE_RCONST(4000.0)/

      character*80 version
      integer*4 ver_len
      data ver_len /80/

      integer*4 device, frame, view, group
      DORE_REAL origin(3), camera(3), up(3)

      data origin / DORE_RCONST(0.0), DORE_RCONST(0.0), DORE_RCONST(0.0)/
      data camera / DORE_RCONST(0.0), DORE_RCONST(0.0), DORE_RCONST(10.0)/
      data up     / DORE_RCONST(0.0), DORE_RCONST(1.0), DORE_RCONST(0.0)/

      common /gd/ datptr, datval, value, slot, view

C***********************************************************

      call dsinit (0)

      call dsqver(version, ver_len)
      print *, 'Version ', version

      datptr = dcisry

 10   format (a14,z8.8)
 20   format (a12,i10)
 30   format (a12,f10.4)

C***********************************************************

      print *, 'Slot Callback Test ...'

      slot = dois()

      call dgaog (dsqvg(slot), docb(slot_callback_datptr,
     +				      dodp(datptr)))
      call dgaog (dsqvg(slot), docb(slot_callback_datval,
     +     			      dodv(datval)))
      call dgaog (dsqvg(slot), docb(slot_callback_null,
     +				      dcnull))

      call dsiv(slot, value)

      print *, 'Slot Callback Test ... Done'
      print *, ' '

C***********************************************************

      print *, 'Standard Callback Test ...'

      group = dog(dctrue)
         call dgao (docb(callback_datptr, dodp(datptr)))
         call dgao (docb(callback_datval, dodv(datval)))
         call dgao (docb(callback_null, dcnull))
      ckstatus = dgck (dgcs())

      print *, 'Standard Callback Test ... Done'
      print *, ' '

C***********************************************************

      device = dod ('rasterfile', 10,
     +		    '-filename /dev/null -width 128 -height 128', 43)
      frame = dofr ()
      call ddsf (device, frame)
      view = dovw ()
      call dgaog (dfqvg (frame), view)
      group = dog(dctrue)
         call dgao (doper (DORE_RCONST(30.0), DORE_RCONST(-0.1),
     x     DORE_RCONST(-100.0)))
         call dgao (dopumx ())
            call dgao (dolaf (origin, camera, up))
            call dgao (docm())
         call dgao (doppmx ())
      call dgaog (dvqdg(view), dgcs ())

      group = dog(dctrue)
         call dgao (dops(dcon))
         call dgao (dopms(dcsphr))
      call dgaog (dvqig(view), dgcs())

C***********************************************************

      print *, 'Pick Callback Test ...'

      call ddspcb(device, docb(pick_callback_datptr,
     +                         dodp(datptr)))

      call ddpo(device, pck_pnt, hit_count, index_size, index,
     +		list_size, hit_list, z_values, DCNULL, DCNULL, 
     +		views, e_flag)

      call ddspcb(device, docb(pick_callback_datval,
     +                         dodv(datval)))
      call ddpo(device, pck_pnt, hit_count, index_size, index,
     +		list_size, hit_list, z_values, DCNULL, DCNULL, 
     +     	views, e_flag)

      call ddspcb(device, docb(pick_callback_null,
     +                         dcnull))
      call ddpo(device, pck_pnt, hit_count, index_size, index,
     +     	list_size, hit_list, z_values, DCNULL, DCNULL, 
     +     	views, e_flag)

      call dsro (device)

      print *, 'Pick Callback Test ... Done'
      print *, ' '

      call dsterm

      end

C **********************************************************************

      subroutine slot_callback_datptr (ldata, lslot, lvalue)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata
      integer*4 lslot
      DORE_REAL lvalue

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view


      if (ldata .ne. datptr) then
         write (6,10)
         write (6,20) 'datptr', datptr, 'ldata', ldata
      endif

      if (lslot .ne. slot) then
         write (6,10)
         write (6,20) 'slot', slot, 'lslot', lslot
      endif

      if (lvalue .ne. value) then
         write (6,10)
         write (6,30) 'value', value, 'lvalue', lvalue
      endif

 10   format (' slot with DoDataPtr')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')
 30   format (1x,a6,' (',f10.4,') != ',a6,' (',f10.4,')')

      return 
      end

C **********************************************************************

      subroutine slot_callback_datval (ldata, lslot, lvalue)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata
      integer*4 lslot
      DORE_REAL lvalue

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view

      if (ldata .ne. datval) then
         write (6,10)
         write (6,20) 'datval', datval, 'ldata', ldata
      endif

      if (lslot .ne. slot) then
         write (6,10)
         write (6,20) 'slot', slot, 'lslot', lslot
      endif

      if (lvalue .ne. value) then
         write (6,10)
         write (6,30) 'value', value, 'lvalue', lvalue
      endif

 10   format (' slot with DoDataVal')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')
 30   format (1x,a6,' (',f10.4,') != ',a6,' (',f10.4,')')

      return 
      end

C **********************************************************************

      subroutine slot_callback_null (ldata, lslot, lvalue)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata
      integer*4 lslot
      DORE_REAL lvalue

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view


      if (ldata .ne. 0) then
         write (6,10)
         write (6,20) 'NULL', 0, 'ldata', ldata
      endif

      if (lslot .ne. slot) then
         write (6,10)
         write (6,20) 'slot', slot, 'lslot', lslot
      endif

      if (lvalue .ne. value) then
         write (6,10)
         write (6,30) 'value', value, 'lvalue', lvalue
      endif

 10   format (' slot with DcNullObject')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')
 30   format (1x,a6,' (',f10.4,') != ',a6,' (',f10.4,')')

      return 
      end

C **********************************************************************

      subroutine callback_datptr (ldata)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view

      if (ldata .ne. datptr) then
         write (6,10)
         write (6,20) 'datptr', datptr, 'ldata', ldata
      endif

 10   format (' standard callback with DoDataPtr')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')

      return 
      end

C **********************************************************************

      subroutine callback_datval (ldata)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view

      if (ldata .ne. datval) then
         write (6,10)
         write (6,20) 'datval', datval, 'ldata', ldata
      endif

 10   format (' standard callback with DoDataVal')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')

      return 
      end

C **********************************************************************

      subroutine callback_null (ldata)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view

      if (ldata .ne. 0) then
         write (6,10)
         write (6,20) 'NULL', 0, 'ldata', ldata
      endif

 10   format (' standard callback with DcNullObject')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')

      return 
      end

C **********************************************************************

      integer*4 function pick_callback_datptr(ldata,pathElements,
     +     Path, ZValue, lview, HitsAccepted)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata
      integer*4 PathElements
      integer*4 Path(*)
      DORE_REAL ZValue
      integer*4 lview
      integer*4 HitsAccepted

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view

      if (ldata .ne. datptr) then
         write (6,10)
         write (6,20) 'datptr', datptr, 'ldata', ldata
      endif

      if (PathElements .ne. 3) then
         write (6,10)
         write (6,30) 'PathElements', PathElements, 3
      endif

      if (HitsAccepted .ne. 0) then
         write (6,10)
         write (6,30) 'HitsAccepted', HitsAccepted, 0
      endif

      if (lview .ne. view) then
         write (6,10)
         write (6,20) 'view', view, 'lview', lview
      endif

C
C      call print_path(PathElements,Path)
C
      pick_callback_datptr = dchacc

 10   format (' pick callback with DoDataPtr')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')
 30   format (1x,a,' (',i,') != ',i)

      return 
      end

C **********************************************************************

      integer*4 function pick_callback_datval(ldata,pathElements,
     +     Path, ZValue, lview, HitsAccepted)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata
      integer*4 PathElements
      integer*4 Path(*)
      DORE_REAL ZValue
      integer*4 lview
      integer*4 HitsAccepted

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view

      if (ldata .ne. datval) then
         write (6,10)
         write (6,20) 'datval', datval, 'ldata', ldata
      endif

      if (PathElements .ne. 3) then
         write (6,10)
         write (6,30) 'PathElements', PathElements, 3
      endif

      if (HitsAccepted .ne. 0) then
         write (6,10)
         write (6,30) 'HitsAccepted', HitsAccepted, 0
      endif

      if (lview .ne. view) then
         write (6,10)
         write (6,20) 'view', view, 'lview', lview
      endif

C
C      call print_path(PathElements,Path)
C
      pick_callback_datval = dchacc

 10   format (' pick callback with DoDataVal')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')
 30   format (1x,a,' (',i,') != ',i)

      return 
      end


C **********************************************************************

      integer*4 function pick_callback_null(ldata,pathElements,
     +     Path, ZValue, lview, HitsAccepted)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 ldata
      integer*4 PathElements
      integer*4 Path(*)
      DORE_REAL ZValue
      integer*4 lview
      integer*4 HitsAccepted

      integer*4 slot
      integer*4 datptr
      integer*4 datval
      DORE_REAL value
      integer*4 view
      common /gd/ datptr, datval, value, slot, view

      if (ldata .ne. 0) then
         write (6,10)
         write (6,20) 'NULL', 0, 'ldata', ldata
      endif

      if (PathElements .ne. 3) then
         write (6,10)
         write (6,30) 'PathElements', PathElements, 3
      endif

      if (HitsAccepted .ne. 0) then
         write (6,10)
         write (6,30) 'HitsAccepted', HitsAccepted, 0
      endif

      if (lview .ne. view) then
         write (6,10)
         write (6,20) 'view', view, 'lview', lview
      endif

C
C      call print_path(PathElements,Path)
C
      pick_callback_null = dchacc

 10   format (' pick callback with DcNullObject')
 20   format (1x,a6,' (0x',z8.8,') != ',a6,' (0x',z8.8,')')
 30   format (1x,a,' (',i,') != ',i)

      return 
      end

C **********************************************************************

      subroutine print_path (size, path)

Cs      implicit none
      include '/usr/include/fortran/DORE'

      integer*4 size
      integer*4 path(*)

      integer*4 type
      integer*4 iname
      character*80 sname
      integer*4 i
      integer*4 status
      integer*4 len

      data len /80/

      print *, '+------------------------------+-------+-------+'
      print *, '|                              |  pick |  ele  |'
      print *, '|           group              |   id  | seq # |'
      print *, '+------------------------------+-------+-------+'

 10   format (1x,a1,i30,a1,i7,a1,i7,a1)
 20   format (1x,a1,a30,a1,i7,a1,i7,a1)
 30   format (1x,a1,a20,i10,a1,i7,a1,i7,a1)

      do i=0, size-1, 1
         status = dsqvos(path(3*i+1))
         if (status .eq. dcovld) then
            type = dsqont(path(3*i+1))
            if (type .eq. dcnint) then
               iname = dsqoni(path(3*i+1))
               write (6, 10) '|', iname, '|', path(3*i+2), 
     +              '|', path(3*i+3), '|'
            else if (type .eq. dcnstr) then
               call dsqons(path(3*i+1), sname, len)
               write (6, 20) '|', sname, '|', path(3*i+2), 
     +              '|', path(3*i+3), '|' 
            else if (type .eq. dcnnon) then
               write (6, 20) '|', 'no name', '|', path(3*i+2), 
     +              '|', path(3*i+3), '|' 
            else 
               write (6, 30) '|', 'unknown name type', type, '|', 
     +              path(3*i+2), '|', path(3*i+3), '|' 
            endif
         else if (status .eq. dcoinv) then
            write (6, 20) '|', 'invalid!', '|', path(3*i+2), 
     +              '|', path(3*i+3), '|' 
         else if (status .eq. dcodel) then
            write (6, 20) '|', 'deleted!', '|', path(3*i+2), 
     +              '|', path(3*i+3), '|' 
         else
            write (6, 30) '|', 'unknown validity type', status, 
     +              '|', path(3*i+2), '|', path(3*i+3), '|' 
         endif
      end do

      print *, '+------------------------------+-------+-------+'

      return
      end


C **********************************************************************
