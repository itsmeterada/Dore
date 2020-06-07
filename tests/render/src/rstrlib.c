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
#include <dore/dore.h>

/* this file supplies the following two functions:
DtInt write_aif_file (filename, width, height, depth, type, data);
DtInt read_aif_file (filename, width, height, depth, type, data);
*/

typedef enum {
   T_unknown,
   T_rastertype,
   T_width,
   T_height,
   T_depth,
   T_pixel,
   T_image,
   T_integer,
   T_pixel_rgb,
   T_pixel_rgba,
   T_pixel_rgbaz,
   T_pixel_rgbz,
   T_pixel_a,
   T_pixel_z,
   T_pixel_abgr,
   T_wordbyteorder,
   T_big_endian,
   T_little_endian,
} TokenType;

typedef enum {
   LOOK_sep,
   LOOK_comment,
   LOOK_attr,
   LOOK_svalue,
   LOOK_integer,
   LOOK_equal,
   LOOK_comma,
   LOOK_value,
   LOOK_FF,
   ERROR_state,
   PROCESS_attr_val,
} State;

typedef struct {
   char *str;
   TokenType token;
} StrToToken;

typedef struct {
   int rastertype;
   int width;
   int height;
   int depth;
   DtRasterType pixel;
   int bytecount;
   int swapint;
   int has_ints;
   char *data;
} RasterHeader;

typedef enum { 
   Token_value, 
   Integer_value, 
   String_value,
} ValueType;

typedef struct {
   ValueType type;
   TokenType token;
   int ivalue;
   char *svalue;
} ValueStruct;

static StrToToken attr_table[] = {
   "rastertype", 	T_rastertype,
   "width",		T_width,
   "height",		T_height,
   "depth",		T_depth,
   "pixel",		T_pixel,
   "wordbyteorder",	T_wordbyteorder,
   "",			T_unknown,
};
static StrToToken value_table[] = {
   "image",		T_image,
   "r8g8b8",		T_pixel_rgb,
   "r8g8b8a8",		T_pixel_rgba,
   "r8g8b8a8z32",	T_pixel_rgbaz,
   "r8g8b8z32",		T_pixel_rgbz,
   "a8",		T_pixel_a,
   "z32",		T_pixel_z,
   "a8b8g8r8",		T_pixel_abgr,
   "big-endian",	T_big_endian,
   "little-endian",	T_little_endian,
   "",			T_unknown,
};


#define BUFSIZE 1024

static char buffer[BUFSIZE];
static char token[BUFSIZE];
static char *separators 	= "# \t\n\r\f";
static char *attr_separators	= "# \t\n\r=\f";
static char *numbers		= "0123456789";

static char err_msg[BUFSIZE];

#ifdef DEBUG
/* Debugging values
 * 	1 - attribute - values
 * 	2 - strings / numbers as found
 * 	3 - state change info
 * 	4 - io info
 */
int pdebug = 0;
#endif

/*
  ======================================================================
 */
static int bigendian_word_byte_order (void)
{
#define BYTES_TO_CHK 4
   typedef int integer;
   integer i;
   char c[16];
   int BIG_ENDIAN    = 0x3c5aa5c3;
   int LITTLE_ENDIAN = 0xc3a55a3c;

   int mask;
   int j;

   c[0] = 0x3c;
   c[1] = 0x5a;
   c[2] = 0xa5;
   c[3] = 0xc3;
   for (j=BYTES_TO_CHK; j<16; j++) c[j] = 0x0;

   bcopy (c, &i, sizeof(integer));

#ifdef DEBUG
   printf ("sizeof (integer) = %d\n", sizeof(integer));
   printf ("i = 0x%x\n", i);
#endif

   if (sizeof(integer) < BYTES_TO_CHK) {
	BIG_ENDIAN = BIG_ENDIAN >> (BYTES_TO_CHK - sizeof(integer)) * 8; 
	for (j = 0, mask = 0x0; j < (BYTES_TO_CHK - sizeof(integer)); j++) {
		mask = (mask << 8) | 0xff;
	}
	LITTLE_ENDIAN = LITTLE_ENDIAN & mask;
   } else if (sizeof(integer) > BYTES_TO_CHK) {
	BIG_ENDIAN = BIG_ENDIAN << (sizeof(integer) - BYTES_TO_CHK) * 8;
   }
#ifdef DEBUG
   printf ("BIG_ENDIAN    = 0x%x\n", BIG_ENDIAN);
   printf ("LITTLE_ENDIAN = 0x%x\n", LITTLE_ENDIAN);
#endif

   if (i == BIG_ENDIAN) {
      return (DcTrue);
   } else if (i == LITTLE_ENDIAN) {
      return (DcFalse);
   } else {
      return (-1);
   }
}
/*
  ======================================================================
 */
