//*****************************************************************************
// Tup Exporter - Avalanche Software
//*****************************************************************************

#include "stdafx.h"
#include "cimporttup.h"
#include <SemanticLayer.h>
#include <xsi_application.h>
#include <xsi_model.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_nurbsdata.h>
#include <xsi_null.h>
#include <xsi_camera.h>
#include <xsi_light.h>
#include <xsi_ref.h>
#include <xsi_primitive.h>
#include <xsi_project.h>
#include <xsi_scene.h>
#include <xsi_ogllight.h>
#include <xsi_point.h>
#include <xsi_facet.h>
#include <xsi_polygonnode.h>
#include <xsi_cluster.h>
#include <xsi_clusterproperty.h>
#include <xsi_shader.h>
#include <xsi_math.h>
#include <xsi_decl.h>
#include <xsi_vector3.h>
#include <xsi_polygonmesh.h>
#include <SceneInfo.h>

// tupperware globals
const char *g_pExporterDate = __DATE__;
const char *g_pExporterTime = __TIME__;

using namespace XSI;

#define _SI_CALLXSI(x,y) { if (result==SI_SUCCESS)	{ result = x;if(result != SI_SUCCESS){m_app.LogMessage(y);}}}

CImportTup::CImportTup()
{
	m_sceneRoot = m_app.GetActiveSceneRoot();
}

CStatus CImportTup::ImportFile( const CString& in_FileName )
{
   CStatus st = CStatus::OK;
   
   // initialize tup pools
   m_pSceneAgg = NULL;
   m_pTupScene = NULL;
   m_pTupNodePool = NULL;
   m_pTupObjectPool = NULL;
   m_pTupMaterialPool = NULL;
   m_pTupMapPool = NULL;
   m_pTupImagePool = NULL;

   char* pslFileName = NULL;
   XSIW2A(&pslFileName, in_FileName.GetWideString());

   // read tup file
   int result = TupperwareLoadSaveManager::Read(pslFileName,&m_pSceneAgg);
   if (!result)
//      THROW_FATAL_ERROR("TupperwareLoadSaveManager::Read failed");
      return CStatus::Fail;

   // exit if scene empty
   if (!m_pSceneAgg)
//      THROW_FATAL_ERROR("SceneAgg empty");
      return CStatus::Fail;

   m_pTupScene = new TupScene(m_pSceneAgg);

   int unitflags;
   m_pTupScene->GetUnitFlags(unitflags);

   // si units = 10 cm

   if (unitflags & TupScene::LINEAR_UNIT_CENTIMETER)
      m_fUnitRatio = 0.1f;// normally 1.0f
   else if (unitflags & TupScene::LINEAR_UNIT_METER)
      m_fUnitRatio = 10.0f;//normally 100.0f
   else
//      THROW_FATAL_ERROR("Unknown unit type");
      return CStatus::Fail;

   // create tup pools
   TupperwareAggregate *pNodePoolAgg = m_pTupScene->GetNodePool();
   if (pNodePoolAgg)
      m_pTupNodePool = new TupNodePool(pNodePoolAgg);
   else
//      THROW_FATAL_ERROR("m_pTupScene->GetNodePool() failed");
      return CStatus::Fail;

   TupperwareAggregate *pObjectPoolAgg = m_pTupScene->GetObjectPool();
   if (pObjectPoolAgg)
      m_pTupObjectPool = new TupObjectPool(pObjectPoolAgg);
   else
//      THROW_FATAL_ERROR("m_pTupScene->GetObjectPool() failed");
      return CStatus::Fail;

   TupperwareAggregate *pMaterialPoolAgg = m_pTupScene->GetMaterialPool();
   if (pMaterialPoolAgg)
      m_pTupMaterialPool = new TupMaterialPool(pMaterialPoolAgg);

   TupperwareAggregate *pMapPoolAgg = m_pTupScene->GetMapPool();
   if (pMapPoolAgg)
      m_pTupMapPool = new TupMapPool(pMapPoolAgg);

   TupperwareAggregate *pImagePoolAgg = m_pTupScene->GetImagePool();
   if (pImagePoolAgg)
      m_pTupImagePool = new TupImagePool(pImagePoolAgg);

   TupperwareAggregate *pModifierPoolAgg = m_pTupScene->GetModifierPool();
   if (pModifierPoolAgg)
      m_pTupModifierPool = new TupModifierPool(pModifierPoolAgg);

//   BuildImages();

   int nRootNodeIndex;
   m_pTupScene->GetRootNodeReference(nRootNodeIndex);

   RecurseTupTree(m_pTupNodePool->GetNode(nRootNodeIndex),X3DObject(m_sceneRoot));

   // delete pools
   delete m_pSceneAgg;
   delete m_pTupScene;
   delete m_pTupNodePool;
   delete m_pTupObjectPool;
   delete m_pTupMaterialPool;
   delete m_pTupModifierPool;
   delete m_pTupMapPool;
   delete m_pTupImagePool;

	return st;
}

