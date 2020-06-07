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
  ======================================================================
  Functions:
	dor_gpolyd_initialize
	dor_gpolyd_create_contour
	dor_gpolyd_create_contour_vertex
	dor_gpolyd_create_edge_list
	dor_gpolyd_create_polygon
	dor_gpolyd_delete_edge
	dor_gpolyd_print_edge
	dor_gpolyd_print_edge_list
	dor_gpolyd_print_points
	dor_gpolyd_activate_edges
	dor_gpolyd_add_edge
	dor_gpolyd_add_vertex
	dor_gpolyd_check_vertex_space
	dor_gpolyd_choose_axes
	dor_gpolyd_close_contour
	dor_gpolyd_close_polygon
	dor_gpolyd_colinear
	dor_gpolyd_edge_intersect
	dor_gpolyd_get_polygon_orientation
	dor_gpolyd_get_vertex
	dor_gpolyd_inaline
	dor_gpolyd_leftmost
	dor_gpolyd_next_event
	dor_gpolyd_output_triangle
	dor_gpolyd_output_trapezoid
	dor_gpolyd_set_epsilon
	dor_gpolyd_split_polygon
	dor_gpolyd_split_vertex
	dor_gpolyd_stack_initialize
	dor_gpolyd_stack_print
	dor_gpolyd_stack_push
	dor_gpolyd_triangulate_concave_polygon
	dor_gpolyd_triangulate_complex_polygon
	dor_gpolyd_triangulate_convex_polygon
	dor_gpolyd_tri_orientation

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/gpolyd.h>

#define VERT(vert) (doe_gpolyd_vertices[vert].vertex)

#define X(vertex) (doe_gpolyd_vertices[vertex].coord[0])
#define Y(vertex) (doe_gpolyd_vertices[vertex].coord[1])
#define Z(vertex) (doe_gpolyd_vertices[vertex].coord[2])

#define U(vertex) (doe_gpolyd_vertices[vertex].coord[doe_gpolyd_axis1])
#define V(vertex) (doe_gpolyd_vertices[vertex].coord[doe_gpolyd_axis2])
#define W(vertex) (doe_gpolyd_vertices[vertex].coord[doe_gpolyd_axis3])


/* polygon decomposition declarations */

DtInt               doe_gpolyd_max_vertices;
DtInt               doe_gpolyd_next_vertex;
dot_gpolyd_vertex  *doe_gpolyd_vertices;
DtInt               doe_gpolyd_max_contours;
DtInt               doe_gpolyd_next_contour;
dot_gpolyd_contour *doe_gpolyd_contours;
dot_gpolyd_edge    *doe_gpolyd_edge_list;
dot_gpolyd_edge    *doe_gpolyd_active_edges;
DtPFV               doe_gpolyd_action;
DtPFV               doe_gpolyd_cmpaction;
void               *doe_gpolyd_actionparm;

/* 3d extent of polygon in modelling (local) coordinates */

DtReal doe_gpolyd_max[3];
DtReal doe_gpolyd_min[3];
DtFlag doe_gpolyd_first_vertex;

/* permitted roundoff error */

DtReal doe_gpolyd_epsilon;

/* axes to project polygon onto */

DtShort doe_gpolyd_axis1, doe_gpolyd_axis2, doe_gpolyd_axis3;

/* number of points in contour */

DtInt doe_gpolyd_vertex_count;
DtReal doe_gpolyd_v_event;

/* orientation of contour */

DtInt doe_gpolyd_orientation;

/* polygon stack for concave decomposition, elements specify 
	last vertex of polygon  */

DtInt *doe_gpolyd_stack;
DtInt doe_gpolyd_stack_size;
DtInt doe_gpolyd_stack_ptr;
/*
 ======================================================================
 */

void dor_gpolyd_initialize (void)
{
    /* allocate vertex list */

    doe_gpolyd_vertices = (dot_gpolyd_vertex *)dor_space_allocate
			(dod_gpolyd_initial_vertices*sizeof(dot_gpolyd_vertex));

    if ( doe_gpolyd_vertices == NULL ) {
	DDerror (ERR_CANT_ALLOC_MEM, "dor_gpolyd_initialize", DcNullPtr);
	return;
    }

    doe_gpolyd_max_vertices = dod_gpolyd_initial_vertices;

    /* allocate contour list */

    doe_gpolyd_contours = (dot_gpolyd_contour *)dor_space_allocate
		(dod_gpolyd_initial_contours*sizeof(dot_gpolyd_contour));

    if ( doe_gpolyd_contours == NULL ) {
	DDerror (ERR_CANT_ALLOC_MEM, "dor_gpolyd_initialize", DcNullPtr);
	return;
    }
    doe_gpolyd_max_contours = dod_gpolyd_initial_contours;

    /* initialize polygon stack */

    doe_gpolyd_stack = (DtInt *)0;
    doe_gpolyd_stack_size = 0;
    doe_gpolyd_stack_ptr = 0;
}
/*
 ======================================================================
 */

void dor_gpolyd_create_contour (void)
{
    if ( doe_gpolyd_next_contour >= doe_gpolyd_max_contours-1 ){
	doe_gpolyd_max_contours += 20;
	doe_gpolyd_contours = 
		(dot_gpolyd_contour *) dor_space_reallocate(
			 doe_gpolyd_contours, 
			 doe_gpolyd_max_contours*sizeof(dot_gpolyd_contour));

	if ( doe_gpolyd_contours == NULL ) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dor_gpolyd_create_contour",DcNullPtr);
	    return;
	}
    }
  
    doe_gpolyd_contours[doe_gpolyd_next_contour++] = 
					doe_gpolyd_next_vertex;
    doe_gpolyd_vertex_count = 0;
}
/*
 ======================================================================
 */

