//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellFile - medium-independent file access
//
// $Workfile: shellFile.c $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"
#include <libver.h>


//-----------------------------------------------------------------------------
// M A C R O S ,   C O N S T A N T S
//-----------------------------------------------------------------------------

#define IOP_COPY_CHUNK_SIZE 2048
#define SHARED_MOD_TABLE_SIZE 16


//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef struct _FileInfo
{
	int  lsn;
	int  bytes;
	char nativeName[128];
	
} FileInfo;

typedef struct _SharedMod
{
	int id;
	int users;
	int result;

} SharedMod;


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

static void _GetIopImagePath( char *path );
static int _GetIopModulePath( const char *modName, char *result );
#ifdef DEBUG
static void _CheckIopName( const char *name );
#endif
static void _NativeName( const char *src, char *dest );
static EShellResult _GetInfo( const char *name, FileInfo *pFileInfo );
static EShellResult _SyncReturn1( EShellFileBlockingMode blkMode );
static EShellResult _SyncLoadCore( EShellFileBlockingMode blkMode );
static EShellResult _LoadCore( const FileInfo *pFileInfo, EShellFileBlockingMode blkMode, void *eeIopAddr,
					  int offset, int sectors );
static EShellResult _SyncLoadChain( EShellFileBlockingMode blkMode );
static EShellResult _SyncLoadIOPm( EShellFileBlockingMode blkMode );


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S
//-----------------------------------------------------------------------------

static int _shellFileInitCalled = 0;
static const char *_iopModPath;
static EShellFileMedium _medium;
static int _isLibcdvdMedium;
static SharedMod _sharedModTable[SHARED_MOD_TABLE_SIZE];
static int _file;
static sceCdRMode _fileMode =
{
	0,              // try count (0 = try 256 times)
	SCECdSpinNom,   // spindle control
	SCECdSecS2048   // data pattern
};

static sceCdRMode _streamMode =
{
	0,              // try count (0 = try 256 times)
	SCECdSpinStm,   // spindle control
	SCECdSecS2048   // data pattern
};
static EShellFileStat _kernelStatus;
static EShellFileError _kernelError;
static EShellResult (*_fileSyncFunc)     ( EShellFileBlockingMode blkMode ) = _SyncReturn1;
static EShellResult (*_realFileSyncFunc) ( EShellFileBlockingMode blkMode );
static u_int _loadChainTag[64 * 3];
static char _namesCpy[64][128];
static void *_eeIopAddrsCpy[64];
static int _curTagInd;
static int _amountRead;
static int _iopAddr;
static char _iopCopyBuf[IOP_COPY_CHUNK_SIZE] data_align(64);
static int _fileSizeBytes;
static int _stLsn;
static int _stBufSize;
static int _stLength;
static int _stPos;
static char _kernelName[64];    // "host0:" or "atfile:host[!port],"
static u_int _kernelNameLen;


//-----------------------------------------------------------------------------
// S T A T I C   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// _GetIopImagePath()
// Description: obtains the path of the expected location of the IOP image file
// Parameters: path - pointer to string to store result
// Notes: The directory used for the IOP image file is the first one
//        specified in the IOP search path.
//-----------------------------------------------------------------------------
static void _GetIopImagePath( char *path )
{
	char *s, *d;
	
	if(_isLibcdvdMedium)
	{
		strcpy(path, "cdrom0:\\");
		d = &path[8];
		s = (char *) _iopModPath;
		
		while(*s && (*s != ';')) *(d++) = (char) toupper(*(s++));

		// Directory-delimiting slash, only if necessary		
		if(*_iopModPath && (*_iopModPath != ';')) *(d++) = '\\';

		s = IOP_IMAGE_FILE;
		while(*s) *(d++) = (char) toupper(*(s++));
		
		*(d++) = ';';
		*(d++) = '1';
		*d = 0;	
	}
	
	else
	{
		strcpy(path, "host0:");
		d = &path[6];
		s = (char *) _iopModPath;
		
		while(*s && (*s != ';')) *(d++) = *(s++);

		// Directory-delimiting slash, only if necessary		
		if(*_iopModPath && (*_iopModPath != ';')) *(d++) = '/';

		s = IOP_IMAGE_file;
		while(*s) *(d++) = *(s++);

		*d = 0;	
	}
}

//-----------------------------------------------------------------------------
// _GetIopModulePath()
// Description: scans through the IOP search path for a filename
// Parameters: modName - filename with no directory, eg. 'libsd.irx'
//             result  - pointer to string to store result
// Returns: 1 = filename found (stored in result), otherwise zero.
//-----------------------------------------------------------------------------

