////////////////////////////////////////////////////////////////////////////
//
// ExportPath
//
// Export Paths into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportPath.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 12/10/03   Time: 12:56p
 * Updated in $/Avalanche/Tools/TupperFilters/ExportPC
 * dbl chunk name change
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 12/09/03   Time: 4:03p
 * Updated in $/Avalanche/Tools/TupperFilters/ExportPC
 * path changes
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 12/09/03   Time: 3:38p
 * Updated in $/Avalanche/Tools/TupperFilters/ExportPC
 * added another 0 for pointers
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 12/09/03   Time: 11:48a
 * Updated in $/Avalanche/Tools/TupperFilters/ExportPC
 * first pass of new path curve export
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 1/23/03    Time: 10:10a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * added path name redirection
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/15/02    Time: 3:45p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * World Emitters
 * Config tree change for named points
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/23/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * changes to skip null pool entries
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 9:31a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Memory Leak
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/03/02    Time: 2:36p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Paths Exporting
 */

#include "ExportPch.h"
#include "ExportPath.h"

#define PATH_NAME_SIZE (64)

#define PF_LOOPED			(1<<0)
#define PF_FLAG1			(1<<1)
#define PF_FLAG2			(1<<2)
#define PF_FLAG3			(1<<3)
#define PF_FLAG4			(1<<4)
#define PF_FLAG5			(1<<5)
#define PF_FLAG6			(1<<6)
#define PF_FLAG7			(1<<7)

ExportPath::ExportPath(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager)
{
}

u32 ExportPath::WritePaths(TupScene &tupScene,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool)
{
	u32 totalPathChunkSize = 0;

	int totalPaths = tupScene.GetBaseAggregate()->FindScalarByKey("Annotate_TotalPaths")->GetAsInt();
	if (!totalPaths)
		return 0;

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	int *pPathNodeIndexTable = new int[totalPaths];
	for (int pathIndex = 0;pathIndex<totalPaths;pathIndex++)
		pPathNodeIndexTable[pathIndex] = -1; // mark as empty

	int *pPathSplineIndexTable = new int[totalPaths];

	int totalNewSplines = 0;
	int totalNodes = tupNodePool.GetNumNodes();
	pathIndex = 0;
	for (int nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);

		// if slot empty then continue
		if (!pNodeAgg)
			continue;

		TupperwareScalar *pPathIndexScalar = pNodeAgg->FindScalarByKey("Annotate_PathIndex");
		// check if this node is a valid instance
		if (pPathIndexScalar)
		{
			TupNode tupNode(pNodeAgg);

			int pathIndex = pPathIndexScalar->GetAsInt();
			assert(pathIndex<totalPaths);

			int objectRef;	
			tupNode.GetObjectReference(objectRef);
			assert(objectRef>=0);
			TupperwareAggregate *pBasePathAgg = AnnotateHelper::FindBasePath(tupObjectPool,objectRef);
			assert(pBasePathAgg);
			int totalSplines = 0;
			TupObject tupObject(pBasePathAgg);
			if (tupObject.GetObjectType()==TupObject::BEZIER_SHAPE)
			{
				TupBezierShape tupBezierShape(pBasePathAgg);
				totalSplines = tupBezierShape.GetNumSplines();
			}
			else if (tupObject.GetObjectType()==TupObject::SPLINE_SHAPE)
			{
				TupSplineShape tupSplineShape(pBasePathAgg);
				totalSplines = tupSplineShape.GetNumSplines();
				totalNewSplines += totalSplines;
			}
			// store path and spline index
			for (int splineIndex=0;splineIndex<totalSplines;splineIndex++)
			{
				pPathNodeIndexTable[pathIndex] = nodeIndex;
				pPathSplineIndexTable[pathIndex] = splineIndex;
				pathIndex++;
			}
		}
	}

