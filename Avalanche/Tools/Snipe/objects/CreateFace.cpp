#include "stdafx.h"
#include "Poly.h"
#include "Vertex.h"
#include "Edge.h"
#include "CreateFace.h"
#include "PolyModel.h"
#include "../GUI/GLBaseView.h"
#include "../GUI/BaseDoc.h"

DeleteFaceInfo::DeleteFaceInfo( Poly *pFace )
{
   m_pFace = pFace;
   m_pFace->Reference();

   m_pMirrorDeleteFaceInfo = NULL;
}

DeleteFaceInfo::~DeleteFaceInfo()
{
   m_pFace->DeReference();

   if (m_pMirrorDeleteFaceInfo)
      delete m_pMirrorDeleteFaceInfo;
}

void DeleteFaceInfo::Delete()
{
   BOOL bState = m_pFace->IsRemoveEdgesOnDisconnectIfNotUsed();
   m_pFace->SetRemoveEdgesOnDisconnectIfNotUsed( FALSE );
   m_pFace->RemoveFromList();
   m_pFace->SetRemoveEdgesOnDisconnectIfNotUsed( bState );

   static bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      if (m_pMirrorDeleteFaceInfo)
         m_pMirrorDeleteFaceInfo->Delete();
      bInMirror = FALSE;
   }
}

void DeleteFaceInfo::UnDelete()
{
   static bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      if (m_pMirrorDeleteFaceInfo)
         m_pMirrorDeleteFaceInfo->UnDelete();
      bInMirror = FALSE;
   }

   m_pFace->ReInsertIntoList();
}

/////////////////////////////////////////////////////////////////////////////////////

static BOOL IsValidPatch(Edge **edgearray, UINT nSides, String *strOutError=NULL)
{
   CMap<Poly *, Poly *, UINT, UINT> polymap;
   polymap.InitHashTable(17);

   for (UINT i=0; i<nSides; i++)
   {
      Edge *pEdge = edgearray[i];
      if (pEdge->GetFirstPoly())
         polymap.SetAt(pEdge->GetFirstPoly(), 0);
      if (pEdge->GetSecondPoly())
         polymap.SetAt(pEdge->GetSecondPoly(), 0);
   }
   
   // Lets see if there is Poly that is a subset of our new poly
   for (POSITION pos = polymap.GetStartPosition(); pos; )
   {
      UINT nDummy;
      Poly *pPoly;

      polymap.GetNextAssoc( pos, pPoly, nDummy );

      BOOL bPolyIsSubset = TRUE;
      for (UINT i=0; i<pPoly->GetNumSides(); i++)
      {
         Edge *pMatchEdge = pPoly->GetEdgeAt(i);
         BOOL bFound = FALSE;
         for (UINT j=0; j<nSides; j++)
         {
            if (edgearray[j] == pMatchEdge)
            {
               bFound = TRUE;
               break;
            }
         }

         if (!bFound)
         {
            bPolyIsSubset = FALSE;
            break;
         }
      }
      
      if (bPolyIsSubset)
      {
         if (strOutError)
         {
            *strOutError = "Can not create face, face(%1) is a subset";
            strOutError->Replace("%1", (String)(ULONG)pPoly->GetID());
         }
         return FALSE;
      }
   }

   return TRUE;
}

