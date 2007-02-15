
#include "Stdafx.h"
#include "HashPlugin.h"
#include "Av2.h"
//#include "HPatchModel.h"
class HPatchModel;

Av2Translator::Av2Translator() 
{
}

void Av2Translator::OnImport(HPolyModel *model, const char *strFileName)
{
   AfxMessageBox("Import AV2", MB_OK);
}