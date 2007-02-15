//

#pragma once

#include "Vector.h"

class BoundingBox
{
public:
	double m_minx, m_miny, m_minz;
	double m_maxx, m_maxy, m_maxz;

   BoundingBox()
	{
      Clear();
	}

	BoundingBox(const BoundingBox &other)
	{
		m_minx = other.m_minx;
		m_miny = other.m_miny;
		m_minz = other.m_minz;
		m_maxx = other.m_maxx;
		m_maxy = other.m_maxy;
		m_maxz = other.m_maxz;
	}
	~BoundingBox() {};

   void Clear()
   {
		m_minx = MAX_FLOAT;
		m_miny = MAX_FLOAT;
		m_minz = MAX_FLOAT;
		m_maxx = -MAX_FLOAT;
		m_maxy = -MAX_FLOAT;
		m_maxz = -MAX_FLOAT;
   }
   BOOL IsEmpty() const
   {
      return m_maxx<m_minx || m_maxy<m_miny || m_maxz<m_minz;
   }

   void Push( const Vector &point )
   {
      if ( point.x < m_minx )
         m_minx = point.x;
      if ( point.y < m_miny )
         m_miny = point.y;
      if ( point.z < m_minz )
         m_minz = point.z;

      if ( point.x > m_maxx )
         m_maxx = point.x;
      if ( point.y > m_maxy )
         m_maxy = point.y;
      if ( point.z > m_maxz )
         m_maxz = point.z;
   }

   void Offset( const Vector &vOffset )
   {
      m_minx += vOffset.x;
      m_miny += vOffset.y;
      m_minz += vOffset.z;

      m_maxx += vOffset.x;
      m_maxy += vOffset.y;
      m_maxz += vOffset.z;
   }

	Vector CalculateCenterPoint() const
	{
      if (IsEmpty())
         return Vector(0.0);
      else
   		return Vector( (float)((m_minx + m_maxx)*0.5), (float)((m_miny + m_maxy)*0.5), (float)((m_minz + m_maxz)*0.5) );
	}

	double GetWidth() const
	{
		return m_maxx - m_minx;
	}

	double GetHeight() const
	{
		return m_maxy - m_miny;
	}

	double GetDepth() const
	{
		return m_maxz - m_minz;
	}

	double GetLargestDimension() const
	{
      if (IsEmpty())
         return 0.0;

		double maxdim = GetWidth();
		if (GetHeight() > maxdim)
			maxdim = GetHeight();
		if (GetDepth() > maxdim)
			maxdim = GetDepth();
		return maxdim;
	}
};

class BoundingSphere
{
public:
   Vector m_vCenter;
   float  m_fRadius;

   BoundingSphere()
   {
      Clear();
   }

   void Clear()
   {
      m_vCenter.Set( 0.0f );
      m_fRadius = 0.0f;
   }

   BOOL IsEmpty() { return m_fRadius == 0.0f; }
};
