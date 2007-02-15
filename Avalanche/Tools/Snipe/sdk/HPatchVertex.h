#pragma once

#include "HVertex.h"

class HPatchModel;
class HPatchEdge;

class SNIPEEXPORT HPatchVertex : public HVertex
{
public:
   HPatchModel *GetModel();

   BOOL CanConstrain( HPatchEdge *pEdge );
   BOOL SetConstrained( HPatchEdge *pEdge );
   BOOL IsConstrained() const;
   BOOL GetConstrainedBetween( HPatchEdge *&pEdge0, HPatchEdge *&pEdge1);
};