#pragma once
#include "SceneObject.h"
#include "ContainerObject.h"

#define DEFAULTFOV 30.0f

class Camera : public SceneObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Camera, SceneObject)

   static BoolProperty::Info    m_bPerspectiveInfo;
   static FloatProperty::Info   m_fFOVInfo;
   static FloatProperty::Info   m_fFocusDistanceInfo;
   static BoolProperty::Info    m_bManualClipInfo;
   static FloatProperty::Info      m_fNearClipInfo;
   static FloatProperty::Info      m_fFarClipInfo;
   static FloatProperty::Info      m_fAspectRatioInfo;

   static GLuint m_glCameraCallList;
   static GLuint m_glCameraLogoCallList;

   BoolProperty    m_bPerspective;
   FloatProperty   m_fFOV;
	FloatProperty   m_fFocusDistance;
   BoolProperty    m_bManualClip;
   FloatProperty      m_fNearClip;
   FloatProperty      m_fFarClip;
   FloatProperty      m_fAspectRatio;

public:
            Camera();
   virtual ~Camera();

   float GetCameraDistanceFactor();

// InstanceObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_CAMERA; };

// SnipeObject overrides
   virtual void Draw( CGLBaseView *pView );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
};

class CameraContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(CameraContainer, ContainerObject)

            CameraContainer();
   virtual ~CameraContainer();

   UINT GetNumCameras() const { return GetNumChildren(); }
   Camera *GetCameraAt( UINT nIndex ) { return (Camera *)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};
