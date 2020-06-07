/* vi:set ts=8 sw=4 ai:
 *[
 *$     MLRD_render.c
 *
 *      Steve Gray - Mon Jul 17 16:51:44 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	rendering main loop and related
 *
 *      Usage:
 *		MLRD_render()		do a render
 *		MLRD_render_delete()	delete ourselvers
 *		MLRD_render_new_class()	???
 *		
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */



/* #define	DEBUG */

/* system includes */
#include <stdio.h>
#include <math.h>

/* dore includes */
#include "dore.h"

/* Render includes */
#include "primitives.h"

/* MLRD Dore interface includes */
#include "global.h"
#include "method.h"

#ifndef RENDER_PIPE
#if defined(TITAN) || defined(TITAN_SP)
#include <sys/types.h>
#endif
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#endif

/*** Magic Number ***/
# define	ART_MAGIC_NUMBER	(0xdeb88bed)

/*** primary header ***/
typedef struct ARThdr1_s {
	int	magic_number;
	int	file_type;
	int	user_space;
} ART_HEADER_1;


/** define number of types supported **/
# define ART_NUM_TYPES		(3)


/*** secondary header  type 1 ***/
typedef struct ARThdr21_s {
	int	width;
	int	height;
} ART_HEADER_2_1;


/*** secondary header  type 2 ***/
typedef struct ARThdr22_s {
	int	width;
	int	height;
} ART_HEADER_2_2;


/*** secondary header  type 3 ***/
typedef struct ARThdr23_s {
	int	width;
	int	height;
} ART_HEADER_2_3;


/*** generic image structure ***/
typedef	struct image_s	{
	int		type;
	int		width;
	int		height;
	unsigned char	*image;
	unsigned char	*red;
	unsigned char	*green;
	unsigned char	*blue;
	unsigned char	*matte;
	unsigned char	*red_colormap;
	unsigned char	*green_colormap;
	unsigned char	*blue_colormap;
} GENERIC_IMAGE;


#ifdef titan
/* constants to put threads to sleep while runing the separate
   rendering process.  This prevents contention for resources 
   with spinning threads.
*/
int mlrd_timeout_iterations = 10;
int mlrd_timeout_sleep = 500;
#endif



/*[
 *$     MLRD_render_direct
 *
 *      Steve Gray - Mon Jul 17 16:53:10 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    render a database directly to the Dore window
 *
 *      Usage:
 *		MLRD_render_direct( view, device, 
 *				    view_device_changed, studio_changed)
 *		DtObjectStructure	*view
 *		DtObjectStructure	*device
 *		DtFlag			view_device_changed
 *		DtFlag			studio_changed
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_render_direct( device, view, view_changed, studio_changed)
DtObjectStructure	*device;
DtObjectStructure	*view;
DtFlag			view_changed;
DtFlag			studio_changed;
{
#ifdef	DEBUG
printf( "MLRD_render_direct\n");
#endif


/* set the mode to direct */
    mlr_mode = MLR_MODE_DIRECT;

/* call the common render control function */
    mlrd_render( view, device, view_changed, studio_changed);
}


/*[
 *$     MLRD_render_file
 *
 *      Steve Gray - Mon Jul 17 16:53:10 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    write render control files
 *
 *      Usage:
 *		MLRD_render_file( view, device, 
 *				  view_device_changed, studio_changed)
 *		DtObjectStructure	*view
 *		DtObjectStructure	*device
 *		DtFlag			view_device_changed
 *		DtFlag			studio_changed
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_render_file( device, view, view_changed, studio_changed)
DtObjectStructure	*device;
DtObjectStructure	*view;
DtFlag			view_changed;
DtFlag			studio_changed;
{
#ifdef	DEBUG
printf( "MLRD_render_file\n");
#endif


/* set the mode to direct */
    mlr_mode = MLR_MODE_FILE;

/* call the common render control function */
    mlrd_render( view, device, view_changed, studio_changed);
}