static int _GetIopModulePath( const char *modName, char *result )
{
	int fd;
	char *d;
	char *s = (char *) _iopModPath;
	char *modNameC;
	
	while(1)
	{
		d = result;
	
		if(_isLibcdvdMedium)
		{
			strcpy(d, "cdrom0:\\");
			d += 8;
			
			while(*s && (*s != ';'))
			{
				*(d++) = (char) toupper(*(s++));
				if(*(d-1) == '/') *(d-1) = '\\';
			}

			// Only add the \ if the directory is not empty: the empty
			// string means cdrom0:\<modName>;1, not cdrom0:\\<modName>;1
			if(d != result + 8)
			{
				*(d++) = '\\';
				*d = 0;
			}
		
			modNameC = (char *) modName;
			while(*modNameC) *(d++) = (char) toupper(*(modNameC++));
			
			*(d++) = ';';
			*(d++) = '1';
			*d = 0;
		}

		else
		{
			strcpy(d, _kernelName);
			d += _kernelNameLen;
			
			while(*s && (*s != ';')) *(d++) = *(s++);
			
			*d = 0;
		
			// Only add the / if the directory is not empty: the empty
			// string means host0:<modName>;1, not host0:/<modName>;1
			if(d != result + _kernelNameLen)
			{
				*(d++) = '/';
				*d = 0;
			}

			strcat(d, modName);
		}
		
		if((fd = sceOpen(result, SCE_RDONLY, 0)) >= 0)
		{
			sceClose(fd);
			return 1;
		}
		
		if(!*s++) break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// _CheckIopName()
// Description: verifies that the parameter is in the correct format for
//              use in a shellFile load IOP module function.
// Notes: only defined for debug builds; is an empty statement otherwise
//-----------------------------------------------------------------------------
#ifdef DEBUG
static void _CheckIopName( const char *name )
{
	int count = 0;
	const char *n = name;
	
	while(*n)
    {
		switch(*(n++))
		{			
		case 'a' ... 'z':
		case 'A' ... 'Z':
		case '0' ... '9':
		case '_':
			break;
			
		default:
			goto fail;
		}
		
		if(++count>8) goto fail;
    }
	
	return;
	
 fail:	
	WARNING(0, "Format of IOP module name '%s' not suitable.", name);
}
#else
#define _CheckIopName(x) ;
#endif

//-----------------------------------------------------------------------------
// _NativeName()
// Description: converts 'src' to a filename native to the medium specified
//              in shellFileInit. Stores result in 'dest'.
//-----------------------------------------------------------------------------
static void _NativeName( const char *src, char *dest )
{
	// "dir/file" (libcdvd) -> "\DIR\FILE;1"
	// "dir/file" (kernel)  -> "host0:dir/file"
	//                          or "atfile:host[!port],dir/file"
	
	if(_isLibcdvdMedium)
	{
		char *d = dest;
		
		*(d++) = '\\';
		do
		{
			*d = (char) toupper(*(src++));
			if(*d=='/') *d = '\\';
		}
		while(*(d++));
		
		strcat(dest, ";1");
	}

	else  // Kernel filename
	{
		strcpy(dest, _kernelName);
		strcpy(dest + _kernelNameLen, src);
	}
}

//-----------------------------------------------------------------------------
// _GetInfo()
// Description: fills in a fileInfo structure
// Returns:  0 = error occurred
//           1 = no error occurred
// Notes: Result contains the filename converted to a medium-native format,
// and if the target is CD, the sector number and filesize are read from the
// TOC.
//-----------------------------------------------------------------------------
static EShellResult _GetInfo( const char *name, FileInfo *pFileInfo )
{
	_NativeName(name, pFileInfo->nativeName);

	if(_isLibcdvdMedium)
    {      
		sceCdlFILE fp;
		
		if(!sceCdSearchFile(&fp, pFileInfo->nativeName))
		{
			WARNING(0, "sceCdSearchFile failed for '%s'",
					pFileInfo->nativeName);
			return SHELL_FAIL;
		}
		
		pFileInfo->lsn   = (int) fp.lsn;
		pFileInfo->bytes = (int) fp.size;
    }
    
    return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// _SyncReturn1()
// Description: dummy function substituted to _fileSyncFunc to always report
//              that a shellFile operation has completed.
// Returns: 1
//-----------------------------------------------------------------------------
static EShellResult _SyncReturn1( EShellFileBlockingMode blkMode )
{
	// No operation is known to be in progress, hence vacuously we've completed
	
	(void) blkMode;    // To prevent unused parameter warnings
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// _SyncLoadCore()
// Description: performs shellFileSync when the command to be synced was
//              (perhaps indirectly) _LoadCore
// Returns:  0 = not completed
//           1 = completed
//-----------------------------------------------------------------------------
static EShellResult _SyncLoadCore( EShellFileBlockingMode blkMode )
{
	int stillExecuting;
	
	if(_isLibcdvdMedium)
    {
		if(blkMode)
		{
			sceCdSync(0x00);
			stillExecuting = 0;
		}
		
		else stillExecuting = (sceCdSync(0x01) != 0);
		
		if(stillExecuting) return SHELL_INDETERMINATE;    
		else
		{
			_fileSyncFunc = _SyncReturn1;
			return SHELL_SUCCESS;
		}
    }
	
	else
    {
		do sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting);
		while(stillExecuting && blkMode);
		
		if(stillExecuting) return SHELL_INDETERMINATE;
		else
		{
			sceClose(_file);
			_fileSyncFunc = _SyncReturn1;
			_kernelStatus = SHELLFILE_STAT_PAUSE;
			_kernelError = SHELLFILE_ER_NO;
			return SHELL_SUCCESS;
		}     
    }
}

//-----------------------------------------------------------------------------
// _LoadCore()
// Description: loads a file from the target medium
// Parameters:
//   fileInfo  - location of file
//   blkMode   - 0 = non-blocking
//               1 = blocking
//   eeIopAddr - pointer to memory buffer to store file data.
//   offset    - starting point in file from which to load, in units of
//               2048 bytes
//   sectors   - number of 2048-byte sectors to read.
//               size == -1 means read the whole file
// Returns:  0 = error occurred
//           1 = no error occurred
//-----------------------------------------------------------------------------
static EShellResult _LoadCore( const FileInfo *pFileInfo, EShellFileBlockingMode blkMode, void *eeIopAddr,
					  int offset, int sectors )
{
	if(_isLibcdvdMedium)
    {
		if(sectors==-1) sectors = BYTES_TO_SECTORS(pFileInfo->bytes);
		
		if(!sceCdRead((u_int) (pFileInfo->lsn+offset),(u_int) sectors, eeIopAddr, &_fileMode))
		{
			WARNING(0, "sceCdRead failed in '%s'", pFileInfo->nativeName);
			return SHELL_FAIL;
		}
		
		if(blkMode) sceCdSync(0x00);
		
		else _fileSyncFunc = _SyncLoadCore;
		
		return SHELL_SUCCESS;
    }
	
	else  
    {
		if(blkMode)
		{
			if((_file = sceOpen(pFileInfo->nativeName, SCE_RDONLY)) < 0)
			{
				WARNING(0, "sceOpen failed (returned %d) for '%s'",
						_file, pFileInfo->nativeName);
				_kernelStatus = SHELLFILE_STAT_EMG;
				return SHELL_FAIL;
			}
			
			if(sectors==-1)
				sectors = BYTES_TO_SECTORS(sceLseek(_file, 0, SCE_SEEK_END));
			
			sceLseek(_file, offset*2048, SCE_SEEK_SET);
		}
		
		else
		{
			int stillExecuting;
			
			if(sectors==-1)
			{
				if((_file = sceOpen(pFileInfo->nativeName, SCE_RDONLY)) < 0)
				{
					_kernelStatus = SHELLFILE_STAT_EMG;
					WARNING(0, "sceOpen failed (returned %d) for '%s'",
							_file, pFileInfo->nativeName);
					return SHELL_FAIL;
				}
				
				sectors = BYTES_TO_SECTORS(sceLseek(_file, 0, SCE_SEEK_END));
				sceClose(_file);
			}	

			if((_file = sceOpen(pFileInfo->nativeName,
								SCE_RDONLY|SCE_NOWAIT)) < 0)
			{
				_kernelStatus = SHELLFILE_STAT_EMG;
				WARNING(0, "sceOpen failed (returned %d) for '%s'",
						_file, pFileInfo->nativeName);
				return SHELL_FAIL;
			}

			do sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting);
			while(stillExecuting);

			if(offset) sceLseek(_file, offset*2048, SCE_SEEK_SET);
		}

		// We now have an opened file, _file, seeked to the right place

		if(sceRead(_file, eeIopAddr, sectors*2048) < 0)
		{
			_kernelStatus = SHELLFILE_STAT_EMG;
			WARNING(0, "sceRead failed for '%s'", pFileInfo->nativeName);
			sceClose(_file);
			return SHELL_FAIL;
		}

		if(blkMode)
		{
			sceClose(_file);
			return SHELL_SUCCESS;
		}

		else
		{			
			_fileSyncFunc = _SyncLoadCore;
			_kernelStatus = SHELLFILE_STAT_READ;
			_kernelError = SHELLFILE_ER_NORDY;
			return SHELL_SUCCESS;
		}
    }
}

//-----------------------------------------------------------------------------
// _SyncLoadChain()
// Description: performs shellFileSync when the command to be synced was
//              _LoadChain
// Returns:  -1 = error occurred
//           0  = not completed
//           1  = completed
//-----------------------------------------------------------------------------
static EShellResult _SyncLoadChain( EShellFileBlockingMode blkMode )
{
	if(_isLibcdvdMedium)
    {
		int stillExecuting;
		
		if(blkMode)
		{
			sceCdSync(0x00);
			stillExecuting = 0;
		}
		
		else stillExecuting = (sceCdSync(0x01) != 0);
		
		if(stillExecuting) return SHELL_INDETERMINATE;    
		else
		{
			_fileSyncFunc = _SyncReturn1;
			return SHELL_SUCCESS;
		}
    }  
	
	else
    {
		do
		{
			EShellResult r=_realFileSyncFunc(blkMode);
			if(r!=SHELL_SUCCESS) return r;
			
			_curTagInd++;
			
			if(_namesCpy[_curTagInd][0]==0)
			{
				_kernelError = SHELLFILE_ER_NO;
				_kernelStatus = SHELLFILE_STAT_PAUSE;
				_fileSyncFunc = _SyncReturn1;
				return SHELL_SUCCESS;
			}
			
			if((int) _eeIopAddrsCpy[_curTagInd] & 1)
			{
				if(!shellFileLoadIOPm(_namesCpy[_curTagInd], blkMode,
									 (int) _eeIopAddrsCpy[_curTagInd] & ~1))
				{
					_fileSyncFunc = _SyncReturn1;
					_kernelError = SHELLFILE_ER_READ;
					_kernelStatus = SHELLFILE_STAT_PAUSE;
					return SHELL_FAIL;
				}
			}
			
			else
			{
				if(!shellFileRead(_namesCpy[_curTagInd], blkMode,
								 (char*) _eeIopAddrsCpy[_curTagInd], 0, -1))
				{
					_fileSyncFunc = _SyncReturn1;
					_kernelError = SHELLFILE_ER_READ;
					_kernelStatus = SHELLFILE_STAT_PAUSE;
					return SHELL_FAIL;
				}
			}
			
			// Reinstate sync function after the earlier shellFileLoadIOPm or
			// shellFileLoad would have altered it.
			
			_realFileSyncFunc = _fileSyncFunc;
			_fileSyncFunc = _SyncLoadChain;
		}
		while(blkMode);
		
		return SHELL_INDETERMINATE;
    }
}

//-----------------------------------------------------------------------------
// _SyncLoadIOPm()
// Description: performs shellFileSync when the command to be synced was 
//              (perhaps indirectly) _LoadIOPm
// Returns:  0 = error occurred
//           1 = no error occurred
//-----------------------------------------------------------------------------
static EShellResult _SyncLoadIOPm( EShellFileBlockingMode blkMode )
{
	int stillExecuting;
	
	if(_isLibcdvdMedium)
    {      
		if(blkMode)
		{
			sceCdSync(0x00);
			stillExecuting = 0;
		}
		
		else stillExecuting = (sceCdSync(0x01) != 0);
		
		if(stillExecuting) return SHELL_INDETERMINATE;    
		else
		{
			_fileSyncFunc = _SyncReturn1;
			return SHELL_FAIL;
		}
    }
	
	else
    {
		sceSifDmaData toIopDMA;
		int did;
		
		do
		{
			do
			{
				sceIoctl(_file, SCE_FS_EXECUTING, &stillExecuting);
			} while(stillExecuting && blkMode);
			
			if(stillExecuting) return SHELL_INDETERMINATE;
			
			toIopDMA.data = (u_int) _iopCopyBuf;
			toIopDMA.addr = (u_int) _iopAddr + _amountRead;
			toIopDMA.size = IOP_COPY_CHUNK_SIZE;
			toIopDMA.mode = 0;
			
			FlushCache(0);
			
			while((did = (int) sceSifSetDma(&toIopDMA, 1)) == 0) {}
			while(sceSifDmaStat((u_int)did) >= 0) {}
			
			_amountRead += IOP_COPY_CHUNK_SIZE;
			
			if(_amountRead >= _fileSizeBytes)
			{
				sceClose(_file);
				_fileSyncFunc = _SyncReturn1;
				_kernelStatus = SHELLFILE_STAT_PAUSE;
				return SHELL_SUCCESS;
			}
			
			else
			{
				// Queue up the next chunk
				
				if(sceRead(_file, _iopCopyBuf, IOP_COPY_CHUNK_SIZE) < 0)
				{
					WARNING(0, "sceRead error occurred in '%s'", __FUNCTION__);
					sceClose(_file);
					return SHELL_FAIL;
				}
			}
		} while(blkMode);
		
		return SHELL_INDETERMINATE;
    }
}


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// shellFileInit()
// Description: initialises shellFile module
// Parameters:
//   medium        - SHELLFILE_MEDIUM_HOST0    = host0
//                   SHELLFILE_MEDIUM_CD       = CD
//                   SHELLFILE_MEDIUM_DVD      = DVD
//                   SHELLFILE_MEDIUM_SCEEDEMO = autoselect CD or DVD, only for
//                                               SCEE-region demo segments
//                   SHELLFILE_MEDIUM_ATMON    = atmon
//   rebootIop     - 0 = don't reboot IOP
//                 - 1 = reboot IOP
//   iopModPath    - string of directories to search for IOP modules,
//                   separated by semicolons
//   medParam      - parameter specific to choice of medium
//
// Notes: If the IOP is to be rebooted, the expected location of the image file
//        is within the first directory specified in iopModPath
//
//        When medium is SHELLFILE_MEDIUM_ATMON, the medParam parameter should
//        be a string pointer which specifies the location of the remote file
//        server. The string should take the format "atfile:host[!port],",
//        where host must be an IP address and port is an optional port number.
//
//        eg. "atfile:192.168.0.19," 
//            "atfile:192.168.0.19!12345," 
//
//        This format was chosen to allow the argv[0] parameter, as passed to
//        main() when compiled with the atmon crt0.s, to be passed in directly.
//
//        For all other medium types, medParam is undefined.
//-----------------------------------------------------------------------------
void shellFileInit( EShellFileMedium medium, int rebootIop,
					const char *iopModPath, void *medParam )
{  
	int n;
	
	_medium = medium;

	strcpy(_kernelName, "");
	_kernelNameLen = strlen(_kernelName);

	switch(medium)
	{
	case SHELLFILE_MEDIUM_HOST0:
		_isLibcdvdMedium = 0;
		break;

	case SHELLFILE_MEDIUM_ATMON:
		_isLibcdvdMedium = 0;
		break;

	default:
		_isLibcdvdMedium = 1;
	}

	ASSERTS(iopModPath, "Null pointer for IOP search path");
	_iopModPath = iopModPath;

	_kernelStatus = SHELLFILE_STAT_PAUSE;
	_kernelError = SHELLFILE_ER_NO;
	
	for(n=0; n<SHARED_MOD_TABLE_SIZE; n++)
	{
		_sharedModTable[n].id = -1;
		_sharedModTable[n].users = 0;
	}	
	_shellFileInitCalled = 1;
}

//-----------------------------------------------------------------------------
// shellFileNativeName()
// Description: converts from shellFile name format to kernel I/O format.
// Parameters:
//   src   - source filename
//   dest  - buffer to hold destination filename
//-----------------------------------------------------------------------------
void shellFileNativeName( const char *src, char *dest )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(src, ("Null pointer argument %s", "src"));
	ASSERTF(dest, ("Null pointer argument %s", "dest"));

	_NativeName(src, dest);
}

//-----------------------------------------------------------------------------
// shellFileLoadStartModule()
// Description: loads and starts an IOP module unless already started
// Parameters:
//   name    - name of IOP module, stripped of all path and suffix,
//             eg. "sio2man"
//   args    - the args parameter (refer to documentation of sceSifLoadModule)
//   argp    - the argp parameter (refer to documentation of sceSifLoadModule)
//   result  - a pointer to a variable for storing the value returned by the
//             module's initialisation routine. Can be null.
// Returns: module ID
// Notes: If the module was already loaded, 'result' will return a copy of the
//        result that was obtained when the module was first started.
//-----------------------------------------------------------------------------
int shellFileLoadStartModule(const char *name, int args, const char *argp,
							int *pResult )
{
	char modifiedModName[128];
	const char *s;
	char *d;
	int ind = 0;
	int id;

	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(name, ("Null pointer argument %s", "name"));

	_CheckIopName(name);
	
	for(s=name, d=modifiedModName; *s; s++, d++) *d = (char) tolower(*s);

	*d = 0;
	
	id = sceSifSearchModuleByName(modifiedModName);
		
	if(id<0)    // Module not already loaded
	{
		char fullModPath[128];
	
		for(ind=0; ind<SHARED_MOD_TABLE_SIZE; ind++)
			if(_sharedModTable[ind].id == -1) break;
		
		ASSERTS(ind<SHARED_MOD_TABLE_SIZE, "No more entries available in "
			   "shared IOP module table");

		strcpy(modifiedModName, name);
		strcat(modifiedModName, ".irx");

		if(_GetIopModulePath(modifiedModName, fullModPath))
		{	
			if((id = sceSifLoadStartModule(fullModPath, args, argp,
										   &_sharedModTable[ind].result)) < 0)
			{
				WARNING(0, "Couldn't load IOP module %s", fullModPath);
				return id;
			}
			
			PRINT("Loaded IOP module %s", fullModPath);
			
			_sharedModTable[ind].id = id;
			_sharedModTable[ind].users = 1;
		}
			
		else WARNING(0, "Couldn't find IOP module %s.irx", name);
	}

	else    // Module already loaded
	{
		for(ind=0; ind<SHARED_MOD_TABLE_SIZE; ind++)
		{
			if(_sharedModTable[ind].id == id)
			{
				_sharedModTable[ind].users++;
				break;
			}
		}
		
		// Module was loaded outside of shellFile. This shouldn't ordinarily
		// happen unless running under atmon which leaves a number of
		// modules installed (sio2man, padman, mcman, etc.)
		
		WARNING(ind<SHARED_MOD_TABLE_SIZE,
				"Module %s already loaded outside of shellFile", name);

		// Create a new entry in the shared mod table for it.

		for(ind=0; ind<SHARED_MOD_TABLE_SIZE; ind++)
			if(_sharedModTable[ind].id == -1) break;
		
		ASSERTS(ind<SHARED_MOD_TABLE_SIZE, "No more entries available in "
			   "shared IOP module table");

		_sharedModTable[ind].id = id;
		_sharedModTable[ind].users = 1;
	}
	
	if(pResult) *pResult = _sharedModTable[ind].result;
	return id;
}

//-----------------------------------------------------------------------------
// shellFileStopUnloadModule()
// Description: stops and unloads an IOP module unless used elsewhere
// Parameters:
//   id      - module ID returned by shellFileLoadStartModule.
//   result  - pointer to variable for storing value returned by module
//             stopping routine. Can be null.
// Returns:
//   < 0   = failure
//   >= 0  = success
// Notes: passing args and argp and obtaining a result from closing the module
//        is not currently supported as these lose usefulness in a shared
//        module system.
//-----------------------------------------------------------------------------
EShellResult shellFileStopUnloadModule( int id )
{
	int ind;
	int ret;
	int args = 0;
	const char *argp = "";
	int res;

	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");

	for(ind=0; ind<SHARED_MOD_TABLE_SIZE; ind++)
	{
		if((_sharedModTable[ind].id == id) && _sharedModTable[ind].users > 0)
		{
			if(--_sharedModTable[ind].users == 0)
			{
				ret = sceSifStopModule(id, args, argp, &res);
				
				if(ret<0) return SHELL_FAIL;

				_sharedModTable[ind].id = -1;

				if((ret=sceSifUnloadModule(id)) < 0)
				{
					WARNING(0, "Couldn't unload stopped module %d (%d)",
							id, ret);
					return SHELL_FAIL;
				}
			}
			
			break;
		}
	}
	
	ASSERTS(ind<SHARED_MOD_TABLE_SIZE, "Module not known to shared IOP "
		   "module table");

	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileDiskReady()
// Description: checks drive status
// Parameters:
//   blkMode - 0 = non-blocking
//             1 = blocking
// Returns:  0       = drive not ready
//           nonzero = drive not ready
//-----------------------------------------------------------------------------
int shellFileDiskReady( EShellFileBlockingMode blkMode )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
		return sceCdDiskReady(blkMode ? 0 : 1) == SCECdComplete;
	else return 1;
}

//-----------------------------------------------------------------------------
// shellFileGetError()
// Description: determines cause of last error
// Returns:  SHELLFILE_ER_FAIL    = shellFileGetError function failed 
//           SHELLFILE_ER_NO      = No error
//           SHELLFILE_ER_EOM     = Outermost track reached during playback
//           SHELLFILE_ER_TRMOPN  = Cover opened during playback
//           SHELLFILE_ER_READ    = Problem occurred during read
//           SHELLFILE_ER_CUD     = Not appropriate for disc in drive
//           SHELLFILE_ER_NORDY   = Processing command
//           SHELLFILE_ER_ABRT    = Abort command received
//-----------------------------------------------------------------------------
EShellFileError shellFileGetError( void )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	return _isLibcdvdMedium ? (EShellFileError) sceCdGetError() : _kernelError;
}

