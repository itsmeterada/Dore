/* ************************************************************************
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
#include <sys/types.h>

#ifdef sun
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include <sunwindow/notify.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

DtInt		backfacecullswitch	= 0;
DtInt		interptype		= 1;
DtInt		reptype			= 2;
DtReal		rotatevalue[3]		= {0.,0.,0.};
DtReal		scalevalue[3]		= {1.,1.,1.};
DtInt		specularswitch		= 0;
DtReal		translatevalue[3]	= { 0., 0., 0.};
DtReal		subdivlevel		= .08;
DtInt		subdivchangeflag	= 1;
DtFlag		clearflag		= DcTrue;
DtInt		shadetype		= 1;
DtReal		ambientintensity	= 0.;
DtReal		diffuseintensity	= 1.;
DtFlag		depthcueswitch		= DcFalse;
DtReal		stereo_eyeseparation	= .08;
DtReal		stereo_distance		= 10.0;

#ifdef sun
Frame		sun_frame;
Canvas		sun_canvas;
int		pid;
#endif

DtReal		gray_scale_map[256*3];
DtReal		color_scale_map[256*3];
DtInt		shade_range[2];

int main (argc, argv)
    int   argc;
    char *argv[];
{
   char		objectgroupname[200];
   char		cameragroupname[200];
   DtColorRGB	backgroundcolor;
   DtObject	camera_group;
   DtObject	object_group;
   char		filename[200];
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
   DtVisualType visualtype;
   DtSwitch	readvaluesflag;
   DtSwitch	remoteswitch;

   for (i=0; i<256; i++) {
      gray_scale_map[i*3] = gray_scale_map[i*3+1] = 
	    gray_scale_map[i*3+2] = (DtReal)(i / 255.);
      color_scale_map[i*3+0] = (DtReal)(i>>5) / 7.;
      color_scale_map[i*3+1] = (DtReal)((i>>2) & 0x07) / 7.;
      color_scale_map[i*3+2] = (DtReal)(i & 0x03) / 3.;
   }
   shade_range[0] = 0;
   shade_range[1] = 255;

   dor_prsarg_get_keyword_string(argc,argv,"-f","test.dou",filename);
   dor_prsarg_get_keyword_string(argc,argv,"-c","cameragroup",cameragroupname);
   dor_prsarg_get_keyword_string(argc,argv,"-g","objectgroup",objectgroupname);

   dor_prsarg_get_keyword_switch(argc,argv,"-readvalues",0,&readvaluesflag);

   dor_prsarg_get_keyword_int(argc,argv,"-width",128,&width);
   dor_prsarg_get_keyword_int(argc,argv,"-height",128,&height);

#ifdef sun
   dor_prsarg_get_keyword_string(argc,argv,"-visualtype","DcPseudoColor",tmpstring);
#else
   dor_prsarg_get_keyword_string(argc,argv,"-visualtype","DcDirectColor",tmpstring);
#endif
   if (strcmp(tmpstring, "DcPseudoColor") == 0) {
      visualtype = DcPseudoColor;
   } else {
      visualtype = DcDirectColor;
   }

   dor_prsarg_get_keyword_string(argc,argv,"-shademode","DcComponent",tmpstring);
   if (strcmp(tmpstring, "DcComponent") == 0) {
      shademode = DcComponent;
   } else if (strcmp (tmpstring, "DcRange") == 0) {
      shademode = DcRange;
   } else {
      printf ("bad shade mode: '%s'\n", argstring);
      exit (1);
   }

#ifdef sun
   dor_prsarg_get_keyword_string(argc,argv,"-dt","sunview",devicetype);
#else
   dor_prsarg_get_keyword_string(argc,argv,"-dt","ardentx11",devicetype);
#endif

   dor_prsarg_get_keyword_int(argc,argv,"-procs",0,&processors);

   dor_prsarg_get_keyword_switch(argc,argv,"-remote",0,&remoteswitch);

   dor_prsarg_check_bad_arguments(argc,argv);

   DsInitialize (processors);

   if (remoteswitch) {
      /* This is an internal call to enable remote raytracing 
	 It is not visible at the user level
	 It is only for testing
       */
      dor_glbrnd_set_remote_switch(DcOn);
   }


   backgroundcolor[0] = .3;
   backgroundcolor[1] = .3;
   backgroundcolor[2] = .3;

   if (strcmp(devicetype,"rasterfile") == 0){
      sprintf(argstring,
	      " -filename test.img -width %d -height %d",
	      width,height);
   }
