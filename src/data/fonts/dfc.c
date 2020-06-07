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
 
/****
*
*	Dore Font Compiler
*
****/

#include <stdio.h>
#include <sys/types.h>

#define MAX_GLYPHS      256
#define MAX_POLYLINES 20000
#define MAX_VERTICES 100000
#define HFONT_MAGIC 2242
#define HFONT_VERSION 1
#define HFONT_TYPE_VECTOR  0
#define HFONT_TYPE_POLYGONAL 1

char in_buff[128] ;
char errmess[200] ;
FILE *in_file ;
char *in_file_name ;
int vertex[MAX_VERTICES][2] ;
int next_vertex ;
int polyline[MAX_POLYLINES] ;
int next_polyline ;
int glyph[MAX_GLYPHS] ;
int character[MAX_GLYPHS] ;
int next_glyph ;
int x_left[MAX_GLYPHS], x_right[MAX_GLYPHS] ;
int y_bottom[MAX_GLYPHS], y_top[MAX_GLYPHS] ;
int topline ;
int bottomline ;
int character_max_x, character_min_x, character_max_y, character_min_y ;
int max_char_width ; /* width of widest character */
int first_vertex ;
int is_first_char ;

int font_format ;
int first_char ;
int last_char ;
int numchar ;
int baseline ;
int capline ;

/*
  ======================================================================
 */
error(routine_name, message)
     char *routine_name ;
     char *message ;
{
   fprintf(stderr,"error detected in '%s': '%s'\n",
	   routine_name, message) ;
   exit(1) ;
} 
/*
  ======================================================================
 */
parse_error(routine_name, message)
     char *routine_name ;
     char *message ;
{
   fprintf(stderr,"error detected in file '%s' in module '%s': '%s'\n",
	   in_file_name, routine_name, message) ;
   exit(1) ;
} 
/*
  ======================================================================
 */
output_font(filename)
     char *filename ;
{
   static char *proc_name = "output_font" ;
   FILE *fontfile ;
   int char_id, i, j, k, npolylines, npts;
   int current_polyline, current_point ;

   if ((fontfile = fopen(filename, "w+")) == NULL ) {
      sprintf(errmess, "Can't open font file '%s'\n",filename) ;
      error(proc_name, errmess ) ;
   }

   /* output magic number */

   putw(HFONT_MAGIC, fontfile) ;

   /* font format version number */

   putw(HFONT_VERSION, fontfile) ;

   /* font type */

   putw(font_format, fontfile) ;

   /* number of charcters in file */

   putw(last_char-first_char+1, fontfile) ;

   /* first character in sequence */

   putw(first_char, fontfile) ;

   /* character width */

   putw(max_char_width+1, fontfile) ;

   /* baseline */

   putw(baseline, fontfile) ;

   /* capline */

   putw(capline, fontfile) ;

   /* topline */

   putw(character_max_y, fontfile) ;

   /* bottom line */

   putw(character_min_y, fontfile) ;

   /* number of polylines */

   putw(next_polyline-1, fontfile) ;

   /* number of points */

   putw(next_vertex, fontfile) ;

   /* output character information */
 
   for ( i = first_char ; i <= last_char ; i++ ) { /* for each character */

      char_id = character[i] ;

      if ( char_id == -1 ) {
	 /* output null character */

	 putw(0, fontfile) ;
	 putw(0, fontfile) ;
	 continue ;
      }

      /* output character width */

      putw(x_right[i]-x_left[i]+1, fontfile) ;

      /* output number of segments in character */

      npolylines = glyph[char_id+1]-glyph[char_id] ;
      current_polyline = glyph[char_id] ;
      putw(npolylines, fontfile) ;

      for ( j = 0 ; j < npolylines ; j++) { /* for each polyline */
	 npts = polyline[current_polyline+1]-polyline[current_polyline] ;
	 current_point = polyline[current_polyline] ;
	 putw(npts, fontfile);

	 for ( k = 0 ; k < npts ; k++) { /* for each point in polyline */
	    putw(vertex[current_point][0]-x_left[i], fontfile) ;
	    putw(vertex[current_point][1], fontfile) ;
	    current_point++ ;
	 } /* each point */
		
	 current_polyline++ ;
      }/* each segment */
   } /* each character */

   fclose(fontfile) ;
}
/*
  ======================================================================
 */
new_font()
{
   int i ;
   first_vertex = 1 ;
   is_first_char = 1 ;
   next_vertex = 0 ;
   next_polyline = 0 ;
   next_glyph = 0 ;
   first_char = -1 ;
   last_char = -1 ;
   max_char_width = 0 ;

   for ( i = 0 ; i < MAX_GLYPHS ; i++ ) {
      character[i] = -1 ;
   }
}
/*
  ======================================================================
 */
new_character()
{
   static char *proc_name = "new_character" ;

   if ( next_glyph == MAX_GLYPHS )
	 error(proc_name, "too many characters") ;

   glyph[next_glyph] = next_polyline ;
	
   return next_glyph++ ;
}
/*
  ======================================================================
 */
