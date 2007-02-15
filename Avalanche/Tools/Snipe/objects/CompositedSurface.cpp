//

#include "stdafx.h"
#include "Poly.h"
#include "Properties/SurfaceDriver.h"
#include "GUI/GLModelView.h"
#include "RTVertex.h"
#include "Bone.h"
#include "PatchModel.h"

CompositedSurface::~CompositedSurface()
{
   for (int i=0; i<m_facearray.GetCount(); i++)
   {
      Poly *pPoly = m_facearray[i];
      pPoly->SetCompositedSurface( NULL, FALSE );
   }

   FreeRTQuads();
}

void CompositedSurface::SetFromSurface( Surface *pSurface )
{
   m_cAmbientColor = pSurface->m_cAmbientColor;
   m_cDiffuseColor = pSurface->m_cDiffuseColor;
   m_cSpecularColor = pSurface->m_cSpecularColor;
   m_cEmissionColor = pSurface->m_cEmissionColor;
   m_fTransparency = pSurface->m_fTransparency;
   m_fShininess = pSurface->m_fShininess;
   m_vNormalOffset = pSurface->m_vNormalOffset;
   m_nCullFaceMode = pSurface->m_nCullFaceMode;
}

void CompositedSurface::ReplaceSurface( Surface *pSurface )
{
   float fOpacity = pSurface->m_fBlendFactor;
   
   if (fOpacity == 1.0f)
   {
      if (pSurface->m_cAmbientColor.IsSet())
         m_cAmbientColor = pSurface->m_cAmbientColor;
      if (pSurface->m_cDiffuseColor.IsSet())
         m_cDiffuseColor = pSurface->m_cDiffuseColor;
      else
         m_cDiffuseColor.AddSurfaceDrivers( pSurface->m_cDiffuseColor );
      if (pSurface->m_cSpecularColor.IsSet())
         m_cSpecularColor = pSurface->m_cSpecularColor;
      if (pSurface->m_cEmissionColor.IsSet())
         m_cEmissionColor = pSurface->m_cEmissionColor;
      if (pSurface->m_fTransparency.IsSet())
         m_fTransparency = pSurface->m_fTransparency;
      if (pSurface->m_fShininess.IsSet())
         m_fShininess = pSurface->m_fShininess;
      if (pSurface->m_vNormalOffset.IsSet())
         m_vNormalOffset = pSurface->m_vNormalOffset;
      if (pSurface->m_nCullFaceMode.IsSet())
         m_nCullFaceMode = pSurface->m_nCullFaceMode;
   }
   else if (fOpacity > 0.0f)
   {
      float fOneMinusOpacity = 1.0f - fOpacity;
      
      if (pSurface->m_cAmbientColor.IsSet())
         m_cAmbientColor = (m_cAmbientColor * fOneMinusOpacity) + (pSurface->m_cAmbientColor.GetValue() * fOpacity);
      if (pSurface->m_cDiffuseColor.IsSet())
         m_cDiffuseColor = (m_cDiffuseColor * fOneMinusOpacity) + (pSurface->m_cDiffuseColor.GetValue() * fOpacity);
      m_cDiffuseColor.AddSurfaceDrivers( pSurface->m_cDiffuseColor );
      if (pSurface->m_cSpecularColor.IsSet())
         m_cSpecularColor = (m_cSpecularColor * fOneMinusOpacity) + (pSurface->m_cSpecularColor.GetValue() * fOpacity);
      if (pSurface->m_cEmissionColor.IsSet())
         m_cEmissionColor = (m_cEmissionColor * fOneMinusOpacity) + (pSurface->m_cEmissionColor.GetValue() * fOpacity);
      if (pSurface->m_fTransparency.IsSet())
         m_fTransparency = (m_fTransparency * fOneMinusOpacity) + (pSurface->m_fTransparency.GetValue() * fOpacity);
      if (pSurface->m_fShininess.IsSet())
         m_fShininess = (m_fShininess * fOneMinusOpacity) + (pSurface->m_fShininess.GetValue() * fOpacity);
      if (pSurface->m_vNormalOffset.IsSet())
         m_vNormalOffset = (m_vNormalOffset * fOneMinusOpacity) + (pSurface->m_vNormalOffset.GetValue() * fOpacity);
      if (pSurface->m_nCullFaceMode.IsSet() && fOpacity >= 0.5f)
         m_nCullFaceMode = pSurface->m_nCullFaceMode;
   }
}