void dor_gpolyd_create_contour_vertex (
    DtInt originalvertex,
    DtReal x, DtReal y, DtReal z)
{
    dor_gpolyd_add_vertex(originalvertex,x,y,z) ;  

    if ( !doe_gpolyd_first_vertex){
	doe_gpolyd_max[0] = max(doe_gpolyd_max[0], x);
	doe_gpolyd_max[1] = max(doe_gpolyd_max[1], y);
	doe_gpolyd_max[2] = max(doe_gpolyd_max[2], z);

	doe_gpolyd_min[0] = min(doe_gpolyd_min[0], x);
	doe_gpolyd_min[1] = min(doe_gpolyd_min[1], y);
	doe_gpolyd_min[2] = min(doe_gpolyd_min[2], z);
    }
    else {
	doe_gpolyd_max[0] = x;
	doe_gpolyd_max[1] = y;
	doe_gpolyd_max[2] = z;

	doe_gpolyd_min[0] = x;
	doe_gpolyd_min[1] = y;
	doe_gpolyd_min[2] = z;
	doe_gpolyd_first_vertex = DcFalse;
    }
}
/*
 ======================================================================
 */

void dor_gpolyd_create_edge_list (void)
{
    DtInt contour;
    DtInt first_vertex, last_vertex, vertex, vertex1;

    doe_gpolyd_edge_list = NULL;
  
    for ( contour = 0 ; contour < doe_gpolyd_next_contour-1 ; contour++) {
	first_vertex = doe_gpolyd_contours[contour];
	last_vertex  = doe_gpolyd_contours[contour+1]-1;
      
	for ( vertex = first_vertex; vertex <= last_vertex; vertex++) {
	    if ( vertex == last_vertex )
		    vertex1 = first_vertex;
	    else vertex1 = vertex+1;
	  
	    dor_gpolyd_add_edge(vertex, vertex1);
	}
    }
}
/*
 ======================================================================
 */

void dor_gpolyd_create_polygon (void)
{
    doe_gpolyd_next_vertex  = 0;
    doe_gpolyd_next_contour = 0;
  
    doe_gpolyd_edge_list = NULL;
    doe_gpolyd_active_edges = NULL;
    doe_gpolyd_first_vertex = DcTrue;
}
/*
 ======================================================================
 */

void dor_gpolyd_delete_edge (dot_gpolyd_edge *edge)
{
#   ifdef debug
	printf ("deleting edge");
	dor_gpolyd_print_edge(edge);
#   endif

    if (edge->u_previous != NULL)
	edge->u_previous->u_next = edge->u_next;
    else
	doe_gpolyd_active_edges = edge->u_next;
  
    if (edge->u_next != NULL)
	edge->u_next->u_previous = edge->u_previous;
  
    dor_space_deallocate(edge);
}

  
/*
 ======================================================================
 */

void dor_gpolyd_print_edge (dot_gpolyd_edge *edge)
{
    printf (" edge from %d to %d\n", edge->upper_vertex, edge->lower_vertex);
    printf ("\t(%f, %f, %f) to (%f, %f, %f)\n",
	X(edge->upper_vertex), Y(edge->upper_vertex), Z(edge->upper_vertex),
	X(edge->lower_vertex), Y(edge->lower_vertex), Z(edge->lower_vertex));
}
/*
 ======================================================================
 */

void dor_gpolyd_print_edge_list (void)
{
    dot_gpolyd_edge *e1, *e2;
    int i = 0;
  
    printf("polyd: current edge list\n");
    e1 = doe_gpolyd_edge_list;
    while ( e1 != NULL ){
	e2 = e1;
	e1 = e1->v_next;
	while ( e2 != NULL ) {
	    printf("edge %d:", i++);
	    dor_gpolyd_print_edge(e2);
	    e2 = e2->u_next;
	}
    }
}
/*
 ======================================================================
 */

void dor_gpolyd_print_points (void)
{
    DtInt contour;
    DtInt first_vertex, last_vertex, vertex;

    doe_gpolyd_edge_list = NULL;
    printf("decomposition: original polygon\n") ;  

    for ( contour = 0 ; contour < doe_gpolyd_next_contour-1 ; contour++) {
	printf("contour %d\n", contour);
	first_vertex = doe_gpolyd_contours[contour];
	last_vertex  = doe_gpolyd_contours[contour+1]-1;
      
	for ( vertex = first_vertex; vertex <= last_vertex; vertex++) {
	    printf("\tpoint %d, address %d coordinates = (%f, %f, %f)\n",
		   vertex, VERT(vertex), X(vertex), Y(vertex), Z(vertex));
	}
    }
}
/*
 ======================================================================
 */

