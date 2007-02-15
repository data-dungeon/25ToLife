#include "stdafx.h"
#include "PropertyPointerProperty.h"

/////////////////////////////////////////
// PropertyPointerProperty
IMPLEMENT_SNIPEOBJECT_CREATE( PropertyPointerProperty );

BaseStream &operator << ( BaseStream &bs, const Property *&value )
{
   return operator << ( bs, (const SnipeObject * &) value);
}

BaseStream &operator >> ( BaseStream &bs, Property *&value )
{
   return operator >> (bs, (SnipeObject *&) value );
}

