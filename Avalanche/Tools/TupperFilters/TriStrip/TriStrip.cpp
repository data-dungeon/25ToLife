////////////////////////////////////////////////////////////////////////////
//
// TriStrip
//
// Class to handle the TriStrip Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TriStripPch.h"
#include "TriStrip.h"
#include "StripBuilder.h"
#include "parts.h"

int TriStripFilter::GetTotalPasses(void)	const
{
	return 1;
}

void TriStripFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	// set up log
	PluginLog::SetLog(&m_log);

	// default options
	m_nBonesPerStrip = 0;
	m_bAllowSwaps = true;
   m_bOptimizeVertexCache = false;
	m_nMaxStripPixels = 0;
	m_normalThreshold = 0.001f;
	m_mapThreshold = 0.001f;
	m_vertexColorThreshold = 0.01f;
	m_vertexAlphaThreshold = 0.01f;
	m_bStripStageFaces = true;
   m_nVertexCacheSize = 20;
   m_nVertexCacheStart = 13;

	CheckOptions(options);

	m_pTupDataManager = &dataManager;

	m_pSceneAgg = dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);

	m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(m_pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool( m_pObjectPoolAgg );

	m_pMaterialPoolAgg = m_pTupScene->GetMaterialPool();
	assert(m_pMaterialPoolAgg);
	m_pTupMaterialPool = new TupMaterialPool( m_pMaterialPoolAgg );

	m_pMapPoolAgg = m_pTupScene->GetMapPool();
	assert(m_pMapPoolAgg);
	m_pTupMapPool = new TupMapPool( m_pMapPoolAgg );

	m_pImagePoolAgg = m_pTupScene->GetImagePool();
	assert(m_pImagePoolAgg);
	m_pTupImagePool = new TupImagePool( m_pImagePoolAgg );


	m_totalStrips = 0;
	m_totalFaces = 0;
	m_totalPoints = 0;
	m_totalSwaps = 0;

	m_meshStripStats.RemoveAll();

	try
	{
		PluginLog::SetProgressText("TriStripping Meshes");

		int total = m_pTupObjectPool->GetNumObjects();
		for (int i=0;i<total;i++)
		{
			if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

			TupperwareAggregate *pObjectAgg = m_pTupObjectPool->GetObjectAggregate(i);

			// if slot empty then continue
			if (!pObjectAgg)
				continue;

			TupObject tupObject(pObjectAgg);
			if (tupObject.GetObjectType()==TupObject::TRI_MESH)
			{
//				PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Tri Stripping Mesh %d of %d",i+1,total);
				TupTriMesh tupTriMesh(tupObject);
				try
				{
					TriStripTriMesh(tupTriMesh);
				}
				catch(PluginException e)
				{
					TupString name = "None";
					TupperwareScalar *pMayaObjectNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
					if (pMayaObjectNameScalar)
					{
						name = pMayaObjectNameScalar->GetAsString();
					}
					throw(PluginException(ERROR_FATAL,"TriStripTriMesh - Mesh %d %s\r\n  %s",i,(const char *)name,(const char *)(e)));
				}
				catch (...)
				{
					TupString name = "None";
					TupperwareScalar *pMayaObjectNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
					if (pMayaObjectNameScalar)
					{
						name = pMayaObjectNameScalar->GetAsString();
					}
					throw(PluginException(ERROR_FATAL,"TriStripTriMesh - Mesh %d %s - unknown exception",i,(const char *)name));
				}
			}
			PluginLog::SetProgressBar(1,total,i+1);
		}

		AddSortedStripsToLog();

		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED|LogFlags::FONT_STYLE_BOLD,"TriStrip Statistics:");
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Total Strips = %d",m_totalStrips);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Total Faces = %d",m_totalFaces);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Total Points = %d",m_totalPoints);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Total Swaps = %d",m_totalSwaps);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Average Triangles Per Strip = %f",((float)m_totalFaces)/m_totalStrips);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"  Average Points Per Face = %f",((float)m_totalPoints)/m_totalFaces);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"TriStrip - Process\r\n  %s",(const char *)(e)));
	}
	catch (...)
	{
		throw(PluginException(ERROR_FATAL,"TriStrip - Process - Unknown Exception"));
	}

	delete m_pTupScene;
	delete m_pTupObjectPool;
	delete m_pTupMaterialPool;
	delete m_pTupMapPool;
	delete m_pTupImagePool;
}

char *TriStripFilter::m_writeDependencyNames[NUM_WRITE_DEPENDENCIES] = 
{
		"TriStrip_TriStripData", // TRI_STRIPS
};

char *TriStripFilter::m_writeDependencyNamesOptions[NUM_OPTIONAL_WRITE_DEPENDENCIES] = 
{
		"TriStrip_BonesPerStrip = ", // BONES_PER_STRIP
		"TriStrip_NoSwaps", // NO_SWAPS
		"TriStrip_MaxStripPixels = ", // MAX_STRIP_PIXELS
		"TriStrip_StripStageFaces = ", // STRIP_STAGE_FACES
      "TriStrip_OptimizeVertexCache = ", // OPTIMIZE_VERTEX_CACHE
};

char *TriStripFilter::m_writeDependencyOptionNames[NUM_OPTIONAL_WRITE_DEPENDENCIES] = 
{
		"BonesPerStrip", // BONES_PER_STRIP
		"NoSwaps", // NO_SWAPS
		"MaxStripPixels", // MAX_STRIP_PIXELS
		"StripStageFaces", // STRIP_STAGE_FACES
      "OptimizeVertexCache", // OPTIMIZE_VERTEX_CACHE
};

Revision TriStripFilter::m_writeDependencyRevisions[NUM_WRITE_DEPENDENCIES] = 
{
	Revision(1,0), // TRI_STRIPS
};

Revision TriStripFilter::m_writeDependencyRevisionsOptions[NUM_OPTIONAL_WRITE_DEPENDENCIES] = 
{
	Revision(1,0), // BONES_PER_STRIP
	Revision(1,0), // NO_SWAPS
	Revision(1,0), // MAX_STRIP_PIXELS
	Revision(1,0), // STRIP_STAGE_FACES
   Revision(1,0), // OPTIMIZE_VERTEX_CACHE
};

