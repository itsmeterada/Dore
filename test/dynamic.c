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
    This file contains the main program for the Dynamic Renderer Tests
    outlined in the Readme.  Also included in this file are platform
    specific routines and interaction processing routines.

    The display objects and the camera and light objects are contained in
    the files prims.c, temple.c, test.c, texture.c, or tree.c.  The makefiles
    provided with Portable Dore allow you to define your own display and 
    studio objects in a file called myobjects.c.

    To use the standard X11 configuration of Dore (stdx)
    compile the "dyn_*" programs with the -Dstdx definition.

    See the Readme file and Chapter 16 of the Dore Developer's Guide for 
    more information.
    ====================================================================== */

#include <dore/dore.h>
#include <stdio.h>
#include <sys/types.h>

#if defined (sun) && !defined (stdx)
#include <suntool/sunview.h>
#include <suntool/canvas.h>
Frame		sun_frame;
Canvas		sun_canvas;
#endif

#ifdef sgi
#include <gl.h>
#include <device.h>
#endif

#if defined (sun) && !defined (stdx)
#define use_SunView
#else
#if defined (sun)
#define stdx
#endif
#endif

#if defined stdx
#define use_X11
#endif
#if defined (titan) && !defined (use_X11)
#define use_X11
#endif
#if defined (titan) && !defined (NO_DGL)
#define use_DGL
#endif
#if defined (titan) && defined(__P3__) && !defined (use_VLL)
#define use_VLL
#endif

#if defined(titan)
int use_XB = 0;
int use_Xtitan = 0;
#endif

#define GRAPHIC_NONE    0x0
#define GRAPHIC_G2      0x1
#define GRAPHIC_G3      0x2
int gr_type = GRAPHIC_NONE;

#ifdef use_X11
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#ifdef titan
#include <X11/XTitan.h>
#endif
Display         *displayPtr;
Window          window;
#endif

#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))

DtSwitch           	backfacecullswitch      = DcOff;
DtInt           	interptype              = 1;
DtInt           	reptype                 = 2;
DtReal          	rotatevalue[3]          = {0.,0.,0.};
DtReal          	scalevalue[3]           = {1.,1.,1.};
DtSwitch           	specularswitch          = DcOff;
DtReal          	translatevalue[3]       = { 0., 0., 0.};
DtReal          	subdivlevel             = .08;
DtFlag          	clearflag               = DcTrue;
DtInt           	shadetype               = 1;
DtReal          	ambientintensity        = .3;
DtReal          	diffuseintensity        = 1.;
DtSwitch          	depthcueswitch          = DcOff;
DtSwitch          	shadowswitch            = DcOff;
DtSwitch          	reflectionswitch        = DcOff;
DtSwitch          	transpswitch            = DcOff;
DtReal          	transpintensity         = .7;
DtExtendMode            umode                   = DcExtendNone;
DtExtendMode            vmode                   = DcExtendNone;
DtReal                  uscale                  = 1.;
DtReal                  vscale                  = 1.;
DtReal                  utranslate              = 0.;
DtReal                  vtranslate              = 0.;
DtTextureAntiAliasMode  aa_mode                 = DcTextureAANone;
DtTextureOperator       tex_op                  = DcTextureReplace;

DtInt		testcase		= 0;
DtFlag		pseudocolor		= DcTrue;
DtSwitch	singlebuffered		= DcOff;
DtVisualType	visualtype		= DcPseudoColor;

DtInt		list_size;
DtInt		index_size;
DtSwitch	reverse;
DtInt		xap;
DtInt		yap;

DtInt          *pick_index;
DtReal         *z_values;
DtReal         *wc_values;
DtReal         *lc_values;
DtObject       *views;
long           *hit_list;

DtRenderStyle   rendstyle               = DcRealTime;
DtInt		num_rendstyles;
char          **rendstyle_name;
DtInt          *rendstyle_id;
DtInt  		current_rendstyle;

DtInt		i;

char            texture_raster_file[200];

char         	tmpstring[200];

DtObject	device;
DtObject	frame;
DtObject	view;

DtReal		gray_scale_map[256*3];
DtReal		color_scale_map[256*3];
DtReal		shade_rng_map[256*3];
DtInt		shade_ranges[2*8];
DtInt		shade_range1[2] = {0, 255};

DtRealTriple range_color[] = {
   0.0, 0.0, 1.0,	/* blue */
   0.0, 1.0, 0.0,	/* green */
   0.0, 1.0, 1.0,	/* cyan */
   1.0, 0.0, 0.0,	/* red */
   1.0, 0.0, 1.0,	/* purple */
   1.0, 1.0, 0.0,	/* yellow */
   1.0, 1.0, 1.0,	/* white */
   1.0, .75, 0.5	/* gold */
};

    
    /***  Function Prototypes  ***/

void         setup_colormaps (void);
int          process_character (char controlchar);
void         process_pick (DtPoint3 pck_pnt);
void         print_path (DtInt size, DtInt path[]);
void         read_values (void);
void         write_values (void);
void         print_help (void);

#ifdef use_SunView
    void         setup_sunview_window (DtInt, DtInt, char*);
    void         do_repaint (Canvas canvas, Pixwin *pw, Rectlist *area);
    Notify_value event_proc (Window window, Event *event, caddr_t arg);
#endif

#ifdef sgi
    void setup_iris_window (DtInt width, DtInt height, char *argstring);
    void iris_process_input (void);
#endif

#ifdef use_X11
    void  setup_x11_window (DtInt width, DtInt height, char *argstring);
    void  x11_process_input (void);
    char *get_graphic_type (void);
#endif

/*
  ======================================================================
 */
