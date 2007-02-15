#pragma once
#include "SceneObject.h"
#include "ContainerObject.h"

class Light : public SceneObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Light, SceneObject)

public:
   enum { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_SPOT };

protected:
   static ColorProperty::Info  m_cAmbientColorInfo;
   static ColorProperty::Info  m_cDiffuseColorInfo;
   static ColorProperty::Info  m_cSpecularColorInfo;
   static TypeProperty::Info   m_nTypeInfo;
   static FloatProperty::Info  m_fSpotExponentInfo;
   static FloatProperty::Info  m_fSpotCutoffInfo;
   static VectorProperty::Info m_vAttenuationInfo;
   static GLuint               m_glDirectionalIconCallList;
   static GLuint               m_glPointIconCallList;
   static GLuint               m_glSpotIconCallList;

public:
   ColorProperty  m_cAmbientColor;
   ColorProperty  m_cDiffuseColor;
   ColorProperty  m_cSpecularColor;
   TypeProperty   m_nType;
   FloatProperty  m_fSpotExponent;
   FloatProperty  m_fSpotCutoff;
   VectorProperty m_vAttenuation;

public:
            Light();
   virtual ~Light();

// Operations
   void Bind( UINT nIndex );
   void UnBind();

// InstanceObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_LIGHT; };
   virtual void Draw( CGLBaseView *pView );

// SnipeObject overrides
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
};

class LightContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(LightContainer, ContainerObject)

            LightContainer();
   virtual ~LightContainer();

   UINT GetNumLights() const { return GetNumChildren(); }
   Light *GetLightAt( UINT nIndex ) { return (Light *)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};