void dor_gpolyd_activate_edges (void)
{
    dot_gpolyd_edge *this, *next; 
    dot_gpolyd_edge *current_active, *previous_active;
    
    current_active = doe_gpolyd_active_edges;
    previous_active = NULL;
    next = doe_gpolyd_edge_list;
    doe_gpolyd_edge_list = next->v_next;

#ifdef debug
    if ( next->u_previous != NULL )
	    DDerror (ERR_VALUE_OUT_OF_RANGE,
			"dor_gpolyd_activate_edges", 
			"edge list out of order");
#endif

    while ( next != NULL ){
	this = next;
	next = next->u_next;

	while ( ( current_active != NULL ) && 
	       ( current_active->this_u <= U(this->upper_vertex))) {
	    previous_active = current_active;
	    current_active = current_active->u_next;
	}
      
	if ( previous_active != NULL )
		previous_active->u_next = this;
	else doe_gpolyd_active_edges = this;
      
	if ( current_active != NULL )
		current_active->u_previous  = this;
      
	this->u_previous = previous_active;
	this->u_next = current_active;
	this->last_vertex.interpolated = DcFalse;
	this->last_vertex.vertex1 = VERT(this->upper_vertex);
	this->last_vertex.coord[0] = X(this->upper_vertex);
	this->last_vertex.coord[1] = Y(this->upper_vertex);
	this->last_vertex.coord[2] = Z(this->upper_vertex);
	this->this_u = this->last_u = U(this->upper_vertex);
#ifdef debug
	printf("adding edge");
	dor_gpolyd_print_edge(this);
#endif
	current_active = this;
    }
}
/*
 ======================================================================
 */

void dor_gpolyd_add_edge (DtInt vertex1, DtInt vertex2)
{
    dot_gpolyd_edge *edge;
    DtReal u1, v1, w1, u2, v2, w2;
    dot_gpolyd_edge *previous, *this;

    u1 = U(vertex1);
    v1 = V(vertex1);
    w1 = W(vertex1);

    u2 = U(vertex2);
    v2 = V(vertex2);
    w2 = W(vertex2);

    if ( abs(v2-v1) < doe_gpolyd_epsilon )
	    /* edge is horizontal, ignore it */
	    return;

    edge = (dot_gpolyd_edge *) 
		dor_space_allocate(sizeof(dot_gpolyd_edge));
  
    if ( edge == NULL ) {
	DDerror (ERR_CANT_ALLOC_MEM, "dor_gpolyd_add_edge", DcNullPtr);
	return;
    }

    /* initialize list pointers */

    edge->u_previous = 
    edge->u_next = 
    edge->v_previous = 
    edge->v_next = NULL;

    edge->slope = 1/(v2-v1);
  
    edge->uslope = (u2-u1)*edge->slope;
    edge->uoffset = u1-edge->uslope*v1;

    edge->wslope = (w2-w1)*edge->slope;
    edge->woffset = w1-edge->wslope*v1;
  
    if ( v1 > v2 ) {
	edge->upper_vertex = vertex1;
	edge->lower_vertex = vertex2;
	edge->last_u = u1;

    } else {
	edge->upper_vertex = vertex2;
	edge->lower_vertex = vertex1;
	edge->last_u = u2;
    }

    /* add edge to edge list */

    for (this = doe_gpolyd_edge_list, previous = NULL ; this != NULL ; 
	 this = this->v_next ) {
	if ( V(this->upper_vertex) <= V(edge->upper_vertex) )
		break;
	previous = this;
    }

    /* case 1: 
	no entries in list lower than the edge we are adding*/
  
    if ( this == NULL ) {
	if ( previous == NULL )
		/* list is empty */
		doe_gpolyd_edge_list = edge;
	else {
	    previous->v_next = edge;
	    edge->v_previous = previous;
	}
	return;
    }

    /* case 2: at least one edge below what were adding */
  
    if ( V(this->upper_vertex) < V(edge->upper_vertex) ) {
	edge->v_previous = this->v_previous;
	this->v_previous = edge;
	edge->v_next = this;
	if ( edge->v_previous != NULL )
		edge->v_previous->v_next = edge;
	else 
		/* was at begining of list */
		doe_gpolyd_edge_list = edge;
	return;
    }
  
    /* case 3: 
	the edge were adding has same v upper coordinate as an 
	edge already in the list place both in the same bucket */

    edge->v_previous = this->v_previous;
    edge->v_next = this->v_next;

    while ( U(this->upper_vertex) < U(edge->upper_vertex) ){
	if ( this->u_next == NULL ) 
		{		/* add it onto end */
		    this->u_next = edge;
		    edge->u_previous = this;
		    edge->u_next = NULL;
		    return;
		}
      
	this = this->u_next;
    }
  
    /* add it into middle of list */

    edge->u_previous = this->u_previous;
    edge->u_next = this;
    this->u_previous = edge;
    if ( edge->u_previous != NULL )
	    edge->u_previous->u_next = edge;
    else {
	/* this was at begining */
      
	if ( this->v_previous != NULL )
		this->v_previous->v_next = edge;
	else doe_gpolyd_edge_list = edge;

	if ( this->v_next != NULL )
		this->v_next->v_previous = edge;
    }
}


/*
 ======================================================================
 */

void dor_gpolyd_add_vertex (
    DtInt originalvertex,
    DtReal x, DtReal y, DtReal z)
{
    if ( doe_gpolyd_next_vertex >= doe_gpolyd_max_vertices){
	doe_gpolyd_max_vertices += dod_gpolyd_vertex_increment;
	doe_gpolyd_vertices = 
		(dot_gpolyd_vertex *) dor_space_reallocate(
			doe_gpolyd_vertices, 
			doe_gpolyd_max_vertices*sizeof(dot_gpolyd_vertex));
      
	if ( doe_gpolyd_vertices == NULL ) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dor_gpolyd_add_vertex", DcNullPtr);
	    return;
	}
    }

    doe_gpolyd_vertex_count++;
  
    VERT(doe_gpolyd_next_vertex) = originalvertex;
    X(doe_gpolyd_next_vertex) = x;
    Y(doe_gpolyd_next_vertex) = y;
    Z(doe_gpolyd_next_vertex) = z;
    doe_gpolyd_next_vertex++;
}
/*
 ======================================================================
 */

