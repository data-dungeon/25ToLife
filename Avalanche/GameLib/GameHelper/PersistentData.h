//============================================================================
//=
//= PersistentData.h - A database class to keep track of game events
//=
//============================================================================

#ifndef _PERSISTENTDATA_H
#define _PERSISTENTDATA_H

//============================================================================

#define MAX_PERSISTENT_DATA_RECORDS		320	// grow this as we need

#define PERSISTENTDATA_MATCH_NAME		0x01
#define PERSISTENTDATA_MATCH_SUB_NAME	0x02
#define PERSISTENTDATA_MATCH_WORLD		0x04
#define PERSISTENTDATA_MATCH_DATA		0x08

//============================================================================

struct PersistentDataRecord
{
	char				name[ 24 ];		// name for a database entry (category name, like "warp", "moonstone")
	char				subName[ 32 ];	// specific name for an entry, to tell apart entries in same category
	int				world;			// world that entry is logged for
	int				data;				// data associated with this Name/SubName/World
};

//============================================================================

class PersistentData
{
public:
	PersistentData();
	virtual ~PersistentData();

	// reset for new games
	void Clear();

	// retrieve data using name/subname/world as the lookup key.  if
	// a matching key isn't found "0" is returned.
	int Get( const char* name, const char* subName, int world ); 

	// set data using name/subname/world as the key.  if a matching
	// key exists it will change the data inside that key, otherwise it
	// will add a new key
	void Set( const char* name, const char* subName, int world, int data );

	// add the value "addAmount" to the data field of the key name/subname/world.
	// if the key doesn't exist it will create it with an assumed value of 0.
	int  Add( const char* name, const char* subName, int world, int addAmount );

	// same as [Get/Set/Add], except the world and hashID ignored
	int  GetGlobal( const char* name );
	void SetGlobal( const char* name, int data );
	int  AddGlobal( const char* name, int addAmount );

	// tally the data of a given named field, for all sub-names and world values
	int  TallyDataByName( const char* name );

	// find the first/next matching records using the bitmask PERSISTENTDATA_MATCH_xxx to
	// control which fields you care about matching.
	PersistentDataRecord* FindFirst( const char* name, const char* subName, int world, int data, int matchMask );
	PersistentDataRecord* FindNext( const char* name, const char* subName, int world, int data, int matchMask, PersistentDataRecord* lastRecord );

private:
	// forcefully create a new record.  doesn't check for duplicates.  should only
	// be used internally.
	PersistentDataRecord* New( const char* name, const char* subName, int world, int data );

private:
	int d_PersistentDataCount;
	PersistentDataRecord d_PersistentData[ MAX_PERSISTENT_DATA_RECORDS ];
	
public:
	const u32			GetLocalSaveGameMemoryNeeded					( void ) const;
	virtual const u32	GetSaveGameMemoryNeeded							( void );
	virtual const u32	SerializeSaveGame									( char *pBuf, bool Direction );
};

//============================================================================

#endif //_PERSISTENTDATA_H
