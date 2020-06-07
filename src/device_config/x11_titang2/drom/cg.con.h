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
 
/*  This header file defines the symbolic constants used in the
    interface to the low level graphics clipping, shading, and
    drawing pipeline.  It also defines the structure of the general 
    control word which appears as an element of the 'cg_sparms'
    data structure, and which is used directly within the low level
    graphics pipeline routines.

    Symbols are defined for constants, bit field positions, widths, 
    masks, and shift amounts to left-align and right-align control bit
    fields.  Left-aligning shift amounts also are defined for bits 
    within bit significant fields

    These data definitions are specified both as C language constants 
    and as assembly language constants .  The C language definitions 
    are prefixed by 'CG_'.  This reduces the chances for conflict 
    with other symbols defined in C language code.

    Conditional definition is under control of the symbol 'ASSEMBLER'. 
    When this symbol is defined to the pre-processor, the assembler 
    formats are defined.  Otherwise, the C language formats are 
    defined.  Note that the assembler symbols CLIP, WCLIP, and MESH
    are internally derived by the pipeline, and are not defined in 
    C language form.



    Control word bit assignments are:

	ctl<31:28> =	MESH	SAVW	WCLIP	NALIAS
			------------------------------
	   <27:24> =	VISM	CLXR	CLXL	CLYT


	ctl<23:20> =	CLYB	CLZH	CLZY	CLMOD
			------------------------------
	   <19:16> =	CLOBR	CLFRU	BDBOX	XFORM


	ctl<15:12> =	BASE	POINTS	LINES	BTSEL
			------------------------------
	    <11:8> =	WTAVG	SHDAVG	DPCFULL	DPCCB


	ctl<7:4>   =	DPCBB	VCOLS	SVERT	SOBJ
			------------------------------
	   <3:0>   =	VNORMS	ENORMS	FFCULL	BFCULL
		  

*/


/*	definitions of symbolic parameter values	*/
#ifndef	ASSEMBLER

/*	C language definitions	*/

/*	generic values 	*/
#define	CG_NO		0		/* generic negative */
#define	CG_NIL		0		/* generic nullity */
#define	CG_OFF		0		/* generic absence */
#define	CG_NULL		0		/* null pointer value */
#define	CG_NONE		0		/* none supplied */
#define	CG_ON		1		/* generic presence */
#define	CG_YES		1		/* generic affirmative */
#define	CG_EXC		-1		/* generic exception */

/*	visibility mask field definitions	*/
#define	CG_TV1		1<<0		/* triangle vertex 1 */
#define	CG_TV2		1<<1		/* triangle vertex 2 */
#define	CG_TV3		1<<2		/* triangle vertex 3 */
#define	CG_TL12		1<<0		/* triangle v1-v2 edge */
#define	CG_TL13		1<<1		/* triangle v1-v3 edge */
#define	CG_TL23		1<<2		/* triangle v2-v3 edge */

/*	control bit field widths	*/
#define	CG_wCULL	2			/* culling flags */
#define	CG_wNORMS	2			/* shading normals */
#define	CG_wSPEC	2			/* specular shading */
#define	CG_wVCOLS	1			/* vertex colors */
#define	CG_wDPCUE	3			/* depth cueing flags */
#define	CG_wSHDAVG	1			/* shade average */
#define	CG_wPSCOLOR	2			/* pseudocolor */
#define	CG_wDRTYP	2			/* drawn figure type */
#define	CG_wCONFIG	1			/* configuration */
#define	CG_wXFORM	1			/* transform flag */
#define	CG_wBDBOX	1			/* bounding box present */
#define	CG_wCLIPF	3			/* clipping flags */
#define	CG_wCLIPM	6			/* clipping mask */
#define	CG_wVISM	1			/* visibility mask */
#define	CG_wNALIAS	1			/* anti-aliasing */
#define	CG_wRES		3			/* reserved */

