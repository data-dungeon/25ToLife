#include "stdafx.h"
#include "tup.h"
#include "HPatchVertex.h"
#include "HVertexNormal.h"
#include "HUV.h"
#include "HPolyModel.h"
#include "HUVPoly.h"
#include "HPatchModel.h"
#include "HPatch.h"
#include "HTessPoly.h"
#include "Vector.h"
#include "Vector2.h"
#include "HTexCoords.h"
#include "HUVPatch.h"
#include "HImage.h"
#include "HImageContainer.h"
#include "HColorProperty.h"
#include "HSurface.h"
#include "HSurfaceDriver.h"
#include "HUVMapSurfaceDriver.h"
#include "HTexCoordsPointerProperty.h"
#include "HImagePointerProperty.h"
#include "HTypeProperty.h"
#include "HPropertyPointerProperty.h"
#include "HFloatProperty.h"
#include "HTransform2Property.h"
#include "HVector2Property.h"
#include "HBone.h"
#include "HTransformProperty.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"
#include "HScaleProperty.h"
#include "Matrix44.h"
#include "CRC.h"
#include "ExportTupSceneOptionsDlg.h"
#include "HTessPropertyPoly.h"
#include "HPropertyPatch.h"
#include "HPropertySet.h"
#include "HTessUVPoly.h"

void TupModelTranslator::OnExport(HPolyModel *pModel, const char *szFileName)
{
   ExportTupSceneOptionsDlg dlg;
   if (dlg.DoModal() != IDOK)
      return;

   m_imagelookup.Clear();
   m_surfacelookup.Clear();
   m_texcoordslookup.Clear();
   m_maplookup.Clear();
   m_bonelookup.Clear();
   m_vmap.Clear();
   m_materiallookup.RemoveAll();
   m_vertexboneinfoarray.RemoveAll();
   m_vertexboneinfomap.Clear();

   InitTupSceneForExport(szFileName);

   RecurseBranch(pModel,m_pWorldNodeAgg);

   // save the tree
   TupperwareLoadSaveManager::FormatType formatType = TupperwareLoadSaveManager::ASCII;
   if (dlg.m_bBinary)
      formatType = TupperwareLoadSaveManager::BINARY;

   int result = TupperwareLoadSaveManager::Write(formatType,szFileName,m_pSceneAgg);
   
   DestroyTupScene();

   if (!result)
      THROW_FATAL_ERROR("Failed to write Tup file.");
}

void TupModelTranslator::InitTupSceneForExport(const char *szFileName)
{
   // create a SNEW scene
   m_pSceneAgg = SNEW TupperwareAggregate;
   m_pTupScene = SNEW TupScene(m_pSceneAgg);

   // create the node pool
   TupperwareAggregate *pNodePoolAgg = m_pTupScene->AddNodePool();
   m_pTupNodePool = SNEW TupNodePool(pNodePoolAgg);

   // create the object pool
   TupperwareAggregate *pObjectPoolAgg = m_pTupScene->AddObjectPool();
   m_pTupObjectPool = SNEW TupObjectPool(pObjectPoolAgg);

   // create the material pool
   TupperwareAggregate *pMaterialPoolAgg = m_pTupScene->AddMaterialPool();
   if (pMaterialPoolAgg)
      m_pTupMaterialPool = SNEW TupMaterialPool(pMaterialPoolAgg);

   // create the map pool
   TupperwareAggregate *pMapPoolAgg = m_pTupScene->AddMapPool();
   if (pMapPoolAgg)
      m_pTupMapPool = SNEW TupMapPool(pMapPoolAgg);

   // create the modifier pool
   TupperwareAggregate *pModifierPoolAgg = m_pTupScene->AddModifierPool();
   if (pModifierPoolAgg)
      m_pTupModifierPool = SNEW TupModifierPool(pModifierPoolAgg);

   // create the image pool
   TupperwareAggregate *pImagePoolAgg = m_pTupScene->AddImagePool();
   if (pImagePoolAgg)
      m_pTupImagePool = SNEW TupImagePool(pImagePoolAgg);

   // add in the date, time and version
   m_pTupScene->AddExporterDate(g_pExporterDate);
   m_pTupScene->AddExporterTime(g_pExporterTime);
   m_pTupScene->AddVersion(TupScene::GetSceneVersion());
   m_pTupScene->AddUnitFlags(TupScene::TIME_UNIT_SECONDS | TupScene::LINEAR_UNIT_METER);
   TupString sourceFileName = szFileName;
   m_pTupScene->AddSourceFileName(sourceFileName);

   int m_rootNodeRef = 0;
   TupperwareAggregate *pTupRootNodeAgg = m_pTupNodePool->AddNode(m_rootNodeRef);
   m_pWorldNodeAgg = pTupRootNodeAgg;
   m_pTupScene->AddRootNodeReference(m_rootNodeRef);

   TupNode tupRootNode(pTupRootNodeAgg);
   tupRootNode.AddName("world");
   tupRootNode.AddParentNodeReference(-1); // root has -1 as its parent reference

   ExportDefaultMaterial();
}

void TupModelTranslator::RecurseBranch(HHierObject *pHierObject,TupperwareAggregate *pParentAgg)
{
   if (!pHierObject)
      return;

   TupperwareAggregate *pNodeAgg = NULL;

   int nObjectRef = -1;
   if (pHierObject->IsKindOf(SOT_TransformableObject))
   {
      HInstanceObject *pInstance = (HInstanceObject*)pHierObject;
      HTransformableObject *pTransformableObject = (HTransformableObject*)pHierObject;
      if (pInstance->GetCache())
         pTransformableObject = (HTransformableObject*)pInstance->GetCache();

      switch(pHierObject->GetSOT())
      {
      case SOT_PolyModel :
         {
            pNodeAgg = ExportNode(pTransformableObject,pParentAgg);
         }
         break;
//      case SOT_BezierSplineModel :
      case SOT_PatchModel :
         {
            pNodeAgg = ExportNode(pTransformableObject,pParentAgg);
         }
         break;
      default :
         {
            pNodeAgg = pParentAgg;
         }
      }
   }
   else
   {
      pNodeAgg = pParentAgg;
   }
  
   int nNumChildren = pHierObject->GetNumChildren();
   HHierObject *pChildHierObject;
   for (int nChildIndex = 0; nChildIndex < nNumChildren; nChildIndex++)
   {
      pChildHierObject = (HHierObject*)pHierObject->GetChildAt(nChildIndex);
      RecurseBranch(pChildHierObject,pNodeAgg);
   }
}