//-----------------------------------------------------------------------------
// shellFileGetSize()
// Description: determines size of file
// Parameters:
//   name - filename in shellFile format
// Returns: size of file in bytes
//-----------------------------------------------------------------------------
unsigned int shellFileGetSize( const char *name )
{
	FileInfo fileInfo;
	
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(!_GetInfo(name, &fileInfo)) return 0;
	
	if(_isLibcdvdMedium) return (unsigned int) fileInfo.bytes;
	else
    {
		unsigned int size;
		
		if((_file = sceOpen(fileInfo.nativeName, SCE_RDONLY)) < 0) return 0;
		
		size = (unsigned int) sceLseek(_file, 0, SCE_SEEK_END);      
		sceClose(_file);
		
		return size;
    }
}

//-----------------------------------------------------------------------------
// shellFileRead()
// Description: read from a file
// Parameters:
//   name           - filename in shellFile format
//   blkMode        - 0 = non-blocking
//                    1 = blocking
//   eeAddr         - target EE address
//   sectorOffset   - offset within file to start reading, in 2048-byte units
//   sectors        - amount of data to read, in 2048-byte units
// Returns:  0       = error occurred
//           nonzero = no error occurred
// Notes: SPR cannot be the destination buffer
//-----------------------------------------------------------------------------
EShellResult shellFileRead( const char *name, EShellFileBlockingMode blkMode, void *eeAddr,
				  int sectorOffset, int sectors )
{
	FileInfo fileInfo;
	
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(name, ("Null pointer argument %s", "name"));
	ASSERTF(eeAddr, ("Null pointer argument %s", "eeAddr"));
	
	WARNING(!((int) eeAddr%64),
			"Load address is not 64-byte aligned; performance may suffer");
	
	if(!_GetInfo(name, &fileInfo)) return SHELL_FAIL;
	
	return _LoadCore(&fileInfo, blkMode, eeAddr, sectorOffset, sectors)?SHELL_SUCCESS:SHELL_FAIL;
}

