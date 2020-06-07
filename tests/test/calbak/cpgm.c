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
 
#include <dore/dore.h>
#include <stdio.h>

DtPtr datptr;
Dt32Bits datval;
DtObject slot;
DtObject view;
DtReal value;

main ()
{
   extern slot_callback_datptr();
   extern slot_callback_datval();
   extern slot_callback_null();
   extern callback_datptr();
   extern callback_datval();
   extern callback_null();
   extern DtHitStatus pick_callback_datptr();
   extern DtHitStatus pick_callback_datval();
   extern DtHitStatus pick_callback_null();
   DtInt x;
   DtPtr version;

   DtInt index_size = 10;
   DtInt index[10];
   DtReal z_values[10];
   DtObject views[10];
   DtInt list_size = 20;
   DtInt hit_list[20];
   DtInt e_flag;
   static DtPoint3 pck_pnt = {64., 64., 4000.};
   DtInt hit_count;

   DtObject device, frame;
   static DtPoint3
	 origin = { 0., 0.,  0.},
	 camera = { 0., 0., 10.};
   static DtVector3
	 up = { 0., 1., 0.};

   datptr = &x;
   datval = 36;
   value = 12.9876;

   DsInitialize (0);

   DsInqVersion (&version);
   printf ("Version '%s'\n", version);

   /***************************************************/

   printf ("Slot Callback Test...");
   fflush(stdout);

   slot = DoInputSlot();

   DgAddObjToGroup(DsInqValuatorGroup(slot),
		   DoCallback(slot_callback_datptr, 
			      DoDataPtr(datptr)));
   DgAddObjToGroup(DsInqValuatorGroup(slot),
		   DoCallback(slot_callback_datval, 
			      DoDataVal(datval)));
   DgAddObjToGroup(DsInqValuatorGroup(slot),
		   DoCallback(slot_callback_null, 
			      DcNullObject));

   DsInputValue(slot, value);

   printf (" done\n");

   /***************************************************/

   printf ("Standard Callback Test...");
   fflush(stdout);

   DoGroup(DcTrue);
      DgAddObj(DoCallback(callback_datptr, DoDataPtr(datptr)));
      DgAddObj(DoCallback(callback_datval, DoDataVal(datval)));
      DgAddObj(DoCallback(callback_null,   DcNullObject));
   DgCheck(DgClose());

   printf (" done\n");

   /***************************************************/

   device = DoDevice("rasterfile", 
		     "-filename /dev/null -width 128 -height 128");

   frame = DoFrame();
   DdSetFrame(device, frame);

   view = DoView();
   DgAddObjToGroup(DfInqViewGroup(frame), view);
   DoGroup(DcTrue);
      DgAddObj(DoPerspective(30., -.1, -100.));
      DgAddObj(DoPushMatrix());
         DgAddObj(DoLookAtFrom(origin, camera, up));
         DgAddObj(DoCamera());
      DgAddObj(DoPopMatrix());
   DgAddObjToGroup(DvInqDefinitionGroup(view), DgClose());

   DoGroup(DcTrue);
      DgAddObj(DoPickSwitch(DcOn));
      DgAddObj(DoPrimSurf(DcSphere));
   DgAddObjToGroup(DvInqDisplayGroup(view), DgClose());

   /***************************************************/

   printf ("Pick Callback Test...");
   fflush(stdout);

   DdSetPickCallback(device, DoCallback(pick_callback_datptr,
					DoDataPtr(datptr)));
   DdPickObjs(device, pck_pnt, &hit_count, index_size, index, 
	      list_size, hit_list, z_values, (DtReal *)0, (DtReal *)0,
	      views, &e_flag);

   DdSetPickCallback(device, DoCallback(pick_callback_datval,
					DoDataVal(datval)));
   DdPickObjs(device, pck_pnt, &hit_count, index_size, index, 
	      list_size, hit_list, z_values, (DtReal *)0, (DtReal *)0,
	      views, &e_flag);

   DdSetPickCallback(device, DoCallback(pick_callback_null,
					DcNullObject));
   DdPickObjs(device, pck_pnt, &hit_count, index_size, index, 
	      list_size, hit_list, z_values, (DtReal *)0, (DtReal *)0,
	      views, &e_flag);

   DsReleaseObj(device);

   printf (" done\n");

   DsTerminate();
}
/*
  ======================================================================
 */
