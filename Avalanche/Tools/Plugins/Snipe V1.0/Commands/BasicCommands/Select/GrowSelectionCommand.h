#pragma once
#include "SelectBaseCommand.h"

class HSnipeObjectList;
class HEdge;

class GrowSelectionCommand : public SelectBaseCommand
{
public:
   GrowSelectionCommand(HCommandPluginHelper *pPlugin) : SelectBaseCommand( pPlugin ) {}

   void GrowVertex( HVertex *pVertex, HSnipeObjectList *pList );
   void GrowEdge( HEdge *pEdge, HSnipeObjectList *pList );
   void GrowFace( HPoly *pFace, HSnipeObjectList *pList );

//PluginCommand Overrides
   virtual BOOL OnDo(StringList &strResults);
};

class GrowSelectionCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( GrowSelectionCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShrinkSelectionCommand : public SelectBaseCommand
{
public:
   ShrinkSelectionCommand(HCommandPluginHelper *pPlugin) : SelectBaseCommand( pPlugin ) {}

   void ShrinkVertex( HVertex *pVertex, HSnipeObjectList *pList );
   void ShrinkEdge( HEdge *pEdge, HSnipeObjectList *pList );
   void ShrinkFace( HPoly *pFace, HSnipeObjectList *pList );

   //PluginCommand Overrides
   virtual BOOL OnDo(StringList &strResults);
};

class ShrinkSelectionCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( ShrinkSelectionCommandPlugin, HCommandPluginHelper )

   //HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

   //HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