void dor_gpolyd_check_vertex_space (DtInt index)
{
    DtInt space;

    if ( doe_gpolyd_max_vertices <= index ){
	doe_gpolyd_max_vertices = index+50;
	space = doe_gpolyd_max_vertices*sizeof(dot_gpolyd_vertex);
	doe_gpolyd_vertices = 
		(dot_gpolyd_vertex *)dor_space_reallocate(
					doe_gpolyd_vertices, space);
    }
}
/*
 ======================================================================
 */

    /*  Decide which axes to project polygon onto by looking at 
	components of normal */
	
static int dor_gpolyd_choose_axes (void)
{
    DtReal indxy, indyz, indzx;
    DtInt next, this, previous;

    for ( this = 0 ; this < doe_gpolyd_next_vertex ; this++){
	if ( this == 0 )	/* first point in polygon? */
		previous = doe_gpolyd_next_vertex-1;
	else previous = this-1;
      
	if ( this == doe_gpolyd_next_vertex-1 ) 
		/* last point in polygon */
		next = 0;
	else next = this+1;
      
	indxy = dor_gpolyd_inaline(X(previous), Y(previous),
				   X(this), Y(this),
				   X(next), Y(next));

	indyz = dor_gpolyd_inaline(Y(previous), Z(previous),
				   Y(this), Z(this),
				   Y(next), Z(next));

	indzx = dor_gpolyd_inaline(Z(previous), X(previous),
				   Z(this), X(this),
				   Z(next), X(next));

	/* points colinear in all 3 projections */

	if ( ( indxy < doe_gpolyd_epsilon ) && 
	    ( indyz < doe_gpolyd_epsilon ) && 
	    ( indzx < doe_gpolyd_epsilon ) )
		continue;

	if ( (indxy >= indyz) && (indxy >= indzx) )
		{		/* project onto xy plane */
		    doe_gpolyd_axis1 = 0;
		    doe_gpolyd_axis2 = 1;
		    doe_gpolyd_axis3 = 2;
#ifdef debug
		    printf("project onto xy plane\n");
#endif
		    return 1;
		}
  
	if ( (indyz >= indxy) && (indyz >= indzx) )
		{		/* project onto yz plane */
#ifdef debug
		    printf("project onto yz plane\n");
#endif
		    doe_gpolyd_axis1 = 1;
		    doe_gpolyd_axis2 = 2;
		    doe_gpolyd_axis3 = 0;
		    return 1;
		}
  
	/* else project onto zx plane */

#ifdef debug
	printf("project onto zx plane\n");
#endif
	doe_gpolyd_axis1 = 2;
	doe_gpolyd_axis2 = 0;
	doe_gpolyd_axis3 = 1;
	return 1;
    }

    DDerror (ERR_VALUE_OUT_OF_RANGE, "dor_gpolyd_choose_axes",
		"degenerate polygon");

    return 0;
}  
  
/*
 ======================================================================
 */

void dor_gpolyd_close_contour (void)
{
    if ( doe_gpolyd_vertex_count < 3 )
	DDerror (ERR_VALUE_OUT_OF_RANGE, "dor_gpolyd_close_contour",
		"not enough points in contour, should be >= 3");
}
/*
 ======================================================================
 */

void dor_gpolyd_close_polygon (void)
{
    if ( doe_gpolyd_next_contour == 0 ) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		 "dor_gpolyd_close_polygon", "no contours in polygon");
	return;
    }
  
    doe_gpolyd_contours[doe_gpolyd_next_contour++] = 
					doe_gpolyd_next_vertex;

    dor_gpolyd_set_epsilon();
}
/*
 ======================================================================
 */

/* check if 3 points in buffer are colinear */

int dor_gpolyd_colinear (DtInt n /* Largest Index of the Three Points */)
{
    DtInt l, m;			/* indices of other two points */

    m = n-1;
    l = n-2;

    /* if less than (some arbitrary) roundoff value,
	deem to be colinear */

    return(dor_gpolyd_inaline(U(l),V(l),U(m),V(m),U(n),V(n)) < 
	   doe_gpolyd_epsilon);
}
/*
 ======================================================================
 */

DtReal dor_gpolyd_edge_intersect (
    dot_gpolyd_edge *edge1,
    dot_gpolyd_edge *edge2)
{
    return ((edge2->uoffset-edge1->uoffset) / (edge1->uslope-edge2->uslope));
}
  
/*
 ======================================================================
 */

