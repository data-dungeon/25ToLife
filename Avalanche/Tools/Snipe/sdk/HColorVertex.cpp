#include "stdafx.h"
#include "HColorVertex.h"
#include "ColorVertex.h"

const RGBAFloat &HColorVertex::GetColor() const 
{ 
   return ((ColorVertex *)this)->GetColor();
}

void HColorVertex::SetColor( const RGBAFloat &color )
{
   ((ColorVertex *)this)->SetColor(color);
}

