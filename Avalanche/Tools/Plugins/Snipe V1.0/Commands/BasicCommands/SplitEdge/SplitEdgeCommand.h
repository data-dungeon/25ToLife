#pragma once
#include "HCommandPluginHelper.h"
#include "HVertex.h"
#include "HSnipeObjectList.h"

////////////////////////////////////////////////////////////////////////////////////

class HEdge;

class SplitEdgeCommand : public CommandInstance
{
public:
   UINT m_nSegments;
   CList< HEdge *> m_edges;

   CList< HVertex::HDeleteInfo *> m_DeleteInfoList;
 
   HSnipeObjectList *m_pNewSelectionList;
   HSnipeObjectList *m_pOldSelectionList;
   HSnipeObject *m_pOldFocusObject;
   HSnipeObject *m_pOldNearFocusObject;

   SplitEdgeCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_nSegments = 2;
      m_pNewSelectionList = HSnipeObjectList::New();
      m_pOldSelectionList = NULL;
      m_pOldFocusObject = NULL;
      m_pOldNearFocusObject = NULL;
   }

   ~SplitEdgeCommand();

   // PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class SplitEdgeCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( SplitEdgeCommandPlugin, HCommandPluginHelper )

   //HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};