/*	field masks for right aligned control bit fields	*/
#define	CG_fCULL	(3<<0)			/* culling flags */
#define	CG_fNORMS	(3<<0)			/* shading normals */
#define	CG_fSPEC	(3<<0)			/* specular shading */
#define	CG_fVCOLS	(1<<0)			/* vertex colors */
#define	CG_fDPCUE	(7<<0)			/* depth cueing flags */
#define	CG_fSHDAVG	(1<<0)			/* shade average */
#define	CG_fPSCOLOR	(3<<0)			/* pseudocolor */
#define	CG_fDRTYP	(3<<0)			/* drawn figure type */
#define	CG_fCONFIG	(1<<0)			/* configuration */
#define	CG_fXFORM	(1<<0)			/* transform flag */
#define	CG_fBDBOX	(1<<0)			/* bounding box present */
#define	CG_fCLIPF	(7<<0)			/* clipping flags */
#define	CG_fCLIPM	(0x3f<<0)		/* clipping mask */
#define	CG_fVISM	(1<<0)			/* visibility mask */
#define	CG_fNALIAS	(1<<0)			/* anti-aliasing */
#define	CG_fRES		(0x1f<<0)		/* reserved */

/*	right bit positions for control bit fields	*/
#define	CG_rCULL	0			/* culling flags */
#define	CG_rNORMS	(CG_rCULL+CG_wCULL)	/* shading normals */
#define	CG_rSPEC	(CG_rNORMS+CG_wNORMS)	/* specular shading */
#define	CG_rVCOLS	(CG_rSPEC+CG_wSPEC)	/* vertex colors */
#define	CG_rDPCUE	(CG_rVCOLS+CG_wVCOLS)	/* depth cueing flags */
#define	CG_rSHDAVG	(CG_rDPCUE+CG_wDPCUE)	/* shade average */
#define	CG_rPSCOLOR	(CG_rSHDAVG+CG_wSHDAVG)	/* pseudocolor */
#define	CG_rDRTYP	(CG_rPSCOLOR+CG_wPSCOLOR) /* drawn figure type */
#define	CG_rCONFIG	(CG_rDRTYP+CG_wDRTYP)	/* configuration */
#define	CG_rXFORM	(CG_rCONFIG+CG_wCONFIG)	/* transform flag */
#define	CG_rBDBOX	(CG_rXFORM+CG_wXFORM)	/* bounding box present */
#define	CG_rCLIPF	(CG_rBDBOX+CG_wBDBOX)	/* clipping flags */
#define	CG_rCLIPM	(CG_rCLIPF+CG_wCLIPF)	/* clipping mask */
#define	CG_rVISM	(CG_rCLIPM+CG_wCLIPM)	/* visibility mask */
#define	CG_rNALIAS	(CG_rVISM+CG_wVISM)	/* anti-aliasing */
#define	CG_rRES		(CG_rNALIAS+CG_wMALIAS)	/* reserved */

/*	culling choices	*/
#define	CG_NOCULL	(0<<CG_rCULL)		/* no culling */
#define	CG_BFCULL	(1<<CG_rCULL)		/* backface culling */
#define	CG_FFCULL	(1<<(1+CG_rCULL))	/* frontface culling */

/*	shading	normals choices*/
#define	CG_NONORMS	(0<<CG_rNORMS)		/* no normals */
#define	CG_ENORMS	(1<<CG_rNORMS)		/* one per element */
#define	CG_VNORMS	(1<<(1+CG_rNORMS))	/* one per vertex  */

/*	specular shading choices	*/
#define	CG_NOSPEC	(0<<CG_rSPEC)		/* no specular shading */
#define	CG_SOBJ		(1<<CG_rSPEC)		/* use object color */
#define	CG_SVERT	(1<<(1+CG_rSPEC))	/* use vertex color */

/*	vertex colors choice	*/
#define	CG_NOVCOLS	(0<<CG_rVCOLS)		/* no vertex colors */
#define	CG_VCOLS	(1<<CG_rVCOLS)		/* vertex colors */

/*	depthcueing choices	*/
#define	CG_NODPCUE	(0<<CG_rDPCUE)		/* no depth cueing */
#define	CG_DPCUEBB	(1<<CG_rDPCUE)		/* depth cue, black bg */
#define	CG_DPCUECB	(1<<(1+CG_rDPCUE))	/* depth cue, color bg */
#define	CG_DPCUEFULL	(1<<(2+CG_rDPCUE))	/* depth cue, full form */

/*	shade averaging choice	*/
#define	CG_NOSHDAVG	(0<<CG_rSHDAVG)		/* no shade averaging */
#define	CG_SHDAVG	(1<<CG_rSHDAVG)		/* shade averaging */

