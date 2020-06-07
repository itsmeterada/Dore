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
 
#include <sys/types.h>
#include <sys/times.h>
#include <dore/dore.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include "Trnsfrmr.h"

#ifdef HAS_POLL
#include <poll.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#ifndef M_PI
#define	M_PI		3.14159265358979323846
#endif
#define DTOR(angle) (angle*M_PI/180.0)  /* degrees to radians */
#define RTOD(angle) (angle*180.0/M_PI)  /* radians to degrees */

DtVolume bound;

DtObject device;
DtObject frame;
DtObject view;
DtObject reptypeInline;
DtObject interptypeInline;
DtObject backfaceInline;
DtObject specularInline;
DtObject subdivInline;

DtReal model_size;
DtReal intens = .70;
DtReal zoom = 25.;
DtReal backcolor[3] = { .1, .1, .1 };

DtInt processors;
DtInt backfacecullswitch = 0;
DtInt highlightswitch = 0;
DtInt reptype = 2;
DtInt shadingtype = DcConstantShade;
DtInt renderstyle = DcRealTime;
DtInt autocycle = 1;
DtInt autorotate = 0;
DtInt autox = 0;
DtInt autoy = 0;
DtInt autoz = 0;
DtInt update;
DtInt update_modified = DcFalse;
DtInt model_num = 0;
DtInt debug;

DtSwitch script;
DtSwitch script_in;
DtSwitch pseudo;
DtSwitch singleb;
DtSwitch shade;

char *pseudostr = " ";
char *singlebstr = " ";
char devicetype[80];
char devicefile[1024];
char argstring[1000];
char dummy[100];
char command[100];
char *getenv();
char outbuf[BUFSIZ];

/* geom_spec global variables */
int geom_again = 0;
extern int nmodels;
extern int firstcycle;
extern int lastcycle;

/* Timing test global variables */
struct tms tbuf0, tbuf1;
long time0;
long time1;
long elapsetime;
long cputime;
long systemtime;
long usertime;

/* Input device global variables */
int ndev;
int kbd_fd;
#ifdef HAS_POLL
int spb;
struct pollfd fds[8];	/* More than we need.  But it doesn't hurt. */
int Fds_kbd;
#else
fd_set fds;
struct timeval tval;
#endif

/* rotate & translate global variables */
DtReal rot_incr = (.1 / 2.);
DtReal tran_inc = .1;
DtReal xrotatevalue = 0.;
DtReal yrotatevalue = 0.;
DtReal zrotatevalue = 0.;
DtReal xtranslatevalue = 0.;
DtReal ytranslatevalue = 0.;
DtReal ztranslatevalue = 0.;
extern DtObject tf_rot_inline;
extern DtObject tf_tran_inline;
extern DtInt tf_rot;
extern DtInt tf_tran;
static float tf_rotx_val = 0.;
static float tf_roty_val = 0.;
static float tf_rotz_val = 0.;



/*******************************************************************************
 ******************************  r e n d e r  **********************************
 *******************************************************************************
 * Module Name: main (alias 'render')
 * Module Type: c
 *     Purpose: This is the main routine for most of the demo programs.  It
 *		- initializes the Dore' environment
 *		- creates the model and studio objects
 *		- displays the objects
 *		- conditionally regenerates model objects
 *		- allows users interaction with the objects
 *		- exits the Dore/Xwindows environment & this program
 *   Arguments: argc ==> number of command line arguments passed to this program
 *		argv ==> array of command line argument strings
 * ReturnValue: None
 ******************************************************************************/
main(argc,argv)
int argc;
char *argv[];
{
/*******************************************************************************
 * Declare variables & functions
 ******************************************************************************/
	DtVolume xbound;
   	extern DtObject studio_group, model_group;

	setvbuf(stdout, outbuf, _IOLBF, BUFSIZ);

/*******************************************************************************
 * 1. Initialize Dore' and this program
 * 2. Make the model objects for the Dore' database
 * 3. Make the studio objects and add them to the Dore' database
 * 4. Add the model & studio objects to the current view (thereby 
 *    outputting them to the appropriate display device)
 ******************************************************************************/
   	do_init(argc,argv);
   	make_models(argc, argv);
   	make_studios();
   	DgAddObjToGroup(DvInqDefinitionGroup(view),studio_group);
   	DgAddObjToGroup(DvInqDisplayGroup(view),model_group);

/*******************************************************************************
 * Conditionally regenerate model objects
 ******************************************************************************/
	if (geom_again) {
		DdUpdate(device);
		geom_spec(&xbound, argc, argv);
		DdUpdate(device);
	}

/*******************************************************************************
 * Handle user interaction with the model, such as dynamic rotation, interpo-
 *  lation type, etc.
 ******************************************************************************/
   	do_interact();

/*******************************************************************************
 * Exit the Dore' environment (and this program) cleanly
 ******************************************************************************/
   	DsReleaseObj(device);
   	DsTerminate();

} /* End of main routine */


