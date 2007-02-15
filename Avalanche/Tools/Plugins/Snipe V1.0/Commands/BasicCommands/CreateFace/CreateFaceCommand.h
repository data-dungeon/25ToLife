#pragma once
#include "HCommandPluginHelper.h"

class HVertex;
class HEdge;
class HDeleteFaceInfo;

class CreateFaceCommand : public CommandInstance
{
public:
   UINT m_nMaxSides;
   CList<HVertex *> m_vertexlist;
   CList<HEdge *> m_edgelist;
   CList<HDeleteFaceInfo *> m_deletefaceonfos;

   CreateFaceCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_nMaxSides = 20;
   }

   ~CreateFaceCommand();

   // PluginCommand Overrides
   virtual UndoType GetUndoType();
   virtual BOOL ParseToken(const char *szToken, TokenList &tokenlist );
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnRedo();
   virtual void OnUndo();
};

class CreateFaceCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( CreateFaceCommandPlugin, HCommandPluginHelper )

   CreateFaceCommandPlugin() 
   {
   }
//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};