int TriStripFilter::GetTotalWriteDependencies(int pass,const FilterOptions &options) const
{
	int totalOptional = 0;
	const char *pValue;

	// bones per strip
	pValue = options.GetValue(m_writeDependencyOptionNames[BONES_PER_STRIP]);
	int nBonesPerStrip = 0;
	if (pValue)
		nBonesPerStrip = atoi(pValue);
	if (nBonesPerStrip>0)
	{
		totalOptional++;
	}

	// no swaps
	pValue = options.GetValue(m_writeDependencyOptionNames[NO_SWAPS]);
	if (pValue && !stricmp(pValue,"true"))
	{
		totalOptional++;
	}

   // optimize vertex cache
   pValue = options.GetValue(m_writeDependencyOptionNames[OPTIMIZE_VERTEX_CACHE]);
   if (pValue && !stricmp(pValue,"true"))
   {
      totalOptional++;
   }

	// max strip pixels
	pValue = options.GetValue(m_writeDependencyOptionNames[MAX_STRIP_PIXELS]);
	int nMaxStripPixels = 0;
	if (pValue)
		nMaxStripPixels = atoi(pValue);
	if (nMaxStripPixels>0)
	{
		totalOptional++;
	}

	// strip stage faces
	pValue = options.GetValue(m_writeDependencyOptionNames[STRIP_STAGE_FACES]);
	if (pValue)
	{
		totalOptional++;
	}

	return totalOptional+NUM_WRITE_DEPENDENCIES;
}

const char *TriStripFilter::GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const
{
	static char tempKey[80];
	int numDependencies = GetTotalWriteDependencies(pass,options);
	assert(index>=0 && index<numDependencies);
	// static ones (do not depend on options)
	if (index<NUM_WRITE_DEPENDENCIES)
	{
		return m_writeDependencyNames[index];
	}
	// ones that depend on options
	else
	{
		int offset = index-NUM_WRITE_DEPENDENCIES;
		int index = GetWriteDependencyOption(options,offset);
		switch (index)
		{
			case BONES_PER_STRIP:
				{
					const char *pValue = options.GetValue(m_writeDependencyOptionNames[BONES_PER_STRIP]);
					assert(pValue);
					int nBonesPerStrip = atoi(pValue);
					assert(nBonesPerStrip>0);
					sprintf(tempKey,"%s%d",m_writeDependencyNamesOptions[BONES_PER_STRIP],nBonesPerStrip);
					return tempKey;
				}
				break;
			case MAX_STRIP_PIXELS:
				{
					const char *pValue = options.GetValue(m_writeDependencyOptionNames[MAX_STRIP_PIXELS]);
					assert(pValue);
					int nMaxStripPixels = atoi(pValue);
					assert(nMaxStripPixels>0);
					sprintf(tempKey,"%s%d",m_writeDependencyNamesOptions[MAX_STRIP_PIXELS],nMaxStripPixels);
					return tempKey;
				}
				break;
			case STRIP_STAGE_FACES:
				{
					const char *pValue = options.GetValue(m_writeDependencyOptionNames[STRIP_STAGE_FACES]);
					assert(pValue);
					sprintf(tempKey,"%s%s",m_writeDependencyNamesOptions[STRIP_STAGE_FACES],pValue);
					return tempKey;
				}
			default:
				{
					return m_writeDependencyNamesOptions[index];
				}
				break;
		}
	}
}

Revision TriStripFilter::GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	int numDependencies = GetTotalWriteDependencies(pass,options);
	assert(index>=0 && index<numDependencies);
	// static ones (do not depend on options)
	if (index<NUM_WRITE_DEPENDENCIES)
	{
		return m_writeDependencyRevisions[index];
	}
	// ones that depend on options
	else
	{
		int offset = index-NUM_WRITE_DEPENDENCIES;
		int index = GetWriteDependencyOption(options,offset);
		return m_writeDependencyRevisionsOptions[index];
	}
}

char *TriStripFilter::m_readDependencyNames[NUM_READ_DEPENDENCIES] = 
{
		"Annotate_BonesToMesh", // BONES_TO_MESH
		"Annotate_ObjectSOMStarts", // OBJECT_SOM_STARTS
		"Annotate_AddVertexColorRefsToMesh", // ADD_VERTEX_COLOR_REFS_TO_MESH
};

Revision TriStripFilter::m_readDependencyRevisions[NUM_READ_DEPENDENCIES] = 
{
	Revision(1,0), // BONES_TO_MESH
	Revision(1,0), // OBJECT_SOM_STARTS
	Revision(1,0), // ADD_VERTEX_COLOR_REFS_TO_MESH
};

int TriStripFilter::GetTotalReadDependencies(int pass,const FilterOptions &options) const
{
	return NUM_READ_DEPENDENCIES;
}

const char *TriStripFilter::GetReadDependencyKey(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyNames[index];
}

Revision TriStripFilter::GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	assert(index>=0 && index<NUM_READ_DEPENDENCIES);
	return m_readDependencyRevisions[index];
}

int TriStripFilter::GetWriteDependencyOption(const FilterOptions &options,int offset) const
{
	const char *pValue;
	// bones per strip
	pValue = options.GetValue(m_writeDependencyOptionNames[BONES_PER_STRIP]);
	int nBonesPerStrip = 0;
	if (pValue)
		nBonesPerStrip = atoi(pValue);
	if (nBonesPerStrip>0)
	{
		if (offset==0) 
		{
			return BONES_PER_STRIP;
		}
		offset--;
	}
	// no swaps
	pValue = options.GetValue(m_writeDependencyOptionNames[NO_SWAPS]);
	if (pValue && !stricmp(pValue,"true"))
	{
		if (offset==0)
		{
			return NO_SWAPS;
		}
		offset--;
	}
   // optimize vertex cache
   pValue = options.GetValue(m_writeDependencyOptionNames[OPTIMIZE_VERTEX_CACHE]);
   if (pValue && !stricmp(pValue,"true"))
   {
      if (offset==0)
      {
         return OPTIMIZE_VERTEX_CACHE;
      }
      offset--;
   }

	pValue = options.GetValue(m_writeDependencyOptionNames[MAX_STRIP_PIXELS]);
	int nMaxStripPixels = 0;
	if (pValue)
		nMaxStripPixels = atoi(pValue);
	if (nMaxStripPixels>0)
	{
		if (offset==0) 
		{
			return MAX_STRIP_PIXELS;
		}
		offset--;
	}
	pValue = options.GetValue(m_writeDependencyOptionNames[STRIP_STAGE_FACES]);
	if (pValue)
	{
		if (offset==0)
		{
			return STRIP_STAGE_FACES;
		}
		offset--;
	}
	assert(false);
	return 0;
}

