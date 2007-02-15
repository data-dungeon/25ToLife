#pragma once

#include "HComponent.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HPolyModel;

class SNIPEEXPORT HVertexNormal : public HComponent
{
public:
   void SetDir( const Vector &direction );
   const Vector &GetDir() const;
   HPolyModel *GetModel();
};