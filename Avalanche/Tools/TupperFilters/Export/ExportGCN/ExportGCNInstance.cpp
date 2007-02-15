////////////////////////////////////////////////////////////////////////////
//
// ExportGCNInstance
//
// Export Instances into the DBL file (Gamecube specific)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportGCNInstance.cpp $
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:29a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 4:33p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * fixed consolidated bug
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 6/16/03    Time: 2:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * consolidate vertex colors on meshes that use one colorset
 * 
 * *****************  Version 10  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 5:56p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * fixed some vector3 stuff
 * 
 * *****************  Version 9  *****************
 * User: Tyler Colbert Date: 6/12/03    Time: 5:55p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 3:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * first pass of data reduction complete
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 5/28/03    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * gamecube 16 bit colors
 * 
 * *****************  Version 6  *****************
 * User: Dwain Skinner Date: 5/02/03    Time: 11:11a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 3:45p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * lighting issues
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/12/02    Time: 12:22p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Vertex color alpha support
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/21/02    Time: 8:45a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * vertex colors are not aligned after each tristrip
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/20/02    Time: 8:44a
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * Instance specific GCN code for vertex colors
 */

#include "ExportPch.h"
#include "ExportInstance.h"


void ExportInstance::GetConsolidatedColorPool(Keeper<ExportInstance::RGBAColor> &colorPool,TupObjectPool &tupObjectPool,TupTriMesh &tupTriMesh,int colorRef,int alphaRef)
{
	colorPool.RemoveAll();

	Vector3 *pVertexColorVerts = NULL;
	int *pVertexColorIndices = NULL;
	int numVertexColorVerts;
	int numVertexColorFaces;
	
	Vector3 *pVertexAlphaVerts = NULL;
	int *pVertexAlphaIndices = NULL;
	int numVertexAlphaVerts;
	int numVertexAlphaFaces;

	if (colorRef!=-1)
	{
		TupperwareAggregate *pChildVertexColorMeshMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(tupObjectPool,colorRef);
		TupTriMeshMap tupTriMeshMap(pChildVertexColorMeshMapAgg);
		tupTriMeshMap.GetMapVerts((float **) &pVertexColorVerts, numVertexColorVerts );
		tupTriMeshMap.GetMapIndices( &pVertexColorIndices, numVertexColorFaces );
	}

	if (alphaRef!=-1)
	{
		TupperwareAggregate *pChildVertexAlphaMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(tupObjectPool,alphaRef);
		TupTriMeshMap tupTriMeshMap(pChildVertexAlphaMeshMapAgg);
		tupTriMeshMap.GetMapVerts((float **) &pVertexAlphaVerts, numVertexAlphaVerts );
		tupTriMeshMap.GetMapIndices( &pVertexAlphaIndices, numVertexAlphaFaces );
	}

	TupperwareAggregate *pTriangleStripsAgg = tupTriMesh.GetTriangleStrips();
	if (!pTriangleStripsAgg)
		throw(PluginException(ERROR_FATAL,"Cannot find tristrip data on mesh"));
	
	TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
	int totalStrips = tupTriangleStripPool.GetNumTriangleStrips();
	if (!totalStrips)
		throw(PluginException(ERROR_FATAL,"Cannot find tristrip data on mesh"));
	
	// add to the pools to determine the size (can handle 8 bit indicies)
	int stripIndex;
	int numPoints = 0;

	for (stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		TupperwareAggregate *pTriangleStripAgg = tupTriangleStripPool.GetTriangleStrip(stripIndex);
		assert(pTriangleStripAgg);
		TupTriangleStrip tupTriangleStrip(pTriangleStripAgg);
		int *pFaceIndices;
		int *pFacePointIndices;
		int numFaceIndices;
		int numFacePointIndices;
		tupTriangleStrip.GetFaceIndices(&pFaceIndices,numFaceIndices);
		assert(numFaceIndices);
		tupTriangleStrip.GetFacePointIndices(&pFacePointIndices,numFacePointIndices);
		assert(numFacePointIndices);
		assert(numFaceIndices==numFacePointIndices);

		for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
		{
			int faceIndex = pFaceIndices[pointIndex];
			int facePointIndex = pFacePointIndices[pointIndex];
			
			RGBAColor destColor32;
			// do vertex color
			if (pVertexColorVerts)
			{
				int vertexIndex = pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex];
				Vector3 &sourceColor = pVertexColorVerts[vertexIndex];
				destColor32.R = (u8)(sourceColor.x() *255.0f);
				destColor32.G = (u8)(sourceColor.y() *255.0f);
				destColor32.B = (u8)(sourceColor.z() *255.0f);
			}
			else
			{
				destColor32.R = 255;
				destColor32.G = 0;
				destColor32.B = 255;
			}
			
			// do vertex alpha
			if (pVertexAlphaVerts)
			{
				int vertexIndex = pVertexAlphaIndices[(faceIndex<<1)+faceIndex+facePointIndex];
				Vector3 &sourceColor = pVertexAlphaVerts[vertexIndex];
				destColor32.A = (u8)(sourceColor.x() *255.0f);
			}
			else
			{
				destColor32.A = 255;
			}
			colorPool.Add(destColor32);
			numPoints++;
		}
	}
}

