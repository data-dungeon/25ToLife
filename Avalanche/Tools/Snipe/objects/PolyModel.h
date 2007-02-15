#pragma once

#include "SceneObject.h"
#include "Poly.h"
#include "Edge.h"
#include "Vertex.h" 
#include "VertexNormal.h"
#include "UV.h"
#include "TexCoords.h"
#include "ComponentList.h"
#include "Interface/CommandList.h"
#include "Interface/MainInterface.h"
#include "CompositedSurface.h"

class BoundingBox;
class BoneContainer;
class RotoscopeContainer;
class PatchModel;
class ColorVertexSetContainer;
class ColorVertexSet;

template<>
UINT AFXAPI HashKey( SurfaceArray *pKey );
//template<>
//template<class TYPE, class ARG_TYPE>
//BOOL AFXAPI CompareElements( const SurfaceArray **pElement1, const SurfaceArray **pElement2 ); 

AFX_INLINE BOOL CompareElements( SurfaceArray * const *pElement1, SurfaceArray * const *pElement2 )
{
   return **pElement1 == **pElement2;
}

typedef CMap<SurfaceArray *, SurfaceArray *, CompositedSurface *, CompositedSurface *> CompositedSurfaceMap;

class PolyModel : public SceneObject
{
   DECLARE_MESSAGE_MAP()
protected:
   static FileInfoProperty::Info  m_fileinfoinfo;
      static Property::Info          m_surfaceinfo;
   static Property::Info          m_statisticsinfo;
      static StatisticProperty::Info m_nVerticesStatInfo;
      static StatisticProperty::Info m_nVertexNormalsStatInfo;
      static StatisticProperty::Info m_nEdgesStatInfo;
      static StatisticProperty::Info m_nFacesStatInfo;
      static StatisticProperty::Info m_nSurfacesStatInfo;
      static StatisticProperty::Info m_nBonesStatInfo;
      static StatisticProperty::Info m_nVertexArraySizeStatInfo;

public:
   FileInfoProperty    m_fileinfo;
   Property            m_surface;
      Surface             m_DefaultSurface;
   Property            m_statistics;
      StatisticProperty   m_nVerticesStat;
      StatisticProperty   m_nVertexNormalsStat;
      StatisticProperty   m_nEdgesStat;
      StatisticProperty   m_nFacesStat;
      StatisticProperty   m_nSurfacesStat;
      StatisticProperty   m_nBonesStat;
      StatisticProperty   m_nVertexArraySizeStat;

protected:
   BoneContainer      *m_pBoneContainer;
   SurfaceContainer   *m_pSurfaceContainer;
   TexCoordsContainer *m_pTexCoordsContainer;
   RotoscopeContainer *m_pRotoscopeContainer;
   ColorVertexSetContainer *m_pColorVertexSetContainer;

   GLuint              m_glCallList;
   GLuint              m_glVertexIndiceCallList;
   GLuint              m_glFaceIndiceCallList;
   GLuint              m_glEdgeIndiceCallList;

   CComponentList<VertexNormal> m_vertexnormallist;
   CComponentList<Vertex> m_vertexlist;
   CComponentList<Edge> m_edgelist;
   CComponentList<Poly> m_polylist;
   CompositedSurfaceMap m_compositedsurfaces;

   RTVertex           *m_pRTVertices;
   UINT                m_nNumRTVertices;
   UINT               *m_pVisibleVertices; // visible and not locked
   UINT                m_nNumVisibleVertices;
   UINT               *m_pLockedVertices; // visible and not locked
   UINT                m_nNumLockedVertices;
   RTEdge             *m_pRTEdges; // soft, hard, border & hidden (in that order)
   UINT                m_nNumRTEdges;
   UINT                m_nNumHardEdges;
   UINT                m_nNumBorderEdges;

   PolyModel          *m_pCurrentInstance; // Set during drawing of an instance
   float               m_fDrawOpacity; // currently used for composited view in motionsets