TupperwareAggregate *TupModelTranslator::ExportNode(HHierObject *pHierObject,TupperwareAggregate *pParentAgg)
{
   TupNode tupParentNode(pParentAgg);

   int nNodeRef = m_pTupNodePool->AddNode();
   tupParentNode.AddChildNodeReference(nNodeRef);

   TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nNodeRef);

   TupNode tupNode(pNodeAgg);
   tupNode.AddName(pHierObject->GetName().Get());
   int nParentRef = tupParentNode.GetNodeReference();
   tupNode.AddParentNodeReference(nParentRef);

   ExportTransform((HTransformableObject*)pHierObject,pNodeAgg);

   tupNode.AddFlags(0);

   tupNode.AddObjectReference(ExportObject(pHierObject,pNodeAgg));

   return pNodeAgg;
}

int TupModelTranslator::ExportObject(HHierObject *pHierObject,TupperwareAggregate *pNodeAgg)
{
   switch(pHierObject->GetSOT())
   {
   case SOT_PolyModel : 
      {
         return ExportSkinnedModel((HPolyModel*)pHierObject,pNodeAgg);
      }
      break;
//   case SOT_BezierSplineModel : 
   case SOT_PatchModel : 
      {
         return ExportSkinnedModel((HPolyModel*)pHierObject,pNodeAgg);
      }
      break;
   default :
      {

      }
   }
   return -1;
}

int TupModelTranslator::ExportSkinnedModel(HPolyModel *pPolyModel,TupperwareAggregate *pNodeAgg)
{
   HBoneContainer *pBoneContainer = pPolyModel->GetBoneContainer();

  if (!pBoneContainer)
      return ExportPolyMesh(pPolyModel,pNodeAgg);

   int nDerivedObjectRef = m_pTupObjectPool->AddObjectAggregate();
   TupperwareAggregate *pDerivedObjectAgg = m_pTupObjectPool->GetObjectAggregate(nDerivedObjectRef);
   TupObject tupObject(pDerivedObjectAgg);
   tupObject.SetObjectType(TupObject::DERIVED);

   TupDerived tupDerived(pDerivedObjectAgg);
   int nObjectRef = ExportPolyMesh(pPolyModel,pNodeAgg);
   tupDerived.AddObjectReference(nObjectRef);

   TupperwareAggregate *pDerivedModifierAgg = tupDerived.AddModifier(0);
   TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);

   // modifier
   int nModifierIndex = m_pTupModifierPool->AddModifier();
   tupDerivedModifier.AddModifierReference(nModifierIndex);
   TupperwareAggregate *pSkinModifierAgg = m_pTupModifierPool->GetModifier(nModifierIndex);
   TupModifier tupModifier(pSkinModifierAgg);
   tupModifier.SetModifierType(TupModifier::SKIN_MODIFIER);

   TupSkinModifier tupSkinModifier(pSkinModifierAgg);
   tupSkinModifier.AddName("Skin");
   tupSkinModifier.AddFlags(TupSkinModifier::FLAGS_ENABLED);

   m_nSkinModifierBoneCount = 0;

   int nNumChildren = pBoneContainer->GetNumTransformableObjects();
   HTransformableObject *pChildHierObject;
   for (int nChildIndex = 0; nChildIndex < nNumChildren; nChildIndex++)
   {
      pChildHierObject = pPolyModel->GetBoneContainer()->GetTransformableObjectAt(nChildIndex);
      ExportBone(pChildHierObject,m_pWorldNodeAgg,pSkinModifierAgg);
   }

   ExportSkinModifierContext(pDerivedModifierAgg,pPolyModel);

   return nDerivedObjectRef;

}

