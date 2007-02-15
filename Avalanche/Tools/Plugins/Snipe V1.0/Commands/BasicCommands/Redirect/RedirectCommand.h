#pragma once
#include "HCommandPluginHelper.h"
#include "HPatchEdge.h"

class HPatchVertex;
class HRedirectInfo;

class RedirectCommand : public CommandInstance
{
public:
   HPatchEdge *m_pFromEdge;
   HPatchEdge *m_pToEdge;
   HPatchVertex *m_pTermVertex;
   HPatchEdge::HRedirectInfo *m_pRedirectInfo;

   RedirectCommand(HCommandPluginHelper *pPlugin) :
    CommandInstance(pPlugin)
   {
      m_pFromEdge = NULL;
      m_pToEdge = NULL;
      m_pTermVertex = NULL;
      m_pRedirectInfo = NULL;
   }

   virtual ~RedirectCommand();
  
//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate(  String &strOutError );
   virtual UndoType GetUndoType();
   virtual BOOL OnDo(StringList &strResults);
   virtual void OnUndo();
   virtual void OnRedo();
};

class RedirectCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( RedirectCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
