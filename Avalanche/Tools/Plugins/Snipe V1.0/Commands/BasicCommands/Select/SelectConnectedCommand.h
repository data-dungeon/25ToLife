#pragma once
#include "SelectBaseCommand.h"

class HSnipeObjectList;
class HVertex;
class HEdge;
class HPoly;

typedef CMap<HEdge *, HEdge *, HEdge *, HEdge *> CEdgeMap;
typedef CMap<HPoly *, HPoly *, HPoly *, HPoly *> CPolyMap;

class SelectConnectedCommand : public SelectBaseCommand
{
public:
   SelectConnectedCommand(HCommandPluginHelper *pPlugin) : SelectBaseCommand( pPlugin ) {}
  
   void SelectConnectedVertex( HSnipeObjectList *pList, CEdgeMap &edgemap, HVertex *pVertex );
   void SelectConnectedEdge( HSnipeObjectList *pList, CEdgeMap &edgemap, HEdge *pEdge );
   void SelectConnectedFace( HSnipeObjectList *pList, CPolyMap &polymap, HPoly *pPoly );

//PluginCommand Overrides
   virtual BOOL OnDo(StringList &strResults);
};

class SelectConnectedCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( SelectConnectedCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