/*******************************************************************************
 *****************************  d o _ i n i t  *********************************
 *******************************************************************************
 * Module Name: do_init
 * Module Type: c
 *     Purpose: Initialize Dore' and get any input arguments passed to this
 *              program by the user.
 *   Arguments: argc ==> number of command line arguments
 *              argv ==> command line argument strings
 * ReturnValue: None
 ******************************************************************************/
do_init(argc,argv)
int argc;
char *argv[];
{
	DtSwitch defalt;
   	DtReal devicewidth, deviceheight;
	char *envvar;
   	int i;

/*******************************************************************************
 * Parse any arguments pass to this program, using defaults for any not passed.
 * Each keyword is prefixed with a '-' and followed by a value parameter. The
 * valid keywords are:
 *  dt ==> device type (ardentx11 by default) specifies the type of "hardware" 
 *	that the Dore' graphics will be output to;  this could be a plotter,
 *	a display screen, a disk file, ... (eg.- RUNME -dt ardentx11 or 
 *	RUNME -dt rasterfile) (there is no default)
 *  df ==> device file (Xwindows geometry spec by default) specifies any 
 *	device-specific information about the Dore' graphics output "hardware"
 *	(eg.- RUNME -df "-geometry 640x480+0+0" or RUNME -df raster.file)
 *	(there is no default)
 *  s ==> echo input commands to stdout (use: RUNME -s 1 > scriptfile) used to
 *	capture the command sequence for later replay (default is 0)
 *  S ==> stdin input is redirected from a file (use: RUNME -S 1 < scriptfile)
 *	used to control this program via commands from a file instead of 
 *	from the keyboard; WARNING: this input is read via a poll command and
 *	since polling doesn't support input from disk files, results from 
 *	using this option seem to work, but are not guaranteed!! (default is 0)
 *      (only true if compiled with HAS_POLL)
 *  w ==> use the specified, pre-existing Xwindow for the Dore' graphical output
 *	output; this assumes that the user has already created an Xwindow and
 * 	has the id of it;  the id must be specified in decimal, not hexadecimal
 *	(eg.- RUNME -w 15724386) (if not specified, this program will create
 *	its own window via Dore')
 *  model ==> which (of multiple) models to display first (eg.- RUNME -model 5)
 *	(default is 0)
 *  procs ==> set the number of processors (threads) to be used for Dore'
 *	(eg.- RUNME -procs 0 or RUNME -procs 2) (default is 0)
 *  d ==> debug mode; if set to 1 turns debug printing on; 0 is off
 *	(eg.- RUNME -d 1) (default is 0)
 *
 * The following environment variables are also used by this program:
 *  DISPLAY ==> must be set to the Xwindows display being used -- normally
 *      this will be 'unix:0' (used in module Ui.c)
 *  DEMODIR ==> set to the full path name of the demo directory -- this is 
 *	usually, but not necessarily, "~demo", the login directory of the demo 
 *	user id.
 *  PSEUDO ==> used to set the default value for the 'pseudo' variable 
 *  SHADE ==> used to set the default value for the 'shade' variable 
 *  SINGLEB ==> used to set the default value for the 'singleb' variable 
 ******************************************************************************/
	prsarg_get_keyword_switch(argc,argv,"-s",0,&script);
	prsarg_get_keyword_switch(argc,argv,"-S",0,&script_in);

	defalt = 0;
	if ((envvar=getenv("PSEUDO")) && (envvar[0] == '1')) defalt = 1;
	prsarg_get_keyword_switch(argc,argv,"-pseudo",defalt,&pseudo);
	defalt = 0;
	if ((envvar=getenv("SHADE")) && (envvar[0] == '1')) defalt = 1;
	prsarg_get_keyword_switch(argc,argv,"-shade",defalt,&shade);
	defalt = 0;
	if ((envvar=getenv("SINGLEB")) && (envvar[0] == '1')) defalt = 1;
	prsarg_get_keyword_switch(argc,argv,"-singleb",defalt,&singleb);

	prsarg_get_keyword_int(argc,argv,"-model",0,&model_num);
	prsarg_get_keyword_int(argc,argv,"-procs",0,&processors);
	prsarg_get_keyword_int(argc,argv,"-d",0,&debug);
	prsarg_get_keyword_string(argc,argv,"-dt","stdx",devicetype);
	prsarg_get_keyword_string(argc,argv,"-df",
				 "-width 512 -height 512",argstring);

	if (singleb) singlebstr = "-singlebuffered";
	if (pseudo) pseudostr = "-visualtype DcPseudoColor";
	sprintf(devicefile,"%s %s %s",
		argstring,singlebstr,pseudostr);
	if ((envvar = getenv("DEVICEFILE")))
	    strcpy(devicefile, envvar);
/*******************************************************************************
 * Echo back the command line that invoked this program.
 ******************************************************************************/
   	if (debug) {
		   printf("invoked with :\n  \"");
		   for(i=0;i<argc;i++) printf("%s ",argv[i]);
		   printf("\"\n");
	}

/*******************************************************************************
 * Setup the devices from which input to the program is expected.  These 
 *  devices will be polled to check for any incoming data to control the
 *  program.
 ******************************************************************************/

	/* Stdin -- normally the keyboard */
	kbd_fd = fileno(stdin);
#ifdef HAS_POLL
	ndev = 0;
	Fds_kbd = ndev++;
	fds[Fds_kbd].fd = kbd_fd;
	fds[Fds_kbd].events = POLLIN;
#else
	ndev = kbd_fd + 1;
	tval.tv_sec = 0;	/* set to poll only */
	tval.tv_usec = 0;
#endif

/*******************************************************************************
 * Initialize the Dore' environment.
 ******************************************************************************/

	/* Initialize Dore', telling it how many threads (processors) to use. */
	if (debug) printf("Initializing Dore.\n");
   	DsInitialize(processors);

	/* Open the specified Dore' output device, determine its horizontal and 
	 *  vertical limits, and set the viewport based on that.  */
	if (debug) printf("DoDevice(type = '%s', file = '%s')\n",
		devicetype,devicefile);

   	if (!(device = DoDevice(devicetype,devicefile))) {
      		printf("can't create device:  type = '%s', file = '%s'\n",
			devicetype,devicefile);
      		exit(1);
   	}

   	DdInqExtent(device,&bound);
   	devicewidth = bound.fur[0] - bound.bll[0];
   	deviceheight = bound.fur[1] - bound.bll[1];
   	if(debug) printf("device width = %g, height = %g\n", devicewidth,
      		 deviceheight);
   	DdSetViewport(device,&bound);


	/* Create a frame to display the view(s) in and map
	 * it to the output device.  */
   	if (!(frame = DoFrame())) {
      		printf("can't create frame\n");
      		exit(1);
   	}
   	DdSetFrame(device,frame);
   	DfSetBoundary(frame,&bound);


	/* Create a view to render the objects in.  */
   	if (!(view = DoView())) {
      		printf("can't create view\n");
      		exit(1);
   	}
   	DvSetClearFlag(view,DcTrue);
   	DvSetRendStyle(view,renderstyle);

   	DgAddObjToGroup(DfInqViewGroup(frame),view);
   	DvSetBoundary(view,&bound);
   	DvSetBackgroundColor(view,DcRGB,backcolor);


	/* Print out bound of the device, frame, & view if debug is set.  */
   	if(debug) {
      		DdInqViewport(device,&bound);
      		printf("device=%g,%g,%g\n      %g,%g,%g\n",
         	    bound.bll[0], bound.bll[1], bound.bll[2],
         	    bound.fur[0], bound.fur[1], bound.fur[2]);
      		DfInqBoundary(frame,&bound);
      		printf("frame=%g,%g,%g\n      %g,%g,%g\n",
         	    bound.bll[0], bound.bll[1], bound.bll[2],
         	    bound.fur[0], bound.fur[1], bound.fur[2]);
      		DvInqBoundary(view,&bound);
      		printf("view=%g,%g,%g\n      %g,%g,%g\n",
         	    bound.bll[0], bound.bll[1], bound.bll[2],
         	    bound.fur[0], bound.fur[1], bound.fur[2]);
   	}

} /* End of do_init function */


