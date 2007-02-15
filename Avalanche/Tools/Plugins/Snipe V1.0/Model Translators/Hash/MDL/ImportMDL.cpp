//

#include "stdafx.h"
#include "MDL.h"
#include "CP.h"
#include "HTexCoords.h"
#include "HPatchVertex.h"
#include "HPatchEdge.h"
#include "HFloatProperty.h"

void MdlTranslator::OnImport( HPolyModel *pModel, const char *filename )
{
   m_pModel = SNEW Model( pModel );

   MemoryStream hs;
   hs.Open( filename, BaseStream::BS_INPUT );

   m_pModel->Load( hs );

   m_pModel->PeakEdges();

   delete m_pModel;
   m_pModel = NULL;
}

void Model::PeakEdges()
{
   GetHPatchModel()->BuildEdges();
   GetHPatchModel()->BuildVertexNormals();

   for (POSITION pos=m_splinelist.GetHeadPosition(); pos; )
   {
      for (CP *pCP=m_splinelist.GetNext(pos); pCP; pCP=pCP->NextInSpline(FALSE))
      {
         if (!pCP->IsSmooth())
         {
            HEdge *pEdge = pCP->GetNextEdge();
            if (!pEdge)
               pEdge = pCP->GetPrevEdge();
            ASSERT( pEdge );

            HVertexPeakInfo *pPeakInfo = ((HPatchVertex *)pCP->GetVertex())->Peak( pEdge, TRUE );
            delete pPeakInfo;
         }
      }
   }
}

HashObject::ParseValue Model::ParseArg( BaseStream &hs, BaseStream &ls, const String &strKey )
{
   if (HashBone::ParseArg( hs, ls, strKey ) == PARSE_OK)
      return PARSE_OK;

//************************************************************   
// Headers we don't care about, but care about nested Headers   
//************************************************************   
   if (strKey == "MODELFILE"
    || strKey == "ENDMODELFILE"
    || strKey == "POSTEFFECTS"
    || strKey == "ENDPOSTEFFECTS"
    || strKey == "SOUNDS"
    || strKey == "ENDSOUNDS"
    || strKey == "MATERIALS"
    || strKey == "ENDMATERIALS"
    || strKey == "OBJECTS"
    || strKey == "ENDOBJECTS"
    || strKey == "MODEL"
    || strKey == "IMAGES"
    || strKey == "ENDIMAGES"
    || strKey == "DECALS"
    || strKey == "ENDDECALS"
    || strKey == "GROUPS"
    || strKey == "ENDGROUPS"
    || strKey == "BONES"
    || strKey == "ENDBONES")
   {
      return PARSE_OK;
   }
   else if (strKey == "ENDMODEL")
   {
      //OnEndModelLoad();
      return PARSE_OK;
   }
   else if (strKey == "MESH")
   {
      if (LoadMesh(hs))
         return PARSE_OK;
      else {
         DisplayWarning( "Unable to load Splines" );
         return PARSE_ABORTLOAD;   
      }
   }
   else if (strKey == "PATCHES")
   {
      if (LoadPatches(hs))
         return PARSE_OK;
      else 
      {
         DisplayWarning( "Unable to load Patches" );
         return PARSE_ABORTLOAD;   
      }
   }
   else if (strKey == "ProductVersion")
   {
      ls >> m_fProductVersion;
      return PARSE_OK;
   }

   return PARSE_FAIL;
}

///////////////////////////////////////////////////////////////
// MESH

#define MESHVERSION 2
BOOL Model::LoadMesh( BaseStream &hs )
{
   Label label;

   int meshversion;
   hs >> label >> meshversion;
   if (strcmp(label.string, "Version")!=0 || meshversion > MESHVERSION)
      return FALSE;

   int numsplines;
   hs >> label >> numsplines;
   if (strcmp(label.string, "Splines")!=0)
      return FALSE;

   for (int i = 0; i < numsplines; i++)
      if (!ReadSpline( hs ))
         return FALSE;

   String header;
   hs.ReadLine( header ); // [ENDMESH]

   SetConstrainedFlagsForHooks();

   BuildEdges();

   return TRUE;
}

BOOL Model::ReadSpline( BaseStream &hs )
{
   Label label;

   int numcps;
   hs >> label >> numcps;
   if (strcmp(label.string, "CPs")!=0)
      return FALSE;

   CP *pPrev = NULL;
   for (int i = 0; i < numcps; i++)
   {
      CP *pCP = ReadCP( hs, pPrev );
      if (!pCP)
         return FALSE;

      if (!pPrev)
         m_splinelist.AddTail( pCP );

      pPrev = pCP;
   }

   return TRUE;
}

