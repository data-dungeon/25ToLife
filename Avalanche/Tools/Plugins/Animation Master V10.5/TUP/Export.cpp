// RC  4/17/2002  \Randy95\Plugin\HXT\Ply\Export.cpp

#include "stdafx.h"
#include "Hxt.h"
#include "HashFile.h"
#include "ExportModelDialog.h"
#include "ExportModelsDialog.h"
#include "ExportActionsDialog.h"
#include "F_Protos.h"

#include "SDK\HModel.h"
#include "SDK\HAction.h"
#include "SDK\Misc.h"
#include "SDK\HProject.h"
#include "SDK\HChor.h"
#include "SDK\HProgres.h"

BOOL CHxtApp::OnExportModelCache(HModelCache *hmc)
{

   CExportModelDialog cdlg(hmc->GetName());
   if (cdlg.DoModal()!=IDOK)
      return TRUE;
   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   String modelmapfilename = cdlg.m_filename.GetWithoutSuffix() + ".bmp";
   IEPolyModel *model = hmc->BuildExportModel(&m_iemp, modelmapfilename.Get());
   if (model==NULL) {
      ReportExportError(cdlg.m_filename);
      return FALSE;
   }

   BOOL result = ExportTUP(model, cdlg.m_filename);
   DeleteIEModel(model);

   if (!result)
      ReportExportError(cdlg.m_filename);

   return result;
}

BOOL CHxtApp::OnExportActionCache(HActionCache *hac)
{
   HModelCache *hmc = hac->GetDefaultModelCache();

   if (hmc==NULL) {
      CString msg, title;
      msg.LoadString(IDS_ACTIONHASNOMODELASSIGNED);
      String temp = (LPCTSTR)msg;
      temp.Replace("%1", hac->GetName());
      msg = temp.Get();
      title.LoadString(IDS_EXPORTTITLE);

      MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
      return FALSE;
   }

   CExportModelDialog cdlg(hmc->GetName());
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   HProject *hp = GetHProject();
   float fps = hp->GetFPS();
   Time totaltime = hac->GetTotalTime();
   String basefilename = cdlg.m_filename;

   HModel *hm = hac->EditWithModelCache(hmc);

   for (Time time; time<=totaltime; time.StepFrame(fps)) {
      int frame = time.GetFrameInt(fps);
      String filename = basefilename.NumberFrame(frame);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPolyModel *model = hm->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         hac->EndEditWithModelCache(hm);
         return FALSE;
      }

      BOOL result = ExportTUP(model, filename);
      DeleteIEModel(model);

      if (!result) {
         ReportExportError(filename);
         hac->EndEditWithModelCache(hm);
         return FALSE;
      }

   }
   hac->EndEditWithModelCache(hm);
   return TRUE;
}

BOOL CHxtApp::OnExportObjectCacheContainer(HObjectCacheContainer *hocc)
{
   CExportModelsDialog cdlg(hocc);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   for (POSITION pos = cdlg.m_selected.GetHeadPosition(); pos; ) {
      HModelCache *hmc = cdlg.m_selected.GetNext(pos);
      String filename = hmc->GetName();
      filename = filename.GetNameOnlyWithoutSuffix() + ".TUP";
      filename = CombineNameToPath(cdlg.m_dir, filename);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPolyModel *model = hmc->BuildExportModel(&m_iemp, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         return FALSE;
      }
      BOOL result = ExportTUP(model, filename);
      DeleteIEModel(model);
      if (!result) {
         ReportExportError(filename);
         return FALSE;
      }
   }
   return TRUE;
}

