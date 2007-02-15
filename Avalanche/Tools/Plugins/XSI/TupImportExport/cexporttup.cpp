//*****************************************************************************
// Tup Exporter - Avalanche Software
//*****************************************************************************

#include "stdafx.h"

#include "cexporttup.h"
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
#include <xsi_longarray.h>

using namespace XSI;

extern XSI::CStatus CValueTOSIVariant(XSI::CValue& in_value, SI_TinyVariant* io_slVariant);

//*****************************************************************************
/*!	Default Constructor 	
 */
//*****************************************************************************
CExportTup::CExportTup()
{
	m_sceneRoot = m_app.GetActiveSceneRoot();
}

CStatus CExportTup::ExportFile
(
	const CString& in_FileName,
	const CString& in_ObjectToExport
)
{
   char* pslFileName = NULL;
   XSIW2A(&pslFileName, in_FileName.GetWideString());

   m_pSceneAgg = NULL;
   m_pTupScene = NULL;
   m_pTupNodePool = NULL;
   m_pTupObjectPool = NULL;
   m_pTupMapPool = NULL;
   m_pTupImagePool=  NULL;
   m_pTupMaterialPool = NULL;
   m_pTupModifierPool = NULL;

   // create a new scene
   m_pSceneAgg = new TupperwareAggregate;
   m_pTupScene = new TupScene(m_pSceneAgg);

   // create the node pool
   TupperwareAggregate *pNodePoolAgg = m_pTupScene->AddNodePool();
   m_pTupNodePool = new TupNodePool(pNodePoolAgg);

   // create the object pool
   TupperwareAggregate *pObjectPoolAgg = m_pTupScene->AddObjectPool();
   m_pTupObjectPool = new TupObjectPool(pObjectPoolAgg);

   // create the material pool
   TupperwareAggregate *pMaterialPoolAgg = m_pTupScene->AddMaterialPool();
   if (pMaterialPoolAgg)
      m_pTupMaterialPool = new TupMaterialPool(pMaterialPoolAgg);

   // create the map pool
   TupperwareAggregate *pMapPoolAgg = m_pTupScene->AddMapPool();
   if (pMapPoolAgg)
      m_pTupMapPool = new TupMapPool(pMapPoolAgg);

   // create the modifier pool
   TupperwareAggregate *pModifierPoolAgg = m_pTupScene->AddModifierPool();
   if (pModifierPoolAgg)
      m_pTupModifierPool = new TupModifierPool(pModifierPoolAgg);

   // create the image pool
   TupperwareAggregate *pImagePoolAgg = m_pTupScene->AddImagePool();
   if (pImagePoolAgg)
      m_pTupImagePool = new TupImagePool(pImagePoolAgg);

   // add in the date, time and version
   m_pTupScene->AddExporterDate(g_pExporterDate);
   m_pTupScene->AddExporterTime(g_pExporterTime);
   m_pTupScene->AddVersion(TupScene::GetSceneVersion());
   m_pTupScene->AddUnitFlags(TupScene::TIME_UNIT_SECONDS | TupScene::LINEAR_UNIT_METER);

   // si units = 10 cm
   m_fUnitRatio = 10.0f;

   TupString sourceFileName = pslFileName;
   m_pTupScene->AddSourceFileName(sourceFileName);

   int m_nRootNodeRef = 0;
   TupperwareAggregate *pTupRootNodeAgg = m_pTupNodePool->AddNode(m_nRootNodeRef);
   m_pTupScene->AddRootNodeReference(m_nRootNodeRef);

   TupNode tupRootNode(pTupRootNodeAgg);
   tupRootNode.AddName("world");
   tupRootNode.AddParentNodeReference(-1); // root has -1 as its parent reference

   if ( in_ObjectToExport.IsEmpty() )
   {
      // export the entire scene
      X3DObject x3dobject(m_sceneRoot);
      RecurseXSITree(m_nRootNodeRef,x3dobject);
   }
   else
   {
      X3DObject child = X3DObject(m_sceneRoot).FindChild( 
         in_ObjectToExport, 
         CString(), 
         CStringArray(), 
         true ) ;

      if ( !child.IsValid() )
      {
         return CStatus::InvalidArgument;
      }
      RecurseXSITree(m_nRootNodeRef,child);
   }

   // save the tree
   TupperwareLoadSaveManager::FormatType formatType = TupperwareLoadSaveManager::ASCII;
   int result = TupperwareLoadSaveManager::Write(formatType,pslFileName,m_pSceneAgg);

   delete m_pSceneAgg;
   delete m_pTupScene;
   delete m_pTupNodePool;
   delete m_pTupObjectPool;
   delete m_pTupMaterialPool;
   delete m_pTupMapPool;
   delete m_pTupImagePool;

   if (!result)
      return CStatus::Fail;
 
	return CStatus::OK;
}

