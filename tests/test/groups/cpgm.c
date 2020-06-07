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
This is a test of the Dore "groups" functionality. All group commands
are used and tested.
In this test, typical uses of each function are and verified,
but most of the functions are used in a straightforward way.

No image is generated. The program is validated completely from
the diognostics printed on the terminal. Some minor errors are purposely
generated to verify that they are trapped and that the proper Dore error
message printed. These error messages are bracketed by diognostic lines.
True test failures are reported with lines beginning "*** test failed ...".
*/

#include <dore/dore.h>

main()
{
   DtObject object1, object2, object3, object4;
   DtObject camera_group, display_group;
   DtObject undisplay_grp1,undisplay_grp2;
   static DtPoint3 origin = {0.,0.,0.}, 
   		camera_from = {0.,0.,10.},
		light_from = {-10.,10.,10.};
   static DtVector3 up = {0.,1.,0.};
   static DtReal green[] = {0.,1.,0.}, 
		red[] = {1.,0.,0.};
   DtObject grp,grp1, grp2, grp3, grp4;
   DtPtr version;

   system("cat README");

   DsInitialize(0);

   DsInqVersion (&version);
   printf ("Version '%s'\n", version);

   if(DgInqOpen() != DcNullObject)
	 ExitWithMessage("incorrect open group", __LINE__);

   /***** create a typical definition group checking element pointer *****/

   if(!(camera_group = DoGroup(DcTrue)))
	 ExitWithMessage("can't create group", __LINE__);

   /* check the opened group */

   if(DgInqOpen() != camera_group)
	 ExitWithMessage("incorrect open group", __LINE__);

   /* add light objects */

   DgAddObj(DoPushMatrix());
	   DgAddObj(DoLightIntens(1.));
	   DgAddObj(DoLookAtFrom(origin, light_from, up));
	   DgAddObj(DoLight());
   DgAddObj(DoPopMatrix());

   /* add camera objects */

   DgAddObj(DoParallel(10.,0.,-100.));
   DgAddObj(DoLookAtFrom(origin, camera_from, up));
   DgAddObj(DoCamera());

   /* check the current position of ptr. */
	
   if(DgInqElePtr() != 8)
	 BadElePtrMessage(__LINE__);

   /***** set pointer and check the position *****/
	
   DgSetElePtr(0,DcBeginning); 
   if(DgInqElePtr() != 0)
	 BadElePtrMessage(__LINE__);
   DgSetElePtr(0,DcEnd);
   if(DgInqElePtr() != 8)
	 BadElePtrMessage(__LINE__);
   DgSetElePtr(2,DcBeginning);
   if(DgInqElePtr() != 2)
	 BadElePtrMessage(__LINE__);
   DgSetElePtr(-2,DcCurrent);
   if(DgInqElePtr() != 0)
	 BadElePtrMessage(__LINE__);
   if(camera_group != DgClose())
	 ExitWithMessage("incorrect return value of group_close",
			 __LINE__);

   if(DcNullObject != DgClose())
	 ExitWithMessage("incorrect return value of group_close",
			 __LINE__);

   /***** check the size of closed group *****/
   if(DgInqSize(camera_group) != 8) /*check the group size*/
	 ExitWithMessage("Incorrect group size", __LINE__);

   if(DgCheck(camera_group) != DcGroupOk)
	 ExitWithMessage("wrong value from DgCheck", __LINE__);

   /*****  create another (display) group *****/
   if(!(display_group = DoGroup(DcTrue)))
	 ExitWithMessage("can't create group", __LINE__);
   DgAddObj(DoPushMatrix());
	   DgAddObj(DoLabel(0));
	   DgAddObj(DoScale(.5,.5,.5));
	   DgAddObj(DoDiffuseColor(DcRGB, green));
	   DgAddObj(DoPrimSurf(DcSphere));
   DgAddObj(DoPopMatrix());
   DgAddObj(object1 = DoTranslate(1.,0.,0.));
   DgAddObj(DoTranslate(0.,1.,0.));
   DgAddObj(DoScale(.5,.5,.5)); 
   DgAddObj(object2 = DoLabel(50));
   DgAddObj(DoDiffuseColor(DcRGB, red));
   DgAddObj(DoRotate(DcXAxis, .3));
   DgAddObj(DoRotate(DcYAxis, .3)); 
   DgAddObj(DoLabel(100));
   DgAddObj(DoPrimSurf(DcSphere));
/*-------------------------------------------------------------
		0	PushMatrix
		1	DoLabel(0) 
		2	Scale(0.5) 
		3	DiffuseColor(green) 
		4	PrimitiveSurface(Sphere) 
		5	PopMatrix	
		6	object1 = Translate(x) 
		7	Translate(y) 
		8	Scale(0.5) 
		9	object2 = Label(50) 
	2   --> 10	DiffuseColor(red) 
	1   --> 11	Rotate(x) 
		12	rotate(y) 
		13	Label(100) 
	4,5 --> 14	PrimitiveSurface(Sphere)
	0,3 --> 15
	-------------------------------------------------------------*/

   DgSetElePtr(11,DcBeginning);
   if(DgInqElePtr() != 11) /* check the current position of ptr. */
	 BadElePtrMessage(__LINE__);

   DgSetElePtr(-5,DcEnd);
   if(DgInqElePtr() != 10) /* check the current position of ptr. */
	 BadElePtrMessage(__LINE__);

   DgSetElePtr(11,DcCurrent);
   if(DgInqElePtr() != 15) /* check the current position of ptr. */
	 BadElePtrMessage(__LINE__);

   /***** check the pointer that is set using the label *****/
   DgSetElePtr(0,DcBeginning);
   if(DcTrue != DgSetElePtrRelLabel(100,1))
	 ExitWithMessage("wrong value from set_ele_ptr_rel_label",
			 __LINE__);
   if(DgInqElePtr() != 14)
	 BadElePtrMessage(__LINE__);

   /* following label 100 should not be found.*/
   if(DcFalse != DgSetElePtrRelLabel(100,1))
	 ExitWithMessage("incorrect value from set_ele_ptr_rel_label",
			 __LINE__);
   if(DgInqElePtr() != 14)
	 BadElePtrMessage(__LINE__);
   DgReplaceObj(DoPrimSurf(DcBox));

	/*-------------------------------------------------------------
		0	PushMatrix
		1	DoLabel(0) 
		2	Scale(0.5) 
		3	DiffuseColor(green) 
		4	PrimitiveSurface(Sphere) 
		5	PopMatrix	
		6	object1 = Translate(x) 
	6,7 --> 7	Translate(y) 
		8	Scale(0.5) 
		9	object2 = Label(50) 
	        10	DiffuseColor(red) 
                11	Tranlate(x) 
		12	rotate(y) 
		13	Label(100) 
	        14	PrimitiveSurface(Box)---replaced
	        15
	-------------------------------------------------------------*/
   DgSetElePtr(0,DcBeginning);
   DgSetElePtrRelLabel(50,-2);
   if(DgInqElePtr() != 7)
	 BadElePtrMessage(__LINE__);

   DgDelEle(-3); /* should do nothing */
   if(DgInqSize(display_group) != 15) {
      char str[100];
      sprintf(str, "Incorrect group size %d\n",
	      (DgInqSize(display_group)));
      ExitWithMessage(str, __LINE__);
   }

   if(DgInqElePtr() != 7)
	 BadElePtrMessage(__LINE__);
   DgDelEle(1); 
	/*-------------------------------------------------------------
		0	PushMatrix
		1	DoLabel(0)
		2	Scale(0.5)
		3	DiffuseColor(green)
		4	PrimitiveSurface(Sphere)
		5	PopMatrix	
		6	object1 = Translate(x)
			(Translate(y))--deleted
	8   --> 7	Scale(0.5)
		8	object2 = Label(50)
		9 	DiffuseColor(red)
		10	Rotate(x)
		11	rotate(y)
		12	Label(100)
		13	PrimitiveSurface(Box)
		14
	-------------------------------------------------------------*/
   if(DgInqElePtr() != 7)
	 BadElePtrMessage(__LINE__);
   DgClose();

   /**** close the group, replace 1 object, add 1 object,
     and verify the objects at guessed position *****/
	    
   DgReplaceObjInGroup(display_group, DoScale(1.,1.,1.));
   DgAddObjToGroup(display_group, object3 = DoRepType(DcSurface));

	/*-------------------------------------------------------------
		0	PushMatrix
		1	DoLabel(0)
		2	Scale(0.5)
		3	DiffuseColor(green)
		4	PrimitiveSurface(Sphere)
		5	PopMatrix	
		6	object1 = Translate(x)
		7	object3 = RepType(Surface)--added
	9   --> 8	Scale(1.)--replaced
		9	object2 = Label(50)
		10 	DiffuseColor(red)
		11	Rotate(x)
		12	rotate(y)
		13	Label(100)
		14	PrimitiveSurface(Box)
	10  --> 15
	-------------------------------------------------------------*/
   if(object1 != DgInqObjAtPos(display_group,6,DcBeginning))
	 ExitWithMessage("incorrect object from DgInqObjAtPos",__LINE__);

   if(object1 != DgInqObjAtPos(display_group,-9,DcEnd))
	 ExitWithMessage("incorrect object from DgInqObjAtPos",__LINE__);

   if(object1 != DgInqObjAtPos(display_group,-2,DcCurrent))
	 ExitWithMessage("incorrect object from DgInqObjAtPos",__LINE__);

   if(object2 != DgInqObjAtPos(display_group,9,DcBeginning))
	 ExitWithMessage("incorrect object from DgInqObjAtPos",__LINE__);

   if(DgInqSize(display_group) != 15)
	 ExitWithMessage("Incorrect group size", __LINE__);
   if(object3 != DgInqObjAtPos(display_group,7,DcBeginning))
	 ExitWithMessage("incorrect object from DgInqObjAtPos",__LINE__);
   DgOpen(display_group,DcFalse);
   if(DgInqElePtr() != 8)
	 BadElePtrMessage(__LINE__);
   DgClose();

   DgOpen(display_group,DcTrue);
   if(DgInqElePtr() != 15)
	 BadElePtrMessage(__LINE__);
   DgClose();
	
   /***** create another (undisplay) group *****/
   if(!(undisplay_grp1 = DoGroup(DcTrue)))
	 ExitWithMessage("can't create group", __LINE__);
   DgAddObj(DoLabel(0));
   DgAddObj(DoTranslate(2.,0.,0.));
   DgAddObj(DoLabel(150));
   DgAddObj(DoTranslate(0.,2.,0.));
   DgAddObj(DoLabel(150));
   DgAddObj(DoRotate(DcXAxis, .3));
   DgAddObj(DoRotate(DcYAxis, .3));
   DgAddObj(DoLabel(200));
   DgAddObj(DoScale(.5,.5,.5));
   DgAddObj(DoLabel(210));
   DgAddObj(DoLabel(220));
   DgAddObj(DoLabel(200));
   DgAddObj(DoDiffuseColor(DcRGB, green));
   DgAddObj(DoPrimSurf(DcSphere));

   /***** delete some elements by various delete commands
     and check the group size *****/

   DgDelEleRange(5,6);
   if(DgInqSize(undisplay_grp1) != 12)
	 ExitWithMessage("Incorrect group size", __LINE__);
   if(DgInqElePtr() != 5)
	 BadElePtrMessage(__LINE__);
	/*-----------------------------------------------------------
		0       DoLabel(0)
		1       Translate(x)
		2       Label(150)
		3       Translate(y)
		4       Label(150)
			(Rotate(x))--deleted
			(Rotate(y))--deleted
           -->	5	Label(200)
		6       Scale(0.5)
		7	Label(210)
		8	Label(220)
		9	label(200)
		10	DiffuseColor(green)
		11      PrimitiveSurface(Sphere)
		12
	------------------------------------------------------------*/
   DgDelEleRange(10,15);
   if(DgInqSize(undisplay_grp1) != 10)
	 ExitWithMessage("Incorrect group size", __LINE__);
   if(DgInqElePtr() != 10)
	 BadElePtrMessage(__LINE__);
	/*-----------------------------------------------------------
		0       DoLabel(0)
		1       Translate(x)
		2       Label(150)
		3       Translate(y)
		4       Label(150)
           	5	Label(200)
		6       Scale(0.5)
		7	Label(210)
		8	Label(220)
		9	label(200)
           -->	10	(DiffuseColor(green))--deleted
		        (PrimitiveSurface(Sphere))--deleted
	------------------------------------------------------------*/
   DgSetElePtr(0,DcBeginning);
   printf("\nvvv should cause 'unable to find second label' error: vvv\n");
   if(DcFalse != DgDelEleBetweenLabels(150,175))
	 ExitWithMessage("incorrect value from DgDelEleBetweenLabels",
			 __LINE__);
   printf(  "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
   if(DcTrue != DgDelEleBetweenLabels(150,200))
	 ExitWithMessage("incorrect value from DgDelEleBetweenLabels",
			 __LINE__);
   if(DgInqSize(undisplay_grp1) != 8)
	 ExitWithMessage("Incorrect group size", __LINE__);
   if(DgInqElePtr() != 3)
	 BadElePtrMessage(__LINE__);
	/*-----------------------------------------------------------
		0       DoLabel(0)
		1       Translate(x)
		2       Label(150)
		        (Translate(y))--deleted
		        (Label(150))--deleted
            --> 3	Label(200)
		4       Scale(0.5)
		5	Label(210)
		6	Label(220)
		7	label(200)
		8
	------------------------------------------------------------*/
   DgAddObj(DoLabel(250));
   DgSetElePtr(0,DcBeginning);
	/*-----------------------------------------------------------
	        0       DoLabel(0)
		1       Translate(x)
		2       Label(150)
		3	Label(250)
            	4	Label(200)
		5       Scale(0.5)
		6	Label(210)
	    -->	7	Label(220)
		8	label(200)
		9
	------------------------------------------------------------*/

   /***** delete the elements between two labels and
     check the group size. *****/
   DgDelEleBetweenLabels(250, 220);
   if(DgInqSize(undisplay_grp1) != 6)
	 ExitWithMessage("Incorrect group size", __LINE__);
   if(DgInqElePtr() != 4)
	 BadElePtrMessage(__LINE__);
	/*-----------------------------------------------------------
	        0       DoLabel(0)
		1       Translate(x)
		2       Label(150)
		3	Label(250)
            		Label(200)---------|
			Scale(0.5)    deleted
			Label(210)---------|
	    -->	4	Label(220)
		5	label(200)
		6
	------------------------------------------------------------*/
   DgEmpty(undisplay_grp1);
   if(DgInqSize(undisplay_grp1) != 0)
	 ExitWithMessage("Incorrect group size", __LINE__);
   DgClose();

   if(!(undisplay_grp2 = DoGroup(DcTrue)))
	 ExitWithMessage("can't create group", __LINE__);

   DgAddObj(DoLabel(300));
   DgAddObj(DoScale(.5,.5,.5));
   DgAddObj(DoLabel(350));
   DgAddObj(DoDiffuseColor(DcRGB, green));
   DgAddObj(DoPrimSurf(DcSphere));
   DgAddObj(undisplay_grp1);
   DgClose();
		
   DgOpen(undisplay_grp1,DcTrue);
   DgAddObj(DoLabel(400));
   DgAddObj(DoScale(.5,.5,.5));
   DgAddObj(DoLabel(450));
   DgAddObj(undisplay_grp2);
   DgClose();

   if(DgCheck(undisplay_grp1) != DcGroupBad)
	 ExitWithMessage("wrong value from DgCheck", __LINE__);

   /***** a second set of tests for DgSetElePtrRelLabel()
     to prove that an old bug is fixed. The bug occured
     when the label was the first element in a group. *****/
   object4 = DoGroup(DcTrue);
   DgAddObj(DoLabel(1));
   DgAddObj(DoLabel(2));
   DgSetElePtr(0,DcBeginning);
   if(DcTrue != DgSetElePtrRelLabel(1,1))
	 ExitWithMessage("Incorrect.g value from DgSetElePtrRelLabel",
			 __LINE__);
   if(DgInqElePtr() != 1)
	 BadElePtrMessage(__LINE__);
   /***** remove the currently open group (leaving no open group) *****/
   DsReleaseObj(object4);

   /***** now purposely generate some minor group editing errors. *****/
   printf("\nvvv should cause 'a group is not currently open' error: vvv\n");
   DgSetElePtrRelLabel(20, 1);
   printf(  "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
   printf("\nvvv should cause 'invalid or deleted object' error: vvv\n");
   DgAddObjToGroup(DoGroup(DcFalse), (DtObject)200000);
   printf(  "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

   /* test open group nesting */
   grp1 = DoGroup(DcTrue);
      if (grp1 != DgInqOpen())
	    ExitWithMessage("Incorrect open group from DgInqOpen",
			    __LINE__);
	 
      DgAddObj(DoLabel(10));
      grp2 = DoGroup(DcTrue);
      if (grp2 != DgInqOpen())
	    ExitWithMessage("Incorrect open group from DgInqOpen",
			    __LINE__);
	 DgAddObj(DoLabel(20));
	 DgAddObj(DoLabel(21));
      DgAddObj(DgClose());
      if (grp1 != DgInqOpen()) 
	    ExitWithMessage("Incorrect open group from DgInqOpen",
			    __LINE__);
      DgAddObj(DoLabel(11));
      grp3 = DoGroup(DcTrue);
         if (grp3 != DgInqOpen()) 
	       ExitWithMessage("Incorrect open group from DgInqOpen",
			       __LINE__);
	 DgAddObj(DoLabel(20));
	 grp4 = DoGroup(DcTrue); /* point here <--- */
            if (grp4 != DgInqOpen()) 
		  ExitWithMessage("Incorrect open group from DgInqOpen",
				  __LINE__);
	    DgAddObj(DoLabel(30));
	 DgAddObj(DgClose());
         if (grp3 != DgInqOpen()) 
		  ExitWithMessage("Incorrect open group from DgInqOpen",
				  __LINE__);
	 DgAddObj(DoLabel(21));
	 DgSetElePtr(0, DcBeginning);/* reset ele ptr */
	 DgSetElePtrRelLabel(20, 1); /* point to last group */
      DgAddObj(DgClose());
      if (grp1 != DgInqOpen()) 
	    ExitWithMessage("Incorrect open group from DgInqOpen",
			    __LINE__);
      DgAddObj(DoLabel(12));
   grp = DgClose();
   if (DcNullObject != DgInqOpen()) 
	 ExitWithMessage("Incorrect open group from DgInqOpen",
			 __LINE__);
   printf("\ngroup before: \n");
   DsPrintObj(grp);
   printf("\nreplacing this element: \n");
   DsPrintObj(DgInqObjAtPos(grp3, 0, DcCurrent));
   DgOpen(grp3,DcFalse);
   if (grp3 != DgInqOpen()) 
	 ExitWithMessage("Incorrect open group from DgInqOpen",
			 __LINE__);
      DgReplaceObj(DoLabel(3));
   DgClose();
   if (DcNullObject != DgInqOpen()) 
	 ExitWithMessage("Incorrect open group from DgInqOpen",
			 __LINE__);
   printf("\ngroup after: \n");
   DsPrintObj(grp);
   printf("\n\n\nALL GROUP TESTS PASSED\n");
   DsTerminate();
}
/*
  ======================================================================
 */
ExitWithMessage(msg, lineno)
char *msg;
int lineno;
{
   printf("*** test failed at line %d *** '%s'\n", lineno, msg);
   /*
     printf("Exiting at line %d in file %s\n", line, __FILE__);
     exit(1);
     */
}
/*
  ======================================================================
 */
BadElePtrMessage(lineno)
int lineno;
{
   char str[100];
   DtObject DgInqOpen();

   if(DcNullObject == DgInqOpen()) {
      printf("--- internal error: no group curently open !!! ---\n");
      exit(1);
   }
   sprintf(str, "incorrect element pointer=%d",
	   DgInqElePtr());
   ExitWithMessage(str, lineno);
}
/*
  ======================================================================
 */
