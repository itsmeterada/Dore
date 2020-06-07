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
 * This is the main program for the Dore' test environment.
 *
 * Compilation Define options:
 *	INC_SAMPLERND	- if set, will optionally include code to install
 *			  the Sample Renderer
 *	INC_SAMPLEDEV	- if set, will optionally include code to install
 *			  the Sample Device driver
 *	INC_STDX	- if set, will optionally include code to install
 *			  the Standard X11 device driver
 *	OFFSET_OUTLINE	- if set, will include code to do offsetting for
 *			  outline polygons only on the titan
 *	USE_TITANG2	- if set, OFFSET_OUTINE will modify the titang2
 *			  device driver otherwise it will modify the
 *			  DGL device driver
 *	TIME_TEST	- if set, will include code to do no draw timing
 *			  tests
 *
 *	use_X11		- if set, will include code to setup for the 
 *			  standard X11 device driver
 *	NO_SUNVIEW	- if set, will exclude code to setup for the
 *			  SunView device drivers on sun machines
 *	use_XFDI	- if set, will include code to setup for the
 *			  the x11_XFDI device driver
 *	use_GL		- if set, will include code to setup for the
 *			  Iris GL device driver
 *      use_DEC         - if set, will include code to setup for the
 *                        DEC PEX device driver.
 */

    /**************************/
    /***  Help Information  ***/
    /**************************/

static char help[] = "\
\n\
------------------------------------------------------ Transformations\n\
j/J k/K l/L - Rotation    +-X, +-Y, +-Z\n\
u/U i/I o/O - Translation +-X, +-Y, +-Z\n\
m/M ,/< ./> - Scaling     +-X, +-Y, +-Z\n\
=/+         - Select transformation callback group\n\
~1          - Reset transformation multiplier to one\n\
~2-~9       - Cumulative transformation multiplier\n\
------------------------------------------------------ Geometry\n\
s/S   - Subdivision Level +/-\n\
~S    - Hardware spheres on/off\n\
C     - Cycle cylinders tessellated/firm/firmnocap\n\
-/_   - Update variable object\n\
b     - Compute bounding volume of object group\n\
------------------------------------------------------ Rendering\n\
a/A   - Ambient intensity +/-\n\
d/D   - Diffuse intensity +/-\n\
2/@   - Cycle representation types\n\
3/#   - Cycle interpolation types\n\
4     - Specular highlight on/off\n\
5/%   - Cycle shading type (component, range)\n\
6/^   - Cycle rendering style\n\
R     - Render with raytracer\n\
Y     - Render with medialogic renderer\n\
~%/~^ - Cycle transparency\n\
h     - lightsource/constant surface shading\n\
------------------------------------------------------ Display\n\
/     - Super-sample on/off\n\
~D    - Dithering on/off\n\
~B    - Single/double buffering\n\
V     - Multiple views / One view\n\
------------------------------------------------------ Depth\n\
c     - Depth-cue on/off\n\
e/E   - Stereo eye separation\n\
f/F   - Stereo focal distance\n\
1     - Backface culling on/off\n\
z     - Hidden surface on/off\n\
Z     - Z-clear flag on/off\n\
~Z    - Z-buffering on/off\n\
------------------------------------------------------ Radiosity\n\
~U    - Change radiosity update type\n\
~C    - Change radiosity convergence criterion\n\
------------------------------------------------------ Updates\n\
W     - Unconditional update\n\
G     - Frame update\n\
g     - Always full-update on/off\n\
~+/~- - Full-update on/off\n\
t     - Time 100 rotations\n\
T     - Update timing flag on/off\n\
------------------------------------------------------ Input / Output\n\
n     - Re-read the dou file\n\
r/w   - Read/write values in \"values.dat\" file\n\
p/P   - Print object/camera group\n\
Q     - Get object's handle and type\n\
~P    - Write image data to file\n\
^E    - Create an environment map\n\
------------------------------------------------------ Miscellany\n\
!?    - Print help information for texture keys\n\
~N    - Reset to original values\n\
~R    - Reset callback values\n\
~0    - Sleep for one second\n\
x/q   - Exit\n\
\n\
";


    /**********************************/
    /***  Machine-Specific Defines  ***/
    /**********************************/

#if defined (titan) && !defined (use_X11)
#   define use_X11
#endif

#if defined (__alpha) && !defined (use_X11)
#   define use_X11
#endif

#if defined (titan) && !defined (NO_DGL)
#   define use_DGL
#endif

#if defined (titan) && defined(__P3__) && !defined (use_VLL)
#   define use_VLL
#endif

#if defined (__alpha) && !defined (use_VLL)
#   define use_VLL
#endif

#if defined (sun) && !defined (NO_SUNVIEW)
#   define use_SunView
#endif

#if defined OFFSET_OUTLINE && !defined(titan)
#   undef OFFSET_OUTLINE
#endif

#if defined OFFSET_OUTLINE && defined(USE_TITANG2)
#   if !defined(titan) || !defined(__P2__)
#      undef USE_TITANG2
#   endif
#endif

#if defined (stellar) && !defined (use_X11)
#   define use_X11
#endif

#if defined (stellar) && !defined (use_XFDI)
#   define use_XFDI
#endif

#if defined (sgi)
#   ifndef use_GL
#      define use_GL
#   endif
#   if !defined(use_X11) && !defined(NO_X11)
#      define use_X11
#   endif
#endif

#if defined(mips) && defined(ultrix)
#   ifndef use_DEC
#       define use_DEC
#   endif
#   if !defined(use_X11) && !defined(NO_X11)
#       define use_X11
#   endif
#endif

#if defined __alpha
#   define use_X11
#endif


    /*** Include Directives ***/

#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>

#ifdef OFFSET_OUTLINE
#   include <dogen.h>
#else
#   include <dore.h>
#endif

#ifdef TIME_TEST
#   include <dore_develop/develop.h>
#endif

#include "trnsfrmr.h"
#include "render.h"

#ifdef use_X11
#   include <X11/Xlib.h>
#endif

#ifdef titan
#   include <sys/sysmips.h>
#   include <sys/param.h>
#else
#   include <sys/time.h>
#endif

#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))

extern DtInt doe_system_timing_points_drawn;
extern DtInt doe_system_timing_lines_drawn;
extern DtInt doe_system_timing_connected_lines_drawn;
extern DtInt doe_system_timing_triangles_drawn;
extern DtInt doe_system_timing_mesh_triangles_drawn;
extern DtInt doe_system_timing_strip_triangles_drawn;
extern DtInt doe_system_timing_quads_drawn;
extern DtInt doe_system_timing_mesh_quads_drawn;
extern DtInt doe_system_timing_firmware_spheres_drawn;
extern DtInt doe_system_timing_firmware_cylinders_drawn;

#if (defined(use_DGL) || defined(use_VLL)) && defined(__P3__) && defined(GP_BRK)
    extern int _gp_break;
#endif

#if defined(titan) || defined(use_VLL) || defined(use_DGL)
    int use_XB = 0;
    int use_Xtitan = 0;
#endif

#define GRAPHIC_NONE	0x0
#define GRAPHIC_G2	0x1
#define GRAPHIC_G3	0x2
#define GRAPHIC_G4      0x4
int gr_type = GRAPHIC_NONE;

DtInt 		outline_offset 		= 100;
DtInt 		use_outline_offset 	= 0;

DtFlag		clearflag		= DcTrue;
DtInt		shadetype		= 1;

DtFlag		updvar1			= DcFalse;
DtFlag		updvar2			= DcFalse;

DtFlag		multiple_views		= DcFalse;
DtFlag		pseudocolor		= DcFalse;
DtSwitch	singlebuffered		= DcFalse;
DtSwitch	ztc			= DcFalse;
DtSwitch	hw_boundingvol		= DcFalse;
DtVisualType	visualtype		= DcDirectColor;
DtSwitch	stereoflag		= DcFalse;
DtSwitch	multi_pass_transp	= DcFalse;

DtSwitch	separate_disp_groups	= DcFalse;

DtRenderStyle	rendstyle		= DcRealTime;
DtInt		sampRendId;
DtSwitch 	use_sample_rnd		= DcFalse;
DtSwitch 	use_sample_dev		= DcFalse;
DtSwitch 	use_stdx		= DcFalse;
char		raster_output[200];
DtSwitch	use_stdin		= DcFalse;
DtSwitch	use_spheres		= DcFalse;
int		cylinders		= 0;  /* tesselated by default */
DtSwitch	time_update		= DcFalse;
DtSwitch	dither_flag		= DcFalse;

DtSwitch	cmp_bnd_vol		= DcFalse;
DtSwitch	logging_flag		= DcFalse;
DtSwitch	script_flag		= DcFalse;
FILE		*log_fd;
FILE		*scr_fd;

DtInt		list_size;
DtInt		index_size;
DtSwitch	reverse;
DtInt		xap;
DtInt		yap;

DtInt		*pick_index;
DtReal		*z_values;
DtReal		*wc_values;
DtReal		*lc_values;
DtObject	*views;
long 		*hit_list;

DtInt		testcase;
DtInt		use_zbuffer;
DtInt		show_picks;
DtInt		use_local_pick;
DtObject	pickgroup;
DtObject	sphere_obj;

DtInt		framecount;
long		initialtime;	/* Initial Time (Microseconds) */
struct tms	buffer;

DtInt		width, height;
DtInt		topx, topy;
Window		windowid = 0;

char		devicetype[200];
DtFlag		user_window;
DtFlag		use_X;
DtObject	device;
DtObject	frame;
DtObject	view;
DtObject	m_view[4];

DtVolume m_volume[4] = {{{ 0.0, 0.5, 0.0 }, { 0.5, 1.0, 1.0 }},
			{{ 0.5, 0.5, 0.0 }, { 1.0, 1.0, 1.0 }},
			{{ 0.0, 0.0, 0.0 }, { 0.5, 0.5, 1.0 }},
			{{ 0.5, 0.0, 0.0 }, { 1.0, 0.5, 1.0 }}};

char		objectgroupname[200];
char		cameragroupname[200];
char		m_objectgroupname[200];
char		m_cameragroupname[200];  /* multiple groups with single camera each */
char		mcamera_group_name[200]; /* single group m cameras */
DtColorRGB	backgroundcolor;

DtInt		gray_scale_size;
DtInt		shade_rng_size;
DtReal		gray_scale_map[4096*3];
DtReal		color_scale_map[4096*3];
DtReal		color1_scale_map[4096*3];
DtReal		shade_rng_map[4096*3];
DtInt		shade_ranges[2*9];

DtInt			use_rad;
DtRadiosityUpdateType	rad_updtype;
DtRadiosityConvType	rad_convtype;
double			tmpdouble;
DtReal			rad_convparam[1];
DtReal			rad_ffres[1];
DtInt			tmpint;

DtInt		step_translate = 1;	/* step size for translate */
DtInt		step_scale = 1;		/* step size for scale     */
DtInt		step_rotate = 1;	/* step size for rotate    */

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


DtObject	camera_group;
DtObject	camera_group1;
DtObject	object_group;
DtObject	object_group1;

DtSwitch multiply_lgt_intens = DcFalse;

DtInt	  num_rendstyles;
char 	**rendstyle_name;
DtInt	 *rendstyle_id;	
DtInt	  current_rendstyle;

char		input_filename[200];
DtFlag		check_groups;

int test_thread;

int snapshot_dev_exists = 0;
DtObject snapshot_device;

    /*******************************/
    /***  Function Declarations  ***/
    /*******************************/

extern char *print_visual ();
extern int   scenefile_read (char*);

void report_timing ();

/*
  ======================================================================
 */

