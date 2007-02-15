#pragma once

#include "HSnipeObject.h"

class SNIPEEXPORT HComponent : public HSnipeObject
{
public:
   UINT GetID();

   const char *GetCommandLineName() const;

   char *Dump()
   {
      const int cMaxResult = 128; 
      static char Result[cMaxResult+1]; 

      _snprintf( Result, cMaxResult, "id: %d", GetID() ); 

      return Result;
   }
};