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

#ifndef DORE_H
#define DORE_H


    /**************************************/
    /***  Machine Specific Definitions  ***/
    /**************************************/

	/* The following definitions are specific to each machine.  The
	** DORE_REAL_DEFAULT_DOUBLE macro is defined if the default floating-
	** point type for that machine is double precision, otherwise the
	** default floating-point type will be single precision.  */

#if defined(titan)
#   define DORE_REAL_DEFAULT_DOUBLE	/* Default Precision is Double */
    typedef          int  DtInt;	/* Must be at least 32 bits. */
    typedef unsigned int  DtUInt;	/* Must be at least 32 bits. */
    typedef unsigned int  Dt32Bits;	/* Must be  exactly 32 bits. */

#elif defined(sgi)
    typedef          int  DtInt;	/* Must be at least 32 bits. */
    typedef unsigned int  DtUInt;	/* Must be at least 32 bits. */
    typedef unsigned int  Dt32Bits;	/* Must be  exactly 32 bits. */

#elif defined(i860)
    typedef          int  DtInt;	/* Must be at least 32 bits. */
    typedef unsigned int  DtUInt;	/* Must be at least 32 bits. */
    typedef unsigned int  Dt32Bits;	/* Must be  exactly 32 bits. */

#elif defined(ultrix)
    typedef          int  DtInt;	/* Must be at least 32 bits. */
    typedef unsigned int  DtUInt;	/* Must be at least 32 bits. */
    typedef unsigned int  Dt32Bits;	/* Must be  exactly 32 bits. */

#elif defined(__alpha)
    typedef          int  DtInt;	/* Must be at least 32 bits. */
    typedef unsigned int  DtUInt;	/* Must be at least 32 bits. */
    typedef unsigned int  Dt32Bits;	/* Must be  exactly 32 bits. */

#else
    typedef          int  DtInt;	/* Must be at least 32 bits. */
    typedef unsigned int  DtUInt;	/* Must be at least 32 bits. */
    typedef unsigned int  Dt32Bits;	/* Must be  exactly 32 bits. */

#endif



    /* Dore is set up to allow DtReal (floating point numbers used by Dore) to
    ** be defined as either double precision or single precision.  
    **
    ** If DORE_REAL_SINGLE is defined then DtReal will be float.  
    ** If DORE_REAL_DOUBLE is defined then DtReal will be double.
    **
    ** The default is double precision for the following machines:
    ** 	   Stardent 1500   (Titan P2G2)
    **	   Stardent 3000   (Titan P3G2)
    **	   Stardent 3000VS (Titan P3G3)
    **
    ** In all other cases, the default is single precision.  */

#if !defined(DORE_REAL_SINGLE) && !defined(DORE_REAL_DOUBLE)
#   if defined DORE_REAL_DEFAULT_DOUBLE
#       define DORE_REAL_DOUBLE
#       undef  DORE_REAL_SINGLE
#   else
#       define DORE_REAL_SINGLE
#       undef  DORE_REAL_DOUBLE
#   endif
#endif

#if defined(DORE_REAL_SINGLE)
    typedef float DtReal;
#   define DORE_REAL_PRECISION 1
#else
    typedef double DtReal;
#   define DORE_REAL_PRECISION 2
#endif

#ifdef WIN32 
#  define M_LOG2E    1.4426950408889634074E0  /*Hex  2^ 0 * 1.71547652B82FE */
#  define M_LOG10E   4.3429448190325182765E-1 /*Hex  2^-2 * 1.BCB7B1526E50E */
#  define M_LN2      6.9314718055994530942E-1 /*Hex  2^-1 * 1.62E42FEFA39EF */
#  define M_LN10     2.3025850929940456840E0  /*Hex  2^ 1 * 1.26bb1bbb55516 */
#  define M_PI       3.1415926535897932385E0  /*Hex  2^ 1 * 1.921FB54442D18 */
#  define M_PI_2     1.5707963267948966192E0  /*Hex  2^ 0 * 1.921FB54442D18 */
#  define M_PI_4     7.8539816339744830962E-1 /*Hex  2^-1 * 1.921FB54442D18 */
#  define M_1_PI     3.1830988618379067154E-1 /*Hex  2^-2 * 1.45f306dc9c883 */
#  define M_2_PI     6.3661977236758134308E-1 /*Hex  2^-1 * 1.45f306dc9c883 */
#  define M_2_SQRTPI 1.1283791670955125739E0  /*Hex  2^ 0 * 1.20dd750429b6d */
#  define M_SQRT2    1.4142135623730950488E0  /*Hex  2^ 0 * 1.6A09E667F3BCD */
#  define M_SQRT1_2  7.0710678118654752440E-1 /*Hex  2^-1 * 1.6a09e667f3bcd */
#endif

#ifdef __STDC__
/* define bcopy to be memove */
#define bcopy(a,b,c)      memmove(b,a,c)
#endif
  
    /* for compatibility with versions that required dore_proto.h */

#define DORE_PROTO_H
#define DORE_FCN_PROTOTYPES

    /* The following macro flags the fact that this version of Dore' is
    ** safe with respect to ints, longs and pointers, i.e., it makes no
    ** assumptions about any of these being the same size. */

#define DORE_SAFE 1


    /********************************/
    /***  Dore' Type Definitions  ***/
    /********************************/

typedef void          *DtPtr;	   /* Pointer to Anything */
typedef DtInt        (*DtPFI)();   /* Pointer to  int-Returning Function */
typedef void         (*DtPFV)();   /* Pointer to void-Returning Function */
typedef short          DtShort;
typedef unsigned short DtUShort;
typedef unsigned char  DtUChar;

typedef DtReal DtRealCouple[2];	/*  Couple of reals. */
typedef DtReal DtRealTriple[3];	/*  Triple of reals.  */
typedef DtReal DtRealQuad[4];	/*  Quadruplet of reals.  */
typedef DtInt  DtIntTriple[3];	/*  Triple of ints.  */

typedef DtReal DtPoint3[3];
typedef DtReal DtPoint4[4];
typedef struct { DtReal x,y;     } DtNPoint2;
typedef struct { DtReal x,y,z;   } DtNPoint3;
typedef struct { DtReal x,y,z,w; } DtNPoint4;

typedef DtReal DtVector3[3];
typedef DtReal DtVector4[4];
typedef struct { DtReal dx,dy,dz; } DtNVector3;

typedef DtReal DtMatrix4x4[4][4];
typedef DtReal DtMatrix3x3[3][3];

typedef DtReal DtColorRGB[3];
typedef struct { DtReal r,g,b; } DtNColor3;

typedef struct {
    DtReal ll[2];
    DtReal ur[2];
} DtArea;

typedef struct {
    DtNPoint2 ll;
    DtNPoint2 ur;
} DtNArea;