static UINT FindFaceBranch( Vertex *pMatchVertex, Vertex *pVertex, Edge *pStartUsingThisEdge, CArray<Edge *> &edgearray, CMap<Edge *, Edge*, UINT, UINT > &edgemap, UINT nSide, UINT nMaxSides )
{
   UINT nDummy;

   if (nSide >= nMaxSides)
      return 0;

   BOOL bFound = FALSE;

   for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
   {
      Edge *pEdge = pVertex->GetSurroundingEdgeAt(i);
      if (pEdge->GetFirstPoly() && pEdge->GetSecondPoly())
         continue;

      if (pStartUsingThisEdge && pEdge != pStartUsingThisEdge)
         continue;

      if (edgemap.Lookup( pEdge, nDummy ))
         continue;

      Vertex *pConnectedVertex = pEdge->GetConnectedVertex( pVertex );

      if (pConnectedVertex==pMatchVertex)
      {
         edgearray[nSide-1] = pEdge;
         if (IsValidPatch(edgearray.GetData(), nSide))
            return nSide;
         continue;
      }

      edgemap.SetAt(pEdge, nSide-1);

      Edge *pOldEdge = edgearray[nSide-1];
      edgearray[nSide-1] = pEdge;
      UINT nNumSides = FindFaceBranch( pMatchVertex, pConnectedVertex, NULL, edgearray, edgemap, nSide+1, nMaxSides);
      edgearray[nSide-1] = pOldEdge;
      edgemap.RemoveKey(pEdge);

      if (nNumSides && nNumSides < nMaxSides)
      {
         nMaxSides = nNumSides;
         edgearray[nSide-1] = pEdge;
         bFound = TRUE;
      }
   }

   if (bFound)
      return nMaxSides;

   return 0;
}

UINT Vertex::FindAndCreateFaces( DeleteFaceInfo **&pOutInfos, UINT nMaxSides/*=4*/, Edge *pOnlyWithThisEdge/*=NULL*/ )
{
   if (nMaxSides < 3)
      return 0;
   
   CArray<Edge *> edgearray;
   edgearray.SetSize(nMaxSides);

   CMap<Edge *, Edge *, UINT, UINT > edgemap;
   edgemap.InitHashTable(GetPrimeLargerThan( nMaxSides * 1.2f ));

   CList<DeleteFaceInfo *> infolist;

   while (TRUE)
   {
      ASSERT(edgemap.IsEmpty());

      UINT nNumSides = FindFaceBranch( this, this, pOnlyWithThisEdge, edgearray, edgemap, 1, nMaxSides+1);
      if (nNumSides)
      {
         PolyModel *pModel = (PolyModel *)GetOwner();
         DeleteFaceInfo *pInfo = pModel->CreateFace( nNumSides, edgearray.GetData(), FALSE );
         if (pInfo)
            infolist.AddTail(pInfo);
         else
         {
            ASSERT(FALSE);
            break;
         }
      }
      else
         break;
   }

   if (infolist.GetCount())
   {
      pOutInfos = SNEW DeleteFaceInfo *[infolist.GetCount()];
      ASSERT( pOutInfos );

      UINT nIndex=0;
      for (POSITION pos = infolist.GetHeadPosition(); pos; nIndex++)
         pOutInfos[nIndex] = infolist.GetNext(pos);
   }
   return infolist.GetSize();
}

