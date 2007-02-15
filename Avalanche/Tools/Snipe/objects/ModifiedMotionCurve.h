#pragma once

#include "MotionCurve.h"

class ModifiedMotionCurve : public FloatMotionCurve
{
public:
   FloatMotionCurve *m_pMotionCurve;

   ModifiedMotionCurve() {}
   virtual ~ModifiedMotionCurve() {}

   // Overrides
   virt_base virtual void Rebuild() {}
};