TupObject::ObjectTypes CImportTup::GetObjectType(TupperwareAggregate *pNodeAgg)
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

CStatus CImportTup::RecurseTupTree(TupperwareAggregate *pNodeAgg,X3DObject in_Parent)
{
   CStatus st = CStatus::OK;

   if (!pNodeAgg)
      return st;

   TupNode tupNode(pNodeAgg);
   char *name;
   tupNode.GetName(&name);

   TupObject::ObjectTypes objType = GetObjectType(pNodeAgg);

   X3DObject newObj(in_Parent);

   switch(objType)
   {
   case TupObject::POLY_MESH :
      {
 //        X3DObject newObj(in_Parent);
         st = ImportPolyMesh(pNodeAgg,in_Parent,newObj);
 //        curObj.SetObject(newObj);
         ImportTransform( pNodeAgg, newObj );
      }
      break;
   default:
      {
         //Get the name of the Mesh
         wchar_t* pwcStr=NULL;
         XSIA2W(&pwcStr, name);
         CString cstrname(pwcStr);

	      Null newNull;
         in_Parent.AddNull(cstrname,newNull);
      }
   }

   int nNumChildren = tupNode.GetNumChildNodes();
   int nChildNodeReference;
   for (int nChildNodeIndex=0; nChildNodeIndex < tupNode.GetNumChildNodes(); nChildNodeIndex++)
   {
      tupNode.GetChildNodeReference(nChildNodeIndex,nChildNodeReference);
      TupperwareAggregate *pChildNodeAgg = m_pTupNodePool->GetNode(nChildNodeReference);
      if (!pChildNodeAgg)
         continue;

      st = RecurseTupTree(pChildNodeAgg,newObj);
   }

   return st;
}