int main (int argc, char *argv[])
{
   static char		logfile[200];
   static char		scrfile[200];
   static char		argstring[500];
   static char		tmpstring[200];
   static DtObject	background_raster = DcNullObject;
   static char		background_aif[200];
   static char		cam_name[10];
   static char		pcallback_name[80];
   static DtVolume	volume;
   static char		controlchar;
   static DtShadeMode	shademode;
   static DtInt	processors;
   static DtInt	i, j;
   static DtInt	shaderange;
   static DtVolume	t_volume;
   static DtReal	range;
   static DtSwitch	framecoord_switch;
   static int          frame_special;
   static DtSize3	aperture,check_aperture;
   static DtPickPathOrder check_path_order;
   static DtObject	check_pick_callback;
   static DtObject	pick_callback;
   static DtPtr 	version;
   static DtGroupNetworkStatus check_group_network();
   static char 	*cptr;
   extern char 	*strrchr();
   static DtSwitch	use_media_logic;	
   static DtSwitch	use_position;
   static DtObject	make_multiplecameras();
   static double       bgcolor[3];
   static DtInt        num_drivers;
   static char       **driver_name;
   static char       **driver_info;

   bgcolor[0] = 0.3;
   bgcolor[1] = 0.3;
   bgcolor[2] = 0.3;

   prsarg_check_help_argument (argc,argv);
   prsarg_get_keyword_int (argc,argv,"-framespecial", 0, &frame_special);
   prsarg_get_keyword_flag (argc,argv,"-nopos", DcTrue, &use_position);
   prsarg_get_keyword_string (argc,argv,"-bgdras", "", background_aif);
   prsarg_get_keyword_switch (argc,argv,"-use_X",1,&use_X);
   prsarg_get_keyword_switch (argc,argv,"-user_win",1,&user_window);
   prsarg_get_keyword_switch (argc,argv,"-multipass",0,&multi_pass_transp);
   prsarg_get_keyword_switch (argc,argv,"-stdin",0,&use_stdin);
   prsarg_get_keyword_switch (argc,argv,"-use_spheres",0,&use_spheres);
   prsarg_get_keyword_switch (argc,argv,"-cylinders",0,&cylinders);
   prsarg_get_keyword_switch (argc,argv,"-dither",0,&dither_flag);

   prsarg_get_keyword_int (argc, argv, "-thread", 0, &test_thread);

   prsarg_get_keyword_colorRGB
      (argc,argv,"-background", bgcolor, bgcolor);

   backgroundcolor[0] = bgcolor[0];
   backgroundcolor[1] = bgcolor[1];
   backgroundcolor[2] = bgcolor[2];

   prsarg_get_keyword_string (argc,argv, "-outfile", "test.img", raster_output);

   prsarg_get_keyword_int (argc,argv,"-show_picks",0,&show_picks);
   prsarg_get_keyword_int (argc,argv,"-local_picks",0,&use_local_pick);

   prsarg_get_keyword_int (argc,argv,"-zbuffer",0,&use_zbuffer);
   prsarg_get_keyword_int (argc,argv,"-test",0,&testcase);
   prsarg_get_keyword_switch (argc,argv,"-medialogic",1,&use_media_logic);
   prsarg_get_keyword_switch (argc,argv,"-bndvol",0,&cmp_bnd_vol);
   prsarg_get_keyword_switch (argc,argv,"-samplernd",0,&use_sample_rnd);
   prsarg_get_keyword_switch (argc,argv,"-sampledev",0,&use_sample_dev);
   prsarg_get_keyword_switch (argc,argv,"-stdx11",0,&use_stdx);
   prsarg_get_keyword_switch (argc,argv,"-mullgtint",0,&multiply_lgt_intens);

   prsarg_get_keyword_int (argc,argv,"-ooffset", 100, &outline_offset);
   prsarg_get_keyword_int (argc,argv,"-use_ooffset",0,&use_outline_offset);

   cptr = ((cptr = strrchr (argv[0], '/')) == NULL) ? argv[0] : cptr + 1;
   if (strcmp (cptr, "penny") == 0) {
      prsarg_get_keyword_string (argc,argv,"-f","penny.dou",input_filename);
   } else if (strcmp (cptr, "foodpros") == 0) {
      prsarg_get_keyword_string (argc,argv,"-f","foodpros.dou",input_filename);
   } else {
      prsarg_get_keyword_string (argc,argv,"-f","test.dou",input_filename);
   }
   prsarg_get_keyword_string (argc,argv,"-c","cameragroup",cameragroupname);
   prsarg_get_keyword_string(argc,argv,"-g","objectgroup",objectgroupname);

   prsarg_get_keyword_string(argc,argv,"-mc","cameragroup%d",m_cameragroupname);
   prsarg_get_keyword_string(argc,argv,"-mg","objectgroup%d",m_objectgroupname);

   prsarg_get_keyword_int  (argc,argv, "-width", 512, &width);
   prsarg_get_keyword_int  (argc,argv, "-height", 512, &height);
   prsarg_get_keyword_int  (argc,argv, "-topx", 0, &topx);
   prsarg_get_keyword_int  (argc,argv, "-topy", 0, &topy);
   prsarg_get_keyword_long (argc,argv, "-windowid", 0, &windowid);

   prsarg_get_keyword_switch(argc,argv,"-singlebuffered",0,&singlebuffered);
   prsarg_get_keyword_switch(argc,argv,"-ztc",0,&ztc);
   prsarg_get_keyword_switch(argc,argv,"-bvol",0,&hw_boundingvol);
   prsarg_get_keyword_switch(argc,argv,"-stereo",0,&stereoflag);

   prsarg_get_keyword_switch(argc,argv,"-multigroup",0,&separate_disp_groups);

/* currently not implemented
   prsarg_get_keyword_string(argc,argv,"-mcameragroup","multiplecameras",mcamera_group_name);
*/

   prsarg_get_keyword_switch(argc,argv,"-log",0,&logging_flag);
   prsarg_get_keyword_switch(argc,argv,"-scr",0,&script_flag);
   prsarg_get_keyword_string(argc,argv,"-logfile","test.scr",logfile);
   prsarg_get_keyword_string(argc,argv,"-scrfile","test.scr",scrfile);

   prsarg_get_keyword_switch(argc,argv,"-check",0,&check_groups);

   prsarg_get_keyword_string(argc,argv,"-shademode","DcComponent",tmpstring);
   if (strcmp(tmpstring, "DcComponent") == 0) {
      shademode = DcComponent;
   } else if (strcmp (tmpstring, "DcRange") == 0) {
      shademode = DcRange;
   } else {
      printf ("bad shade mode: '%s'\n", tmpstring);
      exit (1);
   }

   prsarg_get_keyword_int(argc,argv,"-shaderange",1,&shaderange);
   shaderange = shaderange != 1 && shaderange != 8 ? 1 : shaderange;

   prsarg_get_keyword_string(argc,argv,"-df","",argstring);

#  if defined(use_VLL) || defined(use_DGL)
   {
#     if !defined(titan)
	 gr_type = GRAPHIC_G4;
#     else
      {
	 char *st;
	 extern char *get_graphic_type();

	 st = get_graphic_type();
	 if (!strcmp(st, "G2")) {
	    gr_type = GRAPHIC_G2;
	 } else if (!strcmp(st, "G3")) {
	    gr_type = GRAPHIC_G3;
	 } else if (!strcmp(st, "G4")) {
	    gr_type = GRAPHIC_G4;
	 }
      }
#     endif
      prsarg_get_keyword_string (argc, argv, "-dt", "display", devicetype);
      prsarg_get_keyword_string
      (  argc, argv, "-visualtype",
	 (GRAPHIC_G2 == gr_type) ? "DcDirectColor" : "DcTrueColor",
	 tmpstring
      );
   }
#  else
#  ifdef use_XFDI
   {  prsarg_get_keyword_string (argc,argv,"-dt","stardentx11",devicetype);
      prsarg_get_keyword_string
	 (argc,argv,"-visualtype","DcTrueColor",tmpstring);
   }
#  else 
#  ifdef use_GL
   {  prsarg_get_keyword_string (argc,argv,"-dt","gl",devicetype);
      prsarg_get_keyword_string
	 (argc,argv,"-visualtype","DcTrueColor",tmpstring);
   }
#  else
#  ifdef use_DEC
   {  prsarg_get_keyword_string (argc,argv,"-dt","pex",devicetype);
      prsarg_get_keyword_string
	 (argc,argv,"-visualtype","DcTrueColor",tmpstring);
   }
#  else
#  ifdef use_X11
   {  prsarg_get_keyword_string (argc,argv,"-dt","stdx11",devicetype);
      prsarg_get_keyword_string
	 (argc,argv,"-visualtype","DcPseudoColor",tmpstring);
   }
#  else 
#  ifdef use_SunView
   {  prsarg_get_keyword_string (argc,argv,"-dt","sunviewGP",devicetype);
      prsarg_get_keyword_string
	 (argc,argv,"-visualtype","DcPseudoColor",tmpstring);
   }
#  else
   {  prsarg_get_keyword_string (argc,argv,"-dt","rasterfile",devicetype);
      prsarg_get_keyword_string
	 (argc,argv,"-visualtype","DcPseudoColor",tmpstring);
   }
#  endif
#  endif
#  endif
#  endif
#  endif
#  endif

   if (strcmp(tmpstring, "DcPseudoColor") == 0) {
      visualtype = DcPseudoColor;
   } else if (strcmp(tmpstring, "DcStaticGrey") == 0) {
      visualtype = DcStaticGrey;
   } else if (strcmp(tmpstring, "DcDirectColor") == 0) {
      visualtype = DcDirectColor;
   } else if (strcmp(tmpstring, "DcTrueColor") == 0) {
      visualtype = DcTrueColor;
   } else {
      visualtype = DcDirectColor;
   }

   prsarg_get_keyword_switch(argc,argv,"-fc",0,&framecoord_switch);
   prsarg_get_keyword_int(argc,argv,"-procs",0,&processors);

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

   /* pick options */
   prsarg_get_keyword_int(argc,argv,"-ls",1024,&list_size);
   prsarg_get_keyword_int(argc,argv,"-is",10,&index_size);
   prsarg_get_keyword_int(argc,argv,"-xap",20,&xap);
   prsarg_get_keyword_int(argc,argv,"-yap",16,&yap);
   prsarg_get_keyword_switch(argc,argv,"-rp",0,&reverse);
   prsarg_get_keyword_string(argc,argv,"-pcallback","DcPickAll",pcallback_name);

   pick_index = (DtInt *) malloc (index_size * sizeof (DtInt));
   z_values = (DtReal *) malloc (index_size * sizeof (DtReal));
   wc_values = (DtReal *) malloc (index_size*3 * sizeof (DtReal));
   lc_values = (DtReal *) malloc (index_size*3 * sizeof (DtReal));
   views = (DtObject *) malloc (index_size * sizeof (DtObject));
   hit_list = (long *) malloc (list_size * sizeof (DtInt));
   if (pick_index == NULL || 
       z_values == NULL || 
       wc_values == NULL || 
       lc_values == NULL || 
       views == NULL || 
       hit_list == NULL) {
      fprintf (stderr, "memory allocation failed for picking data\n");
      exit (1);
   }

   /* begin radiosity options */
   prsarg_get_keyword_int(argc,argv,"-use_rad", 0, &use_rad);
   prsarg_get_keyword_string(argc,argv,"-rad_updtype","DcRadiosityComputeDisp" ,
				 tmpstring);
   if (strcmp(tmpstring, "DcRadiosityNone") == 0) {
      rad_updtype = DcRadiosityNone;
   } else if (strcmp (tmpstring, "DcRadiosityComputeDisp") == 0) {
      rad_updtype = DcRadiosityComputeDisp;
   } else if (strcmp (tmpstring, "DcRadiosityDisp") == 0) {
      rad_updtype = DcRadiosityDisp;
   } else {
      printf ("bad radiosity update type '%s'\n", tmpstring);
      exit(1);
   }
   prsarg_get_keyword_string(argc,argv,"-rad_ctype","DcConvGlobalRel" ,
				 tmpstring);
   if (strcmp(tmpstring, "DcConvFixedIterations") == 0) {
      rad_convtype = DcConvFixedIterations;
   } else if (strcmp (tmpstring, "DcConvGlobalRel") == 0) {
      rad_convtype = DcConvGlobalRel;
   } else if (strcmp (tmpstring, "DcConvGlobalAbs") == 0) {
      rad_convtype = DcConvGlobalAbs;
   } else if (strcmp (tmpstring, "DcConvPatchRel") == 0) {
      rad_convtype = DcConvPatchRel;
   } else if (strcmp (tmpstring, "DcConvPatchAbs") == 0) {
      rad_convtype = DcConvPatchAbs;
   } else {
      printf ("bad radiosity convergence type '%s'\n", tmpstring);
      exit(1);
   }
   /* note: prsarg_get_keyword_real needs a double as argument */
   prsarg_get_keyword_real(argc,argv,"-rad_cval", 0.1, &tmpdouble);
   rad_convparam[0] = tmpdouble;

   prsarg_get_keyword_int(argc,argv,"-rad_ffres", 128, &tmpint);
   rad_ffres[0] = tmpint;
   /* end radiosity options */

   /* At this point we've processed all of the options we know about.  Now
   // we collect all of the command-line arguments that are left over,
   // concatenate them, and pass them directly to the device driver.  */

   for (i=1;  i < argc;  ++i) {
      if (!argv[i][0]) continue;
      if (argstring[0])
	  strncat (argstring, " ", sizeof(argstring) - strlen(" ") - 1);
      strncat (argstring, argv[i], sizeof(argstring) - strlen(argv[i]) - 1);
   }

   if (logging_flag) {
      if ((log_fd = fopen(logfile, "w")) == NULL) {
	 fprintf (stderr, "cannot open '%s' for writing\n", logfile);
	 perror ("");
	 logging_flag = DcFalse;
      }
   }

   if (script_flag) {
      if ((scr_fd = fopen(scrfile, "r")) == NULL) {
	 fprintf (stderr, "cannot open '%s' for reading\n", scrfile);
	 perror ("");
	 script_flag = DcFalse;
      }
   }

/*
#  ifdef titan
   {  if (processors > 0)
	  start_server();
   }
#  endif
*/

   DsInitialize (processors);
   if (use_rad)
       DsRadiosityInitialize();

   DsInqVersion (&version);
   printf ("Version '%s'\n", version);

#  ifdef INC_SAMPLERND
   {  if (use_sample_rnd) 
   	 sampRendId = drr_samplernd_install_renderer(DcRendererNewId);
   }
#  endif

   num_rendstyles = DsInqNumRenderers();
   rendstyle_name = (char **) malloc (num_rendstyles * sizeof(char*));
   rendstyle_id = (DtInt *) malloc (num_rendstyles * sizeof(DtInt));

   DsInqRendererNames (rendstyle_name);
   for (i=0; i<num_rendstyles; i++) {
      rendstyle_id[i] = DsInqRendererId(rendstyle_name[i]);
      if (rendstyle == rendstyle_id[i]) current_rendstyle = i;
   }

   print ("\nInstalled Renderers:\n");
   for (i=0; i<num_rendstyles; i++) {
      printf ("\t%d: '%s'\n", rendstyle_id[i], rendstyle_name[i]);
   }

#  ifdef INC_SAMPLEDEV
   {  if (use_sample_dev) 
	 ddr_sampledev_install_driver("sampledev");
   }
#  endif

#  ifdef INC_STDX
   {  if (use_stdx) 
	 ddr_stdx11_install_driver("stdx11");
   }
#  endif

   {  num_drivers = DsInqNumDrivers();
      driver_name = (char **) malloc (num_drivers * sizeof(char*));
      driver_info = (char **) malloc (num_drivers * sizeof(DtPtr));

      DsInqDriverInfo (num_drivers, driver_name, driver_info);
      print ("\nInstalled Drivers:\n");
      for (i=0; i<num_drivers; i++) {
	  printf ("\t%-15s  '%s'\n", driver_name[i], driver_info[i]);
      }
      free (driver_name);
      free (driver_info);
   }


#if defined(use_DGL) || defined(use_VLL)
   if(!strcmp (devicetype, "display") ||
      !strcmp (devicetype, "localx11") ||
      !strncmp (devicetype, "ardent", 6) ||
      !strcmp (devicetype, "stardentx11")) {
      if (user_window) {
	  if (GRAPHIC_G2 == gr_type) {
	      use_XB = 1;
	      use_Xtitan = 1;
	  } else if (GRAPHIC_G3 == gr_type) {
	      use_XB = 1;
	      use_Xtitan = 0;
	  } else /* GRAPHIC_G4 */ {
	      use_XB = 0;
	      use_Xtitan = 0;
	  }
	  setup_x11_window (topx, topy, width, height, argstring);
      } else {
	  use_stdin = DcTrue;
	  if (gr_type == GRAPHIC_G4) {
	     sprintf (&(argstring[strlen(argstring)]),
		  " -geometry %dx%d+%d+%d -visualtype %s %s%s%s%s%s%s%s",
		  width,height,topx,topy,
		  print_visual(visualtype),
		  singlebuffered       ? " -singlebuffered "           : "",
		  ztc                  ? " -ztc "                      : " -noztc ",
		  hw_boundingvol       ? " -boundingvol "              : "",
		  stereoflag           ? " -stereo "                   : "",
		  multi_pass_transp==1 ? " -multipass_transparency "   :
		  multi_pass_transp==2 ? " -sort_transparency "        : "",
		  use_spheres          ? " -spheres "                  : "",
		  cylinders==1 	       ? " -cylinders "   	       :
		  cylinders==2         ? " -cylindersnocap "           : "",
		  dither_flag          ? " -dither "                   : ""
		  );
	  } else {
             /* G2/G3 device */
	     sprintf (&(argstring[strlen(argstring)]), 
		   " -geometry %dx%d+%d+%d -visualtype %s %s%s%s%s%s%s",
		   width,height,topx,topy,
		   print_visual(visualtype),
		   singlebuffered	? " -singlebuffered " 		: "",
		   stereoflag	 	? " -stereo " 			: "",
		   multi_pass_transp==1? " -multipass_transparency " 	: 
		   multi_pass_transp==2? " -sort_transparency "		: "",
		   use_spheres		? " -spheres " 			: "",
		   use_zbuffer		? " -zbuffer "			: "",
		   dither_flag		? " -dither "			: ""
		   );
	  }
      }
   }
   if (strcmp (devicetype, "dgl") == 0) {
      if (user_window) {
	 use_XB = 1;
	 use_Xtitan = 1;
	 setup_x11_window(topx, topy, width, height, argstring);
      } else {
	 use_stdin = DcTrue;
	 sprintf (&(argstring[strlen(argstring)]), 
		  " -geometry %dx%d+%d+%d -visualtype %s %s%s%s%s%s%s",
		  width,height,topx,topy,
		  print_visual(visualtype),
		  singlebuffered	? " -singlebuffered " 		: "",
		  stereoflag	 	? " -stereo " 			: "",
		  multi_pass_transp==1? " -multipass_transparency " 	: 
		  multi_pass_transp==2? " -sort_transparency "		: "",
		  use_spheres		? " -spheres " 			: "",
		  use_zbuffer		? " -zbuffer "			: "",
		  dither_flag		? " -dither "			: ""
		  );
      }
   }

#endif

#ifdef use_VLL
   if ((0 == strcmp (devicetype, "g3")) || (0 == strcmp (devicetype, "vll"))) {
      if (use_X && user_window) {
	 use_XB = 0;
	 use_Xtitan = 0;
	 setup_x11_window(topx, topy, width, height, argstring);
	 sprintf (&(argstring[strlen(argstring)]), " -X ");
      } else {
	 use_stdin = DcTrue;
	 sprintf (&(argstring[strlen(argstring)]),
		  " -geometry %dx%d+%d+%d -visualtype %s %s%s%s%s%s",
		  width, height, topx, topy,
		  print_visual(visualtype),
		  singlebuffered	 ? " -singlebuffered "		: "",
		  stereoflag		 ? " -stereo " 			: "",
		  multi_pass_transp == 1 ? " -multipass_transparency " 	: 
		  multi_pass_transp == 2 ? " -sort_transparency"  	: "",
		  use_spheres		 ? " -spheres " 		: "",
		  dither_flag		 ? " -dither "	 		: "",
		  use_X         	 ? " -X "                       : " -noX ");
      }
   /* when device is called with name "g4vll", then the
      driver does not use X */
   } else if (0 == strcmp (devicetype, "g4vll")) {
      if (use_X && user_window) {
         use_XB = 0;
         use_Xtitan = 0;
         setup_x11_window(topx, topy, width, height, argstring);
         sprintf (&(argstring[strlen(argstring)]), " -X ");
      } else {
         use_stdin = DcTrue;
         sprintf (&(argstring[strlen(argstring)]),
	     " -geometry %dx%d+%d+%d -windowid %ld -visualtype %s %s%s%s%s%s%s",
	     width, height, topx, topy, windowid, print_visual(visualtype),
	     singlebuffered         ? " -singlebuffered "         : "",
	     ztc                    ? " -ztc "                    : " -noztc ",
	     hw_boundingvol         ? " -boundingvol "            : "",
	     stereoflag             ? " -stereo "                 : "",
	     (multi_pass_transp==1) ? " -multipass_transparency " :
	     (multi_pass_transp==2) ? " -sort_transparency "      : "",
	     use_spheres            ? " -spheres "                : "",
	     cylinders==1 	    ? " -cylinders "   	          :
	     cylinders==2           ? " -cylindersnocap "         : "",
             dither_flag            ? " -dither "                 : "",
             use_X                  ? " -X "                      : "");
      }
    }
#endif

#ifdef use_X11
   if (strncmp (devicetype, "stdx11", 6) == 0) {
      if (user_window) {
	 setup_x11_window(topx, topy, width, height, argstring);
      } else {
	 use_stdin = DcTrue;
	 sprintf (&(argstring[strlen(argstring)]), 
		  " -geometry %dx%d+%d+%d -visualtype %s %s%s%s%s%s%s",
		  width,height,topx,topy,
		  print_visual(visualtype),
		  singlebuffered	? " -singlebuffered " 		: "",
		  stereoflag	 	? " -stereo " 			: "",
		  multi_pass_transp==1? " -multipass_transparency " 	: 
		  multi_pass_transp==2? " -sort_transparency "		: "",
		  use_spheres		? " -spheres " 			: "",
		  use_zbuffer		? " -zbuffer "			: "",
		  dither_flag		? " -dither "			: ""
		  );
      }
   }
#endif

#ifdef use_SunView
   if (strncmp (devicetype, "sunview", 7) == 0) {
      setup_sunview_window(topx, topy, width, height, argstring);
   }
#endif

#ifdef use_XFDI
   if (strcmp (devicetype, "stardentx11") == 0) {
      if (user_window) {
	 setup_x11_window(topx, topy, width, height, argstring);
      } else {
	 use_stdin = DcTrue;
	 sprintf (&(argstring[strlen(argstring)]), 
	    " -geometry %dx%d+%d+%d -visualtype %s %s%s%s%s",
	    width,height,topx,topy,
	    print_visual(visualtype),
	    singlebuffered	? " -singlebuffered " 		: "",
	    stereoflag	 	? " -stereo " 			: "",
	    multi_pass_transp==1? " -multipass_transparency " 	: 
	    multi_pass_transp==2? " -sort_transparency "	: "",
	    cylinders==1 	? " -cylinders "   	        :
	    cylinders==2        ? " -cylindersnocap "           : "",
	    use_spheres		? " -spheres " 			: "");
      }
   }
#endif

#ifdef use_GL
   if (strncmp(devicetype, "gl", 2) == 0) {
      if (use_X && user_window) {
	 setup_x11_window(topx, topy, width, height, argstring);
	 strcat (argstring + strlen(argstring), " -useX");
      } else {
	 sprintf (&(argstring[strlen(argstring)]),
	    " -width %d -height %d -visualtype %s %s",
	    width, height, print_visual(visualtype), 
	    singlebuffered ? " -singlebuffered " : "");
	 if (user_window) {
	    int userwindow = -1;

	    userwindow = setup_iris_window(topx,topy,width,height,use_position);
	    if (userwindow >= 0) {
	       sprintf (&(argstring[strlen(argstring)]),
		  " -window %d ", userwindow);
	    } else {
	       use_stdin = DcTrue;
	    }
	 } else {
	    use_stdin = DcTrue;
	 }
      }
   }
#endif

#ifdef use_DEC
   if (0 == strcmp(devicetype, "pex")) {
      if (user_window) {
	 setup_x11_window (topx, topy, width, height, argstring);
      } else {
	 use_stdin = DcTrue;
	 sprintf (&(argstring[strlen(argstring)]),
		  " -geometry %dx%d+%d+%d -visualtype %s%s",
		  width, height, topx, topy,
		  print_visual(visualtype),
		  singlebuffered ? " -singlebuffered" : "");
      }
   }
#endif

   if (strcmp (devicetype, "rasterfile") == 0) {
      sprintf (argstring, "-width %d -height %d -filename %s", width, height,
	       raster_output);
   }

   if (!strcmp (devicetype, "ps")) {
      sprintf
      (  &(argstring[strlen(argstring)]),
	 "-width %f -height %f -filename %s -visualtype %s", 
	 (float)(width) / 72.0, (float)(height) / 72.0, raster_output,
	 print_visual (visualtype)
      );
   }

   /* Create studio and display groups through the scenefile interperter
   ** after creating objects that may be referenced by the scenfile */

   makeobjects ();

   create_callback_functions ();

   if (0 == scenefile_read (input_filename))
      exit (1);

   object_group = DsInqObjViaString (objectgroupname, DcTypeGroup);
   camera_group = DsInqObjViaString (cameragroupname, DcTypeGroup);
   DsHoldObj (object_group);
   DsHoldObj (camera_group);

   if (!separate_disp_groups) {
      DgOpen (camera_group, DcFalse);
      DgSetElePtr (0, DcBeginning);
      DgAddObj (make_multiplecameras());
      DgClose ();
   }

   printf("DoDevice(\"%s\", \"%s\")\n", devicetype, argstring);
   /*exit (0);*/

   if (!(device = DoDevice(devicetype,argstring))) {
      printf
      (  "Can't create device - device type = \"%s\", device file = \"%s\"\n",
	 devicetype,
	 argstring
      );
      exit(1);
   }

   /* setup default color map if device hasn't already */
   setup_colormaps(DdInqColorTableSize(device));

   if (DdInqVisualType(device) == DcPseudoColor) {
      pseudocolor = DcTrue;
      DdSetShadeRanges(device, 0, 9, shade_ranges);
      switch (shademode) {
      case DcComponent:
	 shadetype = 0;
	 DdSetShadeMode(device, DcComponent);
	 DdSetColorEntries(device, DcRGB, 0, 256, color_scale_map);
	 break;
      case DcRange:
	 DdSetShadeMode(device, DcRange);
	 if (shaderange == 1) {
	    shadetype = 2;
	    DdSetColorEntries (device, DcRGB,0, gray_scale_size,gray_scale_map);
	 } else {
	    shadetype = 3;
	    DdSetColorEntries (device, DcRGB, 0, shade_rng_size,shade_rng_map);
	 }
	 break;
      }
   }

   DdInqExtent (device, &volume);

   DdSetViewport(device,&volume);

   if (!(frame = DoFrame())) {
      print ("Can't create frame.\n");
      exit(1);
   }

   DsHoldObj(frame);

   DdSetFrame(device,frame);

   switch (frame_special) {
       case 1:
	   DfInqBoundary(frame, &volume);
	   volume.bll[0] = 0.0;	
	   volume.bll[1] = 0.0;

	   volume.fur[0] = 2.;
	   volume.fur[1] = 1.;
	   DfSetBoundary(frame, &volume);
	   volume.fur[0] = 3.;
	   break;

       case 2:
	   DfInqBoundary(frame, &volume);
	   volume.bll[0] = 0.0;	
	   volume.bll[1] = 0.0;

	   volume.fur[0] = 1.;
	   volume.fur[1] = 2.;
	   DfSetBoundary(frame, &volume);
	   volume.fur[1] = 3.;
	   break;

       default:
	   if (framecoord_switch) {
	       DfInqBoundary(frame,&volume);
	   } else {
	       DfSetBoundary(frame,&volume);
	   }
	   break;
   }

   if (!(view = DoView())) {
      print ("Can't create view.\n");
      exit(1);
   }

   DsHoldObj(view);

   if (background_aif[0] != '\0') {
      DtInt width;
      DtInt height;
      DtInt depth;
      DtRasterType type;
      DtPtr data;
      
      if (DsFileRasterRead(background_aif, &width, &height, 
			   &depth, &type, &data) != -1) {
	 background_raster = DoRaster(width, height, depth, type,
				      DcNullPtr, data, DcDeleteData);
      } else {
	 background_raster = DcNullObject;
      }
   }

   if (background_raster != DcNullObject) {
      DvSetBackgroundRaster(view, background_raster, DcNullObject);
   }

   DvSetClearFlag(view,DcTrue);
   DvSetBackgroundColor(view,DcRGB,backgroundcolor);
   DvSetRendStyle(view,rendstyle_id[current_rendstyle]);
   if (!multiple_views) {
      DgAddObjToGroup(DfInqViewGroup(frame),view);
   }
   DvSetBoundary(view,&volume);

   /* set radiosity options */
   if (use_rad){
      DvSetRadiosityUpdateType(view, rad_updtype);
      DvSetFormFactorCompType(view, DcFormFactorHemicube, rad_ffres);
      DvSetRadiosityConvergence(view, rad_convtype, rad_convparam);
   }

   for (i=0; i<4; i++) {
      DtColorRGB background;

      if (!(m_view[i] = DoView())) {
	 print ("Can't create view.\n");
	 exit(1);
      }

      DsHoldObj(m_view[i]);

      background[0] = backgroundcolor[0];
      background[1] = backgroundcolor[1];
      background[2] = backgroundcolor[2];
      background[i%3] *= .5;

      for (j=0; j<3; j++) {
	 range = volume.fur[j] - volume.bll[j];
	 t_volume.bll[j] = volume.bll[j] + range*m_volume[i].bll[j];
	 t_volume.fur[j] = volume.bll[j] + range*m_volume[i].fur[j];
      }
      DvSetClearFlag(m_view[i],DcTrue);
      DvSetBackgroundColor(m_view[i],DcRGB,background);
      if (background_raster != DcNullObject) {
	 DvSetBackgroundRaster(m_view[i], background_raster, DcNullObject);
      }
      DvSetRendStyle(m_view[i],rendstyle_id[current_rendstyle]);
      DvSetBoundary(m_view[i],&t_volume);
      if (multiple_views) {
	 DgAddObjToGroup(DfInqViewGroup(frame),m_view[i]);
      }
      /* set radiosity options */
      if (use_rad) {
         /* since we know that every view has the same objects, we
            can compute the radiosity values only for one */
         if (i>0 && rad_updtype == DcRadiosityComputeDisp) 
             DvSetRadiosityUpdateType(m_view[i], DcRadiosityDisp);
	 else
             DvSetRadiosityUpdateType(m_view[i], rad_updtype);
         DvSetFormFactorCompType(m_view[i], DcFormFactorHemicube, rad_ffres);
         DvSetRadiosityConvergence(m_view[i], rad_convtype, rad_convparam);
     }
   }
   
   if (cmp_bnd_vol) {
      DsCompBoundingVol(&volume, object_group);
      printf("bounding volume:\n\t(%f, %f, %f) to\n\t(%f, %f, %f)\n\n",
	     volume.bll[0], volume.bll[1], volume.bll[2],
	     volume.fur[0], volume.fur[1], volume.fur[2]);
   }

   if (check_groups) {
      if (check_group_network(objectgroupname, object_group) != DcGroupOk) {
	 DsTerminate ();
	 exit (1);
      }
      if (check_group_network(cameragroupname, camera_group) != DcGroupOk) {
	 DsTerminate ();
	 exit (1);
      }
   }

   DgAddObjToGroup(DvInqDisplayGroup(view),object_group);
   DgAddObjToGroup(DvInqDefinitionGroup(view),camera_group);

   if (show_picks) {
      DtReal color[3];
      color[0] = color[1] = color[2] = .1;
      DgOpen (DvInqDisplayGroup(view),DcFalse);
         DgSetElePtr(0,DcBeginning);
         DgAddObj(pickgroup = DoGroup(DcFalse));
      DgClose();
      sphere_obj = DoGroup (DcTrue);
         DgAddObj (DoPickSwitch(DcOff));
         DgAddObj (DoRepType(DcSurface));
         DgAddObj (DoDiffuseColor(DcRGB, color));
	 DgAddObj (DoScale(.1,.1,.1));
	 DgAddObj (DoPrimSurf(DcSphere));
      DsHoldObj(DgClose());
   }

   if (!separate_disp_groups) {
      camera_group1 = camera_group;
      object_group1 = object_group;
   }

   for (i=0; i<4; i++) {
      if (separate_disp_groups) {
	 sprintf (tmpstring, m_objectgroupname, i+1);
	 object_group1 = DsInqObjViaString (tmpstring, DcTypeGroup);
	 DsHoldObj(object_group1);
	 if (check_groups) {
	    if (check_group_network(tmpstring, object_group1) != DcGroupOk) {
	       DsTerminate ();
	       exit (1);
	    }
	 }
	 sprintf (tmpstring, m_cameragroupname, i+1);
	 camera_group1 = DsInqObjViaString (tmpstring, DcTypeGroup);
	 DsHoldObj(camera_group1);
	 if (check_groups) {
	    if (check_group_network(tmpstring, camera_group1) != DcGroupOk) {
	       DsTerminate ();
	       exit (1);
	    }
	 }
      }
      DgAddObjToGroup(DvInqDefinitionGroup(m_view[i]),camera_group1);
      DgAddObjToGroup(DvInqDisplayGroup(m_view[i]),object_group1);
      if (!separate_disp_groups) {
	 sprintf (cam_name, "camera%d", i+1);
	 DvSetActiveCamera (m_view[i],DsInqObjViaString(cam_name,DcTypeCamera));
      }
   }

   /* set the pick order and check */

   DdSetPickPathOrder(device,reverse ? DcBottomFirst : DcTopFirst);
   check_path_order = DdInqPickPathOrder(device);

   if (check_path_order != (reverse ? DcBottomFirst : DcTopFirst)) {
      print ("Got bad value from DdInqPickPathOrder.\n");
      exit(1);
   }

   /* set the pick aperture and check. */

   aperture.width = (DtReal)xap;
   aperture.height = (DtReal)yap;
   aperture.depth = 400000.;
   DdSetPickAperture(device,&aperture);
   DdInqPickAperture(device,&check_aperture);

   if ((check_aperture.width  != aperture.width)  ||
       (check_aperture.height != aperture.height) ||
       (check_aperture.depth  != aperture.depth)) {
      print ("Got bad value from DdInqPickAperture.\n");
      exit(1);
   }

   /* set the pick callback and check. */
   if (strcmp(pcallback_name,"DcPickFirst") == 0) {
      pick_callback = DcPickFirst;
   } else if (strcmp(pcallback_name,"DcPickClosest") == 0) {
      pick_callback = DcPickClosest;
   } else if (strcmp(pcallback_name,"DcPickAll") == 0) {
      pick_callback = DcPickAll;
   } else {
      fprintf (stderr, "unknown pickcallback '%s'\n", pcallback_name);
      exit(1);
   }

   DdSetPickCallback(device,pick_callback);
   check_pick_callback = DdInqPickCallback(device);

   if (check_pick_callback != pick_callback) {
      print ("Got bad value from DdInqPickCallback.\n");
      exit(1);
   }


   DdUpdate(device);

   DvSetUpdateType(view,DcUpdateDisplay);
   for (i=0; i<4; i++) {
      DvSetUpdateType(m_view[i],DcUpdateDisplay);
   }

#ifdef titan
   printf ("Distinquished thread running on processor id %d\n",
	   sysmips(WHICHPROC,0,0,0));
#endif

#ifdef use_SunView
   if (strncmp (devicetype, "sunview", 7) == 0) {
       run_sunview(pcallback_name);
   }
#endif
   if (script_flag) {
      while ((controlchar = fgetc(scr_fd)) != EOF) {
	 if (process_character(controlchar)) {
	    break;
	 }
      }
   }
#ifdef TIME_TEST
   if (testcase > 0) {
      do_timing_test();
   }
#endif
   if (use_stdin) {
      while (1) {
	 controlchar = getchar();
	 if (controlchar == EOF ||
	     process_character(controlchar))
	       break;
      }
   } else
#ifdef use_X11
   if (strncmp (devicetype, "stdx11", 6) == 0) {
      printf ("%s Picking enabled on Left Mouse Button.\n",pcallback_name);
      x11_process_input();
   } else
#ifdef use_XFDI
   if (strcmp (devicetype, "stardentx11") == 0) {
      printf ("%s Picking enabled on Left Mouse Button.\n",pcallback_name);
      x11_process_input();
   }  else
#endif /* use_XFDI */
#if defined(use_DGL) ||  defined(use_VLL)
   if(!strncmp (devicetype, "ardent", 6) ||
      !strcmp(devicetype, "stardentx11") ||
      (!strcmp(devicetype, "dgl") && user_window) ||
      (!strcmp(devicetype, "display") && use_X) ||
      (!strcmp(devicetype, "g3") && use_X)) {
      printf ("%s Picking enabled on Left Mouse Button.\n",pcallback_name);
      x11_process_input();
   }  else
#endif /* use_DGL || use_VLL */
#endif /* use_X11 */

#ifdef use_GL
   if (!strncmp (devicetype, "gl", 2) && user_window) {
      printf ("%s Picking enabled on Left Mouse Button.\n",pcallback_name);
      if (use_X)
	 x11_process_input();
      else
         iris_process_input();
   } else
#endif /* use_GL */

#ifdef use_DEC
   if (0 == strcmp (devicetype, "pex")) {
      printf ("%s Picking enabled on Left Mouse Button.\n", pcallback_name);
      x11_process_input();
   }  else
#endif /* use_DEC */

   {
      print ("Enter any character to begin display.\n");
      if (!script_flag) {
	 use_stdin = DcTrue;
      }
#ifdef TIME_TEST
      if (testcase > 0) {
	 do_timing_test();
      } else 
#endif
      while (1) {
	 if (script_flag) {
	    controlchar = fgetc(scr_fd); 
	 } else {
	    controlchar = getchar();
	 }
	 if (controlchar == EOF) 
	       break;
	 if (process_character (controlchar))
	       break;
      }
   }

   /*  Releasing the device object will cause it to be deleted, and the
       appropriate dgi close will be done for the device as a result.  This
       should result in correct termination of output to the device (buffer
       flushing).  */

   DsReleaseObj(device);
   if (snapshot_dev_exists) DsReleaseObj(snapshot_device);

   DsTerminate ();

   if (logging_flag) {
      fclose (log_fd);
   }
   if (script_flag) {
      fclose (scr_fd);
   }
}
/*
  ======================================================================
 */
