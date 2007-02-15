// Dan  4/11/00  \Dan85\AMViewer\AV3.h

#define __AV4_H

#ifndef __MODEL_H
#include "Model.h"
#endif

class AV4Model : public PatchModel
{
   virtual void Serialize(CArchive &ar);
};
