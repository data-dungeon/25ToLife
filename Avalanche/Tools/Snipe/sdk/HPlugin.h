#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class SNIPEEXPORT HPlugin
{
public:
   void SetClassName( const char *szClassName );
   const char *GetClassName() const;
   void *GetPluginData();
   void SetTitleName( const char *szTitleName );
   const char *GetTitleName() const;
};