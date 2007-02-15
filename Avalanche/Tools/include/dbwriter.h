#ifndef __DBWRITER_H
#define __DBWRITER_H

#include <mpostr.h>

//#include <dbstructs.h>
#include "game/database/dbfile.h"

#undef CLASS_EXPORT

#if defined DLL_EXPORT
#define CLASS_EXPORT	_declspec(dllexport)
#elif defined DLL_IMPORT
#define CLASS_EXPORT __declspec(dllimport)
#else
#define CLASS_EXPORT
#endif

#define DB_VERSION			0x100

#define CHK_FLUSHABLE		0x0001
#define CHK_NAMEDCHNK		0x0002

class CLASS_EXPORT DBWriter
{
	bool					bOwnStream;

	long					lChunkHdrStart;
	long					lChunkStart;
	long					lFileHdrStart;
	long					lFileStart;

	mpostream			*pMyStream;

	ts_DBFileHdr		FileHdr;
	ts_DBChunkHdr		ChunkHdr;
	ts_DBBaseChunkHdr	BaseChunkHdr;
	ts_DBChunkHdr	  	DefChunkHdr;


	void 				Init( void);
	void 				WriteChunkHdr( unsigned short usType, t_sflagbits dblFlags, unsigned short usVersion, const char *pcName);

public:
	enum db_stream
	{
		PCSt = 0,
		N64St,
		PSXSt,

		ST_MAX
	} ;

	DBWriter( void);
	~DBWriter( void);
	DBWriter( mpostream &str);
	DBWriter( const char *pcFName, db_stream Type);

	bool				Attach( mpostream &str);
	bool				Attach( const char *pcFName, db_stream Type);
	mpostream &		stream( void);
	bool				stream_open( void);
	bool				DBStart( t_lflagbits dbStartFlags, char *pComment = NULL);
	bool				DBEnd( void);
	bool				ChunkStart( unsigned short usType, t_sflagbits dbChunkFlags, unsigned short usVersion, const char *pcName = NULL);
	bool				ChunkEnd( void);





} ;

mpostream &operator<<( mpostream &mpostream, ts_DBFileHdr &hdr);
mpostream &operator<<( mpostream &mpostream, ts_DBChunkHdr &hdr);


#endif
