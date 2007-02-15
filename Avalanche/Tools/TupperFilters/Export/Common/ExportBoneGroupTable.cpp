////////////////////////////////////////////////////////////////////////////
//
// ExportBoneGroupTable
//
// Export the names of the bones, their groups and any mirror bones
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportBoneGroupTable.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/02/03   Time: 2:46p
 * Updated in $/Avalanche/Tools/TupperFilters/ExportPC
 * new bone group table additions
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/03/02    Time: 3:28p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * First Pass at bone export
 */

#include "ExportPch.h"
#include "ExportBoneGroupTable.h"
#include "BoneGroupTable.h"

ExportBoneGroupTable::ExportBoneGroupTable(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager)
{
}

u32 ExportBoneGroupTable::WriteBoneGroupTable(BoneGroupTable &boneGroupTable)
{
	FILE *pDBLFile = DBLUtil::GetDBLFile();

	int numEntries = boneGroupTable.GetBoneGroupTableSize();
	if (!numEntries)
		return 0;

	int mirrorGroups[32];
	bool boneGroupIsMirrored[32];
	for (int i=0;i<32;i++)
	{
		boneGroupIsMirrored[i] = false;
		mirrorGroups[i] = i;
	}

	DBLUtil::StartChunk(DB_BONEINFO, DB_BONEINFO_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	ts_BoneInfo boneInfo;
	memset(&boneInfo,0,sizeof(boneInfo));

	boneInfo.numGBEntries = numEntries;
	int numMirrorErrors = 0;
	int numMirrors = 0;
	// go through and figure out which bone groups are mirrored to other bone groups
	for (i=0;i<numEntries;i++)
	{
		if (boneGroupTable.GetBoneGroupTableMirrorIndex(i)!=-1)
		{
			int boneGroupID = boneGroupTable.GetBoneGroupTableBoneGroup(i);
			int boneGroupIDMirror = boneGroupTable.GetBoneGroupTableBoneGroup(boneGroupTable.GetBoneGroupTableMirrorIndex(i));
			// if not mirrored yet
			if (!boneGroupIsMirrored[boneGroupID])
			{
				mirrorGroups[boneGroupID] = boneGroupIDMirror;
				boneGroupIsMirrored[boneGroupID] = true;
			}
			else if (mirrorGroups[boneGroupID] != boneGroupIDMirror)
			{
				// mirror belongs to another group than a previous mirror
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_RED,"Bone %s attempting to set bone group %d mirror to %d already set to %d",
					boneGroupTable.GetBoneGroupTableName(i),boneGroupID,boneGroupIDMirror,mirrorGroups[boneGroupID]);
				numMirrorErrors++;
			}

			// if not mirrored yet
			if (!boneGroupIsMirrored[boneGroupIDMirror])
			{
				mirrorGroups[boneGroupIDMirror] = boneGroupID;
				boneGroupIsMirrored[boneGroupIDMirror] = true;
			}
			else if (mirrorGroups[boneGroupIDMirror] != boneGroupID)
			{
				// mirror belongs to another group than a previous mirror
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_RED,"Bone %s mirror attempting to set bone group %d mirror to %d already set to %d",
					boneGroupTable.GetBoneGroupTableName(i),boneGroupIDMirror,boneGroupID,mirrorGroups[boneGroupIDMirror]);
				numMirrorErrors++;
			}

			numMirrors++;
		}
	}
	// next pass is to make sure that all non-mirrored bones don't belong to mirrored groups
	for (i=0;i<numEntries;i++)
	{
		if (boneGroupTable.GetBoneGroupTableMirrorIndex(i)==-1)
		{
			int boneGroupID = boneGroupTable.GetBoneGroupTableBoneGroup(i);
			// if this group is mirrored we have a problem
			if (boneGroupIsMirrored[boneGroupID] && mirrorGroups[boneGroupID]!=boneGroupID)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_RED,"Bone %s does not have a mirror but is using bone group %d that has mirrors to group %d",
					boneGroupTable.GetBoneGroupTableName(i),boneGroupID,mirrorGroups[boneGroupID]);
				numMirrorErrors++;
			}
		}
	}

	if (numMirrorErrors)
	{
		throw(PluginException(ERROR_FATAL,"%d Errors processing bone group mirrors",numMirrorErrors));
	}

	boneInfo.numLREntries = numMirrors;

	// write out the main header that provides the number of entries and the number of mirrors
	DBLWrite::WriteRecord(&boneInfo,pDBLFile);
	// write out the entries
	for (i=0;i<numEntries;i++)
	{
		ts_GroupBoneMap groupBoneMap;
		groupBoneMap.boneGroupID = boneGroupTable.GetBoneGroupTableBoneGroup(i);
		strncpy(groupBoneMap.cBoneName,boneGroupTable.GetBoneGroupTableName(i),MAX_BONENAME_LEN);
		groupBoneMap.cBoneName[MAX_BONENAME_LEN-1] = '\0';
		DBLWrite::WriteRecord(&groupBoneMap,pDBLFile);
	}
	// write out the mirror entries
	for (i=0;i<numEntries;i++)
	{
		if (boneGroupTable.GetBoneGroupTableMirrorIndex(i)!=-1)
		{
			ts_LeftRightMap leftRightMap;
			strncpy(leftRightMap.cBoneName1,boneGroupTable.GetBoneGroupTableName(i),MAX_BONENAME_LEN);
			leftRightMap.cBoneName1[MAX_BONENAME_LEN-1] = '\0';
			strncpy(leftRightMap.cBoneName2,boneGroupTable.GetBoneGroupTableMirrorName(i),MAX_BONENAME_LEN);
			leftRightMap.cBoneName2[MAX_BONENAME_LEN-1] = '\0';
			DBLWrite::WriteRecord(&leftRightMap,pDBLFile);
		}
	}
	u32 chunk1Size = DBLUtil::FinishChunk(FALSE);

	// bone group mirrors
	DBLUtil::StartChunk(DB_BONE_GROUP_MIRRORS, DB_BONE_GROUP_MIRRORS_VERSION, DBL_CHUNK_NO_IGNORE, "1000");
	for (i=0;i<32;i++)
	{
		DBLWrite::WriteRecord(&mirrorGroups[i],pDBLFile);
	}
	u32 chunk2Size = DBLUtil::FinishChunk(FALSE);
	return chunk1Size+chunk2Size;

}

