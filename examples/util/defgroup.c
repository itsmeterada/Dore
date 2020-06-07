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
 
#include <dore/dore.h>
#include "dore_util.h"

/*
Makes and returns a generic studio group.
*/

DtObject make_definition_group (void)
{
	static DtPoint3
		origin = {0.,0.,0.},
		z_dir = {0.,0.,4.},
		light0 = {1.,1.,1.},
		light1 = {-1.,1.,1.};
	static DtVector3 up = {0.,1.,0.};


	DoGroup(DcTrue);
		DgAddObj(DoPerspective(60.,-1.,-10.));
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, z_dir, up));
			DgAddObj(DoCamera());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light0, up));
			DgAddObj(DoLightIntens(.7));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
		DgAddObj(DoPushMatrix());
			DgAddObj(DoLookAtFrom(origin, light1, up));
			DgAddObj(DoLightIntens(.7));
			DgAddObj(DoLight());
		DgAddObj(DoPopMatrix());
	return(DgClose());
}
