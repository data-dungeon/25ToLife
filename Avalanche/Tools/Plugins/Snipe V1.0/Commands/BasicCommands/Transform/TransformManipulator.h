//

#pragma once

#include "../Select/SelectManipulator.h"
#include "Matrix44.h"
#include "RGBColor.h"
#include "HFloatProperty.h"
#include "HVectorProperty.h"
#include "HRotateProperty.h"
#include "MouseOn.h"

// Plugin

class TransformManipulatorPlugin : public SelectManipulatorPlugin
{
public:
   DECLARE_MANIPULATOR_PLUGINHELPER( TransformManipulatorPlugin, SelectManipulatorPlugin )

   HCommandPlugin *m_pTranslateCommandPlugin;
   HCommandPlugin *m_pRotateCommandPlugin;
   HCommandPlugin *m_pScaleCommandPlugin;

   GLuint          m_nCallListTranslsateZArrow;
   GLuint          m_nCallListRotateAll;
   GLuint          m_nCallListRotateZ;
   GLuint          m_nCallListRotateScreen;
   GLuint          m_nCallListScaleKnob;
   GLuint          m_nCallListPivotRotate;

   // HManipulatorPluginHelper Overrides
   virtual catch_msg ManipulatorInstance *CreateInstance();

   // HCommandUIPluginHelper Overides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual catch_msg void Init(HCommandPluginManager *pCommandManager);
   virtual catch_msg void Place(HCMainFrame *pMainFrame);
};

// Instance
class TransformManipulator : public SelectManipulator
{
protected:
   enum Mode { MODE_TRANSLATE, MODE_ROTATE, MODE_SCALE,
               MODE_TRANSLATE_ONLY, MODE_ROTATE_ONLY, MODE_SCALE_ONLY };
   enum      { SPACE_LOCAL, SPACE_WORLD, SPACE_NUMSPACES };
   enum      { AXIS_NONE = 0 };

   Mode     m_nMode;
   Matrix44 m_startmatrix;
   Matrix44 m_curmatrix;
   UINT     m_nSelectedElement;
   UINT     m_nMouseOverElement;
   UINT     m_nVisibleElements;
   UINT     m_bManipulating    : 1,
            m_bDrawPick        : 1,
            m_bShowManipulator : 1;

   HFloatProperty::HInfo *m_pScreenSizeInfo;
   HFloatProperty        *m_pScreenSize;
   HTypeProperty::HInfo::HNode *m_pSpaceNodes;
   HTypeProperty::HInfo  *m_pSpaceInfo;
   HTypeProperty         *m_pSpace;
   HBoolProperty::HInfo  *m_pPivotModeInfo;
   HBoolProperty         *m_pPivotMode;

   // PIVOT
   HVectorProperty::HInfo *m_pPivotPosInfo;
   HVectorProperty        *m_pPivotPos;
   HRotateProperty::HInfo *m_pPivotRotateInfo;
   HRotateProperty        *m_pPivotRotate;
   enum { AXIS_PIVOT_X          = 1<<0,
          AXIS_PIVOT_Y          = 1<<1,
          AXIS_PIVOT_Z          = 1<<2,
          AXIS_PIVOT_SCREEN     = 1<<3,
          AXIS_PIVOT_ROTATEX    = 1<<4,
          AXIS_PIVOT_ROTATEY    = 1<<5,
          AXIS_PIVOT_ROTATEZ    = 1<<6,
          AXIS_PIVOT_ROTATEFREE = 1<<7 };

   // TRANSLATE
   enum { AXIS_TRANSLATE_X      = 1<<8,
          AXIS_TRANSLATE_Y      = 1<<9,
          AXIS_TRANSLATE_Z      = 1<<10,
          AXIS_TRANSLATE_XY     = 1<<11,
          AXIS_TRANSLATE_YZ     = 1<<12,
          AXIS_TRANSLATE_XZ     = 1<<13,
          AXIS_TRANSLATE_SCREEN = 1<<14 };
   HVectorProperty::HInfo *m_pTranslateInfo;
   HVectorProperty        *m_pTranslate;
   MouseOnLine     m_mouseonline;
   MouseOnPlane    m_mouseonplane;
   MouseOnScreen   m_mouseonscreen;

