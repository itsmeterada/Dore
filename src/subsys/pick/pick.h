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
#include <dore/internal/clip3d.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/device.h>

struct pick_data {
    dot_object   *device;
    dot_object   *view;

    DtMatrix4x4   projmat;
    DtMatrix4x4   ccstowcsmat;
    DtMatrix4x4   wcstoccsmat;
    DtMatrix4x4   npctowcsmat;
    DtMatrix4x4   wcstofcsmat;
    DtMatrix4x4   fcstowcsmat;
    DtMatrix4x4   fcstolcsmat;

    DtShort       camtype;
    DtVolume      dgi_viewport;
    DtReal        wcscamloc[3];
    DtReal        wcscamnorm[3];
    DtInt         viewtype;

    DtInt        *hit_count;
    DtInt         index_size;
    DtInt        *index;
    DtInt         list_size;
    DtReal       *z_values;
    DtReal       *wcs_points;
    DtReal       *lcs_points;
    dot_object  **views;
    DtInt        *error_word;
    DtInt         hits_on_element;
    DtReal        front_z_of_hit;
    DtNPoint3     wcspoint;
    DtNPoint3     lcspoint;
    long         *stack_top;		/* pointers into local space */
    long         *stack_base;		/* Base of Picking Stack */
    DtInt         stack_words;		/* Number of Values in Picking Stack */
    long         *hit_list;
    long         *last_hit;		/* pointers into user space  */
    long         *next_hit;
    DtHitStatus (*callback_function)();
    DtPtr         callback_data;
    DtFlag        pass_by_ref;
    dot_object   *current_element;
    DtMatrix4x4   lcstofcsmat;
    DtMatrix4x4   lcstofcsmat_temp;
    DtMatrix4x4   object_projmat;
    DtInt         object_viewtype;
};

extern struct pick_data doe_pick_data;

#define dod_pick_fp_max_exp   126
#define dod_pick_fp_min_exp  -126

#define dod_pick_viewtype_perspective	0
#define dod_pick_viewtype_parallel	1
#define dod_pick_viewtype_matrix	2

#define STACK_GROW_SIZE		100
#define STACK_GROW_WORDS	(3 * STACK_GROW_SIZE)

    /***  Function Prototypes  ***/

	/* These functions are used by the pick function group, and are not
	** prototyped in dogen.h.  */

void        dor_pick  (dot_object *);
DtHitStatus dor_pick_all  (DtPtr, DtInt, long[], DtReal, DtObject, DtInt);
DtFlag      dor_pick_bounding_box_out  (DtRealTriple[8]);
DtHitStatus dor_pick_closest  (DtPtr, DtInt, long[], DtReal, DtObject, DtInt);
void        dor_pick_compute_wcstofcs  (void);
DtHitStatus dor_pick_first  (DtPtr, DtInt, long[], DtReal, DtObject, DtInt);
void        dor_pick_initialize_after_environment  (void);
void        dor_pick_initialize_before_environment  (void);
void        dor_pick_initialize_ccstowcsmat  (void);
void        dor_pick_initialize_environment  (void);
DtInt       dor_pick_record_hit  (DtInt);
void        dor_pick_report_point  (dot_clip3d_vertex*, DtPtr*);
void        dor_pick_report_triangle (dot_clip3d_vertex*, dot_clip3d_vertex*,
				      dot_clip3d_vertex*, DtShort, DtPtr);
void        dor_pick_report_vector (dot_clip3d_vertex*, dot_clip3d_vertex*,
				    DtShort, DtPtr);
