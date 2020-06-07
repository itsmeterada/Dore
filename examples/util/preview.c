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
 
/**************************************************************************

			VIEW PREVIEWER
			by Daniel Green, 6/88

The function "view_preview()" is a tool which often helps answer
the common question "I think I'm doing everything correctly, so how
come I don't get any picture?".

The previewer takes a view handle, and creates a completely new dynamic
device showing the given scene in a reasonable perspective with the
user's cameras and lights shown as simple models within the new scene.
This way the user can see if perhaps the cameras and lights are not
oriented correctly.

Several Dore attributes that can often be used incorrectly resulting
in the "no picture" problem are forced (via DoExecSet) to take
reasonable values. Those attributes are:
	DcTypeDiffuseSwitch
	DcTypeDiffuseColor
	DcTypeDiffuseIntens
	DcTypeRepType
	DcTypeInterpType
	DcTypeBackfaceCullable
	DcTypeMarkerScale
	DcTypeExecSet
	DcTypeNameSet
	DcTypeFilter
	DcTypeInvisibilitySwitch
	DcTypeClipSwitch
	DcTypeSubDivSpec
	DcTypeTextExpansionFactor
	DcTypeTextHeight

If the user doesn't want the previewer to disable one or more of
these attributes, then they can specify them by those constants
in the array passed to the previewer. If all are ok to be
disabled (ie, as much help as posible) then simply call the function
like this:   view_preview(view, 0, DcNullPtr)

**************************************************************************/


#include "dore.h"
#include "dore_util.h"




void view_preview (
    DtObject user_view,
    DtInt list_size,
    DtInt save_list[])
{
	DtObject new_device, new_view, new_definition_group, new_display_group;
	DtObject MakeNewDefinitionGroup(), MakeNewDisplayGroup();

	MakeNewView(&new_device, &new_view);
	new_definition_group = MakeNewDefinitionGroup();
	new_display_group = MakeNewDisplayGroup(user_view,list_size,save_list);
	if(new_display_group == DcNullObject)
		{
		printf("***** view_preview: nothing to see in view *****\n");
		DsReleaseObj(new_device);
		return;
		}
	DgAddObjToGroup(DvInqDefinitionGroup(new_view), new_definition_group);
	DgAddObjToGroup(DvInqDisplayGroup(new_view), new_display_group);
	DdUpdate(new_device); 
	getchar(); /* let user look at it before destroying it. */
	DsReleaseObj(new_device);
}



static MakeNewView (
    DtObject *device,
    DtObject *view)
{
	DtObject frame;
	DtVolume volume;

	if(!(*device = DoDevice("display",
			"-singlebuffered -geometry =500x500+759+0")))
		{
		printf("view_preview: can't create device\n");
		exit(1);
		}
	DdInqExtent(*device,&volume);
	if (!(frame = DoFrame()))
		{
		printf("view_preview: can't create frame\n");
		exit(1);
		}
	DdSetFrame(*device,frame);
	DfSetBoundary(frame,&volume);
	if (!(*view = DoView()))
		{
		printf("view_preview: can't create view\n");
		exit(1);
		}
	DgAddObjToGroup(DfInqViewGroup(frame), *view);
	DvSetBoundary(*view,&volume);
}



static DtObject MakeNewDefinitionGroup (void)
{
	static DtPoint3
		origin = {0. , 0., 0.},
		from_x = {10., 0., 0.},
		from_z = {0. , 0., 10.};
	static DtVector3
		y_up = {0.,1.,0.};

	DoGroup(DcTrue);
		DgAddObj(DoParallel(1.5,-1.,-400.));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, from_z, y_up));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, from_x, y_up));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	return(DgClose());
}