void CompositedSurface::AddSurface( Surface *pSurface )
{
   float fOpacity = pSurface->m_fBlendFactor;
   
   if (pSurface->m_cAmbientColor.IsSet())
      m_cAmbientColor += pSurface->m_cAmbientColor.GetValue() * fOpacity;
   if (pSurface->m_cDiffuseColor.IsSet())
      m_cDiffuseColor += pSurface->m_cDiffuseColor.GetValue() * fOpacity;
   if (pSurface->m_cSpecularColor.IsSet())
      m_cSpecularColor += pSurface->m_cSpecularColor.GetValue() * fOpacity;
   if (pSurface->m_cEmissionColor.IsSet())
      m_cEmissionColor += pSurface->m_cEmissionColor.GetValue() * fOpacity;
   if (pSurface->m_fTransparency.IsSet())
      m_fTransparency += pSurface->m_fTransparency.GetValue() * fOpacity;
   if (pSurface->m_fShininess.IsSet())
      m_fShininess += pSurface->m_fShininess.GetValue() * fOpacity;
   if (pSurface->m_vNormalOffset.IsSet())
      m_vNormalOffset += pSurface->m_vNormalOffset.GetValue() * fOpacity;
}

void CompositedSurface::SubtractSurface( Surface *pSurface )
{
   float fOpacity = pSurface->m_fBlendFactor;

   if (pSurface->m_cAmbientColor.IsSet())
      m_cAmbientColor -= pSurface->m_cAmbientColor.GetValue() * fOpacity;
   if (pSurface->m_cDiffuseColor.IsSet())
      m_cDiffuseColor -= pSurface->m_cDiffuseColor.GetValue() * fOpacity;
   if (pSurface->m_cSpecularColor.IsSet())
      m_cSpecularColor -= pSurface->m_cSpecularColor.GetValue() * fOpacity;
   if (pSurface->m_cEmissionColor.IsSet())
      m_cEmissionColor -= pSurface->m_cEmissionColor.GetValue() * fOpacity;
   if (pSurface->m_fTransparency.IsSet())
      m_fTransparency -= pSurface->m_fTransparency.GetValue() * fOpacity;
   if (pSurface->m_fShininess.IsSet())
      m_fShininess -= pSurface->m_fShininess.GetValue() * fOpacity;
   if (pSurface->m_vNormalOffset.IsSet())
      m_vNormalOffset -= pSurface->m_vNormalOffset.GetValue() * fOpacity;
}

void CompositedSurface::MultiplySurface( Surface *pSurface )
{
   float fOpacity = pSurface->m_fBlendFactor;
   float fOneMinusOpacity = 1.0f - fOpacity;

   if (pSurface->m_cAmbientColor.IsSet())
      m_cAmbientColor -= pSurface->m_cAmbientColor.GetValue() * fOpacity;
   if (pSurface->m_cDiffuseColor.IsSet())
      m_cDiffuseColor = (m_cDiffuseColor * fOneMinusOpacity) + (m_cDiffuseColor * pSurface->m_cDiffuseColor.GetValue() * fOpacity);
   if (pSurface->m_cSpecularColor.IsSet())
      m_cSpecularColor -= pSurface->m_cSpecularColor.GetValue() * fOpacity;
   if (pSurface->m_cEmissionColor.IsSet())
      m_cEmissionColor -= pSurface->m_cEmissionColor.GetValue() * fOpacity;
   if (pSurface->m_fTransparency.IsSet())
      m_fTransparency -= pSurface->m_fTransparency.GetValue() * fOpacity;
   if (pSurface->m_fShininess.IsSet())
      m_fShininess -= pSurface->m_fShininess.GetValue() * fOpacity;
   if (pSurface->m_vNormalOffset.IsSet())
      m_vNormalOffset -= pSurface->m_vNormalOffset.GetValue() * fOpacity;
}