DeleteFaceInfo *PolyModel::CreateFace( UINT nNumSides, Edge *pEdges[], BOOL bVerify/*=TRUE*/, String *strOutError/*=NULL*/ )
{
   if (nNumSides<3)
   {
      if (strOutError)
         *strOutError = "Must have at least 3 edges to create a face";
      return NULL;
   }

   CArray<Vertex *> vertexlist;
   vertexlist.SetSize(nNumSides);

   Vertex *pShared = pEdges[0]->GetSharedVertex(pEdges[1]);
   ASSERT(pShared);

   Vertex *pStart = pEdges[0]->GetConnectedVertex(pShared);
   Vertex *pVertex = pStart;
   for (UINT i=0; i<nNumSides; i++)
   {
      vertexlist[i]=pVertex;
      Edge *pEdge = pEdges[i];

      pVertex = pEdge->GetConnectedVertex( pVertex );
      if (!pVertex)
         return NULL;
   }
   if (!(pVertex==pStart))
   {
      if (strOutError)
         *strOutError = "Edges are not all connected together";
      return NULL;
   }

   if (bVerify)
   {
      if (!IsValidPatch(pEdges, nNumSides, strOutError))
         return NULL;
   }

   Poly *pPoly = AddPoly();
   pPoly->SetNumSides( nNumSides );

   // Figure normal alignment here
   BOOL bReverseWinding = FALSE;

   int nReverseCount = 0;
   int nForwardCount = 0;

   for (UINT i=0; i<nNumSides; i++)
   {
      Edge *pEdge = pEdges[i];
      Poly *pPoly = pEdge->GetPoly();
      if (pPoly)
      {
         // Lets do our winding opposite of pWindingPolys so our normals face same direction
         Vertex *pWindingV0 = pEdge->GetAlignedFirstVertex(pPoly);
         Vertex *pWindingV1 = pEdge->GetAlignedSecondVertex(pPoly);

         // we want our winding to go from pWindingV1 towards pWindingV2
         UINT nNextIndex = i+1;
         if (nNextIndex==nNumSides)
            nNextIndex=0;

         Edge *pNextEdge = pEdges[nNextIndex];
         Vertex *pSharedVertex = pEdge->GetSharedVertex(pNextEdge);

         if (pSharedVertex==pWindingV0)
            nForwardCount++;
         else // We need reversed
            nReverseCount++;
      }
   }

   if (!nForwardCount && !nReverseCount) // All Dangles
   {
      // Towards screen
      Vertex *pSharedVertex = pEdges[0]->GetSharedVertex(pEdges[1]);
      Vertex *pEdge1Other = pEdges[1]->GetConnectedVertex( pSharedVertex );
      Vertex *pEdge0Other = pEdges[0]->GetConnectedVertex( pSharedVertex );

      Vector v0;
      Vector v1;
      CGLBaseView *pView = GetMainInterface()->GetCurrentView();
      if (pView && pView->GetDocument()->GetObjectHead() == this)
      {
         v0 = pEdge0Other->GetViewPos(pView) - pSharedVertex->GetViewPos(pView);
         v1 = pEdge1Other->GetViewPos(pView) - pSharedVertex->GetViewPos(pView);
      }
      else
      {
         v0 = pEdge0Other->GetPos() - pSharedVertex->GetPos();
         v1 = pEdge1Other->GetPos() - pSharedVertex->GetPos();
      }

      Vector vcross = v1.Cross(v0);
      if (vcross.z < 0) // pointing away from screen
         bReverseWinding = TRUE;
   }
   else if (nReverseCount > nForwardCount)
      bReverseWinding = TRUE;

   for (UINT i=0; i<nNumSides; i++)
   {
      UINT nVertexIndex;
      UINT nEdgeIndex;

      if (bReverseWinding)
      {
         nVertexIndex = i==0 ? 0 : nNumSides-i;
         nEdgeIndex = nNumSides-i-1;
      }
      else
         nEdgeIndex = nVertexIndex = i;

      pPoly->SetVertexAt(nVertexIndex, vertexlist[i]);
      pPoly->SetEdgeAt(nEdgeIndex, pEdges[i]);
   }

   pPoly->FindAndCreateNormals();
   m_DefaultSurface.AddFace( pPoly );

   DeleteFaceInfo *pInfo = SNEW DeleteFaceInfo( pPoly );

   static bInMirror;
   if (!bInMirror)
   {
      bInMirror = TRUE;
      CArray<Edge *> mirroredges;
      mirroredges.SetSize(nNumSides);
      BOOL bValid = TRUE;

      for (UINT i=0; i<nNumSides; i++)
      {
         Edge *pEdge = pEdges[i];
         Edge *pMirrorEdge = (Edge *)pEdge->GetMirrorObject();
         if (!pMirrorEdge)
         {
            bValid = FALSE;
            break;
         }
         mirroredges[i] = pMirrorEdge;
      }

      if (bValid)
         pInfo->m_pMirrorDeleteFaceInfo = CreateFace(nNumSides, mirroredges.GetData(), bVerify);

      bInMirror = FALSE;
   }
   return pInfo;
}

UINT Edge::FindAndCreateFaces( DeleteFaceInfo **&pOutInfos, UINT nMaxSides/*=4*/ )
{
   return GetFirstVertex()->FindAndCreateFaces( pOutInfos, nMaxSides, this );
}