//-----------------------------------------------------------------------------
// shellFileLoad()
// Description: reads a file into memory
// Parameters:
//   name      - name of the file
//   alignment - how to align the memory allocated for the file data
//   length    - pointer to a variable which will receive the file length
// Returns: pointer to the memory allocated for the file
//-----------------------------------------------------------------------------
void *shellFileLoad( const char *name, unsigned int alignment, unsigned int *length)
{
unsigned int len;
unsigned int blocks;
void *data;

	data=0;
	len=(unsigned int)shellFileGetSize(name);
	if(length) *length=len;
	if(len)
	{
		blocks=(len+2047)>>11;
		data=shellMemoryAllocate(alignment,blocks<<11);
		if(data)
		{
			if(shellFileRead(name, 
							SHELLFILE_BLOCKING,
							data,
							0,
							(int) blocks)==SHELL_FAIL)
				data=0;
		}
	}
	return data;
}

//-----------------------------------------------------------------------------
// shellFileGetLoadPos()
// Description: returns amount of file read
// Returns: file read position in 2048-byte units, or zero if file loading
//          is complete
//-----------------------------------------------------------------------------
int shellFileGetLoadPos( void )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	// Non-committal, ambiguous, meaningless answer. (Could cause problems)
	if(_medium == SHELLFILE_MEDIUM_HOST0) return 0;
	
	else return (int) sceCdGetReadPos() * 16;
}

