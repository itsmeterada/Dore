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
    This program is a filter that converts a Dore rasterfile image to the
    Sun standard rasterfile format.  Once you have a Sun rasterfile image 
    it can be displayed using the tools provided by Sun.
    ====================================================================== */

#include <dore/dore.h>
#include <stdio.h>
#include <sys/types.h>
#include <pixrect/pixrect_hs.h>

int filefd;

main (argc, argv)
int argc;
char *argv[];
{
   char 		tmpstring[200];
   char 		inputfile[200];
   char 		outputfile[200];
   FILE			*fp;
   DtVisualType 	visualtype;
   DtShadeMode 		shademode;
   int 			width, height, depth;
   int			byteSkip;
   DtRasterType		rastertype; 
   DtPtr	 	*pixels;
   Pixrect 		*pr = NULL;
   u_char 		*loc;
   DtInt 		index;
   int 			i,j;
   colormap_t 		cmap;
   DtReal 		compressionWeight[3];
   DtInt 		mask[3];
   DtReal 		intensityWeight[3];
   DtReal 		rangeLimit[3];
   unsigned char	*rptr, *gptr, *bptr;

   prsarg_get_keyword_string(argc,argv,"-visualtype","DcPseudoColor",tmpstring);
   if (strcmp(tmpstring, "DcPseudoColor") == 0) {
      visualtype = DcPseudoColor;
   } else {
      fprintf (stderr, "%s: does not support DcDirectColor\n", argv[0]);
      exit (1);
   }

   prsarg_get_keyword_string(argc,argv,"-shademode","DcComponent",tmpstring);
   if (strcmp(tmpstring, "DcComponent") == 0) {
      shademode = DcComponent;
   } else if (strcmp (tmpstring, "DcRange") == 0) {
      shademode = DcRange;
   } else {
      fprintf (stderr, "%s: bad shade mode: '%s'\n", argv[0], tmpstring);
      exit (1);
   }

   prsarg_get_keyword_string(argc,argv,"-if","test.img", inputfile);
   prsarg_get_keyword_string(argc,argv,"-of","sun.img", outputfile);

   prsarg_check_bad_arguments(argc,argv);

   setup_colormap (visualtype, shademode, &cmap);

   if (read_aif_file(inputfile, &width, &height, &depth, &rastertype,
		&pixels) == -1) {
	fprintf (stderr, "%s: error reading Dore' image\n", argv[0]);
	exit(1);
   }

   /* Create a width*height Sun memory pixrect (8 bits/pixel). */
   pr = mem_create(width, height, 8);

   switch (rastertype) {
      case DcRasterRGB:	
	 byteSkip = 3; break;
      case DcRasterABGR:	
	 byteSkip = 4; break;
      case DcRasterRGBA:	
	 byteSkip = 4; break;
      case DcRasterRGBAZ:	
	 byteSkip = 8; break;
      case DcRasterRGBZ:	
	 byteSkip = 7; break;
      case DcRasterA:	
	 fprintf (stderr, "%s: cannot convert DcRasterA raster\n", argv[0]); 
	 exit(1);
      case DcRasterZ:	
	 fprintf (stderr, "%s: cannot convert DcRasterZ raster\n", argv[0]); 
	 exit(1);
      case DcRasterSpecial:	
	 fprintf (stderr, "%s: cannot convert DcRasterSpecial raster\n", argv[0]); 
	 exit(1);
      default:	
	 fprintf (stderr, "%s: cannot convert %d raster\n", argv[0], rastertype);
	 exit(1);
   }

   switch (shademode) {
      case DcComponent:
         /* Establish:
	  * 1. weight factors to convert a 3 byte RGB value into a 
	  *    3-3-2 weighted 1 byte pseudocolor value.
	  * 2. masking factors to extract the 3-3-2 compressed values. 
	  */
         compressionWeight[0] = 256. / 256.;
         compressionWeight[1] = 32.  / 256.;
         compressionWeight[2] = 4.   / 256.;
   
         mask[0] = 0xe0;
         mask[1] = 0x1c;
         mask[2] = 0x03;

	 if (rastertype == DcRasterABGR) {
	    rptr = ((unsigned char *)pixels) + 3;
	    gptr = ((unsigned char *)pixels) + 2;
	    bptr = ((unsigned char *)pixels) + 1;
	 } else {
	    rptr = ((unsigned char *)pixels) + 0;
	    gptr = ((unsigned char *)pixels) + 1;
	    bptr = ((unsigned char *)pixels) + 2;
	 }
   
	 for (i=0; i<height; i++) {
	    for (j=0; j<width; j++) {
	       /* Return a pointer to a byte in the pixrect; then, using
		  the current RGB byte set from the Dore raster image, 
		  compute the pseudocolor value and stuff it into the 
		  pixrect at this location. */

	       loc = mprd8_addr(mpr_d(pr), j, i, 8);

	       index = ((int)(compressionWeight[0]*(*rptr)) & mask[0]) + 
		       ((int)(compressionWeight[1]*(*gptr)) & mask[1]) +
		       ((int)(compressionWeight[2]*(*bptr)) & mask[2]);
	       index = index < 0 ? 0 : index > 255 ? 255 : index;
	       *loc = (u_char)index;
	       rptr += byteSkip;	
	       gptr += byteSkip;	
	       bptr += byteSkip;	
	    }
	 }
	 break;
      case DcRange:
         /* 
          * Establish:
          * 1. weight factors for converting an RGB value to a gray scale 
          *    value.  These three values must sum to 1.
          * 2. the limits of the shade range and the scale factor. 
          */
         intensityWeight[0] = .30;
         intensityWeight[1] = .59;
         intensityWeight[2] = .11;
   
         rangeLimit[0] = 0;
         rangeLimit[1] = 255;
         rangeLimit[2] = (rangeLimit[1]-rangeLimit[0])/255.;

	 if (rastertype == DcRasterABGR) {
	    rptr = ((unsigned char *)pixels) + 3;
	    gptr = ((unsigned char *)pixels) + 2;
	    bptr = ((unsigned char *)pixels) + 1;
	 } else {
	    rptr = ((unsigned char *)pixels) + 0;
	    gptr = ((unsigned char *)pixels) + 1;
	    bptr = ((unsigned char *)pixels) + 2;
	 }

	 for (i=0; i<height; i++) {
	    for (j=0; j<width; j++) {
	       /* Return a pointer to a byte in the pixrect; then, using
		  the current RGB byte set from the Dore raster image, 
		  compute the scaled range value and stuff it into the 
		  pixrect at this location. */

	       loc = mprd8_addr(mpr_d(pr), j, i, 8);
	       index = rangeLimit[0] + 
				(intensityWeight[0]*(*rptr) +
		   	 	 intensityWeight[1]*(*gptr) +
		         	 intensityWeight[2]*(*bptr)) * rangeLimit[2];
	       if (index > rangeLimit[1]) 
		  index = rangeLimit[1];
	       index = index < 0 ? 0 : index > 255 ? 255 : index;
	       *loc = (u_char)index;
	       rptr += byteSkip;	
	       gptr += byteSkip;	
	       bptr += byteSkip;	
	    }
	 }
	 break;
   }

   close(0);

   /* Dump the pixrect image to a file. */

   if ( ! (fp = fopen(outputfile, "w")) != NULL)
      fprintf (stderr, "%s: cannot access output file %s\n",argv[0],outputfile);
   else
   {  if (PIX_ERR == (pr_dump(pr, fp, &cmap, RT_STANDARD, 0)))
         fprintf (stderr,
	     "%s:  Error from pr_dump() while writing output file.\n", argv[0]);
      fclose (fp);
   }
}

/*
  ======================================================================
 */

setup_colormap (visualtype, shademode, cmap)
DtVisualType visualtype;
DtShadeMode shademode;
colormap_t *cmap;
{
   DtInt i, j;
   static unsigned char r[256], g[256], b[256];

   if (visualtype == DcPseudoColor) {
      cmap->type = RMT_EQUAL_RGB;
      cmap->length = 256;
      cmap->map[0] = r;
      cmap->map[1] = g;
      cmap->map[2] = b;

      switch (shademode) {
      case DcComponent:
	 for (i=0; i<256; i++) {
	    r[i] = (unsigned char) ((DtReal)(i>>5) / 7. * 255.);
	    g[i] = (unsigned char) ((DtReal)((i>>2) & 0x07) / 7. * 255.);
	    b[i] = (unsigned char) ((DtReal)(i & 0x03) / 3. * 255.);
	 }
	 break;

      case DcRange:
	 for (i=0; i<256; i++) {
	    r[i] = i;
	    g[i] = i;
	    b[i] = i;
	 }
	 break;
      }
   }
}
