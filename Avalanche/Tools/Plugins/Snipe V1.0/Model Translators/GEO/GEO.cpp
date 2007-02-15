
#include "Stdafx.h"
#include "HPolyModel.h"
#include "HVertex.h"
#include "HVertexNormal.h"
#include "HUV.h"
#include "HPoly.h"
#include "GEO.h"
#include "FileStream.h"

#define GEO_VERSION (UINT)2

GEOTranslator::GEOTranslator()
{
}

void GEOTranslator::OnExport(HPolyModel *pModel, const char *strFileName)
{
   FileStream fs;
   
   fs.Open( strFileName, BaseStream::BS_OUTPUT | BaseStream::BS_BINARY );

   fs << GEO_VERSION;

   fs << pModel->GetNumVertices();
   CMap< HVertex *, HVertex *, UINT, UINT > vertexmap;
   UINT nIndex=0;
   for (POSITION pos=pModel->GetHeadVertexPosition(); pos; )
   {
      HVertex *pVertex = pModel->GetNextVertex( pos );
      fs << pVertex->GetPos();
      vertexmap.SetAt( pVertex, nIndex++ );
   }

   fs << pModel->GetNumVertexNormals();
   CMap< HVertexNormal *, HVertexNormal *, UINT, UINT > vertexnormalmap;
   nIndex = 0;
   for (POSITION pos=pModel->GetHeadVertexNormalPosition(); pos; )
   {
      HVertexNormal *pVertexNormal = pModel->GetNextVertexNormal( pos );
      fs << pVertexNormal->GetDir();
      vertexnormalmap.SetAt( pVertexNormal, nIndex++ );
   }
/*
   fs << pModel->GetNumUVs();
   CMap< HUV *, HUV *, UINT, UINT > uvmap;
   nIndex = 0;
   for (HUV *pUV=pModel->GetFirstUV(); pUV; pUV=pUV->Next())
   {
      fs << pUV->GetPos();
      uvmap.SetAt( pUV, nIndex++ );
   }
*/
   fs << pModel->GetNumPolys();
   for (POSITION pos=pModel->GetHeadFacePosition(); pos; )
   {
      HPoly *pPoly = pModel->GetNextFace( pos );
      UINT nNumVertices = pPoly->GetNumSides();
      fs << nNumVertices;
      //UINT nNumUVSets = 0;//pPoly->GetNumUVSets();
      //fs << nNumUVSets;
      for (UINT i=0; i<nNumVertices; i++)
      {
         vertexmap.Lookup( pPoly->GetVertexAt(i), nIndex );
         fs << nIndex;
         vertexnormalmap.Lookup( pPoly->GetVertexNormalAt(i), nIndex );
         fs << nIndex;
         //for (UINT nUVSet=0; nUVSet<nNumUVSets; nUVSet++)
         //{
         //   uvmap.Lookup( pPoly->GetUVAt(i, nUVSet), nIndex );
         //   fs << nIndex;
         //}
      }
   }
}