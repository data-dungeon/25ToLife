//============================================================================
//=
//= DBLParse.h - Class for parsing chunks in DBL files
//=
//============================================================================

#include "Game/GamePCH.h"

#include "Audio/audiosys.h"
#include "XML/XML.h"
#include "Layers/Tasking.h"
#include "GameHelper/compression/compress2.h"
#include "Display/Common/TextureAnimatorArray.h"
#include "EngineHelper/NameLibrary.h"
#include "Layers/loadprofile.h"
#include "cmdcon/cmdcon.h"
#include "Game/Database/DBLParse.h"
#include "Navigate/NavMesh.h"
#include "Navigate/NavManager.h"
#include "Network/Session.h"
#include "GameHelper/DebugOutput.h"
#include "EngineHelper/pathfile.h"

#if defined (_XBOX)
#include "platform/Xbox/dxdmanager.h"
#endif

#ifdef PS2
#include "Display/PS2/Renderer/Code/DDMAObject.h"	// for memory report
#endif //PS2

#ifdef GCN
#include "Display/GCN/Shader.h"
#include "geomUtil/AnimCurve.h"
#endif

//============================================================================

struct DBLFileRecord
{
	// base file information
	void*		fileBuffer;

	// pointer back to owner
	DataMgr::OwnerRecord* owner;

	// group ids for particle system
	u16		distortionGroup;
	u16		emitterGroup;

	// flags
	u8			permanent : 1;
	u8			hasBeenLocked : 1;

	u8			lockedCount;

	// filename (structure is allocated large enough to fit name)
	char		name[ 1 ];
};

//============================================================================

CDBLParse*	s_currentParser = NULL;

//============================================================================

#if defined(PS2)
#define WRITEBACK_DATA_CACHE( addr, len )	FlushCache( WRITEBACK_DCACHE )
#elif defined(GCN)
#define WRITEBACK_DATA_CACHE( addr, len ) DCStoreRangeNoSync( addr, len )
#else
#define WRITEBACK_DATA_CACHE( addr, len ) ((void)0)
#endif

//============================================================================

bool LoadNameList(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
DBLFileRecord* dblFileRecordFromHandle( t_Handle handle );
void* dblGetChunkData( u16 type, const char* name, DataMgr::OwnerRecord* owner, int* o_size );

//============================================================================
//= initialize database file system

void dblInit(int nMaxCustomChunkLoaders)
{
}

//============================================================================
//= get a file record from a handle

DBLFileRecord* dblFileRecordFromHandle( t_Handle handle )
{
	if ( handle != INVALID_HANDLE )
	{
		DataMgr::OwnerRecord* owner = DataMgr::FindOwner( (void*)handle );
		if ( owner )
		{
			DBLFileRecord* file = (DBLFileRecord*) handle;
			ASSERT( file->owner == owner );
			return file;
		}
		else
		{
//			ASSERTS( false, "invalid dbl handle" );
		}
	}
	return NULL;
}

//============================================================================
//= unload DBL files

void dblReset( bool removePermanent )
{
	for ( DataMgr::OwnerRecord* owner = DataMgr::FirstOwner() ; owner ; owner = DataMgr::NextOwner() )
		if ( owner->d_type == DATA_DBL_FILE )
			dblRemove( (t_Handle) owner->d_userData, removePermanent );

	#if defined(GCN)
	g_ShaderMgr.Cleanup( );
	#endif
}

//============================================================================
//= shutdown

void dblShutdown(void)
{
	dblReset(true);
}

//============================================================================
//= get the Chunk Key value out of the chunk header

char* dblGetChunkName( ts_DBChunkHdr* chunk )
{
	if ( ( chunk->u16Flags & DBL_NAMED_CHUNK ) && chunk->cName[ 0 ] )
		return chunk->cName;
	else
		return NULL;
}

//============================================================================
//= Load a DBL file if it's not already loaded

t_Handle dblLoad( const char* filename )
{
	t_Handle handle = dblFindFile( filename );
	if ( !handle )
	{
		s32 size;
		memSetAllocMethod( ALC_BTM );
		memSetFitMethod( FIT_FIRST );
		char* buffer = (char*) Media::Load( filename, &size, 0 );
		memSetAllocMethod( ALC_TOP );
		memSetFitMethod( FIT_LAST );
		if ( !buffer )
			g_consolePtr->Notify( ">>>>>>>> dblLoad: unable to load file: %s\n", Media::GetFileName( filename ) );
		else
		{
			handle = dblLoad( filename, buffer, size );
			if ( !handle )
				memFree( buffer );
		}
	}
	return handle;
}

//============================================================================
//= Process a DBL file that's already in memory

t_Handle dblLoad( const char* filename, void* buffer, int size )
{
	u32 timeStart = g_timer.GetOSTime();

	CDBLParse parser;
	if ( !parser.Init( filename, buffer, size, 0 ) )
	{
		g_consolePtr->Notify( ">>>>>>>> dblLoad: unable to load file: %s, bad dbl header\n", Media::GetFileName( filename ) );
		return INVALID_HANDLE;
	}

	while ( parser.ParseChunk() ) {}

	u32 timeEnd = g_timer.GetOSTime();
	if ( timeEnd - timeStart > 250 )
	{
		g_debugOutput.Print( g_debugOutput.Category("LAG"), "LongTime: %dms dblParse %s", timeEnd - timeStart, filename );
	}

	return (t_Handle) parser.MainFileRecord();
}

//============================================================================
//= Find a loaded DBL

t_Handle dblFindFile( const char* filename )
{
	for ( DataMgr::OwnerRecord* owner = DataMgr::FirstOwner() ; owner ; owner = DataMgr::NextOwner() )
	{
		if ( owner->d_type == DATA_DBL_FILE )
		{
			DBLFileRecord* file = (DBLFileRecord*) owner->d_userData;
			if ( PathFile::FilenamesMatch( filename, file->name ) )
				return (t_Handle) file;
		}
	}

	return INVALID_HANDLE;
}

//============================================================================

char* dblGetFileName( t_Handle Handle )
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	return ( file ) ? file->name : NULL;
}

