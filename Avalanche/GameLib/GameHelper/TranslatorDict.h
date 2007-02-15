//============================================================================
//=
//= TranslatorDict.h - binary file format for the language translation files
//= Rob Nelson, Avalanche Software, 2005
//=
//============================================================================

#ifndef _TRANSLATOR_DICT_H
#define _TRANSLATOR_DICT_H

#define DICT_ID		( ( 'D' << 0 ) | ( 'I' << 8 ) | ( 'C' << 16 ) | ( 'T' << 24 ) )
#define DICT_VERSION	( 0x1000 )

class CTranslatorDict
{
public:
	bool Init();		// init binary after load
	bool Deinit();		// deinit binary before save
	static void SwapValue( void* v, int n );
	void Dump() const;
	const char* Lookup( unsigned int hash ) const;

	struct HashEntry
	{
		unsigned int	hash;
		char*				text;
	};

	// file structure, header followed by hash table and then text strings
	int				d_id;
	int				d_version;
	unsigned int	d_seed;
	HashEntry*		d_hashTable;
	int				d_hashTableSize;
};

//============================================================================

#define DICT_DEINIT_PTR( p ) \
{ \
   if ( *(int*)&(p) != 0 ) \
   { \
      *(int*)&(p) = ( (int)(p) - (int)&(p) - 1 ); \
   } \
}

#define DICT_INIT_PTR( p ) \
{ \
   if ( *(int*)&(p) != 0 ) \
   { \
      *(int*)&(p) = (int)p + (int)&(p) + 1; \
   } \
}

#define DICT_SWAP( v ) \
{ \
   if ( swap ) \
   { \
      SwapValue( &(v), sizeof( v ) ); \
   } \
}

//============================================================================

bool CTranslatorDict::Init()
{
	// check endianness
	bool swap = false;
	if ( d_id != DICT_ID )
	{
		swap = true;
		int swapID = DICT_ID;
		DICT_SWAP( swapID );
		if ( d_id != swapID )
			return false;
	}

	DICT_SWAP( d_version );
	if ( d_version != DICT_VERSION )
		return false;

	DICT_SWAP( d_seed );
	DICT_SWAP( d_hashTable );	DICT_INIT_PTR( d_hashTable );
	DICT_SWAP( d_hashTableSize );

	for ( int i = 0 ; i < d_hashTableSize ; i++ )
	{
		DICT_SWAP( d_hashTable[i].hash );
		DICT_SWAP( d_hashTable[i].text );	DICT_INIT_PTR( d_hashTable[i].text );
	}

	return true;
}

//============================================================================

bool CTranslatorDict::Deinit()
{
	if ( d_id != DICT_ID )
		return false;
	if ( d_version != DICT_VERSION )
		return false;

	for ( int i = 0 ; i < d_hashTableSize ; i++ )
		DICT_DEINIT_PTR( d_hashTable[i].text );

	DICT_DEINIT_PTR( d_hashTable );

	return true;
}

//============================================================================

void CTranslatorDict::Dump() const
{
	printf( "HASH TABLE SIZE : %d\n", d_hashTableSize );
	for ( int i = 0 ; i < d_hashTableSize ; i++ )
		printf( "%5d : %08X %s\n", i, d_hashTable[i].hash, d_hashTable[i].text ? d_hashTable[i].text : "" );
}

//============================================================================

const char* CTranslatorDict::Lookup( unsigned int hash ) const
{
	for ( int i = hash % d_hashTableSize ; d_hashTable[i].text ; i = (i + 1) % d_hashTableSize )
		if ( d_hashTable[i].hash == hash )
			return d_hashTable[i].text;
	return NULL;
}

//============================================================================

void CTranslatorDict::SwapValue( void* v, int N )
{
   char T;

   switch ( N )
   {
      case 0:
      case 1:
         break;  // nothing to do
      case 2:
         T = ( ( char* ) v )[ 0 ];
         ( ( char* ) v )[ 0 ] = ( ( char* ) v )[ 1 ];
         ( ( char* ) v )[ 1 ] = T;
         break;
      case 4:
         T = ( ( char* ) v )[ 0 ];
         ( ( char* ) v )[ 0 ] = ( ( char* ) v )[ 3 ];
         ( ( char* ) v )[ 3 ] = T;
         T = ( ( char* ) v )[ 1 ];
         ( ( char* ) v )[ 1 ] = ( ( char* ) v )[ 2 ];
         ( ( char* ) v )[ 2 ] = T;
         break;
   }
}

//============================================================================

#endif //_TRANSLATOR_DICT_H

//============================================================================