//-----------------------------------------------------------------------------
// shellFileLoadChain()
// Description: loads a sequence of files
// Parameters:
//   names      - array of filenames (shellFile format), terminated by a null
//                pointer
//   blkMode    - 0 = non-blocking
//                1 = blocking
//   eeIopAddrs - array of addresses to which files should be loaded. For
//                each address, LSB = 1 means IOP memory is the destination.
//                Array is terminated implicitly be the length of 'names'.
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileLoadChain( const char **names, EShellFileBlockingMode blkMode, void **eeIopAddrs )
{
	int n;
	
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(names, ("Null pointer argument %s", "names"));
	ASSERTF(eeIopAddrs, ("Null pointer argument %s", "eeIopAddrs"));
	
	for(n = 0; n<64; n++)
    {
		if(_isLibcdvdMedium)
		{
			FileInfo fileInfo;
			
			if(names[n]==0)
			{
				_loadChainTag[(n*3)+0] = 0xffffffff;
				_loadChainTag[(n*3)+1] = 0xffffffff;
				_loadChainTag[(n*3)+2] = 0xffffffff;
				break;
			}
			
			if(!_GetInfo(names[n], &fileInfo)) return SHELL_FAIL;
			
			_loadChainTag[(n*3)+0] = (u_int) fileInfo.lsn;
			_loadChainTag[(n*3)+1] = (u_int) BYTES_TO_SECTORS(fileInfo.bytes);
			_loadChainTag[(n*3)+2] = (u_int) eeIopAddrs[n];
		}
		
		else
		{
			if(names[n]==0)
			{
				_namesCpy[n][0] = 0;
				break;
			}
			
			strcpy(_namesCpy[n], names[n]);
			_eeIopAddrsCpy[n] = eeIopAddrs[n];
		}
    }
	
	if(_isLibcdvdMedium)
    {
		if(!sceCdReadChain(_loadChainTag, &_fileMode)) return SHELL_FAIL;
		
		if(blkMode) sceCdSync(0x00);
		else _fileSyncFunc = _SyncLoadChain;
		
		return SHELL_SUCCESS;
    }
	
	else
    {
		_curTagInd = -1;
		_realFileSyncFunc = _SyncReturn1;
		_fileSyncFunc = _SyncLoadChain;
		
		return _SyncLoadChain(blkMode) == -1 ? SHELL_FAIL : SHELL_SUCCESS;
    }
}

