#pragma once

#include "HPropertyVertex.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class SNIPEEXPORT HColorVertex : public HPropertyVertex
{
public:
   const RGBAFloat &GetColor() const;
   void SetColor( const RGBAFloat &color );
};