/*	pseudo color choices	*/
#define	CG_NOPSCOLOR	(0<<CG_rPSCOLOR)	/* no pseudo coloring */
#define	CG_PSCWTAVG	(1<<CG_rPSCOLOR)	/* wt avg pseudo color */
#define	CG_PSCBTSEL	(1<<(1+CG_rPSCOLOR))	/* bit sel pseudo color */

/*	drawing choices	*/
#define	CG_TRIANGLES	(0<<CG_rDRTYP)		/* triangles */
#define	CG_LINES	(1<<CG_rDRTYP)		/* lines */
#define	CG_POINTS	(1<<(1+CG_rDRTYP))	/* points */

/*	graphics board configuration choices	*/
#define	CG_FULL		(0<<CG_rCONFIG)		/* full color planes */
#define	CG_PSEUDO	(0<<CG_rCONFIG)		/* pseudo color planes */
#define	CG_BASE		(1<<CG_rCONFIG)		/* base board planes */

/*	transformation choice	*/
#define	CG_NOXFORM	(0<<CG_rXFORM)		/* no transformation */
#define	CG_XFORM	(1<<CG_rXFORM)		/* transform coordinates */

/*	bounding box choice	*/
#define	CG_NOBDBOX	(0<<CG_rBDBOX)		/* no bounding box */
#define	CG_BDBOX	(1<<CG_rBDBOX)		/* bounding box present */

/*	clipping flags choices	*/
#define	CG_NOCLIP	(0<<CG_rCLIPF)		/* no clipping */
#define	CG_CLFRU	(1<<CG_rCLIPF)		/* frustrum clip */
#define	CG_CLOBR	(1<<(1+CG_rCLIPF))	/* obscuring rects clip */
#define	CG_CLMOD	(1<<(2+CG_rCLIPF))	/* modeling clip */

/*	clipping mask bit definitions	*/
#define	CG_CLNONE	(0<<CG_rCLIPM)		/* no clipping planes */
#define	CG_CLALL	(0x3f<<CG_rCLIPM)	/* all planes */
#define	CG_CLZY		(1<<CG_rCLIPM)		/* yon Z-plane */
#define	CG_CLZH		(1<<(1+CG_rCLIPM))	/* hither Z-plane */
#define	CG_CLYB		(1<<(2+CG_rCLIPM))	/* bottom Y-plane */
#define	CG_CLYT		(1<<(3+CG_rCLIPM))	/* top Y-plane */
#define	CG_CLXL		(1<<(4+CG_rCLIPM))	/* left X-plane */
#define	CG_CLXR		(1<<(5+CG_rCLIPM))	/* right X-plane */

/*	visibility mask choice	*/
#define	CG_NOVISM	(0<<CG_rVISM)		/* no visibility mask */
#define	CG_VISM		(1<<CG_rVISM)		/* visibility mask */

/*	antialiasing choice	*/
#define	CG_NONALIAS	(0<<CG_rNALIAS)		/* no anti-aliasing */
#define	CG_NALIAS	(1<<CG_rNALIAS)		/* anti-aliasing */

/*	masks for unshifted control bit fields	*/
#define	CG_mCULL	(CG_fCULL<<CG_rCULL)	   /* culling flags */
#define	CG_mNORMS	(CG_fNORMS<<CG_rNORMS)	   /* shading normals */
#define	CG_mSPEC	(CG_fSPEC<<CG_rSPEC)	   /* specular shading */
#define	CG_mVCOLS	(CG_fVCOLS<<CG_rVCOLS)	   /* vertex colors */
#define	CG_mDPCUE	(CG_fDPCUE<<CG_rDPCUE)	   /* depth cueing flags */
#define	CG_mSHDAVG	(CG_fSHDAVG<<CG_rSHDAVG)   /* shade average */
#define	CG_mPSCOLOR	(CG_fPSCOLOR<<CG_rPSCOLOR) /* pseudocolor */
#define	CG_mDRTYP	(CG_fDRTYP<<CG_rDRTYP)	   /* drawn figure type */
#define	CG_mCONFIG	(CG_fCONFIG<<CG_rCONFIG)   /* configuration */
#define	CG_mXFORM	(CG_fXFORM<<CG_rXFORM)	   /* transform flag */
#define	CG_mBDBOX	(CG_fBDBOX<<CG_rBDBOX)	   /* bd-box present */
#define	CG_mCLIPF	(CG_fCLIPF<<CG_rCLIPF)	   /* clipping flag */
#define	CG_mCLIPM	(CG_fCLIPM<<CG_rCLIPM)	   /* clipping mask */
#define	CG_mVISM	(CG_fVISM<<CG_rVISM)	   /* visibility mask */
#define	CG_mNALIAS	(CG_fNALIAS<<CG_rNALIAS)   /* anti-aliasing  */
#define	CG_mRES		(CG_fRES<<CG_rRES)	   /* reserved */

