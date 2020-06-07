.\"#ident "%W%" %G%
.\"
.\"
*****************************************************************/
 
#ifndef DORE_H
#define DORE_H

/*
 * Dore is set up to allow DtReal (floating point numbers 
 * used by Dore) to be defined as either double precision
 * or single precision.  
 *
 * If DORE_REAL_SINGLE is defined, DtReal will be float.  
 * If DORE_REAL_DOUBLE is defined, DtReal will be double.
 *
 * The default is double precision for these machines:
 * 	Stardent 1500   (Titan P2G2)
 *	Stardent 3000   (Titan P3G2)
 *	Stardent 3000VS (Titan P3G3)
 *
 * In all other cases, the default is single precision.
 */

#if defined(DORE_REAL_SINGLE)
	typedef float DtReal;
#else 
#  if defined (DORE_REAL_DOUBLE)
	typedef double DtReal;
#  else
#    if defined (titan)
#       define DORE_REAL_DOUBLE
	typedef double DtReal;
#    else
#       define DORE_REAL_SINGLE
	typedef float DtReal;
#    endif
#  endif
#endif

#if defined(__STDC__)
# define DORE_FCN_PROTOTYPES
#else 
# if defined(titan)
#  define DORE_FCN_PROTOTYPES
# endif
#endif

/****************************************************************/
/**                                                  **/ 
/**  Dore user type and constant declarations        **/
/**                                                  **/
/****************************************************************/

typedef void *DtPtr;

typedef unsigned long Dt32Bits;	
/* must always be exactly 32 bits large */

typedef DtPtr DtObject;

typedef short DtShort;

typedef unsigned short DtUShort;


typedef unsigned char DtUChar;

typedef long DtInt;		
/* must always be at least 32 bits large */

typedef unsigned long DtUInt;	
/* must always be at least 32 bits large */

typedef DtReal DtRealCouple[2];	/* Couple of reals. */
typedef DtReal DtRealTriple[3];	/* Triple of reals. */
typedef DtReal DtRealQuad[4];	/* Quadruplet of reals. */
typedef DtInt DtIntTriple[3];	/* Triple of ints. */

typedef DtReal DtPoint3[3];
typedef DtReal DtPoint4[4];
typedef struct { DtReal x,y;     } DtNPoint2;
typedef struct { DtReal x,y,z;   } DtNPoint3;
typedef struct { DtReal x,y,z,w; } DtNPoint4;

typedef DtReal DtVector3[3];
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
	DtInt depth ;
	DtShort width, height ;
	DtUChar	*bits ;
} DtPattern ;

typedef DtInt (*DtPFI)() ;
typedef void  (*DtPFV)() ;
typedef DtPtr (*DtPFF)() ;

typedef struct object {
        DtShort info ;
        DtShort ref_count ;
        DtPtr   data ;	/* object's private data */
	DtPtr  *additional_data ;
} DtObjectStructure; /* used with user-defined objects */


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

typedef DtInt DtUpdateType;
#define DcUpdateAll		0
#define DcUpdateDisplay		1

typedef char DtFlag;
#define DcFalse  0
#define DcTrue   1

typedef char DtSwitch;
#define DcOff  0
#define DcOn   1

typedef char DtSurface;
#define DcSphere    0
#define DcCylinder  1
#define DcBox       2
#define DcCone      3



typedef char DtRepType;
#define DcPoints     		0
#define DcWireframe  		1
#define DcSurface    		2
#define DcOutlines   		3
#define DcSurfaceWireframeEdges	4
#define DcSurfaceOutlineEdges 	5

typedef char DtInterpType;
#define DcConstantShade  0
#define DcVertexShade    1
#define DcSurfaceShade   2

typedef char DtAxis;
#define DcXAxis  0
#define DcYAxis  1
#define DcZAxis  2

typedef char DtMajorPlane;
#define DcXY  0
#define DcYZ  1
#define DcXZ  2

typedef char DtColorModel;
#define DcRGB  0

typedef char DtVisualType;
#define DcStaticGrey   0
#define DcGreyScale    1
#define DcStaticColor  2
#define DcPseudoColor  3
#define DcTrueColor    4
#define DcDirectColor  5

typedef char DtShadeMode;
#define DcRange        0
#define DcComponent    1

typedef char DtVertexType;
#define DcLoc        0
#define DcLocNrm     1
#define DcLocClr     2
#define DcLocNrmClr  3

typedef char DtCtrlPointType;
#define DcCtr     0
#define DcCtrClr  1

typedef char DtCompType;
#define DcPreConcatenate   0
#define DcPostConcatenate  1 
#define DcReplace          2

typedef char DtRenderStyle;
#define DcRendererNewId   -1
#define DcRealTime         0
#define DcProductionTime   2

typedef char DtShapeType;
#define DcConvex     0
#define DcConcave    1
#define DcComplex    2

typedef char DtRelPosition;
#define DcBeginning  0
#define DcEnd        1
#define DcCurrent    2

typedef char DtProjectionType;
#define DcParallel     0
#define DcPerspective  1

typedef char DtCameraMatrixType;
#define DcCameraArbitrary	0
#define DcCameraParallel	1
#define DcCameraPerspective	2
#define DcCameraProjection	3

