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
 
#include "dore/internal/dogen.h"
#include "dore/dore_develop/develop.h"

typedef struct device_driver {
    char *		device_name;
    char *		description;
    DtPFI		get_interface_functions;
    DDt_DCM *	functions;
} dot_device_driver;


typedef struct device {
    dot_object *	next;	
    dot_object *	frame;	
    DtVolume 		extent;
    DtVolume 		viewport;	
    DtFlag 		auto_size;	/* auto size viewport when extent changes */
    DtFlag 		all_clipped;	/* true if clip volume is empty */
    DtVolume 		clipping_volume;/* volume in frame coordinates to
					   which projected objects are clipped */
    dot_object_reference *references; 	/* objects using this device */
    DtVolume 		actual_viewport;/* actual area used for display */
    DtShort 		xmax;		/* device extent */
    DtShort 		xmin;
    DtShort 		ymax;
    DtShort 		ymin;
    DtReal 		ftodscale[3];	/* scaling values for mapping from
					   frame coords to device coords */
    DtReal 		ftodtranslate[3];/* translation values for mapping from 
					    frame coords to device coords */
    DtSize3 		pick_aperture; 	/* current pick variables */
    dot_object *	pick_callback_object;
    DtPickPathOrder	pick_path_order;
    DtVisualType	visual_type;	/* color output capabilities */
    DtShadeMode		shade_mode;

    DtReal 		foreground_color[3]; /* color variables */
    DtReal 		background_color[3];
    DtInt 		shade_index;	/* shade index for converting
					   color to range index */
    DtFlag 		depth_buffer_enable; /* true if depth buffering enabled */
    DtFlag 		depth_buffer_write;
    DtReal 		ctodscale[3]; 	/* mapping from cannonical view */
    DtReal 		ctodtranslate[3]; /* volume to Dore device coordinates */
    DtInt 		ncolors;	/* software color map */
    DtReal *		color_map;

    DtReal 		resolution[2];	/* the size of a pixel on the screen */

    DtVolume 		device_pick_volume; 
    DtInt *		shade_ranges; 	/* shade range table (ranges of 
					   the color table) */
    DtInt 		shade_range_table_size;
    DtInt 		modification_sequence_number; /*  Sequence number for 
							  the last modification made to the 
							  device data. */
    DtFlag 		aborted_draw_flag; /* flag to indicate if drawing 
					      was aborted */
    dot_device_driver *	driver;
    DtPtr 		device_data;	/* device drivers private data */
} dot_device;

/*
  ======================================================================
 */
extern DtPoint3			 doe_device_pick_point;
extern dot_object		*doe_device_list;
extern dot_object		*doe_device_current_device_object;
extern dot_device		*doe_device_current_device;
extern dot_device_driver	*doe_device_current_driver;
extern DDt_DCM		*doe_device_current_functions;
extern dot_device_driver	*doe_device_drivers;
extern DtInt			 doe_device_max_drivers;
extern DtInt			 doe_device_driver_count;
extern DtInt			 doe_device_class_id;

/*
  ======================================================================
 */

#define dod_device_null (dot_device *)0

#define FUNCTION(name) static char *proc_name = name ; \
   if ( doe_device_current_device == (dot_device *)0 ) {\
      DDerror (ERR_INVALID_DEVICE, \
		  proc_name, "current device NULL") ; \
   }

#define OUTFUN(function) (doe_device_current_functions->function)

#define SET_CURRENT_DEVICE(device_object) \
    if (doe_device_current_device_object != (device_object)) {		\
	dot_device *device = (dot_device *)((device_object)->data);	\
	doe_device_current_device_object = (device_object);		\
	doe_device_current_device = device;				\
	doe_device_current_driver = device->driver;			\
	doe_device_current_functions = device->driver->functions;	\
	(*doe_device_current_functions->become_current_driver)		\
		(device->device_data);					\
    }

/*
  ======================================================================
 */

