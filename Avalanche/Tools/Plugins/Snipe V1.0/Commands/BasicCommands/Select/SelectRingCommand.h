#pragma once
#include "SelectBaseCommand.h"

class HEdge;

class SelectRingCommand : public SelectBaseCommand
{
public:
   CList<HEdge *> m_edgelist;
   SelectRingCommand(HCommandPluginHelper *pPlugin) : SelectBaseCommand( pPlugin ) {}

   void SelectRing( HPoly *pPoly, HEdge *pEdge, HSnipeObjectList *pList);

//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
};

class SelectRingCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( SelectRingCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
