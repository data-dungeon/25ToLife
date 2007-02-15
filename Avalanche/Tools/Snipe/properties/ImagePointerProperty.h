#pragma once
#include "Image.h"
#include "Property.h"

class ImagePointerProperty : public PointerProperty<Image *>
{
public:
   DECLARE_SNIPEOBJECT_CREATE( ImagePointerProperty, PointerProperty<Image *> );

   class Info : public PointerProperty<Image *>::Info
   {
   public:
      Info( const String &strName, const String &strMatchName, UINT nPropertyInfoFlags=PIF_DEFAULT )
         : PointerProperty<Image *>::Info( strName, strMatchName, nPropertyInfoFlags )
      {
      }
   };

   static Transform2Property::Info   m_transformInfo;
      static Vector2Property::Info      m_v2TranslateInfo;
      static Vector2Property::Info      m_v2PivotInfo;

   Transform2Property    m_transform;
      Vector2Property       m_v2Pivot;

   ImagePointerProperty();

   BOOL Bind();
   BOOL UnBind();

// SnipeObject overrides
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
};

BaseStream &operator << (BaseStream &bs, const Image *&value );
BaseStream &operator >> (BaseStream &bs, Image *&value );
