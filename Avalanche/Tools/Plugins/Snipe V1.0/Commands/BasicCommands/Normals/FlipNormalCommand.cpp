//

#include "stdafx.h"
#include "FlipNormalCommand.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "HSelectionList.h"
#include "../Resource.h"

CommandInstance *FlipNormalCommandPlugin::CreateCommandInstance() { return SNEW FlipNormalCommand(this); }

const char *FlipNormalCommandPlugin::GetCLIName() const { return "FlipNormal"; }; // Name used in scripts and commandline
const char *FlipNormalCommandPlugin::GetTitleName() const { return "Flip Normal"; }
UINT FlipNormalCommandPlugin::GetBitmapID() const { return IDB_FLIPNORMAL; }

String FlipNormalCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp  = "Flips the normal of the face, If no FACEID's are provided then the current\n";
   strHelp += "selected faces will be used.";
   strHelp += (String) "    " + GetCLIName() + "\n";
   strHelp += (String) "    " + GetCLIName() + " FACEID  [FACEID]+\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " Face(2)\n";
   strHelp += (String) "    " + GetCLIName() + " Face(5) Face(7)\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

UndoType FlipNormalCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

FlipNormalCommand::~FlipNormalCommand()
{
//   for (POSITION pos = m_deletefaceinfos.GetHeadPosition(); pos; )
//      delete m_deletefaceinfos.GetNext(pos);
}

BOOL FlipNormalCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   String strError;
   HMainInterface *pMainInterface = GetHMainInterface();
   HSnipeObject *pObject = pMainInterface->DescriptionToObject( szToken, &strError );
   if (!pObject)
   {
      String strMsg("Error in FlipNormal, Reason: %1.");
      strMsg.Replace("%1", strError);

      THROW_FATAL_ERROR(strMsg);
   }

   if (!pObject->IsKindOf(SOT_Poly))
      m_polylist.AddTail((HPoly *)pObject);
   else
   {
      String strMsg("Error in FlipNormal, %1 is not a face.");
      strMsg.Replace("%1", szToken);
      THROW_FATAL_ERROR(strMsg);
   }

   return TRUE;
}

BOOL FlipNormalCommand::OnValidate( String &strOutError )
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
      strOutError = "No faces provided.";
      return FALSE;
   }

   return TRUE;
}

BOOL FlipNormalCommand::OnDo(StringList &strResults)
{
   for (POSITION pos = m_polylist.GetHeadPosition(); pos; )
   {
      HPoly *pPoly = m_polylist.GetNext(pos);
      pPoly->FlipNormal();
   }

   return TRUE;
}

void FlipNormalCommand::OnUndo()
{
   for (POSITION pos = m_polylist.GetTailPosition(); pos; )
   {
      HPoly *pPoly = m_polylist.GetPrev(pos);
      pPoly->FlipNormal();
   }
}

void FlipNormalCommand::OnRedo()
{
   for (POSITION pos = m_polylist.GetHeadPosition(); pos; )
   {
      HPoly *pPoly = m_polylist.GetNext(pos);
      pPoly->FlipNormal();
   }
}