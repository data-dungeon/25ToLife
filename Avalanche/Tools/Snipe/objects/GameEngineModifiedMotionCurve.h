#pragma once

#include "ModifiedMotionCurve.h"

class GameEngineModifiedMotionCurve : public ModifiedMotionCurve
{
public:
   GameEngineModifiedMotionCurve() {}
   virtual ~GameEngineModifiedMotionCurve() {}

// ModifiedMotionCurve overrides
   virtual void Rebuild();
};

class GemsModifiedMotionCurve : public ModifiedMotionCurve
{
public:
            GemsModifiedMotionCurve() {}
   virtual ~GemsModifiedMotionCurve() {}

public:
// ModifiedMotionCurve overrides
   virtual void Rebuild();
};