static DtObject MakeNewDisplayGroup (
    DtObject view,
    DtInt list_size,
    DtInt save_list[])
{
	DtInt i, ok_count, ok_list[15];
	DtObject user_definition_objs, user_display_objs,
		view_objs, camera_obj, light_obj, temp_group;
	DtVolume bv;
	static DtReal origin[] = {0.,0.,0.};
	DtReal view_bredth;
	char *malloc();
	static DtInt control_list[15] =
		{ DcTypeDiffuseSwitch,
		  DcTypeDiffuseColor,
		  DcTypeDiffuseIntens,
		  DcTypeRepType,
		  DcTypeInterpType,
		  DcTypeBackfaceCullable,
		  DcTypeMarkerScale,
		  DcTypeExecSet,
		  DcTypeNameSet,
		  DcTypeFilter,
		  DcTypeInvisibilitySwitch,
		  DcTypeClipSwitch,
		  DcTypeSubDivSpec,
		  DcTypeTextExpansionFactor,
		  DcTypeTextHeight };


	/* initialize the definition of the camera and light objects
	   to be dimentionless points for purposes of computing the
	   bounding volume of the scene INCLUDING the cameras and
	   lights. Later give them their true representations scaled
	   to a comfortable size. */
	camera_obj = DoGroup(DcTrue);
		DgAddObj(DoPointList(DcRGB, DcLoc, 1, origin));
	DgClose();
	light_obj = DoGroup(DcTrue);
		DgAddObj(DoPointList(DcRGB, DcLoc, 1, origin));
	DgClose();

	/* create a clone of the user's definition group. */
	user_definition_objs = copy_object(DvInqDefinitionGroup(view));

	/* replace the cameras and lights with point primitives */
	if(! replace_type_with(user_definition_objs, DcTypeCamera, camera_obj))
		{
		printf("view_preview: No cameras in view definition group\n");
		}
	if(! replace_type_with(user_definition_objs, DcTypeLight,  light_obj))
		{
		printf("view_preview: No lights in view definition group\n");
		}

	/* create a clone of the user's display group. */
	user_display_objs = copy_object(DvInqDisplayGroup(view));

	/* create a group with both new groups in it in order to
	   compute the bounding volume. */
	temp_group = DoGroup(DcTrue);
		DgAddObj(user_definition_objs);
		DgAddObj(user_display_objs);
	DgClose();

	/* get bounding volume for entire group. ie: the user display
	   objects as well as the cameras and lights (currently repre-
	   sented as points). */
	DsCompBoundingVol(&bv, temp_group);

	/* find largest dimension of bounding volume to be able to 
	   scale camera and light primitives to reasonable sizes
	   and to position the volume for viewing. */

	for(view_bredth=0.0, i=0; i<3; i++)
		view_bredth = UTIL_MAX(view_bredth, bv.fur[i] - bv.bll[i]);

	if(view_bredth <= 0.) /* nothing to view */
		return(DcNullObject);

	/* redefine camera point primitive as model of actual camera. */
	RedefineCamera(camera_obj, view_bredth * 0.05);

	/* redefine light point primitive as model of actual light. */
	RedefineLight(light_obj, view_bredth * 0.05);

	/* compute list of those object types which this program wants to 
	   disable and which the user will allow. */
	for(i=0, ok_count=0; i<15; i++)
		if( ! InList(control_list[i], list_size, save_list))
			ok_list[ok_count++] = control_list[i];
	
	/* put both new groups into final display group */
	view_objs = DoGroup(DcTrue);
		DgAddObj(DoSurfaceShade(DcShaderConstant));
		DgAddObj(DoRotate(DcYAxis, DTOR(-20)));
		DgAddObj(DoRotate(DcXAxis, DTOR(20)));
		DgAddObj(DoScale(1./view_bredth,1./view_bredth,1./view_bredth));
		DgAddObj(DoTranslate(  /* center the volume */
			- (bv.bll[0] + (bv.fur[0] - bv.bll[0]) / 2),
			- (bv.bll[1] + (bv.fur[1] - bv.bll[1]) / 2),
			- (bv.bll[2] + (bv.fur[2] - bv.bll[2]) / 2)));
		DgAddObj(user_definition_objs);
		DgAddObj(DoExecSet(ok_count, ok_list, DcSetDelete));
		DgAddObj(user_display_objs);
	DgClose();

	/* time to take out the garbage. */
	free(ok_list);
	DsReleaseObj(temp_group);

	return(view_objs);
}





