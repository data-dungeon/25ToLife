#pragma once

#include "HProperty.h"

class SNIPEEXPORT HTimeProperty : public HProperty
{
public:
   void StoreValue( const Time &value, const Time &time, BOOL bStoreUndo = TRUE );
   void StoreValue( const Time &value, BOOL bStoreUndo = TRUE );
   Time GetValue( const Time &) const;
   Time GetValue() const;
};