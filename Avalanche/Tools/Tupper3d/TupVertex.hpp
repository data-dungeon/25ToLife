////////////////////////////////////////////////////////////////////////////
//
// TupVertex
//
// classes to handle vertices
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupVertex.hpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/29/03    Time: 3:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * start of poly mesh and converters
 */

template<class TYPE>
TupVertex3<TYPE>::TupVertex3<TYPE>(void) : 
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
}

template<class TYPE>
TupVertex3<TYPE>::TupVertex3<TYPE>(TYPE nx,TYPE ny,TYPE nz) :
	x(nx),
	y(ny),
	z(nz)
{
}

template<class TYPE>
TupVertex3<TYPE>::TupVertex3<TYPE>( const TupVertex3<TYPE> &s ) : // copy constructor
	x(s.x),
	y(s.y),
	z(s.z)
{
}

template<class TYPE>
TupVertex3<TYPE> &TupVertex3<TYPE>::operator=( const TupVertex3<TYPE> &s )  // assignment operator
{
	x = s.x; y = s.y; z = s.z;
	return *this;
}

template<class TYPE>
TupVertex3<TYPE>& TupVertex3<TYPE>::operator+=(const TupVertex3<TYPE> &s ) // += operator
{
	x+=s.x; y+=s.y; z+=s.z;
	return *this;
}

template<class TYPE>
TupVertex3<TYPE>& TupVertex3<TYPE>::operator-=(const TupVertex3<TYPE> &s ) // -= operator
{
	x-=s.x; y-=s.y; z-=s.z;
	return *this;
}

template<class TYPE>
TupVertex3<TYPE>& TupVertex3<TYPE>::operator*=(const TupVertex3<TYPE> &s ) // *= operator
{
	x*=s.x; y*=s.y; z*=s.z;
	return *this;
}

template<class TYPE>
TupVertex3<TYPE>& TupVertex3<TYPE>::operator/=(const TupVertex3<TYPE> &s ) // /= operator
{
	x/=s.x; y/=s.y; z/=s.z;
	return *this;
}

template<class TYPE>
TupVertex3<TYPE>& TupVertex3<TYPE>::operator+=(const TYPE s ) // += operator
{
	x+=s; y+=s; z+=s;
	return *this;
}

template<class TYPE>
TupVertex3<TYPE>& TupVertex3<TYPE>::operator-=(const TYPE s ) // -= operator
{
	x-=s; y-=s; z-=s;
	return *this;
}

template<class TYPE>
TupVertex3<TYPE>& TupVertex3<TYPE>::operator*=(const TYPE s ) // *= operator
{
	x*=s; y*=s; z*=s;
	return *this;
}

template<class TYPE>
TupVertex3<TYPE>& TupVertex3<TYPE>::operator/=(const TYPE s ) // /= operator
{
	x/=s; y/=s; z/=s;
	return *this;
}

template<class TYPE>
void TupVertex3<TYPE>::Clear(void)
{
	x = y = z = (TYPE)0.0;
}

template<class TYPE>
void TupVertex3<TYPE>::Set(TYPE nx,TYPE ny,TYPE nz)
{
	x = nx; y = ny; z=nz;
}

template<class TYPE>
void TupVertex3<TYPE>::Abs(void)
{
	if (x<0.0) x=-x;
	if (y<0.0) y=-y;
	if (z<0.0) z=-z;
}

