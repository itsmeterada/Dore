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
    X11 "xwud" format.  Once you have an xwud image it can be displayed
    using X11 tools.
    ====================================================================== */

#include <dore/dore.h>
#include <stdio.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XWDFile.h>

/*
  ======================================================================
 */
_swaplong (bp, n)
register char *bp;
register unsigned n;
{
    register char c;
    register char *ep = bp + n;
    register char *sp;

    while (bp < ep) {
	sp = bp + 3;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	sp = bp + 1;
	c = *sp;
	*sp = *bp;
	*bp++ = c;
	bp += 2;
    }
}
/*
  ======================================================================
 */
main (argc, argv)
int argc;
char *argv[];
{
   char                 inputfile[200];
   char                 outputfile[200];
   FILE 		*outfp;
   unsigned long	swaptest = 1;
   char			*win_name = "Dore";
   unsigned 		win_name_size;
   int 			width, height, depth;
   DtRasterType 	rastertype;
   DtPtr 		data;
   unsigned char 	*image;
   unsigned char 	*imageptr;
   int 			scanline, p;
   unsigned 		buffer_size;
   XWDFileHeader 	header;
   int 			byte_skip;
   unsigned char 	*r_ptr, *g_ptr, *b_ptr;

   prsarg_get_keyword_string(argc,argv,"-if","test.img", inputfile);
   prsarg_get_keyword_string(argc,argv,"-of","xwud.img", outputfile);

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
   r_ptr = ((unsigned char *)data);
   g_ptr = ((unsigned char *)data) + 1;
   b_ptr = ((unsigned char *)data) + 2;

   win_name_size = strlen(win_name) + sizeof(char);

   buffer_size = width*height*4;
   image = (unsigned char *)malloc (buffer_size*sizeof (unsigned char));
   if (image == NULL) {
      fprintf (stderr, "%s: could not allocate image space\n",
	       argv[0]);
   }

   header.header_size 		= (xwdval) sizeof(header) + win_name_size;
   header.file_version 		= (xwdval) XWD_FILE_VERSION;
   header.pixmap_width		= (xwdval) width;
   header.pixmap_height		= (xwdval) height;
   header.xoffset		= (xwdval) 0;
   header.byte_order		= (xwdval) MSBFirst;
   header.bitmap_unit		= (xwdval) 32;
   header.bitmap_bit_order	= (xwdval) MSBFirst;
   header.bitmap_pad		= (xwdval) 32;
   header.window_width		= (xwdval) width;
   header.window_height		= (xwdval) height;
   header.window_x		= (xwdval) 0;
   header.window_y		= (xwdval) 0;
   header.window_bdrwidth	= (xwdval) 0;

   header.pixmap_format		= (xwdval) ZPixmap;
   header.pixmap_depth		= (xwdval) 24;
   header.bits_per_pixel	= (xwdval) 32;
   header.bytes_per_line	= (xwdval) 4 * width;
   header.visual_class		= (xwdval) DirectColor;
   header.red_mask		= (xwdval) 0xff0000;
   header.green_mask		= (xwdval) 0x00ff00;
   header.blue_mask		= (xwdval) 0x0000ff;
   header.bits_per_rgb		= (xwdval) 24;
   header.colormap_entries	= (xwdval) 256;
   header.ncolors		= (xwdval) 0;

   imageptr = image;

   for (scanline = 1; scanline <= height; scanline++) {
      for ( p = width; p-- > 0; ) {
	 *imageptr++ = 0;
	 *imageptr++ = *r_ptr;
	 *imageptr++ = *g_ptr;
	 *imageptr++ = *b_ptr;
	 
	 r_ptr += byte_skip;
	 g_ptr += byte_skip;
	 b_ptr += byte_skip;
      }
   }

   if (*(char *) &swaptest) {
      _swaplong((char *) &header, sizeof(header));
   }

   if ((outfp = fopen (outputfile, "w")) != NULL) {
      fwrite((char*)&header, sizeof(header), 1, outfp);
      fwrite(win_name, win_name_size, 1, outfp);

      /* write out the buffer */
   
      fwrite(image, buffer_size, 1, outfp);
   } else {
      fprintf(stderr,"%s: cannot access output file %s\n", argv[0], outputfile);
   }
}
/*
  ======================================================================
 */
