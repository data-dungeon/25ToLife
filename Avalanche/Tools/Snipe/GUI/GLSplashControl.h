#pragma once

#include "GLControls.h"

class CGLSplashControl : public CGLImageControl
{
public:
   float m_fOpacity;
   float m_fMinSecondsToDisplay;
   float m_fStartTime;

            CGLSplashControl( CGLRenderWnd *pglRenderWnd, float fMinSecondsToDisplay=2.0f );
   virtual ~CGLSplashControl();

// CGLControl overrides
   virtual void Draw( UINT nDrawFlags=DRAW_CHILDREN|DRAW_PARENT );
};