int TupModelTranslator::ExportPolyMesh(HPolyModel *pModel,TupperwareAggregate *pNodeAgg)
{
   const char *debug = pModel->GetName().Get();

   int nObjectRef = m_pTupObjectPool->AddObjectAggregate();
   TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(nObjectRef);

   pObjectAgg->AddScalarString("MayaObjectName",NULL,pModel->GetName().Get());
   TupObject tupObject(pObjectAgg);

   tupObject.SetObjectType(TupObject::POLY_MESH);

   TupPolyMesh tupPolyMesh(pObjectAgg);

   TupperwareAggregate *pMultiSubObjectMaterialAgg = ExportMultiSubObjectMaterial(pNodeAgg);

   // add default sub material
   TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMultiSubObjectMaterialAgg);	
   TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(0);//default always uses 0
   TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
   tupSubMaterial.AddMaterialReference(0);

   // create MeshMaps
   HTexCoordsContainer *pTexCoordsContainer = pModel->GetTexCoordsContainer();
   
   int nNumTexCoords = 0;
   IntHash *pUVMap = NULL;
   TupperwareAggregate **pMeshMapAggs = NULL;
   int *pUVCount = NULL;

   int nTexCoordIndex;
 
   if (pTexCoordsContainer)
   {
      nNumTexCoords = (int)pTexCoordsContainer->GetNumTexCoords();
      pUVMap = SNEW IntHash[nNumTexCoords];
      pMeshMapAggs = SNEW TupperwareAggregate*[nNumTexCoords];
      pUVCount = SNEW int[nNumTexCoords];

      for (nTexCoordIndex = 0; nTexCoordIndex < nNumTexCoords; nTexCoordIndex++)
      {
         HTexCoords *pTexCoords = pTexCoordsContainer->GetTexCoordsAt(nTexCoordIndex);
         TupperwareAggregate *pMeshMapAgg = tupPolyMesh.AddMeshMap(nTexCoordIndex+1);
         TupPolyMeshMap tupPolyMeshMap(pMeshMapAgg);
         m_texcoordslookup.AddKey((int)pTexCoords,nTexCoordIndex);
         pMeshMapAggs[nTexCoordIndex] = pMeshMapAgg;
      }
   }

   int nCount = 0;
   int nVIndex = 0,nVNIndex = 0;

   // start counts at 1
   for (nTexCoordIndex = 0; nTexCoordIndex < nNumTexCoords; nTexCoordIndex++)
      pUVCount[nTexCoordIndex] = 1;

   IntHash vnmap;

   int nFaceCount = 0;

   BOOL bIsPatchModel=FALSE;
   if (pModel->IsKindOf(SOT_PatchModel))
      bIsPatchModel = TRUE;

   m_vmap.Clear();

   // get count
   for (POSITION pos = pModel->GetHeadFacePosition(); pos; )
   {
      HPatch *pPatch = (HPatch*)pModel->GetNextFace(pos);

      if (bIsPatchModel)
      {
         for (UINT i = 0; i < pPatch->GetNumTessPolys(); i++)
         {
            HTessPoly *pTessPoly = pPatch->GetTessPolyAt(i);
            for (UINT j = 0; j < pTessPoly->GetNumSides(); j++)
            {
               HPatchVertex *pVertex = pTessPoly->GetVertexAt(j);
               HVertexNormal *pVertexNormal = pTessPoly->GetVertexNormalAt(j);

               if (!m_vmap.HasKey((int)pVertex))
                  m_vmap.AddKey((int)pVertex,nVIndex++);

               if (!vnmap.HasKey((int)pVertexNormal))
                  vnmap.AddKey((int)pVertexNormal, nVNIndex++);

               HTessPropertyPoly *pTessPropertyPoly = pTessPoly->GetHeadTessPropertyPoly();
               for (;pTessPropertyPoly;pTessPropertyPoly = pTessPropertyPoly->GetNextTessPropertyPoly())
               {
                  HPropertyPatch *pPropertyPatch = pTessPropertyPoly->GetPropertyPatch();
                  HPropertySet *pPropertySet = pPropertyPatch->GetPropertySet();
                  if (pPropertySet->GetSOT() != SOT_TexCoords)
                     continue;
                  HTessUVPoly *pTessUVPoly = (HTessUVPoly*)pTessPropertyPoly;
                  HTexCoords *pTexCoords = (HTexCoords*)pPropertySet;
                  if (m_texcoordslookup.FindKey((int)pTexCoords,nTexCoordIndex))
                  {
                     HUV *pUV = pTessUVPoly->GetUVAt(j);
                     if (!pUVMap[nTexCoordIndex].HasKey((int)pUV))
                        pUVMap[nTexCoordIndex].AddKey((int)pUV, pUVCount[nTexCoordIndex]++);  
                  }
               }
               nCount++;
            }
            nFaceCount++;
         }
      }
      else
      {
         for (UINT j = 0; j < pPatch->GetNumSides(); j++)
         {
            HVertex *pVertex = pPatch->GetVertexAt(j);
            HVertexNormal *pVertexNormal = pPatch->GetVertexNormalAt(j);
            if (!m_vmap.HasKey((int)pVertex))
               m_vmap.AddKey((int)pVertex,nVIndex++);
            if (!vnmap.HasKey((int)pVertexNormal))
               vnmap.AddKey((int)pVertexNormal,nVNIndex++);

            HPropertyPoly *pPropertyPoly = pPatch->GetHeadPropertyPoly();
            for (;pPropertyPoly;pPropertyPoly = pPropertyPoly->GetNextPropertyPoly())
            {  
               if (pPropertyPoly->GetPropertySet()->GetSOT() != SOT_TexCoords)
                  continue;

               HUVPoly *pUVPoly = (HUVPoly*)pPropertyPoly;
               HTexCoords *pTexCoords = (HTexCoords*)pPropertyPoly->GetPropertySet();
               if (m_texcoordslookup.FindKey((int)pTexCoords,nTexCoordIndex))
               {
                  HUV *pUV = pUVPoly->GetUVAt(j);
                  if (!pUVMap[nTexCoordIndex].HasKey((int)pUV))
                     pUVMap[nTexCoordIndex].AddKey((int)pUV, pUVCount[nTexCoordIndex]++);
               }
            }

            nCount++;
         }
         nFaceCount++;
      }
   }

   // positions	
   int nNumPositionVerts = nVIndex;
   float *pPositionVerts = SNEW float[nNumPositionVerts*3];
   Vector *pVertexArray = (Vector*)pPositionVerts;

   // normals	
   int nNumNormalVerts = nVNIndex;
   float *pNormalVerts = SNEW float[nNumNormalVerts*3];
   Vector *pVertexNormalArray = (Vector*)pNormalVerts;

   // uvs
   float **pUVVerts = SNEW float*[nNumTexCoords];
   for (nTexCoordIndex = 0; nTexCoordIndex < nNumTexCoords; nTexCoordIndex++)
      pUVVerts[nTexCoordIndex] = SNEW float[(pUVCount[nTexCoordIndex]+1)*3];

   // facenormals
   int nNumFaces = nFaceCount;
   int nNumFaceNormalVerts = nNumFaces;
   float *pFaceNormalVerts = SNEW float[nNumFaceNormalVerts*3];
   Vector *pFaceNormalArray = (Vector*)pFaceNormalVerts;

   // set padding
   for (nTexCoordIndex = 0; nTexCoordIndex < nNumTexCoords; nTexCoordIndex++)
   {
      pUVVerts[nTexCoordIndex][0] = 0.0f;
      pUVVerts[nTexCoordIndex][1] = 0.0f;
      pUVVerts[nTexCoordIndex][2] = 0.0f;
   }

   int *pFaceVertexCounts = SNEW int[nNumFaces];
   int *pPositionIndices = SNEW int[nCount];
   int *pNormalIndices = SNEW int[nCount];
   int *pFaceNormalIndicies = SNEW int[nNumFaces];

   int **pUVIndices = NULL;
   if (pTexCoordsContainer)
   {
      pUVIndices = SNEW int*[nNumTexCoords];
      for (nTexCoordIndex = 0; nTexCoordIndex < nNumTexCoords; nTexCoordIndex++)
      {
         pUVIndices[nTexCoordIndex] = SNEW int[nCount];
         memset(pUVIndices[nTexCoordIndex],0,sizeof(int)*nCount);
      }  
   }

   TupArray<int> faceFlags;
   TupArray<int> faceMaterialIndices;
   faceFlags.SetSize(nNumFaces);
   faceMaterialIndices.SetSize(nNumFaces);

   nCount = 0;
   nFaceCount = 0;

   int nUVIndex;

   // build vertex, vertexnormal, and uv hash tables
   for (POSITION pos = pModel->GetHeadFacePosition(); pos; )
   {
      HPatch *pPatch = (HPatch*)pModel->GetNextFace(pos);
      if (bIsPatchModel)
      {
         for (UINT i = 0; i < pPatch->GetNumTessPolys(); i++)
         {
            HTessPoly *pTessPoly = pPatch->GetTessPolyAt(i);

            // add face normal
            pFaceNormalArray[nFaceCount] = *pTessPoly->GetFaceNormal();
            pFaceNormalIndicies[nFaceCount] = nFaceCount;

            for (UINT j = 0; j < pTessPoly->GetNumSides(); j++)
            {
               HPatchVertex *pVertex = pTessPoly->GetVertexAt(j);
               HVertexNormal *pVertexNormal = pTessPoly->GetVertexNormalAt(j);

               // add vertex to pool
               if (!m_vmap.FindKey((int)pVertex,nVIndex))
                  ASSERT(FALSE);

               pVertexArray[nVIndex] = pVertex->GetPos();
               pVertexArray[nVIndex] *= 0.01f;//convert centimeters to meters
               // add vertex index to face
               pPositionIndices[nCount] = nVIndex;

               // add vertexnormal to pool
               if (!vnmap.FindKey((int)pVertexNormal,nVNIndex))
                  ASSERT(FALSE);

               pVertexNormalArray[nVNIndex] = pVertexNormal->GetDir();
               // add vertexnormal index to face
               pNormalIndices[nCount] = nVNIndex;

               HUV *pUV = NULL;

               HTessPropertyPoly *pTessPropertyPoly = pTessPoly->GetHeadTessPropertyPoly();
               for (;pTessPropertyPoly;pTessPropertyPoly = pTessPropertyPoly->GetNextTessPropertyPoly())
               {
                  HPropertyPatch *pPropertyPatch = pTessPropertyPoly->GetPropertyPatch();
                  HPropertySet *pPropertySet = pPropertyPatch->GetPropertySet();
                  if (pPropertySet->GetSOT() != SOT_TexCoords)
                     continue;
                  HTessUVPoly *pTessUVPoly = (HTessUVPoly*)pTessPropertyPoly;
                  HTexCoords *pTexCoords = (HTexCoords*)pPropertySet;
                  if (m_texcoordslookup.FindKey((int)pTexCoords,nTexCoordIndex))
                  {
                     pUV = pTessUVPoly->GetUVAt(j);

                     if (!pUVMap[nTexCoordIndex].FindKey((int)pUV,nUVIndex))
                        ASSERT(FALSE);
                  
                     pUVVerts[nTexCoordIndex][nUVIndex*3] = pUV->GetPos().x;
                     pUVVerts[nTexCoordIndex][nUVIndex*3+1] = pUV->GetPos().y;
                     pUVVerts[nTexCoordIndex][nUVIndex*3+2] = 0.0f;
                     pUVIndices[nTexCoordIndex][nCount] = nUVIndex;
                  }
               }

               nCount++;
            }

            pFaceVertexCounts[nFaceCount] = pTessPoly->GetNumSides();

            faceFlags[nFaceCount] = 4;

            faceMaterialIndices[nFaceCount] = GetMaterialIndex((HPoly*)pPatch,pMultiSubObjectMaterialAgg);

            nFaceCount++;
         }
      }
      else
      {
         // add face normal
         pFaceNormalArray[nFaceCount] = *pPatch->GetFaceNormal();
         pFaceNormalIndicies[nFaceCount] = nFaceCount;

         for (UINT j = 0; j < pPatch->GetNumSides(); j++)
         {
            HVertex *pVertex = pPatch->GetVertexAt(j);
            HVertexNormal *pVertexNormal = pPatch->GetVertexNormalAt(j);

            // add vertex to pool
            if (!m_vmap.FindKey((int)pVertex,nVIndex))
               ASSERT(FALSE);

            pVertexArray[nVIndex] = pVertex->GetPos();
            pVertexArray[nVIndex] *= 0.01f;//convert centimeters to meters
            // add vertex index to face
            pPositionIndices[nCount] = nVIndex;

            // add vertexnormal to pool
            if (!vnmap.FindKey((int)pVertexNormal,nVNIndex))
               ASSERT(FALSE);

            pVertexNormalArray[nVNIndex] = pVertexNormal->GetDir();
            // add vertexnormal index to face
            pNormalIndices[nCount] = nVNIndex;

            HUV *pUV = NULL;



            HPropertyPoly *pPropertyPoly = pPatch->GetHeadPropertyPoly();
            for (;pPropertyPoly;pPropertyPoly = pPropertyPoly->GetNextPropertyPoly())
            {
               if (pPropertyPoly->GetSOT() != SOT_TexCoords)
                  continue;
               HUVPoly *pUVPoly = (HUVPoly*)pPropertyPoly;
               HTexCoords *pTexCoords = (HTexCoords*)pPropertyPoly->GetPropertySet();
               if (m_texcoordslookup.FindKey((int)pTexCoords,nTexCoordIndex))
               {
                  pUV = pUVPoly->GetUVAt(j);

                  if (!pUVMap[nTexCoordIndex].FindKey((int)pUV,nUVIndex))
                     ASSERT(FALSE);

                  pUVVerts[nTexCoordIndex][nUVIndex*3] = pUV->GetPos().x;
                  pUVVerts[nTexCoordIndex][nUVIndex*3+1] = pUV->GetPos().y;
                  pUVVerts[nTexCoordIndex][nUVIndex*3+2] = 0.0f;

                  pUVIndices[nTexCoordIndex][nCount] = nUVIndex;
               }
            }

            nCount++;
         }

         pFaceVertexCounts[nFaceCount] = pPatch->GetNumSides();

         faceFlags[nFaceCount] = 4;

         faceMaterialIndices[nFaceCount] = GetMaterialIndex((HPoly*)pPatch,pMultiSubObjectMaterialAgg);

         nFaceCount++;
      }
   }

   tupPolyMesh.AddPositionVerts(pPositionVerts,nNumPositionVerts);
   tupPolyMesh.AddNormalVerts(pNormalVerts,nNumNormalVerts);

   tupPolyMesh.AddPositionIndices(pPositionIndices,nCount);
   tupPolyMesh.AddNormalIndices(pNormalIndices,nCount);

   tupPolyMesh.AddFaceVertexCounts(pFaceVertexCounts,nNumFaces);
   tupPolyMesh.AddFaceMaterialIndices(faceMaterialIndices.GetData(),nNumFaces);
   tupPolyMesh.AddFaceFlags(faceFlags.GetData(),nNumFaces);

   tupPolyMesh.AddFaceNormalVerts(pFaceNormalVerts,nNumFaces);
   tupPolyMesh.AddFaceNormalIndices(pFaceNormalIndicies,nNumFaces);

   for (nTexCoordIndex = 0; nTexCoordIndex < nNumTexCoords; nTexCoordIndex++)
   {
      TupPolyMeshMap tupPolyMeshMap(pMeshMapAggs[nTexCoordIndex]);
      tupPolyMeshMap.AddMapVerts(pUVVerts[nTexCoordIndex],pUVCount[nTexCoordIndex]+1);
      tupPolyMeshMap.AddMapIndices(pUVIndices[nTexCoordIndex],nCount);
   }

   delete [] pPositionVerts;
   delete [] pNormalVerts;
   delete [] pPositionIndices;
   delete [] pNormalIndices;
   delete [] pFaceVertexCounts;
   delete [] pFaceNormalVerts;
   delete [] pFaceNormalIndicies;

   for (nTexCoordIndex = 0; nTexCoordIndex < nNumTexCoords; nTexCoordIndex++)
   {
      delete [] pUVVerts[nTexCoordIndex];
      delete [] pUVIndices[nTexCoordIndex];
      pUVMap[nTexCoordIndex].Clear();
   }

   if (pUVVerts)
      delete [] pUVVerts;
   if (pUVIndices)
      delete [] pUVIndices;
   if (pUVMap)
      delete [] pUVMap;
   if (pMeshMapAggs)
      delete [] pMeshMapAggs;
   if (pUVCount)
      delete [] pUVCount;

   return nObjectRef;
}