int main (int argc, char *argv[])
{
   char		filename[200];
   char		devicetype[200];
   char		argstring[200];
   char		cam_name[10];
   DtShadeMode	shademode;
   DtInt	processors;
   DtInt	width, height;
   DtInt	shaderange;
   DtObject	makeobject();
   DtObject	makestudio();
   DtSize3	aperture;
   DtSize3	check_aperture;
   DtPickPathOrder check_path_order;
   DtObject	check_pick_callback;
   char		controlchar;
   DtVolume	volume;
   DtColorRGB	backgroundcolor;
   DtObject	studio_group;
   DtObject	object_group;
   DtInt 	num_drivers;
   char       **driver_name;
   char       **driver_info;
   char		background_raster_file[200];
   DtInt 	rstr_width;
   DtInt 	rstr_height;
   DtInt 	rstr_depth;
   DtRasterType rstr_type;
   DtPtr 	rstr_data;
   DtObject 	background_raster;

   setup_colormaps();

   /* 
    * Parse all command line arguments. 
    */
   prsarg_get_keyword_int(argc,argv,"-width",512,&width);
   prsarg_get_keyword_int(argc,argv,"-height",512,&height);
   prsarg_get_keyword_switch(argc,argv,"-singlebuffered",DcFalse,&singlebuffered);
   prsarg_get_keyword_switch(argc, argv, "-transp", DcOff, &transpswitch);
   prsarg_get_keyword_real(argc, argv, "-transpintens", .7, &transpintensity);
   prsarg_get_keyword_int(argc,argv,"-testcase",0,&testcase);
   prsarg_get_keyword_string(argc,argv,"-bgraster","", background_raster_file);
   prsarg_get_keyword_string(argc,argv,"-txraster","tut.aif",
                texture_raster_file);
   prsarg_get_keyword_string(argc,argv,"-rendstyle","DcRealTime",tmpstring);
   if (strcmp(tmpstring, "DcRealTime") == 0) {
      rendstyle = DcRealTime;
   } else if (strcmp (tmpstring, "DcProductionTime") == 0) {
      rendstyle = DcProductionTime;
   } else if (sscanf(tmpstring, "%d", &i) == 1) {
      rendstyle = i;
   } else {
      printf ("bad render style '%s'\n", tmpstring);
      exit(1);
   }

   /* 
    * Establish an appropriate default device type and visualtype for 
    * the platform. 
    */
# ifdef stdx
   prsarg_get_keyword_string(argc,argv,"-dt","stdx11",devicetype);
   prsarg_get_keyword_string(argc,argv,"-visualtype","DcPseudoColor",tmpstring);
#else
#if defined (use_SunView)
   prsarg_get_keyword_string(argc,argv,"-dt","sunviewGP",devicetype);
   prsarg_get_keyword_string(argc,argv,"-visualtype","DcPseudoColor",tmpstring);
# else
# ifdef sgi
   prsarg_get_keyword_string(argc,argv,"-dt","gl",devicetype);
   prsarg_get_keyword_string(argc,argv,"-visualtype","DcTrueColor",tmpstring);
#else
#if defined(use_VLL) || defined(use_DGL)
   {
      char *st;
      extern char *get_graphic_type();

      st = get_graphic_type();
      if (!strcmp(st, "G2")) {
         gr_type = GRAPHIC_G2;
      } else if (!strcmp(st, "G3")) {
         gr_type = GRAPHIC_G3;
      }
   }
   prsarg_get_keyword_string(argc,argv,"-dt","stardentx11",devicetype);
   if (GRAPHIC_G2 == gr_type) {
      prsarg_get_keyword_string(argc,argv,"-visualtype","DcDirectColor",tmpstring);
   } else {
      prsarg_get_keyword_string(argc,argv,"-visualtype","DcTrueColor",tmpstring);
   }
#else
   prsarg_get_keyword_string(argc,argv,"-dt","stdx11",devicetype);
   prsarg_get_keyword_string(argc,argv,"-visualtype","DcPseudoColor",tmpstring);
# endif
# endif
# endif
# endif
   if (strcmp(tmpstring, "DcPseudoColor") == 0) {
      visualtype = DcPseudoColor;
   } else if (strcmp(tmpstring, "DcStaticGrey") == 0) {
      visualtype = DcStaticGrey;
   } else if (strcmp(tmpstring, "DcTrueColor") == 0) {
      visualtype = DcTrueColor;
   } else {
      visualtype = DcDirectColor;
   }


   prsarg_get_keyword_string(argc,argv,"-shademode","DcComponent",tmpstring);
   if (strcmp(tmpstring, "DcComponent") == 0) {
      shademode = DcComponent;
   } else if (strcmp (tmpstring, "DcRange") == 0) {
      shademode = DcRange;
   } else {
      fprintf (stderr, "bad shade mode: '%s'\n", tmpstring);
      exit (1);
   }

   prsarg_get_keyword_int(argc,argv,"-shaderanges",1,&shaderange);
   shaderange = shaderange != 1 && shaderange != 8 ? 1 : shaderange;

   prsarg_get_keyword_int(argc,argv,"-procs",0,&processors);

   /* pick options */
   prsarg_get_keyword_int(argc,argv,"-ls",1024,&list_size);
   prsarg_get_keyword_int(argc,argv,"-is",10,&index_size);
   prsarg_get_keyword_int(argc,argv,"-xap",20,&xap);
   prsarg_get_keyword_int(argc,argv,"-yap",16,&yap);
   prsarg_get_keyword_switch(argc,argv,"-rp",DcFalse,&reverse);

   pick_index = (DtInt *) malloc (index_size * sizeof (DtInt));
   z_values = (DtReal *) malloc (index_size * sizeof (DtReal));
   wc_values = (DtReal *) malloc (index_size * sizeof (DtReal));
   lc_values = (DtReal *) malloc (index_size * sizeof (DtReal));
   views = (DtObject *) malloc (index_size * sizeof (DtObject));
   hit_list = (long  *) malloc (list_size * sizeof (long));
   if (pick_index == NULL || z_values == NULL   || 
       wc_values == NULL  || lc_values == NULL  ||
       views == NULL      || hit_list == NULL) {
      fprintf (stderr, "memory allocation failed for picking data\n");
      exit (1);
   }

   prsarg_get_keyword_string(argc,argv,"-df"," ",argstring);
   prsarg_check_bad_arguments(argc,argv);

   DsInitialize(processors);

   /* 
    * Inquire which renderers are installed in Dore and print them out. 
    */
   num_rendstyles = DsInqNumRenderers();
   rendstyle_name = (char**) malloc (num_rendstyles * sizeof(char*));
   rendstyle_id   = (DtInt*) malloc (num_rendstyles * sizeof(DtInt));

   DsInqRendererNames (rendstyle_name);
   for (i=0; i<num_rendstyles; i++) {
      rendstyle_id[i] = DsInqRendererId(rendstyle_name[i]);
      if (rendstyle == rendstyle_id[i]) current_rendstyle = i;
   }

   printf ("\nRenderers Installed in Dore:\n");
   for (i=0; i<num_rendstyles; i++) {
      printf ("\t%d: '%s'\n", rendstyle_id[i], rendstyle_name[i]);
   }

   /* 
    * Inquire which device drivers are installed in Dore and print them out. 
    */
   num_drivers = DsInqNumDrivers();
   driver_name = (char**) malloc (num_drivers * sizeof(char*));
   driver_info = (char**) malloc (num_drivers * sizeof(char*));

   DsInqDriverInfo (num_drivers, driver_name, driver_info);
   printf ("\nDrivers Installed in Dore:\n");
   for (i=0; i<num_drivers; i++) {
       printf ("\t%-15s  '%s'\n", driver_name[i], driver_info[i]);
   }
   free (driver_name);
   free (driver_info);

#if defined (use_SunView)
   if ((strcmp (devicetype, "sunviewGP") == 0) ||
	  (strcmp(devicetype, "sunview") == 0)) {
      setup_sunview_window(width, height, argstring);
   }
#endif
#ifdef sgi
    if (strcmp (devicetype, "gl") == 0) {
       setup_iris_window(width, height, argstring);
    }
#endif
#if defined(use_DGL) || defined(use_VLL)
   if(!strcmp (devicetype, "ardent") || !strcmp (devicetype, "stardentx11")) {
      if (GRAPHIC_G2 == gr_type) {
         use_XB = 1;
         use_Xtitan = 1;
      } else {
         use_XB = 0;
         use_Xtitan = 0;
      }
      setup_x11_window(width, height, argstring);
   }
#endif
#ifdef use_X11
   if (strncmp (devicetype, "stdx11", 6) == 0) {
      setup_x11_window(width, height, argstring);
   }
#endif

   if (strcmp (devicetype, "rasterfile") == 0) {
      sprintf (argstring, "-width %d -height %d", width, height);
   }

   if (!(device = DoDevice(devicetype,argstring))) {
      fprintf(stderr, "can't create device:  type = '%s',  argstring = '%s'\n",
	     devicetype,argstring);
      exit(1);
   }

   DdSetShadeRanges(device, 0, 8, shade_ranges);
   if (DdInqVisualType(device) == DcPseudoColor) {
      pseudocolor = DcTrue;
      switch (shademode) {
      case DcComponent:
	 shadetype = 0;
	 DdSetShadeMode(device, DcComponent);
	 DdSetColorEntries(device, DcRGB, 0, 256, color_scale_map);
	 break;
      case DcRange:
	 DdSetShadeMode(device, DcRange);
	 if (shaderange == 1) {
	    shadetype = 1;
	    DdSetShadeRanges(device, 0, 1, shade_range1);
	    DdSetColorEntries(device, DcRGB, 0, 256, gray_scale_map);
	 } else {
	    shadetype = 2;
	    DdSetShadeRanges(device, 0, 8, shade_ranges);
	    DdSetColorEntries(device, DcRGB, 0, 256, shade_rng_map);
	 }
	 break;
      }
   } else {
      pseudocolor = DcFalse;
   }

   DdInqExtent(device,&volume);

   DdSetViewport(device,&volume);

   if (!(frame = DoFrame())) {
      fprintf(stderr, "can't create frame\n");
      exit(1);
   }

   DsHoldObj(frame);

   DdSetFrame(device,frame);
   DfSetBoundary(frame,&volume);

   if (!(view = DoView())) {
      fprintf(stderr, "can't create view\n");
      exit(1);
   }

   DsHoldObj(view);

   backgroundcolor[0] = .4;
   backgroundcolor[1] = .4;
   backgroundcolor[2] = .4;

   /* 
    * Make the supplied raster image the background of the view. 
    */
   if (background_raster_file[0] != '\0') {
      if (DsFileRasterRead(background_raster_file, &rstr_width, &rstr_height,
                        &rstr_depth, &rstr_type, &rstr_data) != -1) {
         background_raster = DoRaster(rstr_width, rstr_height, rstr_depth, 
			rstr_type, DcNullPtr, rstr_data, DcDeleteData);
      } else {
         background_raster = DcNullObject;
      }
   }
   if (background_raster != DcNullObject) {
      DvSetBackgroundRaster(view, background_raster, DcNullObject);
   }

   DvSetClearFlag(view,DcTrue);
   DvSetBackgroundColor(view, DcRGB, backgroundcolor);
   DvSetRendStyle(view,rendstyle_id[current_rendstyle]);

   DgAddObjToGroup(DfInqViewGroup(frame),view);

   DvSetBoundary(view,&volume);

   object_group = makeobject();
   DsHoldObj(object_group);

   studio_group = makestudio();
   DsHoldObj(studio_group);

   DgAddObjToGroup(DvInqDefinitionGroup(view),studio_group);
   DgAddObjToGroup(DvInqDisplayGroup(view),object_group);

   /* set the pick order and check */

#if 0
   DdSetPickPathOrder(device,reverse ? DcBottomFirst : DcTopFirst);
   check_path_order = DdInqPickPathOrder(device);

   if (check_path_order != (reverse ? DcBottomFirst : DcTopFirst)) {
      fprintf(stderr,"got bad value from DdInqPickPathOrder\n");
      exit(1);
   }

   /* set the pick aperture and check. */

   aperture.width = (DtReal)xap;
   aperture.height = (DtReal)yap;
   aperture.depth = 100000.;
   DdSetPickAperture(device,&aperture);
   DdInqPickAperture(device,&check_aperture);

   if ((check_aperture.width  != aperture.width)  ||
       (check_aperture.height != aperture.height) ||
       (check_aperture.depth  != aperture.depth)) {
      fprintf(stderr,"got bad value from DdInqPickAperture\n");
      exit(1);
   }

   /* set the pick callback and check. */

   DdSetPickCallback(device,DcPickAll);
   check_pick_callback = DdInqPickCallback(device);

   if (check_pick_callback != DcPickAll) {
      fprintf(stderr,"got bad value from DdInqPickCallback\n");
      exit(1);
   }
#endif

   DvSetUpdateType(view,DcUpdateAll);
   DdUpdate(device);
   DvSetUpdateType(view,DcUpdateDisplay);

#if defined (use_SunView)
   if ((strcmp (devicetype, "sunviewGP") == 0) ||
	  (strcmp(devicetype, "sunview") == 0)) {
      printf ("Picking enabled on Left Mouse Button\n");
      window_main_loop (sun_frame);
   } else
#else
#ifdef sgi
   if (strcmp (devicetype, "gl") == 0 ) {
      printf ("Picking enabled on Left Mouse Button\n");
      iris_process_input();
   } else
#else
#ifdef use_X11
   if (strcmp (devicetype, "stdx11") == 0) {
      printf ("Picking enabled on Left Mouse Button\n");
      x11_process_input();
   } else
#if defined(use_DGL) ||  defined(use_VLL)
   if(!strcmp (devicetype, "ardent") || !strcmp(devicetype, "stardentx11") ||
      (!strcmp(devicetype, "dgl")) || (!strcmp(devicetype, "g3"))) {
      printf ("Picking enabled on Left Mouse Button\n");
      x11_process_input();
   } else
#endif
#endif
#endif
#endif
   {
      printf("enter any character to begin display ('?' for help)\n");

      DvSetUpdateType(view, DcUpdateDisplay);

      while (1) {
	 controlchar = getchar();
	 if (process_character (controlchar))
	       break;
      }
   }

   /*  Releasing the device object will cause it to be deleted, and the
       appropriate dgi close will be done for the device as a result.  This
       should result in correct termination of output to the device (buffer
       flushing).  */

   DsReleaseObj(device);

   DsTerminate();
}

