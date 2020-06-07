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
 
/*  ======================================================================
    This file contains the main program for the Production Renderer Tests
    outlined in the Readme.  Also included in this file are platform
    specific routines and interaction processing routines.

    The display objects and the camera and light objects are contained
    in the files prims.c, temple.c, test.c, or tree.c.  The makefiles
    provided with Portable Dore allow you to define your own display and
    studio objects in a file called myobjects.c.

    See the Readme file and Chapter 16 of the Dore Developer's Guide 
    for more information.
    ====================================================================== */

#include <dore/dore.h>
#include <stdio.h>
#include <sys/types.h>

#if defined(sun) && !defined(SVR4)
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include <sunwindow/notify.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

Frame		sun_frame;
Canvas		sun_canvas;
int		pid;
#endif

#ifdef sgi
#include <gl.h>
#endif

DtVolume	volume;
DtColorRGB	backgroundcolor;
DtObject	studio_group;
DtObject	object_group;
char		controlchar;

DtSwitch		backfacecullswitch	= DcOff;
DtInt			interptype		= 1;
DtInt			reptype			= 2;
DtReal			rotatevalue[3]		= {0.,0.,0.};
DtReal			scalevalue[3]		= {1.,1.,1.};
DtSwitch		specularswitch		= DcOff;
DtReal			translatevalue[3]	= { 0., 0., 0.};
DtReal			subdivlevel		= .08;
DtInt			subdivchangeflag	= 1;
DtFlag			clearflag		= DcTrue;
DtInt			shadetype		= 1;
DtReal			ambientintensity	= 0.;
DtReal			diffuseintensity	= 1.;
DtSwitch		depthcueswitch		= DcOff;
DtSwitch 		shadowswitch		= DcOff;
DtSwitch 		reflectionswitch	= DcOff;
DtSwitch 		transpswitch		= DcOff;
DtReal			transpintensity		= 0.;
DtExtendMode            umode                   = DcExtendNone;
DtExtendMode            vmode                   = DcExtendNone;
DtReal                  uscale                  = 1.;
DtReal                  vscale                  = 1.;
DtReal                  utranslate              = 0.;
DtReal                  vtranslate              = 0.;
DtTextureAntiAliasMode  aa_mode                 = DcTextureAANone;
DtTextureOperator       tex_op                  = DcTextureReplace;
DtVisualType    	visualtype              = DcTrueColor;

DtReal		gray_scale_map[256*3];
DtReal		color_scale_map[256*3];
DtInt		shade_range[2];

/* ====================================================================== */