/*******************************************************************************
 *************************  d o _ i n t e r a c t  *****************************
 *******************************************************************************
 * Module Name: do_interact
 * Module Type: c
 *     Purpose: Accept and process interactive input from the keyboard
 *              or whatever, to "dynamically" manipulate the model.  This is 
 *		done using the Unix "poll" function to determine which of the 
 *              devices has input waiting to be processed.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
do_interact()
{
   	int ifds;

/*******************************************************************************
 * Post the initial view of the scene to the display device.
 ******************************************************************************/
	printf("Displaying initial view of the model...");
	fflush(stdout);
   	DdUpdate(device);
   	update_modified = DcTrue;
	printf("Ready to proceed!\n");

	if (pseudo) {
		ctab_setup();
		ctab_select(0);
	}

/*******************************************************************************
 * Enter an inifinite loop, waiting for input from any of the devices declared
 *  in do_init.  Wait at the top of the loop until input is received, unless
 *  one of the auto flags is set.  If one of the auto flags is set, check for 
 *  input, but don't wait around if there isn't any -- this allows for automatic
 *  modification of variables and display update.
 ******************************************************************************/
   	while(1) {
      		update = 0;

      		if(autorotate || autocycle) {
#ifdef HAS_POLL
         		poll(fds,ndev,0);
#else
			FD_ZERO(&fds);
			FD_SET(kbd_fd, &fds);
			select(ndev, &fds, NULL, NULL, &tval);
#endif
	 		update = 1;

         		if(autox) xrotatevalue += rot_incr;
         		if(autoy) yrotatevalue += rot_incr;
         		if(autoz) zrotatevalue += rot_incr;
			if (autocycle)
				if (++model_num > lastcycle)
					model_num=firstcycle;
      		} else {
#ifdef HAS_POLL
         		poll(fds,ndev,-1);
#else
			FD_ZERO(&fds);
			FD_SET(kbd_fd, &fds);
			select(ndev, &fds, NULL, NULL, &tval);
#endif
      		}

      		if(debug) {
#ifdef HAS_POLL
         		printf("after poll, ");
         		for(ifds=0;ifds<ndev;ifds++) 
				printf("%d ",fds[ifds].revents);
         		printf("\n");
#else
         		printf("after select, ");
         		for(ifds=0;ifds<ndev;ifds++) {
				if(FD_ISSET(ifds,&fds)) {
					printf("%d ", ifds);
				}
			}
         		printf("\n");
#endif
      		}

/*******************************************************************************
 * If there is input waiting from one of the declared devices, process it
 ******************************************************************************/


		/* Input from the keyboard. The keyboard is assumed to be
		 *  stdin.  Since polling doesn't work from files, results 
		 *  are unpredictable if stdin is redirected to a file.
		 *  However, POLLNVAL is included to attempt to accept
		 *  input from stdin redirected to a file. */
#ifdef HAS_POLL
      		if (fds[Fds_kbd].revents == POLLIN ||
		    fds[Fds_kbd].revents == POLLNVAL)
#else
      		if (FD_ISSET(kbd_fd, &fds))
#endif
		{
	 		update = 1;
	 		read_input(kbd_fd);
      		}
		 

/*******************************************************************************
 * If the update flag is set to -1, that means it's time to exit this program.
 * Break out of the infinite while loop and return to the main routine to exit.
 ******************************************************************************/
		if (update == -1) break;


/*******************************************************************************
 * If the update flag is set, update the current display.
 ******************************************************************************/

      		if (update)
			update_display();

   	}  /* end of infinite while */

}  /* End of do_interact function */

