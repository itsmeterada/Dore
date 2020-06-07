/* vi:set ts=8 sw=4 ai:
 *[
 *$     mlrd_image.c
 *
 *      Steve Gray - Tue Dec  5 12:39:44 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	render image file routines
 *
 *      Usage:
 *		mlrd_image_read()
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */


/* #define	DEBUG */

/* system includes */
#include <stdio.h>

/* dore includes */
#include "dore.h"

/* Render includes */
#include "primitives.h"

/* MLRD Dore interface includes */
#include "global.h"
#include "method.h"



/*** Magic Number ***/
# define	ART_MAGIC_NUMBER	(0xdeb88bed)

/*** primary header ***/
typedef struct ARThdr1_s {
	int	magic_number;
	int	file_type;
	int	user_space;
} ART_HEADER_1;


/** define number of types supported **/
# define ART_NUM_TYPES		(3)


/*** secondary header  type 1 ***/
typedef struct ARThdr21_s {
	int	width;
	int	height;
} ART_HEADER_2_1;


/*** secondary header  type 2 ***/
typedef struct ARThdr22_s {
	int	width;
	int	height;
} ART_HEADER_2_2;


/*** secondary header  type 3 ***/
typedef struct ARThdr23_s {
	int	width;
	int	height;
} ART_HEADER_2_3;


/*** generic image structure ***/
typedef	struct image_s	{
	int		type;
	int		width;
	int		height;
	unsigned char	*image;
	unsigned char	*red;
	unsigned char	*green;
	unsigned char	*blue;
	unsigned char	*matte;
	unsigned char	*red_colormap;
	unsigned char	*green_colormap;
	unsigned char	*blue_colormap;
} GENERIC_IMAGE;



/*[
 *$     mlrd_image_read
 *
 *      Steve Gray - Tue Dec  5 12:41:57 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    read image back to Dore screen
 *
 *      Usage:
 *		mlrd_image_read( image, close )
 *		FILE	*image		(in)  file to read
 *		int	(*close)()	(in)  function to close image
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

mlrd_image_read( image, close)
FILE	*image;
int 	(*close)();
{
static Int		old_buffer_len = -1;
static GENERIC_IMAGE	im;
static unsigned Char	*xdata_buf;
Int			n;
ART_HEADER_1		hdr1;
ART_HEADER_2_1		hdr2;
Int			scanline;
unsigned Char		*scanline_ptr;
unsigned Char		*r_ptr;
unsigned Char		*g_ptr;
unsigned Char		*b_ptr;
unsigned Int		buffer_len;
Short			encode_lth;
unsigned Char		encode_buf[8000];
Int			first_time;
unsigned Char		*out_r;
unsigned Char		*out_g;
unsigned Char		*out_b;



#ifdef	DEBUG
printf( "mlrd_image_read\n");
#endif

/** read header **/
    if ( my_fread ((char *)&hdr1, sizeof (ART_HEADER_1), 1, image) == 0 )
    {
	(*close)( image);
	DDerror( ERR_IO_ERROR, "mlrd_image_read", 
		    "error reading image header");
	return;
    }

#ifdef DEBUG
   printf ("hdr1.magic_number = 0x%x\n", hdr1.magic_number);
   printf ("hdr1.file_type    = %d\n", hdr1.file_type);
   printf ("hdr1.user_space   = %d\n", hdr1.user_space);
#endif

/** check magic number **/
    if ( hdr1.magic_number != ART_MAGIC_NUMBER ) 
    {
       post_error ((char *)&hdr1, sizeof(ART_HEADER_1), image);
	(*close)( image);
	DDerror( ERR_IO_ERROR, "mlrd_image_read", 
		    "bad image format (magic number)");
	return;
    }


/** check file type **/
    if ( (hdr1.file_type != 11) && (hdr1.file_type != 13) ) 
    {
       post_error ((char *)&hdr1, sizeof(ART_HEADER_1), image);
	(*close)( image);
	DDerror( ERR_IO_ERROR, "mlrd_image_read", "bad image format (type)");
	return;
    }


/** skip user space if any **/
    for ( n = hdr1.user_space; n > 0; n-- )
	    (void) fgetc ( image );


/** read the secondary header **/
    if ( my_fread ((char *)&hdr2, sizeof (ART_HEADER_2_1), 1, image) == 0 )
    {
	(*close)( image);
	DDerror( ERR_IO_ERROR, "mlrd_image_read", 
		    "read reading secondary header");
	return;
    }


/** check image dimensions **/
    if (hdr2.width <= 0 || hdr2.height <= 0) 
    {
       post_error ((char *)&hdr2, sizeof(ART_HEADER_2_1), image);
	(*close)( image);
	DDerror( ERR_IO_ERROR, "mlrd_image_read", 
		    "bad image format (width/height)");
	return;
    }


