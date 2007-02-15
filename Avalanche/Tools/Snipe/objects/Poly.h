#pragma once

#include "Vertex.h"
#include "Edge.h"
#include "UV.h"
#include "VertexNormal.h"
#include "TexCoords.h"
#include "Surface.h"

class Patch;
class DecalMapSurfaceDriver;

class PropertyVertex;
class PropertyPoly;


inline void SwapPointers( void *&p1, void *&p2 )
{
   void *pTemp = p1;
   p1 = p2;
   p2 = pTemp;
}

#define EDGE_FINDCONTINUOUS (Edge *)0x00000000
#define EDGE_TERMINATE      (Edge *)0x00000001

class CompositedSurface;
   
class BuildPolyInfo
{
public:
   CArray<Vertex *> m_newvertices;
   CArray<VertexNormal *> m_newvertexnormals;
   CArray<Edge *> m_newedges;
   CArray< CArray<PropertyVertex *> > m_newPropertyVertices;
   CArray< CArray<PropertyEdge *> > m_newpropertyedges;
};

class Poly : public Component
{
protected:
   Vector             m_faceNormal;
   Vertex           **m_pVertices;
   UINT              *m_pRTVertexIndices;
   VertexNormal     **m_pVertexNormals;
   Edge             **m_pEdges;
   Surface          **m_pSurfaces;
   CompositedSurface *m_pCompositedSurface;
   PropertyPoly      *m_pHeadPropertyPoly;

   union {
      ULONG           m_nPolyFlags;
      struct {       
         ULONG        m_nNumSides                      : 8,
                      m_nNumSurfaces                   : 8,
                      m_bDirtyFaceNormal               : 1,
                      m_bRemoveEdgesOnDisconnectIfNotUsed : 1,
                      // Patch flags (for sake of memory)
                      m_bDirtyTangents                 : 1,
                      m_bDirtyTessellation             : 1,
                      m_bDirtyTessellationAllocation   : 1;
      };
   };

public:
   class DeleteInfo : public Component::DeleteInfo
   {
   public:
      DeleteInfo( SnipeObject *pSnipeObject )
         : Component::DeleteInfo( pSnipeObject )
      {
         Poly *pPoly = (Poly *)GetSnipeObject();
         for (UINT i=0; i<pPoly->GetNumSides(); i++)
            pPoly->GetVertexAt(i)->Reference();
      }
      ~DeleteInfo()
      {
         Poly *pPoly = (Poly *)GetSnipeObject();
         for (UINT i=0; i<pPoly->GetNumSides(); i++)
            pPoly->GetVertexAt(i)->DeReference();
      }
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

public:
   DECLARE_COMPONENT_CREATE(Poly, Component)

            Poly();
            Poly( UINT nID );
	virtual ~Poly();

   void Init();

   static Poly *FindFace(UINT nVertexCount, Vertex **pVertexArray);
   static Poly *FindPoly( Vertex *pV0, Vertex *pV1 );

   PolyModel *GetModel() { return (PolyModel *)GetOwner(); }
   UINT GetNextIndex( UINT nIndex ) const;
   UINT GetPrevIndex( UINT nIndex ) const;

   Edge::DeleteInfo *SplitPoly( Vertex *pV0, Vertex *pV1, Edge *pPrevEdge=EDGE_FINDCONTINUOUS, Edge *pNextEdge=EDGE_FINDCONTINUOUS );
   void Isolate();

   UINT GetNumSides() const;

   // vertices
   Vertex *GetVertexAt( UINT nIndex ) const;
   BuildPolyInfo *RemoveVertex(Edge *pNewEdge, CArray< PropertyEdge *> *pNewPropertyEdges, Vertex *pAtVertex); // Removes two edges and replaces it with pNewEdge
   BuildPolyInfo *ReinsertVertex( Vertex *pOldVertex, Edge *pOldEdge0, CArray<PropertyEdge *> *pOldPropertyEdges0, Edge *pOldEdge1, CArray<PropertyEdge *> *pOldPropertyEdges1, UINT nOldIndex, VertexNormal *pOldVertexNormal, CArray<PropertyVertex *> *pOldUVs );
   void InsertVertex( Vertex *pNewVertex, VertexNormal *pNewVertexNormal, CArray<PropertyVertex*> *pNewPropertyVertices, Vertex *pAtVertex, float fPosition, Edge *&pOutNewEdge0, CArray<PropertyEdge *> &pOutNewPropertyEdges0, Edge *&pOutNewEdge1, CArray<PropertyEdge *> &pOutNewPropertyEdges1 );

   BOOL IsInFace( Vertex *pVertex );

   // rtvertices
   UINT GetRTVertexAt( UINT nIndex ) const;
   void SetRTVertexAt( UINT nIndex, UINT nRTVertex );

   // vertexnormals
   VertexNormal *GetVertexNormalAt( UINT nIndex, BOOL bCalculateIfNecessary=TRUE );
   void FindAndCreateNormals();

