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
#include <math.h>
#include "Trnsfrmr.h"

extern DtObject device;
extern DtInt debug;
extern DtObject models[];
extern int nmodels;
extern DtReal model_size;
DtReal origin[3];
DtReal camera[3];
DtReal front_clip;
DtReal back_clip;
DtObject reptypeInline;
DtObject interptypeInline;
DtObject backfaceInline;
DtObject specularInline;
DtObject subdivInline;
DtObject tf_rot_inline;
DtObject tf_tran_inline;
extern rotx_callback();
extern roty_callback();
extern rotz_callback();
DtInt tf_rot, tf_tran;

DtObject model_group;

static DtVolume bounds = { 0.,0.,0.,0.,0.,0.};

/*******************************************************************************
 *************************  m a k e _ m o d e l s  *****************************
 *******************************************************************************
 * Module Name: make_objects
 * Module Type: c
 *     Purpose: Create the displayable object model and place it in a group
 *		object for later recall.  Also, set up the viewing parameters
 *		for the model.
 *   Arguments: argc ==> number of command line arguments passed to the program
 *		argv ==> array of command line argument strings
 * ReturnValue: None
 ******************************************************************************/
make_models(argc, argv)
int argc;
char *argv[];
{
/*******************************************************************************
 * Declare variables.
 ******************************************************************************/
   	DtObject DoGroup();

/*******************************************************************************
 * Call the user written function (geom_spec) to actually generate the model(s).
 ******************************************************************************/
   	if(debug)
	   printf("Bounds in: bll = %f %f %f\n           fur = %f %f %f\n",
      		bounds.bll[0],bounds.bll[1],bounds.bll[2],
      		bounds.fur[0],bounds.fur[1],bounds.fur[2]);

   	geom_spec(&bounds, argc, argv);

   	if(debug)
	   printf("Bounds out: bll = %f %f %f\n            fur = %f %f %f\n",
      		bounds.bll[0],bounds.bll[1],bounds.bll[2],
      		bounds.fur[0],bounds.fur[1],bounds.fur[2]);

/*******************************************************************************
 * Determine the size of the model and set up viewing parameters based on it.
 ******************************************************************************/
   	modelsize();

/*******************************************************************************
 * Place the user-specified model, along with default model attributes, in a
 *  group object for later recall and viewing. 
 ******************************************************************************/
   	make_model_group();

}  /* End of make_model function */



/*******************************************************************************
 *********************  m a k e _ m o d e l _ g r o u p  ***********************
 *******************************************************************************
 * Module Name: make_model_group
 * Module Type: c
 *     Purpose: Define the makeup of the model itself (primitive & appearance
 *		objects) and place them all in one group (object) for easy
 *		reference later.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
make_model_group()
{
/*******************************************************************************
 * Declare and initialize variables.
 ******************************************************************************/
   	extern model_callback();
   	DtObject DoGroup(), DoInLineGroup();
   	static DtReal White[3] = { 1., 1., 1. };

/*******************************************************************************
 * Generate Dore' inline groups.
 ******************************************************************************/
	reptypeInline = DoInLineGroup(DcFalse);
	DgReplaceObjInGroup(reptypeInline,DoRepType(DcSurface));

	interptypeInline = DoInLineGroup(DcFalse);
	DgReplaceObjInGroup(interptypeInline,DoInterpType(DcConstantShade));

	backfaceInline = DoInLineGroup(DcFalse);
	DgReplaceObjInGroup(backfaceInline,DoBackfaceCullSwitch(DcOff));

	specularInline = DoInLineGroup(DcFalse);
	DgReplaceObjInGroup(specularInline,DoSpecularSwitch(DcOff));

	subdivInline = DoInLineGroup(DcFalse);

	tf_rot = MakeTransformer(DcPostConcatenate);
	tf_rot_inline = DoInLineGroup(DcFalse);
	DgReplaceObjInGroup(tf_rot_inline,
		ChangeTransformer(tf_rot,DcRotX,0.,DcPostConcatenate));
	
	tf_tran = MakeTransformer(DcPostConcatenate);
	tf_tran_inline = DoInLineGroup(DcFalse);
	DgReplaceObjInGroup(tf_tran_inline,
		ChangeTransformer(tf_tran,DcTransX,0.,DcPostConcatenate));

