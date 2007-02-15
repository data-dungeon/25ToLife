#include "stdafx.h"
#include "PatchModel.h"
#include "Interface\MainInterface.h"
#include "..\Utility\MathTools.h"
#include "GUI\GLModelView.h"
#include "UVPatchEdge.h"
#include "Project.h"

static TypeProperty::Info::Node f_subdoptioninfos[] = {
   TypeProperty::Info::Node( "No Force", "NoForce" ),
   TypeProperty::Info::Node( "On", "On" ),
   TypeProperty::Info::Node( "Off", "Off" ),
};

   StatisticProperty::Info PatchModel::m_nTessPolysStatInfo( "Tessellated Polys", "TessellatedPolys" );
   StatisticProperty::Info PatchModel::m_nQuadsStatInfo( "GL Quads", "GLQuads" );
Property::Info          PatchModel::m_categorySubdivisionInfo( "Subdivision", "Subdivision" );
   BoolProperty::Info      PatchModel::m_bFixedSubdivisionsInfo( "Fixed Tessellation", "Fixed Tessellation", TRUE );
   IntProperty::Info       PatchModel::m_nPatchSubdivisionsInfo( "Patch Subdivisions", "Patch Subdivisions", 1, 0, 4, PIF_NOUNDO );
   FloatProperty::Info     PatchModel::m_fPatchToleranceInfo( "Patch Tolerance", "Patch Tolerance", 0.01f, 0.0001f, 0.0f, 100.0f, FloatProperty::Info::FPT_PERCENT );
Property::Info          PatchModel::m_categoryContinuityInfo( "Continuity", "Continuity" );
   FloatProperty::Info     PatchModel::m_fContinuityToleranceInfo( "Continuity Tolerance", "Continuity Tolerance", 30.0f, 15.0f, 60.0f, 1.0f, FloatProperty::Info::FPT_ANGLE );

String GetNumTessPolysStat( void *pData )
{
   PatchModel *pModel = (PatchModel *)pData;
   return pModel->GetNumTessPolys();
}

String GetNumQuadsStat( void *pData )
{
   PatchModel *pModel = (PatchModel *)pData;

   int nNumPolys = 0;
   for (POSITION pos=pModel->GetHeadCompositedSurfacePosition(); pos; )
   {
      CompositedSurface *pCompositedSurface=pModel->GetNextCompositedSurface(pos);
      nNumPolys += pCompositedSurface->m_nNumRTQuads;
   }

   return nNumPolys;
}

IMPLEMENT_SNIPEOBJECT_CREATE( PatchModel )

PatchModel::PatchModel()
{
   m_pConstrainedRTVertices = NULL;
   m_nNumConstrainedRTVertices = 0;

   m_pDirtyPatchHead = NULL;
   m_pDirtyDangleHead = NULL;

    m_nTessPolysStat.Init( &m_nTessPolysStatInfo, &m_statistics, ::GetNumTessPolysStat, this );
    m_nQuadsStat.Init( &m_nQuadsStatInfo, &m_statistics, ::GetNumQuadsStat, this );
   m_categorySubdivision.Init( &m_categorySubdivisionInfo, this );
    m_bFixedSubdivisions.Init( &m_bFixedSubdivisionsInfo, &m_categorySubdivision );
    m_bFixedSubdivisions.SetVisible( TRUE );
    m_nPatchSubdivisions.Init( &m_nPatchSubdivisionsInfo, &m_categorySubdivision );
    m_fPatchTolerance.Init( &m_fPatchToleranceInfo, &m_categorySubdivision );
    m_fPatchTolerance.SetVisible( TRUE );
   m_categoryContinuity.Init( &m_categoryContinuityInfo, this );
    m_fContinuityTolerance.Init( &m_fContinuityToleranceInfo, &m_categoryContinuity );

   m_nNumTessPolys = 0;
}

PatchModel::~PatchModel()
{
   DirtyTessellationAllocation();
}

Vertex *PatchModel::CreateVertex( UINT nID/*=-1*/ ) const
{
   return PatchVertex::CreateWithID( nID );
}

Edge *PatchModel::CreateEdge( UINT nID/*=-1*/ ) const
{
   return PatchEdge::CreateWithID( nID );
}