int dor_gpolyd_get_polygon_orientation (void)
{
    DtInt left_most, first_vertex, last_vertex, vertex; 
    DtInt contour, lb, la;
  
    doe_gpolyd_orientation = 0;

    for ( contour = 0 ; contour < doe_gpolyd_next_contour-1 ; contour++) {
	first_vertex = doe_gpolyd_contours[contour];
	last_vertex  = doe_gpolyd_contours[contour+1]-1;

	/* find left most vertex of this contour */

	left_most = first_vertex;
      
	for ( vertex = first_vertex+1; vertex <= last_vertex; vertex++) 
		if ( U(vertex) < U(left_most) )
			left_most = vertex;

	/* find a distinct vertex coming before the leftmost */

	lb = left_most;
      
	while (1){
	    /* decrement with wraparound */

	    lb = ( lb == first_vertex )?last_vertex:(lb-1);

	    /* try next contour if all vertices used up */

	    if ( lb == left_most )
		    break;
	  
	    if (abs(U(left_most)-U(lb)) > doe_gpolyd_epsilon )
		    break;
	  
	    if (abs(V(left_most)-V(lb)) > doe_gpolyd_epsilon ) 
		    break;
	  
	    if (abs(W(left_most)-W(lb)) > doe_gpolyd_epsilon )
		    break;

	} /* loop to find distinct vertex coming before the leftmost */

	if ( lb == left_most )
		/* all points on this contour are coincident! */
		continue;

	/* find vertex coming after the left most which gives 
		nonzero cross product */

	la = left_most;
      
	while ( doe_gpolyd_orientation == 0 ){
	    /* increment with wraparound */

	    la = ( la == last_vertex )?first_vertex:la+1;

	    if ( la == left_most )
		    /* all vertices of polygon are colinear */
		    break;
	  
	    doe_gpolyd_orientation = 
			dor_gpolyd_tri_orientation(
				doe_gpolyd_vertices[lb].coord,
				doe_gpolyd_vertices[left_most].coord,
				doe_gpolyd_vertices[la].coord);
	} /* loop to get orientation of contour */
      
	if ( la == left_most ) {
		/* all vertices of contour where colinear */
	    continue ;	
	} else {
	    /* orientation found */
	    break ;		
	}
    }	/* check against each contour */

    return doe_gpolyd_orientation;
}
/*
 ======================================================================
 */

void dor_gpolyd_get_vertex (dot_gpolyd_edge *edge, DtReal v_event)
{
    /*
      DtReal dv = edge->slope;
      */
    DtInt uv = edge->upper_vertex, lv = edge->lower_vertex;

    /* compute lower vertex */
  
    if ( v_event == V(lv)) {
	edge->this_vertex.interpolated = DcFalse;
	edge->this_vertex.vertex1  = VERT(lv);
	edge->this_vertex.coord[0] = X(lv);
	edge->this_vertex.coord[1] = Y(lv);
	edge->this_vertex.coord[2] = Z(lv);

    } else {
	edge->this_vertex.interpolated = DcTrue;
	edge->this_vertex.vertex1 = VERT(uv);
	edge->this_vertex.vertex2 = VERT(lv);
	/*
	  edge->this_vertex.weight1 = 
			(v_event-V(lv))/dv;
	  edge->this_vertex.weight2 = 
			1.0-edge->this_vertex.weight1;
	*/

	edge->this_vertex.weight1 = 
			(v_event-V(lv))/(V(uv) - V(lv));
	edge->this_vertex.weight2 = 
			1.0 - edge->this_vertex.weight1;

	edge->this_vertex.coord[doe_gpolyd_axis1] =
			edge->uslope*v_event+edge->uoffset;
	edge->this_vertex.coord[doe_gpolyd_axis2] = 
			v_event;
	edge->this_vertex.coord[doe_gpolyd_axis3] = 
			edge->wslope*v_event+edge->woffset;
    }
    edge->this_u = edge->this_vertex.coord[doe_gpolyd_axis1];
}
/*
 ======================================================================
 */

DtReal dor_gpolyd_inaline (
    DtReal x1, DtReal y1,
    DtReal x2, DtReal y2,
    DtReal x3, DtReal y3)
{
    DtReal ind;

    ind = (x1-x2)*(y1-y3)-(x1-x3)*(y1-y2);
    return abs(ind);
}
/*
 ======================================================================
 */

int dor_gpolyd_leftmost (DtInt m, DtInt n /* Start & End Vertices of Polygon */)
{
    DtInt j, left_most;

    left_most = m;
    for ( j = m+1 ; j <= n ; j++ )
	if ( U(j) < U(left_most) )
	    left_most = j;

    return left_most;
}
/*
 ======================================================================
 */


DtReal dor_gpolyd_next_event (void)
{
    DtReal v_event = 0.;
    DtFlag intersection_found;
    DtFlag event_defined = DcFalse;
    dot_gpolyd_edge *this, *previous, *pp, *next;
  
  
    /* events can be of three kinds, end of an active edge, 
	activating a new edge, intersection of two active edges */

    for ( this = doe_gpolyd_active_edges ; this != NULL ; this = this->u_next){
	if ( event_defined )
		v_event = max(v_event, V(this->lower_vertex));
	else v_event = V(this->lower_vertex);
	event_defined = DcTrue;
    }
  
    if ( doe_gpolyd_edge_list != NULL ) {
	if ( event_defined )
		v_event = max(v_event, V(doe_gpolyd_edge_list->upper_vertex));
	else v_event = V(doe_gpolyd_edge_list->upper_vertex);
	event_defined = DcTrue;
    }
  
    if (!event_defined)		/* should not happen */
	    DDerror (ERR_SEVERE_INTERNAL_ERROR,
			"dor_gpolyd_next_event", 
			"unable to find next event");

    /* if there are no active edges return */

    if ( doe_gpolyd_active_edges == NULL )
	    return v_event;
  
    /* else, check for any crossings */

    while (DcTrue) {
	intersection_found = DcFalse;
	this = doe_gpolyd_active_edges;
	previous = NULL;
	while ( this != NULL ){
	    dor_gpolyd_get_vertex(this, v_event);
	    if ( previous != NULL )
		if (( previous->this_u > this->this_u ) &&
		    ( abs(previous->this_u-this->this_u) > doe_gpolyd_epsilon )){
		    /* possible intersection */
		    if ( abs(previous->last_u-this->last_u) <= doe_gpolyd_epsilon ) {
		        /* was originally misordered, reorder */
		        pp = previous->u_previous;
			next = this->u_next;
			if ( pp != NULL )
			    pp->u_next = this;
			else doe_gpolyd_active_edges = this;
			this->u_previous = pp;
			this->u_next = previous;
			previous->u_previous = this;
			previous->u_next = next;
			if ( next != NULL )
			    next->u_previous = previous;

		    } else {
			/* edges must have crossed */
			v_event = dor_gpolyd_edge_intersect(previous, this);
		    }
		    intersection_found = DcTrue;
		    break;
	     }
	     previous = this;
	     this = this->u_next;
	}	/* loop for each active edge */
	if (!intersection_found)
		break;
    }	/* loop on all edges until no intersections */
    return v_event;
}
/*
 ======================================================================
 */