typedef char DtClipOperator;
#define DcClipAll           0
#define DcClipAnd           1
#define DcClipAndReverse    2
#define DcClipNoOp          3
#define DcClipAndInverted   4
#define DcClipReplace       5
#define DcClipXOr           6
#define DcClipOr            7
#define DcClipNor           8
#define DcClipEqv           9
#define DcClipInvertVolume 10
#define DcClipOrReverse    11
#define DcClipInvert       12
#define DcClipOrInverted   13
#define DcClipNAnd         14
#define DcClipNone         15

typedef char DtTextPrecision;
#define DcStringPrecision     0
#define DcCharacterPrecision  1
#define DcStrokePrecision     2

typedef char DtTextAlignHorizontal;
#define DcTextHAlignNormal  0
#define DcTextHAlignLeft    1
#define DcTextHAlignCenter  2
#define DcTextHAlignRight   3

typedef char DtTextAlignVertical;
#define DcTextVAlignNormal  0
#define DcTextVAlignTop     1
#define DcTextVAlignCap     2
#define DcTextVAlignHalf    3
#define DcTextVAlignBase    4
#define DcTextVAlignBottom  5

typedef char DtTextPath;
#define DcTextPathRight  0
#define DcTextPathLeft   1
#define DcTextPathUp     2
#define DcTextPathDown   3

typedef char DtNameType;
#define DcNameNone     0
#define DcNameInteger  1
#define DcNameString   2

typedef char DtPickPathOrder;
#define DcTopFirst     0
#define DcBottomFirst  1

typedef char DtHitStatus;
#define DcHitAccept       0
#define DcHitReject       1
#define DcHitOverwrite    2

typedef char DtFilter;
#define DcInvisibilityInclusion  0
#define DcInvisibilityExclusion  1
#define DcPickabilityInclusion   2
#define DcPickabilityExclusion   3

typedef char DtSetOperation;
#define DcSetAdd      0
#define DcSetDelete   1
#define DcSetInvert   2
#define DcSetReplace  3

typedef char DtLineType;
#define DcLineTypeSolid    0
#define DcLineTypeDash     1
#define DcLineTypeDot      2
#define DcLineTypeDotDash  3 

typedef char DtGroupNetworkStatus;
#define DcGroupOk    0
#define DcGroupBad   1

typedef char DtErrorStatus;
#define DcErrorMinor    0
#define DcErrorSevere   1
#define DcErrorFatal    2

typedef char DtReadStatus;
#define DcReadOk    0
#define DcReadTrunc 1
#define DcReadUnsuc 2

typedef char DtObjectStatus;
#define DcObjectValid    0
#define DcObjectInvalid  1
#define DcObjectDeleted  2

typedef DtInt DtFont;
#define DcPlainRoman            0
#define DcSimplexRoman          1
#define DcDuplexRoman           2
#define DcComplexSmallRoman     3
#define DcComplexRoman          4
#define DcTriplexRoman          5
#define DcComplexSmallItalic    6
#define DcComplexItalic         7
#define DcTriplexItalic         8
#define DcSimplexScript         9
#define DcComplexScript         10
#define DcGothicGerman          11
#define DcGothicEnglish         12
#define DcGothicItalian         13
#define DcPlainGreek            14
#define DcSimplexGreek          15
#define DcComplexSmallGreek     16
#define DcComplexGreek          17
#define DcComplexCyrillic       18
#define DcUpperCaseMathematics  19
#define DcLowerCaseMathematics  20
#define DcMusic                 21
#define DcMeteorology           22
#define DcSymbols               23
#define DcAstrology             24
#define DcHelvetica             25

typedef struct {
	DtFont	font ;
	DtTextPrecision precision ;
} DtFontPrecision ;

typedef DtInt DtRasterType;
#define DcRasterRGB		0
#define DcRasterRGBA		1
#define DcRasterRGBAZ		2
#define DcRasterRGBZ		3
#define DcRasterA		4
#define DcRasterZ		5
#define DcRasterABGR		6
#define DcRasterSpecial		99

typedef DtInt DtLocalAntiAliasStyle;
#define DcLocalAntiAliasFast		0
#define DcLocalAntiAliasIntermediate	1
#define DcLocalAntiAliasBest		2

typedef DtInt DtTextureAntiAliasMode;
#define DcTextureAntiAliasNone		0
#define DcTextureAntiAliasMIP		1
#define DcTextureAntiAliasAdaptive	2

typedef DtInt DtExtendMode;
#define DcExtendNone		0
#define DcExtendBlack		1
#define DcExtendClamp		2
#define DcExtendRepeat		3

typedef DtInt DtTextureOperator;
#define DcTextureReplace	0
#define DcTextureMultiply	1
#define DcTextureBlend		2
#define DcTextureAdd		3

typedef DtInt DtMapOperator;
#define DcMapReplace		0
#define DcMapAdd		1

typedef DtInt Dt2PartProjType;
#define DcObjCentroid           0
#define DcISN                   1

typedef DtInt DtGlobalCall;
#define DcGlobalCallNone	0
#define DcGlobalCallPush	1
#define DcGlobalCallPop		2
#define DcGlobalCallGetValue	3
#define DcGlobalCallSetValue	4

typedef char DtAngleUnits;
#define DcAngleOldStyle		0
#define DcAngleRadians		1
#define DcAngleDegrees		2

/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

#define DcNullPtr        (DtPtr)0

#define DcNullObject     (DtObject)0

