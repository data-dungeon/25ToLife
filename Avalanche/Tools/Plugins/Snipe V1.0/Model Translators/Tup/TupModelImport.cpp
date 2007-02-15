#include "stdafx.h"
#include "tup.h"
#include "HPolyModel.h"
#include "HPoly.h"
#include "HUV.h"
#include "HUVPoly.h"
#include "HSurface.h"
#include "HTexCoords.h"
#include "HUVMapSurfaceDriver.h"
#include "HTypeProperty.h"
#include "HColorProperty.h"
#include "HImagePointerProperty.h"
#include "HTransform2Property.h"
#include "HTexCoordsPointerProperty.h"
#include "HVector2Property.h"
#include "HBone.h"
#include "HTransformProperty.h"
#include "HTranslateProperty.h"
#include "HRotateProperty.h"
#include "HFloatProperty.h"
#include "HColorVertex.h"
#include "HColorPoly.h"
#include "HColorVertexSet.h"

#include "Vector2.h"
#include "Matrix44.h"


void TupModelTranslator::OnImport(HPolyModel *pModel, const char *szFileName)
{
   // read tup file
   int result = TupperwareLoadSaveManager::Read(szFileName,&m_pSceneAgg);
   if (!result)
      THROW_FATAL_ERROR("TupperwareLoadSaveManager::Read failed");

   // exit if scene empty
   if (!m_pSceneAgg)
      THROW_FATAL_ERROR("SceneAgg empty");

   InitTupSceneForImport(szFileName);

   RecurseTupTree(m_pWorldNodeAgg,pModel);

   DestroyTupScene();
}

void TupModelTranslator::InitTupSceneForImport(const char *szFileName)
{
   m_pTupScene = SNEW TupScene(m_pSceneAgg);

   // create tup pools
   TupperwareAggregate *pNodePoolAgg = m_pTupScene->GetNodePool();
   if (pNodePoolAgg)
      m_pTupNodePool = SNEW TupNodePool(pNodePoolAgg);
   else
      THROW_FATAL_ERROR("m_pTupScene->GetNodePool() failed");

   TupperwareAggregate *pObjectPoolAgg = m_pTupScene->GetObjectPool();
   if (pObjectPoolAgg)
      m_pTupObjectPool = SNEW TupObjectPool(pObjectPoolAgg);
   else
      THROW_FATAL_ERROR("m_pTupScene->GetObjectPool() failed");

   TupperwareAggregate *pMaterialPoolAgg = m_pTupScene->GetMaterialPool();
   if (pMaterialPoolAgg)
      m_pTupMaterialPool = SNEW TupMaterialPool(pMaterialPoolAgg);

   TupperwareAggregate *pMapPoolAgg = m_pTupScene->GetMapPool();
   if (pMapPoolAgg)
      m_pTupMapPool = SNEW TupMapPool(pMapPoolAgg);

   TupperwareAggregate *pImagePoolAgg = m_pTupScene->GetImagePool();
   if (pImagePoolAgg)
      m_pTupImagePool = SNEW TupImagePool(pImagePoolAgg);

   TupperwareAggregate *pModifierPoolAgg = m_pTupScene->GetModifierPool();
   if (pModifierPoolAgg)
      m_pTupModifierPool = SNEW TupModifierPool(pModifierPoolAgg);

   ImportImages(); 

   int nRootNodeIndex;
   m_pTupScene->GetRootNodeReference(nRootNodeIndex);
   m_pWorldNodeAgg = m_pTupNodePool->GetNode(nRootNodeIndex);
}

void TupModelTranslator::DestroyTupScene()
{
   delete m_pSceneAgg;
   delete m_pTupScene;
   delete m_pTupNodePool;
   delete m_pTupObjectPool;
   delete m_pTupMaterialPool;
   delete m_pTupMapPool;
   delete m_pTupModifierPool;
   delete m_pTupImagePool;

   m_pSceneAgg=NULL;
   m_pTupScene=NULL;
   m_pTupNodePool=NULL;
   m_pTupObjectPool=NULL;
   m_pTupMaterialPool=NULL;
   m_pTupMapPool=NULL;
   m_pTupModifierPool=NULL;
   m_pTupImagePool=NULL;
}

