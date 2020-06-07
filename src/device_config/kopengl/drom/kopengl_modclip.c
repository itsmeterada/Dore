 
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
 
/*****************************************************************************

  Functions:
    ddr_kopengl_drom_set_att_clpswi 
    ddr_kopengl_drom_apply_att_clpvol 
    ddr_kopengl_drom_push_att_clpvol 
    ddr_kopengl_drom_pop_att_clpvol 
    ddr_kopengl_drom_set_att_clpvol

 *****************************************************************************/


#include "drom.h"

/*****************************************************************************
 * Modelling Clip Switch                                                     *
 *****************************************************************************/

void
ddr_kopengl_drom_set_att_clpswi (DtSwitch switchvalue)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_clpswi\n");
#endif

 fprintf (stdout, "drom.set_att_clpswi not yet implemented\n");
}



/*****************************************************************************
 *  Modelling Clip Volume                                                    *
 *****************************************************************************/

void
ddr_kopengl_drom_apply_att_clpvol (DtClipOperator  operator,
                                   DtInt           halfspacecount,
                                   DtHalfSpace    *halfspaces)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.apply_att_clpvol\n");
#endif

 fprintf (stdout, "drom.set_att_clpvol not yet implemented\n");
}



/*****************************************************************************
 * Push Modelling Clip Volume                                                * 
 *****************************************************************************/

void
ddr_kopengl_drom_push_att_clpvol (void)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.push_att_clpvol\n");
#endif

 fprintf (stdout, "drom.push_clpvol not yet implemented\n");
}



/*****************************************************************************
 * Pop Modelling Clip Volume                                                 * 
 *****************************************************************************/

void
ddr_kopengl_drom_pop_att_clpvol (void)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.pop_att_clpvol\n");
#endif

 fprintf (stdout, "drom.pop_clpvol not yet implemented\n");
}

/*****************************************************************************
 *  Modelling Clipping Volume                                                * 
 *****************************************************************************/

void
ddr_kopengl_drom_set_att_clpvol (DtClipOperator  operator, 
                                 DtInt           halfspacecount, 
                                 DtHalfSpace    *halfspaces)
{
#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.set_att_clpvol\n");
#endif

 fprintf (stdout, "drom.set_att_clpvol not yet implemented\n");
}