/* subdivision types */
#define DcSubDivFixed               0
#define DcSubDivAbsolute            1
#define DcSubDivRelative            2
#define DcSubDivSegments	    3

/* methods */
#define DcMethodNull 	        	-1
#define DcMethodDestroy         	0
#define DcMethodAddReference    	1
#define DcMethodRemoveReference 	2
#define DcMethodPrint           	3
#define DcMethodCheckGroup		4
#define DcMethodCmpBndVolume		5
#define DcMethodIniPick			6
#define DcMethodPick            	7
#define DcMethodInqGlbAttVal		8
#define DcMethodUpdStdAltObj		9
#define DcMethodStdRenderStudio		10
#define DcMethodStdRenderDisplay	11

/* rendering methods */
#define DcMethodDynIniRender		12
#define DcMethodDynRender		13
#define DcMethodGlbrndIniRender		14
#define DcMethodGlbrndIniObjs		15
#define DcMethodGlbrndSpcboxOvr		16
#define DcMethodGlbrndRayint		17
#define DcMethodGlbrndUsrdat		18
#define DcMethodGlbrndWcsloc		19
#define DcMethodGlbrndWcsnrm		20
#define DcMethodGlbrndWldBnd		21

#define DcMethodUpdStdAltLineObj	22


/* pick status */
#define DcPickBadStatus	        1 
#define DcPickListOverflow      2
#define DcPickIndexOverflow     4


/* marker types */
#define DcMarkerPoint          -1
#define DcMarkerPlus            1
#define DcMarkerStar            2
#define DcMarkerO               3
#define DcMarkerX               4
#define DcMarkerDiamond         5
#define DcMarkerSquare          6
#define DcMarkerTriangle        7



/* object types */
/******************************************************************
 ************************* WARNING ******************************
 ** This section of the dore.h include file contains   **
 ** constants of the form 'DcType...' These contants   **
 ** are used with 2.2 and earlier versions of Dore.    **
 ** Because these constants have been retained for     **
 ** backward compatibility only, they are not listed   **
 ** in this chapter.                                   **
 ******************************************************************/  


/**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/

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



/* default Texture UV Computation Callback */

extern DtObject DcStdUVCallback;

/* standard Super Sample Filter */

extern DtObject DcFilterBox;


/* standard callback to delete raster data */

extern DtObject DcDeleteData;


/* 
*   Function Prototypes
*/

#if defined(DORE_FCN_PROTOTYPES)
#define ARGS(arg_list) arg_list
#else
#define ARGS(arg_list) ( )
#endif


/*
 * Some parameters look different from the user and 
 * implementation sides. This stuff allows users to 
 *  use either 2D or 1D arrays in certain places.
 */

#if defined(dod_address_null)
#define	PROTO_INT_LIST		DtInt  []
#define	PROTO_REAL_ARRAY	DtReal [][3]
#define	PROTO_REAL_LIST		DtReal []
#else
#define	PROTO_INT_LIST		void *
#define	PROTO_REAL_ARRAY	void *
#define	PROTO_REAL_LIST		void *
#endif


/*
 * Derived types from DtObject (for stronger typing).
 * Devices, frames, groups, and views are Dore objects and
 * may be used anywhere a DtObject is used; but a DtObject
 * of the wrong type may not be used where a more specific
 * type is called for.
 */

typedef	DtObject	DtDevice;	/* Dd functions	*/
typedef	DtObject	DtFrame;	/* Df functions	*/
typedef	DtObject	DtGroup;	/* Dg functions	*/
typedef	DtObject	DtView;		/* Dv functions	*/


/*
 * Device Functions
 */
void
    DdInqColorEntries	ARGS( (DtDevice, DtColorModel,
			       DtInt, DtInt, DtReal[]) );
DtInt
    DdInqColorTableSize	ARGS( (DtDevice) );
void
    DdInqExtent		ARGS( (DtDevice, DtVolume *) );
void
    DdInqFonts		ARGS( (DtDevice, 
			       DtFontPrecision[]) );
DtFrame
    DdInqFrame		ARGS( (DtDevice) );
DtInt	
    DdInqNumFonts	ARGS( (DtDevice) );
void		
    DdInqPickAperture	ARGS( (DtDevice, DtSize3 *) );
DtObject	
    DdInqPickCallback	ARGS( (DtDevice) );
DtPickPathOrder	
    DdInqPickPathOrder	ARGS( (DtDevice) );
DtFlag		
    DdInqPixelData	ARGS( (DtDevice, DtRasterType,
			       DtInt *, DtInt *, 
		               DtRasterType *, DtPtr *,
			       DtFlag *) );
void		
    DdInqResolution	ARGS( (DtDevice, DtReal *, 
                               DtReal *) );
DtShadeMode	
    DdInqShadeMode	ARGS( (DtDevice) );
void	
    DdInqShadeRanges	ARGS( (DtDevice, DtInt, DtInt,
			       DtInt[]) );
void		
    DdInqViewport	ARGS( (DtDevice, DtVolume *) );
DtVisualType	
    DdInqVisualType	ARGS( (DtDevice) );
void		
    DdSetOptions	ARGS( (DtDevice, DtPtr) );
void		
    DdPickObjs		ARGS( (DtDevice, DtPoint3, DtInt *,
			       DtInt[], DtInt, DtInt[],
			       DtInt, DtReal[], DtReal[],
			       DtReal [], DtView[], 
                               DtInt *) );