void ExportInstance::GetConsolidatedColorPool(Keeper<u16> &colorPool,TupObjectPool &tupObjectPool,TupTriMesh &tupTriMesh,int colorRef,int alphaRef)
{
	colorPool.RemoveAll();

	Vector3 *pVertexColorVerts = NULL;
	int *pVertexColorIndices = NULL;
	int numVertexColorVerts;
	int numVertexColorFaces;
	
	Vector3 *pVertexAlphaVerts = NULL;
	int *pVertexAlphaIndices = NULL;
	int numVertexAlphaVerts;
	int numVertexAlphaFaces;

	if (colorRef!=-1)
	{
		TupperwareAggregate *pChildVertexColorMeshMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(tupObjectPool,colorRef);
		TupTriMeshMap tupTriMeshMap(pChildVertexColorMeshMapAgg);
		tupTriMeshMap.GetMapVerts((float **) &pVertexColorVerts, numVertexColorVerts );
		tupTriMeshMap.GetMapIndices( &pVertexColorIndices, numVertexColorFaces );
	}

	if (alphaRef!=-1)
	{
		TupperwareAggregate *pChildVertexAlphaMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(tupObjectPool,alphaRef);
		TupTriMeshMap tupTriMeshMap(pChildVertexAlphaMeshMapAgg);
		tupTriMeshMap.GetMapVerts((float **) &pVertexAlphaVerts, numVertexAlphaVerts );
		tupTriMeshMap.GetMapIndices( &pVertexAlphaIndices, numVertexAlphaFaces );
	}

	TupperwareAggregate *pTriangleStripsAgg = tupTriMesh.GetTriangleStrips();
	if (!pTriangleStripsAgg)
		throw(PluginException(ERROR_FATAL,"Cannot find tristrip data on mesh"));
	
	TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
	int totalStrips = tupTriangleStripPool.GetNumTriangleStrips();
	if (!totalStrips)
		throw(PluginException(ERROR_FATAL,"Cannot find tristrip data on mesh"));
	
	// add to the pools to determine the size (can handle 8 bit indicies)
	int stripIndex;
	int numPoints = 0;
	for (stripIndex=0;stripIndex<totalStrips;stripIndex++)
	{
		TupperwareAggregate *pTriangleStripAgg = tupTriangleStripPool.GetTriangleStrip(stripIndex);
		assert(pTriangleStripAgg);
		TupTriangleStrip tupTriangleStrip(pTriangleStripAgg);
		int *pFaceIndices;
		int *pFacePointIndices;
		int numFaceIndices;
		int numFacePointIndices;
		tupTriangleStrip.GetFaceIndices(&pFaceIndices,numFaceIndices);
		assert(numFaceIndices);
		tupTriangleStrip.GetFacePointIndices(&pFacePointIndices,numFacePointIndices);
		assert(numFacePointIndices);
		assert(numFaceIndices==numFacePointIndices);

		for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
		{
			int faceIndex = pFaceIndices[pointIndex];
			int facePointIndex = pFacePointIndices[pointIndex];
			
			u16 destColor16;
			
			if (pVertexColorIndices)
			{
				int vertexIndex = pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex];
				Vector3 &sourceColor = pVertexColorVerts[vertexIndex];
				destColor16 = ((((u8)(sourceColor.x()*31.0f))&0x1f)<<11)|
					((((u8)(sourceColor.y()*63.0f))&0x3f)<<5)|
					((((u8)(sourceColor.z()*31.0f))&0x1f)<<0);
			}
			else
			{
				destColor16 = (0x1f<<11)|0x1f;
			}
			colorPool.Add(destColor16);
			numPoints++;
		}
	}
}



