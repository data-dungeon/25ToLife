#pragma once

#include "Component.h"
#include "../Properties/Property.h"
#include "RTVertex.h"

class Vertex;
class Poly;
class Edge;
class VertexNormal;
class PropertyVertex;
class HierObject;
class CGL3DView;
class TransformableObject;
class BezierKnot;
class TessPoly;
class PatchEdge;
class CGLModelView;
class Vertex;
class PropertyEdge;
class PropertyPoly;
class PropertySet;
class DeleteFaceInfo;
class VertexPeakInfo;

////////////////////////////////////////////////////////////////////////////////////////////
class SwappedVertexInPoly
{
public:
   Poly *m_pPoly;
   Vertex *m_pFromVertex;
   Vertex *m_pToVertex;

   SwappedVertexInPoly(Poly *pPoly, Vertex *pFromVertex, Vertex *pToVertex)
   {
      // No Need to reference these because the poly wasn't removed,
      // the FromVertex or ToVertex are referenced by the edge that was removed,
      // or by Vertex::AttachInfo
      m_pPoly = pPoly;
      m_pFromVertex = pFromVertex;
      m_pToVertex = pToVertex;
   }

   void SwapToFrom();
   void SwapFromTo();
};
///////////////////////////////////////////////////////////////////////////////////////////

// It is possible that after we swapped UV's in the PropertyEdges, that some PropertyEdges have the same 2 UVs, lets swap out the PropertyEdge 
class SwapPropertyEdge
{
public:
   PropertyPoly *m_pFromPropertyPoly;
   PropertyEdge *m_pFromPropertyEdge;
   PropertyEdge *m_pToPropertyEdge;

   SwapPropertyEdge( PropertyPoly *pPropertyPoly, PropertyEdge *pFrom, PropertyEdge *pTo);
   ~SwapPropertyEdge();