new_polyline()
{
   static char *proc_name = "new_polyline" ;

   if ( next_polyline == MAX_POLYLINES )
	 error(proc_name, "too many polylines") ;

   polyline[next_polyline] = next_vertex ;

   return next_polyline++ ;
}
/*
  ======================================================================
 */
new_vertex(x, y) 
     int x, y ;
{
   static char *proc_name = "new_vertex" ;

   if ( next_vertex == MAX_VERTICES )
	 error(proc_name, "too many vertices") ;

   vertex[next_vertex][0] = x ;
   vertex[next_vertex][1] = y ;

   if ( first_vertex || ( x > character_max_x ))
	 character_max_x = x ;

   if ( first_vertex || ( x < character_min_x ))
	 character_min_x = x ;

   if ( first_vertex || ( y > character_max_y ))
	 character_max_y = y ;

   if ( first_vertex || ( y < character_min_y ))
	 character_min_y = y ;

   first_vertex = 0 ;
   return next_vertex++ ;
}
/*
  ======================================================================
 */
void
parse_file(file_name)
     char *file_name ;
{
   int char_id, i, j, this_font_format, npolyline ;
   int npts, width,  x, y ;
   char *proc_name = "parse_file" ;
   char ff[10];

   in_file_name = file_name ;

   if ( ( in_file = fopen(file_name, "r") ) == NULL ) {
      parse_error(proc_name, "could not open file") ;
      return ;
   }

   /* each character separated by form feed */
   for (fscanf(in_file, "%s", ff); 
	!feof(in_file);
	fscanf(in_file, "%s", ff)) {

      if (ff[0] != 'X') {
	 sprintf (errmess, "did not find 'X' separation character");
	 parse_error(proc_name, errmess );
	 break;
      }

      /* read character code */

      fscanf(in_file, "%d", &char_id) ;
      
      if ( character[char_id] == -1 ) {
	    character[char_id] = new_character() ;
      } else {
	 sprintf(errmess, "attempt to redefine character %d", char_id) ;
	 parse_error(proc_name, errmess ) ;
      }

      fscanf(in_file, "%d", &this_font_format) ;

      if ( is_first_char ) {
	 if (( this_font_format != HFONT_TYPE_VECTOR ) &&
	     ( this_font_format != HFONT_TYPE_POLYGONAL)) {
	    sprintf(errmess, "unknown font format %d at char %d", 
		    this_font_format, char_id) ;
	    parse_error(proc_name, errmess ) ;
	 }
 
	 font_format = this_font_format ;
	 first_char = char_id ;
	 last_char = char_id ;
      } else {
	 if ( font_format != this_font_format ) {
	    sprintf(errmess, "different font formats %d %d at char %d", 
		    font_format, this_font_format, char_id) ;
	    parse_error(proc_name, errmess ) ;
	 }

	 if ( char_id < first_char ) {
	    first_char = char_id ;
	 }
			
	 if ( char_id > last_char ) {
	    last_char = char_id ;
	 }
      }

      fscanf(in_file, "%d %d %d %d", 
	     &x_left[char_id], &y_bottom[char_id], 
	     &x_right[char_id], &y_top[char_id]);

      width = x_right[char_id]-x_left[char_id] ;

      max_char_width = ( width > max_char_width )?width:max_char_width ;

      if ( is_first_char ) {
	 capline = y_top[char_id] ;
	 baseline = y_bottom[char_id] ;
      } else {
	 if ( y_top[char_id] != capline ) {
	    sprintf(errmess, "different caplines %d %d at char %d",
		    capline, y_bottom[char_id], char_id) ;
	    parse_error(proc_name, errmess ) ;
	 }

	 if ( y_bottom[char_id] != baseline ) {
	    sprintf(errmess, "different baselines %d %d at char %d",
		    baseline, y_bottom[char_id], char_id) ;
	    parse_error(proc_name, errmess ) ;
	 }
      }

      fscanf(in_file, "%d", &npolyline ) ;

      /* read polylines */

      for ( i = 0 ; i < npolyline ; i++ ) {
	 new_polyline() ;
	 fscanf(in_file, "%d", &npts ) ;

	 for ( j = 0 ; j < npts ; j++ ) {
	    fscanf(in_file, "%d %d", &x, &y) ;
	    new_vertex(x, y) ;
	 }
      }
      is_first_char = 0 ;
   }
   fclose(in_file) ;
}
/*
  ======================================================================
  compile each character definition in directory
 */
compile_font(data_file, out_file)
     char *data_file ;
     char *out_file ;
{
   new_font() ;
   printf("compiling font %s from %s\n", out_file, data_file) ;

   parse_file(data_file) ; 

   /* add mythical character and polyline to specify number of vertices in 
      last polyline of last character */

   new_character() ;
   new_polyline() ;

   printf("compiled: first character = %d, last character = %d\n",
	  first_char, last_char) ;

   output_font(out_file) ;
}
/*
  ======================================================================
 */
main(argc, argv)
     int argc ;
     char **argv ;
{
   if ( argc < 3 ) {
      sprintf(errmess, "usage: %s font_data_file font_output_file", argv[0]) ;
      error(argv[0], errmess) ;
   }

   compile_font(argv[1], argv[2]) ;
   return(0);
}
