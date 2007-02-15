//

#include "stdafx.h"
#include "MDL.h"
#include "CP.h"
#include "HPatchVertex.h"
#include "HPatchEdge.h"
#include "HPatch.h"
#include "HImage.h"
#include "HSurfaceDriver.h"
#include "HUVMapSurfaceDriver.h"
#include "HTexCoordsPointerProperty.h"
#include "HImagePointerProperty.h"
#include "HPropertyPointerProperty.h"
#include "HVector2Property.h"
#include "HColorProperty.h"
#include "HTypeProperty.h"
#include "HFloatProperty.h"
#include "HTexCoords.h"
#include "HUVPoly.h"
#include "HUV.h"
#include "HSelection.h"
#include "HClampMapSurfaceDriver.h"
#include "HSnipeObjectList.h"
#include "HBone.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"
#include "Rotate.h"
#include "HPlaceholderObject.h"

void MdlTranslator::OnExport( HPolyModel *pModel, const char *filename )
{
   HProject *pProject = GetHMainInterface()->GetCurrentProject();

   m_pModel = SNEW Model( pModel );

   FileStream fs;
   fs.Open( filename, BaseStream::BS_OUTPUT );

   fs << "[MODELFILE]" << nl;
   fs << "ProductVersion=" << 11.05f << nl;

   fs << "[IMAGES]" << nl;
   HSnipeObject *pImageContainer = (HSnipeObject*)pProject->GetImageContainer();
   for (int i=0; i<pImageContainer->GetNumChildren(); i++)
   {
      HImage *pImage = (HImage *)pImageContainer->GetChildAt(i)->IsKindOf( SOT_Image );
      if (!pImage || !pImage->IsUsedBy( pModel ))
         continue;
      SaveImage( fs, pImage );
   }
   fs << "[ENDIMAGES]" << nl;

   fs << "[OBJECTS]" << nl;
   m_pModel->Save( fs );
   fs << "[ENDOBJECTS]" << nl;

   fs << "[ENDMODELFILE]" << nl;

   delete m_pModel;
}

BOOL Model::Save( BaseStream &bs )
{
   bs << "[MODEL]" << nl;
   SaveMesh( bs );
   SavePatches( bs );
   SaveBones( bs );
   SaveDecals( bs );
   SaveGroups( bs );
   SavePlaceholders( bs );
   bs << "[ENDMODEL]" << nl;

   return TRUE;
}

///////////////////////////////////////////////////////////////
// MESH

void Model::SaveMesh( BaseStream &bs )
{
   HPatchModel *pModel = GetHPatchModel();

   UINT nLargestID = 0;
   for (POSITION pos=pModel->GetHeadEdgePosition(); pos; )
   {
      HPatchEdge *pFirstEdge = pModel->GetNextEdge( pos );
UINT nFirstEdgeID = pFirstEdge->GetID();
      if (!pFirstEdge->GetPrevContEdge(FALSE)) // found a new spline
      {
         HPatchVertex *pVertex = pFirstEdge->GetPrevContVertex();
         UINT nID = pVertex->GetID();
         if (nID > nLargestID)
            nLargestID = nID;
         CP *pCP = SNEW CP( pVertex );
         if (pFirstEdge->IsVertexPeaked( pVertex ))
            pCP->SetSmooth( FALSE );
         CP *pParentCP;
         if (m_cpmap.Lookup( nID, pParentCP ))
         {
            for (; pParentCP->GetChildCP(); pParentCP=pParentCP->GetChildCP())
               ;
            pParentCP->SetChildCP( pCP );
            pCP->SetParentCP( pParentCP );
         }
         else
            m_cpmap.SetAt( nID, pCP );
         m_splinelist.AddTail( pCP );

         CP *pFirstCP = pCP;
         CP *pPrevCP = pCP;
         HVertex *pFirstVertex = pVertex;
         for (HPatchEdge *pEdge=pFirstEdge; pEdge; pEdge=pEdge->GetNextContEdge(FALSE))
         {
UINT nEdgeID = pEdge->GetID();
            HVertex *pVertex = pEdge->GetNextContVertex();
            UINT nID = pVertex->GetID();
            if (nID > nLargestID)
               nLargestID = nID;

            if (pVertex==pFirstVertex && pEdge->GetNextContEdge(TRUE)==pFirstEdge)
            {
               pCP = pFirstCP;
               pPrevCP->SetLoop( TRUE );
            }
            else
            {
               pCP = SNEW CP( pVertex );
               if (pEdge->IsVertexPeaked( pVertex ))
                  pCP->SetSmooth( FALSE );
               if (pEdge->IsVertexConstrained( pVertex ) && pVertex->GetNumSurroundingEdges()==3)
                  pCP->SetConstrained( TRUE );

               CP *pParentCP;
               if (m_cpmap.Lookup( nID, pParentCP ))
               {
                  for (; pParentCP->GetChildCP(); pParentCP=pParentCP->GetChildCP())
                     ;
                  pParentCP->SetChildCP( pCP );
                  pCP->SetParentCP( pParentCP );
               }
               else
                  m_cpmap.SetAt( nID, pCP );
            }

            pPrevCP->SetOutAlpha( pEdge->GetFirstVertexHandle(FALSE)->GetAlphaProperty()->GetValue() );
            pPrevCP->SetOutGamma( pEdge->GetFirstVertexHandle(FALSE)->GetGammaProperty()->GetValue() );
            pPrevCP->SetOutMagnitude( pEdge->GetFirstVertexHandle(FALSE)->GetMagnitudeProperty()->GetValue() );

            pCP->SetInAlpha( pEdge->GetSecondVertexHandle(FALSE)->GetAlphaProperty()->GetValue() );
            pCP->SetInGamma( pEdge->GetSecondVertexHandle(FALSE)->GetGammaProperty()->GetValue() );
            pCP->SetInMagnitude( pEdge->GetSecondVertexHandle(FALSE)->GetMagnitudeProperty()->GetValue() );

            pPrevCP->SetNextInSpline( pCP );
            pCP->SetPreviousInSpline( pPrevCP );

            pPrevCP->SetNextEdge( pEdge );
            pCP->SetPrevEdge( pEdge );

            pPrevCP = pCP;
         }
      }
   }

   CList<CP *> renumberedcps;
   for (POSITION pos=m_cpmap.GetStartPosition(); pos; )
   {
      CP *pCP;
      UINT nID;
      m_cpmap.GetNextAssoc( pos, nID, pCP );

      for (pCP=pCP->GetChildCP(); pCP; pCP=pCP->GetChildCP())
      {
         pCP->SetID( ++nLargestID );
         renumberedcps.AddTail( pCP );
      }
   }

   for (POSITION pos=renumberedcps.GetHeadPosition(); pos; )
   {
      CP *pCP = renumberedcps.GetNext( pos );
      m_cpmap.SetAt( pCP->GetID(), pCP );
   }

   bs << "[MESH]" << nl;
   bs << "Version=" << 2 << nl;
   bs << "Splines=" << m_splinelist.GetCount() << nl;
   for (POSITION pos=m_splinelist.GetHeadPosition(); pos; )
   {
      CP *pSplineCPHead = m_splinelist.GetNext( pos );
      SaveSpline( bs, pSplineCPHead );
   }
   bs << "[ENDMESH]" << nl;
}

