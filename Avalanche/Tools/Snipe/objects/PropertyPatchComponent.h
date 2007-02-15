#pragma once

#include "PropertyComponent.h"

class PropertyPatchComponent : public PropertyComponent
{
public:
   DECLARE_COMPONENT_CREATE(PropertyPatchComponent, PropertyComponent)

            PropertyPatchComponent();
            PropertyPatchComponent( UINT nID );
   virtual ~PropertyPatchComponent();
};


