#pragma once

#include "HCDocument.h"
#include "Enum.h"

class HSelectionList;
class HHierObject;

class SNIPEEXPORT HCBaseDoc : public HCDocument
{
public:
   HSelectionList *GetSelectionList() const;
   SnipeDocType GetDocType();
   HHierObject *GetObjectHead();
};