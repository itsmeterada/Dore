#ident "%W%" %G%
 
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
 
/*	This routine is used to save and restore a single copy of
	the entire user-state fpu status.  It is designed for use
	by Dore when implementing valuator calls.  The save and
	restore both are performed solely in user mode.

	AC0-AC3 are saved in vrf locations $V3.0.28 - $V3.0.31,
	and the entire 1024 vrf words are written to memory in
	thread local space.  Twenty-five user state registers are
	then saved.  The fpu is made completely idel before the
	save takes place.  It is also made idle before the state
	is restored.

	This is a leaf routine and does no save or restore of
	integer regs at all.  No stack frame is created.

*/


 #	save area definitions
#define	SZ_VRF	  (4*256*8)		/* vrf save area size */
#define	SZ_REGS	  (4*26)		/* register save area size */
#define	SZ_STATE  (SZ_VRF+SZ_REGS)	/* total save area size */


 #	fpu register definitions
#define	RELOC_MASK	0xf0ff		/* removes sbase from reg_ */
#define	STATUS		0x0c0		/* fpu status */
#define	REGA		0x720		/* reg_a */
#define	REGB		0x760		/* reg_b */
#define	REGD		0x7a0		/* reg_d */
#define	BASEA		0x820		/* base_a */
#define	BASEB		0x920		/* base_b */
#define	BASED		0xa20		/* base_d */
#define	UBASE		0x150		/* ubase */
#define	MASKOFFSET	0x310		/* mask offset */
#define	MASKSTATUS0	0x320		/* mask status 0 */
#define	MASKSTATUS1	0x324		/* mask status 1 */
#define	MODE		0x108		/* fpu mode */
#define	LENGTH		0x880		/* vector length */
#define	STRIDEA		0x830		/* stride_a */
#define	STRIDEB		0x930		/* stride_b */
#define	STRIDED		0xa30		/* stride_d */
#define	OPSTATE1	0x7b4		/* opstate 1 */
#define	MASK		0x200		/* first mask word (of 8) */


 #	save area offset definitions
#define	status		0(t9)		/* fpu status save area */	
#define	rega		4(t9)		/* reg_a save area */
#define	regb		8(t9)		/* reg_b save area */
#define	regd		12(t9)		/* reg_d save area */
#define	basea		16(t9)		/* base_a save area */
#define	baseb		20(t9)		/* base_b save area */
#define	based		24(t9)		/* base_d save area */
#define	ubase		28(t9)		/* ubase save area */
#define	maskoffset	32(t9)		/* mask offset save area */
#define	maskstatus0	36(t9)		/* mask status 0 save area */
#define	maskstatus1	40(t9)		/* mask status 1 save area */
#define	mode		44(t9)		/* fpu mode save area */
#define	length		48(t9)		/* vector length save area */
#define	stridea		52(t9)		/* stride_a save area */
#define	strideb		56(t9)		/* stride_b save area */
#define	strided		60(t9)		/* stride_dsave area  */
#define	opstate1	64(t9)		/* opstate 1 save area */
#define	mask0		68(t9)		/* first mask word save area */
#define	mask1		72(t9)		/* second mask word save area */
#define	mask2		76(t9)		/* third mask word save area */
#define	mask3		80(t9)		/* fourth mask word save area */
#define	mask4		84(t9)		/* fifth mask word save area */
#define	mask5		88(t9)		/* sixth mask word save area */
#define	mask6		92(t9)		/* seventh mask word save area */
#define	mask7		96(t9)		/* eighth mask word save area */
#define	pad0		100(t9)		/* padding word */
#define	vrf0area	104(t9)		/* vrf bank 0 save area */
#define	vrf1area	2048+vrf0area	/* vrf bank 1 save area */
#define	vrf2area	2048+vrf1area	/* vrf bank 2 save area */
#define	vrf3area	2048+vrf2area	/* vrf bank 3 save area */


/*	fpu register addresses and operation addresses	*/
#define	fl( FR )					\
	(0xffff8000 + ((FR) & 0x7fff))(gp)



#include	<regdef.h>

	.set	noreorder
	.text

	.globl	_fpusave
	.globl	_fpurestore


 #	here to save fpu state