DtObject cvt_rgb_to_abgr (DtObject raster)
{
   DtInt rwidth;
   DtInt rheight;
   DtInt rdepth;
   DtRasterType type;
   char *typestring;
   DtPtr data;
   unsigned char *newdata;
   unsigned char *d1;
   unsigned char *d2;
   int w, h, d;
   int have_alpha;
   int skip;

   if (!raster) {
      return DcNullObject;
   }

   DsInqRaster(raster, &rwidth, &rheight, &rdepth, &type, &typestring, 
	       &data);

   if (DcRasterRGB != type &&
       DcRasterRGBA != type) {
      return raster;
   }

   if (DcRasterRGB == type) {
      skip = 3;
      have_alpha = 0;
   } else {
      skip = 4;
      have_alpha = 1;
   }

   if ((newdata = (unsigned char *)malloc(rwidth*rheight*4)) == NULL) {
      print ("Could not allocate space for data.\n");
      exit (1);
   }

   d1 = (unsigned char *)data;
   d2 = newdata;

   for (d=rdepth; d--;) {
      for (h=rheight; h--;) {
	 for (w=rwidth; w--;) {
	    if (have_alpha) d2[0] = d1[3];
	    d2[1] = d1[2];
	    d2[2] = d1[1];
	    d2[3] = d1[0];

	    d2 += 4;
	    d1 += skip;
	 }
      }
   }

   return DoRaster (rwidth, rheight, rdepth, DcRasterABGR, DcNullPtr, 
		    (DtPtr)newdata, DcDeleteData);
}

