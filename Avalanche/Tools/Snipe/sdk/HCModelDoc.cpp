// 
#include "stdafx.h"
#include "HCModelDoc.h"
#include "../GUI/ModelDoc.h"


HPolyModel *HCModelDoc::GetCurrentModel() const
{
   return (HPolyModel *) ((CModelDoc *)this)->GetCurrentModel(); 
}