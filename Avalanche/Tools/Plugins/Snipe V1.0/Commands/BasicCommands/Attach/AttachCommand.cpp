//

#include "stdafx.h"
#include "AttachCommand.h"
#include "HMainInterface.h"
#include "HEdge.h"
#include "../resource.h"
#include "HPoly.h"

CommandInstance *AttachCommandPlugin::CreateCommandInstance() { return SNEW AttachCommand(this); }

const char *AttachCommandPlugin::GetCLIName() const { return "Attach"; }; // Name used in scripts and commandline
const char *AttachCommandPlugin::GetTitleName() const { return "Attach"; }

UINT AttachCommandPlugin::GetBitmapID() const { return IDB_ATTACH; }

String AttachCommandPlugin::GetHelp() const
{
   String strHelp;
   strHelp = "Attaches two vertices together.\n";
   strHelp += "Vertices must be listed in pairs prefixed by a -p, and multiple pairs are allowed.\n";
   strHelp += (String) "  " + GetCLIName() + "[-nf] -p -fv VERTEXNAME -tv VERTEXNAME [-fe EGDENAME -te EDGENAME]...\n";
   strHelp += (String) "    -nf Don't auto find 3 and 4 point faces.\n";
   strHelp += (String) "    -vp New attach pair is to follow.\n";
   strHelp += (String) "    -fv From Vertex.\n";
   strHelp += (String) "    -tv To Vertex.\n";
   strHelp += (String) "    -fe From Edge, optional and only valid for PatchModels.  This is used for continuity.\n";
   strHelp += (String) "    -te To Edge, optional and only valid for PatchModels.  This is used for continuity.\n";
   strHelp += (String) "  Example:\n";
   strHelp += (String) "    " + GetCLIName() + " -p -tv Vertex(1) -fv Vertex(2) -p -fv Vertex(3) -tv Vertex(4) -fe Edge(5) -te Edge(6). This will attach Vertex 1 to Vertex 2 and Vertex 3 to Vertex 4 and make Edge 5 continuous through Edge 6.\n";
   return strHelp;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

AttachCommand::~AttachCommand()
{
   for (POSITION pos = m_vertexpairlist.GetHeadPosition(); pos;)
   {
      VertexPair *pPair = m_vertexpairlist.GetNext(pos);
      delete pPair;
   }
}

BOOL AttachCommand::ParseToken(const char *szToken, TokenList &tokenlist )
{
   if (CommandInstance::ParseToken( szToken, tokenlist ))
      return TRUE;

   if (stricmp( szToken, "-nf" ) == 0)
   {
      m_bAutoFindFaces = FALSE;
      return TRUE;
   }
   else if (stricmp( szToken, "-vp" ) == 0)
   {
      VertexPair *pPair = SNEW VertexPair;
      m_vertexpairlist.AddTail(pPair);
      m_pCurrentVertexPair = pPair;
      return TRUE; 
   }
   else if (stricmp( szToken, "-fv" ) == 0)
   {
      if (!m_pCurrentVertexPair)
      {
         String strMsg("Unable to execute Attach command, No vertex pair -vp provided.");
         THROW_FATAL_ERROR(strMsg);
      }

      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Attach command, no from vertex provided.");

      HMainInterface *pMI = GetHMainInterface();
      ASSERT(pMI);

      String strError;
      HVertex *pFromVertex = (HVertex *)pMI->DescriptionToObject( szNextToken, &strError );
      if (!pFromVertex)
      {
         String strMsg("Unable to execute Attach command, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      if (!pFromVertex->IsKindOf(SOT_Vertex))
         THROW_FATAL_ERROR("Unable to execute Attach command, -fv is not a vertex.");

      m_pCurrentVertexPair->SetFromVertex( pFromVertex );
      return TRUE;
   }
   else if (stricmp( szToken, "-tv" ) == 0)
   {
      if (!m_pCurrentVertexPair)
      {
         String strMsg("Unable to execute Attach command, No vertex pair -vp provided.");
         THROW_FATAL_ERROR(strMsg);
      }

      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Attach command, no to vertex provided.");

      HMainInterface *pMI = GetHMainInterface();
      ASSERT(pMI);

      String strError;
      HVertex *pToVertex = (HVertex *)pMI->DescriptionToObject( szNextToken, &strError );
      if (!pToVertex)
      {
         String strMsg("Unable to execute Attach command, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      if (!pToVertex->IsKindOf(SOT_Vertex))
         THROW_FATAL_ERROR("Unable to execute Attach command, -fv is not a vertex.");

      m_pCurrentVertexPair->SetToVertex( pToVertex );

      return TRUE;
   }
   else if (stricmp( szToken, "-fe" ) == 0)
   {
      if (!m_pCurrentVertexPair)
      {
         String strMsg("Unable to execute Attach command, No vertex pair -vp provided.");
         THROW_FATAL_ERROR(strMsg);
      }

      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Attach command, no from edge provided.");

      HMainInterface *pMI = GetHMainInterface();
      ASSERT(pMI);

      String strError;
      HPatchEdge *pFromEdge = (HPatchEdge *)pMI->DescriptionToObject( szNextToken, &strError );
      if (!pFromEdge)
      {
         String strMsg("Unable to execute Attach command, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      if (!pFromEdge->IsKindOf(SOT_PatchEdge))
         THROW_FATAL_ERROR("Unable to execute Attach command, -fe is not a PatchEdge.");

      m_pCurrentVertexPair->SetFromEdge( pFromEdge );
      return TRUE;
   }
   else if (stricmp( szToken, "-te" ) == 0)
   {
      if (!m_pCurrentVertexPair)
      {
         String strMsg("Unable to execute Attach command, No vertex pair -vp provided.");
         THROW_FATAL_ERROR(strMsg);
      }

      const char *szNextToken = tokenlist.GetNext();
      if (!szNextToken)
         THROW_FATAL_ERROR("Unable to execute Attach command, no to edge provided.");

      HMainInterface *pMI = GetHMainInterface();
      ASSERT(pMI);

      String strError;
      HPatchEdge *pToEdge = (HPatchEdge *)pMI->DescriptionToObject( szNextToken, &strError );
      if (!pToEdge)
      {
         String strMsg("Unable to execute Attach command, Reason: %1.");
         strMsg.Replace("%1", strError);
         THROW_FATAL_ERROR(strMsg);
      }

      if (!pToEdge->IsKindOf(SOT_PatchEdge))
         THROW_FATAL_ERROR("Unable to execute Attach command, -te is not a PatchEdge.");

      m_pCurrentVertexPair->SetToEdge( pToEdge );
      return TRUE;
   }

   return FALSE;
}

BOOL AttachCommand::OnValidate( String &strOutError )
{
   if (!CommandInstance::OnValidate( strOutError ))
      return FALSE;

   if (m_vertexpairlist.IsEmpty())
   {
      strOutError = "Unable to execute Attach command, no parameters provided.";
      return FALSE;
   }

   for (POSITION pos = m_vertexpairlist.GetHeadPosition(); pos;)
   {
      VertexPair *pPair = m_vertexpairlist.GetNext(pos);
      if (!pPair->IsValid(strOutError))
         return FALSE;
   }

   return TRUE;
}

UndoType AttachCommand::GetUndoType()
{
   return UNDO_SUPPORTED;
}

BOOL AttachCommand::OnDo(StringList &strResults)
{
   for (POSITION pos = m_vertexpairlist.GetHeadPosition(); pos;)
   {
      POSITION oldpos = pos;
      VertexPair *pPair = m_vertexpairlist.GetNext( pos );

      String strError;
      if (!pPair->Do(m_bAutoFindFaces, strError))
      {
         if (strError.IsEmpty())
            DisplayWarning( "Error in Attach Command.");
         m_vertexpairlist.RemoveAt( oldpos );
      }

      if (!strError.IsEmpty()) // Because in mirror mode the error could be set but the Do was valid
         DisplayWarning( strError );
   }

   return TRUE;
}

void AttachCommand::OnRedo()
{
   for (POSITION pos = m_vertexpairlist.GetHeadPosition(); pos;)
   {
      VertexPair *pPair = m_vertexpairlist.GetNext( pos );
      pPair->Redo();
   }
}

void AttachCommand::OnUndo()
{
   for (POSITION pos = m_vertexpairlist.GetTailPosition(); pos;)
   {
      VertexPair *pPair = m_vertexpairlist.GetPrev( pos );
      pPair->Undo();
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

VertexPair::~VertexPair()
{
   if (m_pAttachInfo)
      delete m_pAttachInfo;

   if (m_pDeleteFaceInfos && m_nNumDeleteFaceInfos)
   {
      for (UINT i=0; i<m_nNumDeleteFaceInfos; i++)
         delete m_pDeleteFaceInfos[i];
      delete [] m_pDeleteFaceInfos;
   }

   if (m_pRedirectInfo)
      delete m_pRedirectInfo;
}

BOOL VertexPair::Do( BOOL bAutoFindFaces, String &strError )
{
   m_pAttachInfo = m_pFromVertex->AttachTo( m_pToVertex, &strError );
   if (m_pAttachInfo)
   {
      if (bAutoFindFaces)
         m_nNumDeleteFaceInfos = m_pToVertex->FindAndCreateFaces( m_pDeleteFaceInfos );

      if (m_pFromEdge && m_pToEdge)
         m_pRedirectInfo = m_pFromEdge->Redirect( m_pToEdge, &strError );
   }

   return m_pAttachInfo!=NULL;
}

void VertexPair::Undo()
{
   if (m_pRedirectInfo)
      m_pRedirectInfo->Undo();

   for (int i=m_nNumDeleteFaceInfos-1; i>=0; i--)
   {
      HDeleteFaceInfo *pInfo = m_pDeleteFaceInfos[i];
      ASSERT(pInfo);
      pInfo->Delete();
   }
   m_pAttachInfo->UnAttach();
}

void VertexPair::Redo()
{
   m_pAttachInfo->ReAttach();

   for (UINT i=0; i<m_nNumDeleteFaceInfos; i++)
   {
      HDeleteFaceInfo *pInfo = m_pDeleteFaceInfos[i];
      ASSERT(pInfo);
      pInfo->UnDelete();
   }

   if (m_pRedirectInfo)
      m_pRedirectInfo->Redo();
}