typedef struct {
    DtPoint3 bll;
    DtPoint3 fur;
} DtVolume;

typedef struct {
    DtReal width;
    DtReal height;
    DtReal depth;
} DtSize3;

typedef struct {
    DtPoint3  point;
    DtVector3 vector;
} DtHalfSpace;

typedef struct {
    DtReal LowerBound;
    DtReal UpperBound;
} DtInterval;

typedef struct {
    DtInt    depth;
    DtShort  width, height;
    DtUChar *bits;
} DtPattern;

typedef struct object {
    DtShort info;
    DtShort ref_count;
    DtPtr   data;		/* object's private data */
    DtPtr  *additional_data;
} DtObjectStructure;		/* only used with user-defined objects */

typedef DtObjectStructure *DtObject;


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

    /* Booleans & Miscellaneous Types */

typedef enum {		/* bsd: was DtInt with defines */
    DcFalse = 0,
    DcTrue
} DtFlag;

typedef enum {		/* bsd: was DtInt with defines */
    DcOff = 0,
    DcOn
} DtSwitch;

typedef enum {
    DcErrorMinor,
    DcErrorSevere,
    DcErrorFatal
} DtErrorStatus;

typedef enum {
    DcReadOk,
    DcReadTrunc,
    DcReadUnsuc
} DtReadStatus;

typedef enum {
    DcGlobalCallNone,
    DcGlobalCallPush,
    DcGlobalCallPop,
    DcGlobalCallGetValue,
    DcGlobalCallSetValue
} DtGlobalCall;

typedef enum { DcRGB }	DtColorModel;

typedef enum {
    DcStaticGrey,
    DcGreyScale,
    DcStaticColor,
    DcPseudoColor,
    DcTrueColor,
    DcDirectColor
} DtVisualType;

typedef enum {
    DcRange,
    DcComponent
} DtShadeMode;

typedef enum {
    DcRasterRGB	    =  0,
    DcRasterRGBA    =  1,
    DcRasterRGBAZ   =  2,
    DcRasterRGBZ    =  3,
    DcRasterA	    =  4,
    DcRasterZ	    =  5,
    DcRasterABGR    =  6,
    DcRasterSpecial = 99
} DtRasterType;

typedef enum {
    DcConstantShade,
    DcVertexShade,
    DcSurfaceShade
} DtInterpType;

typedef enum {
    DcUpdateAll,
    DcUpdateDisplay
} DtUpdateType;

typedef enum {
    DcParallel,
    DcPerspective
} DtProjectionType;

typedef enum {
    DcPoints,
    DcWireframe,
    DcSurface,
    DcOutlines,
    DcSurfaceWireframeEdges,
    DcSurfaceOutlineEdges
} DtRepType;

typedef enum {
    DcLineTypeSolid,
    DcLineTypeDash,
    DcLineTypeDot,
    DcLineTypeDotDash
} DtLineType;

typedef enum {
    DcRendererNewId  = -1,
    DcRealTime       =  0,
    DcProductionTime =  2
} DtRenderStyle;

typedef enum {
    DcCameraArbitrary,
    DcCameraParallel,
    DcCameraPerspective,
    DcCameraProjection
} DtCameraMatrixType;

typedef enum {
    DcLocalAntiAliasFast,
    DcLocalAntiAliasIntermediate,
    DcLocalAntiAliasBest
} DtLocalAntiAliasStyle;

typedef enum {
    DcMapReplace,
    DcMapAdd
} DtMapOperator;

typedef enum {
    DcTextureAANone         = 0,
    DcTextureAAMIPPoint     = 1,
    DcTextureAAAdaptive     = 2,
    DcTextureAABilinear     = 3,
    DcTextureAASumArea      = 4,
    DcTextureAATrilinear    = 5,
    DcTextureAAMIP2Point    = 6,
    DcTextureAAMIPBilinear  = 7,
    DcTextureAAMIP2Bilinear = 8,

    /* Old Style Constants  */

    DcTextureAntiAliasNone     = 0,
    DcTextureAntiAliasMIP      = 1,
    DcTextureAntiAliasAdaptive = 2,
    DcTextureAntiAliasBilinear = 3,
    DcTextureAntiAliasSumArea  = 4

} DtTextureAntiAliasMode;

typedef enum {
    DcExtendNone,
    DcExtendBlack,
    DcExtendClamp,
    DcExtendRepeat
} DtExtendMode;

typedef enum {
    DcTextureReplace,
    DcTextureMultiply,
    DcTextureBlend,
    DcTextureAdd
} DtTextureOperator;

typedef enum {
    DcAngleOldStyle,
    DcAngleRadians,
    DcAngleDegrees
} DtAngleUnits;

typedef enum {
    DcXAxis,
    DcYAxis,
    DcZAxis
} DtAxis;

typedef enum {
    DcXY,
    DcYZ,
    DcXZ
} DtMajorPlane;

typedef enum {
    DcPreConcatenate,
    DcPostConcatenate,
    DcReplace
} DtCompType;

typedef enum {
    DcConvex,
    DcConcave,
    DcComplex
} DtShapeType;

typedef enum {
    DcCtr,
    DcCtrClr
} DtCtrlPointType;

typedef enum {
    DcObjCentroid,
    DcISN
} Dt2PartProjType;

typedef enum {
    DcSphere,
    DcCylinder,
    DcBox,
    DcCone
} DtSurface;

typedef enum {
    DcClipAll          =  0,
    DcClipAnd          =  1,
    DcClipAndReverse   =  2,
    DcClipNoOp         =  3,
    DcClipAndInverted  =  4,
    DcClipReplace      =  5,
    DcClipXOr          =  6,
    DcClipOr           =  7,
    DcClipNor          =  8,
    DcClipEqv          =  9,
    DcClipInvertVolume = 10,
    DcClipOrReverse    = 11,
    DcClipInvert       = 12,
    DcClipOrInverted   = 13,
    DcClipNAnd         = 14,
    DcClipNone         = 15
} DtClipOperator;

typedef enum {
    DcSetAdd,
    DcSetDelete,
    DcSetInvert,
    DcSetReplace
} DtSetOperation;

typedef enum {
    DcNameNone,
    DcNameInteger,
    DcNameString
} DtNameType;

typedef enum {
    DcBeginning,
    DcEnd,
    DcCurrent
} DtRelPosition;

typedef enum {
    DcGroupOk,
    DcGroupBad
} DtGroupNetworkStatus;

typedef enum {
    DcObjectValid,
    DcObjectInvalid,
    DcObjectDeleted
} DtObjectStatus;

typedef enum {		/* bsd: was DtInt with defines */
    DcHitAccept = 0,
    DcHitReject,
    DcHitOverwrite
} DtHitStatus;

typedef enum {
    DcTopFirst,
    DcBottomFirst
} DtPickPathOrder;

