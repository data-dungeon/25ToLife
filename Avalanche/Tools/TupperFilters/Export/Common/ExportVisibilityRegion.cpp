////////////////////////////////////////////////////////////////////////////
//
// ExportVisibilityRegion
//
// Export the visibility regions
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportVisibilityRegion.h"

ExportVisibilityRegion::ExportVisibilityRegion(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager) :
m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager)
{
}

class VisibilitySortData
{
public:
	int m_setIndex;
	float m_size;
	
	VisibilitySortData(void) : m_setIndex(-1),m_size(0)
	{
	}
	
	bool VisibilitySortData::operator==(const VisibilitySortData& other) const
	{
		return (m_size==other.m_size);
	}
	
	bool VisibilitySortData::operator<=(const VisibilitySortData& other) const
	{
		return (m_size<=other.m_size);
	}
	
	bool VisibilitySortData::operator>=(const VisibilitySortData& other) const
	{
		return (m_size>=other.m_size);
	}
	
	bool VisibilitySortData::operator<(const VisibilitySortData& other) const
	{
		return (m_size<other.m_size);
	}
	
	bool VisibilitySortData::operator>(const VisibilitySortData& other) const
	{
		return (m_size>other.m_size);
	}
};

u32 ExportVisibilityRegion::WriteVisibilityRegions(TupScene &tupScene,TupNodePool &tupNodePool,bool bBuildPartitions)
{
	TupArray<int> visibilitySetNodeRefs;
	tupScene.GetBaseAggregate()->FindListByKey("Annotate_VisibilitySetNodeRefs")->GetAsInt(visibilitySetNodeRefs);
	TupArray<int> visibilitySetSceneTreeIndices;
	tupScene.GetBaseAggregate()->FindListByKey("ExportDBL_VisibilitySetSceneTreeIndices")->GetAsInt(visibilitySetSceneTreeIndices);

	int totalVisibilitySets = visibilitySetNodeRefs.GetSize();

	//-- start chunk
	DBLUtil::StartChunk( DB_VIS_REGIONS, DB_VIS_REGIONS_VERSION, DBL_CHUNK_NO_IGNORE, "1000" );
	DBLUtil::StartRecordList( totalVisibilitySets, RECORD_LIST_HAS_OFFSET_TABLE, 0 );

	TupArray<VisibilitySortData> sortData;
	sortData.SetSize(totalVisibilitySets);
	int setIndex;
	for (setIndex=1;setIndex<totalVisibilitySets;setIndex++)
	{
		VisibilitySortData &sortEntry = sortData[setIndex];
		sortEntry.m_setIndex = setIndex;

		int nodeRef = visibilitySetNodeRefs[setIndex];
		
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeRef);

		Vector3 extents = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBExtents")->GetAsFloat();
		extents*=2.0f;
		sortEntry.m_size = extents.x( ) * extents.y( ) * extents.z( );
	}
	VisibilitySortData &worldSortEntry = sortData[0];
	worldSortEntry.m_setIndex = 0;
	worldSortEntry.m_size = MAXFLOAT32;

	// smallest to largest (world should be last)
	TupSort<VisibilitySortData>::SortAscending(sortData.GetData(),totalVisibilitySets);

	// build index array from unsorted list to sorted list
	TupArray<int> setToSortedIndexArray;
	setToSortedIndexArray.SetSize(totalVisibilitySets);
	int setIndexSorted;
	for (setIndexSorted=0;setIndexSorted<totalVisibilitySets;setIndexSorted++)
	{
		setToSortedIndexArray[sortData[setIndexSorted].m_setIndex] = setIndexSorted;
	}

	for (setIndexSorted=0;setIndexSorted<totalVisibilitySets;setIndexSorted++)
	{
		int setIndex = sortData[setIndexSorted].m_setIndex;

		DBLUtil::RecordListEntry();

		int nodeRef = visibilitySetNodeRefs[setIndex];
		int sceneTreeIndex = visibilitySetSceneTreeIndices[setIndex];

		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeRef);

		TupNode tupNode(pNodeAgg);
		char *pName;
		tupNode.GetName(&pName);

		ts_BoundingBox boundingBox;
		boundingBox.Min = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
		boundingBox.Max = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();

		TupArray<int> visibilitySetLinks;
		pNodeAgg->FindListByKey("Annotate_VisibilitySetLinks")->GetAsInt(visibilitySetLinks);
		int numLinks = visibilitySetLinks.GetSize();

		//-- build output record ( array of 1 is min, expand more if necessary )
		int nSize = sizeof( CVisRegion );
		if ( numLinks > 1 )
		{
			nSize += ( numLinks - 1 ) * sizeof( CVisRegion::VisRegionLinks );
		}
		CVisRegion* pExportRecord = (CVisRegion*) malloc( nSize );

		int nameLength = strlen(pName);
		if (nameLength>VISREGION_NAME_LENGTH-1)
		{
			pExportRecord->SetName(&pName[nameLength-(VISREGION_NAME_LENGTH-1)]); // use the right part of the name
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning - Visibility Set Name \"%s\" exceeds 31 characters.  Truncating to \"%s\".",pName,pExportRecord->GetName());
		}
		else
		{
			pExportRecord->SetName(pName);
		}

		s32 partitionID = (setIndex==0 || !bBuildPartitions) ? -1 : setIndex;
		pExportRecord->SetPartitionID(partitionID);
		pExportRecord->SetBoundingBox( &boundingBox);
		pExportRecord->SetSiblingIndex( visibilitySetSceneTreeIndices[setIndex] );
		pExportRecord->SetVisibleRegionCount( numLinks );

		int flags = 0; // start with being 
		if (pNodeAgg->FindScalarByKey("Annotate_RealtimeCameraRegion")->GetAsInt()==0)
		{
			flags |= CVisRegion::CAMERA_SKIP_REGION; // set flag if the camera skips region
		}

		pExportRecord->SetFlags(flags); // none for now

		DirCos3x3 obbOrientation = *(DirCos3x3 *)pNodeAgg->FindListByKey("Bounding_OBBOrientation")->GetAsFloat();
		Vector3 obbCenter = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBCenter")->GetAsFloat();
		ts_BoundingBox obbBoundingBox;
		obbBoundingBox.Max = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_OBBExtents")->GetAsFloat();
		obbBoundingBox.Min = -obbBoundingBox.Max;

		pExportRecord->SetOBBOrientation(&obbOrientation);
		pExportRecord->SetOBBCenter(&obbCenter);
		pExportRecord->SetOBBBoundingBox(&obbBoundingBox);
		pExportRecord->SetOBB((OBB *)NULL);

		for (int linkIndex =  0 ; linkIndex < numLinks ; linkIndex++ )
		{
			// the first shall be last
			int remappedLinkIndex = setToSortedIndexArray[visibilitySetLinks[linkIndex]];
			pExportRecord->SetVisibleRegionIndex(linkIndex,remappedLinkIndex);
			pExportRecord->SetVisibleRegionFlags(linkIndex,0); // none for now
		}
		
		//-- write
		DBLWrite::WriteRecord( pExportRecord, DBLUtil::GetDBLFile() );
		DBLUtil::AlignFile();
		
		//-- free space
		free( pExportRecord );
	}
	DBLUtil::FinishRecordList();
	return DBLUtil::FinishChunk( FALSE );
}