/* NOTE: DdPick is obseleted by DdPickObjs */
void		
    DdPick		ARGS( (DtDevice, DtPoint3, DtInt *,
			       DtInt, DtInt[], DtInt, 
                               DtInt[], DtReal[], DtView[], 
                               DtInt *) );
void		
    DdSetColorEntries	ARGS( (DtDevice, DtColorModel,
			       DtInt, DtInt,
			       PROTO_REAL_LIST) );
void		
    DdSetFrame		ARGS( (DtDevice, DtFrame) );
void		
    DdSetPickAperture	ARGS( (DtDevice, DtSize3 *) );
void		
    DdSetPickCallback	ARGS( (DtDevice, DtObject) );
void		
    DdSetPickPathOrder	ARGS( (DtDevice, DtPickPathOrder) );
void		
    DdSetShadeMode	ARGS( (DtDevice, DtShadeMode) );
void		
    DdSetShadeRanges	ARGS( (DtDevice, DtInt, DtInt,
			       DtInt[]) );
void		
    DdSetViewport	ARGS( (DtDevice, DtVolume *) );
void		
    DdUpdate		ARGS( (DtDevice) );


/*
 * Frame Functions
 */
void		
    DfInqBoundary	ARGS( (DtFrame, DtVolume *) );
void		
    DfInqJust		ARGS( (DtFrame, DtReal *, 
                               DtReal *) );
DtGroup		
    DfInqViewGroup	ARGS( (DtFrame) );
void		
    DfSetBoundary	ARGS( (DtFrame, DtVolume *) );
void		
    DfSetJust		ARGS( (DtFrame, DtReal, DtReal) );

void		
    DfUpdate		ARGS( (DtFrame) );


/*
 * Group Functions
 */
void		
    DgAddObj		ARGS( (DtObject) );
void		
    DgAddObjToGroup	ARGS( (DtGroup, DtObject) );
DtGroupNetworkStatus 
    DgCheck		ARGS( (DtGroup) );
DtGroup		
    DgClose		ARGS( (void) );
void		
    DgDelEle		ARGS( (DtInt) );
DtFlag		
    DgDelEleBetweenLabels ARGS( (DtInt, DtInt) );
void		
    DgDelEleRange	ARGS( (DtInt, DtInt) );
void		
    DgEmpty		ARGS( (DtGroup) );
DtInt		
    DgInqElePtr		ARGS( (void) );
DtObject
    DgInqObjAtPos	ARGS( (DtGroup, DtInt,
			       DtRelPosition) );
DtGroup		
    DgInqOpen		ARGS( (void) );
DtInt		
    DgInqSize		ARGS( (DtGroup) );
void		
    DgOpen		ARGS( (DtGroup, DtFlag) );
void		
    DgReplaceObj	ARGS( (DtObject) );
void		
    DgReplaceObjInGroup	ARGS( (DtGroup, DtObject) );
void		
    DgSetElePtr		ARGS( (DtInt, DtRelPosition) );
DtFlag		
    DgSetElePtrRelLabel	ARGS( (DtInt, DtInt) );


/*
 * Object Creation Functions
 */
DtObject	
    DoAmbientIntens	ARGS( (DtReal) );

DtObject	
    DoAmbientSwitch	ARGS( (DtSwitch) );
DtObject	
    DoAnnoText		ARGS( (DtReal[], DtPtr) );
DtObject	
    DoBackfaceCullSwitch ARGS( (DtSwitch) );
DtObject	
    DoBackfaceCullable	ARGS( (DtSwitch) );
DtObject	
    DoBoundingVol	ARGS( (DtVolume *, DtObject) );
DtObject	
    DoBoundingVolSwitch	ARGS( (DtSwitch) );
DtObject	
    DoCallback		ARGS( (DtPtr, DtObject) );
DtObject	
    DoCamera		ARGS( (void) );
DtObject	
    DoCameraMatrix	ARGS( (DtMatrix4x4) );
DtObject	
    DoClipSwitch	ARGS( (DtSwitch) );
DtObject	
    DoClipVol		ARGS( (DtClipOperator, DtInt,
			       DtHalfSpace[]) );
DtObject	
    DoCompTextureUVCallback ARGS( (DtObject) );
DtObject	
    DoCompTextureUVSwitch ARGS( (DtSwitch) );
DtObject	
    DoDataPtr		ARGS( (DtPtr) );
DtObject	
    DoDataVal		ARGS( (Dt32Bits) );
DtObject	
    DoDepthCue		ARGS( (DtReal, DtReal, DtReal, 
			       DtReal, DtColorModel, 
			       DtReal[]) );
DtObject	
    DoDepthCueSwitch	ARGS( (DtSwitch) );
DtDevice	
    DoDevice		ARGS( (DtPtr, DtPtr) );
DtObject	
    DoDiffuseColor	ARGS( (DtColorModel, DtReal[]) );
DtObject	
    DoDiffuseIntens	ARGS( (DtReal) );
DtObject	
    DoDiffuseSwitch	ARGS( (DtSwitch) );
DtObject	
    DoExecSet		ARGS( (DtInt, DtInt[],
			       DtSetOperation) );
DtObject	
    DoFileRaster	ARGS( (DtPtr, DtPtr) );