static void error_message (
    char *error,
    char *filename,
    char *msg)
{
   if (msg != (char *)0) {
      fprintf (stderr, "----- %s in routine %s - %s\n", error, filename, msg);
   } else {
      fprintf (stderr, "----- %s in routine %s\n", error, filename);
   }
}
/*
  ======================================================================
 */
static char *token_string (
    TokenType token)
{
   char *rtn;
   static char buf[132];

   switch (token) {
   case T_unknown:	rtn = "<unknown>";	break;
   case T_rastertype:	rtn = "rastertype";	break;
   case T_width:	rtn = "width";		break;
   case T_height:	rtn = "height";		break;
   case T_depth:	rtn = "depth";		break;
   case T_pixel:	rtn = "pixel";		break;
   case T_image:	rtn = "image";		break;
   case T_integer:	rtn = "integer";	break;
   case T_pixel_rgb:	rtn = "r8g8b8";		break;
   case T_pixel_rgba:	rtn = "r8g8b8a8";	break;
   case T_pixel_rgbaz:	rtn = "r8g8b8a8z32";	break;
   case T_pixel_rgbz:	rtn = "r8g8b8z32";	break;
   case T_pixel_a:	rtn = "a8";		break;
   case T_pixel_z:	rtn = "z32";		break;
   case T_pixel_abgr:	rtn = "a8b8g8r8";	break;
   case T_wordbyteorder:rtn = "wordbyteorder";	break;
   case T_big_endian:	rtn = "big-endian";	break;
   case T_little_endian:rtn = "little-endian";	break;
   default:	
      sprintf (buf, "token <%d>", token);
      rtn = buf;
   }
   return rtn;
}
/*
  ======================================================================
 */
static char *state_string (
    State state)
{
   char *rtn;
   static char buf[132];

   switch (state) {
   case LOOK_sep:		rtn = "LOOK_sep";		break;
   case LOOK_comment:		rtn = "LOOK_comment";		break;
   case LOOK_attr:		rtn = "LOOK_attr";		break;
   case LOOK_svalue:		rtn = "LOOK_svalue";		break;
   case LOOK_integer:		rtn = "LOOK_integer";		break;
   case LOOK_equal:		rtn = "LOOK_equal";		break;
   case LOOK_comma:		rtn = "LOOK_comma";		break;
   case LOOK_value:		rtn = "LOOK_value";		break;
   case LOOK_FF:		rtn = "LOOK_FF";		break;
   case ERROR_state:		rtn = "ERROR_state";		break;
   case PROCESS_attr_val:	rtn = "PROCESS_attr_val";	break;
   default:			
      sprintf (buf, "state <%d>", state);
      rtn = buf;
   }
   return rtn;
}
/*
  ======================================================================
 */
static char *value_string (
    ValueStruct value)
{
   char *rtn;
   static char buf[132];

   switch (value.type) {
   case Token_value:
      rtn = token_string (value.token);
      break;
   case Integer_value:
      sprintf (buf, "%d", value.ivalue);
      rtn = buf;
      break;
   case String_value:
      rtn = value.svalue;
      break;
   default:
      sprintf (buf, "Unknown value type <%d>", value.type);
      rtn = buf;
      break;
   }
   return rtn;
}      
/*
  ======================================================================
 */
static isin (
    char c,
    char *s)
{
   for (; *s; s++)
	 if (*s == c)
	       return(1);
   return(0);
}
/*
  ======================================================================
 */
