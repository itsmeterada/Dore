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
 
#ifdef DORE_REAL_SINGLE
#define EvilVFcn v3f
#else
#define EvilVFcn v3d
#endif


	/*
	 * The things one has to do to make things fast...
	 *
	 * For those of you who can't stomach macros, just
	 * think of these as functions with var parameters. ;->
	 */


#define EvilLightingFlag ddr_gl_drom_restore_lighting_flag

static int EvilLightingFlag;

/*
 * This is TRUELY an EVIL macro.
 * It will set both norms to NULL if lighting
 * if OFF already.
 */
#define EvilSetupMacro(enorms,vnorms,vcols) {		\
    EvilLightingFlag = 0;				\
							\
    if (!DevStatus.lighting || DevStatus.depcueswitch) {\
	(enorms) = NULL;				\
	(vnorms) = NULL;				\
    } else if ((vnorms)==NULL && (enorms)==NULL) {	\
	EvilLightingFlag = 1;				\
	ddr_gl_drom_set_att_srfshd(DcShaderConstant);	\
    }							\
}

/*
 * Turning on shading will automatically restore
 * the diffuse color
 */
#define EvilFinishMacro(vcols) {				\
    if (EvilLightingFlag)					\
	ddr_gl_drom_set_att_srfshd(DcShaderLightSource);	\
    else if (vcols)						\
	ddr_iris_set_diffuse_color(DevStatus.diffusecolor);	\
}


#define EvilNormalOutputMacro(norm) {	\
    float            evil2[3];		\
    register DtReal *evil3;		\
					\
    evil3 = (norm);			\
					\
    evil2[0] = *(evil3++);		\
    evil2[1] = *(evil3++);		\
    evil2[2] = *(evil3  );		\
					\
    n3f(evil2);				\
}

/*
 * Sets the color and normal on a per-element basis.
 * Remember that if lighting is OFF, the normals
 * will be set to NULL.
 */
#define EvilElementMacro(enorms,ei,vnorms,vcols,vi) {	\
    if (!DevStatus.smoothswitch) {			\
	if (vcols)					\
		ddr_iris_set_diffuse_color((vcols)[vi]);\
	if (enorms)					\
		EvilNormalOutputMacro((enorms)[ei])	\
	else if (vnorms)				\
		EvilNormalOutputMacro((vnorms)[vi])	\
    } else {						\
	if (!(vnorms) && !(vcols) && (enorms))		\
		EvilNormalOutputMacro((enorms)[ei])	\
    }							\
}

/*
 * Sets the color and normal on a per-vertex basis,
 * and outputs the vertex location.
 */
#define EvilVertexMacro(vlocs,vnorms,vcols,vi,enorms,ei) {	\
    register int evil4;						\
								\
    evil4 = (vi);						\
								\
    if (DevStatus.smoothswitch) {				\
	if (vcols)						\
		ddr_iris_set_diffuse_color((vcols)[evil4]);	\
	if (vnorms)						\
		EvilNormalOutputMacro((vnorms)[evil4])		\
	else if ((vcols) && (enorms))				\
		EvilNormalOutputMacro((enorms)[ei])		\
    }								\
    EvilVFcn((vlocs)[evil4]);					\
}

#define EvilPointMacro(vlocs,vnorms,vcols,vi) {		\
    if (vcols)						\
	    ddr_iris_set_diffuse_color((vcols)[vi]);	\
    if (vnorms)						\
	    EvilNormalOutputMacro((vnorms)[vi])		\
    EvilVFcn((vlocs)[vi]);				\
}

