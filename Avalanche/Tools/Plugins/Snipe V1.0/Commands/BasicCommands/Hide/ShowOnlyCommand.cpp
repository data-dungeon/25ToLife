//

#include "stdafx.h"
#include "ShowOnlyCommand.h"
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

CommandInstance *ShowOnlyCommandPlugin::CreateCommandInstance() { return SNEW ShowOnlyCommand(this); }

const char *ShowOnlyCommandPlugin::GetCLIName() const { return "ShowOnly"; }; // Name used in scripts and commandline
const char *ShowOnlyCommandPlugin::GetTitleName() const { return "Show Only"; }
UINT ShowOnlyCommandPlugin::GetBitmapID() const { return IDB_SHOWONLY; }

void ShowOnlyCommandPlugin::Init(HCommandPluginManager *pCommandManager)
{
   // Collect dependencies here
}

String ShowOnlyCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Shows only the selected objects and components, and hides everything else.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
ShowOnlyCommand::ShowOnlyCommand(HCommandPluginHelper *pPlugin) : CommandInstance(pPlugin)
{
   m_pVertexList = NULL;
}

ShowOnlyCommand::~ShowOnlyCommand()
{
}

UndoType ShowOnlyCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL ShowOnlyCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   StringList strErrorList;
   HMainInterface *pMainInterface = GetHMainInterface();
   HSnipeObjectList *pVertexList = pMainInterface->DescriptionToObjects( szToken, strErrorList );
   if (!pVertexList)
   {
      String strMsg("Error in ShowOnly, Reason: %1.");
      strMsg.Replace("%1", strErrorList.GetHead());

      THROW_FATAL_ERROR(strMsg);
   }

   pVertexList->ConvertCompatibleObjectsToSOT( SOT_Vertex );

   if (pVertexList->IsEmpty())
   {
      String strMsg("Error in ShowOnly, no vertices in %1.");
      strMsg.Replace("%1", szToken);
      delete pVertexList;
      THROW_FATAL_ERROR(strMsg);
   }
   else
   {
      if (!m_pVertexList)
         m_pVertexList = HSnipeObjectList::New();
      m_pVertexList->AddTail( pVertexList );
      delete pVertexList;
   }

   return TRUE;
}

BOOL ShowOnlyCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate(strOutError))
      return FALSE;

   if (!m_pVertexList)
   {
      // Lets use the current selection list
      HMainInterface *pMainInterface = GetHMainInterface();
      HSelectionList *pSelectionList = pMainInterface->GetCurrentSelectionList();
      m_pVertexList = pSelectionList->CreateSnipeObjectList();
      m_pVertexList->ConvertCompatibleObjectsToSOT( SOT_Vertex );
   }

   if (!m_pVertexList || m_pVertexList->IsEmpty())
   {
      strOutError = "Unable to execute ShowOnly command, no objects provided.";
      return FALSE;
   }

   return TRUE;
}

// Dan you may want to reevaluate this stuff. You are using a VertexData in a map. That may not be necessary anymore.
BOOL ShowOnlyCommand::OnDo(StringList &strResults)
{
   CMap<void *, void *, UINT, UINT> vertexmap;
   for (POSITION pos=m_pVertexList->GetHeadPosition(); pos; )
   {
      HVertex *pVertex = (HVertex *)m_pVertexList->GetNext( pos );
      vertexmap.SetAt( pVertex, 0 );
   }

   HPolyModel *pModel = (HPolyModel *)GetHMainInterface()->GetCurrentCommandObject();

   UINT temp;
   // walk all vertices and hide it if its not in the map
   for (POSITION pos=pModel->GetHeadVertexPosition(); pos; )
   {
      HVertex *pVertex = pModel->GetNextVertex( pos );

      if (!vertexmap.Lookup( pVertex, temp ) && pVertex->IsVisible())
      {
         m_objectlist.AddTail( pVertex );
         pVertex->SetVisible( FALSE );
      }
   }
   // walk all vertices and show it if its in the map (two passes because of mirror mode)
   for (POSITION pos=pModel->GetHeadVertexPosition(); pos; )
   {
      HVertex *pVertex = pModel->GetNextVertex( pos );

      if (vertexmap.Lookup( pVertex, temp ) && !pVertex->IsVisible())
      {
         m_objectlist.AddTail( pVertex );
         pVertex->SetVisible( TRUE );
      }
   }

   // walk all edges and hide it if any of its vertices are not in the map
   for (POSITION pos=pModel->GetHeadEdgePosition(); pos; )
   {
      HEdge *pEdge = pModel->GetNextEdge( pos );

      if ((!vertexmap.Lookup( pEdge->GetFirstVertex(), temp ) || !vertexmap.Lookup( pEdge->GetSecondVertex(), temp )) && pEdge->IsVisible())
      {
         m_objectlist.AddTail( pEdge );
         pEdge->SetVisible( FALSE );
      }
   }
   // walk all edges and show it if both of its vertices are in the map
   for (POSITION pos=pModel->GetHeadEdgePosition(); pos; )
   {
      HEdge *pEdge = pModel->GetNextEdge( pos );

      if (vertexmap.Lookup( pEdge->GetFirstVertex(), temp ) && vertexmap.Lookup( pEdge->GetSecondVertex(), temp ) && !pEdge->IsVisible())
      {
         m_objectlist.AddTail( pEdge );
         pEdge->SetVisible( TRUE );
      }
   }

   // walk all polys and hide it if any of its vertices are not in the map
   for (POSITION pos=pModel->GetHeadFacePosition(); pos; )
   {
      HPoly *pPoly = pModel->GetNextFace( pos );

      for (UINT i=0; i<pPoly->GetNumSides(); i++)
      {
         if (!vertexmap.Lookup( pPoly->GetVertexAt(i), temp ))
         {
            if (pPoly->IsVisible())
            {
               m_objectlist.AddTail( pPoly );
               pPoly->SetVisible( FALSE );
            }
            break; // next poly please
         }
      }
   }
   // walk all polys and show it if all of its vertices are in the map
   for (POSITION pos=pModel->GetHeadFacePosition(); pos; )
   {
      HPoly *pPoly = pModel->GetNextFace( pos );

      for (UINT i=0; i<pPoly->GetNumSides(); i++)
      {
         if (!vertexmap.Lookup( pPoly->GetVertexAt(i), temp ))
            break; // next poly please
      }

      if (i==pPoly->GetNumSides() && !pPoly->IsVisible())
      {
         m_objectlist.AddTail( pPoly );
         pPoly->SetVisible( TRUE );
      }
   }

   delete m_pVertexList;
   m_pVertexList = NULL;

   return TRUE;
}

void ShowOnlyCommand::OnUndo()
{
   for (POSITION pos = m_objectlist.GetHeadPosition(); pos; )
   {
      HSnipeObject *pSnipeObject = m_objectlist.GetNext(pos);
      pSnipeObject->SetVisible( !pSnipeObject->IsVisible() );
   }
}

void ShowOnlyCommand::OnRedo()
{
   OnUndo();
}