static int process_attr_value (
    TokenType attr,
    int val_cnt,
    ValueStruct *value,
    RasterHeader *header)
{
   int error = 0;
#ifdef DEBUG
   int i;

   if (pdebug > 0) {
      printf ("attribute: %s\n", token_string(attr));
      for (i=0; i<val_cnt; i++) {
	 printf ("\tvalue[%d]: %s\n", i, value_string (value[i]));
      }
      printf ("\n");
   }
#endif
   if (val_cnt < 1) {
      error_message ("value out of range", "read_aif_file",
		     "All attributes requires at least one argument");
      return 1;
   }

   switch (attr) {
   case T_rastertype:
      if (val_cnt > 1) {
	 sprintf (err_msg, "%d extra argument%s to 'rastertype' ignored",
		  val_cnt - 1, val_cnt > 2 ? "s" : "");
	 error_message ("value out of range", "read_aif_file", err_msg);
      }
      if (value[0].type != Token_value) {
	 sprintf (err_msg, "rastertype attribute requires type 'image' not '%s'",
		  value_string(value[0]));
	 error_message ("value out of range", "read_aif_file", err_msg);
	 error ++;
      } else if (value[0].token != T_image) {
	 sprintf (err_msg, "unknown rastertype '%s'", value_string(value[0]));
	 error_message ("value out of range", "read_aif_file", err_msg);
	 error ++;
	 break;
      } else {
	 header->rastertype = 1;
      }
      break;

   case T_width:
      if (val_cnt > 1) {
	 sprintf (err_msg, "%d extra argument%s to 'width' ignored",
		  val_cnt - 1, val_cnt > 2 ? "s" : "");
	 error_message ("value out of range", "read_aif_file", err_msg);
      }
      if (value[0].type != Integer_value) {
	 error_message ("value out of range", "read_aif_file",
			"width attribute requires integer value -- ignored");
      } else {
	 header->width = value[0].ivalue;
      }
      break;

   case T_height:
      if (val_cnt > 1) {
	 sprintf (err_msg, "%d extra argument%s to 'height' ignored",
		  val_cnt - 1, val_cnt > 2 ? "s" : "");
	 error_message ("value out of range", "read_aif_file", err_msg);
      }
      if (value[0].type != Integer_value) {
	 error_message ("value out of range", "read_aif_file",
			"height attribute requires integer value -- ignored");
      } else {
	 header->height = value[0].ivalue;
      }
      break;

   case T_depth:
      if (val_cnt > 1) {
	 sprintf (err_msg, "%d extra argument%s to 'depth' ignored",
		  val_cnt - 1, val_cnt > 2 ? "s" : "");
	 error_message ("value out of range", "read_aif_file", err_msg);
      }
      if (value[0].type != Integer_value) {
	 error_message ("value out of range", "read_aif_file",
			"depth attribute requires integer value -- ignored");
      } else {
	 header->depth = value[0].ivalue;
      }
      break;

   case T_pixel:
      if (val_cnt > 1) {
	 sprintf (err_msg, "%d extra argument%s to 'pixel' ignored",
		  val_cnt - 1, val_cnt > 2 ? "s" : "");
	 error_message ("value out of range", "read_aif_file", err_msg);
      }
      if (value[0].type != Token_value) {
	 sprintf (err_msg,
		  "Unknown pixel type '%s' -- ignored", 
		  value_string(value[0]));
	 error_message ("value out of range", "read_aif_file", err_msg);
      } else {
	 switch (value[0].token) {
	 case T_pixel_rgb:
	    header->pixel = DcRasterRGB;
	    header->bytecount = 3;
	    header->has_ints = 0;
	    break;
	 case T_pixel_rgba:
	    header->pixel = DcRasterRGBA;
	    header->bytecount = 4;
	    header->has_ints = 0;
	    break;
	 case T_pixel_abgr:
	    header->pixel = DcRasterABGR;
	    header->bytecount = 4;
	    header->has_ints = 0;
	    break;
	 case T_pixel_rgbaz:
	    header->pixel = DcRasterRGBAZ;
	    header->bytecount = 8;
	    header->has_ints = 1;
	    break;
	 case T_pixel_rgbz:
	    header->pixel = DcRasterRGBZ;
	    header->bytecount = 7;
	    header->has_ints = 1;
	    break;
	 case T_pixel_a:
	    header->pixel = DcRasterA;
	    header->bytecount = 1;
	    header->has_ints = 0;
	    break;
	 case T_pixel_z:
	    header->pixel = DcRasterZ;
	    header->bytecount = 4;
	    header->has_ints = 1;
	    break;
	 default:
	    sprintf (err_msg,
		     "Unknown pixel type '%s' -- ignored", 
		     token_string(value[0].token));
	    error_message ("value out of range", "read_aif_file", err_msg);
	 }
      }
      break;

   case T_wordbyteorder:
      if (val_cnt > 1) {
	 sprintf (err_msg, 
		  "%d extra argument%s to 'wordbyteorder' ignored\n",
		  val_cnt - 1, val_cnt > 2 ? "s" : "");
	 error_message ("value out of range", "read_aif_file", err_msg);
      }
      if (value[0].type != Token_value) {
	 sprintf (err_msg, 
		  "wordbyteorder requires 'big-endian' or 'little-endian' not '%s'\n", 
		  value_string(value[0]));
	 error_message ("value out of range", "read_aif_file", err_msg);
      }
      switch (value[0].token) {
      case T_big_endian:
	 header->swapint = !bigendian_word_byte_order();
	 break;
      case T_little_endian:
	 header->swapint = bigendian_word_byte_order();
	 break;
      default:
	 sprintf (err_msg, 
		  "Unknown wordbyteorder '%s' -- ignored\n", 
		  token_string(value[0].token));
	 error_message ("value out of range", "read_aif_file", err_msg);
      }
      break;
   }
   if (header->rastertype != 1) {
      error_message ("value out of range", "read_aif_file",
		     "'rastertype' required as first attribute");
      error++;
   }
   return error;
}
/*
  ======================================================================
 */
