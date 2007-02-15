#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class SNIPEEXPORT HRTVertex
{
public:
// Operations
   const Vector &GetPos() const;
   void SetPos( const Vector &vPos );

   const Vector &GetNormal() const;
   void SetNormal( const Vector &vNormal );
};

class SNIPEEXPORT HRTEdge
{
public:
// Operations
   UINT GetVertexIndex( UINT &nIndex );
   void SetVertexIndex( UINT nIndex, UINT nVertexIndex );
   void SetVertexIndices( UINT nVertexIndex0, UINT nVertexIndex1 );
};

class SNIPEEXPORT HRTQuad
{
public:
   UINT GetVertexIndex( UINT nIndex );
   void SetVertexIndex( UINT nIndex, UINT nVertexIndex );
   void SetVertexIndices( UINT nVertexIndex0, UINT nVertexIndex1, UINT nVertexIndex2, UINT nVertexIndex3 );
   void SetVertexIndices( UINT nVertexArray[/*4*/] );
};

class SNIPEEXPORT HRTPoly
{
public:
   UINT GetVertexCount();
   void SetVertexCount( UINT nVertexCount );
   UINT GetVertexIndex( UINT nIndex );
   void SetVertexIndex( UINT nIndex, UINT nVertexIndex );
   void SetVertexIndices( UINT nVertexCount, UINT *nVertexArray );
};