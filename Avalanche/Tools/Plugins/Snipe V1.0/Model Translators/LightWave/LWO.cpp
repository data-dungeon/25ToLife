
#include "Stdafx.h"
#include "LWPlugin.h"
#include "HPolyModel.h"
#include "HPoly.h"
#include "LWObject/lwo2.h"
#include <afxtempl.h>
#include "LWO.h"

LWOTranslator::LWOTranslator()
{
}

void LWOTranslator::OnImport(HPolyModel *pModel, const char *strFileName)
{
   unsigned int failID = 0;
   int failpos = 0;
   lwObject *obj = lwGetObject( (char *)strFileName, &failID, &failpos );
   if (obj == NULL)
      THROW_FATAL_ERROR("lwGetObject failed");

   CArray< HVertex * > vertexpointerarray;
   CArray< HVertexNormal * > vertexnormalpointerarray;
   for (int nLayer=0; nLayer<obj->nlayers; nLayer++) {
      lwLayer *pLayer = &obj->layer[nLayer];
      for (int nLayerVert=0; nLayerVert<pLayer->point.count; nLayerVert++) {
         Vector v = *(Vector *)pLayer->point.pt[nLayerVert].pos;
         v *= 100.0f; // mm to cm
//            v.z = -v.z;
         vertexpointerarray.Add( pModel->AddVertex( v ));
         vertexnormalpointerarray.Add( pModel->AddVertexNormal( /*v.Normalized()*/));
      }

      for (int nLayerPolyIndex=0; nLayerPolyIndex<pLayer->polygon.count; nLayerPolyIndex++) {
         lwPolygon *pLWPoly = &pLayer->polygon.pol[nLayerPolyIndex];
         HPoly *pHPoly = pModel->AddPoly();
         pHPoly->SetNumSides( pLWPoly->nverts );
         for (int i=0; i<pLWPoly->nverts; i++) {
            pHPoly->SetVertexAt( i, vertexpointerarray[ pLWPoly->v[i].index] );
            pHPoly->SetVertexNormalAt( i, vertexnormalpointerarray[ pLWPoly->v[i].index] );
         }
      }
   }

   lwFreeObject( obj );

   pModel->BuildPolys();
}