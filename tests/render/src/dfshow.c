/****
*
*	Dore program to display stoke characters in Dana Format
*
*	written: M. L. Patrick 9/14/87
*
****/

#include "stdio.h"
#include "math.h"
#include "dore.h"

#ifdef sun
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#endif

#define round(x)	((x>0)?((int)(x+0.5)):((int)(x-0.5)))
#define Border 0

extern char *getenv() ;
DtObject device ;
DtObject frame;
DtObject view ;
DtObject camera_group, object_group;
DtVolume volume;
DtVolume device_extent ;
char errmess[200] ;
DtReal xscale, yscale, scale ;
DtInt first_char, last_char ;
char **font_argv;
int font_argc;
DtFlag show_all;


#ifdef sun
Notify_value	event_proc ();
void		do_repaint();
Frame		sun_frame;
Canvas		sun_canvas;
#endif

DtReal backgroundcolor[3];
DtReal foregroundcolor[3];

error(routine_name, message)
     char *routine_name ;
     char *message ;
{
   fprintf(stderr,"error detected by %s\n", routine_name) ;
   fprintf(stderr, " ****- %s\n", message) ;
   exit(1) ;
}

text(x, y, string, height, font, halign, valign)
     DtReal x, y ;
     char *string ;
     DtReal height ;
     DtInt font ;
     DtInt halign ;
     DtInt valign ;
{
   static DtPoint3 u[] = {1.,0.,0.} ;
   static DtPoint3 v[] = {0.,1.,0.} ;
   DtPoint3 position ;

   position[0] = x ;
   position[1] = y ;
   position[2] = 0. ;

   DgAddObj(DoTextAlign(halign, valign)) ;
   DgAddObj(DoTextExpFactor(1.0)) ;
   DgAddObj(DoTextFont(font)) ;
   DgAddObj(DoTextHeight(height)) ;
   DgAddObj(DoTextPath(DcTextPathRight)) ;
   DgAddObj(DoTextPrecision(DcStrokePrecision)) ;
   DgAddObj(DoTextSpace(0.0)) ;
   DgAddObj(DoTextUpVector(0.0, 1.0)) ;
   DgAddObj(DoText(position, u, v, string)) ;
}

open_device(device_type, connection)
     char *device_type ;
     char *connection ;
{
   static char *proc_name = "open_device" ;
   DtPoint3 at, from ;
   DtVector3 up ;

   DsInitialize(0) ;

   if (! (device = DoDevice(device_type, connection)) ) {
      sprintf(errmess, "Could not open device %s", connection) ;
      error(proc_name, errmess) ;
   }
		
   DdInqExtent(device, &volume) ;

   if ( ! (frame = DoFrame() ) ) {
      printf("can't create frame\n") ;
      exit(1) ;
   }

   DdSetFrame(device, frame) ;

   volume.bll[0] = 0. ;
   volume.bll[1] = -0.2 ;

   volume.fur[0] = 12. ;
   volume.fur[1] = 9.5 ;

   at[0] = 0. ;
   at[1] = 0. ;
   at[2] = 0. ;

   from[0] = 0. ;
   from[1] = 0. ;
   from[2] = 1. ;

   up[0] = 0. ;
   up[1] = 1. ;
   up[2] = 0. ;

   DfSetBoundary(frame, &volume) ;

   if ( ! (view = DoView() ) ) {
      printf("can't create view\n") ;
      exit(1) ;
   }

   DgAddObjToGroup(DfInqViewGroup(frame),view);

   DvSetBackgroundColor(view, DcRGB, backgroundcolor);

   DvSetBoundary(view, &volume) ;

   DvSetRendStyle(view, DcRealTime) ;

   scenefile_read("objects.dou") ;

   camera_group = DsInqObj(DcNameString, "cameragroup", DcTypeGroup);

   if ( camera_group == DcNullObject ) {
      printf("camera group not defined\n") ;
      exit(1) ;
   }

   object_group = DoGroup(DcFalse) ;
	
   DgAddObjToGroup(DvInqDisplayGroup(view),DoDiffuseColor(DcRGB, foregroundcolor));
   DgAddObjToGroup(DvInqDisplayGroup(view),object_group);
   DgAddObjToGroup(DvInqDefinitionGroup(view),camera_group);
}

close_device()
{
   DsReleaseObj(device);
   DsTerminate() ;
}

