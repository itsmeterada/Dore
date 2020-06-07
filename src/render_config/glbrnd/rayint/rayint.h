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
 
#include "../glbrnd.h"

struct rayint_data {
	DtFlag debug_switch;
	struct rayint_object_modifier *current_rayint_object_modifier;
	struct list *rayint_object_list;
	DtReal space_x_minimum;
	DtReal space_y_minimum;
	DtReal space_z_minimum;
	DtReal space_x_maximum;
	DtReal space_y_maximum;
	DtReal space_z_maximum;
	DtInt minimum_objects_per_box;
	DtInt maximum_subdivision_level;
	DtReal minimum_box_size;
	struct rayint_bsptree_node *rayint_bsptree_root_node;
	};

struct rayint_statistics {
	DtInt rayint_objects;
	DtInt bsptree_nodes;
	DtInt face_nodes;
	DtInt boxes;
	DtInt box_objects;
	DtInt active_boxes;
	DtInt treedepth;
	DtInt treepoints;
	};

struct rayint_bsptree_node {
	DtReal x, y, z;
	DtPtr nodes[8];
	DtInt nodetypes;
	};

struct rayint_box {
	DtReal xmin, ymin, zmin;
	DtReal xmax, ymax, zmax;
	struct list *object_list;
	DtInt facetypes;
	DtPtr facenodes[6];
	};

struct rayint_face_node {
	DtReal center1, center2;
	DtPtr nodes[4];
	DtInt nodetypes;
	};

extern struct rayint_data dre_rayint_data;
extern struct rayint_statistics dre_rayint_statistics;