   BoundingSphere      m_BoundingSphere;

   union {
      ULONG            m_nPolyModelFlags;
      struct {
         ULONG         m_bDirtyEdges              : 1,
                       m_bCompositedSurfacesDirty : 1,
                       m_bRebuildBufferObjects    : 1,
                       m_bBoundingSphereDirty     : 1;
      };
   };

   UINT m_nNumBones;
   static RGBFloat m_fBoneColors[16];

public:
   DECLARE_SNIPEOBJECT_CREATE( PolyModel, SceneObject )
   DECLARE_COMMANDOBJECT()
   
            PolyModel();
   virtual ~PolyModel();

   PolyModel *GetRootCache() { return (PolyModel *)SceneObject::GetRootCache(); } // Note: returns this if it is the cache
   PolyModel *GetCache() { return (PolyModel *)SceneObject::GetCache(); } // Note: returns this if it is the cache
 
   BOOL IsBoundingSphereDirty() { return m_bBoundingSphereDirty; }
   void SetBoundingSphereDirty( BOOL bBoundingSphereDirty=TRUE ) { m_bBoundingSphereDirty = bBoundingSphereDirty; }
   const BoundingSphere &GetBoundingSphere() { if (IsBoundingSphereDirty()) ComputeBoundingSphere(); return m_BoundingSphere; }
   void ComputeBoundingSphere();

   PolyModel *GetCurrentInstance() const { return m_pCurrentInstance; }
   void SetCurrentInstance( PolyModel *pPolyModel ) { m_pCurrentInstance = pPolyModel; }

   // Message Maps
   afx_msg void OnUpdateExportModel(CCmdUI* pCmdUI);
   afx_msg void OnExportModel();

   RotoscopeContainer *CreateRotoscopeContainer();
   RotoscopeContainer *GetRotoscopeContainer() { return m_pRotoscopeContainer; }

   // Surfaces
   SurfaceContainer *CreateSurfaceContainer();
   SurfaceContainer *GetSurfaceContainer() { return m_pSurfaceContainer; }
   Surface *AddSurface( const char *name=NULL, int nIndex=-1, Surface::DeleteInfo **pDeleteInfo=NULL );
   void RemoveSurface( Surface *pSurface );

   // Composited surfaces
   void BuildCompositedSurfaces();
   CompositedSurface *FindCompositedSurface( Poly *pPoly );
   UINT GetNumCompositedSurfaces() { return m_compositedsurfaces.GetCount(); }
   POSITION GetHeadCompositedSurfacePosition() { return m_compositedsurfaces.GetStartPosition(); }
   CompositedSurface *GetNextCompositedSurface( POSITION &pos )
   {
      SurfaceArray *pSufaceArray;
      CompositedSurface *pCompositedSurface;
      m_compositedsurfaces.GetNextAssoc( pos, pSufaceArray, pCompositedSurface );
      return pCompositedSurface;
   }
   BOOL RemoveCompositedSurface( CompositedSurface *pCompositedSurface )
   {
      return m_compositedsurfaces.RemoveKey( &pCompositedSurface->m_SurfaceArray );
   }
   void SetCompositedSurfacesDirty( BOOL bDirty ) { m_bCompositedSurfacesDirty = bDirty; }
   void SetDrawOpacity( float fDrawOpacity ) { m_fDrawOpacity = fDrawOpacity; }
   float GetDrawOpacity() const { return m_fDrawOpacity; }

   // Vertices
   UINT GetNumVertices();
   POSITION GetHeadVertexPosition() { return m_vertexlist.GetHeadPosition(); }
   Vertex *GetNextVertex( POSITION &pos ) { return m_vertexlist.GetNext( pos ); }
   Vertex *AddVertex();
   Vertex *AddVertex( UINT nID );
   Vertex *AddVertex( const Vector &pos, Vertex::DeleteInfo **pOutVertexDeleteInfo=NULL, BOOL bMirrorIfInMirrorMode=FALSE );
   void RemoveVertex(Vertex *pVertex);
   void DrawPickVertices( CGLModelView *pView );
   Vertex *FindVertex( UINT nID ) { return m_vertexlist.Find( nID ); }
   UINT GetNumRTVertices() { return m_nNumRTVertices; }

