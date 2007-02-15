#pragma once
#include "BaseDoc.h"

class Camera;
class Light;

class C3DDoc : public CBaseDoc
{
protected:
	DECLARE_BASEDOC(C3DDoc, CBaseDoc)

// Attributes
public:
   Vector m_vBoundingCenter;
   float  m_fBoundingRadius;
   CArray<Camera *> m_cameraarray;
   CArray<Light *>  m_lightarray;

         	C3DDoc();
	virtual ~C3DDoc();

// Operations
   void RebuildCameraArray( SnipeObject *pStartObj=NULL );
   void RebuildLightArray( SnipeObject *pStartObj=NULL );

// Overridables
   virt_base virtual const BoundingSphere &GetBoundingSphere() { static BoundingSphere boundingsphere; return boundingsphere; }

// CBaseDoc overrides
   virtual SnipeDocType GetDocType() { return SDT_C3DDoc; }
   virtual void SetObjectHead( HierObject *pObjectHead );
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