//============================================================================
// remove a database from tracking list. free any allocated memory

bool dblRemove( t_Handle Handle, bool removePermanent )
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	if ( file )
	{
		if ( !file->permanent || removePermanent )
		{
			ASSERTF( file->lockedCount == 0, ( "file still locked: %s %d %d", file->name, file->lockedCount, file->hasBeenLocked ) );

			// remove possible world distortions and emitters
			ParticleMgr__KillEmitterGroup( file->emitterGroup );
			DistortionMgr__KillGroup( file->distortionGroup );

			// free chunks
			for ( DataMgr::DataRecord* data = file->owner->FirstData() ; data ; data = data->next )
				CDBLParse::ReleaseChunkData( data->d_userData, data->d_type );

			// free file buffer
			if ( file->fileBuffer )
				memFree( file->fileBuffer );

			#ifdef _XBOX
			CDXD_Data*	Dx;
			Dx = (CDXD_Data*) dblGetChunkData(DB_XBOX_DXD, DEFAULT_CHUNK_KEY_STRING, Handle, NULL );
			PatchSOM__ReleaseHWDataDXD( Dx );
			delete Dx;
			#endif

			// remove file from tracker
			DataMgr::RemoveOwner( file->owner );
			memFree( file );
		}
	}

	return true;
}

//============================================================================
//= remove a file if it was once locked and now has no users

bool dblRemoveIfUnused(t_Handle Handle)
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	if ( file )
	{
		if ( !file->permanent && file->lockedCount == 0 )
		{
			dblRemove( Handle );
			return true;
		}
	}
	return false;
}

//============================================================================
//= remove data from a dbl file owner. this means the caller is taking
//= responsibility for the data, which must be data outside the file buffer
//= (or else this makes no sense). an example is xmlParse taking responsibility
//= for a parsed xml tree built out of a DB_XML_SOURCE chunk

void dblRemoveData( t_Handle Handle, void* data )
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	if ( file )
	{
		file->owner->RemoveData( data );
	}
}

//============================================================================

void _dblCleanupSOM( ts_PatchSOM* pSOM )
{
	ASSERT_PTR( pSOM );

	if ( pSOM->dh.u8DataType == DATA_MULTI_SOM )
	{
		if ( ( (SMeshSOM*) pSOM )->d_textureArray )
		{
			delete ( (SMeshSOM*) pSOM )->d_textureArray;
			( (SMeshSOM*) pSOM )->d_textureArray = NULL;
		}
	}
	else
	{
		if ( pSOM->pTextureArray )
		{
			delete pSOM->pTextureArray;
			pSOM->pTextureArray = NULL;
		}
	}

#ifdef PS2
	if (pSOM && (pSOM->dh.u8DataType != DATA_MULTI_SOM) && pSOM->DisplayListRecord.pDisplayList)
	{
		// I use the quadwordcount as a hack to detect if its a real display list of just a
		// location in the som data chunk.
		if(pSOM->DisplayListRecord.u16QuadWordCount == 0)
		{
			PS2Renderer::destroyDDMAObject((DDMAObject*)(pSOM)->DisplayListRecord.pDisplayList);
		}
		memset(&pSOM->DisplayListRecord, 0, sizeof(ts_DisplayListRecord));
	}
#endif //PS2

	bool removed = RemoveModelBoneData(pSOM);
	ASSERT(removed);

#ifdef DIRECTX
	if( pSOM->DisplayListRecord.u16QuadWordCount )
		PatchSOM__ReleaseMeshData( pSOM->DisplayListRecord.pDisplayList, pSOM->DisplayListRecord.u16QuadWordCount );

	pSOM->DisplayListRecord.pDisplayList = 0;
	pSOM->DisplayListRecord.u16QuadWordCount = 0;
#endif //DIRECTX

	// remove any associated data
	DataMgr::RemoveOwner( DataMgr::FindOwner( pSOM ) );
}

//============================================================================
// retrieve pointer to chunk data by type and key in current database being loaded

void *dblGetChunkDataCurrentDBL( u16 u16Type, char *pChunkName, int *pnSize /*= NULL*/)
{
	ASSERT( s_currentParser );
	ASSERT( !pChunkName || strcmp( pChunkName, DEFAULT_CHUNK_KEY_STRING ) == 0 );
	ASSERT( DataMgr::FindOwner( s_currentParser->FileRecord() ) == s_currentParser->FileRecord()->owner );
	DataMgr::DataRecord* dataRec = s_currentParser->FileRecord()->owner->FindDataByType( u16Type );
	return ( dataRec ) ? dataRec->d_userData : NULL;
}

//============================================================================
//= look through all loaded dbl files for a pointer to chunk data by type and
//= key. DEFAULT_CHUNK_KEY is a good key value unless you have specific need
//= of multiple chunks of same type in single database

void *dblGetChunkDataAllDBLs(u16 u16Type, char *pChunkName, int *pnSize /*= NULL*/)
{
	ASSERT( !pChunkName || strcmp( pChunkName, DEFAULT_CHUNK_KEY_STRING ) == 0 );
	for ( DataMgr::OwnerRecord* owner = DataMgr::FirstOwner() ; owner ; owner = DataMgr::NextOwner() )
	{
		if ( owner->d_type == DATA_DBL_FILE )
		{
			DataMgr::DataRecord* dataRec = owner->FindDataByType( u16Type );
			if ( dataRec )
				return dataRec->d_userData;
		}
	}

	return NULL;
}

//============================================================================
//= return database .dbl file's reserved distortion group id

u16 dblGetDistortionGroupID( t_Handle Handle )
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	return ( file ) ? file->distortionGroup : DISTORTION_DEFAULT_GROUP;
}

//============================================================================
//= return database .dbl file's reserved particle emitter group id

u16 dblGetEmitterGroupID(t_Handle Handle)
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	return ( file ) ? file->emitterGroup : EMITTER_DEFAULT_GROUP;
}

//============================================================================
//= return pointer to texture database in a dbl

