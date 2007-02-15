//

#include "stdafx.h"
#include "AddBoneCommand.h"
#include "../resource.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HBone.h"
#include "HPolyModel.h"
#include "HHierObject.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"
#include "HFloatProperty.h"
#include "HBoolProperty.h"

CommandInstance *AddBoneCommandPlugin::CreateCommandInstance() { return SNEW AddBoneCommand(this); }

const char *AddBoneCommandPlugin::GetCLIName() const { return "AddBone"; }; // Name used in scripts and commandline
const char *AddBoneCommandPlugin::GetTitleName() const { return "AddBone"; }

UINT AddBoneCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String AddBoneCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Add bone.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

AddBoneCommand::~AddBoneCommand()
{
   if (m_pDeleteInfo)
      delete m_pDeleteInfo;
}

BOOL AddBoneCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   HMainInterface *pMI = GetHMainInterface();
   ASSERT(pMI);

   if (stricmp( szToken, "-p" ) == 0)// bone parent
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing parent bone argument.");

      String strError;
      HSnipeObject *pSnipeObject = pMI->DescriptionToObject( szNextToken, &strError );
      if (pSnipeObject)
      {
         if (pSnipeObject->GetSOT() != SOT_Bone)
         {
            String strMsg("Invalid bone, Reason: %1.");
            strMsg.Replace("%1", strError);
            THROW_FATAL_ERROR(strMsg);
         }

         m_pParentBone = (HBone*)pSnipeObject;
      }
      return TRUE;
   }
   else if (stricmp( szToken, "-m" ) == 0)// bone start
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing parent bone argument.");

      String strError;
      HSnipeObject *pSnipeObject = pMI->DescriptionToObject( szNextToken, &strError );
      if (!pSnipeObject)
      {
         String strMsg("Invalid polymodel, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      if (!pSnipeObject->IsKindOf(SOT_PolyModel))
      {
         String strMsg("Invalid polymodel, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      m_pPolyModel = (HPolyModel*)pSnipeObject;

      return TRUE;
   }
   else if (stricmp( szToken, "-s" ) == 0)// bone start
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      m_vStart.x = String(szNextToken).AsFloat();

      szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      m_vStart.y = String(szNextToken).AsFloat();

      szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      m_vStart.z = String(szNextToken).AsFloat();

      return TRUE;
   }
   else if (stricmp( szToken, "-r" ) == 0)// bone rotate
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      m_vRotate.x = String(szNextToken).AsFloat();

      szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      m_vRotate.y = String(szNextToken).AsFloat();

      szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      m_vRotate.z = String(szNextToken).AsFloat();

      return TRUE;
   }
   else if (stricmp( szToken, "-l" ) == 0)// bone length
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing argument.");

      m_fBoneLength = String(szNextToken).AsFloat();
      return TRUE;
   }
   else if (stricmp( szToken, "-c" ) == 0)// chain to parent
   {
      m_bIsChainedToParent = TRUE;
      return TRUE;
   }
   return FALSE;
}

BOOL AddBoneCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   if (!m_pPolyModel)
      return FALSE;

   if (!m_pParentBone)
      m_pParentBone = (HBone*)m_pPolyModel;

   return TRUE;
}

UndoType AddBoneCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL AddBoneCommand::OnDo(StringList &strResults)
{
   m_pBone = (HBone *)GetHMainInterface()->CreateObject(SOT_Bone);
   m_pBone->SetName("Bone");
   m_pPolyModel->AddBone(m_pBone,m_pParentBone);
   m_pBone->GetStart()->StoreValue(m_vStart);
   m_pBone->GetRotate()->StoreValue(m_vRotate);
   m_pBone->GetLength()->StoreValue(m_fBoneLength);
   m_pBone->GetChained()->StoreValue(m_bIsChainedToParent);

   String strerror;
   if (!m_pBone->Delete((HSnipeObject::HDeleteInfo *&)m_pDeleteInfo,strerror))
      return FALSE;
   OnRedo();
   return TRUE;
}

void AddBoneCommand::OnRedo()
{
   m_pDeleteInfo->UnDelete();
}

void AddBoneCommand::OnUndo()
{
   if (!m_pDeleteInfo)
      return;
   m_pDeleteInfo->Delete();
}
