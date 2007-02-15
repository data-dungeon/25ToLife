#include "stdafx.h"
#include "HPatchVertex.h"
#include "PatchVertex.h"

class PatchEdge;

HPatchModel *HPatchVertex::GetModel()
{
   return (HPatchModel*)((PatchVertex *)this)->GetOwner();
}

BOOL HPatchVertex::CanConstrain( HPatchEdge *pEdge )
{
   return ((PatchVertex *)this)->CanConstrain( (PatchEdge *)pEdge );
}

BOOL HPatchVertex::SetConstrained( HPatchEdge *pEdge )
{
   return ((PatchVertex *)this)->SetConstrained( (PatchEdge *)pEdge );
}

BOOL HPatchVertex::IsConstrained() const 
{ 
   return ((PatchVertex *)this)->IsConstrained();
}

BOOL HPatchVertex::GetConstrainedBetween( HPatchEdge *&pEdge0, HPatchEdge *&pEdge1)
{
   return ((PatchVertex *)this)->GetConstrainedBetween( (PatchEdge *&)pEdge0, (PatchEdge *&)pEdge1);
}