_fpusave:
	la	t9,fpustate		# -> save area
	wfpu				# let all fpu activity complete

 #	save fpu register values
	lw	t0,fl(STATUS)		# fpu status
	lw	t2,fl(REGB)		# reg_b
	sw	t0,status
	lw	t4,fl(BASEA)		# base_a
	and	t2,RELOC_MASK		# remove sbase increment
	sw	t2,regb
	lw	t6,fl(BASED)		# base_d
	sw	t4,basea
	lw	t1,fl(REGA)		# reg_a
	sw	t6,based
	lw	t3,fl(REGD)		# reg_d
	and	t1,RELOC_MASK		# remove sbase increment
	sw	t1,rega
	lw	t5,fl(BASEB)		# base_b
	and	t3,RELOC_MASK		# remove sbase increment
	sw	t3,regd
	lw	t7,fl(UBASE)		# ubase
	sw	t5,baseb
	lw	t0,fl(MASKOFFSET)	# mask offset
	sw	t7,ubase
	lw	t2,fl(MASKSTATUS1)	# mask status 1
	sw	t0,maskoffset
	lw	t4,fl(LENGTH)		# fpu length
	sw	t2,maskstatus1
	lw	t6,fl(STRIDEB)		# stride_b
	sw	t4,length
	lw	t8,fl(OPSTATE1)		# opstate1
	sw	t6,strideb
	lw	t1,fl(MASKSTATUS0)	# mask status 0
	sw	t8,opstate1
	lw	t3,fl(MODE)		# fpu mode
	sw	t1,maskstatus0
	lw	t5,fl(STRIDEA)		# stride_a
	sw	t3,mode
	lw	t7,fl(STRIDED)		# stride_d
	sw	t5,stridea
	lw	t0,fl(MASK)		# mask words
	sw	t7,strided
	lw	t2,fl(8+MASK)
	sw	t0,mask0
	lw	t4,fl(16+MASK)
	sw	t2,mask2
	lw	t6,fl(24+MASK)
	sw	t4,mask4
	lw	t1,fl(4+MASK)
	sw	t6,mask6
	lw	t3,fl(12+MASK)
	sw	t1,mask1
	lw	t5,fl(20+MASK)
	sw	t3,mask3
	lw	t7,fl(28+MASK)
	sw	t5,mask5
	sw	t7,mask7

	li	t0,256			# length for saving vrf banks
	sw	t0,<vlength>

 #	save all vrf cells
	la	t0,vrf0area		# -> bank 0 vrf save area
	dvst	$V0.0.0,0(t0)		# bank 0
	la	t0,vrf1area		# -> bank 1 vrf save area
	dvst	$V1.0.0,0(t0)		# bank 1
	la	t0,vrf2area		# -> bank 2 save area
	dvst	$V2.0.0,0(t0)		# bank 2
	la	t0,vrf3area		# -> bank 3 save area
	dvst	$V3.0.0,0(t0)		# bank 3
 
 #	accumulator values to vrf cells
	pass4	$V0.0.0,$AC		# accumulators to vrf save cells
	li	t0,4			# length for saving acc banks
	sw	t0,<vlength>
	la	t0,28*8+vrf3area
	dvst	$V0.0.0,0(t0)
	j	ra			# return to caller
	wdi				# fpu idle once stores complete



 #	here to restore fpu state
_fpurestore:
	la	t9,fpustate		# -> save area
	wfpu				# let all fpu activity complete


 #	restore all vrf cells
	li	t0,4			# length for restoring vrf banks
	sw	t0,<vlength>
	la	t0,28*8+vrf3area
	dvldb	$V0.0.0,0(t0)
	pass4	$AC,$V0.0.0		# restore accumulators

	li	t0,256			# length for restoring vrf banks
	sw	t0,<vlength>

	la	t0,vrf0area		# -> bank 0 vrf save area
	dvlda	$V0.0.0,0(t0)		# bank 0
	la	t0,vrf1area		# -> bank 1 vrf save area
	dvldb	$V1.0.0,0(t0)		# bank 1
	la	t0,vrf2area		# -> bank 2 save area
	dvlda	$V2.0.0,0(t0)		# bank 2
	la	t0,vrf3area		# -> bank 3 save area
	dvldb	$V3.0.0,0(t0)		# bank 3


 #	restore fpu register values
	sw	$0,ubase
	lw	t0,status
	lw	t1,rega
	sw	t0,fl(STATUS)		# fpu status
	lw	t2,regb
	sw	t1,fl(REGA)		# reg_a
	lw	t3,regd
	sw	t2,fl(REGB)		# reg_b
	lw	t4,basea
	sw	t3,fl(REGD)		# reg_d
	lw	t5,baseb
	sw	t4,fl(BASEA)		# base_a
	lw	t6,based
	sw	t5,fl(BASEB)		# base_b
	lw	t7,ubase
	sw	t6,fl(BASED)		# base_d
	lw	t0,maskoffset
	sw	t7,fl(UBASE)		# ubase

	lw	t1,maskstatus0
	sw	t0,fl(MASKOFFSET)	# mask offset
	lw	t2,maskstatus1
	sw	t1,fl(MASKSTATUS0)	# mask status 0
	lw	t3,mode
	sw	t2,fl(MASKSTATUS1)	# mask status 1
	lw	t4,length
	sw	t3,fl(MODE)		# fpu mode
	lw	t5,stridea
	sw	t4,fl(LENGTH)		# fpu length
	lw	t6,strideb
	sw	t5,fl(STRIDEA)		# stride_a
	lw	t7,strided
	sw	t6,fl(STRIDEB)		# stride_b
	lw	t8,opstate1
	sw	t7,fl(STRIDED)		# stride_d
	lw	t0,mask0
	sw	t8,fl(OPSTATE1)		# opstate1

	lw	t1,mask1
	sw	t0,fl(MASK)		# mask words
	lw	t2,mask2
	sw	t1,fl(4+MASK)
	lw	t3,mask3
	sw	t2,fl(8+MASK)
	lw	t4,mask4
	sw	t3,fl(12+MASK)
	lw	t5,mask5
	sw	t4,fl(16+MASK)
	lw	t6,mask6
	sw	t5,fl(20+MASK)
	lw	t7,mask7
	sw	t6,fl(24+MASK)
	j	ra			# return to caller
	sw	t7,fl(28+MASK)



 #	data area here, thread local to assure mp possible

	.data
 	.ltcomm	fpustate,SZ_STATE	/* vrf save area */