/*[
 *$     mlrd_render
 *
 *      Steve Gray - Mon Jul 17 16:53:10 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    render a database 
 *
 *      Usage:
 *		mlrd_render( view, device, view_device_changed, studio_changed)
 *		DtObjectStructure	*view
 *		DtObjectStructure	*device
 *		DtFlag			view_device_changed
 *		DtFlag			studio_changed
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

mlrd_render( view, device, view_changed, studio_changed)
DtObjectStructure	*view;
DtObjectStructure	*device;
DtFlag			view_changed;
DtFlag			studio_changed;
{
#ifdef	DEBUG
printf( "mlrd_render (id = %d)\n", mlr_control.render_id);
printf( "traverse studio\n");
#endif

    if (mlr_control.device != device) {
	DDdevice_InqInterfaceRoutines (device, DDc_PROM,
				       (DtPtr *)&mlr_control.PROM_fcns);
    }

    if ((DtPtr) mlr_control.PROM_fcns == DcNullPtr) {
	DDerror (ERR_NO_OUTPUT_MODULE,
		    "mlrd_render", "Production");
	return;
    }

    mlr_control.device = device;
    mlr_control.view   = view;

/* init the conversion */
    MLRD_init();

/* if we are doing a conversion file render */
    if ( mlr_mode == MLR_MODE_FILE )
    {
/* traverse the studio */
        DDview_TraverseStudio(mlr_control.view, mlr_control.studio_traversal);

/* traverse the display */
#ifdef	DEBUG
printf( "traverse display\n");
#endif

        DDview_TraverseDisplay(mlr_control.view, mlr_control.display_traversal);

/* write the files and run the renderer */
#ifdef	DEBUG
printf( "run render\n");
#endif

        mlr_render_run();
    }
/* if we are doing a direct render */
    else
    {
#ifdef	DEBUG
printf( "++++++++++++++ traverse, mode: DIRECT_COUNT +++++++++++++++++++\n");
#endif

/* count all the data types */
	mlr_mode = MLR_MODE_DIRECT_COUNT;

/* traverse the database */
        DDview_TraverseStudio(mlr_control.view,  mlr_control.studio_traversal);
        DDview_TraverseDisplay(mlr_control.view,  mlr_control.display_traversal);

/* set the limits */
	MLRD_direct_limits();

#ifdef	DEBUG
printf( "++++++++++++++ traverse, mode: DIRECT +++++++++++++++++++\n");
#endif

/* reset and traverse for real */
	mlr_mode = MLR_MODE_DIRECT;
	MLRD_init();

        DDview_TraverseStudio(mlr_control.view, mlr_control.studio_traversal);
        DDview_TraverseDisplay(mlr_control.view,  mlr_control.display_traversal);

/* execute the render */
	MLRD_direct_exec();
    }
}


/*[
 *$     MLRD_render_delete
 *
 *      Steve Gray - Mon Jul 17 16:58:40 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    delete self
 *
 *      Usage:
 *		MLRD_render_delete()
 *
 *      Notes:  
 *		NULL
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_render_delete()
{
}


/*[
 *$     MLRD_render_new_class
 *
 *      Steve Gray - Mon Jul 17 16:59:15 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    install new classes as they are references
 *
 *      Usage:
 *		MLRD_render_new_class( class_name, class_id)
 *		DtPtr	class_name	(in)  name of new class
 *		DtInt	class_id	(in)  id of class
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_render_new_class( class_name, class_id)
DtPtr	class_name;
DtInt	class_id;
{
Int	i;


#ifdef	DEBUG
printf( "new class %s (%d)\n", class_name, class_id);
#endif


    for ( i = 0; studio_method[i].name != NULL; i++ )
    {
	if ( strcmp( class_name, studio_method[i].name) == 0 )
	{
#ifdef	DEBUG
printf( "set studio method %d\n", i);
#endif

	    DDclass_SetMethod( class_id,
				  mlr_control.studio_traversal,
				  studio_method[i].routine);
	    return;
	}
    }

    for ( i = 0; display_method[i].name != NULL; i++ )
    {
	if ( strcmp( class_name, display_method[i].name) == 0 )
	{
#ifdef	DEBUG
printf( "set display method %d\n", i);
#endif

	    DDclass_SetMethod( class_id,
				  mlr_control.display_traversal,
				  display_method[i].routine);
	    return;
	}
    }
}


/*[
 *$     mlr_render_run
 *
 *      Steve Gray - Mon Jul 17 17:04:36 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    write files and run render
 *
 *      Usage:
 *		mlr_render_run()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

mlr_render_run()
{
Char	sys_call[256];
Char	render_name[256];
Char	*render_path, *getenv();
Int	lit_id, obj_id;
Int	vox_lim;
Float	fact;


/* kludge object limits */
    mlr_limits.obj[1] = mlr_limits.obj[0] * mlr_limits.li * 2;

/* set global ambient level */
    fprintf( mlr_files.camera_fp, "glamb %f %f %f\n", mlr_global_ambient.r,
	     mlr_global_ambient.g, mlr_global_ambient.b);

