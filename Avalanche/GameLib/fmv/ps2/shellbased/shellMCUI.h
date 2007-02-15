//-----------------------------------------------------------------------------
//               shellMCUI - Memory Card User Interface Driver
//
// Memory Card User Interface Driver for TRC compliant memory card user
// interface implementation
//
// $Workfile: shellMCUI.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SHELLMEMCARDUI_H_
#define _SHELLMEMCARDUI_H_

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark INCLUDES
#endif

#if defined (__LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
	//"bool" defined as a keyword if using C++
#else
	#include <stdbool.h>
#endif

#include "shellCore.h"


//-----------------------------------------------------------------------------
// M A C R O S 
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark MACROS
#endif

//No brackets so that it can be used for pre-processor conditionals, 
//eg #if SHELLMCUI_VERSION ...
#define SHELLMCUI_VERSION	 				1001
#define SHELLMCUI_MAX_NUM_SAVES_PER_CARD	(14)	

//18 (max files per dir) - (4 icon files) - (1 user data file):
#define SHELLMCUI_MAX_NUM_DATA_FILES		(13)	


//-----------------------------------------------------------------------------
// T Y P E D E F  E N U M S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark TYPEDEF ENUMS
#endif

//Languages:
typedef enum
{
	SHELLMCUI_LANGUAGE_NULL		= -1,
	SHELLMCUI_LANGUAGE_ENGLISH	= 0,
	SHELLMCUI_LANGUAGE_FRENCH,
	
	SHELLMCUI_LANGUAGE_NUM	
} EShellMCUILanguage;


//Main UI screens:
typedef enum
{
	SHELLMCUI_SCREEN_NULL		= -1,
	SHELLMCUI_SCREEN_MAIN		= 0,
	SHELLMCUI_SCREEN_LIST_SAVES,
	SHELLMCUI_SCREEN_LOAD,
	SHELLMCUI_SCREEN_SAVE,
	
	SHELLMCUI_SCREEN_NUM	
} EShellMCUIScreen;


//Status messages with associated TRC text:
//Must be consistent with SHELLMCUI_ENGLISH_STATUS_TEXT[] in
//shellMCUIText.h:
//Maybe rename SHELLMCUI_STATUS_FLAG_XXX?
typedef enum
{
	SHELLMCUI_STATUS_NULL					= -1,

	//Card status:
	SHELLMCUI_STATUS_DETECTING				= 0,
	SHELLMCUI_STATUS_PS2_CARD_INSERTED,
	SHELLMCUI_STATUS_PS2_CARD_NOT_INSERTED,
	SHELLMCUI_STATUS_UNFORMATTED_CARD,
	
	//Save data status:
	SHELLMCUI_STATUS_INSUFFICIENT_SPACE,
	SHELLMCUI_STATUS_NO_SAVES,
	
	//Request user status:
	SHELLMCUI_STATUS_REQUEST_USER_FORMAT,
	SHELLMCUI_STATUS_REQUEST_USER_OVERWRITE,

	//Operation status:
	SHELLMCUI_STATUS_LOADING,
	SHELLMCUI_STATUS_SAVING,
	SHELLMCUI_STATUS_FORMATTING,
	
	SHELLMCUI_STATUS_LOAD_SUCCESSFUL,
	SHELLMCUI_STATUS_SAVE_SUCCESSFUL,
	SHELLMCUI_STATUS_FORMAT_SUCCESSFUL,
	SHELLMCUI_STATUS_LIST_SAVES_SUCCESSFUL,	//Don't want any text with this

	SHELLMCUI_STATUS_LOAD_FAILED,
	SHELLMCUI_STATUS_SAVE_FAILED,
	SHELLMCUI_STATUS_FORMAT_FAILED,

	SHELLMCUI_STATUS_NUM
} EShellMCUIStatus;