void Model::SaveSpline( BaseStream &bs, CP *pSplineCPHead )
{
   UINT nHeadID = pSplineCPHead->GetID();

   int nCount = 0;
   for (CP *pCP=pSplineCPHead; pCP; pCP=pCP->NextInSpline())
   {
      ASSERT( pCP==pSplineCPHead || pCP->PreviousInSpline() ); // no prev pointer

      if (!pCP->IsTConstrained())
         nCount++;
   }

   bs << "CPs=" << nCount << nl;
   for (CP *pCP=pSplineCPHead; pCP; pCP=pCP->NextInSpline())
   {
      if (!pCP->IsTConstrained())
         SaveCP( bs, pCP );
   }
}

void Model::SaveCP( BaseStream &bs, CP *pCP )
{
   CP *pConstrainedCP = pCP->DrillForTConstrained();

   AMCPFlags flags;
   flags.m_flags = 0L;
   flags.m_issmooth = pCP->IsSmooth();
   if (pCP->NextInSpline(TRUE) && pCP->NextInSpline(TRUE)->IsTConstrained())
   {
      flags.m_ishookbase = TRUE;
      flags.m_isloop = pCP->IsLoop() || pCP->NextInSpline(TRUE)->IsLoop();
   }
   else
   {
      flags.m_ishookbase = FALSE;
      flags.m_isloop = pCP->IsLoop();
   }
   flags.m_ishook = (pConstrainedCP != NULL);
   bs << flags.m_flags << sp;

   BOOL bIsInstance = (pCP->GetParentCP() || flags.m_ishook) ? 1 : 0;
   bs << bIsInstance << sp;

   UINT nID = pCP->GetID();
   bs << nID;

   if (flags.m_ishook)
   {
      float fHookPlacement;
      if (pConstrainedCP->PreviousInSpline(TRUE)->IsTConstrained())
      {
         if (pConstrainedCP->NextInSpline(TRUE)->IsTConstrained())
            fHookPlacement = 0.50f;
         else
            fHookPlacement = 0.75f;
      }
      else if (pConstrainedCP->NextInSpline(TRUE)->IsTConstrained())
         fHookPlacement = 0.25f;
      else
         fHookPlacement = 0.50f;

      bs << sp << fHookPlacement;
   }

   bs << nl;

   if (bIsInstance)
   {
      if (flags.m_ishook)
      {
         CP *pPrevCP = pConstrainedCP->PreviousInSpline(TRUE);
         if (pPrevCP->IsTConstrained())
         {
            if (pPrevCP->GetParentCP())
               pPrevCP = pPrevCP->GetParentCP();
            else if (pPrevCP->GetChildCP())
               pPrevCP = pPrevCP->GetChildCP();
            else
               ASSERT(FALSE);
         }
         bs << pPrevCP->GetID() << nl;
      }
      else
      {
         CP *pNextCP = pCP->GetParentCP()->NextInSpline(TRUE);
         if (pNextCP && pNextCP->IsTConstrained())
         {
            for (CP *pLastConstrained=pNextCP; pLastConstrained->NextInSpline(TRUE) && pLastConstrained->NextInSpline(TRUE)->IsTConstrained(); pLastConstrained=pLastConstrained->NextInSpline(TRUE))
               ;
            if (pLastConstrained->GetParentCP())
               bs << pLastConstrained->GetParentCP()->GetID() << nl;
            else if (pLastConstrained->GetChildCP())
               bs << pLastConstrained->GetChildCP()->GetID() << nl;
            else
               ASSERT(FALSE);
         }
         else
            bs << pCP->GetParentCP()->GetID() << nl;
      }
   }
   else
      bs << pCP->GetPos() << nl;

   Bias inbias( pCP->GetInAlpha(), pCP->GetInGamma(), pCP->GetInMagnitude());
   if (inbias.IsIdentity())
      bs << 0 << nl;
   else
      bs << inbias.m_alpha << sp << inbias.m_gamma << sp << inbias.m_magnitude << nl;

   Bias outbias( pCP->GetOutAlpha(), pCP->GetOutGamma(), pCP->GetOutMagnitude());
   if (outbias.IsIdentity())
      bs << 0 << nl;
   else
      bs << outbias.m_alpha << sp << outbias.m_gamma << sp << outbias.m_magnitude << nl;
}