//-----------------------------------------------------------------------------
// shellFileLoadIOPm()
// Description: loads a file to IOP memory
// Parameters:
//   name     - filename in shellFile format
//   blkMode  - 0 = non-blocking
//              1 = blocking
//   iopAddr  - Target IOP address
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileLoadIOPm( const char *name, EShellFileBlockingMode blkMode, int iopAddr )
{
	FileInfo fileInfo;
	
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(name, ("Null pointer argument %s", "name"));
	
	if(_isLibcdvdMedium)
    {
		if(! _GetInfo(name, &fileInfo)) return SHELL_FAIL;
		
		if(!sceCdReadIOPm((u_int) fileInfo.lsn, (u_int) BYTES_TO_SECTORS(fileInfo.bytes),
						  (void *) iopAddr, &_fileMode)) return SHELL_FAIL;
				
		if(blkMode) sceCdSync(0x00);
		
		else _fileSyncFunc = _SyncLoadIOPm;
		
		return SHELL_SUCCESS;
    }
	
	else
    {
		if(!_GetInfo(name, &fileInfo)) return SHELL_FAIL;
		
		if((_file = sceOpen(fileInfo.nativeName, SCE_RDONLY)) < 0)
		{
			WARNING(0, "sceOpen failed (returned %d) for '%s'",
					_file, fileInfo.nativeName);
			return SHELL_FAIL;
		}
		
		_fileSizeBytes = sceLseek(_file, 0, SCE_SEEK_END);
		
		sceLseek(_file, 0, SCE_SEEK_SET);
		
		if(sceRead(_file, _iopCopyBuf, IOP_COPY_CHUNK_SIZE) < 0)
		{
			WARNING(0, "sceRead error occurred in '%s'",
					fileInfo.nativeName);
			sceClose(_file);
			return SHELL_FAIL;
		}
		
		_fileSyncFunc = _SyncLoadIOPm;
		
		_iopAddr = iopAddr;
		_amountRead = 0;
		
		if(blkMode) return shellFileSync(SHELLFILE_BLOCKING) == -1 ? SHELL_FAIL : SHELL_SUCCESS;
		
		return SHELL_SUCCESS;
    }
}