#else
/*	assembler language definitions	*/

/*	visibility mask field definitions	*/
#define	TV1		1<<0		/* triangle vertex 1 */
#define	TV2		1<<1		/* triangle vertex 2 */
#define	TV3		1<<2		/* triangle vertex 3 */
#define	TL12		1<<3		/* triangle v1-v2 edge */
#define	TL13		1<<4		/* triangle v1-v3 edge */
#define	TL23		1<<5		/* triangle v2-v3 edge */

/*	generic values	*/
#define	NO		0		/* generic negative */
#define	NIL		0		/* generic nullity */
#define	OFF		0		/* generic absence */
#define	NULL		0		/* null pointer value */
#define	NONE		0		/* none supplied */
#define	ON		1		/* generic presence */
#define	YES		1		/* generic affirmative */
#define	EXC		-1		/* generic exception */

/*	control bit field widths	*/
#define	wCULL		2			/* culling flags */
#define	wNORMS		2			/* shading normals */
#define	wSPEC		2			/* specular shading */
#define	wVCOLS		1			/* vertex colors */
#define	wDPCUE		3			/* depth cueing flags */
#define	wSHDAVG		1			/* shade average */
#define	wPSCOLOR	2			/* pseudocolor */
#define	wDRTYP		2			/* drawn figure type */
#define	wCONFIG		1			/* configuration */
#define	wXFORM		1			/* transform flag */
#define	wBDBOX		1			/* bounding box present */
#define	wCLIPF		3			/* clipping flags */
#define	wCLIPM		6			/* clipping mask */
#define	wVISM		1			/* visibility mask */
#define	wNALIAS		1			/* anti-aliasing */
#define	wWCLIP		1			/* clip using W */
#define	wSAVW		1			/* store W, 1/W coords */
#define	wMESH		1			/* mesh geometry */

/*	field masks for right aligned control bit fields	*/
#define	fCULL		(3<<0)			/* culling flags */
#define	fNORMS		(3<<0)			/* shading normals */
#define	fSPEC		(3<<0)			/* specular shading */
#define	fVCOLS		(1<<0)			/* vertex colors */
#define	fDPCUE		(7<<0)			/* depth cueing flags */
#define	fSHDAVG		(1<<0)			/* shade average */
#define	fPSCOLOR	(3<<0)			/* pseudocolor */
#define	fDRTYP		(3<<0)			/* drawn figure type */
#define	fCONFIG		(1<<0)			/* configuration */
#define	fXFORM		(1<<0)			/* transform flag */
#define	fBDBOX		(1<<0)			/* bounding box present */
#define	fCLIPF		(7<<0)			/* clipping flags */
#define	fCLIPM		(0x3f<<0)		/* clipping mask */
#define	fVISM		(1<<0)			/* visibility mask */
#define	fNALIAS		(1<<0)			/* anti-aliasing */
#define	fWCLIP		(1<<0)			/* clip using W */
#define	fSAVW		(1<<0)			/* store W, 1/W coords */
#define	fMESH		(1<<0)			/* mesh geometry */

/*	right bit positions for control bit fields	*/
#define	rCULL		0			/* culling flags */
#define	rNORMS		(rCULL+wCULL)		/* shading normals */
#define	rSPEC		(rNORMS+wNORMS)		/* specular shading */
#define	rVCOLS		(rSPEC+wSPEC)		/* vertex colors */
#define	rDPCUE		(rVCOLS+wVCOLS)		/* depth cueing flags */
#define	rSHDAVG		(rDPCUE+wDPCUE)		/* shade average */
#define	rPSCOLOR	(rSHDAVG+wSHDAVG)	/* pseudocolor */
#define	rDRTYP		(rPSCOLOR+wPSCOLOR)	/* drawn figure type */
#define	rCONFIG		(rDRTYP+wDRTYP)		/* configuration */
#define	rXFORM		(rCONFIG+wCONFIG)	/* transform flag */
#define	rBDBOX		(rXFORM+wXFORM)		/* bounding box present */
#define	rCLIPF		(rBDBOX+wBDBOX)		/* clipping flags */
#define	rCLIPM		(rCLIPF+wCLIPF)		/* clipping mask */
#define	rVISM		(rCLIPM+wCLIPM)		/* visibility mask */
#define	rNALIAS		(rVISM+wVISM)		/* anti-aliasing */
#define	rWCLIP		(rNALIAS+wNALIAS)	/* clip using W */
#define	rSAVW		(rWCLIP+wWCLIP)		/* store W, 1/W coords */
#define	rMESH		(rSAVW+wSAVW)		/* mesh geometry */