void TupModelTranslator::RecurseTupTree(TupperwareAggregate *pNodeAgg,HHierObject *pParent)
{
   if (!pNodeAgg)
      return;
   if (!pParent)
      return;

   TupNode tupNode(pNodeAgg);

   char *name;
   tupNode.GetName(&name);

   HHierObject *pHierObject = pParent;

   HProject *pProject = GetHMainInterface()->GetCurrentProject();

   int nObjectRef;
   tupNode.GetObjectReference(nObjectRef);
   if (nObjectRef>=0)
   {
      TupObject::ObjectTypes objType = GetTupObjectType(pNodeAgg);

      switch(objType)
      {
      case TupObject::POLY_MESH :
         {
            ImportPolyMesh(pNodeAgg,(HPolyModel*)pParent);
         }
         break;
      default :
         {

         }
      }
   }

//   ImportTransform( pNodeAgg, SNEWObj );

   int nNumChildren = tupNode.GetNumChildNodes();
   int nChildNodeReference;
   for (int nChildNodeIndex=0; nChildNodeIndex < tupNode.GetNumChildNodes(); nChildNodeIndex++)
   {
      tupNode.GetChildNodeReference(nChildNodeIndex,nChildNodeReference);
      TupperwareAggregate *pChildNodeAgg = m_pTupNodePool->GetNode(nChildNodeReference);
      if (!pChildNodeAgg)
         continue;
      RecurseTupTree(pChildNodeAgg,pHierObject);
   }

}

void TupModelTranslator::ImportPolyMesh(TupperwareAggregate *pNodeAgg,HPolyModel *pModel)
{
   m_pModel = (HHierObject*)pModel;

   m_surfacelookup.Clear();
   m_texcoordslookup.Clear();
   m_maplookup.Clear();
   m_bonelookup.Clear();
   m_vertexmap.RemoveAll();
   m_texcoordsmap.RemoveAll();

   int nMatRef;
   TupNode tupNode(pNodeAgg);
   tupNode.GetMaterialReference(nMatRef);

   TupperwareAggregate *pObjectAgg = FindObject(pNodeAgg);
   ASSERT(pObjectAgg);

   TupPolyMesh tupPolyMesh(pObjectAgg);

   m_pModelNodeAgg = pObjectAgg;

   Vector *pArray;

   // do positions
   int nNumPositionVerts;
   float *pPositionVerts;
   tupPolyMesh.GetPositionVerts(&pPositionVerts,nNumPositionVerts);
   pArray = (Vector *)pPositionVerts;

   int nNumPositionIndicies;
   int *pPositionIndices;
   tupPolyMesh.GetPositionIndices(&pPositionIndices,nNumPositionIndicies);

   // do normals
   int nNumNormalVerts;
   float *pNormalVerts;
   tupPolyMesh.GetNormalVerts(&pNormalVerts, nNumNormalVerts);

   int nNumNormalIndicies;
   int *pNormalIndices;
   tupPolyMesh.GetNormalIndices(&pNormalIndices,nNumNormalIndicies);

   int nNumFaces;
   int *pFaceVertexCounts;	
   tupPolyMesh.GetFaceVertexCounts(&pFaceVertexCounts,nNumFaces);

   int nNumMatIndicies;
   int *pFaceMaterialIndicies;
   tupPolyMesh.GetFaceMaterialIndices(&pFaceMaterialIndicies,nNumMatIndicies);

   int i;

   CArray<HVertex*> vertexpool;
   vertexpool.SetSize(nNumPositionVerts);
   m_vertexmap.InitHashTable(nNumPositionVerts);
   for (i = 0; i < nNumPositionVerts; i++)
   {
      Vector pos(pPositionVerts[i*3],pPositionVerts[i*3+1],pPositionVerts[i*3+2]);
      HVertex* pVertex = pModel->AddVertex(pos * 100.0f);// convert meters to centimeters
      UINT nID = pVertex->GetID();
      m_vertexmap.SetAt(i,pVertex);
      vertexpool[i] = pVertex;
   }

   CArray<Vector> vertexnormals;
   vertexnormals.SetSize(nNumPositionIndicies);

   CArray<UINT> faceindicies;
   faceindicies.SetSize(nNumFaces);

   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   ImportMaterials(pModel,nMatRef);

   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
   HColorVertexSet *pColorVertexSet = NULL;
   TupperwareAggregate *pModifierContextAgg = NULL;

   int nObjectRef;
   tupNode.GetObjectReference(nObjectRef);
   TupperwareAggregate *pVertexColorModifierAgg = FindVertexColorModifier(m_pTupObjectPool->GetObjectAggregate(nObjectRef),&pModifierContextAgg);
   if (pVertexColorModifierAgg)
      pColorVertexSet = pModel->AddColorVertexSet();
#endif
   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   int nCount = 0;
   int nFaceIndex;
   for (nFaceIndex = 0; nFaceIndex < nNumFaces; nFaceIndex++)
   {
      int nNumVerts = pFaceVertexCounts[nFaceIndex];
      HPoly *pPoly = pModel->AddPoly();
      pPoly->SetNumSides(nNumVerts);
      for (i = 0; i < nNumVerts; i++)
      {
         pPoly->SetVertexAt(i, vertexpool[pPositionIndices[nCount+i]]);
         vertexnormals[nCount+i] = pNormalVerts[pNormalIndices[nCount+i]];
      }

      int nMatIndex = pFaceMaterialIndicies[nFaceIndex];
      if (nMatIndex<0)
      {
         nCount += nNumVerts;
         continue;
      }

      TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(nMatRef);

      AddFaceToMutiSubObjectMaterial(pPoly,pMaterialAgg,nCount,nMatIndex);

#if 0
      //////////////////////////////////////////////////////////////////////////////////
      // add vertex colors

      if (pVertexColorModifierAgg)
      {
         TupApplyVCModifier tupApplyVCModifier(pVertexColorModifierAgg);

         int nNumMapIndicies;
         int *pMapIndices;
         tupApplyVCModifier.GetColorIndices(&pMapIndices,nNumMapIndicies);

         int nNumMapVerts;
         float *pMapVerts;
         tupApplyVCModifier.GetColorVerts(&pMapVerts,nNumMapVerts);

         HColorPoly *pColorPoly = pColorVertexSet->AddPoly(pPoly);
         ASSERT(pColorPoly);

         HColorVertex *pColorVertex;
         int nNumVerts = pPoly->GetNumSides();
         for (int i = 0; i < nNumVerts; i++)
         {
            int nIndex = pMapIndices[nCount+i];
            RGBAFloat rgbColor(pMapVerts[nIndex*3],pMapVerts[nIndex*3+1],pMapVerts[nIndex*3+2],1.0f);
            pColorVertex = pColorVertexSet->AddColorVertex(rgbColor);
            pColorPoly->SetColorVertexAt(i,pColorVertex);
         }
      }

      //////////////////////////////////////////////////////////////////////////////////
#endif
      nCount += nNumVerts;

      faceindicies[nFaceIndex] = pPoly->GetID();
   }

   for (POSITION pos = m_texcoordsmap.GetStartPosition(); pos;)
   {
      MaterialUVSetKey key;
      MaterialUVSetValue *value;
      m_texcoordsmap.GetNextAssoc(pos,key,value);
      delete value;
   }

   pModel->SetVertexNormals(faceindicies,vertexnormals);
   
   ImportSkeleton(pModel,pNodeAgg);
}