/* ====================================================================== */

void setup_colormaps (void)
{
   DtInt i, j;

   for (i=0; i<256; i++) {
      gray_scale_map[i*3] = gray_scale_map[i*3+1] = 
	    gray_scale_map[i*3+2] = (DtReal)(i / 255.);
      color_scale_map[i*3+0] = (DtReal)(i>>5) / 7.;
      color_scale_map[i*3+1] = (DtReal)((i>>2) & 0x07) / 7.;
      color_scale_map[i*3+2] = (DtReal)(i & 0x03) / 3.;
   }

   for (i=0; i<8; i++) {
      shade_ranges[i*2+0] = i*32;
      shade_ranges[i*2+1] = i*32+31;
      for (j=0; j<32; j++) {
	 shade_rng_map[i*32*3+j*3+0] = range_color[i][0] * (DtReal)j / 31.;
	 shade_rng_map[i*32*3+j*3+1] = range_color[i][1] * (DtReal)j / 31.;
	 shade_rng_map[i*32*3+j*3+2] = range_color[i][2] * (DtReal)j / 31.;
      }
   }
}

/* ====================================================================== */

int process_character (char controlchar)
{
   DtFlag fullupdate = DcFalse;
   DtInt rstr_width;
   DtInt rstr_height;
   DtRasterType rstr_type;
   DtPtr rstr_data;
   DtFlag rstr_userdelete;
   DtObject raster;
   char filename[100];
   int answer;

   if (controlchar == 'x') return 1;

   switch (controlchar) {
   case '?':
      print_help();
      break;

   case '1':
      backfacecullswitch = !backfacecullswitch;
      printf ("culling is now %s\n", backfacecullswitch ? "on" : "off");
      break;

   case '2':
      reptype++;
      if (reptype > 5) reptype = 0;
      printf ("representation type is now %s\n",
          reptype == 0 ? "DcPoints" :
          reptype == 1 ? "DcWireframe" :
          reptype == 2 ? "DcSurface" :
          reptype == 3 ? "DcOutlines" :
          reptype == 4 ? "DcSurfaceWireframeEdges" :
          "DcSurfaceOutlineEdges" );
      break;
					
   case '3':
      interptype++;
      if (interptype > 2) interptype = 0;
      printf ("interpolation type is now %s\n",
          interptype == 0 ? "ConstantShade" :
          interptype == 1 ? "VertexShade" : "SurfaceShade");
      break;
      
   case '4':
      specularswitch = !specularswitch;
      printf ("specular switch is now %s\n", specularswitch ? "on" : "off");
      fullupdate = DcTrue;
      break;

   case '5':
      shadetype++;
      if (shadetype > 2) shadetype = 0;
      if (pseudocolor) {
	 switch (shadetype) {
	 case 0:
	    printf ("ShadeMode = DcComponent\n");
	    DdSetShadeMode(device, DcComponent);
	    DdSetColorEntries(device, DcRGB, 0, 256, color_scale_map);
	    break;
	 case 1:
	    printf ("ShadeMode = DcRange (1 range)\n");
	    DdSetShadeMode(device, DcRange);
	    DdSetShadeRanges(device, 0, 1, shade_range1);
	    DdSetColorEntries(device, DcRGB, 0, 256, gray_scale_map);
	    break;
	 case 2:
	    printf ("ShadeMode = DcRange (8 ranges)\n");
	    DdSetShadeMode(device, DcRange);
	    DdSetShadeRanges(device, 0, 8, shade_ranges);
	    DdSetColorEntries(device, DcRGB, 0, 256, shade_rng_map);
	    break;
	 }
	 fullupdate = DcTrue;
      }
      break;

   case '6':
      uscale += 0.1;
      uscale = min(uscale, 2.);
      printf("texture U scale value is now %3.1f\n",uscale);
      break;
   case '^':
      uscale -= 0.1;
      uscale = max(uscale, 0.);
      printf("texture U scale value is now %3.1f\n",uscale);
      break;
   case '7':
      vscale += 0.1;
      vscale = min(vscale, 2.);
      printf("texture V scale value is now %3.1f\n",vscale);
      break;
   case '&':
      vscale -= 0.1;
      vscale = max(vscale, 0.);
      printf("texture V scale value is now %3.1f\n",vscale);
      break;
   case '8':
      utranslate += 0.1;
      utranslate = min(utranslate, 1.);
      printf("texture U translate value is now %3.1f\n",utranslate);
      break;
   case '*':
      utranslate -= 0.1;
      utranslate = max(utranslate, -1.);
      printf("texture U translate value is now %3.1f\n",utranslate);
      break;
   case '9':
      vtranslate += 0.1;
      vtranslate = min(vtranslate, 1.);
      printf("texture V translate value is now %3.1f\n",vtranslate);
      break;
   case '(':
      vtranslate -= 0.1;
      vtranslate = max(vtranslate, -1.);
      printf("texture V translate value is now %3.1f\n",vtranslate);
      break;
   case '0':
      umode++;
      if (umode > 3) umode = 0;
      switch (umode) {
         case DcExtendNone:
            printf("texture U extend mode is DcExtendNone\n");
            break;
         case DcExtendBlack:
            printf("texture U extend mode is DcExtendBlack\n");
            break;
         case DcExtendClamp:
            printf("texture U extend mode is DcExtendClamp\n");
            break;
         case DcExtendRepeat:
            printf("texture U extend mode is DcExtendRepeat\n");
            break;
      }
      break;
   case '-':
      vmode++;
      if (vmode > 3) vmode = 0;
      switch (vmode) {
         case DcExtendNone:
            printf("texture V extend mode is DcExtendNone\n");
            break;
         case DcExtendBlack:
            printf("texture V extend mode is DcExtendBlack\n");
            break;
         case DcExtendClamp:
            printf("texture V extend mode is DcExtendClamp\n");
            break;
         case DcExtendRepeat:
            printf("texture V extend mode is DcExtendRepeat\n");
            break;
      }
      break;
   case '=':
      aa_mode++;
      if (aa_mode > 4) aa_mode = 0;
      switch (aa_mode) {
         case DcTextureAANone:
            printf("texture antialias mode is TextureAANone\n");
            break;
         case DcTextureAAMIPPoint:
            printf("texture antialias mode is DcTextureAAMIPPoint\n");
            break;
         case DcTextureAAMIP2Point:
            printf("texture antialias mode is DcTextureAAMIP2Point\n");
            break;
         case DcTextureAAMIPBilinear:
            printf("texture antialias mode is DcTextureAAMIPBilinear\n");
            break;
         case DcTextureAAMIP2Bilinear:
            printf("texture antialias mode is DcTextureAAMIP2Bilinear\n");
            break;
         case DcTextureAAAdaptive:
            printf("texture antialias mode is DcTextureAAAdaptive\n");
            break;
         case DcTextureAABilinear:
            printf("texture antialias mode is DcTextureAABilinear\n");
            break;
         case DcTextureAATrilinear:
            printf("texture antialias mode is DcTextureAATrilinear\n");
            break;
         case DcTextureAASumArea:
            printf("texture antialias mode is DcTextureAASumArea\n");
            break;
      }
      break;
   case '\\':
      tex_op++;
      if (tex_op > 3) tex_op = 0;
      switch (tex_op) {
         case DcTextureReplace:
            printf("texture operator is DcTextureReplace\n");
            break;
         case DcTextureMultiply:
            printf("texture operator is DcTextureMultiply\n");
            break;
         case DcTextureBlend:
            printf("texture operator is DcTextureBlend\n");
            break;
         case DcTextureAdd:
            printf("texture operator is DcTextureAdd\n");
            break;
      }
      break;

   case 'c':
      depthcueswitch = !depthcueswitch;
      printf("depthcueswitch is now %s\n",depthcueswitch ? "on" : "off");
      break;

   case 'd':
      diffuseintensity += .1;
      diffuseintensity = min(diffuseintensity,1.);
      printf("diffuse intensity is now %3.1f\n",diffuseintensity);
      fullupdate = DcTrue;
      break;

   case 'D':
      diffuseintensity -= .1;
      diffuseintensity = max(diffuseintensity,0.);
      printf("diffuse intensity is now %3.1f\n",diffuseintensity);
      fullupdate = DcTrue;
      break;

   case 'a':
      ambientintensity += .1;
      ambientintensity = min(ambientintensity,1.);
      printf("ambient intensity is now %3.1f\n",ambientintensity);
      fullupdate = DcTrue;
      break;

   case 'A':
      ambientintensity -= .1;
      ambientintensity = max(ambientintensity,0.);
      printf("ambient intensity is now %3.1f\n",ambientintensity);
      fullupdate = DcTrue;
      break;

   case 'm':
      scalevalue[0] += .1;
      break;

   case 'M':
      scalevalue[0] -= .1;
      scalevalue[0] = max(scalevalue[0],.1);
      break;

   case ',':
      scalevalue[1] += .1;
      break;

   case '<':
      scalevalue[1] -= .1;
      scalevalue[1] = max(scalevalue[1],.1);
      break;

   case '.':
      scalevalue[2] += .1;
      break;

   case '>':
      scalevalue[2] -= .1;
      scalevalue[2] = max(scalevalue[2],.1);
      break;

   case 'j':	rotatevalue[0] += .1;		break;
   case 'J':	rotatevalue[0] -= .1;		break;
   case 'k':	rotatevalue[1] += .1;		break;
   case 'K':	rotatevalue[1] -= .1;		break;
   case 'l':	rotatevalue[2] += .1;		break;
   case 'L':	rotatevalue[2] -= .1;		break;

   case 'u':	translatevalue[0] += .1;	break;
   case 'U':	translatevalue[0] -= .1;	break;
   case 'i':	translatevalue[1] += .1;	break;
   case 'I':	translatevalue[1] -= .1;	break;
   case 'o':	translatevalue[2] += .1;	break;
   case 'O':	translatevalue[2] -= .1;	break;

   case 's':
      subdivlevel += .1;
      break;

   case 'S':
      subdivlevel -= .1;
      break;

   case 'P':
      printf ("Do you want Z (1 - yes, 0 - no) ?");
      fflush (stdout);
      scanf("%d", &answer);
      printf ("Getting Pixel Data from device ...", answer ? "with Z" : "");
      fflush(stdout);
      if (DdInqPixelData(device, answer ? DcRasterRGBZ : DcRasterRGB,
                        &rstr_width, &rstr_height, &rstr_type, 
			&rstr_data, &rstr_userdelete)) {
	  printf (" ok\n");
          raster = DoRaster (rstr_width, rstr_height, 1, rstr_type, DcNullPtr, 
			rstr_data, rstr_userdelete ? DcDeleteData : DcNullPtr);
          printf ("filename for raster image: ");
          fflush(stdout);
          scanf ("%s", filename);
          printf ("writing raster to '%s' ...", filename);
          fflush (stdout);
          DsRasterWrite(raster, filename);
          DsReleaseObj(raster);
          printf (" done\n");
      } else {
          printf (" failed\n");
      }
      break;
   case 'w':
      write_values();
      printf("values written to file 'values.dat'\n");
      break;

   case 'r':
      read_values();
      printf("values read in from file 'values.dat'\n");
      fullupdate = DcTrue;
      break;

   case 'W':
      DvSetUpdateType(view, DcUpdateAll);
      DdUpdate(device);
      DvSetUpdateType(view, DcUpdateDisplay);
      printf("unconditional update performed\n");
      break;

   case 'Z':
      clearflag = !clearflag;
      printf ("clear flag is now %s\n", clearflag ? "on" : "off");
      DvSetClearFlag(view,clearflag);
      break;

   }

   if (fullupdate) {
      DvSetUpdateType(view, DcUpdateAll);
   } 
   DdUpdate(device);

   if (fullupdate) {
      DvSetUpdateType(view, DcUpdateDisplay);
   } 

   return 0;
}

