//

#include "stdafx.h"
#include "HTimeProperty.h"
#include "Properties/Property.h"

// TimeProperty
void HTimeProperty::StoreValue( const Time &value, const Time &time, BOOL bStoreUndo /*= TRUE*/)
{
   ((TimeProperty *)this)->StoreValue( value, time, bStoreUndo );
}

void HTimeProperty::StoreValue( const Time &value, BOOL bStoreUndo /*= TRUE*/ )
{
   ((TimeProperty *)this)->StoreValue( value, GetTime(), bStoreUndo );
}

Time HTimeProperty::GetValue() const
{
   return ((TimeProperty *)this)->GetValue();
}

Time HTimeProperty::GetValue(const Time &time) const
{
   return ((TimeProperty *)this)->GetValue(time);
}