//-----------------------------------------------------------------------------
// shellFilePause()
// Description: pause device head
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFilePause( void )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium) return sceCdPause() ? SHELL_SUCCESS : SHELL_FAIL;
	else return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileSeek()
// Description: move device head to file position
// Parameters:
//   name         - filename in shellFile format
//   sectorOffset - offset within file to seek to, in 2048-byte units
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileSeek( const char *name, int sectorOffset )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(name, ("Null pointer argument %s", "name"));
	
	if(_isLibcdvdMedium)
	{
		FileInfo fileInfo;
		
		if(!_GetInfo(name, &fileInfo)) return SHELL_FAIL;
		
		return sceCdSeek((u_int)(fileInfo.lsn + sectorOffset)) ? SHELL_SUCCESS : SHELL_FAIL;
	}

	else return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStandby()
// Description: starts rotation of media
// Parameters:
//   blkMode   - 0 = non-blocking
//               1 = blocking
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStandby( EShellFileBlockingMode blkMode )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
    {
		if(!sceCdStandby()) return SHELL_FAIL;		
		if(blkMode) sceCdSync(0x00);
    }
    
    return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStatus()
// Description: get drive status
// Returns:  SHELLFILE_STAT_SHELLOPEN = Tray is open
//           SHELLFILE_STAT_STOP      = Stopped
//           SHELLFILE_STAT_SPIN      = Spindle is spinning
//           SHELLFILE_STAT_READ      = Reading
//           SHELLFILE_STAT_PAUSE     = Paused
//           SHELLFILE_STAT_SEEK      = Seeking
//           SHELLFILE_STAT_EMG       = Abnormal termination
//-----------------------------------------------------------------------------
EShellFileStat shellFileStatus( void )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium) return (EShellFileStat) sceCdStatus();
	else return _kernelStatus;
}

//-----------------------------------------------------------------------------
// shellFileStop()
// Description: stops rotation of media
// Parameters:
//   blkMode   - 0 = non-blocking
//               1 = blocking
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStop( EShellFileBlockingMode blkMode )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
	{
		sceCdStop();
		if(blkMode) sceCdSync(0x00);
    }

    return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStInit()