#define OUTPUT_OLD_PATHS
#ifdef OUTPUT_OLD_PATHS

	DBLUtil::StartChunk( DB_PATHS, DB_PATHS_VERSION, DBL_CHUNK_NO_IGNORE, "1000" );
	DBLUtil::StartRecordList( totalPaths, RECORD_LIST_HAS_OFFSET_TABLE, 0 );

	char pathName[256];

	for (pathIndex = 0;pathIndex<totalPaths;pathIndex++)
	{
		DBLUtil::RecordListEntry();

		int nodeIndex = pPathNodeIndexTable[pathIndex];
		int splineIndex = pPathSplineIndexTable[pathIndex];

		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
		assert(pNodeAgg);
		TupNode tupNode(pNodeAgg);

		Graphics4x4 localToWorld;

		TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
		TupTransform tupTransform(pTransformAgg);
		if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
		{
			TupStaticMatrix tupStaticMatrix(pTransformAgg);
			float *pMatrix;
			tupStaticMatrix.GetMatrix(&pMatrix);

			localToWorld = Graphics4x4(
				pMatrix[0], pMatrix[1], pMatrix[2],
				pMatrix[4], pMatrix[5], pMatrix[6],
				pMatrix[8], pMatrix[9], pMatrix[10],
				pMatrix[12], pMatrix[13], pMatrix[14]);      // translation
		}
		else
		{
			localToWorld.Identity();
		}

		int objectRef;	
		tupNode.GetObjectReference(objectRef);
		assert(objectRef>=0);
		TupperwareAggregate *pBasePathAgg = AnnotateHelper::FindBasePath(tupObjectPool,objectRef);
		assert(pBasePathAgg);

		TupObject tupObject(pBasePathAgg);
		TupperwareAggregate *pBezierSplineAgg = NULL;
		int totalSplines;
		if (tupObject.GetObjectType()==TupObject::BEZIER_SHAPE)
		{
			TupBezierShape tupBezierShape(pBasePathAgg);
			totalSplines = tupBezierShape.GetNumSplines();
			assert(splineIndex<totalSplines);
			pBezierSplineAgg = tupBezierShape.GetSpline(splineIndex);
		}
		else if (tupObject.GetObjectType()==TupObject::SPLINE_SHAPE)
		{
			TupSplineShape tupSplineShape(pBasePathAgg);
			totalSplines = tupSplineShape.GetNumSplines();
			assert(splineIndex<totalSplines);
			TupperwareAggregate *pSplineAgg = tupSplineShape.GetSpline(splineIndex); 
			pBezierSplineAgg = pSplineAgg->FindAggregateByKey("BezierSpline"); // bezier spline added as a rogue aggregate for now
			assert(pBezierSplineAgg);
		}

		// set the path name
		char *pPathName=pNodeAgg->FindScalarByKey("Annotate_PathName")->GetAsString();
		if (totalSplines==1)
		{
			strcpy(pathName,pPathName);
		}
		else
		{
			sprintf(pathName,"%s(%d)",pPathName,splineIndex);
		}

		// make sure the last character is NULL
		pathName[PATH_NAME_SIZE-1] = '\0';
		DBLWrite::WriteData(pathName, PATH_NAME_SIZE,pDBLFile);

		TupBezierSpline tupBezierSpline(pBezierSplineAgg);

		int splineFlags;
		tupBezierSpline.GetFlags(splineFlags);

		int numPoints;
		Vector3 *pPointLocations;
		Vector3 *pInVectors;
		Vector3 *pOutVectors;
		int *pPointFlags;

		tupBezierSpline.GetPointLocations((float **)&pPointLocations,numPoints);
		tupBezierSpline.GetInVectors((float **)&pInVectors,numPoints);
		tupBezierSpline.GetOutVectors((float **)&pOutVectors,numPoints);
		tupBezierSpline.GetPointFlags(&pPointFlags,numPoints);

		u32 vertexCount = (numPoints*3)-2;
		u32 pathFlags = 0;
		// if a closed spline
		if (splineFlags & TupBezierSpline::FLAGS_CLOSED)
		{
			vertexCount += 3; // add 3 more points if looped
			pathFlags |= PF_LOOPED;
		}
		DBLWrite::WriteRecord(&vertexCount,pDBLFile);
		DBLWrite::WriteRecord(&pathFlags,pDBLFile);

		if (numPoints<2)
			throw(PluginException(ERROR_FATAL,"Export - Path %s %d has spline with %d points",pathName,splineIndex,numPoints));

		// do the points up to the end (but not including the end)
		for (int knotIndex=0;knotIndex<numPoints-1;knotIndex++)
		{
			// write point
			Vector3 point1 = pPointLocations[knotIndex] * localToWorld;

			DBLWrite::WriteRecord(&point1,pDBLFile);
			// write out/in vectors
			if (pPointFlags[knotIndex] & TupBezierSpline::FLAGS_LINE_TYPE_LINE)
			{
				// make the control handles on the line spaced at thirds
				Vector3 point2 = pPointLocations[knotIndex+1] * localToWorld;

				Vector3 lineVector = (point2-point1)/3.0f;
				Vector3 outVector = point1+lineVector;
				Vector3 inVector = point2-lineVector;
				DBLWrite::WriteRecord(&outVector,pDBLFile);
				DBLWrite::WriteRecord(&inVector,pDBLFile);
			}
			else
			{
				// save control handles
				Vector3 outVector = pOutVectors[knotIndex] * localToWorld;
				Vector3 inVector = pInVectors[knotIndex+1] * localToWorld;

				DBLWrite::WriteRecord(&outVector,pDBLFile);
				DBLWrite::WriteRecord(&inVector,pDBLFile);
			}
		}
		// save the end point
		Vector3 endPoint = pPointLocations[numPoints-1] * localToWorld;
		DBLWrite::WriteRecord(&endPoint,pDBLFile);

		// if a closed spline then save 3 more
		if (splineFlags & TupBezierSpline::FLAGS_CLOSED)
		{
			Vector3 startPoint = pPointLocations[0] * localToWorld;
			if (pPointFlags[numPoints-1] & TupBezierSpline::FLAGS_LINE_TYPE_LINE)
			{
				// make the control handles on the line spaced at thirds
				Vector3 lineVector = (startPoint-endPoint)/3.0f;
				Vector3 outVector = endPoint+lineVector;
				Vector3 inVector = startPoint-lineVector;
				DBLWrite::WriteRecord(&outVector,pDBLFile);
				DBLWrite::WriteRecord(&inVector,pDBLFile);
			}
			else
			{
				// save control handles
				Vector3 outVector = pOutVectors[numPoints-1] * localToWorld;
				Vector3 inVector = pInVectors[0] * localToWorld;
				DBLWrite::WriteRecord(&outVector,pDBLFile);
				DBLWrite::WriteRecord(&inVector,pDBLFile);
			}
			// first point again
			DBLWrite::WriteRecord(&startPoint,pDBLFile);
		}
		DBLUtil::AlignFile();
	}

	DBLUtil::FinishRecordList();
	totalPathChunkSize += DBLUtil::FinishChunk(FALSE);

