//

#define SINOFSMALLANGLE    0.0872      // 5 DEGREES

inline Ray::Ray( const Vector &point, const Vector &dir )
{
   m_point = point;
   m_dir = dir;
}

inline Ray &Ray::operator *= ( const Matrix44 &matrix )
{
   m_point *= matrix;
   TransformVectorNoTranslate( matrix, m_dir );
   m_dir.Normalize();
   return *this;
}

inline Ray Ray::operator * ( const Matrix44 &matrix ) const
{
   return Ray(*this) *= matrix;
}

inline Vector Ray::Intersect( const Plane &plane, BOOL preventprofileplanes/*=TRUE*/ )
{
   float denom = Dot(plane.m_normal, m_dir);
   if ( preventprofileplanes && fabs(denom) < .017 ) {
      Plane plane2( plane.m_point, m_dir );
      Vector planeintersect = m_point + Dot((plane2.m_point - m_point), plane2.m_normal) * m_dir;
      Vector delta = planeintersect;
      Ray targetray( plane.m_point, Cross( m_dir, plane.m_normal ));
      delta -= targetray.m_point;
      float along = Dot( delta, targetray.m_dir );
      Vector rvalue = targetray.m_dir;
      rvalue *= along;
      rvalue += targetray.m_point;
      return rvalue;
   }
   else
      return m_point + Dot((plane.m_point - m_point), plane.m_normal) / denom * m_dir;
}

inline void Ray::IntersectSphere( const Vector &center, float radiussq, Vector &hit0, Vector &hit1 )
{
   float a, b;

   Vector delta = center;
   delta -= m_point;
   a = Dot( delta, m_dir );
   b = a*a - Dot( delta, delta ) + radiussq;
   if (b <= 0)
      b = 0;
   else
      b = sqrt( b );

   hit0 = m_dir;
   hit0 *= a - b;
   hit0 += m_point;

   hit1 = m_dir;
   hit1 *= a + b;
   hit1 += m_point;
}

inline BOOL Ray::IsIntersectSphere( const Vector &center, float radiussq )
{
   float a, b;

   Vector delta = center;
   delta -= m_point;
   a = Dot( delta, m_dir );
   if ((b = a*a - Dot( delta, delta ) + radiussq) < 0)
      return FALSE;
   return TRUE;  
}

inline Ray HCGLBaseView::GetScreenMouseRay( const CPoint &point ) // should be GetViewMouseRay
{
   Ray mouseray;

#if 1
   Vector ptatcamera( (float)point.x, (float)point.y, 0.0f );
   ScreenToWorld( ptatcamera );
   ptatcamera *= GetWorldToViewMatrix();
   mouseray.m_point = ptatcamera;

   Vector ptnear( (float)point.x, (float)point.y, -50.0f );
   ScreenToWorld( ptnear );
   ptnear *= GetWorldToViewMatrix();

   Vector ptfar( (float)point.x, (float)point.y, -100.0f );
   ScreenToWorld( ptfar );
   ptfar *= GetWorldToViewMatrix();

   mouseray.m_dir = ptnear - ptfar;
#else
   mouseray.m_point.Set( 0 );
   Vector temp( 0,0,0 );
   WorldToScreen( temp );
   mouseray.m_dir.Set( (float)point.x, (float)point.y, temp.z );
   UnProject( mouseray.m_dir );
   mouseray.m_dir.Normalize();
#endif

   return mouseray;
}

inline Ray HCGLBaseView::GetWorldMouseRay( const CPoint &point )
{
   return GetScreenMouseRay( point ) * GetViewToWorldMatrix();
}

inline void MouseOnLine::Init( HCGLBaseView *pView, const Vector &worldstart, const Vector &worlddir )
{
   m_targetray.m_point = worldstart;
   m_targetray.m_dir = worlddir;
   m_targetray.m_dir.Normalize();

   Vector screen1 = m_targetray.m_point;
   Vector screen2 = m_targetray.m_point;
   screen2 += m_targetray.m_dir;
   pView->WorldToScreen( screen1 );
   pView->WorldToScreen( screen2 );
   screen2 -= screen1;
   screen2.Normalize();

   Vector otherraydir;
   if (fabs(screen2.x) > fabs(screen2.y))
      otherraydir = Vector( 0, 1, 0 );
   else
      otherraydir = Vector( 1, 0, 0 );
   TransformVectorNoTranslate( pView->GetViewToWorldMatrix(), otherraydir );

   Vector worldnormal = Cross( m_targetray.m_dir, otherraydir );
   MouseOnPlane::Init( pView, worldstart, worldnormal );
}

inline Vector MouseOnLine::GetWorldPos( CPoint point )
{
   Vector planeintersect = MouseOnPlane::GetWorldPos( point );
   Vector delta = planeintersect;
   delta -= m_targetray.m_point;
   float along = Dot( delta, m_targetray.m_dir );
   Vector rvalue = m_targetray.m_dir;
   rvalue *= along;
   rvalue += m_targetray.m_point;
   return rvalue;
}

inline void MouseOnPlane::Init( HCGLBaseView *pView, const Vector &worldstart, const Vector &worlddir )
{
   m_pView = pView;
   m_plane.m_point = worldstart;
   m_plane.m_normal = worlddir;
}

inline Vector MouseOnPlane::GetWorldPos( CPoint ppoint, BOOL preventprofileplanes/*=TRUE*/ )
{
   Ray ray = m_pView->GetWorldMouseRay( ppoint );
   return ray.Intersect( m_plane, preventprofileplanes );
}

