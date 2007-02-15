//============================================================================
//=
//= DBFile.h - DBL file loader and resource manager
//=
//= DBL files are multi-purpose database files.  Each DBL is built up from
//= one or more chunks.
//=
//============================================================================

#ifndef _DBFILE_H
#define _DBFILE_H

#include "platform/BaseType.h"
#include "Layers/LayersStruct.h"
#include "Game/Database/DataMgr.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

//============================================================================

struct ts_Geometry;				// in World.h
class TextureDatabase;

//============================================================================

#define DB_VERSION				0x100

// flags for DBLParse::Init
#define DBL_NO_COLLAPSE_HEADERS			0x00000004	// do not collapse file and chunk headers

/* byte alignment for data, files */

#define ALIGN_BYTES		4	// valid values are 1, 2, 4, 8, 16, 32, etc
#define IS_ALIGNED(n)	((n & (ALIGN_BYTES - 1)) == 0)
#define ALIGNED_SIZE(n)	((n + (ALIGN_BYTES - 1)) & ~(ALIGN_BYTES - 1))

/* record list. a common form of chunk data is a list of records, like SOM model data or instance records. we typically need
random access into the record list. this can be handled by an offset table, one entry per record, that gets resolved at load
time from an offset from the beginning of the chunk into a pointer to the record. if all records are the same length, another
way is just to multiply the index by the record size. a third way is to parse the records until you find the requested one.
offset tables use RAM, of course, but are fastest. if you need fast random access to the records during the game, you might
choose an offset table. if not, one of the other methods might be better */

#define RECORD_LIST_HAS_OFFSET_TABLE		0x0001
#define RECORD_LIST_SAME_SIZE_RECORDS		0x0002
#define RECORD_LIST_TYPE_MASK					0x0003
#define RECORD_LIST_OFFSET_TABLE_RESOLVED	0x0004	// set once the offset table has been resolved into pointers
#define RECORD_LIST_QUAD_WORD_ALIGNED		0x0008	// first data record starts on quad-word (16-byte) boundary. record list size gets padded out to 16-byte boundary to implement

/* chunk name length */

#define DBL_MAXNAME					32

/* default chunk key. for named chunks, chunk key is result of atoi on cName field */

#define DEFAULT_CHUNK_KEY			1000
#define DEFAULT_CHUNK_KEY_STRING	"1000"	// for new named chunk approach

/* flags for ts_DBChunkHdr.usFlags **AND** ts_DBFileHdr.u32FileFlags */

#define DBL_FLUSHABLE						0x0001
#define DBL_NAMED_CHUNK						0x0002	// chunk has cName field, used to carry a numeric string. ts_DBChunkHdr only
#define DBL_CHUNK_NO_IGNORE				0x0004	// game engine must be able to load chunk-- asserts if it doesn't know how
#define DBL_HEADER_32BYTE					0x0008	// chunk header has been padded to a multiple of 32 bytes
#define DBL_FILE_NO_COLLAPSE_HEADERS	0x0010	// prevents collapsing any headers in file

// CHUNK TYPES
enum {
#define CHUNKDESC(chunk,id,ver,nice) chunk = id,
#include "Game/Database/DBLChunkDesc.h"
#undef CHUNKDESC
};

// CHUNK VERSIONS
enum {
#define CHUNKDESC(chunk,id,ver,nice) chunk ## _VERSION = ver,
#include "Game/Database/DBLChunkDesc.h"
#undef CHUNKDESC
};


#if defined (_XBOX)
struct ts_DXDChunk
{
	DWORD						dwType;
	DWORD						dwSystemMemorySize;
	DWORD						dwSystemMemoryOffset;
	DWORD						dwVideoMemorySize;
	DWORD						dwVideoMemoryOffset;
	char						chPad[12];
};
#endif
//============================================================================

/* chunk record list */

#pragma pack(1)	// pack byte-by-byte
struct ts_RecordList
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	unsigned short usRecordCount PRAGMAPACK;
	unsigned short	usFlags		  PRAGMAPACK;
	union
	{
		u32	u32OffsetTable[1]	PRAGMAPACK;	// variable-length data: unresolved offset table (offsets from chunk data start)
		void	*pRecordTable[1]	PRAGMAPACK;	// variable-length data: resolved offset table (pointers to records)
		u32	u32RecordSize		PRAGMAPACK;	// all-same-length data: record size
	};

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};
#pragma pack()	// back to default

//============================================================================

/* unnamed version of chunk header */

#pragma pack(1)	// pack byte-by-byte
struct ts_DBBaseChunkHdr
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

   u16		u16Type		PRAGMAPACK;
   u16		u16Flags		PRAGMAPACK;
   u32		u32Size		PRAGMAPACK;
	u16		u16Version	PRAGMAPACK;
	char		filler[22]	PRAGMAPACK;
#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( u16Type);
		ByteSwap( u16Flags);
		ByteSwap( u32Size);
		ByteSwap( u16Version);
	}
