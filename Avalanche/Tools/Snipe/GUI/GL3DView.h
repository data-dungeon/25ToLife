#pragma once

#include "GL2DView.h"
#include "GLComboControl.h"
#include "Light.h"
#include "Camera.h"

class ViewAngle
{
public:
   Vector m_vTranslate;
   Vector m_vRotate;
   float  m_fFOV;

   ViewAngle()
   {
      m_vTranslate.Set(0.0f);
      m_vRotate.Set(0.0f);
      m_fFOV = DEFAULTFOV;
   }

   ViewAngle( const Vector &vRotate )
   {
      m_vTranslate.Set(0.0f);
      m_vRotate = vRotate;
      m_fFOV = DEFAULTFOV;
   }

   void Set( const Vector &vRotate )
   {
      m_vRotate = vRotate;
   }
};

class ViewAnglePreset : public ViewAngle
{
public:
   const char *m_name;
   BOOL m_bZoomFit;

   ViewAnglePreset()
   {
      m_name = NULL;
      m_bZoomFit = FALSE;
   }

   ViewAnglePreset( const char *name, const Vector &vRotate=Vector(0.0f) ) :
      ViewAngle( vRotate )
   {
      m_name = name;
      m_bZoomFit = FALSE;
   }

   void Set( const char *name, const Vector &vRotate=Vector(0.0f))
   {
      ViewAngle::Set( vRotate );
      m_name = name;
   }
};

class C3DDoc;

class CGL3DView : public CGL2DView
{
   DECLARE_BASEVIEW(CGL3DView, CGL2DView)
public:
   static BoolProperty::Info m_bDisplayAxisInfo;
   static TypeProperty::Info m_nViewAngleInfo;

   BoolProperty    m_bDisplayAxis;
   TypeProperty    m_nViewAngle;

   // Header controls
   CGLComboControl  m_glViewAngleComboControl;

   CArray<ViewAnglePreset> m_viewanglepreset;
   ViewAngleIndex  m_nPrevViewAngle;
   Light           m_defaultlight;
   Light          *m_pCurLight;

            CGL3DView();
   virtual ~CGL3DView();

// Operations
   C3DDoc *GetDocument() const { return (C3DDoc *)CGLBaseView::GetDocument(); }
   Light *GetCurLight() { return m_pCurLight; }

   ViewAngleIndex GetViewAngle() const { return (ViewAngleIndex)m_nViewAngle.GetCurrentValue(); }
   void SetViewAngle( ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye=TRUE );
   void DefaultSetViewAngle( ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye=TRUE ); // Called by SetViewAngle
   void FillViewAngleCombo();

   TypeProperty *GetViewAngleProperty() { return &m_nViewAngle; }

   void DrawAxis();

   void GetProjectionFromWorld(const Vector &from, Vector2 &to);
   void GetViewFromProjection(const Vector &from, Vector &to);

   BOOL GetIntersectionOfProjectionPointAndPlane( Vector2 projection_point, Vector plane_position, Vector plane_normal, Vector &intersection );
   BOOL GetIntersectionOfLineAndPlane( Vector point0, Vector point1, Vector plane_position, Vector plane_normal, Vector &intersection );

   afx_msg void OnViewAnglePreset( UINT nID );
   afx_msg void OnTogglePerspective();

   void Draw3DCircleFacingScreen( Vector translate, float fRadius, UINT nSegments, bool bFilled = TRUE );

// CGL2DView overrides
   virtual void DrawGrid();
   virtual void GetNearAndFarClip( float &fNearClip, float &fFarClip );

// CGLHeaderView overrides
   virtual void UpdateHeader();

// CGLBaseView overrides
   virtual float GetScreenPixelSize( float fDistance ) const;

// CGLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual void SetProjectionMatrix();

// SnipeObject overrides
   virtual String GetTitle();
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