int RedefineCamera (
    DtObject camera_obj,
    DtReal scale_factor)
{
	static DtReal red[] = {1.,0.,0.};

	DgEmpty(camera_obj);
	DgOpen(camera_obj, DcFalse);
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoBackfaceCullable(DcOn));
		DgAddObj(DoDiffuseColor(DcRGB, red));
		DgAddObj(DoScale(scale_factor, scale_factor, scale_factor));
		DgAddObj(DoPushMatrix()); /* lens */
			DgAddObj(DoScale(.5,.5,1.));
			DgAddObj(DoTranslate(0.,0.,-1.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* body */
			DgAddObj(DoScale(1.,2.,4.25));
			DgAddObj(DoTranslate(-.5,-.5,0.));
			DgAddObj(DoPrimSurf(DcBox));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* front film reel */
			DgAddObj(DoTranslate(0.,1.,1.));
			DgAddObj(DoRotate(DcYAxis, DTOR(90)));
			DgAddObj(DoTranslate(0.,0.,-.5));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* back film reel */
			DgAddObj(DoTranslate(0.,1.,3.25));
			DgAddObj(DoRotate(DcYAxis, DTOR(90)));
			DgAddObj(DoTranslate(0.,0.,-.5));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* tripod center post */
			DgAddObj(DoTranslate(0.,-1.,2.));
			DgAddObj(DoRotate(DcXAxis, DTOR(90)));
			DgAddObj(DoScale(.25,.25,2.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* front tripod leg */
			DgAddObj(DoTranslate(0.,-3.,2.));
			DgAddObj(DoRotate(DcXAxis, DTOR(120)));
			DgAddObj(DoScale(.25,.25,6.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* right tripod leg */
			DgAddObj(DoTranslate(0.,-3.,2.));
			DgAddObj(DoRotate(DcYAxis, DTOR(120)));
			DgAddObj(DoRotate(DcXAxis, DTOR(120)));
			DgAddObj(DoScale(.25,.25,6.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* left tripod leg */
			DgAddObj(DoTranslate(0.,-3.,2.));
			DgAddObj(DoRotate(DcYAxis, DTOR(-120)));
			DgAddObj(DoRotate(DcXAxis, DTOR(120)));
			DgAddObj(DoScale(.25,.25,6.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
	DgClose();
}





int RedefineLight (
    DtObject light_obj,
    DtReal scale_factor)
{
	static DtReal blue[] = {0.,0.,1.};

	DgEmpty(light_obj);
	DgOpen(light_obj, DcFalse);
		DgAddObj(DoRepType(DcSurface));
		DgAddObj(DoBackfaceCullable(DcOn));
		DgAddObj(DoDiffuseColor(DcRGB, blue));
		DgAddObj(DoScale(scale_factor, scale_factor, scale_factor));
		DgAddObj(DoPushMatrix()); /* lamp shade */
			DgAddObj(DoScale(1.,1.,3.));
			DgAddObj(DoRotate(DcYAxis, DTOR(180)));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* tripod center post */
			DgAddObj(DoRotate(DcXAxis, DTOR(90)));
			DgAddObj(DoScale(.25,.25,8.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* front tripod leg */
			DgAddObj(DoTranslate(0.,-8.,0.));
			DgAddObj(DoRotate(DcXAxis, DTOR(120)));
			DgAddObj(DoScale(.25,.25,6.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* right tripod leg */
			DgAddObj(DoTranslate(0.,-8.,0.));
			DgAddObj(DoRotate(DcYAxis, DTOR(120)));
			DgAddObj(DoRotate(DcXAxis, DTOR(120)));
			DgAddObj(DoScale(.25,.25,6.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix()); /* left tripod leg */
			DgAddObj(DoTranslate(0.,-8.,0.));
			DgAddObj(DoRotate(DcYAxis, DTOR(-120)));
			DgAddObj(DoRotate(DcXAxis, DTOR(120)));
			DgAddObj(DoScale(.25,.25,6.));
			DgAddObj(DoPrimSurf(DcCylinder));
		DgAddObj(DoPopMatrix());
	DgClose();
}




int InList (
    DtInt item,
    DtInt list_size,
    DtInt list[])
{
	int i;

	for(i=0; i<list_size; i++)
		if(list[i] == item)
			return(1);
	return(0);
} /* end of preview.c */
