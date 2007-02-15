#pragma once

#include "HProperty.h"
#include "HStringProperty.h"

class SNIPEEXPORT HFileNameProperty : public HStringProperty
{
public:
};

class SNIPEEXPORT HFileInfoProperty : public HProperty
{
public:
   HFileNameProperty *GetFileNameProperty();
};