typedef enum {
    DcInvisibilityInclusion,
    DcInvisibilityExclusion,
    DcPickabilityInclusion,
    DcPickabilityExclusion
} DtFilter;

typedef enum {
    DcStringPrecision,
    DcCharacterPrecision,
    DcStrokePrecision
} DtTextPrecision;

typedef enum {
    DcTextHAlignNormal,
    DcTextHAlignLeft,
    DcTextHAlignCenter,
    DcTextHAlignRight
} DtTextAlignHorizontal;

typedef enum {
    DcTextVAlignNormal,
    DcTextVAlignTop,
    DcTextVAlignCap,
    DcTextVAlignHalf,
    DcTextVAlignBase,
    DcTextVAlignBottom
} DtTextAlignVertical;

typedef enum {
    DcTextPathRight,
    DcTextPathLeft,
    DcTextPathUp,
    DcTextPathDown
} DtTextPath;

typedef enum {
    DcPlainRoman,
    DcSimplexRoman,
    DcDuplexRoman,
    DcComplexSmallRoman,
    DcComplexRoman,
    DcTriplexRoman,
    DcComplexSmallItalic,
    DcComplexItalic,
    DcTriplexItalic,
    DcSimplexScript,
    DcComplexScript,
    DcGothicGerman,
    DcGothicEnglish,
    DcGothicItalian,
    DcPlainGreek,
    DcSimplexGreek,
    DcComplexSmallGreek,
    DcComplexGreek,
    DcComplexCyrillic,
    DcUpperCaseMathematics,
    DcLowerCaseMathematics,
    DcMusic,
    DcMeteorology,
    DcSymbols,
    DcAstrology,
    DcHelvetica
} DtFont;

typedef struct {
    DtFont          font;
    DtTextPrecision precision;
} DtFontPrecision;

typedef enum {
    DcDispIntermediateAll,
    DcDispFinal,
    DcDispRegularIntervals,
    DcDispIrregularIntervals
} DtIntermediateDispType;

typedef enum {
    DcFormFactorHemicube
} DtFormFactorCompType;

typedef enum {
    DcConvFixedIterations,
    DcConvGlobalRel,
    DcConvGlobalAbs,
    DcConvPatchRel,
    DcConvPatchAbs
} DtRadiosityConvType;

typedef enum {
    DcRadiosityNone,
    DcRadiosityComputeDisp,
    DcRadiosityDisp
} DtRadiosityUpdateType;

    /* The DtVertexType type is a bitfield that indicates the information
     * available for each vertex.  The bitfield assumes a minimum size
     * of 32 bits and is currently laid out like this:
     *
     *      Bit 0:      normals included flag
     *      Bit 1:      colors included flag
     *      Bit 2:      alpha included flag
     *      Bits 3-15:  unused
     *      Bits 16-23: number of UV texture coordinates
     *      Bits 24-31: number of UVW texture coordinates
     *
     *  31                                  0
     *  +--------+--------+--------+--------+
     *  |xxxxxxxx|xxxxxxxx|        |     xxx|
     *  +--------+--------+--------+--------+
     *   \______/ \______/               \_/
     *       |        |                   |_____ normals/colors/alpha
     *       |        |_________________________ number of UV coordinates 
     *       |__________________________________ number of UVW coordinates
     *
    */

typedef DtInt DtVertexType;

#define DcLocNrm          0x01
#define DcLocClr          0x02
#define DcLocAlpha        0x04

#define DcLoc             0x00
#define DcLocNrmClr       0x03  /* (DcLocNrm | DcLocClr)   */
#define DcLocNrmAlpha     0x05  /* (DcLocNrm | DcLocAlpha) */
#define DcLocClrAlpha     0x06  /* (DcLocClr | DcLocAlpha) */
#define DcLocNrmClrAlpha  0x07  /* (DcLocNrm | DcLocClr | DcLocAlpha) */

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

/* bsd: was #define DcNullPtr        ((DtPtr)0) */
#define DcNullPtr        0

#define DcNullObject     ((DtObject)0)

/* subdivision types */
#define DcSubDivFixed     0
#define DcSubDivAbsolute  1
#define DcSubDivRelative  2
#define DcSubDivSegments  3

    /* Method Routine Types */

#define DcMethodNull              -1
#define DcMethodDestroy            0
#define DcMethodAddReference       1
#define DcMethodRemoveReference    2
#define DcMethodPrint              3
#define DcMethodCheckGroup         4
#define DcMethodCmpBndVolume       5
#define DcMethodIniPick            6
#define DcMethodPick               7
#define DcMethodInqGlbAttVal       8
#define DcMethodUpdStdAltObj       9
#define DcMethodStdRenderStudio   10
#define DcMethodStdRenderDisplay  11

/* rendering methods */
#define DcMethodDynIniRender      12
#define DcMethodDynRender         13
#define DcMethodGlbrndIniRender   14
#define DcMethodGlbrndIniObjs     15
#define DcMethodGlbrndSpcboxOvr   16
#define DcMethodGlbrndRayint      17
#define DcMethodGlbrndUsrdat      18
#define DcMethodGlbrndWcsloc      19
#define DcMethodGlbrndWcsnrm      20
#define DcMethodGlbrndWldBnd      21
#define DcMethodUpdStdAltLineObj  22

typedef DtPtr (*DtMethodPtr)();	/* Method Routine Pointer Type */

typedef struct {
    DtInt       id;		/* Method Identifier */
    DtMethodPtr routine;	/* Pointer to Method Routine Function */
} DtMethodEntry;

/* pick status */
#define DcPickBadStatus	     1 
#define DcPickListOverflow   2
#define DcPickIndexOverflow  4

/* marker types */
#define DcMarkerPoint    -1
#define DcMarkerPlus      1
#define DcMarkerStar      2
#define DcMarkerO         3
#define DcMarkerX         4
#define DcMarkerDiamond   5
#define DcMarkerSquare    6
#define DcMarkerTriangle  7

/* object types */
/***********************************************************************
 **************************** WARNING **********************************
 ** Dore 2.2 and older use constants for identifying classes.         **
 ** The following constants of the form 'DcType...' have been         **
 ** retained for backward compatibility.                              **
 ** As new classes are added they will NOT have assigned constants.   **
 ** Instead the class identifier should be obtained from the class    **
 ** name through the system routine DsInqClassId.  The class name for **
 ** all classes (both new and old) is the same as the name of the C   **
 ** routine used to create an instance of the class.  For example the **
 ** class name for a View is "DoView", corresponding to the routine   **
 ** DoView().                                                         **
 ***********************************************************************/  

#define DcTypeAny			-1
#define DcTypeClass			0
#define DcTypeDeleted			1

