////////////////////////////////////////////////////////////////////////////
//
// DBLDebug
//
// Class to handle keeping track of how the dibble was written
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLDebug.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/23/02    Time: 3:53p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * oops
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * DBL Debug Code
 */

#include "DBLUtilsPch.h"
#include "DBLDebug.h"

#ifdef __DEBUG_DBL_ENABLE

int DBLDebug::m_numEntries = 0;
int DBLDebug::m_entryIndex = 0;
TupArray<DBLDebug::ChunkAccessEntry> DBLDebug::m_chunkAccessEntryArray;	

void DBLDebug::Reset(void)
{
	m_numEntries = 0;
	m_chunkAccessEntryArray.RemoveAll();
	m_entryIndex = -1;
}

int DBLDebug::NextEntry(void)
{
	ChunkAccessEntry newEntry;
	if (m_numEntries==0)
	{
		newEntry.m_index = 0;
		newEntry.m_name = "undefined";
		newEntry.m_fileWriteMax = -2147483647;
		newEntry.m_fileWriteMin = 2147483647;
	}
	else
	{
		ChunkAccessEntry &oldEntry = m_chunkAccessEntryArray[m_numEntries-1];
		newEntry.m_index = oldEntry.m_index+1;
		newEntry.m_name = oldEntry.m_name;
		newEntry.m_fileWriteMax = -2147483647;
		newEntry.m_fileWriteMin = 2147483647;
	}
	m_chunkAccessEntryArray.Add(newEntry);
	m_numEntries++;
	m_entryIndex=m_numEntries-1;
	return m_entryIndex;
}

void DBLDebug::SetName(const char *pName)
{
	if (m_entryIndex>=0 && m_entryIndex<m_numEntries)
	{
		ChunkAccessEntry &entry = m_chunkAccessEntryArray[m_entryIndex];
		entry.m_name = pName;
	}
}

void DBLDebug::SetIndex(int index)
{
	if (m_entryIndex>=0 && m_entryIndex<m_numEntries)
	{
		ChunkAccessEntry &entry = m_chunkAccessEntryArray[m_entryIndex];
		entry.m_index = index;
	}
}

void DBLDebug::SetEntryIndex(int index)
{
	if (index>=0 && index<m_numEntries)
	{
		m_entryIndex = index;
	}
}

int DBLDebug::GetNumEntries(void)
{
	return m_numEntries;
}

int DBLDebug::GetEntryIndex(void)
{
	return m_entryIndex;
}

const char *DBLDebug::GetName(int index)
{
	if (index>=0 && index<m_numEntries)
	{
		ChunkAccessEntry &entry = m_chunkAccessEntryArray[index];
		return entry.m_name;
	}
	return NULL;
}

int DBLDebug::GetIndex(int index)
{
	if (index>=0 && index<m_numEntries)
	{
		ChunkAccessEntry &entry = m_chunkAccessEntryArray[index];
		return entry.m_index;
	}
	return 0;
}

int DBLDebug::GetFileWriteMin(int index)
{
	if (index>=0 && index<m_numEntries)
	{
		ChunkAccessEntry &entry = m_chunkAccessEntryArray[index];
		return entry.m_fileWriteMin;
	}
	return 0;
}

int DBLDebug::GetFileWriteMax(int index)
{
	if (index>=0 && index<m_numEntries)
	{
		ChunkAccessEntry &entry = m_chunkAccessEntryArray[index];
		return entry.m_fileWriteMax;
	}
	return 0;
}

size_t DBLDebug::FileWrite (const void *p, size_t size, size_t num, FILE *f)
{
	if (m_entryIndex>=0 && m_entryIndex<m_numEntries)
	{
		int locationStart = ftell(f);
		int locationEnd = locationStart+(size*num);
		ChunkAccessEntry &entry = m_chunkAccessEntryArray[m_entryIndex];
		if (entry.m_fileWriteMin>locationStart)
		{
			entry.m_fileWriteMin = locationStart;
		}
		if (entry.m_fileWriteMax<locationEnd)
		{
			entry.m_fileWriteMax = locationEnd;
		}
	}
	return fwrite(p,size,num,f);
}

#endif // _DEBUG
