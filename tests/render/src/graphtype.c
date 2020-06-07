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
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <machine/tigr.h>
#include <fcntl.h>
#include <errno.h>
#include <machine/cpu.h>
#include <machine/gr_board.h>

#ifndef TGIOGCTYPE
#define TGIOGCTYPE 44
#endif

static char graphic_type[200];

char *get_graphic_type (void)
{
   struct utsname name;
   extern char *sys_errlist[];
   extern int sys_nerr;

   uname(&name);
   if (!strcmp(name.machine, "3000") ||
       !strcmp(name.machine, "1500") ||
       !strcmp(name.machine, "titan")) { 
      int k;
      char *p, *cp;
      char buf[20];
      int fd;
      int slen;
      char iotype;

      for (k=1; k<10; k++) {
	 p = "/dev/tigrX";
	 slen = 9;

	 for (cp=buf; *cp++=*p++; );
	 cp = buf;
      
	 cp[slen] = k + '0';
	 if ((fd = open (cp, O_WRONLY)) >= 0) {
	    break;
	 } else if (EBUSY==errno) {
	    continue;
	 } else {
	    fprintf (stderr, "open of %s failed: (%d) %s\n",
		     cp, errno, 
		     errno < sys_nerr ? sys_errlist[errno] : "");
	    return NULL;
	 }
      }
      if ((iotype = ioctl(fd, TGIOGCTYPE, 0)) < 0) {
	 if (EINVAL == errno) {
	    /* old device driver -- G2 */
	    iotype = 0x0;
	 } else {
	    fprintf (stderr,
		     "ioctl (TGIOGCTYPE) failed: (%d) %s\n",
		     errno, errno < sys_nerr ? sys_errlist[errno] : "");
	    return NULL;
	 }
      }

      close(fd);

      if (0x0 == iotype) { /* G2 board */
	 sprintf (graphic_type, "G2");
      } else if (0x1 == iotype) { /* G3 board */
	 sprintf (graphic_type, "G3");
      } else if (0x2 == iotype) { /* G4 board */
	 sprintf (graphic_type, "G4");
      }
   } else {
      sprintf (graphic_type, "%s", name.machine);
   }
   return graphic_type;
}