CStatus CExportTup::RecurseXSITree(int nParentNodeIndex, XSI::X3DObject &x3dobject)
{
   CStatus st = CStatus::OK;

   if ( !x3dobject.IsValid())
      return CStatus::InvalidArgument;

   TupperwareAggregate *pParentNodeAgg = m_pTupNodePool->GetNode(nParentNodeIndex);
   TupNode tupParentNode(pParentNodeAgg);

   int nNodeIndex = m_pTupNodePool->AddNode();
   TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nNodeIndex);
   TupNode tupNode(pNodeAgg);

   char *name = NULL;
   XSIW2A(&name, x3dobject.GetName().GetWideString());

   tupNode.AddName(name);
   tupNode.AddParentNodeReference(nParentNodeIndex);

   tupParentNode.AddChildNodeReference(nNodeIndex);

   tupNode.AddFlags(0);

   ExportTransform(pNodeAgg,x3dobject);

   if ( x3dobject.IsA( siCameraID ) )
   {
   }
   else if ( x3dobject.IsA( siLightID ) )
   {				
   }
   else if ( x3dobject.IsA( siNullID ) )
   {				
   }
   else if ( x3dobject.IsA( siModelID ) )
   {
   }
   else if ( x3dobject.IsA( siX3DObjectID ) )
   {
      // Default case, it may or may not have gemetry
      st = ExportGeometry(pNodeAgg, x3dobject);		
   }
   else
   {
      ExportNull(pNodeAgg,x3dobject);
   }

   //Export the children
   CRefArray refChildrenArray(x3dobject.GetChildren());

   for(long i = 0; i < refChildrenArray.GetCount(); i++)
   {
      X3DObject x3dobject(refChildrenArray[i]);
      RecurseXSITree( nNodeIndex, x3dobject ) ;
   }

   return st;
}

CStatus CExportTup::ExportGeometry(TupperwareAggregate *pNodeAgg, XSI::X3DObject &x3dobject)
{
   CStatus st = CStatus::OK;

   TupNode tupNode(pNodeAgg);

   Geometry geom(x3dobject.GetActivePrimitive().GetGeometry());

   //
   //The reason why we are getting the reference ClassID rather
   //than the API class ID is because the reference ClassID will return
   //the instantiated object leaf class id rather than the API class ID.
   //On a PolygonMesh
   //geom.GetRef().GetClassID() -> siPolygonMeshID
   //geom.GetClassID() -> siGeometryID
   //
   switch(geom.GetRef().GetClassID())
   {
   case siPolygonMeshID:
      {
         ExportPolygonMesh(pNodeAgg, x3dobject);
      }
      break;
   case siNurbsSurfaceMeshID:
      {
      }
      break;
   case siNurbsCurveListID:
      {
      }
      break;
   }

   // export materials
   int nMatIndex = ExportMultiSubObjectMaterial(x3dobject);
   if (nMatIndex>=0)
   {
      TupperwareAggregate *pMultiSubObjectMaterialAgg = m_pTupMaterialPool->GetMaterial(nMatIndex);
      tupNode.AddMaterialReference(nMatIndex);
   }

   return st;
}

void CExportTup::ExportNull(TupperwareAggregate *pNodeAgg, X3DObject &x3dobject)
{
   int nObjectRef = m_pTupObjectPool->AddObjectAggregate();
   TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(nObjectRef);
   char *name = NULL;
   XSIW2A(&name, x3dobject.GetName().GetWideString());
   pObjectAgg->AddScalarString("MayaObjectName",NULL,name);
   TupObject tupObject(pObjectAgg);
   tupObject.SetObjectType(TupObject::LOCATOR);
   TupLocator tupLocator(pObjectAgg);
   tupLocator.AddFlags(0);
}