BOOL CHxtApp::OnExportActionCacheContainer(HActionCacheContainer *hacc)
{
   CExportActionsDialog cdlg(hacc);
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   HProject *hp = GetHProject();
   float fps = hp->GetFPS();

   for (POSITION pos = cdlg.m_selected.GetHeadPosition(); pos; ) {
      HActionCache *hac = cdlg.m_selected.GetNext(pos);
      HModelCache *hmc = hac->GetDefaultModelCache();

      if (hmc==NULL) {
         CString msg, title;
         msg.LoadString(IDS_ACTIONHASNOMODELASSIGNED);
         String temp = (LPCTSTR)msg;
         temp.Replace("%1", hac->GetName());
         msg = temp.Get();
         title.LoadString(IDS_EXPORTTITLE);
         MessageBox(GetMainApplicationWnd(), msg, title, MB_OK);
         return FALSE;
      }

      Time totaltime = hac->GetTotalTime();
      String basefilename = hac->GetName();
      basefilename = basefilename.GetNameOnlyWithoutSuffix() + "-0000.TUP";
      basefilename = CombineNameToPath(cdlg.m_dir, basefilename);

      HModel *hm = hac->EditWithModelCache(hmc);

      for (Time time; time<=totaltime; time.StepFrame(fps)) {
         int frame = time.GetFrameInt(fps);
         String filename = basefilename.NumberFrame(frame);
         String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
         IEPolyModel *model = hm->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
         if (model==NULL) {
            ReportExportError(filename);
            hac->EndEditWithModelCache(hm);
            return FALSE;
         }

         BOOL result = ExportTUP(model, filename);
         DeleteIEModel(model);

         if (!result) {
            ReportExportError(filename);
            hac->EndEditWithModelCache(hm);
            return FALSE;
         }

      }
      hac->EndEditWithModelCache(hm);
   }
   return TRUE;
}

BOOL CHxtApp::OnExportChor(HChor *hchor)
{
   CExportModelDialog cdlg(hchor->GetName());
   if (cdlg.DoModal()!=IDOK)
      return TRUE;

   CWaitCursor wait;
   m_iemp.m_isbuildmodelmap = cdlg.m_generatemodelmaps;
   m_iemp.m_minmodelmapsize = (MapLimit)cdlg.m_minsize;
   m_iemp.m_maxmodelmapsize = (MapLimit)cdlg.m_maxsize;
   m_iemp.m_subtype = (SubType)cdlg.m_subtype;

   HProject *hp = GetHProject();
   float fps = hp->GetFPS();
   HTimeProperty *totaltime = hchor->GetTotalTime();
   String basefilename = cdlg.m_filename;

   for (Time time; time<=totaltime->GetValue(); time.StepFrame(fps)) {
      int frame = time.GetFrameInt(fps);
      String filename = basefilename.NumberFrame(frame);
      String modelmapfilename = filename.GetWithoutSuffix() + ".bmp";
      IEPolyModel *model = hchor->BuildExportModel(&m_iemp, time, modelmapfilename.Get());
      if (model==NULL) {
         ReportExportError(filename);
         return FALSE;
      }

      BOOL result = ExportTUP(model, filename);
      DeleteIEModel(model);

      if (!result) {
         ReportExportError(filename);
         return FALSE;
      }
   }
   return TRUE;
}

void CHxtApp::ReportExportError(const String &filename)
{
   CString caption;
   caption.LoadString(IDS_EXPORTTITLE);
   CString text;
   text.LoadString(IDS_ERROREXPORTINGMODEL);
   String temp = (LPCTSTR)text;
   temp.Replace("%1", filename);
   text = temp.Get();
   MessageBox(GetMainApplicationWnd(), text, caption, MB_OK); 
}
 
static void CountBoneBranch(IEBone *bone, LONG *count)
{
   for (;bone;bone=bone->m_sibling) {
      (*count)++;
      if (bone->m_child)
         CountBoneBranch(bone->m_child, count);
   }  
}

LONG CHxtApp::CountBones(IEPolyModel *model)
{
   LONG count=0;
   CountBoneBranch(model->m_bonehead, &count);
   return count;
}

