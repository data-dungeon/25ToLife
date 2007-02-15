//

#include "stdafx.h"
#include "DeleteFaceCommand.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "../Resource.h"

CommandInstance *DeleteFaceCommandPlugin::CreateCommandInstance() { return SNEW DeleteFaceCommand(this); }

const char *DeleteFaceCommandPlugin::GetCLIName() const { return "DeleteFace"; }; // Name used in scripts and commandline
const char *DeleteFaceCommandPlugin::GetTitleName() const { return "Delete Face"; }
UINT DeleteFaceCommandPlugin::GetBitmapID() const { return IDB_DELETEFACE; }

String DeleteFaceCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp  = "Deletes the face only, leaving the edges intact and vertices intact.\n";
   strHelp += "If no FACEID's are provided then the current selected faces will be used.";
   strHelp += (String) "    " + GetCLIName() + "\n";
   strHelp += (String) "    " + GetCLIName() + " FACEID  [FACEID]+\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " Face(2)\n";
   strHelp += (String) "    " + GetCLIName() + " Face(5) Face(7)\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UndoType DeleteFaceCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

DeleteFaceCommand::~DeleteFaceCommand()
{
   for (POSITION pos = m_deletefaceinfos.GetHeadPosition(); pos; )
      delete m_deletefaceinfos.GetNext(pos);
}

BOOL DeleteFaceCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   String strError;
   HMainInterface *pMainInterface = GetHMainInterface();
   HSnipeObject *pObject = pMainInterface->DescriptionToObject( szToken, &strError );
   if (!pObject)
   {
      String strMsg("Error in DeleteFace, Reason: %1.");
      strMsg.Replace("%1", strError);

      THROW_FATAL_ERROR(strMsg);
   }

   if (!pObject->IsKindOf(SOT_Poly))
      m_polylist.AddTail((HPoly *)pObject);
   else
   {
      String strMsg("Error in DeleteFace, %1 is not a face.");
      strMsg.Replace("%1", szToken);
      THROW_FATAL_ERROR(strMsg);
   }

   return TRUE;
}

BOOL DeleteFaceCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate(strOutError))
      return FALSE;

   if (m_polylist.IsEmpty())
   {
      // Lets use the current selectionlist
      HMainInterface *pMainInterface = GetHMainInterface();
      HSelectionList *pSelectionList = pMainInterface->GetCurrentSelectionList();
      
      for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
      {
         HSnipeObject *pObject = pSelectionList->GetNext(pos);
         if (pObject->IsKindOf(SOT_Poly))
            m_polylist.AddTail((HPoly *)pObject);
      }
   }

   if (m_polylist.IsEmpty())
   {
      strOutError = "Unable to execute DeleteFace command, no faces provided.";
      return FALSE;
   }

   return TRUE;
}

BOOL DeleteFaceCommand::OnDo(StringList &strResults)
{
   for (POSITION pos = m_polylist.GetHeadPosition(); pos; )
   {
      HPoly *pPoly = m_polylist.GetNext(pos);
      HDeleteFaceInfo *pInfo = pPoly->DeleteFace();
      if (pInfo)
         m_deletefaceinfos.AddTail(pInfo);
   }

   return TRUE;
}

void DeleteFaceCommand::OnUndo()
{
   for (POSITION pos = m_deletefaceinfos.GetTailPosition(); pos; )
   {
      HDeleteFaceInfo *pInfo = m_deletefaceinfos.GetPrev(pos);
      pInfo->UnDelete();
   }
}

void DeleteFaceCommand::OnRedo()
{
   for (POSITION pos = m_deletefaceinfos.GetHeadPosition(); pos; )
   {
      HDeleteFaceInfo *pInfo = m_deletefaceinfos.GetNext(pos);
      pInfo->Delete();
   }
}