CStatus CExportTup::ExportPolygonMesh(TupperwareAggregate *pNodeAgg, X3DObject &x3dobject)
{
   CStatus st = CStatus::OK;

   Geometry geom(x3dobject.GetActivePrimitive().GetGeometry());
   PolygonMesh polygonmesh(geom);

   int nObjectRef = m_pTupObjectPool->AddObjectAggregate();

   TupNode tupNode(pNodeAgg);
   tupNode.AddObjectReference(nObjectRef);
   TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(nObjectRef);

   char *pslName = NULL;
   XSIW2A(&pslName, x3dobject.GetName().GetWideString());

   pObjectAgg->AddScalarString("MayaObjectName",NULL,pslName);
   TupObject tupObject(pObjectAgg);

   tupObject.SetObjectType(TupObject::POLY_MESH);

   TupPolyMesh tupPolyMesh(pObjectAgg);
  
   // get positions from XSI polygonmesh
   CPointRefArray pointRefArray(polygonmesh.GetPoints());
   MATH::CVector3Array positionArray(pointRefArray.GetPositionArray());

   // set positions	
   int nNumPositionVerts = pointRefArray.GetCount();
   float *pPositionVerts = new float[nNumPositionVerts*3];
   for(int i = 0; i  < pointRefArray.GetCount() ;i++)
   { 
      pPositionVerts[i*3] = (float)positionArray[i].GetX() * m_fUnitRatio;
      pPositionVerts[i*3+1] = (float)positionArray[i].GetY() * m_fUnitRatio;
      pPositionVerts[i*3+2] = (float)positionArray[i].GetZ() * m_fUnitRatio;
   }  

   // walk facets, build face vertex count array
   int nNumFaces = polygonmesh.GetFacets().GetCount();

   int nNumFaceNormalVerts = nNumFaces;
   float *pFaceNormalVerts = new float[nNumFaceNormalVerts*3];

   int *pFaceVertexCounts = new int[nNumFaces];
   int *pFaceNormalIndices = new int[nNumFaces];
   int *pFaceFlags = new int[nNumFaces];
   int *pFaceMaterialIndices = new int[nNumFaces];

   CFacetRefArray facetRefArray(polygonmesh.GetFacets());
   int nCount=0;
   for(i = 0; i < facetRefArray.GetCount();++i)
   {
      Facet facet(facetRefArray.GetItem(i));
      int nNumVerts = facet.GetPoints().GetCount();
      nCount += nNumVerts;
      pFaceVertexCounts[i] = nNumVerts;
   }

   // In XSI, I don't seem to have a way to get indices for samples or vertex normals.
   // So the array length for samples and normals are equal and based on the sum of the face vertex counts.
   int nSumVertexCounts = nCount;

   // get vertex normals from XSI polygonmesh
   CPolygonNodeRefArray nodeRefArray(polygonmesh.GetNodes());
   MATH::CVector3Array  normalArray(nodeRefArray.GetNormalArray());	
   int nNumNormalVerts = nSumVertexCounts;
   float *pNormalVerts = new float[nNumNormalVerts*3];

   // set vertexnormals
   for(int i = 0; i  < nodeRefArray.GetCount() ;i++)
   { 
      pNormalVerts[i*3] = (float)normalArray[i].GetX();
      pNormalVerts[i*3+1] = (float)normalArray[i].GetY();
      pNormalVerts[i*3+2] = (float)normalArray[i].GetZ();
   }  

   CRefArray clusterRefArray;
   polygonmesh.GetClusters().Filter(siSampledPointCluster,CStringArray(),L"",clusterRefArray);

   CRefArray uvClusterPropertiesRefArray;

   for(i = 0; i < clusterRefArray.GetCount(); i++)
   {
      Cluster cluster(clusterRefArray[i]);		
      if(cluster.GetProperties().Filter(siClsUVSpaceTxtType,CStringArray(), L"",uvClusterPropertiesRefArray) == CStatus::OK)		
         break;
   }

   TupArray<float> *pUVVerts;
   TupArray<int> *pUVIndices;
   int nNumUVSets = uvClusterPropertiesRefArray.GetCount();
   pUVVerts = new TupArray<float>[nNumUVSets];
   pUVIndices = new TupArray<int>[nNumUVSets];

   for(int nUVSetIndex = 0; nUVSetIndex < uvClusterPropertiesRefArray.GetCount(); nUVSetIndex++)
   {
      ClusterProperty uvProp(uvClusterPropertiesRefArray[nUVSetIndex]);

      //Set the elements
      CClusterPropertyElementArray uvElementArray = uvProp.GetElements();

      CDoubleArray uvValueArray = uvElementArray.GetArray();
      int nNumUVs = uvValueArray.GetCount()/3;
      pUVVerts[nUVSetIndex].SetSize(nNumUVs*3);
      pUVIndices[nUVSetIndex].SetSize(nSumVertexCounts);
      TupperwareAggregate *pMeshMapAgg = tupPolyMesh.AddMeshMap(i+1);
      for (i = 0; i < nNumUVs; i++)
      {
         pUVVerts[nUVSetIndex][i*3] = (float)uvValueArray[i*3];
         pUVVerts[nUVSetIndex][i*3+1] = (float)uvValueArray[i*3+1];
         pUVVerts[nUVSetIndex][i*3+2] = 1.0f;
      }
   }

   // walk facets, build indices
   int *pPositionIndices = new int[nSumVertexCounts];
   int *pVertexNormalIndices = new int[nSumVertexCounts];

	CSampleRefArray sampleRefArray;	
   CLongArray pointIndexArray;
   CLongArray sampleIndexArray;

   nCount = 0;
   for(i = 0; i < facetRefArray.GetCount();++i)
   {
      MATH::CVector3 facenormal(0,0,0);

      Facet facet(facetRefArray.GetItem(i));
      int nNumVerts = facet.GetPoints().GetCount();

      pointRefArray	= facet.GetPoints();
      sampleRefArray	= facet.GetSamples();

      pointIndexArray  = pointRefArray.GetIndexArray();
      sampleIndexArray = sampleRefArray.GetIndexArray();

      for(int j = 0; j < nNumVerts; ++j)
      {
         pPositionIndices[nCount] = pointIndexArray[j];
         pVertexNormalIndices[nCount] = nCount;
         facenormal += MATH::CVector3(pNormalVerts[nCount*3],pNormalVerts[nCount*3+1],pNormalVerts[nCount*3+2]);

         for (int nUVSetIndex =0; nUVSetIndex < nNumUVSets; nUVSetIndex++)
            pUVIndices[nUVSetIndex][nCount] = nCount;

         nCount++;
      }
   
      pFaceFlags[i] =  4;
      facenormal.Normalize(facenormal);

      pFaceNormalVerts[i*3] = (float)facenormal.GetX();
      pFaceNormalVerts[i*3+1] = (float)facenormal.GetY();
      pFaceNormalVerts[i*3+2] = (float)facenormal.GetZ();
      pFaceNormalIndices[i] = i;

      pFaceMaterialIndices[i] = 0;
   }

   tupPolyMesh.AddPositionVerts(pPositionVerts,nNumPositionVerts);
   tupPolyMesh.AddNormalVerts(pNormalVerts,nNumNormalVerts);

   tupPolyMesh.AddPositionIndices(pPositionIndices,nCount);
   tupPolyMesh.AddNormalIndices(pVertexNormalIndices,nCount);

   tupPolyMesh.AddFaceVertexCounts(pFaceVertexCounts,nNumFaces);
   tupPolyMesh.AddFaceMaterialIndices(pFaceMaterialIndices,nNumFaces);
   tupPolyMesh.AddFaceFlags(pFaceFlags,nNumFaces);

   tupPolyMesh.AddFaceNormalVerts(pFaceNormalVerts,nNumFaces);
   tupPolyMesh.AddFaceNormalIndices(pFaceNormalIndices,nNumFaces);

   for (int nUVSetIndex = 0; nUVSetIndex < nNumUVSets; nUVSetIndex++)
   {
      TupperwareAggregate *pMeshMapAgg = tupPolyMesh.GetMeshMap(nUVSetIndex);
      TupPolyMeshMap tupPolyMeshMap(pMeshMapAgg);
      tupPolyMeshMap.AddMapVerts(pUVVerts[nUVSetIndex].GetData(),pUVVerts[nUVSetIndex].GetSize()/3);
      tupPolyMeshMap.AddMapIndices(pUVIndices[nUVSetIndex].GetData(),nCount);
   }

   delete [] pPositionIndices;
   delete [] pVertexNormalIndices;
   delete [] pFaceNormalIndices;
   delete [] pFaceFlags;
   delete [] pFaceMaterialIndices;
   delete [] pUVVerts;
   delete [] pUVIndices;

   return st;
}

