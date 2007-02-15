//

#pragma once

#include "GLControls.h"

class CGLMenuBarControl : public CGLControl
{
public:
// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

