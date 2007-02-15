//
#pragma once

#include "GLImageList.h"
#include "KeyModifiers.h"
#include "MouseStates.h"
#include "MouseOn.h"

class CGLViewControl;
class CGLBaseView;
class PickHit;

typedef void (* PFN_VIEWCONTROLCHANGEDCALLBACK) ( CGLViewControl *pViewControl, void *pData );
void ViewControlChangedCallback( CGLViewControl *pViewControl, void *pData );

class CGLViewControl
{
public:
   CGLBaseView *m_pBaseView;
   PFN_VIEWCONTROLCHANGEDCALLBACK m_pfnCallback;
   void        *m_pCallbackData;

            CGLViewControl( CGLBaseView *pBaseView );
   virtual ~CGLViewControl();

   void SetCallback( CGLViewControl *pViewControl ) { SetCallback( ViewControlChangedCallback, pViewControl ); }
   void SetCallback( PFN_VIEWCONTROLCHANGEDCALLBACK pfnCallback, void *pData ) { m_pfnCallback = pfnCallback; m_pCallbackData = pData; }

   // Overridables
   virt_base virtual BOOL IsFor( void *pData ) { return FALSE; }
   virt_base virtual void Draw() {}
   virt_base virtual BOOL HitTest( const CPoint &ptMouse ) { return FALSE; }
   virt_base virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit ) { return FALSE; }
   virt_base virtual BOOL OnLButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnRButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnMButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnMButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnMButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnMouseWheel( KeyModifiers keymodifier, MouseStates mousestate, short zDelta, const CPoint &ptMouse, const PickHit &pickHit  ) { return FALSE; }
   virt_base virtual BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit ) { return FALSE; }
   virt_base virtual UINT GetToolTipText( const CPoint &ptMouse, String &strText ) { return -1; }
   virt_base virtual KeyModifiers GetKeyModifiers( MouseStates mousestate, const PickHit &pickHit ) { return KEYMOD_NONE; }
   virt_base virtual void OnViewControlChanged( CGLViewControl *pViewControl ) {}
};

class CGLKnobViewControl : public CGLViewControl
{
public:
   static GLImageList m_glImageListKnob;

private:
   Matrix44 m_matrix;
   Vector   m_vPos;
   UINT     m_bPressed : 1;

public:
   CGLKnobViewControl( CGLBaseView *pBaseView, const Vector &vPos, const Matrix44 &matrix=Identity44() );

   void SetMatrix( const Matrix44 &matrix ) { m_matrix = matrix; }
   const Matrix44 &GetMatrix() { return m_matrix; }
   void SetPos( const Vector &vPos );
   const Vector &GetPos() const { return m_vPos; }
   void SetPressed( BOOL bPressed ) { m_bPressed = bPressed; }

// CGLViewControl overrides
   virtual void Draw();
   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  );
   virtual BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  );
};

class CGL1DTranslateViewControl : public CGLKnobViewControl
{
public:
   MouseOnLine m_MouseOnLine;
   Vector      m_vDir;

   CGL1DTranslateViewControl( CGLBaseView *pBaseView, const Vector &vPos, const Vector &vDir, const Matrix44 &matrix=Identity44() );

   void SetPos( const Vector &vPos ) { CGLKnobViewControl::SetPos( vPos ); m_MouseOnLine.Init( (HCGLBaseView *)m_pBaseView, GetPos(), m_vDir ); }
   void SetDirection( const Vector &vDir ) { m_vDir = vDir; m_MouseOnLine.Init( (HCGLBaseView *)m_pBaseView, GetPos(), m_vDir ); }

// CGLViewControl overrides
   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
};

class CGL2DTranslateViewControl : public CGL1DTranslateViewControl
{
public:
   MouseOnPlane m_MouseOnPlane;
   Vector       m_vPlane;

   CGL2DTranslateViewControl( CGLBaseView *pBaseView, const Vector &vPos, const Vector &vPlane, const Matrix44 &matrix=Identity44() );

// CGLViewControl overrides
   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
};

class CGL3DTranslateViewControl : public CGL2DTranslateViewControl
{
public:
   MouseOnScreen m_MouseOnScreen;

   CGL3DTranslateViewControl( CGLBaseView *pBaseView, const Vector &vPos, const Matrix44 &matrix=Identity44() );

// CGLViewControl overrides
   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
};

class CGLSliderViewControl : public CGL1DTranslateViewControl
{
public:
   Vector m_vOrigin;
   Vector m_vDirection;
   float  m_fMin;
   float  m_fMax;
   float  m_fPos;
   float  m_fScale;
   UINT   m_bDrawTrack : 1,
          m_bDrawFacingScreen : 1;

