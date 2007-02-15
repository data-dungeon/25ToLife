#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HCommandPlugin;

class SNIPEEXPORT HCommandPluginManager
{
public:
   HCommandPlugin *FindCommandByClassName(const char *szPluginName, const char *szClassName);
   HCommandPlugin *FindCommandByCLIName(const char *szPluginName, const char *szCLIName);

   POSITION GetHeadPosition();
   HCommandPlugin *GetNext(POSITION pos);
};