CStatus CImportTup::ImportPolyMesh(TupperwareAggregate *pNodeAgg,X3DObject in_Parent,X3DObject &x3dobject)
{
   CStatus st = CStatus::OK;

   TupNode tupNode(pNodeAgg);

   int objectRef;
   tupNode.GetObjectReference(objectRef);
   if (objectRef==-1)
      return st;

   // find base object reference
   int baseObjRef = TupObjectHelper::FindBaseObjectRef(*m_pTupObjectPool,objectRef);
   TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(baseObjRef);

   int matRef;
   tupNode.GetMaterialReference(matRef);

   TupObject tupObject(pObjectAgg);

   char *name;
   tupNode.GetName(&name);

   //Get the name of the Mesh
   wchar_t* pwcStr=NULL;
   XSIA2W(&pwcStr, name);
   CString cstrname(pwcStr);

   TupPolyMesh tupMesh(pObjectAgg);

   // do positions
   int nNumPositionVerts;
   float *pPositionVerts;
   tupMesh.GetPositionVerts(&pPositionVerts,nNumPositionVerts);

   MATH::CVector3Array cv3aVertices(nNumPositionVerts);

   for (int i=0; i < nNumPositionVerts; i++)
   {
      MATH::CVector3 position(pPositionVerts[i*3]*m_fUnitRatio,pPositionVerts[(i*3)+1]*m_fUnitRatio,pPositionVerts[(i*3)+2]*m_fUnitRatio);
      cv3aVertices[i] = position;
   }

/*
   // do vertex normals
   int numNormalVerts;
   float *pNormalVerts;
   tupMesh.GetNormalVerts(&pNormalVerts,numNormalVerts);
   pArray = (Vector *)pNormalVerts;

   for (i=0;i<numNormalVerts;i++)
   {
      Vector &dir = pArray[i];
      VertexNormalEntry *vertexnormalentry = beziersplineobject->AddVertexNormal(Snipe::Vector3(dir.x,dir.y,dir.z));
      m_vnmap.SetValue(i,(int)vertexnormalentry);
   }
*/

   int nNumFaces,nNumPositionIndicies,nNumMatIndicies;
   int *pPositionIndices;
   tupMesh.GetPositionIndices(&pPositionIndices,nNumPositionIndicies);
//   int *pVNormalIndices;
//   tupMesh.GetNormalIndices(&pVNormalIndices,nNumNormalIndicies);
   int *pFaceVertexCounts;	
   tupMesh.GetFaceVertexCounts(&pFaceVertexCounts,nNumFaces);
   int *pFaceMaterialIndicies;
   tupMesh.GetFaceMaterialIndices(&pFaceMaterialIndicies,nNumMatIndicies);

   int nNumMeshMaps = tupMesh.GetNumMeshMaps();
   CDoubleArray *pNewUVs = NULL;
   if (nNumMeshMaps>0)
      pNewUVs = new CDoubleArray[nNumMeshMaps];

   for (int nMeshMapIndex=0; nMeshMapIndex < nNumMeshMaps; nMeshMapIndex++)
   {
      TupperwareAggregate *pUVMeshMapAgg = tupMesh.GetMeshMap(nMeshMapIndex);
      TupPolyMeshMap tupPolyMeshMap(pUVMeshMapAgg);
      int nMapChannel = tupPolyMeshMap.GetMapChannel();
      if (nMapChannel <= 0)
         continue;
      int nNumMapIndicies;
      int *pMapIndices;
      tupPolyMeshMap.GetMapIndices(&pMapIndices,nNumMapIndicies);
      pNewUVs[nMapChannel-1].Resize(nNumMapIndicies);
   }

   int nCount=0;
   for (i=0; i < nNumFaces; i++)
   {
      int nNumVerts = pFaceVertexCounts[i];
      nCount += nNumVerts+1;
   }

   CLongArray claVertexIndicies(nCount);
   int nCount0=0;//new position indicies count, needs vertex count mixed in with position indicies
   int nCount1=0;
   for (int nFaceIndex=0; nFaceIndex < nNumFaces; nFaceIndex++)
   {
      int nNumVerts = pFaceVertexCounts[nFaceIndex];
      claVertexIndicies[nCount0] = nNumVerts;
      for(int j = 0; j < nNumVerts; j++)
         claVertexIndicies[nCount0+j+1] = pPositionIndices[nCount1+j];

      int nMatIndex = pFaceMaterialIndicies[nFaceIndex];
      if (nNumMatIndicies==0)
      {
         nCount0 +=nNumVerts+1;
         nCount1 +=nNumVerts;
         continue;
      }

      for (int nMeshMapIndex = 0; nMeshMapIndex < nNumMeshMaps; nMeshMapIndex++)
      {
         TupperwareAggregate *pUVMeshMapAgg = tupMesh.GetMeshMap(nMeshMapIndex);
         TupPolyMeshMap tupPolyMeshMap(pUVMeshMapAgg);

         int nMapChannel = tupPolyMeshMap.GetMapChannel();
         if (nMapChannel <= 0)
            continue;

         int nNumMapVerts;
         float *pMapVerts;
         tupPolyMeshMap.GetMapVerts(&pMapVerts,nNumMapVerts);
 
         int nNumMapIndicies;
         int *pMapIndices;
         tupPolyMeshMap.GetMapIndices(&pMapIndices,nNumMapIndicies);

         int nMapIndex;

         for (i = 0; i < nNumVerts; i++)
         {
            nMapIndex = pMapIndices[nCount1+i];
            pNewUVs[nMapChannel-1][(nCount1+i)*3] = pMapVerts[(pMapIndices[nCount1+i])*3];
            pNewUVs[nMapChannel-1][(nCount1+i)*3+1] = pMapVerts[(pMapIndices[nCount1+i])*3+1];
            pNewUVs[nMapChannel-1][(nCount1+i)*3+2] = 1.0f;

            double x = pNewUVs[nMapChannel-1][(nCount1+i)*3];
            double y = pNewUVs[nMapChannel-1][(nCount1+i)*3+1];
            double z = pNewUVs[nMapChannel-1][(nCount1+i)*3+2];
            int debug = 1;

         }

#if 0
            TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(matRef);
            TupMaterial tupMaterial(pMaterialAgg);
            TupMaterial::MaterialTypes materialtype = tupMaterial.GetMaterialType();

            if (materialtype == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
            {
               TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);

               if (tupMultiSubObjectMaterial.GetNumSubMaterials()==0)
                  continue;

               TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(matIndex);
               if (!pSubMaterialAgg)
                  continue;

               TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
               int subMaterialRef;
               tupSubMaterial.GetMaterialReference(subMaterialRef);
               pSubMaterialAgg = m_pTupMaterialPool->GetMaterial(subMaterialRef);

               TupMaterial tupMaterial2(pSubMaterialAgg);

               if (tupMaterial2.GetMaterialType() != TupMaterial::STANDARD_MATERIAL)
                  continue;

               TupStandardMaterial tupStandardMaterial(pSubMaterialAgg);

               int nummaps = tupStandardMaterial.GetNumMaps();

               int nMapIndex;
               for (nMapIndex = 0; nMapIndex < nummaps; nMapIndex++)
               {
                  TupperwareAggregate *pStandardMaterialMap = tupStandardMaterial.GetMap(nMapIndex);
                  TupStandardMaterialMap tupStandardMaterialMap(pStandardMaterialMap);

                  char *mapname;
                  tupStandardMaterialMap.GetMapName(&mapname);

                  if (strcmp(mapname,"Diffuse Color")!=0)
                     continue;

                  int mapRef;
                  tupStandardMaterialMap.GetMapReference(mapRef);

                  TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapRef);

                  TupMap tupMap(pMapAgg);

                  if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
                     BuildBitmapMap(pModel,pPoly,channelnum,pMapAgg,uvhasharray,matIndex,pMapIndices,count);
                  else if (tupMap.GetMapType() == TupMap::COMPOSITE_MAP)
                     BuildCompositeMap(pModel,pPoly,channelnum,pMapAgg,uvhasharray,matIndex,pMapIndices,count);

               }//nMapIndex loop
            }//if MultiSubObjectMaterial
            else if (materialtype == TupMaterial::STANDARD_MATERIAL)
            {
               TupStandardMaterial tupStandardMaterial(pMaterialAgg);

               int nummaps = tupStandardMaterial.GetNumMaps();
               int nMapIndex;
               for (nMapIndex = 0; nMapIndex < nummaps; nMapIndex++)
               {
                  TupperwareAggregate *pStandardMaterialMap = tupStandardMaterial.GetMap(nMapIndex);
                  TupStandardMaterialMap tupStandardMaterialMap(pStandardMaterialMap);

                  char *mapname;
                  tupStandardMaterialMap.GetMapName(&mapname);

                  if (strcmp(mapname,"Diffuse Color")!=0)
                     continue;

                  int mapRef;
                  tupStandardMaterialMap.GetMapReference(mapRef);

                  TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapRef);

                  TupMap tupMap(pMapAgg);

                  if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
                     BuildBitmapMap(pModel,pPoly,1,pMapAgg,uvhasharray,matIndex,pMapIndices,count);
                  else if (tupMap.GetMapType() == TupMap::COMPOSITE_MAP)
                     BuildCompositeMap(pModel,pPoly,1,pMapAgg,uvhasharray,matIndex,pMapIndices,count);

               }//nMapIndex loop
            }//if STANDARDMATERIAL
#endif
      }
      nCount0 +=nNumVerts+1;
      nCount1 +=nNumVerts;
   }

   st = in_Parent.AddPolygonMesh(cv3aVertices ,claVertexIndicies, cstrname, x3dobject);

   PolygonMesh newPolygonMesh( x3dobject.GetActivePrimitive().GetGeometry() );

   //Create a projection for each sets
   for ( long nUVSetIndex=0; nUVSetIndex < nNumMeshMaps; nUVSetIndex++)
      CreateProjectionCmd(x3dobject);

   //Set the UV
   CRefArray sampleClusters;
   st = newPolygonMesh.GetClusters().Filter(L"sample", CStringArray(), CString(), sampleClusters ); 
   if ( CStatus::OK != st )
      return CStatus::Fail;

   for(i = 0; i < sampleClusters.GetCount() ;i++)
   {
      ClusterProperty uvClsProp;
      st = Cluster(sampleClusters[i]).GetProperties().Find(L"uvspace", uvClsProp);
      if ( CStatus::OK != st )
         return CStatus::Fail;
      uvClsProp.GetElements().PutArray(pNewUVs[i]);
      FreezeObjCmd(uvClsProp);
   }

   if (pNewUVs)
      delete[] pNewUVs;

   return st;
}