DtObject	
    DoFilter		ARGS( (DtFilter, DtInt, DtInt[],
			       DtSetOperation) );
DtFrame		
    DoFrame		ARGS( (void) );
DtObject	
    DoGlbRendMaxObjs	ARGS( (DtInt) );
DtObject	
    DoGlbRendMaxSub	ARGS( (DtInt) );
DtObject	
    DoGlbRendRayLevel	ARGS( (DtInt) );
DtGroup		
    DoGroup		ARGS( (DtFlag) );
DtObject	
    DoHiddenSurfSwitch	ARGS( (DtSwitch) );
DtObject	
    DoInLineGroup	ARGS( (DtFlag) );
DtObject	
    DoInputSlot		ARGS( (void) );
DtObject	
    DoInterpType	ARGS( (DtInterpType) );
DtObject	
    DoInvisSwitch	ARGS( (DtSwitch) );
DtObject	
    DoLabel		ARGS( (DtInt) );
DtObject	
    DoLight		ARGS( (void) );
DtObject	
    DoLightSpreadAngles	ARGS( (DtReal, DtReal) );
DtObject	
    DoLightSpreadExp	ARGS( (DtReal) );
DtObject	
    DoLightAttenuation	ARGS( (DtReal, DtReal) );
DtObject	
    DoLightSwitch	ARGS( (DtObject, DtSwitch) );
DtObject	
    DoLightColor	ARGS( (DtColorModel, DtReal[]) );
DtObject	
    DoLightIntens	ARGS( (DtReal) );
DtObject	
    DoLightType		ARGS( (DtObject) );
DtObject	
    DoLineList		ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST) );
DtObject	
    DoLineType		ARGS( (DtLineType) );
DtObject	
    DoLineWidth		ARGS( (DtReal) );
DtObject	
    DoLocalAntiAliasSwitch ARGS( (DtSwitch) );
DtObject	
    DoLocalAntiAliasStyle ARGS( (DtLocalAntiAliasStyle) );
DtObject
    DoLookAtFrom	ARGS( (DtPoint3, DtPoint3,
			       DtVector3) );
DtObject	
    DoMarkerFont	ARGS( (DtFont) );
DtObject	
    DoMarkerGlyph	ARGS( (DtInt) );
DtObject	
    DoMarkerScale	ARGS( (DtReal) );
DtObject	
    DoMatrix		ARGS( (DtInt, DtInt,
			       PROTO_REAL_LIST) );
DtObject	
    DoMinBoundingVolExt	ARGS( (DtReal) );
DtObject	
    DoNURBSurf		ARGS( (DtColorModel, DtCtrlPointType,
			       DtNArea,
			       DtInt, DtInt, PROTO_REAL_LIST,
			       DtInt, DtInt, PROTO_REAL_LIST,
			       DtInt, DtInt, PROTO_REAL_LIST
			       ) );
DtObject	
    DoNameSet		ARGS( (DtInt, DtInt[],
			       DtSetOperation) );
DtObject	
    DoParallel		ARGS( (DtReal, DtReal, DtReal) );
DtObject	
    DoPatch		ARGS( (DtColorModel, DtVertexType,
			       DtObject, PROTO_REAL_LIST,
			       DtObject) );
DtObject	
    DoPerspective	ARGS( (DtReal, DtReal, DtReal) );
DtObject	
    DoPickID		ARGS( (DtInt) );
DtObject	
    DoPickSwitch	ARGS( (DtSwitch) );
DtObject	
    DoPointList		ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST) );
DtObject	
    DoPolygon		ARGS( (DtColorModel, DtVertexType,
			       DtInt, DtInt[],
			       PROTO_REAL_LIST,
			       DtShapeType) );




DtObject	
    DoPolygonMesh	ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST,
			       DtInt, DtInt[],
			       DtInt[], DtInt[],
			       DtShapeType, DtFlag) );
DtObject	
    DoPolyline		ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST) );
DtObject	
    DoPolymarker	ARGS( (DtInt, PROTO_REAL_ARRAY) );
DtObject	
    DoPopAtts		ARGS( (void) );
DtObject	
    DoPopMatrix		ARGS( (void) );
DtObject	
    DoPrimSurf		ARGS( (DtSurface) );
DtObject	
    DoProjection	ARGS( (DtArea *, DtProjectionType,
			       DtPoint3, DtReal,
			       DtReal, DtReal) );
DtObject	
    DoPushAtts		ARGS( (void) );
DtObject	
    DoPushMatrix	ARGS( (void) );
DtObject	
    DoRaster		ARGS( (DtInt, DtInt, DtInt, 
			       DtRasterType, DtPtr, 
                               DtPtr, DtObject) );
DtObject	
    DoReflectionSwitch	ARGS( (DtSwitch) );
DtObject	
    DoRefractionSwitch	ARGS( (DtSwitch) );
DtObject	
    DoRefractionIndex	ARGS( (DtReal) );
DtObject	
    DoRepType		ARGS( (DtRepType) );
DtObject	
    DoRotate		ARGS( (DtAxis, DtReal) );
DtObject	
    DoSampleAdaptive	ARGS( (DtReal) );
DtObject	
    DoSampleAdaptiveSwitch ARGS( (DtSwitch) );
DtObject	
    DoSampleFilter	ARGS( (DtObject, DtReal, DtReal) );