slot_callback_datptr(ldata, lslot, lvalue)
     DtPtr ldata;
     DtObject lslot;
     DtReal lvalue;
{
   if (ldata != datptr) {
      printf ("slot with DoDataPtr:\n");
      printf ("datptr (0x%x) != ldata (0x%x)\n", datptr, ldata);
   }
   if (lslot != slot) {
      printf ("slot with DoDataPtr:\n");
      printf ("slot (0x%x) != lslot (0x%x)\n", slot, lslot);
   }
   if (lvalue != value) {
      printf ("slot with DoDataPtr:\n");
      printf ("value (%g) != lvalue (%g)\n", value, lvalue);
   }
}
/*
  ======================================================================
 */
slot_callback_datval(ldata, lslot, lvalue)
     Dt32Bits ldata;
     DtObject lslot;
     DtReal lvalue;
{
   if (ldata != datval) {
      printf ("slot with DoDataVal:\n");
      printf ("datval (%d) != ldata (%d)\n", datval, ldata);
   }
   if (lslot != slot) {
      printf ("slot with DoDataVal:\n");
      printf ("slot (0x%x) != lslot (0x%x)\n", slot, lslot);
   }
   if (lvalue != value) {
      printf ("slot with DoDataVal:\n");
      printf ("value (%g) != lvalue (%g)\n", value, lvalue);
   }
}
/*
  ======================================================================
 */
slot_callback_null(ldata, lslot, lvalue)
     Dt32Bits ldata;
     DtObject lslot;
     DtReal lvalue;
{
   if (ldata != 0) {
      printf ("slot with DcNullObject:\n");
      printf ("NULL != ldata (%d)\n", ldata);
   }
   if (lslot != slot) {
      printf ("slot with DcNullObject:\n");
      printf ("slot (0x%x) != lslot (0x%x)\n", slot, lslot);
   }
   if (lvalue != value) {
      printf ("slot with DcNullObject:\n");
      printf ("value (%g) != lvalue (%g)\n", value, lvalue);
   }
}
/*
  ======================================================================
 */
callback_datptr (ldata)
     DtPtr ldata;
{
   if (ldata != datptr) {
      printf ("standard callback with DoDataPtr:\n");
      printf ("datptr (0x%x) != ldata (0x%x)\n", datptr, ldata);
   }
}
/*
  ======================================================================
 */
callback_datval (ldata)
     Dt32Bits ldata;
{
   if (ldata != datval) {
      printf ("standard callback with DoDataVal:\n");
      printf ("datval (%d) != ldata (%d)\n", datval, ldata);
   }
}
/*
  ======================================================================
 */
callback_null (ldata)
     DtPtr ldata;
{
   if (ldata != 0) {
      printf ("standard callback with DcNullObject:\n");
      printf ("NULL != ldata (%d)\n", ldata);
   }
}
/*
  ======================================================================
 */
DtHitStatus
pick_callback_datptr(ldata,PathElements, Path, ZValue, View, HitsAccepted)
     DtPtr ldata;
     DtInt PathElements;
     DtInt Path[];
     DtReal ZValue;
     DtObject View;
     DtInt HitsAccepted;
{

   if (ldata != datptr) {
      printf ("pick callback with DoDataPtr:\n");
      printf ("datptr (0x%x) != ldata (0x%x)\n", datptr, ldata);
   }
   if (PathElements != 3) {
      printf ("pick callback with DoDataPtr:\n");
      printf ("PathElements (%d) != 3\n", PathElements);
   }
   if (View != view) {
      printf ("pick callback with DoDataPtr:\n");
      printf ("View (0x%x) != view (0x%x)\n", View, view);
   }
   if (HitsAccepted != 0) {
      printf ("pick callback with DoDataPtr:\n");
      printf ("HitsAccepted (%d) != 0\n", HitsAccepted);
   }

/*
   printf ("   ZValue       = %g\n", ZValue);
   PrintPath(PathElements, Path);
*/
   return (DcHitAccept);
}
/*
  ======================================================================
 */
