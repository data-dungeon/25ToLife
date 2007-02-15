//

#pragma once

#include "HCGLBaseView.h"
#include "../Utility/Matrix44.h"

class Plane
{
public:
   Vector m_point;
   Vector m_normal;
   float  m_equation[4];

   Plane() {}
   Plane(const Vector &point, const Vector &normal);
   Plane(const Vector &p1, const Vector &p2, const Vector &p3);

   BOOL  IsFrontFacingTo(const Vector& direction) const;
   float SignedDistanceTo(const Vector& point) const;
};

inline Plane::Plane(const Vector &point, const Vector &normal)
{
   m_point = point;
   m_normal = normal;
   m_equation[0] = normal.x;
   m_equation[1] = normal.y;
   m_equation[2] = normal.z;
   m_equation[3] = -(normal.x*point.x + normal.y*point.y + normal.z*point.z);
}

inline Plane::Plane( const Vector& p1,const Vector& p2, const Vector& p3)
{
   m_normal = (p2-p1).Cross(p3-p1);
   m_normal.Normalize();
   m_point = p1;
   m_equation[0] = m_normal.x;
   m_equation[1] = m_normal.y;
   m_equation[2] = m_normal.z;
   m_equation[3] = -(m_normal.x*m_point.x + m_normal.y*m_point.y + m_normal.z*m_point.z);
}

inline BOOL Plane::IsFrontFacingTo(const Vector& direction) const
{
   double dot = m_normal.Dot(direction);
   return (dot <= 0);
}

inline float Plane::SignedDistanceTo(const Vector& point) const
{
   return (point.Dot(m_normal)) + m_equation[3];
}

class MouseOnPlane
{
protected:
   Plane         m_plane;
   HCGLBaseView *m_pView;
public:
   void Init( HCGLBaseView *pView, const Vector &worldstart, const Vector &worlddir );
   Vector GetWorldPos( CPoint cpoint, BOOL preventprofileplanes=TRUE );
};
   
class MouseOnLine : public MouseOnPlane
{
protected:
   Ray    m_targetray;

public:
   void   Init( HCGLBaseView *pView, const Vector &worldstart, const Vector &worlddir );
   Vector GetWorldPos( CPoint cpoint );
   float  GetWorldLength(CPoint point) { return (GetWorldPos(point)-m_targetray.m_point).Length(); }
   Vector GetWorldStart() { return m_targetray.m_point; }
};
   
class MouseOnCircle : public MouseOnPlane
{
protected:
   BOOL    m_uselinetest,
           m_maintainlength;
   float   m_rolllengthsquared,
           m_rolllength;
   Vector  m_screenrolldir,
           m_worldstart,
           m_screenstart;
   Vector2 m_center,
           m_linedir;

public:
   void Init( HCGLBaseView *pView, const Vector &worldstart, const Vector &worlddir, float radius, BOOL maintainlength=TRUE );
   Vector GetWorldPos( CPoint cpoint );
   const Vector &GetWorldCenter() const
   {
      return m_worldstart;
   }
};
   
class MouseOnScreen 
{
protected:
   Vector        m_screenstart3;
   HCGLBaseView *m_pView;

public:
   void Init( HCGLBaseView *pView, const Vector &worldstart );
   Vector GetWorldPos( CPoint cpoint );
};
   
class MouseOnSphere
{
protected:
   HCGLBaseView *m_pView;
   Vector        m_worldcenter;
   float         m_radiussquared;
   BOOL          m_istransformed;
   Matrix44      m_worldmatrix,
                 m_worldinverse;
   Vector        m_bonecenter;

public:
   void Init( HCGLBaseView *pView, const Vector &worldcenter, float pradius );
   void Init( HCGLBaseView *pView, const Vector &worldcenter, float pradius, const Matrix44 &matrix, const Matrix44 &inverse );
   void SetWorldCenter( const Vector &worldcenter );
   Vector GetWorldPos( CPoint cpoint );
};

#include "MouseOn.hpp"