void TriStripFilter::CheckOptions(const FilterOptions &options)
{
	const char *pValue;
	// bones per strip
	pValue = options.GetValue(m_writeDependencyOptionNames[BONES_PER_STRIP]);
	if (pValue)
	{
		m_nBonesPerStrip = atoi(pValue);
		if (m_nBonesPerStrip>0&&m_nBonesPerStrip<4)
			throw(PluginException(ERROR_FATAL,"Bones per strip needs to be 0 or greater than 3."));
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Bones per strip set to %d.",m_nBonesPerStrip);
	}

	// no swaps
	pValue = options.GetValue(m_writeDependencyOptionNames[NO_SWAPS]);
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bAllowSwaps = false;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Swaps disabled for Tristrips.");
	}

   // no swaps
   pValue = options.GetValue(m_writeDependencyOptionNames[OPTIMIZE_VERTEX_CACHE]);
   if (pValue && !stricmp(pValue,"true"))
   {
      m_bOptimizeVertexCache = true;
      PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Optimizing for Vertex Cache.");
   }

	// max strip pixels
	pValue = options.GetValue(m_writeDependencyOptionNames[MAX_STRIP_PIXELS]);
	if (pValue)
	{
		m_nMaxStripPixels = atoi(pValue);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Max strip pixels to %d",m_nMaxStripPixels);
	}

	// strip stage faces
	pValue = options.GetValue(m_writeDependencyOptionNames[STRIP_STAGE_FACES]);
	if (pValue)
	{
		if (!stricmp(pValue,"true"))
		{
			m_bStripStageFaces = true;
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Tristripping staged faces.");
		}
		else
		{
			m_bStripStageFaces = false;
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Not tristripping staged faces.");
		}
	}
	

	// normal threshold
	pValue = options.GetValue("NormalThreshold");
	if (pValue)
	{
		m_normalThreshold = (float) atof(pValue);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Normal Threshold set to %f",m_normalThreshold);
	}

	// map threshold
	pValue = options.GetValue("MapThreshold");
	if (pValue)
	{
		m_mapThreshold = (float) atof(pValue);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Map Threshold set to %f",m_mapThreshold);
	}

	// vertex color threshold
	pValue = options.GetValue("VertexColorThreshold");
	if (pValue)
	{
		m_vertexColorThreshold = (float) atof(pValue);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Vertex Color Threshold set to %f",m_vertexColorThreshold);
	}

	// vertex alpha threshold
	pValue = options.GetValue("VertexAlphaThreshold");
	if (pValue)
	{
		m_vertexAlphaThreshold = (float) atof(pValue);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Vertex Alpha Threshold set to %f",m_vertexAlphaThreshold);
	}

   pValue = options.GetValue("VertexCacheSize");
   if (pValue)
   {
      m_nVertexCacheSize = atoi(pValue);
      PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Vertex Cache Size set to %d",m_nVertexCacheSize);
   }

   pValue = options.GetValue("VertexCacheStart");
   if (pValue)
   {
      m_nVertexCacheStart = atoi(pValue);
      PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Vertex Cache Start set to %d",m_nVertexCacheStart);
   }
}

