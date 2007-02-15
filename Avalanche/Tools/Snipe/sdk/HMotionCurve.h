#pragma once

#include "HHierObject.h"

class HStringMotionKey;
class HFloatMotionKey;
class HMotionKey;
class HModifiedMotionCurve;

class SNIPEEXPORT HMotionCurve
{
public:
   Time GetStartTime();
   Time GetStopTime();
   UINT CountNumKeys();
   HMotionKey *GetHeadKey() const;
   HMotionKey *GetTailKey() const;
   HMotionKey *GetLastKey() const;
};

class SNIPEEXPORT HFloatMotionCurve : public HMotionCurve
{
public:
   HFloatMotionKey *StoreValue( const float &value, const Time &time = GetTime(), BOOL bStoreUndo = TRUE);
   float GetValue( const Time &time = GetTime() );
};

class SNIPEEXPORT HStringMotionCurve : public HMotionCurve
{
public:
   HStringMotionKey *StoreValue( const String &value, const Time &time = GetTime(), BOOL bStoreUndo = TRUE);
   String GetValue( const Time &time = GetTime() );
};

