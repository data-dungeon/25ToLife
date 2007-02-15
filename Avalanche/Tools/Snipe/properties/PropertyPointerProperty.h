#pragma once
#include "Property.h"

// Pointer Properties
class PropertyPointerProperty : public PointerProperty<Property *>
{
public:
   DECLARE_SNIPEOBJECT_CREATE( PropertyPointerProperty, PointerProperty<Property *> );
};

BaseStream &operator << (BaseStream &bs, const Property *&value );
BaseStream &operator >> (BaseStream &bs, Property *&value );

