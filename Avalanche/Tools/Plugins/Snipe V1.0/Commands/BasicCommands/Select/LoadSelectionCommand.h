#pragma once
#include "SelectBaseCommand.h"

class HSnipeObjectList;
class HTexCoords;
class HSurface;
class HSelection;
class HBone; 
class HPropertySet;

class LoadSelectionCommand : public SelectBaseCommand
{
public:
   HSnipeObjectList *m_pObjectList, *m_pSelectionList;
   enum nSelectionType { REPLACE, ADD, SUBTRACT, TOGGLE } m_nSelectionType;

   LoadSelectionCommand(HCommandPluginHelper *pPlugin);
   virtual ~LoadSelectionCommand();

   void LoadSelection( HSelection *pSelection );
   void LoadSelection( HSurface *pSurface );
   void LoadSelection( HPropertySet *pPropertySet );
   void LoadSelection( HBone *pBone );

//PluginCommand Overrides
   virtual BOOL ParseToken( const char *szToken, TokenList &tokenlist );
   virtual BOOL OnDo(StringList &strResults);
};

class LoadSelectionCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( LoadSelectionCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const;

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