void TupModelTranslator::ImportImages()
{
   HProject *hp = GetHMainInterface()->GetCurrentProject();

   m_imagelookup.Clear();

   if (m_pTupImagePool)
   {
      int nNumImages = m_pTupImagePool->GetNumImages();
      for (int i = 0; i < nNumImages; i++)
      {
         TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(i);
         TupImage tupImage(pImageAgg);
         char *filepathname;
         tupImage.GetFilePathName(&filepathname);  
         HImage *pImage = hp->AddImage(filepathname);
         m_imagelookup.AddKey(i,(int)pImage);
      }
   }
}

void TupModelTranslator::ImportMaterials(HPolyModel *pModel,int nMatRef)
{ 
   TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(nMatRef);
   TupMaterial tupMaterial(pMaterialAgg);
   TupMaterial::MaterialTypes materialtype = tupMaterial.GetMaterialType();

   if (materialtype == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
      ImportMultiSubObjectMaterial(pMaterialAgg,pModel);
   else if (materialtype == TupMaterial::STANDARD_MATERIAL)
      ImportMaterial(pMaterialAgg,pModel);
}

void TupModelTranslator::ImportMultiSubObjectMaterial(TupperwareAggregate *pMaterialAgg,HPolyModel *pModel)
{
   TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
   int nNumSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();

   for (int i=0; i < nNumSubMaterials; i++)
   {
      TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(i);
      TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
      int nMatRef;
      tupSubMaterial.GetMaterialReference(nMatRef);
      TupperwareAggregate *pStandardMatrialAgg = m_pTupMaterialPool->GetMaterial(nMatRef);
      ImportMaterial(pStandardMatrialAgg,pModel);
   }
}

void TupModelTranslator::ImportMaterial(TupperwareAggregate *pMaterialAgg,HPolyModel *pModel)
{
   TupStandardMaterial tupStandardMaterial(pMaterialAgg);

   char *name;
   tupStandardMaterial.GetName(&name);

   HSurface *pSurface = pModel->AddSurface(name);
   m_surfacelookup.AddKey((int)pMaterialAgg,(int)pSurface);

   float *pColor;
   tupStandardMaterial.GetDiffuseColor(&pColor);
   //pSurface->GetDiffuseProperty()->StoreValue(RGBFloat(pColor[0],pColor[1],pColor[2]));

   RGBFloat color = pSurface->GetDiffuseProperty()->GetValue();

   int nNumMaps = tupStandardMaterial.GetNumMaps();
   for (int i = 0; i < nNumMaps; i++)
   {
      TupperwareAggregate *pStandardMaterialMap = tupStandardMaterial.GetMap(i);
      ImportMap(pMaterialAgg,pStandardMaterialMap,pSurface);
   }
}

void TupModelTranslator::ImportMap(TupperwareAggregate *pMaterialAgg,TupperwareAggregate *pStandardMaterialMapAgg,HSurface *pSurface)
{
   TupStandardMaterialMap tupStandardMaterialMap(pStandardMaterialMapAgg);

   char *mapname;
   tupStandardMaterialMap.GetMapName(&mapname);

   if (strcmp(mapname,"Diffuse Color")!=0)
      return;// only handles color maps right now

   int nMapRef;
   tupStandardMaterialMap.GetMapReference(nMapRef);
   TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(nMapRef);
   TupMap tupMap(pMapAgg);

   if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
      ImportBitmap(pMaterialAgg,pMapAgg,pSurface);

   TupCompositeMap compositeMap(pMapAgg);
   int nNumSubMaps = compositeMap.GetNumSubMaps();
   for (int nSubMapIndex = 0; nSubMapIndex < nNumSubMaps;  nSubMapIndex++)
   {
      TupperwareAggregate *pSubMapAgg = compositeMap.GetSubMap(nSubMapIndex);
      TupCompositeMapSubMap tupCompositeSubMap(pSubMapAgg);

      int nSubMapRef;
      tupCompositeSubMap.GetMapReference(nSubMapRef);

      TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(nSubMapRef);

      TupMap tupMap(pMapAgg);

      if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
         ImportBitmap(pMaterialAgg,pMapAgg,pSurface);
   }
}

void TupModelTranslator::ImportBitmap(TupperwareAggregate *pMaterialAgg,TupperwareAggregate *pMapAgg,HSurface *pSurface)
{
   TupBitmapMap tupBitMap(pMapAgg);

   int nMapChannel;
   tupBitMap.GetMapChannel(nMapChannel);
   nMapChannel--;

   int nImageRef;
   tupBitMap.GetImageReference(nImageRef);

   int nValue;
   HImage *pImage = NULL;
   if (m_imagelookup.FindKey(nImageRef,nValue))
      pImage = (HImage*)nValue;

   if (!pImage)
      return;

   HUVMapSurfaceDriver *pUVMapSurfaceDriver = (HUVMapSurfaceDriver *)GetHMainInterface()->CreateObject( SOT_UVMapSurfaceDriver );
   ASSERT(pUVMapSurfaceDriver);

   pSurface->AddSurfaceDriver( pUVMapSurfaceDriver, pSurface->GetDiffuseProperty() );

   HImagePointerProperty *pImagePointerProperty = pUVMapSurfaceDriver->GetImage();
   pImagePointerProperty->StoreValue( pImage, FALSE );

   int nFlags;
   tupBitMap.GetFlags(nFlags);

   bool utile = false, vtile = false;
   if (nFlags & TupBitmapMap::TUP_U_WRAP)
      utile = true;
   if (nFlags & TupBitmapMap::TUP_V_WRAP)
      vtile = true;

   //pImagePointerProperty->GetTileU()->StoreValue(utile);
   //pImagePointerProperty->GetTileV()->StoreValue(vtile);

   float fScaleX,fScaleY;
   tupBitMap.GetUTiling(fScaleX);
   tupBitMap.GetVTiling(fScaleY);

   pUVMapSurfaceDriver->GetImage()->GetTransform()->GetScale()->StoreValue(Vector2(fScaleX,fScaleY));

   int nBlendMode;
   tupBitMap.GetBlendMode(nBlendMode);
   if (nBlendMode == TupBitmapMap::BLEND_MODE_MULTIPLY)
      pUVMapSurfaceDriver->GetBlendStyle()->StoreValue(HMapSurfaceDriver::MULTIPLY);
   else if (nBlendMode == TupBitmapMap::BLEND_MODE_OVER)
      pUVMapSurfaceDriver->GetBlendStyle()->StoreValue(HMapSurfaceDriver::REPLACE);
   else if (nBlendMode == TupBitmapMap::BLEND_MODE_ADD)
      pUVMapSurfaceDriver->GetBlendStyle()->StoreValue(HMapSurfaceDriver::ADDITIVE);

   // break up uvsets, so that each surface has its own uvset
   HPolyModel *pPolyModel = (HPolyModel*)m_pModel;
   HTexCoords *pTexCoords;
   MaterialUVSetKey key(pMaterialAgg,nMapChannel);
   MaterialUVSetValue *value;
   if (!m_texcoordsmap.Lookup(key,value))
   {
      TupPolyMesh tupPolyMesh(m_pModelNodeAgg);
      TupperwareAggregate *pUVMeshMapAgg = tupPolyMesh.GetMeshMap(nMapChannel);
      TupPolyMeshMap tupPolyMeshMap(pUVMeshMapAgg);
      String texcoordsname = "UVSet";
      texcoordsname += String(nMapChannel);
      pTexCoords = pPolyModel->AddTexCoords(texcoordsname.Get());
      value = SNEW MaterialUVSetValue(pTexCoords);
      m_texcoordsmap.SetAt(key,value);
   }

   pUVMapSurfaceDriver->GetTexCoords()->StoreValue( pTexCoords, FALSE );
}

void TupModelTranslator::AddFaceToMutiSubObjectMaterial(HPoly *pPoly,TupperwareAggregate *pMaterialAgg,int nCount,int nSubMatIndex)
{
   TupMaterial tupMaterial(pMaterialAgg);
   TupMaterial::MaterialTypes materialtype = tupMaterial.GetMaterialType();

   if (materialtype == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
   {
      TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
      TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(nSubMatIndex);
      TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
      int nMatRef;
      tupSubMaterial.GetMaterialReference(nMatRef);
      TupperwareAggregate *pStandardMatrialAgg = m_pTupMaterialPool->GetMaterial(nMatRef);
      AddFaceToMaterial(pPoly,pStandardMatrialAgg,nCount);
   }
   else if (materialtype == TupMaterial::STANDARD_MATERIAL)
   {
      AddFaceToMaterial(pPoly,pMaterialAgg,nCount);
   }
}

void TupModelTranslator::AddFaceToMaterial(HPoly *pPoly,TupperwareAggregate *pMaterialAgg,int nCount)
{
   int nValue;
   if (!m_surfacelookup.FindKey((int)pMaterialAgg,(int)nValue))
      return;

   HSurface *pSurface = (HSurface*)nValue;
   pSurface->AddFace(pPoly);

   TupStandardMaterial tupStandardMaterial(pMaterialAgg);

   int nNumMaps = tupStandardMaterial.GetNumMaps();
   int nMapIndex;
   for (nMapIndex = 0; nMapIndex < nNumMaps; nMapIndex++)
   {
      TupperwareAggregate *pStandardMaterialMap = tupStandardMaterial.GetMap(nMapIndex);
      TupStandardMaterialMap tupStandardMaterialMap(pStandardMaterialMap);

      char *mapname;
      tupStandardMaterialMap.GetMapName(&mapname);

      if (strcmp(mapname,"Diffuse Color")!=0)
         continue;// only handles color maps right now

      int nMapRef;
      tupStandardMaterialMap.GetMapReference(nMapRef);

      TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(nMapRef);

      AddFaceToMap(pPoly,pMaterialAgg,pMapAgg,nCount);
   }
}

void TupModelTranslator::AddFaceToMap(HPoly *pPoly,TupperwareAggregate *pMaterialAgg,TupperwareAggregate *pMapAgg,int nCount)
{
   TupMap tupMap(pMapAgg);

   if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
      AddFaceToTexCoords(pPoly,pMaterialAgg,pMapAgg,nCount);

   if (tupMap.GetMapType() != TupMap::COMPOSITE_MAP)
      return;

   TupCompositeMap compositeMap(pMapAgg);
   int nNumSubMaps = compositeMap.GetNumSubMaps();
   for (int nSubMapIndex = 0; nSubMapIndex < nNumSubMaps;  nSubMapIndex++)
   {
      TupperwareAggregate *pSubMapAgg = compositeMap.GetSubMap(nSubMapIndex);
      TupCompositeMapSubMap tupCompositeMap(pSubMapAgg);

      int nMapRef;
      tupCompositeMap.GetMapReference(nMapRef);

      TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(nMapRef);

      TupMap tupSubMaterialMap(pMapAgg);

      if (tupSubMaterialMap.GetMapType() == TupMap::BITMAP_MAP)
         AddFaceToTexCoords(pPoly,pMaterialAgg,pMapAgg,nCount);
   }
}

void TupModelTranslator::AddFaceToTexCoords(HPoly *pPoly,TupperwareAggregate *pMaterialAgg,TupperwareAggregate *pMapAgg,int nCount)
{
   TupPolyMesh tupPolyMesh(m_pModelNodeAgg);

   TupBitmapMap tupBitmapMap(pMapAgg);

   int nMapChannel;
   tupBitmapMap.GetMapChannel(nMapChannel);
   if (nMapChannel<=0)
      return;

   nMapChannel--;//map channel is always +1, since 0 is for vertex colors

   TupperwareAggregate *pUVMeshMapAgg = tupPolyMesh.GetMeshMap(nMapChannel);
   TupPolyMeshMap tupPolyMeshMap(pUVMeshMapAgg);

   int nNumMapIndicies;
   int *pMapIndices;
   tupPolyMeshMap.GetMapIndices(&pMapIndices,nNumMapIndicies);

   // add uvs and assign uvpolys to uvset
   HTexCoords *pTexCoords;
   MaterialUVSetKey key(pMaterialAgg,nMapChannel);
   MaterialUVSetValue *value;
   if (!m_texcoordsmap.Lookup(key,value))
      return;

   pTexCoords = value->m_pTexCoords;

   int nNumMapVerts;
   float *pMapVerts;
   tupPolyMeshMap.GetMapVerts(&pMapVerts,nNumMapVerts);

   HUVPoly *pUVPoly = pTexCoords->AddPoly(pPoly);
   ASSERT(pUVPoly);

   HUV *pUV;
   int nNumVerts = pPoly->GetNumSides();
   for (int i = 0; i < nNumVerts; i++)
   {
      int nIndex = pMapIndices[nCount+i];
      if (!value->m_uvmap.Lookup(nIndex,pUV))
      {
         pUV = pTexCoords->AddUV(Vector2(pMapVerts[nIndex*3],pMapVerts[nIndex*3+1]));
         value->m_uvmap.SetAt(nIndex,pUV);
      }
      pUVPoly->SetUVAt(i,pUV);
   }
}

TupObject::ObjectTypes TupModelTranslator::GetTupObjectType(TupperwareAggregate *pNodeAgg)
{
   TupNode tupNode(pNodeAgg);

   int objectRef;
   tupNode.GetObjectReference(objectRef);
   if (objectRef==-1)
      return TupObject::UNKNOWN;

   // find base object reference
   int baseObjRef = TupObjectHelper::FindBaseObjectRef(*m_pTupObjectPool,objectRef);
   TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(baseObjRef);

   TupObject tupObject(pObjectAgg);

   return tupObject.GetObjectType();
}

TupperwareAggregate *TupModelTranslator::FindObject(TupperwareAggregate *pNodeAgg)
{
   TupNode tupNode(pNodeAgg);

   int nObjectRef;
   tupNode.GetObjectReference(nObjectRef);
   if (nObjectRef==-1)
      return NULL;

   // find base object reference
   int nBaseObjectRef = TupObjectHelper::FindBaseObjectRef(*m_pTupObjectPool,nObjectRef);
   TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(nBaseObjectRef);

   return pObjectAgg;
}

TupperwareAggregate *TupModelTranslator::FindSkinModifier(TupperwareAggregate *pObjectAgg,TupperwareAggregate **pModifierContextAgg)
{
   TupObject tupObject(pObjectAgg);

   TupDerived tupDerived(pObjectAgg);

   if (tupObject.GetObjectType() != TupObject::DERIVED)
      return NULL;

   int nModifierRef;
   int nNumModifiers = tupDerived.GetNumModifierSlots();
   for (int i = 0; i < nNumModifiers; i++)
   {
      TupperwareAggregate *pDerivedModifierAgg  = tupDerived.GetModifierBySlot(i);
      if (pDerivedModifierAgg)
      {
         TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
         tupDerivedModifier.GetModifierReference(nModifierRef);
         TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(nModifierRef);
         TupModifier tupModifier(pModifierAgg);
         if (tupModifier.GetModifierType() == TupModifier::SKIN_MODIFIER)
         {
            *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
            return pModifierAgg;
         }
      }
   }

   int nChildObjectRef;
   tupDerived.GetObjectReference(nChildObjectRef);
   if (nChildObjectRef<0)
   {
      pModifierContextAgg = NULL;
      return NULL;
   }

   TupperwareAggregate *pChildObjectAgg = m_pTupObjectPool->GetObjectAggregate(nChildObjectRef);

   return FindSkinModifier(pChildObjectAgg,pModifierContextAgg);
}

TupperwareAggregate *TupModelTranslator::FindVertexColorModifier(TupperwareAggregate *pObjectAgg,TupperwareAggregate **pModifierContextAgg)
{
   TupObject tupObject(pObjectAgg);

   TupDerived tupDerived(pObjectAgg);

   if (tupObject.GetObjectType() != TupObject::DERIVED)
      return NULL;

   int nModifierRef;
   int nNumModifiers = tupDerived.GetNumModifierSlots();
   for (int i = 0; i < nNumModifiers; i++)
   {
      TupperwareAggregate *pDerivedModifierAgg  = tupDerived.GetModifierBySlot(i);
      if (pDerivedModifierAgg)
      {
         TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
         tupDerivedModifier.GetModifierReference(nModifierRef);
         TupperwareAggregate *pModifierAgg = m_pTupModifierPool->GetModifier(nModifierRef);
         TupModifier tupModifier(pModifierAgg);
         if (tupModifier.GetModifierType() == TupModifier::APPLY_VC_MODIFIER)
         {
            *pModifierContextAgg = tupDerivedModifier.GetModifierContext();
            return pModifierAgg;
         }
      }
   }

   int nChildObjectRef;
   tupDerived.GetObjectReference(nChildObjectRef);
   if (nChildObjectRef<0)
   {
      pModifierContextAgg = NULL;
      return NULL;
   }

   TupperwareAggregate *pChildObjectAgg = m_pTupObjectPool->GetObjectAggregate(nChildObjectRef);

   return FindVertexColorModifier(pChildObjectAgg,pModifierContextAgg);
}

TupperwareAggregate *TupModelTranslator::FindRootBone(TupperwareAggregate *pNodeAgg)
{
   TupNode tupNode(pNodeAgg);

   int nParentNodeRef;
   tupNode.GetParentNodeReference(nParentNodeRef);

   TupperwareAggregate *pParentNodeAgg = m_pTupNodePool->GetNode(nParentNodeRef);
   TupNode tupParentNode(pParentNodeAgg);

   int nObjectRef;
   tupParentNode.GetObjectReference(nObjectRef);
   if (nObjectRef<0)
      return pNodeAgg;

   return FindRootBone(pParentNodeAgg);	
}

void TupModelTranslator::ImportSkeleton(HPolyModel *pModel,TupperwareAggregate *pNodeAgg)
{
   TupNode tupNode(pNodeAgg);
   int nObjectRef;
   tupNode.GetObjectReference(nObjectRef);
   TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(nObjectRef);

   TupperwareAggregate *pModifierContextAgg = NULL;
   TupperwareAggregate *pModifierAgg = FindSkinModifier(pObjectAgg,&pModifierContextAgg);
   if (!pModifierAgg)
      return;

   TupModifier tupModifier(pModifierAgg);
   if (tupModifier.GetModifierType() != TupModifier::SKIN_MODIFIER)
      return;

   TupSkinModifier tupSkinModifier(pModifierAgg);

// import bones associated with mesh
// if SkinModifier is empty return
   int nNumBones = tupSkinModifier.GetNumBones();
   if (nNumBones==0)
      return;

// find bone belonging to SkinModifier
   TupperwareAggregate *pSkinModifierBoneAgg = tupSkinModifier.GetBone(0);
   TupSkinModifierBone tupSkinModifierBone(pSkinModifierBoneAgg);

// get NodeRef to bone
   int nBoneNodeRef;
   tupSkinModifierBone.GetNodeReference(nBoneNodeRef);

// walk up parents to root bone
   TupperwareAggregate *pModifierNodeAgg = m_pTupNodePool->GetNode(nBoneNodeRef);
   TupperwareAggregate *pRootBoneAgg = FindRootBone(pModifierNodeAgg);

   // skip root bone (i.e. model bone)
   TupNode tupRootBoneNode(pRootBoneAgg);
   int nChildNodeRef;
   tupRootBoneNode.GetChildNodeReference(0,nChildNodeRef);

   TupperwareAggregate *pChildNodeAgg = m_pTupNodePool->GetNode(nChildNodeRef);

   ImportBone(pModel,(HBone*)pModel,pChildNodeAgg,NULL,pModifierAgg,pModifierContextAgg);
}

TupperwareAggregate *TupModelTranslator::FindSkinModifierBone(TupperwareAggregate *pNodeAgg,TupperwareAggregate *pModifierAgg)
{
   TupSkinModifier tupSkinModifier(pModifierAgg);
   int nNumBones = tupSkinModifier.GetNumBones();
   if (nNumBones==0)
      return NULL;

   TupNode tupNode(pNodeAgg);

   int nNodeRef = tupNode.GetNodeReference();
   int nNodeRef2;
   for (int i = 0; i < nNumBones; i++)
   {
      TupperwareAggregate *pBoneAgg = tupSkinModifier.GetBone(i);

      TupSkinModifierBone tupSkinModifierBone(pBoneAgg);
      tupSkinModifierBone.GetNodeReference(nNodeRef2);
      if (nNodeRef == nNodeRef2)
         return pBoneAgg;
   }
   return NULL;
}

void TupModelTranslator::ImportBone(HPolyModel *pModel,
                               HBone *pParentBone,
                               TupperwareAggregate *pNodeAgg,
                               TupperwareAggregate *pParentNodeAgg,
                               TupperwareAggregate *pModifierAgg,
                               TupperwareAggregate *pModifierContextAgg)
{
   TupSkinModifier tupSkinModifier(pModifierAgg);

   TupNode tupNode(pNodeAgg);

   TupperwareAggregate *pObjectAgg = FindObject(pNodeAgg);

   TupObject tupObject(pObjectAgg);

   if (tupObject.GetObjectType() != TupObject::BONE)
      return;

   char *bonename;
   tupNode.GetName(&bonename);

   HBone *pBone = (HBone *)GetHMainInterface()->CreateObject(SOT_Bone);
   ASSERT(pBone);

   pModel->AddBone( pBone, pParentBone );
   pBone->SetName(String(bonename));

   TupBone tupBone(pObjectAgg);

   float bonelength;
   tupBone.GetLength(bonelength);
   bonelength *= 100.0f;// convert meters to centimeters

// Set Transform
   TupperwareAggregate *pSkinModifierBoneAgg = FindSkinModifierBone(pNodeAgg,pModifierAgg);
   TupSkinModifierBone tupSkinModifierBone(pSkinModifierBoneAgg);
   float *pMatrix;
   tupSkinModifierBone.GetBoneInitObjectTM(&pMatrix);// get bind pose matrix
   Matrix44 matrix;
   ConvertTupMatrixToMatrix44(pMatrix,matrix);
   Vector translation;
   matrix.GetTranslation(translation);
   RotateEuler euler(matrix * YRotate(90.0f));
   pBone->GetStart()->StoreValue(translation,FALSE);
   pBone->GetRotate()->StoreValue(Vector(euler.x, euler.y, euler.z),FALSE);
   pBone->GetLength()->StoreValue(bonelength,FALSE);

   TupSkinModifierContext tupSkinModifierContext(pModifierContextAgg);
   TupperwareAggregate *pVertexPoolAgg = tupSkinModifierContext.GetVertexPool();
   TupSkinModifierContextVertexPool tupSkinModifierContextVertexPool(pVertexPoolAgg);

   int nNodeRef = tupNode.GetNodeReference();

   int nNumVertices = tupSkinModifierContextVertexPool.GetNumVertices();
   for (int i = 0; i < nNumVertices; i++)
   {
      TupperwareAggregate *pVertexAgg = tupSkinModifierContextVertexPool.GetVertex(i);
      if (!pVertexAgg)
         continue;

      TupSkinModifierContextVertex tupSkinModifierContextVertex(pVertexAgg);

      int *pBones;
      int nNumBones;
      tupSkinModifierContextVertex.GetBones(&pBones,nNumBones);

      for (int j = 0; j < nNumBones; j++)
      {
         TupperwareAggregate *pBoneAgg = tupSkinModifier.GetBone(pBones[j]);
         TupSkinModifierBone tupSkinModifierBone(pBoneAgg);

         int boneNodeRef;
         tupSkinModifierBone.GetNodeReference(boneNodeRef);

         if (boneNodeRef != nNodeRef)
            continue;

         TupperwareAggregate *pBoneNodeAgg = m_pTupNodePool->GetNode(boneNodeRef);

         TupNode tupBoneNode(pBoneNodeAgg);

         char *bonename;
         tupBoneNode.GetName(&bonename);

         HVertex *pVertex = NULL;
         if (!m_vertexmap.Lookup(i,pVertex))
         {
            ASSERT(FALSE);
            continue;
         }
         pBone->AddVertex(pVertex);
      }
   }

   int nNumChildren = tupNode.GetNumChildNodes();
   for (int i = 0; i < nNumChildren; i++)
   {
      int nChildNodeRef;
      tupNode.GetChildNodeReference(i,nChildNodeRef);
      TupperwareAggregate *pChildAgg = m_pTupNodePool->GetNode(nChildNodeRef);
      ImportBone(pModel,pBone,pChildAgg,pNodeAgg,pModifierAgg,pModifierContextAgg); 
   }
}

void TupModelTranslator::ConvertTupMatrixToMatrix44(float *pMatrix,Matrix44 &matrix)
{
   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         matrix.Set(i,j,pMatrix[(i * 4)+j]);

   matrix.Set(3,0,matrix.Get(3,0)*100.0f);// convert meters to centimeters
   matrix.Set(3,1,matrix.Get(3,1)*100.0f);
   matrix.Set(3,2,matrix.Get(3,2)*100.0f);

   matrix = matrix.Transpose();// convert to row major
}
