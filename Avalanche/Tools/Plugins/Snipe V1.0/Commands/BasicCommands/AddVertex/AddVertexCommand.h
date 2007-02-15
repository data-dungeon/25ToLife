#pragma once
#include "../Select/SelectBaseCommand.h"
#include "HEdge.h"
#include "Vector.h"
#include "HPoly.h"

class HPolyModel;

class NewVertexParameter
{
public:
   // Overridables
   // Pure

   virt_base virtual void AddVertex( HVertex *&pOutVertex, HVertex::HDeleteInfo *&pOutDeleteInfo ) = 0;
};

class NewVertexAlongEdgeParameter : public NewVertexParameter
{
public:
   HEdge *m_pEdge;
   float m_fPos;

   NewVertexAlongEdgeParameter( HEdge *pEdge, float fPos )
   {
      m_pEdge = pEdge;
      m_fPos = fPos;
   }

   // NewVertexParameter overrides
   virtual void AddVertex( HVertex *&pOutVertex, HVertex::HDeleteInfo *&pOutDeleteInfo );
};

class NewVertexAtPosParameter : public NewVertexParameter
{
public:
   HPolyModel *m_pModel;
   Vector m_vPos;

   NewVertexAtPosParameter( HPolyModel *pModel, Vector &vPos )
   {
      m_pModel = pModel;
      m_vPos = vPos;
   }

   // NewVertexParameter overrides
   virtual void AddVertex( HVertex *&pOutVertex, HVertex::HDeleteInfo *&pOutDeleteInfo );
};

class UseVertexParameter : public NewVertexParameter
{
public:
   HVertex *m_pVertex;

   UseVertexParameter( HVertex *pVertex )
   {
      m_pVertex = pVertex;
   }

   // NewVertexParameter overrides
   virtual void AddVertex( HVertex *&pOutVertex, HVertex::HDeleteInfo *&pOutDeleteInfo )
   {
      pOutDeleteInfo = NULL;
      pOutVertex = m_pVertex;
   }
};

/////////////////////////////////////////////////////////////////////////

class AddVertexCommand : public SelectBaseCommand
{
protected:
   // Parameters
   CList< NewVertexParameter *> m_parameterlist;
   HEdge *m_pPreviousContinuity;
   HEdge *m_pNextContinuity;
   BOOL m_bFindFaces;
   BOOL m_bAutoLoop; // if Vertices are added that could form a loop, go ahead and connect the loop with an edge
   HSnipeObjectList *m_pNewSelectedVerts;
   HVertex *m_pFocusVertex;
   HEdge *m_pNearEdge;

   enum SelectionType { ST_NONE, ST_ALL, ST_LAST };
   SelectionType m_selectiontype;


   // Undo/Redo
   CList<HVertex::HDeleteInfo *> m_newvertexdeleteinfos;
   HEdge::HDeleteInfo **m_pNewEdgeDeleteInfos;
   int m_nNumNewEdgeDeleteInfos;

   CList<HDeleteFaceInfo *> m_newdeletefaceinfos;

public:
   AddVertexCommand(HCommandPluginHelper *pPlugin) :
    SelectBaseCommand(pPlugin)
   {
      m_pNewEdgeDeleteInfos = NULL;
      m_nNumNewEdgeDeleteInfos = 0;
      m_pPreviousContinuity = HEDGE_TERMINATE;
      m_pNextContinuity = HEDGE_TERMINATE;
      m_bFindFaces = TRUE;
      m_selectiontype = ST_NONE;

      m_pNewSelectedVerts = NULL;
      m_pFocusVertex = NULL;
      m_pNearEdge = NULL;
      m_bAutoLoop = FALSE;
   }

   virtual ~AddVertexCommand();

//PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
   virtual void OnRedo();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
};

class AddVertexCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( AddVertexCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
