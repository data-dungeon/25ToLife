#include "stdafx.h"
#include "TessPoly.h"
#include "UVPatch.h"
#include "Patch.h"
#include "TexCoords.h"
#include "PropertyPatch.h"

TessPoly::~TessPoly()
{
   SetNumSides(0);
   // TessUVPolys are allocated as a block and freed as a block in the ~UVPatch
}

void TessPoly::Disconnect()
{
   for (UINT i = 0; i < GetNumSides(); i++)
   {
      PatchVertex *pPatchVertex = (PatchVertex *)GetVertexAt(i);
      if (pPatchVertex)
         pPatchVertex->RemoveSecondarySurroundingPoly( this );
   }
}

void TessPoly::SetNumSides( UINT nNumSides )
{
   if (m_nNumSides == nNumSides)
      return;

   for (int i = m_nNumSides-1; i >=0; i--)
   {
      // Polys don't reference vertices, only edges do since they are the ones in charge of destroying them
      SetVertexAt(i, NULL);
      SetVertexNormalAt(i, NULL);
      for (TessPropertyPoly *pTessPropertyPoly = GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly = pTessPropertyPoly->GetNextTessPropertyPoly())
         pTessPropertyPoly->SetPropertyVertexAt(i,NULL);
   }

   if (nNumSides > m_nNumSides || nNumSides == 0)
   {
      if (m_pVertices)
      {
         delete [] m_pVertices;
         m_pVertices = NULL;

         delete [] m_pRTVertexIndices;
         m_pRTVertexIndices = NULL;

         ASSERT(m_pVertexNormals);
         delete [] m_pVertexNormals;
         m_pVertexNormals = NULL;

         for (TessPropertyPoly *pTessPropertyPoly = GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly = pTessPropertyPoly->GetNextTessPropertyPoly())
         {
            ASSERT(pTessPropertyPoly->m_pPropertyVertices);
            delete [] pTessPropertyPoly->m_pPropertyVertices;
            pTessPropertyPoly->m_pPropertyVertices = NULL;
         }
      }

      if (nNumSides > m_nNumSides )
      {
         m_pVertices = SNEW PatchVertex *[nNumSides];
         memset( m_pVertices, 0, nNumSides * sizeof(PatchVertex *) );

         m_pRTVertexIndices = SNEW UINT [nNumSides];
         memset( m_pRTVertexIndices, 0, nNumSides * sizeof(UINT));

         m_pVertexNormals = SNEW VertexNormal *[nNumSides];
         memset( m_pVertexNormals, 0, nNumSides * sizeof(VertexNormal *) );

         for (TessPropertyPoly *pTessPropertyPoly = GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly = pTessPropertyPoly->GetNextTessPropertyPoly())
         {
            pTessPropertyPoly->m_pPropertyVertices = SNEW PropertyVertex *[nNumSides];
            memset( pTessPropertyPoly->m_pPropertyVertices, 0, nNumSides * sizeof(PropertyVertex *) );
         }
      }
   }

   m_nNumSides = nNumSides;
}

void TessPoly::SetVertexAt( UINT nIndex, PatchVertex *pVertex )
{
   ASSERT(nIndex < GetNumSides());

   if (m_pVertices[nIndex])
   {
      if (m_pVertices[nIndex] == pVertex)
         return;

      m_pVertices[nIndex]->RemoveSecondarySurroundingPoly( this );
      m_pVertices[nIndex]->DeReference();
   }

   m_pVertices[nIndex] = pVertex;

   if (pVertex)
   {
      pVertex->Reference();
      pVertex->AddSecondarySurroundingPoly( this );
   }
}

void TessPoly::SetVertexNormalAt( UINT nIndex, VertexNormal *pVertexNormal )
{
   ASSERT(nIndex < GetNumSides());

   if (m_pVertexNormals[nIndex])
   {
      if (m_pVertexNormals[nIndex] == pVertexNormal)
         return;

      m_pVertexNormals[nIndex]->DeReference();
   }

   m_pVertexNormals[nIndex] = pVertexNormal;

   if (pVertexNormal)
   {
      pVertexNormal->Reference();
      pVertexNormal->SetVertexNormalDirtyFlag( TRUE );
   }
}

void TessPoly::SetDirtyFaceNormal( BOOL bDirtyFaceNormal )
{
   if (m_bDirtyFaceNormal == bDirtyFaceNormal)
      return;

   m_bDirtyFaceNormal = bDirtyFaceNormal;

   if (bDirtyFaceNormal) // dirty the vertex normals too
   {
      for (UINT i=0; i<GetNumSides(); i++)
      {
         VertexNormal *pVertexNormal = GetVertexNormalAt(i, FALSE);
         pVertexNormal->SetVertexNormalDirtyFlag( TRUE );
      }
   }
}

