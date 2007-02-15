//

#include "stdafx.h"
#include "CreateFaceCommand.h"
#include "HPoly.h"
#include "HMainInterface.h"
#include "../Resource.h"

CommandInstance *CreateFaceCommandPlugin::CreateCommandInstance() { return SNEW CreateFaceCommand(this); }

const char *CreateFaceCommandPlugin::GetCLIName() const { return "CreateFace"; }; // Name used in scripts and commandline
const char *CreateFaceCommandPlugin::GetTitleName() const { return "Create Face"; }
UINT CreateFaceCommandPlugin::GetBitmapID() const { return IDB_CREATEFACE; }

String CreateFaceCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp  = "Creates all valid faces for a given vertex or edge.  Multiple edges and vertices can\n";
   strHelp += "be provided.  OBJECTNAME must be an Edge or Vertex.\n";
   strHelp += (String) "  " + GetCLIName() + " [-s MAXNUMBEROFSIDES] OBJECTNAMES\n";
   strHelp += (String) "    -s MAXNUMBEROFSIDES (Maximum number of sides to allow a face to be created, defaults to 20.)\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " Edge(2)";
   strHelp += (String) "    " + GetCLIName() + "-s 4 Edge(2)";
   strHelp += (String) "    " + GetCLIName() + "-s Edge(2) Edge(5)";
   strHelp += (String) "    " + GetCLIName() + "-s Vertex(2) Edge(5)";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CreateFaceCommand::~CreateFaceCommand()
{
   for (POSITION pos = m_deletefaceonfos.GetHeadPosition(); pos; )
      delete m_deletefaceonfos.GetNext(pos);
}

UndoType CreateFaceCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL CreateFaceCommand::ParseToken( const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-s" ) == 0)
   {
      char *szToken = tokenlist.GetNext();
      m_nMaxSides = atoi(szToken);
      return TRUE;
   }

   // Must be an Edge or Vertex
   String strError;
   HMainInterface *pMainInterface = GetHMainInterface();
   HSnipeObject *pObject = pMainInterface->DescriptionToObject( szToken, &strError );
   if (!pObject)
   {
      String strMsg("Error in CreateFace, Reason: %1.");
      strMsg.Replace("%1", strError);

      THROW_FATAL_ERROR(strMsg);
   }

   if (pObject->IsKindOf(SOT_Vertex))
      m_vertexlist.AddTail((HVertex *)pObject);
   else if (pObject->IsKindOf(SOT_Edge))
      m_edgelist.AddTail((HEdge *)pObject);
   else
   {
      String strMsg("Error in CreateFace, %1 is an invalid object type.");
      strMsg.Replace("%1", szToken);
      THROW_FATAL_ERROR(strMsg);
   }

   return TRUE;
}

BOOL CreateFaceCommand::OnDo(StringList &strResults)
{
   ULONG nTotal=0;
   for (POSITION pos = m_vertexlist.GetHeadPosition(); pos;)
   {
      HVertex *pVertex = m_vertexlist.GetNext(pos);
      HDeleteFaceInfo **pInfos = NULL;;
      UINT nCount = pVertex->FindAndCreateFaces(pInfos, m_nMaxSides);
      if (nCount)
      {
         for (UINT i=0; i<nCount; i++)
            m_deletefaceonfos.AddTail( pInfos[i] );
         delete [] pInfos;
      }
      nTotal+=nCount;
   }

   for (POSITION pos = m_edgelist.GetHeadPosition(); pos;)
   {
      HEdge *pEdge = m_edgelist.GetNext(pos);
      HDeleteFaceInfo **pInfos = NULL;;
      UINT nCount = pEdge->FindAndCreateFaces(pInfos, m_nMaxSides);
      if (nCount)
      {
         for (UINT i=0; i<nCount; i++)
            m_deletefaceonfos.AddTail( pInfos[i] );
         delete [] pInfos;
      }
      nTotal+=nCount;
   }
   String strMsg("%1 faces added.");
   strMsg.Replace("%1", (String)nTotal);
   DisplayMessage(strMsg);

   return TRUE;
}

void CreateFaceCommand::OnUndo()
{
   for (POSITION pos = m_deletefaceonfos.GetTailPosition(); pos; )
   {
      HDeleteFaceInfo *pInfo = m_deletefaceonfos.GetPrev(pos);
      pInfo->Delete();
   }
}

void CreateFaceCommand::OnRedo()
{
   for (POSITION pos = m_deletefaceonfos.GetHeadPosition(); pos; )
   {
      HDeleteFaceInfo *pInfo = m_deletefaceonfos.GetNext(pos);
      pInfo->UnDelete();
   }
}