inline void MouseOnCircle::Init( HCGLBaseView *pView, const Vector &worldstart,
 const Vector &worlddir, float radius, BOOL maintainlength/*=TRUE*/ )
{
   m_pView = pView;
   m_worldstart = worldstart;

   Vector screendir( worlddir );
   TransformVectorNoTranslate( pView->GetWorldToViewMatrix(), screendir );
   screendir.Normalize();

   //if ( pView->viewplane )
   if (screendir.x || screendir.y)
   {
      Vector frontofringdir;
      if (fabs(screendir.z) < SINOFSMALLANGLE)
         frontofringdir = Vector(0,0,1);
      else {
         Vector inscreenplane( screendir.x, screendir.y, 0 );
         Vector firstcross = Cross( screendir, inscreenplane );
         frontofringdir = Cross( screendir, firstcross );
         frontofringdir.Normalize();
      }
      Vector viewstart = worldstart;
      pView->WorldToView( viewstart );
      Vector frontofringpos = viewstart + frontofringdir * radius;
      Vector endofaxis = viewstart + screendir * radius;
      Vector screenstart = viewstart;
      pView->Project( screenstart );
      pView->Project( frontofringpos );
      pView->Project( endofaxis );
      screenstart.z = frontofringpos.z = endofaxis.z = 0.0f;
      Vector ringdir = screenstart - frontofringpos;  // because of perspective projection, front of vertical ring can still be up or down from center
      Vector axisdir = screenstart - endofaxis;
      axisdir.Normalize();
      float diff = fabs(Dot( ringdir, axisdir ));
      m_uselinetest = diff <= 10;  // if the front of the ring is closer than 10 pixels to the center, then use line
   }
   else
      m_uselinetest = fabs(screendir.z) < SINOFSMALLANGLE;
   
   if (m_uselinetest) {
      m_screenstart = m_worldstart;
      pView->WorldToScreen( m_screenstart );
      m_center.Set( m_screenstart.x, m_screenstart.y );
      m_linedir.Set( screendir.y, screendir.x );
      m_linedir.Normalize();
//      m_screenrolldir = radiuspoint - worldstart;
//      TransformVectorNoTranslate( *pView->GetWorldToViewMatrix(), m_screenrolldir );
//      float rolllen2D = m_screenrolldir.Length();// * rinfo->zoom;
//      m_rolllengthsquared = rolllen2D * rolllen2D;
      m_rolllengthsquared = radius * radius;
   }
   else
      MouseOnPlane::Init( pView, worldstart, worlddir );

   m_maintainlength = maintainlength;
   if ( m_maintainlength )
      m_rolllength = radius;
}

inline Vector MouseOnCircle::GetWorldPos( CPoint ppoint )
{
   Vector vec;
   
   if (m_uselinetest) {
      Vector2 mousedir( Vector2((float)ppoint.x, (float)ppoint.y) - m_center );
      float mousedirlen = Dot( mousedir, m_linedir );
      mousedir = m_linedir * mousedirlen;
      vec.Set( mousedir.x+m_center.x, mousedir.y+m_center.y, sqrt( MAX(m_rolllengthsquared - mousedirlen*mousedirlen,0)));
      if (m_screenrolldir.z < 0)
         vec.z = -vec.z;
//      vec.z /= rinfo->zoom;
      vec.z += m_screenstart.z;
      m_pView->ScreenToWorld( vec );
   }
   else
      vec = MouseOnPlane::GetWorldPos( ppoint );

   if ( m_maintainlength ) {
//      vec = worldstart + rolllength * (vec-worldstart).Normalized(); // THE SLOWER ONE-LINE VERSION - REQUIRES 4 TEMPORARY VECTORS
      vec -= m_worldstart;
      vec.Normalize();
      vec *= m_rolllength;
      vec += m_worldstart;
   }
   return vec;
}

inline void MouseOnScreen::Init( HCGLBaseView *pView, const Vector &worldstart )
{
   m_pView = pView;
   m_screenstart3 = worldstart;
   pView->WorldToScreen( m_screenstart3 );
}

inline Vector MouseOnScreen::GetWorldPos( CPoint point )
{
   Vector worldpos( (float)point.x, (float)point.y, m_screenstart3.z );
   m_pView->ScreenToWorld( worldpos );
   return worldpos;
}

inline void MouseOnSphere::Init( HCGLBaseView *pView, const Vector &worldcenter, float radius )
{
   m_pView = pView;
   m_radiussquared = radius * radius;
   m_worldcenter = worldcenter;
   m_istransformed = FALSE;
}

inline void MouseOnSphere::Init( HCGLBaseView *pView, const Vector &worldcenter, float radius, const Matrix44 &matrix, const Matrix44 &inverse )
{
   m_pView = pView;
   m_radiussquared = radius * radius;
   m_worldcenter = worldcenter;
   m_istransformed = TRUE;
   
   m_worldmatrix = matrix;
   m_worldinverse  = inverse;
   m_bonecenter = inverse * worldcenter;
}

inline void MouseOnSphere::SetWorldCenter( const Vector &worldcenter )
{
   m_worldcenter = worldcenter;
}

inline Vector MouseOnSphere::GetWorldPos( CPoint ppoint )
{
   Vector hit0, hit1, hit0view, hit1view;
   
   Ray ray = m_pView->GetWorldMouseRay( ppoint );
   if (m_istransformed) {
      ray *= m_worldinverse;
      ray.IntersectSphere( m_bonecenter, m_radiussquared, hit0, hit1 );
      hit0 *= m_worldmatrix;
      hit1 *= m_worldmatrix;
   }
   else
      ray.IntersectSphere( m_worldcenter, m_radiussquared, hit0, hit1 );
   hit0view = hit0;
   m_pView->WorldToView( hit0view );
   hit1view = hit1;
   m_pView->WorldToView( hit1view );
   if ( hit0view.z > hit1view.z )
      return hit0;
   else
      return hit1;   
}