#define DcTypeGroup                   	2
#define DcTypeInlineGroup             	3
#define DcTypeDevice                  	4
#define DcTypeFrame                   	5
#define DcTypeView                    	6
#define DcTypeLabel                   	7
#define DcTypeTransformMatrix         	8
#define DcTypeLookAtFrom              	9
#define DcTypePopMatrix              	10
#define DcTypePushMatrix             	11
#define DcTypeRotate                 	12
#define DcTypeScale                  	13
#define DcTypeShear                  	14
#define DcTypeTranslate              	15
#define DcTypeAmbientSwitch          	16
#define DcTypeAnnotationText         	17
#define DcTypeBackfaceCullSwitch     	20
#define DcTypeBoundingVolume         	21
#define DcTypeCallback               	22
#define DcTypeCamera                 	23
#define DcTypeCameraMatrix           	25
#define DcTypeClipSwitch             	26
#define DcTypeClipVolume             	27
#define DcTypeDataPtr                	29
#define DcTypeDataVal                	30
#define DcTypeDepthCue               	31
#define DcTypeDepthCueSwitch         	32
#define DcTypeDiffuseColor           	33
#define DcTypeDiffuseIntens          	34
#define DcTypeDiffuseSwitch          	35
#define DcTypeExecSet                	37
#define DcTypeFilter                 	38
#define DcTypeGlbRndMaxObjs          	39
#define DcTypeGlbRndMaxSub           	40
#define DcTypeGlbRndRayLevel         	41
#define DcTypeHiddenSurfaceSwitch    	42
#define DcTypeInputSlot              	45
#define DcTypeInterpType             	46
#define DcTypeInvisibilitySwitch     	47
#define DcTypeLight                  	48
#define DcTypeLightColor             	49
#define DcTypeLightIntens            	52
#define DcTypeLightType              	55
#define DcTypeLineType               	57
#define DcTypeLineWidth              	58
#define DcTypeMatrix                 	59
#define DcTypeMinBoundingExtension   	60
#define DcTypeMarkerFont             	61
#define DcTypeMarkerGlyph            	62
#define DcTypeMarkerScale            	63
#define DcTypeNameSet                	64
#define DcTypeNURBSurface           	66
#define DcTypeParallel               	69
#define DcTypePatch                  	70
#define DcTypePickID                 	71
#define DcTypePickabilitySwitch      	72
#define DcTypePerspective            	73
#define DcTypePolygon                	74
#define DcTypePolyline               	75
#define DcTypePolymarker             	76
#define DcTypePolygonMesh            	77
#define DcTypePopAttributes          	78
#define DcTypePrimitiveSurface       	79
#define DcTypeProjection             	80
#define DcTypePushAttributes         	81
#define DcTypeReflectionSwitch       	82
#define DcTypeRepType                	83
#define DcTypeShadowSwitch           	84
#define DcTypeShadeIndex             	85
#define DcTypeSimplePolygon          	86
#define DcTypeSimplePolygonMesh      	87
#define DcTypeSpecularColor          	91
#define DcTypeSpecularFactor         	92
#define DcTypeSpecularIntens         	93
#define DcTypeSpecularSwitch         	94
#define DcTypeTorus                  	98
#define DcTypeTriangleMesh          	100
#define DcTypeTranspColor           	101
#define DcTypeTranspIntens          	102
#define DcTypeTranspSwitch          	103
#define DcTypeText                  	104
#define DcTypeTextAlign             	105
#define DcTypeTextExpansionFactor   	106
#define DcTypeTextFont              	107
#define DcTypeTextHeight            	108
#define DcTypeTextPath              	109
#define DcTypeTextPrecision         	110
#define DcTypeTextSpace             	111
#define DcTypeTextUpVector          	112
#define DcTypeAttAmbientSwitch      	115
#define DcTypeAttDiffuseSwitch      	116
#define DcTypeAttSpecularSwitch     	117
#define DcTypeAttDiffuseColor       	118
#define DcTypeAttSpecularColor      	119
#define DcTypeAttDiffuseIntens	   	120
#define DcTypeAttSpecularIntens		121
#define DcTypeAttSpecularFactor     	122
#define DcTypeAttRepType		123
#define DcTypeAttInterpType		124
#define DcTypeAttLcstowcsmat		125
#define DcTypeAttBackfaceCullSwitch	126
#define DcTypeBackfaceCullable		127
#define DcTypeAttBackfaceCullable	128
#define DcTypeAttLightIntens		129
#define DcTypeAttLightColor		131
#define DcTypeAttShadowSwitch		132
#define DcTypeAttReflectionSwitch	133
#define DcTypeAttGlbRndMaxObjs		134
#define DcTypeAttGlbRndMaxSub		135
#define DcTypeAttGlbRndRayLevel		136
#define DcTypeAttTranspColor		137
#define DcTypeAttTranspIntens		138
#define DcTypeAttTranspSwitch		139
#define DcTypeAttTextAlignment		145
#define DcTypeAttTextExpansion		146
#define DcTypeAttTextFont		147
#define DcTypeAttTextHeight		148
#define DcTypeAttTextPath		149
#define DcTypeAttTextSpace		150
#define DcTypeAttTextUpVector		151
#define DcTypeAttTextPrecision		152
#define DcTypeRayinttri			153
#define DcTypeRayintmshtri		154
#define DcTypeNull			155
#define DcTypeAttLineType		156
#define DcTypeAttLineWidth		157
#define DcTypeAttMarkerScale		158
#define DcTypeAttMarkerGlyph		159
#define DcTypeAttMarkerFont		160
#define DcTypeAttExecSet		161
#define DcTypeAttNameSet		162
#define DcTypeAttFilter			163
#define DcTypeAttInvisibilitySwitch	164
#define DcTypeAttPickabilitySwitch	165
#define DcTypeAttClipSwitch		168
#define DcTypeAttClipVolume		169
#define DcTypeBoundingVolumeSwitch	170
#define DcTypeAttBoundingVolumeSwitch	171
#define DcTypeAttMinBoundingExtension	172
#define DcTypeAttPickID			173
#define DcTypeTriangleList              175
#define DcTypeAttDepthCue               176
#define DcTypeAttDepthCueSwitch         177
#define DcTypeAttHiddenSurfaceSwitch    178
#define DcTypeSubDivSpec		179
#define DcTypeAttSubDivSpec   		180
#define DcTypeAttLightType		182
#define DcTypeSurfaceShader		183
#define DcTypeAttSurfaceShader		184
#define DcTypeLineList			185
#define DcTypePointList			186
#define DcTypeAttShadeIndex		187
#define DcTypeSphereList		189
#define DcTypeCylinderList		190
#define DcTypeStereoSwitch		193
#define DcTypeAttStereoSwitch		194
#define DcTypeStereo			195
#define DcTypeAttStereo			196
#define DcTypeAmbientIntens		199
#define DcTypeAttAmbientIntens	   	200
#define DcTypeVarTriangleMesh		201
#define DcTypeVarLineList		202
#define DcTypeVarPointList		203
#define DcTypeVarSimplePolygonMesh	204
#define DcTypeAttCammat			205

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