BOOL CHxtApp::ExportTUP(IEPolyModel *model, const String &filename)
{

	LONG totalitems, curritem = 0;
	
	String oldpath = GetBasePath();
	SetBasePath(filename); // used to make texturemap filenames relative to model exported location
	
	m_hprogressbar = GetProgressBar();
	if (m_hprogressbar)
	{
		m_hprogressbar->Initialize();
		CString text;
		text.LoadString(IDS_EXPORTTITLE);
		m_hprogressbar->SetMessage1((LPCTSTR)text);
		m_hprogressbar->SetMessage2((LPCTSTR)filename.GetNameOnly().Get());
	}
	
	// Get total items for percent bar
	totalitems = model->m_attrlist.GetSize() +
		model->m_maplist.GetSize() +
		model->m_vertexlist.GetSize() +
		model->m_normallist.GetSize() +
		model->m_polyarray.GetSize() + CountBones(model);
 
	// create a new scene
	m_pSceneAgg = new TupperwareAggregate;
	m_pTupScene = new TupScene(m_pSceneAgg);

	// create the node pool
	TupperwareAggregate *pNodePoolAgg = m_pTupScene->AddNodePool();
	m_pTupNodePool = new TupNodePool(pNodePoolAgg);

	// create the object pool
	TupperwareAggregate *pObjectPoolAgg = m_pTupScene->AddObjectPool();
	m_pTupObjectPool = new TupObjectPool(pObjectPoolAgg);

	// create the image pool
	TupperwareAggregate *pMaterialPoolAgg = m_pTupScene->AddMaterialPool();
	if (pMaterialPoolAgg)
		m_pTupMaterialPool = new TupMaterialPool(pMaterialPoolAgg);

	// create the image pool
	TupperwareAggregate *pImagePoolAgg = m_pTupScene->AddImagePool();
	if (pImagePoolAgg)
		m_pTupImagePool = new TupImagePool(pImagePoolAgg);

	// add in the date, time and version
	m_pTupScene->AddExporterDate(g_pExporterDate);
	m_pTupScene->AddExporterTime(g_pExporterTime);
	m_pTupScene->AddVersion(TupScene::GetSceneVersion());
	m_pTupScene->AddUnitFlags(TupScene::TIME_UNIT_SECONDS | TupScene::LINEAR_UNIT_METER);
	TupString sourceFileName = m_modelname = filename.GetNameOnlyWithoutSuffix().buf;
	m_pTupScene->AddSourceFileName(sourceFileName);

	int m_rootNodeRef = 0;
	TupperwareAggregate *pTupRootNodeAgg = m_pTupNodePool->AddNode(m_rootNodeRef);
	m_pTupScene->AddRootNodeReference(m_rootNodeRef);

	TupNode tupRootNode(pTupRootNodeAgg);
	tupRootNode.AddName("world");
	tupRootNode.AddParentNodeReference(-1); // root has -1 as its parent reference
	tupRootNode.AddChildNodeReference(1);

	TupperwareAggregate *pModelNodeAgg = m_pTupNodePool->AddNode(1);
	TupNode tupNode(pModelNodeAgg);
	tupNode.AddName(filename.GetNameOnlyWithoutSuffix().buf);
	tupNode.AddParentNodeReference(0);
	tupNode.AddTransform();

	int objectRef = m_pTupObjectPool->AddObjectAggregate();
	TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(objectRef);
	tupNode.AddObjectReference(objectRef);

	ExportImages(model);

	ExportModel(filename.GetNameOnlyWithoutSuffix().buf,pObjectAgg,model);

	// save the tree
	TupperwareLoadSaveManager::FormatType formatType = TupperwareLoadSaveManager::ASCII;
	int result = TupperwareLoadSaveManager::Write(formatType,filename.Get(),m_pSceneAgg);
	if (!result)
		return FALSE;

	delete m_pSceneAgg;
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupObjectPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;

	if (m_hprogressbar)
	{
		m_hprogressbar->Uninitialize();
		m_hprogressbar = NULL;
	}
	SetBasePath(oldpath);

   return TRUE;
}

//#define EXPORTTRIMESH

int CHxtApp::ExportImages(IEPolyModel *model)
{

	int numMaps = model->m_maplist.GetSize();
	for (int i = 0; i < numMaps; i++)
	{
		IEMap *map = &model->m_maplist[i];

		int refindex = m_pTupImagePool->AddImage();
		TupperwareAggregate *pImageAgg = m_pTupImagePool->GetImage(refindex);

		TupImage tupImage(pImageAgg);
		tupImage.AddFilePathName(map->m_filename);
	}

	return 1;
}