void TriStripFilter::TriStripTriMesh(TupTriMesh &tupTriMesh)
{
	StripBuilder stripBuilder;

	float *pPositionVerts;
	float *pNormalVerts;
	int *pPositionIndices;
	int *pNormalIndices;
	int *pFaceMaterialIndices;
	int *pFaceFlags;
	float *pFaceNormalVerts;
	int *pFaceNormalIndices;

	int numPositionVerts;
	int numNormalVerts;
	int numFaceNormalVerts;
	int numFaces;

	tupTriMesh.GetPositionVerts(&pPositionVerts,numPositionVerts);
	tupTriMesh.GetNormalVerts(&pNormalVerts,numNormalVerts);
	tupTriMesh.GetPositionIndices(&pPositionIndices,numFaces);
	tupTriMesh.GetNormalIndices(&pNormalIndices,numFaces);
	tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);
	tupTriMesh.GetFaceFlags(&pFaceFlags,numFaces);
	tupTriMesh.GetFaceNormalVerts(&pFaceNormalVerts,numFaceNormalVerts);
	tupTriMesh.GetFaceNormalIndices(&pFaceNormalIndices,numFaces);

	TupperwareAggregate *pMeshAgg = tupTriMesh.GetBaseAggregate();

	int *pBoneVerts=NULL; // start with no bone verts
	// if we need to check the bones on the position verts
	if (m_nBonesPerStrip>0)
	{
		TupperwareAggregate *pBoneMeshAgg = pMeshAgg->FindAggregateByKey("Annotate_BoneMeshData");
		if (pBoneMeshAgg)
		{
			TupperwareList *pBoneVertList = pBoneMeshAgg->FindListByKey("MeshBoneVerts");
			assert(pBoneVertList);
			pBoneVerts = pBoneVertList->GetAsInt();
			int numBoneVerts = pBoneVertList->GetLength();
			assert(numBoneVerts == numPositionVerts);
		}
	}

	bool bMeshVertexColors = true;
	bool bMeshNormals = false;

	bool bALit = pMeshAgg->FindScalarByKey("RealTimeLighting_Ambient")->GetAsInt() ? true : false;
	bool bPLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Point")->GetAsInt() ? true : false;
	bool bDLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Directional")->GetAsInt() ? true : false;
	bool bVLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Vertex")->GetAsInt() ? true : false;

	if (pBoneVerts || bALit || bPLit || bDLit)
	{
		bMeshVertexColors = bVLit;
		bMeshNormals = true;
	}

	if (numFaces==0)
	{
		throw(PluginException(ERROR_FATAL,"TriStripTriMesh - Mesh with no faces"));
	}
	stripBuilder.SetNumFaces(numFaces);
	stripBuilder.AddPositionVerts(pPositionVerts,numPositionVerts);
	stripBuilder.AddPositionIndices(pPositionIndices);
	if (bMeshNormals)
	{
		stripBuilder.AddNormalVerts(pNormalVerts,numNormalVerts);
		stripBuilder.AddNormalIndices(pNormalIndices);
	}
	stripBuilder.AddFaceMaterialIndices(pFaceMaterialIndices);
	stripBuilder.AddFaceFlags(pFaceFlags);
	stripBuilder.AddFaceNormalVerts(pFaceNormalVerts,numFaceNormalVerts);
	stripBuilder.AddFaceNormalIndices(pFaceNormalIndices);

	TupString name = "None";
	TupperwareScalar *pMayaObjectNameScalar = tupTriMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
	if (pMayaObjectNameScalar)
	{
		name = pMayaObjectNameScalar->GetAsString();
	}
	stripBuilder.AddName(name);

	// check zbias value,two sided flag,as well as cloth
	int faceFlagMask = TupTriMesh::FLAGS_FACE_Z_BIAS_MASK|
							 TupTriMesh::FLAGS_FACE_TWO_SIDED|
							 TupTriMesh::FLAGS_FACE_NO_Z_WRITE|
							 TupTriMesh::FLAGS_FACE_USES_CLOTH|
							 TupTriMesh::FLAGS_FACE_STAGE_MASK|
							 TupTriMesh::FLAGS_FACE_NO_RECEIVE_SHADOWS|
							 TupTriMesh::FLAGS_FACE_HIDDEN;
	stripBuilder.SetFaceFlagMask(faceFlagMask); 

	if (pBoneVerts)
	{
		stripBuilder.AddBoneVerts(pBoneVerts,numPositionVerts);
		stripBuilder.SetBonesPerStrip(m_nBonesPerStrip);
	}
	stripBuilder.SetAllowSwaps(m_bAllowSwaps);
   stripBuilder.SetOptimizeVertexCache(m_bOptimizeVertexCache);

	m_uvChannelToIndexKeeper.RemoveAll();
	m_uvMaps.RemoveAll();

	int numMeshMaps = tupTriMesh.GetNumMeshMaps();
	for (int meshMapIndex=0;meshMapIndex<numMeshMaps;meshMapIndex++)
	{
		TupperwareAggregate *pMeshMapAgg = tupTriMesh.GetMeshMap(meshMapIndex);
		TupTriMeshMap tupTriMeshMap(pMeshMapAgg);
		int mapChannel = tupTriMeshMap.GetMapChannel();
		if (mapChannel>=TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
		{
			UVMap newUVMap;
			newUVMap.m_mapChannel = mapChannel;
			int oldSize = m_uvChannelToIndexKeeper.GetSize();
			int newIndex = m_uvChannelToIndexKeeper.Add(mapChannel); // keep track of mapChannel to array index
			if (oldSize==newIndex)
			{
				int numMapFaces;
				tupTriMeshMap.GetMapVerts((float **) &newUVMap.m_pUVVerts, newUVMap.m_uvVerts );
				tupTriMeshMap.GetMapIndices( &newUVMap.m_pUVIndices, numMapFaces);
				if (numMapFaces!=numFaces)
				{
					throw(PluginException(ERROR_FATAL,"TriStripFilter - TriStripTriMesh numMapFaces is not correct count. Is %d, should be %d",numMapFaces,numFaces));
				}
				stripBuilder.AddMeshMap((float *)newUVMap.m_pUVVerts,newUVMap.m_uvVerts,newUVMap.m_pUVIndices);
				m_uvMaps.Add(newUVMap);
			}
		}
	}

	m_vertexColorMaps.RemoveAll();
	m_vertexAlphaMaps.RemoveAll();

	// search for the vertex color reference list
	// set up vertex color info so we can later determine if we need to increment the vertex color offsets
	int *pVertexColorRefData = NULL;
	int *pVertexAlphaRefData = NULL;
	int nNumColorSets = 0;
	if (bMeshVertexColors)
	{
		TupperwareList *pVertexColorRefList = pMeshAgg->FindListByKey("Annotate_VertexColorRefList");
		TupperwareList *pVertexAlphaRefList = pMeshAgg->FindListByKey("Annotate_VertexAlphaRefList");
		if (pVertexColorRefList && pVertexAlphaRefList)
		{
			pVertexColorRefData = pVertexColorRefList->GetAsInt();
			pVertexAlphaRefData = pVertexAlphaRefList->GetAsInt();
			nNumColorSets = pVertexColorRefList->GetLength();
			// check if all references are pointing to a -1 reference (standard white)
			if (nNumColorSets==1 && pVertexColorRefData[0] == -1 && pVertexAlphaRefData[0] == -1)
			{
				// lets say there are no vertex color sets
				nNumColorSets = 0;
			}
		}
	}
	// set up the vertex color sets
	if (nNumColorSets)
	{
		m_vertexColorMaps.SetSize(nNumColorSets);
		m_vertexAlphaMaps.SetSize(nNumColorSets);

		for (int colorSetIndex = 0;colorSetIndex<nNumColorSets;colorSetIndex++)
		{
			VertexColorMap &vertexColorMap = m_vertexColorMaps[colorSetIndex];
			VertexColorMap &vertexAlphaMap = m_vertexAlphaMaps[colorSetIndex];

			int colorObjectRef = pVertexColorRefData[colorSetIndex];
			int alphaObjectRef = pVertexAlphaRefData[colorSetIndex];

			if (colorObjectRef!=-1) // if valid object reference
			{
				TupperwareAggregate *pChildMeshMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(*m_pTupObjectPool,colorObjectRef);
				assert(pChildMeshMapAgg);

				TupTriMeshMap tupTriMeshMap(pChildMeshMapAgg);

				int numMapFaces;
				tupTriMeshMap.GetMapVerts((float **) &vertexColorMap.m_pColorVerts, vertexColorMap.m_numColorVerts );
				tupTriMeshMap.GetMapIndices( &vertexColorMap.m_pColorIndices, numMapFaces );
				assert(numFaces == numMapFaces);
				stripBuilder.AddVertexColorMap((float *)vertexColorMap.m_pColorVerts,vertexColorMap.m_numColorVerts,vertexColorMap.m_pColorIndices);
			}
			else
			{
				// make this entry not point to any data
				vertexColorMap.m_numColorVerts=0;
				vertexColorMap.m_pColorVerts=NULL;
				vertexColorMap.m_pColorIndices=NULL;
			}
			if (alphaObjectRef!=-1) // if valid object reference
			{
				TupperwareAggregate *pChildMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(*m_pTupObjectPool,alphaObjectRef);
				assert(pChildMeshMapAgg);

				TupTriMeshMap tupTriMeshMap(pChildMeshMapAgg);

				int numMapFaces;
				tupTriMeshMap.GetMapVerts((float **) &vertexAlphaMap.m_pColorVerts, vertexAlphaMap.m_numColorVerts );
				tupTriMeshMap.GetMapIndices( &vertexAlphaMap.m_pColorIndices, numMapFaces );
				assert(numFaces == numMapFaces);
				stripBuilder.AddVertexAlphaMap((float *)vertexAlphaMap.m_pColorVerts,vertexAlphaMap.m_numColorVerts,vertexAlphaMap.m_pColorIndices);
			}
			else
			{
				// make this entry not point to any data
				vertexAlphaMap.m_numColorVerts=0;
				vertexAlphaMap.m_pColorVerts=NULL;
				vertexAlphaMap.m_pColorIndices=NULL;
			}
		}
	}

	// get material reference
	TupArray<TupArray<UVTransform> >uvTransformArray;
	TupArray<TupArray<Vector3> >uvLimitsArray;
	TupArray<TupArray<Vector3> >faceUVArray;
	if (m_nMaxStripPixels)
	{
		int materialRef = pMeshAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();
		BuildUVTransformsAndLimits(materialRef,uvTransformArray,uvLimitsArray);
		BuildUVFaceData(uvTransformArray,tupTriMesh,faceUVArray);
		stripBuilder.AddUVLimitsArray(uvLimitsArray);
		stripBuilder.AddFaceUVArray(faceUVArray);
	}
	stripBuilder.SetNormalThreshold(m_normalThreshold);
	stripBuilder.SetMapThreshold(m_mapThreshold);
	stripBuilder.SetVertexColorThreshold(m_vertexColorThreshold);
	stripBuilder.SetVertexAlphaThreshold(m_vertexAlphaThreshold);
	stripBuilder.SetStripStageFaces(m_bStripStageFaces);
   stripBuilder.SetVertexCacheSize(m_nVertexCacheSize);
   stripBuilder.SetVertexCacheStart(m_nVertexCacheStart);

	// try to find a staged face index array
	TupperwareList *pNextStageFaceIndicesList = pMeshAgg->FindListByKey("CombineMeshes_NextStageFaceIndices");
	if (!pNextStageFaceIndicesList)
		pNextStageFaceIndicesList = pMeshAgg->FindListByKey("MultiTex_NextStageFaceIndices");
	if (pNextStageFaceIndicesList)
	{
		stripBuilder.AddNextStagedFaceIndices(pNextStageFaceIndicesList->GetAsInt());
	}
	
	stripBuilder.ComputeStrips();
	TupArray<StripBuilder::PointStrip> &pointStrips = stripBuilder.GetPointStripData();
	TupArray<StripBuilder::StripSortData> &stripSortData = stripBuilder.GetStripSortData();
	TupperwareAggregate *pStripsAgg = tupTriMesh.AddTriangleStrips();
	TupTriangleStripPool tupTriangleStripPool(pStripsAgg);
	int numStrips = pointStrips.GetSize();
	assert(numStrips == stripSortData.GetSize());
	int faceCount=0;
	int pointCount=0;
	int swapCount=0;
	for (int stripIndex=0;stripIndex<numStrips;stripIndex++)
	{
		// get correct strip (use sort data)
		StripBuilder::PointStrip &strip = pointStrips[stripSortData[stripIndex].m_stripIndex];

		TupperwareAggregate *pStripAgg = tupTriangleStripPool.AddTriangleStrip(stripIndex);
		TupTriangleStrip tupTriangleStrip(pStripAgg);

		int numPoints = strip.GetNumPoints();
		faceCount+=strip.GetNumTriangles();
		pointCount+=numPoints;
		swapCount+=numPoints-(strip.GetNumTriangles()+2);
		// add indices to strip
		tupTriangleStrip.AddFaceIndices(strip.GetFaceIndices(),numPoints);
		tupTriangleStrip.AddFacePointIndices(strip.GetPointIndices(),numPoints);
//		pStripAgg->AddScalarInt("NumFaces",NULL,strip.GetNumTriangles());
//		pStripAgg->AddScalarInt("NumSwaps",NULL,numPoints-(strip.GetNumTriangles()+2));
	}

	if (numStrips==0)
		throw(PluginException(ERROR_FATAL,"TriStripTriMesh - Mesh has no visible faces.  Cannot generate strips"));

	TupArray<int> &faceConnectionGroups = stripBuilder.GetConnectedFaceGroups();
	pMeshAgg->AddListInt("TriStrip_ConnectedFaceGroups",NULL,faceConnectionGroups);

	const StripBuilder::Stats &stripBuilderStats = stripBuilder.GetStats();

	MeshStripStats meshStripStats;
	meshStripStats.m_name = name;
	meshStripStats.m_numFaces = numFaces;
	meshStripStats.m_numStrips = numStrips;
	meshStripStats.m_numPoints = pointCount;
	meshStripStats.m_numSwaps = swapCount;
	meshStripStats.m_pointsPerFace = (((float)pointCount)/faceCount);
	meshStripStats.m_trianglesPerStrip = (((float)faceCount)/numStrips);

	meshStripStats.m_totalConnections = stripBuilderStats.m_totalConnections;
	meshStripStats.m_totalFaceConnectionGroups = stripBuilderStats.m_totalFaceConnectionGroups;
	meshStripStats.m_connectionBreakMaterialIndex = stripBuilderStats.m_connectionBreakMaterialIndex;
	meshStripStats.m_connectionBreakFaceFlags = stripBuilderStats.m_connectionBreakFaceFlags;
	meshStripStats.m_connectionBreakVertexNormals = stripBuilderStats.m_connectionBreakVertexNormals;
	meshStripStats.m_connectionBreakMeshMaps = stripBuilderStats.m_connectionBreakMeshMaps;
	meshStripStats.m_connectionBreakVertexColors = stripBuilderStats.m_connectionBreakVertexColors;
	meshStripStats.m_connectionBreakVertexAlphas = stripBuilderStats.m_connectionBreakVertexAlphas;
	meshStripStats.m_connectionBreakStages = stripBuilderStats.m_connectionBreakStages;
	meshStripStats.m_buildPathBreaksBones = stripBuilderStats.m_buildPathBreaksBones;
	meshStripStats.m_buildPathBreaksUVLimit = stripBuilderStats.m_buildPathBreaksUVLimit;
	meshStripStats.m_buildStripFromPathOneFace = stripBuilderStats.m_buildStripFromPathOneFace;
	meshStripStats.m_buildStripFromPathTwoFace = stripBuilderStats.m_buildStripFromPathTwoFace;
	meshStripStats.m_buildStripFromPathBadEdge = stripBuilderStats.m_buildStripFromPathBadEdge;
	meshStripStats.m_buildStripFromPathBadEvenStrip = stripBuilderStats.m_buildStripFromPathBadEvenStrip;
	meshStripStats.m_buildStripFromPathBadEvenStripEdge = stripBuilderStats.m_buildStripFromPathBadEvenStripEdge;
	meshStripStats.m_combineStrips = stripBuilderStats.m_combineStrips;
	meshStripStats.m_sortedStripsBreakMaterialIndex = stripBuilderStats.m_sortedStripsBreakMaterialIndex;
	meshStripStats.m_sortedStripsBreakFaceFlags = stripBuilderStats.m_sortedStripsBreakFaceFlags;
   meshStripStats.m_vertexCacheMisses = stripBuilderStats.m_vertexCacheMisses;
   meshStripStats.m_vertexCacheHits = stripBuilderStats.m_vertexCacheHits;

	m_meshStripStats.Add(meshStripStats);

	m_totalStrips += numStrips;
	m_totalFaces += faceCount;
	m_totalPoints += pointCount;
	m_totalSwaps += swapCount;

//	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"  Total Strips = %d, Total Faces = %d, Total Points = %d, Total Swaps = %d",
//												numStrips,faceCount,pointCount,swapCount);
//	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"  Average Triangles Per Strip = %f, Average Points Per Face = %f",
//												((float)faceCount)/numStrips,((float)pointCount)/faceCount);

//	pStripsAgg->AddScalarInt("Total Strips",NULL,numStrips);
//	pStripsAgg->AddScalarInt("Total Faces",NULL,faceCount);
//	pStripsAgg->AddScalarInt("Total Points",NULL,pointCount);
//	pStripsAgg->AddScalarFloat("Average Triangles Per Strip",NULL,((float)faceCount)/numStrips);
//	pStripsAgg->AddScalarFloat("Average Points Per Face",NULL,((float)pointCount)/faceCount);
#if 0
	StripBuilder::TriFace *pTriFaceArray = stripBuilder.GetFaceData();
	TupperwareAggregate *pMeshAgg = tupTriMesh.GetBaseAggregate();
	TupperwareAggregate *pLinksAgg = pMeshAgg->AddAggregate("Triangle Strip Face Links",NULL);
	char string[40];
	for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		itoa(faceIndex,string,10);
		TupperwareAggregate *pLinkAgg = pLinksAgg->AddAggregate("Face",string);
		StripBuilder::TriFace &triFace = pTriFaceArray[faceIndex];
		pLinkAgg->AddScalarInt("Flags",NULL,triFace.GetFlags());
		for (int pointIndex=0;pointIndex<3;pointIndex++)
		{
			StripBuilder::Edge &edge = triFace.GetEdge(pointIndex);
			itoa(pointIndex,string,10);
			TupperwareAggregate *pPointAgg = pLinkAgg->AddAggregate("Edge",string);
			pPointAgg->AddScalarInt("FaceIndex",NULL,edge.GetFaceIndex());
			pPointAgg->AddScalarInt("PointIndex",NULL,edge.GetPointIndex());
		}
		pLinkAgg->AddScalarInt("PathParent",NULL,triFace.GetPathParentFace());
		pLinkAgg->AddScalarInt("PathNumSplits",NULL,triFace.GetPathNumSplits());
		pLinkAgg->AddScalarInt("StripIndex",NULL,triFace.GetStripIndex());
	}
	TupArray<StripBuilder::Strip> &strips = stripBuilder.GetStripData();
	TupperwareAggregate *pStripsAgg = pMeshAgg->AddAggregate("TriangleStrip Stats",NULL);
	int validStrips = 0;
	for (int stripIndex=0;stripIndex<strips.size();stripIndex++)
	{
		StripBuilder::Strip &strip = strips[stripIndex];
		if (!(strip.GetFlags()&StripBuilder::Strip::REMOVED))
		{
			validStrips++;
			itoa(stripIndex,string,10);
			TupperwareAggregate *pStripAgg = pStripsAgg->AddAggregate("Strip",string);
			pStripAgg->AddScalarInt("StartFace",NULL,strip.GetStartFace());
			pStripAgg->AddScalarInt("EndFace",NULL,strip.GetEndFace());
			pStripAgg->AddScalarInt("NumFaces",NULL,strip.GetNumFaces());
		}
	}
	pStripsAgg->AddScalarInt("Total Strips",NULL,validStrips);
