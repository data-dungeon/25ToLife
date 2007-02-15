// RC  5/3/2002  \Randy95\Plugin\HXT\Ply\Import.cpp

#include "stdafx.h"
#include "StringLi.h"
#include "Hxt.h"
#include "HashFile.h"
#include "F_Protos.h"
#include "ImportModelDialog.h"

#include "SDK\Misc.h"
#include "SDK\HModel.h"

BOOL CHxtApp::OnImportModelCache(HModelCache *hmc)
{
   CString caption, filter;
   caption.LoadString(IDS_IMPORTTITLE);
   filter.LoadString(IDS_FILEFILTERS);
   String defdir;//not used for multiselect dlg
   String ext("TUP");
   StringList *namelist;

   if (!OpenRequest((LPCTSTR)caption, (LPCTSTR)filter, defdir, ext.Get(), ext.Get(), NULL, NULL, &namelist))
      return TRUE;

   if (namelist==NULL) {
      CString caption;
      caption.LoadString(IDS_IMPORTTITLE);
      CString text;
      text.LoadString(IDS_ERRORIMPORTING);
      MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
      return FALSE;
   }

   CImportModelDialog cdlg;
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   for (POSITION namepos = namelist->GetHeadPosition();namepos;) {
      String filename = namelist->GetNext(namepos);
      String nameonly = filename.GetNameOnly();
      IEPolyModel *patchmodel = ImportTUP(filename);
      if (patchmodel==NULL) {
         ReportImportError(filename);
         continue;
      }
      if (!hmc->MergeIEModel(patchmodel, filename.Get(), cdlg.m_peaktolerance, TRUE, TRUE))
         ReportImportError(filename);

      delete patchmodel;
   }
   DeleteStringList(namelist);

   return TRUE;
}

BOOL CHxtApp::OnImportObjectCacheContainer(HObjectCacheContainer *hocc)
{
   CString caption, filter;
   caption.LoadString(IDS_IMPORTTITLE);
   filter.LoadString(IDS_FILEFILTERS);
   String defdir;//not used for multiselect dlg
   String ext("TUP");
   StringList *namelist;

   if (!OpenRequest((LPCTSTR)caption, (LPCTSTR)filter, defdir, ext.Get(), ext.Get(), NULL, NULL, &namelist))
      return TRUE;

   if (namelist==NULL) {
      CString caption;
      caption.LoadString(IDS_IMPORTTITLE);
      CString text;
      text.LoadString(IDS_ERRORIMPORTING);
      MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
      return FALSE;
   }

   CImportModelDialog cdlg;
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   for (POSITION namepos = namelist->GetHeadPosition();namepos;) {
      String filename = namelist->GetNext(namepos);
      String nameonly = filename.GetNameOnly();
      HModelCache *hmc = HModelCache::New(nameonly.Get());
      if (hmc==NULL) {
         ReportImportError(filename);
         return FALSE;
      }
      IEPolyModel *patchmodel = ImportTUP(filename);
      if (patchmodel==NULL) {
         ReportImportError(filename);
         continue;
      }
      if (!hmc->MergeIEModel(patchmodel, filename.Get(), cdlg.m_peaktolerance, FALSE, TRUE))
         ReportImportError(filename);

      delete patchmodel;
   }

   return TRUE;
}

void CHxtApp::ReportImportError(const String &filename)
{
   CString caption;
   caption.LoadString(IDS_IMPORTTITLE);
   CString text;
   text.LoadString(IDS_ERRORIMPORTINGMODEL);
   String temp = (LPCTSTR)text;
   temp.Replace("%1", filename);
   text = temp.Get();
   MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
}