Poly *PatchModel::CreatePoly( UINT nID/*=-1*/ ) const
{
   return Patch::CreateWithID( nID );
}

// SnipeObject overrides
BOOL PatchModel::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty==&m_nPatchSubdivisions)
         {
            DirtyDependentEdges();
            DirtyTessellationAllocation();
         }
         else if (pProperty==&m_bFixedSubdivisions)
         {
         }
         else if (pProperty==&m_fPatchTolerance)
         {
         }
         else if (pProperty==&m_fContinuityTolerance)
         {
            DirtyDependentEdges();
         }
         break;
      }
   }

   return PolyModel::OnObjectNotify( msg, wParam, lParam );
}

void PatchModel::DirtyTessellationAllocation()
{
   for (POSITION pos=m_edgelist.GetHeadPosition(); pos; )
   {
      PatchEdge *pEdge = (PatchEdge *)m_edgelist.GetNext(pos);
      pEdge->SetTessellationAllocationDirty(TRUE);
   }
   FreeRTVertices();
}

void PatchModel::ConvertFrom(HierObject *pHierObject)
{
   if (pHierObject->GetSOT() == SOT_PolyModel)
   {
      MemoryStream ms;
      pHierObject->MarkSaveDependents();
      pHierObject->Save( ms );

      for (HierObject *pParent = (HierObject *)pHierObject->GetParent(); pParent; pParent = (HierObject *)pParent->GetParent())
         pParent->m_bSaveDependent = FALSE;

      ms.Seek( 0, BaseStream::ST_BEGINNING );
      Load( ms );

      GetMainInterface()->GetProject()->NotifyEndProjectLoad();
      BuildContinuity();
   }
}

void PatchModel::BuildContinuity()
{
   for (POSITION pos = GetHeadEdgePosition(); pos; )
   {
      PatchEdge *pPatchEdge = GetNextEdge(pos);
      pPatchEdge->SetPrevContinuityDirty( TRUE );
      pPatchEdge->SetNextContinuityDirty( TRUE );
   }

   for (POSITION pos = GetHeadEdgePosition(); pos; )
   {
      PatchEdge *pPatchEdge = GetNextEdge(pos);
      pPatchEdge->BuildContinuity();
   }
}

void PatchModel::FreeConstrainedRTVertices()
{
   if (m_pConstrainedRTVertices)
   {
      delete [] m_pConstrainedRTVertices;
      m_pConstrainedRTVertices = NULL;
      m_nNumConstrainedRTVertices = 0;
   }
}

void PatchModel::CreateConstrainedRTVertices()
{
   ASSERT( IsCache() );
   if (!m_pRTVertices)
      return;

   FreeConstrainedRTVertices();

   for (POSITION pos=GetHeadVertexPosition(); pos; )
   {
      PatchVertex *pVertex = (PatchVertex *)GetNextVertex( pos );
      if (pVertex->IsConstrained() && pVertex->IsDraw())
         m_nNumConstrainedRTVertices++;
   }

   if (m_nNumConstrainedRTVertices)
   {
      m_pConstrainedRTVertices = SNEW UINT[ m_nNumConstrainedRTVertices ];

      UINT nConstrainedRTVertexIndex = 0;
      for (POSITION pos=GetHeadVertexPosition(); pos; )
      {
         PatchVertex *pVertex = (PatchVertex *)GetNextVertex( pos );
         if (pVertex->IsConstrained() && pVertex->IsDraw())
         {
            RTVertex *pRTVertex = pVertex->GetRTVertexHead();
            UINT nIndex = pRTVertex - m_pRTVertices;
            m_pConstrainedRTVertices[ nConstrainedRTVertexIndex++ ] = nIndex;
         }
      }
   }
}