void CompositedSurface::Compute()
{
   Surface *pSurface = m_SurfaceArray.m_pSurfaces[0];
   SetFromSurface( pSurface );

   for (UINT i=1; i<m_SurfaceArray.m_nNumSurfaces; i++)
   {
      pSurface = m_SurfaceArray.m_pSurfaces[i];

      switch (pSurface->m_nBlendMethod)
      {
         case Surface::BM_REPLACE:
            ReplaceSurface( pSurface );
            break;

         case Surface::BM_ADD:
            AddSurface( pSurface );
            break;

         case Surface::BM_SUBTRACT:
            SubtractSurface( pSurface );
            break;

         case Surface::BM_MULTIPLY:
            MultiplySurface( pSurface );
            break;
      }
   }

   SetDirty( FALSE );
}

void CompositedSurface::Bind()
{
   if (m_bDirty)
      Compute();

   GLfloat params[4];
   params[3] = 1.0f - (m_fTransparency / 100.0f);

   params[3] *= m_pModelCache->GetCurrentInstance()->GetDrawOpacity();

   params[0] = m_cAmbientColor.GetValue().GetRed(); params[1] = m_cAmbientColor.GetValue().GetGreen(); params[2] = m_cAmbientColor.GetValue().GetBlue();
   glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, params );

   params[0] = m_cDiffuseColor.GetValue().GetRed(); params[1] = m_cDiffuseColor.GetValue().GetGreen(); params[2] = m_cDiffuseColor.GetValue().GetBlue();
   glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, params );

   params[0] = m_cSpecularColor.GetValue().GetRed(); params[1] = m_cSpecularColor.GetValue().GetGreen(); params[2] = m_cSpecularColor.GetValue().GetBlue();
   glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, params );

   //params[0] = m_cEmissionColor.GetValue().GetRed(); params[1] = m_cEmissionColor.GetValue().GetGreen(); params[2] = m_cEmissionColor.GetValue().GetBlue();
   //glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, params );

   glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_fShininess );

   switch (m_nCullFaceMode)
   {
      case Surface::CFM_NONE:
         glDisable(GL_CULL_FACE);
         break;
      case Surface::CFM_FRONT:
         glCullFace( GL_FRONT );
         glEnable( GL_CULL_FACE );
         break;
      case Surface::CFM_BACK:
         glCullFace( GL_BACK );
         glEnable( GL_CULL_FACE );
         break;
   }
}

void CompositedSurface::UnBind()
{
}

UINT CompositedSurface::AddFace( Poly *pFace )
{
   ASSERT( pFace->m_bInList );
   FreeRTQuads();
   UINT nIndex = m_facearray.Add( pFace );
   return nIndex;
}

BOOL CompositedSurface::RemoveFace( Poly *pFace )
{
   UINT nIndex = FindFace( pFace );
   if (nIndex == -1)
      return FALSE;
   m_facearray.RemoveAt( nIndex );
   FreeRTQuads();
   return TRUE;
}

UINT CompositedSurface::FindFace( Poly *pFace ) const
{
   for (UINT i=0; i<GetNumFaces(); i++)
      if (GetFaceAt(i) == pFace)
         return i;

   return -1;
}

void CompositedSurface::DrawFacesImmediateBase( const Matrix44 &viewmatrix, BOOL bDrawTess, SurfaceDriver *pSurfaceDriver/*=NULL*/ )
{
   for (UINT i=0; i<GetNumFaces(); i++)
   {
      Poly *pPoly = GetFaceAt(i);
      if (!pPoly->IsDraw())
         continue;

      pPoly->Draw( viewmatrix, bDrawTess, pSurfaceDriver );
   }
}

