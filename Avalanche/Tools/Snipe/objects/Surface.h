#pragma once

#include "SetObject.h"
#include "ContainerObject.h"
#include "../Properties/Property.h"

class DecalMapSurfaceDriverContainer;
class DecalMapSurfaceDriver;
class Image;
class TexCoords;
class Surface;
class SurfaceDriver;
class UVPoly;
class Poly;
class PolyModel;

class Surface : public HierObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Surface, HierObject)

   static GLuint m_glSurfaceIconCallList;

   static TypeProperty::Info    m_nBlendMethodInfo;
     static FloatProperty::Info    m_fBlendFactorInfo;
   static ColorProperty::Info   m_cAmbientColorInfo;
   static ColorProperty::Info   m_cDiffuseColorInfo;
   static ColorProperty::Info   m_cSpecularColorInfo;
   static ColorProperty::Info   m_cEmissionColorInfo;
   static FloatProperty::Info   m_fTransparencyInfo;
   static FloatProperty::Info   m_fShininessInfo;
   static Vector2Property::Info m_vNormalOffsetInfo;
   static TypeProperty::Info    m_nCullFaceModeInfo;

   TypeProperty    m_nBlendMethod; enum { BM_REPLACE, BM_ADD, BM_SUBTRACT, BM_MULTIPLY };
      FloatProperty   m_fBlendFactor;
   ColorProperty   m_cAmbientColor;
   ColorProperty   m_cDiffuseColor;
   ColorProperty   m_cSpecularColor;
   ColorProperty   m_cEmissionColor;
   FloatProperty   m_fTransparency;
   FloatProperty   m_fShininess;
   Vector2Property m_vNormalOffset;
   TypeProperty    m_nCullFaceMode; enum { CFM_NONE, CFM_FRONT, CFM_BACK };

   CArray<Poly *>  m_facearray;
   BoundingBox     m_bounds;

            Surface();
   virtual ~Surface();

// Operations
   PolyModel *GetModel() { return (PolyModel *)GetCommandObject(); }

   void Bind();
   void UnBind();

   BoundingBox GetBoundingBox() const;

   void DrawFacesBase( const Matrix44 &viewmatrix, SurfaceDriver *pSurfaceDriver=NULL );
   void DrawFaces( const Matrix44 &viewmatrix );

   UINT GetNumFaces() const;
   Poly *GetFaceAt( UINT nIndex ) const;
   UINT AddFace( Poly *pFace );
   BOOL RemoveFace( Poly *pFace, BOOL bRemoveSurfaceFromFaceToo=TRUE );
   BOOL RemoveAllFaces();
   UINT FindFace( Poly *pFace ) const;
   void DirtyCompositedSurfaces();
   void DestroyCompositedSurfaces();

   ColorProperty *GetAmbientProperty() { return &m_cAmbientColor; }
   ColorProperty *GetDiffuseProperty() { return &m_cDiffuseColor; }
   ColorProperty *GetSpecularProperty() { return &m_cSpecularColor; }
   ColorProperty *GetEmissionProperty() { return &m_cEmissionColor; }
   FloatProperty *GetTransparencyProperty() { return &m_fTransparency; }
   FloatProperty *GetShininessProperty() { return &m_fShininess; }
   Vector2Property *GetNormalOffsetProperty() { return &m_vNormalOffset; }
   TypeProperty *GetCullFaceModeProperty() { return &m_nCullFaceMode; }

   UINT GetNumSurfaceDrivers();
   int AddSurfaceDriver( SurfaceDriver *pSurfaceDriver, Property *pProperty );
   SurfaceDriver *GetSurfaceDriverAt(UINT nIndex);
   SurfaceDriver *GetSurfaceDriver( Property *pProperty );

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual int GetItemHeight();
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
};

class SurfaceContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(SurfaceContainer, ContainerObject)

            SurfaceContainer();
   virtual ~SurfaceContainer();

   UINT GetNumSurfaces();
   Surface *GetSurfaceAt( UINT nIndex );
   void RemoveSurface( Surface *pSurface );

// SnipeObject overrides
   virtual BOOL Load( BaseStream &bs );
   virtual void Draw( CGLBaseView *pView ) {} // don't recurse, it's a waiste of time
};

#include "Surface.hpp"