#endif
}

void TriStripFilter::BuildUVTransformsAndLimits(int materialRef,TupArray<TupArray<UVTransform> > &uvTransformArray,TupArray<TupArray<Vector3> > &uvLimitsArray)
{
	BuildUVTransformManager buildUVTransformManager(*m_pTupMaterialPool,*m_pTupMapPool,*m_pTupImagePool,false);
	ImageManager &imageManager = m_pTupDataManager->GetImageManager();

	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialRef!=-1)
		pMaterialAgg = m_pTupMaterialPool->GetMaterial(materialRef);

	if (pMaterialAgg)
	{
		TupMaterial tupMat(pMaterialAgg);

		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
			int totalUVTransforms = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (totalUVTransforms)
			{
				uvTransformArray.SetSize(totalUVTransforms);
				uvLimitsArray.SetSize(totalUVTransforms);
				for (int materialIndex = 0;materialIndex<totalUVTransforms;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int subMaterialRef;
					tupSubMaterial.GetMaterialReference(subMaterialRef);
					buildUVTransformManager.BuildUVTransforms(subMaterialRef,uvTransformArray[materialIndex]);
					int numLayers = uvTransformArray[materialIndex].GetSize();
					uvLimitsArray[materialIndex].SetSize(numLayers);
					for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
					{
						if (uvTransformArray[materialIndex][layerIndex].m_imageRef!=-1)
						{
							uvLimitsArray[materialIndex][layerIndex].x( ((float)m_nMaxStripPixels)/(float)uvTransformArray[materialIndex][layerIndex].m_imageWidth );
							uvLimitsArray[materialIndex][layerIndex].y( ((float)m_nMaxStripPixels)/(float)uvTransformArray[materialIndex][layerIndex].m_imageHeight );
						}
						else
						{
							uvLimitsArray[materialIndex][layerIndex].x( 10000.0f );
							uvLimitsArray[materialIndex][layerIndex].y( 10000.0f );
						}
					}
				}
			}
			else
			{
				uvTransformArray.SetSize(1);
				uvTransformArray[0].SetSize(1);
				uvTransformArray[0][0].Init();
				uvLimitsArray.SetSize(1);
				uvLimitsArray[0].SetSize(1);
				uvLimitsArray[0][0].x( 10000.0f );
				uvLimitsArray[0][0].y( 10000.0f );
			}
		}
		else
		{
			uvTransformArray.SetSize(1);
			uvLimitsArray.SetSize(1);
			buildUVTransformManager.BuildUVTransforms(materialRef,uvTransformArray[0]);
			int numLayers = uvTransformArray[0].GetSize();
			uvLimitsArray[0].SetSize(numLayers);
			for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
			{
				if (uvTransformArray[0][layerIndex].m_imageRef!=-1)
				{
					uvLimitsArray[0][layerIndex].x( ((float)m_nMaxStripPixels)/(float)uvTransformArray[0][layerIndex].m_imageWidth );
					uvLimitsArray[0][layerIndex].y( ((float)m_nMaxStripPixels)/(float)uvTransformArray[0][layerIndex].m_imageHeight );
				}
				else
				{
					uvLimitsArray[0][layerIndex].x( 10000.0f );
					uvLimitsArray[0][layerIndex].y( 10000.0f );
				}
			}
		}
	}
	else
	{
		uvTransformArray.SetSize(1);
		uvTransformArray[0].SetSize(1);
		uvTransformArray[0][0].Init();
		uvLimitsArray.SetSize(1);
		uvLimitsArray[0].SetSize(1);
		uvLimitsArray[0][0].x( 10000.0f );
		uvLimitsArray[0][0].y( 10000.0f );
	}
}