int CHxtApp::ExportModel(const char *mayaname,TupperwareAggregate *pObjectAgg,IEPolyModel *model)
{
	int i;

	pObjectAgg->AddScalarString("MayaObjectName",NULL,mayaname);
	TupObject tupObject(pObjectAgg);

#ifdef EXPORTTRIMESH
	tupObject.SetObjectType(TupObject::TRI_MESH);
#else
	tupObject.SetObjectType(TupObject::POLY_MESH);
#endif


	TupArray<Vector> m_positionVertices;
	TupArray<Vector> m_normalVertices;
	
	TupArray<int> m_positionIndicies;
	TupArray<int> m_normalIndices;


#ifdef EXPORTTRIMESH	
	TupTriMesh tupTriMesh(pObjectAgg);
#else
	TupPolyMesh tupPolyMesh(pObjectAgg);
#endif
	
	// do positions	
	int numPositionVerts = model->m_vertexlist.GetSize();
	float *pPositionVerts = new float[numPositionVerts*3];
	Vector *pArray = (Vector*)pPositionVerts;
	
	for (i = 0; i < numPositionVerts; i++)
	{
		pArray[i] = model->m_vertexlist[i];
		pArray[i] *= 0.01f;
	}

#ifdef EXPORTTRIMESH	
	tupTriMesh.AddPositionVerts(pPositionVerts,numPositionVerts);
#else	
	tupPolyMesh.AddPositionVerts(pPositionVerts,numPositionVerts);
#endif

	// do normals	
	int numNormalVerts = model->m_normallist.GetSize();
	float *pNormalVerts = new float[numNormalVerts*3];
	pArray = (Vector*)pNormalVerts;
	
	for (i = 0; i < numNormalVerts; i++)
	{
		pArray[i] = model->m_normallist[i];
		pArray[i].Normalize();
	}

#ifdef EXPORTTRIMESH	
	tupTriMesh.AddNormalVerts(pNormalVerts,numNormalVerts);
#else	
	tupPolyMesh.AddNormalVerts(pNormalVerts,numNormalVerts);
#endif

	int numFaces = model->m_polyarray.GetSize();
	int numTriFaces = 0;
	int count = 0,tricount = 0;
	// count indicies
	for (i = 0; i < numFaces; i++)
	{
		IEPoly *poly = model->m_polyarray[i];
		if (poly->m_vertexid[0] == poly->m_vertexid[3])
		{
			count += 3;
			numTriFaces += 1;
			tricount += 3;
		}
		else
		{
			count += 4;//ngon
			numTriFaces += 2;
			tricount += 6;
		}
	}
#ifdef EXPORTTRIMESH	
	int *pPositionIndices = new int[tricount];
	int *pNormalIndices = new int[tricount];
#else
	int *pPositionIndices = new int[count];
	int *pNormalIndices = new int[count];
#endif
	
#ifndef EXPORTTRIMESH
	int *pFaceVertexCounts = new int[numFaces];
	int *pFaceTriangleCountsList = new int[numTriFaces];
	int *pTrianglePointIndicesList = new int[tricount];
#endif
	
	count = 0; tricount = 0;
	int triangle = 0;
	for (i = 0; i < numFaces; i++)
	{
		IEPoly *poly = model->m_polyarray[i];

		int nummapnodes = poly->m_mapnodearray.GetSize();

		// grab map on end of list
		IEMapNode *mapnode = poly->m_mapnodearray[nummapnodes-1];


		if (poly->m_vertexid[0] == poly->m_vertexid[3])
		{
			pPositionIndices[count] = poly->m_vertexid[0];
			pPositionIndices[count+1] = poly->m_vertexid[1];
			pPositionIndices[count+2] = poly->m_vertexid[2];

			pNormalIndices[count] = poly->m_normalid[0];
			pNormalIndices[count+1] = poly->m_normalid[1];
			pNormalIndices[count+2] = poly->m_normalid[2];

#ifndef EXPORTTRIMESH	
			pFaceVertexCounts[i] = 3;

			pFaceTriangleCountsList[triangle] = 3;

			pTrianglePointIndicesList[tricount] = poly->m_vertexid[0];
			pTrianglePointIndicesList[tricount+1] = poly->m_vertexid[1];
			pTrianglePointIndicesList[tricount+2] = poly->m_vertexid[2];

			triangle += 1;

			tricount += 3;
#endif

			count += 3;
		}
#ifdef EXPORTTRIMESH	
		else // break quads into triangles
		{
			pPositionIndices[count] = poly->m_vertexid[0];
			pPositionIndices[count+1] = poly->m_vertexid[1];
			pPositionIndices[count+2] = poly->m_vertexid[2];

			pPositionIndices[count+3] = poly->m_vertexid[0];
			pPositionIndices[count+4] = poly->m_vertexid[2];
			pPositionIndices[count+5] = poly->m_vertexid[3];

			pNormalIndices[count] = poly->m_normalid[0];
			pNormalIndices[count+1] = poly->m_normalid[1];
			pNormalIndices[count+2] = poly->m_normalid[2];
			
			pNormalIndices[count+3] = poly->m_normalid[0];
			pNormalIndices[count+4] = poly->m_normalid[2];
			pNormalIndices[count+5] = poly->m_normalid[3];

			count += 6;			
		}
#else
		else // put out quad as is
		{
			pPositionIndices[count] = poly->m_vertexid[0];
			pPositionIndices[count+1] = poly->m_vertexid[1];
			pPositionIndices[count+2] = poly->m_vertexid[2];
			pPositionIndices[count+3] = poly->m_vertexid[3];

			pNormalIndices[count] = poly->m_normalid[0];
			pNormalIndices[count+1] = poly->m_normalid[1];
			pNormalIndices[count+2] = poly->m_normalid[2];
			pNormalIndices[count+3] = poly->m_normalid[3];

			pFaceVertexCounts[i] = 4;

			pFaceTriangleCountsList[triangle] = 3;
			pFaceTriangleCountsList[triangle+1] = 3;

			pTrianglePointIndicesList[tricount] = poly->m_vertexid[0];
			pTrianglePointIndicesList[tricount+1] = poly->m_vertexid[1];
			pTrianglePointIndicesList[tricount+2] = poly->m_vertexid[2];

			pTrianglePointIndicesList[tricount+3] = poly->m_vertexid[0];
			pTrianglePointIndicesList[tricount+4] = poly->m_vertexid[2];
			pTrianglePointIndicesList[tricount+5] = poly->m_vertexid[3];

			triangle += 2;
			count += 4;
			tricount += 6;
		}
#endif

		KeeperIEMapNode keepermapnode(mapnode);
		m_mapnodes.Add(keepermapnode);
	}

#ifdef EXPORTTRIMESH		
	tupTriMesh.AddPositionIndices(pPositionIndices,numTriFaces);
	tupTriMesh.AddNormalIndices(pNormalIndices,numTriFaces);
#else
	tupPolyMesh.AddPositionIndices(pPositionIndices,numFaces);
	tupPolyMesh.AddNormalIndices(pNormalIndices,numFaces);
	tupPolyMesh.AddFaceVertexCounts(pFaceVertexCounts,numFaces);
#endif

#ifndef EXPORTTRIMESH
//	TupperwareAggregate *pMeshMapAgg = tupPolyMesh.AddMeshMap(0);
//	TupPolyMeshMap tupPolyMeshMap(pMeshMapAgg);
//	TupVertex3 *pMapVerts;
//	float *p
//	tupPolyMeshMap.AddMapVerts((floatpMapVerts,count);
#endif

	// create sub material for each mapnode in keeper
	int refindex = m_pTupMaterialPool->AddMaterial();
	TupperwareAggregate *pMaterialAgg = m_pTupMaterialPool->GetMaterial(refindex);
	TupMultiSubObjectMaterial tupMultiSubObjectMaterial(pMaterialAgg);
	
	tupMultiSubObjectMaterial.AddName("MultiSub");
	
	for (i = 0; i < m_mapnodes.GetSize(); i++)
	{
		KeeperIEMapNode *mapnode = &m_mapnodes[i];

		TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.AddSubMaterial(i);
		TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
		
		refindex = m_pTupMaterialPool->AddMaterial();
		tupSubMaterial.AddMaterialReference(refindex);
		TupperwareAggregate *materialReferenceAgg = m_pTupMaterialPool->GetMaterial(refindex);
		
		TupStandardMaterial tupStandardMaterial(materialReferenceAgg);
		
		refindex = m_pTupMapPool->AddMap();
		
		TupperwareAggregate *pMapAgg = tupStandardMaterial.GetMap(refindex);
		
		TupMap tupMap(pMapAgg);
		tupMap.SetMapType(TupMap::BITMAP_MAP);
		TupBitmapMap bitmapMap(pMapAgg);
		bitmapMap.AddMapChannel(0);
		
		float diffuseColor[] = {1.0f,1.0f,1.0f};
		tupStandardMaterial.AddDiffuseColor((float *)&diffuseColor);
		
		bitmapMap.AddImageReference(mapnode->m_mapid);
	}

	delete pPositionVerts;
	delete pNormalVerts;
	delete pPositionIndices;
	delete pNormalIndices;
#ifndef EXPORTTRIMESH
	delete pFaceVertexCounts;
	delete pFaceTriangleCountsList;
	delete pTrianglePointIndicesList;
#endif
	
	return 1;
}