update_display()
{
	DdUpdate(device);
	update_modified = DcTrue;

}


/*******************************************************************************
 *************************  p a r s e _ i n p u t  *****************************
 *******************************************************************************
 * Module Name: parse_input
 * Module Type: c
 *     Purpose: Parse the passed string.
 *   Arguments: command ==> a single ascii string command.
 * ReturnValue: None
 ******************************************************************************/
parse_input(command)
char *command;
{
	if (debug) printf("Command to parse: %s\n",command);
	if (script) printf("%s\n",command);

/*******************************************************************************
 * See if the user knows what to do with the command.  If so, we are done.
 ******************************************************************************/
	if (user_parse(command)==0)
		default_parse(command);

/*******************************************************************************
 * If the input command is from a file, update the display.
 ******************************************************************************/
	if (script_in) {
		DdUpdate(device);
		update_modified = DcTrue;
		update = 0;
	}
}  /* End of parse_input function */



/*******************************************************************************
 ***********************  d e f a u l t _ p a r s e  ***************************
 *******************************************************************************
 * Module Name: default_parse
 * Module Type: c
 *     Purpose: Parse input commands not already parse by the application
 *		specific user_parse function.
 *   Arguments: command ==> a single ascii string command
 * ReturnValue: None
 ******************************************************************************/
