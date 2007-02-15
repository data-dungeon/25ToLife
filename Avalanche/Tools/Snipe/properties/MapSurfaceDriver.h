#pragma once

#include "Properties/SurfaceDriver.h"
#include "ImagePointerProperty.h"

class MapSurfaceDriver : public SurfaceDriver
{
public:
   enum BlendStyle
   {
      REPLACE,
      MULTIPLY,
      ADDITIVE,
      DECAL,//this is for internal use only
   };

   static GLImageList m_glImageListPivot;

   static ImagePointerProperty::Info m_pImageInfo;
   static TypeProperty::Info         m_nBlendStyleInfo;
   static FloatProperty::Info        m_fOpacityInfo;

   ImagePointerProperty  m_pImage;
   TypeProperty          m_nBlendStyle;
   FloatProperty         m_fOpacity;

   DECLARE_SNIPEOBJECT_PURE( MapSurfaceDriver, SurfaceDriver )

   MapSurfaceDriver();

   BOOL operator == ( MapSurfaceDriver &other );

   void DoStateChange(const BlendStyle style,float opacity);
   Image *GetImage() const { return ((MapSurfaceDriver *)this)->m_pImage.GetValue(); }
   void TransformUV( Vector2 &uv );

// SurfaceDriver Overrides
   virtual BOOL IsValid() { return GetImage()!=NULL; }
   virtual void Bind();
   virtual void UnBind();

// SnipeObject overrides
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
};

#ifdef __AFXTEMPL_H__
template<> AFX_INLINE UINT AFXAPI HashKey<const MapSurfaceDriver &>( const MapSurfaceDriver &key )
{
   return (DWORD)(((DWORD_PTR)key.GetImage())>>4);
}
#endif

inline BOOL MapSurfaceDriver::operator == ( MapSurfaceDriver &other )
{
   if ((m_pImage==other.m_pImage)&&(m_nBlendStyle==other.m_nBlendStyle)&&(m_fOpacity==m_fOpacity))
      return TRUE;

   return FALSE;
}
