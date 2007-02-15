//

#include "stdafx.h"
#include "AssignVertexToBoneManipulator.h"
#include "../resource.h"
#include "HCGLModelView.h"
#include "HHitArray.h"
#include "HCommandPluginManager.h"
#include "MouseOn.h"
#include "CommandUIAccel.h"
#include "MemoryStream.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HCBaseDoc.h"
#include "HCModelDoc.h"
#include "HPolyModel.h"
#include "HBone.h"
#include "RGBColor.h"
#include "HColorProperty.h"
#include "HSnipeObjectList.h"
#include "HPatchVertex.h"
#include "HPoly.h"


// Plugin //////////////////////////////////////////
AssignVertexToBoneManipulatorPlugin::AssignVertexToBoneManipulatorPlugin()
{
   m_pAssignVertexToBoneCommandPlugin = NULL;
}

ManipulatorInstance *AssignVertexToBoneManipulatorPlugin::CreateInstance()
{
   return SNEW AssignVertexToBoneManipulator( this );
}

const char *AssignVertexToBoneManipulatorPlugin::GetTitleName() const { return "AssignVertexToBone"; }

UINT AssignVertexToBoneManipulatorPlugin::GetBitmapID() const
{
   return IDB_ASSIGNVERTEX;
}

/*
static CommandUIAccel f_pAccel [] = 
{
   CommandUIAccel(FVIRTKEY, 'b')
};

void AssignVertexToBoneManipulatorPlugin::GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const
{
   nCount = sizeof(f_pAccel)/sizeof(CommandUIAccel);
   pAccel = f_pAccel;
}
*/

void AssignVertexToBoneManipulatorPlugin::Init(HCommandPluginManager *pCommandManager)
{
   SelectManipulatorPlugin::Init(pCommandManager);
   // Collect dependencies here
   m_pAssignVertexToBoneCommandPlugin = pCommandManager->FindCommandByCLIName("SNIPEBASICCOMMANDS", "AssignVertexToBone");
   if (!m_pAssignVertexToBoneCommandPlugin)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : AssignVertexToBone ).");
}


BOOL AssignVertexToBoneManipulatorPlugin::SupportSOT( SnipeObjectType nType)
{
   return (nType == SOT_Bone);
}


// Instance ////////////////////////////////////////
AssignVertexToBoneManipulator::AssignVertexToBoneManipulator( AssignVertexToBoneManipulatorPlugin *pPluginHelper) :
 SelectManipulator( pPluginHelper )
{
   m_bValidateSelection = FALSE;
   m_pBone = NULL;
}

AssignVertexToBoneManipulator::~AssignVertexToBoneManipulator()
{
}

void AssignVertexToBoneManipulator::DoPick( HCGLBaseView *pView, HPickHitArray *pPickHitArray, const CRect &rcRect, BOOL *bOutSinglePick/*=NULL*/ )
{
   BOOL bSinglePick = FALSE;
   if ((rcRect.Width() == 0)&&(rcRect.Height() == 0))
      bSinglePick = TRUE;

   if (bSinglePick)
   {
      pView->DoPick(rcRect,pPickHitArray,bOutSinglePick,SOT_Bone);
      if (pPickHitArray->GetNumHits())
      {
         return;
      }
   }

   if (bOutSinglePick)
      *bOutSinglePick = FALSE;

   pView->DoPick(rcRect, pPickHitArray, NULL, SOT_Vertex);
}

BOOL AssignVertexToBoneManipulator::OnSelect( HCGLBaseView *pView, HSnipeObjectList *pSelectList, BOOL bTemp, BOOL *bSingleSelect/*=NULL*/, HSnipeObject *pFocus/*=NULL*/ )
{
   UINT nNumHits = pSelectList->GetSize();
   HSnipeObject *pSnipeObject = NULL;
   if (nNumHits>0)
      pSnipeObject = pSelectList->GetHead();
  
   if (pSnipeObject && pSnipeObject->GetSOT() == SOT_Bone)
      return SelectManipulator::OnSelect(pView, pSelectList, bTemp,bSingleSelect);    

   if (nNumHits==0)
      return FALSE;

   HSelectionList *pSelectionList = pView->GetDocument()->GetSelectionList();
   if (pSelectionList->GetCount()==0)
      return FALSE;

   m_pBone = (HBone*)pSelectionList->GetHead();

   // count number of vertices hit
   UINT nCount = 0;
   for (POSITION pos = pSelectList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = pSelectList->GetNext(pos);
      if (pSnipeObject && pSnipeObject->IsKindOf(SOT_Vertex))
         nCount++;
   }

   if (nCount==0)
      return FALSE;

   m_vertexarray.RemoveAll();
   m_vertexarray.SetSize(nCount);

   nCount = 0;
   for (POSITION pos = pSelectList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = pSelectList->GetNext(pos);
      if ( pSnipeObject && pSnipeObject->IsKindOf(SOT_Vertex))
      {
         HVertex *pVertex = (HVertex*)pSnipeObject;
         m_vertexarray.SetAt(nCount++, pVertex);
      }
   }

   if (m_pBone && nCount>0 )
      m_bValidateSelection = TRUE;

   return TRUE;
}

