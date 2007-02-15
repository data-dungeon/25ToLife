#pragma once

#include "HNavigatorPluginHelper.h"
#include "HVectorProperty.h"
#include "HFloatProperty.h"
#include "HBOOLProperty.h"
#include "HTypeProperty.h"
#include "HPropertyArray.h"
#include "HGLImageList.h"

class HCGL2DView;

class SnipeNavigatorPlugin : public HNavigatorPluginHelper
{
public:
   DECLARE_NAVIGATOR_PLUGINHELPER( SnipeNavigator, HNavigatorPluginHelper )

   HBoolProperty::HInfo   *m_pAnimateCameraInfo;
   HBoolProperty::HInfo   *m_pDisplayTargetInfo;
   HVectorProperty::HInfo *m_pTargetInfo;
   HFloatProperty::HInfo  *m_pRotationInfo;
   HFloatProperty::HInfo  *m_pAngleInfo;

   HGLImageList           *m_pImageList;

            SnipeNavigatorPlugin();
   virtual ~SnipeNavigatorPlugin();

// HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance( HCGLBaseView *pView );

// HCommandUIPlugin Overrides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
};

class SnipeNavigator : public NavigatorInstance
{
public:
   DECLARE_NAVIGATORINSTANCE( SnipeNavigator, NavigatorInstance )
   float m_ViewAngleDistance[NUM3DVIEWANGLEPRESETS];

   HPropertyArray  *m_pPropertyArray;
   HBoolProperty   *m_pAnimateCamera;
   HBoolProperty   *m_pDisplayTarget;
   HVectorProperty *m_pTarget;
//   HTypeProperty   *m_pViewAngle; // points to the one in the view, this is until we break it apart and move it into the navigator
   HBoolProperty   *m_pPerspective;   // points to the one in the current camera
   HFloatProperty  *m_pFOV;           // points to the one in the current camera
   HFloatProperty  *m_pFocusDistance; // points to the one in the current camera
	HFloatProperty  *m_pRotation;
	HFloatProperty  *m_pAngle;
   HBoolProperty   *m_pManualClip;    // points to the one in the current camera
   HFloatProperty     *m_pNearClip;   // points to the one in the current camera
   HFloatProperty     *m_pFarClip;    // points to the one in the current camera
   HFloatProperty  *m_pAspectRatio;   // points to the one in the current camera

   UINT m_nFOVIndex;
   UINT m_nFocusDistanceIndex;
   UINT m_nManualClipIndex;
//   UINT m_nViewAngleIndex;
   UINT m_nPerspectiveIndex;
   UINT m_nAspectRatioIndex;

   CPoint m_ptPrev;
   CPoint m_ptStart;
   BOOL m_bCalculateTransform;
   BOOL m_bCalculateProperties;

   enum NavigationMethod { NM_NONE, NM_TRUCK, NM_TRUCK_AXIS_LOCKED, NM_DOLLY, NM_DOLLY_AXIS_LOCKED, NM_ORBIT };
   NavigationMethod m_nCurrentNavigation;

   enum NavigationAxisLock { NAL_UNDETERMINED, NAL_HORIZONTAL, NAL_VERTICAL };
   NavigationAxisLock m_nNavigationAxisLock;

            SnipeNavigator( SnipeNavigatorPlugin *pPluginHelper, HCGLBaseView *pView );
   virtual ~SnipeNavigator(); 

   SnipeNavigatorPlugin *GetSnipeNavigatorPlugin() { return (SnipeNavigatorPlugin *)GetNavigatorPluginHelper(); }

// Operations
   void RotateCamera( float dx, float dy );
   void DollyCamera( float delta );
   void ScaleCamera( float deltaH, float deltaV );
   void CalculateTransform();
   void CalculateProperties();
   void ResetCameraProperties();

// NavigatorInstance Overrides
   virtual catch_msg BOOL EvaluateButtons( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView  );
   virtual catch_msg BOOL SetViewAngle( HCGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye );
   virtual catch_msg BOOL ZoomFit( HCGLBaseView *pView, HSnipeObjectList *pList );

// ManipulatorInstance Overrides
   virtual const char *GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const;
   virtual KeyModifiers GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const;
   virtual catch_msg void PostDraw( HCGLBaseView *pView );
   virtual catch_msg BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnMouseWheel( KeyModifiers keymodifier, MouseStates mousestate, short zDelta, CPoint point, HCGLBaseView *pView );
   virtual catch_msg HPropertyArray *GetPropertyArray() const;
   virtual catch_msg BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
