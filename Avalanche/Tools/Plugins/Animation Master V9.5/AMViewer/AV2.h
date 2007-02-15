// Dan  4/11/00  \Dan85\AMViewer\AV2.h

#define __AV2_H

#ifndef __MODEL_H
#include "Model.h"
#endif

class AV2Model : public PatchModel
{
   virtual void Serialize(CArchive &ar);
};
