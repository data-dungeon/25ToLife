////////////////////////////////////////////////////////////////////////////
//
// DBLDebug
//
// Class to handle keeping track of how the dibble was written
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLDebug.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * DBL Debug Code
 */

#ifndef __DBLDEBUG_H
#define __DBLDEBUG_H

#ifdef _DEBUG
// comment out this line to do dbl debugging
//#define __DEBUG_DBL_ENABLE
#endif

#ifdef __DEBUG_DBL_ENABLE
#define DBL_FILEWRITE(a,b,c,d) DBLDebug::FileWrite(a,b,c,d)

class DBLDebug
{
	class ChunkAccessEntry
	{
	public:
		TupString m_name;
		int m_index;
		int m_fileWriteMin;
		int m_fileWriteMax;
	};

	static TupArray<ChunkAccessEntry> m_chunkAccessEntryArray;	
	static int m_numEntries;
	static int m_entryIndex;

public:

	static void Reset(void);
	static int NextEntry(void);
	static void SetName(const char *pName);
	static void SetIndex(int index);
	static void SetEntryIndex(int index);
	static int GetNumEntries(void);
	static int GetEntryIndex(void);
	static const char *GetName(int index);
	static int GetIndex(int index);
	static int GetFileWriteMin(int index);
	static int GetFileWriteMax(int index);

	static size_t FileWrite (const void *p, size_t size, size_t num, FILE *f);
};
	
#else

// normal writes are passed directly to fwrite
#define DBL_FILEWRITE(a,b,c,d) fwrite(a,b,c,d)

#endif

#endif