/* standard matrices for cubic curves and patches */

extern DtObject DcBezier4;
extern DtObject DcHermite4;
extern DtObject DcBSpline4;

/* standard light type objects */

extern DtObject DcLightAmbient;
extern DtObject DcLightInfinite;
extern DtObject DcLightPoint;
extern DtObject DcLightPointAttn;
extern DtObject DcLightSpot;
extern DtObject DcLightSpotAttn;

/* standard surface shader objects */

extern DtObject DcShaderConstant;
extern DtObject DcShaderLightSource;

/* standard pick callbacks */

extern DtObject DcPickFirst;
extern DtObject DcPickAll;
extern DtObject DcPickClosest;

/* standard input slots */

extern DtObject DcTransXSlot;
extern DtObject DcTransYSlot;
extern DtObject DcTransZSlot;
extern DtObject DcScaleXSlot;
extern DtObject DcScaleYSlot;
extern DtObject DcScaleZSlot;
extern DtObject DcRotXSlot;
extern DtObject DcRotYSlot;
extern DtObject DcRotZSlot;
extern DtObject DcUndoSlot;
extern DtObject DcUpdateSlot;

/* standard Texture Map Operators */

extern DtObject DcStdBumpMap;
extern DtObject DcStdSphereEnvironMap;
extern DtObject DcStdCubeEnvironMap;
extern DtObject DcStdTableLookup;
extern DtObject DcStd3dTableLookup;
extern DtObject Dc2PartMap;

/* default Texture UV/UVW Computation Callbacks */

extern DtObject DcStdUVCallback;
extern DtObject DcStdUVWCallback;

/* standard Super Sample Filter */

extern DtObject DcFilterBox;

/* standard callback to delete raster data */

extern DtObject DcDeleteData;


    /*****************************/
    /***  Function Prototypes  ***/
    /*****************************/

/* This is kept for backwards compatibility. */
#define ARGS(arg_list) arg_list

	/* Device Functions */

void		DdInqColorEntries  (DtObject,DtColorModel,DtInt,DtInt,DtReal[]);
DtInt		DdInqColorTableSize(DtObject);
void		DdInqExtent	   (DtObject, DtVolume *);
void		DdInqFonts	   (DtObject, DtFontPrecision[]);
DtObject        DdInqFrame	   (DtObject);
DtInt		DdInqNumFonts	   (DtObject);
void		DdInqPickAperture  (DtObject, DtSize3 *);
DtObject	DdInqPickCallback  (DtObject);
DtPickPathOrder	DdInqPickPathOrder (DtObject);
DtFlag		DdInqPixelData	   (DtObject, DtRasterType, DtInt*, DtInt*,
					DtRasterType*, DtPtr*, DtFlag*);
void		DdInqResolution	   (DtObject, DtReal*, DtReal*);
DtShadeMode	DdInqShadeMode	   (DtObject);
void		DdInqShadeRanges   (DtObject, DtInt, DtInt, DtInt[]);
void		DdInqViewport	   (DtObject, DtVolume *);
DtVisualType	DdInqVisualType	   (DtObject);
void		DdSetOptions	   (DtObject, DtPtr);
void		DdPickObjs	   (DtObject, DtPoint3, DtInt*, DtInt, DtInt[],
					DtInt, long[], DtReal[], DtReal[],
					DtReal[], DtObject[], DtInt*);

    /* NOTE: DdPick is obseleted by DdPickObjs */

void DdPick		(DtObject, DtPoint3, DtInt *, DtInt, DtInt[], DtInt,
			    long[], DtReal[], DtObject[], DtInt *);
void DdSetColorEntries	(DtObject, DtColorModel, DtInt, DtInt, DtReal[]);
void DdSetFrame		(DtObject, DtObject);
void DdSetPickAperture	(DtObject, DtSize3 *);
void DdSetPickCallback	(DtObject, DtObject);
void DdSetPickPathOrder	(DtObject, DtPickPathOrder);
void DdSetShadeMode	(DtObject, DtShadeMode);
void DdSetShadeRanges	(DtObject, DtInt, DtInt, DtInt[]);
void DdSetViewport	(DtObject, DtVolume *);
void DdUpdate		(DtObject);

	/* Frame Functions */

void	 DfInqBoundary	(DtObject, DtVolume *);
void     DfInqJust	(DtObject, DtReal*, DtReal*);
DtObject DfInqViewGroup	(DtObject);
void	 DfSetBoundary	(DtObject, DtVolume *);
void	 DfSetJust	(DtObject, DtReal, DtReal);
void	 DfUpdate	(DtObject);

	/* Group Functions */

void	 DgAddObj		(DtObject);
void	 DgAddObjToGroup	(DtObject, DtObject);
DtGroupNetworkStatus
	 DgCheck		(DtObject);
DtObject DgClose		(void);
void	 DgDelEle		(DtInt);
DtFlag	 DgDelEleBetweenLabels	(DtInt, DtInt);
void	 DgDelEleRange		(DtInt, DtInt);
void	 DgEmpty		(DtObject);
DtInt	 DgInqElePtr		(void);
DtObject DgInqObjAtPos		(DtObject, DtInt, DtRelPosition);
DtObject DgInqOpen		(void);
DtInt	 DgInqSize		(DtObject);
void	 DgOpen			(DtObject, DtFlag);
void	 DgReplaceObj		(DtObject);
void	 DgReplaceObjInGroup	(DtObject, DtObject);
void	 DgSetElePtr		(DtInt, DtRelPosition);
DtFlag	 DgSetElePtrRelLabel	(DtInt, DtInt);

	/* Object Creation Functions */

DtObject DoAmbientIntens	 (DtReal);
DtObject DoAmbientSwitch	 (DtSwitch);
DtObject DoAnnoText		 (DtReal[], char*);
DtObject DoBackfaceCullSwitch	 (DtSwitch);
DtObject DoBackfaceCullable	 (DtSwitch);
DtObject DoBoundingVol		 (DtVolume *, DtObject);
DtObject DoBoundingVolSwitch	 (DtSwitch);
DtObject DoCallback		 (DtPFI, DtObject);
DtObject DoCamera		 (void);
DtObject DoCameraMatrix		 (DtMatrix4x4);
DtObject DoClipSwitch		 (DtSwitch);
DtObject DoClipVol		 (DtClipOperator, DtInt, DtHalfSpace[]);
DtObject DoCompTextureUVCallback (DtObject);
DtObject DoCompTextureUVSwitch   (DtSwitch);
DtObject DoCompTextureUVWCallback(DtObject);
DtObject DoCompTextureUVWSwitch  (DtSwitch);
DtObject DoDataPtr		 (DtPtr);
DtObject DoDataVal		 (Dt32Bits);
DtObject DoDepthCue		 (DtReal, DtReal, DtReal, DtReal, DtColorModel,
				    DtReal[]);