void AssignVertexToBoneManipulator::PostDraw(HCGLBaseView *pView)
{
   SelectManipulator::PostDraw(pView);

   if (pView->GetDocument()->GetDocType() != SDT_CModelDoc)
      return;

   HCModelDoc *pModelDoc = (HCModelDoc*)pView->GetDocument();

   glDisable(GL_LIGHTING);

   HHierObject *pHierObject = pModelDoc->GetObjectHead();

   HSelectionList *pSelectionList = pModelDoc->GetSelectionList();

   DrawVertices(pView,pHierObject,pSelectionList);
}

void AssignVertexToBoneManipulator::DrawVertices(HCGLBaseView *pView,HHierObject *pHierObject,HSelectionList *pSelectionList)
{
   if (!pHierObject->IsKindOf(SOT_PolyModel))
      return;

   HPolyModel *pPolyModel = (HPolyModel*)pHierObject;

   HBoneContainer *pBoneContainer = pPolyModel->GetBoneContainer();
   if (!pBoneContainer)
      return;

   // find max ID, for quick lookup
   UINT nMaxID = 0;
   for (POSITION pos = pPolyModel->GetHeadVertexPosition(); pos; )
   {
      HVertex *pVertex = pPolyModel->GetNextVertex(pos);
      if (pVertex->GetID()>nMaxID)
         nMaxID = pVertex->GetID();
   }

   m_vertexlookup.RemoveAll();
   m_vertexlookup.SetSize(nMaxID+1);
   memset(m_vertexlookup.GetData(),FALSE,sizeof(BOOL));

   for (UINT nChildIndex = 0; nChildIndex < pBoneContainer->GetNumTransformableObjects(); nChildIndex++)
   {
      HTransformableObject *pChildBone = pBoneContainer->GetTransformableObjectAt(nChildIndex);
      DrawBoneVertices(pView,pChildBone,pSelectionList);
   }

   // draw vertices not assigned to any bones
   for (POSITION pos = pPolyModel->GetHeadVertexPosition(); pos; )
   {
      HVertex *pVertex = pPolyModel->GetNextVertex(pos);
      UINT nID = pVertex->GetID();
      if (!m_vertexlookup[nID])
      {
         pVertex->SetDrawColor(RGBAFloat(0.0f,0.0f,0.0f,1.0f));
         pVertex->Draw(pView);
      }
   }
}

void AssignVertexToBoneManipulator::DrawBoneVertices(HCGLBaseView *pView,HTransformableObject *pTransformableObject,HSelectionList *pSelectionList)
{
   if (pTransformableObject->IsBone())
   {
      HBone *pBone = (HBone*)pTransformableObject;
      float fAlpha = 1.0f;
      RGBFloat color = pBone->GetBoneColor()->GetValue();
      BOOL bSelected = pTransformableObject->IsSelected();// pSelectionList->Find(pTransformableObject);
      if (!bSelected)
         fAlpha = 0.3f;

      for (POSITION pos = pBone->GetVertexHeadPosition(); pos; )
      {
         HVertex *pVertex = pBone->GetNextVertex(pos);

         BOOL bDrawVertex = FALSE;
         for (UINT i = 0; i < pVertex->GetNumSurroundingPolys(); i++)
         {
            HPoly *pSurroundingPoly = pVertex->GetSurroundingPolyAt(i);
            if (pSurroundingPoly->IsVisible())
               bDrawVertex = TRUE;          
         }
      
         if (!bDrawVertex)
         {
            m_vertexlookup.SetAt(pVertex->GetID(),TRUE);
            continue;
         }
   
         UINT nID = pVertex->GetID();
         pVertex->SetDrawColor(RGBAFloat(color.GetRed(),color.GetGreen(),color.GetBlue(),fAlpha));
         pVertex->Draw(pView);
         m_vertexlookup.SetAt(pVertex->GetID(),TRUE);
      }
   }

   for (UINT nChildIndex = 0; nChildIndex < pTransformableObject->GetNumTransformableObjects(); nChildIndex++)
   {
      HTransformableObject *pChildBone = pTransformableObject->GetTransformableObjectAt(nChildIndex);
      DrawBoneVertices(pView,pChildBone,pSelectionList);
   }
}

BOOL AssignVertexToBoneManipulator::OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView ) 
{
   if (m_bValidateSelection)
   {
      m_bValidateSelection = FALSE;

      HMainInterface *pMI = GetHMainInterface();
      ASSERT(pMI);

      String strParameters = "-b ";

      String strDescription;
      pMI->ObjectToDescription(m_pBone,strDescription);// add bone to parameterlist

      strParameters += strDescription;

      // add vertices to paramterlist

      for (int i = 0; i < m_vertexarray.GetSize(); i++)
      {
         pMI->ObjectToDescription(m_vertexarray.GetAt(i),strDescription);
         strParameters += " ";
         strParameters += strDescription;
      }

      StringList strResults;
      pMI->DoCommand( GetAssignVertexToBoneManipulatorPlugin()->m_pAssignVertexToBoneCommandPlugin, strParameters, strResults);
   }
 
   return SelectManipulator::OnLButtonUp(keymodifier,mousestate,point,pView);
}