   void SwapToFrom();
   void SwapFromTo();
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CollapsedPropertyEdge
{
public:
   PropertyPoly *m_pFromPropertyPoly;
   PropertyPoly *m_pToPropertyPoly;
   PropertyEdge *m_pFromPropertyEdge;
   PropertyEdge *m_pToPropertyEdge;
   CList<PropertyPoly *> m_PropertyPolysThatSwappedPropertyVertices[2]; // 0 is FirstUV of m_pFromPropertyEdge, 1 is SecondUV
   CList<PropertyEdge *> m_PropertyEdgesThatSwappedPropertyVertices[2]; // 0 is FirstUV of m_pFromPropertyEdge, 1 is SecondUV
   CList<SwapPropertyEdge *> m_PropertyEdgesThatSwappedOutOfPropertyPoly; // It is possible that after we swapped UV's in the PropertyEdges, that some PropertyEdges have the same 2 UVs, lets swap out the PropertyEdge 
   union {
      ULONG m_nCollapsedPropertyEdgeFlags;
      struct {
         ULONG m_bAligned        : 1,
               m_bPropertyEdgesThatSwappedOutOfPropertyPolyHaveBeenFound : 1;
      };
   };

   CollapsedPropertyEdge( PropertyEdge *pFrom, PropertyEdge *pTo, BOOL bAligned );
   ~CollapsedPropertyEdge();

   void CollapseFromTo();
   void CollapseToFrom();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CollapsedEdge // Edges being merged together
{
public:
   Poly *m_pFromPoly;
   Edge *m_pFromEdge;
   Edge *m_pToEdge;
   BOOL  m_bAligned;
   VertexNormal *m_pFromVertexNormal[2]; // normals used by pFromPoly at each end of pFromEdge
   VertexNormal *m_pToVertexNormal[2]; // normals used by pFromPoly at each end of pFromEdge
   CList<Poly *> m_VertexNormalPolyList[2];// list of polys that used m_pFromVertexNorma[0] or m_pFromVertexNormal[1].  We need it to change there normals to maintain soft/hard edges
   CList<PropertyEdge *> m_PropertyEdgesThatChangedEdgeLists;
   CList<CollapsedPropertyEdge *> m_CollapsedPropertyEdges;

   CollapsedEdge( Edge *pFrom, Edge *pTo, BOOL bAligned, BOOL bCollapsePropertyEdges);
   ~CollapsedEdge();

   void CollapseFromTo();
   void CollapseToFrom();
};
////////////////////////////////////////////////////////////////////////////////////////////////

class SwappedPropertyVertexInPropertyEdge
{
public:
   PropertyEdge *m_pPropertyEdge;
   PropertyVertex *m_pFromPropertyVertex;
   PropertyVertex *m_pToPropertyVertex;

   SwappedPropertyVertexInPropertyEdge(PropertyEdge *pPropertyEdge, PropertyVertex *pFromPropertyVertex, PropertyVertex *pToPropertyVertex);
   ~SwappedPropertyVertexInPropertyEdge();

   void SwapToFrom();
   void SwapFromTo();
};

/////////////////////////////////////////////////////////////////////////////////////////////

class SwappedPropertyVertexInPropertyPoly
{
public:
   PropertyPoly *m_pPropertyPoly;
   PropertyVertex *m_pFromPropertyVertex;
   PropertyVertex *m_pToPropertyVertex;

   SwappedPropertyVertexInPropertyPoly(PropertyPoly *pPropertyPoly, PropertyVertex *pFromPropertyVertex, PropertyVertex *pToPropertyVertex)
   {
      m_pPropertyPoly = pPropertyPoly;
      m_pFromPropertyVertex = pFromPropertyVertex;// No need to reference because PropertyPolys don't reference UV's
      m_pToPropertyVertex = pToPropertyVertex; // No need to reference because PropertyPolys don't reference UV's
   }

   void SwapToFrom();
   void SwapFromTo();
};

////////////////////////////////////////////////////////////////////////////////////////////

class CollapseFace
{
public:
   Poly *m_pCollapsedPoly;
   Poly *m_pConnectedPoly;
   Edge *m_pFromEdge;
   Edge *m_pToEdge;
   Vertex *m_pFromSharedVertex; // can be different in Spline Models
   Vertex *m_pToSharedVertex;
   CMap<PropertySet *, PropertySet *, PropertyEdge *, PropertyEdge *> m_fromPropertyEdges;
   CMap<PropertySet *, PropertySet *, PropertyEdge *, PropertyEdge *> m_toPropertyEdges;

   CollapseFace()
   {
      m_pCollapsedPoly = NULL;
      m_pConnectedPoly = NULL;
      m_pFromEdge = NULL;
      m_pToEdge = NULL;
      m_pFromSharedVertex = NULL;
      m_pToSharedVertex = NULL;
      m_fromPropertyEdges.InitHashTable(17);
      m_toPropertyEdges.InitHashTable(17);
   }

   ~CollapseFace();
   void Fill( Edge *pFromEdge, Edge *pToEdge, Poly *pCollapsedPoly);
   void ReCollapse();
   void UnCollapse();
};

/////////////////////////////////////////////////////////////////////////////////////////////

#define VERTEXSIZE 4.0f

class Bone;
class PolyModel;
class PropertySet;
class Vertex;
typedef CMap<Vertex *, Vertex *, Vertex *, Vertex *> VertexMap;

class Vertex : public Component
{
public:
   class WeldInfo
   {
   public:
      Edge *m_pWeldedEdge; // Edge removed
      Vertex *m_pFromVertex; // Vertex Removed
      Vertex *m_pToVertex; // Vertex Removed
      VertexNormal *m_pFromVertexNormal[2]; // From Vertex Normal for each of m_pWeldedEdge's polys
      VertexNormal *m_pToVertexNormal[2]; // From Vertex Normal for each of m_pWeldedEdge's polys
      Poly *m_pPoly[2]; // Polys of m_pWeldedEdge
      int m_nVertexFaceRelativeIndex[2]; // face relative index of m_pFromVertex for m_pPoly[0] and m_pPoly[1]
      int m_nEdgeFaceRelativeIndex[2]; // face relative index of m_pWeldedEdge for m_pPoly[0] and m_pPoly[1]
      CollapseFace *m_pCollapseFaceInfo[2]; // Info for redo and Undo for collapsing a face if a 3 sides poly.

      CList<SwappedPropertyVertexInPropertyEdge *> m_PropertyVerticesInPropertyEdgeChanged;
      CList<SwappedPropertyVertexInPropertyPoly *> m_PropertyVerticesInPropertyPolyChanged;

      CMap<PropertySet *, PropertySet *, PropertyVertex *, PropertyVertex *> m_fromPropertyVertices; // UVs Removed
      CMap<PropertySet *, PropertySet *, PropertyEdge *, PropertyEdge *> m_weldedPropertyEdges; // PropertyEdges Removed

      CList<Edge *> m_edgeschanged; // Edges that need to swap m_pFromVertex with m_pToVertex
      CList<SwappedVertexInPoly *> m_vertexinpolyschanged; // Polys that need to swap m_pFromVertex with m_pToVertex
      CList<Poly *> m_normalinpolyschanged[2]; // Polys that need to swap m_pFromVertexNormal with m_pToVertexNormal, the array is for the normals from m_pPoly[0] or [1]

      CArray< Vertex * > m_oldvertices[2]; // Original Face Data
      CArray< VertexNormal * > m_oldvertexnormals[2];
      CArray< Edge * > m_oldedges[2];
      CArray< CArray< PropertyVertex * > > m_oldPropertyVertices[2];
      CArray< CArray< PropertyEdge * > > m_oldPropertyEdges[2];
      CList<CollapsedEdge *> m_CollapsedEdges;// edges being merged together

      SnipeObject *m_pFocusObject;
      SnipeObject *m_pNearFocusObject;

      union {
         ULONG m_nWeldInfoFlags;
         struct {
            ULONG m_bFromVertexSelected  : 1,
                  m_bToVertexSelected    : 1;
         };
      };

      WeldInfo *m_pMirrorInfo;

      WeldInfo();
      virtual ~WeldInfo();

      void AddCollapsedEdge( Edge *pFromEdge, Edge *pToEdge, BOOL bAligned )
      {
         CollapsedEdge *pCollapsedEdge = SNEW CollapsedEdge(pFromEdge, pToEdge, bAligned, TRUE);
         m_CollapsedEdges.AddTail( pCollapsedEdge );
      }

      void SetRedoSelection();
      void SetUndoSelection();

      virt_base virtual BOOL Fill( Vertex *pFromVertex, Vertex *pToVertex, String *strError=NULL );
      virt_base virtual void ReWeld();
      virt_base virtual void UnWeld();
   };

   /////////////////////////////////////////////////////////////////////////////////
   class AttachInfo
   {
   public:
      ///////////////////////////////////////////////////////////////////////////////// 
      Vertex *m_pFromVertex;
      Vertex *m_pToVertex;
      SnipeObject *m_pFocusObject;
      SnipeObject *m_pNearFocusObject;
      CList<Edge *> m_edgeschanged;// Edges that got a vertex changed
      CList<SwappedVertexInPoly *> m_SwappedVertexInPoly;// Polys that got a vertex changed
      CList<CollapsedEdge *> m_CollapsedEdges;// edges being merged together
      union {
         ULONG m_nAttachInfoFlags;
         struct {
            ULONG m_bFromVertexSelected  : 1,
                  m_bToVertexSelected    : 1;
         };
      };

      AttachInfo *m_pMirrorInfo;

      AttachInfo()
      {
         m_pFromVertex = NULL;
         m_pToVertex = NULL;
         m_pFocusObject = NULL;
         m_pNearFocusObject = NULL;
         m_nAttachInfoFlags = 0;
         m_pMirrorInfo = NULL;
      }

      virtual ~AttachInfo()
      {
         SetFromVertex(NULL);
         SetToVertex(NULL);
         while(!m_CollapsedEdges.IsEmpty())
            delete m_CollapsedEdges.RemoveHead();
         while(!m_SwappedVertexInPoly.IsEmpty())
            delete m_SwappedVertexInPoly.RemoveHead();

         if (m_pMirrorInfo)
            delete m_pMirrorInfo;
      }

      // Setup
      void SetFromVertex( Vertex *pVertex ) { AssignWithReference( (SnipeObject *&)m_pFromVertex, (SnipeObject *)pVertex ); }
      void SetToVertex( Vertex *pVertex ) { AssignWithReference( (SnipeObject *&)m_pToVertex, (SnipeObject *)pVertex ); }
      void SetFocusObject( SnipeObject *pSnipeObject ) { m_pFocusObject = pSnipeObject; }
      void SetNearFocusObject( SnipeObject *pSnipeObject ) { m_pNearFocusObject = pSnipeObject; }
      void AddCollapsedEdge( Edge *pFromEdge, Edge *pToEdge, BOOL bAligned )
      {
         CollapsedEdge *pCollapsedEdge = SNEW CollapsedEdge(pFromEdge, pToEdge, bAligned, FALSE);
         m_CollapsedEdges.AddTail( pCollapsedEdge );
      }
      void AddSwappedVertexInPoly( Poly *pPoly, Vertex *pFrom, Vertex *pTo )
      {
         SwappedVertexInPoly *pSwappedVertexInPoly = SNEW SwappedVertexInPoly(pPoly, pFrom, pTo);
         m_SwappedVertexInPoly.AddTail( pSwappedVertexInPoly );
      }

      void SetRedoSelection();
      void SetUndoSelection();

      // Operations
      virt_base virtual BOOL Fill( Vertex *pFromVertex, Vertex *pToVertex, String *strError );
      virt_base virtual void ReAttach();
      virt_base virtual void UnAttach();
   };
   ////////////////////////////////////////////////////////////////////////////

   class DeleteInfo : public Component::DeleteInfo
   {
   private:
      Edge *m_pNewEdge;
      CArray<PropertyEdge *> m_NewPropertyEdges[2]; // this are face specific

      Edge *m_pOldEdge0;
      Edge *m_pOldEdge1;
      Poly *m_pPoly0;
      Poly *m_pPoly1;

      CArray<PropertyEdge *> m_OldPropertyEdges0[2]; // correspond with Edge0, these are face specific
      CArray<PropertyEdge *> m_OldPropertyEdges1[2]; // correspond with Edge1, these are face specific

      UINT m_nOldVertexIndex[2]; // these are face specific
      VertexNormal *m_pOldVertexNormal[2]; // these are face specific
      CArray<PropertyVertex *> m_OldPropertyVertices[2]; // these are face specific
      BOOL m_bReversedEdge1;

   public:
      DeleteInfo( SnipeObject *pSnipeObject )
         : Component::DeleteInfo( pSnipeObject )
      {
         m_pNewEdge = NULL;
         m_pOldEdge0 = NULL;
         m_pOldEdge1 = NULL;
         m_pOldVertexNormal[0]=NULL;
         m_pOldVertexNormal[1]=NULL;
         m_bReversedEdge1 = FALSE;
         m_pPoly0 = NULL;
         m_pPoly1 = NULL;
      }
      ~DeleteInfo();

      void SetNewEdge( Edge *pEdge ) { AssignWithReference( (SnipeObject *&)m_pNewEdge, (SnipeObject *)pEdge );  }
      void SetNumNewPropertyEdges( UINT nPolyIndex, UINT nNumPropertyEdges )
      {
#ifdef _DEBUG
         for (int i=0; i<m_NewPropertyEdges[nPolyIndex].GetCount(); i++)
            ASSERT( m_NewPropertyEdges[nPolyIndex][i] == NULL ); // someone left a PropertyEdge pointer in my list, this is possibly bad since it never got Dereferenced.
#endif
         m_NewPropertyEdges[nPolyIndex].SetSize( nNumPropertyEdges );
      }
      void SetNewPropertyEdge( UINT nPolyIndex, UINT nPropertySetIndex, PropertyEdge *pPropertyEdge ) { AssignWithReference( (SnipeObject *&) m_NewPropertyEdges[nPolyIndex][nPropertySetIndex], (SnipeObject *)pPropertyEdge ); }

      void SetOldEdge0( Edge *pEdge ) { AssignWithReference( (SnipeObject *&)m_pOldEdge0, (SnipeObject *)pEdge ); }
      void SetOldEdge1( Edge *pEdge ) { AssignWithReference( (SnipeObject *&)m_pOldEdge1, (SnipeObject *)pEdge ); }

      void SetPoly0( Poly *pPoly ) { m_pPoly0 = pPoly; } // Don't need to assign with reference because the polys are referenced by the list and they are never removed
      void SetPoly1( Poly *pPoly ) { m_pPoly1 = pPoly; } // Don't need to assign with reference because the polys are referenced by the list and they are never removed

      void SetNumOldPropertyEdges0( UINT nPolyIndex, UINT nNumPropertyEdges )
      {
#ifdef _DEBUG
         for (int i=0; i<m_OldPropertyEdges0[nPolyIndex].GetCount(); i++)
            ASSERT( m_OldPropertyEdges0[nPolyIndex][i] == NULL ); // someone left a PropertyEdge pointer in my list, this is possibly bad since it never got Dereferenced.
#endif
         m_OldPropertyEdges0[nPolyIndex].SetSize( nNumPropertyEdges );
      }

      void SetNumOldPropertyEdges1( UINT nPolyIndex, UINT nNumPropertyEdges )
      {
#ifdef _DEBUG
         for (int i=0; i<m_OldPropertyEdges1[nPolyIndex].GetCount(); i++)
            ASSERT( m_OldPropertyEdges1[nPolyIndex][i] == NULL ); // someone left a PropertyEdge pointer in my list, this is possibly bad since it never got Dereferenced.
#endif
         m_OldPropertyEdges1[nPolyIndex].SetSize( nNumPropertyEdges );
      }
      void SetOldPropertyEdge0( UINT nPolyIndex, UINT nPropertySetIndex, PropertyEdge *pPropertyEdge ) { AssignWithReference( (SnipeObject *&)m_OldPropertyEdges0[nPolyIndex][nPropertySetIndex], (SnipeObject *)pPropertyEdge ); }
      void SetOldPropertyEdge1( UINT nPolyIndex, UINT nPropertySetIndex, PropertyEdge *pPropertyEdge ) { AssignWithReference( (SnipeObject *&)m_OldPropertyEdges1[nPolyIndex][nPropertySetIndex], (SnipeObject *)pPropertyEdge ); }
      void SetOldPolyVertexIndex( UINT nPolyIndex, UINT nVertexIndex ) { m_nOldVertexIndex[nPolyIndex] = nVertexIndex; }
      void SetOldPolyVertexNormal( UINT nPolyIndex, VertexNormal *pVertexNormal ) { AssignWithReference( (SnipeObject *&)m_pOldVertexNormal[nPolyIndex], (SnipeObject *)pVertexNormal); }
      void SetOldPolyPropertySetPropertyVertex( UINT nPolyIndex, UINT nPropertySetIndex, PropertyVertex *pPropertyVertex ) { AssignWithReference( (SnipeObject *&)m_OldPropertyVertices[nPolyIndex][nPropertySetIndex], (SnipeObject *)pPropertyVertex ); }
      void SetOldPolyNumPropertySets( UINT nPolyIndex , int nSize ) 
      {
#ifdef _DEBUG
         for (int i=0; i<m_OldPropertyVertices[nPolyIndex].GetCount(); i++)
            ASSERT( m_OldPropertyVertices[nPolyIndex][i] == NULL ); // someone left a UV pointer in my list, this is possibly bad since it never got Dereferenced.
#endif
         m_OldPropertyVertices[nPolyIndex].SetSize( nSize );
      }

      void SetReversedEdge1( BOOL bReversed ) { m_bReversedEdge1 = bReversed; }

      Edge *GetNewEdge() const { return m_pNewEdge; }
      UINT GetNumNewPropertyEdges( UINT nPolyIndex ) const { return m_NewPropertyEdges[nPolyIndex].GetCount(); }
      PropertyEdge *GetNewPropertyEdge( UINT nPolyIndex, UINT nPropertySetIndex ) const { return m_NewPropertyEdges[nPolyIndex][nPropertySetIndex]; }
      CArray<PropertyEdge *> *GetNewPropertyEdges( UINT nPolyIndex) { return &m_NewPropertyEdges[nPolyIndex]; }

      Edge *GetOldEdge0() const { return m_pOldEdge0; }
      Edge *GetOldEdge1() const { return m_pOldEdge1; }

      Poly *GetPoly0() const { return m_pPoly0; }
      Poly *GetPoly1() const { return m_pPoly1; }

      UINT GetNumOldPropertyEdges0( UINT nPolyIndex ) const { return m_OldPropertyEdges0[nPolyIndex].GetCount(); }
      UINT GetNumOldPropertyEdges1( UINT nPolyIndex ) const { return m_OldPropertyEdges1[nPolyIndex].GetCount(); }
      PropertyEdge *GetOldPropertyEdge0( UINT nPolyIndex , UINT nPropertySetIndex ) const { return m_OldPropertyEdges0[nPolyIndex][nPropertySetIndex]; }
      PropertyEdge *GetOldPropertyEdge1( UINT nPolyIndex , UINT nPropertySetIndex ) const { return m_OldPropertyEdges1[nPolyIndex][nPropertySetIndex]; }
      CArray<PropertyEdge *> *GetOldPropertyEdges0( UINT nPolyIndex) { return &m_OldPropertyEdges0[nPolyIndex]; }
      CArray<PropertyEdge *> *GetOldPropertyEdges1( UINT nPolyIndex) { return &m_OldPropertyEdges1[nPolyIndex]; }

      UINT GetOldPolyVertexIndex( UINT nPolyIndex ) const { return m_nOldVertexIndex[nPolyIndex]; }
      VertexNormal *GetOldPolyVertexNormal( UINT nPolyIndex ) const { return m_pOldVertexNormal[nPolyIndex]; }
      UINT GetOldPolyNumPropertySets( UINT nPolyIndex ) const { return m_OldPropertyVertices[nPolyIndex].GetCount(); }
      PropertyVertex *GetOldPolyPropertySetPropertyVertices( UINT nPolyIndex, UINT nPropertySetIndex ) const { return m_OldPropertyVertices[nPolyIndex][nPropertySetIndex]; }
      CArray<PropertyVertex *> *GetOldPolyPropertySet( UINT nPolyIndex ) { return &m_OldPropertyVertices[nPolyIndex]; }
      BOOL GetReversedEdge1() const { return m_bReversedEdge1; } 
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

   ////////////////////////////////////////////////////
   // Vertex
   ////////////////////////////////////////////////////
protected:
   static TranslateProperty::Info m_vPositionInfo;

   union {
      ULONG     m_nVertexFlags;
      struct {
      ULONG     // Vertex flags;
                //PatchVertex flags (added here for memory sake)
                m_bContrained : 1,
                m_bNPatchHook : 1,
                m_bSetDirtyCalled : 1;
      };
   };

public:
   TranslateProperty  m_vPosition;
   Vector    m_pos;
   RTVertex *m_pRTVertexHead;
  
   Poly **m_pPrimarySurroundingPolys;
   Edge **m_pSurroundingEdges;
   Bone **m_pBones;

   unsigned char m_nNumSurroundingFaces, m_nNumMaxSurroundingFaces;
   unsigned char m_nNumSurroundingEdges, m_nNumMaxSurroundingEdges;
   unsigned char m_nNumBones, m_nNumMaxBones;
   unsigned char m_nUnused1, m_nUnused2;

   DECLARE_COMPONENT_CREATE( Vertex, Component )

   // Constructors
   Vertex();
   Vertex( UINT nID );
   virtual ~Vertex();

   void Init();

   // Operators
   Vertex &operator = ( Vertex &other );
   BOOL operator == ( Vertex &other );
   BOOL operator != ( Vertex &other );

   // Operations
   PolyModel *GetModel() { return (PolyModel *)GetOwner(); }

   void SetPos( const Vector &pos, BOOL bDoMirror=TRUE, BOOL bDirty=TRUE );

   Vector GetWorldPos(TransformableObject *pTransformObject);
   Vector2 GetViewPos(TransformableObject *pTransformObject, CGL3DView *pView);
   void GetViewPos( CGLBaseView *pView, Vector &pos );
   Vector GetViewPos( CGLBaseView *pView );

   Edge *FindNearestEdge( CGLModelView *pView, const CPoint &pt );
   Poly *FindNearestFace( CGLModelView *pView, const CPoint &pt );

   AttachInfo *AttachTo( Vertex *pAttachToVertex, String *strError=NULL );
   WeldInfo *WeldTo( Vertex *pToVertex, String *strError=NULL);
   UINT FindAndCreateFaces( DeleteFaceInfo **&pOutInfos, UINT nMaxSides=4, Edge *pOnlyWithThisEdge=NULL );

   VertexPeakInfo *Peak( Edge *pEdge, BOOL bPeak );

   void AddSurroundingPoly( Poly *pPoly );
   UINT GetNumSurroundingPolys() const;
   Poly *GetSurroundingPolyAt( UINT nIndex ) const;
   UINT FindSurroundingPoly( Poly *pPoly ) const;
   void RemoveSurroundingPolyAt( const UINT nIndex );
   void RemoveSurroundingPoly(Poly *pPoly);

   UINT GetNumSurroundingEdges() const;
   Edge *GetSurroundingEdgeAt( UINT nIndex ) const;
   void RemoveSurroundingEdge( Edge *pEdge );

   RTVertex *GetRTVertexHead() const { return m_pRTVertexHead; }
   void SetRTVertexHead( RTVertex *pRTVertex ) { m_pRTVertexHead = pRTVertex; }
   void SetRTVertexPos( const Vector &vPos );

   void CalcNormal(VertexNormal *pVertexNormal);
   Vector CalcSoftNormal();

   VertexNormal *DoesShareVertexNormal(Poly *pPolyA,Poly *pPolyB);

   void GetEdgePair(Poly *pPoly, Edge **pEdge0, Edge **pEdge1, UINT *nIndex0=NULL, UINT *nIndex1=NULL);

   Edge *FindEdge(Vertex *pVertex, BOOL bUseAttached=TRUE) const;

   Edge *GetPrevEdgeClockwise(Edge *pEdge);
   Edge *GetNextEdgeClockwise(Edge *pEdge);

   Poly *GetPrevFaceClockwise(Poly *pPoly);
   Poly *GetNextFaceClockwise(Poly *pPoly);

   void SetMirrorVertex( Vertex *pVertex );

   // Bone
   UINT GetNumBones() const { return m_nNumBones; }
   Bone *GetBoneAt( UINT nIndex );
   void RemoveBoneAt( UINT nIndex );
   void AddBone( Bone *pBone );
   void RemoveBone( Bone *pBone );

   // Overridables
   virt_base virtual const Vector &GetPos();
   virt_base virtual void Draw();
   virt_base virtual void DrawPick(CGLModelView *pView);
   virt_base virtual BOOL GetTangents(Poly *pPoly,Vector &v0,Vector &v1) { return FALSE; }
   virt_base virtual void SetDirty();
   virt_base virtual void AddToVertexMap( VertexMap &vertexmap );
   virt_base virtual BOOL IsInVertexMap( VertexMap &vertexmap );
   virt_base virtual void AddSurroundingEdge( Edge *pEdge );
   virt_base virtual void RemoveSurroundingEdgeAt( UINT nIndex );
   virt_base virtual Vertex *GetTerminatingVertex( CList<Edge *> *pNotInEdgesList=NULL );
   virt_base virtual Vertex *GetChildVertex() { return NULL; }
   virt_base virtual BOOL IsAttachedConstrained() const { return FALSE; }
   virt_base virtual BOOL IsConstrained() const { return FALSE; }
   virt_base virtual void Compact();
   virt_base virtual BOOL IsPeaked( Edge *pEdge );
   virt_base virtual Vertex::WeldInfo *AllocWeldInfo();

   // Component overrides
   virtual BOOL IsUsed();
   virtual void Reconnect();
   virtual void Disconnect();

   // SnipeObject overrides
   virtual BOOL GetDrawColor( RGBAFloat &color );
   virtual BOOL GetWorldPos( Vector &pos );
   virtual void Transform( const Matrix44 &matrix );
   virtual void DirtyDependentFaces();
   virtual void DirtyDependentEdges(Edge *pNotThis=NULL);
   virtual SnipeObject *FindMirrorObject();
   virtual void PostConstruct();
   virtual void PreDestruct();
   virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
   virtual void Draw( CGLBaseView *pView );
   virtual void SetDrawColor(const RGBAFloat &color);
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

/////////////////////////////////////////////////////////////////////////////////////////

class ChangedNormals
{
public:
   Poly *m_pFace;
   UINT m_nIndex;
   VertexNormal *m_pOldVertexNormal;
   VertexNormal *m_pNewVertexNormal;

   ChangedNormals(Poly *pFace, UINT nIndex, VertexNormal *pOldVertexNormal, VertexNormal *pNewVertexNormal);
   ~ChangedNormals();

   BOOL Peak();
   BOOL Smooth();
};

class VertexPeakInfo
{
public:
   CList<ChangedNormals *> m_changedlist;
   Vertex *m_pVertex;
   Edge *m_pEdge;
   BOOL m_bChangedPeakFlagOnly;

   VertexPeakInfo *m_pMirrorInfo;

   VertexPeakInfo(Vertex *pVertex, Edge *pEdge) 
   {
      m_bChangedPeakFlagOnly = FALSE;
      m_pVertex = pVertex;
      m_pEdge = pEdge;
      m_pMirrorInfo = NULL;
   }
   ~VertexPeakInfo();

   void AddChangedNormals(ChangedNormals *pChangedNormals) { ASSERT(pChangedNormals); m_changedlist.AddTail(pChangedNormals); }
   UINT GetNumChangedVertexNormals() const { return m_changedlist.GetSize(); }

  virt_base virtual BOOL Peak();
  virt_base virtual BOOL Smooth();
};

#include "Vertex.hpp"