void CompositedSurface::DrawFacesImmediate( const Matrix44 &viewmatrix, CGLModelView *pModelView, BOOL bDrawTess )
{
   Bind();

   if (!pModelView->m_bDisplayTextures || !m_cDiffuseColor.m_nNumSurfaceDrivers)
      DrawFacesImmediateBase( viewmatrix, bDrawTess );

   if (pModelView->m_bDisplayTextures)
   {
      for (UINT nSurfaceDriverIndex=0; nSurfaceDriverIndex<m_cDiffuseColor.m_nNumSurfaceDrivers; nSurfaceDriverIndex++)
      {
         SurfaceDriver *pSurfaceDriver = m_cDiffuseColor.m_pSurfaceDrivers[ nSurfaceDriverIndex ];
         if (!pSurfaceDriver->IsValid())
            continue;

         pSurfaceDriver->Bind();
            DrawFacesImmediateBase( viewmatrix, bDrawTess, pSurfaceDriver );
         pSurfaceDriver->UnBind();
      }
   }

   UnBind();
}

void CompositedSurface::DrawFacesRetained( const Matrix44 &viewmatrix, CGLModelView *pModelView, BOOL bDrawTess )
{
   if (!m_pRTQuads && !CreateRTQuads( bDrawTess ))
      return;

   Bind();

   if (!pModelView->m_bDisplayTextures || !m_cDiffuseColor.m_nNumSurfaceDrivers || pModelView->m_bDoPick)
      glDrawElements( GL_QUADS, m_nNumRTQuads*4, GL_UNSIGNED_INT, m_pRTQuads );

   if (pModelView->m_bDisplayTextures && !pModelView->m_bDoPick)
   {
      for (UINT nSurfaceDriverIndex=0; nSurfaceDriverIndex<m_cDiffuseColor.m_nNumSurfaceDrivers; nSurfaceDriverIndex++)
      {
         SurfaceDriver *pSurfaceDriver = m_cDiffuseColor.m_pSurfaceDrivers[ nSurfaceDriverIndex ];
         if (!pSurfaceDriver->IsValid())
            continue;

         pSurfaceDriver->Bind();
            glDrawElements( GL_QUADS, m_nNumRTQuads*4, GL_UNSIGNED_INT, m_pRTQuads );
         pSurfaceDriver->UnBind();
      }
   }

   UnBind();
}

void CompositedSurface::FreeRTQuads()
{
   if (m_pRTQuads)
   {
      delete [] m_pRTQuads;
      m_pRTQuads = NULL;
      m_nNumRTQuads = 0;
   }
}

BOOL CompositedSurface::CreateRTQuads( BOOL bTessellated )
{
   FreeRTQuads();

   return m_pModelCache->CreateRTQuads( this, bTessellated );
}

BOOL PolyModel::CreateRTQuads( CompositedSurface *pCompositedSurface, BOOL bTessellated )
{
   UINT nNumFaces = pCompositedSurface->GetNumFaces();
   // we turn everything (ngons and 3s too) into quads, so we need an accurate quad count
   pCompositedSurface->m_nNumRTQuads = 0;
   for (UINT nFace=0; nFace<nNumFaces; nFace++)
   {
      Poly *pFace = pCompositedSurface->GetFaceAt( nFace );
      if (!pFace->IsDraw())
         continue;
      UINT nNumSides = pFace->GetNumSides();
      UINT nNumSubQuads = (nNumSides - 1) / 2;
      pCompositedSurface->m_nNumRTQuads += nNumSubQuads;
   }
   pCompositedSurface->m_pRTQuads = SNEW RTQuad[ pCompositedSurface->m_nNumRTQuads ];

   UINT nRTQuad = 0;
   for (UINT nFace=0; nFace<nNumFaces; nFace++)
   {
      Poly *pFace = pCompositedSurface->GetFaceAt( nFace );
      if (!pFace->IsDraw())
         continue;

      UINT nNumSides = pFace->GetNumSides();
      UINT nNumSubQuads = (nNumSides - 1) / 2;

      for (UINT nSubQuad=0; nSubQuad<nNumSubQuads; nSubQuad++, nRTQuad++)
      {
         RTQuad *pRTQuad = &pCompositedSurface->m_pRTQuads[nRTQuad];

         for (UINT nQuadSide=0; nQuadSide<4; nQuadSide++)
         {
            UINT nFaceSide;
            if (nSubQuad)
            {
               if (nQuadSide == 0)
                  nFaceSide = 0;
               else
                  nFaceSide = (nSubQuad*4) + nQuadSide - 2;
            }
            else
               nFaceSide = nQuadSide;

            if (nFaceSide >= nNumSides)
               nFaceSide = nNumSides - 1;

            UINT nRTVertexIndex = pFace->GetRTVertexAt( nFaceSide );
            pRTQuad->SetVertexIndex( nQuadSide, nRTVertexIndex );
         }
      }
   }

   return TRUE;
}