/* write limits to limits file */
    if ( mlr_limits.cp != 0 )
        fprintf( mlr_files.limits_fp, "limit cp %d\n", mlr_limits.cp);

    if ( mlr_limits.tex[0] != 0 )
    {
	if ( mlr_limits.tex[1] == 0 )
	    mlr_limits.tex[1] = 1;

        fprintf( mlr_files.limits_fp, "limit tex %d %d\n", mlr_limits.tex[0],
						           mlr_limits.tex[1]);
    }

    if ( mlr_limits.tri[0] != 0 )
    {
	if ( mlr_limits.tri[1] == 0 )
	    mlr_limits.tri[1] = 1;

        fprintf( mlr_files.limits_fp, "limit tri %d %d\n", mlr_limits.tri[0],
						           mlr_limits.tri[1]);
    }

    if ( mlr_limits.line[0] != 0 )
    {
	if ( mlr_limits.line[1] == 0 )
	    mlr_limits.line[1] = 1;

        fprintf( mlr_files.limits_fp, "limit line %d %d\n", mlr_limits.line[0],
						            mlr_limits.line[1]);
    }

    if ( mlr_limits.ma != 0 )
        fprintf( mlr_files.limits_fp, "limit ma %d\n", mlr_limits.ma);

    if ( mlr_limits.li != 0 )
        fprintf( mlr_files.limits_fp, "limit li %d\n", mlr_limits.li);

    if ( mlr_limits.obj[0] != 0 )
    {
	if ( mlr_limits.obj[1] == 0 )
	    mlr_limits.obj[1] = 1;

        fprintf( mlr_files.limits_fp, "limit obj %d %d\n", mlr_limits.obj[0],
						           mlr_limits.obj[1]);
    }
    
/* TOTALLY ARBITRARY HURISTIC for determining the size of the voxel database */
    if ( mlr_limits.tri[0] < 100000 )
    {
	fact = (((Float) mlr_limits.tri[0]) / 100000.0) * (3.141592 / 2.0);
	fact = sin( (double) fact);

	fact = pow( (double) fact, (double) 0.3);

	vox_lim = fact * 14.0;
    
        if ( vox_lim < 2 )
        {
	    vox_lim = 2;
        }
        else if ( vox_lim > 14 )
        {
	    vox_lim = 14;
        }
    }
    else
    {
	vox_lim = 14;
    }

    fprintf( mlr_files.limits_fp, "limit vox %d %d\n", vox_lim, vox_lim);

/* write the render control file */
    fprintf( mlr_files.render_fp, "call %s\n", mlr_files.limits_file);
    fprintf( mlr_files.render_fp, "call %s\n", mlr_files.camera_file);
    fprintf( mlr_files.render_fp, "call %s\n", mlr_files.light_file);
    fprintf( mlr_files.render_fp, "call %s\n", mlr_files.object_file);
    fprintf( mlr_files.render_fp, "\n");

/* print global light model info */
    MATD_print_gllmod();

    fprintf( mlr_files.render_fp, "\n");

    fprintf( mlr_files.render_fp, "\n");
    fprintf( mlr_files.render_fp, "render\n");
    fprintf( mlr_files.render_fp, "\n");

/* close all the files */
    fclose( mlr_files.render_fp);
    fclose( mlr_files.camera_fp);
    fclose( mlr_files.light_fp);
    fclose( mlr_files.limits_fp);
    fclose( mlr_files.object_fp);

/* if we are auto rendering, run the render */
    if ( mlr_auto_render )
    {
#ifdef titan
       int save_timeout_iterations;
       int save_timeout_sleep;

       save_timeout_iterations = _get_timeout_iterations();
       save_timeout_sleep = _get_timeout_secs();

       _set_timeout(mlrd_timeout_iterations, mlrd_timeout_sleep);
#endif

#ifdef RENDER_PIPE
        render_pipe();
#else
	render_inetd();
#endif

#ifdef titan
       _set_timeout(save_timeout_iterations, save_timeout_sleep);
#endif

    }
}


#ifdef RENDER_PIPE
/*[
 *$     render_pipe
 *
 *      Steve Gray - Tue Dec  5 12:33:39 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    run the render and pipe back the image
 *
 *      Usage:
 *		render_pipe()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

render_pipe()
{
FILE	*image;
Char	*render_path, *render_options, *getenv();
Char	render_prefix[256], render_name[256];
Char	sys_call[512];
Int	processors;
extern	pclose();

/* render executable names */
#if	defined(TITAN) | defined(TITAN_SP)
static Char	*render_exec = "Synap.titan";
#endif