default_parse(command)
char *command;
{
/*******************************************************************************
 * Declare variables.
 ******************************************************************************/
   	char c;
   	float xrot, yrot, zrot;
	float xtran, ytran, ztran;
	float tmpbg[3];
	DtReal bg[3];
	float value;
	int width, height;	/* of raytraced image */
	int repeat;
	int ct;

/*******************************************************************************
 * Get the first character of the command.  If it's an 'x' or a 'q', set the
 *  update flag to exit the program and return.  Otherwise, continue processing
 *  the remainder of the command.
 ******************************************************************************/
	if (debug)
		printf("Default_parse: '%s'\n", command);

   	c = *(command++);

   	if ((c == 'x') | (c == 'q')) {
	   	update = -1;
		return;
	}

/*******************************************************************************
 * Continue processing the command, one character at a time.
 ******************************************************************************/
   	switch (c)
      	{
      		case 'a':   /* auto rotation */
	 		c = *(command++);
	 		switch (c) {
	 			case 'x':
	 				autox = !autox;
					break;
	 			case 'y':
	 				autoy = !autoy;
					break;
	 			case 'z':
	 				autoz = !autoz;
					break;
				default:
					autox = autoy = autoz = !autox;
	 		}
	 		autorotate = autox | autoy | autoz;
         		break;

      		case 'A':   /* auto cycle */
         		autocycle = !autocycle;
         		break;
   
      		case 'B':   /* auto everything (cycle & rotate) */
         		autocycle = !autocycle;
			autox = autoy = autoz = !autox;
	 		autorotate = autox;
         		break;

      		case 'b': /* background color in R,G,B */
	 		sscanf(command,"%f,%f,%f",
			    &tmpbg[0],&tmpbg[1],&tmpbg[2]);
			bg[0] = tmpbg[0]; bg[1] = tmpbg[1]; bg[2] = tmpbg[2];
	 		DvSetBackgroundColor(view,DcRGB,bg);
	 		update_modified = DcFalse;
         		break;

      		case 'C':   /* select color table */
			sscanf(command, "%d", &ct);
			ctab_select(ct);
			break;

      		case 'c':   /* backface culling: on/off */
         		backfacecullswitch = !backfacecullswitch;
   			DgReplaceObjInGroup(backfaceInline,
				DoBackfaceCullSwitch(backfacecullswitch) );
  			break;

      		case 'd':   /* debug mode or drawing type */
			if(strcmp(command,"ebug") == 0) {   /* debug on/off */
				debug = !debug;
				if (debug) printf("debug turned on\n");
				if (!debug) printf("debug turned off\n");
			}

			else {   /* drawing type: points, lines, shaded */
	         		reptype++;
	       	  		if (reptype > 2) reptype = 0;
				DgReplaceObjInGroup(reptypeInline,
					DoRepType(reptype));
			}
         		break;

      		case 'h':   /* help or specular highlights */
			if(strcmp(command,"elp") == 0) {   /* print help */
				prnthelp();
			}

			else {   /* specular highlights: on/off */
	         		highlightswitch = !highlightswitch;
   				DgReplaceObjInGroup(specularInline,
					DoSpecularSwitch(highlightswitch));
			}
			break;

      		case 'i':   /* decrease intensity of lights */
         		intens -= .05;
         		if (intens < 0.0) intens = 0.;
         		update_modified = DcFalse;
      			break;

      		case 'I':   /* increase intensity of lights */
			intens += .05;
			if (intens < 1.0) intens = 1.;
			update_modified = DcFalse;
			break;

		case 'm':   /* model group selection */
			model_num++;
			if (model_num > (nmodels-1)) model_num = 0;
			break;

		case 'p':   /* print current rotations & translations */
			printf("\nx,y,z rotation = %g, %g, %g\n",
				RTOD(xrotatevalue), 
				RTOD(yrotatevalue), 
				RTOD(zrotatevalue));
			printf("x,y,z translation = %g, %g, %f\n\n",
				xtranslatevalue, ytranslatevalue, 
				ztranslatevalue);
			break;

		case 'r':   /* set rotation values */
			sscanf(command,"%f,%f,%f",&xrot,&yrot,&zrot);
			xrotatevalue = (DTOR(xrot));
			yrotatevalue = (DTOR(yrot));
			zrotatevalue = (DTOR(zrot));
	 		if(debug) printf("absolute x,y,z = %g, %g, %g\n",
				xrotatevalue,yrotatevalue,
				zrotatevalue);
			break;

		case 'R':  /*  Raytrace.  */
			width = 100; height = 100;
			sscanf(command, "%d %d", &width, &height);
			raytrace(width, height);
			DdUpdate(device);
			break;

		case 's':   /* shading type: flat/smooth */
			shadingtype++;
			if (shadingtype > 1) shadingtype = 0;
			DgReplaceObjInGroup(interptypeInline,
				DoInterpType(shadingtype));
			break;

		case 'S':   /* Set Subdivision amount. */
			sscanf(command,"%f",&value);
			DgReplaceObjInGroup(subdivInline,
				DoSubDivSpec(DcSubDivRelative,&value));
			printf("Setting subdivision level to %g\n",
				value);
			break;

		case 't':   /* set translation values */
			sscanf(command,"%f,%f,%f",&xtran,&ytran,&ztran);
			do_transx(xtran);
			do_transy(ytran);
			do_transz(ztran);
			if(debug) printf("absolute x,y,z = %f, %f, %f\n",
				xtranslatevalue, ytranslatevalue,
				ztranslatevalue);
			break;

		case 'T':   /* execute a timing test */
			time0 = times(&tbuf0);
			for (repeat=0;repeat<100;repeat++) {
				xrotatevalue += 0.1;
				DdUpdate(device);
			}
			time1 = times(&tbuf1);
			elapsetime = time1 - time0;
			usertime = tbuf1.tms_utime - tbuf0.tms_utime;
			systemtime = tbuf1.tms_stime - tbuf0.tms_stime;
			cputime = usertime + systemtime;
			printf("time: %d %d %d %d\n",elapsetime, cputime,
				usertime,systemtime);
			break;

		case 'z':   /* zoom out */
			zoom += 1.;
			update_modified = DcFalse;
			break;

		case 'Z':   /* zoom in */
			zoom -= 1.;
			update_modified = DcFalse;
			break;

   	}  /* end of switch(c) */

}  /* End of default_parse function */



/*******************************************************************************
 **************************  r e a d _ i n p u t  ******************************
 *******************************************************************************
 * Module Name: read_input
 * Module Type: c
 *     Purpose: Read incoming \n terminated command from an input device, 
 *		calling parse_input() with the result.
 *   Arguments: fd ==> file descriptor of the input device to read from.
 * ReturnValue: None
 ******************************************************************************/