void TupModelTranslator::ExportTransform(HTransformableObject *pTransformableObject,TupperwareAggregate *pNodeAgg)
{
   TupNode tupNode(pNodeAgg);

   TupperwareAggregate *pTransformAgg = tupNode.AddTransform();
   TupTransform tupTransform(pTransformAgg);

   tupTransform.SetTransformType(TupTransform::STATIC_MATRIX);
   TupStaticMatrix tupStaticMatrix(pTransformAgg);

   float *fMatrix = SNEW float[16];

   Matrix44 matrix = pTransformableObject->GetTransform()->GetObjectToWorldMatrix();
   ConvertMatrix44ToTupmatrix(matrix,fMatrix);

   tupStaticMatrix.AddMatrix(fMatrix);
   tupStaticMatrix.AddFlags(TupStaticMatrix::FLAGS_ROTATION_ORDER_XYZ);

   /* NEEDS DECOMPOSED MATRIX
   tupStaticMatrix.AddTranslation(translationf);
   tupStaticMatrix.AddRotation(rotationf);
   tupStaticMatrix.AddScale(scalef);
   tupStaticMatrix.AddShear(shearf);
   tupStaticMatrix.AddScalePivot(junk0);
   tupStaticMatrix.AddRotationPivot(junk0);
   tupStaticMatrix.AddScalePivotTranslation(junk0);
   tupStaticMatrix.AddRotationPivotTranslation(junk0);
   tupStaticMatrix.AddRotationOrientation(junk0);
   */

   delete [] fMatrix;
}

