#pragma once

#include "HTessPropertyPoly.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HUV;
class HUVPatch;

class SNIPEEXPORT HTessUVPoly : public HTessPropertyPoly
{
public:
   HUV *GetUVAt( UINT nIndex );
   HUVPatch *GetUVPatch() const;
};
