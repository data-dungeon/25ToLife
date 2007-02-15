//
// this is a (somewhat) optimized version of bool Segment::TrueIntersectWithPlane(Vector3 &planeP,Vector3 &planeN,Vector3 &i,float &t)
// As the compiler wasnt inlining it anyway, I figured Id take it completely out of the code and save myself the stack manipulations
// etc.  This code takes about 60% of the original amount of time.  But because the of the references and such, we have a lot of
// reads & writes and putting vectors back together that can be avoided by optimizing the upstream function instead (which I will
// probably do as well).  As the 2 primary parents of this function represent 10% of a frame in the majority of circumstances, and
// as much as 40% in unusual circumstances, its probably worth it.
//
// below it is the same thing for Ray::TrueIntersectWithPlane(Vector3 &planeP,Vector3 &planeN,Vector3 &i,float &t)
//
// I say these are somewhat optimized because they still have LOTS of stall slots in them (something nearing 50%) due to resource
// hazards that could probably be resolved (and in fact may be by the time you read this via integration with the parent functions).
//
// The parent functions will probably be done with inline asm since they contain the primary loop that calls this thing, and the
// function prep & cleanup for the parent function is negligable.
//

		  
		  .include "../platform/ps2/ps2asm.i"

		  // GCC 3.0.3 name mangling produces: (these are old!!! need to be updated!!)
		  .global _ZN7Segment22TrueIntersectWithPlaneER7Vector3S1_S1_Rf
		  .global _ZN3Ray22TrueIntersectWithPlaneER7Vector3S1_S1_Rf

		  // GCC 2.95.3 name mangling produces:
		  .global TrueIntersectWithPlane__C7SegmentRC7Vector3T1R7Vector3Rf
		  .global TrueIntersectWithPlane__C3RayRC7Vector3T1R7Vector3Rf

_ZN7Segment22TrueIntersectWithPlaneER7Vector3S1_S1_Rf:
TrueIntersectWithPlane__C7SegmentRC7Vector3T1R7Vector3Rf:	
.set noreorder
		  // ok, hand written now...
		  // arguments
#define pSegment			a0
#define pPlaneP			a1
#define pPlaneN			a2
#define pI					a3
#define pT					t0
#define retVal				v0
		  
#define gStartX			v0
#define gStartY			t1
#define gStartZ			t2
#define gEndX				t3
#define gEndY				t4
#define gEndZ				t5
#define gPlaneNX			t6
#define gPlaneNY			t7
#define gPlaneNZ			t8
#define gPlanePX			t9
#define gPlanePY			v0
#define gPlanePZ			t1

#define gIX					t1
#define gIY					t2
#define gIZ					t3
		  
#define gStart				t2
#define gEnd				t3
#define gPlaneP			t9
#define gTemp				v0

#define vStart				vf01
#define vEnd				vf02
#define vPlaneP			vf03
#define vPlaneNX			vf04
#define vPlaneNY			vf05
#define vPlaneNZ			vf06

#define vDotN				vf07
#define vDotNx				vf07x
#define vAbsDotN			vf08
#define vDiff				vf09
#define vDot				vf10
#define vDotx				vf10x
#define vI					vf11
#define vW					vf12
#define vT					vf13
		  