TextureDatabase* dblGetTextures( t_Handle handle, char* chunkName /*=NULL*/ )
{
	DBLFileRecord* file = dblFileRecordFromHandle( handle );
	TextureDatabase* textures = NULL;
	if ( file )
	{
		textures = (TextureDatabase*) dblGetChunkData( DB_TEXTURE_SET, chunkName, file->owner, NULL );
		if ( !textures )
			textures = (TextureDatabase*) dblGetChunkData( DB_MERGED_TEXTURE_SET, chunkName, file->owner, NULL );
	}
	return textures;
}

//============================================================================
//= return pointer to either LOD record (if it exists) or first SOM model

void dblGetGeometry( ts_Geometry* geom, t_Handle Handle, char* name /*= NULL*/ )
{
	ASSERT_PTR( geom );
	geom->pSOM = NULL;
	geom->pLOD = NULL;

	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	if ( file )
	{
		ts_RecordList* recs = (ts_RecordList*) dblGetChunkData( DB_LOD, name, file->owner, NULL );
		if ( recs )
		{
			geom->pLOD = (ts_LOD*) GetRecordAddress( recs, 0 );
		}
		else
		{
			geom->pSOM = (ts_PatchSOM *) dblGetChunkData( DB_SNGLSOMBONE, name, file->owner, NULL );
			if ( !geom->pSOM )
				geom->pSOM = (ts_PatchSOM *) dblGetChunkData( DB_SNGLSOM, name, file->owner, NULL );
			if ( !geom->pSOM )
				geom->pSOM = (ts_PatchSOM *) dblGetChunkData( DB_DMASNGLSOM, name, file->owner, NULL );
			if ( !geom->pSOM )
			{
				ts_RecordList* soms = (ts_RecordList*) dblGetChunkData( DB_SOMARRAY, name, file->owner, NULL );
				if ( !soms )
					soms = (ts_RecordList *) dblGetChunkData( DB_SOMARRAYBONE, name, file->owner, NULL );
				if ( !soms )
					soms = (ts_RecordList *) dblGetChunkData( DB_DMASOMARRAY, name, file->owner, NULL );
				if ( soms )
					geom->pSOM = (ts_PatchSOM*) GetRecordAddress( soms, 0 );
			}
		}
	}
}

//============================================================================
//= return handle that "contains" the geometry, or INVALID_HANDLE if it doesn't exist

t_Handle dblGetHandleFromGeometry( ts_Geometry* geometry )
{
	ASSERT_PTR( geometry );

	for ( DataMgr::OwnerRecord* owner = DataMgr::FirstOwner() ; owner ; owner = DataMgr::NextOwner() )
	{
		if ( owner->d_type == DATA_DBL_FILE )
		{
			DBLFileRecord* file = (DBLFileRecord*) owner->d_userData;
			if ( geometry->pLOD )
			{
				ts_RecordList *pRecordList;
				pRecordList = (ts_RecordList *) dblGetChunkData(DB_LOD, DEFAULT_CHUNK_KEY_STRING, owner, NULL );
				if ((pRecordList) && (geometry->pLOD == (ts_LOD *) GetRecordAddress(pRecordList, 0)))
					return( (t_Handle)file );
			}
			else
			{
				ASSERT(geometry->pSOM);
				if ((geometry->pSOM == dblGetChunkData( DB_SNGLSOMBONE, DEFAULT_CHUNK_KEY_STRING, owner, NULL ) ) ||
					(geometry->pSOM == dblGetChunkData( DB_SNGLSOM,     DEFAULT_CHUNK_KEY_STRING, owner, NULL ) ) ||
					(geometry->pSOM == dblGetChunkData( DB_DMASNGLSOM,  DEFAULT_CHUNK_KEY_STRING, owner, NULL ) ) )
					return (t_Handle) file;

				void* arr;
				if ( ( arr = dblGetChunkData( DB_SOMARRAY,     DEFAULT_CHUNK_KEY_STRING, owner, NULL ) ) != NULL ||
					( arr = dblGetChunkData( DB_DMASOMARRAY,  DEFAULT_CHUNK_KEY_STRING, owner, NULL ) ) != NULL ||
					( arr = dblGetChunkData( DB_SOMARRAYBONE, DEFAULT_CHUNK_KEY_STRING, owner, NULL ) ) != NULL )
				{
					ts_RecordList* list = (ts_RecordList*) arr;
					for ( int i = 0 ; i < list->usRecordCount ; i++ )
						if ( geometry->pSOM == GetRecordAddress( list, i ) )
							return (t_Handle)file;
				}
			}
		}
	}

	return INVALID_HANDLE;
}

//============================================================================
//= mark a .dbl file as permanent or not

void dblSetPermanent( t_Handle Handle, bool permanent )
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	if ( file )
	{
		file->permanent = permanent;

		TextureDatabase* textureDatabase = dblGetTextures( Handle, DEFAULT_CHUNK_KEY_STRING );
		if ( textureDatabase )
		{
			if ( permanent )
				textureDatabase->SetFlags( TextureDatabase::PERMENANT );
			else
				textureDatabase->ClearFlags( TextureDatabase::PERMENANT );
		}
	}
}

//============================================================================
//= check if a dbl is loaded permanently

bool dblIsPermanent( t_Handle Handle )
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	return ( file ) ? file->permanent : false;
}

//============================================================================
//= simple loader for name string list

bool LoadNameList(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	ResolveRecordList((ts_RecordList *) pData);

	// this is permanent data
	pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
	*ppRetData = pData;
	*pu32DataSize = pChunkHdr->u32Size;

	return true;
}

//============================================================================
//= if the record list uses an offset table, resolve the entries to pointers
//= to the records

void ResolveRecordList( ts_RecordList* recordList )
{
	if ( recordList->usFlags & RECORD_LIST_HAS_OFFSET_TABLE )
	{
		ASSERT( ( recordList->usFlags & RECORD_LIST_OFFSET_TABLE_RESOLVED ) == 0 );
		recordList->usFlags |= RECORD_LIST_OFFSET_TABLE_RESOLVED;
		for ( int i = 0 ; i < recordList->usRecordCount ; i++ )
			recordList->pRecordTable[ i ] = (char*)recordList + recordList->u32OffsetTable[ i ];
	}
}

//============================================================================