DtObject DoDepthCueSwitch	 (DtSwitch);
DtObject DoDevice		 (DtPtr, DtPtr);
DtObject DoDiffuseColor		 (DtColorModel, DtReal[]);
DtObject DoDiffuseIntens	 (DtReal);
DtObject DoDiffuseSwitch	 (DtSwitch);
DtObject DoEmissionColor	 (DtColorModel, DtReal[]);
DtObject DoEmissionFactor	 (DtReal);
DtObject DoEmissionSwitch	 (DtSwitch);
DtObject DoExecSet		 (DtInt, DtInt[], DtSetOperation);
DtObject DoFileRaster		 (DtPtr, DtPtr);
DtObject DoFilter		 (DtFilter, DtInt, DtInt[], DtSetOperation);
DtObject DoFrame		 (void);
DtObject DoGenerateTextureUV     (DtSwitch);
DtObject DoGlbRendMaxObjs	 (DtInt);
DtObject DoGlbRendMaxSub	 (DtInt);
DtObject DoGlbRendRayLevel	 (DtInt);
DtObject DoGroup		 (DtFlag);
DtObject DoHiddenSurfSwitch	 (DtSwitch);
DtObject DoInLineGroup		 (DtFlag);
DtObject DoInputSlot		 (void);
DtObject DoIntermediateDispType	 (DtIntermediateDispType, DtInt[]);
DtObject DoInterpType		 (DtInterpType);
DtObject DoInvisSwitch		 (DtSwitch);
DtObject DoLabel		 (DtInt);
DtObject DoLight		 (void);
DtObject DoLightAttenuation	 (DtReal, DtReal);
DtObject DoLightColor		 (DtColorModel, DtReal[]);
DtObject DoLightIntens		 (DtReal);
DtObject DoLightSpreadAngles	 (DtReal, DtReal);
DtObject DoLightSpreadExp	 (DtReal);
DtObject DoLightSwitch		 (DtObject, DtSwitch);
DtObject DoLightType		 (DtObject);
DtObject DoLineList		 (DtColorModel, DtVertexType, DtInt, DtReal[]);
DtObject DoLineType		 (DtLineType);
DtObject DoLineWidth		 (DtReal);
DtObject DoLocalAntiAliasStyle	 (DtLocalAntiAliasStyle);
DtObject DoLocalAntiAliasSwitch	 (DtSwitch);
DtObject DoLookAtFrom		 (DtPoint3, DtPoint3, DtVector3);
DtObject DoMarkerFont		 (DtFont);
DtObject DoMarkerGlyph		 (DtInt);
DtObject DoMarkerScale		 (DtReal);
DtObject DoMatrix		 (DtInt, DtInt, DtReal[]);
DtObject DoMinBoundingVolExt	 (DtReal);
DtObject DoNURBSurf		 (DtColorModel, DtCtrlPointType, DtNArea, DtInt,
				    DtInt, DtReal[], DtInt, DtInt, DtReal[],
				    DtInt, DtInt, DtReal[]);
DtObject DoNameSet		 (DtInt, DtInt[], DtSetOperation);
DtObject DoParallel		 (DtReal, DtReal, DtReal);
DtObject DoPatch		 (DtColorModel, DtVertexType, DtObject,
				    DtReal[], DtObject);
DtObject DoPerspective		 (DtReal, DtReal, DtReal);
DtObject DoPickID		 (DtInt);
DtObject DoPickSwitch		 (DtSwitch);
DtObject DoPointList		 (DtColorModel, DtVertexType, DtInt, DtReal[]);
DtObject DoPolygon		 (DtColorModel, DtVertexType, DtInt, DtInt[],
				    DtReal[], DtShapeType);
DtObject DoPolygonMesh		 (DtColorModel, DtVertexType, DtInt, DtReal[],
				    DtInt, DtInt[], DtInt[], DtInt[],
				    DtShapeType, DtFlag);
DtObject DoPolyline		 (DtColorModel, DtVertexType, DtInt, DtReal[]);
DtObject DoPolymarker		 (DtInt, DtReal[][3]);
DtObject DoPopAtts		 (void);
DtObject DoPopMatrix		 (void);
DtObject DoPrimSurf		 (DtSurface);
DtObject DoProjection		 (DtArea *, DtProjectionType, DtPoint3, DtReal,
				    DtReal, DtReal);
DtObject DoPushAtts		 (void);
DtObject DoPushMatrix		 (void);
DtObject DoQuadGrid		 (DtColorModel, DtVertexType, DtInt, DtInt,
				    DtReal[], DtFlag);
DtObject DoQuadList		 (DtColorModel, DtVertexType, DtInt, DtReal[]);
DtObject DoQuadMesh		 (DtColorModel, DtVertexType, DtInt, DtReal[],
				    DtInt, DtInt[], DtFlag);
DtObject DoRadiosityAmbientSwitch(DtSwitch);
DtObject DoRadiositySwitch	 (DtSwitch);
DtObject DoRaster		 (DtInt, DtInt, DtInt, DtRasterType, DtPtr,
				    DtPtr, DtObject);
DtObject DoReflectionSwitch	 (DtSwitch);
DtObject DoRefractionIndex	 (DtReal);
DtObject DoRefractionSwitch	 (DtSwitch);
DtObject DoRepType		 (DtRepType);
DtObject DoRotate		 (DtAxis, DtReal);
DtObject DoSampleAdaptive	 (DtReal);
DtObject DoSampleAdaptiveSwitch	 (DtSwitch);
DtObject DoSampleFilter		 (DtObject, DtReal, DtReal);
DtObject DoSampleJitter		 (DtReal);
DtObject DoSampleJitterSwitch	 (DtSwitch);
DtObject DoSampleSuper		 (DtInt, DtInt);
DtObject DoSampleSuperSwitch	 (DtSwitch);
DtObject DoScale		 (DtReal, DtReal, DtReal);
DtObject DoShadeIndex		 (DtInt);
DtObject DoShadowSwitch		 (DtSwitch);
DtObject DoShear		 (DtMajorPlane, DtReal, DtReal);
DtObject DoSimplePolygon	 (DtColorModel, DtVertexType, DtInt, DtReal[],
				    DtShapeType );
DtObject DoSimplePolygonMesh	 (DtColorModel, DtVertexType, DtInt, DtReal[],
				    DtInt, DtInt[], DtInt[], DtShapeType,
				    DtFlag);