#define fEpsilon			f0
#define fZero				f1
#define fOne				f0
#define fTest				f2
		  		  
		  // loads & merges
		  lw         gStartX,0x0000(pSegment)
		  lw         gStartZ,0x0008(pSegment)
		  lw         gStartY,0x0004(pSegment)
		  lw         gEndX,0x000C(pSegment)
		  lw         gEndZ,0x0014(pSegment)
		  lw         gEndY,0x0010(pSegment)
		  lw         gPlaneNX,0x0000(pPlaneN)
		  lw         gPlaneNY,0x0004(pPlaneN)
		  lw         gPlaneNZ,0x0008(pPlaneN)
		  lw         gPlanePX,0x0000(pPlaneP)
		  pextlw		 gStart, gStartZ, gStartX
		  lw         gPlanePY,0x0004(pPlaneP)
		  pextlw		 gStart, gStartY, gStart
		  lw         gPlanePZ,0x0008(pPlaneP)

		  // get them onto vu0
		  pextlw		 gEnd, gEndZ, gEndX
		  pextlw		 gEnd, gEndY, gEnd
		  qmtc2		 gStart, vStart
		  pextlw		 gPlaneP, gPlanePZ, gPlanePX
		  mtc1		 zero, fZero
		  pextlw		 gPlaneP, gPlanePY, gPlaneP
		  lui        gTemp,0x33D6

		  qmtc2		 gEnd, vEnd
		  ori        gTemp,gTemp,0xBF95
		  qmtc2		 gPlaneP, vPlaneP
		  mtc1       gTemp,fEpsilon
		  qmtc2		 gPlaneNX, vPlaneNX
		  qmtc2		 gPlaneNY, vPlaneNY
		  qmtc2		 gPlaneNZ, vPlaneNZ
		  
		  // w=end-start
		  vsub		 vW, vEnd, vStart

		  // 	float vDotN = Vector3::Dot(w, planeN);
		  vmulax.x	 ACC, vPlaneNX, vW
		  vmadday.x	 ACC, vPlaneNY, vW
		  vmaddz.x	 vDotN, vPlaneNZ, vW

		  // abs(vDotN)
		  vabs		 vAbsDotN, vDotN

		  // planeP - start
		  vsub		 vDiff, vPlaneP, vStart

		  // Dot((planeP - start), planeN)
		  vmulax.x	 ACC, vPlaneNX, vDiff
		  vmadday.x	 ACC, vPlaneNY, vDiff
		  vmaddz.x	 vDot, vPlaneNZ, vDiff

		  // vDot / vDotN
		  vdiv		 Q, vDotx, vDotNx

		  // 	if(vDotN < 1.0e-7f) return false;
		  qmfc2		 gTemp, vAbsDotN
		  mtc1		 gTemp, fTest
		  lui        t9,0x3F80
		  c.olt.s	 fTest, fEpsilon
		  nop
		  bc1t	    SegDoneFailure

		  // load fOne
		  mtc1       t9,fOne
		  
		  // check divide
		  vwaitq
		  vaddq	    vT, vf00, Q

		  // 	if(t < 0.0f || t > 1.0f) return false;
		  qmfc2		 gTemp, vT
		  mtc1		 gTemp, fTest
		  sw			 gTemp, 0x0000(pT)
		  c.olt.s	 fTest, fZero
		  nop
		  bc1t	    SegDoneFailure

		  vaddax		 ACC, vStart, vf00x
		  c.ole.s	 fTest, fOne
		  nop
		  bc1f		 SegDoneFailure
		  
		  // i = start + w * t;
		  vmaddx		 vI, vW, vT

		  // save I back out...
		  qmfc2		 gIX, vI
		  pexew		 gIZ, gIX
		  prot3w		 gIY, gIX

		  sw			 gIX, 0x0000(pI)
		  sw			 gIY, 0x0004(pI)
		  sw			 gIZ, 0x0008(pI)
		  jr         ra
		  addiu		 retVal, zero, 1
SegDoneFailure:	  
		  jr         ra
		  addiu		 retVal, zero, 0
.set reorder

_ZN3Ray22TrueIntersectWithPlaneER7Vector3S1_S1_Rf:
TrueIntersectWithPlane__C3RayRC7Vector3T1R7Vector3Rf:		
.set noreorder
		  // ok, hand written now...
		  // arguments
#define pRay				a0
#define pPlaneP			a1
#define pPlaneN			a2
#define pI					a3
#define pT					t0
#define retVal				v0
		  
#define gOrigX				v0
#define gOrigY				t1
#define gOrigZ				t2
#define gDirX				t3
#define gDirY				t4
#define gDirZ				t5
#define gPlaneNX			t6
#define gPlaneNY			t7
#define gPlaneNZ			t8
#define gPlanePX			t9
#define gPlanePY			v0
#define gPlanePZ			t1

#define gIX					t1
#define gIY					t2
#define gIZ					t3
		  
#define gOrig				t2
#define gDir				t3
#define gPlaneP			t9
#define gTemp				v0

#define vOrig				vf01
#define vDir				vf02
#define vPlaneP			vf03
#define vPlaneNX			vf04
#define vPlaneNY			vf05
#define vPlaneNZ			vf06