#ifdef GCN
void ResolveRecordListSwapped( ts_RecordList* recordList )
{
	ByteSwap( recordList->usRecordCount );
	ByteSwap( recordList->usFlags );

	if ( recordList->usFlags & RECORD_LIST_HAS_OFFSET_TABLE )
	{
		ASSERT( ( recordList->usFlags & RECORD_LIST_OFFSET_TABLE_RESOLVED ) == 0 );
		recordList->usFlags |= RECORD_LIST_OFFSET_TABLE_RESOLVED;
		for ( int i = 0 ; i < recordList->usRecordCount ; i++ )
		{
			u32 offset = recordList->u32OffsetTable[ i ];
			ByteSwap( offset );
			recordList->pRecordTable[ i ] = (char*)recordList + offset;
		}
	}
}
#endif

//============================================================================
//= return a pointer to a record within a chunk, based on header type

void* GetRecordAddress( ts_RecordList* recordList, u32 index )
{
	if ( recordList )
	{
		u32 size;

		// mask off high byte, it's used as a partition or world indicator
		index &= 0x00ffffff;

		switch ( recordList->usFlags & RECORD_LIST_TYPE_MASK )
		{
			case RECORD_LIST_HAS_OFFSET_TABLE:
				// offset table
				ASSERT( index < recordList->usRecordCount );
				ASSERT( recordList->usFlags & RECORD_LIST_OFFSET_TABLE_RESOLVED );
				return recordList->pRecordTable[ index ];

			case RECORD_LIST_SAME_SIZE_RECORDS:
				// same-size records, following list header
				ASSERT( index < recordList->usRecordCount );
				size = sizeof( ts_RecordList );
				if ( recordList->usFlags & RECORD_LIST_QUAD_WORD_ALIGNED )
					size = ( size + 15 ) & ~15;
				return (char*)recordList + size + ( index * recordList->u32RecordSize );

			default:
				ASSERT( false );
		}
	}

	return NULL;
}

//============================================================================
//= retrieve pointer to chunk data by type and key in database with passed id.
//= DEFAULT_CHUNK_KEY is a good key value unless you have specific need of
//= multiple chunks of same type in single database

void* dblGetChunkData( u16 type, const char* name, DataMgr::OwnerRecord* owner, int* o_size )
{
	ASSERT_PTR( owner );
	ASSERT( !name || strcmp( name, DEFAULT_CHUNK_KEY_STRING ) == 0 );	// phasing out this field, make sure it's empty
	DataMgr::DataRecord* rec = owner->FindDataByType( type );
	if ( rec )
	{
		if ( o_size )
			*o_size = rec->d_size;
		return rec->d_userData;
	}
	return NULL;
}

//============================================================================
//= retrieve pointer to chunk data by type and key in database with passed id.
//= DEFAULT_CHUNK_KEY is a good key value unless you have specific need of
//= multiple chunks of same type in single database

void *dblGetChunkData( u16 type, const char* name, t_Handle Handle, int* o_size /*= NULL*/ )
{
	DBLFileRecord* file = dblFileRecordFromHandle( Handle );
	return ( file ) ? dblGetChunkData( type, name, file->owner, o_size ) : NULL;
}

//============================================================================
//= return the current file name being processed.

char* dblGetCurrentFileName()
{
	ASSERT( s_currentParser );
	return s_currentParser->FileRecord()->name;
}

//============================================================================

void dblLock( t_Handle handle )
{
	DBLFileRecord* file = dblFileRecordFromHandle( handle );
	if ( file )
	{
		file->hasBeenLocked = true;
		ASSERT( file->lockedCount < 255 );
		file->lockedCount++;
	}
}

//============================================================================

void dblUnlock( t_Handle handle )
{
	DBLFileRecord* file = dblFileRecordFromHandle( handle );
	if ( file )
	{
		ASSERT( file->hasBeenLocked );
		ASSERT( file->lockedCount > 0 );
		file->lockedCount--;
	}
}

//============================================================================

bool dblPurgeUnlocked()
{
	bool purged = false;
	for ( DataMgr::OwnerRecord* owner = DataMgr::FirstOwner() ; owner ; owner = DataMgr::NextOwner() )
	{
		if ( owner->d_type == DATA_DBL_FILE )
		{
			DBLFileRecord* file = (DBLFileRecord*) owner->d_userData;
			if ( file->hasBeenLocked && file->lockedCount == 0 )
			{
				dbgPrint( "**PURGING %s\n", file->name );
				dblRemove( (t_Handle) file, false );
				purged = true;
			}
		}
	}
	if ( purged )
		dbgPrint( "**PURGING -----\n" );
	return purged;
}

//============================================================================

