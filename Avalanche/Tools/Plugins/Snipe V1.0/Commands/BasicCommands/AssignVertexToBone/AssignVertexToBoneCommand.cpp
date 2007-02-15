//

#include "stdafx.h"
#include "AssignVertexToBoneCommand.h"
#include "../resource.h"
#include "HSelectionList.h"
#include "HMainInterface.h"
#include "HBone.h"
#include "HPatchVertex.h"
#include "HPolyModel.h"

CommandInstance *AssignVertexToBoneCommandPlugin::CreateCommandInstance() { return SNEW AssignVertexToBoneCommand(this); }

const char *AssignVertexToBoneCommandPlugin::GetCLIName() const { return "AssignVertexToBone"; }; // Name used in scripts and commandline
const char *AssignVertexToBoneCommandPlugin::GetTitleName() const { return "AssignVertexToBone"; }

UINT AssignVertexToBoneCommandPlugin::GetBitmapID() const { return IDB_DEFAULT; }

String AssignVertexToBoneCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Assign vertices to bones.";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

AssignVertexToBoneCommand::~AssignVertexToBoneCommand()
{
   for (int i = 0; i < m_bonevertexinfoarray.GetSize(); i++)
   {
      BoneVertexInfo *pBoneVertexInfo = m_bonevertexinfoarray.GetAt(i);
      delete pBoneVertexInfo;
   }
}

BOOL AssignVertexToBoneCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-b" ) == 0)
   {
      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Missing bone argument.");

      HMainInterface *pMI = GetHMainInterface();
      ASSERT(pMI);

      String strError;
      m_pBone = (HBone*)pMI->DescriptionToObject( szNextToken, &strError );

      if (!m_pBone)
      {
         String strMsg("Invalid bone, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

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

      if (pSnipeObject->IsKindOf(SOT_Vertex))
      {
         BoneVertexInfo *pBoneVertexInfo = SNEW BoneVertexInfo;
         pBoneVertexInfo->m_pVertex = (HVertex*)pSnipeObject;
         m_bonevertexinfoarray.Add(pBoneVertexInfo);
      }
 
      return TRUE;
   }
   return FALSE;
}

// put this function here, because it makes the assumption that a vertex can only be assigned to one bone
HBone *FindBone(HVertex *pVertex,HTransformableObject *pTransformableObject)
{
   if (pTransformableObject->IsBone())
   {
      HBone *pBone = (HBone*)pTransformableObject;

      for (POSITION pos = pBone->GetVertexHeadPosition(); pos; )
      {
         HVertex *pBoneVertex = (HVertex*)pBone->GetNextVertex(pos);
         if (pVertex == pBoneVertex)
            return (HBone*)pTransformableObject;
      }
   }

   for (UINT nChildIndex = 0; nChildIndex < pTransformableObject->GetNumTransformableObjects(); nChildIndex++)
   {
      HTransformableObject *pChildBone = pTransformableObject->GetTransformableObjectAt(nChildIndex);
      return FindBone(pVertex,pChildBone);
   }
   return NULL;
}

BOOL AssignVertexToBoneCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   for (int i = 0; i < m_bonevertexinfoarray.GetSize(); i++)
   {
      BoneVertexInfo *pBoneVertexInfo = m_bonevertexinfoarray.GetAt(i);

      if (!pBoneVertexInfo->m_pVertex->IsKindOf(SOT_Vertex))
         continue;

      HPolyModel *pPolyModel = pBoneVertexInfo->m_pVertex->GetModel();

      HBoneContainer *pBoneContainer = pPolyModel->GetBoneContainer();
      if (!pBoneContainer)
         return FALSE;

      // find old vertex bone assignment
      HBone *pOldBone = NULL;
      for (UINT nChildIndex = 0; nChildIndex < pBoneContainer->GetNumTransformableObjects(); nChildIndex++)
      {
         HTransformableObject *pChildBone = pBoneContainer->GetTransformableObjectAt(nChildIndex);
         HBone *pResultBone = FindBone((HPatchVertex*)pBoneVertexInfo->m_pVertex,pChildBone);
         if (pResultBone)
            pOldBone = pResultBone;
      }
      pBoneVertexInfo->m_pOldBone = pOldBone;
   }

   return TRUE;
}

UndoType AssignVertexToBoneCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL AssignVertexToBoneCommand::OnDo(StringList &strResults)
{
   OnRedo();

   return TRUE;
}

void AssignVertexToBoneCommand::OnRedo()
{
   for (int i = 0; i < m_bonevertexinfoarray.GetSize(); i++)
   {
      BoneVertexInfo *pBoneVertexInfo = m_bonevertexinfoarray.GetAt(i);
      if (pBoneVertexInfo->m_pOldBone)
      {
         BOOL bResult = pBoneVertexInfo->m_pOldBone->RemoveVertex(pBoneVertexInfo->m_pVertex);
         ASSERT(bResult);
      }
      m_pBone->AddVertex(pBoneVertexInfo->m_pVertex);
   }
}

void AssignVertexToBoneCommand::OnUndo()
{
   for (int i = 0; i < m_bonevertexinfoarray.GetSize(); i++)
   {
      BoneVertexInfo *pBoneVertexInfo = m_bonevertexinfoarray.GetAt(i);
      if (pBoneVertexInfo->m_pOldBone)
      {
         BOOL bResult = m_pBone->RemoveVertex(pBoneVertexInfo->m_pVertex);
         ASSERT(bResult);
      }
      pBoneVertexInfo->m_pOldBone->AddVertex(pBoneVertexInfo->m_pVertex);
   }
}