void CImportTup::ImportTransform(TupperwareAggregate *pNodeAgg,X3DObject &x3dobject)
{
   TupNode tupNode(pNodeAgg);
   TupperwareAggregate *pTransformAgg = tupNode.GetTransform();
   TupTransform tupTransform(pTransformAgg);
   if (tupTransform.GetTransformType() != TupTransform::STATIC_MATRIX)
      return;
 
   TupStaticMatrix tupStaticMatrix(pTransformAgg);	

   MATH::CMatrix4 cmatrix4;
   ConvertTupMatrixToCMatrix4(pTransformAgg,cmatrix4);

   KinematicState KineState = x3dobject.GetKinematics().GetLocal();
   MATH::CTransformation cTransform;
   cTransform.SetMatrix4(cmatrix4);
   KineState.PutTransform(cTransform);

   return;
}

CStatus CImportTup::ConvertTupMatrixToCMatrix4(TupperwareAggregate *pTransformAgg,MATH::CMatrix4 &matrix)
{
   matrix.SetIdentity();
   TupTransform tupTransform(pTransformAgg);

   if (tupTransform.GetTransformType() != TupTransform::STATIC_MATRIX)
      return CStatus::Fail;

   TupStaticMatrix tupStaticMatrix(pTransformAgg);
   float *pMatrix;
   tupStaticMatrix.GetMatrix(&pMatrix);

   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         matrix.SetValue(i,j,pMatrix[(i * 4)+j]);

   matrix.SetValue(3,0,matrix.GetValue(3,0)*m_fUnitRatio);
   matrix.SetValue(3,1,matrix.GetValue(3,1)*m_fUnitRatio);
   matrix.SetValue(3,2,matrix.GetValue(3,2)*m_fUnitRatio);

   return CStatus::OK;
}