void  main(int  argc, char **argv)
{
   char		file[200];
   char		devicetype[200];
   char		argstring[200];
   char		tmpstring[200];
   DtShadeMode	shademode;
   DtObject	device;
   DtObject	frame;
   DtObject	view;
   DtInt	processors;
   DtInt	width;
   DtInt	height;
   DtVolume	rayvolume;
   DtInt	i;
   DtObject	makeobject();
   DtObject	makestudio();
   DtSwitch	readvaluesswitch;

   for (i=0; i<256; i++) {
      gray_scale_map[i*3] = gray_scale_map[i*3+1] = 
	    gray_scale_map[i*3+2] = (DtReal)(i / 255.);
      color_scale_map[i*3+0] = (DtReal)(i>>5) / 7.;
      color_scale_map[i*3+1] = (DtReal)((i>>2) & 0x07) / 7.;
      color_scale_map[i*3+2] = (DtReal)(i & 0x03) / 3.;
   }
   shade_range[0] = 0;
   shade_range[1] = 255;


   /* Parse all command line arguments. */
   prsarg_get_keyword_int(argc,argv,"-width",128,&width);
   prsarg_get_keyword_int(argc,argv,"-height",128,&height);

   prsarg_get_keyword_switch(argc, argv, "-shadows", DcOff, &shadowswitch);
   prsarg_get_keyword_switch(argc, argv, "-reflections", DcOff, &reflectionswitch);
   prsarg_get_keyword_switch(argc, argv, "-transp", DcOff, &transpswitch);
   prsarg_get_keyword_real(argc, argv, "-transpintens", .7, &transpintensity);
   prsarg_get_keyword_string(argc,argv,"-dt","rasterfile",devicetype);
   prsarg_get_keyword_int(argc,argv,"-procs",0,&processors);
   prsarg_get_keyword_switch(argc,argv,"-read",DcOff,&readvaluesswitch);
   prsarg_get_keyword_string(argc,argv,"-file","values.dat",file);
   prsarg_get_keyword_string(argc,argv,"-df"," ",argstring);

   prsarg_check_bad_arguments(argc,argv);

   DsInitialize(processors);

   backgroundcolor[0] = .6;
   backgroundcolor[1] = .5;
   backgroundcolor[2] = .5;

   if (strcmp(devicetype,"rasterfile") == 0){
      sprintf(argstring,
	      "-filename test.img -width %d -height %d",
	      width,height);
   }

   if (!(device = DoDevice(devicetype,argstring))) {
      printf("can't create device:  device type = '%s',argstring = '%s'\n",
	     devicetype,argstring);
      exit(1);
   }

   rayvolume.bll[0] = 0;
   rayvolume.bll[1] = 0;
   rayvolume.bll[2] = 0;

   rayvolume.fur[0] = width;
   rayvolume.fur[1] = height;
   rayvolume.fur[2] = 65536.;

   DdSetViewport(device,&rayvolume);

   if (!(frame = DoFrame())) {
      printf("can't create frame\n");
      exit(1);
   }

   DdSetFrame(device,frame);

   DfSetBoundary(frame,&rayvolume);

   if (!(view = DoView())) {
      printf("can't create view\n");
      exit(1);
   }

   DvSetClearFlag(view,DcTrue);
   DvSetBackgroundColor(view,DcRGB,backgroundcolor);

   DvSetRendStyle(view,DcProductionTime);
   
   DgAddObjToGroup(DfInqViewGroup(frame),view);

   DvSetBoundary(view,&rayvolume);

   DvSetUpdateType(view,DcUpdateAll);

   object_group = makeobject();
   DgAddObjToGroup(DvInqDisplayGroup(view),object_group);

   studio_group = makestudio();
   DgAddObjToGroup(DvInqDefinitionGroup(view),studio_group);

   if (readvaluesswitch) {
      read_values(file);
   }

   printf("Computing raytraced image.\n");

   DdUpdate(device);

   fputs ("\nHit return to continue  ", stdout);
   fflush (stdout);
   getchar ();

   /*  Releasing the device object will cause it to be deleted, and the
   //  appropriate dgi close will be done for the device as a result.  This
   //  should result in correct termination of output to the device (buffer
   //  flushing).  */
   
   DsReleaseObj(device);

   DsTerminate();
}

/*
  ======================================================================
 */

read_values(char *file)
{
   FILE *valuesfile;
   FILE *fopen();

   if ((valuesfile = fopen(file,"r")) == NULL) {
      fprintf (stderr, "could not open file %s for reading\n", file);
      perror ("");
      return;
   }

   fscanf(valuesfile,"backfacecullswitch %d\n",&backfacecullswitch);
   fscanf(valuesfile,"interptype %d\n",&interptype);
   fscanf(valuesfile,"reptype %d\n",&reptype);
   fscanf(valuesfile,"rotatevalue %lf %lf %lf\n",
	  &rotatevalue[0],&rotatevalue[1],&rotatevalue[2]);
   fscanf(valuesfile,"scalevalue %lf %lf %lf\n",
	  &scalevalue[0],&scalevalue[1],&scalevalue[2]);
   fscanf(valuesfile,"specularswitch %d\n",&specularswitch);
   fscanf(valuesfile,"translatevalue %lf %lf %lf\n",
	  &translatevalue[0],&translatevalue[1],&translatevalue[2]);

   fscanf(valuesfile,"subdivlevel %lf\n", &subdivlevel);
   subdivchangeflag = 1;
   fscanf(valuesfile,"shadetype %d\n", &shadetype);
   fscanf(valuesfile,"ambientintensity %lf\n", &ambientintensity);
   fscanf(valuesfile,"diffuseintensity %lf\n", &diffuseintensity);
   fscanf(valuesfile,"depthcueswitch %d\n", &depthcueswitch);

   fclose(valuesfile);
}
