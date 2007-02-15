/***************************************************************************/
/* Low level (platform independent) file I/O
/***************************************************************************/
#ifndef __MEDIALOW_H
#define __MEDIALOW_H

#include "Layers/LayersStruct.h"

/* each platform has a different root directory */
#define COMMON_DATA_ROOT	"DATA\\"
#define GCN_DATA_ROOT		"DATA_GCN\\"
#define PS2_DATA_ROOT		"DATA_PS2\\"
#define XBOX_DATA_ROOT		"DATA_XBOX\\"

#if defined (DIRECTX)			// temp PAB
#define PC_DATA_ROOT			"DATA_PC\\"
#else
#define PC_DATA_ROOT			"DATA_XBOX\\"
#endif

typedef void *			t_FileHandle;

#if defined(WIN32)

typedef DWORD			t_FileError;

#elif defined(PS2)

typedef int				t_FileError;

#elif defined(GCN)

typedef s32				t_FileError;

#endif

bool           _medExists(const char *fileName);
bool				_medOpen(const char *fileName, t_FileHandle *pFileHandle, u32 openFlags);
u32				_medGetOpenFlags( t_FileHandle fileHandle);
void				_medClose(t_FileHandle FileHandle);
bool 				_medSeek(t_FileHandle FileHandle, s32 s32Pos);
s32				_medLength(t_FileHandle FileHandle);
t_Error			_medTranslateError( t_FileError platformError, t_Error defaultError);
bool				_medRead(void *pBuffer, s32 s32Len, t_FileHandle FileHandle);
bool				_medWrite(const void *pBuffer, s32 s32Len, t_FileHandle FileHandle);
bool				_medAwaitIO( t_FileHandle fileHandle, s32 timeOut, void **ppBuffer, s32 *pIOSize, t_Error *pError);
void				_medSetRootDir(const char *newRootDir);
void				_medMakeFileName(const char *baseFileName, char *realFileName, bool bPlatformDir);
char				*_medFileName83(const char *fileName);
/* Set and Get the state of name-mangling of file names */
void				_medEnableNameMangling(bool bState);
bool				_medGetNameManglingState(void);


#endif
