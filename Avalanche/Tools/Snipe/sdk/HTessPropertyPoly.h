#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HPropertyPatch;

class SNIPEEXPORT HTessPropertyPoly
{
public:
   HTessPropertyPoly *GetNextTessPropertyPoly();
   HPropertyPatch *GetPropertyPatch();
};
