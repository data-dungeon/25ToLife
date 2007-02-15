//============================================================================
//=
//= PersistentData.cpp - A database class to keep track of game events
//=
//============================================================================

#include "GameHelper/GameHelperPCH.h"
#include "GameHelper/PersistentData.h"

//============================================================================

#define NO_WORLD		-1

//============================================================================

PersistentData::PersistentData()
{
	Clear();
}

//============================================================================

PersistentData::~PersistentData()
{
}

//============================================================================

void PersistentData::Clear()
{
	// init persistent data
	d_PersistentDataCount = 0;
	memset( &d_PersistentData, 0, sizeof( d_PersistentData ) );
}

//============================================================================

PersistentDataRecord* PersistentData::New( const char* name, const char* subName, int world, int data )
{
	// check for overflow
	if ( d_PersistentDataCount >= MAX_PERSISTENT_DATA_RECORDS )
	{
		ASSERTS( false, "increase MAX_PERSISTENT_DATA_RECORDS" );
		return NULL;
	}

	// get our ptr to the new record, increment record count
	PersistentDataRecord* rec = &d_PersistentData[ d_PersistentDataCount ];
	d_PersistentDataCount++;

	// fill in record
	strncpy( rec->name, name, sizeof( rec->name ) );
	strncpy( rec->subName, subName, sizeof( rec->subName ) );
	rec->world   = world;
	rec->data    = data;

	return rec;
}

//============================================================================

int PersistentData::Get( const char* name, const char* subName, int world )
{
	ASSERT_PTR( name );
	ASSERT( name[0] != 0 );

	// find the record
	PersistentDataRecord* rec = FindFirst( name, subName, world, 0, PERSISTENTDATA_MATCH_NAME | PERSISTENTDATA_MATCH_SUB_NAME | PERSISTENTDATA_MATCH_WORLD );
	if ( rec == NULL )
		return 0;
	else
		return rec->data;
}

//============================================================================

void PersistentData::Set( const char* name, const char* subName, int world, int data )
{
	ASSERT_PTR( name );
	ASSERT( name[0] != 0 );

	// if we don't already have this record, make a new one
	PersistentDataRecord* rec = FindFirst( name, subName, world, 0, PERSISTENTDATA_MATCH_NAME | PERSISTENTDATA_MATCH_SUB_NAME | PERSISTENTDATA_MATCH_WORLD );
	if ( rec == NULL )
	{
		rec = New( name, subName, world, data );
	}
	else
	{
		//If we're already set to this value, we're done
		if (rec->data != data)
		{
			rec->data = data;
		}
	}
}

//============================================================================

int PersistentData::Add( const char* name, const char* subName, int world, int addAmount )
{
	ASSERT_PTR( name );
	ASSERT( name[0] != 0 );

	// if we don't already have this record, make a new one
	PersistentDataRecord* rec = FindFirst( name, subName, world, 0, PERSISTENTDATA_MATCH_NAME | PERSISTENTDATA_MATCH_SUB_NAME | PERSISTENTDATA_MATCH_WORLD );
	if ( rec == NULL )
		rec = New( name, subName, world, addAmount );
	else
		rec->data += addAmount;

	// return new data
	return rec->data;
}

//============================================================================

int PersistentData::GetGlobal( const char* name )
{
	return Get( name, "", NO_WORLD );
}

//============================================================================

void PersistentData::SetGlobal( const char* name, int data )
{
	Set( name, "", NO_WORLD, data );
}

//============================================================================

int PersistentData::AddGlobal( const char* name, int addAmount )
{
	return Add( name, "", NO_WORLD, addAmount );
}

//============================================================================

PersistentDataRecord* PersistentData::FindFirst( const char* name, const char* subName, int world, int data, int matchMask )
{
	return FindNext( name, subName, world, data, matchMask, NULL );
}

//============================================================================

PersistentDataRecord* PersistentData::FindNext( const char* name, const char* subName, int world, int data, int matchMask, PersistentDataRecord* lastRecord )
{
	int startIndex;

	if ( lastRecord == NULL )
		startIndex = 0;
	else
	{
		ASSERT( lastRecord >= &d_PersistentData[ 0 ] );
		ASSERT( lastRecord <  &d_PersistentData[ d_PersistentDataCount ] );
		startIndex = ( lastRecord - &d_PersistentData[ 0 ] ) + 1;
	}

	for ( int i = startIndex ; i < d_PersistentDataCount ; i++ )
	{
		PersistentDataRecord* rec = &d_PersistentData[ i ];

		// check for key mismatches
		if ( ( matchMask & PERSISTENTDATA_MATCH_WORLD		) && ( rec->world != world ) )
			continue;
		if ( ( matchMask & PERSISTENTDATA_MATCH_DATA			) && ( rec->data != data ) )
			continue;
		if ( ( matchMask & PERSISTENTDATA_MATCH_SUB_NAME	) && ( strnicmp( rec->subName, subName, sizeof( rec->subName ) ) != 0 ) )
			continue;
		if ( ( matchMask & PERSISTENTDATA_MATCH_NAME			) && ( strnicmp( rec->name, name, sizeof( rec->name ) ) != 0 ) )
			continue;

		// we have a match
		return rec;
	}

	// no match found
	return NULL;
}

//============================================================================

int  PersistentData::TallyDataByName( const char* name )
{
	ASSERT_PTR( name );
	ASSERT( name[0] != 0 );

	PersistentDataRecord* rec;
	int count = 0;

	rec = FindFirst( name, "", 0, 0, PERSISTENTDATA_MATCH_NAME );
	while ( rec != NULL )
	{
		count += rec->data;
		rec = FindNext( name, "", 0, 0, PERSISTENTDATA_MATCH_NAME, rec );
	}

	return count;
}

//============================================================================

// ------------------------------------- Save/Load Game Functions -----------------------------------------

const u32 PersistentData::GetLocalSaveGameMemoryNeeded( void ) const
{
	u32 size;

	size = sizeof( d_PersistentDataCount );
	size += ( sizeof( PersistentDataRecord ) * d_PersistentDataCount );
	return( Align_RoundTo16( size ) );
}

// --------------------------------------------------------------------------------------------------------

const u32 PersistentData::GetSaveGameMemoryNeeded( void )
{
	return( GetLocalSaveGameMemoryNeeded( ) + 0 );
}

// --------------------------------------------------------------------------------------------------------

const u32 PersistentData::SerializeSaveGame( char *pBuf, bool bDirection )
{
	// --- Save Data?
	if( bDirection )
	{
		u8	*pSaveTo = ( u8 * )pBuf;

		memcpy( pSaveTo, &d_PersistentDataCount, sizeof( d_PersistentDataCount ) );
		pSaveTo += sizeof( d_PersistentDataCount );
		memcpy( pSaveTo, d_PersistentData, sizeof( PersistentDataRecord ) * d_PersistentDataCount );
	}

	// --- Load Data?
	else
	{
		u8 *pLoadFrom = ( u8 * )pBuf;

		memcpy( &d_PersistentDataCount, pLoadFrom, sizeof( d_PersistentDataCount ) );
		pLoadFrom += sizeof( d_PersistentDataCount );
		memcpy( d_PersistentData, pLoadFrom, sizeof( PersistentDataRecord ) * d_PersistentDataCount );
	}

	// --- return amount of memory we serialized in.  Must call CSimple:: because this
	// --- function is virtual and we don't want to call the derrived class's version.
	return( PersistentData::GetSaveGameMemoryNeeded( ) );
}