/*	culling choices	*/
#define	NOCULL		(0<<rCULL)		/* no culling */
#define	BFCULL		(1<<rCULL)		/* backface culling */
#define	FFCULL		(1<<(1+rCULL))		/* frontface culling */

/*	left-align shifts for culling choices	*/
#define	BFCULLSH	1			/* backface culling */
#define	FFCULLSH	0			/* frontface culling */

/*	shading	normals choices */
#define	NONORMS		(0<<rNORMS)		/* no normals */
#define	ENORMS		(1<<rNORMS)		/* one per element */
#define	VNORMS		(1<<(1+rNORMS))		/* one per vertex  */

/*	left-align shifts for normals choices	*/
#define	ENORMSSH	1			/* one per element */
#define	VNORMSSH	0			/* one per vertex */

/*	specular shading choices	*/
#define	NOSPEC		(0<<rSPEC)		/* no specular shading */
#define	SOBJ		(1<<rSPEC)		/* use object color */
#define	SVERT		(1<<(1+rSPEC))		/* use vertex color */

/*	left-align shifts for specular shading choices	*/
#define	SOBJSH		1			/* use object color */
#define	SVERTSH		0			/* use vertex color */

/*	vertex colors choice	*/
#define	NOVCOLS		(0<<rVCOLS)		/* no vertex colors */
#define	VCOLS		(1<<rVCOLS)		/* vertex colors */

/*	depthcueing choices	*/
#define	NODPCUE		(0<<rDPCUE)		/* no depth cueing */
#define	DPCUEBB		(1<<rDPCUE)		/* depth cue, black bg */
#define	DPCUECB		(1<<(1+rDPCUE))		/* depth cue, color bg */
#define	DPCUEFULL	(1<<(2+rDPCUE))		/* depth cue, full form */

/* left-align shifts for depthcueing choices */
#define	DPCUEBBSH	2			/* depth cue, black bg */
#define	DPCUECBSH	1			/* depth cue, colored bg */
#define	DPCUEFULLSH	0			/* depth cue, full form */

/*	shade averaging choice	*/
#define	NOSHDAVG	(0<<rSHDAVG)		/* no shade averaging */
#define	SHDAVG		(1<<rSHDAVG)		/* shade averaging */

/*	pseudo color choices	*/
#define	NOPSCOLOR	(0<<rPSCOLOR)		/* no pseudo coloring */
#define	PSCWTAVG	(1<<rPSCOLOR)		/* wt avg pseudo color */
#define	PSCBTSEL	(1<<(1+rPSCOLOR))	/* bit sel pseudo color */

/*	left-align shifts for pseudocolor choices	*/
#define	PSCWTAVGSH	1			/* wt avg pseudo color */
#define	PSCBTSELSH	0			/* bit sel pseudo color */

/*	drawing choices	*/
#define	TRIANGLES	(0<<rDRTYP)		/* triangles */
#define	LINES		(1<<rDRTYP)		/* lines */
#define	POINTS		(1<<(1+rDRTYP))		/* points */

/*	left-align shifts for drawing choices	*/
#define	LINESSH		1			/* lines */
#define	POINTSSH	0			/* points */

/*	graphics board configuration choices	*/
#define	FULL		(0<<rCONFIG)		/* all color planes */
#define	PSEUDO		(0<<rCONFIG)		/* pseudo color planes */
#define	BASE		(1<<rCONFIG)		/* base board planes */

/*	transformation choice	*/
#define	NOXFORM		(0<<rXFORM)		/* no transformation */
#define	XFORM		(1<<rXFORM)		/* transform coordinates */