void PatchModel::FreeRTVertices()
{
   ASSERT( IsCache() );

   FreeConstrainedRTVertices();

   if (m_pRTVertices)
   {
      for (POSITION pos=GetHeadFacePosition(); pos; )
      {
         Patch *pPatch = GetNextFace( pos );
         UINT nNumTessPolys = pPatch->GetNumTessPolys( FALSE );

         for (UINT nTessPoly=0; nTessPoly<nNumTessPolys; nTessPoly++)
         {
            TessPoly *pTessPoly = pPatch->GetTessPolyAt( nTessPoly );
            UINT nNumSides = pTessPoly->GetNumSides();
            for (UINT nSide=0; nSide<nNumSides; nSide++)
            {
               Vertex *pVertex = pTessPoly->GetVertexAt( nSide );
               pVertex->SetRTVertexHead( NULL );
            }
         }
      }

      // dangles
      for (POSITION pos=GetHeadEdgePosition(); pos; )
      {
         PatchEdge *pEdge = GetNextEdge( pos );
         if (!pEdge->GetFirstPoly())
         {
            TessEdgeVertices *pAlignedTessEdgeVertices = &pEdge->m_alignedTessedgeVertices;
            UINT nNumVertsInEdge = pAlignedTessEdgeVertices->GetNumVertices();
            if (nNumVertsInEdge < 3)
               continue;
            for (UINT nVert=1; nVert<nNumVertsInEdge-1; nVert++)
            {
               Vertex *pVertex = pAlignedTessEdgeVertices->GetVertexAt( nVert );
               pVertex->SetRTVertexHead( NULL );
            }
         }
      }
   }

   PolyModel::FreeRTVertices();
}