#ifdef sun
   else if (strncmp (devicetype, "sunview", 7) == 0) {
      
      if (((sun_frame = window_create (NULL, FRAME, 0)) == NULL) ||
	  ((sun_canvas = window_create (sun_frame, CANVAS,
					CANVAS_RETAINED, FALSE,
					CANVAS_WIDTH, width,
					CANVAS_HEIGHT, height,
					WIN_WIDTH, width,
					WIN_HEIGHT, height,
					0)) == NULL)) {
	 printf ("can't create sunview window\n");
	 exit (1);
      } 
      window_fit (sun_frame);

      sprintf (argstring,
	       " -canvas %d -width %d -height %d",sun_canvas,width,height);
   }
#else
   else if (strcmp (devicetype, "ardentx11") == 0) {
      sprintf (argstring,
	       " -singlebuffered -geometry %dx%d",
	       width,height);
      if (visualtype == DcPseudoColor) {
	 sprintf (&(argstring[strlen(argstring)]), 
		  " -visualtype DcPseudoColor");
      }
   }
#endif

   if (!(device = DoDevice(devicetype,argstring))) {
      printf("can't create device:  type = '%s',  argstring = '%s'\n",
	     devicetype,argstring);
      exit(1);
   }

   if (DdInqVisualType(device) == DcPseudoColor) {
      switch (shademode) {
      case DcComponent:
	 DdSetShadeMode(device, DcComponent);
	 DdSetColorEntries(device, DcRGB, 0, 256, color_scale_map);
	 break;
      case DcRange:
	 DdSetShadeMode(device, DcRange);
	 DdSetShadeRanges(device, 0, 1, shade_range);
	 DdSetColorEntries(device, DcRGB, 0, 256, gray_scale_map);
	 break;
      }
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

   makeobjects();

   create_callback_functions();

   dor_scenefile_read(filename);

   object_group = DsInqObjViaString (objectgroupname, DcTypeGroup);
   DgAddObjToGroup(DvInqDisplayGroup(view),object_group);

   camera_group = DsInqObjViaString (cameragroupname, DcTypeGroup);
   DgAddObjToGroup(DvInqDefinitionGroup(view),camera_group);

   printf("Computing raytraced image.\n");

   if (readvaluesflag) {
      read_values();
   }

#ifdef sun
   if (strncmp (devicetype, "sunview", 7) == 0) {
      if (pid = fork()) {
	 Notify_value my_handler();
	 /*
	  * parent context
	  */
	 notify_set_wait3_func (device, my_handler, pid);

	 window_main_loop (sun_frame);
      } else if (pid == 0) {
	 /*
	  * child context
	  */
	 DdUpdate(device);
	 exit (0);
      }
   } else {
      DdUpdate(device);
   }
#else 
   DdUpdate(device);
#endif

#ifndef sun
   if (strcmp(devicetype,"ardentx11") == 0){
      getchar();
   }
#endif

   /*  Releasing the device object will cause it to be deleted, and the
       appropriate dgi close will be done for the device as a result.  This
       should result in correct termination of output to the device (buffer
       flushing).  */
   
   DsReleaseObj(device);

   DsTerminate ();
}

/* ====================================================================== */

void read_values (void)
{
   FILE *valuesfile;

   valuesfile = dor_io_file_open_read("values.dat");

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
   fscanf(valuesfile,"stereo_eyeseparation %lf\n", &stereo_eyeseparation);
   fscanf(valuesfile,"stereo_distance %lf\n", &stereo_distance);

   dor_io_file_close(valuesfile);
}

/* ====================================================================== */

