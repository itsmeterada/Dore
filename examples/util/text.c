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
				TEXT.C

	This module contains some useful text handling utilities.
	Visible functions are:

	subscript() -- creates Dore text objects with sub/super-scripts.
	make_text() -- creates Dore text objects in paragraph form.
	get_text() -- reads text from a file sutable for use with make_text().
*/



#include <dore/dore.h>
#include "dore_util.h"
#include <stdio.h>


#define TAB_WIDTH 8
#define STRBUFSIZ 1024
#define TEXT         1
#define SUBSCRIPT    2
#define SUPERSCRIPT  3
#define ERROR       -1



/*
Takes a string with embedded subscripts or superscripts and returns a
Dore text object (actually a group) with the strings and sub/super scripts.
A superscript is specified within a string with the circumflex character '^'.
The superscript string is then expected to follow between parentheses.
Subscript strings are given between parentheses following a comma character.
For example Einstein's equation is generated with the string "E = MC^(2)"
while the formula for water can be generated with the string "H,(2)O".
Multiple sub/super scripts can be specified in one string but they 
do not nest.  Also, trailing blanks after strings will be ignored.  In other
words, the string "H   ,(2)   O" will give the same results as "H,(2)O".
*/

DtObject subscript (
    char *str)
{
	char buf[STRBUFSIZ];
	int  text_type, firsttime=1;
	DtObject grp, text_obj;
	DtVolume vol;
	static DtPoint3 origin = {0.,0.,0.};
	static DtVector3 u = {1.,0.,0.}, v = {0.,1.,0.};

	grp = DoGroup(DcTrue);
		DgAddObj(DoTextAlign(DcTextHAlignNormal, DcTextVAlignNormal));
		DgAddObj(DoTextExpFactor(1.0));
		DgAddObj(DoTextFont(DcHelvetica));
		DgAddObj(DoTextHeight(1.0));
		DgAddObj(DoTextPath(DcTextPathRight));
		DgAddObj(DoTextPrecision(DcStrokePrecision));
		DgAddObj(DoTextSpace(0.0));
		DgAddObj(DoTextUpVector(0.0, 1.0));

	while(*str != '\0') {
		text_type = gettok(str, buf);
		printf("buf = %s\n", buf);
		text_obj = DoText(origin, u, v, buf);
		if(!firsttime) /* how big before adding next text object? */
			DsCompBoundingVol(&vol, grp);
		DgAddObj(DoPushMatrix());
			if(!firsttime)
				DgAddObj(DoTranslate(vol.fur[0], 0.0, 0.0));
			switch(text_type) {
				case TEXT:
					str += strlen(buf);
					break;
				case SUBSCRIPT:
					str += strlen(buf) + 3;
					DgAddObj(DoTranslate(0.0, -.75, 0.0));
					break;
				case SUPERSCRIPT:
					str += strlen(buf) + 3;
					DgAddObj(DoTranslate(0.0,  .75, 0.0));
					break;
				case ERROR:
					printf("subscript: couldn't parse %s\n",
						buf);
					return(DgClose());
			}
			DgAddObj(text_obj);
		DgAddObj(DoPopMatrix());
		firsttime = 0;
	}

	return(DgClose());
}



static int gettok (
    char *str,
    char into[])
{
	int i=0;

	switch(*str) {
		case '^':
			str++;
			if('(' != *str)
				return(ERROR);
			str++;  /* read past the '(' character */
			while(*str != ')') {
				if('\0' == *str)
					return(ERROR);
				into[i++] = *str++;
			}
			into[i++] = '\0';
			return(SUPERSCRIPT);
		case ',':
			str++;
			if('(' != *str)
				return(ERROR);
			str++;  /* read past the '(' character */
			while(*str != ')') {
				if('\0' == *str)
					return(ERROR);
				into[i++] = *str++;
			}
			into[i++] = '\0';
			return(SUBSCRIPT);
		default:
			while(*str != '\0' && *str != '^' && *str != ',') {
				into[i++] = *str++;
			}
			into[i++] = '\0';
			return(TEXT);
	}
}


/*
Reads a string of characters from a named file into a buffer.
This buffer can subsequently be read by make_text().
*/

int get_text (
    char *FileName,
    char *Buf,
    int BufSize)
{
     register int i, Count=0;
     FILE *fp, *fopen();

     if(NULL == (fp = fopen(FileName, "r"))) 
	 return(0);
     while(((Buf[Count]=fgetc(fp))!=EOF)&&(Count<BufSize-1)) 
	 if(Buf[Count] == '\t')
	     for(i=0; (i<TAB_WIDTH) && (Count<BufSize-1); i++)
		 Buf[Count++] = ' ';
	 else Count++; 
     Buf[Count] = '\0';
     return(Count);
} /* end get_text */





/*
Takes a character string (array) and returns a group object containing
Dore text objects made from the string (see DoText).
Embedded newline characters (newline = "\n") in the string cause
the text to be broken into lines stacked one above the other.
*/

DtObject make_text (
    char *Text)
{
	register int i, Length;
	static DtPoint3 Pos = {0.,0.,0.};
	static DtVector3 U = {1.,0.,0.}, V = {0.,1.,0.}; 

	Length = strlen(Text);
	DoGroup(DcTrue);
		for(i=0; i<Length; i++)
			if(Text[i] == '\n') 
		Text[i] = '\0';
		for(i=0; i<Length-1; i+=(strlen(Text+i)+1))
			{
			DgAddObj(DoText(Pos, U, V, Text+i));
			Pos[1] -= 1.5;
			}
		Pos[1] = 0.;
	return(DgClose());
} /* end make_text */