IEPolyModel *CHxtApp::ImportTUP(const String &pfilename)
{
 	FileStream fs;
	String filename = pfilename;

	IEPolyModel *model = new IEPolyModel;

	m_pSceneAgg = NULL;
	m_pTupScene = NULL;
	m_pTupNodePool = NULL;
	m_pTupObjectPool = NULL;
	m_pTupMaterialPool = NULL;
	m_pTupMapPool = NULL;
	m_pTupImagePool = NULL;

	int result = TupperwareLoadSaveManager::Read(filename.Get(),&m_pSceneAgg);
	if (!result)
	{
		return NULL;
	}

	if (!m_pSceneAgg)
		return NULL;
	
	TupScene *m_pTupScene = new TupScene(m_pSceneAgg);
	
	int unitflags;
	m_pTupScene->GetUnitFlags(unitflags);

	if (unitflags & TupScene::LINEAR_UNIT_CENTIMETER)
		m_unitratio = 1.0f;
	else if (unitflags & TupScene::LINEAR_UNIT_METER)
		m_unitratio = 100.0f;
	else
		return NULL;

	TupperwareAggregate *pNodePoolAgg = m_pTupScene->GetNodePool();
	if (pNodePoolAgg)
		m_pTupNodePool = new TupNodePool(pNodePoolAgg);
	else
		return NULL;

	TupperwareAggregate *pObjectPoolAgg = m_pTupScene->GetObjectPool();
	if (pObjectPoolAgg)
		m_pTupObjectPool = new TupObjectPool(pObjectPoolAgg);
	else
		return NULL;

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

	TupObjectConverter tupObjectConverter(*m_pTupObjectPool,*m_pTupModifierPool);

	tupObjectConverter.ConvertObjects(TupObject::POLY_MESH,TupObject::TRI_MESH);

	int numNodes = m_pTupNodePool->GetNumNodes();

	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);
		int objectRef;
		tupNode.GetObjectReference(objectRef);
		if (objectRef==-1)
			continue;

		// find the base tri mesh
		TupperwareAggregate *pObjectAgg;
		while (1)
		{
			pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectRef);
			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType() == TupObject::TRI_MESH)
			{
				break;
			}
			if (tupObject.GetObjectType() == TupObject::DERIVED)
			{
				TupDerived tupDerived(pObjectAgg);
				tupDerived.GetObjectReference(objectRef);
			}
			else
			{
				pObjectAgg = NULL; // oops we don't know what this is...
				break;
			}
		}
		if (!pObjectAgg)
			continue;

		TupperwareAggregate *pTransformAgg = tupNode.GetTransform();		

		char *debug;
		tupNode.GetName(&debug);
		
		int i;

		// Set Material Attributes List
		int numMaterials = 1;
		model->m_attrlist.SetSize(numMaterials);
		for (i = 0; i < numMaterials; i++)
		{
			  Attr &attr = model->m_attrlist.ElementAt(i);

			  BYTE red = 128, green = 128, blue = 128;

			  attr.m_diffusecolor.red = (float)red/255.0f;
			  attr.m_diffusecolor.green = (float)green/255.0f;
			  attr.m_diffusecolor.blue = (float)blue/255.0f;

			  attr.m_specularcolor.red = (float)red/255.0f;
			  attr.m_specularcolor.green = (float)green/255.0f;
			  attr.m_specularcolor.blue = (float)blue/255.0f;

			  attr.m_diffusefalloff = 1.0;
			  attr.m_specularsize = 0.0;
			  attr.m_specularintensity = 0.0;
			  attr.m_ambiance = 0.0;
			  attr.m_roughness = 0.0;
			  attr.m_roughnessscale = 0.0;
			  attr.m_reflectivity = 0.0;
			  attr.m_transparency = 0.0;
			  attr.m_refraction = 0.0;
		}

		// Load Texture Maps List

		if (m_pTupImagePool)
		{
			int numImages = m_pTupImagePool->GetNumImages();
			model->m_maplist.SetSize(numImages);
			for (i = 0; i < numImages; i++)
			{
				TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(i);
				TupImage tupImage(pImageAgg);
				IEMap &map = model->m_maplist.ElementAt(i);
				char *pathname;
				tupImage.GetFilePathName(&pathname);
				strcpy(map.m_filename,pathname);
			}
		}
		
		int materialRef;
		tupNode.GetMaterialReference(materialRef);

		AddTriMeshObject(pObjectAgg,pTransformAgg,materialRef,model);
	}

	delete m_pSceneAgg;
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupObjectPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;

	return model;
}