// Description: initialise file streaming functions
// Parameters:
//   sectors     - capaciy of IOP stream buffer, in 2048-byte units
//   partitions  - number of stream buffer partitions
//   iopAddr     - IOP pointer to stream buffer
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStInit( int sectors, int partitions, int iopAddr )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(iopAddr, ("Null pointer argument %s", "iopAddr"));
	
	if(SCE_LIBRARY_VERSION >= 0x2600)
	{
		// Streaming functions devolved to a separate module from 2.6.0

		if(!shellFileLoadStartModule("cdvdstm", 0, "", NULL))
			ASSERTS(0, "Couldn't load IOP module");
	}

	if(_isLibcdvdMedium)
		sceCdStInit((u_int) sectors,(u_int) partitions, (u_int) iopAddr);

	else _stBufSize = sectors;
	
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStStart()
// Description: begin streaming from a file
// Parameters:
//   name          - filename in shellFile format
//   sectorOffset  - offset from start of file to begin streaming, in
//                   2048-byte units
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStStart( char *name, int sectorOffset )
{
	FileInfo fileInfo;
	
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(name, ("Null pointer argument %s", "name"));
	
	if(!_GetInfo(name, &fileInfo)) return SHELL_FAIL;
	
	if(_isLibcdvdMedium)
    {
		_stLsn = fileInfo.lsn;
		sceCdStStart((u_int)  (_stLsn + sectorOffset), &_streamMode);
		return SHELL_SUCCESS;
    }
	
	else
    {
		if((_file = sceOpen(fileInfo.nativeName, SCE_RDONLY)) < 0) return SHELL_FAIL;
		
		_stLength = (sceLseek(_file, 0, SCE_SEEK_END) + 2047) / 2048;
		sceLseek(_file, sectorOffset*2048, SCE_SEEK_SET);
		_stPos = sectorOffset;
    }
	
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStRead()
// Description: read stream data
// Parameters:
//   eeAddr   - target EE address
//   sectors  - amount of data to read in 2048-byte units
//   blkMode  - 0 = non-blocking
//              1 = blocking
// Returns: number of sectors read
//-----------------------------------------------------------------------------
int shellFileStRead( void *eeAddr, int sectors, EShellFileBlockingMode blkMode )
{
	int n;

	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(eeAddr, ("Null pointer argument %s", "eeAddr"));
	
	if(_isLibcdvdMedium)
    {
		u_int err;
		
		return sceCdStRead((u_int) sectors, (u_int *) eeAddr,
						   (u_int) (blkMode ? STMBLK : STMNBLK), &err);
    }
	
	else
	{
		_stPos += n = (sceRead(_file, eeAddr, sectors*2048)+2047)/2048;
		
		if(_stPos >= _stLength) return sectors;
		else return n;
	}
}

//-----------------------------------------------------------------------------
// shellFileStStop()
// Description: stop streaming
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStStop( void )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
		return sceCdStStop() ? SHELL_SUCCESS : SHELL_FAIL;
	
	else return (sceClose(_file) == -1) ? SHELL_FAIL : SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStSeek()
// Description: change stream position
// Parameters:
//   sectors - offset from start of file to resume streaming, in 2048-byte
//             units
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileStSeek( int sectors )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium)
		return sceCdStSeek((u_int) (_stLsn + sectors)) ? SHELL_SUCCESS : SHELL_FAIL;
	
	else
	{
		if(sceLseek(_file, sectors*2048, SCE_SEEK_SET) == -1) return SHELL_FAIL;
		else _stPos = sectors;
	}
	
	return SHELL_SUCCESS;
}

//-----------------------------------------------------------------------------
// shellFileStStat()
// Description: get stream data read status
// Returns: amount of data accumulated in stream buffer, in 2048-byte units
//-----------------------------------------------------------------------------
int shellFileStStat( void )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	if(_isLibcdvdMedium) return sceCdStStat();
	
	// We claim the entire pretend buffer has been filled, just in case
	// the caller was waiting for it to be completely full before performing
	// some action.
	
	else return _stBufSize;
}

//-----------------------------------------------------------------------------
// shellFileSync()
// Description: wait for or detect command completion
// Parameters:
//   blkMode - 0 = non-blocking
//             1 = blocking
// Returns:  -1        = error occurred
//           0         = not completed
//           otherwise = completed
//-----------------------------------------------------------------------------
EShellResult shellFileSync( EShellFileBlockingMode blkMode )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	
	return _fileSyncFunc(blkMode);
}

//-----------------------------------------------------------------------------
// shellFileTrayReq()
// Description: open, close or detect status of tray
// Parameters:
//   command   - 0 = open tray
//               1 = close tray
//               2 = get tray state change
//   pTrayCnt  - pointer to hold tray state change result
// Returns:  0       = error occurred
//           nonzero = no error occurred
//-----------------------------------------------------------------------------
EShellResult shellFileTrayReq( int command, int *pTrayCnt )
{
	ASSERTS(_shellFileInitCalled, "shellFileInit has not been called");
	ASSERTF(pTrayCnt, ("Null pointer argument %s", "pTrayCnt"));
	
	if(_isLibcdvdMedium)
		return sceCdTrayReq(command, (unsigned int *) pTrayCnt) ? SHELL_SUCCESS : SHELL_FAIL;
	
	else
    {     
		*pTrayCnt = 0;
		return SHELL_SUCCESS;
    }
}