///////////////////////////////////////////////////////////////
// PATCHES

void Model::SavePatches( BaseStream &bs )
{
   HPatchModel *pModel = GetHPatchModel();

   bs << "[PATCHES]" << nl;
   {
      bs << "Version=" << 3 << nl;

      CList<HPatch *> patchlist;
      CList<HPatch *> patch5list;
      for (POSITION pos=pModel->GetHeadFacePosition(); pos; )
      {
         HPatch *pPatch = pModel->GetNextFace(pos);
UINT nPatchID = pPatch->GetID();
         int nNumActualVertices = pPatch->GetNumSides();
         int nNumVerticesWeCareAbout = 0; // we exclude hooks when they hook into one of our edges
         for (int i=0; i<nNumActualVertices; i++)
         {
            HVertex *pVertex = pPatch->GetVertexAt( i );
            UINT nVertexID = pVertex->GetID();
            CP *pCP = NULL;
            m_cpmap.Lookup( nVertexID, pCP );
            ASSERT( pCP );
            pCP = GetCPOnEdge( pPatch->GetEdgeAt( i ), pCP );
            if (!pCP->IsTConstrained() || !pCP->IsTConstrainedCPInEdge( pPatch, i ))
               nNumVerticesWeCareAbout++;
         }

         if (nNumVerticesWeCareAbout==3 || nNumVerticesWeCareAbout==4)
            patchlist.AddTail( pPatch );
         else if (nNumVerticesWeCareAbout == 5)
            patch5list.AddTail( pPatch );
         else if (nNumVerticesWeCareAbout > 5)
            DisplayWarning( (String)"Could not export Face("+pPatch->GetID()+"), too many sides (Hash MDL's support 3, 4 & 5 sided patches)." );
      }

      bs << "Count=" << patchlist.GetCount() << nl;
      for (POSITION pos=patchlist.GetHeadPosition(); pos; )
      {
         HPatch *pPatch = patchlist.GetNext( pos );
         SavePatch( bs, pPatch );
      }

      bs << patch5list.GetCount() << nl;
      for (POSITION pos=patch5list.GetHeadPosition(); pos; )
      {
         HPatch *pPatch = patchlist.GetNext( pos );
         SavePatch( bs, pPatch );
      }
   }
   bs << "[ENDPATCHES]" << nl;
}

CP *Model::GetCPOnEdge( HEdge *pEdge, CP *pFindCP )
{
HVertex *pVertex0 = pEdge->GetFirstVertex();
HVertex *pVertex1 = pEdge->GetSecondVertex();

   CP *pCP;
   for (pCP=pFindCP; pCP->GetParentCP(); pCP=pCP->GetParentCP())
      ;

   for (; pCP; pCP=pCP->GetChildCP())
   {
      if (pCP->GetNextEdge()==pEdge || pCP->GetPrevEdge()==pEdge)
         return pCP;
   }

   ASSERT( FALSE );
   return NULL;
}

static UINT GetPatchID( CP *pCP )
{
   UINT nID = pCP->GetID();

   for ( ; pCP; pCP=pCP->GetParentCP())
   {
      if (pCP->IsTConstrained())
         return nID;

      nID = pCP->GetID();
   }

   return nID;
}

