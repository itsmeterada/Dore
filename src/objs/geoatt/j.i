typedef union _h_val {
  	unsigned long _i[2];
	double _d;
} _h_val;
extern const _h_val __huge_val;
extern double acos  ( double );
extern double asin  ( double );
extern double atan  ( double );
extern double atan2  ( double , double );
extern double cos  ( double );
extern double sin  ( double );
extern double tan  ( double );
extern double cosh  ( double );
extern double sinh  ( double );
extern double tanh  ( double );
extern double exp  ( double );
extern double frexp  ( double , int * );
extern double ldexp  ( double , int );
extern double log  ( double );
extern double log10  ( double );
extern double modf  ( double , double * );
extern double pow  ( double , double );
extern double sqrt  ( double );
extern double ceil  ( double );
extern double fabs  ( double );
extern double floor  ( double );
extern double fmod  ( double , double );
typedef unsigned int	size_t;
typedef long	fpos_t;
typedef struct	 
{
	int		_cnt;	 
	unsigned char	*_ptr;	 
	unsigned char	*_base;	 
	unsigned char	_flag;	 
	unsigned char	_file;	 
} FILE;
extern FILE		__iob[ 20 ];
extern FILE		*_lastbuf;
extern unsigned char	*_bufendtab[];
extern unsigned char	 _sibuf[], _sobuf[];
extern int	remove(const char *);
extern int	rename(const char *, const char *);
extern FILE	*tmpfile(void);
extern char	*tmpnam(char *);
extern int	fclose(FILE *);
extern int	fflush(FILE *);
extern FILE	*fopen(const char *, const char *);
extern FILE	*freopen(const char *, const char *, FILE *);
extern void	setbuf(FILE *, char *);
extern int	setvbuf(FILE *, char *, int, size_t);
extern int	fprintf(FILE *, const char *, ...);
extern int	fscanf(FILE *, const char *, ...);
extern int	printf(const char *, ...);
extern int	scanf(const char *, ...);
extern int	sprintf(char *, const char *, ...);
extern int	sscanf(const char *, const char *, ...);
extern int	vfprintf(FILE *, const char *, void *);
extern int	vprintf(const char *, void *);
extern int	vsprintf(char *, const char *, void *);
extern int	fgetc(FILE *);
extern char	*fgets(char *, int, FILE *);
extern int	fputc(int, FILE *);
extern int	fputs(const char *, FILE *);
extern int	getc(FILE *);
extern int	getchar(void);
extern char	*gets(char *);
extern int	putc(int, FILE *);
extern int	putchar(int);
extern int	puts(const char *);
extern int	ungetc(int, FILE *);
extern size_t	fread(void *, size_t, size_t, FILE *);
extern size_t	fwrite(const void *, size_t, size_t, FILE *);
extern int	fgetpos(FILE *, fpos_t *);
extern int	fseek(FILE *, long, int);
extern int	fsetpos(FILE *, const fpos_t *);
extern long	ftell(FILE *);
extern void	rewind(FILE *);
extern void	clearerr(FILE *);
extern int	feof(FILE *);
extern int	ferror(FILE *);
extern void	perror(const char *);
extern int	__filbuf(FILE *);
extern int	__flsbuf(int, FILE *);
typedef	struct {
	int	quot;
	int	rem;
} div_t;
typedef struct {
	long	quot;
	long	rem;
} ldiv_t;
typedef long	uid_t;
typedef long wchar_t;
extern unsigned char	__ctype[];
extern double atof(const char *);
extern int atoi(const char *);
extern long int atol(const char *);
extern double strtod(const char *, char **);
extern long int strtol(const char *, char **, int);
extern unsigned long int strtoul(const char *, char **, int);
extern int rand(void);
extern void srand(unsigned int);
extern void *calloc(size_t, size_t);
extern void free(void *);
extern void *malloc(size_t);
extern void *realloc(void *, size_t);
extern void abort(void);
extern int atexit(void (*)(void));
extern void exit(int);
extern char *getenv(const char *);
extern int system(const char *);
extern void *bsearch(const void *, const void *, size_t, size_t,
	int (*)(const void *, const void *));
extern void qsort(void *, size_t, size_t,
	int (*)(const void *, const void *));
extern int abs(int);
extern div_t div(int, int);
extern long int labs(long);
extern ldiv_t ldiv(long, long);
extern int mbtowc(wchar_t *, const char *, size_t);
extern int mblen(const char *, size_t);
extern int wctomb(char *, wchar_t);
extern size_t mbstowcs(wchar_t *, const char *, size_t);
extern size_t wcstombs(char *, const wchar_t *, size_t);
extern void *memcpy(void *, const void *, size_t);
extern void *memmove(void *, const void *, size_t);
extern char *strcpy(char *, const char *);
extern char *strncpy(char *, const char *, size_t);
extern char *strcat(char *, const char *);
extern char *strncat(char *, const char *, size_t);
extern int memcmp(const void *, const void *, size_t);
extern int strcmp(const char *, const char *);
extern int strcoll(const char *, const char *);
extern int strncmp(const char *, const char *, size_t);
extern size_t strxfrm(char *, const char *, size_t);
extern void *memchr(const void *, int, size_t);
extern char *strchr(const char *, int);
extern size_t strcspn(const char *, const char *);
extern char *strpbrk(const char *, const char *);
extern char *strrchr(const char *, int);
extern size_t strspn(const char *, const char *);
extern char *strstr(const char *, const char *);
extern char *strtok(char *, const char *);
extern void *memset(void *, int, size_t);
extern char *strerror(int);
extern char *strsignal(int);
extern size_t strlen(const char *);
    typedef          int  DtInt;	 
    typedef unsigned int  DtUInt;	 
    typedef unsigned int  Dt32Bits;	 
    typedef float DtReal;
typedef void          *DtPtr;	    
typedef DtInt        (*DtPFI)();    
typedef void         (*DtPFV)();    
typedef short          DtShort;
typedef unsigned short DtUShort;
typedef unsigned char  DtUChar;
typedef DtReal DtRealCouple[2];	 
typedef DtReal DtRealTriple[3];	 
typedef DtReal DtRealQuad[4];	 
typedef DtInt  DtIntTriple[3];	 
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
    DtPtr   data;		 
    DtPtr  *additional_data;
} DtObjectStructure;		 
typedef DtObjectStructure *DtObject;
typedef enum {		 
    DcFalse = 0,
    DcTrue
} DtFlag;
typedef enum {		 
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
typedef enum {		 
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
typedef DtInt DtVertexType;
typedef DtPtr (*DtMethodPtr)();	 
typedef struct {
    DtInt       id;		 
    DtMethodPtr routine;	 
} DtMethodEntry;
extern DtObject DcBezier4;
extern DtObject DcHermite4;
extern DtObject DcBSpline4;
extern DtObject DcLightAmbient;
extern DtObject DcLightInfinite;
extern DtObject DcLightPoint;
extern DtObject DcLightPointAttn;
extern DtObject DcLightSpot;
extern DtObject DcLightSpotAttn;
extern DtObject DcShaderConstant;
extern DtObject DcShaderLightSource;
extern DtObject DcPickFirst;
extern DtObject DcPickAll;
extern DtObject DcPickClosest;
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
extern DtObject DcStdBumpMap;
extern DtObject DcStdSphereEnvironMap;
extern DtObject DcStdCubeEnvironMap;
extern DtObject DcStdTableLookup;
extern DtObject DcStd3dTableLookup;
extern DtObject Dc2PartMap;
extern DtObject DcStdUVCallback;
extern DtObject DcStdUVWCallback;
extern DtObject DcFilterBox;
extern DtObject DcDeleteData;
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
void	 DfInqBoundary	(DtObject, DtVolume *);
void     DfInqJust	(DtObject, DtReal*, DtReal*);
DtObject DfInqViewGroup	(DtObject);
void	 DfSetBoundary	(DtObject, DtVolume *);
void	 DfSetJust	(DtObject, DtReal, DtReal);
void	 DfUpdate	(DtObject);
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
DtVolume*      DsCompBoundingVol     (DtVolume*, DtObject);
void	       DsExecuteObj	     (DtObject);
void	       DsExecutionAbort	     (void);
void	       DsExecutionReturn     (void);
DtInt	       DsFileRasterRead	     (DtPtr, DtInt*, DtInt*, DtInt*,
					DtRasterType*, DtPtr*);
DtObject       DsHoldObj	     (DtObject);
void DsInitFunction (DtInt processors, int dtreal_precision);
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
void	       DsTerminateSystem     (void);
DtVertexType   DsTextureUVCount	     (DtInt);
DtVertexType   DsTextureUVWCount     (DtInt);
void	       DsUpdateAllViews	     (void);
void	       DsValuatorSwitch	     (DtSwitch);
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
DtInt	 DeAddClass	     (char*, DtInt, DtMethodEntry[], DtMethodPtr);
DtObject DeCreateObject	     (DtInt, DtPtr);
void	 DeDeleteObject	     (DtObject);
void	 DeExecuteAlternate  (DtObject);
void	 DeInitializeObjPick (DtObject);
DtFlag	 DeInqPickable	     (DtInt);
DtFlag	 DeInqRenderable     (DtInt);
void  DDerror         (DtInt number, char* funcname, char* string);
char* DDerror_GetLine (void);
typedef struct mapdifclr_stdattributes {
    DtTextureAntiAliasMode aamode;
    DtTextureOperator      operator;
    DtObject               surface_object;
    Dt2PartProjType        projection_type;
    struct {
        DtInt        idx;
        DtExtendMode extend[2];
        DtMatrix3x3  matrix;
    } uv;
    struct {
        DtInt        idx;
        DtExtendMode extend[3];
        DtMatrix4x4  matrix;
    }uvw;
} dot_stdtexatt;
typedef DtInt DDt_DeviceInterface;
typedef struct {	 
    DtInt version_number;
    DtPtr  (*create_local_data) (DtObject);
    DtFlag (*initialize_device) (DtObject, DtPtr, DtPtr, DtPtr);
    void   (*become_current_driver) (DtPtr);
    void   (*close_device) (void);
    void   (*flush) (void);
    void   (*swap_buffers) (void);
    void   (*update_geometry) (void);
    void   (*inquire_device_extent) (DtVolume *);
    void   (*inquire_stereo) (DtFlag *);
    void   (*inquire_ncolors) (DtInt *);
    void   (*inquire_resolution) (DtReal *, DtReal *);
    void   (*inquire_visual_type) (DtVisualType *);
    void   (*inquire_auto_size) (DtFlag*);
    void   (*inquire_color_entries) (DtColorModel, DtInt, DtInt, DtReal[]);
    void   (*set_color_entries) (DtColorModel, DtInt, DtInt, DtReal[]);
    void   (*set_foreground_color) (DtColorModel, DtReal[]);
    void   (*set_background_color) (DtColorModel, DtReal[]);
    void   (*set_shade_index) (DtInt);
    void   (*inquire_clip_list) (DtUInt*, DtReal**, DtReal[4], DtFlag*);
    void   (*clear_rectangle_depth) (DtShort, DtShort, DtShort, DtShort);
    void   (*clear_rectangle_depth_and_color) (DtShort,DtShort,DtShort,DtShort);
    void   (*set_depth_buffer_enable) (DtFlag);
    void   (*set_depth_buffer_write) (DtFlag);
    void   (*write_scanline_byte) (DtInt, DtInt, DtInt, DtUChar *);
    void   (*set_current_view) (DtObject);
    DtFlag (*inquire_pixel_data) (DtRasterType, DtInt*, DtInt*, DtRasterType*,
				  DtPtr*, DtFlag*);
    void   (*set_options) (DtPtr);
} DDt_DCM;
typedef struct {	 
    DtInt version_number;
    void (*write_rectangle_byte_rgb) (DtInt, DtInt, DtInt, DtInt, DtUChar*);
} DDt_PROM;
typedef struct {	 
    DtInt version_number;
    void (*pre_initialization)	(void);
    void (*post_initialization)  (void);
    DtPtr (*create_local_device_data)  (DtObject);
    DtPtr (*create_local_view_data)  (DtObject);
    DtPtr (*create_local_window_data)  (DtObject, DtObject);
    void (*start_update)  (DtObject, DtObject, DtPtr, DtPtr, DtPtr);
    void (*update_local_data)  (void);
    void (*update_studio)  (DtPFI);
    void (*update_display)  (DtPFI);
    void (*camera)  (void);
    void (*light)  (DtObject);
    void (*set_camera_matrix)  (DtMatrix4x4);
    void (*set_parallel_matrix)  (DtReal, DtReal, DtReal);
    void (*set_perspective_matrix)  (DtReal, DtReal, DtReal);
    void (*set_projection_matrix)  (
	     DtArea*, DtProjectionType, DtPoint3, DtReal, DtReal, DtReal);
    void (*push_att_clpvol)  (void);
    void (*pop_att_clpvol)  (void);
    void (*set_att_clpvol)  (DtClipOperator, DtInt, DtHalfSpace*);
    void (*apply_att_clpvol)  (DtClipOperator, DtInt, DtHalfSpace*);
    void   (*get_wcstofcsmat)  (DtMatrix4x4);
    void   (*pop_lcstofcsmat)  (void);
    void   (*push_lcstofcsmat)  (DtMatrix4x4);
    DtFlag (*transform_clip_z_point)  (DtPoint3, DtMatrix4x4);
    void (*render_point_list) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	     DtRealTriple*, DtInt, DtRealCouple**, DtInt, DtRealTriple**,
	     DtReal*);
    void (*render_line_list) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	     DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	     DtRealTriple**, DtReal*);
    void (*render_connected_line_list) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*,
	     DtRealTriple*, DtRealTriple*, DtRealTriple*, DtInt,
	     DtRealCouple**, DtInt, DtRealTriple**, DtReal*);
    void (*render_triangle_list) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtRealTriple*,
	     DtRealTriple*, DtRealTriple*, DtInt, DtRealCouple**, DtInt,
	     DtRealTriple**, DtReal*);
    void (*render_triangle_mesh) (
	     DtColorModel, DtRealTriple[8], DtInt, DtRealTriple*, DtInt,
	     DtRealTriple*, DtRealTriple*, DtRealTriple*, DtInt[][3],
	     DtInt[][3], DtInt, DtRealCouple**, DtInt, DtRealTriple**, DtReal*);
    void (*set_att_ambint)	(DtReal);
    void (*set_att_ambswi)	(DtSwitch);
    void (*set_att_bacfacculble)(DtSwitch);
    void (*set_att_bacfacculswi)(DtSwitch);
    void (*set_att_clpswi)	(DtSwitch);
    void (*set_att_depcue)	(DtReal,DtReal, DtReal,DtReal, DtColorModel,
				 DtReal[]);
    void (*set_att_depcueswi)	(DtSwitch);
    void (*set_att_difclr)	(DtColorModel, DtReal[]);
    void (*set_att_difint)	(DtReal);
    void (*set_att_difswi)	(DtSwitch);
    void (*set_att_hidsrfswi)	(DtSwitch);
    void (*set_att_inttyp)	(DtInterpType);
    void (*set_att_lgtclr)	(DtColorModel, DtReal[]);
    void (*set_att_lgtint)	(DtReal);
    void (*set_att_lgttyp)	(DtObject);
    void (*set_att_lintyp)	(DtLineType);
    void (*set_att_linwid)	(DtReal);
    void (*set_att_refswi)	(DtSwitch);
    void (*set_att_reptyp)	(DtRepType);
    void (*set_att_shaswi)	(DtSwitch);
    void (*set_att_shdidx)	(DtInt);
    void (*set_att_spcclr)	(DtColorModel, DtReal[]);
    void (*set_att_spcfct)	(DtReal);
    void (*set_att_spcint)	(DtReal);
    void (*set_att_spcswi)	(DtSwitch);
    void (*set_att_srfshd)	(DtObject);
    void (*set_att_stereo)	(DtReal, DtReal);
    void (*set_att_stereoswi)	(DtSwitch);
    void (*set_att_transpclr)	(DtColorModel, DtReal[]);
    void (*set_att_transpint)	(DtReal);
    void (*set_att_transpswi)	(DtSwitch);
    void (*update_lcstowcsmat_lokatfrm)(DtPoint3,DtPoint3,DtVector3,DtCompType);
    void (*update_lcstowcsmat_pop)     (void);
    void (*update_lcstowcsmat_push)    (void);
    void (*update_lcstowcsmat_rotate)  (DtAxis, DtReal, DtCompType);
    void (*update_lcstowcsmat_scale)   (DtReal, DtReal, DtReal, DtCompType);
    void (*update_lcstowcsmat_shear)   (DtMajorPlane,DtReal,DtReal,DtCompType);
    void (*update_lcstowcsmat_tfmmat)  (DtMatrix4x4, DtCompType);
    void (*update_lcstowcsmat_transl)  (DtReal, DtReal, DtReal, DtCompType);
    void (*set_att_lgtswi)	(DtInt, DtObject*);
    void (*set_att_lgtspdexp)	(DtReal);
    void (*set_att_lgtspdang)	(DtReal, DtReal);
    void (*set_att_lgtatn)	(DtReal, DtReal);
    void (*set_att_mapbmpswi)	(DtSwitch);
    void (*set_att_mapbmp)	(DtInt, DtObject[],DtObject[], dot_stdtexatt[]);
    void (*set_att_mapdifclrswi)(DtSwitch);
    void (*set_att_mapdifclr)	(DtInt, DtObject[],DtObject[], dot_stdtexatt[]);
    void (*set_att_mapenvswi)	(DtSwitch);
    void (*set_att_mapenv)	(DtInt, DtObject[],DtObject[], dot_stdtexatt[]);
    void (*set_att_maptrnintswi)(DtSwitch);
    void (*set_att_maptrnint)	(DtInt, DtObject[],DtObject[], dot_stdtexatt[]);
    void (*get_lcstowcsmat)	(DtMatrix4x4);
    void (*delete_local_device_data) (DtPtr);
    void (*delete_local_view_data)   (DtPtr);
    void (*delete_local_window_data) (DtPtr);
    DtFlag use_separate_wireframe;
    void (*set_att_srfedgclr)  (DtColorModel, DtReal[]);
    void (*set_att_localaaswi) (DtSwitch);
    void (*set_att_localaasty) (DtLocalAntiAliasStyle);
    void (*set_att_transporientclr) (DtColorModel, DtReal[]);
    void (*set_att_transporientexp) (DtReal);
    void (*set_att_transporientint) (DtReal);
    void (*set_att_transporientswi) (DtSwitch);
    void (*set_att_smpadapt) (DtReal);
    void (*set_att_smpadaptswi) (DtSwitch);
    void (*set_att_smpfilter) (DtObject, DtReal, DtReal);
    void (*set_att_smpjitter) (DtReal);
    void (*set_att_smpjitterswi) (DtSwitch);
    void (*set_att_smpsuper) (DtInt, DtInt);
    void (*set_att_smpsuperswi) (DtSwitch);
} DDt_DROM;
typedef struct {
    char        *name;
    DtMethodPtr  routine;
} DDt_DROMMethod;
DtInt       DDclass_AddMethod            (char*);
DtInt       DDclass_AddInitializedMethod (char*, DtMethodPtr);
void        DDclass_AddNotify            (DtPFV);
DtInt       DDclass_AddObjectData        (DtInt, DtMethodPtr, DtMethodPtr,
					  DtMethodPtr);