char *font_name(font_id)
     DtInt font_id ;
{
   switch (font_id) {
   case DcPlainRoman:            return "Plain Roman" ;
   case DcSimplexRoman:          return "Simplex Roman" ;
   case DcDuplexRoman:           return "Duplex Roman" ;
   case DcComplexSmallRoman:     return "Complex Small Roman" ;
   case DcComplexRoman:          return "Complex Roman" ;
   case DcTriplexRoman:          return "Triplex Roman" ;
   case DcComplexSmallItalic:    return "Complex Small Italic" ;
   case DcComplexItalic:         return "Complex Italic" ;
   case DcTriplexItalic:         return "Triplex Italic" ;
   case DcSimplexScript:         return "Simplex Script" ;
   case DcComplexScript:         return "Complex Script" ;
   case DcGothicGerman:          return "Gothic German" ;
   case DcGothicEnglish:         return "Gothic English" ;
   case DcGothicItalian:         return "Gothic Italian" ;
   case DcPlainGreek:            return "Plain Greek" ;
   case DcSimplexGreek:          return "Simplex Greek" ;
   case DcComplexSmallGreek:     return "Complex Small Greek" ;
   case DcComplexGreek:          return "Complex Greek" ;
   case DcComplexCyrillic:       return "Complex Cyrillic" ;
   case DcUpperCaseMathematics:  return "Upper Case Mathematics" ;
   case DcLowerCaseMathematics:  return "Lower Case Mathematics" ;
   case DcMusic:                 return "Music" ;
   case DcMeteorology:           return "Meteorology" ;
   case DcSymbols:               return "Symbols" ;
   case DcAstrology:             return "Astrology" ;
   case DcHelvetica:             return "Helvetica" ;
   default: return NULL ;
   }
   return NULL;
}

draw_characters(font_id, first, last)
     DtInt font_id ;
     DtInt first, last ;
{
   int character ;
   DtReal x, y ;
   char string[10] ;

   for ( character = first ; character <= last ; character++ ) {
      x = (character-first)%12 ;
      y = 7.-(int)((character-first)/12) ;
      string[0] = character ;
      string[1] = 0 ;
      text(x+.5, y+.3, string, 0.4, font_id, DcTextHAlignCenter,
	   DcTextVAlignNormal) ;
      if ( ( character >= 32 ) && ( character <= 127 ))
	    text(x+.05, y+.05, string, 0.1, DcSimplexRoman, 
		 DcTextHAlignLeft, DcTextVAlignNormal) ;
      sprintf(string, "%d", character) ;
      text(x+.95, y+.05, string, 0.1, DcSimplexRoman, DcTextHAlignRight,
	   DcTextVAlignNormal) ;
   }

   text(6., 8.75, font_name(font_id), 0.4, DcSimplexRoman, 
	DcTextHAlignCenter, DcTextVAlignHalf) ;

   text(12., -.13, "Kubota Pacific Computer Inc. - Dore Fonts", 
	0.1, DcSimplexRoman, DcTextHAlignRight, 
	DcTextVAlignNormal) ;
}

display_font_screen(font_id)
     DtInt font_id ;
{
   static int save_font_id = -1;
   static int first_display_char, last_display_char, nscreens, nchars ;
   static int screen = 0;

   if (font_id == save_font_id) {
      if (screen >= nscreens) {
	 save_font_id = -1;
	 return 0;
      }
   } else {
/*      dor_font_inquire_bounds(font_id, &first_char, &last_char) ; */
      first_char = 0;
      last_char = 127;

      nchars = last_char-first_char+1 ;
      nscreens = nchars/96 ;
      if ( nchars%96 != 0 )
	    nscreens++ ;

      first_display_char = first_char ;
      screen = 0;
      save_font_id = font_id;
   }

   DgEmpty(object_group) ;
   DgOpen(object_group, DcTrue) ;

   last_display_char = first_display_char+95 ;
   if ( last_display_char > last_char )
	 last_display_char = last_char ;

   draw_characters(font_id, first_display_char, last_display_char) ;

   DgClose() ;
   DdUpdate(device);

   first_display_char += 96 ;
   
   screen++;
   if (screen < nscreens)
	 return 1;
   else
	 return 0;
}

