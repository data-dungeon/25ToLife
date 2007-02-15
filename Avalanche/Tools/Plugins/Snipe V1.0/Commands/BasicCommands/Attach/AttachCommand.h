#pragma once
#include "HCommandPluginHelper.h"
#include "HVertex.h"
#include "HPatchEdge.h"

class VertexPair
{
private:
   HVertex::HAttachInfo *m_pAttachInfo;
   HPatchEdge::HRedirectInfo *m_pRedirectInfo;
   HDeleteFaceInfo **m_pDeleteFaceInfos;
   UINT m_nNumDeleteFaceInfos;

   HVertex *m_pFromVertex;
   HVertex *m_pToVertex;
   HPatchEdge *m_pFromEdge; // For Continuity
   HPatchEdge *m_pToEdge;

public:
   VertexPair()
   {
      m_pFromVertex = NULL;
      m_pToVertex = NULL;
      m_pFromEdge = NULL;
      m_pToEdge = NULL;

      m_pAttachInfo = NULL;
      m_pDeleteFaceInfos = NULL;
      m_nNumDeleteFaceInfos = 0;
      m_pRedirectInfo = NULL;
   }

   ~VertexPair();

   void SetFromVertex(HVertex *pVertex) { m_pFromVertex = pVertex; }
   void SetToVertex(HVertex *pVertex) { m_pToVertex = pVertex; }
   void SetFromEdge(HPatchEdge *pEdge) { m_pFromEdge = pEdge; }
   void SetToEdge(HPatchEdge *pEdge) { m_pToEdge = pEdge; }

   HVertex *GetFromVertex() const { return m_pFromVertex; } 
   HVertex *GetToVertex() const { return m_pToVertex; } 
   HEdge *GetFromEdge() const { return m_pFromEdge; } 
   HEdge *GetToEdge() const { return m_pToEdge; }

   BOOL IsValid( String &strOutError )
   {
      if (!m_pFromVertex)
      {
         strOutError = "No From Vertex provided.";
         return FALSE;
      }
      if (!m_pToVertex)
      {
         strOutError = "No To Vertex provided.";
         return FALSE;
      }
      if (m_pFromEdge && !m_pToEdge)
      {
         strOutError = "No To Edge provided.";
         return FALSE;
      }
      if (m_pToEdge && !m_pFromEdge)
      {
         strOutError = "No From Edge provided.";
         return FALSE;
      }

      return TRUE;
   }

   BOOL Do( BOOL bAutoFindFaces, String &strError ); 
   void Undo(); 
   void Redo(); 
};

class AttachCommand : public CommandInstance
{
public:
   CList<VertexPair *> m_vertexpairlist;
   BOOL m_bAutoFindFaces;
   VertexPair *m_pCurrentVertexPair;

   AttachCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_bAutoFindFaces = TRUE;
      m_pCurrentVertexPair = NULL;
   }

   virtual ~AttachCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class AttachCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( AttachCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