/** allocate an image structure to read the data into **/
    im.width = hdr2.width;
    im.height = hdr2.height;
    buffer_len = hdr2.width;

#ifdef	DEBUG
printf( "mlrd_image_read, %d %d, type %d, buffer len %d\n", im.width, im.height,
	hdr1.file_type, buffer_len);
#endif

    if ( old_buffer_len == -1 )
    {
#ifdef	DEBUG
printf( "alloc for first time\n");
#endif

        im.red = (unsigned char *) malloc ( buffer_len );
        im.green = (unsigned char *) malloc ( buffer_len );
        im.blue = (unsigned char *) malloc ( buffer_len );

        xdata_buf = (unsigned char *) malloc( buffer_len * 3);

	old_buffer_len = buffer_len;
    }
    else if ( old_buffer_len < buffer_len )
    {
#ifdef	DEBUG
printf( "realloc from %d\n", old_buffer_len);
#endif

        im.red = (unsigned char *) realloc ( im.red, buffer_len );
        im.green = (unsigned char *) realloc ( im.green, buffer_len );
        im.blue = (unsigned char *) realloc ( im.blue, buffer_len );

        xdata_buf = (unsigned char *) realloc( xdata_buf, buffer_len * 3);

	old_buffer_len = buffer_len;
    }

/* get those scan lines */
    for ( scanline = hdr2.height-1; scanline >= 0; scanline-- )
    {
#ifdef	DEBUG
printf( "read line %d\n", scanline);
#endif

	if ( my_fread( &encode_lth, sizeof(short), 1, image) == 0 )
	{
	    (*close)( image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"premature end of file");
	    return;
	}

	if ( encode_lth > (2 * hdr2.width) )
	{
	   post_error ((char *)&encode_lth, sizeof(short), image);
            (*close) (image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"bad image format (1)");
	    return;
	}

#ifdef	DEBUG
printf( "decode %d bytes of red\n", encode_lth); fflush( stdout);
#endif

	if ( my_fread( encode_buf, sizeof(char), encode_lth, image) == 0 )
	{
	    (*close)( image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"premature end of file");
	    return;
	}
	decode_scan( encode_buf, im.red, encode_lth, hdr2.width);

	if ( my_fread( &encode_lth, sizeof(short), 1, image) == 0 )
	{
	    (*close)( image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"premature end of file");
	    return;
	}

	if ( encode_lth > (2 * hdr2.width) )
	{
	   post_error ((char *)&encode_lth, sizeof(short), image);
            (*close) (image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"bad image format (2)");
	    return;
	}


#ifdef	DEBUG
printf( "decode %d bytes of green\n", encode_lth); fflush( stdout);
#endif

	if ( my_fread( encode_buf, sizeof(char), encode_lth, image) == 0 )
	{
	    (*close)( image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"premature end of file");
	    return;
	}

	decode_scan( encode_buf, im.green, encode_lth, hdr2.width);

	if ( my_fread( &encode_lth, sizeof(short), 1, image) == 0 )
	{
	    (*close)( image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"premature end of file");
	    return;
	}

	if ( encode_lth > (2 * hdr2.width) )
	{
	   post_error ((char *)&encode_lth, sizeof(short), image);
            (*close) (image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"bad image format (3)");
	    return;
	}


#ifdef	DEBUG
printf( "decode %d bytes of blue\n", encode_lth); fflush( stdout);
#endif

	if ( my_fread( encode_buf, sizeof(char), encode_lth, image) == 0 )
	{
	    (*close)( image);
	    DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			"premature end of file");
	    return;
	}

	decode_scan( encode_buf, im.blue, encode_lth, hdr2.width);

	if ( hdr1.file_type == 13 )
	{
	    if ( my_fread( &encode_lth, sizeof(short), 1, image) == 0 )
	    {
	        (*close)( image);
	        DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			    "premature end of file");
		return;
	    }

	    if ( encode_lth > (2 * hdr2.width) )
	    {
	       post_error ((char *)&encode_lth, sizeof(short), image);
                (*close)( image);
	        DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			    "bad image format (4)");
		return;
	    }

#ifdef	DEBUG
printf( "skip %d bytes of matte\n", encode_lth); fflush( stdout);
#endif


	    if ( my_fread( encode_buf, sizeof(char), encode_lth, image) == 0 )
	    {
	        (*close)( image);
	        DDerror( ERR_IO_ERROR, "mlrd_image_read", 
			    "premature end of file");
		return;
	    }
	}

	r_ptr = im.red;
	g_ptr = im.green;
	b_ptr = im.blue;

	out_r = xdata_buf;
	out_g = xdata_buf + 1;
	out_b = xdata_buf + 2;

	for ( n = hdr2.width; n-- > 0; )
	{
	    *out_r = *(r_ptr++);
	    *out_g = *(g_ptr++);
	    *out_b = *(b_ptr++);

	    out_r += 3;
	    out_g += 3;
	    out_b += 3;
	}