#ifdef TIME_TEST
/*
  ======================================================================
 */
int null_routine (void) {}
#endif
/*
  ======================================================================
 */
int do_timing_test (void)
{
#ifdef TIME_TEST
   static DDt_DROM_object_method check_methods[] = {
      "DoPointList",	(DtPFI)null_routine,
      "DoLineList",	(DtPFI)null_routine,
      "DoPolyline",	(DtPFI)null_routine,
      "DoTriangleList",	(DtPFI)null_routine,
      "DoTriangleMesh",	(DtPFI)null_routine,
      0,		0,
   };

   DtInt i, class_id;

   switch (testcase) {
   case 1:	/* timing with no display */
      for (i=0 ;check_methods[i].name != 0; i++) {
	 class_id = DsInqClassId(check_methods[i].name);
	 if (class_id != -1) {
	    check_methods[i].routine = 
	    DDclass_InqMethod(class_id, DcMethodDynRender);
	    DDclass_SetMethod(class_id, DcMethodDynRender, null_routine);
	 }
      }

      print ("Timing without drawing.\n");
      start_timing();
      for (i=0;i<100;i++) {
	 rotatevalue[0][0] += .1;
	 DdUpdate(device); 
	 framecount++;
      }
      report_timing();

      for (i=0 ;check_methods[i].name != 0; i++) {
	 class_id = DsInqClassId(check_methods[i].name);
	 if (class_id != -1) {
	    DDclass_SetMethod(class_id,
			       DcMethodDynRender, check_methods[i].routine);
	 }
      }

   case 2:	/* timing with display */
      print ("Timing with drawing.\n");
      start_timing();
      for (i=0;i<100;i++) {
	 rotatevalue[0][0] += .1;
	 DdUpdate(device); 
	 framecount++;
      }
      report_timing();
      break;
   }
#endif
}
/*
  ======================================================================
 */