#undef debug

void dor_gpolyd_output_triangle (
    dot_gpolyd_interpolated_vertex *vertex1,
    dot_gpolyd_interpolated_vertex *vertex2,
    dot_gpolyd_interpolated_vertex *vertex3)
{
    DtInt orientation;
  
#ifdef debug
    printf("decomp: outputing triangle: (%f, %f, %f), (%f, %f, %f), (%f, %f, %f)\n",
	   vertex1->coord[0], vertex1->coord[1], vertex1->coord[2],
	   vertex2->coord[0], vertex2->coord[1], vertex2->coord[2],
	   vertex3->coord[0], vertex3->coord[1], vertex3->coord[2]);
#endif
  
    orientation = dor_gpolyd_tri_orientation(vertex1->coord, 
						vertex2->coord, 
					        vertex3->coord);

    if ( orientation == 0 )
	    return;
  
    if ( orientation == doe_gpolyd_orientation )
	(*doe_gpolyd_cmpaction)(doe_gpolyd_actionparm, 
				vertex1,vertex2,vertex3);
    else 
  	(*doe_gpolyd_cmpaction)(doe_gpolyd_actionparm, 
				vertex3,vertex2,vertex1);
}
/*
 ======================================================================
 */

void dor_gpolyd_output_trapezoid (
    dot_gpolyd_edge *in_edge,
    dot_gpolyd_edge *out_edge)
{
    dor_gpolyd_output_triangle(&(in_edge->this_vertex), 
				&(in_edge->last_vertex), 
			        &(out_edge->last_vertex));
    dor_gpolyd_output_triangle(&(in_edge->this_vertex), 
				&(out_edge->last_vertex),
			        &(out_edge->this_vertex));
}
/*
 ======================================================================
 */

void dor_gpolyd_set_epsilon (void)
{
    doe_gpolyd_epsilon = 0.00000001;
}
/*
 ======================================================================
 */

/* split polygon into two polygons */

void dor_gpolyd_split_polygon (
    DtInt m,  DtInt m1,
    DtInt n1, DtInt n)
{
    DtInt j, k, k1, k2;

    /* m and n are the bounds of large polygon 
       m1 and n1 are the bounds of one of the subpolygons */

    /* check theres enough space for copy of subpolygon */

    dor_gpolyd_check_vertex_space(n+n1-m1+3) ; 
    
    /* copy subpolygon out of the way */

    k = n+3-m1;
    for (j = m1 ; j <= n1 ; j++)
      	doe_gpolyd_vertices[j+k] = doe_gpolyd_vertices[j];
    
    /* move top half of second polygon */

    for ( j = n ; j>= n1 ; j-- )
        doe_gpolyd_vertices[j+2] = doe_gpolyd_vertices[j];

    doe_gpolyd_next_vertex = n+3;

    /* move bottom half of second subpolygon */

    k = n1-m1+1;
    for ( j = m1; j>=m; j--)
     	doe_gpolyd_vertices[j+k] = doe_gpolyd_vertices[j];

    /* copy back the first subpolygon */

    k1 = n+3-m1;
    k2 = m-m1;
    
    for ( j = m1 ; j<= n1 ; j++ )
         doe_gpolyd_vertices[j+k2] = doe_gpolyd_vertices[j+k1];
}
/*
 ======================================================================
 */

/* find two vertices on which to divide the polygon */