void CHxtApp::AddTriMeshObject(TupperwareAggregate *pObjectAgg,TupperwareAggregate *pTransformAgg,int matref,IEPolyModel *model)
{
	int i;
	Vector *pArray;
	TupObject tupObject(pObjectAgg);

	if (tupObject.GetObjectType() == TupObject::TRI_MESH)
	{
		TupTriMesh tupMesh(pObjectAgg);

		int position_offset = model->m_vertexlist.m_array.GetSize();
		int normal_offset = model->m_normallist.m_array.GetSize();

		// convert node transform matrix
		Matrix44 matrix;
		matrix.SetIdentity();
		ConvertTupMatrixToMatrix44(pTransformAgg,matrix);

		// do positions
		int numPositionVerts;
		float *pPositionVerts;
		tupMesh.GetPositionVerts(&pPositionVerts,numPositionVerts);
		pArray = (Vector *)pPositionVerts;
		
		for (i=0;i<numPositionVerts;i++)
		{
			Vector &position = pArray[i];
			Vector transformedposition = matrix.Transpose() * position;
			transformedposition *= m_unitratio;
			model->m_vertexlist.m_array.Add(transformedposition);
		}

		// do vertex normals
		int numNormalVerts;
		float *pNormalVerts;
		tupMesh.GetNormalVerts(&pNormalVerts,numNormalVerts);
		pArray = (Vector *)pNormalVerts;
				
		for (i=0;i<numNormalVerts;i++)
		{
			Vector &vector = pArray[i];
			model->m_normallist.m_array.Add(vector);
		}

		int numFaces,numMatFaces,numNormalFaces,dummy;
		int *pPositionIndices;
		tupMesh.GetPositionIndices(&pPositionIndices,numFaces);
		int *pNormalIndices;
		tupMesh.GetNormalIndices(&pNormalIndices,numNormalFaces);

		int *pFaceMaterialIndicies;
		tupMesh.GetFaceMaterialIndices(&pFaceMaterialIndicies,numMatFaces);

		TupperwareAggregate *pMap = NULL;	

		int cnt = 0;
		for (i = 0; i < numFaces; i++)
		{
			IEPoly *poly = new IEPoly(NULL);

			poly->m_attrid = 0;//default shader

			poly->m_vertexid[0] = pPositionIndices[cnt]+position_offset;
			poly->m_vertexid[1] = pPositionIndices[cnt+1]+position_offset;
			poly->m_vertexid[2] = pPositionIndices[cnt+2]+position_offset;
			poly->m_vertexid[3] = pPositionIndices[cnt]+position_offset;

			if (numNormalFaces>0)
			{
				poly->m_normalid[0] = pNormalIndices[cnt]+normal_offset;
				poly->m_normalid[1] = pNormalIndices[cnt+1]+normal_offset;
				poly->m_normalid[2] = pNormalIndices[cnt+2]+normal_offset;
				poly->m_normalid[3] = pNormalIndices[cnt]+normal_offset;
			}
			else
			{
				poly->m_normalid[0] = -1;
				poly->m_normalid[1] = -1;
				poly->m_normalid[2] = -1;
				poly->m_normalid[3] = -1;
			}
			// Load Poly Texture Maps
			
			if (numMatFaces>0)
			{
				int materialIndex = pFaceMaterialIndicies[i];

				int numMeshMaps = tupMesh.GetNumMeshMaps();
				
				int flags;
				float utile = 1.0f,vtile = 1.0f;

				for (int j = 0; j < numMeshMaps; j++)
				{
					IEMapNode *mapnode = new IEMapNode;
					
					TupperwareAggregate *pUVMeshMapAgg = tupMesh.GetMeshMap(j);
					TupTriMeshMap tupTriMeshMap(pUVMeshMapAgg);

					int channelnum = tupTriMeshMap.GetMapChannel();

					int imageref = -1;
					
					if (channelnum >= 1)
					{
						int *pMapIndices;
						
						tupTriMeshMap.GetMapIndices(&pMapIndices,dummy);
						
						TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(matref);
						TupMaterial tupMaterial(pMaterialAgg);
						
						if (tupMaterial.GetMaterialType() == TupMaterial::MULTI_SUB_OBJECT_MATERIAL)
						{
							TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
							int numSubMaterials = tupMultiSubObjectMaterial.GetNumSubMaterials();
							if (materialIndex>=numSubMaterials)
								materialIndex=0;

							TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
							TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);

							int subMaterialRef;
							tupSubMaterial.GetMaterialReference(subMaterialRef);

							pSubMaterialAgg = m_pTupMaterialPool->GetMaterial(subMaterialRef);
								
							TupMaterial tupMaterial(pSubMaterialAgg);

							if (tupMaterial.GetMaterialType() == TupMaterial::STANDARD_MATERIAL)
							{
								TupStandardMaterial tupStandardMaterial(pSubMaterialAgg);

								int numMaps = tupStandardMaterial.GetNumMaps();
								for (int l = 0; l < numMaps; l++)
								{
									TupperwareAggregate *pStandardMaterialMap = tupStandardMaterial.GetMap(l);

									TupStandardMaterialMap tupStandardMaterialMap(pStandardMaterialMap);

									int mapref;

									tupStandardMaterialMap.GetMapReference(mapref);

									pMap = m_pTupMapPool->GetMap(mapref);

									TupMap tupMap(pMap);

									if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
									{
										TupBitmapMap bitmapMap(pMap);

										int mapChannel;
										bitmapMap.GetMapChannel(mapChannel);

										// did we find one that matches this meshmap?
										if (mapChannel == channelnum)
										{
											bitmapMap.GetImageReference(imageref);

											bitmapMap.GetFlags(flags);
											
											if (flags & TupBitmapMap::TUP_U_WRAP)
											{
												bitmapMap.GetUTiling(utile);
												if (utile==1.0f)
													utile=1.01f;
											}
											
											if (flags & TupBitmapMap::TUP_U_WRAP)
											{
												bitmapMap.GetUTiling(vtile);
												if (vtile==1.0f)
													vtile=1.01f;
											}
										}
									}
								}
							}
						}
						else
						{
							TupStandardMaterial tupStandardMaterial(pMaterialAgg);
							
							int numMaps = tupStandardMaterial.GetNumMaps();
							for (int l = 0; l < numMaps; l++)
							{
								TupperwareAggregate *pStandardMaterialMap = tupStandardMaterial.GetMap(l);
								
								TupStandardMaterialMap tupStandardMaterialMap(pStandardMaterialMap);
								
								int mapref;
								
								tupStandardMaterialMap.GetMapReference(mapref);
								
								TupperwareAggregate *pMap = m_pTupMapPool->GetMap(mapref);
								
								TupMap tupMap(pMap);
								
								if (tupMap.GetMapType() == TupMap::BITMAP_MAP)
								{
									TupBitmapMap bitmapMap(pMap);

									int mapChannel;
									bitmapMap.GetMapChannel(mapChannel);

									if (mapChannel == channelnum)
									{
										bitmapMap.GetImageReference(imageref);

										bitmapMap.GetFlags(flags);
										
										if (flags & TupBitmapMap::TUP_U_WRAP)
										{
											bitmapMap.GetUTiling(utile);
											if (utile==1.0f)
												utile=1.01f;
										}
										
										if (flags & TupBitmapMap::TUP_U_WRAP)
										{
											bitmapMap.GetUTiling(vtile);
											if (vtile==1.0f)
												vtile=1.01f;
										}
									}
								}
							}
						}
						
						if (imageref>=0)
						{
							int numVerts = 0;
							TupVertex3 *pMapVerts;
							
							int *pMapIndices;
							tupTriMeshMap.GetMapIndices(&pMapIndices,dummy);
							
							tupTriMeshMap.GetMapVerts((float**)&pMapVerts,numVerts);

							mapnode->m_mapid = imageref;
							
							mapnode->m_uv[0].x = pMapVerts[pMapIndices[cnt]].x;
							mapnode->m_uv[0].y = pMapVerts[pMapIndices[cnt]].y;
							mapnode->m_uv[1].x = pMapVerts[pMapIndices[cnt+1]].x;
							mapnode->m_uv[1].y = pMapVerts[pMapIndices[cnt+1]].y;
							mapnode->m_uv[2].x = pMapVerts[pMapIndices[cnt+2]].x;
							mapnode->m_uv[2].y = pMapVerts[pMapIndices[cnt+2]].y;
							mapnode->m_uv[3].x = pMapVerts[pMapIndices[cnt]].x;
							mapnode->m_uv[3].y = pMapVerts[pMapIndices[cnt]].y;
							
							mapnode->m_repeat.x = utile;
							mapnode->m_repeat.y = vtile;
							
							mapnode->m_kind = IEMapNode::COLORMAP;

							poly->m_mapnodearray.Add(mapnode);
						}
					}
				}
			}

			cnt+=3;
			
			model->m_polyarray.Add(poly);
		}
	}
}