int CExportTup::ExportMultiSubObjectMaterial(X3DObject &x3dobject)
{
   int nMatIndex = -1;

   Material objectMaterial(x3dobject.GetMaterial());
   if(objectMaterial.IsValid())
   {
      nMatIndex = m_pTupMaterialPool->AddMaterial();
      TupperwareAggregate *pMultiSubObjectMaterialAgg = m_pTupMaterialPool->GetMaterial(nMatIndex);
      TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMultiSubObjectMaterialAgg);		
      tupMultiSubObjectMaterial.AddName("MultiSub");
      TupMaterial tupMaterial(pMultiSubObjectMaterialAgg);
      tupMaterial.SetMaterialType(TupMaterial::MULTI_SUB_OBJECT_MATERIAL);

      //First we must export the shaders as they will be required by the connection points
      CRefArray shaderRefArray(objectMaterial.GetShaders());

      CRefArray allShaderRefArray;
      for(int i = 0; i < shaderRefArray.GetCount(); i++)
      {
         Shader shader(shaderRefArray[i]);
         TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(i);
         TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
         int nMaterialIndex = ExportShader(shader,allShaderRefArray);	
         tupSubMaterial.AddMaterialReference(nMaterialIndex);
         tupSubMaterial.AddFlags(1);
      }
   }

   return nMatIndex;
}

