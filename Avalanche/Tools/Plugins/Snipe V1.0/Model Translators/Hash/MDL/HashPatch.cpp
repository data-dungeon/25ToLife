#include "stdafx.h"
#include "HashPatch.h"
#include "MDL.h"
#include "HPatch.h"
#include "CP.h"

HashPatch::HashPatch( Model *pModel )
{
   m_pPatch = (HPatch *)pModel->GetHPatchModel()->AddPoly();
}

UINT HashPatch::GetID()
{
   return m_pPatch->GetID();
}

UINT HashPatch::GetNumSides()
{
   return m_cparray.GetCount();
}

void HashPatch::SetNumSides( UINT nNumSides )
{
   m_cparray.SetSize( nNumSides );
   m_pPatch->SetNumSides( nNumSides );
}

CP *HashPatch::GetCPAt( UINT nIndex )
{
   return m_cparray[nIndex];
}

void HashPatch::SetCPAt( UINT nIndex, CP *pCP )
{
   m_cparray[nIndex] = pCP;
   HVertex *pVertex = pCP->GetVertex();
   ASSERT( pVertex );
   m_pPatch->SetVertexAt( nIndex, pVertex );
}