DtInt       DDclass_CopyMethod           (DtInt, char*);
DtMethodPtr DDclass_InqDefaultMethod     (DtInt);
DtMethodPtr DDclass_InqMethod            (DtInt, DtInt);
DtInt       DDclass_ReplaceMethod        (char*, DtInt, DtInt);
void        DDclass_SetMethod            (DtInt, DtInt, DtMethodPtr);
DtFlag      DDcondex_QueryExecute            (DtInt);
DtFlag      DDcondex_QueryRender             (DtInt);
void        DDcondex_SetDisablePrimExecFlag  (DtFlag);
DtFlag      DDcondex_QueryPrimExecFlag       (void);
void        DDdevice_InstallDriver        (char*, char*, DtPFV);
DDt_DCM*    DDdevice_CreateDCMStruct      (void);
DDt_PROM*   DDdevice_CreatePROMStruct     (void);
DDt_DROM*   DDdevice_CreateDROMStruct     (void);
void        DDdevice_ClearRectangleDepth  (DtObject, DtInt,DtInt, DtInt,DtInt);
void        DDdevice_ClearRectangleDepthColor(DtObject,DtInt,DtInt,DtInt,DtInt);
void        DDdevice_Flush                (DtObject);
DtFlag      DDdevice_InqAbortedDraw       (DtObject);
void        DDdevice_InqClipList          (DtObject, DtUInt*, DtReal**,
					   DtReal[4], DtFlag*);
DtVolume*   DDdevice_InqClippingVolume    (DtObject);
void        DDdevice_InqFrameScaleTrans   (DtObject, DtReal[3], DtReal[3]);
void        DDdevice_InqInterfaceRoutines (DtObject,DDt_DeviceInterface,DtPtr*);
void        DDdevice_InqFrustumScaleTrans (DtObject, DtReal[3], DtReal[3]);
void        DDdevice_InqShadeRange        (DtObject, DtInt, DtInt*, DtInt*);
DtFlag      DDdevice_InqStereo            (DtObject);
DtVolume*   DDdevice_InqActualViewport    (DtObject);
void        DDdevice_SetAbortedDraw       (DtObject, DtFlag);
void        DDdevice_SetBackgroundColor   (DtObject, DtColorModel, DtReal[]);
void        DDdevice_SetDepthBufferEnable (DtObject, DtFlag);
void        DDdevice_SetDepthBufferWrite  (DtObject, DtFlag);
void        DDdevice_SetForegroundColor   (DtObject, DtColorModel, DtReal[]);
void        DDdevice_SetShadeIndex        (DtObject, DtInt);
char*       DDgetenv (char*);
void        DDobject_AddReference     (DtObject, DtObject);
void        DDobject_DeleteReference  (DtObject, DtObject);
DtMethodPtr DDobject_InqCurrentMethod (DtObject);
void        DDobject_Delete           (DtObject);
DtMethodPtr DDobject_InqDefaultMethod (DtObject);
DtMethodPtr DDobject_InqMethod        (DtObject, DtInt);
void        DDprint            (char*);
char*       DDprint_GetLine    (void);
void        DDprint_IndentUp   (void);
void        DDprint_IndentDown (void);
DtInt       DDrender_AddRenderer
		(DtInt, char*, DtMethodPtr, DtMethodPtr, DtPFV, DtMethodPtr);
DtPtr       DDspace_Allocate   (DtInt);
DtPtr       DDspace_Reallocate (DtPtr, unsigned);
void        DDspace_Deallocate (DtPtr);
char*       DDstrtok (char*, char*);
DtInt       DDsystem_InqNumberProcessors (void);
DtObject    DDview_ResizeRaster    (DtObject, DtInt, DtInt);
DtFlag      DDview_TraverseDisplay (DtObject, DtInt);
DtFlag      DDview_TraverseStudio  (DtObject, DtInt);
void         dor_error_initialize        (void);
void         dor_error                   (DtInt, char*, char*);
char*        dor_error_get_line          (void);
DtReadStatus dor_error_inquire_message   (DtInt, DtInt, char[], DtErrorStatus*);
void         dor_error_inquire_variables (DtInt*, DtPFV*);
void         dor_error_set_variables     (DtInt, DtPFV, DtFlag);
DtReal  dor_math_acos  (DtReal value);
DtReal  dor_math_angle  (DtReal x, DtReal y);
DtReal  dor_math_asin  (DtReal value);
DtReal  dor_math_atan  (DtReal value);
DtReal  dor_math_atan2  (DtReal x, DtReal y);
DtReal  dor_math_ceil  (DtReal value);
DtReal  dor_math_cos  (DtReal value);
void    dor_math_cross_cosines
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		 DtReal, DtReal*, DtReal*, DtReal*);
void    dor_math_crossproduct
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal*,
		 DtReal*, DtReal*);
