#include "stdafx.h"
#include "UV.h"
#include "UVPoly.h"
#include "HPoly.h"
#include "HUV.h"
#include "HTexCoords.h"
#include "TexCoords.h"


UINT HTexCoords::GetNumUVs()
{
   return ((TexCoords *)this)->GetNumUVs();
}

HUV *HTexCoords::AddUV()
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HUV*)((TexCoords *)this)->AddUV();
}

HUV *HTexCoords::AddUV( const Vector2 &pos )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HUV*)((TexCoords *)this)->AddUV(pos);
}

void HTexCoords::RemoveUV(HUV *pUV)
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((TexCoords *)this)->RemoveUV((UV*)pUV);
}

UINT HTexCoords::GetNumUVPolys()
{
   return ((TexCoords *)this)->GetNumUVPolys();
}

HUVPoly *HTexCoords::AddPoly( HPoly *pPoly )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   return (HUVPoly *)((TexCoords *)this)->AddPoly( (Poly*)pPoly );
}

void HTexCoords::RemovePoly( HUVPoly *pUVPoly )
{
   AFX_MANAGE_STATE(AfxGetAppModuleState());
   ((TexCoords *)this)->RemovePoly( (UVPoly *)pUVPoly );
}

UINT HTexCoordsContainer::GetNumTexCoords() const
{
   return ((TexCoordsContainer *)this)->GetNumTexCoords();
}

HTexCoords *HTexCoordsContainer::GetTexCoordsAt( UINT nIndex )
{
   return (HTexCoords*)((TexCoordsContainer *)this)->GetTexCoordsAt( nIndex );
}