void create_callback_functions (void)
{
   DtObject callbackobject;
   extern light_ambient_callback();
   extern light_infinite_callback();
   extern ambient_intensity_callback();
   extern diffuse_intensity_callback();
   extern stereo_callback();
   extern backface_callback();
   extern interptype_callback();
   extern reptype_callback();
   extern rotate_callback();
   extern scale_callback();
   extern specular_callback();
   extern subdivspec_callback();
   extern translate_callback();

   callbackobject = DoCallback(light_ambient_callback, DcNullObject);
   DsSetObjNameString (callbackobject,"ambientlight",DcFalse);

   callbackobject = DoCallback(light_infinite_callback, DcNullObject);
   DsSetObjNameString (callbackobject,"infinitelight",DcFalse);

   callbackobject = DoCallback(ambient_intensity_callback, DcNullObject);
   DsSetObjNameString (callbackobject,"ambientintensity",DcFalse);

   callbackobject = DoCallback(diffuse_intensity_callback, DcNullObject);
   DsSetObjNameString (callbackobject,"diffuseintensity",DcFalse);

   callbackobject = DoCallback(stereo_callback, DcNullObject);
   DsSetObjNameString (callbackobject,"stereo",DcFalse);

   callbackobject = DoCallback(backface_callback,DcNullObject);
   DsSetObjNameString (callbackobject,"backface",DcFalse);

   callbackobject = DoCallback(interptype_callback,DcNullObject);
   DsSetObjNameString (callbackobject,"interptype",DcFalse);

   callbackobject = DoCallback(reptype_callback,DcNullObject);
   DsSetObjNameString (callbackobject,"reptype",DcFalse);

   callbackobject = DoCallback(rotate_callback,DcNullObject);
   DsSetObjNameString (callbackobject,"rotate",DcFalse);

   callbackobject = DoCallback(scale_callback,DcNullObject);
   DsSetObjNameString (callbackobject,"scale",DcFalse);

   callbackobject = DoCallback(specular_callback,DcNullObject);
   DsSetObjNameString (callbackobject,"specular",DcFalse);

   callbackobject = DoCallback(subdivspec_callback,DcNullObject);
   DsSetObjNameString (callbackobject,"subdivspec",DcFalse);

   callbackobject = DoCallback(translate_callback,DcNullObject);
   DsSetObjNameString (callbackobject,"translate",DcFalse);
}
/*
  ======================================================================
 */
light_ambient_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoLightType(DcLightAmbient) );
}
/*
  ======================================================================
 */
light_infinite_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoLightType(DcLightInfinite) );
}
/*
  ======================================================================
 */
ambient_intensity_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoLightIntens(ambientintensity) );
}
/*
  ======================================================================
 */
diffuse_intensity_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoLightIntens(diffuseintensity) );
}
/*
  ======================================================================
 */
stereo_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoStereo(stereo_eyeseparation, stereo_distance) );
}
/*
  ======================================================================
 */
interptype_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoInterpType(interptype) );
}
/*
  ======================================================================
 */
reptype_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoRepType(reptype) );
}
/*
  ======================================================================
 */
specular_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoSpecularSwitch(specularswitch) );
}
/*
  ======================================================================
 */
backface_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoBackfaceCullSwitch(backfacecullswitch) );
}
/*
  ======================================================================
 */
rotate_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoRotate(0,rotatevalue[0]) );
   DsExecuteObj( DoRotate(1,rotatevalue[1]) );
   DsExecuteObj( DoRotate(2,rotatevalue[2]) );
}
/*
  ======================================================================
 */
scale_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoScale(scalevalue[0],scalevalue[1],scalevalue[2]));
}
/*
  ======================================================================
 */
subdivspec_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   if (subdivchangeflag) {
      DsExecuteObj( DoSubDivSpec(2,&subdivlevel) );

      subdivchangeflag = 0;
   }
}
/*
  ======================================================================
 */
translate_callback(/* dataobject */)
     /* DtObject dataobject; */
{
   DsExecuteObj( DoTranslate(translatevalue[0],translatevalue[1],
			     translatevalue[2]));
}
/*
  ======================================================================
 */
#ifdef sun
Notify_value
my_handler (me, pid, status, rusage)
     Notify_client me;
     int pid;
     union wait *status;
     struct rusage *rusage;
{
   if (WIFEXITED(*status)) {
      if (status->w_retcode != 0) {
	 printf ("pid: %d exited with %d %s\n", pid, status->w_retcode,
		 status->w_coredump ? "(Core Dumped)" : " ");
      }
      printf ("process %d\n\tuser: %5d.%06d sec\n\tsys:  %5d.%06d sec\n",
	      pid,
	      rusage->ru_utime.tv_sec, rusage->ru_utime.tv_usec,
	      rusage->ru_stime.tv_sec, rusage->ru_stime.tv_usec);
      return (NOTIFY_DONE);
   }

   return (NOTIFY_IGNORED);
}
#endif
/*
  ======================================================================
 */

/* end %M% */