DtGroupNetworkStatus
check_group_network (name, handle)
      DtPtr name;
      DtObject handle;
{
   DtGroupNetworkStatus status;

   switch (status = DgCheck(handle)) {
   case DcGroupOk:
      printf ("group '%s' is DcGroupOk\n", name);
      break;
   case DcGroupBad:
      printf ("group '%s' is DcGroupBad\n", name);	
      break;
   default:
      printf ("group '%s' network status is %d\n", name, status);
      break;
   }
   return status;
}
/*
  ======================================================================
 */
process_character (controlchar)
     char controlchar;
{
#ifdef titan
   static int thread_count = 0;
   static int thread_timeout = 0;
#endif
   static DtSwitch alternate_set = DcFalse;
   static DtSwitch texture_set = DcFalse;
   static DtSwitch always_fullupdate = DcFalse;
   DtInt i;
   DtSwitch fullupdate = always_fullupdate;
   DtVolume volume;
   DtInt oldshadetype = shadetype;
#if defined(use_DGL) && defined(INC_TITANG2)
#ifdef DEBUG
   extern DtInt Pe_x11_titang2_dynout_draw_buffer;
#endif
#endif
   static int no_update = 0;

   if (logging_flag) {
      putc (controlchar, log_fd);
   }

   if (alternate_set) {
      switch(controlchar) {
      case 'x':
      case 'q':
	 return 1;

     case '+':
         no_update = 0;
         print ("Update is turned on.\n");
         break;

     case '-':
         no_update = 1;
         print ("Update is turned off.\n");
         break;

#ifdef titan
      case 'T':
	 print  ("Setting timeout for threads.\n");
	 printf ("The current values are: count %d\ttimeout %d ms\n",
		 thread_count, thread_timeout);
	 print  ("New values: ");
	 scanf ("%d %d", &thread_count, &thread_timeout);
	 printf ("The new values are:     count %d\ttimeout %d ms\n", 
		 thread_count, thread_timeout);

	 _set_timeout(thread_count, thread_timeout);
	 return 0;
#endif
      case 'P':
	 {
	    DtInt width;
	    DtInt height;
	    DtRasterType type;
	    DtPtr data;
	    DtFlag userdelete;
	    DtObject raster;
	    char filename[100];
	    int ans;

	    print ("Do you want Z (1 - yes, 0 - no) ?");
	    fflush (stdout);
	    scanf("%d", &ans);
	    printf ("Getting Pixel Data%s from device ...",
		ans ? " with Z" : "");
	    fflush(stdout);
	    if (DdInqPixelData(device, ans ? DcRasterRGBZ : DcRasterRGB, 
			       &width, &height, &type, &data, &userdelete)) {
	       print (" ok\n");
	       raster = DoRaster (width, height, 1, type, DcNullPtr, data, 
				  userdelete ? DcDeleteData : DcNullPtr);
	       print ("Filename for raster image: ");
	       fflush(stdout);
	       scanf ("%s", filename);
	       printf ("Writing raster to '%s' ...", filename);
	       fflush (stdout);
	       DsRasterWrite(raster, filename);
	       DsReleaseObj(raster);
	       print (" done\n");
	    } else {
	       print (" failed\n");
	    }
	 }
	 break;
      case 'A':
	 {
	    DtInt width;
	    DtInt height;
	    DtRasterType type;
	    DtPtr data;
	    DtFlag userdelete;
	    DtObject raster;
	    char filename[100];
	    DtInt total_bytes;
	    int length;
	    int bytes;
	    int bytes_written;
	    char *data_ptr;
	    FILE *fp;

	    fflush(stdout);
	    if (DdInqPixelData(device, DcRasterRGB, 
			       &width, &height, &type, &data, &userdelete)) {
	       raster = DoRaster (width, height, 1, type, DcNullPtr, data, 
				  userdelete ? DcDeleteData : DcNullPtr);
	       print ("Filename for image: ");
	       fflush(stdout);
	       scanf ("%s", filename);
	       printf ("Writing rgb data to '%s' ...", filename);
	       fflush (stdout);
	       if ((filename[0] == '\0') || ((fp = fopen (filename, "w")) == NULL)) {
		    print ("Open failed.\n");
   		}
		total_bytes = width * height * 3;
		printf ("width %d height %d total_bytes %d\n", 
				width, height, total_bytes);
	 	data_ptr = (char *)data;
   	 	bytes = total_bytes;
   	 	bytes_written = 0;
		while ((length = fwrite (data_ptr, sizeof(char), bytes, fp)) != bytes && length > 0) {
      		    data_ptr += length;
      		    bytes -= length;
      		    bytes_written += length;
   		}
		if (length > 0) bytes_written += length;

   		if (bytes_written != total_bytes) {
			printf ("Wrote %d of %d bytes of data.\n",
				bytes_written, total_bytes);
		}
		if (length < 0) {
			extern int errno;

			printf ("Write failed, system error code %d.", errno);
		}
		fclose(fp);


	       DsReleaseObj(raster);
	       print (" done\n");
	    } else {
	       print (" DdInqPixelData failed.\n");
	    }
	 }
	 break;

      case 'R':
	 reset_callbacks();
	 outline_offset		= 100;
	 shadetype		= 1;
	 clearflag		= DcTrue;
	 updvar1		= DcFalse;
	 updvar2		= DcFalse;

	 reset_objects();
	 break;

      case '0':
            sleep(1);
            break;
      case '1':
            step_translate = step_rotate = step_scale = 1;
            break;
      case '2':
            step_translate *= 2;
            step_rotate *= 2;
            step_scale *= 2;
            break;
      case '3':
            step_translate *= 3;
            step_rotate *= 3;
            step_scale *= 3;
            break;
      case '4':
            step_translate *= 4;
            step_rotate *= 4;
            step_scale *= 4;
            break;
      case '5':
            step_translate *= 5;
            step_rotate *= 5;
            step_scale *= 5;
            break;
      case '6':
            step_translate *= 6;
            step_rotate *= 6;
            step_scale *= 6;
            break;
      case '7':
            step_translate *= 7;
            step_rotate *= 7;
            step_scale *= 7;
            break;
      case '8':
            step_translate *= 8;
            step_rotate *= 8;
            step_scale *= 8;
            break;
      case '9':
            step_translate *= 9;
            step_rotate *= 9;
            step_scale *= 9;
	    break;

      case '%':
	 {
	    char *argstring;

	    multi_pass_transp++;
	    if (multi_pass_transp > 2) multi_pass_transp = 0;
	    switch (multi_pass_transp) {
	    case 0:
	       print ("Transparency mode set to Two-pass.\n");
	       argstring = "-two_pass_transparency";
	       break;
	    case 1:
	       print ("Transparency mode set to Multi-pass.\n");
	       argstring = "-multipass_transparency";
	       break;
	    case 2:
	       print ("Transparency mode set to Sorted.\n");
	       argstring = "-sort_transparency";
	       break;
	    }
	    DdSetOptions(device, argstring);
	 }
	 break;
      case '^':
	 {
	    char *argstring;

	    multi_pass_transp--;
	    if (multi_pass_transp < 0) multi_pass_transp = 2;
	    switch (multi_pass_transp) {
	    case 0:
	       print ("Transparency mode set to Two-pass.\n");
	       argstring = "-two_pass_transparency";
	       break;
	    case 1:
	       print ("Transparency mode set to Multi-pass.\n");
	       argstring = "-multipass_transparency";
	       break;
	    case 2:
	       print ("Transparency mode set to Sorted.\n");
	       argstring = "-sort_transparency";
	       break;
	    }
	    DdSetOptions(device, argstring);
	 }
	 break;

      case 'B':
	 {
	    char *argstring;

	    singlebuffered = !singlebuffered;

	    if (singlebuffered) {
	       print ("Single Buffered.\n");
	       argstring = "-singlebuffered";
	    } else {
	       print ("Double Buffered.\n");
	       argstring = "-doublebuffered";
	    }
	    DdSetOptions (device, argstring);
	 }
	 break;

      case 'D':
	 {
	    char *argstring;

	    dither_flag = !dither_flag;

	    if (dither_flag) {
	       print ("Dithering\n");
	       argstring = "-dither";
	    } else {
	       print ("No Dithering\n");
	       argstring = "-nodither";
	    }
	    DdSetOptions (device, argstring);
	 }
	 break;

      case 'Z':
	 {
	    char *argstring;

	    use_zbuffer = !use_zbuffer;

	    if (use_zbuffer) {
	       print ("Zbuffering\n");
	       argstring = "-zbuffer";
	    } else {
	       print ("No Zbuffering\n");
	       argstring = "-nozbuffer";
	    }
	    DdSetOptions (device, argstring);
	 }
	 break;

      case 'S':
	 {
	    char *argstring;

	    use_spheres = !use_spheres;

	    if (use_spheres) {
	       print ("Hardware Spheres\n");
	       argstring = "-spheres";
	    } else {
	       print ("No Hardware Spheres\n");
	       argstring = "-nospheres";
	    }
	    DdSetOptions (device, argstring);
	    fullupdate = DcTrue;
	 }
	 break;

      case '~':
        {
            char readstring[200];
            char dev[200];
            char devargs[500];

            if (!snapshot_dev_exists) {
/*
                print ("Enter device name: ");
                fflush(stdout);
                gets(readstring);
                if (readstring[0] == '\0')
                    sprintf(dev, "g4vll");
                else
                    sprintf(dev, readstring);
                print ("Enter device argstring: ");
                fflush(stdout);
                gets(readstring);
                if (readstring[0] == '\0')
                    sprintf(devargs, "");
                else
                    sprintf(devargs, readstring);
*/
		sprintf(dev, "g4vll");
	 	sprintf(devargs,
		    " -noX -geometry %dx%d+%d+%d -windowid %ld %s%s%s%s%s%s%s",
		    width,height,topx,topy, windowid,
                    singlebuffered       ? " -singlebuffered "         : "",
                    stereoflag           ? " -stereo "                 : "",
                    multi_pass_transp==1? " -multipass_transparency "  :
                    multi_pass_transp==2? " -sort_transparency "       : "",
                    use_spheres          ? " -spheres "                : "",
                    use_zbuffer          ? " -zbuffer "                : "",
		    ztc			 ? "" 			       : " -noztc ",
		    hw_boundingvol       ? " -boundingvol "  : " -noboundingvol ",
                    dither_flag          ? " -dither "                 : "");

                if (!(snapshot_device = DoDevice(dev, devargs))) {
                    printf ("can't create snapshot device '%s', (args = '%s')\n",
                         dev, devargs);
                } else {
                    printf ("created DoDevice(%s, %s)\n", dev, devargs);
                    DdSetFrame(snapshot_device, frame);
                    print("updating snapshot device... ");  fflush(stdout);
                    DdUpdate(snapshot_device);
                    print("done\n");  fflush(stdout);
                    snapshot_dev_exists = 1;
                    DsHoldObj(snapshot_device);
                }
            } else {
                print("updating snapshot device... ");  fflush(stdout);
                DdUpdate(snapshot_device);
                print("done\n");  fflush(stdout);
            }
        }
        break;

      case 'U':
	 {
            if (!use_rad) 
               print("Radiosity not initialized. Use -use_rad 1 option.\n");
            else {
               DtInt int_updtype;

               print("0 - DcRadiosityNone\n");
               print("1 - DcRadiosityComputeDisp\n");
               print("2 - DcRadiosityDisp\n");
               int_updtype = rad_updtype; 
               printf("\nRadiosity Update Type is : %d\n", int_updtype);
               print("\nEnter new Update Type: ");
               scanf("%d", &int_updtype);
               rad_updtype = int_updtype;
               DvSetRadiosityUpdateType(view, rad_updtype);
               for (i=0; i<4; i++)  {
                  /* since we know that every view has the same objects, we
                   can compute the radiosity values only for one */
                   if (i>0 && rad_updtype == DcRadiosityComputeDisp) 
                      DvSetRadiosityUpdateType(m_view[i], DcRadiosityDisp);
	           else
                      DvSetRadiosityUpdateType(m_view[i], rad_updtype);
                }
           }
	 }
         break;

      case 'C':
	 {
            if (!use_rad) 
               print("Radiosity not initialized. Use -use_rad 1 option.\n");
            else {
               DtInt int_convtype;

               print("0 - DcConvFixedIterations\n");
               print("1 - DcConvGlobalRel\n");
               print("2 - DcConvGlobalAbs\n");
               print("3 - DcConvPatchRel\n");
               print("4 - DcConvPatchAbs\n");
               int_convtype = rad_convtype; 
               printf("\nRadiosity Convergence Type is: %d\n", int_convtype);
               printf("Radiosity Convergence parameter is: %g\n",
                      rad_convparam[0]);
               print("\nEnter new Convergence Type: ");
               scanf("%d", &int_convtype);
               rad_convtype = int_convtype;
               print("\nEnter new Convergence parameter: ");
               if (sizeof(float) == sizeof(DtReal))
                   scanf("%f", &rad_convparam[0]);
               else
                   scanf("%lf", &rad_convparam[0]);

               DvSetRadiosityConvergence(view,rad_convtype,rad_convparam);
               for (i=0; i<4; i++) 
                   DvSetRadiosityConvergence(m_view[i],rad_convtype,rad_convparam);
           }
	 }
	 break;

      default:
	 printf ("character %c (0x%x) ignored\n", controlchar, controlchar);
      }

      alternate_set = DcFalse;

   } else if (texture_set) {
         texture_set = DcFalse;
	 switch (process_texture_callback(controlchar)){
	 case 1:	/* full update */
	     fullupdate = DcTrue;
	     break;
	 case 2:	/* no update */
	     return 0;
	 }
   } else {
      switch (controlchar) {
      case 'x':
      case 'q':
	 return 1;

      case '~':
	 alternate_set = DcTrue;
	 return 0;

      case '!':
	 texture_set = DcTrue;
	 return 0;

#if (defined(use_DGL) || defined(use_VLL)) && defined(__P3__) && defined(GP_BRK)
      case '':
	/* provide hook for pipeline */
	 _gp_break = 0x1;
	 break;
#endif

      case '':
	 {
	    float floatx, floaty, floatz;
	    char outfile[200];
	    DtPoint3 frompoint;

	    print ("Point from which to create environment map:");
	    fflush(stdout);
	    scanf("%f %f %f", &floatx, &floaty, &floatz);
	    frompoint[0] = floatx;
	    frompoint[1] = floaty;
	    frompoint[2] = floatz;
	
	    print ("Name of output file:");
	    fflush(stdout);
	    scanf("%s", outfile);
	
	    printf("Creating environment map %s from [%g,%g,%g]...", outfile, frompoint[0], frompoint[1], frompoint[2]);
	    g3create_emap(device, view, frompoint, outfile);
	    break;	
	 }
      case 'n':
	 printf ("re-reading '%s'\n", input_filename);
	 release_old_groups();
	 read_input_file();
	 fullupdate = DcTrue;
	 break;

      case '':
      case '\014':
	 if (use_stdin)
	       return 0;
	 break;

      case 'Q':
	 {
	    char inname[1024];
	    DtObject obj;

	    print ("Enter name of object to check for: ");
	    fflush (stdout);
	    scanf ("%s", inname);
	    obj = DsInqObjViaString (inname, DcTypeAny);
	    if (obj != DcNullObject) {
	       printf ("Object with name '%s' is at 0x%x with type %d\n", 
		       inname, obj, DsInqObjClassId(obj));
	    } else {
	       printf ("Object with name '%s' does not exist\n", inname);
	    }
	 }
	 return 0;
      
      case '?':
	 print_help();
	 return 0;

#ifdef OFFSET_OUTLINE
      case '8':
	 outline_offset += 1;
	 printf ("offset = %d\n", outline_offset);
	 break;

      case '*':
	 outline_offset -= 1;
	 printf ("offset = %d\n",outline_offset);
	 break;
#endif

      case '5':
	 shadetype++;
	 if (shadetype > 3) shadetype = 0;
	 break;

      case '%':
	 shadetype--;
	 if (shadetype < 0) shadetype = 3;
	 break;

      case 'p':	
	 DsPrintObj(object_group);
	 return 0;

      case 'P':
	 DsPrintObj(camera_group);
	 return 0;

      case 'b':
	 DsCompBoundingVol(&volume, object_group); 
	 printf("bounding volume:\n\t(%f, %f, %f) to\n\t(%f, %f, %f)\n\n",
		volume.bll[0], volume.bll[1], volume.bll[2],
		volume.fur[0], volume.fur[1], volume.fur[2]);	
	 return 0;

      case 'r':
	 read_values();
	 print ("values read in from file 'values.dat'\n");
	 fullupdate = DcTrue;
	 break;

      case 'R':
	 makeraytrace();
	 return 0;

      case 'Y':
	 makemedialogic();
	 return 0;

      case 'T':
	 time_update = !time_update;
	 break;

      case 't':
	 if (fullupdate) {
	    for (i=0; i<4; i++) {
	       DvSetUpdateType(m_view[i], DcUpdateAll);
	    }
	    DvSetUpdateType(view, DcUpdateAll);
	 }
	 start_timing();
	 
	 for (i=0;i<100;i++) {
	    extern DtReal rotatevalue[9][3];

	    rotatevalue[0][0] += .1;
	 
	    DdUpdate(device); 
#ifdef titan
	    if (test_thread) {
	       threadserial();
	    }
#endif
	    framecount++;
	 }

	 report_timing();
      
	 if (fullupdate) {
	    for (i=0; i<4; i++) {
	       DvSetUpdateType(m_view[i], DcUpdateDisplay);
	    }
	    DvSetUpdateType(view, DcUpdateDisplay);
	 }
	 return 0;

      case 'w':
	 write_values();
	 print ("values written to file 'values.dat'\n");
	 return 0;

      case 'W':
	 for (i=0; i<4; i++) {
	    DvSetUpdateType(m_view[i], DcUpdateAll);
	 }
	 DvSetUpdateType(view, DcUpdateAll);

	 DdUpdate(device);

	 for (i=0; i<4; i++) {
	    DvSetUpdateType(m_view[i], DcUpdateDisplay);
	 }
	 DvSetUpdateType(view, DcUpdateDisplay);
	 print ("Unconditional update performed.\n");
#if defined(use_DGL) && defined(INC_TITANG2)
#ifdef DEBUG
	 printf ("(After Update) Draw Buffer = %d\n",Pe_x11_titang2_dynout_draw_buffer);
#endif
#endif
	 return 0;

      case 'g':
	 always_fullupdate = !always_fullupdate;
	 printf ("always_fullupdate is now %s.\n", always_fullupdate ? "on" : "off");
	 break;

      case 'G':
	 print ("Frame update performed.\n");
	 DfUpdate(frame);
	 return 0;

      case 'C':
         {
            char *argstring;

            cylinders++;
	    if (cylinders > 2)  cylinders = 0;

      	    switch (cylinders) {
      		case 0: print ("Use tesselated cylinders\n");
               	        argstring = "-nocylinders";
			break;
      		case 1: print ("Use firmware capped cylinders\n");
               	        argstring = "-cylinders";
			break;
      		case 2: print ("Use firmware uncapped cylinders\n");
               	        argstring = "-cylindersnocap";
			break;
	    }
            DdSetOptions (device, argstring);
            fullupdate = DcTrue;
         }
         break;

      case 'Z':
	 clearflag = !clearflag;
	 printf ("Clear flag is now %s.\n", clearflag ? "on" : "off");
	 for (i=0; i<4; i++) {
	    DvSetClearFlag(m_view[i],clearflag);
	 }
	 DvSetClearFlag(view,clearflag);
	 break;

      case 'V':
	 multiple_views = !multiple_views;
	 DgEmpty (DfInqViewGroup(frame));
	 if (multiple_views) {
	    for (i=0; i<4; i++) {
	       DgAddObjToGroup(DfInqViewGroup(frame),m_view[i]);
	    }
	 } else {
	    DgAddObjToGroup(DfInqViewGroup(frame),view);
	 }
	 fullupdate = DcTrue;
	 break;

      case '^':
	 current_rendstyle--;
	 if (current_rendstyle < 0) current_rendstyle = num_rendstyles - 1;
	 printf ("Current Renderer is '%s'\n", rendstyle_name[current_rendstyle]);
	 for (i=0; i<4; i++) {
	    DvSetRendStyle(m_view[i],rendstyle_id[current_rendstyle]);
	 }
	 DvSetRendStyle(view,rendstyle_id[current_rendstyle]);
	 break;
      
      case '6':
	 current_rendstyle++;
	 if (current_rendstyle >= num_rendstyles) current_rendstyle = 0;
	 printf ("Current Renderer is '%s'\n", rendstyle_name[current_rendstyle]);
	 for (i=0; i<4; i++) {
	    DvSetRendStyle(m_view[i],rendstyle_id[current_rendstyle]);
	 }
	 DvSetRendStyle(view,rendstyle_id[current_rendstyle]);
	 break;

      case '-':
	 updvar1 = DcTrue;
	 break;

      case '_':
	 updvar2 = DcTrue;
	 break;

     default:
	 switch (process_callback (controlchar)){
	 case 1:	/* full update */
	     fullupdate = DcTrue;
	     break;
	 case 2:	/* no update */
	     return 0;
	 }
      }
   }

   if (pseudocolor && (shadetype != oldshadetype)) {
      fullupdate = DcTrue;
      switch (shadetype) {
      case 0:
	 print ("ShadeMode = DcComponent (1)\n");
	 DdSetShadeMode(device, DcComponent);
	 DdSetColorEntries(device, DcRGB, 0, 256, color_scale_map);
	 break;
      case 1:
	 print ("ShadeMode = DcComponent (2)\n");
	 DdSetShadeMode(device, DcComponent);
	 DdSetColorEntries(device, DcRGB, 0, 256, color1_scale_map);
	 break;
      case 2:
	 print ("ShadeMode = DcRange (1 range)\n");
	 DdSetShadeMode(device, DcRange);
	 DdSetColorEntries(device, DcRGB, 0, gray_scale_size, gray_scale_map);
	 break;
      case 3:
	 print ("ShadeMode = DcRange (8 ranges)\n");
	 DdSetShadeMode(device, DcRange);
	 DdSetColorEntries(device, DcRGB, 0, shade_rng_size, shade_rng_map);
	 break;
      }
   }

   if (no_update) return 0;

   if (fullupdate) {
      for (i=0; i<4; i++) {
	 DvSetUpdateType(m_view[i], DcUpdateAll);
      }
      DvSetUpdateType(view, DcUpdateAll);
   }

   if (time_update) 
	 start_timing();

   DdUpdate(device);

   if (time_update) {
      framecount++;
      report_timing();
   }

#if defined(use_DGL) && defined(INC_TITANG2)
#ifdef DEBUG
   printf ("(After Update) Draw Buffer = %d\n",Pe_x11_titang2_dynout_draw_buffer);
#endif
#endif

   if (fullupdate) {
      for (i=0; i<4; i++) {
	 DvSetUpdateType(m_view[i], DcUpdateDisplay);
      }
      DvSetUpdateType(view, DcUpdateDisplay);
   }

   return 0;
}
/*
  ======================================================================
 */