// Randy you may want to reevaluate this stuff. You are using a VertexData in a map. That may not be necessary anymore.
void PatchModel::CreateRTVertices( BOOL bTessellated )
{
   ASSERT( IsCache() );

   if (!bTessellated)
   {
      PolyModel::CreateRTVertices( bTessellated );
      return;
   }

   FreeRTVertices();

   UINT nNumFaces = GetNumPolys();
   UINT nVertsPerEdge = m_nPatchSubdivisions + 2;
   UINT nVertsPerPatch = nVertsPerEdge * nVertsPerEdge;
   UINT nMapSize = GetPrimeLargerThan( nNumFaces * nVertsPerPatch );
   CRTVertexMap rtvertexmap;
   rtvertexmap.InitHashTable( nMapSize );
   CMap<UINT, UINT, Vertex *, Vertex *> inttovertexmap;
   inttovertexmap.InitHashTable( nMapSize );
   RTVertexMapEntry rtvertex;
   UINT nRTVertexIndex = 0;

   for (POSITION pos=GetHeadFacePosition(); pos; )
   {
      Patch *pPatch = GetNextFace( pos );

      UVPatch *pUVPatch = (UVPatch *)pPatch->FindLastPropertyPoly(SOT_TexCoords);

      UINT nNumTessPolys = pPatch->GetNumTessPolys();

      for (UINT nTessPoly=0; nTessPoly<nNumTessPolys; nTessPoly++)
      {
         TessPoly *pTessPoly = pPatch->GetTessPolyAt( nTessPoly );
         TessUVPoly *pTessUVPoly = NULL;
         if (pUVPatch)
            pTessUVPoly = pUVPatch->GetTessUVPoly( nTessPoly );

         UINT nNumSides = pTessPoly->GetNumSides();
         for (UINT nFaceSide=0; nFaceSide<nNumSides; nFaceSide++)
         {
            Vertex *pVertex = pTessPoly->GetVertexAt( nFaceSide );
            rtvertex.SetVertex( pVertex );
            rtvertex.SetPos( pVertex->GetPos() );

            VertexNormal *pVertexNormal = pTessPoly->GetVertexNormalAt( nFaceSide );
            rtvertex.SetNormal( pVertexNormal->GetDir() );

            if (pTessUVPoly)
            {
               UV *pUV = pTessUVPoly->GetUVAt( nFaceSide );
               rtvertex.SetUV( pUV->GetPos() );
            }
            else
               rtvertex.SetUV( 0.0f );

            UINT nIndex;
            if (!rtvertexmap.Lookup( rtvertex, nIndex ))
            {
               nIndex = nRTVertexIndex++;
               rtvertexmap.SetAt( rtvertex, nIndex );
               inttovertexmap.SetAt( nIndex, pVertex );

               if (pVertex->GetID()!=-1 && pVertex->IsDraw())
               {
                  if (pVertex->IsLocked())
                     m_nNumLockedVertices++;
                  else
                     m_nNumVisibleVertices++;
               }
            }

            pTessPoly->SetRTVertexAt( nFaceSide, nIndex );
         }
      }
   }

   // add dangles
   for (POSITION pos=GetHeadEdgePosition(); pos; )
   {
      PatchEdge *pEdge = GetNextEdge( pos );

      if (!pEdge->GetFirstPoly())
      {
         UINT nNumVertsInEdge = pEdge->GetAlignedTessEdgeVertices()->GetNumVertices();
         for (UINT nVert=0; nVert<nNumVertsInEdge; nVert++)
         {
            Vertex *pVertex = pEdge->GetAlignedTessEdgeVertices()->GetVertexAt( nVert );
            if (pVertex->GetNumSurroundingPolys())
               continue;
            rtvertex.SetVertex( pVertex );
            rtvertex.SetPos( pVertex->GetPos() );
            rtvertex.SetNormal( 0.0f );
            rtvertex.SetUV( 0.0f );
            UINT nIndex;
            if (!rtvertexmap.Lookup( rtvertex, nIndex ))
            {
               nIndex = nRTVertexIndex++;
               rtvertexmap.SetAt( rtvertex, nIndex );
               inttovertexmap.SetAt( nIndex, pVertex );

               if (pVertex->GetID()!=-1 && pVertex->IsDraw())
               {
                  if (pVertex->IsLocked())
                     m_nNumLockedVertices++;
                  else
                     m_nNumVisibleVertices++;
               }
            }
         }
      }
   }

   // transfer from map to member array
   m_nNumRTVertices = rtvertexmap.GetCount();
   if (!m_nNumRTVertices)
      return;

   m_pVisibleVertices = SNEW UINT[ m_nNumVisibleVertices ];
   m_pLockedVertices = SNEW UINT[ m_nNumLockedVertices ];

   CMap<Vertex *, Vertex *, RTVertex *, RTVertex *> lastrtvertexmap;
   lastrtvertexmap.InitHashTable( nMapSize );
   m_pRTVertices = SNEW RTVertex[ m_nNumRTVertices ];

   // chain all RTVertices that are for the same actual vertex together
   UINT nVisibleVertex=0, nLockedVertex=0;
   for (const CRTVertexMap::CPair *pPair=rtvertexmap.PGetFirstAssoc(); pPair; pPair=rtvertexmap.PGetNextAssoc( pPair ))
   {
      UINT nIndex = pPair->value;
      RTVertex *pRTVertex = &m_pRTVertices[ nIndex ];
      *pRTVertex = pPair->key;

      Vertex *pVertex;
      inttovertexmap.Lookup( nIndex, pVertex );

      if (pVertex->GetID()!=-1 && pVertex->IsDraw())
      {
         if (pVertex->IsLocked())
            m_pLockedVertices[ nLockedVertex++ ] = nIndex;
         else
            m_pVisibleVertices[ nVisibleVertex++ ] = nIndex;
      }

      RTVertex *pPrevRTVertex;
      if (lastrtvertexmap.Lookup( pVertex, pPrevRTVertex ))
         pPrevRTVertex->SetNextWithSameVertex( pRTVertex );
      else
         pVertex->SetRTVertexHead( pRTVertex );
      lastrtvertexmap.SetAt( pVertex, pRTVertex );
   }

   // Edges
   for (POSITION pos=GetHeadEdgePosition(); pos; )
   {
      PatchEdge *pEdge = GetNextEdge( pos );
      if (!pEdge->IsDraw())
         continue;

      UINT nNumLinesInEdge = pEdge->GetAlignedTessEdgeVertices()->GetNumVertices() - 1;

      m_nNumRTEdges += nNumLinesInEdge;

      if (pEdge->IsBorderEdge())
         m_nNumBorderEdges += nNumLinesInEdge;
      else if (!pEdge->IsSoftEdge())
         m_nNumHardEdges += nNumLinesInEdge;
   }

   m_pRTEdges = SNEW RTEdge[ m_nNumRTEdges ];

   UINT nRTSoftEdgeIndex = 0;
   UINT nRTBorderEdgeIndex = m_nNumRTEdges - m_nNumBorderEdges;
   UINT nRTHardEdgeIndex = nRTBorderEdgeIndex - m_nNumHardEdges;
   for (POSITION pos=GetHeadEdgePosition(); pos; )
   {
      PatchEdge *pEdge = GetNextEdge( pos );
      UINT nRTEdgeIndex;
      if (!pEdge->IsDraw())
         continue;

      UINT nNumLinesInEdge = pEdge->GetAlignedTessEdgeVertices()->GetNumVertices() - 1;

      if (pEdge->IsBorderEdge())
      {
         nRTEdgeIndex = nRTBorderEdgeIndex;
         nRTBorderEdgeIndex += nNumLinesInEdge;
      }
      else if (!pEdge->IsSoftEdge())
      {
         nRTEdgeIndex = nRTHardEdgeIndex;
         nRTHardEdgeIndex += nNumLinesInEdge;
      }
      else
      {
         nRTEdgeIndex = nRTSoftEdgeIndex;
         nRTSoftEdgeIndex += nNumLinesInEdge;
      }

      RTEdge *pRTEdge = &m_pRTEdges[ nRTEdgeIndex ];

      for (UINT nVert=0; nVert<nNumLinesInEdge; nVert++, pRTEdge++)
      {
         Vertex *pVertex0 = pEdge->GetAlignedTessEdgeVertices()->GetVertexAt( nVert );
         RTVertex *pRTVertex0 = pVertex0->GetRTVertexHead();
         ASSERT( pRTVertex0 );
         //BOOL rValue = lastrtvertexmap.Lookup( pVertex0->GetVertexData(), pRTVertex0 );
         //ASSERT( rValue );

         Vertex *pVertex1 = pEdge->GetAlignedTessEdgeVertices()->GetVertexAt( nVert+1 );
         RTVertex *pRTVertex1 = pVertex1->GetRTVertexHead();
         ASSERT( pRTVertex1 );
         //RTVertex *pRTVertex1;
         //rValue = lastrtvertexmap.Lookup( pVertex1->GetVertexData(), pRTVertex1 );
         //ASSERT( rValue );

         UINT nIndex0 = pRTVertex0 - m_pRTVertices;
         UINT nIndex1 = pRTVertex1 - m_pRTVertices;
         pRTEdge->SetVertexIndices( nIndex0, nIndex1 );
      }
   }

   CreateConstrainedRTVertices();
}

