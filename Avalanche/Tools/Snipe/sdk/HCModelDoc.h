#pragma once

#include "HCBaseDoc.h"

class HPolyModel;

class SNIPEEXPORT HCModelDoc : public HCBaseDoc
{
public:
   HPolyModel *GetCurrentModel() const;
};