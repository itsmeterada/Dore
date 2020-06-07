#ifndef DORE_GPOLYD_H
#define DORE_GPOLYD_H
 
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


    /******************************/
    /***  Constant Definitions  ***/
    /******************************/

#define dod_gpolyd_initial_vertices    50
#define dod_gpolyd_initial_contours    10
#define dod_gpolyd_stack_increment     50
#define dod_gpolyd_vertex_increment    50


    /**************************/
    /***  Type Definitions  ***/
    /**************************/

typedef struct {
    DtFlag interpolated;
    DtInt vertex1;
    DtInt vertex2;
    DtReal weight1;
    DtReal weight2;
    DtPoint3 coord;
} dot_gpolyd_interpolated_vertex ;

typedef struct {
  DtInt  vertex;
  DtReal coord[3];
} dot_gpolyd_vertex;

typedef DtInt dot_gpolyd_contour;

typedef struct dot_gpolyd_s_edge {
    struct dot_gpolyd_s_edge *u_previous;
    struct dot_gpolyd_s_edge *u_next;
    struct dot_gpolyd_s_edge *v_previous;
    struct dot_gpolyd_s_edge *v_next;
    DtInt upper_vertex;
    DtInt lower_vertex;
    DtReal slope;
    DtReal uslope;
    DtReal uoffset;
    DtReal wslope;
    DtReal woffset;
    DtReal last_u;
    DtReal this_u;
    dot_gpolyd_interpolated_vertex last_vertex;
    dot_gpolyd_interpolated_vertex this_vertex;
} dot_gpolyd_edge;


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

void   dor_gpolyd_activate_edges          (void);
void   dor_gpolyd_add_edge                (DtInt vertex1, DtInt vertex2);
void   dor_gpolyd_add_vertex              (DtInt, DtReal, DtReal, DtReal);
void   dor_gpolyd_check_vertex_space      (DtInt);
int    dor_gpolyd_chose_axes              (void);
void   dor_gpolyd_close_contour           (void);
void   dor_gpolyd_close_polygon           (void);
int    dor_gpolyd_colinear                (DtInt);
void   dor_gpolyd_create_contour          (void);
void   dor_gpolyd_create_contour_vertex   (DtInt, DtReal, DtReal, DtReal);
void   dor_gpolyd_create_edge_list        (void);
void   dor_gpolyd_create_polygon          (void);
void   dor_gpolyd_delete_edge             (dot_gpolyd_edge* edge);
DtReal dor_gpolyd_edge_intersect          (dot_gpolyd_edge*, dot_gpolyd_edge*);
int    dor_gpolyd_get_polygon_orientation (void);
void   dor_gpolyd_get_vertex              (dot_gpolyd_edge*, DtReal);
DtReal dor_gpolyd_inaline           (DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
void   dor_gpolyd_initialize              (void);
int    dor_gpolyd_leftmost                (DtInt, DtInt);
DtReal dor_gpolyd_next_event              (void);
void   dor_gpolyd_output_triangle         (dot_gpolyd_interpolated_vertex*,
					   dot_gpolyd_interpolated_vertex*,
					   dot_gpolyd_interpolated_vertex*);
void   dor_gpolyd_output_trapezoid        (dot_gpolyd_edge*, dot_gpolyd_edge*);
void   dor_gpolyd_print_edge              (dot_gpolyd_edge *edge);
void   dor_gpolyd_print_edge_list         (void);
void   dor_gpolyd_print_points            (void);
void   dor_gpolyd_set_epsilon             (void);
void   dor_gpolyd_split_polygon           (DtInt, DtInt, DtInt, DtInt);
int    dor_gpolyd_split_vertex            (DtInt, DtInt, DtInt, DtInt, DtInt);
void   dor_gpolyd_stack_initialize        (void);
void   dor_gpolyd_stack_print             (void);
void   dor_gpolyd_stack_push              (DtInt);
int    dor_gpolyd_tri_orientation         (DtReal[3], DtReal[3], DtReal[3]);
void   dor_gpolyd_triangulate_complex_polygon (void);
void   dor_gpolyd_triangulate_concave_polygon (void);
void   dor_gpolyd_triangulate_convex_polygon (void);
void   dor_gpolyd_triangulate_polygon  (DtShapeType, DtPFV, DtPFV, void*);

#endif