   // ROTATE
   enum { AXIS_ROTATE_X      = 1<<15,
          AXIS_ROTATE_Y      = 1<<16,
          AXIS_ROTATE_Z      = 1<<17,
          AXIS_ROTATE_SCREEN = 1<<18,
          AXIS_ROTATE_FREE   = 1<<19 };
   HRotateProperty::HInfo *m_pRotateInfo;
   HRotateProperty        *m_pRotate;
   MouseOnCircle   m_mouseoncircle;
   MouseOnSphere   m_mouseonsphere;
   Vector          m_vStart;
   Vector          m_vCurrent;
   Vector          m_vAxis;
   float           m_fAngle;

   // SCALE
   enum { AXIS_SCALE_X   = 1<<20,
          AXIS_SCALE_Y   = 1<<21,
          AXIS_SCALE_Z   = 1<<22,
          AXIS_SCALE_XY  = 1<<23,
          AXIS_SCALE_YZ  = 1<<24,
          AXIS_SCALE_XZ  = 1<<25,
          AXIS_SCALE_ALL = 1<<26 };
   HVectorProperty::HInfo *m_pScaleInfo;
   HVectorProperty        *m_pScale;

public:
   DECLARE_MANIPULATORINSTANCE( TransformManipulator, SelectManipulator )

    TransformManipulator(TransformManipulatorPlugin *pTransformManipulatorPlugin);
   ~TransformManipulator();

   TransformManipulatorPlugin *GetTransformManipulatorPlugin() { return (TransformManipulatorPlugin *) GetManipulatorPluginHelper(); }

// Operations
   void ComputePivot();
   void ComputeMatrix();
   UINT HitTest( const CPoint &pt, HCGLBaseView *pView );
   BOOL DrawElement( GLuint nElement, const RGBAFloat &color, GLuint nCallList, BOOL bForceDraw=FALSE );
   void DrawPlane( GLuint nElement, const RGBAFloat &colorx, const RGBAFloat &colory );
   void DrawScaleAxis( float fScale, float fAlpha );
   void DrawPivotAxis( GLuint nElement, const RGBAFloat &color );
   void DrawTranslateScreen( HCGLBaseView *pView );
   void SetColor( GLuint nElement, const RGBAFloat &color );
   void SetMode( Mode nMode );
   void SetModeToSelectedElement();
   void OnTranslateChanged( BOOL bTemp );
   void OnRotateChanged( BOOL bTemp );
   void OnScaleChanged( BOOL bTemp );

   void SetShowManipulator( BOOL bShow )
   {
      if (bShow==m_bShowManipulator)
         return;

      m_bShowManipulator = bShow;

      if (!bShow)
         m_nSelectedElement = AXIS_NONE;
   }

   BOOL IsShowManipulator() const { return m_bShowManipulator; }

// Overridables
   virt_base virtual void Draw( HCGLBaseView *pView, float fAlpha=1.0f, BOOL bOpaqueItemsOnly=FALSE );
   virt_base virtual void OnStartManipulating( CPoint point, HCGLBaseView *pView );
   virt_base virtual void OnManipulating( CPoint point, HCGLBaseView *pView );
   virt_base virtual void OnEndManipulating( CPoint point, HCGLBaseView *pView );

// ManipulatorInstance overrides
   virtual catch_msg BOOL OnObjectNotify(ObjectNotificationMsg nMsg, WPARAM wParam, LPARAM lParam);
   virtual catch_msg void DrawPick(HCGLBaseView *pView);
   virtual catch_msg void PostDraw(HCGLBaseView *pView);
   virtual catch_msg BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnLButtonDblClk( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnMButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnMButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg UINT GetToolTipText( CPoint point, String &strText, const HCGLBaseView *pView );
   virtual catch_msg const char *GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const;
   virtual catch_msg KeyModifiers GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const;

// HCallbackHelper overrides
   virtual void OnSelectionListChanged( HSelectionList *pSelectionList, BOOL bTemporary );
};
