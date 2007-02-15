
#ifndef __RPATCHER_H__
#define __RPATCHER_H__

#include <cstdio>
#include <cassert>
#include <cstring>

class RPatcher
{
public:

	struct RPatchInfo
	{
		char* filename;
		FILE* patchFile;

		unsigned int patchedResultSize;
		unsigned int patchDataOffset;
		unsigned int patchDataSize;
	};


	RPatcher();
	~RPatcher();

	void LoadAllPatchInfo();
	bool GetPatchInfo( const char* const filename, const RPatchInfo*& patchInfo ) const;
	void LoadAndPatch( const RPatchInfo* const patchInfo, unsigned char*& destinationMemory ) const;

protected:

	void LoadPatchInfo( FILE* patchFile, RPatchInfo* result );

	RPatchInfo*	m_patchedFiles;
	unsigned int		m_numPatchedFiles;

	enum DataSource
	{
		DATA_INVALID	= 0xff,
		DATA_ORIGINAL	= 0,
		DATA_MODIFIED	= 1,
		DATA_LITERAL	= 2
	};
};

#endif //__RPATCHER_H__