/*	bounding box choice	*/
#define	NOBDBOX		(0<<CG_rBDBOX)		/* no bounding box */
#define	BDBOX		(1<<CG_rBDBOX)		/* bounding box present */

/*	clipping flags choices	*/
#define	NOCLIP		(0<<rCLIPF)		/* no clipping */
#define	CLFRU		(1<<rCLIPF)		/* frustrum clip */
#define	CLOBR		(1<<(1+rCLIPF))		/* obscuring rects clip */
#define	CLMOD		(1<<(2+rCLIPF))		/* modeling clip */

/*	left-align shifts for clipping flags choices	*/
#define	CLFRUSH		2			/* frustrum clip */
#define	CLOBRSH		1			/* obscuring rects clip */
#define	CLMODSH		0			/* modeling clip */

/*	clipping mask bit definitions	*/
#define	CLNONE		(0<<rCLIPM)		/* no clipping planes */
#define	CLALL		(0x3f<<rCLIPM)		/* all planes */
#define	CLZY		(1<<rCLIPM)		/* yon Z-plane */
#define	CLZH		(1<<(1+rCLIPM))		/* hither Z-plane */
#define	CLYB		(1<<(2+rCLIPM))		/* bottom Y-plane */
#define	CLYT		(1<<(3+rCLIPM))		/* top Y-plane */
#define	CLXL		(1<<(4+rCLIPM))		/* left X-plane */
#define	CLXR		(1<<(5+rCLIPM))		/* right X-plane */

/* left-align shifts for clipping mask	*/
#define	CLZYSH		5			/* yon Z-plane */
#define	CLZHSH		4			/* hither Z-plane */
#define	CLYBSH		3			/* bottom Y-plane */
#define	CLYTSH		2			/* top Y-plane */
#define	CLXLSH		1			/* left X-plane */
#define	CLXRSH		0			/* right X-plane */

/*	visibility mask choice	*/
#define	NOVISM		(0<<rVISM)		/* no visibility mask */
#define	VISM		(1<<rVISM)		/* visibility mask */

/*	anti-aliasing choice	*/
#define	NONALIAS	(0<<rNALIAS)		/* no anti-aliasing */
#define	NALIAS		(1<<rNALIAS)		/* anti-aliasing */

/*	internally derived controls	*/
#define	WCLIP		(1<<rWCLIP)		/* clip using W */
#define	SAVW		(1<<rSAVW)		/* store W, 1/W */
#define	MESH		(1<<rMESH)		/* mesh geometry */

/*	masks for unshifted control bit fields	*/
#define	mCULL		(fCULL<<rCULL)		/* culling flags */
#define	mNORMS		(fNORMS<<rNORMS)	/* shading normals */
#define	mSPEC		(fSPEC<<rSPEC)		/* specular shading */
#define	mVCOLS		(fVCOLS<<rVCOLS)	/* vertex colors */
#define	mDPCUE		(fDPCUE<<rDPCUE)	/* depth cueing flags */
#define	mSHDAVG		(fSHDAVG<<rSHDAVG)	/* shade average */
#define	mPSCOLOR	(fPSCOLOR<<rPSCOLOR)	/* pseudocolor */
#define	mDRTYP		(fDRTYP<<rDRTYP)	/* drawn figure type */
#define	mCONFIG		(fCONFIG<<rCONFIG)	/* configuration */
#define	mXFORM		(fXFORM<<rXFORM)	/* transform flag */
#define	mBDBOX		(fBDBOX<<rBDBOX)	/* bounding box present */
#define	mCLIPF		(fCLIPF<<rCLIPF)	/* clipping flags */
#define	mCLIPM		(fCLIPM<<rCLIPM)	/* clipping mask */
#define	mVISM		(fVISM<<rVISM)		/* visibility mask */
#define	mNALIAS		(fNALIAS<<rNALIAS)	/* anti-aliasing */
#define	mWCLIP		(fWCLIP<<rWCLIP)	/* clip using W */
#define	mSAVW		(fSAVW<<rSAVW)		/* store W, 1/W */
#define	mMESH		(fMESH<<rMESH)		/* mesh geometry */