   // VertexNormals
   UINT GetNumVertexNormals();
   POSITION GetHeadVertexNormalPosition() { return m_vertexnormallist.GetHeadPosition(); }
   VertexNormal *GetNextVertexNormal( POSITION &pos ) { return m_vertexnormallist.GetNext( pos ); }
   VertexNormal *AddVertexNormal();
   VertexNormal *AddVertexNormal( UINT nID );
   VertexNormal *AddVertexNormal( const Vector &dir );
   VertexNormal *FindVertexNormal( UINT nID ) { return m_vertexnormallist.Find( nID ); }
   void SetVertexNormals(const CArray<UINT> &faceindicies,const CArray<Vector> &vertexnormals);
   void DrawVertexNormalIndices( CGLModelView *pModelView );

   // Edges
   UINT GetNumEdges();
   POSITION GetHeadEdgePosition();
   Edge *GetNextEdge( POSITION &pos ) { return m_edgelist.GetNext( pos ); }
   Edge *AddEdge();
   Edge *AddEdge( UINT nID );
   Edge *AddEdge(Vertex *pV0, Vertex *pV1 );
   void RemoveEdge(Edge *pEdge);
   void DrawPickEdges( CGLModelView *pView );
   BOOL HasDirtyEdges() { return m_bDirtyEdges; }
   void SetDirtyEdgesFlag(BOOL bFlag) { m_bDirtyEdges = bFlag; }
   int AddEdges( SnipeObjectList *pVertices, Edge::DeleteInfo **&pOutEdgeDeleteInfos, Edge *pPrevEdge=EDGE_TERMINATE, Edge *pNextEdge=EDGE_TERMINATE );
   Edge::DeleteInfo *AddEdge( Vertex *pV0, Vertex *pV1, Edge *pPrevEdge, Edge *pNextEdge);
   void BuildEdges();
   Edge *FindMatchingManifoldEdge(Edge *pEdge,Poly *pPoly);

   // Poly
   UINT GetNumPolys();
   POSITION GetHeadFacePosition() { return m_polylist.GetHeadPosition(); }
   Poly *GetNextFace( POSITION &pos ) { return m_polylist.GetNext( pos ); }
   Poly *AddPoly();
   Poly *AddPoly( UINT nID );
   void RemovePoly(Poly *pPoly) { m_polylist.Remove( pPoly ); }
   Poly *FindPoly( UINT nID ) { return m_polylist.Find( nID ); }
    void DrawPickFaces( CGLModelView *pView );
   void DrawFaces( CGLModelView *pModelView );
   DeleteFaceInfo *CreateFace( UINT nNumSides, Edge *pEdges[], BOOL bVerify=TRUE, String *strOutError=NULL ); // Used for modeling not loading, must delete return value

   // texture coordinates
   TexCoordsContainer *CreateTexCoordsContainer();
   TexCoordsContainer *GetTexCoordsContainer() { return m_pTexCoordsContainer; }
   TexCoords *AddTexCoords(const char *name = NULL);

   // vertex color sets
   ColorVertexSetContainer *CreateColorVertexSetContainer();
   ColorVertexSetContainer *GetColorVertexSetContainer() { return m_pColorVertexSetContainer; }
   ColorVertexSet *AddColorVertexSet(const char *name = NULL);