void ExportInstance::WriteGCNInstanceVertexColors(InstanceHolder *pInstanceHolder,int instanceIndex,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	int realTimeLit = 0;

	InstanceHolder &instanceHolder = pInstanceHolder[instanceIndex];
	CInstance &instance = instanceHolder.GetInstanceBuffer();
	if (instanceHolder.GetVertexColorInstanceIndex() >= 0)
	{
		return; // if I am a slave then don't save my colors
	}
	TupperwareAggregate *pNodeAgg = instanceHolder.GetNodeAgg();
	TupNode tupNode(pNodeAgg);
	int objectRef;	
	tupNode.GetObjectReference(objectRef);
	assert(objectRef!=-1);
	TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh(tupObjectPool,objectRef);
	assert(pBaseMeshAgg);
	TupTriMesh tupTriMesh(pBaseMeshAgg);

	TupperwareAggregate *pChildVertexColorMeshMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(tupObjectPool,objectRef);
	TupperwareAggregate *pChildVertexAlphaMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(tupObjectPool,objectRef);

	bool bALit = pBaseMeshAgg->FindScalarByKey("RealTimeLighting_Ambient")->GetAsInt() ? true : false;
	bool bPLit = pBaseMeshAgg->FindScalarByKey("RealTimeLighting_Point")->GetAsInt() ? true : false;
	bool bDLit = pBaseMeshAgg->FindScalarByKey("RealTimeLighting_Directional")->GetAsInt() ? true : false;

	// if real time lit but not wanting vertex colors then stop
	if (bALit || bPLit || bDLit)
	{
		bool bNeedsVertexColors = pBaseMeshAgg->FindScalarByKey("RealTimeLighting_Vertex")->GetAsInt() ? true : false;
		if (!bNeedsVertexColors)
			return;
	}

	// check the mesh to see what kinds of vertex colors are use with it
	int *pVertexColorRefData = NULL;
	int *pVertexAlphaRefData = NULL;
	int nNumColorSets = 0;

	bool bUseConsolidatedColors = false;

	TupperwareList *pVertexColorRefList = pBaseMeshAgg->FindListByKey("Annotate_VertexColorRefList");
	TupperwareList *pVertexAlphaRefList = pBaseMeshAgg->FindListByKey("Annotate_VertexAlphaRefList");
	if (pVertexColorRefList && pVertexAlphaRefList)
	{
		pVertexColorRefData = pVertexColorRefList->GetAsInt();
		pVertexAlphaRefData = pVertexAlphaRefList->GetAsInt();
		nNumColorSets = pVertexColorRefList->GetLength();
		// check if all references are pointing to a -1 reference (standard white)
		if (nNumColorSets==1)
		{
			if (pVertexColorRefData[0] == -1 && pVertexAlphaRefData[0] == -1)
			{
				// lets say there are no vertex color sets
				nNumColorSets = 0;
			}
			else
			{
				bUseConsolidatedColors = true;
			}
		}
	}

	// we always have vertex colors
	// we either save out a consolidated color pool (when only one color set is used)
	// or save out one color (in case mesh references the first color only)
	// or each point using the same color (in case the master instance does not have a color but the mesh is using 1 color per point)
	// or each point uses the color from the mesh map

	// save the offset into the file
	instanceHolder.SetInstanceVertexColorFilePos(ftell(pDBLFile));

	// save out the consolidated colors
	if (bUseConsolidatedColors)
	{
		if (instance.VertexColor32Bit())
		{
			Keeper<RGBAColor> colorKeeper;
			GetConsolidatedColorPool(colorKeeper,tupObjectPool,tupTriMesh,pVertexColorRefData[0],pVertexAlphaRefData[0]);
			int numColors = colorKeeper.GetSize();
			for (int colorIndex=0;colorIndex<numColors;colorIndex++)
			{
				ts_bRGBA color = colorKeeper[colorIndex];
				DBLWrite::WriteRecord(&color, pDBLFile);
			}
		}
		else
		{
			Keeper<u16> colorKeeper;
			GetConsolidatedColorPool(colorKeeper,tupObjectPool,tupTriMesh,pVertexColorRefData[0],pVertexAlphaRefData[0]);
			int numColors = colorKeeper.GetSize();
			for (int colorIndex=0;colorIndex<numColors;colorIndex++)
			{
				u16 color = colorKeeper[colorIndex];
				DBLWrite::WriteRecord(&color, pDBLFile);
			}
		}
	}
	else
	{
		// get the number of faces in the base mesh
		int *pPositionIndices;
		int numFaces;
		tupTriMesh.GetPositionIndices( &pPositionIndices, numFaces );
		
		Vector3 *pVertexColorVerts;
		int *pVertexColorIndices;
		int numVertexColorVerts;
		int numVertexColorFaces;
		
		bool bBuiltFakeColors = false;
		bool bOneColorForAll = false; // if all tristrips use the same color
		
		Vector3 *pVertexAlphaVerts;
		int *pVertexAlphaIndices;
		int numVertexAlphaVerts;
		int numVertexAlphaFaces;
		
		bool bBuiltFakeAlphas = false;
		
		
		// use the vertex colors from the modifier(s)
		if (pChildVertexColorMeshMapAgg && nNumColorSets>0)
		{
			TupTriMeshMap tupTriMeshMap(pChildVertexColorMeshMapAgg);
			tupTriMeshMap.GetMapVerts((float **) &pVertexColorVerts, numVertexColorVerts );
			tupTriMeshMap.GetMapIndices( &pVertexColorIndices, numVertexColorFaces );
			assert(numVertexColorFaces==numFaces);
		}
		else // if not then build a set of white colors
		{
			bBuiltFakeColors = true;
			pVertexColorVerts = new Vector3;
			pVertexColorVerts->Set(1.0f,1.0f,1.0f);
			numVertexColorVerts = 1;
			if (nNumColorSets==0)
			{
				bOneColorForAll = true; // all points in the mesh use one color
				pVertexColorIndices = NULL; // we don't need this one
			}
			else
			{
				// fake all the points in the mesh to point to the first color
				pVertexColorIndices = new int[numFaces*3];
				memset(pVertexColorIndices,0,sizeof(int)*numFaces*3);
				numVertexColorFaces = numFaces;
			}
		}
		
		// use the vertex alpha from the modifier(s)
		if (pChildVertexAlphaMeshMapAgg && nNumColorSets>0)
		{
			TupTriMeshMap tupTriMeshMap(pChildVertexAlphaMeshMapAgg);
			tupTriMeshMap.GetMapVerts((float **) &pVertexAlphaVerts, numVertexAlphaVerts );
			tupTriMeshMap.GetMapIndices( &pVertexAlphaIndices, numVertexAlphaFaces );
			assert(numVertexAlphaFaces==numFaces);
		}
		else // if not then build a set of white colors
		{
			bBuiltFakeAlphas = true;
			pVertexAlphaVerts = new Vector3;
			pVertexAlphaVerts->Set(1.0f,1.0f,1.0f);
			numVertexAlphaVerts = 1;
			if (nNumColorSets==0)
			{
				pVertexAlphaIndices = NULL; // we don't need this one
			}
			else
			{
				// fake all the points in the mesh to point to the first color
				pVertexAlphaIndices = new int[numFaces*3];
				memset(pVertexAlphaIndices,0,sizeof(int)*numFaces*3);
				numVertexAlphaFaces = numFaces;
			}
		}
		
		if (!bOneColorForAll) // save out a color for every point in mesh
		{
			TupperwareAggregate *pTriangleStripsAgg = tupTriMesh.GetTriangleStrips();
			assert(pTriangleStripsAgg);
			
			TupTriangleStripPool tupTriangleStripPool(pTriangleStripsAgg);
			int totalStrips = tupTriangleStripPool.GetNumTriangleStrips();
			for (int stripIndex=0;stripIndex<totalStrips;stripIndex++)
			{
				TupperwareAggregate *pTriangleStripAgg = tupTriangleStripPool.GetTriangleStrip(stripIndex);
				assert(pTriangleStripAgg);
				TupTriangleStrip tupTriangleStrip(pTriangleStripAgg);
				int *pFaceIndices;
				int *pFacePointIndices;
				int numFaceIndices;
				int numFacePointIndices;
				tupTriangleStrip.GetFaceIndices(&pFaceIndices,numFaceIndices);
				assert(numFaceIndices);
				tupTriangleStrip.GetFacePointIndices(&pFacePointIndices,numFacePointIndices);
				assert(numFacePointIndices);
				assert(numFaceIndices==numFacePointIndices);
				
				// output vertex colors for the strip
				for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
				{
					int faceIndex = pFaceIndices[pointIndex];
					int facePointIndex = pFacePointIndices[pointIndex];

					if (instance.VertexColor32Bit())
					{
						ts_bRGBA destColor32;
						// do vertex color
						{
							int vertexIndex = pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex];
							Vector3 &sourceColor = pVertexColorVerts[vertexIndex];
							destColor32.R = (u8)(sourceColor.x() *255.0f);
							destColor32.G = (u8)(sourceColor.y() *255.0f);
							destColor32.B = (u8)(sourceColor.z() *255.0f);
						}
						// do vertex alpha
						{
							int vertexIndex = pVertexAlphaIndices[(faceIndex<<1)+faceIndex+facePointIndex];
							Vector3 &sourceColor = pVertexAlphaVerts[vertexIndex];
							destColor32.A = (u8)(sourceColor.x() *255.0f);
						}
						DBLWrite::WriteRecord(&destColor32, pDBLFile);
					}
					else
					{
						u16 destColor16;
						int vertexIndex = pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex];
						Vector3 &sourceColor = pVertexColorVerts[vertexIndex];
						destColor16 = ((((u8)(sourceColor.x()*31.0f))&0x1f)<<11)|
							((((u8)(sourceColor.y()*63.0f))&0x3f)<<5)|
							((((u8)(sourceColor.z()*31.0f))&0x1f)<<0);
						DBLWrite::WriteRecord(&destColor16, pDBLFile);
					}
				}
			}
		}
		else // just save out 1 color
		{
			if (instance.VertexColor32Bit())
			{
				ts_bRGBA destColor32;
				Vector3 &sourceColor = pVertexColorVerts[0];
				destColor32.R = (u8)(sourceColor.x() *255.0f);
				destColor32.G = (u8)(sourceColor.y() *255.0f);
				destColor32.B = (u8)(sourceColor.z() *255.0f);
				Vector3 &sourceAlpha = pVertexAlphaVerts[0];
				destColor32.A = (u8)(sourceColor.x() *255.0f);
				DBLWrite::WriteRecord(&destColor32, pDBLFile);
			}
			else
			{
				u16 destColor16;
				Vector3 &sourceColor = pVertexColorVerts[0];
				destColor16 = ((((u8)(sourceColor.x()*31.0f))&0x1f)<<11)|
					((((u8)(sourceColor.y()*63.0f))&0x3f)<<5)|
					((((u8)(sourceColor.z()*31.0f))&0x1f)<<0);
				DBLWrite::WriteRecord(&destColor16, pDBLFile);
			}
		}
		if (bBuiltFakeColors)
		{
			delete pVertexColorVerts;
			delete pVertexColorIndices;
		}
		if (bBuiltFakeAlphas)
		{
			delete pVertexAlphaVerts;
			delete pVertexAlphaIndices;
		}
	}
	DBLUtil::AlignChunkDataQuadWord();
}