CP *Model::ReadCP( BaseStream &hs, CP *pPrevCP )
{
   static MemoryStream ms(256);

   if (!hs.ReadLine( ms ))
      return NULL;

   AMCPFlags flags;
   signed short isinstance;
   LONG cpindex;
   float hookplacement = 0.5f;
   sscanf( ms.GetBuffer(), "%ld %hd %ld %f", &flags, &isinstance, &cpindex, &hookplacement );

   HPatchModel *pPatchModel = GetHPatchModel();
   CP *pCP = NULL;

   if (!m_cpmap.Lookup( cpindex, pCP ))
      pCP = SNEW CP( cpindex );

   SetCPFlagsFromHashFlags( pCP, flags );

   if (!isinstance)
   {
      HVertex *pVertex = pPatchModel->AddVertex( cpindex );
      for (CP *pCP2=pCP; pCP2; pCP2=pCP2->GetChildCP())
         pCP2->SetVertex( pVertex );

      Vector pos;
      hs >> pos;
      pCP->SetPos( pos );
   }
   else
   {
      int cpinstance;
      hs >> cpinstance;

      CP *pCP2 = NULL;

      if (!m_cpmap.Lookup( cpinstance, pCP2 ))// if cp was already added to pool
      {
         // create new cp to store away for later
         // when we actually reach that cp in the file
         pCP2 = SNEW CP( cpinstance );
         m_cpmap.SetAt( cpinstance, pCP2 );
      }

      pCP->MoveToStackWithChildren( pCP2 );

      if (flags.m_ishook)
      {
         POSITION pos = m_hooks.Find( pCP2 );
         if (pos) // parent is a hook too
            m_hooks.InsertAfter( pos, pCP );
         else
         {
            CP *pChildCP = pCP->GetChildCP();
            if (pChildCP && (pos=m_hooks.Find(pChildCP))) // child is a hook too
               m_hooks.InsertBefore( pos, pCP );
            else
               m_hooks.AddTail( pCP );
         }
      }
   }

   ASSERT(pCP);
   m_cpmap.SetAt( cpindex, pCP );

   //COMEBACK   pCP->SetHookPlacement(hookplacement);

   float fAlpha, fGamma, fMagnitude;
   if (!hs.ReadLine( ms ))
      return NULL;
   if (sscanf( ms.GetBuffer(), "%f %f %f", &fAlpha, &fGamma, &fMagnitude ) == 1) {
      fGamma = 0.0f;
      fMagnitude = 1.0f;
   }
   pCP->SetInAlpha( fAlpha );
   pCP->SetInGamma( fGamma );
   pCP->SetInMagnitude( fMagnitude );

   if (!hs.ReadLine( ms ))
      return NULL;
   if (sscanf( ms.GetBuffer(), "%f %f %f", &fAlpha, &fGamma, &fMagnitude ) == 1) {
      fGamma = 0.0f;
      fMagnitude = 1.0f;
   }
   pCP->SetOutAlpha( fAlpha );
   pCP->SetOutGamma( fGamma );
   pCP->SetOutMagnitude( fMagnitude );

   if (flags.m_ishook)
      if (!pCP->IsSmooth())
         pCP->SetSmooth( TRUE );//force all hooks to be smooth

   if (pPrevCP)
      pPrevCP->InsertCPAfter( pCP );

   return pCP;
}

void Model::SetCPFlagsFromHashFlags( CP *pCP, AMCPFlags flags )
{
   pCP->SetSmooth( flags.m_issmooth );
   pCP->SetLoop( flags.m_isloop );
   //this needs set on the cp the hook is attached to
   //pCP->SetConstrainedFlag( flags.m_ishook );
}

