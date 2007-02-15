#pragma once

#include "HComponent.h"

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class SNIPEEXPORT HMotionKey : public HComponent
{
public:
   enum InterpolationMethod { LINEAR, SPLINE, HOLD };

   void SetTime(const Time &time);
   const Time &GetTime();

   HMotionKey *GetNextKey();
   HMotionKey *GetPrevKey();
};

class SNIPEEXPORT HFloatMotionKey : public HMotionKey
{
public:
   void SetValue(float value);
   float GetValue();

   void SetInTangent(const Vector2 &tangent);
   Vector2 GetInTangent();
   void SetOutTangent(const Vector2 &tangnent);
   Vector2 GetOutTangent();

   enum InterpolationMethod { LINEAR, SPLINE, HOLD };
   void SetInterpolationMethod(InterpolationMethod interpolationmethod);
   InterpolationMethod GetInterpolationMethod();
};

class SNIPEEXPORT HStringMotionKey : public HMotionKey
{
public:
   void SetValue(const String &value);
   String GetValue();
};
