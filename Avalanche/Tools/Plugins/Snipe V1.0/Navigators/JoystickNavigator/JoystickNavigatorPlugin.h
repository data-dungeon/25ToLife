#pragma once

#include "HNavigatorPluginHelper.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"
#include "HFloatProperty.h"
#include "HBoolProperty.h"
#include "HTypeProperty.h"
#include "HPropertyArray.h"
#include "HGLImageList.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class HCGL2DView;

class JoystickNavigatorPlugin : public HNavigatorPluginHelper
{
public:
   DECLARE_NAVIGATOR_PLUGINHELPER( JoystickNavigator, HNavigatorPluginHelper )

   HBoolProperty::HInfo   *m_pAnimateCameraInfo;
   //HTranslateProperty::HInfo *m_pPositionInfo;
   //HRotateProperty::HInfo  *m_pOrientationInfo;

   LPDIRECTINPUT8       m_pDI;
   LPDIRECTINPUTDEVICE8 m_pJoystick;
   int                  m_nSliderCount;  // Number of returned slider controls
   int                  m_nPOVCount;     // Number of returned POV controls

            JoystickNavigatorPlugin();
   virtual ~JoystickNavigatorPlugin();

// Operations
   HRESULT InitDirectInput();
   void FreeDirectInput();
   BOOL EnumJoysticksCallback( const DIDEVICEINSTANCE *pdidInstance );
   BOOL EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE *pdidoi );

// HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance( HCGLBaseView *pView );

// HCommandUIPlugin Overrides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
};

class JoystickNavigator : public NavigatorInstance
{
public:
   DECLARE_NAVIGATORINSTANCE( JoystickNavigator, NavigatorInstance )

   LPDIRECTINPUTDEVICE8 m_pJoystick;
   UINT_PTR             m_nTimerID;

   HPropertyArray  *m_pPropertyArray;
   HBoolProperty      *m_pAnimateCamera;
   HBoolProperty      *m_pPerspective;   // points to the one in the current camera
   HFloatProperty     *m_pFOV;           // points to the one in the current camera
   HFloatProperty     *m_pFocusDistance; // points to the one in the current camera
   HTranslateProperty *m_pPosition;      // points to the one in the current camera
   HRotateProperty    *m_pOrientation;   // points to the one in the current camera
   HBoolProperty      *m_pManualClip;    // points to the one in the current camera
      HFloatProperty     *m_pNearClip;   // points to the one in the current camera
      HFloatProperty     *m_pFarClip;    // points to the one in the current camera

   UINT m_nPerspectiveIndex;
   UINT m_nFOVIndex;
   UINT m_nFocusDistanceIndex;
   UINT m_nPositionIndex;
   UINT m_nOrientationIndex;
   UINT m_nManualClipIndex;

   BOOL m_bPerspectiveSetLastTime;

            JoystickNavigator( JoystickNavigatorPlugin *pPluginHelper, HCGLBaseView *pView );
   virtual ~JoystickNavigator(); 

   JoystickNavigatorPlugin *GetJoystickNavigatorPlugin() { return (JoystickNavigatorPlugin *)GetNavigatorPluginHelper(); }

// Operations
   void OnTimer();
   void ResetCameraProperties();

// NavigatorInstance Overrides
   virtual catch_msg BOOL SetViewAngle( HCGL3DView *pView, ViewAngleIndex nIndex, BOOL bSnapToLastBirdsEye );
   virtual catch_msg BOOL ZoomFit( HCGLBaseView *pView, HSnipeObjectList *pList );

// ManipulatorInstance Overrides
   virtual catch_msg HPropertyArray *GetPropertyArray() const;
};