/* ====================================================================== */

void process_pick (DtPoint3 pck_pnt)
{
#if 0
   DtInt hit_count;
   DtInt i;
   DtInt e_flag;
   DtVolume volume;

   DdInqExtent(device,&volume);

   printf("\n\n******** PICK REPORT ********\n");
   printf("   volume      { (%g,%g,%g) (%g,%g,%g) }\n",
	  volume.bll[0],volume.bll[1],volume.bll[2],
	  volume.fur[0],volume.fur[1],volume.fur[2]);

   printf("   pick point  (%g,%g,%g)\n",
	  pck_pnt[0],pck_pnt[1],pck_pnt[2]);

   DdPickObjs(device,pck_pnt,&hit_count,index_size,pick_index,
	  list_size,hit_list,z_values,wc_values,lc_values,views,&e_flag);

   printf("   accepted    %d hit%c\n   eflag       0x%0x\n",
	  hit_count, hit_count != 1 ? 's' : ' ',e_flag);
   printf("   paths       %sreversed\n\n", reverse ? "not " : "");

   for(i=0; i<hit_count; i++) {
      printf("hit %d  z: %g\n\tworld_point: (%g %g %g)\n\tlocal_point: (%g %g %g)\n\tview: 0x%0x\n",
	     	i,z_values[i],
		wc_values[3*i],wc_values[3*i+1],wc_values[3*i+2],
             	lc_values[3*i],lc_values[3*i+1],lc_values[3*i+2],
		views[i]);
      print_path((pick_index[i+1]-pick_index[i]) /3,hit_list+pick_index[i]);
   }
#endif
}