int CExportTup::ExportShader(Shader &shader,XSI::CRefArray &shaderRefArray)
{
   if( shaderRefArray.GetItem(shader.GetName()).IsValid() != false )
      return -1;

   int nMaterialIndex = m_pTupMaterialPool->AddMaterial();
   TupperwareAggregate *pStandardMaterialMapAgg = m_pTupMaterialPool->GetMaterial(nMaterialIndex);

   TupStandardMaterial tupStandardMaterial(pStandardMaterialMapAgg);

   TupMaterial tupMaterial(pStandardMaterialMapAgg);
   tupMaterial.SetMaterialType(TupMaterial::STANDARD_MATERIAL);

   char* pslShaderName = NULL;
   XSIW2A(&pslShaderName, shader.GetName().GetWideString());

   tupStandardMaterial.AddName(pslShaderName);

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

   shaderRefArray.Add(shader.GetRef());

   CRefArray nestedShaderRefArray(shader.GetShaders());

   for(int i=0; i < nestedShaderRefArray.GetCount(); i++)
   {
      Shader shader(nestedShaderRefArray[i]);
      if (nestedShaderRefArray[i].IsA(siShaderID))
      {
         ExportShader(shader, shaderRefArray);
      }
   }

   return nMaterialIndex;
}

int CExportTup::ExportDefaultMaterial(Material material)
{
   int nMaterialIndex = m_pTupMaterialPool->AddMaterial();
   TupperwareAggregate *pStandardMaterialMapAgg = m_pTupMaterialPool->GetMaterial(nMaterialIndex);

   TupStandardMaterial tupStandardMaterial(pStandardMaterialMapAgg);

   TupMaterial tupMaterial(pStandardMaterialMapAgg);
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

   return nMaterialIndex;
}