// bake all uv coordincate into the faceUVArray
void TriStripFilter::BuildUVFaceData(TupArray<TupArray<UVTransform> > &uvTransformArray,TupTriMesh &tupTriMesh,TupArray<TupArray<Vector3> >&faceUVArray)
{
	int *pFaceMaterialIndices;
	int numFaces;
	tupTriMesh.GetFaceMaterialIndices(&pFaceMaterialIndices,numFaces);
	faceUVArray.SetSize((numFaces<<1)+numFaces);

	int faceIndex;
	for (faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		int faceMaterial = pFaceMaterialIndices[faceIndex];
		int numLayers = uvTransformArray[faceMaterial].GetSize();
		// set number of layers for each point in the face
		for (int pointIndex=0;pointIndex<3;pointIndex++)
		{
			int facePointIndex = (faceIndex<<1)+faceIndex+pointIndex;
			faceUVArray[facePointIndex].SetSize(numLayers);
		}
		// loop through each layer
		int layerIndex;
		for (layerIndex=0;layerIndex<numLayers;layerIndex++)
		{
			UVTransform &uvTransform = uvTransformArray[faceMaterial][layerIndex];
			int uvMapIndex = m_uvChannelToIndexKeeper.GetIndex(uvTransform.m_mapChannel);
			// make sure we have a valid uv set
			if (uvMapIndex!=-1)
			{
				Vector3 *pUVVerts = m_uvMaps[uvMapIndex].m_pUVVerts;
				int *pUVIndices = m_uvMaps[uvMapIndex].m_pUVIndices;
				for (int pointIndex=0;pointIndex<3;pointIndex++)
				{
					int facePointIndex = (faceIndex<<1)+faceIndex+pointIndex;
					Vector3 &facePointUVFill = faceUVArray[facePointIndex][layerIndex];
					faceUVArray[facePointIndex].SetSize(numLayers);
					// if there is no vert channel then set to 0
					if (!pUVVerts)
					{
						facePointUVFill.Clear();
					}
					else
					{
						// start with channel uv coordinate
						facePointUVFill = pUVVerts[pUVIndices[facePointIndex]];
						// transform coordinate
						ApplyUVTransform::Apply(uvTransform,facePointUVFill);
					}
				}
			}
		}
	}
}