read_input(fd)
int fd;
{
/*******************************************************************************
 * Declare variables.
 ******************************************************************************/
	int n,i;
	static char buf[257];
	char *cp;

/*******************************************************************************
 * Try to get up to 256 characters from the input device.
 ******************************************************************************/
	if ((n=read(fd,buf,256))<1) {
		fprintf(stderr,"render.c/read_input: ");
		fprintf(stderr,"Lost connection on input stream (EOF)\n");
		exit(0);
	}

	cp = buf;

/*******************************************************************************
 * Look for a '\n' to terminate the input text.  If a valid input line was
 *  read, pass it on to parse_input().
 ******************************************************************************/
	while (n>0) {
		for (i=0; i<n; i++)
			if (cp[i] == '\n')
				break;

		if (i==n) {
			if (i==256) {
				fprintf(stderr,"render.c/read_input: ");
				fprintf(stderr,"Input buffer overflow.\n");
				exit(1);
			}
			if (debug) {
				printf("render.c/read_input: ");
				printf("Partial (non <cr> terminated) input!\n"
					);
				cp[i] = '\0';
				printf("        message was: '%s'\n",cp);
				fflush(stdout);
			}
			for (i=0; i<n; i++)
				buf[i] = cp[i];
			if ((i=read(fd,buf+n,256-n))<1) {
				fprintf(stderr,"render.c/read_input: ");
				fprintf(stderr,"Lost connection on phase II ");
				fprintf(stderr,"input stream (EOF)\n");
				exit(0);
			}
			n+=i;
			cp = buf;
			continue;
		}

		cp[i++]='\0';
		parse_input(cp);

		n-=i;
		cp+=i;

	}  /* end of while(n>0) */

}  /* End of read_input function */



/*******************************************************************************
 ***********************  r o t x _ c a l l b a c k  ***************************
 *******************************************************************************
 * Module Name: rotx_callback
 * Module Type: c
 *     Purpose: Sets the x rotation for object in 'modelgroup'
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
rotx_callback()
{
	static DtReal xrotatevalue0 = 0.;
	static DtObject rotx_handle = DcNullObject; 
 
	if (rotx_handle == DcNullObject) {
		rotx_handle = DoRotate(0,0.);
		DsHoldObj(rotx_handle);
	}

	if (xrotatevalue == xrotatevalue0) {
		DsExecuteObj(rotx_handle);
	} 
	else {
		DsReleaseObj(rotx_handle);
		rotx_handle = DoRotate(0,xrotatevalue);
		DsHoldObj(rotx_handle);
		DsExecuteObj(rotx_handle);
		xrotatevalue0 = xrotatevalue;
		if (debug) printf("xrotatevalue = %g\n",xrotatevalue);
	}

}  /* End of rotx_callback function */



/*******************************************************************************
 ***********************  r o t y _ c a l l b a c k  ***************************
 *******************************************************************************
 * Module Name: roty_callback
 * Module Type: c
 *     Purpose: Sets the y rotation for object in 'modelgroup'
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
roty_callback()
{
	static DtReal yrotatevalue0 = 0.;
	static DtObject roty_handle = DcNullObject;  

	if (roty_handle == DcNullObject) {
		roty_handle = DoRotate(1,0.);
		DsHoldObj(roty_handle);
	}

	if (yrotatevalue == yrotatevalue0) {
		DsExecuteObj(roty_handle);
	} 
	else {
		DsReleaseObj(roty_handle);
		roty_handle = DoRotate(1,yrotatevalue);
		DsHoldObj(roty_handle);
		DsExecuteObj(roty_handle);
		yrotatevalue0 = yrotatevalue;
		if (debug) printf("yrotatevalue = %g\n",yrotatevalue);
	}

}  /* End of roty_callback function */



/*******************************************************************************
 ***********************  r o t z _ c a l l b a c k  ***************************
 *******************************************************************************
 * Module Name: rotz_callback
 * Module Type: c
 *     Purpose: Sets the z rotation for object in 'modelgroup'
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
rotz_callback()
{
	static DtReal zrotatevalue0 = 0.;
	static DtObject rotz_handle = DcNullObject;

	if (rotz_handle == DcNullObject) {
		rotz_handle = DoRotate(2,0.);
		DsHoldObj(rotz_handle);
	}

	if (zrotatevalue == zrotatevalue0) {
		DsExecuteObj(rotz_handle);
	} 
	else {
		DsReleaseObj(rotz_handle);
		rotz_handle = DoRotate(2,zrotatevalue);
		DsHoldObj(rotz_handle);
		DsExecuteObj(rotz_handle);
		zrotatevalue0 = zrotatevalue;
		if (debug) printf("zrotatevalue = %g\n",zrotatevalue);
	}

}  /* End of rotz_callback function */



