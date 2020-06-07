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
 
/**************************************************************************
				DORE_UTIL.H

This is the header file for the standard Dore utility library.
All constants and function prototypes are defined here.
This file should be included in all source files that use the utilities.
**************************************************************************/

/* transformation types */
#define NoTransf 0
#define TransX   1
#define TransY   2
#define TransZ   3
#define ScaleX   4
#define ScaleY   5
#define ScaleZ   6
#define RotX     7
#define RotY     8
#define RotZ     9
#define Reset   10

/* knob modes */
#define	KnobVirgin	0
#define KnobBlock       1
#define KnobNonBlock    2
#define KnobSig         3

/* Degrees to radians */
#define DTOR(angle) ((angle) * (3.14159265358979 / 180.0))

/* integer or floating point max and min */
#define UTIL_MAX(a, b) ((a) > (b) ? (a) : (b))
#define UTIL_MIN(a, b) ((a) < (b) ? (a) : (b))

/* integer or pointer swap */
#define SWAP(a, b) { (a) ^= (b); (b) ^= (a); (a) ^= (b); }


DtObject copy_object  (DtObject);
DtObject make_definition_group  (void);
void fcn  (void);
int inq_expansion_board  (void);
int pre_211_inq_expansion_board  (void);
void hsv_to_rgb  (DtReal [3], DtReal [3]);
void set_knobs  (DtObject, DtObject, DtObject, DtObject, DtObject, DtObject, DtObject, DtObject);
void SETKNB  (DtObject *, DtObject *, DtObject *, DtObject *, DtObject *, DtObject *, DtObject *, DtObject *);
void disable_knobs  (void);
void DISKNB  (void);
void KNBDIS  (void);
void enable_knobs  (void);
void ENBKNB  (void);
void KNBENB  (void);
void flush_knobs  (void);
void FLSKNB  (void);
void start_knobs  (int, int, int);
void STKNB  (int *, int *, int *);
void INIKNB  (void);
int poll_knobs  (void);
int PLLKNB  (void);
void view_preview  (DtObject, DtInt, DtInt []);
int RedefineCamera  (DtObject, DtReal);
int RedefineLight  (DtObject, DtReal);
int InList  (DtInt, DtInt, DtInt []);
DtFlag prsarg_get_keyword_int  (int, char *[], char *, DtInt, DtInt *);
DtFlag prsarg_get_keyword_real  (int, char *[], char *, DtReal, DtReal *);
DtFlag prsarg_get_keyword_short  (int, char *[], char *, DtShort, DtShort *);
DtFlag prsarg_get_keyword_string  (int, char *[], char *, char *, char *);
DtFlag prsarg_get_keyword_switch  (int, char *[], char *, DtSwitch, DtSwitch *);
DtFlag prsarg_check_bad_arguments  (int, char *[]);
DtObject rect_mesh  (DtColorModel, DtVertexType, DtInt, DtInt, DtReal [], DtFlag);
DtObject RCTMSH  (DtInt *, DtInt *, DtInt *, DtInt *, DtReal [], DtInt *);
DtInt replace_type_with  (DtObject, DtInt, DtObject);
DtObject stacked_group  (DtFlag, DtFlag);
DtObject close_stacked_group  (void);
void open_stacked_group  (DtObject);
DtObject super_mesh  (DtColorModel, DtVertexType, DtInt, DtReal [], DtInt, DtInt [], DtInt [], DtShapeType, DtFlag, DtReal [], DtFlag);
void fcn  (void);
int inq_expansion_board  (void);
int pre_211_inq_expansion_board  (void);
DtObject subscript  (char *);
int get_text  (char *, char *, int);
DtObject make_text  (char *);
DtInt make_transformer  (DtCompType);
DtObject change_transformer  (DtInt, DtInt, DtReal, DtCompType, DtFlag);
void get_transformer_matrix  (DtInt, DtMatrix4x4);
void put_transformer_matrix  (DtInt, DtMatrix4x4);
int concatenate  (DtMatrix4x4, DtMatrix4x4, DtCompType);