void TupModelTranslator::ExportDefaultMaterial()
{
   int matRef = m_pTupMaterialPool->AddMaterial();
   TupperwareAggregate *pStandardMaterialAgg = m_pTupMaterialPool->GetMaterial(matRef);	
   TupStandardMaterial tupStandardMaterial(pStandardMaterialAgg);

   TupMaterial tupMaterial(pStandardMaterialAgg);
   tupMaterial.SetMaterialType(TupMaterial::STANDARD_MATERIAL);

   tupStandardMaterial.AddName("DefaultMaterial");
   tupStandardMaterial.AddShadingTypeName("Blinn");

   float ambientColor[3] = {0.0f,0.0f,0.0f};
   tupStandardMaterial.AddAmbientColor(ambientColor);
   float diffuseColor[3] = {0.5f,0.5f,0.5f};
   tupStandardMaterial.AddDiffuseColor(diffuseColor);
   float specularColor[3] = {0.0f,0.0f,0.0f};
   tupStandardMaterial.AddSpecularColor(specularColor);
   float filterColor[3] = {0.5f,0.5f,0.5f};
   tupStandardMaterial.AddFilterColor(filterColor);
   tupStandardMaterial.AddGlossiness(0.1f);
   tupStandardMaterial.AddSpecularLevel(0.0f);
   tupStandardMaterial.AddSpecularSoften(0.1f);
   tupStandardMaterial.AddTransparency(0.0f);
   tupStandardMaterial.AddWireSize(1.0f);
   tupStandardMaterial.AddIndexOfRefraction(1.5f);
   tupStandardMaterial.AddReflectionDimIntensity(0.0f);
   tupStandardMaterial.AddReflectionDimMultiplier(3.0f);
   tupStandardMaterial.AddOpacity(1.0f);
   tupStandardMaterial.AddOpacityFalloff(0.0f);
   tupStandardMaterial.AddTransparencyType(2);
   tupStandardMaterial.AddSelfIllumination(0.0f);
   tupStandardMaterial.AddFlags(0);	
}

TupperwareAggregate *TupModelTranslator::ExportMultiSubObjectMaterial(TupperwareAggregate *pNodeAgg)
{
   int nMatRef = m_pTupMaterialPool->AddMaterial();
   TupNode tupNode(pNodeAgg);
   tupNode.AddMaterialReference(nMatRef);
   TupperwareAggregate *pMultiSubObjectMaterialAgg = m_pTupMaterialPool->GetMaterial(nMatRef);
   TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMultiSubObjectMaterialAgg);		
   tupMultiSubObjectMaterial.AddName("MultiSub");
   TupMaterial tupMaterial(pMultiSubObjectMaterialAgg);
   tupMaterial.SetMaterialType(TupMaterial::MULTI_SUB_OBJECT_MATERIAL);
   return pMultiSubObjectMaterialAgg;
}

int TupModelTranslator::GetMaterialIndex(HPoly *pPoly,TupperwareAggregate *pMultiSubObjectAgg)
{
   TestMaterial testmaterial;
   UINT nNumSurfaces = pPoly->GetNumSurfaces();
   for (UINT nSurfaceIndex = 0; nSurfaceIndex < nNumSurfaces; nSurfaceIndex++)
   {
      HSurface *pSurface = pPoly->GetSurfaceAt(nSurfaceIndex);
      BuildTestMaterial(pSurface,testmaterial);
   }
   int nMatIndex;
   if (!m_materiallookup.Lookup(testmaterial,nMatIndex))
   {
      nMatIndex = ExportTestMaterial(testmaterial,pMultiSubObjectAgg);
      m_materiallookup.SetAt(testmaterial,nMatIndex);
   }

   return nMatIndex;
}

void TupModelTranslator::BuildTestMaterial(HSurface *pSurface,TestMaterial &testmaterial)
{
   testmaterial.m_name = pSurface->GetName();
   testmaterial.m_DiffuseColor[0] = pSurface->GetDiffuseProperty()->GetValue().GetRed();
   testmaterial.m_DiffuseColor[1] = pSurface->GetDiffuseProperty()->GetValue().GetGreen();
   testmaterial.m_DiffuseColor[2] = pSurface->GetDiffuseProperty()->GetValue().GetBlue();

   testmaterial.m_AmbientColor[0] = pSurface->GetAmbientProperty()->GetValue().GetRed();
   testmaterial.m_AmbientColor[1] = pSurface->GetAmbientProperty()->GetValue().GetGreen();
   testmaterial.m_AmbientColor[2] = pSurface->GetAmbientProperty()->GetValue().GetBlue();

   testmaterial.m_SpecularColor[0] = pSurface->GetSpecularProperty()->GetValue().GetRed();
   testmaterial.m_SpecularColor[1] = pSurface->GetSpecularProperty()->GetValue().GetGreen();
   testmaterial.m_SpecularColor[2] = pSurface->GetSpecularProperty()->GetValue().GetBlue();

   testmaterial.m_Transprency = pSurface->GetTransparencyProperty()->GetValue();

   UINT nNumSurfaceDrivers = pSurface->GetNumSurfaceDrivers();
   for (UINT nSurfaceDriverIndex = 0; nSurfaceDriverIndex < nNumSurfaceDrivers; nSurfaceDriverIndex++)
   {
      HSurfaceDriver *pSurfaceDriver = pSurface->GetSurfaceDriverAt(nSurfaceDriverIndex);
      testmaterial.m_surfacedrivers.Add(pSurfaceDriver);
   }
}

