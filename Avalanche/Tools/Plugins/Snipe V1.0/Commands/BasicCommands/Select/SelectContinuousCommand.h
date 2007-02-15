#pragma once
#include "SelectBaseCommand.h"

class HSnipeObjectList;
class HEdge;

void GetContinuousVertices( HEdge *pStartEdge, HSnipeObjectList *pList );
void GetContinuousEdges( HEdge *pStartEdge, HSnipeObjectList *pList );

class SelectContinuousCommand : public SelectBaseCommand
{
public:
   SelectContinuousCommand(HCommandPluginHelper *pPlugin) : SelectBaseCommand( pPlugin ) {}

//PluginCommand Overrides
   virtual BOOL OnDo(StringList &strResults);
};

class SelectContinuousCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( SelectContinuousCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