class StripSortData
{
public:
	int m_numStrips;
	int m_numFaces;
	int m_numPoints;
	int m_numSwaps;
	int m_totalConnections;
	int m_totalFaceConnectionGroups;
	int m_connectionBreakMaterialIndex;
	int m_connectionBreakFaceFlags;
	int m_connectionBreakVertexNormals;
	int m_connectionBreakMeshMaps;
	int m_connectionBreakVertexColors;
	int m_connectionBreakVertexAlphas;
	int m_connectionBreakStages;
	int m_buildPathBreaksBones;
	int m_buildPathBreaksUVLimit;
	int m_buildStripFromPathOneFace;
	int m_buildStripFromPathTwoFace;
	int m_buildStripFromPathBadEdge;
	int m_buildStripFromPathBadEvenStrip;
	int m_buildStripFromPathBadEvenStripEdge;
	int m_combineStrips;
	int m_sortedStripsBreakMaterialIndex;
	int m_sortedStripsBreakFaceFlags;
   int m_vertexCacheMisses;
   int m_vertexCacheHits;
	float m_pointsPerFace;
	float m_trianglesPerStrip;
	const char *m_pName;

	StripSortData(void) : m_numStrips(0),m_numFaces(0),m_numPoints(0),m_numSwaps(0),m_totalConnections(0),m_totalFaceConnectionGroups(0),
		m_connectionBreakMaterialIndex(0),m_connectionBreakFaceFlags(0),m_connectionBreakVertexNormals(0),
		m_connectionBreakMeshMaps(0),m_connectionBreakVertexColors(0),m_connectionBreakVertexAlphas(0),
		m_connectionBreakStages(0),m_buildPathBreaksBones(0),m_buildPathBreaksUVLimit(0),m_buildStripFromPathOneFace(0),
		m_buildStripFromPathTwoFace(0),m_buildStripFromPathBadEdge(0),m_buildStripFromPathBadEvenStrip(0),
		m_buildStripFromPathBadEvenStripEdge(0),
      m_vertexCacheMisses(0),m_vertexCacheHits(0),m_combineStrips(0),
		m_sortedStripsBreakMaterialIndex(0),m_sortedStripsBreakFaceFlags(0),
		m_pointsPerFace(0.0f),m_trianglesPerStrip(0.0f),m_pName(NULL)
	{
	}

	bool StripSortData::operator==(const StripSortData& other) const
	{
		return (m_pointsPerFace==other.m_pointsPerFace &&
			strcmp(m_pName,other.m_pName)==0);
	}

	bool StripSortData::operator<=(const StripSortData& other) const
	{
		if (m_pointsPerFace<other.m_pointsPerFace)
			return true;
		else if (m_pointsPerFace>other.m_pointsPerFace)
			return false;
		// m_pointsPerFace ==
		// return true if path is less than or equal other
		return (strcmp(m_pName,other.m_pName)<1);
	}

	bool StripSortData::operator>=(const StripSortData& other) const
	{
		if (m_pointsPerFace>other.m_pointsPerFace)
			return true;
		else if (m_pointsPerFace<other.m_pointsPerFace)
			return false;
		// m_pointsPerFace ==
		// return true if path is greater than or equal other
		return (strcmp(m_pName,other.m_pName)>-1);
	}

	bool StripSortData::operator<(const StripSortData& other) const
	{
		if (m_pointsPerFace>other.m_pointsPerFace)
			return false;
		else if (m_pointsPerFace<other.m_pointsPerFace)
			return true;
		// m_pointsPerFace ==
		// return true if path is less than other
		return (strcmp(m_pName,other.m_pName)<0);
	}

