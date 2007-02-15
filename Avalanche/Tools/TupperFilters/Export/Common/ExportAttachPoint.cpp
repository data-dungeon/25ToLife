////////////////////////////////////////////////////////////////////////////
//
// ExportAttachPoint
//
// Export Attach Points into the DBL file
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportAttachPoint.h"


ExportAttachPoint::ExportAttachPoint(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot) :
m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager), m_tupConfigRoot(tupConfigRoot)
{
}

u32 ExportAttachPoint::WriteAttachPoints(TupNodePool &tupNodePool)
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

		TupperwareScalar *pForceBoneToAttachPointScalar = pNodeAgg->FindScalarByKey("Annotate_ForceBoneToAttachPoint");

		// if set to an attach point
		if (pForceBoneToAttachPointScalar || !strcmp(tupKeyValueContext.GetValueAsString(0),"attach_point"))
		{
			totalNamedPoints++;
		}
	}
	if (!totalNamedPoints)
		return 0;

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	DBLUtil::StartChunk(DB_ATTACH_POINTS, DB_ATTACH_POINTS_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
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

		TupperwareScalar *pForceBoneToAttachPointScalar = pNodeAgg->FindScalarByKey("Annotate_ForceBoneToAttachPoint");

		// if set to an attach point
		if (pForceBoneToAttachPointScalar || !strcmp(tupKeyValueContext.GetValueAsString(0),"attach_point"))
		{
			ts_AttachPoint writeData;
			memset(&writeData,0,sizeof(writeData));
			writeData.m_matrix.Identity();
			// Get Transformation information
			TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );

			// for now use the local to world matrix (like the instance)
			TupTransform tupTransform(pTransformAgg);
			if (tupTransform.GetTransformType()==TupTransform::STATIC_MATRIX)
			{
				TupStaticMatrix tupStaticMatrix(pTransformAgg);
				float *pMatrix;
				tupStaticMatrix.GetMatrix(&pMatrix);

				writeData.m_matrix = Graphics4x4(
					pMatrix[8], pMatrix[9], pMatrix[10],
					pMatrix[4], pMatrix[5], pMatrix[6],
					pMatrix[0], pMatrix[1], pMatrix[2],
					pMatrix[12], pMatrix[13], pMatrix[14]);      // translation
			}

			// write the name
			char *pName;
			tupNode.GetName(&pName);
			TupString name(pName);
			name.MakeLower();
			strncpy(writeData.m_cAttachPointName,name,MAX_ATTACHPOINTNAME_LEN);
			writeData.m_cAttachPointName[MAX_ATTACHPOINTNAME_LEN-1] = '\0';

			float *pMatrix = (float *)&writeData.m_matrix;
			// write the matrix
			for (int x = 0; x< 16; x++)
			{
				DBLWrite::WriteRecord(pMatrix+x, pDBLFile);
			}

			DBLWrite::WriteData(&writeData.m_cAttachPointName,MAX_ATTACHPOINTNAME_LEN,pDBLFile);
			DBLWrite::WriteData(&writeData.m_cParentBoneName,MAX_BONENAME_LEN,pDBLFile);
			DBLWrite::WriteData(&writeData.pad,sizeof(writeData.pad),pDBLFile);
			
			DBLUtil::AlignFile();
		}
	}
	return DBLUtil::FinishChunk(FALSE);
}