int TupModelTranslator::ExportTestMaterial(const TestMaterial &testmaterial,TupperwareAggregate *pMultiSubObjectAgg)
{
   int nMatRef;
   TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMultiSubObjectAgg);
   int nNewSubMaterialIndex = m_materiallookup.GetSize()+1;
   TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(nNewSubMaterialIndex);
   TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
   nMatRef = m_pTupMaterialPool->AddMaterial();
   tupSubMaterial.AddMaterialReference(nMatRef);
   TupperwareAggregate *pStandardMaterialAgg = m_pTupMaterialPool->GetMaterial(nMatRef);
   TupStandardMaterial tupStandardMaterial(pStandardMaterialAgg);

   TupMaterial tupMaterial(pStandardMaterialAgg);
   tupMaterial.SetMaterialType(TupMaterial::STANDARD_MATERIAL);

   tupStandardMaterial.AddName(testmaterial.m_name.Get());
   tupStandardMaterial.AddShadingTypeName("Blinn");

   // add surface properties
   tupStandardMaterial.AddDiffuseColor(testmaterial.m_DiffuseColor);
   tupStandardMaterial.AddAmbientColor(testmaterial.m_AmbientColor);
   tupStandardMaterial.AddSpecularColor(testmaterial.m_SpecularColor);
   float filterColor[3] = {0.5f,0.5f,0.5f};
   tupStandardMaterial.AddFilterColor(filterColor);
   tupStandardMaterial.AddGlossiness(0.1f);
   tupStandardMaterial.AddSpecularLevel(0.0f);
   tupStandardMaterial.AddSpecularSoften(0.1f);
   tupStandardMaterial.AddTransparency(testmaterial.m_Transprency);
   tupStandardMaterial.AddWireSize(1.0f);
   tupStandardMaterial.AddIndexOfRefraction(1.5f);
   tupStandardMaterial.AddReflectionDimIntensity(0.0f);
   tupStandardMaterial.AddReflectionDimMultiplier(3.0f);
   tupStandardMaterial.AddOpacity(1.0f);
   tupStandardMaterial.AddOpacityFalloff(0.0f);
   tupStandardMaterial.AddTransparencyType(2);
   tupStandardMaterial.AddSelfIllumination(0.0f);
   tupStandardMaterial.AddFlags(0);	

   int nMapIndex = 0;

   CMap <HProperty*,HProperty*,MaterialMapInfo,MaterialMapInfo> m_materialmaplookup;

   UINT nNumSurfaceDrivers = testmaterial.m_surfacedrivers.GetSize();
   for (UINT nSurfaceDriverIndex = 0; nSurfaceDriverIndex < nNumSurfaceDrivers; nSurfaceDriverIndex++)
   {
      HSurfaceDriver *pSurfaceDriver = testmaterial.m_surfacedrivers[nSurfaceDriverIndex];
      switch (pSurfaceDriver->GetSOT())
      {
      case SOT_UVMapSurfaceDriver :
         {
            HSnipeObject *pSnipeObject = pSurfaceDriver->GetObjectDriven();
            ASSERT(pSnipeObject);
            if (!pSnipeObject)
               continue;
            if (pSnipeObject->GetSOT()!=SOT_Surface)
               continue;
            HSurface *pSurface = (HSurface*)pSnipeObject;
  
            MaterialMapInfo materialmapinfo;
            if (!m_materialmaplookup.Lookup(pSurfaceDriver->GetProperty()->GetValue(),materialmapinfo))
            {
               // create submaterial map
               TupperwareAggregate *pStandardMaterialMap = tupStandardMaterial.AddMap(nMapIndex);
               TupStandardMaterialMap tupStandardMaterialMap(pStandardMaterialMap);
               if (pSurfaceDriver->GetProperty()->GetValue() == pSurface->GetDiffuseProperty())
                  tupStandardMaterialMap.AddMapName("Diffuse Color");
               else
                  continue;
               materialmapinfo.m_pStandardMaterialMapAgg = pStandardMaterialMap;

               HUVMapSurfaceDriver *pUVMapSurfaceDriver = (HUVMapSurfaceDriver*)pSurfaceDriver;

               tupStandardMaterialMap.AddAmount(pUVMapSurfaceDriver->GetOpacity()->GetValue());
               tupStandardMaterialMap.AddFlags(TupStandardMaterialMap::ENABLED);

               m_materialmaplookup.SetAt(pSurfaceDriver->GetProperty()->GetValue(),materialmapinfo);

               nMapIndex++;
            }
            materialmapinfo.m_nSubMapIndex++;    
         }
         break;
      }
   }

   for (nSurfaceDriverIndex = 0; nSurfaceDriverIndex < nNumSurfaceDrivers; nSurfaceDriverIndex++)
   {
      HSurfaceDriver *pSurfaceDriver = testmaterial.m_surfacedrivers[nSurfaceDriverIndex];
      switch (pSurfaceDriver->GetSOT())
      {
      case SOT_UVMapSurfaceDriver :
         {
            MaterialMapInfo materialmapinfo;
            if (!m_materialmaplookup.Lookup(pSurfaceDriver->GetProperty()->GetValue(),materialmapinfo))
               continue;

            if (materialmapinfo.m_nSubMapIndex>1)
            {
               if (!materialmapinfo.m_pCompositeMapAgg)
               {
                  // create composite map
                  int nMapRef = m_pTupMapPool->AddMap();
                  TupperwareAggregate *pCompositeMap = m_pTupMapPool->GetMap(nMapRef);
                  TupCompositeMap tupCompositeMap(pCompositeMap);
                  TupMap tupMap(pCompositeMap);
                  tupMap.SetMapType(TupMap::COMPOSITE_MAP);
                  tupCompositeMap.AddName("LayeredTexture");
                  TupStandardMaterialMap tupStandardMaterialMap(materialmapinfo.m_pStandardMaterialMapAgg);
                  tupStandardMaterialMap.AddMapReference(nMapRef);
                  materialmapinfo.m_pCompositeMapAgg = pCompositeMap;
               }
               ExportSubMap((HUVMapSurfaceDriver*)pSurfaceDriver,materialmapinfo);
            }
            else
            {
               // create bitmap map
               int nMapRef = m_pTupMapPool->AddMap();
               TupperwareAggregate *pBitmapMap = m_pTupMapPool->GetMap(nMapRef);
               TupBitmapMap tupBitmapMap(pBitmapMap);
               TupMap tupMap(pBitmapMap);
               tupMap.SetMapType(TupMap::BITMAP_MAP);
               tupBitmapMap.AddName("Bitmap Map");
               TupStandardMaterialMap tupStandardMaterialMap(materialmapinfo.m_pStandardMaterialMapAgg);
               tupStandardMaterialMap.AddMapReference(nMapRef);
               ExportBitMap((HUVMapSurfaceDriver*)pSurfaceDriver,pBitmapMap);
            }
          }
         break;
      }
   }
   return nNewSubMaterialIndex;
}