/*******************************************************************************
 * Make the model group object by putting together all of the model attributes
 *  and the model/models itself/themselves.
 ******************************************************************************/
   	model_group = DoGroup(DcTrue);

		DgAddObj(DoReflectionSwitch(DcOn));
		DgAddObj(DoShadowSwitch(DcOn));

      		DgAddObj(reptypeInline);
      		DgAddObj(interptypeInline);

      		DgAddObj(backfaceInline);
      		DgAddObj(DoBackfaceCullable(DcOff));

      		DgAddObj(DoSpecularColor(DcRGB,White));
      		DgAddObj(DoSpecularFactor(80.));
      		DgAddObj(DoSpecularIntens(.8));
      		DgAddObj(specularInline);

		DgAddObj(subdivInline);

      		DgAddObj(DoAmbientSwitch(1));
      		DgAddObj(DoDiffuseColor(DcRGB,White));
      		DgAddObj(DoDiffuseIntens(1.));

		DgAddObj(DoCallback(rotx_callback,DcNullObject));
		DgAddObj(DoCallback(roty_callback,DcNullObject));
		DgAddObj(DoCallback(rotz_callback,DcNullObject));
		DgAddObj(DoTranslate(origin[0],origin[1],origin[2]));
		DgAddObj(tf_rot_inline);
		DgAddObj(tf_tran_inline);

		if (debug) printf("in make_model_group; nmodels = %d\n",
			nmodels);
		if (nmodels == 1) {
			DgAddObj(models[0]);
		}
		else {
      			DgAddObj(DoCallback(model_callback,DcNullObject));
		}
   	DgClose();

   	DsHoldObj(model_group);

}  /* End of make_model_group function */



/*******************************************************************************
 ****************************  m o d e l s i z e  ******************************
 *******************************************************************************
 * Module Name: modelsize
 * Module Type: c
 *     Purpose: Compute the bounding volume & origin of the model and set
 *		global viewing parameters based on that.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
modelsize() 
{
/*******************************************************************************
 * Declare variables.
 ******************************************************************************/
   	float xsize, ysize, zsize;
   
/*******************************************************************************
 * Determine the size of the model, checking to see whether the bounds of the
 *  model were calculated by the user or need to be computed here.
 ******************************************************************************/
	if (bounds.bll[1]==bounds.fur[1] &&
	    bounds.bll[0]==bounds.fur[0] &&
	    bounds.bll[2]==bounds.fur[2])
		DsCompBoundingVol(&bounds,models[0]);

   	if(debug)
	   printf("Bounds used: bll = %f %f %f\n             fur = %f %f %f\n",
      		bounds.bll[0],bounds.bll[1],bounds.bll[2],
      		bounds.fur[0],bounds.fur[1],bounds.fur[2]);

   	xsize = bounds.fur[0] - bounds.bll[0];
   	ysize = bounds.fur[1] - bounds.bll[1];
   	zsize = bounds.fur[2] - bounds.bll[2];

   	if(xsize > ysize) model_size = xsize;
   		else model_size = ysize;
   	if(zsize > model_size) model_size = zsize;

	if (debug)
		printf("model_size: %f\n", model_size);

/*******************************************************************************
 * Determine the centroid of the model for viewing and rotations.
 ******************************************************************************/
   	origin[0] = -.5*(bounds.fur[0] + bounds.bll[0]);
   	origin[1] = -.5*(bounds.fur[1] + bounds.bll[1]);
   	origin[2] = -.5*(bounds.fur[2] + bounds.bll[2]);

   	if(debug) printf("x,y,z offset = %lf,%lf,%lf\n",
      		origin[0],origin[1],origin[2]);
 
/*******************************************************************************
 * Set the camera and clip-plane parameters based on the model size.  These
 *  will be used in the make_studios function.
 ******************************************************************************/
   	camera[0] = 0.;
   	camera[1] = 0.;
   	camera[2] = 3.0 * model_size;

   	front_clip = -1.0 * model_size;
   	back_clip = -4.0 * model_size;

}  /* End of modelsize function */