#define vDotN				vf07
#define vDotNx				vf07x
#define vAbsDotN			vf08
#define vDiff				vf09
#define vDot				vf10
#define vDotx				vf10x
#define vI					vf11
#define vW					vf12
#define vT					vf13
		  
#define fEpsilon			f0
#define fZero				f1
#define fOne				f0
#define fTest				f2
		  		  
		  // loads & merges
		  lw         gOrigX,0x0000(pRay)
		  lw         gOrigZ,0x0008(pRay)
		  lw         gOrigY,0x0004(pRay)
		  lw         gDirX,0x000C(pRay)
		  lw         gDirZ,0x0014(pRay)
		  lw         gDirY,0x0010(pRay)
		  lw         gPlaneNX,0x0000(pPlaneN)
		  lw         gPlaneNY,0x0004(pPlaneN)
		  lw         gPlaneNZ,0x0008(pPlaneN)
		  lw         gPlanePX,0x0000(pPlaneP)
		  pextlw		 gOrig, gOrigZ, gOrigX
		  lw         gPlanePY,0x0004(pPlaneP)
		  pextlw		 gOrig, gOrigY, gOrig
		  lw         gPlanePZ,0x0008(pPlaneP)

		  // get them onto vu0
		  pextlw		 gDir, gDirZ, gDirX
		  pextlw		 gDir, gDirY, gDir
		  qmtc2		 gOrig, vOrig
		  pextlw		 gPlaneP, gPlanePZ, gPlanePX
		  mtc1		 zero, fZero
		  pextlw		 gPlaneP, gPlanePY, gPlaneP
		  lui        gTemp,0x33D6

		  qmtc2		 gDir, vDir
		  ori        gTemp,gTemp,0xBF95
		  qmtc2		 gPlaneP, vPlaneP
		  mtc1       gTemp,fEpsilon
		  qmtc2		 gPlaneNX, vPlaneNX
		  qmtc2		 gPlaneNY, vPlaneNY
		  qmtc2		 gPlaneNZ, vPlaneNZ
		  
		  // 		float dirDotN = Vector3::Dot(dir, planeN);
		  vmulax.x	 ACC, vPlaneNX, vDir
		  vmadday.x	 ACC, vPlaneNY, vDir
		  vmaddz.x	 vDotN, vPlaneNZ, vDir

		  // abs(vDotN)
		  vabs		 vAbsDotN, vDotN

		  // planeP - orig
		  vsub		 vDiff, vPlaneP, vOrig

		  // Dot((planeP - orig), planeN)
		  vmulax.x	 ACC, vPlaneNX, vDiff
		  vmadday.x	 ACC, vPlaneNY, vDiff
		  vmaddz.x	 vDot, vPlaneNZ, vDiff

		  // vDot / vDotN
		  vdiv		 Q, vDotx, vDotNx

		  // 	if(vDotN < 1.0e-7f) return false;
		  qmfc2		 gTemp, vAbsDotN
		  mtc1		 gTemp, fTest
		  nop
		  c.olt.s	 fTest, fEpsilon
		  nop
		  bc1t	    RayDoneFailure

		  // check divide
		  vwaitq
		  vaddq	    vT, vf00, Q

		  // 	if(t < 0.0f) return false;
		  qmfc2		 gTemp, vT
		  mtc1		 gTemp, fTest
		  sw			 gTemp, 0x0000(pT)
		  c.olt.s	 fTest, fZero
		  vaddax		 ACC, vOrig, vf00x
		  bc1t	    RayDoneFailure

		  
		  // i = orig + dir * t;
		  vmaddx		 vI, vDir, vT

		  // save I back out...
		  qmfc2		 gIX, vI
		  pexew		 gIZ, gIX
		  prot3w		 gIY, gIX

		  sw			 gIX, 0x0000(pI)
		  sw			 gIY, 0x0004(pI)
		  sw			 gIZ, 0x0008(pI)
		  jr         ra
		  addiu		 retVal, zero, 1
RayDoneFailure:	  
		  jr         ra
		  addiu		 retVal, zero, 0
.set reorder
  