void Model::SavePatch( BaseStream &bs, HPatch *pPatch )
{
   int nNumActualVertices = pPatch->GetNumSides();
   int nNumAMVertices = 0;
   BOOL bHook[5] = { 0,0,0,0,0 };
   BOOL bLastIsHook = FALSE;
   CArray<CP *> hashcps; // cps in patch that don't include hooks in an edge
   hashcps.SetSize( nNumActualVertices ); // there will actually only be nNumAMVertices when done

UINT nPatchID = pPatch->GetID();

   for (int i=0; i<nNumActualVertices; i++)
   {
      HVertex *pVertex = pPatch->GetVertexAt( i );
      UINT nVertexID = pVertex->GetID();
      CP *pCP = NULL;
      m_cpmap.Lookup( nVertexID, pCP );
      ASSERT( pCP );
      pCP = GetCPOnEdge( pPatch->GetEdgeAt( i ), pCP ); // fix cps2 to have every vertex lie in a unique edge

      if (pCP->IsTConstrained())
      {
         if (pCP->IsTConstrainedCPInEdge( pPatch, i ))
         {
            // bHook is set when there is a hook in the edge of the patch
            if (nNumAMVertices > 0)
               bHook[ nNumAMVertices-1 ] = TRUE;
            else
               bLastIsHook = TRUE;
            continue;
         }
         else
         {
            if (pCP->GetChildCP())
               pCP = pCP->GetChildCP();
            else
               pCP = pCP->GetParentCP();
         }
      }

      if (nNumAMVertices == 5) // A:M only supports 3, 4 and 5 point patches (we just found our 6th)
      {
         DisplayWarning( (String)"Could not export patch("+pPatch->GetID()+"), too many sides (Hash MDL's support 3, 4 & 5 sided patches)." );
         return;
      }

      hashcps.SetAt( nNumAMVertices++, pCP );
   }

   if (bLastIsHook)
      bHook[ nNumAMVertices-1 ] = TRUE;

   // Find the lowest head vertex id so we can make it the first vertex
   int nLowestIndex = 0;
   UINT nLowestID = GetPatchID( hashcps[0] );
   for (int i=1; i<nNumAMVertices; i++)
   {
      UINT nID = GetPatchID( hashcps[i] );
      if (nID < nLowestID)
      {
         nLowestIndex = i;
         nLowestID = nID;
      }
   }
   // rotate patch to have the lowest head vertex id at index[0]
   if (nLowestIndex > 0)
   {
      CArray<CP *> tempcps;
      tempcps.SetSize( nNumAMVertices );
      BOOL bTempHook[5];
      for (int i=0; i<nNumAMVertices; i++)
      {
         tempcps[i] = hashcps[(nLowestIndex + i) % nNumAMVertices];
         bTempHook[i] = bHook[(nLowestIndex + i) % nNumAMVertices];
      }
      for (int i=0; i<nNumAMVertices; i++)
      {
         hashcps[i] = tempcps[i];
         bHook[i] = bTempHook[i];
      }
   }
   // end rotate

   AMPatchFlags flags;
   flags.m_flags = 0L;
   BOOL bFlip[5] = { 0,0,0,0,0 };
   BOOL bUseHook[5] = { 0,0,0,0,0 };
   BOOL bHookData[5] = { 0,0,0,0,0 };
   BOOL bEndHook[5] = { 0,0,0,0,0 };

   for (int i=0; i<nNumAMVertices; i++)
   {
      CP *pCP = hashcps[i];
      CP *pNextInPatch = hashcps[ ((i+1)<nNumAMVertices) ? (i+1) : 0 ];

      CP *pConstrained = pCP->DrillForTConstrained();
      if (pConstrained && !pCP->IsTConstrained())
      {
         // nHookData is always set on one cp that is a hook (if there are two on the same edge, it is set on the last one in the direction of the spline)
         if (pNextInPatch->DrillForTConstrained() && !pNextInPatch->IsTConstrained())
         {
         }
         else
            bHookData[i] = TRUE;

         // bUseHook
         CP *pHookNeighbor = pCP->NextInSpline(TRUE);
         if (!pHookNeighbor)
            pHookNeighbor = pCP->PreviousInSpline(TRUE);
         if (pHookNeighbor->DrillForCP(pNextInPatch))
            bUseHook[i>0 ? (i-1) : (nNumAMVertices-1)] = TRUE;
         else
            bUseHook[i] = TRUE;

         // bEndHook (TODO: something still wrong with this logic, but it is time to move on.  RC 2-1-05)
         ASSERT( pConstrained->IsTConstrained() );
         CP *pHookAnchor = pConstrained->PreviousInSpline(TRUE);
         ASSERT( pHookAnchor );
         CP *pPrevInPatch = hashcps[ (i>0) ? (i-1) : (nNumAMVertices-1) ];
         if (!pHookAnchor->DrillForCP(pPrevInPatch) && !pHookAnchor->DrillForCP(pNextInPatch))
            bEndHook[i] = TRUE;

         if (pNextInPatch->DrillForCP( pHookNeighbor ))
         {
            CP *pNextInSpline = pCP->NextInSpline(TRUE);
            if (pNextInSpline && pNextInSpline->IsTConstrained())
               pNextInSpline = pNextInSpline->NextInSpline(TRUE);
            bFlip[i] = pNextInPatch->DrillForCP( pNextInSpline );
         }
         else
            bFlip[i] = pPrevInPatch->DrillForCP( pHookNeighbor );
      }
      else
      {
         for (CP *pNextInSpline=pCP->NextInSpline(TRUE); pNextInSpline && pNextInSpline->IsTConstrained(); pNextInSpline=pNextInSpline->NextInSpline(TRUE))
            ;
         bFlip[i] = pNextInPatch->DrillForCP( pNextInSpline );
      }
   }

   flags.m_ishook1 = bHook[0];
   flags.m_isflipp1 = bFlip[0];
   flags.m_isusehook1 = bUseHook[0];
   flags.m_ishookdata1 = bHookData[0];
   flags.m_isendhook1 = bEndHook[0];

   flags.m_ishook2 = bHook[1];
   flags.m_isflipp2 = bFlip[1];
   flags.m_isusehook2 = bUseHook[1];
   flags.m_ishookdata2 = bHookData[1];
   flags.m_isendhook2 = bEndHook[1];

   flags.m_ishook3 = bHook[2];
   flags.m_isflipp3 = bFlip[2];
   flags.m_isusehook3 = bUseHook[2];
   flags.m_ishookdata3 = bHookData[2];
   flags.m_isendhook3 = bEndHook[2];

   if (nNumAMVertices > 3)
   {
      flags.m_ishook4 = bHook[3];
      flags.m_isflipp4 = bFlip[3];
      flags.m_isusehook4 = bUseHook[3];
      flags.m_ishookdata4 = bHookData[3];
      flags.m_isendhook4 = bEndHook[3];
   }

   if (nNumAMVertices == 5)
   {
      flags.m_is5point = TRUE;
      flags.m_isflipp5 = bFlip[4];
   }

   flags.m_groupimagerotation = 0; // TODO

   bs << flags.m_flags << nl;

   for (int i=0; i<nNumAMVertices; i++)
   {
      CP *pCP = hashcps[i];
      if (i > 0)
         bs << sp;
      UINT nID = pCP->GetID();
      bs << nID;
   }
   if (nNumAMVertices == 3)
      bs << sp << hashcps[0]->GetID();
   bs << nl;
}

