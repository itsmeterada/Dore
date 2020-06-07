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
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/priatt.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/studio.h>
#include <dore/dore_develop/private/device.h>
#include <dore/dore_develop/private/patchmat.h>

extern DtInt doe_system_execution_depth;
extern DtInt doe_system_edit_source;

extern DtFlag doe_system_bndvol_first_report;
extern DtReal
	doe_system_bndvol_min_x,
	doe_system_bndvol_max_x,
	doe_system_bndvol_min_y,
	doe_system_bndvol_max_y,
	doe_system_bndvol_min_z,
	doe_system_bndvol_max_z;

extern DtPtr doe_system_version_string;


    /***  Local Function Prototypes  ***/

void  dor_system_initialize_texture_mapping_callbacks  (void);
DtInt dor_system_inq_edit_source  (DtSwitch);