#ifdef	GS1000
static Char	*render_exec = "Synap.gs1000";
#endif

#ifdef	IRIS4D
static Char	*render_exec = "Synap.iris4d";
#endif

#ifdef	IRIS4DMP
static Char	*render_exec = "Synap.iris4dmp";
#endif

#ifdef	SUN4
static Char	*render_exec = "Synap.sun4";
#endif

#ifdef	SUN3
static Char	*render_exec = "Synap.sun3";
#endif


/* figure out the name of the renderer */
    if ( (render_path = getenv( "RENDER_PATH")) == NULL )
    {
	strcpy( render_prefix, "/usr/dore/bin");
    }
    else
    {
	strcpy( render_prefix, render_path);
    }

    strcpy( render_name, render_prefix);
    strcat( render_name, "/");
    strcat( render_name, render_exec);

/* get the number of threads to use */
    processors = DDsystem_InqNumberProcessors();

/* do a popen to run the render */
    if ( (render_options = getenv( "RENDER_OPTIONS")) == NULL )
    {
        sprintf( sys_call, "%s -st %d %s\n", 
	         render_name, processors, mlr_files.render_file);
    }
    else
    {
        sprintf( sys_call, "%s %s %s\n", 
	         render_name, render_options, 
		 mlr_files.render_file);
    }

    if ( (image = popen( sys_call, "r")) == NULL )
    {
	fprintf( stderr, "system command: %s\n", sys_call);
	DDerror( ERR_SOCKET_FAILURE, "render_pipe", 
		    "unable to run renderer");
    }

/* read the image back and display it */
    mlrd_image_read( image, pclose);
    pclose( image);

/* if we should delete the control files, do so */
    if ( mlr_auto_render && (getenv( "RENDER_NO_DELETE") == NULL) )
    {
        unlink( mlr_files.render_file);
        unlink( mlr_files.camera_file);
        unlink( mlr_files.light_file);
        unlink( mlr_files.limits_file);
        unlink( mlr_files.object_file);
    }
}

#else /* !defined(RENDER_PIPE) */

/*[
 *$     render_inetd
 *
 *      Daniel R. McLachlan - Thu Feb 15 15:23:48 PST 1990
 *      Stardent Comuter Inc,
 *?
 *      Description:    run the renderer using inetd
 *
 *      Usage:
 *		render_inetd()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

render_inetd()
{
   char *getenv();
   int io;
   FILE *image;
   char *host;
   extern fclose();
   char buf[1024];
   char *errfile;
   char errfile1[1024];
   unsigned int count;
   FILE *err;

   host = getenv ("RENDER_HOST");

   if ((errfile = getenv("RENDER_LISTFILE")) == NULL) {
      errfile = errfile1;
      sprintf (errfile, "%s.err", mlr_files.file_prefix);
   }

   if ((io = connect_to_Synap(host)) == -1)
	 return -1;

   if (send_input_file (io, errfile) == -1)
	 return -1;

   if ((image = fdopen (io, "r")) == NULL) {
      DDerror ( ERR_IO_ERROR, "render_inetd", "fdopen failed");
      return -1;
   }

   mlrd_image_read (image, fclose);
   close(io);

   if ((err = fopen (errfile, "r")) != NULL) {
      while ((count = fread (buf, 1, 1023, err)) > 0) {
	 fwrite (buf, count, 1, stderr);
      }
      fclose(err);
      if (getenv( "RENDER_NO_DELETE") == NULL) 
	    unlink (errfile);
   }

/* if we should delete the control files, do so */
    if ( mlr_auto_render && (getenv( "RENDER_NO_DELETE") == NULL) )
    {
        unlink( mlr_files.render_file);
        unlink( mlr_files.camera_file);
        unlink( mlr_files.light_file);
        unlink( mlr_files.limits_file);
        unlink( mlr_files.object_file);
    }
}


