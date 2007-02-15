#pragma once

#include "PluginManager.h"

class CommandPlugin;
class CommandPluginManager : public PluginManager
{
private:
   void Validate( CommandPlugin *pPlugin ); // Throws exception if it can't validate
   BOOL RemoveCommand( CommandPlugin *pPlugin );

public:
	         CommandPluginManager();
	virtual ~CommandPluginManager(); // We don't need to delete the entries, the SnipePlugin will do it for us since it points to the same entries

   CommandPlugin *FindCommandByClassName(const String &strPluginName, const String &strClassName);
   CommandPlugin *FindCommandByCLIName(const String &strPluginName, const String &strCLIName);
   void ValidateAll();

   // PluginManager overrides
   virtual String GetFolderName() const { return "Commands"; }
   virtual void InitPlugins();
};