DtObject DoSpecularColor	 (DtColorModel, DtReal[]);
DtObject DoSpecularFactor	 (DtReal);
DtObject DoSpecularIntens	 (DtReal);
DtObject DoSpecularSwitch	 (DtSwitch);
DtObject DoSphereList            (DtColorModel, DtInt, DtReal[], DtReal[],
				    DtReal[]);
DtObject DoStereo		 (DtReal, DtReal);
DtObject DoStereoSwitch		 (DtSwitch);
DtObject DoSubDivSpec		 (DtInt, DtReal[]);
DtObject DoSurfaceEdgeColor	 (DtColorModel, DtReal[]);
DtObject DoSurfaceShade		 (DtObject);
DtObject DoText			 (DtPoint3, DtVector3, DtVector3, char*);
DtObject DoTextAlign		 (DtTextAlignHorizontal, DtTextAlignVertical);
DtObject DoTextExpFactor	 (DtReal);
DtObject DoTextFont		 (DtFont);
DtObject DoTextHeight		 (DtReal);
DtObject DoTextPath		 (DtTextPath);
DtObject DoTextPrecision	 (DtTextPrecision);
DtObject DoTextSpace		 (DtReal);
DtObject DoTextUpVector		 (DtReal, DtReal);
DtObject DoTextureAntiAlias	 (DtTextureAntiAliasMode);
DtObject DoTextureExtendUV	 (DtExtendMode, DtExtendMode);
DtObject DoTextureExtendUVW	 (DtExtendMode, DtExtendMode, DtExtendMode);
DtObject DoTextureIntermediateSurf (DtObject, Dt2PartProjType);
DtObject DoTextureMapBump	 (DtMapOperator, DtObject, DtObject);
DtObject DoTextureMapBumpSwitch	 (DtSwitch);
DtObject DoTextureMapDiffuseColor (DtMapOperator, DtObject, DtObject);
DtObject DoTextureMapDiffuseColorSwitch (DtSwitch);
DtObject DoTextureMapEnviron	 (DtMapOperator, DtObject, DtObject);
DtObject DoTextureMapEnvironSwitch (DtSwitch);
DtObject DoTextureMapTranspIntens (DtMapOperator, DtObject, DtObject);
DtObject DoTextureMapTranspIntensSwitch (DtSwitch);
DtObject DoTextureMatrixUV	 (DtMatrix3x3);
DtObject DoTextureMatrixUVW	 (DtMatrix4x4);
DtObject DoTextureOp		 (DtTextureOperator);
DtObject DoTextureScaleUV	 (DtReal, DtReal);
DtObject DoTextureScaleUVW	 (DtReal, DtReal, DtReal);
DtObject DoTextureTranslateUV	 (DtReal, DtReal);
DtObject DoTextureTranslateUVW	 (DtReal, DtReal, DtReal);
DtObject DoTextureUVIndex	 (DtInt);
DtObject DoTextureUVWIndex	 (DtInt);
DtObject DoTorus		 (DtReal, DtReal);
DtObject DoTransformMatrix	 (DtMatrix4x4, DtCompType);
DtObject DoTranslate		 (DtReal, DtReal, DtReal);
DtObject DoTranspColor		 (DtColorModel, DtReal[]);
DtObject DoTranspIntens		 (DtReal);
DtObject DoTranspOrientColor	 (DtColorModel, DtReal[]);
DtObject DoTranspOrientExp	 (DtReal);
DtObject DoTranspOrientIntens	 (DtReal);
DtObject DoTranspOrientSwitch	 (DtSwitch);
DtObject DoTranspSwitch		 (DtSwitch);
DtObject DoTriangleList		 (DtColorModel, DtVertexType, DtInt, DtReal[]);
DtObject DoTriangleMesh		 (DtColorModel, DtVertexType, DtInt, DtReal[],
				    DtInt, DtInt[], DtFlag);
DtObject DoTriangleStrip	 (DtColorModel, DtVertexType, DtInt, DtReal[]);
DtObject DoVarLineList		 (DtColorModel, DtInt, DtReal[], DtReal[],
				    DtReal[]);
DtObject DoVarPointList		 (DtColorModel, DtInt, DtReal[], DtReal[],
				    DtReal[]);
DtObject DoVarQuadGrid		 (DtColorModel, DtInt, DtInt, DtReal[],
				    DtReal[], DtReal[], DtFlag);
DtObject DoVarQuadMesh		 (DtColorModel, DtInt, DtReal[], DtReal[],
				    DtReal[], DtInt, DtInt[], DtFlag);
DtObject DoVarSimplePolygonMesh	 (DtColorModel, DtInt, DtReal[], DtReal[],
				    DtReal[], DtInt, DtInt[], DtInt[],
				    DtShapeType, DtFlag);
DtObject DoVarTriangleList	 (DtColorModel, DtInt, DtReal[], DtReal[],
				    DtReal[], DtReal[]);
DtObject DoVarTriangleMesh	 (DtColorModel, DtInt, DtReal[], DtReal[],
				    DtReal[], DtInt, DtInt[], DtFlag);
DtObject DoVarTriangleStrip	 (DtColorModel, DtInt, DtReal[], DtReal[],
				    DtReal[]);
DtObject DoView			 (void);

	/* Primitive Object Functions */

void DpUpdVarLineList	         (DtObject, DtInt, DtReal[], DtReal[],DtReal[]);
void DpUpdVarPointList	         (DtObject, DtInt, DtReal[], DtReal[],DtReal[]);
void DpUpdVarQuadGrid	         (DtObject, DtReal[],DtReal[],DtReal[], DtFlag);
void DpUpdVarQuadGridUV	         (DtObject, DtInt, DtReal[]);
void DpUpdVarQuadGridUVW         (DtObject, DtInt, DtReal[]);
void DpUpdVarQuadMesh	         (DtObject, DtReal[],DtReal[],DtReal[], DtFlag);
void DpUpdVarQuadMeshUV	         (DtObject, DtInt, DtReal[]);
void DpUpdVarQuadMeshUVW         (DtObject, DtInt, DtReal[]);
void DpUpdVarSimplePolygonMesh   (DtObject, DtReal[], DtReal[], DtReal[],
				    DtShapeType, DtFlag, DtFlag);
void DpUpdVarSimplePolygonMeshUV (DtObject, DtInt, DtReal[]);
void DpUpdVarSimplePolygonMeshUVW(DtObject, DtInt, DtReal[]);
void DpUpdVarTriangleMesh	 (DtObject, DtReal[], DtReal[], DtReal[],
				    DtFlag);
void DpUpdVarTriangleMeshUV	 (DtObject, DtInt, DtReal[]);
void DpUpdVarTriangleMeshUVW	 (DtObject, DtInt, DtReal[]);
void DpUpdVarTriangleStrip	 (DtObject, DtInt, DtReal[],DtReal[],DtReal[]);
void DpUpdVarTriangleStripUV	 (DtObject, DtInt, DtReal[]);
void DpUpdVarTriangleStripUVW    (DtObject, DtInt, DtReal[]);

	/* System Functions */