DtObject	
    DoSampleJitter	ARGS( (DtReal) );
DtObject	
    DoSampleJitterSwitch ARGS( (DtSwitch) );

DtObject	
    DoSampleSuper	ARGS( (DtInt, DtInt) );
DtObject	
    DoSampleSuperSwitch	ARGS( (DtSwitch) );
DtObject	
    DoScale		ARGS( (DtReal, DtReal, DtReal) );
DtObject	
    DoShadeIndex	ARGS( (DtInt) );
DtObject	
    DoShadowSwitch	ARGS( (DtSwitch) );
DtObject	
    DoShear		ARGS( (DtMajorPlane,
			       DtReal, DtReal) );
DtObject	
    DoSimplePolygon	ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST,
			       DtShapeType ) );
DtObject	
    DoSimplePolygonMesh	ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST,
			       DtInt, DtInt[], DtInt[],
			       DtShapeType, DtFlag) );
DtObject	
    DoSpecularColor	ARGS( (DtColorModel, DtReal[]) );
DtObject	
    DoSpecularFactor	ARGS( (DtReal) );
DtObject	
    DoSpecularIntens	ARGS( (DtReal) );
DtObject	
    DoSpecularSwitch	ARGS( (DtSwitch) );
DtObject        
    DoSphereList        ARGS( (DtColorModel, DtInt,
                               PROTO_REAL_ARRAY,
                               PROTO_REAL_LIST,
                               PROTO_REAL_ARRAY) );
DtObject	
    DoStereo		ARGS( (DtReal, DtReal) );
DtObject	
    DoStereoSwitch	ARGS( (DtSwitch) );
DtObject	
    DoSubDivSpec	ARGS( (DtInt, DtReal[]) );
DtObject	
    DoSurfaceEdgeColor	ARGS( (DtColorModel, DtReal[]) );
DtObject	
    DoSurfaceShade	ARGS( (DtObject) );
DtObject	
    DoText		ARGS( (DtPoint3, DtVector3, 
                               DtVector3, DtPtr) );


DtObject	
    DoTextAlign		ARGS( (DtTextAlignHorizontal,
			       DtTextAlignVertical) );
DtObject	
    DoTextExpFactor	ARGS( (DtReal) );
DtObject	
    DoTextFont		ARGS( (DtFont) );
DtObject	
    DoTextHeight	ARGS( (DtReal) );
DtObject	
    DoTextPath		ARGS( (DtTextPath) );
DtObject	
    DoTextPrecision	ARGS( (DtTextPrecision) );
DtObject	
    DoTextSpace		ARGS( (DtReal) );
DtObject	
   DoTextUpVector	ARGS( (DtReal, DtReal) );
DtObject	
   DoTextureAntiAlias	ARGS( (DtTextureAntiAliasMode) );
DtObject	
   DoTextureExtendUV	ARGS( (DtExtendMode, DtExtendMode) );
DtObject	
   DoTextureExtendUVW	ARGS( (DtExtendMode, DtExtendMode,
			       DtExtendMode) );
DtObject	
   DoTextureIntermediateSurf ARGS( (DtObject, 
                                    Dt2PartProjType) );
DtObject	
    DoTextureMapBump	ARGS( (DtMapOperator, DtObject, 
			       DtObject) );
DtObject	
    DoTextureMapBumpSwitch ARGS( (DtSwitch) );
DtObject	
    DoTextureMapDiffuseColor ARGS( (DtMapOperator, DtObject,
			            DtObject) );
DtObject	
    DoTextureMapDiffuseColorSwitch ARGS( (DtSwitch) );
DtObject	
    DoTextureMapEnviron	ARGS( (DtMapOperator, DtObject,
			       DtObject) );
DtObject	
    DoTextureMapEnvironSwitch ARGS( (DtSwitch) );
DtObject	
    DoTextureMapTranspIntens ARGS( (DtMapOperator, DtObject,
			            DtObject) );
DtObject	
    DoTextureMapTranspIntensSwitch ARGS( (DtSwitch) );
DtObject	
    DoTextureMatrixUV	ARGS( (DtMatrix3x3) );

DtObject	
    DoTextureMatrixUVW	ARGS( (DtMatrix4x4) );
DtObject	
    DoTextureOp		ARGS( (DtTextureOperator) );
DtObject	
    DoTextureScaleUV	ARGS( (DtReal, DtReal) );
DtObject	
    DoTextureScaleUVW	ARGS( (DtReal, DtReal, DtReal) );
DtObject	
    DoTextureTranslateUV ARGS( (DtReal, DtReal) );
DtObject	
    DoTextureTranslateUVW ARGS( (DtReal, DtReal, DtReal) );
DtObject	
    DoTextureUVIndex	ARGS( (DtInt) );
DtObject	
    DoTextureUVWIndex	ARGS( (DtInt) );
DtObject	
    DoTorus		ARGS( (DtReal, DtReal) );
DtObject	
    DoTransformMatrix	ARGS( (DtMatrix4x4, DtCompType) );
DtObject	
    DoTranslate		ARGS( (DtReal, DtReal, DtReal) );
DtObject	
    DoTranspColor	ARGS( (DtColorModel, DtReal[]) );
DtObject	
    DoTranspIntens	ARGS( (DtReal) );
DtObject	
    DoTranspSwitch	ARGS( (DtSwitch) );
