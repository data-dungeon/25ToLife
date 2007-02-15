#pragma once

#include "GLViewControl.h"

class MotionChannelBiasObject;

class CGLMotionChannelObjectBiasViewControl : public CGLViewControl
{
public:
   CGL3DFloatPropertyViewControl m_TranslateSlider;
   CGL3DFloatPropertyViewControl m_RotateSlider;
   CGL3DFloatPropertyViewControl m_ScaleSlider;

   MotionChannelBiasObject *m_pMotionChannelBiasObject;

public:
   CGLMotionChannelObjectBiasViewControl( CGLBaseView *pBaseView, MotionChannelBiasObject *pMotionChannelBiasObject );

   // CGLViewControl overrides
   virtual BOOL IsFor( void *pData )
   {
      return pData == m_pMotionChannelBiasObject;
   }
};
