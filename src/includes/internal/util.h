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
 
#ifndef DORE_UTIL_H
#define DORE_UTIL_H
 
/*****************************************************************************
**  This file contains function prototypes for Dore' utility functions that
**  don't have a category-specific header file (e.g. list.h).
*****************************************************************************/

DtReal  dor_math_acos  (DtReal value);
DtReal  dor_math_angle  (DtReal x, DtReal y);
DtReal  dor_math_asin  (DtReal value);
DtReal  dor_math_atan  (DtReal value);
DtReal  dor_math_atan2  (DtReal x, DtReal y);
DtReal  dor_math_ceil  (DtReal value);
DtReal  dor_math_cos  (DtReal value);
void    dor_math_cross_cosines
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		 DtReal, DtReal*, DtReal*, DtReal*);
void    dor_math_crossproduct
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal*,
		 DtReal*, DtReal*);
DtReal  dor_math_distance  (DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtReal  dor_math_distance_squared  (DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
DtReal  dor_math_dotproduct  (DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtReal  dor_math_fabs  (DtReal arg);
void    dor_math_fast_cross_cosines  (DtReal[3],DtReal[3],DtReal[3],DtReal[3]);
DtFlag  dor_math_fast_normal  (DtReal[3], DtReal[3], DtReal[3]);
DtReal  dor_math_fast_sqrt  (DtReal x);
DtReal  dor_math_floor  (DtReal value);
void    dor_math_homo_trns  (DtReal, DtReal, DtReal, DtReal, DtMatrix4x4,
			     DtReal*, DtReal*, DtReal*, DtReal*);
void    dor_math_homo_trns_fast  (DtReal[4], DtMatrix4x4, DtReal[4]);
DtFlag  dor_math_line_intersect
		(DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
void    dor_math_line_intersect_point
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		 DtReal, DtReal*, DtReal*);
DtReal  dor_math_log  (DtReal value);
DtFlag  dor_math_normal  (DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
			  DtReal*, DtReal*, DtReal*);
DtFlag  dor_math_pixel_window
		(DtReal, DtReal, DtReal, DtReal, DtInt, DtReal, DtReal, DtReal,
		 DtReal, DtInt*, DtInt*, DtInt*, DtInt*);
DtFlag  dor_math_plane_equation
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		 DtReal, DtReal*, DtReal*, DtReal*, DtReal*);
void    dor_math_pnttrns
		(DtReal,DtReal,DtReal,DtMatrix4x4,DtReal*,DtReal*,DtReal*);
void    dor_math_pnttrns_fast  (DtReal[3], DtMatrix4x4, DtReal[3]);
DtReal  dor_math_point_line_distance
	       (DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
void    dor_math_polygon_normal_fast  (DtInt, DtRealTriple*, DtReal[3]);
void    dor_math_polygon_plane_equation
		(DtInt, DtRealTriple*, DtReal*, DtReal*, DtReal*, DtReal*);
DtReal  dor_math_pow  (DtReal x, DtReal y);
DtReal  dor_math_projected_polygon_area (DtInt, DtRealTriple *, DtInt, DtInt);
DtReal  dor_math_projected_polygon_area_fast  (DtInt,DtRealTriple*,DtInt,DtInt);
DtReal  dor_math_projected_triangle_area_squared
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtReal  dor_math_projected_triangle_area_squared_fast
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtFlag  dor_math_renormalize  (DtReal*, DtReal*, DtReal*);
DtFlag  dor_math_renormalize_fast  (DtReal normal[3]);
DtFlag  dor_math_renormalize_vector  (DtVector3 vector);
DtShort dor_math_same_side
		(DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
DtReal  dor_math_sin  (DtReal value);
DtReal  dor_math_sqrt  (DtReal value);
void    dor_math_vectrns
		(DtReal,DtReal, DtReal, DtMatrix4x4, DtReal*, DtReal*, DtReal*);
void    dor_math_vectrns_fast  (DtReal[3], DtMatrix4x4, DtReal[3]);

void    dor_print_initialize  (void);
void    dor_print_output  (char *);
char*   dor_print_get_line  (void);
void    dor_print_clear  (char *, int);
void    dor_print_indent_down  (void);
void    dor_print_indent_up  (void);
void    dor_print_set_indent  (int);
void    dor_print_set_field  (char *, int, char *);

DtFlag  dor_quadmsh_check_degen_quad_vtx_values (
		DtReal[3], DtReal[3], DtReal[3], DtReal[3]);

DtPtr   dor_space_allocate  (unsigned);
DtPtr   dor_space_reallocate  (DtPtr, unsigned);
void    dor_space_deallocate  (DtPtr);

DtFlag  dor_util_bigendian_word_byte_order  (void);
char*   dor_util_getenv  (char *);
char*   dor_util_strtok  (char *, char *);

#endif