void CHxtApp::SaveAttributes(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_attrlist.GetSize();
   fs << count << nl;
   Attr *attrarray = model->m_attrlist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      fs << (BYTE)(attrarray[i].m_diffusecolor.red*255.0f) << nl;
      fs << (BYTE)(attrarray[i].m_diffusecolor.green*255.0f) << nl;
      fs << (BYTE)(attrarray[i].m_diffusecolor.blue*255.0f) << nl;
      
      fs << attrarray[i].m_diffusefalloff << nl;
      
      fs << (BYTE)(attrarray[i].m_specularcolor.red*255.0f) << nl;
      fs << (BYTE)(attrarray[i].m_specularcolor.green*255.0f) << nl;
      fs << (BYTE)(attrarray[i].m_specularcolor.blue*255.0f) << nl;
      
      fs << attrarray[i].m_specularsize << nl;
      fs << attrarray[i].m_specularintensity << nl;
      fs << attrarray[i].m_ambiance << nl;
      fs << attrarray[i].m_roughness << nl;
      fs << attrarray[i].m_roughnessscale << nl;
      fs << attrarray[i].m_reflectivity << nl;
      fs << attrarray[i].m_transparency << nl;
      fs << attrarray[i].m_refraction << nl;
   }
}

void CHxtApp::SaveTextureMaps(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_maplist.GetSize();
   fs << count << nl;
   IEMap *maparray = model->m_maplist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      String mapname = maparray[i].m_filename;
      mapname = mapname.GetRelativePath();
      fs.Write(mapname.Get(), mapname.length());
      fs << '\0';
      fs << maparray[i].m_width << sp << maparray[i].m_height << nl;
   }
}