#endif // OUTPUT_OLD_PATHS

	if (totalPaths == totalNewSplines)
	{
		DBLUtil::StartChunk( DB_CURVES, DB_CURVES_VERSION, DBL_CHUNK_NO_IGNORE, "1000" );
		DBLUtil::StartRecordList( totalPaths, RECORD_LIST_HAS_OFFSET_TABLE, 0 );

#ifdef EXPORT_PS2
		DBLUtil::AlignChunkDataQuadWord();
#endif

		for (pathIndex = 0;pathIndex<totalPaths;pathIndex++)
		{
			DBLUtil::RecordListEntry();

			int nodeIndex = pPathNodeIndexTable[pathIndex];
			int splineIndex = pPathSplineIndexTable[pathIndex];

			TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
			assert(pNodeAgg);
			TupNode tupNode(pNodeAgg);

			Graphics4x4 localToWorld;

			TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
			TupTransform tupTransform(pTransformAgg);
			if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
			{
				TupStaticMatrix tupStaticMatrix(pTransformAgg);
				float *pMatrix;
				tupStaticMatrix.GetMatrix(&pMatrix);

				localToWorld = Graphics4x4(
					pMatrix[0], pMatrix[1], pMatrix[2],
					pMatrix[4], pMatrix[5], pMatrix[6],
					pMatrix[8], pMatrix[9], pMatrix[10],
					pMatrix[12], pMatrix[13], pMatrix[14]);      // translation
			}
			else
			{
				localToWorld.Identity();
			}

			int objectRef;	
			tupNode.GetObjectReference(objectRef);
			assert(objectRef>=0);
			TupperwareAggregate *pBasePathAgg = AnnotateHelper::FindBasePath(tupObjectPool,objectRef);
			assert(pBasePathAgg);

			TupObject tupObject(pBasePathAgg);
			int totalSplines;
			assert(tupObject.GetObjectType()==TupObject::SPLINE_SHAPE);
			TupSplineShape tupSplineShape(pBasePathAgg);
			totalSplines = tupSplineShape.GetNumSplines();
			assert(splineIndex<totalSplines);
			TupperwareAggregate *pSplineAgg = tupSplineShape.GetSpline(splineIndex); 
			assert(pSplineAgg);

			TupSpline tupSpline(pSplineAgg);

			int splineFlags;
			tupSpline.GetFlags(splineFlags);

			int numPoints;
			int numKnots;
			Vector4 *pControlPoints;
			float *pKnots;

			tupSpline.GetControlPoints((float **)&pControlPoints,numPoints);
			tupSpline.GetKnots(&pKnots,numKnots);

			u32 reserved = 0;
			DBLWrite::WriteRecord(&splineFlags,pDBLFile);
			DBLWrite::WriteRecord(&numPoints,pDBLFile);
			DBLWrite::WriteRecord(&numKnots,pDBLFile);
			DBLWrite::WriteRecord(&reserved,pDBLFile); // for *pControlPoints
			DBLWrite::WriteRecord(&reserved,pDBLFile); // for *pKnots
			DBLWrite::WriteRecord(&reserved,pDBLFile); // for *pName
			DBLWrite::WriteRecord(&reserved,pDBLFile); // for pad1
			DBLWrite::WriteRecord(&reserved,pDBLFile); // for pad2

			for (int controlPointIndex=0;controlPointIndex<numPoints;controlPointIndex++)
			{
				// write point
				Vector3 point1 = pControlPoints[controlPointIndex];
				point1 = point1 * localToWorld;

				DBLWrite::WriteRecord(&point1,pDBLFile);
// four bytes per vertex				
#ifdef EXPORT_PS2
				float pad=0.0;
				DBLWrite::WriteRecord(&pad,pDBLFile);
#endif		
			}
			for (int knotIndex=0;knotIndex<numKnots;knotIndex++)
			{
				DBLWrite::WriteRecord(&pKnots[knotIndex],pDBLFile);
			}
			// set the path name
			char *pPathName=pNodeAgg->FindScalarByKey("Annotate_PathName")->GetAsString();
			TupString pathName(pPathName);
			
			if (totalSplines!=1)
			{
				char temp[20];
				sprintf(temp,"(%d)",splineIndex);
				pathName += temp;
			}
			DBLWrite::WriteData((const char *)pathName, pathName.GetLength()+1,pDBLFile);

#ifdef EXPORT_PS2
			DBLUtil::AlignChunkData32Byte();
#else
			DBLUtil::AlignFile();
#endif
		}
		DBLUtil::FinishRecordList();
		totalPathChunkSize += DBLUtil::FinishChunk(FALSE);
	}

	delete [] pPathNodeIndexTable;
	delete [] pPathSplineIndexTable;

	return totalPathChunkSize;

}
