#pragma once

#include "CommandEntry.h"

class CommandManager : public CList<CommandEntry *>
{
private:
   void Validate( CommandEntry *pEntry ); // Throws exception if it can't validate
   BOOL RemoveCommand( CommandEntry *pEntry );

public:
	         CommandManager();
	virtual ~CommandManager(); // We don't need to delete the entries, the SnipePlugin will do it for us since it points to the same entries

   CommandEntry *FindCommand(const String &strPluginName, const String &strClassName);
   void ValidateAll();
};

