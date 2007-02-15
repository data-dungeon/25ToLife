#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class HPatchVertex;
class HVertexNormal;
class HEdge;
class HPatch;
class HTessPropertyPoly;

class SNIPEEXPORT HTessPoly
{
public:
   UINT GetNumSides() const;

   void SetVertexAt( UINT nIndex, HPatchVertex *pVertex );

   HPatchVertex *GetVertexAt( UINT nIndex ) const;

   HVertexNormal *GetVertexNormalAt( UINT nIndex ) const;

   Vector *GetFaceNormal();

   HTessPropertyPoly *GetHeadTessPropertyPoly() const;
};