/*******************************************************************************
 *****************  l i t e i n t e n s _ c a l l b a c k  *********************
 *******************************************************************************
 * Module Name: liteintens_callback
 * Module Type: c
 *     Purpose: Create the 'liteintens' callback function to change intensity
 *		of the lights in the scene.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
liteintens_callback()
{
   	if(debug) printf("light intensity = %g\n",intens);
   	DsExecuteObj( DoLightIntens(intens) );

}  /* End of liteintens_callback function



/*******************************************************************************
 ***********************  z o o m _ c a l l b a c k  ***************************
 *******************************************************************************
 * Module Name: zoom_callback
 * Module Type: c
 *     Purpose: Create the 'zoom' callback function to change the field-of-view
 *		angle to effect zooming.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
zoom_callback()
{
   	extern DtReal front_clip, back_clip;

   	if(debug) printf("zoom=%g, frontclip=%g, backclip=%g\n",
      		zoom, front_clip, back_clip);
   	DsExecuteObj( DoPerspective(zoom,front_clip,back_clip) );

}  /* End of zoom_callback function



/*******************************************************************************
 **********************  m o d e l _ c a l l b a c k  **************************
 *******************************************************************************
 * Module Name: model_callback
 * Module Type: c
 *     Purpose: Create the 'model' callback function to execute the appropriate
 *		Dore' model object.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
model_callback()
{
   	extern DtObject models[];

   	if(debug) printf("before executing models[%d] (%d)\n",
      		model_num,models[model_num]);
   	DsExecuteObj(models[model_num]);
 
}  /* End of model_callback function



/*******************************************************************************
 ****************************  r a y t r a c e *********************************
 *******************************************************************************
 * Module Name: raytrace
 * Module Type: c
 *     Purpose: Generate a ray-traced view of the current scene to a file.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
raytrace(width, height)
int width, height;
{
	DtObject ray_device, ray_frame, ray_view;
	extern DtObject studio_group, model_group;
	char arg[80];
	long start_time;

	printf("Raytrace res: %dx%d\n", width, height);
	start_time = time(0);
/*******************************************************************************
 * Open the specified Dore' output device, determine its horizontal and
 *  vertical limits, and set the viewport based on that.
 ******************************************************************************/
       sprintf(arg, "-filename raytraced.img -width %d -height %d",
		width, height);

	if (!(ray_device = DoDevice("rasterfile", arg))) {
		printf("can't create device: device type = '%s',");
		printf("device file = '%s'\n","rasterfile","raytraced.img");
		exit(1);
	}
	DdSetViewport(ray_device,&bound);

/*******************************************************************************
 * Create a frame to display the view(s) in and map it to the output device.
 ******************************************************************************/
	if (!(ray_frame = DoFrame())) {
		printf("can't create frame\n");
		exit(1);
	}
	DdSetFrame(ray_device,ray_frame);
	DfSetBoundary(ray_frame,&bound);

/*******************************************************************************
 * Create a ray_view to render the objects in.
 ******************************************************************************/
	if (!(ray_view = DoView())) {
		printf("can't create view\n");
		exit(1);
	}
	DvSetClearFlag(ray_view,DcTrue);
	DvSetBackgroundColor(ray_view,DcRGB,backcolor);
	DvSetRendStyle(ray_view,DcProductionTime);

	DgAddObjToGroup(DfInqViewGroup(ray_frame),ray_view);
	DvSetBoundary(ray_view,&bound);

/*******************************************************************************
 * Add the studio & display objects to the current ray_view
 & display it.
 ******************************************************************************/
	DgAddObjToGroup(DvInqDisplayGroup(ray_view),model_group);

	DgAddObjToGroup(DvInqDefinitionGroup(ray_view),studio_group);

	printf("Computing raytraced image.\n");

	DdUpdate(ray_device);

	DsReleaseObj(ray_device);

	printf("Raytrace complete in just %d seconds!!\n", time(0)-start_time);

}  /* End of raytrace function */


/*******************************************************************************
 ***************************  d o _ t f _ r o t x  *****************************
 *******************************************************************************
 * Module Name: do_tf_rotx
 * Module Type: c
 *     Purpose: Set the x global rotation value for the model object
 *   Arguments: val ==> rotation amount in radius
 * ReturnValue: None
 ******************************************************************************/
do_tf_rotx(val)
float val;
{
	val -= tf_rotx_val;

	DgReplaceObjInGroup(tf_rot_inline,
		ChangeTransformer(tf_rot,DcRotX,val,DcPostConcatenate));

	tf_rotx_val += val;

}  /* End of do_tf_rotx function */


/*******************************************************************************
 ***************************  d o _ t f _ r o t y  *****************************
 *******************************************************************************
 * Module Name: do_tf_roty
 * Module Type: c
 *     Purpose: Set the y global rotation value for the model object
 *   Arguments: val ==> rotation amount in radians
 * ReturnValue: None
 ******************************************************************************/
do_tf_roty(val)
float val;
{
	val -= tf_roty_val;

	DgReplaceObjInGroup(tf_rot_inline,
		ChangeTransformer(tf_rot,DcRotY,val,DcPostConcatenate));

	tf_roty_val += val;

}  /* End of do_tf_roty function */



