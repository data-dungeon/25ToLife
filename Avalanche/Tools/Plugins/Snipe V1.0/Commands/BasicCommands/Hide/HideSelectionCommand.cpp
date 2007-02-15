//

#include "stdafx.h"
#include "HideSelectionCommand.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HPoly.h"
#include "HPolyModel.h"
#include "HVertex.h"
#include "HEdge.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HCommandPluginManager.h"
#include "../Resource.h"

CommandInstance *HideSelectionCommandPlugin::CreateCommandInstance() { return SNEW HideSelectionCommand(this); }

const char *HideSelectionCommandPlugin::GetCLIName() const { return "Hide"; }; // Name used in scripts and commandline
const char *HideSelectionCommandPlugin::GetTitleName() const { return "Hide Selection"; }
UINT HideSelectionCommandPlugin::GetBitmapID() const { return IDB_HIDESELECTION; }

void HideSelectionCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
   m_pDeselectAllCommandPlugin = FindCommandByCLINameHelper(pCommandManager, "SNIPEBASICCOMMANDS", "DeselectAll"); // Use FindCommandHelper when you want to call a command from within a command
   if (m_pDeselectAllCommandPlugin==NULL)
      THROW_FATAL_ERROR("Missing dependent command ( SnipeBasicCommand : DeselectAll ).");
}

String HideSelectionCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Hides selected objects and components.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
HideSelectionCommand::HideSelectionCommand(HCommandPluginHelper *pPlugin) : CommandInstance(pPlugin)
{
   m_pDeselectAllCommand = ((HideSelectionCommandPlugin *)GetCommandPluginHelper())->m_pDeselectAllCommandPlugin->CreateCommandInstance();
}

HideSelectionCommand::~HideSelectionCommand()
{
   if (m_pDeselectAllCommand)
      delete m_pDeselectAllCommand;
}

UndoType HideSelectionCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL HideSelectionCommand::OnDo(StringList &strResults)
{
   // four step process...

   // 1) Find all selected polys (may be selected by selecting surrounding vertices or edges)
   CList< HPoly * > polylist;
   CMap< HVertex *, HVertex *, UINT, UINT > selectedvertexmap;
   CMap< HEdge *, HEdge *, UINT, UINT > selectededgemap;
   UINT temp = 0;

   HSelectionList *pSelectionList = GetHMainInterface()->GetCurrentSelectionList();

   for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = pSelectionList->GetNext(pos);
      if (pSnipeObject->IsKindOf(SOT_Vertex))
      {
         HVertex *pVertex = (HVertex *)pSnipeObject;
         selectedvertexmap.SetAt( pVertex, temp );
      }
      else if (pSnipeObject->IsKindOf(SOT_Poly))
      {
         HPoly *pPoly = (HPoly *)pSnipeObject;
         polylist.AddTail( pPoly );
      }
      else if (pSnipeObject->IsKindOf(SOT_Edge))
      {
         HEdge *pEdge = (HEdge *)pSnipeObject;
         selectededgemap.SetAt( pEdge, temp );
      }
      else if (pSnipeObject->IsVisible())
      {
         pSnipeObject->SetVisible( FALSE );
         m_objectlist.AddTail( pSnipeObject );
      }
   }

   BOOL bHidComponent = FALSE;
   if (!selectedvertexmap.IsEmpty())
   {
      bHidComponent = TRUE;

      // walk all polys and see if all of its vertices are in the selected maps
      HPolyModel *pModel = (HPolyModel *)GetHMainInterface()->GetCurrentCommandObject();
      for (POSITION pos=pModel->GetHeadFacePosition(); pos; )
      {
         HPoly *pPoly = pModel->GetNextFace( pos );
         UINT i;
         for (i=0; i<pPoly->GetNumSides(); i++)
         {
            if (!selectedvertexmap.Lookup( pPoly->GetVertexAt(i), temp ))
               break;
         }

         if (i == pPoly->GetNumSides()) // all vertices found
            polylist.AddTail( pPoly );
      }
   }

   if (!selectededgemap.IsEmpty())
   {
      bHidComponent = TRUE;
      
      // walk all polys and see if all of its edges are in the selected maps
      HPolyModel *pModel = (HPolyModel *)GetHMainInterface()->GetCurrentCommandObject();
      for (POSITION pos=pModel->GetHeadFacePosition(); pos; )
      {
         HPoly *pPoly = pModel->GetNextFace( pos );
         UINT i;
         for (i=0; i<pPoly->GetNumSides(); i++)
         {
            if (!selectededgemap.Lookup( pPoly->GetEdgeAt(i), temp ))
               break;
         }

         if (i == pPoly->GetNumSides()) // all vertices found
            polylist.AddTail( pPoly );
      }
   }

   if (!polylist.IsEmpty())
   {
      bHidComponent = TRUE;

   // 2) Hide those polys
      for (POSITION pos = polylist.GetHeadPosition(); pos; )
      {
         HPoly *pPoly = polylist.GetNext(pos);
         if (pPoly->IsVisible())
         {
            m_objectlist.AddTail( pPoly );
            pPoly->SetVisible( FALSE );
         }
      }
   }

   if (bHidComponent)
      SetComponentsVisibiltyToParentsVisibility(&m_objectlist);

   m_pDeselectAllCommand->OnDo(strResults);

   return TRUE;
}