///////////////////////////////////////////////////////////////
// DECALS

void Model::SaveDecals( BaseStream &bs )
{
   HPatchModel *pModel = GetHPatchModel();
   HSurfaceContainer *pSurfaceContainer = pModel->GetSurfaceContainer();
   if (!pSurfaceContainer)
      return;

   int nNumSurfaces = pSurfaceContainer->GetNumSurfaces();
   if (nNumSurfaces == 0)
      return;

   CList<HSurface *> surfacelist;
   for (int i=0; i<nNumSurfaces; i++)
   {
      HSurface *pSurface = pSurfaceContainer->GetSurfaceAt( i );
      int nNumSurfaceDrivers = pSurface->GetNumSurfaceDrivers();
      for (int j=0; j<nNumSurfaceDrivers; j++)
      {
         HSurfaceDriver *pSurfaceDriver = pSurface->GetSurfaceDriverAt(j);
         if (pSurfaceDriver->IsKindOf(SOT_UVMapSurfaceDriver))
         {
            surfacelist.AddTail( pSurface );
            break;
         }
      }
   }

   bs << "[DECALS]" << nl;
   {
      for (POSITION pos=surfacelist.GetHeadPosition(); pos; )
      {
         HSurface *pSurface = surfacelist.GetNext( pos );
         SaveDecal( bs, pSurface );
      }
   }
   bs << "[ENDDECALS]" << nl;
}

void Model::SaveDecal( BaseStream &bs, HSurface *pSurface )
{
   // Given a surface, figure out how many different texture coordinate sets are used.
   // Then for each set, write out an A:M decal.
   CList<HTexCoords *> stamplist;
   int nNumSurfaceDrivers = pSurface->GetNumSurfaceDrivers();
   for (int i=0; i<nNumSurfaceDrivers; i++)
   {
      HSurfaceDriver *pSurfaceDriver = pSurface->GetSurfaceDriverAt( i );
      HUVMapSurfaceDriver *pUVMapSurfaceDriver = (HUVMapSurfaceDriver *)pSurfaceDriver->IsKindOf(SOT_UVMapSurfaceDriver);
      if (pUVMapSurfaceDriver)
      {
         HTexCoords *pTexCoords = (HTexCoords *)pUVMapSurfaceDriver->GetTexCoords()->GetValue();
         if (pTexCoords && !stamplist.Find(pTexCoords))
            stamplist.AddTail( pTexCoords );
      }
   }

   for (POSITION pos=stamplist.GetHeadPosition(); pos; )
   {
      HTexCoords *pTexCoords = stamplist.GetNext( pos );

      bs << "[DECAL]" << nl;
         bs << "Name=" << pSurface->GetName() << nl;

         bs << "[DECALIMAGES]" << nl;
            for (int i=0; i<nNumSurfaceDrivers; i++)
            {
               HSurfaceDriver *pSurfaceDriver = pSurface->GetSurfaceDriverAt(i);
               HUVMapSurfaceDriver *pUVMapSurfaceDriver = (HUVMapSurfaceDriver *)pSurfaceDriver->IsKindOf(SOT_UVMapSurfaceDriver);
               if (pUVMapSurfaceDriver && (pUVMapSurfaceDriver->GetTexCoords()->GetValue() == pTexCoords))
                  SaveDecalImage( bs, pSurface, pUVMapSurfaceDriver );
            }
         bs << "[ENDDECALIMAGES]" << nl;

         bs << "[STAMPS]" << nl;
            for (int i=0; i<nNumSurfaceDrivers; i++)
            {
               HSurfaceDriver *pSurfaceDriver = pSurface->GetSurfaceDriverAt(i);
               HUVMapSurfaceDriver *pUVMapSurfaceDriver = (HUVMapSurfaceDriver *)pSurfaceDriver->IsKindOf(SOT_UVMapSurfaceDriver);
               if (pUVMapSurfaceDriver && (pUVMapSurfaceDriver->GetTexCoords()->GetValue() == pTexCoords))
               {
                  SaveStamp( bs, pTexCoords, pSurface );
                  break;
               }
            }
         bs << "[ENDSTAMPS]" << nl;
      bs << "[ENDDECAL]" << nl;
   }
}

void Model::SaveDecalImage( BaseStream &bs, HSurface *pSurface, HMapSurfaceDriver *pMapSurfaceDriver )
{
   bs << "[DECALIMAGE]" << nl;
   {
      HImage *pImage = (HImage *)pMapSurfaceDriver->GetImage()->GetValue();
      if (pImage)
      {
         bs << "[Image]" << nl;
            bs << "Value=..|..|..|..|..|..|Images|\"" << pImage->GetFileName() << "\"" << nl;
            bs << "Repeat=1.001 1.001" << nl;// Hack for Barry because textures in Maya repeat by default and look wrong in Hash
         bs << "[EndImage]" << nl;
      }

      HProperty *pProperty = (HProperty *)pMapSurfaceDriver->GetProperty()->GetValue();
      if (pProperty == pSurface->GetNormalOffsetProperty())
         bs << "DecalType=" << "Bump" << nl;
      else if (pProperty == pSurface->GetAmbientProperty())
         bs << "DecalType=" << "Ambiance" << nl;
      else if (pProperty == pSurface->GetSpecularProperty())
         bs << "DecalType=" << "SpecularIntensity" << nl;
      else if (pProperty == pSurface->GetDiffuseProperty() && pMapSurfaceDriver->GetBlendStyle()->GetValue()==HMapSurfaceDriver::REPLACE)
         bs << "DecalType=" << "Cookie-Cut" << nl;

      bs << "Percent=" << pMapSurfaceDriver->GetOpacity()->GetValue() * 100.0f << nl;
   }
   bs << "[ENDDECALIMAGE]" << nl;
}

