#pragma once

#include "HierObject.h"
#include "Properties/ImagePointerProperty.h"
#include "ContainerObject.h"

class Rotoscope : public HierObject
{
protected:
   static TypeProperty::Info         m_nViewAngleInfo;
   static ImagePointerProperty::Info m_pImageInfo;
   static FloatProperty::Info        m_fOpacityInfo;
   static Transform2Property::Info   m_transformInfo;

public:
   TypeProperty          m_nViewAngle;
   ImagePointerProperty  m_pImage;
   FloatProperty         m_fOpacity;
   Transform2Property    m_transform;

   DECLARE_SNIPEOBJECT_CREATE( Rotoscope, HierObject )

            Rotoscope();
   virtual ~Rotoscope();

   void PushMatrix( CGL3DView *p3DView );

// SnipeObject overrides
   virtual int GetItemHeight();
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virtual void Draw( CGLBaseView *pView );
   virtual void DrawFocused( CGLBaseView *pView );
};

////////////////////////////////////////////////////////////////////

class RotoscopeContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(RotoscopeContainer, ContainerObject)

            RotoscopeContainer();
   virtual ~RotoscopeContainer();

   UINT GetNumRotoscopes() const { return GetNumChildren(); }
   Rotoscope *GetRotoscopeAt( UINT nIndex ) { return (Rotoscope *)GetChildAt(nIndex); }
};