DtHitStatus
pick_callback_datval(ldata,PathElements, Path, ZValue, View, HitsAccepted)
     Dt32Bits ldata;
     DtInt PathElements;
     DtInt Path[];
     DtReal ZValue;
     DtObject View;
     DtInt HitsAccepted;
{

   if (ldata != datval) {
      printf ("pick callback with DoDataVal\n");
      printf ("datval (%d) != ldata (%d)\n", datval, ldata);
   }
   if (PathElements != 3) {
      printf ("pick callback with DoDataVal\n");
      printf ("PathElements (%d) != 3\n", PathElements);
   }
   if (View != view) {
      printf ("pick callback with DoDataVal\n");
      printf ("View (0x%x) != view (0x%x)\n", View, view);
   }
   if (HitsAccepted != 0) {
      printf ("pick callback with DoDataVal\n");
      printf ("HitsAccepted (%d) != 0\n", HitsAccepted);
   }

/*
   printf ("   ZValue       = %g\n", ZValue);
   PrintPath(PathElements, Path);
*/
   return (DcHitAccept);
}
/*
  ======================================================================
 */
DtHitStatus
pick_callback_null(ldata,PathElements, Path, ZValue, View, HitsAccepted)
     Dt32Bits ldata;
     DtInt PathElements;
     DtInt Path[];
     DtReal ZValue;
     DtObject View;
     DtInt HitsAccepted;
{

   if (ldata != 0) {
      printf ("pick callback with DcNullObject\n");
      printf ("NULL != ldata (%d)\n", ldata);
   }
   if (PathElements != 3) {
      printf ("pick callback with DcNullObject\n");
      printf ("PathElements (%d) != 3\n", PathElements);
   }
   if (View != view) {
      printf ("pick callback with DcNullObject\n");
      printf ("View (0x%x) != view (0x%x)\n", View, view);
   }
   if (HitsAccepted != 0) {
      printf ("pick callback with DcNullObject\n");
      printf ("HitsAccepted (%d) != 0\n", HitsAccepted);
   }

/*
   printf ("   ZValue       = %g\n", ZValue);
   PrintPath(PathElements, Path);
*/
   return (DcHitAccept);
}
/*
  ======================================================================
 */
PrintPath(Size,Path)
     DtInt Size,Path[];

{
   DtNameType type;
   DtPtr *name;
   DtInt i;

   printf("+------------------------------+-------+-------+\n");
   printf("|                              |  pick |  ele  |\n");
   printf("|           group              |   id  | seq # |\n");
   printf("+------------------------------+-------+-------+\n");
   for(i=0; i<Size; i++) {
      switch(DsInqObjStatus((DtObject)Path[3*i]))	{
      case DcObjectValid:
	 DsInqObjName((DtObject)Path[3*i],&type,&name); 
	 switch(type){
	 case DcNameInteger:
	    printf("|%30d",*(int *)name);
	    break;
	 case DcNameString:
	    printf("|%30s",name);
	    break;
	 case DcNameNone:
	    printf("|%30s","no name");
	    break;
	 default: 
	    printf("|%30s", "unknown name type");
	    break;
	 }
	 break;
      case DcObjectInvalid:
	 printf("|30%s", "invalid!");
	 break;
      case DcObjectDeleted:
	 printf("|%30s", "deleted!");
	 break;
      default: 
	 printf("|%30s", "unknown validity type");
	 break;
      }
      printf("|%7d|%7d|\n",Path[3*i+1],Path[3*i+2]);
   } 
   printf("+------------------------------+-------+-------+\n");
}
/*
  ======================================================================
 */
   
