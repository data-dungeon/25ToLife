#pragma once

#include "Component.h"
#include "Vertex.h"

class VertexNormal;
class Edge;
class Patch;
class UV;
class UVPoly;
class UVEdge;
class TexCoords;
class DeleteFaceInfo;
class PeakEdgeInfo;
class PropertyVertex;
class PropertyEdge;
class PropertySet;

class EdgePolyInfo
{
public:
   Poly *m_pPoly;
   UINT  m_nEdgeIndex : 8;

public:
   EdgePolyInfo()
   {
      m_pPoly = NULL;
      m_nEdgeIndex = 0;
   }
   virtual ~EdgePolyInfo() {}
};

class Edge : public Component
{
   friend class PolyModel;

protected:
   enum SaveFlags {
      SF_NONE          = 0,
      SF_V0PEAKED      = 1<<0,
      SF_V1PEAKED      = 1<<1,
      // PatchEdge bits
      SF_V0CONSTRAINED = 1<<2,
      SF_V1CONSTRAINED = 1<<3,
      SF_V0BIAS        = 1<<4,
      SF_V1BIAS        = 1<<5,
      SF_LOOP          = 1<<6
   };
   Vertex       *m_vertexarray[2];
   EdgePolyInfo *m_pEdgePolyInfo[2];
   PropertyEdge *m_pHeadPropertyEdge;

   union {
      ULONG m_nEdgeFlags;
      struct {
         ULONG m_bSoft                    : 1, // for drawing only
               m_bSoftDirty               : 1,
               m_bManifold                : 1,
               m_bV0Peaked                : 1,
               m_bV1Peaked                : 1,
         /////////////////////////////////// PatchEdge bits, here for memory sake
               m_bV0Constrained           : 1,
               m_bV1Constrained           : 1,
               m_bTangentsDirty           : 1,
               m_bTessellationDirty       : 1,
               m_bPropertyEdgeListDirty   : 1,
               m_bPrevContinuityDirty     : 1, // temp bits used during BuildContinuity
               m_bNextContinuityDirty     : 1, // ""
               m_bContinuityLoop          : 1;
      };
   };

public:
   /////////////////////////////////////////////////////////////////////////////////

   class DeleteInfo : public Component::DeleteInfo
   {
   private:
      Poly *m_pNewPoly;
      Poly *m_pOldPoly0;
      Poly *m_pOldPoly1;
      Edge *m_pReversedEdge;

      union {
         ULONG    m_edgedetachinfoflags;
         struct {
            ULONG m_bReversedWindingPoly0 : 1;
         };
      };

   public:
      Vertex::DeleteInfo *m_pVertexDeleteInfo0; // These are used in the case where you delete and edge but it doesn't merge two polys together
      Vertex::DeleteInfo *m_pVertexDeleteInfo1;

      DeleteInfo( SnipeObject *pSnipeObject )
         : Component::DeleteInfo( pSnipeObject )
      {
         m_pVertexDeleteInfo0 = NULL;
         m_pVertexDeleteInfo1 = NULL;
         m_pNewPoly = NULL;
         m_pOldPoly0 = NULL;
         m_pOldPoly1 = NULL;
         m_pReversedEdge = NULL;
         m_edgedetachinfoflags = 0;
      }
      ~DeleteInfo();

      void SetNewPoly( Poly *pPoly ) { AssignWithReference( (SnipeObject *&)m_pNewPoly, (SnipeObject *)pPoly ); }
      void SetOldPoly0( Poly *pPoly ) { AssignWithReference( (SnipeObject *&)m_pOldPoly0, (SnipeObject *)pPoly ); }
      void SetOldPoly1( Poly *pPoly ) { AssignWithReference( (SnipeObject *&)m_pOldPoly1, (SnipeObject *)pPoly ); }
      void SetReversedEdge( Edge *pEdge ) { m_pReversedEdge = pEdge; }
      void SetReversedWindingPoly0( BOOL bReversed ) { m_bReversedWindingPoly0 = bReversed; }

      Poly *GetNewPoly() { return m_pNewPoly; }
      Poly *GetOldPoly0() { return m_pOldPoly0; }
      Poly *GetOldPoly1() { return m_pOldPoly1; }
      Edge *GetReversedEdge() { return m_pReversedEdge; }
      BOOL GetReversedWindingPoly0() { return m_bReversedWindingPoly0; }
   };
   virtual SnipeObject::DeleteInfo *AllocDeleteInfo() { return SNEW DeleteInfo(this); }