void Model::SaveStamp( BaseStream &bs, HTexCoords *pTexCoords, HSurface *pSurface )
{
   //  1  2  3  4
   //  5        8
   //  9       12
   // 13 14 15 16
   CMap<HPoly *, HPoly *, UINT, UINT> surfacefaces;
   surfacefaces.InitHashTable(GetPrimeLargerThan( (UINT)(pSurface->GetNumFaces()*1.2f) ));
   int nNumFaces = pSurface->GetNumFaces();

   for (int i=0; i<nNumFaces; i++)
   {
      HPoly *pPoly = pSurface->GetFaceAt(i);
      surfacefaces.SetAt( pPoly, 0 );
   }

   CArray<HUVPoly *> hitfacearray;
   hitfacearray.SetSize(0, nNumFaces);

   UINT unused;
   for (POSITION pos=pTexCoords->GetHeadPropertyPolyPosition(); pos; )
   {
      HPropertyPoly *pPropertyPoly = pTexCoords->GetNextPropertyPoly(pos);
      if (pPropertyPoly->GetPropertySet()->GetSOT()!=SOT_TexCoords)
         continue;
      HUVPoly *pUVPoly = (HUVPoly*)pPropertyPoly;
      if (surfacefaces.Lookup( pUVPoly->GetPoly(), unused))
         hitfacearray.Add(pUVPoly);
   }

   bs << "[STAMP]" << nl;
      bs << "Name=" << pTexCoords->GetName() << nl;

      bs << "[DATA]" << nl;
         bs << "PatchesHit=" << hitfacearray.GetSize() << nl;
         for (int i=0; i<hitfacearray.GetSize(); i++)
         {
            HUVPoly *pUVPoly = hitfacearray[i];
            HPatch *pPatch = (HPatch *)pUVPoly->GetPoly();
            UINT nNumUVs = pUVPoly->GetNumSides();
            UINT nNumAMUVs = 0;
            CArray<UINT> cpids;
            cpids.SetSize( nNumUVs );
            CArray<HUV *> uvs;
            uvs.SetSize( nNumUVs );
            for (UINT i=0; i<nNumUVs; i++)
            {
               HVertex *pVertex = pPatch->GetVertexAt( i );
               UINT nVertexID = pVertex->GetID();
               CP *pCP = NULL;
               m_cpmap.Lookup( nVertexID, pCP );
               ASSERT( pCP );
               pCP = GetCPOnEdge( pPatch->GetEdgeAt( i ), pCP ); // have every vertex lie in a unique edge
               if (pCP->IsTConstrained() && pCP->IsTConstrainedCPInEdge( pPatch, i ))
                  continue;

               if (!pCP->DrillForTConstrained())
                  pCP = pCP->DrillForTop();
               else if (pCP->IsTConstrained())
               {
                  pCP = pCP->DrillForTop();
                  if (pCP->IsTConstrained())
                  {
                     for (pCP=pCP->GetChildCP(); pCP; pCP=pCP->GetChildCP())
                        if (!pCP->IsTConstrained())
                           break;
                     ASSERT( pCP );
                  }
               }

               uvs[nNumAMUVs] = pUVPoly->GetUVAt(i);
               cpids[nNumAMUVs] = pCP->GetID();
               nNumAMUVs++;
            }

            // Find the lowest cp id so we can make it the first vertex
            int nLowestIndex = 0;
            UINT nLowestID = cpids[0];
            for (UINT i=1; i<nNumAMUVs; i++)
            {
               if (cpids[i] < nLowestID)
               {
                  nLowestIndex = i;
                  nLowestID = cpids[i];
               }
            }

            // rotate patch to have the lowest vertex id at index[0]
            if (nLowestIndex > 0)
            {
               CArray<UINT> tempids;
               tempids.SetSize( nNumAMUVs );
               CArray<HUV *> tempuvs;
               tempuvs.SetSize( nNumAMUVs );
               for (UINT i=0; i<nNumAMUVs; i++)
               {
                  tempids[i] = cpids[(nLowestIndex + i) % nNumAMUVs];
                  tempuvs[i] = uvs[(nLowestIndex + i) % nNumAMUVs];
               }
               for (UINT i=0; i<nNumAMUVs; i++)
               {
                  cpids[i] = tempids[i];
                  uvs[i] = tempuvs[i];
               }
            }

            // write out the ids
            for (UINT i=0; i<nNumAMUVs; i++)
            {
               if (i > 0)
                  bs << ' ';

               if (i == 4)
                  bs << "5";
               else
                  bs << cpids[i];
            }

            if (nNumAMUVs==3)
               bs << ' ' << cpids[0];
            bs << nl;

            for (UINT i=0; i<nNumAMUVs; i++)
            {
               HUV *pUV = uvs[i];
               HUV *pNextUV = uvs[ (i+1)<nNumAMUVs ? i+1 : 0 ];
               Vector p1 = pUV->GetPos();
               Vector p4 = pNextUV->GetPos();
               Vector p2 = p1 + (p4 - p1) * 0.33f;
               Vector p3 = p1 + (p4 - p1) * 0.66f;
               bs << p1 << sp << p2 << sp << p3 << nl;
            }
            if (nNumAMUVs==3)
            {
               HUV *pUV = uvs[0];
               HUV *pNextUV = uvs[1];
               Vector p1 = pUV->GetPos();
               Vector p4 = pNextUV->GetPos();
               Vector p2 = p1 + (p4 - p1) * 0.33f;
               Vector p3 = p1 + (p4 - p1) * 0.66f;
               bs << p1 << sp << p2 << sp << p3 << nl;
            }
         }
      bs << "[ENDDATA]" << nl;
   bs << "[ENDSTAMP]" << nl;
}