int dor_gpolyd_split_vertex (
    DtInt l,		/* index of the leftmost vertex */
    DtInt la, DtInt lb,	/* indices of the vtxs before and after leftmost */
    DtInt m,  DtInt n)	/* indices of the first and last vtxs of the poly */
{
    DtReal yu,yl;    /* upper and lower boundries of the triangle */
    DtInt lpu,lpl;   /* indices of upper and lower triangle division pts */
    DtInt k ;        /* a variable for stepping through the polygon vtxs */
    DtInt split_vertex;

    /* compute upper and lower bounds for triangle */

    yu = max(V(l), V(la));
    yu = max(yu, V(lb));
  
    yl = min(V(l), V(la));
    yl = min(yl, V(lb));

    /* determine which of the two vertices lies above the other */

    if ( V(lb) > V(la)){
      	lpu = lb;
      	lpl = la;
    } else {
      	lpu = la;
      	lpl = lb;
    }

    /* initial choice for split_vertex is the 
	rightmost of la and lb */
  
    if ( U(lb) > U(la))
    	split_vertex = lb;
    else 
	split_vertex = la;

    /* search for any polygon vertices interior 
	to triangle l, la, lb */
  
    for ( k = m ; k<=n ; k++ ) {
    /* disregard the vertices of the proposed 
	triangle l, la, lb */

    	if ( k == la ) 
	    continue;
	
    	if ( k == l ) 
	    continue;
	
    	if ( k == lb ) 
	    continue;

  	/* disregard vertex if above proposed 
		triangle l, la, lb */

    	if ( V(k) > yu ) 
	    continue;

  	/* disregard vertex if below proposed 
		triangle l, la, lb */

    	if ( V(k) < yl )  
	    continue;

  	/* disregard vertex if further right 
		than current leftmost point */

    	if ( U(k) >= U(split_vertex))
	    continue;

	/* if all above tests succeed check if point k 
		is inside proposed triangle */

	/* test against line l-lpu */

    	if (( V(k) - V(l))*( U(lpu) - U(l)) > 
	    ( V(lpu) - V(l))*( U(k) - U(l)))
	    continue;
         
	/* test against line lpu-lpl */

    	if ((V(k) - V(lpu))*(U(lpl) - U(lpu)) >
	    (V(lpl) - V(lpu))*(U(k) - U(lpu))) 
		 continue;
         
	/* test against line l-lpl */

    	if (( V(k) - V(l))*( U(lpl) - U(l)) < 
	    ( V(lpl) - V(l))*( U(k) - U(l)))
		 continue;

	/* point k must be interior to the triangle and 
	further left than any other point so far so mark it 
	the new vertex to split polygon on  */

      	split_vertex = k;
      }

  return split_vertex;
}
/*
 ======================================================================
 */

void dor_gpolyd_stack_initialize (void)
{
    doe_gpolyd_stack_ptr = -1;
    dor_gpolyd_stack_push (-1);
}
/*
 ======================================================================
 */

void dor_gpolyd_stack_print (void)
{
    int i, j;

    for ( i = doe_gpolyd_stack_ptr ; i > 0 ; i--) {
	printf("polygon %d\n", i);
	for ( j = doe_gpolyd_stack[i-1]+1 ; j <= doe_gpolyd_stack[i] ; j++) {
	    printf("\tid = %d, vertex %d = (%f, %f, %f)\n",
		   j, VERT(j), X(j), Y(j), Z(j));
	}
    }
}
/*
 ======================================================================
 */

void dor_gpolyd_stack_push (DtInt value)
{
    DtInt space;

    doe_gpolyd_stack_ptr++;

    if ( doe_gpolyd_stack_ptr == doe_gpolyd_stack_size ){
	doe_gpolyd_stack_size += dod_gpolyd_stack_increment;
	space = doe_gpolyd_stack_size*sizeof(DtInt);
	doe_gpolyd_stack = dor_space_reallocate(doe_gpolyd_stack, space);
    }

    doe_gpolyd_stack[doe_gpolyd_stack_ptr] = value;
}
/*
 ======================================================================
 */

void dor_gpolyd_triangulate_concave_polygon (void)
{
    DtInt m,n;	  /* begining and ending vertices of polygon */
    DtInt l;	  /* index of leftmost vertex of polygon */
    DtInt la,lb;  /* indices of vtxs coming before and after leftmost */
    DtInt ls;	  /* index of a possible vertex for the split */
    DtInt m1,n1;  /* the vertices of the split */

    dor_gpolyd_stack_initialize();
    dor_gpolyd_stack_push(doe_gpolyd_next_vertex-1);

#ifdef debug
    printf("decomposing polygon\n");
    dor_gpolyd_stack_print();
#endif
	
    while ( doe_gpolyd_stack_ptr > 0 ) {
	m = doe_gpolyd_stack[doe_gpolyd_stack_ptr-1]+1;
	n = doe_gpolyd_stack[doe_gpolyd_stack_ptr];
	if ( n-m == 2 ) {	
	    /* we have a 3 vertex polygon, check for degeneracy */

	    if ( !dor_gpolyd_colinear(m+2)) { 
		/* we have a triangle output it */
		(*doe_gpolyd_action)(doe_gpolyd_actionparm,
				     VERT(m), VERT(m+1), VERT(m+2));
	    }
	    doe_gpolyd_stack_ptr--;
#ifdef debug
	    dor_gpolyd_stack_print();
#endif
	} else {
	    l = dor_gpolyd_leftmost(m, n);
	    la = (l==n)?m:l+1;
	    lb = (l==m)?n:l-1;
	    ls = dor_gpolyd_split_vertex(l, la, lb, m, n);
#ifdef debug
	    printf("l = %d, la = %d, lb = %d, ls = %d\n", 
			l, la, lb, ls);
#endif
	    if ( (ls==la) || (ls==lb)) {
		m1 = min(la, lb);
		n1 = max(la, lb);
	    } else {
		m1 = min(l, ls);
		n1 = max(l, ls);
	    }
		
	    dor_gpolyd_split_polygon(m, m1, n1, n);
	    doe_gpolyd_stack[doe_gpolyd_stack_ptr] = m+n1-m1;
	    dor_gpolyd_stack_push(n+2);
#ifdef debug
	    dor_gpolyd_stack_print();
#endif
	}
    }
}
/*
 ======================================================================
 */