void HideSelectionCommand::OnUndo()
{
   m_pDeselectAllCommand->OnUndo();

   for (POSITION pos = m_objectlist.GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_objectlist.GetNext(pos);
      pSnipeObject->SetVisible( TRUE );
   }
}

void HideSelectionCommand::OnRedo()
{
   for (POSITION pos = m_objectlist.GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_objectlist.GetNext(pos);
      pSnipeObject->SetVisible( FALSE );
   }
   m_pDeselectAllCommand->OnRedo();
}

void SetComponentsVisibiltyToParentsVisibility(CList<HSnipeObject *> *pChangedList)
{
   HPolyModel *pModel = (HPolyModel *)GetHMainInterface()->GetCurrentCommandObject();

   HSnipeObjectList *pEdgeList = pModel->CreateListFromSOT( SOT_Edge );
   for (POSITION pos=pEdgeList->GetHeadPosition(); pos; )
   {
      HEdge *pEdge = (HEdge *)pEdgeList->GetNext( pos );

      BOOL bVisible = 
         (pEdge->GetFirstPoly() && pEdge->GetFirstPoly()->IsVisible())
       ||(pEdge->GetSecondPoly() && pEdge->GetSecondPoly()->IsVisible())
       ||(pEdge->GetFirstPoly()==NULL && pEdge->GetSecondPoly()==NULL && (!pEdge->GetFirstVertex()->IsSelected() || !pEdge->GetSecondVertex()->IsSelected())); // Dangle

      if (pEdge->IsVisible() != bVisible)
      {
         pEdge->SetVisible( bVisible );
         pChangedList->AddTail( pEdge );
      }
   }
   delete pEdgeList;

   HSnipeObjectList *pVertexList = pModel->CreateListFromSOT( SOT_Vertex );
   for (POSITION pos=pVertexList->GetHeadPosition(); pos; )
   {
      HVertex *pVertex = (HVertex *)pVertexList->GetNext( pos );\
       
      UINT nID = pVertex->GetID();

      BOOL bVisible = FALSE;

      for (UINT i=0; i<pVertex->GetNumSurroundingEdges(); i++)
      {
         HEdge *pEdge = pVertex->GetSurroundingEdgeAt(i);
         if (pEdge->IsVisible())
         {
            bVisible = TRUE;
            break;
         }
      }

      if (pVertex->IsVisible() != bVisible )
      {
         pVertex->SetVisible( bVisible );
         pChangedList->AddTail( pVertex );
      }
   }
   delete pVertexList;
}