/*******************************************************************************
 ***************************  d o _ t f _ r o t z  *****************************
 *******************************************************************************
 * Module Name: do_tf_rotz
 * Module Type: c
 *     Purpose: Set the x global rotation value for the model object
 *   Arguments: val ==> rotation amount in radians
 * ReturnValue: None
 ******************************************************************************/
do_tf_rotz(val)
float val;
{
	val -= tf_rotz_val;

	DgReplaceObjInGroup(tf_rot_inline,
		ChangeTransformer(tf_rot,DcRotZ,val,DcPostConcatenate));

	tf_rotz_val += val;

}  /* End of do_tf_rotz function */



/*******************************************************************************
 ****************************  d o _ t r a n s x  ******************************
 *******************************************************************************
 * Module Name: do_transx
 * Module Type: c
 *     Purpose: Set the x translation value for the model object
 *   Arguments: val ==> translation amount
 * ReturnValue: None
 ******************************************************************************/
do_transx(val)
float val;
{
	val *= model_size;
	val -= xtranslatevalue;

	DgReplaceObjInGroup(tf_tran_inline,
		ChangeTransformer(tf_tran,DcTransX,val,DcPostConcatenate));

	xtranslatevalue += val;

}  /* End of do_transx function */



/*******************************************************************************
 ****************************  d o _ t r a n s y  ******************************
 *******************************************************************************
 * Module Name: do_transy
 * Module Type: c
 *     Purpose: Set the y translation value for the model object
 *   Arguments: val ==> translation amount
 * ReturnValue: None
 ******************************************************************************/
do_transy(val)
float val;
{
	val *= model_size;
	val -= ytranslatevalue;

	DgReplaceObjInGroup(tf_tran_inline,
		ChangeTransformer(tf_tran,DcTransY,val,DcPostConcatenate));

	ytranslatevalue += val;

}  /* End of do_transy function */



/*******************************************************************************
 ****************************  d o _ t r a n s z  ******************************
 *******************************************************************************
 * Module Name: do_transz
 * Module Type: c
 *     Purpose: Set the z translation value for the model object
 *   Arguments: val ==> translation amount
 * ReturnValue: None
 ******************************************************************************/
do_transz(val)
float val;
{
	val *= model_size;
	val -= ztranslatevalue;

	DgReplaceObjInGroup(tf_tran_inline,
		ChangeTransformer(tf_tran,DcTransZ,val,DcPostConcatenate));

	ztranslatevalue += val;

}  /* End of do_transz function */



/*******************************************************************************
 *****************************  p r n t h e l p  *******************************
 *******************************************************************************
 * Module Name: prnthelp
 * Module Type: c
 *     Purpose: This routine list the default keyboard commands to the xterm
 *		window that this program is being run from.
 *   Arguments: None
 * ReturnValue: None
 ******************************************************************************/
prnthelp()
{
	printf("\n\n\n");
	printf("Default keyboard commands are:\n");
	printf("  a - turns auto x, y, & z rotation on/off\n");
	printf("  ax - turns auto x rotation on/off\n");
	printf("  ay - turns auto y rotation on/off\n");
	printf("  az - turns auto z rotation on/off\n");
	printf("  A - turns autocycling through models (animation) on/off\n");
	printf("  c - turns on/off all polygons with back facing normals\n");
	printf("  d - switched drawing type between surface, points,& lines\n");
	printf("  debug - turns debug printout on/off\n");
	printf("  h - turns specular highlights on/off\n");
	printf("  help - prints this message\n");
	printf("  i - decreases the intensity of the lights \n");
	printf("  I - increases the intensity of the lights \n");
	printf("  m - switches between the available models \n");
	printf("  q - exit this program (same as x)\n");
	printf("  s - switch between smooth and flat shading \n");
	printf("  x - exit this program (same as q)\n");
	printf("  z - zoom in \n");
	printf("  Z - zoom out \n");
	printf("\n");

}  /* End of prnthelp function */


/* color table things */
DtReal gray_scale_map[256*3];
DtReal color_scale_map[256*3];
DtReal shade_rng_map[256*3];
DtInt shade_ranges[2*8];
DtInt shade_range1[2] = {0, 255};

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

ctab_setup()
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

ctab_select(i)
	int i;
{
	switch(i) {
	    case 0:
		if (debug)
			printf ("\nShadeMode = DcComponent\n");
		DdSetShadeMode(device, DcComponent);
		DdSetColorEntries(device, DcRGB, 0, 255, color_scale_map);
		break;
	    case 1:
		if (debug)
			printf ("\nShadeMode = DcRange (1 range)\n");
		DdSetShadeMode(device, DcRange);
		DdSetShadeRanges(device, 0, 1, shade_range1);
		DdSetColorEntries(device, DcRGB, 0, 255, gray_scale_map);
		break;
	    case 2:
		if (debug)
			printf ("\nShadeMode = DcRange (8 ranges)\n");
		DdSetShadeMode(device, DcRange);
		DdSetShadeRanges(device, 0, 8, shade_ranges);
		DdSetColorEntries(device, DcRGB, 0, 255, shade_rng_map);
		break;
	}

	DdUpdate(device);
}
