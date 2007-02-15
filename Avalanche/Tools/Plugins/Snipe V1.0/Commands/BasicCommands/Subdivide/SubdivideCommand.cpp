//

#include "stdafx.h"
#include "SubdivideCommand.h"
#include "../resource.h"
#include "HCBaseDoc.h"
#include "HSelectionList.h"
#include "HComponent.h"
#include "HMainInterface.h"
#include "HSnipeObjectList.h"
#include "HPatch.h"
#include "HIntProperty.h"

CommandInstance *SubdivideCommandPlugin::CreateCommandInstance() { return SNEW SubdivideCommand(this); }

const char *SubdivideCommandPlugin::GetCLIName() const { return "Subdivide"; }; // Name used in scripts and commandline
const char *SubdivideCommandPlugin::GetTitleName() const { return "Subdivide"; }

UINT SubdivideCommandPlugin::GetBitmapID() const { return IDB_SUBDIVIDEUP; }

String SubdivideCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Set patch subdivision level.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

SubdivideCommand::~SubdivideCommand()
{
}

BOOL SubdivideCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-fix" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing subdivisions argument.");

      m_nSubdivisions = (UINT)String(szNextToken).AsInt();
      m_nSubdivideType = FIXED;

      return TRUE;
   }
   else if (stricmp( szToken, "-inc" ) == 0)
   {
      m_nSubdivideType = INCREMENT;
      return TRUE;
   }
   else if (stricmp( szToken, "-dec" ) == 0)
   {
      m_nSubdivideType = DECREMENT;
      return TRUE;
   }
   else
   {
      HMainInterface *pMI = GetHMainInterface();
      ASSERT(pMI);

      String strError;
      HSnipeObject *pSnipeObject = pMI->DescriptionToObject( szToken, &strError );

      if (!pSnipeObject)
      {
         String strMsg("Invalid component, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      if (pSnipeObject->IsKindOf(SOT_Patch))
      {
         HPatch *pPatch = (HPatch*)pSnipeObject;
         PatchSubdivideInfo *pPatchSubdivideInfo = SNEW PatchSubdivideInfo;
         pPatchSubdivideInfo->m_pPatch = pPatch;
         m_patchsubdivideinfoarray.Add(pPatchSubdivideInfo);
      }

      return TRUE;
   }
   return FALSE;
}

BOOL SubdivideCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   for (int i = 0; i < m_patchsubdivideinfoarray.GetSize(); i++)
   {
      PatchSubdivideInfo *pPatchSubdivideInfo = m_patchsubdivideinfoarray.GetAt(i);

      pPatchSubdivideInfo->m_nPrevSubdivisions = pPatchSubdivideInfo->m_pPatch->GetMaxSubdivision();
      pPatchSubdivideInfo->m_nSubdivisions = pPatchSubdivideInfo->m_nPrevSubdivisions;
      if (m_nSubdivideType == INCREMENT)
      {
         pPatchSubdivideInfo->m_nSubdivisions++;
         if (pPatchSubdivideInfo->m_nSubdivisions>4)
            pPatchSubdivideInfo->m_nSubdivisions=4;
      }
      else if (m_nSubdivideType == DECREMENT)
      {
         int nSubdivisions = pPatchSubdivideInfo->m_nSubdivisions;
         nSubdivisions--;
         if (nSubdivisions<0)
            nSubdivisions=0;
         pPatchSubdivideInfo->m_nSubdivisions = nSubdivisions;
      }
      else if (m_nSubdivideType == FIXED)
      {
         pPatchSubdivideInfo->m_nSubdivisions = m_nSubdivisions;
      }
   }

   return TRUE;
}

UndoType SubdivideCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL SubdivideCommand::OnDo(StringList &strResults)
{
   OnRedo();
   return TRUE;
}

void SubdivideCommand::OnRedo()
{
   for (int i = 0; i < m_patchsubdivideinfoarray.GetSize(); i++)
   {
      PatchSubdivideInfo *pPatchSubdivideInfo = m_patchsubdivideinfoarray.GetAt(i);
      pPatchSubdivideInfo->m_pPatch->SetMaxSubdivision(pPatchSubdivideInfo->m_nSubdivisions);
   }
}

void SubdivideCommand::OnUndo()
{
   for (int i = 0; i < m_patchsubdivideinfoarray.GetSize(); i++)
   {
      PatchSubdivideInfo *pPatchSubdivideInfo = m_patchsubdivideinfoarray.GetAt(i);
      pPatchSubdivideInfo->m_pPatch->SetMaxSubdivision(pPatchSubdivideInfo->m_nPrevSubdivisions);
   }
}

