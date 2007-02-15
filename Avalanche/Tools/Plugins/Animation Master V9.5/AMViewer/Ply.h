// Dan  4/11/00  \Dan85\AMViewer\Ply.h

#define __PLY_H

#ifndef __MODEL_H
#include "Model.h"
#endif

class PLYModel : public PolyModel
{
   virtual void Serialize(CArchive &ar);
};