#if 0
void CHxtApp::AddMapReference(TupperwareAggregate *pStandardMaterialAgg,INode *mapnode,int mapRef)
{
   MapObject *mapobject = (MapObject*)mapnode->GetObject();
   TupStandardMaterial tupStandardMaterial(pStandardMaterialAgg);

   int nummaps = tupStandardMaterial.GetNumMaps();
   TupperwareAggregate *pStandardMaterialMapAgg = tupStandardMaterial.AddMap(nummaps);
   TupStandardMaterialMap standardMaterialMap(pStandardMaterialMapAgg);
   if (mapobject->GetMapType()->GetValue() == MapObject::REPLACE)
      standardMaterialMap.AddMapName("Diffuse Color");
   else if (mapobject->GetMapType()->GetValue() == MapObject::REFLECTIVE)
      standardMaterialMap.AddMapName("Reflection");

   standardMaterialMap.AddAmount(mapobject->GetOpacity()->GetValue());
   standardMaterialMap.AddFlags(1);

   standardMaterialMap.AddMapReference(mapRef); 
}

TupperwareAggregate *CHxtApp::AddMap(TupperwareAggregate *pStandardMaterialAgg,INode *mapnode,int channelnum)
{
   MapObject *mapobject = (MapObject*)mapnode->GetObject();
   TupStandardMaterial tupStandardMaterial(pStandardMaterialAgg);

   int nummaps = tupStandardMaterial.GetNumMaps();
   TupperwareAggregate *pStandardMaterialMapAgg = tupStandardMaterial.AddMap(nummaps);
   TupStandardMaterialMap standardMaterialMap(pStandardMaterialMapAgg);

   if (mapobject->GetMapType()->GetValue() == MapObject::REFLECTIVE)
      standardMaterialMap.AddMapName("Reflection");
   else if (mapobject->GetMapType()->GetValue() == MapObject::MULTIPLY)
      standardMaterialMap.AddMapName("Diffuse Color");// no equivalent
   else
      standardMaterialMap.AddMapName("Diffuse Color");

   standardMaterialMap.AddAmount(mapobject->GetOpacity()->GetValue());
   standardMaterialMap.AddFlags(1);

   int mapRef = m_pTupMapPool->AddMap();
   standardMaterialMap.AddMapReference(mapRef);

   TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapRef);	
   TupMap tupMap(pMapAgg);
   tupMap.SetMapType(TupMap::BITMAP_MAP);

   TupBitmapMap bitmapMap(pMapAgg);
   bitmapMap.AddName(mapnode->GetName());
   bitmapMap.AddMapChannel(channelnum);
   bitmapMap.AddMapping(0);
   bitmapMap.AddUOffset(0.000000);
   bitmapMap.AddVOffset(0.000000);
   bitmapMap.AddUTiling(mapobject->GetScaleX()->GetValue());
   bitmapMap.AddVTiling(mapobject->GetScaleY()->GetValue());
   bitmapMap.AddMapAxis(0);
   bitmapMap.AddUAngle(0.000000);
   bitmapMap.AddVAngle(0.000000);
   bitmapMap.AddWAngle(0.000000);
   bitmapMap.AddBlur(1.000000);
   bitmapMap.AddBlurOffset(0.000000);
   bitmapMap.AddNoiseAmount(1.000000);
   bitmapMap.AddNoiseLevels(1);
   bitmapMap.AddNoiseSize(1.000000);
   bitmapMap.AddCroppingPlacementU(0.000000);
   bitmapMap.AddCroppingPlacementV(0.000000);
   bitmapMap.AddCroppingPlacementW(1.000000);
   bitmapMap.AddCroppingPlacementH(1.000000);
   bitmapMap.AddPlacementJitter(1.000000);
   bitmapMap.AddStartFrame(0);
   bitmapMap.AddPlaybackRate(1.000000);
   bitmapMap.AddOutputAmount(1.000000);
   bitmapMap.AddOutputRGBOffset(0.000000);
   bitmapMap.AddOutputRGBLevel(1.000000);
   bitmapMap.AddOutputBumpAmount(1.000000);
   bitmapMap.AddBlendMode(1);

   int flags = 0;
   if (mapobject->GetTileX()->GetValue())
      flags |= TupBitmapMap::TUP_U_WRAP;
   if (mapobject->GetTileY()->GetValue())
      flags |= TupBitmapMap::TUP_V_WRAP;

   bitmapMap.AddFlags(flags);

   return pMapAgg;
}

