#pragma once

#include "ModelTranslatorEntry.h"
class PolyModel;

class ModelTranslatorManager : public CList<ModelTranslatorEntry *>
{
public:
	         ModelTranslatorManager();
	virtual ~ModelTranslatorManager(); // We don't need to delete the entries, the SnipePlugin will do it for us since it points to the same entries

   PolyModel *Import();
   BOOL QueryFormat(ModelTranslatorEntry **pOutEntry, String &strOutFileName );
};