bool CDBLParse::Init( const char* filename, void* data, int len, int flags )
{
	// setup parse struct
	d_fileBuffer            = (char*) data;
	d_readPtr					= (char*) data;
	d_writePtr					= (char*) data;
	d_endPtr						= (char*) data;
	d_originalSize          = len;
	d_readSize					= len;
	d_swapEndian				= false;
	d_flags						= flags;
	d_fileRecord				= NULL;
	d_mainFileRecord			= NULL;
	d_firstPersistentData	= NULL;

	// start of buffer is an ascii string -- it has the # of bytes to skip before header.
	int skip = atoi( (char*) d_readPtr );
	ASSERT( skip >= 0 && skip < d_readSize );

	// make copy of file header
	memcpy( &d_fileHeader, d_readPtr + skip, sizeof(ts_DBFileHdr) );
	SkipHeader( skip + sizeof(ts_DBFileHdr) );

	// fixup header
	#ifdef GCN
	if ( d_fileHeader.u32Version != DB_VERSION )
	{
		d_swapEndian = true;
		d_fileHeader.EndianSwap();
	}
	#endif

	// validate header version
	if ( d_fileHeader.u32Version != DB_VERSION )
	{
		ASSERTF( false, ( "Invalid DBL header, file %s bad version (want %d, got %d)", filename, DB_VERSION, d_fileHeader.u32Version ) );
		return false;
	}

	// validate header platform id
	#if defined(PS2)
	if ( strcmp( d_fileHeader.cID, "DB"    ) != 0 &&
		  strcmp( d_fileHeader.cID, "PS2"   ) != 0 )
	#elif defined(GCN)
	if ( strcmp( d_fileHeader.cID, "DB"    ) != 0 &&
		  strcmp( d_fileHeader.cID, "GCN"   ) != 0 )
	#elif defined(_XBOX)
	if ( strcmp( d_fileHeader.cID, "DB"    ) != 0 &&
		  strcmp( d_fileHeader.cID, "DX"    ) != 0 &&
		  strcmp( d_fileHeader.cID, "XBOX"  ) != 0 &&
		  strcmp( d_fileHeader.cID, "WIN32" ) != 0 &&
		  strcmp( d_fileHeader.cID, "PS2"   ) != 0 )
	#elif defined(DIRECTX)
	if ( strcmp( d_fileHeader.cID, "DB"    ) != 0 &&
		  strcmp( d_fileHeader.cID, "DX"    ) != 0 &&
		  strcmp( d_fileHeader.cID, "WIN32" ) != 0 &&
		  strcmp( d_fileHeader.cID, "PS2"   ) != 0 )
	#elif defined(WIN32)
	if ( strcmp( d_fileHeader.cID, "DB"    ) != 0 &&
		  strcmp( d_fileHeader.cID, "WIN32" ) != 0 &&
		  strcmp( d_fileHeader.cID, "GL"    ) != 0 )
	#endif
	{
		ASSERTF( false, ( "Invalid DBL header, file %s", filename ) );
		return false;
	}

	d_fileRecord     = MakeFileRecord( filename, data, len );
	d_mainFileRecord = d_fileRecord;
	d_fileRecord->owner = DataMgr::AddOwner( d_fileRecord, DATA_DBL_FILE );

	return true;
}

//============================================================================

void CDBLParse::SkipHeader( int bytes )
{
	if ( ( d_flags & DBL_NO_COLLAPSE_HEADERS ) ||
		  ( d_fileHeader.u32FileFlags & DBL_FILE_NO_COLLAPSE_HEADERS ) )
	{
		if ( d_writePtr != d_readPtr )
			memcpy( d_writePtr, d_readPtr, bytes );
		d_writePtr += bytes;
	}
	d_readPtr  += bytes;
	d_readSize -= bytes;
}

//============================================================================

bool CDBLParse::ParseChunk()
{
	if ( d_readSize > 0 )
	{
		// determine header size
		int headerSize;
		ts_DBBaseChunkHdr* chunk = (ts_DBBaseChunkHdr*) d_readPtr;
		#ifdef GCN
		if ( d_swapEndian )
			chunk->EndianSwap();
		#endif
		if ( chunk->u16Flags & DBL_HEADER_32BYTE )
			headerSize = ( chunk->u16Flags & DBL_NAMED_CHUNK ) ? sizeof(ts_DBChunkHdr) : sizeof(ts_DBBaseChunkHdr);
		else
			headerSize = ( chunk->u16Flags & DBL_NAMED_CHUNK ) ? sizeof(ts_DBChunkHdrNonAlign) : sizeof(ts_DBBaseChunkHdrNonAlign);

		// copy header
		memcpy( &d_chunkHeader, d_readPtr, headerSize );
		SkipHeader( headerSize );

		// check version
		int needVersion = GetChunkTypeVersion( d_chunkHeader.u16Type );
		ASSERTF( needVersion == -1 || needVersion == d_chunkHeader.u16Version,
			( "DBL chunk version mismatch. File:'%s' Chunk:'%s' NeedVersion=%d HaveVersion=%d",
			  d_mainFileRecord->name, GetChunkTypeString( d_chunkHeader.u16Type, false ),
			  needVersion, d_chunkHeader.u16Version ) );

		// move data for this chunk into place
		char* data = d_writePtr;
		if ( d_writePtr != d_readPtr )
		{
			memcpy( d_writePtr, d_readPtr, d_chunkHeader.u32Size );
			WRITEBACK_DATA_CACHE( d_writePtr, d_chunkHeader.u32Size );
		}
		d_writePtr += d_chunkHeader.u32Size;
		d_readPtr  += d_chunkHeader.u32Size;
		d_readSize -= d_chunkHeader.u32Size;

		// Handle raw chunk processing
		void* persistentData = ProcessChunkData( data );
		if ( !d_firstPersistentData )
			d_firstPersistentData = persistentData;

		// if chunk is flushable roll back the write ptr
		if ( d_chunkHeader.u16Flags & DBL_FLUSHABLE )
			d_writePtr = data;
		else
			d_endPtr = d_writePtr;

		// if we just finished, free up ram if we can
		if ( d_readSize == 0 )
		{
			int used = d_endPtr - d_fileBuffer;
			ASSERT( used <= d_originalSize );
			if ( used == 0 )
			{
				memFree( d_fileBuffer );
				d_mainFileRecord->fileBuffer = NULL;
			}
			else
			{
				void* newaddr = memShrink( d_fileBuffer, used );
				ASSERT( newaddr == d_fileBuffer );	// must not change address!
			}
		}
	}

	return d_readSize > 0;	// return true if more data to process
}

//============================================================================

