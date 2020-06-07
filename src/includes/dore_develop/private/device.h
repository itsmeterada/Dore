#ifndef DD_DEVICE_H
#define DD_DEVICE_H
 
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
 
#include <dore/internal/dogen.h>
#include <dore/dore_develop/develop.h>

typedef struct device_driver {
    char    *device_name;
    char    *description;
    DtPFV    get_interface_functions;
    DDt_DCM *functions;
} dot_device_driver;

typedef struct device {
    dot_object *next;	
    dot_object *frame;	
    DtVolume    extent;
    DtVolume    viewport;	
    DtFlag      auto_size;	/* auto size viewport when extent changes */
    DtFlag      all_clipped;	/* true if clip volume is empty */
    DtVolume    clipping_volume;/* volume in frame coordinates to
				   which projected objects are clipped */
    dot_object_reference *references; 	/* objects using this device */
    DtVolume 	actual_viewport;/* actual area used for display */
    DtShort 	xmax;		/* device extent */
    DtShort 	xmin;
    DtShort 	ymax;
    DtShort 	ymin;
    DtReal 	ftodscale[3];	/* scaling values for mapping from
				   frame coords to device coords */
    DtReal 	ftodtranslate[3];/* translation values for mapping from 
				    frame coords to device coords */
    DtSize3 	     pick_aperture; 	/* current pick variables */
    dot_object      *pick_callback_object;
    DtPickPathOrder  pick_path_order;
    DtVisualType     visual_type;	/* color output capabilities */
    DtShadeMode	     shade_mode;

    DtReal 	foreground_color[3]; /* color variables */
    DtReal 	background_color[3];
    DtInt 	shade_index;	/* shade index for converting
				   color to range index */
    DtFlag 	depth_buffer_enable; /* true if depth buffering enabled */
    DtFlag 	depth_buffer_write;
    DtReal 	ctodscale[3]; 	/* mapping from cannonical view */
    DtReal 	ctodtranslate[3]; /* volume to Dore device coordinates */
    DtInt 	ncolors;	/* software color map */
    DtReal *	color_map;

    DtReal 	resolution[2];	/* the size of a pixel on the screen */

    DtVolume 	device_pick_volume; 
    DtInt *	shade_ranges;  /*shade range table (ranges of the color table)*/
    DtInt 	shade_range_table_size;
    DtInt 	modification_sequence_number; /* Sequence number for the last
						modification made to the device
						data. */
    DtFlag 	aborted_draw_flag; /* flag to indicate if drawing was aborted */
    dot_device_driver *driver;
    DtPtr 	device_data;	/* device drivers private data */
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

/* ====================================================================== */

void         dor_device_add_reference  (dot_object*, dot_object*);
void         dor_device_clear_rectangle_depth
		 (dot_object *, DtInt, DtInt, DtInt, DtInt);
void         dor_device_clear_rectangle_depth_and_color
		 (dot_object *, DtInt, DtInt, DtInt, DtInt);
void         dor_device_compute_actual_viewport  (dot_device *);
void         dor_device_compute_clip_volume  (dot_device *, dot_object *);
void         dor_device_compute_frame_trans  (dot_device *);
void         dor_device_compute_pick_volume  (dot_device *, dot_object *);
dot_object*  dor_device_create  (char*, char*);
DDt_DCM*     dor_device_create_DCM_struct  (void);
DDt_DROM*    dor_device_create_DROM_struct  (void);
DDt_PROM*    dor_device_create_PROM_struct  (void);
void         dor_device_delete  (dot_object *);
void         dor_device_flush  (dot_object *);
DtFlag       dor_device_get_aborted_draw_flag  (dot_object *);
DtVolume*    dor_device_get_device_pick_volume  (dot_object *);
void         dor_device_get_ftod_parms  (dot_object*, DtReal[3], DtReal[3]);
void         dor_device_get_scltrn_parms  (dot_object*, DtReal[3], DtReal[3]);
void         dor_device_initialize  (void);
DtVolume*    dor_device_inq_actual_viewport_ptr  (dot_object *);
DtFlag       dor_device_inq_auto_size  (dot_object *);
DtVolume*    dor_device_inq_clipping_volume_ptr  (dot_object *);
void         dor_device_inq_color_entries
		 (dot_object *, DtColorModel, DtInt, DtInt, DtReal []);
DtInt        dor_device_inq_color_table_size  (dot_object *);
void         dor_device_inq_device_extent
		 (dot_object*, DtReal*, DtReal*, DtReal*, DtReal*, DtReal*,
		  DtReal*);
void         dor_device_inq_driver_info  (DtInt, char*[], char*[]);
void         dor_device_inq_extent  (dot_object *, DtVolume *);
void         dor_device_inq_fonts  (dot_object *, DtFontPrecision []);
dot_object*  dor_device_inq_frame  (dot_object *);
DtInt        dor_device_inq_num_drivers  (void);
DtInt        dor_device_inq_num_fonts  (dot_object *);
void         dor_device_inq_pick_aperture  (dot_object *, DtSize3 *);
dot_object*  dor_device_inq_pick_callback  (dot_object *);
DtPickPathOrder dor_device_inq_pick_path_order  (dot_object *);
DtFlag       dor_device_inq_pixeldata
		 (dot_object*, DtRasterType, DtInt*, DtInt*, DtRasterType*,
		  DtPtr*, DtFlag*);
void         dor_device_inq_resolution  (dot_object *, DtReal *, DtReal *);
void         dor_device_inq_sequence_number  (dot_object *, DtInt *);
DtShadeMode  dor_device_inq_shade_mode  (dot_object *);
void         dor_device_inq_shade_range  (dot_object*, DtInt, DtInt*, DtInt*);
void         dor_device_inq_shade_ranges  (dot_object*, DtInt, DtInt, DtInt[]);
void         dor_device_inq_view_extent
		 (dot_object*, dot_object*, DtVolume*, DtVolume *);
void         dor_device_inq_viewport  (dot_object *, DtVolume *);
DtVisualType dor_device_inq_visual_type  (dot_object *);
void         dor_device_inquire_clip_list
		 (dot_object*, DtUInt*, DtReal**, DtReal[4], DtFlag*);
void         dor_device_inquire_interface_functions  (dot_object*,DtInt,DtPtr*);
void         dor_device_inquire_stereo  (dot_object*, DtFlag*);
void         dor_device_install_driver  (char*, char*, DtPFV);
void         dor_device_pick
		 (dot_object*, DtPoint3, DtInt*, DtInt, DtInt[], DtInt, long[],
		  DtReal[], DtReal[], DtReal[], dot_object*[], DtInt*);
void         dor_device_pick_view  (dot_object *, dot_object *, dot_object *);
void         dor_device_print  (dot_object *);
void         dor_device_remove_reference  (dot_object *, dot_object *);
void         dor_device_resize  (dot_object *);
dot_object*  dor_device_return_current_device_object  (void);
void         dor_device_set_aborted_draw_flag  (dot_object *, DtFlag);
void         dor_device_set_auto_size  (dot_object *, DtFlag);
void         dor_device_set_background_color
		 (dot_object *, DtColorModel, DtReal []);
void         dor_device_set_color_entries
		 (dot_object *, DtColorModel, DtInt, DtInt, DtReal []);
void         dor_device_set_current_view  (dot_object *, DtObject);
void         dor_device_set_depth_buffer_enable  (dot_object *, DtFlag);
void         dor_device_set_depth_buffer_write  (dot_object *, DtFlag);
void         dor_device_set_foreground_color
		 (dot_object*, DtColorModel, DtReal []);
void         dor_device_set_frame  (dot_object *, dot_object *);
void         dor_device_set_options  (dot_object *, DtPtr);
void         dor_device_set_pick_aperture  (dot_object *, DtSize3 *);
void         dor_device_set_pick_callback  (dot_object *, dot_object *);
void         dor_device_set_pick_path_order  (dot_object *, DtPickPathOrder);
void         dor_device_set_shade_index  (dot_object *, DtInt);
void         dor_device_set_shade_mode  (dot_object *, DtShadeMode);
void         dor_device_set_shade_ranges
		 (dot_object *, DtInt, DtInt, DtInt [][2]);
void         dor_device_set_viewport  (dot_object *, DtVolume *);
void         dor_device_swap_buffers  (dot_object *);
void         dor_device_terminate  (void);
void         dor_device_update  (dot_object *);
void         dor_device_update_all  (void);
void         dor_device_update_geometry  (dot_object *);
void         dor_device_update_notify  (dot_object *);
void         dor_device_update_view  (dot_object *, dot_object *, dot_object *);
void         dor_device_update_window  (dot_object *, dot_object *);
void         dor_device_viewport_size  (dot_object *);
void         dor_device_write_pixel_byte_rgb (dot_object*,DtInt,DtInt,DtUChar*);
void         dor_device_write_rectangle_byte_rgb
		 (dot_object *, DtInt, DtInt, DtInt, DtInt, DtUChar *);
void         dor_device_write_scanline_byte_rgb
		 (dot_object *, DtInt, DtInt, DtInt, DtUChar *);

#endif