//Types of status message:
typedef enum
{
	SHELLMCUI_STATUS_TYPE_NULL		= -1,
	SHELLMCUI_STATUS_TYPE_CARD		= 0,
	SHELLMCUI_STATUS_TYPE_OPERATION,
	SHELLMCUI_STATUS_TYPE_REQUEST_USER,
	SHELLMCUI_STATUS_TYPE_USER_INPUT_INFO,	//"X to continue", "Triangle to cancel", etc
	
	SHELLMCUI_STATUS_TYPE_NUM
} EShellMCUIStatusType;


//User input:
//Main menu option:
typedef enum
{
	SHELLMCUI_MAIN_MENU_OPTION_NULL	= -1,
	SHELLMCUI_MAIN_MENU_OPTION_LOAD	= 0,
	SHELLMCUI_MAIN_MENU_OPTION_SAVE,

	SHELLMCUI_MAIN_MENU_OPTION_NUM
} EShellMCUIMainMenuOption;

//Yes / no options:
typedef enum
{
	SHELLMCUI_YESNO_NULL			= -1,
	SHELLMCUI_YESNO_NO				= 0,
	SHELLMCUI_YESNO_YES,
	
	SHELLMCUI_YESNO_NUM	
} EShellMCUIYesNo;


//Requests by the module for input from the user:
typedef enum
{
	SHELLMCUI_REQUEST_USER_NULL		= -1,
	SHELLMCUI_REQUEST_USER_CONTINUE	= 0,
	SHELLMCUI_REQUEST_USER_CANCEL,
	SHELLMCUI_REQUEST_USER_YESNO,	
	SHELLMCUI_REQUEST_USER_NUM
} EShellMCUIUserRequest;


//Requests by the module for data from the application:
typedef enum
{
	SHELLMCUI_REQUEST_DATA_NULL				= -1,
	SHELLMCUI_REQUEST_DATA_LIST_SAVE_PARMS	= 0,
	SHELLMCUI_REQUEST_DATA_LOAD_PARMS,
	SHELLMCUI_REQUEST_DATA_SAVE_PARMS,	
	SHELLMCUI_REQUEST_DATA_NUM
} EShellMCUIDataRequest;


//-----------------------------------------------------------------------------
// T Y P E D E F  S T R U C T S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark TYPEDEF STRUCTS
#endif

//User (keypress) input:
typedef struct _ShellMCUIUserInput
{
	EShellMCUIMainMenuOption	userMainMenuOption;
	EShellMCUIYesNo				userYesNo;
	bool						userContinue;
	bool						userCancel;		//Triangle button

} ShellMCUIUserInput;



//Parameters for saving:
typedef struct _ShellMCUISaveParms
{
	//Icon file info; icon files saved only when creating a new dir:
	//sceMcIconSys is a structure for generating icon.sys files as used by
	//the console browser:
	sceMcIconSys*	pIconSysData;
	
	//Parms for list, copy and delete icons:
	char*			pIconFileData[SHELLMEMCARD_ICON_NUM_ICONS];
	u_int			iconFileSize[SHELLMEMCARD_ICON_NUM_ICONS];

	char			iconFileNames[SHELLMEMCARD_ICON_NUM_ICONS][SHELLMEMCARD_MAX_LEN_FILENAME];

	//User data file (has same name as save dir - TRC requirement):
	char*			pUserFileData;
	u_int			userFileSize;

} ShellMCUISaveParms;


//Initialisation paramaters:
typedef struct _ShellMCUIInitParms
{
	bool			separateDirPerSave;
	int				maxNumSavesPerCard;
	
	const char*		keyCode;
	const char* 	productModelNo;
	
} ShellMCUIInitParms;


//shellMCUI input parameters:
typedef struct _ShellMCUIInputs
{
	bool 	dataValid;	//Set this to indicate that data input is valid

	//*********************************************************************	    				
	//Data input: 
	//Port no & slot no:; for this release both must be set to 0 since only
	//slot 1 is supported:
	int 	portNo;
	int 	slotNo;

	char 	dirNameExtension[SHELLMEMCARD_MAX_LEN_FILENAME];

	//Data file info:
	int		numDataFiles;
	char*	pDataFileData[SHELLMCUI_MAX_NUM_DATA_FILES];
	int		dataFileSize[ SHELLMCUI_MAX_NUM_DATA_FILES];
	char	dataFileNames[SHELLMCUI_MAX_NUM_DATA_FILES][SHELLMEMCARD_MAX_LEN_FILENAME];
			
	//int				maxNumSaves;
	ShellMCUISaveParms	saveParms;

	//*********************************************************************	    				
	//Keypress / user input (main menu option, yes/no, continue, cancel):
	ShellMCUIUserInput	userInput;
			
} ShellMCUIInputs;


