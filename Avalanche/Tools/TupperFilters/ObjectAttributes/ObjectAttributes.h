////////////////////////////////////////////////////////////////////////////
//
// MakeYUp
//
// Class to handle the Make YUp Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ObjectAttributes.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/23/03    Time: 1:51p
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * add version and checksum to .oa file
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 8:33a
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * save out binary .oa file
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/29/02    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * Added Formatting of Instance Attributes
 * Added Comment for Instance Positions
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:46p
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * File Dependency Check
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:46p
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * Made dumping the attribute data an option
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 4/09/02    Time: 9:35a
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * Attribute Access Test Function
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/27/02    Time: 1:13p
 * Updated in $/Avalanche/tools/TupperFilters/ObjectAttributes
 * First pass of filter
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/27/02    Time: 10:20a
 * Created in $/Avalanche/tools/TupperFilters/ObjectAttributes
 */ 

////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_ATTRIBUTES_H
#define OBJECT_ATTRIBUTES_H

#include "Container/DArray.h"

class TupperwareAggregate;
class TupperChainLogBase;
class TupNodePool;

//============================================================================

class CExclusions
{
public:
	void AddKey( const char* key )
	{
		if ( strlen( key ) )
		{
			m_exclusionList.Add( key );
		}
	}

	bool AddKeysFromFile( const char* filename )
	{
		FILE* fp = fopen( filename, "rt" );
		char buf[ 256 ];
		if ( !fp ) return false;
		while ( fgets( buf, sizeof( buf ), fp ) )
		{
			// kill comments and strip white space from end of line
			char* p;
			if ( ( p = strstr( buf, "//" ) ) != NULL ) { *p = 0; }
			if ( ( p = strstr( buf, ";"  ) ) != NULL ) { *p = 0; }
			for ( p = buf + strlen( buf ) - 1 ; p > buf && isspace( *p ) ; )
				*p-- = 0;

			// find start of key without leading space
			for ( p = buf ; isspace( *p ) ; )
				p++;

			AddKey( p );
		}
		fclose( fp );
		return true;
	}

	bool IsExcluded( const char* key )
	{
		for ( int i = 0 ; i < m_exclusionList.GetSize() ; i++ )
			if ( stricmp( m_exclusionList[ i ], key ) == 0 )
				return true;
		return false;
	}

	void Dump()
	{
		for ( int i = 0 ; i < m_exclusionList.GetSize() ; i++ )
		{
			printf( "exclusion '%s'\n", (const char *)m_exclusionList[ i ] );
		}
	}

private:
	TupArray<TupString> m_exclusionList;
};

//============================================================================

class ObjectAttributesFilter : public FilterPlugin
{
	enum ReadDependencies
	{
		INSTANCE_INDEXES,
		NUM_READ_DEPENDENCIES // last entry
	};

	static char *m_readDependencyNames[NUM_READ_DEPENDENCIES];
	static Revision m_readDependencyRevisions[NUM_READ_DEPENDENCIES];

	TupString m_objectAttributesFile;
	CExclusions	m_exclusions;

public:
	ObjectAttributesFilter(TupperChainLogBase &log) : FilterPlugin(log) {}

	int GetTotalPasses(void) const;
	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	void GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray);

	int GetTotalReadDependencies(int pass,const FilterOptions &options) const;
	const char *GetReadDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const;

private:
	bool WriteActorParameterFile(TupperwareAggregate *pRoot, const char* outputFilename);
};

//============================================================================

#endif
