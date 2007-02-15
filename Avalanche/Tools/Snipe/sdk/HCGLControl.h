#pragma once

#include "HCCmdTarget.h"

class SNIPEEXPORT HCGLControl : public HCCmdTarget
{
public:
   void Push2DAttributes();
   void PopAttributes();

   void Invalidate();

   CSize GetTextExtent( const String &text );
   void DrawText( float x, float y, const String &text );
};