//shellMCUI output parameters:
typedef struct _ShellMCUIOutputs
{
	//Valid main menu options:
	bool	mainMenuOptionsValid[SHELLMCUI_MAIN_MENU_OPTION_NUM];

	//List of saves:
	int		numSavesFound;
	int		spaceRequiredForSave;
	
	//Names of title saves:	
	char	saveNames[SHELLMCUI_MAX_NUM_SAVES_PER_CARD][SHELLMEMCARD_MAX_LEN_FILENAME];	
	
	//Messages to display (errors / warnings):
	bool	statusFlags[SHELLMCUI_STATUS_NUM];
	
	//User input request (yes/no, continue, load / save slot number):
	bool	requestUser[SHELLMCUI_REQUEST_USER_NUM];
	bool	requestData[SHELLMCUI_REQUEST_DATA_NUM];
	
	//Card status info:
	ShellMemCardStatus			cardStatus;
	EShellMCUIMainMenuOption	mainMenuOptionSelected;
	
} ShellMCUIOutputs;


//-----------------------------------------------------------------------------
// C O N S T A N T S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark CONSTANTS
#endif

//Extern language constants:
extern const char* 	SHELLMCUI_STATUS_TEXT_ENGLISH[SHELLMCUI_STATUS_NUM];
extern const char* 	SHELLMCUI_STATUS_TEXT_FRENCH[SHELLMCUI_STATUS_NUM];
extern const char** SHELLMCUI_STATUS_TEXT_LANGUAGES[];


//Status message classification:
static const EShellMCUIStatusType SHELLMCUI_STATUS_TYPES[SHELLMCUI_STATUS_NUM] = 
{
	//Card status:
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_DETECTING,
	SHELLMCUI_STATUS_TYPE_CARD,			//SHELLMCUI_STATUS_PS2_CARD_INSERTED,
	SHELLMCUI_STATUS_TYPE_CARD,			//SHELLMCUI_STATUS_PS2_CARD_NOT_INSERTED,
	SHELLMCUI_STATUS_TYPE_CARD,			//SHELLMCUI_STATUS_UNFORMATTED_CARD,
	
	//Save data status:
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_INSUFFICIENT_SPACE,
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_NO_SAVES,
	
	//Request user status:
	SHELLMCUI_STATUS_TYPE_REQUEST_USER,	//SHELLMCUI_STATUS_REQUEST_USER_FORMAT,
	SHELLMCUI_STATUS_TYPE_REQUEST_USER,	//SHELLMCUI_STATUS_REQUEST_USER_OVERWRITE,

	//Operation status:
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_LOADING,
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_SAVING,
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_FORMATTING,
	
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_LOAD_SUCCESSFUL,
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_SAVE_SUCCESSFUL,
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_FORMAT_SUCCESSFUL,
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_LIST_SAVES_SUCCESSFUL,

	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_LOAD_FAILED,
	SHELLMCUI_STATUS_TYPE_OPERATION,	//SHELLMCUI_STATUS_SAVE_FAILED,
	SHELLMCUI_STATUS_TYPE_OPERATION		//SHELLMCUI_STATUS_FORMAT_FAILED
};


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

#ifdef __MWERKS__
	#pragma mark -
	#pragma mark PROTOTYPES
#endif

#ifdef __cplusplus
extern "C" {
#endif

//Module initialisation:
void				shellMCUIInit(ShellMCUIInitParms*	pInitParms);

//Module tick function:
EShellMCUIScreen	shellMCUIUpdate(ShellMCUIOutputs* 	pOutputParms,
									ShellMCUIInputs*	pInputParms);



#ifdef __cplusplus
}
#endif

#endif