void CHxtApp::SaveVertexList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_vertexlist.GetSize();
   fs << count << nl;
   Vector *vectorarray = model->m_vertexlist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      fs << vectorarray[i] << nl;
   }
}

void CHxtApp::SaveNormalList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_normallist.GetSize();
   fs << count << nl;
   Vector *vectorarray = model->m_normallist.GetArray();
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      fs << vectorarray[i] << nl;
   }
}

void CHxtApp::SavePolys(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   int count = model->m_polyarray.GetSize();
   fs << count << nl;
   IEPoly **polyarray = model->m_polyarray.GetArray();
   for (int i=0; i < count; i++) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      fs << polyarray[i]->m_attrid << nl;
      int c;
      for (c=0; c < 4; c++)
         fs << polyarray[i]->m_vertexid[c] << nl;
      for (c=0; c < 4; c++)
         fs << polyarray[i]->m_normalid[c] << nl;
      int mapnodecount = polyarray[i]->m_mapnodearray.GetSize();
      IEMapNode **mapnodearray = polyarray[i]->m_mapnodearray.GetArray();
      fs << mapnodecount << nl;
      for (int mi=0; mi < mapnodecount; mi++) {
         fs << mapnodearray[mi]->m_mapid << nl;
         fs << mapnodearray[mi]->m_percent << nl;
         fs << mapnodearray[mi]->m_repeat << nl;
         fs << (BYTE)mapnodearray[mi]->m_kind << nl;
         fs << (BYTE)mapnodearray[mi]->m_isseamless << nl;
         
         for (c=0; c < 4; c++)
            fs << mapnodearray[mi]->m_uv[c] << nl;
      }
   }
}

void CHxtApp::SaveBoneBranch(FileStream &fs, IEBone *bone, LONG &totalitems, LONG &curritem)
{
   for (; bone; bone = bone->m_sibling) {
      if (m_hprogressbar)
         m_hprogressbar->SetProgress((int)((float)curritem++/totalitems*100));
      if (bone->m_child)
         fs << (BYTE)1 << nl;
      else
         fs << (BYTE)0 << nl;
      if (bone->m_sibling)
         fs << (BYTE)1 << nl;
      else
         fs << (BYTE)0 << nl;

      fs.Write( bone->m_name, strlen(bone->m_name));
      fs << '\0';
      
      fs << bone->m_pivotpos << nl;
      fs << bone->m_endpos << nl;
      fs << bone->m_rollpos << nl;
      
      int count = bone->m_vertexidarray.GetSize();
      fs << count << nl;
      LONG *array = bone->m_vertexidarray.GetArray();
      for (int i=0; i < count; i++)
         fs << array[i] << nl;

      if (bone->m_child)      
         SaveBoneBranch( fs, bone->m_child, totalitems, curritem );
   }
}

void CHxtApp::SaveBones(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem)
{
   SaveBoneBranch(fs, model->m_bonehead, totalitems, curritem);
}