DtObject	
    DoTranspOrientColor	ARGS( (DtColorModel, DtReal[]) );
DtObject	
    DoTranspOrientIntens ARGS( (DtReal) );
DtObject	
    DoTranspOrientExp	ARGS( (DtReal) );
DtObject	
    DoTranspOrientSwitch ARGS( (DtSwitch) );
DtObject	
    DoTriangleList	ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST) );
DtObject
    DoTriangleMesh	ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST,
			       DtInt, PROTO_INT_LIST,
			       DtFlag) );
DtObject	
    DoTriangleStrip	ARGS( (DtColorModel, DtVertexType,
			       DtInt, PROTO_REAL_LIST) );




DtObject	
    DoVarTriangleMesh	ARGS( (DtColorModel, DtInt, 
			       PROTO_REAL_LIST, 
			       PROTO_REAL_LIST,
			       PROTO_REAL_LIST,
			       DtInt, PROTO_INT_LIST,
			       DtFlag) );
DtObject	
    DoVarSimplePolygonMesh ARGS( (DtColorModel, DtInt, 
			          PROTO_REAL_LIST, 
			          PROTO_REAL_LIST,
			          PROTO_REAL_LIST,
			          DtInt, PROTO_INT_LIST,
			          PROTO_INT_LIST,
			          DtShapeType, DtFlag) );
DtObject	
    DoVarLineList	ARGS( (DtColorModel, DtInt, 
			       PROTO_REAL_LIST, 
			       PROTO_REAL_LIST,
			       PROTO_REAL_LIST) );
DtObject	
    DoVarPointList	ARGS( (DtColorModel, DtInt, 
			       PROTO_REAL_LIST, 
			       PROTO_REAL_LIST,
			       PROTO_REAL_LIST) );
DtObject	
    DoVarTriangleStrip	ARGS( (DtColorModel, DtInt, 
			       PROTO_REAL_LIST, 
			       PROTO_REAL_LIST,
			       PROTO_REAL_LIST) );
DtView		
    DoView		ARGS( (void) );

/*
 * Primitive Object Functions
 */
void		
    DpUpdVarTriangleMesh ARGS( (DtObject,
			        PROTO_REAL_LIST, 
			        PROTO_REAL_LIST,
			        PROTO_REAL_LIST,
			        DtFlag) );
void		
    DpUpdVarTriangleMeshUV ARGS( (DtObject,
			          DtInt,
			          PROTO_REAL_LIST) );
void		
    DpUpdVarTriangleMeshUVW ARGS( (DtObject,
			           DtInt,
			           PROTO_REAL_LIST) );
void		
    DpUpdVarSimplePolygonMesh ARGS( (DtObject,
         		             PROTO_REAL_LIST, 
				     PROTO_REAL_LIST,
				     PROTO_REAL_LIST,
				     DtShapeType, DtFlag,
				     DtFlag) );
void		
    DpUpdVarSimplePolygonMeshUV ARGS( (DtObject,
				       DtInt,
				       PROTO_REAL_LIST) );
void		
    DpUpdVarSimplePolygonMeshUVW ARGS( (DtObject,
 				        DtInt,
				        PROTO_REAL_LIST) );
void		
    DpUpdVarLineList	ARGS( (DtObject,
			       DtInt,
			       PROTO_REAL_LIST, 
			       PROTO_REAL_LIST,
			       PROTO_REAL_LIST) );
void		
    DpUpdVarPointList	ARGS( (DtObject,
			       DtInt,
			       PROTO_REAL_LIST, 
			       PROTO_REAL_LIST,
			       PROTO_REAL_LIST) );
void		
    DpUpdVarTriangleStrip ARGS( (DtObject,
			         DtInt,
			         PROTO_REAL_LIST, 
			         PROTO_REAL_LIST,
			         PROTO_REAL_LIST) );
void		
    DpUpdVarTriangleStripUV ARGS( (DtObject,
			           DtInt,
			           PROTO_REAL_LIST) );
void		
    DpUpdVarTriangleStripUVW ARGS( (DtObject,
				    DtInt,
				    PROTO_REAL_LIST) );


/*
 * System Functions
 */
DtVolume *	
    DsCompBoundingVol	ARGS( (DtVolume *, DtObject) );
void		
    DsExecuteObj	ARGS( (DtObject) );
void		
    DsExecutionAbort	ARGS( (void) );
void		
    DsExecutionReturn	ARGS( (void) );
DtInt		
    DsFileRasterRead	ARGS( (DtPtr, DtInt *, DtInt *,
			       DtInt *, DtRasterType *,
			       DtPtr *) );
DtObject	
    DsHoldObj		ARGS( (DtObject) );
void		
    DsInitialize	ARGS( (DtInt) );
void		
    DsInitializeSystem	ARGS( (DtInt) );
/* NOTE: DsInitializeSystem is obsoleted by DsInitialize */
void		
    DsInputValue	ARGS( (DtObject, DtReal) );
DtAngleUnits	
    DsInqAngleUnits	ARGS( (void) );
DtInt		
    DsInqClassId	ARGS( (DtPtr) );
DtInt		
    DsInqCurrentMethod	ARGS( (void) );
void		
    DsInqDriverInfo	ARGS( (DtInt count, DtPtr name[], 
			       DtPtr description[]) );
