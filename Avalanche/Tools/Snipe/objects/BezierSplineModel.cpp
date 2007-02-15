#include "stdafx.h"
//#include "BezierSplineModel.h"
//#include "Bone.h"
//#include "Selection.h"
//#include "Project.h"
//#include "GUI\GLModelView.h"
//#include "UVBezierSplineEdge.h"
//
//IMPLEMENT_SNIPEOBJECT_CREATE( BezierSplineModel )
//
//BezierSplineModel::BezierSplineModel()
//{
//   m_pConstrainedRTVertices = NULL;
//   m_nNumConstrainedRTVertices = 0;
//}
//
//BezierSplineModel::~BezierSplineModel()
//{
//   FreeRTVertices();
//}
//
//Vertex *BezierSplineModel::CreateVertex( UINT nID/*=-1*/ ) const
//{
//   return BezierKnot::CreateWithID( nID );
//}
//
//Edge *BezierSplineModel::CreateEdge( UINT nID/*=-1*/ ) const
//{
//   return BezierSplineEdge::CreateWithID( nID );
//}
//
//Poly *BezierSplineModel::CreatePoly( UINT nID/*=-1*/ ) const
//{
//   return BezierPatch::CreateWithID( nID );
//}
//
//UVEdge *BezierSplineModel::CreateUVEdge( UINT nID/*=-1*/ ) const
//{
//   return UVBezierSplineEdge::CreateWithID( nID );
//}
//
//static void SetNormal(BezierSplineModel *pModel, BezierKnot *pKnot, Poly *pPoly)
//{
//   UINT nIndex = pPoly->FindVertexFaceRelativeIndex(pKnot);
//   Edge *pEdge1, *pEdge2;
//   ASSERT(nIndex!=-1);
//
//   if (pPoly->GetVertexNormalAt(nIndex))
//      return; // Already Set
//
//   pKnot->GetEdgePair(pPoly, &pEdge1, &pEdge2);
//
//   VertexNormal *pNormal = NULL;
//
//   BezierKnot *pKnot1 = (BezierKnot *)pEdge1->GetVertexOnEdge(pKnot);
//   if (pKnot1->IsSmooth())
//   {
//      Poly *pConnectedPoly = pEdge2->GetConnectedPoly(pPoly); // Opposite
//      if (pConnectedPoly)
//         pNormal = pConnectedPoly->GetVertexNormalAt(pConnectedPoly->FindVertexFaceRelativeIndex(pKnot));
//   }
//
//   if (!pNormal)
//   {
//      BezierKnot *pKnot2 = (BezierKnot *)pEdge2->GetVertexOnEdge(pKnot);
//      if (pKnot2->IsSmooth())
//      {
//         Poly *pConnectedPoly = pEdge1->GetConnectedPoly(pPoly); // Opposite
//         if (pConnectedPoly)
//            pNormal = pConnectedPoly->GetVertexNormalAt(pConnectedPoly->FindVertexFaceRelativeIndex(pKnot));
//      }
//   }
//
//   if (!pNormal)
//      pNormal = pModel->AddVertexNormal();
//
//   ASSERT(pNormal);
//   pPoly->SetVertexNormalAt(nIndex, pNormal);
//}
//
//static void FillAllConnectedPolyNormals(BezierSplineModel *pModel, BezierKnot *pKnot, Poly *pPoly)
//{
//   VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(pPoly->FindVertexFaceRelativeIndex(pKnot));
//   if (pVertexNormal)
//      return; // Already Done
//
//   Edge *pEdge1, *pEdge2;
//   pKnot->GetEdgePair(pPoly, &pEdge1, &pEdge2);
//   
//   Poly *pCurrentPoly = pPoly;
//   Edge *pCurrentEdge = pEdge1;
//   Edge *pStartEdge = pEdge1;
//
//   BOOL bFirstDirection = TRUE;
//
//   while (TRUE)
//   {
//      SetNormal(pModel, pKnot, pCurrentPoly);
//
//      pCurrentPoly = pCurrentEdge->GetConnectedPoly(pCurrentPoly);
//      if (!pCurrentPoly && bFirstDirection)
//      {
//         bFirstDirection = FALSE;
//         pStartEdge = pEdge2;
//         pCurrentEdge = pEdge2;
//         pCurrentPoly = pCurrentEdge->GetConnectedPoly(pPoly);
//      }
//
//      if (!pCurrentPoly)
//         break;
//
//      Edge *pTempEdge1, *pTempEdge2;
//      pKnot->GetEdgePair(pCurrentPoly, &pTempEdge1, &pTempEdge2);
//
//      pCurrentEdge = (pTempEdge1==pCurrentEdge) ? pTempEdge2 : pTempEdge1;
//      if (pCurrentEdge == pStartEdge)
//         break;
//   }
//}
//
//void BezierSplineModel::BuildVertexNormals()
//{
//   if (!m_vertexnormallist.IsEmpty())
//      return;
//
//#ifdef _DEBUG
//   OutputWindowTimer timer( "Build Vertex Normals" );
//#endif // _DEBUG
//
//   for (POSITION pos=GetHeadVertexPosition(); pos; )
//   {
//      BezierKnot *pKnot = (BezierKnot *)GetNextVertex( pos );
//      if (!pKnot->IsHead())
//         continue;
//
//      Edge *pStartEdge = NULL;
//
//      for (UINT i=0; i<pKnot->GetNumSurroundingPolys();i++)
//      {
//         Poly *pPoly = pKnot->GetSurroundingPolyAt(i);
//         FillAllConnectedPolyNormals(this, pKnot, pPoly);
//      }
//   }
//
//#ifdef _DEBUG
//   for (POSITION pos=m_polylist.GetHeadPosition(); pos; )
//   {
//      Poly *pPoly = m_polylist.GetNext( pos );
//      for (UINT i=0; i<pPoly->GetNumSides(); i++)
//         ASSERT( pPoly->GetVertexNormalAt( i ));
//   }
//#endif // _DEBUG
//}
//
//Edge *BezierSplineModel::AddEdgeBetween( Vertex *pV0, Vertex *pV1, BOOL *pOutReversedV1/*=NULL*/, Vertex *pPrev/*=VERTEX_FINDCONTINUOUS*/, Vertex *pNext/*=VERTEX_FINDCONTINUOUS*/)
//{
//   BezierKnot *pKnot[2];
//   BezierKnot *pAttach[2];
//  
//   pKnot[0] = (BezierKnot *)pV0;
//   pKnot[1] = (BezierKnot *)pV1;
//
//   pAttach[0] = (BezierKnot *)pPrev;
//   pAttach[1] = (BezierKnot *)pNext;
//
//   for( int i=0; i<2; i++ )
//   {
//      BOOL bAddKnot = FALSE;
//      if ( pAttach[i] == VERTEX_FINDCONTINUOUS )
//      {
//         if (pKnot[i]->PreviousInSpline(TRUE) && pKnot[i]->NextInSpline(TRUE))
//         {
//            bAddKnot = TRUE;
//            for (BezierKnot *pCurrKnot=(BezierKnot*)pKnot[i]->DrillForTop(); pCurrKnot; pCurrKnot=pCurrKnot->GetChildKnot())
//            {
//               if (!pCurrKnot->PreviousInSpline(TRUE) || !pCurrKnot->NextInSpline(TRUE))
//               {
//                  pKnot[i] = pCurrKnot;
//                  bAddKnot = FALSE;
//                  break;
//               }
//            }
//         }
//      }
//      else if (pAttach[i] == VERTEX_TERMINATE)
//      {
//         if (pKnot[i]->PreviousInSpline(TRUE) || pKnot[i]->NextInSpline(TRUE))
//            bAddKnot = TRUE;
//      }
//      else // != VERTEX_TERMINATE has prev or next
//      {
//         if (!pAttach[i]->PreviousInSpline(TRUE) || !pAttach[i]->NextInSpline(TRUE))
//            pKnot[i] = pAttach[i];
//         else // They gave us a prev or next knot that can't be attached to, lets look for one
//         {
//            bAddKnot = TRUE;
//            for (BezierKnot *pCurrKnot=(BezierKnot*)pKnot[i]->DrillForTop(); pCurrKnot; pCurrKnot=pCurrKnot->GetChildKnot())
//            {
//               if (!pCurrKnot->PreviousInSpline(TRUE) || !pCurrKnot->NextInSpline(TRUE))
//               {
//                  pKnot[i] = pCurrKnot;
//                  bAddKnot = FALSE;
//                  break;
//               }
//            }
//         }
//      }
//
//      if (bAddKnot)
//      {
//         BezierKnot *pNewKnot = AddKnot();
//         pNewKnot->MoveToStack( pKnot[i] );
//         pKnot[i] = pNewKnot;
//      }
//   }
//
//   if ((pKnot[0]->NextInSpline() && pKnot[1]->NextInSpline())
//    || (pKnot[0]->PreviousInSpline() && pKnot[1]->PreviousInSpline())) // Need to reverse
//   {
//      pKnot[1]->ReverseSpline();
//      if (pOutReversedV1)
//         *pOutReversedV1 = TRUE;
//   }
//   else if (pOutReversedV1)
//      *pOutReversedV1 = FALSE;
//
//   if (!pKnot[0]->NextInSpline() && !pKnot[1]->PreviousInSpline())
//      return PatchModel::AddEdgeBetween( pKnot[0], pKnot[1] );
//
//   ASSERT(!pKnot[0]->PreviousInSpline() && !pKnot[1]->NextInSpline());
//   return PatchModel::AddEdgeBetween( pKnot[1], pKnot[0] );
//}
//
//void BezierSplineModel::OnEndObjectLoad()
//{
//#ifdef _DEBUG
//   OutputWindowTimer timer( "Build Splines" );
//#endif // _DEBUG
//
//   m_vertexlist.RebuildMap(); // otherwise FindKnot will rebuild it while we're traversing
//   for (POSITION pos=GetHeadVertexPosition(); pos; )
//   {
//      BezierKnot *pKnot = GetNextKnot( pos );
//
//      if (pKnot->m_pParentKnot)
//      {
//         UINT nParentID = (UINT)pKnot->m_pParentKnot;
//         pKnot->m_pParentKnot = NULL;
//         BezierKnot *pParentKnot = FindKnot( nParentID );
//         ASSERT(pParentKnot);
//         if (pParentKnot)
//            pKnot->MoveToStackWithChildren( pParentKnot );
//      }
//
//      if (pKnot->m_pNextInSpline)
//      {
//         UINT nNextID = (UINT)pKnot->m_pNextInSpline;
//         pKnot->m_pNextInSpline = FindKnot( nNextID );
//         ASSERT(pKnot->m_pNextInSpline);
//         if (pKnot->m_pNextInSpline)
//            pKnot->m_pNextInSpline->m_pPreviousInSpline = pKnot;
//      }
//   }
//
//   for (POSITION pos=GetHeadVertexPosition(); pos; )
//   {
//      BezierKnot *pModelKnot = (BezierKnot *)GetNextVertex( pos );
//      if (!pModelKnot->PreviousInSpline(FALSE))
//      {
//         for (BezierKnot *pKnot0=pModelKnot; pKnot0; pKnot0=pKnot0->NextInSpline(FALSE))
//         {
//            BezierKnot *pKnot1 = pKnot0->NextInSpline( TRUE );
//            if (pKnot1 && !pKnot0->IsAttached(pKnot1))
//               AddEdge( pKnot0, pKnot1 );
//         }
//      }
//      pModelKnot->OnConstrainedChanged();
//   }
//
//   PatchModel::OnEndObjectLoad();
//}
//
//void BezierSplineModel::ConvertFrom(HierObject *pHierObject)
//{
//   if (pHierObject->GetSOT() == SOT_PolyModel)
//      ConvertFromPolyModel((PolyModel*)pHierObject);
//}
//
//BezierKnot *AddBezierKnot(Vertex *pVertex,BezierSplineModel *pBezierSplineModel,CMap<Vertex*,Vertex*,BezierKnot*,BezierKnot*> &vertexmap)
//{
//   BezierKnot *pKnot = (BezierKnot*)pBezierSplineModel->AddVertex(pVertex->GetPos());
//   BezierKnot *pHeadKnot;
//   if (!vertexmap.Lookup(pVertex,pHeadKnot))
//   {
//      vertexmap.SetAt(pVertex,pKnot);
//   }
//   else
//   {
//      String error;
//      pKnot->MoveToStack(pHeadKnot);
//      vertexmap.SetAt(pVertex,pKnot);
//   }
//   return pKnot;
//}
//
//void AssignVerticesToBone(Bone *pBone,Bone *pOtherBone,CMap<Vertex*,Vertex*,BezierKnot*,BezierKnot*> &vertexmap)
//{
//   if ((!pBone)||(!pOtherBone))
//      return;
//
//   BezierKnot *pKnot;
//   if (pBone->GetSOT() == SOT_Bone)
//   {
//      pBone->RemoveAllVertices();
//      for (POSITION pos = pOtherBone->GetVertexHeadPosition(); pos;)
//      {
//         Vertex *pVertex = pOtherBone->GetNextVertex(pos);
//         if (vertexmap.Lookup(pVertex,pKnot))
//         {
//            pBone->AddVertex(pKnot);
//         }
//      }
//   }
//   for (int nChildIndex = 0; nChildIndex < pOtherBone->GetNumChildren(); nChildIndex++)
//   {
//      HierObject *pOtherChild = (HierObject*)pOtherBone->GetChildAt(nChildIndex);
//      HierObject *pChild = (HierObject*)pBone->GetChildAt(nChildIndex);
//      AssignVerticesToBone((Bone*)pChild,(Bone*)pOtherChild,vertexmap);
//   }
//}
//
//void BezierSplineModel::ConvertFromPolyModel(PolyModel *pPolyModel)
//{
//   float fAngleTolerance = 30.0f;
//
//   CMap<Edge*,Edge*,BezierSplineEdge*,BezierSplineEdge*> edgemap;
//   CMap<Vertex*,Vertex*,BezierKnot*,BezierKnot*> vertexmap;
//   BezierSplineEdge *pBezierEdge;
//   Edge *pEdge;
//   BOOL bEdgeLoop;
//
//   // build bezier edges
//   for ( POSITION pos = pPolyModel->GetHeadEdgePosition(); pos; )
//   {
//      pEdge = pPolyModel->GetNextEdge(pos);
//
//      if (edgemap.Lookup(pEdge,pBezierEdge))
//         continue;
//
//      // walk backwards
//      Edge *pCurrEdge = pEdge;
//      Edge *pLastEdge = NULL;
//      bEdgeLoop = FALSE;
//  
//      BezierKnot *pPrevKnot;
//      BezierKnot *pKnot,*pOtherKnot;
//      BezierKnot *pNextPrevKnot;
//  
//      while( TRUE )
//      {
//         if (edgemap.Lookup(pCurrEdge,pBezierEdge))
//            break;
//
//         Edge *pPrevEdge = pCurrEdge->GetContinuousEdge( pLastEdge, FALSE, fAngleTolerance );
// 
//         if (pPrevEdge == pEdge)
//            bEdgeLoop = TRUE;
//
//         if (pLastEdge)
//         {
//            if (!bEdgeLoop)
//            {
//               Vertex *pSharedVertex = pCurrEdge->GetSharedVertex(pLastEdge);
//               Vertex *pVertex = pCurrEdge->GetConnectedVertex(pSharedVertex);
//               pKnot = AddBezierKnot(pVertex,this,vertexmap);
//               pOtherKnot = pPrevKnot;
//            }
//            else
//            {
//               pKnot = pNextPrevKnot;
//               pOtherKnot = pPrevKnot;
//            }
//          }
//         else
//         {
//            Vertex *pVertex = pCurrEdge->GetFirstVertex();
//            pKnot = AddBezierKnot(pVertex,this,vertexmap);
//            Vertex *pOtherVertex = pCurrEdge->GetSecondVertex();
//            pOtherKnot = AddBezierKnot(pOtherVertex,this,vertexmap);
//            pPrevKnot = pOtherKnot;
//            pNextPrevKnot = pOtherKnot;
//         }
//
//         pBezierEdge = (BezierSplineEdge*)AddEdge(pKnot,pOtherKnot);
//  
//         pPrevKnot->InsertKnotBefore(pKnot);
//
//         pPrevKnot = pKnot;
//
//         edgemap.SetAt(pCurrEdge,pBezierEdge);
//
//          if (pPrevEdge==NULL)
//            break;
//
//         // test for edge loop
//         if (bEdgeLoop)
//            break;
//
//         pLastEdge = pCurrEdge;
//         pCurrEdge = pPrevEdge;
//      }
//
//      if (bEdgeLoop)
//         continue;
//
//      pLastEdge = NULL;
//      pPrevKnot = pNextPrevKnot;
//      pCurrEdge = pEdge;
//
//      // walk forwards
//      while( TRUE )
//      {
//         Edge *pNextEdge = pCurrEdge->GetContinuousEdge( pLastEdge, TRUE, fAngleTolerance );
//         if (pNextEdge==NULL)
//            break;
//
//         if (edgemap.Lookup(pNextEdge,pBezierEdge))
//            break;
//
//         Vertex *pSharedVertex = pNextEdge->GetSharedVertex(pCurrEdge);
//         Vertex *pOtherVertex = pNextEdge->GetConnectedVertex(pSharedVertex);
//         pOtherKnot = AddBezierKnot(pOtherVertex,this,vertexmap);
//   
//         pBezierEdge = (BezierSplineEdge*)AddEdge(pPrevKnot,pOtherKnot);
//
//         pOtherKnot->InsertKnotBefore(pPrevKnot);
//
//         pPrevKnot = pOtherKnot;
//
//         edgemap.SetAt(pNextEdge,pBezierEdge);  
//
//         pLastEdge = pCurrEdge;
//         pCurrEdge = pNextEdge;
//      }
//   }
//
//   CMap<VertexNormal*,VertexNormal*,VertexNormal*,VertexNormal*> vertexnormalmap;
//
//   // build patches
//   Poly *pPoly;
//   BezierKnot *pKnot;
//   for ( POSITION pos = pPolyModel->GetHeadFacePosition(); pos; )
//   {
//      pPoly = pPolyModel->GetNextFace(pos);
//
//      Patch *pPatch = AddPatch(pPoly->GetID());
//
//      pPatch->SetNumSides(pPoly->GetNumSides());
//
//      for (UINT i = 0; i < pPoly->GetNumSides(); i++)
//      {
//         Edge *pEdge = pPoly->GetEdgeAt(i);
//         if (!edgemap.Lookup(pEdge,pBezierEdge))
//         {
//            ASSERT(FALSE);
//            continue;
//         }
//         
//         Vertex *pVertex = pPoly->GetVertexAt(i);
//         if (!vertexmap.Lookup(pVertex,pKnot))
//         {
//            ASSERT(FALSE);
//            continue;
//         }
//
//         BezierKnot *pKnotFound;
//         if (pKnot->DrillForBezierKnot(pBezierEdge->GetFirstVertex()))
//         {
//            pKnotFound = pBezierEdge->GetFirstVertex(); 
//         }
//         else if (pKnot->DrillForBezierKnot(pBezierEdge->GetSecondVertex()))
//         {
//            pKnotFound = pBezierEdge->GetSecondVertex(); 
//         }
//         else
//         {
//            ASSERT(FALSE);
//            continue;
//         }
//         
//         pPatch->SetVertexAt(i,pKnotFound);
//
//         VertexNormal *pVertexNormal = pPoly->GetVertexNormalAt(i);
//
//         VertexNormal *pNewVertexNormal;
//         if (!vertexnormalmap.Lookup(pVertexNormal,pNewVertexNormal))
//         {
//            pNewVertexNormal = AddVertexNormal();
//            vertexnormalmap.SetAt(pVertexNormal,pNewVertexNormal);
//         }
//         pPatch->SetVertexNormalAt(i,pNewVertexNormal);
//
//         pPatch->SetEdgeAt(i,pBezierEdge);
//         ASSERT(pBezierEdge);
//      }
//   }
//
//   // copy surfaces and things
//   MemoryStream ms;
//
//   pPolyModel->MarkSaveDependents();
//   pPolyModel->SceneObject::Save( ms );
//  
//   for (HierObject *pParent = (HierObject *)pPolyModel->GetParent(); pParent; pParent = (HierObject *)pParent->GetParent())
//      pParent->m_bSaveDependent = FALSE;
//
//
//   ms.Seek( 0, BaseStream::ST_BEGINNING );
//   Load( ms );
//
//   GetMainInterface()->GetProject()->NotifyEndProjectLoad();
//
//   // fix vertex bone assignments
//   BoneContainer *pPolyModelBoneContainer = pPolyModel->GetBoneContainer();
//   BoneContainer *pBoneContainer = GetBoneContainer();
//   
//   AssignVerticesToBone((Bone*)pBoneContainer,(Bone*)pPolyModelBoneContainer,vertexmap);
//
//   SelectionContainer *pPolyModelSelectionContainer = pPolyModel->GetSelectionContainer();
//   SelectionContainer *pSelectionContainer = GetSelectionContainer();
//
//   MainInterface *pMainInterface = GetMainInterface();
//
//   // fix vertex selections
//   if ((pPolyModelSelectionContainer)&&(pSelectionContainer))
//   {
//      for (int nSelectionIndex = 0; nSelectionIndex < pPolyModelSelectionContainer->GetNumChildren(); nSelectionIndex++)
//      {
//         Selection *pOtherSelection = (Selection*)pPolyModelSelectionContainer->GetChildAt(nSelectionIndex);
//         Selection *pSelection = (Selection*)pSelectionContainer->GetChildAt(nSelectionIndex);
//         pSelection->RemoveAll();
//         StringList outerrorlist;
//         SnipeObjectList *pSnipeObjectList = pMainInterface->DescriptionToObjects(pOtherSelection->GetSelectionDescription(),outerrorlist);
//         for (POSITION pos = pSnipeObjectList->GetHeadPosition(); pos;)
//         {
//            SnipeObject *pSnipeObject = pSnipeObjectList->GetNext(pos);
//            if (pSnipeObject->GetSOT()==SOT_Vertex)
//               if (vertexmap.Lookup((Vertex*)pSnipeObject,pKnot))
//                  pSelection->AddObject(pKnot);
//         }
//         delete pSnipeObjectList;
//      }
//   }
//}
//
//void BezierSplineModel::FreeRTVertices()
//{
//   FreeConstrainedRTVertices();
//
//   PatchModel::FreeRTVertices();
//}
//
//void BezierSplineModel::FreeConstrainedRTVertices()
//{
//   if (m_pConstrainedRTVertices)
//   {
//      delete [] m_pConstrainedRTVertices;
//      m_pConstrainedRTVertices = NULL;
//      m_nNumConstrainedRTVertices = 0;
//   }
//}
//
//void BezierSplineModel::CreateRTVertices( BOOL bTessellated )
//{
//   ASSERT( IsCache() );
//
//   PatchModel::CreateRTVertices( bTessellated );
//
//   CreateConstrainedRTVertices();
//}
//
//void BezierSplineModel::CreateConstrainedRTVertices()
//{
//   ASSERT( IsCache() );
//   if (!m_pRTVertices)
//      return;
////   ASSERT(m_pRTVertices);
//
//   FreeConstrainedRTVertices();
//
//   for (POSITION pos=GetHeadVertexPosition(); pos; )
//   {
//      BezierKnot *pBezierKnot = GetNextKnot( pos );
//      if (pBezierKnot->IsConstrained() && pBezierKnot->IsDraw())
//         m_nNumConstrainedRTVertices++;
//   }
//
//   if (m_nNumConstrainedRTVertices)
//   {
//      m_pConstrainedRTVertices = SNEW UINT[ m_nNumConstrainedRTVertices ];
//
//      UINT nConstrainedRTVertexIndex = 0;
//      for (POSITION pos=GetHeadVertexPosition(); pos; )
//      {
//         BezierKnot *pBezierKnot = GetNextKnot( pos );
//         if (pBezierKnot->IsConstrained() && pBezierKnot->IsDraw())
//         {
//            RTVertex *pRTVertex = pBezierKnot->GetRTVertexHead();
//            UINT nIndex = pRTVertex - m_pRTVertices;
//            m_pConstrainedRTVertices[ nConstrainedRTVertexIndex++ ] = nIndex;
//         }
//      }
//   }
//}
//
//void BezierSplineModel::DrawRetainedMode( CGLModelView *pModelView )
//{
//   PatchModel::DrawRetainedMode( pModelView );
//
//   BezierSplineModel *pCache = GetRootCache();
//   if (pModelView->GetCurrentSelectionSOT() == SOT_Vertex && pCache->m_pConstrainedRTVertices)
//   {
//      glEnableClientState( GL_VERTEX_ARRAY );
//      glVertexPointer( 3, GL_FLOAT, sizeof(RTVertex), &pCache->m_pRTVertices->GetPos() );
//      glDisable( GL_LIGHTING );
//      glPointSize( VERTEXSIZE-2 );
//      glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
//      glDrawElements( GL_POINTS, pCache->m_nNumConstrainedRTVertices, GL_UNSIGNED_INT, pCache->m_pConstrainedRTVertices );
//      glPointSize( VERTEXSIZE );
//      glDisableClientState( GL_VERTEX_ARRAY );
//   }
//}