#endif	//GCN

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};
#pragma pack()	// back to default

//============================================================================

/* unnamed version of chunk header */

#pragma pack(1)	// pack byte-by-byte
struct ts_DBBaseChunkHdrNonAlign
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

   u16		u16Type		PRAGMAPACK;
   u16		u16Flags		PRAGMAPACK;
   u32		u32Size		PRAGMAPACK;
	u16		u16Version	PRAGMAPACK;
#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( u16Type);
		ByteSwap( u16Flags);
		ByteSwap( u32Size);
		ByteSwap( u16Version);
	}
#endif	//GCN

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};
#pragma pack()	// back to default

//============================================================================

/* named version of chunk header */

#pragma pack(1)	// pack byte-by-byte
struct ts_DBChunkHdr
{
   u16		u16Type					PRAGMAPACK;
   u16		u16Flags					PRAGMAPACK;
   u32		u32Size					PRAGMAPACK;
	u16		u16Version				PRAGMAPACK;
   char		cName[DBL_MAXNAME]	PRAGMAPACK;
	char		filler[22]				PRAGMAPACK;
#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( u16Type);
		ByteSwap( u16Flags);
		ByteSwap( u32Size);
		ByteSwap( u16Version);
	}
#endif	//GCN
};
#pragma pack()	// back to default

#pragma pack(1)	// pack byte-by-byte
struct ts_DBChunkHdrNonAlign
{
   u16		u16Type					PRAGMAPACK;
   u16		u16Flags					PRAGMAPACK;
   u32		u32Size					PRAGMAPACK;
	u16		u16Version				PRAGMAPACK;
   char		cName[DBL_MAXNAME]	PRAGMAPACK;
#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( u16Type);
		ByteSwap( u16Flags);
		ByteSwap( u32Size);
		ByteSwap( u16Version);
	}
#endif	//GCN
};
#pragma pack()	// back to default

//============================================================================

/* database file header */

#define SKIP_COUNT_BYTES 8
#define ID_BYTES 8
#pragma pack(1)	// pack byte-by-byte
struct  ts_DBFileHdr
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	char		cSkipCount[SKIP_COUNT_BYTES]	PRAGMAPACK;	// for variable-size text field at start of .dbl file. see comments in _dblGetFileHdr in DBFile.cpp
	char		cID[ID_BYTES]						PRAGMAPACK;	// export platform, or "DB" for generic data
	u32		u32Version							PRAGMAPACK;
	u32		u32FileFlags						PRAGMAPACK;
	u32		u32Chunks							PRAGMAPACK;
	u32		u32MaxChunkSize					PRAGMAPACK;
#ifdef GCN
	inline void EndianSwap( void)
	{
		ByteSwap( u32Version);
		ByteSwap( u32FileFlags);
		ByteSwap( u32Chunks);
		ByteSwap( u32MaxChunkSize);
	}
#endif	//GCN

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};
#pragma pack()	// back to default

//============================================================================

void					dblInit(int nMaxCustomChunkLoaders);
void					dblReset(bool bRemovePermanent = false);
void					dblShutdown(void);
t_Handle				dblLoad( const char* filename );
t_Handle				dblLoad( const char* filename, void* data, int size );
t_Handle				dblFindFile( const char *pFileID);
char *				dblGetFileName( t_Handle Handle );
bool					dblRemove(t_Handle Handle, bool bRemovePermanent = false);
bool					dblRemoveIfUnused(t_Handle Handle);
void					dblRemoveData(t_Handle Handle, void *pData);
char *				dblGetChunkName(ts_DBChunkHdr *pChunkHdr);
void *				dblGetChunkData(u16 u16Type, const char *pChunkName, t_Handle Handle, int *pnSize = NULL);
void *				dblGetChunkDataCurrentDBL(u16 u16Type, char *pChunkName, int *pnSize = NULL);
void *				dblGetChunkDataAllDBLs(u16 u16Type, char *pChunkName, int *pnSize = NULL);
u16					dblGetDistortionGroupID(t_Handle Handle);
u16					dblGetEmitterGroupID(t_Handle Handle);
TextureDatabase*	dblGetTextures(t_Handle handle, char* chunkName = NULL);
void					dblGetGeometry(ts_Geometry *pGeometry, t_Handle Handle, char *pChunkName = NULL);
t_Handle				dblGetHandleFromGeometry(ts_Geometry *pGeometry);
void					ResolveRecordList(ts_RecordList *pListHeader);
#ifdef GCN
void					ResolveRecordListSwapped(ts_RecordList *pListHeader);
#endif
void *				GetRecordAddress(ts_RecordList *pListHeader, u32 u32Index);
void					dblSetPermanent(t_Handle Handle, bool bPermanent);
bool					dblIsPermanent(t_Handle Handle);
char*					dblGetCurrentFileName();
void					dblLock(t_Handle handle);
void					dblUnlock(t_Handle handle);
bool					dblPurgeUnlocked();

//============================================================================

#endif //_DBFILE_H

//============================================================================