process_pick (pck_pnt)
    DtPoint3 pck_pnt;
{
    DtInt hit_count;
    DtInt i;
    DtInt e_flag;
    DtVolume volume;

    DdInqExtent(device,&volume);

    print("\n\n******** PICK REPORT ********\n");
    printf("   volume      { (%g,%g,%g) (%g,%g,%g) }\n",
	volume.bll[0],volume.bll[1],volume.bll[2],
	volume.fur[0],volume.fur[1],volume.fur[2]);

    printf("   pick point  (%g,%g,%g)\n",
	pck_pnt[0],pck_pnt[1],pck_pnt[2]);

    DdPickObjs
    (   device, pck_pnt, &hit_count, index_size, pick_index,
	list_size, hit_list, z_values, wc_values, lc_values, views, &e_flag
    );

    printf("   accepted    %d hit%c\n   eflag       0x%0x\n",
	hit_count, hit_count != 1 ? 's' : ' ',e_flag);
    printf("   paths       %sreversed\n\n", reverse ? "not " : "");

    for(i=0; i<hit_count; i++) {
	printf ("hit %d  z_value: %g\n", i, z_values[i]);
	printf ("\tworld_point: (%g %g %g)\n", 
	    wc_values[3*i], wc_values[3*i+1], wc_values[3*i+2]);
	printf ("\tlocal_point: (%g %g %g)\n",
	    lc_values[3*i], lc_values[3*i+1], lc_values[3*i+2]);
	printf ("\tview: 0x%0x\n", views[i]);
	PrintPath ((pick_index[i+1]-pick_index[i])/3, hit_list+pick_index[i]);

	if (show_picks) {
	    AddHit((pick_index[i+1]-pick_index[i]) /3,hit_list+pick_index[i],
	    &(wc_values[3*i]), &(lc_values[3*i]));
	}
    }
    if (show_picks)
	DdUpdate(device);
}

/* ====================================================================== */

