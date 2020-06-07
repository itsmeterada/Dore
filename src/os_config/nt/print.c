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
 
/*
  ======================================================================
  Functions:
	dor_print_initialize
	dor_print_output
	dor_print_get_line
	dor_print_clear
	dor_print_indent_down
	dor_print_indent_up
	dor_print_set_indent
	dor_print_set_field

  ======================================================================
 */
#include <stdio.h>
#include <string.h>
#include <dore/internal/dogen.h>

int doe_print_indent_value;
struct stack *doe_print_indent_stack;

/*
 ======================================================================
 */

void dor_print_initialize (void)
{
    dor_print_set_indent (0);
}
/*
 ======================================================================
 */

void dor_print_output (
    char *line)
{
    int charno;

    for (charno=1;charno<=doe_print_indent_value;charno++)
   	printf(" ");

    printf("%s\n",line);
}
/*
 ======================================================================
 */

char *dor_print_get_line (void)
{
    static char print_line[200] = "<print_line>";

    return print_line;
}
/*
 ======================================================================
 */

void dor_print_clear (
    char *line,
    int linelength)
{
    int charno;

    for (charno=0;charno<linelength;charno++)
	line[charno] = ' ';

    line[linelength] = '\0';
}
/*
 ======================================================================
 */

void dor_print_indent_down (void)
{
    doe_print_indent_value -= 5;
}
/*
 ======================================================================
 */

void dor_print_indent_up (void)
{
    doe_print_indent_value += 5;
}
/*
 ======================================================================
 */

void dor_print_set_indent (
    int newindent)
{
    doe_print_indent_value = newindent;
}
/*
 ======================================================================
 */

void dor_print_set_field (
    char *line,
    int fieldbegin,
    char *field)
{
    int fieldlength;

    fieldlength = strlen(field);

    strncpy(&(line[fieldbegin]),field,fieldlength);
}
