//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellFile header file
//
// $Workfile: shellFile.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SHELLFILE_H_
#define _SHELLFILE_H_

//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------

// These correspond to libcdvd's SCECdEr definitions

typedef enum
{
	SHELLFILE_ER_FAIL    = -1,
	SHELLFILE_ER_NO      = 0x00,
	SHELLFILE_ER_EOM     = 0x32,
	SHELLFILE_ER_TRMOPN  = 0x31,
	SHELLFILE_ER_READ    = 0x30,
	SHELLFILE_ER_CUD     = 0x14,
	SHELLFILE_ER_NORDY   = 0x13,
	SHELLFILE_ER_ABRT    = 0x01
	
} EShellFileError;

// These correspond to libcdvd's SCECdStat definitions

typedef enum
{
	SHELLFILE_STAT_SHELLOPEN  = 0x01,
	SHELLFILE_STAT_STOP       = 0x00,
	SHELLFILE_STAT_SPIN       = 0x02,
	SHELLFILE_STAT_READ       = 0x06,
	SHELLFILE_STAT_PAUSE      = 0x0a,
	SHELLFILE_STAT_SEEK       = 0x12,
	SHELLFILE_STAT_EMG        = 0x20
	
} EShellFileStat;

typedef enum
{
	SHELLFILE_NON_BLOCKING = 0,
	SHELLFILE_BLOCKING = 1
	
} EShellFileBlockingMode;

typedef enum
{
	SHELLFILE_MEDIUM_HOST0,
	SHELLFILE_MEDIUM_CD,
	SHELLFILE_MEDIUM_DVD,
	SHELLFILE_MEDIUM_SCEEDEMO,
	SHELLFILE_MEDIUM_ATMON
	
} EShellFileMedium;

#define BYTES_TO_SECTORS(x) (((x)+2047)/2048)


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

void           shellFileInit( EShellFileMedium medium, int rebootIop,
							  const char *iopModPath, void *medParam );
void           shellFileNativeName( const char *src, char *dest );
int            shellFileLoadStartModule(const char *name, int args, const char *argp,
										int *pResult );
EShellResult    shellFileStopUnloadModule( int id );
int            shellFileDiskReady( EShellFileBlockingMode blkMode );
EShellFileError shellFileGetError( void );
unsigned int   shellFileGetSize( const char *name );
EShellResult    shellFileRead( const char *name, EShellFileBlockingMode blkMode, void *eeAddr,
							int sectorOffset, int sectors );
void          *shellFileLoad( const char *name, unsigned int alignment, unsigned int *length);
int            shellFileGetLoadPos( void );
EShellResult    shellFileLoadChain( const char **names, EShellFileBlockingMode blkMode, void **eeIopAddrs );
EShellResult    shellFileLoadIOPm( const char *name, EShellFileBlockingMode blkMode, int iopAddr );
EShellResult    shellFilePause( void );
EShellResult    shellFileSeek( const char *name, int sectorOffset );
EShellResult    shellFileStandby( EShellFileBlockingMode blkMode );
EShellFileStat  shellFileStatus( void );
EShellResult    shellFileStop( EShellFileBlockingMode blkMode );
EShellResult    shellFileStInit( int sectors, int partitions, int iopAddr );
EShellResult    shellFileStStart( char *name, int sectorOffset );
int            shellFileStRead( void *eeAddr, int sectors, EShellFileBlockingMode blkMode );
EShellResult    shellFileStStop( void );
EShellResult    shellFileStSeek( int sectors );
int            shellFileStStat( void );
EShellResult    shellFileSync( EShellFileBlockingMode blkMode );
EShellResult    shellFileTrayReq( int command, int *pTrayCnt );

#ifdef __cplusplus
}
#endif

#endif _SHELLFILE_H_