int AddHit (Size,Path, wcs, lcs)
     DtInt  Size;
     long   Path[];
     DtReal wcs[3];
     DtReal lcs[3];
{
    if (use_local_pick) {
      if (reverse) {
	 DgOpen((DtObject)Path[1*3],DcFalse);
	 DgSetElePtr(Path[1*3+2],DcBeginning);
      } else {
	 DgOpen((DtObject)Path[(Size-2)*3],DcFalse);
	 DgSetElePtr(Path[(Size-2)*3+2],DcBeginning);
      }
      DgAddObj(DoPushMatrix());
	 DgAddObj(DoTranslate(lcs[0], lcs[1], lcs[2]));
	 DgAddObj(sphere_obj);
      DgAddObj(DoPopMatrix());
      DgClose();
   } else {
      DgOpen(pickgroup,DcTrue);
      DgAddObj(DoPushMatrix());
	 DgAddObj(DoTranslate(wcs[0], wcs[1], wcs[2]));
	 DgAddObj(sphere_obj);
      DgAddObj(DoPopMatrix());
      DgClose();
   }
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

   print ("+------------------------------+-------+-------+\n");
   print ("|                              |  pick |  ele  |\n");
   print ("|           group              |   id  | seq # |\n");
   print ("+------------------------------+-------+-------+\n");
   for(i=0; i<Size; i++) {
      switch(DsInqObjStatus((DtObject)Path[3*i])) {
	  case DcObjectValid:
	     switch (DsInqObjNameType((DtObject)Path[3*i])) {
		 case DcNameInteger:
		    printf ("|%30d", DsInqObjNameInteger((DtObject)Path[3*i]));
		    break;
		 case DcNameString:
		    printf ("|%30s", DsInqObjNameInteger((DtObject)Path[3*i]));
		    break;
		 case DcNameNone:
		    printf ("|%30s", "no name");
		    break;
		 default: 
		    printf("|%30s", "unknown name type");
		    break;
	     }
	     break;
	  case DcObjectInvalid:
	     printf("|%30s", "<deleted>");
	     break;
	  case DcObjectDeleted:
	     printf("|30%s", "<invalid>");
	     break;
	  default: 
	     printf("|%30s", "unknown validity type");
	     break;
      }
      printf ("|%7d|%7d|\n", Path[3*i+1], Path[3*i+2]);
   } 
   print ("+------------------------------+-------+-------+\n");
}
/*
  ======================================================================
 */
makeraytrace()
{
   DtInt width, height;
   char filename[100];
   char argstring[300];
   DtObject device;
   DtObject frame;
   DtObject view;
   DtVolume rayvolume;
   DtObject object_group;
   DtObject camera_group;

   print ("enter width and height:  ");
   scanf ("%d %d",&width,&height);
   print ("enter filename(without .img):  ");
   scanf ("%s", filename);

   sprintf(argstring,"-filename %s.img -width %d -height %d",
	   filename,width,height);

   if (!(device = DoDevice("rasterfile",argstring))) {
      printf("can't create device:  device type = '%s',device file = '%s.img'\n",
	     "rasterfile",filename);
      return;
   }

   rayvolume.bll[0] = 0;
   rayvolume.bll[1] = 0;
   rayvolume.bll[2] = 0;

   rayvolume.fur[0] = width;
   rayvolume.fur[1] = height;
   rayvolume.fur[2] = 65536.;

   DdSetViewport(device,&rayvolume);

   if (!(frame = DoFrame())){
      print ("Can't create frame.\n");
      return;
   }

   DdSetFrame(device,frame);

   DfSetBoundary(frame,&rayvolume);

   if (!(view = DoView())) {
      print ("Can't create view.\n");
      return;
   }

   DvSetClearFlag(view,DcTrue);
   DvSetBackgroundColor(view,DcRGB,backgroundcolor);

   DvSetRendStyle(view,DcProductionTime);
   
   DgAddObjToGroup(DfInqViewGroup(frame),view);

   DvSetBoundary(view,&rayvolume);

   DvSetUpdateType(view,DcUpdateAll);

   object_group = DsInqObjViaString (objectgroupname, DcTypeGroup);
   DgAddObjToGroup(DvInqDisplayGroup(view),object_group);

   camera_group = DsInqObjViaString (cameragroupname, DcTypeGroup);
   DgAddObjToGroup(DvInqDefinitionGroup(view),camera_group);

   print ("Computing raytraced image.\n");

   DdUpdate(device); 

   /*  Releasing the device object will cause it to be deleted, and the
       appropriate dgi close will be done for the device as a result.  This
       should result in correct termination of output to the device (buffer
       flushing).  */

   DsReleaseObj(device);
   printf ("Raytracing Completed to file '%s.img'\n", filename);
}
/*
  ======================================================================
 */
makemedialogic()
{
   DtInt width, height;
   char filename[100];
   char argstring[300];
   DtObject device;
   DtObject frame;
   DtObject view;
   DtVolume rayvolume;
   DtObject object_group;
   DtObject camera_group;
   DtInt mlr_id;

   print ("Enter width and height:  ");
   scanf ("%d %d", &width, &height);
   print ("Enter filename (without .img):  ");
   scanf ("%s", filename);

   sprintf (argstring, "-filename %s.img -width %d -height %d",
	   filename,width,height);

   if (!(device = DoDevice("rasterfile",argstring))) {
      printf ("Can't create device:  device type = '%s', device file = '%s.img'\n", "rasterfile",filename);
      return;
   }

   rayvolume.bll[0] = 0;
   rayvolume.bll[1] = 0;
   rayvolume.bll[2] = 0;

   rayvolume.fur[0] = width;
   rayvolume.fur[1] = height;
   rayvolume.fur[2] = 1.;

   DdSetViewport(device,&rayvolume);

   if (!(frame = DoFrame())){
      print ("Can't create frame.\n");
      return;
   }

   DdSetFrame(device,frame);

   DfSetBoundary(frame,&rayvolume);

   if (!(view = DoView())) {
      print ("Can't create view.\n");
      return;
   }

   DvSetClearFlag(view,DcTrue);
   DvSetBackgroundColor(view,DcRGB,backgroundcolor);

   mlr_id = DsInqRendererId("Media Logic File Renderer");
   DvSetRendStyle(view,mlr_id);
   
   DgAddObjToGroup(DfInqViewGroup(frame),view);

   DvSetBoundary(view,&rayvolume);

   DvSetUpdateType(view,DcUpdateAll);

   object_group = DsInqObjViaString (objectgroupname, DcTypeGroup);
   DgAddObjToGroup(DvInqDisplayGroup(view),object_group);

   camera_group = DsInqObjViaString (cameragroupname, DcTypeGroup);
   DgAddObjToGroup(DvInqDefinitionGroup(view),camera_group);

   print ("Computing Media Logic image.\n");

   DdUpdate(device); 

   /*  Releasing the device object will cause it to be deleted, and the
       appropriate dgi close will be done for the device as a result.  This
       should result in correct termination of output to the device (buffer
       flushing).  */

   DsReleaseObj(device);
   printf ("Media Logic Rendering Completed to file '%s.img'\n", filename);
}
/*
  ======================================================================
 */
start_timing()
{
    framecount = 0;

    doe_system_timing_lines_drawn = 0;
    doe_system_timing_points_drawn = 0;
    doe_system_timing_connected_lines_drawn = 0;
    doe_system_timing_triangles_drawn = 0;
    doe_system_timing_mesh_triangles_drawn = 0;
    doe_system_timing_strip_triangles_drawn = 0;
    doe_system_timing_quads_drawn = 0;
    doe_system_timing_mesh_quads_drawn = 0;
    doe_system_timing_firmware_spheres_drawn = 0;
    doe_system_timing_firmware_cylinders_drawn = 0;

    /* Get the current time in microseconds. */

#ifdef titan
    initialtime = times(&buffer) * (1000000 / HZ);
#else
    {   auto struct timezone tz;
	auto struct timeval  currtime;
	gettimeofday (&currtime, &tz);
	initialtime = (currtime.tv_sec * 1000000) + currtime.tv_usec;
    }
#endif
}
/*
  ======================================================================
 */
void report_timing()
{
    auto   DtReal elapsedtime;		/* Elapsed Time (Seconds) */
    auto   int    element_count;	/* Number of Elements Drawn */
    static char   liner[] = "-------------------------------------------------------------------------------\n";

    /* Store the current time in 'elapsedtime'. */

#   ifdef titan
	elapsedtime = times(&buffer) * (1000000 / HZ);
#   else
    {   auto struct timezone tz;
	auto struct timeval  currtime;
	gettimeofday (&currtime, &tz);
	elapsedtime = (currtime.tv_sec * 1000000) + currtime.tv_usec;
    }
#   endif

    /* Find the elapsed time in seconds. */

    if (elapsedtime == initialtime)
	elapsedtime = 0.0000001;
    else
	elapsedtime = (elapsedtime - initialtime) / 1000000;

    element_count = 0;
    
    print (liner);

    if (doe_system_timing_points_drawn > 0) {
	printf ("%8d Points\n", doe_system_timing_points_drawn);
	element_count += doe_system_timing_points_drawn;
    }
    if (doe_system_timing_lines_drawn > 0) {
        printf ("%8d List Lines\n", doe_system_timing_lines_drawn);
	element_count += doe_system_timing_lines_drawn;
    }
    if (doe_system_timing_connected_lines_drawn > 0) {
        printf("%8d Connected Lines\n",doe_system_timing_connected_lines_drawn);
	element_count += doe_system_timing_connected_lines_drawn;
    }
    if (doe_system_timing_triangles_drawn > 0) {
        printf ("%8d List Triangles\n", doe_system_timing_triangles_drawn);
	element_count += doe_system_timing_triangles_drawn;
    }
    if (doe_system_timing_mesh_triangles_drawn > 0) {
        printf ("%8d Mesh Triangles\n", doe_system_timing_mesh_triangles_drawn);
	element_count += doe_system_timing_mesh_triangles_drawn;
    }
    if (doe_system_timing_strip_triangles_drawn > 0) {
        printf("%8d Strip Triangles\n",doe_system_timing_strip_triangles_drawn);
	element_count += doe_system_timing_strip_triangles_drawn;
    }
    if (doe_system_timing_quads_drawn > 0) {
        printf ("%8d List Quadrilaterals\n", doe_system_timing_quads_drawn);
	element_count += doe_system_timing_quads_drawn;
    }
    if (doe_system_timing_mesh_quads_drawn > 0) {
        printf ("%8d Mesh/Grid Quadrilaterals\n",
	    doe_system_timing_mesh_quads_drawn);
	element_count += doe_system_timing_mesh_quads_drawn;
    }
    if (doe_system_timing_firmware_spheres_drawn > 0) {
        printf ("%8d Firmware Spheres\n", 
	    doe_system_timing_firmware_spheres_drawn);
	element_count += doe_system_timing_firmware_spheres_drawn;
    }
    if (doe_system_timing_firmware_cylinders_drawn > 0) {
        printf ("%8d Firmware Cylinders\n", 
	    doe_system_timing_firmware_cylinders_drawn);
	element_count += doe_system_timing_firmware_cylinders_drawn;
    }

    printf ("\n%g seconds,  %g frames/second,  %g elements/second\n",
	elapsedtime, framecount/elapsedtime, element_count/elapsedtime);
    print (liner);
}
/*
======================================================================
*/
DtObject make_multiplecameras()
{
    DtObject camera;
    static DtPoint3 at = {0., 0., 0.};
    static DtPoint3 from = {0., 5., 10.};
    static DtVector3 up = {0., 1., 0.};
    static DtReal color[3];
    DtPoint3 prp;
    static DtReal xmin[] = { -5., 0., -5.,  0.};
    static DtReal xmax[] = {  0., 5.,  0.,  5.};
    static DtReal ymin[] = {  0., 0., -5., -5.};
    static DtReal ymax[] = {  5., 5.,  0.,  0.};

    DoGroup(DcTrue);

    DgAddObj (DoGlbRendMaxObjs(4));
    DgAddObj (DoGlbRendMaxSub(150));

    DgAddObj (DoStereoSwitch(DcTrue));

    DgAddObj (DsInqObjViaString ("stereo", DcTypeCallback));

    /*
    if (stereoflag) {
      window.ll[0] = xmin[0] - (xmax[0] - xmin[0]) / 2.;
      window.ur[0] = xmax[0] + (xmax[0] - xmin[0]) / 2.;
    } else {
      window.ll[0] = xmin[0];
      window.ur[0] = xmax[0];
    }
    window.ll[1] = ymin[0];
    window.ur[1] = ymax[0];

    prp[0] = window.ll[0] + (window.ur[0] - window.ll[0]) / 2.;
    prp[1] = window.ll[1] + (window.ur[1] - window.ll[1]) / 2.;
    prp[2] = 25.;

    DgAddObj (DoProjection (&window, DcParallel, prp, 10.,-1., -100.));
    */
    DgAddObj (DoPerspective (30., -1., -100.));

    DgAddObj (DoPushMatrix());

    from[0] = 0.;
    from[1] = 5.;
    from[2] = 10.;
    DgAddObj (DoLookAtFrom(at, from, up));

    DgAddObj (camera = DoCamera());
    DsSetObjNameString (camera, "camera1", DcOn);

    DgAddObj (DoPopMatrix());

    /*
    if (stereoflag) {
      window.ll[0] = xmin[1] - (xmax[1] - xmin[1]) / 2.;
      window.ur[0] = xmax[1] + (xmax[1] - xmin[1]) / 2.;
    } else {
      window.ll[0] = xmin[1];
      window.ur[0] = xmax[1];
    }
    window.ll[1] = ymin[1];
    window.ur[1] = ymax[1];

    prp[0] = window.ll[0] + (window.ur[0] - window.ll[0]) / 2.;
    prp[1] = window.ll[1] + (window.ur[1] - window.ll[1]) / 2.;
    prp[2] = 25.;

    DgAddObj (DoProjection (&window, DcParallel, prp, 10.,-1., -100.));
    */
    DgAddObj (DoPerspective (30., -1., -100.));

    DgAddObj (DoPushMatrix());

    from[0] = 10.;
    from[1] = 5.;
    from[2] = 0.;
    DgAddObj (DoLookAtFrom(at, from, up));

    DgAddObj (camera = DoCamera());
    DsSetObjNameString (camera, "camera2", DcOn);

    DgAddObj (DoPopMatrix());

    /*
    if (stereoflag) {
      window.ll[0] = xmin[2] - (xmax[2] - xmin[2]) / 2.;
      window.ur[0] = xmax[2] + (xmax[2] - xmin[2]) / 2.;
    } else {
      window.ll[0] = xmin[2];
      window.ur[0] = xmax[2];
    }
    window.ll[1] = ymin[2];
    window.ur[1] = ymax[2];

    prp[0] = window.ll[0] + (window.ur[0] - window.ll[0]) / 2.;
    prp[1] = window.ll[1] + (window.ur[1] - window.ll[1]) / 2.;
    prp[2] = 25.;

    DgAddObj (DoProjection (&window, DcParallel, prp, 10.,-1., -100.));
    */
    DgAddObj (DoPerspective (30., -1., -100.));

    DgAddObj (DoPushMatrix());

    from[0] = -10.;
    from[1] = 5.;
    from[2] = 0.;
    DgAddObj (DoLookAtFrom(at, from, up));

    DgAddObj (camera = DoCamera());
    DsSetObjNameString (camera, "camera3", DcOn);

    DgAddObj (DoPopMatrix());

    /*
    if (stereoflag) {
      window.ll[0] = xmin[3] - (xmax[3] - xmin[3]) / 2.;
      window.ur[0] = xmax[3] + (xmax[3] - xmin[3]) / 2.;
    } else {
      window.ll[0] = xmin[3];
      window.ur[0] = xmax[3];
    }
    window.ll[1] = ymin[3];
    window.ur[1] = ymax[3];

    prp[0] = window.ll[0] + (window.ur[0] - window.ll[0]) / 2.;
    prp[1] = window.ll[1] + (window.ur[1] - window.ll[1]) / 2.;
    prp[2] = 25.;

    DgAddObj (DoProjection (&window, DcParallel, prp, 10.,-1., -100.)); 
    */
    DgAddObj (DoPerspective (30., -1., -100.)); 

    DgAddObj (DoPushMatrix());

    from[0] = 7.9;
    from[1] = 5.;
    from[2] = 7.9;
    DgAddObj (DoLookAtFrom(at, from, up));

    DgAddObj (camera = DoCamera());
    DsSetObjNameString (camera, "camera4", DcOn);

    DgAddObj (DoPopMatrix());

    return (DgClose());
}