void CHxtApp::AddComposite(MyMaterial *mymaterial,TupperwareAggregate *pStandardMaterialAgg,IntMap *materialmaplookup,IntMap *uvdatamap,IntMap *imagelookup)
{
   // set map on material
   TupStandardMaterial tupStandardMaterial(pStandardMaterialAgg);
   int nummaps = tupStandardMaterial.GetNumMaps();
   TupperwareAggregate *pStandardMaterialMapAgg = tupStandardMaterial.AddMap(nummaps);
   TupStandardMaterialMap standardMaterialMap(pStandardMaterialMapAgg);

   standardMaterialMap.AddMapName("Diffuse Color");//support only

   standardMaterialMap.AddAmount(1.0f);
   standardMaterialMap.AddFlags(1);

   // add composite map to pool
   int mapRef = m_pTupMapPool->AddMap();//do we really need map reference?
   standardMaterialMap.AddMapReference(mapRef);
   TupperwareAggregate *pCompositeMapAgg = m_pTupMapPool->GetMap(mapRef);	
   TupMap tupMap(pCompositeMapAgg);
   tupMap.SetMapType(TupMap::COMPOSITE_MAP);

   TupCompositeMap compositeMap(pCompositeMapAgg);
   compositeMap.AddName("layeredTexture");

   // add submaps

   for (int mapindex = 0; mapindex < mymaterial->m_materialmaps.Length(); mapindex++)
   {
      MyMaterialMap *mymaterialmap = &mymaterial->m_materialmaps[mapindex];

      INode *mapnode = mymaterialmap->m_mapnode;
      MapObject *mapobject = (MapObject*)mymaterialmap->m_mapnode->GetObject();

      if ((mapobject->GetMapType()->GetValue()!=MapObject::REPLACE)&&(mapobject->GetMapType()->GetValue()!=MapObject::MULTIPLY))
         continue;

      ImageObject *imageobject = mymaterialmap->m_imageobject;

      if (materialmaplookup->HasKey((int)mymaterialmap->GetHashValue()))
         continue;

      materialmaplookup->SetValue((int)mymaterialmap->GetHashValue(),1);

      MyMaterialStamp *mymaterialstamp = &mymaterial->m_materialstamps[mapindex];
      StampObject *stampobject = mymaterialstamp->m_stampobject;
      if (stampobject)
      {
         int meshmapindex;
         if (uvdatamap->GetValue((int)stampobject,meshmapindex))
         {		
            INode *mapnode = mymaterialmap->m_mapnode;
            TupperwareAggregate *pMapAgg = AddSubMap(pCompositeMapAgg,mapnode,meshmapindex);

            int imageRef;
            if (!imagelookup->GetValue((int)imageobject,imageRef))
            {
               AddImage(imageobject->GetFilePathName(),pMapAgg,imageRef);
               imagelookup->SetValue((int)imageobject,(int)imageRef);
            }
            else
            {
               AddImageReference(pMapAgg,imageRef);
            }
         }
      }
   }
}