   // edges
   Edge *GetEdgeAt( UINT nIndex );
   UINT FindEdge( Edge *pEdge ); // returns -1 if not found
   Edge *GetSharedEdge( Poly *pPoly );
   BOOL BuildEdges();
   Edge *FindNearestEdge( CGLModelView *pView, const CPoint &pt );
   void SetRemoveEdgesOnDisconnectIfNotUsed( BOOL bState ) { m_bRemoveEdgesOnDisconnectIfNotUsed = bState; }
   BOOL IsRemoveEdgesOnDisconnectIfNotUsed() const { return m_bRemoveEdgesOnDisconnectIfNotUsed; }

   // UVPoly
//   UINT CountNumUVPolys();
   PropertyPoly *FindLastPropertyPoly(SnipeObjectType nPropertySetSOT);
   UVPoly *FindUVPoly( TexCoords *pTexCoords );

   // PropertyPolys
   UINT CountNumPropertyPolys();
   PropertyPoly *GetHeadPropertyPoly() const { return m_pHeadPropertyPoly; }
   void SetHeadPropertyPoly( PropertyPoly *pPropertyPoly ) { m_pHeadPropertyPoly = pPropertyPoly; }
   PropertyPoly *FindPropertyPoly( PropertySet *pPropertySet );
  
   // Surfaces
   UINT GetNumSurfaces() const;
   void SetNumSurfaces( UINT nSize );
   UINT AddSurface( Surface *pSurface );
   Surface *GetSurfaceAt( UINT nIndex ) const;
   void SetSurfaceAt( UINT nIndex, Surface *pSurface );
   BOOL RemoveSurface( Surface *pSurface );
   void RemoveSurfaceAt( UINT nIndex );
   UINT FindSurface( Surface *pSurface );

   CompositedSurface *GetCompositedSurface();
   void SetCompositedSurface( CompositedSurface *pCompositedSurface, BOOL bRemoveFromPrev=TRUE );
 
   // face normals
   void BuildFaceNormal();
   Vector *GetFaceNormal();

   // misc
   void BuildPoly( BuildPolyInfo *pInfo );
   void BuildPoly( CArray<Vertex *> &newvertices, CArray<VertexNormal *> &newvertexnormals, CArray<Edge *> &newedges,
    CArray< CArray<PropertyVertex *> > &newuvs, CArray< CArray<PropertyEdge *> > &newPropertyEdges );
   int FindVertexFaceRelativeIndex(Vertex *pVertex, BOOL bIncludeAttached=TRUE);
   int FindVertexNormalFaceRelativeIndex(VertexNormal *pFind);
   void CalcCenter(Vector &pos);
   BOOL IsAdjacentPolyWindingTheSame( Edge *pEdge );

   BOOL IsDirtyFaceNormal() { return m_bDirtyFaceNormal; }
   void SetDirtyFaceNormal( BOOL bDirtyFaceNormal );

   Poly *FindConnectedPoly(Vertex *pV0, Vertex *pV1);

   DeleteFaceInfo *DeleteFace();

// Overridables
   virt_base virtual void SetNumSides( UINT nNumSides );
   virt_base virtual void Draw( const Matrix44 &viewmatrix, BOOL bDrawTess, SurfaceDriver *pSurfaceDriver=NULL );
   virt_base virtual void SetDirty();
   virt_base virtual void DrawPick( const Matrix44 &viewmatrix, BOOL bDrawTess, SurfaceDriver *pSurfaceDriver=NULL ) { Draw( viewmatrix, bDrawTess, pSurfaceDriver ); }
   virt_base virtual void DrawPickNearest( Vertex *pVertex );
   virt_base virtual void DrawPickNearest( Edge *pEdge );
   virt_base virtual Edge *FindEdgeClosestToPoint( const Vector &point );
   virt_base virtual void ReverseWinding();
   virt_base virtual void FlipNormal();
   virt_base virtual void SetVertexAt( UINT nIndex, Vertex *pVertex );
   virt_base virtual void SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal );
   virt_base virtual BOOL SetEdgeAt( UINT nIndex, Edge *pEdge );

   // property poly
   virt_base virtual void AddPropertyPoly( PropertyPoly *pPropertyPoly );
   virt_base virtual BOOL RemovePropertyPoly( PropertyPoly *pUVPoly );

// Component overrides
   virtual BOOL IsUsed();
   virtual void Reconnect();
   virtual void Disconnect();

// SnipeObject overrides
   virtual BOOL GetWorldPos( Vector &pos );
   virtual void PreTransform();
   virtual void Transform( const Matrix44 &matrix );
   virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual void DirtyDependentFaces();
   virtual SnipeObject *FindMirrorObject();
   virtual BOOL GetDrawColor( RGBAFloat &color );
   virtual void PushBounds( BoundingBox &box, CGLBaseView *pView=NULL );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
   virtual void Draw( CGLBaseView *pView );
   virtual void DrawSelected( CGLBaseView *pView );
   virtual void DrawMirrored( CGLBaseView *pView );
   virtual void DrawFocused( CGLBaseView *pView );
   virtual void DrawNearestFocused( CGLBaseView *pView, SnipeObject *pFocusObject );
};

#include "Poly.hpp"