///////////////////////////////////////////////////////////////
// GROUPS

void Model::SaveGroups( BaseStream &bs )
{
   HPatchModel *pModel = MdlTranslator::GetModel()->GetHPatchModel();
   HSurfaceContainer *pSurfaceContainer = pModel->GetSurfaceContainer();
   HSelectionContainer *pSelectionContainer = pModel->GetSelectionContainer();
   if (!pSurfaceContainer && !pSelectionContainer)
      return;

   bs << "[GROUPS]" << nl;
      if (pSurfaceContainer)
      {
         for (UINT i=0; i<pSurfaceContainer->GetNumSurfaces(); i++)
         {
            HSurface *pSurface = pSurfaceContainer->GetSurfaceAt( i );
            if (pSurface)
            {
               BOOL bSave = FALSE;

               if (pSurface->GetNumChildren()==0)
                  bSave = TRUE;
               else
               {
                  for (int j=0; j<pSurface->GetNumChildren(); j++)
                  {
                     if (pSurface->GetChildAt(j)->IsKindOf(SOT_ClampMapSurfaceDriver))
                     {
                        bSave = TRUE;
                        break;
                     }
                  }
               }

               if (bSave)
                  SaveGroup( bs, pSurface );
            }
         }
      }

      if (pSelectionContainer)
      {
         for (UINT i=0; i<pSelectionContainer->GetNumSelections(); i++)
         {
            HSelection *pSelection = pSelectionContainer->GetSelectionAt(i);
            if (pSelection)
               SaveGroup( bs, pSelection );
         }
      }
   bs << "[ENDGROUPS]" << nl;
}

void Model::SaveGroup( BaseStream &bs, HSurface *pSurface )
{
   CMap<UINT, UINT, UINT, UINT> cpmap;

   for (UINT i=0; i<pSurface->GetNumFaces(); i++)
   {
      HPoly *pPoly = pSurface->GetFaceAt( i );
      for (UINT nVertex=0; nVertex<pPoly->GetNumSides(); nVertex++)
      {
         HVertex *pVertex = pPoly->GetVertexAt( nVertex );
         UINT nVertexID = pVertex->GetID();
         CP *pCP = NULL;
         m_cpmap.Lookup( nVertexID, pCP );
         ASSERT( pCP );
         while (pCP && pCP->IsTConstrained())
            pCP = pCP->PreviousInSpline(TRUE);

         if (pCP)
         {
            CP *pTop = pCP->DrillForTop();
            if (pTop->IsTConstrained())
               cpmap.SetAt( pCP->GetID(), 0 );
            else
               cpmap.SetAt( pTop->GetID(), 0 );
         }
      }
   }

   bs << "[GROUP]" << nl;
      bs << "Name=" << pSurface->GetName() << nl;
      bs << "DiffuseColor=" << pSurface->GetDiffuseProperty()->GetValue()*255.0f << nl;
      bs << "Transparency=" << pSurface->GetTransparencyProperty()->GetValue() << nl;

      bs << "Count=" << cpmap.GetCount() << nl;
      for (POSITION pos=cpmap.GetStartPosition(); pos; )
      {
         UINT nCPID, nValue;
         cpmap.GetNextAssoc( pos, nCPID, nValue );
         bs << nCPID << nl;
      }

      if (pSurface->GetNumSurfaceDrivers())
      {
         bs << "[DECALIMAGES]" << nl;
            for (UINT i=0; i<pSurface->GetNumSurfaceDrivers(); i++)
            {
               HClampMapSurfaceDriver *pClampMap = (HClampMapSurfaceDriver *)pSurface->GetSurfaceDriverAt(i)->IsKindOf(SOT_ClampMapSurfaceDriver);
               if (pClampMap)
                  SaveDecalImage( bs, pSurface, pClampMap );
            }
         bs << "[ENDDECALIMAGES]" << nl;
      }
   bs << "[ENDGROUP]" << nl;
}