void TupModelTranslator::ExportSubMap(HUVMapSurfaceDriver *pUVMapSurfaceDriver,const MaterialMapInfo &materialmapinfo)
{
   TupCompositeMap tupCompositeMap(materialmapinfo.m_pCompositeMapAgg);
   TupperwareAggregate *pCompositeMapSubMapAgg = tupCompositeMap.AddSubMap(materialmapinfo.m_nSubMapIndex);
   TupCompositeMapSubMap tupCompositeMapSubMap(pCompositeMapSubMapAgg);
   int nBitMapRef = m_pTupMapPool->AddMap();
   TupperwareAggregate *pBitMapAgg = m_pTupMapPool->GetMap(nBitMapRef);
   ExportBitMap(pUVMapSurfaceDriver,pBitMapAgg);
   tupCompositeMapSubMap.AddMapReference(nBitMapRef);
   tupCompositeMapSubMap.AddFlags(TupCompositeMapSubMap::ENABLED);
}

void TupModelTranslator::ExportBitMap(HUVMapSurfaceDriver *pUVMapSurfaceDriver,TupperwareAggregate *pBitMapAgg)
{
   HTexCoords *pTexCoords = pUVMapSurfaceDriver->GetTexCoords()->GetValue();
 
   int nMapChannel;
   if (!m_texcoordslookup.FindKey((int)pTexCoords,nMapChannel))
      return;

   TupMap tupMap(pBitMapAgg);
   tupMap.SetMapType(TupMap::BITMAP_MAP);

   HImage *pImage = pUVMapSurfaceDriver->GetImage()->GetValue();

   TupBitmapMap tupBitmapMap(pBitMapAgg);
   tupBitmapMap.AddName(pImage->GetFileName().GetNameOnlyWithoutSuffix().Get());

   tupBitmapMap.AddMapping(0);
   tupBitmapMap.AddMapChannel(nMapChannel+1);
   tupBitmapMap.AddUOffset(0.0f);
   tupBitmapMap.AddVOffset(0.0f);
   tupBitmapMap.AddUTiling(pUVMapSurfaceDriver->GetImage()->GetTransform()->GetScale()->GetValue().x);
   tupBitmapMap.AddVTiling(pUVMapSurfaceDriver->GetImage()->GetTransform()->GetScale()->GetValue().y);
   tupBitmapMap.AddMapAxis(0);
   tupBitmapMap.AddUAngle(0.0f);
   tupBitmapMap.AddVAngle(0.0f);
   tupBitmapMap.AddWAngle(0.0f);
   tupBitmapMap.AddBlur(1.0f);
   tupBitmapMap.AddBlurOffset(0.0f);
   tupBitmapMap.AddNoiseAmount(1.0f);
   tupBitmapMap.AddNoiseLevels(1);
   tupBitmapMap.AddNoiseSize(1.0f);
   tupBitmapMap.AddCroppingPlacementU(0.0f);
   tupBitmapMap.AddCroppingPlacementV(0.0f);
   tupBitmapMap.AddCroppingPlacementW(1.0f);
   tupBitmapMap.AddCroppingPlacementH(1.0f);
   tupBitmapMap.AddPlacementJitter(1.0f);
   tupBitmapMap.AddStartFrame(0);
   tupBitmapMap.AddPlaybackRate(1.0f);
   tupBitmapMap.AddOutputAmount(0.0f);
   tupBitmapMap.AddOutputRGBOffset(0.0f);
   tupBitmapMap.AddOutputRGBLevel(1.0f);
   tupBitmapMap.AddOutputBumpAmount(1.0f);

   switch (pUVMapSurfaceDriver->GetBlendStyle()->GetValue())
   {
   case HUVMapSurfaceDriver::ADDITIVE :
      {
         tupBitmapMap.AddBlendMode(TupBitmapMap::BLEND_MODE_ADD);
      }
      break;
   case HUVMapSurfaceDriver::MULTIPLY :
      {
         tupBitmapMap.AddBlendMode(TupBitmapMap::BLEND_MODE_MULTIPLY);
      }
      break;
   default :
      {
         tupBitmapMap.AddBlendMode(TupBitmapMap::BLEND_MODE_NONE);
      }
   }
   int nFlags=0;
   //if (pUVMapSurfaceDriver->GetImage()->GetTileU())
   //   nFlags |= TupBitmapMap::TUP_U_WRAP;
   //else if (pUVMapSurfaceDriver->GetImage()->GetTileV())
   //   nFlags |= TupBitmapMap::TUP_V_WRAP;

   tupBitmapMap.AddFlags(nFlags);

   int nImageRef;
   if (!m_imagelookup.FindKey((int)pImage,nImageRef))
   {
      // if image not added then add
      nImageRef = m_pTupImagePool->AddImage();
      TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(nImageRef);
      TupImage tupImage(pImageAgg);
      tupImage.AddFilePathName(pImage->GetFileName().Get());
      m_imagelookup.AddKey((int)pImage,(int)nImageRef);      
   }

   tupBitmapMap.AddImageReference(nImageRef);
}

TestMaterial &TestMaterial::operator = ( const TestMaterial &other )
{
    m_DiffuseColor[0] = other.m_DiffuseColor[0];
    m_DiffuseColor[1] = other.m_DiffuseColor[1];
    m_DiffuseColor[2] = other.m_DiffuseColor[2];

    m_surfacedrivers.SetSize(other.m_surfacedrivers.GetSize());
    for (int i = 0; i < m_surfacedrivers.GetSize(); i++)
       m_surfacedrivers[i] = other.m_surfacedrivers[i];

    return *this;
}

BOOL TestMaterial::operator == ( const TestMaterial &other ) const
{
   CRC crcA,crcB;
   for (int i = 0; i < m_surfacedrivers.GetSize(); i++)
   {
      BOOL bFound = FALSE;
      for (int j = 0; j < other.m_surfacedrivers.GetSize(); j++)
      {
         if (m_surfacedrivers[i] == other.m_surfacedrivers[j])
            bFound = TRUE;
      }
      if (!bFound)
         return FALSE;
   }
   return TRUE;
}