void PatchModel::DrawRetainedMode( CGLModelView *pModelView )
{
   for (Patch *pPatch=m_pDirtyPatchHead; pPatch; pPatch=pPatch->GetNextDirtyPatch())
      pPatch->Tessellate();
   for (PatchEdge *pEdge=m_pDirtyDangleHead; pEdge; pEdge=pEdge->GetNextDirtyDangle())
      pEdge->Tessellate();

   PolyModel::DrawRetainedMode( pModelView );

   PatchModel *pCache = GetRootCache();
   if (pModelView->GetCurrentSelectionSOT() == SOT_Vertex && pCache->m_pConstrainedRTVertices)
   {
      glEnableClientState( GL_VERTEX_ARRAY );
      glVertexPointer( 3, GL_FLOAT, sizeof(RTVertex), &pCache->m_pRTVertices->GetPos() );
      glDisable( GL_LIGHTING );
      glPointSize( VERTEXSIZE-2 );
      glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
      glDrawElements( GL_POINTS, pCache->m_nNumConstrainedRTVertices, GL_UNSIGNED_INT, pCache->m_pConstrainedRTVertices );
      glPointSize( VERTEXSIZE );
      glDisableClientState( GL_VERTEX_ARRAY );
   }

#if 0 // draws tess verts
   if (pModelView->GetCurrentSelectionSOT() == SOT_Vertex && pCache->m_pRTVertices)
   {
      glEnableClientState( GL_VERTEX_ARRAY );
      glVertexPointer( 3, GL_FLOAT, sizeof(RTVertex), &pCache->m_pRTVertices->GetPos() );
      glDisable( GL_LIGHTING );
      glPointSize( VERTEXSIZE-2 );
      glColor4f( 1.0f, 0.2f, 0.2f, 0.5f );
      glDrawArrays( GL_POINTS, 0, pCache->m_nNumRTVertices );
      glPointSize( VERTEXSIZE );
      glDisableClientState( GL_VERTEX_ARRAY );
   }
#endif
}

