#pragma once
#include "TexCoords.h"
#include "Property.h"

class TexCoordsPointerProperty : public PointerProperty<TexCoords *>
{
public:
   DECLARE_SNIPEOBJECT_CREATE( TexCoordsPointerProperty, PointerProperty<TexCoords *> );

// SnipeObject overrides
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
};

BaseStream &operator << (BaseStream &bs, const TexCoords *&value );
BaseStream &operator >> (BaseStream &bs, TexCoords *&value );