main(argc, argv)
     int argc ;
     char **argv ;
{
   char devicetype[200];
   char argstring[200];
   int font, i ;
   DtInt width, height;

   backgroundcolor[0] = .0;
   backgroundcolor[1] = .0;
   backgroundcolor[2] = .0;

   foregroundcolor[0] = 1.0;
   foregroundcolor[1] = 1.0;
   foregroundcolor[2] = 1.0;

   prsarg_get_keyword_colorRGB(argc,argv,"-background", backgroundcolor, backgroundcolor);
   prsarg_get_keyword_colorRGB(argc,argv,"-foreground", foregroundcolor, foregroundcolor);

#ifdef sun
   prsarg_get_keyword_string(argc,argv,"-dt","sunviewGP",devicetype);
#else
   prsarg_get_keyword_string(argc,argv,"-dt","stardentx11",devicetype);
#endif
   prsarg_get_keyword_int(argc,argv,"-width",512,&width);
   prsarg_get_keyword_int(argc,argv,"-height",512,&height);

   prsarg_get_keyword_switch(argc,argv,"-all",1,&show_all);

   prsarg_get_keyword_string(argc,argv,"-df"," ", argstring);

#ifdef sun
   if (!strcmp (devicetype, "sunviewGP")) {
      setup_sunview_window(width, height, argstring);
   }
#endif
#ifdef titan
   if (!strcmp (devicetype, "stardentx11")) {
      setup_ardentx11_window(width, height, argstring);
   }
#endif
   open_device(devicetype, argstring) ;

#ifdef sun
   if (!strcmp(devicetype, "sunviewGP")) {
      font_argv = argv;
      font_argc = argc;
      display_font_screen(DcPlainRoman);
      window_main_loop(sun_frame);
      close_device();
      exit (0);
   } 
#endif

   if ( show_all ) {
      for ( font = DcPlainRoman; font <= DcHelvetica ; font++ ) {
	 while (display_font_screen(font)) {
	    if (!strcmp(devicetype,"stardentx11")) {
	       if (getchar() == 'b') {
		  font = font - 2;
	       }
	    }
	 }
      }
   } else  {
      for ( i = 1 ; i < argc ; i++ ) {
	 if (strcmp(argv[i], "")) {
	    font = atoi(argv[i]) ;
	    if (( font >= DcPlainRoman ) && ( font <= DcHelvetica )) 	
		  while (display_font_screen(font)) {
		     if (!strcmp(devicetype,"stardentx11")) {
			if (getchar() == 'b') {
			   font = font - 2;
			}
		     }
		  }
	    else {
	       sprintf(errmess, "unknown font %d", font) ;
	       error(argv[0], errmess) ;	
	    }
	 }
      }
   }

   close_device() ;
}
#ifdef sun
/*
  ======================================================================
 */
setup_sunview_window(width, height, argstring)
     DtInt width;
     DtInt height;
     char *argstring;
{
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
}
/*
  ======================================================================
 */
void
do_repaint (canvas, pw, area)
     Canvas canvas;
     Pixwin *pw;
     Rectlist *area;
{
   DdUpdate (device);
}
/*
  ======================================================================
 */
Notify_value 
event_proc (window, event, arg)
     Window window;
     Event *event;
     caddr_t arg;
{
   static int font = DcPlainRoman;
   static int done = 0;
   static int argi = 1;

   if (event_is_ascii(event)) {
      switch (event_id(event)) {
      case 'x':
	 window_set(sun_frame, FRAME_NO_CONFIRM, TRUE, 0);
	 window_done(window);
	 break;
      case 'b':
	 if ( show_all ) {
	    font = font-2;
	    if (font < DcPlainRoman) font = DcPlainRoman;
	    if (!display_font_screen(font))
		  font++;
	 } else {
	    argi = argi - 2;
	    if (argi < 1) argi = 1;
	    if (strcmp(font_argv[argi],"")) {
	       font = atoi(font_argv[argi]) ;
	       if (( font >= DcPlainRoman ) && ( font <= DcHelvetica )) {
		  if (!display_font_screen(font))
			argi++;
	       } else {
		  sprintf(errmess, "unknown font %d", font) ;
		  error(font_argv[0], errmess) ;	
	       }
	    }
	 }
	 break;
      case 'f':
	 if ( show_all ) {
	    if (font > DcHelvetica) {
	       font = DcPlainRoman;
	    } 
	    if (!display_font_screen(font))
		  font++;
	 } else {
	    while (argi < font_argc && !strcmp (font_argv[argi],"")) argi++;
	    if (argi < font_argc) {
	       font = atoi(font_argv[argi]) ;
	       if (( font >= DcPlainRoman ) && ( font <= DcHelvetica )) {
		  if (!display_font_screen(font))
			argi++;
	       } else {
		  sprintf(errmess, "unknown font %d", font) ;
		  error(font_argv[0], errmess) ;	
	       }
	    }
	 }
	 break;
      default: 
	 DdUpdate(device);
	 break;
      }
   }
}
#endif
#ifdef titan
/*
 ======================================================================
 */
setup_ardentx11_window(width, height, argstring)
     DtInt width;
     DtInt height;
     char *argstring;
{
   sprintf (&(argstring[strlen(argstring)]), 
	    " -geometry %dx%d+0+0",
	    width,height);
}
#endif