template<class TYPE>
TYPE TupVertex3<TYPE>::DotProduct(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
{
	return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
}

template<class TYPE>
TYPE TupVertex3<TYPE>::Length(void)
{
	return (TYPE) TupVertex3Sqrt(x*x+y*y+z*z);
}

template<class TYPE>
TYPE TupVertex3<TYPE>::LengthSq(void)
{
	return (x*x+y*y+z*z);
}

template<class TYPE>
TYPE TupVertex3<TYPE>::Distance(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
{
	TupVertex3<TYPE> temp = p1-p2;
	return temp.Length();
}

template<class TYPE>
TYPE TupVertex3<TYPE>::DistanceSq(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
{
	TupVertex3<TYPE> temp = p1-p2;
	return temp.LengthSq();
}

template<class TYPE>
void TupVertex3<TYPE>::Normalize(void)
{
	TYPE length = Length();
	if (length>(TYPE)1.0e-9)
	{
		x /= length;
		y /= length;
		z /= length;
	}
	else Set((TYPE)0.0,(TYPE)1.0,(TYPE)0.0);
}

template<class TYPE>
void TupVertex3<TYPE>::Normalize(TYPE &length)
{
	length = Length();
	if (length>(TYPE)1.0e-9)
	{
		x /= length;
		y /= length;
		z /= length;
	}
	else Set((TYPE)0.0,(TYPE)1.0,(TYPE)0.0);
}

template<class TYPE>
bool TupVertex3<TYPE>::SphereLineIntersection(TupVertex3<TYPE> &point1, TupVertex3<TYPE> &point2, TupVertex3<TYPE> &sphereCenter, TYPE radius, TupVertex3<TYPE> &inpoint, TupVertex3<TYPE> &outpoint)
{
	TupVertex3<TYPE> lineDir = point2-point1;
	TupVertex3<TYPE> sphereLine = point1-sphereCenter;
	TYPE a = DotProduct(lineDir,lineDir);
	TYPE b = 2.0f * (DotProduct(lineDir,sphereLine));
	TYPE c = DotProduct(sphereCenter,sphereCenter) + 
		DotProduct(point1,point1) - 
		2.0f * (DotProduct(sphereCenter,point1)) - 
		(radius * radius);

	TYPE i = b*b-4.0f*a*c;

	if (i<0.0f) return false; // if <0 then does not intersect
	TYPE u1 = (-b + (TYPE)sqrt(i))/(2.0f*a);
	TYPE u2 = (-b - (TYPE)sqrt(i))/(2.0f*a);

	if (u1<u2)
	{
		inpoint = point1 + (u1 * lineDir);
		outpoint = point1 + (u2 * lineDir);
	}
	else
	{
		inpoint = point1 + (u2 * lineDir);
		outpoint = point1 + (u1 * lineDir);
	}
	return true;
}

template<class TYPE>
TupVertex3<TYPE> TupVertex3<TYPE>::CrossProduct(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2)
{
	TupVertex3<TYPE> result;
	result.x = p1.y*p2.z - p1.z*p2.y;
	result.y = p1.z*p2.x - p1.x*p2.z;
	result.z = p1.x*p2.y - p1.y*p2.x;
	return result;
}

template<class TYPE>
TupVertex3<TYPE> TupVertex3<TYPE>::ComputeFaceNormal(const TupVertex3<TYPE> &p1,const TupVertex3<TYPE> &p2,const TupVertex3<TYPE> &p3)
{
	TupVertex3<TYPE> result,right,left;
	TYPE mag;

	// Find vectors to the right and left of the first vertex.

	right = p3-p2;

	left  = p1-p2;

	// Take the cross product to get a vector normal to the face.

	result = CrossProduct(right,left);

	// If the square of the magnitude of the normal vector is zero,
	// at least two of the vertices are coincident and this is an
	// illegal face.

	mag = result.Length();

	if (mag<1.0e-9)
	{
		//		assert(FALSE);
		result.Set(0.0,1.0,0.0);
		mag = 1.0f;
	}

	// Normalize the normal vector.

	result /= mag;
	return result;
}

template<class TYPE>
TYPE TupVertex3<TYPE>::DistanceToLine(const TupVertex3<TYPE> &point1,const TupVertex3<TYPE> &point2)
{
	TupVertex3<TYPE> intersection(PointToLineIntersection(point1,point2));
	return (intersection-(*this)).Length();
}

template<class TYPE>
TupVertex3<TYPE> TupVertex3<TYPE>::PointToLineIntersection(const TupVertex3<TYPE> &point1,const TupVertex3<TYPE> &point2)
{
	if (point1==point2) // if the distance between points is 0 then give one of the points
		return point1;
	TYPE u = ((x-point1.x)*(point2.x-point1.x)+(y-point1.y)*(point2.y-point1.y)+(z-point1.z)*(point2.z-point1.z));
	TYPE lengthSq = (point2-point1).LengthSq();
	u /= lengthSq;
	return point1+(point2-point1)*u;
}

template<class TYPE>
TYPE TupVertex3<TYPE>::PointToLineIntersectionPercent(const TupVertex3<TYPE> &point1,const TupVertex3<TYPE> &point2)
{
	if (point1==point2) // if the distance between points is 0 then give one of the points
		return 0.0;
	TYPE u = ((x-point1.x)*(point2.x-point1.x)+(y-point1.y)*(point2.y-point1.y)+(z-point1.z)*(point2.z-point1.z));
	TYPE lengthSq = (point2-point1).LengthSq();
	u /= lengthSq;
	return u;
}