DtInt read_aif_file (
    char *filename,
    DtInt *width,
    DtInt *height,
    DtInt *depth,
    DtRasterType *type,
    DtPtr *data)
{
   FILE *fp = NULL;
   RasterHeader header;
   ValueStruct value[10];
   int val_num = 0;
   char *cp;
   char *tp;
   char *end_buffer;
   int length;
   int cur_attr = 0;
   int cur_val = 0;
   State state = LOOK_sep;
   State next_state = LOOK_attr;
   char *sep = attr_separators;
   int done = 0;
   int error = 0;
   int attr_pending = 0;
   int bytes;
   int total_bytes;
   int bytes_read;
   int i;

   header.rastertype = -1;
   header.width = -1;
   header.height = -1;
   header.depth = 1;
   header.bytecount = -1;
   header.swapint = !bigendian_word_byte_order();
   header.has_ints = 0;
   header.data = (char *)0;

   if (filename == (char *)0) {
      error_message ("value out of range", "read_aif_file", "NULL filename");
      return -1;
   }

   if ((filename[0] == '\0') ||
       ((fp = fopen (filename, "r")) == NULL)) {
      sprintf (err_msg, "Could not open file '%s'", filename);
      error_message ("io error", "read_aif_file", err_msg);
      return -1;
   }

   while (!done && (length = fread (buffer, sizeof(char), BUFSIZE, fp)) > 0) {
      cp = buffer;
      end_buffer = &(buffer[length]);
      
#ifdef DEBUG
      if (pdebug > 3) 
	    printf ("read next buffer\n");
#endif
      cp = buffer;

      while (cp < end_buffer && !done) {
#ifdef DEBUG
	 if (pdebug > 2) 
	       printf ("state = %s\n", state_string(state));
	 if (pdebug > 3) 
	       printf ("cp = '%s'\n", cp);
#endif

	 switch (state) {
	 case LOOK_sep:
	    while (cp < end_buffer && *cp != '#' && 
		   *cp != '\f' && isin(*cp,sep)) {
	       cp++;
	    }
	    if (cp < end_buffer) {
	       if (*cp == '#') {
		  state = LOOK_comment;
	       } else if (*cp == '\f') {
		  cp++;
		  state = LOOK_FF;
		  if (attr_pending) {
		     if (process_attr_value (attr_table[cur_attr].token, 
					     val_num, value, &header)) {
			/* error with attribute */
			state = ERROR_state;
		     }
		     for (i=0; i<val_num;i++) {
			if (value[i].type == String_value)
			      free(value[i].svalue);
		     }
		     attr_pending = 0;
		  }
	       } else {
		  state = next_state;
		  tp = token;
	       }
	    }
	    break;

	 case LOOK_comment:
	    while (cp < end_buffer && !isin(*cp, "\n\f"))
		  *cp++;
	    if (cp < end_buffer) {
	       state = LOOK_sep;
	    }
	    break;

	 case LOOK_FF:
	    while (cp < end_buffer && *cp != '\f') 
		  *cp++;
	    if (cp < end_buffer) {
	       cp++;
	       done = 1;
	    }
	    break;

	 case LOOK_integer:
	    while (cp < end_buffer && isin(*cp, numbers))
		  *(tp++) = *(cp++);
	    if (cp < end_buffer) {
	       *(tp++) = '\0';
	       sscanf (token, "%d", &value[val_num].ivalue);
	    }
#ifdef DEBUG
	    if (pdebug > 1)
		  printf ("found integer %d\n", value[val_num].ivalue);
#endif
	    value[val_num].type = Integer_value;
	    sep = separators;
	    state = LOOK_sep;
	    next_state = LOOK_comma;
	    val_num++;
	    break;

	 case LOOK_equal:
	    if (*cp == '=') {
	       cp++;
#ifdef DEBUG
	       if (pdebug > 1) 
		     printf ("found '='\n");
#endif
	       state = LOOK_sep;
	       next_state = LOOK_value;
	       val_num = 0;
	    } else {
	       sprintf (err_msg,
			"invalid character after attribute '%c' looking for '='",
			*cp);
	       error_message ("value out of range", "read_aif_file", err_msg);
	       state = ERROR_state;
	    }
	    break;

	 case LOOK_attr:
	    /*
	     * copy buffer characters to token string
	     */
	    while (cp < end_buffer && !isin (*cp, sep)) 
		  *(tp++) = *(cp++);
	    if (cp < end_buffer) {
	       *(tp++) = '\0';
#ifdef DEBUG
	       if (pdebug > 1) 
		     printf ("found attr string '%s'\n", token);
#endif
	       for (cur_attr=0; attr_table[cur_attr].token != T_unknown; cur_attr++) {
		  if (!strcmp(attr_table[cur_attr].str, token)) {
		     break;
		  }
	       }
#ifdef WARN_UNKNOWN
	       if (attr_table[cur_attr].token == T_unknown) {
		  sprintf(err_msg, "Unknown attribute '%s'", 
			  token);
		  error_message("value out of range", "read_aif_file", err_msg);
	       }
#endif
	       sep = separators;
	       state = LOOK_sep;
	       next_state = LOOK_equal;
	    }
	    break;
   
	 case LOOK_svalue:
	    /*
	     * copy buffer characters to token string
	     */
	    while (cp < end_buffer && !isin (*cp, sep)) 
		  *(tp++) = *(cp++);
	    if (cp < end_buffer) {
	       *(tp++) = '\0';
#ifdef DEBUG
	       if (pdebug > 1) 
		     printf ("found value string '%s'\n", token);
#endif
	       for (cur_val=0; value_table[cur_val].token != T_unknown; cur_val++) {
		  if (!strcmp(value_table[cur_val].str, token)) {
		     break;
		  }
	       }
#ifdef DEBUG
	       if (pdebug > 1) 
		     printf ("cur_val = %d; token = '%s'; table = '%s'\n",
			     cur_val, token, value_table[cur_val].str);
#endif
	       if (value_table[cur_val].token == T_unknown) {
#ifdef WARN_UNKNOWN
		  sprintf(err_msg, "Unknown attribute value '%s'", 
			  token);
		  error_message("value out of range", "read_aif_file", err_msg);
#endif
		  value[val_num].type = String_value;
		  value[val_num].svalue = (char *) malloc (strlen(token)+1);
		  strcpy(value[val_num].svalue, token);
	       } else {
		  value[val_num].type = Token_value;
		  value[val_num].token = value_table[cur_val].token;
	       }

	       sep = separators;
	       state = LOOK_sep;
	       next_state = LOOK_comma;
	       val_num++;
	    }
	    break;
	    
	 case LOOK_comma:
	    if (*cp == ',') {
	       cp++;
	       sep = separators;
	       state = LOOK_sep;
	       next_state = LOOK_value;
	    } else {
	       state = PROCESS_attr_val;
	    }
	    break;

	 case LOOK_value:
	    attr_pending = 1;
	    sep = separators;
	    if (isin(*cp, numbers)) {
	       state = LOOK_integer;
	    } else {
	       state = LOOK_svalue;
	    }
	    break;

	 case ERROR_state:
	    error_message ("io error", "read_aif_file",
			   "unable to continue reading header");
	    error ++;
	    goto error_exit;
	    break;

	 case PROCESS_attr_val:
	    state = LOOK_sep;
	    next_state = LOOK_attr;
	    sep = attr_separators;
	    attr_pending = 0;
	    if (process_attr_value (attr_table[cur_attr].token, 
				    val_num, value, &header)) {
	       /* error with attribute */
	       state = ERROR_state;
	    }
	    for (i=0; i<val_num;i++) {
	       if (value[i].type == String_value)
		     free(value[i].svalue);
	    }
	    break;
	 }
      }
   }
   if (header.rastertype != 1) {
      error_message ("value out of range", "read_aif_file",
		     "Missing 'rastertype' attribute");
      error++;
   }
   if (header.width < 1) {
      sprintf (err_msg, "Invalid width %d\n", header.width);
      error_message ("value out of range", "read_aif_file", err_msg);
      error++;
   }

   if (header.height < 1) {
      sprintf (err_msg, "Invalid height %d\n", header.height);
      error_message ("value out of range", "read_aif_file", err_msg);
      error++;
   }

   if (header.depth < 1) {
      sprintf (err_msg, "Invalid depth %d\n", header.depth);
      error_message ("value out of range", "read_aif_file", err_msg);
      error++;
   }

   if (header.bytecount < 0) {
      error_message ("value out of range", "read_aif_file",
		     "Invalid pixel type");
      error ++;
   }

   if (!error) {
      /*
       * read raster data
       */

#ifdef DEBUG
      if (pdebug > 3) {
	 printf ("cp = '%s'\n", cp);
      }
#endif

      total_bytes = header.width * header.height * header.depth *
	    header.bytecount;

#ifdef DEBUG
      if (pdebug > 3) {
	 printf ("total bytes = %d\n", total_bytes);
      }
#endif
      header.data = (char *) malloc (total_bytes * sizeof(char));
      if (header.data == (char *)0) {
	 sprintf (err_msg, 
		  "Unable to allocate %d bytes for Raster Data",
		  total_bytes);
	 error_message ("unable to allocate memory", "read_aif_file",
			err_msg);
	 error++;
      } else {
	 tp = header.data;
	 bytes = end_buffer - cp;
#ifdef DEBUG
	 if (pdebug > 3) {
	    printf ("cp = 0x%x, end_buffer = 0x%x, diff = %d\n", 
		    cp, end_buffer, bytes);
	 }
#endif
	 if (bytes > 0) {
	    if (bytes > total_bytes) 
		  bytes = total_bytes;
	    bcopy (cp , tp, bytes);
	    tp += bytes;
	    bytes_read = bytes;
	    bytes = total_bytes - bytes;
	 } else {
	    bytes_read = 0;
	    bytes = total_bytes;
	 }

	 while ((length = fread (tp, sizeof(char), bytes, fp)) != bytes &&
		length > 0) {
#ifdef DEBUG
	    if (pdebug > 3) {
	       printf ("bytes = %d\nlength = %d\n", bytes, length);
	    }
#endif
	    tp += length;
	    bytes -= length;
	    bytes_read += length;
	 }
	 if (length > 0) bytes_read += length;

	 if (bytes_read != total_bytes) {
	    sprintf (err_msg, 
		     "Read %d bytes of %d bytes for required data", 
		     bytes_read, total_bytes);
	    error_message ("io error", "read_aif_file", err_msg);
	    error++;
	 }
	 if (length < 0) {
	    extern int errno;

	    sprintf (err_msg, "Read failed, system error code %d", 
		     errno);
	    error_message ("io error", "read_aif_file", err_msg);
	    error++;
	 }
	 if (error) {
	    free (header.data);
	    header.data = (char *)0;
	 } else if (header.has_ints && header.swapint) {
	    /*
	     * the machine byte order for integers is different than
	     * the byte order of the rasterfile so the Z values 
	     * must be flipped around.  The internal format of
	     * the data is always the format of the machine.
	     */
	    char tmp;
#ifdef DEBUG
	    if (pdebug > 0) {
	       printf ("swapping Z values\n");
	    }
#endif
	    tp = header.data + header.bytecount - 4;
	    while (tp < header.data + total_bytes) {
	       tmp = tp[0];
	       tp[0] = tp[3];
	       tp[3] = tmp;
	       tmp = tp[1];
	       tp[1] = tp[2];
	       tp[2] = tmp;
	       tp += header.bytecount;
	    }
	 }
      }
   }
 error_exit:
   fclose(fp);
   if (error) {
      return -1;
   } else {
      *width = header.width;
      *height = header.height;
      *depth = header.depth;
      *type = header.pixel;
      *data = header.data;
      return 0;
   }
}
/*
  ======================================================================
 */
