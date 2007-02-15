#include "stdafx.h"
#include "TessEdge.h"
#include "PatchVertex.h"
#include "VertexNormal.h"
#include "UV.h"
#include "PatchEdge.h"
#include "../Utility/Matrix44.h"
#include "TransformableObject.h"
#include "../Utility/MathTools.h"
#include "GUI/GLBaseView.h"
#include "PropertyVertex.h"

void TessEdgeVertices::Reverse(TessEdgeVertices &result)
{
   result.SetNumVertices(GetNumVertices());
   for (UINT i = 0; i < GetNumVertices(); i++)
      result.SetVertexAt(GetNumVertices()-i-1,GetVertexAt(i));
}

void TessEdgeVertices::SetVertexAt( UINT nIndex, PatchVertex *pVertex )
{
   ASSERT(nIndex<GetNumVertices());

   if (m_vertices[nIndex])
   {
      if (m_vertices[nIndex] == pVertex)
         return;
      m_vertices[nIndex]->DeReference();
   }
   m_vertices[nIndex] = pVertex;
   if (pVertex)
      pVertex->Reference();
}

void TessEdgeVertices::FindClosestIntersection( CGLBaseView *pView, TransformableObject *pTransformableObject, const CPoint &point, float &fPos )
{
   if (GetNumVertices() != 0)
   {
      UINT nNumVertsMinusOne = GetNumVertices()-1;

      for (UINT i = 0; i < nNumVertsMinusOne; i++)
      {
         Vector p0 = GetVertexAt(i)->GetWorldPos( pTransformableObject );
         Vector p1 = GetVertexAt(i+1)->GetWorldPos( pTransformableObject );

         pView->WorldToScreen( p0 );
         pView->WorldToScreen( p1 );

         Vector2 vMouse(point.x-p0.x, point.y-p0.y);
         Vector2 vEdge(p1.x-p0.x, p1.y-p0.y);
         float fLen = vEdge.Length();

         float fPercent = vEdge.Dot( vMouse ) / (fLen * fLen);

         if ( i == 0 && fPercent < 0.0f )
         {
            fPos = 0.0f;
            return;
         }

         if ( fPercent <= 1.0 )
         {
            float ratio = (float)i / (float)nNumVertsMinusOne;
            fPos = ratio;
            if ( fPercent >= 0.0f )
               fPos += ( fPercent / (float)nNumVertsMinusOne );
            return;
         }
      }
   }

   fPos = 1.0f;
}

void TessEdgeVertices::GetFirstHalf(TessEdgeVertices &tessedgevertices)
{
   int numVerts = GetNumVertices();
   UINT half = (int)(numVerts * 0.5f);
   float roundup = (float)(numVerts * 0.5f - int(numVerts * 0.5f));
   if (roundup >= 0.5f)
      half++;

   UINT j;
   tessedgevertices.SetNumVertices(half);
   for (j = 0; j < (UINT)half; j++)
      tessedgevertices.SetVertexAt(j,GetVertexAt(j));
}

void TessEdgeVertices::GetSecondHalf(TessEdgeVertices &tessedgevertices)
{
   int numVerts = GetNumVertices();
   UINT half = (int)(numVerts * 0.5f);
   float roundup = (float)(numVerts * 0.5f - int(numVerts * 0.5f));
   if (roundup >= 0.5f)
      half++;

   UINT j;
   tessedgevertices.SetNumVertices(half);
   for (j = 0; j < (UINT)half; j++)
      tessedgevertices.SetVertexAt(j,GetVertexAt(j+half-1));
}

UINT TessEdgeVertices::FindNPatchEdgeHook()
{
   for (UINT i = 0; i < GetNumVertices(); i++)
      if (GetVertexAt(i)->IsNPatchHook())
         return i;
   return -1;
}

void TessEdgeVertices::Clear()
{
   for (UINT i = 0; i < GetNumVertices(); i++)
      SetVertexAt(i, NULL);
   m_vertices.RemoveAll();
}


//////////////////////////////////////////////////////////////////////////////