DtReal  dor_math_distance  (DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtReal  dor_math_distance_squared  (DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
DtReal  dor_math_dotproduct  (DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtReal  dor_math_fabs  (DtReal arg);
void    dor_math_fast_cross_cosines  (DtReal[3],DtReal[3],DtReal[3],DtReal[3]);
DtFlag  dor_math_fast_normal  (DtReal[3], DtReal[3], DtReal[3]);
DtReal  dor_math_fast_sqrt  (DtReal x);
DtReal  dor_math_floor  (DtReal value);
void    dor_math_homo_trns  (DtReal, DtReal, DtReal, DtReal, DtMatrix4x4,
			     DtReal*, DtReal*, DtReal*, DtReal*);
void    dor_math_homo_trns_fast  (DtReal[4], DtMatrix4x4, DtReal[4]);
DtFlag  dor_math_line_intersect
		(DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
void    dor_math_line_intersect_point
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		 DtReal, DtReal*, DtReal*);
DtReal  dor_math_log  (DtReal value);
DtFlag  dor_math_normal  (DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
			  DtReal*, DtReal*, DtReal*);
DtFlag  dor_math_pixel_window
		(DtReal, DtReal, DtReal, DtReal, DtInt, DtReal, DtReal, DtReal,
		 DtReal, DtInt*, DtInt*, DtInt*, DtInt*);
DtFlag  dor_math_plane_equation
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal, DtReal,
		 DtReal, DtReal*, DtReal*, DtReal*, DtReal*);
void    dor_math_pnttrns
		(DtReal,DtReal,DtReal,DtMatrix4x4,DtReal*,DtReal*,DtReal*);
void    dor_math_pnttrns_fast  (DtReal[3], DtMatrix4x4, DtReal[3]);
DtReal  dor_math_point_line_distance
	       (DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
void    dor_math_polygon_normal_fast  (DtInt, DtRealTriple*, DtReal[3]);
void    dor_math_polygon_plane_equation
		(DtInt, DtRealTriple*, DtReal*, DtReal*, DtReal*, DtReal*);
DtReal  dor_math_pow  (DtReal x, DtReal y);
DtReal  dor_math_projected_polygon_area (DtInt, DtRealTriple *, DtInt, DtInt);
DtReal  dor_math_projected_polygon_area_fast  (DtInt,DtRealTriple*,DtInt,DtInt);
DtReal  dor_math_projected_triangle_area_squared
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtReal  dor_math_projected_triangle_area_squared_fast
		(DtReal, DtReal, DtReal, DtReal, DtReal, DtReal);
DtFlag  dor_math_renormalize  (DtReal*, DtReal*, DtReal*);
DtFlag  dor_math_renormalize_fast  (DtReal normal[3]);
DtFlag  dor_math_renormalize_vector  (DtVector3 vector);
DtShort dor_math_same_side
		(DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal,DtReal);
DtReal  dor_math_sin  (DtReal value);
DtReal  dor_math_sqrt  (DtReal value);
void    dor_math_vectrns
		(DtReal,DtReal, DtReal, DtMatrix4x4, DtReal*, DtReal*, DtReal*);
void    dor_math_vectrns_fast  (DtReal[3], DtMatrix4x4, DtReal[3]);
void    dor_print_initialize  (void);
void    dor_print_output  (char *);
char*   dor_print_get_line  (void);
void    dor_print_clear  (char *, int);
void    dor_print_indent_down  (void);
void    dor_print_indent_up  (void);
void    dor_print_set_indent  (int);
void    dor_print_set_field  (char *, int, char *);
DtFlag  dor_quadmsh_check_degen_quad_vtx_values (
		DtReal[3], DtReal[3], DtReal[3], DtReal[3]);
DtPtr   dor_space_allocate  (unsigned);
DtPtr   dor_space_reallocate  (DtPtr, unsigned);
void    dor_space_deallocate  (DtPtr);
DtFlag  dor_util_bigendian_word_byte_order  (void);
char*   dor_util_getenv  (char *);
char*   dor_util_strtok  (char *, char *);
typedef struct  
{
    int    type;            
    int    numchar;         
    int    firstchar;       
    int    char_width;      
    int    baseline;        
    int    capline;         
    int    topline;         
    int    bottomline;      
    int    nsegs;           
    int    npoints;         
    int   *widths;          
    int   *nsegments;       
    int   *first_segment;   
    int   *first_point;     
    int   *x_coord;         
    int   *y_coord;         
} dot_font;
typedef struct 
{   DtReal x;
    DtReal y;
} dot_point2d ;
typedef struct 
{
  char         *string;   	 
  DtInt         length;  	 
  dot_point2d   position; 	 
  DtReal   	height;   	 
  DtReal   	charexp;  	 
  DtTextPath  	path;     	 
  DtReal   	xup;		 
  DtReal   	yup; 		 
  DtReal   	spacing;   	 
  DtFont    	font_index;
  DtTextAlignHorizontal halign;	 
  DtTextAlignVertical 	valign;	 
  char         *user_data; 	 
  DtPFV         polyline; 	 
  DtPFV         polygon; 	 
} dot_font_state ;
typedef struct object   dot_object;    
typedef struct txtupvec {
    DtReal xup;
    DtReal yup;
} dot_txtupvec;
typedef struct txtaln {
    DtTextAlignHorizontal halign;
    DtTextAlignVertical valign;
} dot_txtaln;
typedef struct object_reference {
    struct object_reference *next;
    dot_object              *object;
} dot_object_reference;
typedef struct {
   DtReal width;
   DtReal height;
   DtReal depth;
} dot_size ;
void        dor_alternate_object_execute  (dot_object *);
dot_object* dor_calbak_create             (DtPFI, DtObject, DtFlag);
void        dor_calbak_execute            (dot_object *);
dot_object* dor_calbak_get_dataobject     (dot_object *);
DtPFI       dor_calbak_get_function       (dot_object *);
DtFlag      dor_calbak_get_pass_by_ref    (dot_object *);
void        dor_calbak_initialize         (void);
void        dor_calbak_print              (dot_object *);
void	    dor_calbak_destroy		  (dot_object *);
DtFlag      dor_check_degen_triangle_vtx_values (DtReal[3],DtReal[3],DtReal[3]);
DtFlag      dor_check_degen_quad_vtx_values 
		    (DtReal[3],DtReal[3],DtReal[3],DtReal[3]);
DtInt       dor_class_add  (char*, DtInt, DtMethodEntry[], DtMethodPtr);
void        dor_class_add_notify  (DtPFV);
DtInt       dor_class_add_object_data
		    (DtInt, DtMethodPtr, DtMethodPtr, DtMethodPtr);
DtInt       dor_class_all_add_initialized_method
		    (char*, DtInt, DtMethodEntry[], DtMethodPtr);
DtInt       dor_class_all_add_method   (char*, DtInt, DtMethodEntry[]);
DtInt       dor_class_all_copy_method
		    (DtInt, char*, DtInt, DtMethodEntry[]);
DtFlag      dor_class_all_install_method
		    (DtInt, DtInt, DtMethodEntry[], DtInt, DtMethodPtr);
DtInt       dor_class_all_replace_method
		    (char*, DtInt, DtInt, DtMethodEntry[], DtInt);
dot_object *dor_class_create  (DtInt, DtInt, DtMethodEntry[], DtMethodPtr);
void        dor_class_delete  (dot_object *);
DtMethodPtr dor_class_inq_default_method  (DtInt);
DtInt       dor_class_inq_id  (char*);
DtMethodPtr dor_class_inq_method  (DtInt, DtInt);
DtInt       dor_class_install  (DtInt,char*,DtInt,DtMethodEntry[],DtMethodPtr);
void        dor_class_print  (dot_object *);
void        dor_class_set_method  (DtInt, DtInt, DtMethodPtr);
void        dor_condex_initialize                   (void);
DtFlag      dor_condex_query_execute                (DtInt);
DtFlag      dor_condex_query_render                 (DtInt);
DtFlag      dor_condex_query_pick                   (DtInt);
void        dor_condex_set_disable_prim_exec_flag   (DtFlag);
DtFlag      dor_condex_query_disable_prim_exec_flag (void);
dot_object* dor_datptr_create             (DtPtr);
DtPtr       dor_datptr_get_value          (dot_object *);
void        dor_datptr_initialize         (void);
void        dor_datptr_print              (dot_object *);
void	    dor_datptr_destroy		  (dot_object *);
dot_object* dor_datval_create             (Dt32Bits);
Dt32Bits    dor_datval_get_value          (dot_object *);
void        dor_datval_initialize         (void);
void        dor_datval_print              (dot_object *);
void	    dor_datval_destroy		  (dot_object *);
struct dictionary
	   *dor_dictionary_create           (void);
void        dor_dictionary_set_name_integer (struct dictionary *, dot_object*,
					     DtInt, DtFlag);
void        dor_dictionary_set_name_string  (struct dictionary *, dot_object*,
					     char*, DtFlag);
DtNameType  dor_dictionary_inq_name_type    (struct dictionary *, dot_object*);
DtInt       dor_dictionary_inq_name_integer (struct dictionary *, dot_object*);
char*       dor_dictionary_inq_name_string  (struct dictionary *, dot_object*);
dot_object *dor_dictionary_inq_obj_integer  (struct dictionary *, DtInt, DtInt);
dot_object *dor_dictionary_inq_obj_string   (struct dictionary *, char*, DtInt);
dot_object* dor_fil_create          (DtFilter, DtInt, DtInt[], DtSetOperation);
void        dor_fil_execute         (dot_object *);
void        dor_fil_initialize      (void);
void        dor_fil_print           (dot_object *);
dot_object* dor_filerstr_create     (char*, char*);
void        dor_filerstr_destroy    (dot_object *);
void        dor_filerstr_initialize (void);
void        dor_filerstr_print      (dot_object *);
void        dor_font_draw_text      (dot_font_state *);
dot_font*   dor_font_get_font       (DtInt);
int         dor_font_get_type       (dot_font_state *);
void        dor_font_initialize     (void);
void        dor_font_inquire_bounds (DtInt, DtInt *, DtInt *);
dot_font*   dor_font_load           (DtInt);
char*       dor_get_font_name       (DtInt);
void        dor_frame_add_reference     (dot_object *, dot_object *);
dot_object* dor_frame_create            (void);
void        dor_frame_delete            (dot_object *);
void        dor_frame_initialize        (void);
void        dor_frame_inq_boundary      (dot_object *, DtVolume *);
DtVolume*   dor_frame_inq_boundary_ptr  (dot_object *);
void        dor_frame_inq_justification (dot_object *, DtReal *, DtReal *);
dot_object* dor_frame_inq_view_group    (dot_object *);
void        dor_frame_pick              (dot_object *, dot_object *);
void        dor_frame_print             (dot_object *);
void        dor_frame_remove_reference  (dot_object *, dot_object *);
void        dor_frame_set_boundary      (dot_object *, DtVolume *);
void        dor_frame_set_justification (dot_object *, DtReal, DtReal);
void        dor_frame_update            (dot_object *);
void        dor_frame_update_device     (dot_object *, dot_object *);
void        dor_frame_update_notify     (dot_object *);
void        dor_frame_update_view       (dot_object *, dot_object *);
void        dor_gas_initialize          (void);
struct group;	 
void        dor_group_add_object            (dot_object *);
void        dor_group_add_object_into       (dot_object *, dot_object *);
void        dor_group_add_reference         (dot_object *, dot_object *);
DtGroupNetworkStatus
	    dor_group_check                 (dot_object *);
void        dor_group_check_network         (dot_object *);
dot_object* dor_group_close                 (void);
dot_object* dor_group_create                (DtFlag, DtFlag);
DtFlag      dor_group_delete_between_labels (DtInt, DtInt);
void        dor_group_delete_element_range  (DtInt, DtInt);
void        dor_group_delete_elements       (DtInt);
void        dor_group_destroy               (dot_object *);
void        dor_group_empty                 (dot_object *);
void        dor_group_execute_current       (dot_object *);
DtInt       dor_group_find_label            (DtInt);
dot_object* dor_group_get_current_group     (void);
void        dor_group_initialize            (void);
DtInt       dor_group_inq_element_pointer   (void);
dot_object* dor_group_inq_open              (void);
dot_object_reference* dor_group_inq_references (dot_object *);
DtInt       dor_group_inq_size              (dot_object *);
dot_object* dor_group_inquire_object_at_position
					    (dot_object*, DtInt, DtInt);
void        dor_group_insert_element        (dot_object *, dot_object *);
void        dor_group_open                  (dot_object *, DtFlag);
void        dor_group_pick                  (dot_object *);
void        dor_group_pop                   (DtFlag);
void        dor_group_print                 (dot_object *);
void        dor_group_push                  (dot_object *, DtFlag);
void        dor_group_record_attribute      (dot_object *, DtPFV);
void        dor_group_remove_reference      (dot_object *, dot_object *);
void        dor_group_replace               (dot_object *);
void        dor_group_replace_element       (dot_object *, dot_object *);
void        dor_group_replace_in            (dot_object *, dot_object *);
DtFlag      dor_group_set_ele_ptr_rel_label (DtInt, DtInt);
void        dor_group_set_element_pointer   (DtInt, DtInt);
void        dor_group_table_add_block       (void);
void        dor_group_table_delete_group    (struct group *);
void        dor_group_table_initialize      (void);
void        dor_group_table_print           (void);
void        dor_group_traversal_abort       (void);
void        dor_group_traversal_return      (void);
DtInt       drr_glbrnd_install_renderer  (DtInt);
FILE*       dor_io_file_open_read  (char *);
FILE*       dor_io_file_open_write  (char *);
int         dor_io_open_read  (char *);
int         dor_io_open_write  (char *);
int         dor_io_read  (int, char*, unsigned int);
void        dor_io_write  (int, char*, unsigned int);
int         dor_io_file_close  (FILE *);
int         dor_io_close  (int);
dot_object* dor_label_create            (DtInt);
void        dor_label_initialize        (void);
void        dor_label_print             (dot_object *);
dot_object* dor_namset_create           (DtInt, DtInt [], DtSetOperation);
void        dor_namset_execute          (dot_object *);
void        dor_namset_initialize       (void);
void        dor_namset_print            (dot_object *);
DtObject    dor_null_create             (DtPtr);
void       *dor_null_default_method     (dot_object *);
void        dor_null_initialize         (void);
dot_object* dor_mapbmpswi_create        (DtSwitch);
void        dor_mapbmpswi_execute       (dot_object *);
void        dor_mapbmpswi_initialize    (void);
void        dor_mapbmpswi_print         (dot_object *);
dot_object* dor_mapdifclrswi_create     (DtSwitch);
void        dor_mapdifclrswi_execute    (dot_object *);
void        dor_mapdifclrswi_initialize (void);
void        dor_mapdifclrswi_print      (dot_object *);
dot_object* dor_mapenvswi_create        (DtSwitch);
void        dor_mapenvswi_execute       (dot_object *);
void        dor_mapenvswi_initialize    (void);
void        dor_mapenvswi_print         (dot_object *);
dot_object* dor_maptrnintswi_create     (DtSwitch);
void        dor_maptrnintswi_execute    (dot_object *);
void        dor_maptrnintswi_initialize (void);
void        dor_maptrnintswi_print      (dot_object *);
void        dor_object_add_reference      (dot_object *, dot_object *);
void        dor_object_allocate           (void);
void        dor_object_check_deletion     (dot_object *);
DtFlag      dor_object_confirm_type       (dot_object *, DtInt);
dot_object* dor_object_create             (DtInt, DtPtr);
void        dor_object_delete_reference   (dot_object *, dot_object *);
DtMethodPtr dor_object_get_current_method (dot_object *);
DtMethodPtr dor_object_get_method         (dot_object *, DtInt);
dot_object* dor_object_hold               (dot_object *);
void        dor_object_initialize         (void);
DtInt       dor_object_inq_class_type     (dot_object *);
DtMethodPtr dor_object_inq_default_method (dot_object *);
dot_object* dor_object_inq_via_integer    (DtInt, DtInt);
dot_object* dor_object_inq_via_string     (char*, DtInt);
DtFlag      dor_object_inq_hold           (dot_object *);
DtInt       dor_object_inq_method_id      (char*);
DtNameType  dor_object_inq_name_type      (dot_object*);
DtInt       dor_object_inq_name_integer   (dot_object*);
char*       dor_object_inq_name_string    (dot_object*);
int         dor_object_inq_ntypes         (void);
DtInt       dor_object_inq_type           (dot_object *);
DtFlag      dor_object_method_id_valid    (DtInt);
void       *dor_object_method_null        (dot_object *);
void        dor_object_print              (dot_object *);
void        dor_object_print_method_table (void);
void        dor_object_print_type         (dot_object *);
void        dor_object_reference_insert   (dot_object_reference**, dot_object*);
void        dor_object_reference_remove   (dot_object_reference**, dot_object*);
void        dor_object_release            (dot_object *);
void        dor_object_set_name_integer   (dot_object*, DtInt, DtFlag);
void        dor_object_set_name_string    (dot_object*, char*, DtFlag);
void        dor_object_terminate          (void);
DtInt       dor_object_validate           (dot_object *);
dot_object* dor_pckid_create              (DtInt);
void        dor_pckid_execute             (dot_object *);
void        dor_pckid_initialize          (void);
void        dor_pckid_print               (dot_object *);
void        dor_pick_connected_line_list  (DtRealTriple[8],DtInt,DtRealTriple*);
void        dor_pick_initialize  (void);
void        dor_pick_initialize_camera  (dot_object *);
void        dor_pick_initialize_element  (DtFlag);
void        dor_pick_initialize_object  (dot_object *);
void        dor_pick_line_list  (DtRealTriple [8], DtInt, DtReal [][3]);
void        dor_pick_point_list  (DtRealTriple [8], DtInt, DtRealTriple *);
void        dor_pick_pop_group  (void);
void        dor_pick_pop_lcstofcsmat  (void);
void        dor_pick_push_group  (dot_object*);
void        dor_pick_push_lcstofcsmat  (DtMatrix4x4);
void        dor_pick_set_camera_matrix  (DtMatrix4x4);
void        dor_pick_set_parallel_matrix  (DtReal, DtReal, DtReal);
void        dor_pick_set_perspective_matrix  (DtReal, DtReal, DtReal);
void        dor_pick_set_projection_matrix  (DtArea*, DtProjectionType,
					     DtPoint3, DtReal, DtReal, DtReal);
void        dor_pick_setup  (DtObject, DtInt*, DtInt, DtInt[], DtInt, long[],
			     DtReal[],DtReal[],DtReal[],dot_object*[],DtInt*);
void        dor_pick_terminate_element  (void);
DtFlag      dor_pick_transform_clip_z_point  (DtRealTriple, DtMatrix4x4);
void        dor_pick_triangle_list  (DtRealTriple[8], DtInt, DtRealTriple*);
void        dor_pick_triangle_mesh  (DtRealTriple[8], DtInt, DtInt,
				     DtRealTriple*, DtInt[][3]);
void        dor_pick_triangle_strip  (DtRealTriple[8], DtInt, DtRealTriple*);
void        dor_pick_view  (dot_object*, dot_object*);
dot_object* dor_popatt_create             (void);
void        dor_popatt_execute            (dot_object *);
void        dor_popatt_initialize         (void);
void        dor_popatt_print              (dot_object *);
dot_object* dor_pshatt_create             (void);
void        dor_pshatt_destroy            (dot_object *);
void        dor_pshatt_execute            (dot_object *);
void        dor_pshatt_initialize         (void);
void        dor_pshatt_print              (dot_object *);
DtInt       dor_render_add_renderer (
                  DtInt, char*, DtMethodPtr, DtMethodPtr, DtPFV, DtMethodPtr);
void        dor_render_get_wcstofcsmat    (DtMatrix4x4);
void        dor_render_initialize         (void);
DtInt       dor_render_inq_num_renderers  (void);
DtInt       dor_render_inq_renderer_id    (char*);
void        dor_render_inq_renderer_names (char*[]);
void        dor_render_render             (dot_object *, dot_object *);
dot_object* dor_rstr_create               (DtInt, DtInt, DtInt, DtRasterType,
					   DtPtr, DtPtr, DtObject);
void        dor_rstr_destroy              (dot_object *);
void        dor_rstr_init_delete_callback (void);
void        dor_rstr_initialize           (void);
void        dor_rstr_inq                  (dot_object*, DtInt*, DtInt*, DtInt*,
					   DtRasterType*, char**, DtPtr*);
void        dor_rstr_print                (dot_object *);
void        dor_rstr_update               (dot_object *);
void        dor_slot_activate             (dot_object *, DtReal);
dot_object* dor_slot_create               (void);
void        dor_slot_destroy              (dot_object *);
void        dor_slot_enqueue_input        (dot_object *, DtReal);
void        dor_slot_flush_input          (void);
void        dor_slot_initialize           (void);
void        dor_slot_input_value          (dot_object *, DtReal);
dot_object* dor_slot_inq_valuator_group   (dot_object *);
void        dor_slot_print                (dot_object *);
dot_object* dor_texaa_create            (DtTextureAntiAliasMode);
void        dor_texaa_destroy           (dot_object *);
void        dor_texaa_execute           (dot_object *);
void        dor_texaa_initialize        (void);
void        dor_texaa_print             (dot_object *);
dot_object* dor_texextuv_create         (DtExtendMode, DtExtendMode);
void        dor_texextuv_destroy        (dot_object *);
void        dor_texextuv_execute        (dot_object *);
void        dor_texextuv_initialize     (void);
void        dor_texextuv_print          (dot_object *);
dot_object* dor_texextuvw_create       (DtExtendMode,DtExtendMode,DtExtendMode);
void        dor_texextuvw_destroy       (dot_object *);
void        dor_texextuvw_execute       (dot_object *);
void        dor_texextuvw_initialize    (void);
void        dor_texextuvw_print         (dot_object *);
dot_object* dor_texintsrf_create        (DtObject, Dt2PartProjType);
void        dor_texintsrf_destroy       (dot_object *);
void        dor_texintsrf_execute       (dot_object *);
void        dor_texintsrf_initialize    (void);
void        dor_texintsrf_print         (dot_object *);
dot_object* dor_texmatuv_create         (DtMatrix3x3);
void        dor_texmatuv_destroy        (dot_object *);
void        dor_texmatuv_execute        (struct object *);
void        dor_texmatuv_initialize     (void);
void        dor_texmatuv_print          (dot_object *);
dot_object* dor_texmatuvw_create        (DtMatrix4x4);
void        dor_texmatuvw_destroy       (dot_object *);
void        dor_texmatuvw_execute       (struct object *);
void        dor_texmatuvw_initialize    (void);
void        dor_texmatuvw_print         (dot_object *);
dot_object* dor_texop_create            (DtTextureOperator);
void        dor_texop_destroy           (dot_object *);
void        dor_texop_execute           (dot_object *);
void        dor_texop_initialize        (void);
void        dor_texop_print             (dot_object *);
dot_object* dor_texscluv_create         (DtReal, DtReal);
void        dor_texscluv_destroy        (dot_object *);
void        dor_texscluv_execute        (dot_object *);
void        dor_texscluv_initialize     (void);
void        dor_texscluv_print          (dot_object *);
dot_object* dor_texscluvw_create        (DtReal, DtReal, DtReal);
void        dor_texscluvw_destroy       (dot_object *);
void        dor_texscluvw_execute       (dot_object *);
void        dor_texscluvw_initialize    (void);
void        dor_texscluvw_print         (dot_object *);
dot_object* dor_textrnuv_create         (DtReal, DtReal);
void        dor_textrnuv_destroy        (dot_object *);
void        dor_textrnuv_execute        (dot_object *);
void        dor_textrnuv_initialize     (void);
void        dor_textrnuv_print          (dot_object *);
dot_object* dor_textrnuvw_create        (DtReal, DtReal, DtReal);
void        dor_textrnuvw_destroy       (dot_object *);
void        dor_textrnuvw_execute       (dot_object *);
void        dor_textrnuvw_initialize    (void);
void        dor_textrnuvw_print         (dot_object *);
dot_object* dor_texuvidx_create         (DtInt);
void        dor_texuvidx_destroy        (dot_object *);
void        dor_texuvidx_execute        (dot_object *);
void        dor_texuvidx_initialize     (void);
void        dor_texuvidx_print          (dot_object *);
dot_object* dor_texuvwidx_create        (DtInt);
void        dor_texuvwidx_destroy       (dot_object *);
void        dor_texuvwidx_execute       (dot_object *);
void        dor_texuvwidx_initialize    (void);
void        dor_texuvwidx_print         (dot_object *);
void        dor_utilpri_print_vertextype  (DtVertexType);
void        dor_view_add_reference        (dot_object *, dot_object *);
dot_object *dor_view_create               (void);
void        dor_view_delete               (dot_object *);
void        dor_view_initialize           (void);
dot_object *dor_view_inq_active_camera    (dot_object*);
void        dor_view_inq_background_color (dot_object*, DtColorModel*,DtReal[]);
void        dor_view_inq_background_just  (dot_object*, DtReal*, DtReal*);
void        dor_view_inq_background_raster(dot_object*, dot_object**,
					   dot_object**);
void        dor_view_inq_boundary         (dot_object*, DtVolume*);
DtVolume   *dor_view_inq_boundary_ptr     (dot_object*);
DtFlag      dor_view_inq_clear_flag       (dot_object*);
dot_object *dor_view_inq_definition_group (dot_object*);
dot_object *dor_view_inq_display_group    (dot_object*);
void        dor_view_inq_formfac_comptype (dot_object*, DtFormFactorCompType*,
					   DtReal*);
void        dor_view_inq_rad_convergence  (dot_object*, DtRadiosityConvType*,
					   DtReal*);
DtRadiosityUpdateType
	    dor_view_inq_rad_updatetype   (dot_object*);
void        dor_view_inq_render_data      (dot_object*, DtInt, DtPtr*);
DtRenderStyle
	    dor_view_inq_render_style     (dot_object*);
void        dor_view_inq_sequence_number  (dot_object*, DtInt*);
DtInt       dor_view_inq_shade_index      (dot_object*);
DtUpdateType
	    dor_view_inq_update_type     (dot_object*);
void        dor_view_print                (dot_object*);
void        dor_view_remove_reference     (dot_object*, dot_object *);
DtObject    dor_view_resize_raster        (dot_object*, DtInt, DtInt);
void        dor_view_set_active_camera    (dot_object*, dot_object*);
void        dor_view_set_background_color (dot_object*, DtColorModel, DtReal[]);
void        dor_view_set_background_just  (dot_object*, DtReal, DtReal);
void        dor_view_set_background_raster(dot_object*,dot_object*,dot_object*);
void        dor_view_set_boundary         (dot_object*, DtVolume*);
void        dor_view_set_clear_flag       (dot_object*, DtFlag);
void        dor_view_set_formfac_comptype (dot_object*, DtFormFactorCompType,
					   DtReal*);
void        dor_view_set_rad_convergence  (dot_object*, DtRadiosityConvType,
					   DtReal*);
void        dor_view_set_rad_updatetype   (dot_object*, DtRadiosityUpdateType);
void        dor_view_set_render_data      (dot_object*, DtInt, DtPtr);
void        dor_view_set_render_style     (dot_object*, DtRenderStyle);
void        dor_view_set_shade_index      (dot_object*, DtInt);
void        dor_view_set_update_type      (dot_object*, DtUpdateType);
DtFlag      dor_view_traverse_display     (dot_object*, DtInt);
DtFlag      dor_view_traverse_studio      (dot_object*, DtInt);
void        dor_view_update               (dot_object*);
void        dor_view_update_notify        (dot_object*);
void        dpr_radiosity_initialize  (void);
DtFlag      dpr_radiosity_render      (dot_object*, dot_object*, DtInt, DtPFI);
void        dre_dynrnd_reset_current_display_method  (DtInt);
DtInt       drr_dynrnd_install_renderer  (DtInt);
void        fortran_call_error_handler  (DtPFV, DtInt, DtInt, char*, char*);
dot_object*    dor_lokatfrm_create  (DtPoint3, DtPoint3, DtVector3);
void           dor_lokatfrm_destroy  (dot_object *);
void           dor_lokatfrm_execute  (dot_object *);
void           dor_lokatfrm_initialize  (void);
void           dor_lokatfrm_print  (dot_object *);
struct object* dor_popmat_create  (void);
void           dor_popmat_execute  (dot_object *);
void           dor_popmat_initialize  (void);
void           dor_popmat_print  (dot_object *);
dot_object*    dor_pshmat_create  (void);
void           dor_pshmat_dynexe  (dot_object *);
void           dor_pshmat_execute  (dot_object *);
void           dor_pshmat_initialize  (void);
void           dor_pshmat_print  (dot_object *);
struct object* dor_rotate_create  (DtAxis, DtReal);
void           dor_rotate_destroy  (dot_object *);
void           dor_rotate_execute  (struct object *);
void           dor_rotate_initialize  (void);
void           dor_rotate_print  (dot_object *);
dot_object*    dor_scale_create  (DtReal, DtReal, DtReal);
void           dor_scale_destroy  (dot_object *);
void           dor_scale_execute  (struct object *);
void           dor_scale_initialize  (void);
void           dor_scale_print  (dot_object *);
dot_object*    dor_shear_create  (DtMajorPlane, DtReal, DtReal);
void           dor_shear_destroy  (dot_object *);
void           dor_shear_execute  (struct object *);
void           dor_shear_initialize  (void);
void           dor_shear_print  (dot_object *);
dot_object*    dor_tfmmat_create  (DtMatrix4x4, DtCompType);
void           dor_tfmmat_destroy  (dot_object *);
void           dor_tfmmat_execute  (struct object *);
void           dor_tfmmat_initialize  (void);
void           dor_tfmmat_print  (dot_object *);
dot_object*    dor_transl_create  (DtReal, DtReal, DtReal);
void           dor_transl_destroy  (dot_object *);
void           dor_transl_execute  (dot_object *);
void           dor_transl_initialize  (void);
void           dor_transl_print  (dot_object *);
typedef struct {
    DtInt        halfspacecount;	 
    DtHalfSpace *halfspaces;		 
    DtPoint4    *plneqns;		 
} dot_modclpvol;
void           dor_modclpvol_apply_volume   (dot_modclpvol*, DtMatrix4x4,
					     DtClipOperator,DtInt,DtHalfSpace*);
dot_modclpvol *dor_modclpvol_copy           (dot_modclpvol*);
dot_modclpvol *dor_modclpvol_create         (DtMatrix4x4, DtClipOperator, DtInt,
					     DtHalfSpace*);
void           dor_modclpvol_destroy        (dot_modclpvol*);
void           dor_modclpvol_get_value      (dot_modclpvol*, DtInt*, DtPoint3*,
					     DtVector3*, DtPoint4*);
void           dor_modclpvol_plane_equation (dot_modclpvol*,DtInt,DtMatrix4x4);
void           dor_modclpvol_print          (dot_modclpvol*);
void           dor_modclpvol_set_value      (dot_modclpvol*, DtMatrix4x4,
					     DtClipOperator,DtInt,DtHalfSpace*);
typedef struct {
    DtShort   ints;
    DtShort   objs;
    Dt32Bits *bitset;
} dot_exeset;
void        dor_exeset_copy_exeset    (dot_exeset*, dot_exeset*);
dot_object* dor_exeset_create         (DtInt, DtInt[], DtSetOperation);
dot_exeset* dor_exeset_create_exeset  (DtInt);
void        dor_exeset_destroy        (dot_object*);
void        dor_exeset_destroy_exeset (dot_exeset*);
void        dor_exeset_execute        (dot_object*);
void        dor_exeset_initialize     (void);
void        dor_exeset_print          (dot_object*);
typedef Dt32Bits dot_bitfield[ 8 ];
void   dor_bitfield_add               (dot_bitfield, dot_bitfield, DtInt);
void   dor_bitfield_copy              (dot_bitfield, dot_bitfield, DtInt);
DtPtr  dor_bitfield_create            (void);
void   dor_bitfield_delete            (dot_bitfield, dot_bitfield, DtInt);
void   dor_bitfield_destroy           (dot_bitfield);
void   dor_bitfield_invert            (dot_bitfield, dot_bitfield, DtInt);
void   dor_bitfield_load              (dot_bitfield,DtInt,DtInt[],DtInt,DtInt);
void   dor_bitfield_print_with_string (char*, dot_bitfield, DtInt);
DtFlag dor_bitfield_test_intersect    (dot_bitfield, dot_bitfield, DtInt);
DtFlag dor_bitfield_test_membership   (dot_bitfield, DtInt);
void   dor_bitfield_unload            (dot_bitfield, DtInt*, DtInt[]);
typedef dot_bitfield dot_filters[ 6 ];
dot_filters *dor_filters_create             (void);
void         dor_filters_destroy            (dot_filters);
void         dor_filters_copy               (dot_filters, dot_filters);
DtFlag       dor_filters_test_intersections (dot_bitfield, dot_bitfield,
					     dot_bitfield);
struct cammat;
struct global_depcue_data;
struct global_intermdisptype_data;
struct global_lgtatn_data;
struct global_lgtspdang_data;
struct global_lgtswi_data;
struct global_mapbmp_data;
struct global_mapdifclr_data;
struct global_mapenv_data;
struct global_maptrnint_data;
struct global_smpfilter_data;
struct global_smpsuper_data;
struct global_stereo_data;
struct global_subdivspc_data;
struct global_texextuv_data;
struct global_texextuvw_data;
struct global_texintsrf_data;
struct texmatuv;
struct texmatuvw;
void        dor_global_ambint_destroy_element (DtReal*);
DtReal*     dor_global_ambint_get_data_pointer (void);
DtFlag      dor_global_ambint_get_value (DtReal*);
void        dor_global_ambint_initialize (void);
void        dor_global_ambint_pop_group (void);
DtFlag      dor_global_ambint_pop_value (void);
void        dor_global_ambint_push_value (DtReal);
void        dor_global_ambint_set_value (DtReal);
void        dor_global_ambint_update_execute (void);
void        dor_global_ambswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_ambswi_get_data_pointer (void);
DtFlag      dor_global_ambswi_get_value (DtSwitch*);
void        dor_global_ambswi_initialize (void);
void        dor_global_ambswi_pop_group (void);
DtFlag      dor_global_ambswi_pop_value (void);
void        dor_global_ambswi_push_value (DtSwitch);
void        dor_global_ambswi_set_value (DtSwitch);
void        dor_global_ambswi_update_execute (void);
void        dor_global_bacfacculble_destroy_element (DtSwitch*);
DtSwitch*   dor_global_bacfacculble_get_data_pointer (void);
DtFlag      dor_global_bacfacculble_get_value (DtSwitch*);
void        dor_global_bacfacculble_initialize (void);
void        dor_global_bacfacculble_pop_group (void);
DtFlag      dor_global_bacfacculble_pop_value (void);
void        dor_global_bacfacculble_push_value (DtSwitch);
void        dor_global_bacfacculble_set_value (DtSwitch);
void        dor_global_bacfacculble_update_execute (void);
void        dor_global_bacfacculswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_bacfacculswi_get_data_pointer (void);
DtFlag      dor_global_bacfacculswi_get_value (DtSwitch*);
void        dor_global_bacfacculswi_initialize (void);
void        dor_global_bacfacculswi_pop_group (void);
DtFlag      dor_global_bacfacculswi_pop_value (void);
void        dor_global_bacfacculswi_push_value (DtSwitch);
void        dor_global_bacfacculswi_set_value (DtSwitch);
void        dor_global_bacfacculswi_update_execute (void);
void        dor_global_bndvolswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_bndvolswi_get_data_pointer (void);
DtFlag      dor_global_bndvolswi_get_value (DtSwitch*);
void        dor_global_bndvolswi_initialize (void);
void        dor_global_bndvolswi_pop_group (void);
DtFlag      dor_global_bndvolswi_pop_value (void);
void        dor_global_bndvolswi_push_value (DtSwitch);
void        dor_global_bndvolswi_set_value (DtSwitch);
void        dor_global_bndvolswi_update_execute (void);
void        dor_global_cammat_destroy_element (struct cammat*);
struct cammat*
            dor_global_cammat_get_data_pointer (void);
DtFlag      dor_global_cammat_get_value (DtCameraMatrixType* , DtMatrix4x4);
void        dor_global_cammat_initialize (void);
void        dor_global_cammat_pop_group (void);
DtFlag      dor_global_cammat_pop_value (void);
void        dor_global_cammat_print_value (void);
void        dor_global_cammat_push_value (DtCameraMatrixType, DtMatrix4x4);
void        dor_global_cammat_set_value (DtCameraMatrixType, DtMatrix4x4);
void        dor_global_cammat_update_execute (void);
void        dor_global_clpswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_clpswi_get_data_pointer (void);
DtFlag      dor_global_clpswi_get_value (DtSwitch*);
void        dor_global_clpswi_initialize (void);
void        dor_global_clpswi_pop_group (void);
DtFlag      dor_global_clpswi_pop_value (void);
void        dor_global_clpswi_push_value (DtSwitch);
void        dor_global_clpswi_set_value (DtSwitch);
void        dor_global_clpswi_update_execute (void);
void        dor_global_clpvol_apply_volume (DtClipOperator,DtInt,DtHalfSpace*);
void        dor_global_clpvol_destroy_element (dot_modclpvol*);
dot_modclpvol*
            dor_global_clpvol_get_data_pointer (void);
DtFlag      dor_global_clpvol_get_value (DtInt*,DtPoint3*,DtVector3*,DtPoint4*);
void        dor_global_clpvol_initialize (void);
void        dor_global_clpvol_notify_change (void);
void        dor_global_clpvol_pop_group (void);
DtFlag      dor_global_clpvol_pop_value (void);
void        dor_global_clpvol_push_apply_volume (
		DtClipOperator, DtInt, DtHalfSpace*);
void        dor_global_clpvol_push_value (DtClipOperator, DtInt, DtHalfSpace*);
void        dor_global_clpvol_set_value (DtClipOperator, DtInt, DtHalfSpace*);
void        dor_global_clpvol_update_execute (DtGlobalCall, DtInt);
void        dor_global_cmptexuvcbk_destroy_element (DtObject*);
DtObject*   dor_global_cmptexuvcbk_get_data_pointer (void);
DtFlag      dor_global_cmptexuvcbk_get_value (DtObject*);
void        dor_global_cmptexuvcbk_initialize (void);
void        dor_global_cmptexuvcbk_pop_group (void);
DtFlag      dor_global_cmptexuvcbk_pop_value (void);
void        dor_global_cmptexuvcbk_push_value (DtObject);
void        dor_global_cmptexuvcbk_set_value (DtObject);
void        dor_global_cmptexuvcbk_update_execute (void);
void        dor_global_cmptexuvswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_cmptexuvswi_get_data_pointer (void);
DtFlag      dor_global_cmptexuvswi_get_value (DtSwitch*);
void        dor_global_cmptexuvswi_initialize (void);
void        dor_global_cmptexuvswi_pop_group (void);
DtFlag      dor_global_cmptexuvswi_pop_value (void);
void        dor_global_cmptexuvswi_push_value (DtSwitch);
void        dor_global_cmptexuvswi_set_value (DtSwitch);
void        dor_global_cmptexuvswi_update_execute (void);
void        dor_global_cmptexuvwcbk_destroy_element (DtObject*);
DtObject*   dor_global_cmptexuvwcbk_get_data_pointer (void);
DtFlag      dor_global_cmptexuvwcbk_get_value (DtObject*);
void        dor_global_cmptexuvwcbk_initialize (void);
void        dor_global_cmptexuvwcbk_pop_group (void);
DtFlag      dor_global_cmptexuvwcbk_pop_value (void);
void        dor_global_cmptexuvwcbk_push_value (DtObject);
void        dor_global_cmptexuvwcbk_set_value (DtObject);
void        dor_global_cmptexuvwcbk_update_execute (void);
void        dor_global_cmptexuvwswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_cmptexuvwswi_get_data_pointer (void);
DtFlag      dor_global_cmptexuvwswi_get_value (DtSwitch*);
void        dor_global_cmptexuvwswi_initialize (void);
void        dor_global_cmptexuvwswi_pop_group (void);
DtFlag      dor_global_cmptexuvwswi_pop_value (void);
void        dor_global_cmptexuvwswi_push_value (DtSwitch);
void        dor_global_cmptexuvwswi_set_value (DtSwitch);
void        dor_global_cmptexuvwswi_update_execute (void);
void        dor_global_depcue_destroy_element (struct global_depcue_data*);
struct global_depcue_data*
            dor_global_depcue_get_data_pointer (void);
DtFlag      dor_global_depcue_get_value
                (DtReal*, DtReal*, DtReal*, DtReal*, DtColorModel*, DtReal[3]);
void        dor_global_depcue_initialize (void);
void        dor_global_depcue_pop_group (void);
DtFlag      dor_global_depcue_pop_value (void);
void        dor_global_depcue_push_value
                (DtReal, DtReal, DtReal, DtReal, DtColorModel, DtReal[]);
void        dor_global_depcue_set_value
                (DtReal, DtReal, DtReal, DtReal, DtColorModel, DtReal[]);
void        dor_global_depcue_update_execute (void);
void        dor_global_depcueswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_depcueswi_get_data_pointer (void);
DtFlag      dor_global_depcueswi_get_value (DtSwitch*);
void        dor_global_depcueswi_initialize (void);
void        dor_global_depcueswi_pop_group (void);
DtFlag      dor_global_depcueswi_pop_value (void);
void        dor_global_depcueswi_push_value (DtSwitch);
void        dor_global_depcueswi_set_value (DtSwitch);
void        dor_global_depcueswi_update_execute (void);
void        dor_global_difclr_destroy_element (DtColorRGB*);
DtColorRGB* dor_global_difclr_get_data_pointer (void);
DtFlag      dor_global_difclr_get_value (DtColorModel* , DtReal[3]);
void        dor_global_difclr_initialize (void);
void        dor_global_difclr_pop_group (void);
DtFlag      dor_global_difclr_pop_value (void);
void        dor_global_difclr_push_value (DtColorRGB);
void        dor_global_difclr_set_value (DtColorRGB);
void        dor_global_difclr_update_execute (void);
void        dor_global_difint_destroy_element (DtReal*);
DtReal*     dor_global_difint_get_data_pointer (void);
DtFlag      dor_global_difint_get_value (DtReal*);
void        dor_global_difint_initialize (void);
void        dor_global_difint_pop_group (void);
DtFlag      dor_global_difint_pop_value (void);
void        dor_global_difint_push_value (DtReal);
void        dor_global_difint_set_value (DtReal);
void        dor_global_difint_update_execute (void);
void        dor_global_difswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_difswi_get_data_pointer (void);
DtFlag      dor_global_difswi_get_value (DtSwitch*);
void        dor_global_difswi_initialize (void);
void        dor_global_difswi_pop_group (void);
DtFlag      dor_global_difswi_pop_value (void);
void        dor_global_difswi_push_value (DtSwitch);
void        dor_global_difswi_set_value (DtSwitch);
void        dor_global_difswi_update_execute (void);
void        dor_global_emisclr_destroy_element (DtColorRGB*);
DtColorRGB* dor_global_emisclr_get_data_pointer (void);
DtFlag      dor_global_emisclr_get_value (DtColorModel* , DtReal[3]);
void        dor_global_emisclr_initialize (void);
void        dor_global_emisclr_pop_group (void);
DtFlag      dor_global_emisclr_pop_value (void);
void        dor_global_emisclr_push_value (DtColorRGB);
void        dor_global_emisclr_set_value (DtColorRGB);
void        dor_global_emisclr_update_execute (void);
void        dor_global_emisfct_destroy_element (DtReal*);
DtReal*     dor_global_emisfct_get_data_pointer (void);
DtFlag      dor_global_emisfct_get_value (DtReal*);
void        dor_global_emisfct_initialize (void);
void        dor_global_emisfct_pop_group (void);
DtFlag      dor_global_emisfct_pop_value (void);
void        dor_global_emisfct_push_value (DtReal);
void        dor_global_emisfct_set_value (DtReal);
void        dor_global_emisfct_update_execute (void);
void        dor_global_emisswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_emisswi_get_data_pointer (void);
DtFlag      dor_global_emisswi_get_value (DtSwitch*);
void        dor_global_emisswi_initialize (void);
void        dor_global_emisswi_pop_group (void);
DtFlag      dor_global_emisswi_pop_value (void);
void        dor_global_emisswi_push_value (DtSwitch);
void        dor_global_emisswi_set_value (DtSwitch);
void        dor_global_emisswi_update_execute (void);
void        dor_global_exeset_destroy_element (dot_exeset*);
dot_exeset* dor_global_exeset_get_data_pointer (void);
DtFlag      dor_global_exeset_get_value (dot_exeset*);
void        dor_global_exeset_initialize (void);
void        dor_global_exeset_obj_types_report (DtInt);
void        dor_global_exeset_pop_group (void);
DtFlag      dor_global_exeset_pop_value (void);
dot_exeset* dor_global_exeset_push_value (dot_exeset*);
void        dor_global_exeset_set_value (dot_exeset*);
void        dor_global_exeset_update_execute (void);
void        dor_global_fil_destroy_element (dot_filters);
DtPtr       dor_global_fil_get_data_pointer (void);
DtFlag      dor_global_fil_get_value (dot_filters);
void        dor_global_fil_initialize (void);
void        dor_global_fil_pop_group (void);
DtFlag      dor_global_fil_pop_value (void);
void        dor_global_fil_push_value (dot_filters);
void        dor_global_fil_set_value (DtFilter, dot_bitfield);
void        dor_global_fil_update_execute (void);
void        dor_global_glbrndmaxobjs_destroy_element (DtInt*);
DtInt*      dor_global_glbrndmaxobjs_get_data_pointer (void);
DtFlag      dor_global_glbrndmaxobjs_get_value (DtInt*);
void        dor_global_glbrndmaxobjs_initialize (void);
void        dor_global_glbrndmaxobjs_pop_group (void);
DtFlag      dor_global_glbrndmaxobjs_pop_value (void);
void        dor_global_glbrndmaxobjs_push_value (DtInt);
void        dor_global_glbrndmaxobjs_set_value (DtInt);
void        dor_global_glbrndmaxobjs_update_execute (void);
void        dor_global_glbrndmaxsub_destroy_element (DtInt*);
DtInt*      dor_global_glbrndmaxsub_get_data_pointer (void);
DtFlag      dor_global_glbrndmaxsub_get_value (DtInt*);
void        dor_global_glbrndmaxsub_initialize (void);
void        dor_global_glbrndmaxsub_pop_group (void);
DtFlag      dor_global_glbrndmaxsub_pop_value (void);
void        dor_global_glbrndmaxsub_push_value (DtInt);
void        dor_global_glbrndmaxsub_set_value (DtInt);
void        dor_global_glbrndmaxsub_update_execute (void);
void        dor_global_glbrndraylvl_destroy_element (DtInt*);
DtInt*      dor_global_glbrndraylvl_get_data_pointer (void);
DtFlag      dor_global_glbrndraylvl_get_value (DtInt*);
void        dor_global_glbrndraylvl_initialize (void);
void        dor_global_glbrndraylvl_pop_group (void);
DtFlag      dor_global_glbrndraylvl_pop_value (void);
void        dor_global_glbrndraylvl_push_value (DtInt);
void        dor_global_glbrndraylvl_set_value (DtInt);
void        dor_global_glbrndraylvl_update_execute (void);
void        dor_global_hidsrfswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_hidsrfswi_get_data_pointer (void);
DtFlag      dor_global_hidsrfswi_get_value (DtSwitch*);
void        dor_global_hidsrfswi_initialize (void);
void        dor_global_hidsrfswi_pop_group (void);
DtFlag      dor_global_hidsrfswi_pop_value (void);
void        dor_global_hidsrfswi_push_value (DtSwitch);
void        dor_global_hidsrfswi_set_value (DtSwitch);
void        dor_global_hidsrfswi_update_execute (void);
void        dor_global_intermdisptype_destroy_element
                (struct global_intermdisptype_data*);
struct global_intermdisptype_data*
            dor_global_intermdisptype_get_data_pointer (void);
DtFlag      dor_global_intermdisptype_get_value
                (DtIntermediateDispType* , DtInt*, DtInt, DtInt*);
void        dor_global_intermdisptype_initialize (void);
void        dor_global_intermdisptype_pop_group (void);
DtFlag      dor_global_intermdisptype_pop_value (void);
void        dor_global_intermdisptype_push_value(DtIntermediateDispType,DtInt*);
void        dor_global_intermdisptype_set_value (DtIntermediateDispType,DtInt*);
void        dor_global_intermdisptype_update_execute (void);
void        dor_global_inttyp_destroy_element (DtSwitch*);
DtInterpType*
            dor_global_inttyp_get_data_pointer (void);
DtFlag      dor_global_inttyp_get_value (DtInterpType*);
void        dor_global_inttyp_initialize (void);
void        dor_global_inttyp_pop_group (void);
DtFlag      dor_global_inttyp_pop_value (void);
void        dor_global_inttyp_push_value (DtInterpType);
void        dor_global_inttyp_set_value (DtInterpType);
void        dor_global_inttyp_update_execute (void);
void        dor_global_invswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_invswi_get_data_pointer (void);
DtFlag      dor_global_invswi_get_value (DtSwitch*);
void        dor_global_invswi_initialize (void);
void        dor_global_invswi_pop_group (void);
DtFlag      dor_global_invswi_pop_value (void);
void        dor_global_invswi_push_value (DtSwitch);
void        dor_global_invswi_set_value (DtSwitch);
void        dor_global_invswi_update_execute (void);
void        dor_global_lcstowcsmat_destroy_element (DtMatrix4x4*);
DtFlag      dor_global_lcstowcsmat_get_value (DtMatrix4x4);
void        dor_global_lcstowcsmat_initialize (void);
void        dor_global_lcstowcsmat_notify_change (void);
void        dor_global_lcstowcsmat_pop_group (void);
DtFlag      dor_global_lcstowcsmat_pop_value (void);
void        dor_global_lcstowcsmat_print_value (void);
void        dor_global_lcstowcsmat_push_value (DtMatrix4x4);
void        dor_global_lcstowcsmat_set_value (DtMatrix4x4);
void        dor_global_lcstowcsmat_update_execute
                (DtMatrix4x4, DtGlobalCall, DtInt);
void        dor_global_lgtatn_destroy_element (struct global_lgtatn_data*);
struct global_lgtatn_data*
                dor_global_lgtatn_get_data_pointer (void);
DtFlag      dor_global_lgtatn_get_value (DtReal* , DtReal*);
void        dor_global_lgtatn_initialize (void);
void        dor_global_lgtatn_pop_group (void);
DtFlag      dor_global_lgtatn_pop_value (void);
void        dor_global_lgtatn_push_value (DtReal, DtReal);
void        dor_global_lgtatn_set_value (DtReal, DtReal);
void        dor_global_lgtatn_update_execute (void);
void        dor_global_lgtclr_destroy_element (DtColorRGB*);
DtColorRGB* dor_global_lgtclr_get_data_pointer (void);
DtFlag      dor_global_lgtclr_get_value (DtColorModel* , DtReal[3]);
void        dor_global_lgtclr_initialize (void);
void        dor_global_lgtclr_pop_group (void);
DtFlag      dor_global_lgtclr_pop_value (void);
void        dor_global_lgtclr_push_value (DtColorRGB);
void        dor_global_lgtclr_set_value (DtColorRGB);
void        dor_global_lgtclr_update_execute (void);
void        dor_global_lgtint_destroy_element (DtReal*);
DtReal*     dor_global_lgtint_get_data_pointer (void);
DtFlag      dor_global_lgtint_get_value (DtReal*);
void        dor_global_lgtint_initialize (void);
void        dor_global_lgtint_pop_group (void);
DtFlag      dor_global_lgtint_pop_value (void);
void        dor_global_lgtint_push_value (DtReal);
void        dor_global_lgtint_set_value (DtReal);
void        dor_global_lgtint_update_execute (void);
void        dor_global_lgtspdang_destroy_element(struct global_lgtspdang_data*);
struct global_lgtspdang_data*
            dor_global_lgtspdang_get_data_pointer (void);
DtFlag      dor_global_lgtspdang_get_value (DtReal* , DtReal*);
void        dor_global_lgtspdang_initialize (void);
void        dor_global_lgtspdang_pop_group (void);
DtFlag      dor_global_lgtspdang_pop_value (void);
void        dor_global_lgtspdang_push_value (DtReal, DtReal);
void        dor_global_lgtspdang_set_value (DtReal, DtReal);
void        dor_global_lgtspdang_update_execute (void);
void        dor_global_lgtspdexp_destroy_element (DtReal*);
DtReal*     dor_global_lgtspdexp_get_data_pointer (void);
DtFlag      dor_global_lgtspdexp_get_value (DtReal*);
void        dor_global_lgtspdexp_initialize (void);
void        dor_global_lgtspdexp_pop_group (void);
DtFlag      dor_global_lgtspdexp_pop_value (void);
void        dor_global_lgtspdexp_push_value (DtReal);
void        dor_global_lgtspdexp_set_value (DtReal);
void        dor_global_lgtspdexp_update_execute (void);
void        dor_global_lgtswi_destroy_element (struct global_lgtswi_data *);
DtFlag      dor_global_lgtswi_get_value (DtInt* , DtPtr*, DtInt);
void        dor_global_lgtswi_initialize (void);
void        dor_global_lgtswi_pop_group (void);
DtFlag      dor_global_lgtswi_pop_value (void);
void        dor_global_lgtswi_push_value (DtInt, DtPtr*);
void        dor_global_lgtswi_set_value (DtObject, DtSwitch);
void        dor_global_lgtswi_update_execute (void);
void        dor_global_lgttyp_destroy_element (DtObject*);
DtObject*   dor_global_lgttyp_get_data_pointer (void);
DtFlag      dor_global_lgttyp_get_value (DtObject*);
void        dor_global_lgttyp_initialize (void);
void        dor_global_lgttyp_pop_group (void);
DtFlag      dor_global_lgttyp_pop_value (void);
void        dor_global_lgttyp_push_value (DtObject);
void        dor_global_lgttyp_set_value (DtObject);
void        dor_global_lgttyp_update_execute (void);
void        dor_global_lintyp_destroy_element (DtLineType*);
DtLineType* dor_global_lintyp_get_data_pointer (void);
DtFlag      dor_global_lintyp_get_value (DtLineType*);
void        dor_global_lintyp_initialize (void);
void        dor_global_lintyp_pop_group (void);
DtFlag      dor_global_lintyp_pop_value (void);
void        dor_global_lintyp_push_value (DtLineType);
void        dor_global_lintyp_set_value (DtLineType);
void        dor_global_lintyp_update_execute (void);
void        dor_global_linwid_destroy_element (DtReal*);
DtReal*     dor_global_linwid_get_data_pointer (void);
DtFlag      dor_global_linwid_get_value (DtReal*);
void        dor_global_linwid_initialize (void);
void        dor_global_linwid_pop_group (void);
DtFlag      dor_global_linwid_pop_value (void);
void        dor_global_linwid_push_value (DtReal);
void        dor_global_linwid_set_value (DtReal);
void        dor_global_linwid_update_execute (void);
void        dor_global_localaasty_destroy_element (DtLocalAntiAliasStyle*);
DtLocalAntiAliasStyle*
            dor_global_localaasty_get_data_pointer (void);
DtFlag      dor_global_localaasty_get_value (DtLocalAntiAliasStyle*);
void        dor_global_localaasty_initialize (void);
void        dor_global_localaasty_pop_group (void);
DtFlag      dor_global_localaasty_pop_value (void);
void        dor_global_localaasty_push_value (DtLocalAntiAliasStyle);
void        dor_global_localaasty_set_value (DtLocalAntiAliasStyle);
void        dor_global_localaasty_update_execute (void);
void        dor_global_localaaswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_localaaswi_get_data_pointer (void);
DtFlag      dor_global_localaaswi_get_value (DtSwitch*);
void        dor_global_localaaswi_initialize (void);
void        dor_global_localaaswi_pop_group (void);
DtFlag      dor_global_localaaswi_pop_value (void);
void        dor_global_localaaswi_push_value (DtSwitch);
void        dor_global_localaaswi_set_value (DtSwitch);
void        dor_global_localaaswi_update_execute (void);
void        dor_global_mapbmp_destroy_element (struct global_mapbmp_data*);
struct global_mapbmp_data*
            dor_global_mapbmp_get_data_pointer (void);
DtFlag      dor_global_mapbmp_get_value
                (DtInt, DtInt*, DtObject[], DtObject[], DtInt,
                 unsigned char[], DtInt[]);
void        dor_global_mapbmp_initialize (void);
void        dor_global_mapbmp_pop_group (void);
DtFlag      dor_global_mapbmp_pop_value (void);
void        dor_global_mapbmp_push_value
                (DtMapOperator, DtObject, DtObject, DtPtr, DtInt);
void        dor_global_mapbmp_set_value
                (DtMapOperator, DtObject, DtObject, DtPtr, DtInt);
void        dor_global_mapbmp_update_execute (void);
void        dor_global_mapbmpswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_mapbmpswi_get_data_pointer (void);
DtFlag      dor_global_mapbmpswi_get_value (DtSwitch*);
void        dor_global_mapbmpswi_initialize (void);
void        dor_global_mapbmpswi_pop_group (void);
DtFlag      dor_global_mapbmpswi_pop_value (void);
void        dor_global_mapbmpswi_push_value (DtSwitch);
void        dor_global_mapbmpswi_set_value (DtSwitch);
void        dor_global_mapbmpswi_update_execute (void);
void        dor_global_mapdifclr_destroy_element(struct global_mapdifclr_data*);
struct global_mapdifclr_data*
            dor_global_mapdifclr_get_data_pointer (void);
DtFlag      dor_global_mapdifclr_get_value
                (DtInt, DtInt* , DtObject[], DtObject[], DtInt,
                 unsigned char[], DtInt[]);
void        dor_global_mapdifclr_initialize (void);
void        dor_global_mapdifclr_pop_group (void);
DtFlag      dor_global_mapdifclr_pop_value (void);
void        dor_global_mapdifclr_push_value
                (DtMapOperator, DtObject, DtObject, DtPtr, DtInt);
void        dor_global_mapdifclr_set_value
                (DtMapOperator, DtObject, DtObject, DtPtr, DtInt);
void        dor_global_mapdifclr_update_execute (void);
void        dor_global_mapdifclrswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_mapdifclrswi_get_data_pointer (void);
DtFlag      dor_global_mapdifclrswi_get_value (DtSwitch*);
void        dor_global_mapdifclrswi_initialize (void);
void        dor_global_mapdifclrswi_pop_group (void);
DtFlag      dor_global_mapdifclrswi_pop_value (void);
void        dor_global_mapdifclrswi_push_value (DtSwitch);
void        dor_global_mapdifclrswi_set_value (DtSwitch);
void        dor_global_mapdifclrswi_update_execute (void);
void        dor_global_mapenv_destroy_element (struct global_mapenv_data*);
struct global_mapenv_data*
            dor_global_mapenv_get_data_pointer (void);
DtFlag      dor_global_mapenv_get_value
                (DtInt, DtInt* , DtObject[], DtObject[], DtInt,
                 unsigned char[], DtInt[]);
void        dor_global_mapenv_initialize (void);
void        dor_global_mapenv_pop_group (void);
DtFlag      dor_global_mapenv_pop_value (void);
void        dor_global_mapenv_push_value
                (DtMapOperator, DtObject, DtObject, DtPtr, DtInt);
void        dor_global_mapenv_set_value
                (DtMapOperator, DtObject, DtObject, DtPtr, DtInt);
void        dor_global_mapenv_update_execute (void);
void        dor_global_mapenvswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_mapenvswi_get_data_pointer (void);
DtFlag      dor_global_mapenvswi_get_value (DtSwitch*);
void        dor_global_mapenvswi_initialize (void);
void        dor_global_mapenvswi_pop_group (void);
DtFlag      dor_global_mapenvswi_pop_value (void);
void        dor_global_mapenvswi_push_value (DtSwitch);
void        dor_global_mapenvswi_set_value (DtSwitch);
void        dor_global_mapenvswi_update_execute (void);
void        dor_global_maptrnint_destroy_element
                (struct global_maptrnint_data*);
struct global_maptrnint_data*
            dor_global_maptrnint_get_data_pointer (void);
DtFlag      dor_global_maptrnint_get_value
                (DtInt, DtInt* , DtObject[], DtObject[], DtInt,
                 unsigned char[], DtInt[]);
void        dor_global_maptrnint_initialize (void);
void        dor_global_maptrnint_pop_group (void);
DtFlag      dor_global_maptrnint_pop_value (void);
void        dor_global_maptrnint_push_value
                (DtMapOperator, DtObject, DtObject, DtPtr, DtInt);
void        dor_global_maptrnint_set_value
                (DtMapOperator, DtObject, DtObject, DtPtr, DtInt);
void        dor_global_maptrnint_update_execute (void);
void        dor_global_maptrnintswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_maptrnintswi_get_data_pointer (void);
DtFlag      dor_global_maptrnintswi_get_value (DtSwitch*);
void        dor_global_maptrnintswi_initialize (void);
void        dor_global_maptrnintswi_pop_group (void);
DtFlag      dor_global_maptrnintswi_pop_value (void);
void        dor_global_maptrnintswi_push_value (DtSwitch);
void        dor_global_maptrnintswi_set_value (DtSwitch);
void        dor_global_maptrnintswi_update_execute (void);
struct global_minbndext_data*
            dor_global_minbndext_create_data (void);
void        dor_global_minbndext_destroy (struct global_minbndext_data*);
void        dor_global_minbndext_destroy_element (DtReal*);
DtReal*     dor_global_minbndext_get_data_pointer (void);
DtFlag      dor_global_minbndext_get_value (DtReal*);
void        dor_global_minbndext_initialize (void);
void        dor_global_minbndext_pop_group (void);
DtFlag      dor_global_minbndext_pop_value (void);
void        dor_global_minbndext_push_value (DtReal);
void        dor_global_minbndext_set_value (DtReal);
void        dor_global_minbndext_update_execute (void);
void        dor_global_mkrfnt_destroy_element (DtFont*);
DtInt*      dor_global_mkrfnt_get_data_pointer (void);
DtFlag      dor_global_mkrfnt_get_value (DtFont*);
void        dor_global_mkrfnt_initialize (void);
void        dor_global_mkrfnt_pop_group (void);
DtFlag      dor_global_mkrfnt_pop_value (void);
void        dor_global_mkrfnt_push_value (DtFont);
void        dor_global_mkrfnt_set_value (DtFont);
void        dor_global_mkrfnt_update_execute (void);
void        dor_global_mkrgly_destroy_element (DtInt*);
DtInt*      dor_global_mkrgly_get_data_pointer (void);
DtFlag      dor_global_mkrgly_get_value (DtInt*);
void        dor_global_mkrgly_initialize (void);
void        dor_global_mkrgly_pop_group (void);
DtFlag      dor_global_mkrgly_pop_value (void);
void        dor_global_mkrgly_push_value (DtInt);
void        dor_global_mkrgly_set_value (DtInt);
void        dor_global_mkrgly_update_execute (void);
struct global_mkrscl_data*
            dor_global_mkrscl_create_data (void);
void        dor_global_mkrscl_destroy (struct global_mkrscl_data*);
void        dor_global_mkrscl_destroy_element (DtReal*);
DtReal*     dor_global_mkrscl_get_data_pointer (void);
DtFlag      dor_global_mkrscl_get_value (DtReal*);
void        dor_global_mkrscl_initialize (void);
void        dor_global_mkrscl_pop_group (void);
DtFlag      dor_global_mkrscl_pop_value (void);
void        dor_global_mkrscl_push_value (DtReal);
void        dor_global_mkrscl_set_value (DtReal);
void        dor_global_mkrscl_update_execute (void);
void        dor_global_namset_destroy_element (dot_bitfield*);
dot_bitfield*
            dor_global_namset_get_data_pointer (void);
DtFlag      dor_global_namset_get_value (dot_bitfield);
void        dor_global_namset_initialize (void);
void        dor_global_namset_pop_group (void);
DtFlag      dor_global_namset_pop_value (void);
void        dor_global_namset_push_value (dot_bitfield);
void        dor_global_namset_set_value (dot_bitfield);
void        dor_global_namset_update_execute (void);
void        dor_global_pckid_destroy_element (DtInt*);
DtInt*      dor_global_pckid_get_data_pointer (void);
DtFlag      dor_global_pckid_get_value (DtInt*);
void        dor_global_pckid_initialize (void);
void        dor_global_pckid_pop_group (void);
DtFlag      dor_global_pckid_pop_value (void);
void        dor_global_pckid_push_value (DtInt);
void        dor_global_pckid_set_value (DtInt);
void        dor_global_pckid_update_execute (void);
void        dor_global_pckswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_pckswi_get_data_pointer (void);
DtFlag      dor_global_pckswi_get_value (DtSwitch*);
void        dor_global_pckswi_initialize (void);
void        dor_global_pckswi_pop_group (void);
DtFlag      dor_global_pckswi_pop_value (void);
void        dor_global_pckswi_push_value (DtSwitch);
void        dor_global_pckswi_set_value (DtSwitch);
void        dor_global_pckswi_update_execute (void);
void        dor_global_radambswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_radambswi_get_data_pointer (void);
DtFlag      dor_global_radambswi_get_value (DtSwitch*);
void        dor_global_radambswi_initialize (void);
void        dor_global_radambswi_pop_group (void);
DtFlag      dor_global_radambswi_pop_value (void);
void        dor_global_radambswi_push_value (DtSwitch);
void        dor_global_radambswi_set_value (DtSwitch);
void        dor_global_radambswi_update_execute (void);
void        dor_global_radswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_radswi_get_data_pointer (void);
DtFlag      dor_global_radswi_get_value (DtSwitch*);
void        dor_global_radswi_initialize (void);
void        dor_global_radswi_pop_group (void);
DtFlag      dor_global_radswi_pop_value (void);
void        dor_global_radswi_push_value (DtSwitch);
void        dor_global_radswi_set_value (DtSwitch);
void        dor_global_radswi_update_execute (void);
void        dor_global_reflecswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_reflecswi_get_data_pointer (void);
DtFlag      dor_global_reflecswi_get_value (DtSwitch*);
void        dor_global_reflecswi_initialize (void);
void        dor_global_reflecswi_pop_group (void);
DtFlag      dor_global_reflecswi_pop_value (void);
void        dor_global_reflecswi_push_value (DtSwitch);
void        dor_global_reflecswi_set_value (DtSwitch);
void        dor_global_reflecswi_update_execute (void);
void        dor_global_refractidx_destroy_element (DtReal*);
DtReal*     dor_global_refractidx_get_data_pointer (void);
DtFlag      dor_global_refractidx_get_value (DtReal*);
void        dor_global_refractidx_initialize (void);
void        dor_global_refractidx_pop_group (void);
DtFlag      dor_global_refractidx_pop_value (void);
void        dor_global_refractidx_push_value (DtReal);
void        dor_global_refractidx_set_value (DtReal);
void        dor_global_refractidx_update_execute (void);
void        dor_global_refractswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_refractswi_get_data_pointer (void);
DtFlag      dor_global_refractswi_get_value (DtSwitch*);
void        dor_global_refractswi_initialize (void);
void        dor_global_refractswi_pop_group (void);
DtFlag      dor_global_refractswi_pop_value (void);
void        dor_global_refractswi_push_value (DtSwitch);
void        dor_global_refractswi_set_value (DtSwitch);
void        dor_global_refractswi_update_execute (void);
void        dor_global_reptyp_destroy_element (DtSwitch*);
DtRepType*  dor_global_reptyp_get_data_pointer (void);
DtFlag      dor_global_reptyp_get_value (DtRepType*);
void        dor_global_reptyp_initialize (void);
void        dor_global_reptyp_pop_group (void);
DtFlag      dor_global_reptyp_pop_value (void);
void        dor_global_reptyp_push_value (DtRepType);
void        dor_global_reptyp_set_value (DtRepType);
void        dor_global_reptyp_update_execute (void);
void        dor_global_shadowswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_shadowswi_get_data_pointer (void);
DtFlag      dor_global_shadowswi_get_value (DtSwitch*);
void        dor_global_shadowswi_initialize (void);
void        dor_global_shadowswi_pop_group (void);
DtFlag      dor_global_shadowswi_pop_value (void);
void        dor_global_shadowswi_push_value (DtSwitch);
void        dor_global_shadowswi_set_value (DtSwitch);
void        dor_global_shadowswi_update_execute (void);
void        dor_global_shdidx_destroy_element (DtInt*);
DtInt*      dor_global_shdidx_get_data_pointer (void);
DtFlag      dor_global_shdidx_get_value (DtInt*);
void        dor_global_shdidx_initialize (void);
void        dor_global_shdidx_pop_group (void);
DtFlag      dor_global_shdidx_pop_value (void);
void        dor_global_shdidx_push_value (DtInt);
void        dor_global_shdidx_set_value (DtInt);
void        dor_global_shdidx_update_execute (void);
void        dor_global_smpadapt_destroy_element (DtReal*);
DtReal*     dor_global_smpadapt_get_data_pointer (void);
DtFlag      dor_global_smpadapt_get_value (DtReal*);
void        dor_global_smpadapt_initialize (void);
void        dor_global_smpadapt_pop_group (void);
DtFlag      dor_global_smpadapt_pop_value (void);
void        dor_global_smpadapt_push_value (DtReal);
void        dor_global_smpadapt_set_value (DtReal);
void        dor_global_smpadapt_update_execute (void);
void        dor_global_smpadaptswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_smpadaptswi_get_data_pointer (void);
DtFlag      dor_global_smpadaptswi_get_value (DtSwitch*);
void        dor_global_smpadaptswi_initialize (void);
void        dor_global_smpadaptswi_pop_group (void);
DtFlag      dor_global_smpadaptswi_pop_value (void);
void        dor_global_smpadaptswi_push_value (DtSwitch);
void        dor_global_smpadaptswi_set_value (DtSwitch);
void        dor_global_smpadaptswi_update_execute (void);
void        dor_global_smpfilter_destroy_element(struct global_smpfilter_data*);
struct global_smpfilter_data*
            dor_global_smpfilter_get_data_pointer (void);
DtFlag      dor_global_smpfilter_get_value (DtObject* , DtReal*, DtReal*);
void        dor_global_smpfilter_initialize (void);
void        dor_global_smpfilter_pop_group (void);
DtFlag      dor_global_smpfilter_pop_value (void);
void        dor_global_smpfilter_push_value (DtObject, DtReal, DtReal);
void        dor_global_smpfilter_set_value (DtObject, DtReal, DtReal);
void        dor_global_smpfilter_update_execute (void);
void        dor_global_smpjitter_destroy_element (DtReal*);
DtReal*     dor_global_smpjitter_get_data_pointer (void);
DtFlag      dor_global_smpjitter_get_value (DtReal*);
void        dor_global_smpjitter_initialize (void);
void        dor_global_smpjitter_pop_group (void);
DtFlag      dor_global_smpjitter_pop_value (void);
void        dor_global_smpjitter_push_value (DtReal);
void        dor_global_smpjitter_set_value (DtReal);
void        dor_global_smpjitter_update_execute (void);
void        dor_global_smpjitterswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_smpjitterswi_get_data_pointer (void);
DtFlag      dor_global_smpjitterswi_get_value (DtSwitch*);
void        dor_global_smpjitterswi_initialize (void);
void        dor_global_smpjitterswi_pop_group (void);
DtFlag      dor_global_smpjitterswi_pop_value (void);
void        dor_global_smpjitterswi_push_value (DtSwitch);
void        dor_global_smpjitterswi_set_value (DtSwitch);
void        dor_global_smpjitterswi_update_execute (void);
void        dor_global_smpsuper_destroy_element (struct global_smpsuper_data*);
struct global_smpsuper_data*
            dor_global_smpsuper_get_data_pointer (void);
DtFlag      dor_global_smpsuper_get_value (DtInt* , DtInt*);
void        dor_global_smpsuper_initialize (void);
void        dor_global_smpsuper_pop_group (void);
DtFlag      dor_global_smpsuper_pop_value (void);
void        dor_global_smpsuper_push_value (DtInt, DtInt);
void        dor_global_smpsuper_set_value (DtInt, DtInt);
void        dor_global_smpsuper_update_execute (void);
void        dor_global_smpsuperswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_smpsuperswi_get_data_pointer (void);
DtFlag      dor_global_smpsuperswi_get_value (DtSwitch*);
void        dor_global_smpsuperswi_initialize (void);
void        dor_global_smpsuperswi_pop_group (void);
DtFlag      dor_global_smpsuperswi_pop_value (void);
void        dor_global_smpsuperswi_push_value (DtSwitch);
void        dor_global_smpsuperswi_set_value (DtSwitch);
void        dor_global_smpsuperswi_update_execute (void);
void        dor_global_spcclr_destroy_element (DtColorRGB*);
DtColorRGB* dor_global_spcclr_get_data_pointer (void);
DtFlag      dor_global_spcclr_get_value (DtColorModel* , DtReal[3]);
void        dor_global_spcclr_initialize (void);
void        dor_global_spcclr_pop_group (void);
DtFlag      dor_global_spcclr_pop_value (void);
void        dor_global_spcclr_push_value (DtColorRGB);
void        dor_global_spcclr_set_value (DtColorRGB);
void        dor_global_spcclr_update_execute (void);
void        dor_global_spcfct_destroy_element (DtReal*);
DtReal*     dor_global_spcfct_get_data_pointer (void);
DtFlag      dor_global_spcfct_get_value (DtReal*);
void        dor_global_spcfct_initialize (void);
void        dor_global_spcfct_pop_group (void);
DtFlag      dor_global_spcfct_pop_value (void);
void        dor_global_spcfct_push_value (DtReal);
void        dor_global_spcfct_set_value (DtReal);
void        dor_global_spcfct_update_execute (void);
void        dor_global_spcint_destroy_element (DtReal*);
DtReal*     dor_global_spcint_get_data_pointer (void);
DtFlag      dor_global_spcint_get_value (DtReal*);
void        dor_global_spcint_initialize (void);
void        dor_global_spcint_pop_group (void);
DtFlag      dor_global_spcint_pop_value (void);
void        dor_global_spcint_push_value (DtReal);
void        dor_global_spcint_set_value (DtReal);
void        dor_global_spcint_update_execute (void);
void        dor_global_spcswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_spcswi_get_data_pointer (void);
DtFlag      dor_global_spcswi_get_value (DtSwitch*);
void        dor_global_spcswi_initialize (void);
void        dor_global_spcswi_pop_group (void);
DtFlag      dor_global_spcswi_pop_value (void);
void        dor_global_spcswi_push_value (DtSwitch);
void        dor_global_spcswi_set_value (DtSwitch);
void        dor_global_spcswi_update_execute (void);
void        dor_global_srfedgclr_destroy_element (DtColorRGB*);
DtColorRGB* dor_global_srfedgclr_get_data_pointer (void);
DtFlag      dor_global_srfedgclr_get_value (DtColorModel* , DtReal[3]);
void        dor_global_srfedgclr_initialize (void);
void        dor_global_srfedgclr_pop_group (void);
DtFlag      dor_global_srfedgclr_pop_value (void);
void        dor_global_srfedgclr_push_value (DtColorRGB);
void        dor_global_srfedgclr_set_value (DtColorRGB);
void        dor_global_srfedgclr_update_execute (void);
void        dor_global_srfshd_destroy_element (DtObject*);
DtObject*   dor_global_srfshd_get_data_pointer (void);
DtFlag      dor_global_srfshd_get_value (DtObject*);
void        dor_global_srfshd_initialize (void);
void        dor_global_srfshd_pop_group (void);
DtFlag      dor_global_srfshd_pop_value (void);
void        dor_global_srfshd_push_value (DtObject);
void        dor_global_srfshd_set_value (DtObject);
void        dor_global_srfshd_update_execute (void);
void        dor_global_stereo_destroy_element (struct global_stereo_data*);
struct global_stereo_data*
            dor_global_stereo_get_data_pointer (void);
DtFlag      dor_global_stereo_get_value (DtReal* , DtReal*);
void        dor_global_stereo_initialize (void);
void        dor_global_stereo_pop_group (void);
DtFlag      dor_global_stereo_pop_value (void);
void        dor_global_stereo_push_value (DtReal, DtReal);
void        dor_global_stereo_set_value (DtReal, DtReal);
void        dor_global_stereo_update_execute (void);
void        dor_global_stereoswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_stereoswi_get_data_pointer (void);
DtFlag      dor_global_stereoswi_get_value (DtSwitch*);
void        dor_global_stereoswi_initialize (void);
void        dor_global_stereoswi_pop_group (void);
DtFlag      dor_global_stereoswi_pop_value (void);
void        dor_global_stereoswi_push_value (DtSwitch);
void        dor_global_stereoswi_set_value (DtSwitch);
void        dor_global_stereoswi_update_execute (void);
DtFlag      dor_global_subdivlvl_get_value (DtReal*);
void        dor_global_subdivspc_destroy_element(struct global_subdivspc_data*);
struct global_subdivspc_data*
            dor_global_subdivspc_get_data_pointer (void);
DtFlag      dor_global_subdivspc_get_value (DtInt* , DtReal*);
void        dor_global_subdivspc_initialize (void);
void        dor_global_subdivspc_pop_group (void);
DtFlag      dor_global_subdivspc_pop_value (void);
void        dor_global_subdivspc_push_value (DtInt, DtReal*);
void        dor_global_subdivspc_set_value (DtInt, DtReal*);
void        dor_global_subdivspc_update_execute (void);
void        dor_global_texaa_destroy_element (DtTextureAntiAliasMode*);
DtTextureAntiAliasMode*
            dor_global_texaa_get_data_pointer (void);
DtFlag      dor_global_texaa_get_value (DtTextureAntiAliasMode*);
void        dor_global_texaa_initialize (void);
void        dor_global_texaa_pop_group (void);
DtFlag      dor_global_texaa_pop_value (void);
void        dor_global_texaa_push_value (DtTextureAntiAliasMode);
void        dor_global_texaa_set_value (DtTextureAntiAliasMode);
void        dor_global_texaa_update_execute (void);
void        dor_global_texextuv_destroy_element (struct global_texextuv_data*);
struct global_texextuv_data*
            dor_global_texextuv_get_data_pointer (void);
DtFlag      dor_global_texextuv_get_value (DtExtendMode* , DtExtendMode*);
void        dor_global_texextuv_initialize (void);
void        dor_global_texextuv_pop_group (void);
DtFlag      dor_global_texextuv_pop_value (void);
void        dor_global_texextuv_push_value (DtExtendMode, DtExtendMode);
void        dor_global_texextuv_set_value (DtExtendMode, DtExtendMode);
void        dor_global_texextuv_update_execute (void);
void        dor_global_texextuvw_destroy_element(struct global_texextuvw_data*);
struct global_texextuvw_data*
            dor_global_texextuvw_get_data_pointer (void);
DtFlag      dor_global_texextuvw_get_value
                (DtExtendMode* , DtExtendMode*, DtExtendMode*);
void        dor_global_texextuvw_initialize (void);
void        dor_global_texextuvw_pop_group (void);
DtFlag      dor_global_texextuvw_pop_value (void);
void        dor_global_texextuvw_push_value
                (DtExtendMode, DtExtendMode, DtExtendMode);
void        dor_global_texextuvw_set_value
                (DtExtendMode, DtExtendMode, DtExtendMode);
void        dor_global_texextuvw_update_execute (void);
void        dor_global_texintsrf_destroy_element(struct global_texintsrf_data*);
struct global_texintsrf_data*
            dor_global_texintsrf_get_data_pointer (void);
DtFlag      dor_global_texintsrf_get_value (DtObject* , Dt2PartProjType*);
void        dor_global_texintsrf_initialize (void);
void        dor_global_texintsrf_pop_group (void);
DtFlag      dor_global_texintsrf_pop_value (void);
void        dor_global_texintsrf_push_value (DtObject, Dt2PartProjType);
void        dor_global_texintsrf_set_value (DtObject, Dt2PartProjType);
void        dor_global_texintsrf_update_execute (void);
void        dor_global_texmatuv_destroy_element (struct texmatuv*);
struct texmatuv*
            dor_global_texmatuv_get_data_pointer (void);
DtFlag      dor_global_texmatuv_get_value (DtMatrix3x3);
void        dor_global_texmatuv_initialize (void);
void        dor_global_texmatuv_pop_group (void);
DtFlag      dor_global_texmatuv_pop_value (void);
void        dor_global_texmatuv_print_value (void);
void        dor_global_texmatuv_push_value (DtMatrix3x3);
void        dor_global_texmatuv_set_value (DtMatrix3x3);
void        dor_global_texmatuv_update_execute (void);
void        dor_global_texmatuvw_destroy_element (struct texmatuvw*);
struct texmatuvw*
            dor_global_texmatuvw_get_data_pointer (void);
DtFlag      dor_global_texmatuvw_get_value (DtMatrix4x4);
void        dor_global_texmatuvw_initialize (void);
void        dor_global_texmatuvw_pop_group (void);
DtFlag      dor_global_texmatuvw_pop_value (void);
void        dor_global_texmatuvw_print_value (void);
void        dor_global_texmatuvw_push_value (DtMatrix4x4);
void        dor_global_texmatuvw_set_value (DtMatrix4x4);
void        dor_global_texmatuvw_update_execute (void);
void        dor_global_texop_destroy_element (DtTextureOperator*);
DtTextureOperator*
            dor_global_texop_get_data_pointer (void);
DtFlag      dor_global_texop_get_value (DtTextureOperator*);
void        dor_global_texop_initialize (void);
void        dor_global_texop_pop_group (void);
DtFlag      dor_global_texop_pop_value (void);
void        dor_global_texop_push_value (DtTextureOperator);
void        dor_global_texop_set_value (DtTextureOperator);
void        dor_global_texop_update_execute (void);
void        dor_global_texuvidx_destroy_element (DtInt*);
DtInt*      dor_global_texuvidx_get_data_pointer (void);
DtFlag      dor_global_texuvidx_get_value (DtInt*);
void        dor_global_texuvidx_initialize (void);
void        dor_global_texuvidx_pop_group (void);
DtFlag      dor_global_texuvidx_pop_value (void);
void        dor_global_texuvidx_push_value (DtInt);
void        dor_global_texuvidx_set_value (DtInt);
void        dor_global_texuvidx_update_execute (void);
void        dor_global_texuvwidx_destroy_element (DtInt*);
DtInt*      dor_global_texuvwidx_get_data_pointer (void);
DtFlag      dor_global_texuvwidx_get_value (DtInt*);
void        dor_global_texuvwidx_initialize (void);
void        dor_global_texuvwidx_pop_group (void);
DtFlag      dor_global_texuvwidx_pop_value (void);
void        dor_global_texuvwidx_push_value (DtInt);
void        dor_global_texuvwidx_set_value (DtInt);
void        dor_global_texuvwidx_update_execute (void);
void        dor_global_transpclr_destroy_element (DtColorRGB*);
DtColorRGB* dor_global_transpclr_get_data_pointer (void);
DtFlag      dor_global_transpclr_get_value (DtColorModel* , DtReal[3]);
void        dor_global_transpclr_initialize (void);
void        dor_global_transpclr_pop_group (void);
DtFlag      dor_global_transpclr_pop_value (void);
void        dor_global_transpclr_push_value (DtColorRGB);
void        dor_global_transpclr_set_value (DtColorRGB);
void        dor_global_transpclr_update_execute (void);
void        dor_global_transpint_destroy_element (DtReal*);
DtReal*     dor_global_transpint_get_data_pointer (void);
DtFlag      dor_global_transpint_get_value (DtReal*);
void        dor_global_transpint_initialize (void);
void        dor_global_transpint_pop_group (void);
DtFlag      dor_global_transpint_pop_value (void);
void        dor_global_transpint_push_value (DtReal);
void        dor_global_transpint_set_value (DtReal);
void        dor_global_transpint_update_execute (void);
void        dor_global_transporientclr_destroy_element (DtColorRGB*);
DtColorRGB* dor_global_transporientclr_get_data_pointer (void);
DtFlag      dor_global_transporientclr_get_value (DtColorModel* , DtReal[3]);
void        dor_global_transporientclr_initialize (void);
void        dor_global_transporientclr_pop_group (void);
DtFlag      dor_global_transporientclr_pop_value (void);
void        dor_global_transporientclr_push_value (DtColorRGB);
void        dor_global_transporientclr_set_value (DtColorRGB);
void        dor_global_transporientclr_update_execute (void);
void        dor_global_transporientexp_destroy_element (DtReal*);
DtReal*     dor_global_transporientexp_get_data_pointer (void);
DtFlag      dor_global_transporientexp_get_value (DtReal*);
void        dor_global_transporientexp_initialize (void);
void        dor_global_transporientexp_pop_group (void);
DtFlag      dor_global_transporientexp_pop_value (void);
void        dor_global_transporientexp_push_value (DtReal);
void        dor_global_transporientexp_set_value (DtReal);
void        dor_global_transporientexp_update_execute (void);
void        dor_global_transporientint_destroy_element (DtReal*);
DtReal*     dor_global_transporientint_get_data_pointer (void);
DtFlag      dor_global_transporientint_get_value (DtReal*);
void        dor_global_transporientint_initialize (void);
void        dor_global_transporientint_pop_group (void);
DtFlag      dor_global_transporientint_pop_value (void);
void        dor_global_transporientint_push_value (DtReal);
void        dor_global_transporientint_set_value (DtReal);
void        dor_global_transporientint_update_execute (void);
void        dor_global_transporientswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_transporientswi_get_data_pointer (void);
DtFlag      dor_global_transporientswi_get_value (DtSwitch*);
void        dor_global_transporientswi_initialize (void);
void        dor_global_transporientswi_pop_group (void);
DtFlag      dor_global_transporientswi_pop_value (void);
void        dor_global_transporientswi_push_value (DtSwitch);
void        dor_global_transporientswi_set_value (DtSwitch);
void        dor_global_transporientswi_update_execute (void);
void        dor_global_transpswi_destroy_element (DtSwitch*);
DtSwitch*   dor_global_transpswi_get_data_pointer (void);
DtFlag      dor_global_transpswi_get_value (DtSwitch*);
void        dor_global_transpswi_initialize (void);
void        dor_global_transpswi_pop_group (void);
DtFlag      dor_global_transpswi_pop_value (void);
void        dor_global_transpswi_push_value (DtSwitch);
void        dor_global_transpswi_set_value (DtSwitch);
void        dor_global_transpswi_update_execute (void);
void        dor_global_txtaln_destroy_element (dot_txtaln*);
dot_txtaln* dor_global_txtaln_get_data_pointer (void);
DtFlag      dor_global_txtaln_get_value (dot_txtaln*);
DtFlag      dor_global_txtaln_get_value2
                (DtTextAlignHorizontal* , DtTextAlignVertical*);
void        dor_global_txtaln_initialize (void);
void        dor_global_txtaln_pop_group (void);
DtFlag      dor_global_txtaln_pop_value (void);
void        dor_global_txtaln_push_value (dot_txtaln*);
void        dor_global_txtaln_set_value (dot_txtaln*);
void        dor_global_txtaln_update_execute (void);
void        dor_global_txtexp_destroy_element (DtReal*);
DtReal*     dor_global_txtexp_get_data_pointer (void);
DtFlag      dor_global_txtexp_get_value (DtReal*);
void        dor_global_txtexp_initialize (void);
void        dor_global_txtexp_pop_group (void);
DtFlag      dor_global_txtexp_pop_value (void);
void        dor_global_txtexp_push_value (DtReal);
void        dor_global_txtexp_set_value (DtReal);
void        dor_global_txtexp_update_execute (void);
void        dor_global_txtfnt_destroy_element (DtFont*);
DtInt*      dor_global_txtfnt_get_data_pointer (void);
DtFlag      dor_global_txtfnt_get_value (DtFont*);
void        dor_global_txtfnt_initialize (void);
void        dor_global_txtfnt_pop_group (void);
DtFlag      dor_global_txtfnt_pop_value (void);
void        dor_global_txtfnt_push_value (DtFont);
void        dor_global_txtfnt_set_value (DtFont);
void        dor_global_txtfnt_update_execute (void);
void        dor_global_txthgt_destroy_element (DtReal*);
DtReal*     dor_global_txthgt_get_data_pointer (void);
DtFlag      dor_global_txthgt_get_value (DtReal*);
void        dor_global_txthgt_initialize (void);
void        dor_global_txthgt_pop_group (void);
DtFlag      dor_global_txthgt_pop_value (void);
void        dor_global_txthgt_push_value (DtReal);
void        dor_global_txthgt_set_value (DtReal);
void        dor_global_txthgt_update_execute (void);
void        dor_global_txtpat_destroy_element (DtTextPath*);
DtTextPath* dor_global_txtpat_get_data_pointer (void);
DtFlag      dor_global_txtpat_get_value (DtTextPath*);
void        dor_global_txtpat_initialize (void);
void        dor_global_txtpat_pop_group (void);
DtFlag      dor_global_txtpat_pop_value (void);
void        dor_global_txtpat_push_value (DtTextPath);
void        dor_global_txtpat_set_value (DtTextPath);
void        dor_global_txtpat_update_execute (void);
void        dor_global_txtprc_destroy_element (DtTextPrecision*);
DtTextPrecision*
            dor_global_txtprc_get_data_pointer (void);
DtFlag      dor_global_txtprc_get_value (DtTextPrecision*);
void        dor_global_txtprc_initialize (void);
void        dor_global_txtprc_pop_group (void);
DtFlag      dor_global_txtprc_pop_value (void);
void        dor_global_txtprc_push_value (DtTextPrecision);
void        dor_global_txtprc_set_value (DtTextPrecision);
void        dor_global_txtprc_update_execute (void);
void        dor_global_txtspc_destroy_element (DtReal*);
DtReal*     dor_global_txtspc_get_data_pointer (void);
DtFlag      dor_global_txtspc_get_value (DtReal*);
void        dor_global_txtspc_initialize (void);
void        dor_global_txtspc_pop_group (void);
DtFlag      dor_global_txtspc_pop_value (void);
void        dor_global_txtspc_push_value (DtReal);
void        dor_global_txtspc_set_value (DtReal);
void        dor_global_txtspc_update_execute (void);
void        dor_global_txtupvec_destroy_element (dot_txtupvec*);
dot_txtupvec*
            dor_global_txtupvec_get_data_pointer (void);
DtFlag      dor_global_txtupvec_get_value (dot_txtupvec*);
DtFlag      dor_global_txtupvec_get_value2 (DtReal* , DtReal*);
void        dor_global_txtupvec_initialize (void);
void        dor_global_txtupvec_pop_group (void);
DtFlag      dor_global_txtupvec_pop_value (void);
void        dor_global_txtupvec_push_value (dot_txtupvec*);
void        dor_global_txtupvec_set_value (dot_txtupvec*);
void        dor_global_txtupvec_update_execute (void);
extern DtInt     doe_system_active_traversals;
extern DtFlag    doe_system_alternate_object_execution_flag;
extern DtInt     doe_system_current_method;
extern DtInt     doe_system_main_processors;
extern Dt32Bits  doe_system_state;
extern DtFlag    doe_system_subdivision_modification_flag;
extern DtSwitch  doe_system_valuator_switch;
extern DtInt     doe_system_timing_connected_lines_drawn;
extern DtInt     doe_system_timing_lines_drawn;
extern DtInt     doe_system_timing_mesh_quads_drawn;
extern DtInt     doe_system_timing_mesh_triangles_drawn;
extern DtInt     doe_system_timing_points_drawn;
extern DtInt     doe_system_timing_quads_drawn;
extern DtInt     doe_system_timing_strip_triangles_drawn;
extern DtInt     doe_system_timing_triangles_drawn;
extern DtInt     doe_system_timing_firmware_spheres_drawn;
extern DtInt     doe_system_timing_firmware_cylinders_drawn;
extern DtInt     doe_system_timing_display_traverse_count;
void  dor_doresys_initialize  (DtInt);
void  dor_doresys_initialize_drivers  (void);
void  dor_doresys_initialize_renderers  (void);
void  dor_doresys_terminate  (void);
DtVolume*    dor_system_compute_bounding_volume  (DtVolume*, DtObject);
void         dor_system_execute_object  (dot_object*);
DtInt        dor_system_filerstr_read
                     (char*, DtInt*, DtInt*, DtInt*, DtRasterType*, DtPtr*);
void         dor_system_initialize  (DtInt, int);
DtAngleUnits dor_system_inq_angle_units  (void);
DtInt        dor_system_inq_current_method  (void);
DtSwitch     dor_system_inq_execution_depth  (void);
DtInt        dor_system_inq_number_processors  (void);
void         dor_system_inq_version  (DtPtr*);
void         dor_system_radiosity_initialize  (void);
void         dor_system_report_bounding_points  (DtInt, DtReal[]);
DtInt        dor_system_rstr_write  (DtObjectStructure*, char*);
void         dor_system_set_angle_units  (DtAngleUnits);
void         dor_system_set_edit_source  (DtSwitch);
void         dor_system_set_execution_depth  (DtInt);
void         dor_system_set_valuator_switch  (DtSwitch);
void         dor_system_terminate  (void);
DtVertexType dor_system_texture_uv_count  (Dt32Bits);
DtVertexType dor_system_texture_uvw_count  (Dt32Bits);
typedef struct list      dot_list;
typedef struct list_node dot_list_node;
struct list_node {
    struct list_node *list_node_next;
    struct list_node *list_node_previous;
    DtPtr             list_node_data;
};
struct list {
    struct list_node *list_header_node;
    DtInt             list_nodemax;
    short             list_direction;
    struct list_node *list_current_node;
};
dot_list_node *dor_list_add_data_before      (dot_list*, dot_list_node*, DtPtr);
dot_list_node *dor_list_add_data_first       (dot_list*, DtPtr);
dot_list_node *dor_list_add_data_last        (dot_list*, DtPtr);
void           dor_list_add_node_before      (dot_list*, dot_list_node*,
					      dot_list_node*);
void           dor_list_add_node_first       (dot_list*, dot_list_node*);
void           dor_list_add_node_last        (dot_list*, dot_list_node*);
dot_list      *dor_list_create               (void);
dot_list_node *dor_list_create_node          (DtPtr);
void           dor_list_delete_node          (dot_list*, dot_list_node*);
void           dor_list_destroy              (dot_list*);
DtPtr          dor_list_destroy_current_data (dot_list*);
void           dor_list_empty                (dot_list*);
DtPtr          dor_list_get_current_data     (dot_list*);
dot_list_node *dor_list_get_current_node     (dot_list*);
DtPtr          dor_list_get_first_data       (dot_list*);
dot_list_node *dor_list_get_first_node       (dot_list*);
DtPtr          dor_list_get_last_data        (dot_list*);
int            dor_list_get_list_maximum     (dot_list*list);
DtPtr          dor_list_get_next_data        (dot_list*);
dot_list_node *dor_list_get_next_node        (dot_list_node*);
DtPtr          dor_list_get_node_data        (dot_list*, dot_list_node *);
DtPtr          dor_list_get_previous_data    (dot_list*);
void           dor_list_print                (dot_list*);
DtPtr          dor_list_remove_current_data  (dot_list*);
dot_list_node *dor_list_remove_current_node  (dot_list*);
DtPtr          dor_list_remove_data_node     (dot_list*, dot_list_node*);
void           dor_list_remove_node          (dot_list*, dot_list_node*);
void           dor_list_replace_current_data (dot_list*, DtPtr);
void           dor_list_replace_last_data    (dot_list*, DtPtr);
dot_list *dor_stack_create          (void);
void      dor_stack_destroy         (dot_list *stack);
void      dor_stack_push            (dot_list *stack, DtPtr value);
DtFlag    dor_stack_pop             (dot_list *stack, DtPtr *value);
DtFlag    dor_stack_get_top_element (dot_list *stack, DtPtr *value);
void      dor_stack_set_top_element (dot_list *stack, DtPtr value);
void    dor_matrix_concat  (DtMatrix4x4, DtMatrix4x4, DtFlag);
DtPtr   dor_matrix_create  (void);
void    dor_matrix_destroy  (DtMatrix4x4);
void    dor_matrix_invert  (DtMatrix4x4);
void    dor_matrix_look_at_from  (DtMatrix4x4,DtReal,DtReal,DtReal,DtReal,
	                      DtReal,DtReal,DtReal,DtReal,DtReal,DtFlag);
void    dor_matrix_load  (DtMatrix4x4, DtMatrix4x4);
void    dor_matrix_load_constant  (DtMatrix4x4, DtReal);
void    dor_matrix_load_identity  (DtMatrix4x4);
void    dor_matrix_load_look_at_from  (DtMatrix4x4,DtReal,DtReal,DtReal,DtReal,
			   DtReal,DtReal,DtReal,DtReal,DtReal);
void    dor_matrix_load_parallel  (DtMatrix4x4, DtReal,DtReal,DtReal);
void    dor_matrix_load_perspective  (DtMatrix4x4, DtReal,DtReal,DtReal);
void    dor_matrix_load_projection  (DtMatrix4x4, DtArea*, DtProjectionType,
                                     DtPoint3, DtReal, DtReal, DtReal);
void    dor_matrix_load_rotate_x  (DtMatrix4x4, DtReal);
void    dor_matrix_load_rotate_y  (DtMatrix4x4, DtReal);
void    dor_matrix_load_rotate_z  (DtMatrix4x4, DtReal);
void    dor_matrix_load_scale  (DtMatrix4x4, DtReal,DtReal,DtReal);
void    dor_matrix_load_shear  (DtMatrix4x4,DtMajorPlane,DtReal,DtReal);
void    dor_matrix_load_shear_z  (DtMatrix4x4,DtReal,DtReal);
void    dor_matrix_load_translate  (DtMatrix4x4, DtReal,DtReal,DtReal);
void    dor_matrix_multiply  (DtMatrix4x4, DtMatrix4x4, DtMatrix4x4);
void    dor_matrix_new_vectormultiply (DtVector4,DtMatrix4x4,DtVector4);
void    dor_matrix_parallel  (DtMatrix4x4,DtReal,DtReal,DtReal,DtFlag);
void    dor_matrix_perspective  (DtMatrix4x4, DtReal, DtReal, DtReal, DtFlag);
void    dor_matrix_post_concatenate  (DtMatrix4x4, DtMatrix4x4);
void    dor_matrix_pre_concatenate  (DtMatrix4x4, DtMatrix4x4);
void    dor_matrix_print  (DtMatrix4x4);
void    dor_matrix_rotate_x  (DtMatrix4x4, DtReal, DtFlag);
void    dor_matrix_rotate_y  (DtMatrix4x4, DtReal, DtFlag);
void    dor_matrix_rotate_z  (DtMatrix4x4, DtReal, DtFlag);
void    dor_matrix_scale  (DtMatrix4x4, DtReal, DtReal, DtReal, DtFlag);
void    dor_matrix_shear  (DtMatrix4x4, DtMajorPlane, DtReal, DtReal, DtFlag);
void    dor_matrix_shear_z  (DtMatrix4x4, DtReal, DtReal, DtFlag);
DtShort dor_matrix_solve  (DtReal[4][4], DtReal[4][4], DtReal[4][4]);
void    dor_matrix_translate  (DtMatrix4x4, DtReal, DtReal, DtReal, DtFlag);
void    dor_matrix_transpose  (DtMatrix4x4);
void    dor_matrix_vectormultiply  (DtVector4, DtMatrix4x4, DtVector4);
void    dor_matrix_load3x3  (DtMatrix3x3, DtMatrix3x3);
void    dor_matrix_load_constant3x3  (DtMatrix3x3, DtReal);
void    dor_matrix_load_identity3x3  (DtMatrix3x3);
void    dor_matrix_print3x3  (DtMatrix3x3);
struct rotate_data {
    DtAxis axis;
    DtReal radians;
};
DtInt doe_rotate_class_id;
static DtMethodEntry rotate_methods[] = {
    {  3 ,            (DtMethodPtr) dor_rotate_print },
    {  0 ,          (DtMethodPtr) dor_rotate_destroy },
    {  6 ,          (DtMethodPtr) dor_rotate_execute },
    {  7 ,             (DtMethodPtr) dor_rotate_execute },
    {  5 ,     (DtMethodPtr) dor_rotate_execute },
    {  10 ,  (DtMethodPtr) dor_rotate_execute },
    {  11 , (DtMethodPtr) dor_rotate_execute },
};
void dor_rotate_initialize (void)
{
    doe_rotate_class_id = dor_class_install ( 12 , "DoRotate", 
			7,rotate_methods,  0 );
}
struct object *dor_rotate_create (
    DtAxis axis,
    DtReal angle)
{
    static DtFlag initialized = DcFalse;
    struct rotate_data *rotdat;
    dot_object *rotobj;
    if (!initialized) {
	dor_rotate_initialize();
	initialized = DcTrue;
    }
    rotdat = dor_space_allocate (sizeof (*rotdat));
    if (dor_system_inq_angle_units() == DcAngleDegrees) {
	rotdat->radians = angle * M_PI / 180.0;
    } else {
	rotdat->radians = angle;
    }
    rotdat->axis = axis;
    rotobj = dor_object_create(doe_rotate_class_id,rotdat);
    return(rotobj);
}
void dor_rotate_destroy (dot_object *object)
{
    dor_space_deallocate(object->data);
}
void dor_rotate_print (dot_object *object)
{
    struct rotate_data *rotate;
    DtAngleUnits angle;
    DtReal degrees;
    rotate = (struct rotate_data *)object->data;
    if (rotate ==  0 ){
	dor_print_output("bad values");
	return;
    }
    switch (rotate->axis){
        case DcXAxis:
	    dor_print_output("axis = DcXAxis");
	    break;
        case DcYAxis:
	    dor_print_output("axis = DcYAxis");
	    break;
        case DcZAxis:
	    dor_print_output("axis = DcZAxis");
	    break;
        default:
	    dor_print_output("axis = UNKNOWN");
	    break;
    }
    angle = dor_system_inq_angle_units(); 
    switch (angle) {
        case DcAngleOldStyle:
        case DcAngleRadians:
    	    sprintf(dor_print_get_line(),"radians = %lf",rotate->radians);
    	    dor_print_output(dor_print_get_line());
	    break;
        case DcAngleDegrees:
	    degrees = rotate->radians * 180. / M_PI; 
    	    sprintf(dor_print_get_line(),"degrees = %lf",degrees);
    	    dor_print_output(dor_print_get_line());
	    break;
        default:
            break;
    }
}
void dor_rotate_execute (struct object *object)
{
    struct rotate_data *rotdat;
    DtMatrix4x4 newmatrix;
    if (!dor_condex_query_execute(doe_rotate_class_id))
	    return;
    rotdat = (struct rotate_data *)object->data;
    dor_global_lcstowcsmat_get_value(newmatrix);
    switch(rotdat->axis){
    case DcXAxis:
	dor_matrix_rotate_x(newmatrix,rotdat->radians,DcPreConcatenate);
	break;
    case DcYAxis:
	dor_matrix_rotate_y(newmatrix,rotdat->radians,DcPreConcatenate);
	break;
    case DcZAxis:
	dor_matrix_rotate_z(newmatrix,rotdat->radians,DcPreConcatenate);
	break;
    }
    dor_global_lcstowcsmat_set_value(newmatrix);
}
