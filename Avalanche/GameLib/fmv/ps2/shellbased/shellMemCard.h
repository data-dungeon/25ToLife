//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellMemCard header file
//
// $Workfile: shellMemCard.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SHELLMEMCARD_H_
#define _SHELLMEMCARD_H_

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

//Overall memory card format status (used by shellMemCardGetStatus):
typedef struct _ShellMemCardStatus
{
  int					type;				//libmc requires type to be int
  int					numFreeClusters;
  int					formatStatus;		//libmc requires formatStatus to be int
} ShellMemCardStatus;


typedef struct _ShellMemCardFunctionResult
{
  EShellResult			success;			//SHELL_SUCCESS, SHELL_FAIL or SHELL_INDETERMINATE
  int					subState;			//Diagnostic info: subState within EShellMemCardModuleState
  int					libMCErrorCode;		//Error code as specified by libmc
} ShellMemCardFunctionResult;

//Memory card module mode:
typedef enum
{
	SHELLMEMCARD_STATE_NULL 				= -1,
	SHELLMEMCARD_STATE_IDLE 				= 0,
	SHELLMEMCARD_STATE_ENTERING_STATE,
	
	//General fns:
 	SHELLMEMCARD_STATE_FORMAT,
 	
 	//Directory fns:
	SHELLMEMCARD_STATE_FIND_DIR,
	SHELLMEMCARD_STATE_CREATE_DIR,
 	SHELLMEMCARD_STATE_GET_NUM_FILES_IN_DIR,
 	SHELLMEMCARD_STATE_GET_DIR_LIST,
 	SHELLMEMCARD_STATE_GET_DIR_SIZE,
 	
 	//File fns:
 	SHELLMEMCARD_STATE_FIND_FILE,
	SHELLMEMCARD_STATE_LOAD_FILE,
	SHELLMEMCARD_STATE_SAVE_FILE,

	SHELLMEMCARD_STATE_NUM_STATES
} EShellMemCardState;

//-----------------------------------------------------------------------------
// M A C R O S 
//-----------------------------------------------------------------------------
#define SHELLMEMCARD_CLUSTER_SIZE									(1024)
#define SHELLMEMCARD_GET_CLUSTER_SIZE(size)							(((size) + SHELLMEMCARD_CLUSTER_SIZE - 1) / SHELLMEMCARD_CLUSTER_SIZE)
#define SHELLMEMCARD_GET_CLUSTERS_FOR_FILE_ENTRIES(numFiles)		((numFiles + 1) / 2)
#define SHELLMEMCARD_MAX_FILES_PER_DIRECTORY						(18)

#define SHELLMEMCARD_MAX_LEN_FILENAME								(32)
#define SHELLMEMCARD_CLUSTERS_FOR_DIRECTORY							(2)
#define SHELLMEMCARD_FILE_NAME_ICON_SYS								"icon.sys"
#define SHELLMEMCARD_ICON_LIST 										(0)
#define SHELLMEMCARD_ICON_COPY 										(1)
#define SHELLMEMCARD_ICON_DELETE									(2)
#define SHELLMEMCARD_ICON_NUM_ICONS 								(3)
//Shouldn't use SHELL_INDETERMINATE since this would involve mixing enums & ints:
#define SHELLMEMCARD_INDETERMINATE 									(-1)


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

//General fns:
EShellResult			shellMemCardInit(const char* keyCode, const char* productModelNo);
EShellResult			shellMemCardFormat(int portNo, int slotNo);
EShellMemCardState		shellMemCardUpdate(int portNo, int slotNo, ShellMemCardStatus* pCardStatus);

//Directory fns:
EShellResult			shellMemCardCreateDir(int portNo, int slotNo, const char* dirNameExtension);
EShellResult 			shellMemCardFindDir(int portNo, int slotNo, const char* dirNameExtension,
										 	int* pDirFound, sceMcTblGetDir* pDirInfo);

EShellResult 			shellMemCardGetDirSize(int portNo, int slotNo, const char* dirNameExtension,
											   int* pDirSize);

EShellResult			shellMemCardGetNumFilesInDir(int portNo, int slotNo, int* pNumFiles, 
												const char* dirNameExtension);
EShellResult			shellMemCardGetDirList(	int portNo, int slotNo, const char* dirPrefix, 
											int numDirsToSkip, int numDirsToGet, 
											int* pNumDirsObtained,
											sceMcTblGetDir* pDirInfo);


//File fns:
EShellResult 		shellMemCardFindFile(int portNo, int slotNo, const char* fileName, 
										const char* dirNameExtension, int* pFileSize,
										sceMcTblGetDir* pFileInfo);
EShellResult 		shellMemCardLoadFile(int portNo, int slotNo, const char* dirNameExtension,
										const char* fileName, char* pData, u_int dataSize);
EShellResult 		shellMemCardSaveFile(int portNo, int slotNo, const char* dirNameExtension, 
										const char* fileName, char* pData, u_int dataSize);

//Access fns:
EShellMemCardState	shellMemCardGetState(); 
void 				shellMemCardGetFunctionResult(EShellMemCardState state,
												 ShellMemCardFunctionResult* pFunctionResult);

//Helper fns:
void 				shellMemCardCreateUserDataFileName(	char* userDataFileName,
								   						const char* dirNameExtension);
void 				shellMemCardResetIconSys(sceMcIconSys* pIconSys);


#ifdef __cplusplus
}
#endif

#endif