TupperwareAggregate *CHxtApp::AddSubMap(TupperwareAggregate *pCompositeMapAgg,INode *mapnode,int meshmapindex)
{
   MapObject *mapobject = (MapObject*)mapnode->GetObject();

   int mapRef = m_pTupMapPool->AddMap();	
   TupperwareAggregate *pMapAgg = m_pTupMapPool->GetMap(mapRef);	
   TupMap tupMap(pMapAgg);
   tupMap.SetMapType(TupMap::BITMAP_MAP);

   TupBitmapMap bitmapMap(pMapAgg);
   bitmapMap.AddName(mapnode->GetName());
   bitmapMap.AddMapChannel(meshmapindex);
   bitmapMap.AddMapping(0);
   bitmapMap.AddUOffset(0.000000);
   bitmapMap.AddVOffset(0.000000);
   bitmapMap.AddUTiling(mapobject->GetScaleX()->GetValue());
   bitmapMap.AddVTiling(mapobject->GetScaleY()->GetValue());
   bitmapMap.AddMapAxis(0);
   bitmapMap.AddUAngle(0.000000);
   bitmapMap.AddVAngle(0.000000);
   bitmapMap.AddWAngle(0.000000);
   bitmapMap.AddBlur(1.000000);
   bitmapMap.AddBlurOffset(0.000000);
   bitmapMap.AddNoiseAmount(1.000000);
   bitmapMap.AddNoiseLevels(1);
   bitmapMap.AddNoiseSize(1.000000);
   bitmapMap.AddCroppingPlacementU(0.000000);
   bitmapMap.AddCroppingPlacementV(0.000000);
   bitmapMap.AddCroppingPlacementW(1.000000);
   bitmapMap.AddCroppingPlacementH(1.000000);
   bitmapMap.AddPlacementJitter(1.000000);
   bitmapMap.AddStartFrame(0);
   bitmapMap.AddPlaybackRate(1.000000);
   bitmapMap.AddOutputAmount(1.000000);
   bitmapMap.AddOutputRGBOffset(0.000000);
   bitmapMap.AddOutputRGBLevel(1.000000);
   bitmapMap.AddOutputBumpAmount(1.000000);


   int flags = 0;
   if (mapobject->GetMapType()->GetValue() == MapObject::MULTIPLY)
      flags = TupBitmapMap::BLEND_MODE_MULTIPLY;
   else
      flags = TupBitmapMap::BLEND_MODE_OVER;

   bitmapMap.AddBlendMode(flags);

   flags = 0;

   if (mapobject->GetTileX()->GetValue())
      flags |= TupBitmapMap::TUP_U_WRAP;
   if (mapobject->GetTileY()->GetValue())
      flags |= TupBitmapMap::TUP_V_WRAP;

   bitmapMap.AddFlags(flags);

   TupCompositeMap compositeMap(pCompositeMapAgg);
   int numsubmaps = compositeMap.GetNumSubMaps();
   TupperwareAggregate *compositeSubMapAgg = compositeMap.AddSubMap(numsubmaps);
   TupCompositeMapSubMap tupCompositeSubMap(compositeSubMapAgg);
   tupCompositeSubMap.AddMapReference(mapRef);	
   tupCompositeSubMap.AddFlags(1);

   return pMapAgg;
}
#endif

void CExportTup::ExportTransform(TupperwareAggregate *pNodeAgg,X3DObject &x3dobject)
{
   MATH::CMatrix4 cmatrix4;
 
   KinematicState KineState = x3dobject.GetKinematics().GetLocal();
   MATH::CTransformation cTransform;
   cTransform = KineState.GetTransform();
   cmatrix4 = cTransform.GetMatrix4();

   TupNode tupNode(pNodeAgg);

   TupperwareAggregate *pTransformAgg = tupNode.AddTransform();
   TupTransform tupTransform(pTransformAgg);

   tupTransform.SetTransformType(TupTransform::STATIC_MATRIX);
   TupStaticMatrix tupStaticMatrix(pTransformAgg);

   float matrix[] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
   tupStaticMatrix.AddMatrix(matrix);

   ConvertCMatrix4ToTupMatrix(pTransformAgg,cmatrix4);

   return;
}

CStatus CExportTup::ConvertCMatrix4ToTupMatrix(TupperwareAggregate *pTransformAgg,MATH::CMatrix4 &matrix)
{
   TupTransform tupTransform(pTransformAgg);

   if (tupTransform.GetTransformType() != TupTransform::STATIC_MATRIX)
      return CStatus::Fail;

   TupStaticMatrix tupStaticMatrix(pTransformAgg);

   float *pMatrix;
   tupStaticMatrix.GetMatrix(&pMatrix);

   for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
         pMatrix[(i*4)+j] = (float)matrix.GetValue(i,j);

   pMatrix[(3*4)+0] = pMatrix[(3*4)+0] * m_fUnitRatio;
   pMatrix[(3*4)+1] = pMatrix[(3*4)+1] * m_fUnitRatio;
   pMatrix[(3*4)+2] = pMatrix[(3*4)+2] * m_fUnitRatio;
 
   return CStatus::OK;
}
