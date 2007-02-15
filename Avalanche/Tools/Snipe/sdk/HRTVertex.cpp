#include "stdafx.h"
#include "HRTVertex.h"
#include "RTVertex.h"

// HRTVertex //////////////////////////////////////////////////////////////
const Vector &HRTVertex::GetPos() const
{
   return ((RTVertex *)this)->GetPos();
}

void HRTVertex::SetPos( const Vector &vPos )
{
   ((RTVertex *)this)->SetPos( vPos );
}

const Vector &HRTVertex::GetNormal() const
{
   return ((RTVertex *)this)->GetNormal();
}

void HRTVertex::SetNormal( const Vector &vNormal )
{
   ((RTVertex *)this)->SetNormal( vNormal );
}

// HRTEdge //////////////////////////////////////////////////////////////
UINT HRTEdge::GetVertexIndex( UINT &nIndex )
{
   return ((RTEdge *)this)->GetVertexIndex( nIndex );
}

void HRTEdge::SetVertexIndex( UINT nIndex, UINT nVertexIndex )
{
   ((RTEdge *)this)->SetVertexIndex( nIndex, nVertexIndex );
}

void HRTEdge::SetVertexIndices( UINT nVertexIndex0, UINT nVertexIndex1 )
{
   ((RTEdge *)this)->SetVertexIndices( nVertexIndex0, nVertexIndex1 );
}

// HRTQuad //////////////////////////////////////////////////////////////
UINT HRTQuad::GetVertexIndex( UINT nIndex )
{
   return ((RTQuad *)this)->GetVertexIndex( nIndex );
}

void HRTQuad::SetVertexIndex( UINT nIndex, UINT nVertexIndex )
{
   ((RTQuad *)this)->SetVertexIndex( nIndex, nVertexIndex );
}

void HRTQuad::SetVertexIndices( UINT nVertexIndex0, UINT nVertexIndex1, UINT nVertexIndex2, UINT nVertexIndex3 )
{
   ((RTQuad *)this)->SetVertexIndices( nVertexIndex0, nVertexIndex1, nVertexIndex2, nVertexIndex3  );
}

void HRTQuad::SetVertexIndices( UINT nVertexArray[/*4*/] )
{
   ((RTQuad *)this)->SetVertexIndices( nVertexArray  );
}

// HRTPoly //////////////////////////////////////////////////////////////
UINT HRTPoly::GetVertexCount()
{
   return ((RTPoly *)this)->GetVertexCount();
}

void HRTPoly::SetVertexCount( UINT nVertexCount )
{
   ((RTPoly *)this)->SetVertexCount( nVertexCount );
}

UINT HRTPoly::GetVertexIndex( UINT nIndex )
{
   return ((RTPoly *)this)->GetVertexIndex( nIndex );
}

void HRTPoly::SetVertexIndex( UINT nIndex, UINT nVertexIndex )
{
   ((RTPoly *)this)->SetVertexIndex( nIndex, nVertexIndex );
}

void HRTPoly::SetVertexIndices( UINT nVertextCount, UINT *nVertexArray )
{
   ((RTPoly *)this)->SetVertexIndices( nVertextCount, nVertexArray  );
}