/*[
 *$     connect_to_Synap
 *
 *      Daniel R. McLachlan - Thu Feb 15 15:23:48 PST 1990
 *      Stardent Comuter Inc,
 *?
 *      Description:    connect to the rendered through inetd
 *
 *      Usage:
 *		connect_to_Synap
 *		char	*host	(in)  host to use
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

int
connect_to_Synap(host)
     char *host;
{
   int flags;
   char server_name[100];
   struct hostent *remote_host;
   struct sockaddr_in server_address;
   struct servent *service_block;
   int client_socket;

   service_block = getservbyname("Synap","tcp");
   if (service_block == 0) {
      DDerror( ERR_UNKNOWN_SERVICE, "connect_to_Synap", 
		  "could not get service for Synap");
      return -1;
   }
   server_address.sin_family = AF_INET;
   server_address.sin_port = service_block->s_port;

   if (host == NULL) {
      gethostname(server_name, 100);
   } else {
      strcpy (server_name, host);
   }
   remote_host = gethostbyname(server_name);

   if (remote_host == 0) {
      DDerror( ERR_UNKNOWN_HOST, "connect_to_Synap", 
		  "could not get host info");
      return -1;
   }
   bcopy(remote_host->h_addr,(char *)&server_address.sin_addr,
	 remote_host->h_length);

   if ((client_socket = socket(AF_INET,SOCK_STREAM,0)) < 0) {
      DDerror( ERR_SOCKET_FAILURE, "connect_to_Synap", 
		  "could not create a socket");
      return -1;
   }
   if (connect(client_socket,&server_address,sizeof(server_address)) < 0) {
      char buffer[128];
      sprintf (buffer, "connection failed to server %s", server_name);
      DDerror( ERR_SOCKET_FAILURE, "connect_to_Synap", buffer);
      return -1;
   }

   flags = fcntl (client_socket, F_GETFL, 0);
   fcntl (client_socket, F_SETFL, flags | O_SYNC);

   return client_socket;
}

/*[
 *$     send_input_File
 *
 *      Daniel R. McLachlan - Thu Feb 15 15:23:48 PST 1990
 *      Stardent Comuter Inc,
 *?
 *      Description:    send primary control file to remote render
 *
 *      Usage:
 *		send_input_file( fd, errfile)
 *		int	fd		(in)  file descriptor to write to
 *		char	*errfile	(in)  name of render error file
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

int
send_input_file(fd, errfile)
     int fd;
     char *errfile;
{
   FILE *in;
   char buffer[1024];
   unsigned int count;
   int processors;
   char *verbose;

   sprintf (buffer, "listfile %s\n", errfile);
   write (fd, buffer, strlen(buffer));
   processors = DDsystem_InqNumberProcessors();

   if (getenv("RENDER_VERBOSE") != NULL) {
      verbose = "on";
   } else {
      verbose = "off";
   }
   sprintf (buffer, "verbose %s\ngid %d\nuid %d\nthread %d\n",
	    verbose, getegid(), geteuid(), processors);
   write (fd, buffer, strlen(buffer));


/* send limits file */
   in = fopen (mlr_files.limits_file, "r");
   if (in == NULL) {
      sprintf (buffer, "could not open file '%s'", mlr_files.limits_file);
      DDerror( ERR_IO_ERROR, "send_input_file", buffer);
      return -1;
   }

   while ((count = fread (buffer, 1, 1023, in)) > 0) {
      write (fd, buffer, count);
   }
   fclose (in);

/* send camera file */
   in = fopen (mlr_files.camera_file, "r");
   if (in == NULL) {
      sprintf (buffer, "could not open file '%s'", mlr_files.camera_file);
      DDerror( ERR_IO_ERROR, "send_input_file", buffer);
      return -1;
   }

   while ((count = fread (buffer, 1, 1023, in)) > 0) {
      write (fd, buffer, count);
   }
   fclose (in);

/* lights file */
   in = fopen (mlr_files.light_file, "r");
   if (in == NULL) {
      sprintf (buffer, "could not open file '%s'", mlr_files.light_file);
      DDerror( ERR_IO_ERROR, "send_input_file", buffer);
      return -1;
   }

   while ((count = fread (buffer, 1, 1023, in)) > 0) {
      write (fd, buffer, count);
   }
   fclose (in);

/* object file */
   in = fopen (mlr_files.object_file, "r");
   if (in == NULL) {
      sprintf (buffer, "could not open file '%s'", mlr_files.object_file);
      DDerror( ERR_IO_ERROR, "send_input_file", buffer);
      return -1;
   }

   while ((count = fread (buffer, 1, 1023, in)) > 0) {
      write (fd, buffer, count);
   }
   fclose (in);

/* send render command */
   sprintf (buffer, "render\n");
   write (fd, buffer, strlen(buffer));

/* all ok */
   return 0;
}

#endif /* defined (RENDER_PIPE) */