/*	left shift amounts for control bit fields, sll left aligns	*/
#define	lCULL		(32-rCULL-wCULL)	/* culling flags */
#define	lNORMS		(32-rNORMS-wNORMS)	/* shading normals */
#define	lSPEC		(32-rSPEC-wSPEC)	/* specular shading */
#define	lVCOLS		(32-rVCOLS-wVCOLS)	/* vertex colors */
#define	lDPCUE		(32-rDPCUE-wDPCUE)	/* depth cue */
#define	lSHDAVG		(32-rSHDAVG-wSHDAVG)	/* shade average */
#define	lPSCOLOR	(32-rPSCOLOR-wPSCOLOR)	/* pseudocolor */
#define	lDRTYP		(32-rDRTYP-wDRTYP)	/* drawn figure type */
#define	lCONFIG		(32-rCONFIG-wCONFIG)	/* configuration */
#define	lXFORM		(32-rXFORM-wXFORM)	/* transform flag */
#define	lBDBOX		(32-rBDBOX-wBDBOX)	/* bounding box present */
#define	lCLIPF		(32-rCLIPF-wCLIPF)	/* clipping flags */
#define	lCLIPM		(32-rCLIPM-wCLIPM)	/* clipping mask */
#define	lVISM		(32-rVISM-wVISM)	/* visibility mask */
#define	lNALIAS		(32-rNALIAS-wNALIAS)	/* anti-aliasing */
#define	lWCLIP		(32-rWCLIP-wWCLIP)	/* clip using W */
#define	lSAVW		(32-rSAVW-wSAVW)	/* store W, 1/W */
#define	lMESH		(32-rMESH-wMESH)	/* mesh geometry */

/*	left shift amounts for bits within control bit fields	*/
#define	bBFCULL		(lCULL+BFCULLSH)	/* backface cull */
#define	bFFCULL		(lCULL+FFCULLSH)	/* frontface cull */
#define	bENORMS		(lNORMS+ENORMSSH)	/* one per element */
#define	bVNORMS		(lNORMS+VNORMSSH)	/* one per vertex */
#define	bSOBJ		(lSPEC+SOBJSH)		/* object color highlights */
#define	bSVERT		(lSPEC+SVERTSH)		/* vertex color highlights */
#define	bVCOLS		(lVCOLS)		/* vertex colors */
#define	bDPCUEBB	(lDPCUE+DPCUEBBSH)	/* black background */
#define	bDPCUECB	(lDPCUE+DPCUECBSH)	/* colored background */
#define	bDPCUEFULL	(lDPCUE+DPCUEFULLSH)	/* full algorithm */
#define	bSHDAVG		(lSHDAVG)		/* shade average */
#define	bPSCWTAVG	(lPSCOLOR+PSCWTAVGSH)	/* wt avg pseudocolor */
#define	bPSCBTSEL	(lPSCOLOR+PSCBTSELSH)	/* bit select pseudocolor */
#define	bLINES		(lDRTYP+LINESSH)	/* draw lines */
#define	bPOINTS		(lDRTYP+POINTSSH)	/* draw points */
#define	bFULL		(lCONFIG)		/* configuration */
#define	bPSEUDO		(lCONFIG)		/* configuration */
#define	bBASE		(lCONFIG)		/* configuration */
#define	bXFORM		(lXFORM)		/* transform flag */
#define	bBDBOX		(lBDBOX)		/* bounding box present */
#define	bCLFRU		(lCLIPF+CLFRUSH)	/* frustrum clip */
#define	bCLOBR		(lCLIPF+CLOBRSH)	/* obscuring rects clip */
#define	bCLMOD		(lCLIPF+CLMODSH)	/* modeling clip */
#define	bCLXR		(lCLIPM+CLXRSH)		/* right X coord */
#define	bCLXL		(lCLIPM+CLXLSH)		/* left X coord */
#define	bCLYT		(lCLIPM+CLYTSH)		/* top Y coord */
#define	bCLYB		(lCLIPM+CLYBSH)		/* bottom Y coord */
#define	bCLZH		(lCLIPM+CLZHSH)		/* hither Z coord */
#define	bCLZY		(lCLIPM+CLZYSH)		/* yon Z coord */
#define	bVISM		(lVISM)			/* visibility mask */
#define	bNALIAS		(lNALIAS)		/* anti-aliasing */
#define	bWCLIP		(lWCLIP)		/* clip using W */
#define	bSAVW		(lSAVW)			/* store W, 1/W */
#define	bMESH		(lMESH)			/* mesh structure */


#endif