	bool StripSortData::operator>(const StripSortData& other) const
	{
		if (m_pointsPerFace>other.m_pointsPerFace)
			return true;
		else if (m_pointsPerFace<other.m_pointsPerFace)
			return false;
		// m_pointsPerFace ==
		// return true if path is greater than other
		return (strcmp(m_pName,other.m_pName)>0);
	}
};

void TriStripFilter::AddSortedStripsToLog(void)
{
	int numMeshes = m_meshStripStats.GetSize();

	TupArray<StripSortData> sortedStrips;

	for (int meshIndex=0;meshIndex<numMeshes;meshIndex++)
	{
		StripSortData stripSortData;
		MeshStripStats &meshStripStats = m_meshStripStats[meshIndex];

		stripSortData.m_numStrips = meshStripStats.m_numStrips;
		stripSortData.m_numFaces = meshStripStats.m_numFaces;
		stripSortData.m_numPoints = meshStripStats.m_numPoints;
		stripSortData.m_numSwaps = meshStripStats.m_numSwaps;
		stripSortData.m_totalConnections = meshStripStats.m_totalConnections;
		stripSortData.m_totalFaceConnectionGroups = meshStripStats.m_totalFaceConnectionGroups;
		stripSortData.m_connectionBreakMaterialIndex = meshStripStats.m_connectionBreakMaterialIndex;
		stripSortData.m_connectionBreakFaceFlags = meshStripStats.m_connectionBreakFaceFlags;
		stripSortData.m_connectionBreakVertexNormals = meshStripStats.m_connectionBreakVertexNormals;
		stripSortData.m_connectionBreakMeshMaps = meshStripStats.m_connectionBreakMeshMaps;
		stripSortData.m_connectionBreakVertexColors = meshStripStats.m_connectionBreakVertexColors;
		stripSortData.m_connectionBreakVertexAlphas = meshStripStats.m_connectionBreakVertexAlphas;
		stripSortData.m_connectionBreakStages = meshStripStats.m_connectionBreakStages;
      stripSortData.m_vertexCacheMisses = meshStripStats.m_vertexCacheMisses;
      stripSortData.m_vertexCacheHits = meshStripStats.m_vertexCacheHits;

		stripSortData.m_buildPathBreaksBones = meshStripStats.m_buildPathBreaksBones;
		stripSortData.m_buildPathBreaksUVLimit = meshStripStats.m_buildPathBreaksUVLimit;
		stripSortData.m_buildStripFromPathOneFace = meshStripStats.m_buildStripFromPathOneFace;
		stripSortData.m_buildStripFromPathTwoFace = meshStripStats.m_buildStripFromPathTwoFace;
		stripSortData.m_buildStripFromPathBadEdge = meshStripStats.m_buildStripFromPathBadEdge;
		stripSortData.m_buildStripFromPathBadEvenStrip = meshStripStats.m_buildStripFromPathBadEvenStrip;
		stripSortData.m_buildStripFromPathBadEvenStripEdge = meshStripStats.m_buildStripFromPathBadEvenStripEdge;
		stripSortData.m_combineStrips = meshStripStats.m_combineStrips;

		stripSortData.m_sortedStripsBreakMaterialIndex = meshStripStats.m_sortedStripsBreakMaterialIndex;
		stripSortData.m_sortedStripsBreakFaceFlags = meshStripStats.m_sortedStripsBreakFaceFlags;

		stripSortData.m_pointsPerFace = meshStripStats.m_pointsPerFace;
		stripSortData.m_trianglesPerStrip = meshStripStats.m_trianglesPerStrip;
		stripSortData.m_pName = meshStripStats.m_name; // point to other array

		sortedStrips.Add(stripSortData);
	}
	int numSortedMeshes = sortedStrips.GetSize();
	TupSort<StripSortData>::SortAscending(sortedStrips.GetData(),numSortedMeshes);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Sorted TriStrip List:");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH," Average   Average                                                                                |-------------Face Connection Breaks-------------|-Build Path Break-|------Build Strip From Path-----|---------|Sorted Strip Breaks|           ");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH," Points   Triangles  Num   Num    Num    Num   Num Cache  Num Cache   Num Face       Num Face     |Material Face  Vertex  Mesh Vertex Vertex       |     Max   UV     |One   Two  Bad  Even  Even Strip| Combine |    Material Face  |           ");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"Per Face  Per Strip Faces Strips Points Swaps    Hits      Misses   Connections  Connection Groups| Index   Flags Normals Maps Colors Alphas Stages|    Bones Limit   |Face Faces Edge Strip   Edge    | Strips  |     Index   Flags |  Mesh Name");
	int totalSize=0;
	for (meshIndex=0;meshIndex<numSortedMeshes;meshIndex++)
	{
		StripSortData &stripSortData = sortedStrips[meshIndex];
//                                                   appf	atps	  nf	  ns	np		ns	  nch     ncm       nfc        nfcg        	  mi	  ff		vn	  mm	 vc     va   s         mb  uvl      of  tf   be   es  ese        cs         mi    ff
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_HIGH,"% 6.4f   % 8.4f  %5d  %5d  %5d  %4d   %5d     %5d       %5d        %4d            %4d   %4d   %4d  %4d   %4d   %4d  %4d       %4d  %4d     %4d %4d   %4d %4d  %4d        %4d        %4d   %4d       %s",
			stripSortData.m_pointsPerFace,stripSortData.m_trianglesPerStrip,stripSortData.m_numFaces,stripSortData.m_numStrips,
			stripSortData.m_numPoints,stripSortData.m_numSwaps,stripSortData.m_vertexCacheHits,stripSortData.m_vertexCacheMisses,
         stripSortData.m_totalConnections,stripSortData.m_totalFaceConnectionGroups,
			stripSortData.m_connectionBreakMaterialIndex,stripSortData.m_connectionBreakFaceFlags,stripSortData.m_connectionBreakVertexNormals,
			stripSortData.m_connectionBreakMeshMaps,stripSortData.m_connectionBreakVertexColors,stripSortData.m_connectionBreakVertexAlphas,stripSortData.m_connectionBreakStages,
			stripSortData.m_buildPathBreaksBones,stripSortData.m_buildPathBreaksUVLimit,stripSortData.m_buildStripFromPathOneFace,
			stripSortData.m_buildStripFromPathTwoFace,stripSortData.m_buildStripFromPathBadEdge,stripSortData.m_buildStripFromPathBadEvenStrip,
			stripSortData.m_buildStripFromPathBadEvenStripEdge,stripSortData.m_combineStrips,
			stripSortData.m_sortedStripsBreakMaterialIndex,stripSortData.m_sortedStripsBreakFaceFlags,
			stripSortData.m_pName);
	}
}
