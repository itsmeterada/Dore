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
    This program is a filter that converts a Dore rasterfile image to an
    SGI Iris image.  Once you have an Iris image it can be displayed
    using the tools provided by SGI.
    ====================================================================== */

#include <dore/dore.h>
#include <stdio.h>
#include "image.h"

main (argc, argv)
int argc;
char *argv[];
{
   char 		inputfile[200];
   char 		outputfile[200];
   int 			width, height, depth;
   DtRasterType 	rastertype;
   DtPtr 		data;
   IMAGE 		*image;
   int 			i, j;
   int 			byte_skip;
   unsigned char 	*rptr, *gptr, *bptr;
   unsigned short 	*rdata, *gdata, *bdata;

   prsarg_get_keyword_string(argc,argv,"-if","test.img", inputfile);
   prsarg_get_keyword_string(argc,argv,"-of","iris.img", outputfile);

   prsarg_check_bad_arguments(argc,argv);

   if (read_aif_file(inputfile, &width, &height, &depth, 
			&rastertype, &data) == -1) {
      fprintf (stderr, "%s: error reading Dore' image\n", argv[0]);
      exit (1);
   }

   switch (rastertype) {
      case DcRasterRGB:	
	 byte_skip = 3;	break;
      case DcRasterRGBA:	
	 byte_skip = 4;	break;
      case DcRasterRGBAZ:	
	 byte_skip = 8;	break;
      case DcRasterRGBZ:	
	 byte_skip = 7;	break;
      case DcRasterA:
         fprintf (stderr, 
	    "%s: Cannot Convert raster of type DcRasterA\n", argv[0]);
         exit(1);
      case DcRasterZ:
         fprintf (stderr, 
	    "%s: Cannot Convert raster of type DcRasterZ\n", argv[0]);
         exit(1);
      case DcRasterSpecial:
         fprintf (stderr, 
	    "%s: Cannot Convert raster of type DcRasterSpecial\n", argv[0]);
         exit(1);
      default:
         fprintf (stderr, 
	    "%s: Cannot Convert raster of type %d\n", argv[0], rastertype);
         exit(1);
   }

   image = iopen(outputfile,"w",RLE(1),3,width,height,3);
   if (!image) {
      fprintf (stderr, "%s: cannot open output file %s\n", argv[0], outputfile);
      exit(1);
   }

   if (rastertype == DcRasterABGR) {
      rptr = ((unsigned char *)data) + 3;
      gptr = ((unsigned char *)data) + 2;
      bptr = ((unsigned char *)data) + 1;
   } else {
      rptr = ((unsigned char *)data);
      gptr = ((unsigned char *)data) + 1;
      bptr = ((unsigned char *)data) + 2;
   }

   /* Allocate space for row of Rs, Gs, and Bs. */
   if ((rdata = (unsigned short *)malloc(width*sizeof(unsigned short *))) 
		== NULL) {
      fprintf (stderr, "%s: cannot allocate necessary temporary space\n",
	  argv[0]);
      exit(1);
   }
   if ((gdata = (unsigned short *)malloc(width*sizeof(unsigned short *))) 
		== NULL) {
      fprintf (stderr, "%s: cannot allocate necessary temporary space\n",
	  argv[0]);
      exit(1);
   }
   if ((bdata = (unsigned short *)malloc(width*sizeof(unsigned short *))) 
		== NULL) {
      fprintf (stderr, "%s: cannot allocate necessary temporary space\n",
	  argv[0]);
      exit(1);
   }

   for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
	 *(rdata+j) = (unsigned short)*rptr;
	 *(gdata+j) = (unsigned short)*gptr;
	 *(bdata+j) = (unsigned short)*bptr;
	 
	 rptr += byte_skip;
	 gptr += byte_skip;
	 bptr += byte_skip;
      }
      /* Dore raster image origin is upper left corner versus Iris
	 origin of lower left corner. */
      putrow(image, rdata, height-(i+1), 0);
      putrow(image, gdata, height-(i+1), 1);
      putrow(image, bdata, height-(i+1), 2);
   }

   iclose(image);
}