/* there are a number of areas where the algorithm used here 
   could be improved:
   
   Preallocate an edge table, rather than allocating individual 
   elements each time this module is called (ala the vertex and 
   contour lists).

   Rather than searching for the leftmost vertex we could keep 
   a record of which vertex (in the first contour) contributed 
   to the minimum value of each component.  Then we would not 
   need to search if the first contour was not degenerate 
   (the normal case).

   We have done most of the work to compute the polygons normal, 
   we could calculate this and provide it with the triangle 
   vertices.  

   Where should backfacing polygons be removed it does not make 
   sense to decompose polygons which are backfacing when backface 
   culling is being performed.

   To provide a more optimal decomposition for the typical case 
   of single contours, could check for intersections, if none are 
   found then call cnvpolyd.  This could help with the helvetica text.

   In cnvpolyd could check for convexity then user simpler 
   decomposition algorithm.

   Might be able to improve search for next event by keeping 
   ordered list of lower vertices for each active edge.  */


void dor_gpolyd_triangulate_complex_polygon (void)
{
    dot_gpolyd_edge *next_edge, *in_edge, *out_edge, *this_edge;

    /* create edge list */

    dor_gpolyd_create_edge_list();

#ifdef debug
    dor_gpolyd_print_edge_list();
#endif

    doe_gpolyd_active_edges = NULL;
  
    while (( doe_gpolyd_active_edges != NULL ) || 
	   ( doe_gpolyd_edge_list != NULL )){
	/* find next event, reordering active edges if necessary, 
	   also compute the intersection of each active edge 
	   (if any) with the line y = doe_gpolyd_v_event */

	doe_gpolyd_v_event = dor_gpolyd_next_event();

#ifdef debug
	printf("next event = %f\n", doe_gpolyd_v_event);
#endif

	next_edge = doe_gpolyd_active_edges;
      
	while ( next_edge != NULL ) { 
	    /* compute trapezoids for active edges */

	    in_edge = next_edge;
	    out_edge = in_edge->u_next;

	    if ( out_edge == NULL ) { /* should never happen */
		DDerror (ERR_SEVERE_INTERNAL_ERROR,
			    "dor_gpolyd_decompose", 
			    "unable to find closing edge");
	    } 

	    next_edge = out_edge->u_next;

	    dor_gpolyd_output_trapezoid(in_edge, out_edge);
	}

	/* prepare for next itteration */

	next_edge = doe_gpolyd_active_edges;

	while ( next_edge != NULL ){
	    this_edge = next_edge;
	    next_edge = next_edge->u_next;
	    if ( abs(V(this_edge->lower_vertex)-doe_gpolyd_v_event) < 
		doe_gpolyd_epsilon) { /* edge finished with ? */
		dor_gpolyd_delete_edge(this_edge);
	    } else {
#ifdef debug
		printf("did not delete");
		dor_gpolyd_print_edge(this_edge);
#endif
		this_edge->last_vertex = this_edge->this_vertex;
		this_edge->last_u = this_edge->this_u;
	    }
	}
            
	while (( doe_gpolyd_edge_list != NULL ) &&
	       ( abs(V(doe_gpolyd_edge_list->upper_vertex) - doe_gpolyd_v_event )
		< doe_gpolyd_epsilon)) { /* introduce next set of edges */
	    dor_gpolyd_activate_edges();
	}
    }
}
/*
 ======================================================================
 */

void dor_gpolyd_triangulate_convex_polygon (void)
{
    DtInt i;

    for ( i = 1 ; i < doe_gpolyd_next_vertex-1 ; i++ ) {
	(*doe_gpolyd_action)( doe_gpolyd_actionparm, 
				VERT(0), VERT(i), VERT(i+1));
    }
}
/*
 ======================================================================
 */

int dor_gpolyd_tri_orientation (
    DtReal vertex1[3],
    DtReal vertex2[3],
    DtReal vertex3[3])
{
    register DtReal ind ;   /* axis3 component of cross product */

    ind = (vertex2[doe_gpolyd_axis1]-vertex1[doe_gpolyd_axis1])*
	   (vertex3[doe_gpolyd_axis2]-vertex2[doe_gpolyd_axis2])-
	   (vertex3[doe_gpolyd_axis1]-vertex2[doe_gpolyd_axis1])*
	   (vertex2[doe_gpolyd_axis2]-vertex1[doe_gpolyd_axis2]);

    /* if less than (some arbitrary) roundoff value, 
	deem to be colinear */

    return (( abs(ind)<1.e-10 )?0:((ind<0)?-1:1));
}
/*
 ======================================================================
 */

void dor_gpolyd_triangulate_polygon (
    DtShapeType shape,
    DtPFV       action,
    DtPFV       cmpaction,
    void       *actionparm)
{
#   ifdef debug
	printf("decomposing polygon\n");
	dor_gpolyd_print_points();
#   endif

    doe_gpolyd_action = action;
    doe_gpolyd_cmpaction = cmpaction;
    doe_gpolyd_actionparm = actionparm;
    
    /* decide which projection to use */

    dor_gpolyd_choose_axes();

    /* get polygon orientation */

    if ( dor_gpolyd_get_polygon_orientation() == 0 )
      /* polygon is degenerate: all contours of polygon 
	    are straight lines */
      return;

    /* check for convex polygon */

    if (( doe_gpolyd_next_contour == 2 ) && 
	( shape == DcConvex )) {
	    dor_gpolyd_triangulate_convex_polygon();
	    return;
    }

    /* check for concave polygon */

    if (( doe_gpolyd_next_contour == 2 ) && 
	( shape == DcConcave )) {
	    dor_gpolyd_triangulate_concave_polygon();
	    return;
    }
	    
    /* decompose complex polygon */

    dor_gpolyd_triangulate_complex_polygon();
}