#if 0
void CImportTup::ImportBitmapMap(HPolyModel *pModel,
                                   HPoly *pPoly,
                                   int channelnum,
                                   TupperwareAggregate *pMapAgg,
                                   CArray< CArray<HUV*> > &uvhasharray,
                                   int matIndex,
                                   int *pMapIndices,
                                   int count)
{
   int i;
   int numVerts = pPoly->GetNumVertices();

   TupBitmapMap tupBitMap(pMapAgg);

   int mapChannel;
   tupBitMap.GetMapChannel(mapChannel);

   // did we find one that matches this meshmap?
   if (mapChannel == channelnum)
   {
      int value;
      if (m_surfacelookup.FindKey((int)pMapAgg,value))
      {
         HSurface *pSurface = (HSurface*)value;
         pSurface->AddFace(pPoly);
         if (m_decalstamplookup.FindKey(mapChannel,value))
         {
            HTexCoords *pTexCoords = (HTexCoords*)value;
            HPolyStamp *pPolyStamp = pTexCoords->AddPoly(pPoly);

            ASSERT(pPolyStamp);
            pPolyStamp->SetNumUVs(numVerts);
            for (i = 0; i < numVerts; i++)
            {
               HUV *pUV = uvhasharray[mapChannel-1][pMapIndices[count+i]];
               Vector pos = pUV->GetPos();
               pPolyStamp->SetUVAt(i,pUV);
            }
         }
      }
   }
}

void CImportTup::ImportCompositeMap(HPolyModel *pModel,
                                      HPoly *pPoly,
                                      int channelnum,
                                      TupperwareAggregate *pCompositeMapAgg,
                                      CArray< CArray<HUV*> > &uvhasharray,
                                      int matIndex,
                                      int *pMapIndices,
                                      int count)
{
   TupCompositeMap compositeMap(pCompositeMapAgg);
   int numsubmaps = compositeMap.GetNumSubMaps();
   for (int nSubMapIndex = 0; nSubMapIndex < numsubmaps;  nSubMapIndex++)
   {
      TupperwareAggregate *pSubMapAgg = compositeMap.GetSubMap(nSubMapIndex);
      TupCompositeMapSubMap tupCompositeMap(pSubMapAgg);

      int subMapRef;
      tupCompositeMap.GetMapReference(subMapRef);

      TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(subMapRef);

      TupMap tupMap(pMapAgg);

      if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
         BuildBitmapMap(pModel,pPoly,1,pMapAgg,uvhasharray,matIndex,pMapIndices,count);
   }
}
#endif

//*****************************************************************************
/*!	Methode that execute the command CreateProjection for the PolygonMesh. 
		This will create a projection so we can set the UVs into.
	\param p3DObj Object under which the projection is created. 	
 */
//*****************************************************************************
CStatus CImportTup::CreateProjectionCmd(  X3DObject p3DObj )const
{
	CValueArray args(2);  
	CValue ret;  

	// First Arg - Object
	args[0] = p3DObj;
 
	// Second Arg - Proj type
	args[1] = (short)siTxtSpatial; 

	return(m_app.ExecuteCommand( L"CreateProjection" , args,ret ));

}
//*****************************************************************************
/*!	Methode that execute the command FreezeObj used for the PolygonMesh after 
		the UVs are set.
	\param Obj The Cluster Property. 	
 */
//*****************************************************************************
CStatus CImportTup::FreezeObjCmd( ClusterProperty& Obj )const
{
	CValueArray args(1);  
	CValue	ret;

	// First Arg - Object
	args[0] = Obj ;
 
	return (m_app.ExecuteCommand(L"FreezeObj",args,ret));

}