   CGLSliderViewControl( CGLBaseView *pBaseView, const Vector &vOrigin, const Vector &vDirection, float nMin=0.0f, float nMax=MAX_FLOAT, float nPos=0.0f, BOOL bDrawTrack=TRUE, BOOL bDrawFacingScreen=TRUE, const Matrix44 &matrix=Identity44() );

   void SetOrigin(const Vector &vOrigin);
   void SetPos( const CPoint &ptMouse );
   void PositionSlider();
   void InitMouseOnLine();
   void SetPos();

// CGLViewControl overrides
   virtual void Draw();
   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit  );
   virtual BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );

};

class CGLRotateViewControl : public CGLKnobViewControl
{
public:
   MouseOnCircle m_MouseOnCircle;

   CGLRotateViewControl( CGLBaseView *pBaseView, const Vector &vOrigin, const Vector &vDirection, float fRadius=1.0f );

   // CGLViewControl overrides
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
   virtual BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, const CPoint &ptMouse, const PickHit &pickHit );
};

class CGL2DScaleViewControl : public CGLViewControl
{
public:
   CGL1DTranslateViewControl m_SliderN;
   CGL2DTranslateViewControl m_SliderNE;
   CGL1DTranslateViewControl m_SliderE;
   CGL2DTranslateViewControl m_SliderSE;
   CGL1DTranslateViewControl m_SliderS;
   CGL2DTranslateViewControl m_SliderSW;
   CGL1DTranslateViewControl m_SliderW;
   CGL2DTranslateViewControl m_SliderNW;

   Vector2 m_v2Size;

   CGL2DScaleViewControl( CGLBaseView *pBaseView, const Vector &vOrigin, const Vector2 &v2Size, const Matrix44 &matrix );

   void PositionSliders();
   void SetMatrix( const Matrix44 &matrix );

// CGLViewControl overrides
   virtual void Draw();
   virtual void OnViewControlChanged( CGLViewControl *pViewControl );
};

////////////////////////////////////////////////

#include "SnipeObject.h"

class TransformProperty;

class CGLTransformPropertyViewControl : public CGLViewControl, public SnipeObject
{
public:
   TransformProperty *m_pTransformProperty;

   CGL3DTranslateViewControl m_Translate;
   CGL1DTranslateViewControl m_SliderN;
   CGL2DTranslateViewControl m_SliderNE;
   CGL1DTranslateViewControl m_SliderE;
   CGL2DTranslateViewControl m_SliderSE;
   CGL1DTranslateViewControl m_SliderS;
   CGL2DTranslateViewControl m_SliderSW;
   CGL1DTranslateViewControl m_SliderW;
   CGL2DTranslateViewControl m_SliderNW;
   CGLRotateViewControl      m_RollHandle;

            CGLTransformPropertyViewControl( CGLBaseView *pBaseView, TransformProperty *pTransformProperty );
   virtual ~CGLTransformPropertyViewControl();

   void PositionSliders();

// CGLViewControl overrides
   virtual BOOL IsFor( void *pData ) { return pData == m_pTransformProperty; }
   //virtual void Draw();
   virtual void OnViewControlChanged( CGLViewControl *pViewControl );

// SnipeObject overrides
   virtual const char *GetClassName() const { return "CGLTransformPropertyViewControl"; }
   virtual String GetMatchName() const { return "CGLTransformPropertyViewControl"; }
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

////////////////////////////////////////////////

class FloatProperty;

class CGL3DFloatPropertyViewControl : public CGLViewControl, public SnipeObject
{
protected:
   CGLSliderViewControl m_Slider;
   FloatProperty *m_pFloatProperty;

public:
   CGL3DFloatPropertyViewControl( CGLBaseView *pBaseView, FloatProperty *pFloatProperty );
   virtual ~CGL3DFloatPropertyViewControl();

   void SetOrigin(const Vector &vOrigin) { m_Slider.SetOrigin(vOrigin); }

// CGLViewControl overrides
   virtual BOOL IsFor( void *pData ) { return pData == m_pFloatProperty; }
   virtual void OnViewControlChanged( CGLViewControl *pViewControl );
   virtual void Draw();

// SnipeObject overrides
   virtual const char *GetClassName() const { return "CGL3DFloatPropertyViewControl"; }
   virtual String GetMatchName() const { return "CGL3DFloatPropertyViewControl"; }
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