   // Bones
   UINT GetNumBones() { return m_nNumBones; }
   BoneContainer *CreateBoneContainer();
   BoneContainer *GetBoneContainer() { return m_pBoneContainer; }
   void AddBone( TransformableObject *pBone, TransformableObject *pParent );
   RGBFloat NewBoneColor();
   void SetBoneCount(UINT nNumBones) { m_nNumBones = nNumBones; }
   TransformableObject *FindBone( const char *name );
   TransformableObject *FindBone( const char *name, TransformableObject *pTransformableObject );

   void Compact();// compacts component neighbor arrays

protected:
   void DrawVertices();
   void DrawVertexNormals( CGLModelView *pModelView );

   void DrawEdges( CGLModelView *pModelView );
   void DrawEdgeIndices( CGLModelView *pModelView );

   void DrawFaceNormals( CGLModelView *pModelView );
   void DrawFaceIndices( CGLModelView *pModelView );

   void DrawFirstVertices( CGLModelView *pModelView );

   void DrawUVBoundaries( CGLModelView *pModelView );
   void DrawUVFaceIndices( CGLModelView *pModelView );
   void DrawUVIndices( CGLModelView *pModelView );
   void DrawUVEdgeIndices( CGLModelView *pModelView );

   BOOL IsDrawTess( CGLModelView *pModelView );

   BoundingBox CalculateBoundingBox( CoordinateSpace nSpace, CGLModelView *pView=NULL );

   void ConvertFromPatchModel(PatchModel *pPatchModel);

public:
   // Overridables
   virt_base virtual Vertex *CreateVertex( UINT nID=-1 ) const;
   virt_base virtual Edge *CreateEdge( UINT nID=-1 ) const;
   virt_base virtual Poly *CreatePoly( UINT nID=-1 ) const;
   virt_base virtual Edge *AddEdgeBetween( Vertex *pV0, Vertex *pV1, Edge **pOutReversedEdge=NULL, Edge *pPrevEdge=EDGE_FINDCONTINUOUS, Edge *pNextEdge=EDGE_FINDCONTINUOUS );
   virt_base virtual void DrawVertexIndices( CGLModelView *pModelView );
   virt_base virtual BOOL DrawPick( CGLBaseView *pBaseView );
   virt_base virtual void DrawImmediateMode( CGLModelView *pModelView );
   virt_base virtual void DrawRetainedMode( CGLModelView *pModelView );
   virt_base virtual void FreeRTVertices();
   virt_base virtual void CreateRTVertices( BOOL bTessellated );
   virt_base virtual BOOL CreateRTQuads( CompositedSurface *pCompositedSurface, BOOL bTessellated );
   virt_base virtual void BuildVertexNormals();

   // TransformableObject overrides
   virtual void PositionVertices( PolyModel *pModel, BOOL bTessellated );

   // HierObject overrides
   virtual CSnipeDocTemplate *GetDocTemplate();
   virtual void ConvertFrom(HierObject *pHierObject);
   virtual BOOL AddRotoscope( Rotoscope *pRotoscope );
   virtual BOOL IsDraggable() { return TRUE; }

   // InstanceObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_MODEL; };
   virtual void Draw( CGLBaseView *pBaseView );
   virtual void CountPickObjectType( CGLBaseView *pBaseView, UINT &nCount );

   // SnipeObject overrides
   virtual UINT GetDefaultMenuItemID();
   virtual SimpleComponentList *FindComponentList( const String &strComponentType );
   virtual FileInfoProperty *GetFileInfo() { return &m_fileinfo; }
   virtual void DirtyDependentFaces();
   virtual void DirtyDependentEdges();
   virtual Component *CreateComponent( SnipeObjectType nSOT );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL ParseData( BaseStream &bs, const DataHeader &dataheader );
   virtual void OnEndProjectLoad();
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
   virtual Property *GetStatisticsProperty() { return &m_statistics; }
};

class ModelPointerProperty : public PointerProperty<PolyModel *>
{
};

BaseStream &operator << (BaseStream &bs, const PolyModel *&value );
BaseStream &operator >> (BaseStream &bs, PolyModel *&value );

#include "PolyModel.hpp"
