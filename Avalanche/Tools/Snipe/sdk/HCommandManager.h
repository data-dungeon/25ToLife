#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HCommandEntry;

class SNIPEEXPORT HCommandManager
{
public:
   HCommandEntry *FindCommand(const char *szPluginName, const char *szClassName);

   POSITION GetHeadPosition();
   HCommandEntry *GetNext(POSITION pos);
};