// Made this function so GetVertexNormal could be inlined without including patch.h
void TessPoly::TessellatePatch() const
{
   GetPatch()->TessellateWithSurrounding();
}

void TessPoly::BuildFaceNormal()
{
   Vector normal(0,0,0);
   int numVerts = (int)GetNumSides();
   Vector A,B;
   int i;
   for (i = 0; i < numVerts; i++)
   {
      int prev = i - 1;
      if (prev<0)
         prev = numVerts-1;
      int next = i + 1;
      if (next>numVerts-1)
         next = 0;
      A = m_pVertices[i]->GetPos() - m_pVertices[prev]->GetPos();
      B = m_pVertices[next]->GetPos() - m_pVertices[i]->GetPos();
      A.Normalize();
      B.Normalize();
      //     normal += B ^ A;
      normal += A ^ B;
   }
   m_vFaceNormal = normal.Normalize();
   SetDirtyFaceNormal( FALSE );
}

TessUVPoly *TessPoly::FindTessUVPoly( TexCoords *pTexCoords )
{
   for (TessPropertyPoly *pTessPropertyPoly = GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly=pTessPropertyPoly->GetNextTessPropertyPoly())
   {
      if (pTessPropertyPoly->GetPropertyPatch()->GetSOT() != SOT_UVPatch)
         continue;
      if (pTessPropertyPoly->GetPropertyPatch()->GetPropertySet() == (PropertySet*)pTexCoords)
         return (TessUVPoly*)pTessPropertyPoly;
   }
   return NULL;
}

//////////////////////////////////////////////////////////////////////////

void TessPoly::AddTessPropertyPoly( TessPropertyPoly *pTessPropertyPoly )
{
   TessPropertyPoly **pUpLink;
   TessPropertyPoly *pCurrTessPropertyPoly;

   for (pUpLink = &m_pHeadTessPropertyPoly, pCurrTessPropertyPoly=*pUpLink; pCurrTessPropertyPoly; pUpLink = pCurrTessPropertyPoly->GetNextTessPropertyPolyUplink(), pCurrTessPropertyPoly = *pUpLink);

   *pUpLink = pTessPropertyPoly;
   pTessPropertyPoly->SetNextTessPropertyPoly( NULL );

   UINT nNumSides = GetNumSides();

   if (pTessPropertyPoly->m_pPropertyVertices==NULL && nNumSides)
   {
      pTessPropertyPoly->m_pPropertyVertices = SNEW PropertyVertex *[nNumSides];
      memset( pTessPropertyPoly->m_pPropertyVertices, 0, nNumSides * sizeof( PropertyVertex *) );
   }
}

BOOL TessPoly::RemoveTessPropertyPoly( TessPropertyPoly *pTessPropertyPolyMatch )
{
   TessPropertyPoly *pTessPropertyPoly, **ppTessPropertyPoly;

   for ( ppTessPropertyPoly = &m_pHeadTessPropertyPoly, pTessPropertyPoly = m_pHeadTessPropertyPoly; pTessPropertyPoly; ppTessPropertyPoly = &pTessPropertyPoly->m_pNextTessPropertyPoly, pTessPropertyPoly = pTessPropertyPoly->m_pNextTessPropertyPoly ) 
   {
      if (pTessPropertyPoly == pTessPropertyPolyMatch)
      {
         *ppTessPropertyPoly = pTessPropertyPoly->m_pNextTessPropertyPoly;
         pTessPropertyPoly->m_pNextTessPropertyPoly = NULL;
         return TRUE;
      }
   }

   return FALSE;
}

TessPropertyPoly *TessPoly::FindTessPropertyPoly( PropertySet *pPropertySet )
{
   for (TessPropertyPoly *pTessPropertyPoly = GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly=pTessPropertyPoly->GetNextTessPropertyPoly())
   {
      if (pTessPropertyPoly->GetPropertyPatch()->GetPropertySet() == pPropertySet)
         return pTessPropertyPoly;
   }
   return NULL;
}

UINT TessPoly::CountNumTessPropertyPolys() const
{
   UINT nCount = 0;

   for (TessPropertyPoly *pTessPropertyPoly = GetHeadTessPropertyPoly(); pTessPropertyPoly; pTessPropertyPoly=pTessPropertyPoly->GetNextTessPropertyPoly())
      nCount++;
   return nCount;
}
//////////////////////////////////////////////////////////////////////////

void TessPropertyPoly::SetPropertyVertexAt(UINT nIndex, PropertyVertex *pPropertyVertex)
{
   if (m_pPropertyVertices[nIndex])
   {
      if (m_pPropertyVertices[nIndex] == pPropertyVertex)
         return;

      m_pPropertyVertices[nIndex]->DeReference();
   }

   m_pPropertyVertices[nIndex] = pPropertyVertex;;

   if (pPropertyVertex)
      pPropertyVertex->Reference();
}
