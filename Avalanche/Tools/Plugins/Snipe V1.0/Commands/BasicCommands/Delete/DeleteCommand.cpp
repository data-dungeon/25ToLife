//

#include "stdafx.h"
#include "DeleteCommand.h"
#include "../resource.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HComponent.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HPatchModel.h"
#include "HPatch.h"
#include "HPoly.h"
#include "HEdge.h"

CommandInstance *DeleteCommandPlugin::CreateCommandInstance() { return SNEW DeleteCommand(this); }

const char *DeleteCommandPlugin::GetCLIName() const { return "Delete"; }; // Name used in scripts and commandline
const char *DeleteCommandPlugin::GetTitleName() const { return "Delete"; }

UINT DeleteCommandPlugin::GetBitmapID() const { return IDB_DELETE; }

String DeleteCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Deletes objects and components.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

DeleteCommand::~DeleteCommand()
{
   for (POSITION pos = m_DetachedInfoList.GetHeadPosition(); pos; )
   {
      HSnipeObject::HDeleteInfo *pDetachedInfo = m_DetachedInfoList.GetNext(pos);
      delete pDetachedInfo;
   }
}

UndoType DeleteCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL DeleteCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   String strError;
   if (GetHMainInterface()->DescriptionToObject( szToken, &strError ))
      m_deletelist.AddTail(szToken);
   else
      THROW_FATAL_ERROR(strError);

   return TRUE;
}

BOOL DeleteCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate(strOutError))
      return FALSE;

   if (m_deletelist.IsEmpty())
   {
      // Lets use the current selectionlist
      HMainInterface *pMainInterface = GetHMainInterface();
      HSelectionList *pSelectionList = pMainInterface->GetCurrentSelectionList();

      for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pObject = pSelectionList->GetNext(pos);
         String strDescription;
         if (pMainInterface->ObjectToDescription( pObject, strDescription ))
            m_deletelist.AddTail( strDescription );
         else
            ASSERT(FALSE);
      }
   }

   if (m_deletelist.IsEmpty())
   {
      strOutError = "Unable to execute DeleteCommand command, no objects provided.";
      return FALSE;
   }

   return TRUE;
}

// Removes one object and also removes it from the selection list
BOOL DeleteCommand::DeleteSnipeObject( HSnipeObject *pSnipeObject, String &strError )
{
   HSnipeObject::HDeleteInfo *pDeleteInfo = NULL;

   // remove object from anyone who is referencing it
   if (pSnipeObject->Delete( pDeleteInfo, strError ))
   {
      // and add to the list of objects to undo
      m_DetachedInfoList.AddTail( pDeleteInfo );
      return TRUE;
   }

   return FALSE;
}

BOOL DeleteCommand::OnDo(StringList &strResults)
{
   HMainInterface *pMI = GetHMainInterface();

   for (POSITION pos = m_deletelist.GetHeadPosition(); pos; )
   {
      String &strObject = m_deletelist.GetNext(pos);
      HSnipeObject *pSnipeObject = pMI->DescriptionToObject( strObject );
      if (!pSnipeObject) // Object must have been removed by another delete
         continue;

      String strError;
      if (!DeleteSnipeObject(pSnipeObject, strError)) // It is possible on BezierSplineModels that Deleting one knot can remove another knot in the stack 
      {
         DisplayWarning(strError);
         continue;
      }

      // Auto delete edges vertices
      if (/*FALSE && */pSnipeObject->IsKindOf(SOT_Edge))
      {
         HEdge *pEdge = (HEdge *)pSnipeObject;

         HVertex *pVertex = pEdge->GetFirstVertex();
         if (pVertex->IsInProject() && pVertex->GetNumSurroundingEdges() == 2)
            DeleteSnipeObject(pVertex, strError);

         pVertex = pEdge->GetSecondVertex();
         if (pVertex->IsInProject() && pVertex->GetNumSurroundingEdges() == 2)
            DeleteSnipeObject(pVertex, strError);
      }
   }

   return TRUE;
}

void DeleteCommand::OnRedo()
{
   for (POSITION pos = m_DetachedInfoList.GetHeadPosition(); pos; )
   {
      HSnipeObject::HDeleteInfo *pDetachedInfo = m_DetachedInfoList.GetNext(pos);
      pDetachedInfo->Delete();
   }
}

void DeleteCommand::OnUndo()
{
   for (POSITION pos = m_DetachedInfoList.GetTailPosition(); pos; )
   {
      HSnipeObject::HDeleteInfo *pDetachedInfo = m_DetachedInfoList.GetPrev(pos);
      pDetachedInfo->UnDelete();
   }
}
