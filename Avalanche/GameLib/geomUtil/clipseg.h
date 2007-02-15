//***************************************************************************
// I believe that this is a Cohen-Sutherland line clipping algorithm.
//
// A class to helps you clip segments into a box.  The box is treated as
// being in the XZ plane.  You can treat the XZ coordinates however you
// want to do this in the XY, YZ planes too.
//
// This class works by setting up a set of sectors like:
//
//        |         |
//   6    |    7    |    8
// -------+---------+-------- maxz
//        |         |
//   3    |    4    |    5
// -------+---------+-------- minz
//        |         |
//   0    |    1    |    2
//      minx       maxx
//
// Segments are clipped based on what two sectors the endpoints lie.
//***************************************************************************
#ifndef CLIPSEG_H
#define CLIPSEG_H

#include "Math/Vector.h"

// This is gross
class ClipSegment;
typedef bool (ClipSegment::*ClipFunc)(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);

class ClipSegment
{
public:
   // The constructors
   ClipSegment(void) {};
   ClipSegment(float theMinX, float theMinZ, float theMaxX, float theMaxZ)
      { minx = theMinX; minz = theMinZ; maxx = theMaxX; maxz = theMaxZ; }
   ~ClipSegment(void) {};

   // Setup the extents of the clipping region
   void SetExtents(float theMinX, float theMinZ, float theMaxX, float theMaxZ)
      { minx = theMinX; minz = theMinZ; maxx = theMaxX; maxz = theMaxZ; }

   // Clip the segment a-b to aclipped-bclipped
   // Return true if some portion of the segment lies the clipping region,
   // false if not.  On a false return, aclipped and bclipped are not set.
   inline bool Clip(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);

private:
   // The bounding box
   float minx, minz;
   float maxx, maxz;

   // Static table to help clip quickly
   static ClipFunc sectorClip[9][9];

	// Type for SolveIntersection return
	typedef enum
	{
		XSET =	0,
		ZSET =	1
	} SOLVERETURN;

   // Really fast clipping stuff
   static inline void FigureABC(Vector3CRef a, Vector3CRef b, float *ABC);
   static inline SOLVERETURN SolveXIntersection(Vector3CRef a, float *ABC, float xval, Vector3 &clipped);
   static inline SOLVERETURN SolveZIntersection(Vector3CRef a, float *ABC, float zval, Vector3 &clipped);
   static inline SOLVERETURN SolveXZIntersectionFar(Vector3CRef a, float *ABC, float xval, float zval, Vector3 &clipped);
   static inline SOLVERETURN SolveXZIntersectionNear(Vector3CRef a, float *ABC, float xval, float zval, Vector3 &clipped);

	inline bool InXRange(float x)
		{ return(x >= minx && x <= maxx); }
	inline bool InZRange(float z)
		{ return(z >= minz && z <= maxz); }
	inline bool TestRange(SOLVERETURN ret, Vector3 &clipped)
		{ if (ret == XSET) return(InZRange(clipped.z())); else return(InXRange(clipped.x())); }

   // The clipping functions
   bool Clip04(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip05(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip07(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip08(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip13(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip14(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip15(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip16(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip17(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip18(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip23(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip24(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip26(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip27(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip34(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip35(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip37(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip38(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip44(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip45(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip46(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip47(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip48(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip56(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
   bool Clip57(Vector3CRef a, Vector3CRef b, Vector3 &aclipped, Vector3 &bclipped);
};

// Include the implementation
#include "geomUtil/clipseg.hpp"

#endif
