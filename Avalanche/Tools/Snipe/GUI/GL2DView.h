#pragma once

#include "GLHeaderView.h"
#include "Camera.h"
#include "Ray.h"

class CGL2DView : public CGLHeaderView
{
   DECLARE_BASEVIEW(CGL2DView, CGLHeaderView)
public:
   static BoolProperty::Info    m_bDisplayGridInfo;
   static Vector2Property::Info    m_nSquareSizeInfo;
   static Vector2Property::Info    m_nNumSquaresInfo;

   BoolProperty    m_bDisplayGrid;
   Vector2Property    m_nSquareSize;
   Vector2Property    m_nNumSquares;

   Camera   m_defaultcamera;
   Camera  *m_pCurCamera;
   Matrix44 m_viewtoobjectmatrix;
   Matrix44 m_objecttoviewmatrix;
   GLdouble m_GLViewTransformMatrix[16];
   GLuint   m_glGridCallList;

            CGL2DView();
   virtual ~CGL2DView();

// Operations
   Camera *GetCamera() { return m_pCurCamera; }
   void SetCamera(Camera *pCamera);

   Ray GetScreenMouseRay( const CPoint &point );
   Ray GetWorldMouseRay( const CPoint &point );

   BOOL IsSphereInFrustum( const BoundingSphere &boundingsphere );

   void Draw3DText( const String &text, const Vector &pos, const Vector &dir, float fHeightAtCameraPlane=20.0f, const Vector &up=Vector(0,1,0), Justify justify=JUST_CENTER );

// Message map
   afx_msg void OnDisplayGrid();

// Overridables
   virt_base virtual void DrawGrid();
   virt_base virtual void GetNearAndFarClip( float &fNearClip, float &fFarClip );

// CGLBaseView overrides
   virtual float GetScreenPixelSize( float fDistance ) const;
   virtual void ViewToWorld( Vector &p, BOOL bRotateOnly=FALSE ) const;
   virtual void WorldToView( Vector &p ) const;
   virtual void ScreenToWorld( Vector &p, BOOL bRotateOnly=FALSE ) const;
   virtual void WorldToScreen( Vector &p ) const;
   virtual BOOL Project( Vector &p ) const;
   virtual BOOL UnProject( Vector &p ) const;
   virtual const Matrix44 &GetWorldToViewMatrix() const;
   virtual const Matrix44 &GetViewToWorldMatrix() const;
   virtual SnipeNavigatorInstance *SetNavigatorInstance( SnipeNavigator *pNavigator );

// CGLControl overrides
   virtual void PostInit();
   virtual void SetProjectionMatrix();

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