DtReadStatus	
    DsInqErrorMessage	ARGS( (DtInt, DtInt, char[],
			       DtErrorStatus *) );
void		
    DsInqErrorVars	ARGS( (DtInt *, DtPFI *) );
DtInt		
    DsInqExeDepthLimit	ARGS( (void) );
DtFlag		
    DsInqHoldObj	ARGS( (DtObject) );
DtInt		
    DsInqNumDrivers	ARGS( (void) );
DtInt		
    DsInqNumRenderers	ARGS( (void) );
DtInt		
    DsInqMethodId	ARGS( (DtPtr) );
DtObject	
    DsInqObj		ARGS( (DtNameType, DtPtr, DtInt) );
void		
    DsInqObjClassId	ARGS( (DtObject, DtNameType *,
			       DtPtr) );
void		
    DsInqObjName	ARGS( (DtObject, DtNameType *,
			       DtPtr) );

DtObjectStatus	
    DsInqObjStatus	ARGS( (DtObject) );
DtInt		
    DsInqObjType	ARGS( (DtObject) );
/* NOTE: DsInqObjType is obsoleted by DsInqObjClassId */
void		
    DsInqRaster		ARGS( (DtObject, DtInt *, DtInt *,
			       DtInt *, DtRasterType *, 
			       DtPtr *, DtPtr *) );
DtInt		
    DsInqRendererId	ARGS( (DtPtr) );
void		
    DsInqRendererNames	ARGS( (DtPtr []) );
DtFlag		
    DsInqSafeFlag	ARGS( (void) );
DtObject	
    DsInqValuatorGroup	ARGS( (DtObject) );
void		
    DsInqVersion	ARGS( (DtPtr *) );
void		
    DsPrintObj		ARGS( (DtObject) );
void		
    DsRasterUpdate	ARGS( (DtObject) );
DtInt	
    DsRasterWrite	ARGS( (DtObject, DtPtr) );
void		
    DsReleaseObj	ARGS( (DtObject) );
void	
    DsSetAngleUnits	ARGS( (DtAngleUnits units) );
void		
    DsSetErrorVars	ARGS( (DtInt, DtPFI) );
void		
    DsSetExeDepthLimit	ARGS( (DtInt) );
void		
    DsSetObjName	ARGS( (DtObject, DtNameType, DtPtr,
					       DtFlag) );
void		
    DsSetSafeFlag	ARGS( (DtFlag) );
void		
    DsTerminate		ARGS( (void) );
void		
    DsTerminateSystem	ARGS( (void) );
/* NOTE: DsTerminateSystem is obsoleted by DsTerminate */
DtVertexType	
    DsTextureUVCount	ARGS( (DtInt) );
DtVertexType	
    DsTextureUVWCount	ARGS( (DtInt) );
void		
    DsUpdateAllViews	ARGS( (void) );

void		
    DsValuatorSwitch	ARGS( (DtSwitch) );


/*
 * Viewport Functions
 */
DtObject	
    DvInqActiveCamera	ARGS( (DtView) );
void		
    DvInqBackgroundColor ARGS( (DtView, DtColorModel *,
			        DtReal[]) );
void		
    DvInqBackgroundJust	ARGS( (DtView, DtReal *, DtReal *) );
void		
    DvInqBackgroundRaster ARGS( (DtView, DtObject *, 
                                  DtObject *) );
void		
    DvInqBoundary	ARGS( (DtView, DtVolume *) );
DtFlag		
    DvInqClearFlag	ARGS( (DtView) );
DtGroup		
    DvInqDefinitionGroup ARGS( (DtView) );
DtGroup		
    DvInqDisplayGroup	ARGS( (DtView) );
DtRenderStyle	
    DvInqRendStyle	ARGS( (DtView) );
DtInt		
    DvInqShadeIndex	ARGS( (DtView) );
DtUpdateType	
    DvInqUpdateType	ARGS( (DtView) );
void		
    DvSetActiveCamera	ARGS( (DtView, DtObject) );
void		
    DvSetBackgroundColor ARGS( (DtView, DtColorModel,
			        DtReal[]) );
void		
    DvSetBackgroundJust	ARGS( (DtView, DtReal, DtReal) );
void		
    DvSetBackgroundRaster ARGS( (DtView, DtObject, 
                                 DtObject) );
void		
    DvSetBoundary 	ARGS( (DtView, DtVolume *) );
void		
    DvSetClearFlag	ARGS( (DtView, DtFlag) );
void		
    DvSetRendStyle	ARGS( (DtView, DtRenderStyle) );
void		
    DvSetShadeIndex	ARGS( (DtView, DtInt) );


void		
    DvSetUpdateType	ARGS( (DtView, DtUpdateType) );
void		
    DvUpdate		ARGS( (DtView) );

/*
 * User extension functions
 */
DtInt		
    DeAddClass		ARGS( (DtPtr, DtInt, DtInt[], 
                               DtPFI) );
DtObject	
    DeCreateObject	ARGS( (DtInt, DtPtr) );
void		
    DeDeleteObject	ARGS( (DtObject) );
void		
    DeExecuteAlternate	ARGS( (DtObject) );
void		
    DeInitializeObjPick	ARGS( (DtObject) );
DtFlag		
    DeInqPickable	ARGS( (DtInt) );
DtFlag		
    DeInqRenderable	ARGS( (DtInt) );

#endif
