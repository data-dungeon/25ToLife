//***************************************************************************
// The segment clipping class
//***************************************************************************

/******************* includes ****************************/

#include "geomUtil/GeomUtilPCH.h"





// A table to help quickly reject segments out of the height field
ClipFunc ClipSegment::sectorClip[9][9] =
{
   //                   0                     1                     2                     3                     4                     5                     6                     7                     8
   {                    0,                    0,                    0,                    0, &ClipSegment::Clip04, &ClipSegment::Clip05,                    0, &ClipSegment::Clip07, &ClipSegment::Clip08 },
   {                    0,                    0,                    0, &ClipSegment::Clip13, &ClipSegment::Clip14, &ClipSegment::Clip15, &ClipSegment::Clip16, &ClipSegment::Clip17, &ClipSegment::Clip18 },
   {                    0,                    0,                    0, &ClipSegment::Clip23, &ClipSegment::Clip24,                    0, &ClipSegment::Clip26, &ClipSegment::Clip27,                    0 },
   {                    0, &ClipSegment::Clip13, &ClipSegment::Clip23,                    0, &ClipSegment::Clip34, &ClipSegment::Clip35,                    0, &ClipSegment::Clip37, &ClipSegment::Clip38 },
   { &ClipSegment::Clip04, &ClipSegment::Clip14, &ClipSegment::Clip24, &ClipSegment::Clip34, &ClipSegment::Clip44, &ClipSegment::Clip45, &ClipSegment::Clip46, &ClipSegment::Clip47, &ClipSegment::Clip48 },
   { &ClipSegment::Clip05, &ClipSegment::Clip15,                    0, &ClipSegment::Clip35, &ClipSegment::Clip45,                    0, &ClipSegment::Clip56, &ClipSegment::Clip57,                    0 },
   {                    0, &ClipSegment::Clip16, &ClipSegment::Clip26,                    0, &ClipSegment::Clip46, &ClipSegment::Clip56,                    0,                    0,                    0 },
   { &ClipSegment::Clip07, &ClipSegment::Clip17, &ClipSegment::Clip27, &ClipSegment::Clip37, &ClipSegment::Clip47, &ClipSegment::Clip57,                    0,                    0,                    0 },
   { &ClipSegment::Clip08, &ClipSegment::Clip18,                    0, &ClipSegment::Clip38, &ClipSegment::Clip48,                    0,                    0,                    0,                    0 }
};

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip04(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   SolveXZIntersectionFar(a, ABC, minx, minz, aclipped);
   bclipped = b;
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip05(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   // solve agains the maxx line
   SolveXIntersection(a, ABC, maxx, bclipped);
   if (!InZRange(bclipped.z()))
      return(false);

	// Figure out the close intersection
	SolveXZIntersectionFar(a, ABC, minx, minz, aclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip07(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   // Solve against maxz
   SolveZIntersection(a, ABC, maxz, bclipped);
   if (!InXRange(bclipped.x()))
      return(false);

   SolveXZIntersectionFar(a, ABC, minx, minz, aclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip08(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   // Solve the 0 sector intersection
   SOLVERETURN ret = SolveXZIntersectionFar(a, ABC, minx, minz, aclipped);
   if (!TestRange(ret, aclipped))
      return(false);

   SolveXZIntersectionNear(a, ABC, maxx, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip13(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   // Solve against the minz line
   SolveZIntersection(a, ABC, minz, aclipped);
   if (!InXRange(aclipped.x()))
      return(false);

   SolveXIntersection(a, ABC, minx, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip14(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   SolveZIntersection(a, ABC, minz, aclipped);
   bclipped = b;
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip15(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   // Solve against the minz line
   SolveZIntersection(a, ABC, minz, aclipped);
   if (!InXRange(aclipped.x()))
      return(false);

   SolveXIntersection(a, ABC, maxx, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip16(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   // Solve against minz
   SolveZIntersection(a, ABC, minz, aclipped);
   if (!InXRange(aclipped.x()))
      return(false);

   SolveXZIntersectionNear(a, ABC, minx, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip17(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   SolveZIntersection(a, ABC, minz, aclipped);
   SolveZIntersection(a, ABC, maxz, bclipped);
   return(true);
}

//***************************************************************************
//***************************************************************************
bool ClipSegment::Clip18(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   // Solve against the minz line
   SolveZIntersection(a, ABC, minz, aclipped);
   if (!InXRange(aclipped.x()))
      return(false);

   SolveXZIntersectionNear(a, ABC, maxx, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip23(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   SolveXIntersection(a, ABC, minx, bclipped);
   if (!InZRange(bclipped.z()))
      return(false);

   SolveXZIntersectionFar(a, ABC, maxx, minz, aclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip24(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   SolveXZIntersectionFar(a, ABC, maxx, minz, aclipped);
   bclipped = b;
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip26(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   SOLVERETURN ret = SolveXZIntersectionFar(a, ABC, maxx, minz, aclipped);
   if (!TestRange(ret, aclipped))
      return(false);

   SolveXZIntersectionNear(a, ABC, minx, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip27(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   SolveZIntersection(a, ABC, maxz, bclipped);
   if (!InXRange(bclipped.x()))
      return(false);

   SolveXZIntersectionFar(a, ABC, maxx, minz, aclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip34(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   SolveXIntersection(a, ABC, minx, aclipped);
   bclipped = b;
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip35(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   SolveXIntersection(a, ABC, minx, aclipped);
   SolveXIntersection(a, ABC, maxx, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip37(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   SolveXIntersection(a, ABC, minx, aclipped);
   if (!InZRange(aclipped.z()))
      return(false);

   SolveZIntersection(a, ABC, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip38(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   SolveXIntersection(a, ABC, minx, aclipped);
   if (!InZRange(aclipped.z()))
      return(false);

   SolveXZIntersectionNear(a, ABC, maxx, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip44(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   aclipped = a;
   bclipped = b;
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip45(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   aclipped = a;
   SolveXIntersection(a, ABC, maxx, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip46(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   aclipped = a;
   SolveXZIntersectionNear(a, ABC, minx, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip47(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   aclipped = a;
   SolveZIntersection(a, ABC, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip48(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);
   aclipped = a;
   SolveXZIntersectionNear(a, ABC, maxx, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip56(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   SolveXIntersection(a, ABC, maxx, aclipped);
   if (!InZRange(aclipped.z()))
      return(false);

   SolveXZIntersectionNear(a, ABC, minx, maxz, bclipped);
   return(true);
}

//***************************************************************************
// Clip segment a-b into the central region
//***************************************************************************
bool ClipSegment::Clip57(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   float ABC[3];

   FigureABC(a, b, ABC);

   SolveXIntersection(a, ABC, maxx, aclipped);
   if (!InZRange(aclipped.z()))
      return(false);

   SolveZIntersection(a, ABC, maxz, bclipped);
   return(true);
}

