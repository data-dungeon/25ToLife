//============================================================================
//=
//= Plane.h - a simple class for working with geometric planes, represented
//=           by the equation Ax+By+Cz+D=0.  [ABC] is the normal facing
//=           away from the plane, and [D] is the offset from the origin.
//=
//============================================================================

#ifndef _PLANE_H
#define _PLANE_H

#include "Math/Vector.h"

//============================================================================

#define PLANE_EPSILON 0.00001f

//============================================================================

class CPlane
{
public:
	/// set using literal plane equation values
	inline void Set( Vector3CRef normal, float d )
	{
		d_normal = normal;
		d_d      = d;
	}

	/// set from three points on the plane, using right hand rule to make normal
	inline void Set( Vector3CRef a, Vector3CRef b, Vector3CRef c )
	{
		d_normal = ( b - a ).Cross( c - a ).Normalized();
		d_d      = -d_normal.Dot( a );
	}

	/// set from normal and point on the plane
	inline void Set( Vector3CRef normal, Vector3CRef point )
	{
		d_normal = normal;
		d_d      = -d_normal.Dot( point );
	}

	/// Constructors
	inline CPlane()                                              {}
	inline CPlane( Vector3CRef normal, float d )                 { Set( normal, d );     }
	inline CPlane( Vector3CRef a, Vector3CRef b, Vector3CRef c ) { Set( a, b, c );       }
	inline CPlane( Vector3CRef normal, Vector3CRef point )       { Set( normal, point ); }

	/// distance of a point from the plane
	inline float	 Dist( Vector3CRef v ) const                  { return d_normal.Dot( v ) + d_d; }

	/// point is on the positive side of the plane
	inline bool		 IsFacing( Vector3CRef v ) const              { return Dist( v ) > 0.0f; }

	/// sphere is on the positive side of the plane
	inline bool		 IsFacing( const ts_BoundingSphere& s ) const { return Dist( s.Center ) > s.Radius; }

	/// reflect a point on the opposite side of the plane
	inline Vector3	 Reflect( Vector3CRef v ) const               { return v - d_normal * ( Dist( v ) * 2.0f ); }

	/// invert the plane
	inline void		 Invert()                                     { d_normal = -d_normal; d_d = -d_d; }

	/// plane normal (A,B,C from plane equation Ax+By+Cz+D=0)
	inline Vector3& Normal()                                     { return d_normal; }
	inline Vector3CRef Normal() const                            { return d_normal; }

	/// D from plane equation Ax+By+Cz+D=0
	inline float&   D()                                          { return d_d; }
	inline float    D() const                                    { return d_d; }

	/// which side of the plane is a point on?  (returns +1, 0, -1)
	inline int      SideOf( Vector3CRef v ) const
	{
		float d = Dist( v );
		if      ( d >  PLANE_EPSILON ) { return  1; }
		else if ( d < -PLANE_EPSILON ) { return -1; }
		else                           { return  0; }
	}

	/// Intersection of a line segment on the plane
	bool ChordIntersection( Vector3CRef a, Vector3CRef b, Vector3* out ) const
	{
		int sideA = SideOf( a );
		int sideB = SideOf( b );

		if ( sideA == sideB )
			return false;	// both points on same side, no intersection
		else
		{
			if ( out )
			{
				Vector3 v = b - a;
				float   t = -Dist( a ) / d_normal.Dot( v );
				*out      = a + ( v * t );
			}
			return true;
		}
	}

private:
	Vector3 d_normal;
	float   d_d;
};

//============================================================================

class CPlaneList
{
public:
	int		d_numPlanes;
	CPlane	d_planes[ 1 ];		// at least one, more if malloc'd.
};

//============================================================================

#endif //_PLANE_H