Edge *PatchModel::AddEdgeBetween( Vertex *pV0, Vertex *pV1, Edge **pOutReversedEdge/*=NULL*/, Edge *pPrevEdge/*=EDGE_FINDCONTINUOUS*/, Edge *pNextEdge/*=EDGE_FINDCONTINUOUS*/)
{
   PatchEdge *pNewEdge = (PatchEdge *)PolyModel::AddEdgeBetween(pV0, pV1, pOutReversedEdge, pPrevEdge, pNextEdge);

   for (UINT i=0; i<2; i++)
   {
      PatchEdge *pContinuousEdge = (PatchEdge *)(i==0 ? pPrevEdge : pNextEdge);
      PatchVertex *pVertex = (PatchVertex *) (i==0 ? pV0 : pV1);

      if (pContinuousEdge == EDGE_TERMINATE)
         continue;
      else if (pContinuousEdge == EDGE_FINDCONTINUOUS)
         pContinuousEdge = (PatchEdge *)pNewEdge->Edge::GetContinuousEdge(pVertex, m_fContinuityTolerance);

      if (pContinuousEdge)
      {
         if (i==1 && (pNewEdge->GetPrevContVertex() == pContinuousEdge->GetPrevContVertex() || pNewEdge->GetNextContVertex() == pContinuousEdge->GetNextContVertex())) // V1, Test to see if it needs reversed
         {
            if (pOutReversedEdge)
               *pOutReversedEdge = pContinuousEdge;
            pContinuousEdge->ReverseSpline();
         }
         pContinuousEdge->SetContEdge( pVertex, pNewEdge );
      }
   }
   return pNewEdge;
}

BOOL PatchModel::ParseData( BaseStream &bs, const DataHeader &dataheader )
{
   BOOL bResult = PolyModel::ParseData( bs, dataheader );

   if (dataheader.m_strKey == "Edges")
   {
      m_edgelist.RebuildMap(); 
      // Convert all of the IDs that are stuffed in the next and prev edge pointers to actual pointers
      for (POSITION pos=GetHeadEdgePosition(); pos; )
      {
         PatchEdge *pEdge = GetNextEdge( pos );
         pEdge->m_pPrevContEdge = (PatchEdge *)m_edgelist.Find( (UINT)pEdge->m_pPrevContEdge );
         pEdge->m_pNextContEdge = (PatchEdge *)m_edgelist.Find( (UINT)pEdge->m_pNextContEdge );

         // transfer bias from next edge's v0 to this edge's v1 if our v1 is smooth and continuous
         if (!pEdge->IsSecondVertexPeaked())
         {
            Vertex *pV1 = pEdge->GetSecondVertex();
            PatchEdge *pV1ContEdge = (PatchEdge *)pEdge->GetContinuousEdge( pV1 );
            if (pV1ContEdge)
            {
               VertexHandle *pSecondVertexHandle = pEdge->GetSecondVertexHandle( FALSE );
               VertexHandle *pNextEdgeFirstVertexHandle = pV1ContEdge->GetFirstVertexHandle( FALSE );
               pSecondVertexHandle->m_fAlpha.StoreValue( pNextEdgeFirstVertexHandle->m_fAlpha, 0, FALSE );
               pSecondVertexHandle->m_fGamma.StoreValue( pNextEdgeFirstVertexHandle->m_fGamma, 0, FALSE );
               pSecondVertexHandle->m_fMagnitude.StoreValue( pNextEdgeFirstVertexHandle->m_fMagnitude, 0, FALSE );
            }
         }

         if (pEdge->IsFirstVertexConstrained())
            ((PatchVertex *)pEdge->GetFirstVertex())->SetConstrained( pEdge );
         if (pEdge->IsSecondVertexConstrained())
            ((PatchVertex *)pEdge->GetSecondVertex())->SetConstrained( pEdge );
      }
   }

   return bResult;
}
