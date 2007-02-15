#pragma once

#include "HMotionCurveDriver.h"

class HFloatMotionKey;

class SNIPEEXPORT HFloatMotionCurveDriver : public HMotionCurveDriver
{
public:
   HFloatMotionKey *StoreValue( float value, const Time &time, BOOL bStoreUndo = TRUE );
   float GetValue( const Time &time) const;
};

class SNIPEEXPORT HVectorMotionCurveDriver : public HMotionCurveDriver
{
public:
   void StoreValue( Vector value, const Time &time, BOOL bStoreUndo = TRUE );
   Vector GetValue( const Time &time) const;
};

class SNIPEEXPORT HVector2MotionCurveDriver : public HMotionCurveDriver
{
public:
   void StoreValue( Vector2 value, const Time &time, BOOL bStoreUndo = TRUE );
   Vector2 GetValue( const Time &time) const;
};

class Vector4;

class SNIPEEXPORT HVector4MotionCurveDriver : public HMotionCurveDriver
{
public:
   void StoreValue( Vector4 value, const Time &time, BOOL bStoreUndo = TRUE );
   Vector4 GetValue( const Time &time) const;
};

class SNIPEEXPORT HEulerMotionCurveDriver : public HVectorMotionCurveDriver
{
public:
};

class SNIPEEXPORT HQuaternionMotionCurveDriver : public HVector4MotionCurveDriver
{
public:
   void StoreValue( Quaternion value, const Time &time, BOOL bStoreUndo = TRUE );
   Quaternion GetValue( const Time &time) const;
};