   /////////////////////////////////////////////////////////////////////////////////
   DECLARE_COMPONENT_CREATE(Edge, Component)

            Edge();
            Edge( UINT nID );
	virtual ~Edge();

   void Init();
   Edge &operator = ( const Edge &other );

   Vertex *GetFirstVertex() const { return m_vertexarray[0]; }
   Vertex *GetSecondVertex() const { return m_vertexarray[1]; }
   Vertex *GetVertexOnEdge( Vertex *pVertex );

   BOOL IsFirstVertexPeaked() const { return m_bV0Peaked; }
   BOOL IsSecondVertexPeaked() const { return m_bV1Peaked; }
   BOOL IsVertexPeaked(Vertex *pVertex) const;

   Vertex *GetConnectedVertex( Vertex *pNotThis ) const;
   void RemoveVertex( Vertex *pVertex );
   Vertex *GetSharedVertex( Edge *pEdge ) const;

   UVEdge *FindUVEdge( UV *pUV0, UV *pUV1);
   UVEdge *FindUVEdge( TexCoords *pMatchTexCoords, UV *pMatchUV = NULL  );

   static Edge *FindEdge( const Vertex *pVertex0, const Vertex *pVertex1 );
   static Edge *FindEdgeFromFaces( Vertex *pVertex0, Vertex *pVertex1 );

   Poly *GetFirstPoly();
   void SetFirstPoly(Poly *pPoly,UINT nEdgeIndex);
   Poly *GetSecondPoly();
   void SetSecondPoly(Poly *pPoly,UINT nEdgeIndex);
   Poly *GetPoly();
   Poly *GetConnectedPoly(Poly *pNotThis);
   BOOL IsAligned(Poly *pPoly);
   EdgePolyInfo *GetEdgePolyInfo(Poly *pPoly);
   BOOL HasPoly(Poly *pPoly);

   BOOL IsSoftEdge();

   BOOL IsBorderEdge() const;
   BOOL IsDangle() const;

   BOOL IsInEdge(Vertex *pVertex) { return GetFirstVertex()==pVertex || GetSecondVertex()==pVertex; }

   BYTE GetFaceRelativeIndex(Poly *pPoly);//returns -1 if no poly is not found
   BOOL SetFaceRelativeIndex(Poly *pPoly,BYTE nIndex); 

   void GetVector(Vector &vect);
   BOOL GetAngle(Edge *pNextEdge, float &angle);
   float CalculateDot( Edge *pEdge );
   Edge *GetContinuousEdgeByDot( Vertex *pVertex, float fMinDot, Edge *pNoCallBackEdge=NULL );

   UINT FindAndCreateFaces( DeleteFaceInfo **&pOutInfos, UINT nMaxSides/*=4*/ );
   Edge *GetContinuousEdge( Edge *pPrev, BOOL bStartOtherDirectionIfNUll=FALSE, float fAngleTolerance=50.0f );
   Edge *GetOppositeEdge( Poly *&pPoly, BOOL *bOutAligned=NULL ); // Whether the opposite edge is running V0 - V1 in the same direction

   UINT LoadGuts( BaseStream &bs, UINT &nFlags );

   // flags
   ULONG GetEdgeFlags() const { return m_nEdgeFlags; }
   void SetEdgeFlags(ULONG nFlags) { m_nEdgeFlags = nFlags; }

   void SetSoftEdgeDirtyFlag(BOOL bFlag) { m_bSoftDirty = bFlag; }

   BOOL IsUVBorderEdge();

   Vertex *GetAlignedFirstVertex(Poly *pPoly);
   Vertex *GetAlignedSecondVertex(Poly *pPoly);

   void SetManifoldFlag(BOOL bFlag) { m_bManifold = bFlag; }
   BOOL IsManifold() { return m_bManifold; }