DtInt write_aif_file (
    char *filename,
    DtInt width,
    DtInt height,
    DtInt depth,
    DtRasterType type,
    DtPtr data)
{
   FILE *fp;
   char *pixel_type;
   int errors = 0;
   long clock;
   char *ctime();
   char *logname;
   extern char *getenv();
   char *data_ptr;
   int total_bytes;
   int bytes_per_pixel;
   int bytes;
   int bytes_written;
   int length;
   int z_values;

   if (filename == DcNullPtr) {
      error_message ("value out of range", "write_aif_file",
		  "NULL filename");
      return -1;
   }

   switch (type) {
   case DcRasterRGB:
      bytes_per_pixel = 3;
      pixel_type = "r8g8b8";
      z_values = 0;
      break;
   case DcRasterRGBA:
      bytes_per_pixel = 4;
      pixel_type = "r8g8b8a8";
      z_values = 0;
      break;
   case DcRasterABGR:
      bytes_per_pixel = 4;
      pixel_type = "a8b8g8r8";
      z_values = 0;
      break;
   case DcRasterRGBAZ:
      bytes_per_pixel = 8;
      pixel_type = "r8g8b8a8z32";
      z_values = 1;
      break;
   case DcRasterRGBZ:
      bytes_per_pixel = 7;
      pixel_type = "r8g8b8z32";
      z_values = 1;
      break;
   case DcRasterA:
      bytes_per_pixel = 1;
      pixel_type = "a8";
      break;
   case DcRasterZ:
      bytes_per_pixel = 4;
      pixel_type = "z32";
      z_values = 1;
      break;
   default:
      sprintf (err_msg, "data type <%d>", type);
      error_message ("value out of range", "write_aif_file", err_msg);
      return -1;
   }

   total_bytes = width * height * depth *
	 bytes_per_pixel;

   if (total_bytes < 1) {
      sprintf (err_msg, "total byte count (%d)  <= 0", total_bytes);
      error_message ("value out of range", "write_aif_file",
		  err_msg);
      return -1;
   }

   if ((filename[0] == '\0') ||
       ((fp = fopen (filename, "w")) == NULL)) {
      sprintf (err_msg, "Could not open file '%s' for writing", filename);
      error_message ("io error", "write_aif_file", err_msg);
      return -1;
   }

   time (&clock);
   if ((logname = getenv("LOGNAME")) == NULL) 
	 logname = "<UNKNOWN>";

   fprintf (fp, "# write_aif_file: Dore' Rasterfile created by '%s' on %s", 
	       logname, ctime (&clock));
   fprintf (fp, "rastertype = image\n");
   fprintf (fp, "width = %d\n", width);
   fprintf (fp, "height = %d\n", height);
   fprintf (fp, "depth = %d\n", depth);
   fprintf (fp, "pixel = %s\n", pixel_type);
   if (z_values) {
      if (bigendian_word_byte_order()) {
	 fprintf (fp, "wordbyteorder = big-endian\n");
      } else {
	 fprintf (fp, "wordbyteorder = little-endian\n");
      }
   }
   fprintf (fp, "\f\f");
   fflush (fp);

   data_ptr = (char *)data;
   bytes = total_bytes;
   bytes_written = 0;
   while ((length = fwrite (data_ptr, sizeof(char), bytes, fp)) != bytes &&
	  length > 0) {
      data_ptr += length;
      bytes -= length;
      bytes_written += length;
   }
   if (length > 0) bytes_written += length;

   if (bytes_written != total_bytes) {
      sprintf (err_msg, "Wrote %d of %d bytes of data",
	       bytes_written, total_bytes);
      error_message ("io error", "write_aif_file", err_msg);
      errors++;
   }
   if (length < 0) {
      extern int errno;

      sprintf (err_msg, "Write failed, system error code %d", errno);
      error_message ("io error", "write_aif_file", err_msg);
      errors++;
   }

   fclose (fp);

   if (errors) {
      return -1;
   } else {
      return 0;
   }
}
/*
  ======================================================================
 */