/* ====================================================================== */

void print_path (DtInt size, DtInt path[])
{
#if 0
   DtNameType type;
   DtPtr *name;
   DtInt i;

   printf("+------------------------------+-------+-------+\n");
   printf("|                              |  pick |  ele  |\n");
   printf("|           group              |   id  | seq # |\n");
   printf("+------------------------------+-------+-------+\n");
   for(i=0; i<size; i++) {
      switch(DsInqObjStatus((DtObject)path[3*i]))	{
      case DcObjectValid:
	 DsInqObjName((DtObject)path[3*i],&type,&name); 
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
	 printf("|%30s", "deleted!");
	 break;
      case DcObjectDeleted:
	 printf("|30%s", "invalid!");
	 break;
      default: 
	 printf("|%30s", "unknown validity type");
	 break;
      }
      printf("|%7d|%7d|\n",path[3*i+1],path[3*i+2]);
   } 
   printf("+------------------------------+-------+-------+\n");
#endif
}

/* ====================================================================== */

void read_values (void)
{
   FILE *valuesfile;

   if ((valuesfile = fopen("values.dat","r")) == NULL) {
      fprintf (stderr, "could not open file 'values.dat' for reading\n");
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
   fscanf(valuesfile,"shadetype %d\n", &shadetype);
   fscanf(valuesfile,"ambientintensity %lf\n", &ambientintensity);
   fscanf(valuesfile,"diffuseintensity %lf\n", &diffuseintensity);
   fscanf(valuesfile,"depthcueswitch %d\n", &depthcueswitch);

   fclose(valuesfile);
}

/*
  ======================================================================
 */

void write_values (void)
{
   FILE *valuesfile;

   if ((valuesfile = fopen("values.dat", "w")) == NULL) {
      fprintf (stderr, "could not open file 'values.dat' for writing\n");
      perror ("");
      return;
   }

   fprintf(valuesfile,"backfacecullswitch %d\n",backfacecullswitch);
   fprintf(valuesfile,"interptype %d\n",interptype);
   fprintf(valuesfile,"reptype %d\n",reptype);
   fprintf(valuesfile,"rotatevalue %lf %lf %lf\n",
	   rotatevalue[0],rotatevalue[1],rotatevalue[2]);
   fprintf(valuesfile,"scalevalue %lf %lf %lf\n",
	   scalevalue[0],scalevalue[1],scalevalue[2]);
   fprintf(valuesfile,"specularswitch %d\n",specularswitch);
   fprintf(valuesfile,"translatevalue %lf %lf %lf\n",
	   translatevalue[0],translatevalue[1],translatevalue[2]);

   fprintf(valuesfile,"subdivlevel %lf\n", subdivlevel);
   fprintf(valuesfile,"shadetype %d\n", shadetype);
   fprintf(valuesfile,"ambientintensity %lf\n", ambientintensity);
   fprintf(valuesfile,"diffuseintensity %lf\n", diffuseintensity);
   fprintf(valuesfile,"depthcueswitch %d\n", depthcueswitch);

   fclose (valuesfile);
}

/*
  ======================================================================
 */

void print_help (void)
{
   printf ("Commands:\n");
   printf ("\tx - exit\n");
   printf ("\t1 - toggle backface cull switch\n");
   printf ("\t2 - cycle representation type\n");
   printf ("\t3 - cycle interpolation type\n");
   printf ("\t4 - toggle specular highlights\n");
   printf ("\t5 - cycle shading type (Component, Range 1, Range 8)\n\n");
   printf ("\t6/^ - increase/decrease texture scale in U\n");
   printf ("\t7/& - increase/decrease texture scale in V\n");
   printf ("\t8/* - increase/decrease texture translate in U\n");
   printf ("\t9/( - increase/decrease texture translate in V\n");
   printf ("\t0 - cycle texture extend mode in U\n");
   printf ("\t- - cycle texture extend mode in V\n");
   printf ("\t= - cycle texture antialias mode\n");
   printf ("\t\\ - cycle texture operator\n\n");
   printf ("\tc - toggle Depth Cue Switch\n");
   printf ("\ts/S - increase/decrease Subdivision Level\n");
   printf ("\td/D - increase/decrease Diffuse Intensity\n");
   printf ("\ta/A - increase/decrease Ambient Intensity\n\n");
   printf ("\tm/M - X Scale\n");
   printf ("\t,/< - Y Scale\n");
   printf ("\t./> - Z Scale\n\n");
   printf ("\tj/J - X Rotation\n");
   printf ("\tk/K - Y Rotation\n");
   printf ("\tl/L - Z Rotation\n\n");
   printf ("\tu/U - X Translation\n");
   printf ("\ti/I - Y Translation\n");
   printf ("\to/O - Z Translation\n\n");
   printf ("\tP - get pixel data from device\n");
   printf ("\tr - read values from 'values.dat'\n");
   printf ("\tw - write values to 'values.dat'\n");
   printf ("\tW - unconditional Update\n");
   printf ("\tZ - clear flag switch\n\n");
}

/* ======================================================================
			BEGIN SUN-SPECIFIC CODE
   ====================================================================== */
#if defined (use_SunView)
void setup_sunview_window (DtInt width, DtInt height, char *argstring)
{
   Notify_value	event_proc ();
   void		do_repaint();

   if (((sun_frame = window_create (NULL, FRAME, 0)) == NULL) ||
       ((sun_canvas = window_create (sun_frame, CANVAS,
				     CANVAS_RETAINED, FALSE,
				     CANVAS_WIDTH, width,
				     CANVAS_HEIGHT, height,
				     WIN_WIDTH, width,
				     WIN_HEIGHT, height,
				     WIN_CONSUME_KBD_EVENTS, 
				     WIN_ASCII_EVENTS, 0,
				     WIN_CONSUME_PICK_EVENTS,
				     WIN_MOUSE_BUTTONS, 0,
				     WIN_IGNORE_PICK_EVENTS,
				     WIN_UP_EVENTS, 0,
				     WIN_EVENT_PROC, event_proc,
				     CANVAS_REPAINT_PROC, do_repaint,
				     0)) == NULL)) {
      printf ("can't create sunview window\n");
      exit (1);
   } 
   window_fit (sun_frame);
   sprintf (&(argstring[strlen(argstring)]), 
	    " -width %d -height %d -canvas %d", 
	    width, height, sun_canvas);
   if (singlebuffered) {
      sprintf (&(argstring[strlen(argstring)]), " -singlebuffered");
   }
}

/* ====================================================================== */

void do_repaint (Canvas canvas, Pixwin *pw, Rectlist *area)
{
   DvSetUpdateType(view,DcUpdateAll);
   DdUpdate (device);
   DvSetUpdateType(view,DcUpdateDisplay);
}

/* ====================================================================== */

Notify_value event_proc (Window window, Event *event, caddr_t arg)
{
   static DtPoint3 pck_pnt = {250., 250., 4000.};
   DtVolume volume;

   if (event_is_ascii(event)) {
      if (process_character(event_id(event))) {
	 window_set(sun_frame, FRAME_NO_CONFIRM, TRUE, 0);
	 window_done(window);
      }
   } else if (event_action(event) == MS_LEFT) {
      DdInqExtent(device,&volume);

      pck_pnt[0] = (DtReal)event_x(event);
      pck_pnt[1] = (volume.fur[1] - volume.bll[1]) - (DtReal)event_y(event);
      
      process_pick (pck_pnt);
   }
}
#endif
/* ======================================================================
			END OF SUN-SPECIFIC CODE
   ====================================================================== */

/* ======================================================================
			BEGIN IRIS-SPECIFIC CODE
   ====================================================================== */
#ifdef sgi
void setup_iris_window (DtInt width, DtInt height, char *argstring)
{
    long window;

    foreground();
    prefposition(0, width, 0, height);
    if ((window = winopen("Dore Window")) == NULL) {
	printf ("can't create iris window\n");
	exit (1);
    }

    sprintf (&(argstring[strlen(argstring)]), " -width %d -height %d -window %d",
                	width, height, window);

    if (visualtype == DcTrueColor) {
	sprintf (&(argstring[strlen(argstring)]), " -visualtype DcTrueColor");
    } else {
	sprintf (&(argstring[strlen(argstring)]), " -visualtype DcPseudoColor");
    }
}

/* ====================================================================== */

void iris_process_input (void)
{
    int dev;
    short val;

    qdevice(KEYBD);
    qdevice(LEFTMOUSE);
    qdevice(REDRAW);

    while (1) {
       dev = qread(&val);

       if (dev == LEFTMOUSE && val) {

          long x0,y0;
          DtPoint3 pck_pnt;

          getorigin(&x0,&y0);

          pck_pnt[0] = getvaluator(MOUSEX)-x0;
          pck_pnt[1] = getvaluator(MOUSEY)-y0;
          pck_pnt[2] = 0.;

          process_pick(pck_pnt);
       } else if (dev == KEYBD && process_character(val)) {
           break;
       } else if (dev == REDRAW) {
           DdUpdate(device);
       }
    }
}
#endif
/* ======================================================================
			END OF IRIS-SPECIFIC CODE
   ====================================================================== */

/* ======================================================================
			BEGIN X11-SPECIFIC CODE
   ====================================================================== */
#ifdef use_X11
void setup_x11_window (DtInt width, DtInt height, char *argstring)
{
   XVisualInfo *visual_info,vinfo_template;
   DtVisualType new_visualtype;
   int nvisuals;
   Colormap cmap;
   char *displayName;
   extern char *getenv();
   int screen;
   XSetWindowAttributes attributes;
   XEvent event;
   int buffercount;
   int attrMask;
   static XWMHints xwmh = {
      (InputHint|StateHint),/* flags */
      True,		/* input */
      NormalState,	/* initial_state */
      0,		/* icon pixmap */
      0,		/* icon window */
      0, 0,		/* icon location */
      0,		/* icon mask */
      0,		/* Window group */
   };
   XSizeHints  xsh;
   XClassHint class_hints;
   DtInt nplanes;

   xsh.flags = (USPosition|USSize);
   xsh.width = width;
   xsh.height = height;
   xsh.x = 0;
   xsh.y = 0;

   /* create an X11 window,and Dore device (window) */

   if ((displayName = getenv("DISPLAY")) == NULL) {
      printf("main: couldn't get display name\n");
      exit(1);
   }

   if ((displayPtr = XOpenDisplay(displayName)) == NULL ) {
      printf("couldn't open display\n");
      exit(1);
   }

   screen = XDefaultScreen(displayPtr);

   vinfo_template.screen = screen;

   attributes.background_pixel = 0x000000;
   attributes.border_pixel = 0xffffff;
   attrMask = CWBackPixel|CWBorderPixel;

   switch (visualtype) {
   case DcTrueColor:
      new_visualtype = DcTrueColor;
      vinfo_template.class = TrueColor;
      vinfo_template.depth = 24;
      nplanes = 24;
      visual_info = XGetVisualInfo(displayPtr,
			VisualScreenMask|VisualClassMask|VisualDepthMask,
			&vinfo_template,&nvisuals);
      if (nvisuals > 0) {
	 if (visual_info->visual == DefaultVisual(displayPtr,screen)) {
	    printf ("using DefaultColormap (TrueColor) of screen %d\n", screen);
	    cmap = DefaultColormap(displayPtr,screen);
	 } else {
	    cmap = XCreateColormap(displayPtr, RootWindow(displayPtr,screen),
				   visual_info->visual,AllocNone);
	 }
	 if (cmap == NULL) {
	    printf ("could not allocate color map\n");
	    exit(1);
	 }
	 attributes.colormap = cmap;
	 attrMask |= CWColormap;
	 
	 break;
      }
      break;

   case DcDirectColor:
      new_visualtype = DcDirectColor;
      vinfo_template.class = DirectColor;
      vinfo_template.depth = 24;
      nplanes = 24;
      visual_info = XGetVisualInfo(displayPtr,
			VisualScreenMask|VisualClassMask|VisualDepthMask,
			&vinfo_template,&nvisuals);
      if (nvisuals > 0) {
#if defined(titan)
	 if (use_Xtitan) 
	       cmap = XTitanDefaultDirectColormap(displayPtr,screen);
	 else
#endif
         cmap = XCreateColormap(displayPtr, RootWindow(displayPtr,screen),
				visual_info->visual,AllocAll);
	 if (cmap == NULL) {
	    printf ("could not allocate color map\n");
	    exit(1);
	 }
	 attributes.colormap = cmap;
	 attrMask |= CWColormap;
	 break;
      }
      break;

   case DcPseudoColor:
      new_visualtype = DcPseudoColor;
      vinfo_template.class = PseudoColor;
      vinfo_template.depth = 12;
      nplanes = 12;
      visual_info = XGetVisualInfo(displayPtr,
			VisualScreenMask|VisualClassMask|VisualDepthMask,
			&vinfo_template,&nvisuals);
      if (nvisuals == 0) {
	 vinfo_template.class = PseudoColor;
	 vinfo_template.depth = 8;
	 nplanes = 8;
	 visual_info = XGetVisualInfo(displayPtr,
			VisualScreenMask|VisualClassMask|VisualDepthMask,
			&vinfo_template,&nvisuals);
      }
      if (nvisuals > 0) {
	 if (visual_info->visual == DefaultVisual(displayPtr,screen)) {
	    printf ("using DefaultColormap (PseudoColor) of screen %d\n", screen);
	    cmap = DefaultColormap(displayPtr,screen);
	 } else {
	     cmap = XCreateColormap(displayPtr, RootWindow(displayPtr,screen),
 				    visual_info->visual,AllocAll);
	     if (cmap == NULL) {
		 printf ("could not allocate color map\n");
		 exit(1);
	     }
	     setup_colormaps();
	 }
	 setup_colormaps();
	 attributes.colormap = cmap;
	 attrMask |= CWColormap;
	 break;
      }
      break;

   case DcStaticGrey:
      new_visualtype = DcStaticGrey;
      vinfo_template.class = StaticGray;
      vinfo_template.depth = 1;
      nplanes = 1;
      visual_info = XGetVisualInfo(displayPtr,
			VisualScreenMask|VisualClassMask|VisualDepthMask,
			&vinfo_template,&nvisuals);
      if (nvisuals > 0) break;
      break;
   default:
      nvisuals = 0;
   }

   if (nvisuals == 0) {
      printf ("screen doesn't support specified visual type\n");
      exit (1);
   }

   if (visualtype != new_visualtype) {
      visualtype = new_visualtype;
   }
	      
#ifdef titan
   if (use_Xtitan) {
      if(VendorRelease(displayPtr) >= 211) {
	 if ((visualtype != DcPseudoColor) &&
	     !XTitanGetExpansionStatus(displayPtr, screen)) {
	    printf ("No expansion board so using single buffered mode\n");
	    singlebuffered = DcTrue;
	 }
      }
   }

   if (use_XB) {
      if (singlebuffered) {
	 buffercount = 1;
      } else {
	 buffercount = 2;
      }

      window = XBCreateWindow(displayPtr, RootWindow(displayPtr,screen),
		xsh.x, xsh.y, xsh.width, xsh.height, 0, 
		nplanes, InputOutput,visual_info->visual,
		buffercount,1, attrMask,&attributes);
   } else 
#endif
   {
      window = XCreateWindow(displayPtr,
			      RootWindow(displayPtr,screen),
			      xsh.x, xsh.y, xsh.width, xsh.height, 0,
			      nplanes,InputOutput,visual_info->visual,
			      attrMask,&attributes);
   }

   if (window == NULL) {
      printf("main: couldn't open window\n");
      exit(1);
   }

   XStoreName(displayPtr,window,"Dore Window");
   XSetIconName(displayPtr,window,"Dore Window");
   XSetWMHints(displayPtr,window,&xwmh);
   XSetSizeHints(displayPtr,window,&xsh,XA_WM_NORMAL_HINTS);
   class_hints.res_name = "dore";
   class_hints.res_class = "Dore";
   XSetClassHint(displayPtr,window,&class_hints);

   XSelectInput(displayPtr,window,ExposureMask);
   XMapWindow(displayPtr,window);
   while(XNextEvent(displayPtr,&event)) {
      if (event.type == Expose)
	    break;
   }

   XFlush(displayPtr);

   sprintf (&(argstring[strlen(argstring)]), 
	    " -geometry %dx%d+%d+%d -display %ld -window %ld",
	    width, height, 0, 0, displayPtr, window);
   if (singlebuffered) {
      sprintf (&(argstring[strlen(argstring)]),
               " -singlebuffered");
   }
   switch (visualtype) {
      case DcPseudoColor:
	 sprintf (&(argstring[strlen(argstring)])," -visualtype DcPseudoColor");
	 break;
      case DcTrueColor:
	 sprintf (&(argstring[strlen(argstring)])," -visualtype DcTrueColor");
	 break;
      case DcDirectColor:
	 sprintf (&(argstring[strlen(argstring)])," -visualtype DcDirectColor");
	 break;
      case DcStaticGrey:
	 sprintf (&(argstring[strlen(argstring)])," -visualtype DcStaticGrey");
	 break;
   }
}
/*
  ======================================================================
 */
void x11_process_input (void)
{
   XEvent event;
   static DtPoint3 pck_pnt = {250., 250., 4000.};
   DtVolume volume;
   static char buffer[10];
   static int bufsize = 10;
   int count;
   int i;
   KeySym keysym;
   XComposeStatus compose;

   XSelectInput(displayPtr,window,KeyPressMask|ButtonPressMask);

   while(1) {
      XNextEvent(displayPtr,&event);

      switch (event.type) {
      case KeyPress:
	 count = XLookupString (&event, buffer, bufsize, &keysym, &compose);
	 for (i=0; i<count; i++) {
	    if (process_character(buffer[i])) {
	       return;
	    }
	 }
	 break;
      case ButtonPress:
	 if (event.xbutton.button == Button1) {
	    DdInqExtent(device, &volume);
	    pck_pnt[0] = (DtReal)(event.xbutton.x);
	    pck_pnt[1] = (volume.fur[1] - volume.bll[1]) - 
		  (DtReal)event.xbutton.y;
	    process_pick (pck_pnt);
	 }
	 break;
      }
   }
}

#if defined(use_VLL) || defined(use_DGL)
/* This code determines what kind of graphics capabilities a Stardent Titan
   series machine has.  */

#include <sys/utsname.h>
#include <machine/tigr.h>
#include <fcntl.h>
#include <errno.h>
#include <machine/cpu.h>
#include <machine/gr_board.h>

#ifndef TGIOGCTYPE
#define TGIOGCTYPE 44
#endif

static char graphic_type[200];

char *get_graphic_type (void)
{
   struct utsname name;
   extern char *sys_errlist[];
   extern int sys_nerr;

   uname(&name);
   if (!strcmp(name.machine, "3000") ||
       !strcmp(name.machine, "1500") ||
       !strcmp(name.machine, "titan")) { 
      int k;
      char *p, *cp;
      char buf[20];
      int fd;
      int slen;
      char iotype;

      for (k=1; k<10; k++) {
	 p = "/dev/tigrX";
	 slen = 9;

	 for (cp=buf; *cp++=*p++; );
	 cp = buf;
      
	 cp[slen] = k + '0';
	 if ((fd = open (cp, O_WRONLY)) >= 0) {
	    break;
	 } else if (EBUSY==errno) {
	    continue;
	 } else {
	    fprintf (stderr, "open of %s failed: (%d) %s\n",
		     cp, errno, 
		     errno < sys_nerr ? sys_errlist[errno] : "");
	    return NULL;
	 }
      }
      if ((iotype = ioctl(fd, TGIOGCTYPE, 0)) < 0) {
	 if (EINVAL == errno) {
	    /* old device driver -- G2 */
	    iotype = 0x0;
	 } else {
	    fprintf (stderr,
		     "ioctl (TGIOGCTYPE) failed: (%d) %s\n",
		     errno, errno < sys_nerr ? sys_errlist[errno] : "");
	    return NULL;
	 }
      }

      close(fd);

      if (0x0 == iotype) { /* G2 board */
	 sprintf (graphic_type, "G2");
      } else if (0x1 == iotype) { /* G3 board */
	 sprintf (graphic_type, "G3");
      }
   } else {
      sprintf (graphic_type, "%s", name.machine);
   }
   return graphic_type;
}
#endif
#endif
/* ======================================================================
			END OF X11-SPECIFIC CODE
   ====================================================================== */
