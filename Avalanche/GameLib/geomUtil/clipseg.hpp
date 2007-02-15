//***************************************************************************
// Implementation file for fast segment clipping
//***************************************************************************

//***************************************************************************
// Clip the segment a-b to aclipped-bclipped
// Return true if some portion of the segment lies the clipping region,
// false if not.  On a false return, aclipped and bclipped are not set.
//***************************************************************************
inline bool ClipSegment::Clip(
Vector3CRef a,
Vector3CRef b,
Vector3 &aclipped,
Vector3 &bclipped)
{
   int ai, bi;
   ClipFunc clipFn;

   // Do the segments cross the field in XZ?
   ai = 0;
   if (a.x() > maxx)
      ai += 2;
   else if (a.x() > minx)
      ai += 1;
   if (a.z() > maxz)
      ai += 6;
   else if (a.z() > minz)
      ai += 3;

   bi = 0;
   if (b.x() > maxx)
      bi += 2;
   else if (b.x() > minx)
      bi += 1;
   if (b.z() > maxz)
      bi += 6;
   else if (b.z() > minz)
      bi += 3;

   // Get the function
   if ((clipFn = sectorClip[ai][bi]) == 0)
      return(false);

   // Clip the segment
   if (ai < bi)
      return((this->*clipFn)(a, b, aclipped, bclipped));

   return((this->*clipFn)(b, a, bclipped, aclipped));
}

//***************************************************************************
// Figure out A, B, C
//***************************************************************************
inline void ClipSegment::FigureABC(
Vector3CRef a,
Vector3CRef b,
float *ABC)
{
	Vector3 res = b - a;
   ABC[0] = res.x();
	ABC[1] = res.y();
   ABC[2] = res.z();
}

//***************************************************************************
// Solve the intersection with a single line
//***************************************************************************
inline ClipSegment::SOLVERETURN ClipSegment::SolveXIntersection(
Vector3CRef a,
float *ABC,
float xval,
Vector3 &clipped)
{
   float t = (xval - a.x()) / ABC[0];

   clipped.Set(
		xval,
		a.y() + ABC[1] * t,
		a.z() + ABC[2] * t);

	return(XSET);
}

//***************************************************************************
// Solve the intersection with a single line
//***************************************************************************
inline ClipSegment::SOLVERETURN ClipSegment::SolveZIntersection(
Vector3CRef a,
float *ABC,
float zval,
Vector3 &clipped)
{
   float t = (zval - a.z()) / ABC[2];

	clipped.Set((float)(a.x() + ABC[0] * t),
					(float)(a.y() + ABC[1] * t),
					zval);

	return(ZSET);
}

//***************************************************************************
// Solve the intersection with a two lines and take the one that is
// farther away
//***************************************************************************
inline ClipSegment::SOLVERETURN ClipSegment::SolveXZIntersectionFar(
Vector3CRef a,
float *ABC,
float xval,
float zval,
Vector3 &clipped)
{
   float tx = (xval - a.x()) / ABC[0];
   float tz = (zval - a.z()) / ABC[2];

   if (tx > tz)
   {
		clipped.Set(xval,
						(float)(a.y() + ABC[1] * tx),
						(float)(a.z() + ABC[2] * tx));
		return(XSET);
   }
   else
   {
      clipped.Set((float)(a.x() + ABC[0] * tz),
						(float)(a.y() + ABC[1] * tz),
						zval);
		return(ZSET);
   }
}

//***************************************************************************
// Solve the intersection with a two lines and take the one that is
// closer
//***************************************************************************
inline ClipSegment::SOLVERETURN ClipSegment::SolveXZIntersectionNear(
Vector3CRef a,
float *ABC,
float xval,
float zval,
Vector3 &clipped)
{
   float tx = (xval - a.x()) / ABC[0];
   float tz = (zval - a.z()) / ABC[2];

   if (tx < tz)
   {
      clipped.Set(xval,
						(float)(a.y() + ABC[1] * tx),
						(float)(a.z() + ABC[2] * tx));
		return(XSET);
   }
   else
   {
      clipped.Set((float)(a.x() + ABC[0] * tz),
						(float)(a.y() + ABC[1] * tz),
						zval);
		return(ZSET);
   }
}