DtVolume*      DsCompBoundingVol     (DtVolume*, DtObject);
void	       DsExecuteObj	     (DtObject);
void	       DsExecutionAbort	     (void);
void	       DsExecutionReturn     (void);
DtInt	       DsFileRasterRead	     (DtPtr, DtInt*, DtInt*, DtInt*,
					DtRasterType*, DtPtr*);
DtObject       DsHoldObj	     (DtObject);

    /* The following code is written thus in order that the Dore' library can
    ** ensure that both the library and the application code agree on the
    ** precision of a DtReal.  This prevents a single-precision application
    ** from trying to run after linking with a double-precision Dore' library,
    ** for example.  Note that DORE_REAL_PRECISION will be either 1 or 2.  */

void DsInitFunction (DtInt processors, int dtreal_precision);

#define DsInitialize(processors) \
     DsInitFunction (processors, DORE_REAL_PRECISION)

#define DsInitializeSystem(processors) \
     DsInitFunction (processors, DORE_REAL_PRECISION)

void	       DsInputValue	     (DtObject, DtReal);
DtAngleUnits   DsInqAngleUnits	     (void);
DtInt	       DsInqClassId	     (char*);
DtInt	       DsInqCurrentMethod    (void);
void	       DsInqDriverInfo	     (DtInt, char*[], char*[]);
DtReadStatus   DsInqErrorMessage     (DtInt, DtInt, char[], DtErrorStatus *);
void	       DsInqErrorVars	     (DtInt *, DtPFI *);
DtInt	       DsInqExeDepthLimit    (void);
DtFlag	       DsInqHoldObj	     (DtObject);
DtInt	       DsInqMethodId	     (char*);
DtInt	       DsInqNumDrivers	     (void);
DtInt	       DsInqNumRenderers     (void);
DtObject       DsInqObj		     (DtNameType, DtPtr, DtInt);
DtInt	       DsInqObjClassId	     (DtObject);
void	       DsInqObjName	     (DtObject, DtNameType *, DtPtr);
DtInt          DsInqObjNameInteger   (DtObject);
char          *DsInqObjNameString    (DtObject);
DtNameType     DsInqObjNameType      (DtObject);
DtObjectStatus DsInqObjStatus	     (DtObject);

/* NOTE: DsInqObjType is obseleted by DsInqObjClassId */
DtInt	       DsInqObjType	     (DtObject);
DtObject       DsInqObjViaInteger    (DtInt, DtInt);
DtObject       DsInqObjViaString     (char*, DtInt);
void	       DsInqRaster	     (DtObject, DtInt*, DtInt*, DtInt*,
					DtRasterType*, char**, DtPtr*);
DtInt	       DsInqRendererId	     (char*);
void	       DsInqRendererNames    (char *[]);
DtFlag	       DsInqSafeFlag	     (void);
DtObject       DsInqValuatorGroup    (DtObject);
void	       DsInqVersion	     (DtPtr *);
void	       DsPrintObj	     (DtObject);
void	       DsRadiosityInitialize (void);
void	       DsRasterUpdate	     (DtObject);
DtInt	       DsRasterWrite	     (DtObject, char*);
void	       DsReleaseObj	     (DtObject);
void	       DsSetAngleUnits	     (DtAngleUnits units);
void	       DsSetErrorVars	     (DtInt, DtPFI);
void	       DsSetExeDepthLimit    (DtInt);
void	       DsSetObjName	     (DtObject, DtNameType, DtPtr, DtFlag);
void           DsSetObjNameInteger   (DtObject, DtInt, DtFlag);
void           DsSetObjNameString    (DtObject, char*, DtFlag);
void	       DsSetSafeFlag	     (DtFlag);
void	       DsTerminate	     (void);

/* NOTE: DsTerminateSystem is obseleted by DsTerminate */
void	       DsTerminateSystem     (void);
DtVertexType   DsTextureUVCount	     (DtInt);
DtVertexType   DsTextureUVWCount     (DtInt);
void	       DsUpdateAllViews	     (void);
void	       DsValuatorSwitch	     (DtSwitch);

    /* Viewport Functions */

DtObject      DvInqActiveCamera	        (DtObject);
void	      DvInqBackgroundColor	(DtObject, DtColorModel *, DtReal[]);
void	      DvInqBackgroundJust	(DtObject, DtReal *, DtReal *);
void	      DvInqBackgroundRaster	(DtObject, DtObject *, DtObject *);
void	      DvInqBoundary		(DtObject, DtVolume *);
DtFlag	      DvInqClearFlag		(DtObject);
DtObject      DvInqDefinitionGroup	(DtObject);
DtObject      DvInqDisplayGroup	        (DtObject);
void	      DvInqFormFactorCompType	(DtObject, DtFormFactorCompType*,
					    DtReal[]);
void	      DvInqRadiosityConvergence(DtObject,DtRadiosityConvType*,DtReal[]);
DtRadiosityUpdateType
	      DvInqRadiosityUpdateType  (DtObject);
DtRenderStyle DvInqRendStyle		(DtObject);
DtInt	      DvInqShadeIndex		(DtObject);
DtUpdateType  DvInqUpdateType		(DtObject);
void	      DvSetActiveCamera	        (DtObject, DtObject);
void	      DvSetBackgroundColor	(DtObject, DtColorModel, DtReal[]);
void	      DvSetBackgroundJust	(DtObject, DtReal, DtReal);
void	      DvSetBackgroundRaster	(DtObject, DtObject, DtObject);
void	      DvSetBoundary		(DtObject, DtVolume *);
void	      DvSetClearFlag		(DtObject, DtFlag);
void	      DvSetFormFactorCompType	(DtObject, DtFormFactorCompType,
					    DtReal[]);
void	      DvSetRadiosityConvergence (DtObject,DtRadiosityConvType,DtReal[]);
void	      DvSetRadiosityUpdateType  (DtObject, DtRadiosityUpdateType);
void	      DvSetRendStyle		(DtObject, DtRenderStyle);
void	      DvSetShadeIndex		(DtObject, DtInt);
void	      DvSetUpdateType		(DtObject, DtUpdateType);
void	      DvUpdate		        (DtObject);

    /* User extension functions */

DtInt	 DeAddClass	     (char*, DtInt, DtMethodEntry[], DtMethodPtr);
DtObject DeCreateObject	     (DtInt, DtPtr);
void	 DeDeleteObject	     (DtObject);
void	 DeExecuteAlternate  (DtObject);
void	 DeInitializeObjPick (DtObject);
DtFlag	 DeInqPickable	     (DtInt);
DtFlag	 DeInqRenderable     (DtInt);

#endif
