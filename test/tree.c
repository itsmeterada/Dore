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
 
/*  ======================================================================
    This file contains the display and studio objects for the executables:

	pro_tree
	dyn_tree

    The display group is a tree composed of cylinders and spheres.
    Reflections and shadows are displayable on some platforms.
    ====================================================================== */

#include <dore/dore.h>

#ifdef SVR4
#define RANDOM(x)         rand(x)
#define SRANDOM(x)        srand(x)
#else
#define RANDOM(x)         random(x)
#define SRANDOM(x)        srandom(x)
#endif

#define RAND ((double)(RANDOM()%512)/512)        /* random double 0..1 */
#define DTOR(alpha)  ((alpha) * 0.01745329252)     /* degrees to radians */

#define SCALE .7    /* scale factor to shrink subtree by at each level */
#define MAX_DEPTH 4 /* greatest number of levels in the tree           */

extern int shadow_callback();
extern int reflection_callback();
extern int backface_callback();
extern int reptype_callback();
extern int interptype_callback();
extern int specular_callback();
extern int translate_callback();
extern int rotate_callback();
extern int scale_callback();
extern int subdivspec_callback();

extern void grow_tree(int);

DtObject makeobject()
{
   static DtReal groundcolor[] = {1.,.8,1.};

   DoGroup(DcTrue);
        DgAddObj(DoPickSwitch(DcOn));

        DgAddObj(DoCallback(backface_callback, DcNullObject));
        DgAddObj(DoBackfaceCullable(DcOn));

        DgAddObj(DoCallback(reptype_callback, DcNullObject));
        DgAddObj(DoCallback(interptype_callback, DcNullObject));

        DgAddObj(DoDiffuseIntens(1.));
        DgAddObj(DoSpecularIntens(1.));

        DgAddObj(DoCallback(specular_callback, DcNullObject));

        DgAddObj(DoCallback(translate_callback, DcNullObject));

        DgAddObj(DoCallback(rotate_callback, DcNullObject));

        DgAddObj(DoCallback(scale_callback, DcNullObject));

        DgAddObj(DoCallback(subdivspec_callback, DcNullObject));


        DgAddObj(DoCallback(shadow_callback, DcNullObject));
	
	DgAddObj(DoPushMatrix());
                DgAddObj(DoRepType(DcSurface));          
                DgAddObj(DoTranslate(0.,-1.2,0.));     
                DgAddObj(DoRotate(DcXAxis, DTOR(-90))); 

		DgAddObj(DoPushMatrix());
        		DgAddObj(DoCallback(reflection_callback, DcNullObject));
			DgAddObj(DoScale(5.,5.,.2));
			DgAddObj(DoTranslate(-.5,-.5,-.6));
			DgAddObj(DoDiffuseIntens(.7));
			DgAddObj(DoDiffuseColor(DcRGB,groundcolor));
			DgAddObj(DoPrimSurf(DcBox));
		DgAddObj(DoPopMatrix());

		DgAddObj(DoPushMatrix());
			DgAddObj(DoReflectionSwitch(DcOff));
                	DgAddObj(DoScale(2.,2.,2.));          
			grow_tree(MAX_DEPTH);                  
		DgAddObj(DoPopMatrix());
	DgAddObj(DoPushMatrix());
   return(DgClose());
}


void grow_tree (int level)
{
    DtReal age;
    DtReal color[3];
    static DtReal greenish[] = {0.,1.,.3};
    int i;

    DgAddObj(DoPushMatrix());
    DgAddObj(DoScale(SCALE, SCALE, SCALE));
    switch(level) {
	case 0: /* make a fruit	 */
		age = RAND;
		color[0] = age;
		color[1] = 1.-age;
		color[2] = .2;
		DgAddObj(DoPushMatrix());
		   DgAddObj(DoDiffuseColor(DcRGB, color));
		   DgAddObj(DoSpecularIntens(age));
		   DgAddObj(DoScale(.5*(age+.5), .5*(age+.5), .5*(age+.5)));
		   DgAddObj(DoPrimSurf(DcSphere));
		DgAddObj(DoPopMatrix());
		break;
	case 1: /* maybe make a fruit */
		if(0==(RANDOM()%4)) {	
		    age = RAND;
		    color[0] = age;
		    color[1] = 1.-age;
		    color[2] = .2;
		    DgAddObj(DoPushMatrix());
		        DgAddObj(DoScale(SCALE, SCALE, SCALE));
		        DgAddObj(DoPushMatrix());
		    	    DgAddObj(DoDiffuseColor(DcRGB, color));
		    	    DgAddObj(DoSpecularIntens(age));
		    	    DgAddObj(DoScale(.5*(age+.5), .5*(age+.5), .5*(age+.5)));
		    	    DgAddObj(DoPrimSurf(DcSphere));
		        DgAddObj(DoPopMatrix());
		    DgAddObj(DoPopMatrix());
		break;
		} /* else drop through into default */
	default:  /* make three subtrees  */
		for(i=0; i<3; i++) {
			DgAddObj(DoPushMatrix());
				DgAddObj(DoRotate(DcZAxis,
					DTOR(120*(i+RAND-.5))));
				DgAddObj(DoRotate(DcXAxis, 
					DTOR(35) + (RAND-.5)*DTOR(20) +
					(MAX_DEPTH-level)*DTOR(6)));
				DgAddObj(DoPushMatrix());
					DgAddObj(DoDiffuseColor(DcRGB, greenish));
					DgAddObj(DoScale(.05, .05, 1.));
					DgAddObj(DoPrimSurf(DcCylinder));
				DgAddObj(DoPopMatrix());
				DgAddObj(DoTranslate(0., 0., 1.));
				grow_tree(level-1);
			DgAddObj(DoPopMatrix());
		}
		break;
    }
    DgAddObj(DoPopMatrix());
}



/* ======================================================================== */

DtObject makestudio (void)
{
    static DtPoint3  origin = { 0.0, 0.0, 0.0 }, 
		     camera = { 0.0, 7.0, 9.0 },
		     light  = { 8.0, 8.0, 4.0 };
    static DtVector3 up     = { 0.0, 1.0, 0.0 };

    DoGroup (DcTrue);
	DgAddObj (DoPerspective(35.,-.1,-16.));
	DgAddObj (DoPushMatrix());
		DgAddObj (DoLookAtFrom(origin, camera, up));
		DgAddObj (DoCamera());
	DgAddObj (DoPopMatrix());
	DgAddObj (DoPushMatrix());
		DgAddObj (DoLookAtFrom(origin, light, up));
		DgAddObj (DoLightIntens(1.));
		DgAddObj (DoLightType(DcLightInfinite));
		DgAddObj (DoLight());
	DgAddObj (DoPopMatrix());
    return (DgClose());
}
