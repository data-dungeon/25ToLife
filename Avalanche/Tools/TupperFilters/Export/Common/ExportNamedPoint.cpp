////////////////////////////////////////////////////////////////////////////
//
// ExportNamedPoint
//
// Export Named Points into the DBL file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportNamedPoint.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 5:34p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 5  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:54p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 7/15/02    Time: 3:45p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * World Emitters
 * Config tree change for named points
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 11:25a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Named Points Exporting
 */

#include "ExportPch.h"
#include "ExportNamedPoint.h"


ExportNamedPoint::ExportNamedPoint(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager), m_tupConfigRoot(tupConfigRoot)
{
}

u32 ExportNamedPoint::WriteNamedPoints(TupNodePool &tupNodePool)
{
	int totalNamedPoints = 0;
	int totalNodes = tupNodePool.GetNumNodes();
	int nodeIndex;
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);

		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many named points we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey("instance_type");
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
		// if set to a world emitter
		if (!strcmp(tupKeyValueContext.GetValueAsString(0),"named_point"))
		{
			totalNamedPoints++;
		}
	}
	if (!totalNamedPoints)
		return 0;

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	DBLUtil::StartChunk(DB_NAMED_POINTS, DB_NAMED_POINTS_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
	/* add the record list (dummy data is written and later replaced when the chunk is finished) */
	DBLUtil::StartRecordList(totalNamedPoints, RECORD_LIST_HAS_OFFSET_TABLE, 0);
	/* write each named point */
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);

		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many named points we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey("instance_type");
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
		// if set to a world emitter
		if (!strcmp(tupKeyValueContext.GetValueAsString(0),"named_point"))
		{
			DBLUtil::RecordListEntry();
			// Get Transformation information
			TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
			Vector3 vOrigin;
			vOrigin.Set(0.0f,0.0f,0.0f); // default is middle of world

			TupTransform tupTransform(pTransformAgg);
			if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
			{
				TupStaticMatrix tupStaticMatrix(pTransformAgg);
				float *pMatrix;
				tupStaticMatrix.GetMatrix(&pMatrix);
				vOrigin.x( pMatrix[12] );
				vOrigin.y( pMatrix[13] );
				vOrigin.z( pMatrix[14] ); // get translation off of matrix
			}
			// write location
			DBLWrite::WriteRecord(&vOrigin, pDBLFile);
			// write the name
			char *pName;
			tupNode.GetName(&pName);
			DBLWrite::WriteData(pName,strlen(pName)+1,pDBLFile);
			DBLUtil::AlignFile();
		}
	}
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}