void Model::SaveGroup( BaseStream &bs, HSelection *pSelection )
{
   CList<CP *> cplist;

   StringList strErrorList;
   HSnipeObjectList *pList = GetHMainInterface()->DescriptionToObjects( pSelection->GetSelectionDescription(), strErrorList );

   for (POSITION pos=pList->GetHeadPosition(); pos; )
   {
      HSnipeObject *pObject = pList->GetNext( pos );
      if (pObject->IsKindOf(SOT_Vertex))
      {
         HVertex *pVertex = (HVertex *)pObject;
         UINT nVertexID = pVertex->GetID();
         CP *pCP = NULL;
         m_cpmap.Lookup( nVertexID, pCP );
         ASSERT( pCP );
         if (!pCP->IsTConstrained())
            cplist.AddTail( pCP );
      }
   }
   delete pList;

   bs << "[GROUP]" << nl;
      bs << "Name=" << pSelection->GetName() << nl;
      bs << "Count=" << cplist.GetCount() << nl;
      for (POSITION pos=cplist.GetHeadPosition(); pos; )
      {
         CP *pCP = cplist.GetNext( pos );
         bs << pCP->GetID() << nl;
      }
   bs << "[ENDGROUP]" << nl;
}

///////////////////////////////////////////////////////////////
// BONES

void Model::SaveBones( BaseStream &bs )
{
   HPatchModel *pModel = MdlTranslator::GetModel()->GetHPatchModel();
   HBoneContainer *pBoneContainer = pModel->GetBoneContainer();
   if (!pBoneContainer)
      return;

   bs << "[BONES]" << nl;
      SaveChildBones( bs, pBoneContainer );
   bs << "[ENDBONES]" << nl;
}

void Model::SaveChildBones( BaseStream &bs, HHierObject *pParent )
{
   for (int i=0; i<pParent->GetNumChildren(); i++)
   {
      HBone *pBone = (HBone *)pParent->GetChildAt(i)->IsKindOf(SOT_Bone);
      if (pBone)
      {
         switch (pBone->GetSOT())
         {
            case SOT_Bone:
               SaveBone( bs, pBone );
               break;
         }
      }
   }
}

void Model::SaveBone( BaseStream &bs, HBone *pBone )
{
   bs << "[SEGMENT]" << nl;
      bs << "Name=" << pBone->GetName() << nl;

      bs << "[NONSKINNEDCPS]" << nl;
      UINT nVertexCount = pBone->GetNumVertices();
      bs << "Count=" << nVertexCount << nl;
      for (POSITION pos=pBone->GetVertexHeadPosition(); pos; )
      {
         HVertex *pVertex = pBone->GetNextVertex( pos );
         bs << pVertex->GetID() << nl;
      }
      bs << "[ENDNONSKINNEDCPS]" << nl;

      bs << "BoneColor=" << pBone->GetBoneColor()->GetValue()*255 << nl;
      bs << "Start=" << pBone->GetStart()->GetValue() << nl;
      Vector vRotate = pBone->GetRotate()->GetValue();
      RotateEuler euler(vRotate.x, vRotate.y, vRotate.z);
      Quaternion quat(euler);
      bs << "Rotate=" << quat << nl;
      bs << "Length=" << pBone->GetLength()->GetValue() << nl;
      bs << "Chained=" << pBone->GetChained()->GetValue() << nl;
      bs << "Locked=" << pBone->GetLockIK()->GetValue() << nl;
      switch (pBone->GetRollMethod()->GetValue())
      {
         case HBone::RM_YPOLESSINGULARITY:
            bs << "RollMethod=Y-Poles-Singularity" << nl;
            break;
         case HBone::RM_ROLLHISTORY:
            bs << "RollMethod=Roll-History" << nl;
            break;
      }
      switch (pBone->GetManipulationMethod()->GetValue())
      {
         case HBone::MM_TRANSLATEONLY:
            bs << "Manipulation=TranslateOnly" << nl;
            break;
         case HBone::MM_ROTATEONLY:
            bs << "Manipulation=RotateOnly" << nl;
            break;
      }
      bs << "[LimitManipulators]" << nl;
         bs << "Value=" << pBone->GetLimitManipulators()->GetValue() << nl;
         if (!pBone->GetAllowXTranslate()->GetValue())
            bs << "AllowXTranslate=FALSE" << nl;
         if (!pBone->GetAllowYTranslate()->GetValue())
            bs << "AllowYTranslate=FALSE" << nl;
         if (!pBone->GetAllowZTranslate()->GetValue())
            bs << "AllowZTranslate=FALSE" << nl;
         if (!pBone->GetAllowXScale()->GetValue())
            bs << "AllowXScale=FALSE" << nl;
         if (!pBone->GetAllowYScale()->GetValue())
            bs << "AllowYScale=FALSE" << nl;
         if (!pBone->GetAllowZScale()->GetValue())
            bs << "AllowZScale=FALSE" << nl;
         if (!pBone->GetAllowXRotate()->GetValue())
            bs << "AllowXRotate=FALSE" << nl;
         if (!pBone->GetAllowYRotate()->GetValue())
            bs << "AllowYRotate=FALSE" << nl;
         if (!pBone->GetAllowZRotate()->GetValue())
            bs << "AllowZRotate=FALSE" << nl;
      bs << "[EndLimitManipulators]" << nl;

      SaveChildBones( bs, pBone );
   bs << "[ENDSEGMENT]" << nl;
}

///////////////////////////////////////////////////////////////
// PLACEHOLDERS

void Model::SavePlaceholders( BaseStream &bs )
{
   HPatchModel *pModel = MdlTranslator::GetModel()->GetHPatchModel();
   for (int i=0; i<pModel->GetNumChildren(); i++)
   {
      HPlaceholderObject *pPlaceholder = (HPlaceholderObject *)pModel->GetChildAt(i)->IsKindOf(SOT_PlaceholderObject);
      if (pPlaceholder)
      {
         bs << '[' << pPlaceholder->GetName() << ']' << nl;
         bs << pPlaceholder->GetFileText(); 
      }
   }
}