   // property edge
   void SetPropertyEdgeListDirty( BOOL bDirty ) { m_bPropertyEdgeListDirty = bDirty; }
   BOOL IsPropertyEdgeListDirty() { return m_bPropertyEdgeListDirty; }
   PropertyEdge *FindPropertyEdge( PropertyVertex *pPropertyVertex0, PropertyVertex *pPropertyVertex1);
   PropertyEdge *FindPropertyEdge( PropertySet *pMatchPropertySet, PropertyVertex *pMatchPropertyVertex = NULL  );
   void AddPropertyEdge( PropertyEdge *pPropertyEdge );
   BOOL RemovePropertyEdge( PropertyEdge *pMatchPropertyEdge );
   PropertyEdge *GetHeadPropertyEdge() { return m_pHeadPropertyEdge; }
   UINT CountNumPropertyEdges();

// Overridables
   virt_base virtual void SetFirstVertex(Vertex *pVertex);
   virt_base virtual void SetSecondVertex(Vertex *pVertex);
   virt_base virtual BOOL AddPoly(Poly *pPoly,UINT nEdgeIndex);
   virt_base virtual void RemovePoly(Poly *pPoly, BOOL bRemoveIfNotUsed=TRUE);
   virt_base virtual void Draw( BOOL bDrawTess );
   virt_base virtual void DrawPick( CGLModelView *pView, BOOL bDrawTess );
   virt_base virtual void DrawPickNearest( Vertex *pVertex);
   virt_base virtual void DrawPickNearest( Poly *pVertex);
   virt_base virtual EdgePolyInfo *CreateNewEdgePolyInfo() { return SNEW EdgePolyInfo; }
   virt_base virtual Edge *GetContinuousEdge( Vertex *pVertex, float fAngleTolerance=50.0f );
   virt_base virtual void SetDirty();
   virt_base virtual Vector GetPosAlongEdge( float fPosition, Poly *pPoly = NULL );
   virt_base virtual Vertex::DeleteInfo *SplitEdge( float fPosition=0.5f, Edge **pOutEdge0=NULL, Edge **pOutEdge1=NULL  );
   virt_base virtual void FindClosestIntersection( CGLBaseView *pView, const CPoint &point, float &fOutPos );
   virt_base virtual void ReverseSpline() {}
   virt_base virtual void GetDisplayIDText(CGLModelView *pView, String &strText) { strText = (int)GetID(); }
   virt_base virtual void SetFirstVertexPeakedFlag(BOOL bPeaked) { m_bV0Peaked = bPeaked; } // Don't call directly, call Vertex::Peak
   virt_base virtual void SetSecondVertexPeakedFlag(BOOL bPeaked) { m_bV1Peaked = bPeaked; } // Don't call directly, call Vertex::Peak
   virt_base virtual UINT GetSaveFlags();
   virt_base virtual void ApplySaveFlags( UINT nFlags );

// Component overrides
   virtual BOOL IsUsed();
   virtual void Reconnect();
   virtual void Disconnect();

// SnipeObject overrides
   virtual BOOL GetWorldPos( Vector &pos );
   virtual void PreTransform();
   virtual void Transform( const Matrix44 &matrix );
   virtual void DirtyDependentFaces();
   virtual SnipeObject *FindMirrorObject();
   virtual void FreeDeleteInfo( SnipeObject::DeleteInfo *pDeleteInfo ) { delete (DeleteInfo *)pDeleteInfo; }// DeleteInfo does not have a v-table, this must be here to get to the right destructor
   virtual BOOL Delete( SnipeObject::DeleteInfo *&pDeleteInfo, String &strOutError );
   virtual BOOL UnDelete( SnipeObject::DeleteInfo *pDeleteInfo );
   virtual void PushBounds( BoundingBox &box, CGLBaseView *pView=NULL );
   virtual catch_msg void Save( BaseStream &bs, BOOL bWriteHeader=FALSE ); // Always call BaseClass with FALSE!!!!!
   virtual catch_msg BOOL Load( BaseStream &bs );
   virtual BOOL GetDrawColor( RGBAFloat &color );
   virtual void Draw( CGLBaseView *pView );
   virtual void DrawSelected( CGLBaseView *pView );
   virtual void DrawMirrored( CGLBaseView *pView );
   virtual void DrawFocused( CGLBaseView *pView );
   virtual void DrawNearestFocused( CGLBaseView *pView, SnipeObject *pFocusObject );
};

#include "Edge.hpp"