int CHxtApp::ConvertTupMatrixToMatrix44(TupperwareAggregate *pTransformAgg,Matrix44 &matrix)
{
	TupTransform tupTransform(pTransformAgg);
	if (tupTransform.GetTransformType() == TupTransform::STATIC_MATRIX)
	{
	TupStaticMatrix tupStaticMatrix(pTransformAgg);
		float *pMatrix;
		tupStaticMatrix.GetMatrix(&pMatrix);

		Vector4 *v0 = (Vector4*)&pMatrix[0];
		Vector4 *v1 = (Vector4*)&pMatrix[4];
		Vector4 *v2 = (Vector4*)&pMatrix[8];
		Vector4 *v3 = (Vector4*)&pMatrix[12];
		matrix = Matrix44(*v0,*v1,*v2,*v3);
		return 1;
	}
	return 0;
}

void CHxtApp::LoadBones(FileStream &fs, IEBone **parentuplink, IEBone *parent)
{
   BYTE child, sibling=TRUE;
   IEBone **siblinguplink = NULL;

   while (sibling && !fs.Eof()) {
      fs.Read(&child, 1);
      fs >> sibling;
      IEBone *bone = new IEBone;
      
      if (parentuplink) {
         *parentuplink = bone;
         parentuplink = NULL;
      }

      if (siblinguplink)
         *siblinguplink = bone;
      fs.ParseToken(bone->m_name, '\0');
      
      fs >> bone->m_pivotpos;
      fs >> bone->m_endpos;
      fs >> bone->m_rollpos;

      LONG count;
      fs >> count;
      bone->m_vertexidarray.SetSize(count);
      for (int i =0; i < count; i++) {
         fs >> (LONG &)bone->m_vertexidarray[i];
      }

      if (child)
         LoadBones(fs, &bone->m_child, bone);

      siblinguplink = sibling?&bone->m_sibling:NULL;
   }
}
