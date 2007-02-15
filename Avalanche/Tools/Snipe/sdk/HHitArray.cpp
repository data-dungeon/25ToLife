//

#include "stdafx.h"
#include "HHitArray.h"
#include "Interface/GLHitArray.h"

// HPickHit
HPickHit *HPickHit::New()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   PickHit *pPickHit = SNEW PickHit;
   return (HPickHit *)pPickHit;
}

UINT HPickHit::GetZMin() const
{
   return ((PickHit *)this)->GetZMin();
}

UINT HPickHit::GetZMax() const
{
   return ((PickHit *)this)->GetZMax();
}

UINT HPickHit::GetNumNames() const
{
   return ((PickHit *)this)->GetNumNames();
}

UINT HPickHit::GetName(int nIndex) const
{
   const PickHit *pPickHit = (const PickHit *)this;
   return (*pPickHit)[nIndex];
}

void HPickHit::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   delete (PickHit *)pVoid;
}

// HPickHitArray
HPickHitArray *HPickHitArray::New()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   PickHitArray *pPickHitArray = SNEW PickHitArray;
   return (HPickHitArray *)pPickHitArray;
}

UINT HPickHitArray::GetNumHits() const
{
   return ((PickHitArray *)this)->GetNumHits();
}

HPickHit *HPickHitArray::GetHit( UINT nIndex ) const
{
   const PickHitArray *pHitArray = (const PickHitArray *)this;
   const PickHit *pPickHit = &(*pHitArray)[nIndex];
   return (HPickHit *) pPickHit;
}

void HPickHitArray::operator delete (void *pVoid)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   delete (PickHitArray *)pVoid;
}
