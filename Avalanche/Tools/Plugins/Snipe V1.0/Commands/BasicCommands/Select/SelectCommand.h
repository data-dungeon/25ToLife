#pragma once
#include "SelectBaseCommand.h"

class HComponent;
class HParentableObject;
class MemoryStream;
class HSnipeObjectList;
class HEdge;
class HPoly;

class SelectCommand : public SelectBaseCommand
{
protected:
   HSnipeObjectList *m_pickedobjects;
   enum nSelectionType { REPLACE, ADD, SUBTRACT, TOGGLE, CLEAR } m_nSelectionType;

public:
   SelectCommand(HCommandPluginHelper *pPlugin) :
    SelectBaseCommand(pPlugin)
   {
      m_nSelectionType = REPLACE;
      m_pickedobjects = NULL;
   }
   virtual ~SelectCommand();

   // PluginCommand Overrides
   virtual BOOL ParseToken(const char *szToken, TokenList &tokenlist);
   virtual BOOL OnValidate( String &strOutError );
   virtual BOOL OnDo(StringList &strResults);
};

class SelectCommandPlugin : public HCommandPluginHelper
{
public:
   DECLARE_COMMAND_PLUGINHELPER( SelectCommandPlugin, HCommandPluginHelper )

//HCommandPluginHelper overrides
   virtual CommandInstance *CreateCommandInstance();
   virtual UINT GetBitmapID() const;
   virtual String GetHelp() const;
   virtual const char *GetCLIName() const; // Name used in scripts and commandline

//HPluginHelper overrides
   virtual const char *GetTitleName() const;
};