/* ====================================================================== */

print_help()
{
    print (help);

#   ifdef titan
	print ("~T    - Set thread timeout\n");
#   endif

#   ifdef OFFSET_OUTLINE
	print ("8/\*   - Offset outline shift\n");
#   endif
}

/* ====================================================================== */

setup_colormaps (size)
    DtInt size;
{
    static initialized = 0;
    DtInt i, j;
    DtInt range_size;
    DtReal divider;

    if (initialized) return;
    initialized = 1;

    if (size > 4096) size = 4096;

    gray_scale_size = size;
    shade_ranges[0] = 0;
    shade_ranges[1] = size - 1;

    divider = size - 1;
    for (i=0; i<size; i++) {
      gray_scale_map[i*3] = gray_scale_map[i*3+1] = gray_scale_map[i*3+2] = 
	    (DtReal)i / divider;
    }

    for (i=0; i<256; i++) {
      color_scale_map[i*3+0] = (DtReal)(i>>5) / 7.;
      color_scale_map[i*3+1] = (DtReal)((i>>2) & 0x07) / 7.;
      color_scale_map[i*3+2] = (DtReal)(i & 0x03) / 3.;

      color1_scale_map[i*3+0] = (DtReal)((i>>5)+1) / 8.;
      color1_scale_map[i*3+1] = (DtReal)(((i>>2) & 0x07)+1) / 8.;
      color1_scale_map[i*3+2] = (DtReal)((i & 0x03)+1) / 4.;
    }

    range_size = size/8;
    divider = range_size - 1;
    shade_rng_size = range_size*8;

    for (i=0; i<8; i++) {
      shade_ranges[(i+1)*2+0] = i*range_size;
      shade_ranges[(i+1)*2+1] = i*range_size+(range_size-1);
      for (j=0; j<range_size; j++) {
	 shade_rng_map[i*range_size*3+j*3+0] = 
	       range_color[i][0] * (DtReal)j / divider;
	 shade_rng_map[i*range_size*3+j*3+1] = 
	       range_color[i][1] * (DtReal)j / divider;
	 shade_rng_map[i*range_size*3+j*3+2] = 
	       range_color[i][2] * (DtReal)j / divider;
      }
    }
}



/*****************************************************************************
**  This routine returns the string representation of the given visual type.
*****************************************************************************/

char *print_visual (DtVisualType visualtype)
{
   return (visualtype == DcTrueColor)   ? "DcTrueColor"   :
	  (visualtype == DcDirectColor) ? "DcDirectColor" :
	  (visualtype == DcPseudoColor) ? "DcPseudoColor" :
	  (visualtype == DcStaticGrey)  ? "DcStaticGrey"  : "Unknown";
      
}

/* ====================================================================== */

release_old_groups()
{
    DtInt i;

    DsReleaseObj(object_group);
    DsReleaseObj(camera_group);

    DgEmpty(DvInqDisplayGroup(view));
    DgEmpty(DvInqDefinitionGroup(view));

    for (i=0; i<4; i++) {
      if (separate_disp_groups) {
	 DsReleaseObj(DgInqObjAtPos(DvInqDisplayGroup(m_view[i]), 0, DcBeginning));
	 DsReleaseObj(DgInqObjAtPos(DvInqDefinitionGroup(m_view[i]), 0, DcBeginning));
      }
      DgEmpty(DvInqDisplayGroup(m_view[i]));
      DgEmpty(DvInqDefinitionGroup(m_view[i]));
    }
}
/*
======================================================================
*/
read_input_file()
{
    DtInt i;
    char tmpstring[200];
    char		cam_name[10];
    DtObject make_multiplecameras();

    scenefile_read(input_filename);

    object_group = DsInqObjViaString (objectgroupname, DcTypeGroup);
    camera_group = DsInqObjViaString (cameragroupname, DcTypeGroup);
    DsHoldObj(object_group);
    DsHoldObj(camera_group);

    if (check_groups) {
      if (check_group_network(objectgroupname, object_group) != DcGroupOk) {
	 DsTerminate ();
	 return 0;
      }
      if (check_group_network(cameragroupname, camera_group) != DcGroupOk) {
	 DsTerminate ();
	 return 0;
      }
    }

    if (!separate_disp_groups) {
       DgOpen(camera_group, DcFalse);
       DgSetElePtr (0, DcBeginning);
       DgAddObj(make_multiplecameras());
       DgClose();
    }

    DgAddObjToGroup(DvInqDisplayGroup(view),object_group);
    DgAddObjToGroup(DvInqDefinitionGroup(view),camera_group);

    if (!separate_disp_groups) {
      camera_group1 = camera_group;
      object_group1 = object_group;
    }

    for (i=0; i<4; i++) {
      if (separate_disp_groups) {
	 sprintf (tmpstring, m_objectgroupname, i+1);
	 object_group1 = DsInqObjViaString (tmpstring, DcTypeGroup);
	 DsHoldObj(object_group1);
	 if (check_groups) {
	    if (check_group_network(tmpstring, object_group1) != DcGroupOk) {
	       DsTerminate ();
	       return 0;
	    }
	 }
	 sprintf (tmpstring, m_cameragroupname, i+1);
	 camera_group1 = DsInqObjViaString (tmpstring, DcTypeGroup);
	 DsHoldObj(camera_group1);
	 if (check_groups) {
	    if (check_group_network(tmpstring, camera_group1) != DcGroupOk) {
	       DsTerminate ();
	       return 0;
	    }
	 }
      }
      DgAddObjToGroup(DvInqDefinitionGroup(m_view[i]),camera_group1);
      DgAddObjToGroup(DvInqDisplayGroup(m_view[i]),object_group1);
      if (!separate_disp_groups) {
	 sprintf (cam_name, "camera%d", i+1);
	 DvSetActiveCamera(m_view[i],
			   DsInqObjViaString (cam_name, DcTypeCamera));
      }
    }

    if (show_picks) {
      DtReal color[3];
      color[0] = color[1] = color[2] = .1;
      DgOpen (DvInqDisplayGroup(view),DcFalse);
	 DgSetElePtr(0,DcBeginning);
	 DgAddObj(pickgroup = DoGroup(DcFalse));
      DgClose();
    }

    return 1;
}

#ifdef OFFSET_OUTLINE
/*
======================================================================
*/
#ifdef USE_TITANG2
#include "../src/graphic_config/x11_titang2/dynout/do.con.h"
#include "../src/graphic_config/x11_titang2/dynout/do.str.h"
#include "../src/graphic_config/x11_titang2/dynout/cg.con.h"
#include "../src/graphic_config/x11_titang2/dynout/cg.str.h"
#include "../src/graphic_config/x11_titang2/dynout/do.ext.h"
#else
#include "../src/graphic_config/x11_DGL/dynout/do.con.h"
#include "../src/graphic_config/x11_DGL/dynout/do.str.h"
#include "../src/graphic_config/x11_DGL/dynout/do.ext.h"
#endif

/*
======================================================================
*/
set_offset()
{
    if (reptype != DcSurface) {
	DsExecutionReturn();
    }

    if (DsInqCurrentMethod() == DcMethodDynRender) {
#       ifdef USE_TITANG2
	    Pe_x11_titang2_dynout_cg_sparms.cg_xlate[2] += outline_offset;
#       else
	    Pe_x11_DGL_dynout_curdevdat->ctodtrans[2] += outline_offset/65535.0;
	    XdSetViewportMapping
	    (   Pe_x11_DGL_dynout_curdevdat->pGC,
		Pe_x11_DGL_dynout_curdevdat->ctodscale,
		Pe_x11_DGL_dynout_curdevdat->ctodtrans
	    );
#       endif
    }
}

/* ====================================================================== */

clr_offset()
{
    if (DsInqCurrentMethod() == DcMethodDynRender) {
#       ifdef USE_TITANG2
	    Pe_x11_titang2_dynout_cg_sparms.cg_xlate[2] -= outline_offset;
#       else
	    Pe_x11_DGL_dynout_curdevdat->ctodtrans[2] -= outline_offset/65535.;
	    XdSetViewportMapping
	    (   Pe_x11_DGL_dynout_curdevdat->pGC,
		Pe_x11_DGL_dynout_curdevdat->ctodscale,
		Pe_x11_DGL_dynout_curdevdat->ctodtrans
	    );
#       endif
    }
}

/* ====================================================================== */

#include "device_if/dcm.str.h"
#include "device_if/if.type.h"
#include "../src/subsys/device/do.con.h"
#include "../src/subsys/device/do.str.h"
#include "../src/subsys/device/do.ext.h"

#include <X11/XB.h>
#ifdef USE_TITANG2
#   include "machine/tigr.h"
#   include "machine/rasterizer.h"
#   include "../src/graphic_config/x11_titang2/device/do.con.h"
#   include "../src/graphic_config/x11_titang2/device/do.str.h"
#   include "../src/graphic_config/x11_titang2/device/do.ext.h"
#else
#   include "../src/graphic_config/x11_DGL/device/do.con.h"
#   include "../src/graphic_config/x11_DGL/device/do.str.h"
#   include "../src/graphic_config/x11_DGL/device/do.ext.h"
#endif

static void inquire_device_extent (volume)
    DtVolume *volume;
{
#   ifdef USE_TITANG2
	volume->bll[0] = XExtent(Pe_x11_titang2_current_device_data).x1;
	volume->bll[1] = 1024-XExtent(Pe_x11_titang2_current_device_data).y2;
	volume->bll[2] = 0.;

	volume->fur[0] = XExtent(Pe_x11_titang2_current_device_data).x2;
	volume->fur[1] = 1024-XExtent(Pe_x11_titang2_current_device_data).y1;
	volume->fur[2] = 65000.; 
#   else
	volume->bll[0] = XExtent(Pe_x11_DGL_current_device_data).x1;
	volume->bll[1] = 1024-XExtent(Pe_x11_DGL_current_device_data).y2;
	volume->bll[2] = 0.;

	volume->fur[0] = XExtent(Pe_x11_DGL_current_device_data).x2;
	volume->fur[1] = 1024-XExtent(Pe_x11_DGL_current_device_data).y1;
	volume->fur[2] = 65000./65535.; 
#   endif
}

/* ====================================================================== */

void fixup_device()
{
    Pt_DCM_fcns *DCM_fcns;
    DtInt i;
    extern void inquire_device_extent();
#   ifdef USE_TITANG2
	extern Pr_x11_titang2_return_functions();
	DtPFI return_functions = Pr_x11_titang2_return_functions;
#   else
	extern Pr_x11_DGL_return_functions();
	DtPFI return_functions = Pr_x11_DGL_return_functions;
#   endif

    (*return_functions)(Pc_DCM_fcns, &DCM_fcns);

    DCM_fcns->inquire_device_extent = (DtPFI)inquire_device_extent;

    for (i=0; doe_device_drivers[i] != NULL; i++) {
      if (doe_device_drivers[i]->get_interface_functions == return_functions) {
	 doe_device_drivers[i]->inquire_device_extent =
	       (DtPFI)inquire_device_extent;
	 break;
      }
    }
}

#endif OFFSET_OUTLINE