void Model::SetConstrainedFlagsForHook( CP *pHook )
{
   UINT nHookID = pHook->GetID();
   for (CP *pAnchor=pHook->GetParentCP(); pAnchor && m_hooks.Find(pAnchor); pAnchor=pAnchor->GetParentCP())
      ;
   for (CP *pPrev=pAnchor; pPrev->NextInSpline(TRUE) && pPrev->NextInSpline(TRUE)->IsTConstrained(); pPrev=pPrev->NextInSpline(TRUE))
      ;

   CP *pNewCP = SNEW CP( pHook->GetID() );
   m_cpmap.SetAt( pNewCP->GetID(), pNewCP );
   HVertex *pVertex = GetHPatchModel()->AddVertex( pNewCP->GetID() );
   pVertex->SetPos(0);
   pNewCP->SetVertex( pVertex );
   pPrev->InsertCPAfter( pNewCP );
   pHook->MoveToStack( pNewCP );
static UINT nNextHookID = 10000000;
   pHook->SetID( nNextHookID++ );
   m_cpmap.SetAt( pHook->GetID(), pHook );
   pNewCP->SetConstrained( TRUE );
}

void Model::SetConstrainedFlagsForHooks()
{
   for (POSITION pos=m_hooks.GetHeadPosition(); pos; )
   {
      CP *pHook = m_hooks.GetNext( pos );
      SetConstrainedFlagsForHook( pHook );
   }
}

void Model::BuildEdges()
{
   HPatchModel *pModel = GetHPatchModel();

   for (POSITION pos=m_splinelist.GetHeadPosition(); pos; )
   {
      CP *pHeadCP = m_splinelist.GetNext( pos );

      HPatchEdge *pPrevEdge = NULL;
      for (CP *pCP0=pHeadCP; pCP0; pCP0=pCP0->NextInSpline(FALSE))
      {
         CP *pCP1 = pCP0->NextInSpline( TRUE );
         if (pCP1 && !pCP0->IsAttached(pCP1))
         {
            HPatchEdge *pEdge = (HPatchEdge *)pModel->AddEdge();
            pEdge->SetFirstVertex( pCP0->GetVertex() );
            pEdge->SetSecondVertex( pCP1->GetVertex() );

            pEdge->GetFirstVertexHandle(FALSE)->GetAlphaProperty()->StoreValue( pCP0->GetOutAlpha(), FALSE );
            pEdge->GetFirstVertexHandle(FALSE)->GetGammaProperty()->StoreValue( pCP0->GetOutGamma(), FALSE );
            pEdge->GetFirstVertexHandle(FALSE)->GetMagnitudeProperty()->StoreValue( pCP0->GetOutMagnitude(), FALSE );

            pEdge->GetSecondVertexHandle(FALSE)->GetAlphaProperty()->StoreValue( pCP1->GetInAlpha(), FALSE );
            pEdge->GetSecondVertexHandle(FALSE)->GetGammaProperty()->StoreValue( pCP1->GetInGamma(), FALSE );
            pEdge->GetSecondVertexHandle(FALSE)->GetMagnitudeProperty()->StoreValue( pCP1->GetInMagnitude(), FALSE );

            pCP0->SetNextEdge( pEdge );
            pCP1->SetPrevEdge( pEdge );

            if (pPrevEdge)
               pPrevEdge->SetNextContEdge( pEdge );
            else
               pEdge->SetPrevContEdge( NULL );

            if (pCP1 == pHeadCP) // loop
               pEdge->SetNextContEdge( (HPatchEdge *)pHeadCP->GetNextEdge() );
            else
               pEdge->SetNextContEdge( NULL );

            pPrevEdge = pEdge;

            if (pCP0->IsConstrained())
               ((HPatchVertex *)pCP0->GetVertex())->SetConstrained( pEdge );
         }
      }
   }
}

///////////////////////////////////////////////////////////////
// PATCHES

#define PATCHVERSION  3
BOOL Model::LoadPatches( BaseStream &hs )
{
   m_vmap.Clear();

   Label label;
   int patchversion;
   hs >> label >> patchversion;
   if (strcmp(label.string, "Version")!=0 || patchversion > PATCHVERSION)
      return FALSE;

   int nPatchCount;
   hs >> label >> nPatchCount;
   if (strcmp(label.string, "Count")!=0)
      return FALSE;

   if (nPatchCount == 0)
      return TRUE;

   for (int i = 0; i < nPatchCount; i++)// read in quad patches
   {
      if (!ReadPatch( hs, FALSE ))
         return FALSE;
   }

   int nPatch5Count;
   hs >> nPatch5Count;
   for (i = 0; i < nPatch5Count; i++)
   {
      if (!ReadPatch( hs, TRUE ))
         return FALSE;
   }

   String header;
   hs.ReadLine( header ); // [ENDPATCHES]

   return TRUE;
}