BOOL PatchModel::CreateRTQuads( CompositedSurface *pCompositedSurface, BOOL bTessellated )
{
   if (!bTessellated)
      return PolyModel::CreateRTQuads( pCompositedSurface, bTessellated );

   UINT nNumFaces = pCompositedSurface->GetNumFaces();
   // we turn everything (ngons and 3s too) into quads, so we need an accurate quad count
   pCompositedSurface->m_nNumRTQuads = 0;
   for (UINT nPatch=0; nPatch<nNumFaces; nPatch++)
   {
      Patch *pPatch = (Patch *)pCompositedSurface->GetFaceAt( nPatch );
      if (!pPatch->IsDraw())
         continue;

      UINT nNumTessPolys = pPatch->GetNumTessPolys();
      for (UINT nTessPoly=0; nTessPoly<nNumTessPolys; nTessPoly++)
      {
         TessPoly *pTessPoly = pPatch->GetTessPolyAt( nTessPoly );
         UINT nNumSides = pTessPoly->GetNumSides();
         UINT nNumSubQuads = (nNumSides - 1) / 2;
         pCompositedSurface->m_nNumRTQuads += nNumSubQuads;
      }
   }
   pCompositedSurface->m_pRTQuads = SNEW RTQuad[ pCompositedSurface->m_nNumRTQuads ];

   UINT nRTQuad = 0;
   for (UINT nPatch=0; nPatch<nNumFaces; nPatch++)
   {
      Patch *pPatch = (Patch *)pCompositedSurface->GetFaceAt( nPatch );
      if (!pPatch->IsDraw())
         continue;

      UINT nNumTessPolys = pPatch->GetNumTessPolys();
      for (UINT nTessPoly=0; nTessPoly<nNumTessPolys; nTessPoly++)
      {
         TessPoly *pTessPoly = pPatch->GetTessPolyAt( nTessPoly );
         UINT nNumSides = pTessPoly->GetNumSides();
         UINT nNumSubQuads = (nNumSides - 1) / 2;

         for (UINT nSubQuad=0; nSubQuad<nNumSubQuads; nSubQuad++, nRTQuad++)
         {
            RTQuad *pRTQuad = &pCompositedSurface->m_pRTQuads[nRTQuad];

            for (UINT nQuadSide=0; nQuadSide<4; nQuadSide++)
            {
               UINT nFaceSide;
               if (nSubQuad)
               {
                  if (nQuadSide == 0)
                     nFaceSide = 0;
                  else
                     nFaceSide = (nSubQuad*4) + nQuadSide - 2;
               }
               else
                  nFaceSide = nQuadSide;

               if (nFaceSide >= nNumSides)
                  nFaceSide = nNumSides - 1;

               UINT nRTVertexIndex = pTessPoly->GetRTVertexAt( nFaceSide );
               pRTQuad->SetVertexIndex( nQuadSide, nRTVertexIndex );
            }
         }
      }
   }

   return TRUE;
}