void TupModelTranslator::ExportBone(HTransformableObject *pTransformableObject,TupperwareAggregate *pParentAgg,TupperwareAggregate *pSkinModifierAgg)
{
   TupperwareAggregate *pNodeAgg = pParentAgg;

   int nObjectRef = -1;
#if 0
   if ((pTransformableObject->GetSOT()==SOT_Bone)&&
      (stricmp(pTransformableObject->GetName().Left(5).Get(),"proxy")!=0)&&
      (stricmp(pTransformableObject->GetName().Left(7).Get(),"control")!=0))
#else
   if (pTransformableObject->IsBone())
#endif
   {
      ///////////////////////
      // export bone node
      ///////////////////////
      HBone *pBone = (HBone*)pTransformableObject;

      String bonename = pBone->GetName();

      TupNode tupParentNode(pParentAgg);

      int nNodeRef = m_pTupNodePool->AddNode();
      tupParentNode.AddChildNodeReference(nNodeRef);

      pNodeAgg = m_pTupNodePool->GetNode(nNodeRef);

      TupNode tupNode(pNodeAgg);

      tupNode.AddName(bonename.Get());

      int nParentRef = tupParentNode.GetNodeReference();
      tupNode.AddParentNodeReference(nParentRef);

      Matrix44 bonematrix;
      bonematrix = YXZRotate(pBone->GetRotate()->GetValue());
      bonematrix = bonematrix * YRotate(-90.0f);
      bonematrix.SetTranslation(pBone->GetStart()->GetValue()*0.01f);
      bonematrix = bonematrix.Transpose();

      TupperwareAggregate *pTransformAgg = tupNode.AddTransform();
      TupTransform tupTransform(pTransformAgg);
      tupTransform.SetTransformType(TupTransform::STATIC_MATRIX);
      TupStaticMatrix tupStaticMatrix(pTransformAgg);

      float matrix[16];
      bonematrix.GetFloatMatrix(matrix);
      tupStaticMatrix.AddMatrix(matrix);
      tupStaticMatrix.AddFlags(TupStaticMatrix::FLAGS_ROTATION_ORDER_XYZ);

      nObjectRef = m_pTupObjectPool->AddObjectAggregate();
      TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(nObjectRef);
      TupObject tupObject(pObjectAgg);
      tupObject.SetObjectType(TupObject::BONE);
      TupBone tupBone(pObjectAgg);
      tupBone.AddLength(pBone->GetLength()->GetValue()*0.01f);

      tupNode.AddFlags(0);

      tupNode.AddObjectReference(nObjectRef);

      //////////////////////////////
      // export skin modifier bone
      //////////////////////////////

      TupSkinModifier tupSkinModifier(pSkinModifierAgg);
      TupperwareAggregate *pBoneAgg = tupSkinModifier.AddBone(m_nSkinModifierBoneCount);
      TupSkinModifierBone tupSkinModifierBone(pBoneAgg);
      tupSkinModifierBone.AddName(pBone->GetName().Get());
      int nFlags = TupSkinModifierBone::ABSOLUTE_WEIGHTS;
      tupSkinModifierBone.AddFlags(nFlags);
      tupSkinModifierBone.AddNodeReference(nNodeRef);
      tupSkinModifierBone.AddBoneInitObjectTM(matrix);

      // store away modifier context info
      HVertex *pVertex;
      VertexBoneInfo *pVertexBoneInfo;
      for (POSITION pos = pBone->GetVertexHeadPosition(); pos;)
      {
         pVertex = pBone->GetNextVertex( pos );
         int nModelRelativeVertexIndex;
         if (!m_vmap.FindKey((int)pVertex,nModelRelativeVertexIndex))
            continue;

         int nIndex;
         if (!m_vertexboneinfomap.FindKey((int)pVertex,nIndex))
         {
            nIndex = m_vertexboneinfoarray.GetSize();
            m_vertexboneinfomap.AddKey((int)pVertex,nIndex);
            m_vertexboneinfoarray.Add(VertexBoneInfo());
         }
         pVertexBoneInfo = &m_vertexboneinfoarray[nIndex];
         pVertexBoneInfo->m_bones.Add(m_nSkinModifierBoneCount);
         pVertexBoneInfo->m_weights.Add(1.0f);
         pVertexBoneInfo->m_pVertex = pVertex;
         pVertexBoneInfo->m_nModelRelativeVertexIndex = nModelRelativeVertexIndex;
      }
      m_nSkinModifierBoneCount++;
   }

   int nNumChildren = pTransformableObject->GetNumTransformableObjects();
   HTransformableObject *pChildHierObject;
   for (int nChildIndex = 0; nChildIndex < nNumChildren; nChildIndex++)
   {
      pChildHierObject = pTransformableObject->GetTransformableObjectAt(nChildIndex);
      ExportBone(pChildHierObject,pNodeAgg,pSkinModifierAgg);
   }
}

void TupModelTranslator::ExportSkinModifierContext(TupperwareAggregate *pDerivedModifierAgg,HTransformableObject *pTransformableObject)
{
   TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
   TupperwareAggregate *pModifierContextAgg = tupDerivedModifier.AddModifierContext();
   TupSkinModifierContext tupSkinModifierContext(pModifierContextAgg);
   TupperwareAggregate *pVertexPoolAgg = tupSkinModifierContext.AddVertexPool();
   TupSkinModifierContextVertexPool tupSkinModifierContextVertexPool(pVertexPoolAgg);

   Matrix44 meshmatrix;
   meshmatrix = pTransformableObject->GetTransform()->GetObjectToWorldMatrix();
   float matrix[16];//mesh matrix
   meshmatrix.GetFloatMatrix(matrix);
   tupSkinModifierContext.AddBaseTM(matrix);

   int nNumVertices = m_vmap.Length();
   bool *bSkinModifierContextVertexCreated = SNEW bool[nNumVertices];
   memset(bSkinModifierContextVertexCreated,0,sizeof(bool)*nNumVertices);

   for (int i = 0; i < m_vertexboneinfoarray.GetSize(); i++)
   {
      VertexBoneInfo *pVertexBoneInfo = &m_vertexboneinfoarray[i];
      TupperwareAggregate *pVertexAgg = tupSkinModifierContextVertexPool.AddVertex(pVertexBoneInfo->m_nModelRelativeVertexIndex);
      TupSkinModifierContextVertex tupSkinModifierContextVertex(pVertexAgg);
      tupSkinModifierContextVertex.AddBones(pVertexBoneInfo->m_bones.GetData(),pVertexBoneInfo->m_bones.GetSize());
      tupSkinModifierContextVertex.AddWeights(pVertexBoneInfo->m_weights.GetData(),pVertexBoneInfo->m_weights.GetSize());
      bSkinModifierContextVertexCreated[pVertexBoneInfo->m_nModelRelativeVertexIndex] = true;
   }

   for (i = 0; i < nNumVertices; i++)
   {
      if (!bSkinModifierContextVertexCreated[i])
      {
         DisplayWarning("Found vertex not assigned to bone.\n");
         TupperwareAggregate *pVertexAgg = tupSkinModifierContextVertexPool.AddVertex(i);      
      }
   }

   delete [] bSkinModifierContextVertexCreated;
}

void TupModelTranslator::ConvertMatrix44ToTupmatrix(Matrix44 matrix,float *pMatrix)
{
   matrix.Set(0,3,matrix.Get(0,3)*0.01f);
   matrix.Set(1,3,matrix.Get(1,3)*0.01f);
   matrix.Set(2,3,matrix.Get(2,3)*0.01f);

   matrix = matrix.Transpose();// convert to column major

   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         pMatrix[(i * 4)+j] = matrix.Get(i,j);
}
