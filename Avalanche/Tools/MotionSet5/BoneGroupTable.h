////////////////////////////////////////////////////////////////////////////
//
// BoneGroupTable
//
// Deal with the bone group table file and queries
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#define BONE_GROUP_TABLE_H

class BoneGroupTableEntry
{
public:
	TupString m_name;
	TupString m_mirrorName;
	int m_boneGroup;
	int m_mirrorIndex;

};

class BoneGroupTable
{
	Keeper<TupString> m_boneGroupNames;
	TupArray<int> m_boneGroupNameValues;

	Keeper<TupString> m_boneGroupTableNames;
	TupArray<BoneGroupTableEntry> m_boneGroupTableEntries;

	Keeper<TupString> m_invalidModelChannelNames;
	TupArray<int> m_invalidModelChannelValues;

	Keeper<TupString> m_invalidMotionChannelNames;
	TupArray<int> m_invalidMotionChannelValues;

	int m_numErrors;

public:
	BoneGroupTable(TupperwareAggregate *pTupRoot);

	// from m_boneGroupTableNames & m_boneGroupTableEntries
	int GetBoneGroup(const char *pString) const;
	int GetBoneGroupTableSize(void) const;
	const char *GetBoneGroupTableName(int index) const;
	const char *GetBoneGroupTableMirrorName(int index) const;
	int GetBoneGroupTableBoneGroup(int index) const;
	int GetBoneGroupTableMirrorIndex(int index) const;

	// from m_invalidModelChannelNames & m_invalidMotionChannelValues
	int IsValidModelChannel(const char *pChannelName) const;

	// from m_invalidMotionChannelNames & m_invalidMotionChannelValues
	bool IsValidMotionChannel(const char *pChannelName) const;

	// from m_boneGroupNames & m_boneGroupNameValues
	int GetBoneGroupFromName(const char *pBoneGroupName) const;
	const char *GetBoneGroupNameFromID(int id) const;

	int GetNumErrors(void) const;
private:
	void GetBoneGroupNameData(TupperwareAggregate *pTupRoot);
	void GetBoneGroupData(TupperwareAggregate *pTupRoot);
	void GetInvalidModelChannelData(TupperwareAggregate *pTupRoot);
	void GetInvalidMotionChannelData(TupperwareAggregate *pTupRoot);

};