#ifdef	DEBUG
printf( "write line %d\n", scanline);
#endif

	(*mlr_control.PROM_fcns->write_rectangle_byte_rgb)
		(0, scanline, hdr2.width, 1, xdata_buf);
    }
}


/*[
 *$     decode_scan
 *
 *      Steve Gray - Tue Dec  5 12:51:27 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    decode an ART RLE scan
 *
 *      Usage:
 *		decode_scan( src, dest, src_len, dest_len)
 *		unsigned char	*src		(in)  encoded buffer
 *		unsigned char	*dest		(out) decoded buffer
 *		int		src_len		(in)  source length
 *		int		dest_len	(in)  max output length
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

decode_scan( src, dest,  src_len, dest_len )
unsigned char *src;   /* encoded buffer */
unsigned char *dest;  /* destination buffer for decoded scan */
int src_len;          /* length of encoded buffer */
int dest_len;         /* max length of decoded buffer */
{
register unsigned char *s;    /* source pointer */
register unsigned char *d;    /* destination pointer */
register unsigned char  v;    /* current run value */
register unsigned char  r;    /* current run length */
unsigned char      *s_end;    /* end of source buffer */
unsigned char      *d_end;    /* end of destination buffer */

    s = src;
    d = dest;
    s_end = s+src_len;
    d_end = d+dest_len;

    while( s < s_end && d < d_end ){
	r = *s;
	if( r & 0x80 ){
	    /* this is a discrete run */
	    r &= 0x7f;
	    for( ++s; r > 0 && d < d_end; ++s, ++d, --r )
		*d = *s;
	}
	else{
	    /* this is a compressed run */
	    v = *(s+1);
	    s += 2;
	    for( ; r > 0 && d < d_end; ++d, --r )
		*d = v;
	}
    }
}


/*[
 *$     my_fread
 *
 *      Daniel R. McLachlan - Thu Feb 15 15:23:48 PST 1990
 *      Stardent Comuter Inc,
 *?
 *      Description:    read from the image file
 *
 *      Usage:
 *		my_fread( ptr, size, nitems, stream)
 *		void		*ptr	(in)  where to read to
 *		unsigned int 	size	(in)  item size
 *		int		nitmes	(in)  number of items to read
 *		FILE		*stream	(in)  file to read
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

static int
my_fread (ptr, size, nitems, stream)
     void *ptr;
     unsigned int size;
     int nitems;
     FILE *stream;
{
   char *p;
   int left;
   int rd;
   int rd_total;

   p = (char *)ptr;
   left = size * nitems;
   rd_total = 0;

   while (!feof(stream) && !ferror(stream) && left > 0) {
      rd = fread (p, sizeof(char), left, stream);
      rd_total += rd;
      left -= rd;
      p += rd;
   }

   if (rd_total != size*nitems) 
      fprintf (stderr, "fread error: got %d of %d\n", rd_total, size*nitems);
    

#ifdef DEBUG
   printf ("fread got %d of %d\n", rd_total, size*nitems);
#endif

   return rd_total;
}

/*[
 *$     post_error
 *
 *      Daniel R. McLachlan - Thu Feb 15 15:23:48 PST 1990
 *      Stardent Comuter Inc,
 *?
 *      Description:    print out error info after invalid image file read
 *
 *      Usage:
 *		post_error( ptr, size, stream)
 *		void		*ptr	(in)  data already read
 *		unsigned int	size	(in)  size of data
 *		FILE		*stream	(in)  image file
 *
 *      Notes:  
 *		only prints data if "DEBUG" is defined
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

post_error (ptr, size, stream)
     void *ptr;
     unsigned size;
     FILE *stream;
{
   char buffer[1024];
   int count;
   char *p1, *p2;
   int i;
   char out[30];

   p1 = (char *)ptr;
   p2 = buffer;
   for (i=0; i<size; i++){
      *p2++ = *p1++;
   }

#ifdef	DEBUG
   my_fread (p2, 1, 1023 - size, stream);

   fprintf (stderr, "Dumping data from stream after error condition\n");

   for (i=1; i<=1024; i++) {
      fprintf (stderr, "%02x", buffer[i]);
      if (i%2 == 0) fprintf (stderr, " ");
      if (buffer[i-1] >= 0x20 && buffer[i-1] <= 0x7e) {
	 out[i%20] = buffer[i-1];
      } else {
	 out[i%20] = ' ';
      }
      if (i%20 == 0) {
	 out[24] = '\0';
	 fprintf (stderr, "%s\n", out);
      }
   }
   if (i%20 != 0) {
      out[i%20] = '\0';
      fprintf (stderr, " %s\n", out);
   }
#endif	DEBUG
}