void* CDBLParse::ProcessChunkData( void* data )
{
	s_currentParser = this;

	u32 chunkType = d_chunkHeader.u16Type;

	LoadProfile::NotifyChunk( chunkType, (int)d_chunkHeader.u32Size );

	void*				persistentData = NULL;
	u32				persistentDataSize = 0;

	#if defined (_XBOX)
	char				Fname[128];
	int				NumChunks = 0;
	int				DXDDataSize = 0;
	int*				BasePtr = (int*)data;
	ts_DXDChunk*	DXDChunk;
	#endif

	switch ( chunkType )
	{
	#if defined (_XBOX)
	case DB_XBOX_DXD:

	// get num DXD chunks
	NumChunks = *BasePtr;
	DXDDataSize = 	*((int*)((char*)BasePtr + sizeof(int)));
	DXDChunk = (ts_DXDChunk*) ((char*)data + (sizeof(int) * 2));

	if (NumChunks)
	{
		// Load the DXD Data Chunk
		CDXD_Data*	Dx;

		persistentData = (void*)new CDXD_Data;
		persistentDataSize = sizeof (CDXD_Data);
		Dx = (CDXD_Data*) persistentData;
		strcpy (Fname,  d_fileRecord->name);
		strncpy (&Fname[strlen(d_fileRecord->name) - 3], "dxd", 3);
		
		Dx->Init();
		if (!Dx->LoadDXDData(Fname, DXDDataSize))
			ASSERTF(false, ("Unable to load DXD file: %s", Fname ));
		
		// save off DBL name
		Dx->SetDBLName(d_fileRecord->name);

		// loop thru and set them all up
		for (int i = 0; i < NumChunks; i++)
		{
			Dx->SetHeader(DXDChunk->dwType, (XPR_HEADER*)((char*)BasePtr + DXDChunk->dwSystemMemoryOffset));
			Dx->InitSysmem(DXDChunk->dwType, DXDChunk->dwSystemMemorySize - sizeof (XPR_HEADER), (char*)((char*)BasePtr + DXDChunk->dwSystemMemoryOffset + sizeof (XPR_HEADER)));
			Dx->CreateResource(DXDChunk->dwType, DXDChunk->dwVideoMemoryOffset );
			DXDChunk++;
		}

		// setup vertices
		if (Dx->m_XVDresourceCreated)
			PatchSOM__CheckHWDataDXD(Dx);
	}
	d_chunkHeader.u16Flags |= DBL_FLUSHABLE;
	break;
	#endif

	case DB_SNGLSOM:
	case DB_SNGLSOMBONE:
	case DB_DMASNGLSOM:	// ps2 dma data
		LoadSingleSOM( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_SOMARRAY:
	case DB_SOMARRAYBONE:
	case DB_DMASOMARRAY:	// ps2 dma data
		LoadSOMArray( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_SOMTEXTUREIDARRAY:
		LoadSOMTextures( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

#ifdef WIN32	// only intended for use by terraintoola and obbworld, this chunk type should never be in a game .dbl file
	case DB_COLLISIONSOMARRAY:
		ASSERTS( false, "do we use this?" );
		LoadCollisionSOMArray( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;
#endif //WIN32

	case DB_LOD:
		LoadLODs( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_INSTANCE:
		CInstance::LoadInstances( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_GROUP:
		LoadGroups( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_PATHS:
		LoadPaths( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_NAMED_POINTS:
		LoadNamedPoints( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_SOUNDSOURCE:
		LoadSoundSources( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_SOUNDFIELD:
		LoadSoundFields( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_HALO:
		LoadHalo( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_BLOOM:
		LoadBloom( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_STATIONARYCAM:
		LoadStationaryCam( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_WATER:
		LoadWater( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_GLASS:
		LoadWater( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_EXPANSION_TABLE:
		LoadExpansionTables( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_PARTICLE_EFFECT:
		LoadParticleEffects( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_PARTICLE_SOURCE:
		LoadParticleSource( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_PARTICLE_EMITTER:
		LoadParticleEmitters( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_SWOOSH_SOURCE:
		LoadSwooshSource( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_SWOOSH_EMITTER:
		LoadSwooshEmitters( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_INSTANCE_GROUP:
		ASSERT(false);	//not handled yet
		break;

	case DB_TEXTURE_SET:
	case DB_MERGED_TEXTURE_SET:
		#if defined (_XBOX)
		CDXD_Data*	Dx;
		Dx = (CDXD_Data*) dblGetChunkData(DB_XBOX_DXD, DEFAULT_CHUNK_KEY_STRING, d_fileRecord->owner, NULL );

		if (Dx == NULL)
			ASSERTS(false, "Dx Pointer = 0.. are you missing a DXD file?");

		TextureMgr::AddTextureDatabase( d_fileRecord->name, (void*)Dx, data, &d_chunkHeader, &persistentData, &persistentDataSize);
		#else
		TextureMgr::AddTextureDatabase( d_fileRecord->name,0 ,data, &d_chunkHeader, &persistentData, &persistentDataSize);
		#endif

		#if defined (_XBOX)
		d_chunkHeader.u16Flags |= DBL_FLUSHABLE;
		#endif
		break;

	case DB_ANIMATED_TEXTURE_SET:
	{
		// find the database...
		TextureDatabase* pTextureDatabase = (TextureDatabase *) dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, d_fileRecord->owner, NULL );
		if (pTextureDatabase == NULL)
			pTextureDatabase = (TextureDatabase *) dblGetChunkData(DB_MERGED_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, d_fileRecord->owner, NULL );
		ASSERT_PTR(pTextureDatabase);

		// stitch in animated set...
		pTextureDatabase->StitchAnimatedSetIntoPlace( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;
	}

	case DB_MOTION:
		LoadMOT( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_MOTION_NAMES:
		LoadMOTNames( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_BONEINFO:
		LoadBoneInfo( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_BONE_GROUP_MIRRORS:
		LoadBoneGroupInfo( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_TEXTURE_CONTEXT:
		TextureMgr::LoadTextureContexts( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_TERRAIN_LIST:
	case DB_EMITTER_LIST:
	case DB_EFFECT_LIST:
	case DB_PARTICLE_LIST:
	case DB_PARTICLE_SURFACE_LIST:
	case DB_SWOOSH_LIST:
	case DB_SWOOSH_EMITTER_LIST:
		LoadNameList(  data, &d_chunkHeader, &persistentData, &persistentDataSize );
		break;

	case DB_WORLD_EMITTERS:
		LoadWorldEmitters( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_WORLD_DISTORTIONS:
		LoadWorldDistortions( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_SCENE_TREE:
		LoadSceneTree( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_VIS_REGIONS:
		LoadVisRegions( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_LIGHTS:
		loadLightSet( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

//yuch-- no loaders for these yet
	case DB_SECTORS:
		d_chunkHeader.u16Flags |= DBL_FLUSHABLE;
		break;

	case DB_INSTANCE_ATTRIBUTES:
		CInstance::LoadInstanceAttributes( data, &d_chunkHeader, &persistentData, &persistentDataSize);
		d_chunkHeader.u16Flags |= DBL_FLUSHABLE;
		break;

	case DB_CDB:
		TerrainCollideLayerList::FixDiskLoad( data);
		persistentData =  data;
		persistentDataSize = d_chunkHeader.u32Size;
		d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
		break;

	case DB_ODB:
		ObjectLayerList::FixDiskLoad( data);
		persistentData =  data;
		persistentDataSize = d_chunkHeader.u32Size;
		d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
		break;

	case DB_PDB:
	case DB_CPH:
		persistentData =  data;
		persistentDataSize = d_chunkHeader.u32Size;
		d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
		break;

	case DB_NAV:
		persistentData =  data;
		Navigate::Mesh::DiskFixup(  data );
		Navigate::Manager::RegisterMesh( ( Navigate::Mesh* ) data );
		persistentDataSize = d_chunkHeader.u32Size;
		d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
		break;

	case DB_CUT_BINARY:
		// no fixup required-- just make sure data is persistant
		persistentData =  data;
		persistentDataSize = d_chunkHeader.u32Size;
		d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
		break;

	case DB_ENV_BINARY:
	case DB_CONTEXT_BINARY:
	case DB_FILESTREAM:
		// no fixup required-- just make sure data is persistant
#ifdef GCN
		{
			BOOL	enabled;

			enabled = OSDisableInterrupts( );
			OSDisableScheduler( );
			OSRestoreInterrupts( enabled);

			t_Handle		hSave = memSetActiveHandle( Layers__GetVirtualHeap( ));

			void			*pVirtualData = memAlloc( d_chunkHeader.u32Size);

			memSetActiveHandle( hSave);

			enabled = OSDisableInterrupts( );
			OSEnableScheduler( );
			OSRestoreInterrupts( enabled);

			if( pVirtualData == NULL)
			{
				persistentData =  data;
				persistentDataSize = d_chunkHeader.u32Size;
				d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
				break;
			}

			memcpy( pVirtualData,  data, d_chunkHeader.u32Size);

			persistentData = pVirtualData;
			persistentDataSize = d_chunkHeader.u32Size;
			d_chunkHeader.u16Flags |= DBL_FLUSHABLE;
		}
#else
		persistentData =  data;
		persistentDataSize = d_chunkHeader.u32Size;
		d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
#endif
		break;

	case DB_ANIM_CURVE:
		// just have to resolve record list, no other fixup required-- make sure data is persistant
#ifdef GCN
		{
			ResolveRecordListSwapped( (ts_RecordList *) data);
			ts_RecordList *pRecordList = (ts_RecordList *) data;
			for( int i = 0; i < pRecordList->usRecordCount; i++)
			{
				ts_AnimCurve *pCurve = (ts_AnimCurve *)GetRecordAddress( pRecordList, i);
				pCurve->EndianSwap( );
				// endian swap...
			}
		}
#else
		ResolveRecordList( (ts_RecordList *) data);
#endif
		persistentData =  data;
		persistentDataSize = d_chunkHeader.u32Size;
		d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
		break;

	case DB_XML_SOURCE:
		persistentData = xmlParseBuffer( (unsigned char *) data, d_chunkHeader.u32Size);
		d_chunkHeader.u16Flags |= DBL_FLUSHABLE;	// discard source text
		break;

	case DB_OA_SOURCE:
		ASSERTS(false, "dbl-wrapped .oa source no longer supported");
		d_chunkHeader.u16Flags |= DBL_FLUSHABLE;	// discard source text
		break;

	case DB_SKIP_CHUNK:
		// caller-supplied callback said to skip this chunk entirely
		break;

	case DB_FILENAME_CHUNK:
		// from uber file, name of file that contributed data to uber file, for already-loaded-file checking
		// in dblFindFile
		d_fileRecord = MakeFileRecord( (char*)data, NULL, 0 );	// NULL since this doesn't own file buffer
		d_fileRecord->owner = DataMgr::AddOwner( d_fileRecord, DATA_DBL_FILE );

		// if we're keeping headers, keep these too - too many to collapse
		if ( ( d_flags & DBL_NO_COLLAPSE_HEADERS ) ||
			  ( d_fileHeader.u32FileFlags & DBL_FILE_NO_COLLAPSE_HEADERS ) )
		{
			d_chunkHeader.u16Flags &= ~DBL_FLUSHABLE;
		}
		break;

	case DB_SOUNDBANK_3D:
	case DB_SOUNDBANK_2D:
		ASSERT( false );		// old format of sound data no longer supported
		break;

	case DB_SOUNDBANK_MOD:
		g_audio.LoadSoundBankChunk(  data, &d_chunkHeader, &persistentData, &persistentDataSize);
		break;

	case DB_CURVES:
		// new functionality, currently not supported
		break;

	case DB_TEXTURE_NAMES:
		// new functionality, currently not supported
		break;

	case DB_ATTACH_POINTS:
		// Save this for use in a later API
		ASSERT(((uint)data % 16) == 0);
		persistentData = data;
		persistentDataSize = d_chunkHeader.u32Size;
		d_chunkHeader.u16Flags &= ~(DBL_FLUSHABLE);
		break;

	default:
		ASSERT( ( d_chunkHeader.u16Flags & DBL_CHUNK_NO_IGNORE ) == false );
		break;
	}

	if ( persistentData )
		d_fileRecord->owner->AddData( persistentData, d_chunkHeader.u16Type, 0, persistentDataSize );

	s_currentParser = NULL;

	return persistentData;
}

//============================================================================
//= clean up a chunk before removing it, like memFree's or making sure
//= contents aren't referred to from outside.  e.g. for instance chunk we might
//= make sure that no instance in the chunk is in the scene manager's instance
//= list.  if chunk allocated outside memory, set pointer here to free it (or
//= free it yourself in chunk removing routine)

void CDBLParse::ReleaseChunkData( void* data, int type )
{
	int i;
	ts_RecordList* pRecordList;
	void *pOutsideBuffer = NULL;

	switch( type )
	{

	case DB_INSTANCE:
		pRecordList = (ts_RecordList *) data;
		for ( i = 0 ; i < (int)pRecordList->usRecordCount ; i++ )
		{
			CInstance* inst = (CInstance*) GetRecordAddress( pRecordList, i );
			inst->Destroy();
		}
		break;

	case DB_TEXTURE_SET:
	case DB_MERGED_TEXTURE_SET:
		TextureMgr::RemoveTextureDatabase((TextureDatabase*)data);
		break;

	case DB_ANIMATED_TEXTURE_SET:
		break;

	case DB_SNGLSOM:
	case DB_SNGLSOMBONE:
	case DB_DMASNGLSOM:
		{
			ts_PatchSOM *pSOM = (ts_PatchSOM *) data;
			_dblCleanupSOM( pSOM );
			break;
		}

	case DB_SOMARRAY:
	case DB_SOMARRAYBONE:
	case DB_DMASOMARRAY:
		pRecordList = (ts_RecordList *) data;
		for (i = 0; i < (signed) pRecordList->usRecordCount; i++)
		{
			ts_PatchSOM *pSOM = (ts_PatchSOM *) GetRecordAddress(pRecordList, i);
			_dblCleanupSOM( pSOM );
		}
		break;

	case DB_MOTION:
		// motion data gets tracked by motLoadMotionChunk
		DataMgr::RemoveOwner( DataMgr::FindOwner( data ) );

#ifdef GCN
		// for gcn, motion data gets put into virtual memory, and needs to be free'd
		pOutsideBuffer = data;
#endif //GCN
		break;

	case DB_MOTION_NAMES:
		{
			MotionNameHash* nameHash = (MotionNameHash*) data;
			delete nameHash;
		}
		break;

	case DB_VIS_REGIONS:
		pRecordList = (ts_RecordList *) data;
		for (i = 0; i < (signed) pRecordList->usRecordCount; i++)
		{
			CVisRegion *pVisRegion = (CVisRegion *) GetRecordAddress(pRecordList, i);
			pVisRegion->FreeOBB();
		}
		break;

	case DB_XML_SOURCE:
		// free parsed tree
		xmlFreeElement((xmlElement *) data);
		break;

	case DB_ENV_BINARY:
		#ifdef GCN
		// for gcn, filestream data gets put into virtual memory, and needs to be free'd
		pOutsideBuffer = data;
		#endif //GCN
		break;

	// nothing to do for these chunk types, but i want them listed so that the default statement below is meaningful
	case DB_CDB:
	case DB_ODB:
	case DB_PDB:
	case DB_CPH:
		break;

	case DB_NAV:
		break;

	case DB_CONTEXT_BINARY:
	case DB_FILESTREAM:
		#ifdef GCN
		// for gcn, filestream data gets put into virtual memory, and needs to be free'd
		pOutsideBuffer = data;
		#endif //GCN
		break;

	case DB_SOUNDBANK_MOD:
		g_audio.UnloadBank( (SoundBank*) data );
		break;

	// nothing to do for these chunk types, but i want them listed so that the default statement below is meaningful
	case DB_TEXTURE_CONTEXT:
	case DB_PARTICLE_SOURCE:
	case DB_PARTICLE_EFFECT:
	case DB_PARTICLE_EMITTER:
	case DB_SWOOSH_SOURCE:
	case DB_SWOOSH_EMITTER:
	case DB_EMITTER_LIST:
	case DB_EFFECT_LIST:
	case DB_PARTICLE_LIST:
	case DB_SWOOSH_LIST:
	case DB_SWOOSH_EMITTER_LIST:
	case DB_BONEINFO:
	case DB_PATHS:
	case DB_NAMED_POINTS:
	case DB_WATER:
	case DB_GLASS:
	case DB_SCENE_TREE:
	case DB_TERRAIN_LIST:
	case DB_WORLD_EMITTERS:
	case DB_LIGHTS:
	case DB_ANIM_CURVE:
	case DB_SOUNDSOURCE:
	case DB_SOUNDFIELD:
	case DB_HALO:
	case DB_BLOOM:
	case DB_STATIONARYCAM:
	case DB_CUT_BINARY:
	case DB_BONE_GROUP_MIRRORS:
	case DB_ATTACH_POINTS:
	case DB_XBOX_DXD:					// gets closed down afterwards
		break;

	default:
		// trying to clean up an unknown chunk type, typically this is bad. if the chunk type requires no
		// clean up, it should go into the nothing-to-case statement above
		ASSERTF(false, ("Unhandled chunk type in clean up: %s", CDBLParse::GetChunkTypeString(type, false)));
		break;
	}

	// chunk may have allocated data outside file buffer. if so, free it
	if (pOutsideBuffer)
		memFree(pOutsideBuffer);
}

//============================================================================

DBLFileRecord* CDBLParse::MakeFileRecord( const char* filename, void* data, int len )
{
	DBLFileRecord* file		= (DBLFileRecord*) memAlloc( sizeof(DBLFileRecord) + strlen(filename) );
	file->fileBuffer			= data;
	file->permanent			= false;
	file->hasBeenLocked     = false;
	file->lockedCount       = 0;
	file->distortionGroup	= DistortionMgr__RequestReservedGroupID();
	file->emitterGroup		= ParticleMgr__RequestReservedEmitterGroupID();

	// copy name, fix slashes
	char* d = file->name;
	for ( const char* s = filename ; s[0] ; s++ )
		*d++ = ( s[0] == '/' ) ? '\\' : s[0];
	*d = 0;

	return file;
}

//============================================================================

int CDBLParse::GetChunkTypeVersion( int type )
{
	switch ( type )
	{
		#define CHUNKDESC(chunk,id,ver,nice) case chunk: return ver;
		#include "Game/Database/DBLChunkDesc.h"
		#undef CHUNKDESC

		default:
			return -1;
	}
}

//============================================================================

const char* CDBLParse::GetChunkTypeString( int type, bool easy )
{
	switch ( type )
	{
		#define CHUNKDESC(chunk,id,ver,nice) case chunk: return easy ? nice : #chunk;
		#include "Game/Database/DBLChunkDesc.h"
		#undef CHUNKDESC

		default:
			return easy ? "Unknown data" : "UNKNOWN";
	}
}

//============================================================================
