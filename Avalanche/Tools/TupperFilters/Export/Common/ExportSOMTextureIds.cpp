////////////////////////////////////////////////////////////////////////////
//
// ExportSOMTextureIDs
//
// Export Texture ids used for the meshes
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportSOMTextureIds.h"

ExportSOMTextureIDs::ExportSOMTextureIDs(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupImagePool &tupImagePool,int currentPartition) :
m_pDBLFileName(pDBLFileName),
m_options(options),
m_dataManager(dataManager),
m_tupImagePool(tupImagePool),
m_currentPartition(currentPartition)
{
}

void ExportSOMTextureIDs::StartChunk(int numMeshes)
{
	DBLUtil::StartChunk(DB_SOMTEXTUREIDARRAY, DB_SOMTEXTUREIDARRAY_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	/* start record list-- writes out dummy data, to be replaced later */
	DBLUtil::StartRecordList(numMeshes, RECORD_LIST_HAS_OFFSET_TABLE, 0);

	/* alignment */
	DBLUtil::AlignFile();
}

/* write out SOM model offset table after all SOM models have been output */
u32 ExportSOMTextureIDs::FinishChunk(void)
{
	/* finish record list */
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk(FALSE);
}

void ExportSOMTextureIDs::WriteTextureIDs(TupTriMesh &tupTriMesh)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();
	DBLUtil::RecordListEntry();

	m_pTupTriMesh = &tupTriMesh;

	// get which material this mesh will use
	TupperwareAggregate *pObjectAgg = m_pTupTriMesh->GetBaseAggregate();
	// there should be only one material per mesh
	TupperwareList *pMeshImageRefList = pObjectAgg->FindListByKey("Annotate_MeshImageRefs");
	int numValidRefs = 0;
	int numRefs = 0;
	int *pImageRefData = NULL;
	if (pMeshImageRefList)
	{
		numRefs = pMeshImageRefList->GetLength();
		pImageRefData = pMeshImageRefList->GetAsInt();
		for (int refIndex=0;refIndex<numRefs;refIndex++)
		{
			if (pImageRefData[refIndex]!=-1)
				numValidRefs++;
		}
	}

	// write the number of texture refs
	DBLWrite::WriteRecord(&numValidRefs, pDBLFile);

	for (int refIndex=0;refIndex<numRefs;refIndex++)
	{
		int ref = pImageRefData[refIndex];
		if (ref==-1)
			continue;
		TupperwareAggregate *pImageAgg = m_tupImagePool.GetImage(ref);
		ASSERT(pImageAgg);

		TupperwareList *pTextureIDList = pImageAgg->FindListByKey("Annotate_TextureIDs");
		ASSERT(pTextureIDList);
		ASSERT(m_currentPartition<pTextureIDList->GetLength());
		int *pTextureIDData = pTextureIDList->GetAsInt();
		int id = pTextureIDData[m_currentPartition];
		// write id for texture
		DBLWrite::WriteRecord(&id,pDBLFile);
	}
}

