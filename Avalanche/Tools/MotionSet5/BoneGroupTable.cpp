////////////////////////////////////////////////////////////////////////////
//
// BoneGroupTable
//
// Deal with the bone group table file and queries
//
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BoneGroupTable.h"

BoneGroupTable::BoneGroupTable(TupperwareAggregate *pTupRoot) 
{
	m_numErrors = 0;

	GetBoneGroupNameData(pTupRoot);
	GetBoneGroupData(pTupRoot);
	GetInvalidModelChannelData(pTupRoot);
	GetInvalidMotionChannelData(pTupRoot);
}

static bool WildCardMatch(const TupString &testString, const TupString &matchString)
{
	// do match string first (if any characters following "^" character exist in the test string then we have a match
	int matchAllStart = matchString.Find('^');
	if (matchAllStart!=-1)
	{																														
		for (int matchAllIndex = matchAllStart+1; matchAllIndex < matchString.GetLength(); matchAllIndex++)
		{
			if (testString.Find(matchString[matchAllIndex])!=-1)
				return true;
		}
	}

	int testStringLength = testString.GetLength();
	int matchStringLength = matchString.GetLength();

	for (int charIndex = 0; charIndex < testStringLength; charIndex++)
	{
		if (charIndex < matchStringLength)
		{
			if (matchString[charIndex] == '*') 
			{
				return true; // match due to wildcard
			}
			else if (matchString[charIndex] == '?')
			{
				continue; // keep looking 
			}
			else if (testString[charIndex] != matchString[charIndex])
			{
				return false;
			}
		}
		else // no more characters in the match string
		{
			return false;
		}
	}
	// if we got here the test string matches the match string up to the length of the test string
	if (testStringLength < matchStringLength)
	{
		// if there is an "*" in the match string after the test string length then we have a match
		if (matchString[testStringLength]=='*')
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	// same size strings and all characters matched
	return true;
}

int BoneGroupTable::GetBoneGroup(const char *pString) const
{
	TupString tempName(pString);
	tempName.MakeLower();
	for (int nameIndex = 0; nameIndex < m_boneGroupTableNames.GetSize(); nameIndex++)
	{
		if (WildCardMatch(tempName,m_boneGroupTableNames[nameIndex]))
		{
			return m_boneGroupTableEntries[nameIndex].m_boneGroup;
		}
	}
	return -1;
}

int BoneGroupTable::GetBoneGroupTableSize(void) const
{
	return m_boneGroupTableEntries.GetSize();
}

const char *BoneGroupTable::GetBoneGroupTableName(int index) const
{
	return m_boneGroupTableEntries[index].m_name;
}

const char *BoneGroupTable::GetBoneGroupTableMirrorName(int index) const
{
	if (m_boneGroupTableEntries[index].m_mirrorName.GetLength())
	{
		return m_boneGroupTableEntries[index].m_mirrorName;
	}
	else
	{
		return NULL;
	}
}

int BoneGroupTable::GetBoneGroupTableBoneGroup(int index) const
{
	return m_boneGroupTableEntries[index].m_boneGroup;
}

int BoneGroupTable::GetBoneGroupTableMirrorIndex(int index) const
{
	return m_boneGroupTableEntries[index].m_mirrorIndex;
}

void BoneGroupTable::GetBoneGroupNameData(TupperwareAggregate *pTupRoot)
{
	TupperwareAggregate *pBoneGroupNamesAgg = pTupRoot->FindAggregateByKey("BoneGroupNames");
	if (pBoneGroupNamesAgg)
	{
		for (TupperwareIterator iter = pBoneGroupNamesAgg->FindAtomByKey("Entry"); !iter.Finished(); iter.Next()) 
		{
			TupperwareAtom *pItemAtom = iter.GetAsAtom();

			if (!pItemAtom->GetName())
			{
				printf("BoneGroupNames - Entry specified without the name\n");
				m_numErrors++;
				continue;
			}

			if (pItemAtom->GetAtomType()!=TupperwareAtom::ATOM_SCALAR)
			{
				printf("Invalid BoneGroupName %s - Use Entry ""NAME"" = VALUE\n",pItemAtom->GetName());
				m_numErrors++;
				continue;
			}

			TupperwareScalar *pItemScalar = (TupperwareScalar *) pItemAtom;
			if (pItemScalar->GetDataType()!=TupperwareScalar::DATA_INT)
			{
				printf("Invalid BoneGroupName %s - Use Entry ""NAME"" = VALUE\n",pItemAtom->GetName());
				m_numErrors++;
				continue;
			}

			TupString name = pItemAtom->GetName();
			name.MakeLower();

			int oldIndex = m_boneGroupNames.GetSize();
			int newIndex = m_boneGroupNames.Add(name);
			// if we added a new name
			if (newIndex==oldIndex)
			{
				int value = pItemScalar->GetAsInt();
				if (value<0||value>31)
				{
					printf("BoneGroupsNames - BoneGroup for name %s needs to be between 0 and 31 currently set to %d\n",pItemAtom->GetName(),value);
					m_numErrors++;
					continue;
				}

				m_boneGroupNameValues.Add(value);
			}
			else
			{
				printf("Duplicate BoneGroupName found %s\n",pItemAtom->GetName());
				m_numErrors++;
			}
		}
	}
}

void BoneGroupTable::GetBoneGroupData(TupperwareAggregate *pTupRoot)
{
	TupperwareAggregate *pBoneGroupAgg = pTupRoot->FindAggregateByKey("BoneGroups");
	if (!pBoneGroupAgg)
	{
		printf("Cannot find BoneGroups within group table file\n");
		m_numErrors++;
		return;
	}

	for (TupperwareIterator iter = pBoneGroupAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		TupperwareAtom *pItemAtom = iter.GetAsAtom();
		if (pItemAtom->GetAtomType()!=TupperwareAtom::ATOM_SCALAR)
		{
			printf("BoneGroups - Invalid Bone %s - Use NAME [""MIRRORNAME""] = VALUE|""NAME""\n",pItemAtom->GetKey());
			m_numErrors++;
			continue;
		}

		TupperwareScalar *pItemScalar = (TupperwareScalar *) pItemAtom;

		int value;
		if (pItemScalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = pItemScalar->GetAsInt();
			if (value<0||value>31)
			{
				printf("BoneGroups - BoneGroup for bone %s needs to be between 0 and 31 currently set to %d\n",pItemAtom->GetKey(),value);
				m_numErrors++;
				continue;
			}

		}
		else if (pItemScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			value = GetBoneGroupFromName(pItemScalar->GetAsString());
			if (value==-1)
			{
				printf("BoneGroups - Bone %s cannot find %s in BoneGroupNames\n",pItemAtom->GetKey(),pItemScalar->GetAsString());
				m_numErrors++;
				continue;
			}
			else if (value<0||value>31)
			{
				printf("BoneGroups - BoneGroup for bone %s needs to be between 0 and 31 currently set to %d\n",pItemAtom->GetKey(),value);
				m_numErrors++;
				continue;
			}
		}
		else
		{
			printf("BoneGroups - Invalid Bone %s - Use NAME [""MIRRORNAME""] = VALUE|""NAME""\n",pItemAtom->GetKey());
			m_numErrors++;
			continue;
		}

		BoneGroupTableEntry newEntry;
		newEntry.m_name = pItemAtom->GetKey();
		newEntry.m_name.MakeLower();
		if (pItemAtom->GetName())
		{
			newEntry.m_mirrorName = pItemAtom->GetName();
			newEntry.m_mirrorName.MakeLower();
		}
		newEntry.m_boneGroup = value;
		newEntry.m_mirrorIndex = -1;
		int oldIndex = m_boneGroupTableNames.GetSize();
		int newIndex = m_boneGroupTableNames.Add(newEntry.m_name);
		if (oldIndex==newIndex)
		{
			m_boneGroupTableEntries.Add(newEntry);
		}
		else
		{
			printf("Duplicate Bone Found %s\n",pItemAtom->GetKey());
			m_numErrors++;
			continue;
		}
	}
	// set up the mirror indexes
	int numEntries = m_boneGroupTableEntries.GetSize();
	for (int entryIndex = 0;entryIndex<numEntries;entryIndex++)
	{
		BoneGroupTableEntry &entry = m_boneGroupTableEntries[entryIndex];
		if (entry.m_mirrorName.GetLength())
		{
			int mirrorIndex = m_boneGroupTableNames.GetIndex(entry.m_mirrorName);
			if (mirrorIndex==-1)
			{
				printf("Mirror Bone Not Found %s -> %s\n",(const char *)entry.m_name,(const char *)entry.m_mirrorName);
				m_numErrors++;
				continue;
			}

			BoneGroupTableEntry &mirrorEntry = m_boneGroupTableEntries[mirrorIndex];
			if (mirrorEntry.m_mirrorName != entry.m_name)
			{
				printf("Mirrored Bones not in sync %s -> %s - %s -> %s\n",(const char *)entry.m_name,(const char *)entry.m_mirrorName,(const char *)mirrorEntry.m_name,(const char *)mirrorEntry.m_mirrorName);
				m_numErrors++;
				continue;
			}

			// hook together
			entry.m_mirrorIndex = mirrorIndex;
			mirrorEntry.m_mirrorIndex = entryIndex;
		}
	}
}

void BoneGroupTable::GetInvalidModelChannelData(TupperwareAggregate *pTupRoot)
{
	TupperwareAggregate *pInvalidModelChannelsAgg = pTupRoot->FindAggregateByKey("InvalidMotionChannels");
	if (pInvalidModelChannelsAgg)
	{
		for (TupperwareIterator iter = pInvalidModelChannelsAgg->FindAtomByKey("Entry"); !iter.Finished(); iter.Next()) 
		{
			TupperwareAtom *pItemAtom = iter.GetAsAtom();
			if (!pItemAtom->GetName())
			{
				printf("Invalid Channel Use Entry ""NAME"" = VALUE\n");
				m_numErrors++;
				continue;
			}

			if (pItemAtom->GetAtomType()!=TupperwareAtom::ATOM_SCALAR)
			{
				printf("Invalid Channel %s - Use Entry ""NAME"" = VALUE\n",pItemAtom->GetName());
				m_numErrors++;
				continue;
			}
			TupperwareScalar *pItemScalar = (TupperwareScalar *) pItemAtom;
			if (pItemScalar->GetDataType()!=TupperwareScalar::DATA_INT)
			{
				printf("Invalid Channel %s - Use Entry ""NAME"" = VALUE\n",pItemAtom->GetName());
				m_numErrors++;
				continue;
			}

			TupString name = pItemAtom->GetName();
			name.MakeLower();

			int oldIndex = m_invalidModelChannelNames.GetSize();
			int newIndex = m_invalidModelChannelNames.Add(name);
			// if we added a new name
			if (newIndex==oldIndex)
			{
				int value = pItemScalar->GetAsInt();
				m_invalidModelChannelValues.Add(value);
			}
			else
			{
				printf("Duplicate InvalidMotionChannel found %s\n",pItemAtom->GetName());
				m_numErrors++;
			}
		}
	}
	else
	{
		printf("InvalidMotionChannels not found.  Using defaults\n");
		int value1 = -1;
		int value2 = 31;
		m_invalidMotionChannelNames.Add(TupString("control"));
		m_invalidMotionChannelValues.Add(value1);
		m_invalidMotionChannelNames.Add(TupString("shortcut"));
		m_invalidMotionChannelValues.Add(value1);
		m_invalidMotionChannelNames.Add(TupString("light"));
		m_invalidMotionChannelValues.Add(value1);
		m_invalidMotionChannelNames.Add(TupString("ol "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("rl "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("olxyz "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("sl "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("olsl "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("slol "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("rlsl "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("slrl "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("olxyzsl "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("slolxyz "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("unscale "));
		m_invalidMotionChannelValues.Add(value2);
		m_invalidMotionChannelNames.Add(TupString("proxy"));
		m_invalidMotionChannelValues.Add(value1);
	}
}

void BoneGroupTable::GetInvalidMotionChannelData(TupperwareAggregate *pTupRoot)
{
	TupperwareAggregate *pInvalidMotionChannelsAgg = pTupRoot->FindAggregateByKey("InvalidMotionChannels");
	if (pInvalidMotionChannelsAgg)
	{
		for (TupperwareIterator iter = pInvalidMotionChannelsAgg->FindAtomByKey("Entry"); !iter.Finished(); iter.Next()) 
		{
			TupperwareAtom *pItemAtom = iter.GetAsAtom();
			if (!pItemAtom->GetName())
			{
				printf("Invalid Channel Use Entry ""NAME"" = VALUE\n");
				m_numErrors++;
				continue;
			}

			if (pItemAtom->GetAtomType()!=TupperwareAtom::ATOM_SCALAR)
			{
				printf("Invalid Channel %s - Use Entry ""NAME"" = VALUE\n",pItemAtom->GetName());
				m_numErrors++;
				continue;
			}
			TupperwareScalar *pItemScalar = (TupperwareScalar *) pItemAtom;
			if (pItemScalar->GetDataType()!=TupperwareScalar::DATA_INT)
			{
				printf("Invalid Channel %s - Use Entry ""NAME"" = VALUE\n",pItemAtom->GetName());
				m_numErrors++;
				continue;
			}

			TupString name = pItemAtom->GetName();
			name.MakeLower();

			int oldIndex = m_invalidMotionChannelNames.GetSize();
			int newIndex = m_invalidMotionChannelNames.Add(name);
			// if we added a new name
			if (newIndex==oldIndex)
			{
				int value = pItemScalar->GetAsInt();
				m_invalidMotionChannelValues.Add(value);
			}
			else
			{
				printf("Duplicate ValidChannels found %s\n",pItemAtom->GetName());
				m_numErrors++;
			}
		}
	}
	else
	{
		printf("ValidMotionChannels not found.  Using defaults\n");
		int value = -1;
		m_invalidMotionChannelNames.Add(TupString("control"));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("shortcut"));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("light"));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("ol "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("rl "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("olxyz "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("sl "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("olsl "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("slol "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("rlsl "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("slrl "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("olxyzsl "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("slolxyz "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("unscale "));
		m_invalidMotionChannelValues.Add(value);
		m_invalidMotionChannelNames.Add(TupString("proxy"));
		m_invalidMotionChannelValues.Add(value);
	}
}

// from m_invalidModelChannelNames & m_invalidMotionChannelValues
// returns value from invaluemodelchannel table if found
// return 0 if not found
int BoneGroupTable::IsValidModelChannel(const char *pChannelName) const
{
	int numEntries = m_invalidModelChannelNames.GetSize();
	for (int index=0;index<numEntries;index++)
	{
		if (!strnicmp(pChannelName,m_invalidModelChannelNames[index],m_invalidModelChannelNames[index].GetLength()))
		{
			return m_invalidModelChannelValues[index];
		}
	}
	return 0;
}

// from m_invalidMotionChannelNames & m_invalidMotionChannelValues
bool BoneGroupTable::IsValidMotionChannel(const char *pChannelName) const
{
	int numEntries = m_invalidMotionChannelNames.GetSize();
	for (int index=0;index<numEntries;index++)
	{
		if (!strnicmp(pChannelName,m_invalidMotionChannelNames[index],m_invalidMotionChannelNames[index].GetLength()))
		{
			return (m_invalidMotionChannelValues[index]!=-1);
		}
	}
	return true;
}

// from m_boneGroupNames & m_boneGroupNameValues
int BoneGroupTable::GetBoneGroupFromName(const char *pBoneGroupName) const
{
	int numEntries = m_boneGroupNames.GetSize();
	for (int index=0;index<numEntries;index++)
	{
		if (!stricmp(pBoneGroupName,m_boneGroupNames[index]))
		{
			return m_boneGroupNameValues[index];
		}
	}
	return -1;
}

const char *BoneGroupTable::GetBoneGroupNameFromID(int id) const
{
	int numEntries = m_boneGroupNames.GetSize();
	for (int index=0;index<numEntries;index++)
	{
		if (m_boneGroupNameValues[index] == id)
		{
			return m_boneGroupNames[index];
		}
	}
	return NULL;
}

int BoneGroupTable::GetNumErrors(void) const
{
	return m_numErrors;
}