void TessEdgeNormals::SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal )
{
   ASSERT(nIndex < GetNumVertexNormals());

   if (m_vertexnormals[nIndex])
   {
      if (m_vertexnormals[nIndex] == pVertexNormal)
         return;
      m_vertexnormals[nIndex]->DeReference();
   }
   m_vertexnormals[nIndex] = pVertexNormal;

   if (pVertexNormal)
      pVertexNormal->Reference();
}

void TessEdgeNormals::Reverse(TessEdgeNormals &result)
{
   result.SetNumVertexNormals(GetNumVertexNormals());
   for (UINT i = 0; i < GetNumVertexNormals(); i++)
      result.SetVertexNormalAt(GetNumVertexNormals()-i-1,GetVertexNormalAt(i));
}

void TessEdgeNormals::GetFirstHalf(TessEdgeNormals &tessedgenormals)
{
   int numVerts = GetNumVertexNormals();
   UINT half = (int)(numVerts * 0.5f);
   float roundup = (float)(numVerts * 0.5f - int(numVerts * 0.5f));
   if (roundup >= 0.5f)
      half++;

   UINT j;
   tessedgenormals.SetNumVertexNormals(half);

   for (j = 0; j < (UINT)half; j++)
      tessedgenormals.SetVertexNormalAt(j,GetVertexNormalAt(j));
}

void TessEdgeNormals::GetSecondHalf(TessEdgeNormals &tessedgenormals)
{
   int numVerts = GetNumVertexNormals();
   UINT half = (int)(numVerts * 0.5f);
   float roundup = (float)(numVerts * 0.5f - int(numVerts * 0.5f));
   if (roundup >= 0.5f)
      half++;

   UINT j;
   tessedgenormals.SetNumVertexNormals(half);

   for (j = 0; j < (UINT)half; j++)
      tessedgenormals.SetVertexNormalAt(j,GetVertexNormalAt(j+half-1));
}

void TessEdgeNormals::Clear()
{
   for (UINT i = 0; i < GetNumVertexNormals(); i++)
      SetVertexNormalAt(i, NULL);

   m_vertexnormals.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////////
// TessPropertyEdge

void TessPropertyEdge::SetPropertyVertexAt( UINT nIndex, PropertyVertex *pPropertyVertex )
{
   ASSERT(nIndex < GetNumPropertyVertices());

   if (m_PropertyVertices[nIndex])
   {
      if (m_PropertyVertices[nIndex] == pPropertyVertex)
         return;
      m_PropertyVertices[nIndex]->DeReference();
   }

   m_PropertyVertices[nIndex] = pPropertyVertex;

   if (pPropertyVertex)
      pPropertyVertex->Reference();
}

void TessPropertyEdge::GetFirstHalf(TessPropertyEdge &tesspropertyedge)
{
   int nNumPropertyVertices = GetNumPropertyVertices();
   UINT half = (int)(nNumPropertyVertices * 0.5f);
   float roundup = (float)(nNumPropertyVertices * 0.5f - int(nNumPropertyVertices * 0.5f));
   if (roundup >= 0.5f)
      half++;

   UINT j;
   tesspropertyedge.SetNumPropertyVertices(half);

   for (j = 0; j < (UINT)half; j++)
      tesspropertyedge.SetPropertyVertexAt(j, GetPropertyVertexAt(j, FALSE));
}

void TessPropertyEdge::GetSecondHalf(TessPropertyEdge &tesspropertyedge)
{
   int nNumPropertyVertices = GetNumPropertyVertices();
   UINT half = (int)(nNumPropertyVertices * 0.5f);
   float roundup = (float)(nNumPropertyVertices * 0.5f - int(nNumPropertyVertices * 0.5f));
   if (roundup >= 0.5f)
      half++;

   UINT j;
   tesspropertyedge.SetNumPropertyVertices(half);

   for (j = 0; j < (UINT)half; j++)
      tesspropertyedge.SetPropertyVertexAt(j, GetPropertyVertexAt(j+half-1, FALSE));
}

void TessPropertyEdge::Clear()
{
   for (UINT i = 0; i < GetNumPropertyVertices(); i++)
      SetPropertyVertexAt(i, NULL);
   m_PropertyVertices.RemoveAll();
}

