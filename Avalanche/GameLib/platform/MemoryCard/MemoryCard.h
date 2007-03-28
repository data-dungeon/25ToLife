//--------------------------------------------------------------//
// MemoryCard.h
//
// Memory Card Handler
//
// *NOTE* - Using this asyncronously
//  
// To do this, you need to set up a polling function in your
// main loop that checks the following:
//
// 1) Check PopupRequest - If this is true, a popup needs to be
//	   started. The popup is responsible for setting the popup
//    result. The memory card thread will sit and wait for this
//    before going further. Note, that once the popup has been
//    activated, you are responsible for clearing the popuprequest
//    flag, so another can be requested once this one is done.
// 2) Check PopupClearRequest - As above except to remove the
//    popup when requested.
// 3) Poll GetState() == CMEMORYCARD_IDLE.. If this is true
//    the memory card thread is done with its current operations.    
// 4) Note, that actually writing / reading to the card will be
//    a blocking call unless the user specifies an update function
//    to be called. This is all handled in the low-level platform
//    drivers for various systems and not in the memcard thread
//    manager ( I may end up re-writing some of that stuff )
//
//--------------------------------------------------------------//
#ifndef _MEMORYCARD_H
#define _MEMORYCARD_H

#include "platform/MemoryCard/Memcard_english_common.h"

// networking
#if defined (PS2)
#include <netz.h>
#include <platform/stack/ps2common/ps2networking.h>
#include <platform/stack/ps2common/ps2module.h>
#include <platform/stack/ps2common/ps2profilelist.h>
#endif

class CThread;	// forward decl

//--------------------------------------------------------------//

#define	MAX_FILES_ON_CARD		256
#define	ENCRYPTION_KEY			"Don't try to hack this game you l33t wankers"
#define	MEMCARD_VERSION		0xBAFF0004						// update this if memcard funcs change

// DEBUG ONLY!!!
//#define	DONT_USE_POPUPS		// turn this on to avoid using popups.. the choices will be made automatically

#if defined (_XBOX)
#define	MAX_MEMCARDS			9		// Max (8 controllers slots + 1 HD)
#elif defined (PS2)
#define	MAX_MEMCARDS			2		// Max (2)
#elif defined (GCN)
#define	MAX_MEMCARDS			4		// Max (4)
#else
#define	MAX_MEMCARDS			1		// Max (1)
#endif

// card change tracking for autosave only
#define TURN_ON_CARD_CHANGE

//--------------------------------------------------------------//

typedef	void	(CMemCard_VoidFunc)(void);
typedef	int	(CMemCard_IntFunc)(void);

//--------------------------------------------------------------//
// memcard struct header

struct	ts_MemcardHeader
{
	int	version;
	int	appVersion1;			// user defined fields
	int	appVersion2;			// 
	int	compressed;				// compressed?
	int	compression_type;
	int	compressedSize;
	int	encrypted;				// encrypted?
	int	encryption_type;
	int	encryptedSize;
	int	uncompressedSize;		// datasize without header
	int	checksum;
	int	checksumsize;
	int	datasize;				// datasize (passed into Save)
	char	InfoString[64];		// use this for whatever you like
	char	DateString[64];		// for date text if needed
};

// encryption types supported

enum	{
	MC_ENCRYPT_HELIX,
	MC_ENCRYPT_RIJNDAEL,
	MC_ENCRYPT_BLOWFISH,
	MC_MAX_ENCRYPT_TYPES,
};

// compression types supported

enum	{
	MC_COMPRESS_0,
	MC_MAX_COMPRESS_TYPES,
};

//--------------------------------------------------------------//

enum {
	MC_SAVED_GAME,				// default
	MC_SAVED_PLAYER,			
	MC_SAVED_SETTINGS,
	MC_SAVED_PROFILE,
	MC_SAVED_PATCH,
   MC_SAVED_ACCOUNT,
};

// popup messages
enum	{
	CMEMCARDMSG_NO_VALID_SAVE,
	CMEMCARDMSG_MEMCARD_FULL,
	CMEMCARDMSG_MEMCARD_FULL_MULTIPLE,
	CMEMCARDMSG_NO_CARD_SAVE,
	CMEMCARDMSG_NO_CARD_LOAD,
	CMEMCARDMSG_AUTOSAVE_NO_CARD,
	CMEMCARDMSG_SURE_LOAD,
	CMEMCARDMSG_SURE_SAVE,
	CMEMCARDMSG_SURE_DELETE,
	CMEMCARDMSG_SURE_OVERWRITE,
	CMEMCARDMSG_LOADING_FILE,
	CMEMCARDMSG_SAVING_FILE,
	CMEMCARDMSG_OVERWRITING_FILE,
	CMEMCARDMSG_DELETE_FILE,
	CMEMCARDMSG_LOADING_CANCELLED,
	CMEMCARDMSG_SAVING_CANCELLED,
	CMEMCARDMSG_DELETE_CANCELLED,
	CMEMCARDMSG_FORMAT_CANCELLED,
	CMEMCARDMSG_FORMATTING,
	CMEMCARDMSG_SURE_FORMAT,
	CMEMCARDMSG_ACCESSING_CARD,
	CMEMCARDMSG_DETECT_CARD,
	CMEMCARDMSG_OTHER_MARKET,
	CMEMCARDMSG_UNFORMATTED_CARD,
	CMEMCARDMSG_DAMAGED_CARD,
	CMEMCARDMSG_CORRUPT_CARD,
	CMEMCARDMSG_CARD_REMOVED,
	CMEMCARDMSG_COMPLETED_OK,
	CMEMCARDMSG_COMMAND_FAILED,
	CMEMCARDMSG_FORMAT_FAILED,
	CMEMCARDMSG_AUTOSAVE_REMOVED,
	CMEMCARDMSG_UNFORMATTED_CARD_OK,
	CMEMCARDMSG_FORMAT_COMPLETED_OK,
	CMEMCARDMSG_DELETE_COMPLETED_OK,
	CMEMCARDMSG_LOAD_COMPLETED_OK,
	CMEMCARDMSG_SAVE_COMPLETED_OK,
	CMEMCARDMSG_CARD_CHANGED,
	CMEMCARDMSG_NETWORK_CONFIG_FAIL,
	CMEMCARDMSG_NETWORK_MAGIC_FAIL,
	CMEMCARDMSG_NETWORK_CONFIG_BAD,
	CMEMCARDMSG_AUTOSAVING,
	CMEMCARDMSG_SAVE_FAILED,
	CMEMCARDMSG_MEMCARD_FULL_STARTUP_PAL,
	CMEMCARDMSG_MEMCARD_FULL_FILES,
	CMEMCARDMSG_MEMCARD_CONTINUE_SURE,
	CMEMCARDMSG_MEMCARD_FULL_STARTUP,			//	(NTSC)
	CMEMCARDMSG_MEMCARD_OVERWRITE_FAILED,		// overwrites
	CMEMCARDMSG_MEMCARD_SURE_OVERWRITE,
	CMEMCARDMSG_MEMCARD_OVERWRITE_FILE,
	CMEMCARDMSG_MEMCARD_CONTINUE_SURE_NOCARD,
	CMEMCARDMSG_MEMCARD_OVERWRITE_COMPLETED_OK,
	CMEMCARDMSG_NO_CARD_STARTUP_1,
	CMEMCARDMSG_NO_CARD_STARTUP_2,
	CMEMCARDMSG_BAD_CONTROLLER,
	CMEMCARDMSG_ARE_YOU_SURE,
};

// popup results
enum {
	PR_COMPLETED_OK,
	PR_LOAD_NO,
	PR_LOAD_CANCEL,
	PR_SAVE_NO,
	PR_SAVE_CANCEL,
	PR_DELETE_NO,
	PR_DELETE_CANCEL,
	PR_OVERWRITE_NO,
	PR_OVERWRITE_CANCEL,
	PR_FORMAT_NO,
	PR_FORMAT_CANCEL,
	PR_FORMAT_FAILED,
	PR_FORMAT_OK,
	PR_CORRUPT_CANCEL,
	PR_DAMAGED_CANCEL,
	PR_OTHER_MARKET_CANCEL,
	PR_WRONG_TYPE_CANCEL,
	PR_MEMCARD_FULL_CANCEL,
	PR_CONTINUE_WITHOUT_SAVING,
	PR_CHOOSE_EXISTING,
	PR_NOCARD_CANCEL,
	PR_NO_VALID_SAVE_CANCEL,
	PR_BADPARAM,
	PR_FAILED,
	PR_NOCARD_NO,
	PR_NOCARD_YES,
	PR_QUIT,
	PR_RETRY_YES,
	PR_CANCEL,
};

// substring types
enum {
	CMEMORYCARD_SAVE_TYPE,
	CMEMORYCARD_MEMCARD_NAME,
	CMEMORYCARD_MEMCARD_PORT,
	CMEMORYCARD_CONTROL_PORT,
	CMEMORYCARD_BLOCKS_FREE,
	CMEMORYCARD_BLOCKS_NEEDED,
	CMEMORYCARD_BLOCKS_ALL,
	CMEMORYCARD_TITLE_NAME,
};

// memcard operations
enum {
	CMEMORYCARD_STATE_LOAD,
	CMEMORYCARD_STATE_SAVE,
	CMEMORYCARD_STATE_DELETE,
	CMEMORYCARD_STATE_STARTUP,
	CMEMORYCARD_STATE_EXISTS,
	CMEMORYCARD_STATE_FORMAT,
	CMEMORYCARD_STATE_DIR,
	CMEMORYCARD_STATE_AUTOSAVE,
	CMEMORYCARD_STATE_LOAD_NETWORK,
	CMEMORYCARD_STATE_CONTROLLER_STARTUP,
};

#define	MAX_POPUP_CHOICES	8

//--------------------------------------------------------------//
// Popups

struct	ts_PopupChoices
{
	char	*ChoiceText;
	int	ChoiceResult;
};

struct	ts_PopupInfo
{
	char	Title[64];											// popup title
	char	String[256];										// string to display
	int	NumChoices;											// Number of choices
	int	Result;												// result
	int	Default;												// default menu choice
	int	PassThrough;										// used to make sure a popup goes away automatically.

	ts_PopupChoices	Choices[MAX_POPUP_CHOICES];	// choices & results needed
};

//--------------------------------------------------------------//

struct	ts_CMemCardDate
{
	int	Month;
	int	Day;
	int	Year;
};

struct	ts_CMemCardTime
{
	int	Hour;
	int	Minutes;
	int	Seconds;
};

struct	ts_CMemCardFiles
{
	char					Filename[64];
	char					Directory[64];
	int					SizeInBytes;
	int					SizeInBlocks;
	int					ControllerPort;
	int					Port;
	char					MemcardName[64];
	char					DisplayName[64];
	char					LoadFilename[64];
	ts_CMemCardDate	Date;
	ts_CMemCardTime	Time;
};

struct	ts_CMemCardMsg
{
	char*		String;
	int		SubString1;
	int		SubString2;
	int		SubString3;
	int		SubString4;
};

class	CMemoryCard
{
public:

	// state
	enum	{
		CMEMORYCARD_IDLE,
		CMEMORYCARD_DETECT,
		CMEMORYCARD_DETECT_WITH_RESULT,
		CMEMORYCARD_CHECKCARD,
		CMEMORYCARD_LOAD,
		CMEMORYCARD_SAVE,
		CMEMORYCARD_STARTUP,
		CMEMORYCARD_DELETE,
		CMEMORYCARD_FORMAT,
		CMEMORYCARD_FILE_EXISTS,
		CMEMORYCARD_ERROR,
		CMEMORYCARD_HANDLE_RESULT,
		CMEMORYCARD_GET_FILES_ON_CARD,
		CMEMORYCARD_GET_FILES_ON_ALL_CARDS,
		CMEMORYCARD_DETECT_ON_START,
		CMEMORYCARD_LOAD_NETWORK,
		CMEMORYCARD_CONTROLLER_STARTUP,
	};

	enum {
		CMEMORYCARD_UNCOMPRESSED,
		CMEMORYCARD_COMPRESSED,
	};

	enum {
		CMEMORYCARD_UNENCRYPTED,
		CMEMORYCARD_ENCRYPTED,
	};

	enum {
		SHOWPOPUP,
		HIDEPOPUP,
		POPUPRESULT,
		CLEARRESULT,
	};

	enum	{
		CMEMORYCARD_NOTHING_IN_PROGRESS,
		CMEMORYCARD_IN_PROGRESS
	};
	
	enum {
		CMEMORYCARD_RIJNDAEL_RU = 16,
		CMEMORYCARD_BLOWFISH_RU = 0,
	};
	//ctor/dtor
	CMemoryCard();
	~CMemoryCard();

	//main funcs
	bool		Init(bool Compress, int CompressionType, bool Encrypt, int EncryptionType, 
					  int MaxSlots, int FixedSlotSize, char* TitleName, char* TitleID, int NumCardsToPoll, int RoundBuffer = 0,
					  int AppVersion1 = 0, int AppVersion2 = 0);
	void		DeInit(void);
	void		Update();

	//gets
	CThread*	GetThread() { return (m_MemCardThread); };								// Get memory card thread
	int		GetState()  { return (m_State); };											// Get current Thread state (internal to memory card thread)
	int		GetLastState()  { return (m_LastState); };								//
	char*		GetFilename() { return (m_Filename); };									// Get current filename for load/save
	char*		GetComment() { return (m_Comment); };										// Get comment (used for dir/container name)
	int		GetPort() { return (m_Port); };												// Get active memcard port
	int		GetControllerPort() { return (m_Controller); };							// Get controller that memcard is in
	int		GetFilesize() { return (m_Filesize); };									// Get filesize to load/save
	char*		GetBuffer() { return (m_Buffer); };											// Get Data buffer
	int		GetResult() { return (m_Result); };											// Get last result of memcard operation
	bool		GetNoSave() { return (m_ContinueWithoutSaving); };						// Has continue without saving been chosen
	int		GetNumFilesOnCard() { return (m_NumFilesOnCard); };					// Get num files on a memcard
	bool		GetCompress() { return (m_Compress); };									// Are we to compress save game?
	bool		GetEncrypt() { return (m_Encrypt); };										// Are we to encrypt save game?
	bool		GetAreYouSure() { return (m_AreYouSure); };								// Do we want 'are you sure' on this memcard operation
	int		GetSaveType() { return (m_SaveType); };									// Save type for popup strings (game, settings, etc)
	int		GetBlocksToDelete() { return(m_BlocksToDelete); };						// Number of blocks to delete (for dashboard use)
	void		GetFileInfo(int FileNum, ts_CMemCardFiles &memCardFile );			// Get a directory entry
	char*		GetSearchMask() { return (m_SearchMask); };								// Search mask for directory listings
	char*		GetExt() { return (m_SaveGameExt); };										// Extension to use on a save game
	bool		GetUsePopups() { return (m_UsePopups); };									// Are we using popups (DEBUG).
	char*		GetLoadSaveBuffer() { return (m_LoadSaveBuffer); };					// Get temp compress buffer
	char*		GetName() { return (m_Name); };												// Get Name
	bool		GetFirstSave() { return(m_FirstSave); };									// Is this first save (needed for sony for TCR)
	char*		GetTitleID() { return (m_TitleID); };										// Title ID needed for Sony dir name
	int		GetPopupExitResult() { return (m_PopupExitResult); };					// Exit result from last popup used
	bool		GetAutosave() { return (m_UseAutoSave); };								// Are we using autosave
	int		GetMainState() { return (m_MainState); };									// What was the inital memcard operation (load / save / etc)
	int		GetEncryptionType() { return (m_EncryptionType); };					// Encryption Type
	int		GetCompressionType() { return (m_CompressionType); };					// Compression Type
	bool		GetCardChanged(int Port) { return (m_CardChanged[Port]); };			// Memcard inserted or removed
	bool		GetCardActive(int Slot) { return (m_CardSlotsActive[Slot]); };		// set a card state
	bool		GetCardPollingOn() { return (m_CardPollingOn); };						// check cards for insertion/removal
	int		GetNumCardsToPoll() { return (m_NumCardsToPoll); };					// how many slots to check
	int		GetCompressedSize() { return (m_CompressedSize); };					// Size of compressed file
	int		GetEncryptedSize() { return (m_EncryptedSize); };						// Size of encrypted file
	int		GetUncompressedSize() { return (m_UncompressedSize); };				// Size of uncompressed file
	bool		GetStartupCheckDone() { return (m_StartupCheckDone); };				// Set startup check is done?
	bool		GetPopupRequest() { return (m_PopupRequest); };							// Popup Needed?
	bool		GetPopupClearRequest() { return (m_PopupClearRequest); };			// Popup Needed?
	bool		GetLoadHeader() { return (m_LoadHeader); };								// Load Header only?
	char*		GetInfoString() { return (m_InfoString); };								// Get Info
	char*		GetDateString() { return (m_DateString); };								// Get Date
	bool		GetCardInserted(int Port) { return (m_CardInserted[Port]); };		// Card Changed
	int		GetAppVersion1() { return (m_appVersion1); };							// app defined field 1
	int		GetAppVersion2() { return (m_appVersion2); };							// app defined field 2
	int		GetLoadedAppVersion1() { return (m_LoadedappVersion1); };			// app defined field 1 on load
	int		GetLoadedAppVersion2() { return (m_LoadedappVersion2); };			// app defined field 2 on load
	bool		GetAutoSaveIcon() { return (m_AutoSaveIcon); };							// are we using autosave icon or popups?
	char*		GetTitleName() { return (m_TitleName); };									// Name of title for popups
	char*		GetDefaultFileName() { return (m_DefaultFileName); };					// Default filename in case of blanks
	void*		GetPopupCallback() { return (m_ShowPopup); };							// get popup callback pointer
	int		GetInProgress() { return (m_InProgress); };								// get in progress type
	bool		GetIgnoreErrors() { return (m_IgnoreErrors); };							// Ignore Errors.. special case needed for PS2
	int		GetNumNetworkProfiles() { return (m_NumNetworkProfiles); };			// get network profiles on card
	bool		GetOverwriteConfirm() { return (m_OverwriteConfirm); };				// Do we want 'overwrite?' on this memcard operation
	bool		GetAreYouReallySure() { return (m_AreYouReallySure); };				// Are you sure? Double confirmation
	bool		GetNoOkConfirmation() { return (m_NoOkConfirmation); };				// No ok, on task done if completed without errors
	bool		GetUseActualFileSize() { return (m_UseActualFileSize); };			// use actual filesize, no icon size etc
	char*		GetPassedInSaveBuffer() { return (m_PassedInSaveBuffer); };			// the original ptr to data to save.
	bool		GetCancelOperation() { return (m_CancelOperation); };			// use actual filesize, no icon size etc

	#if defined (PS2)
	PS2ProfileList*		GetNetworkProfileList() { return (&m_NetworkProfileList); };		// get network profile list (PS2)
	#endif


	//sets
	void		SetState(int state) { m_State = state; };
	void		SetLastState(int state) { m_LastState = state; };
	void		SetFilename(char* Filename) { m_Filename = Filename; };
	void		SetComment(char* Comment) { strcpy(m_Comment, Comment); };
	void		SetName(char* Name) { m_Name = Name; };
	void		SetPort(int Port) { m_Port = Port; };
	void		SetControllerPort(int Port) { m_Controller = Port; };
	void		SetFilesize(int Size) { m_Filesize = Size; };
	void		SetBuffer(char* Buffer) { m_Buffer = Buffer; };
	void		SetResult(int Result) { m_Result = Result; };
	void		SetNoSave(bool NoSave) { m_ContinueWithoutSaving = NoSave; };
	void		SetNumFilesOnCard(int NumFiles) { m_NumFilesOnCard = NumFiles; };
	void		SetAreYouSure(bool AreYouSure) { m_AreYouSure = AreYouSure; };
	void		SetSaveType(int SaveType) { m_SaveType = SaveType; };
	void		SetBlocksToDelete(int BlocksToDelete) { m_BlocksToDelete = BlocksToDelete; };
	void		SetSearchMask(char* Mask) { m_SearchMask = Mask; };
	void		SetExt(char* Ext) { m_SaveGameExt = Ext; };
	void		SetUsePopups(bool UsePopups) { m_UsePopups = UsePopups; };
	void		SetFirstSave(bool FirstSave) { m_FirstSave = FirstSave; };
	void		SetTitleID(char* TitleID) { m_SaveGameExt = TitleID; };
	void		SetPopupExitResult(int Result) { m_PopupExitResult = Result; };
	void		SetAutosave(bool Autosave) { m_UseAutoSave = Autosave; };
	void		SetMainState(int State) { m_MainState = State; };
	void		SetEncryptionType(int Type) { m_EncryptionType = Type; };
	void		SetCompressionType(int Type) { m_CompressionType = Type; };
	void		SetCardChanged(int Port, bool Changed) { m_CardChanged[Port] = Changed; };
	void		SetCardActive(int Slot, bool Active) { m_CardSlotsActive[Slot] = Active; };
	void		SetCardPollingOn(bool Polling) { m_CardPollingOn = Polling; };
	void		SetNumCardsToPoll(int NumCards) { m_NumCardsToPoll = NumCards; };
	void		SetCompressedSize(int Size) { m_CompressedSize = Size; };
	void		SetUncompressedSize(int Size) { m_UncompressedSize = Size; };
	void		SetEncryptedSize(int Size) { m_EncryptedSize = Size; };
	void		SetStartupCheckDone(bool Flag) { m_StartupCheckDone = Flag; };
	void		SetPopupRequest(bool Flag) { m_PopupRequest = Flag; };
	void		SetPopupClearRequest(bool Flag) { m_PopupClearRequest = Flag; };
	void		SetLoadHeader(bool Flag) { m_LoadHeader = Flag; };
	void		SetInfoString(char* String) { strcpy(m_InfoString, String); };
	void		SetDateString(char* String) { strcpy(m_DateString, String); };
	void		SetCardInserted(int Port, bool Flag) { m_CardInserted[Port] = Flag; };
	void		SetAppVersion1(int Version) { m_appVersion1 = Version; };							// app defined field 1
	void		SetAppVersion2(int Version) { m_appVersion2 = Version; };							// app defined field 2
	void		SetLoadedAppVersion1(int Version) { m_LoadedappVersion1 = Version; };			// app defined field 1 on load
	void		SetLoadedAppVersion2(int Version) { m_LoadedappVersion2 = Version; };			// app defined field 2 on load
	void		SetAutoSaveIcon(bool Flag) { m_AutoSaveIcon = Flag; };								// use autosave icon
	void		SetTitleName(char* String) { strcpy(m_TitleName, String); };						// set game title name
	void		SetDefaultFileName(char* String) { strcpy(m_DefaultFileName, String); };		// set game default filename
	void		SetInProgress(int InProgress) { m_InProgress = InProgress; };						// in progress type
	void		SetIgnoreErrors(bool IgnoreErrors) { m_IgnoreErrors = IgnoreErrors; };			// ignore errors
	void		SetNumNetworkProfiles(int NumProfiles) { m_NumNetworkProfiles = NumProfiles; };	// num net profiles
	void		SetOverwriteConfirm(bool Flag) { m_OverwriteConfirm = Flag; };
	void		SetAreYouReallySure (bool Flag) { m_AreYouReallySure = Flag; };					// Are you sure? Double confirmation
	void		SetNoOkConfirmation (bool Flag) { m_NoOkConfirmation = Flag; };					// No Ok confirmation on complete withour errors
	void		SetUseActualFileSize(bool Flag) { m_UseActualFileSize = Flag; };					// Use filesize, no icon size etc
	void		SetPassedInSaveBuffer(char* Buffer) { m_PassedInSaveBuffer = Buffer; };			// Original Data ptr.. passed in to Save
	void		SetCancelOperation(bool Flag) { m_CancelOperation = Flag; };						// Cancel Operation
	
	//file funcs
	void		SaveFile(char *Filename, char* Comment, int Port, char* Buffer, int Filesize, bool AreYouSure, int SaveType = MC_SAVED_GAME);
	void		LoadFile(char *Filename, char* Comment, int Port, char* Buffer, int Filesize, bool AreYouSure, int SaveType = MC_SAVED_GAME);
	void		LoadNetwork(int Port);
	void		LoadHeader(char *Filename, char* Comment, int Port, char* Buffer, bool AreYouSure, int SaveType = MC_SAVED_GAME );
	void		StartupCheck( int Port, int Filesize, int SaveType = MC_SAVED_GAME);
	void		StartupControllerCheck( int Port );
	void		DeleteFile(char *Filename, char* Comment, int Port, int SaveType = MC_SAVED_GAME);
	void		DetectCard(int Port);
	void		DetectCardWithResult(int Port);
	void		DetectCardOnStart(int Port);
	void		FormatCard(int Port);
	void		FileExists(char *Filename, char* Comment, int Port);
	void		GetFilesOnCard(int Port, char *Filename, char*SearchMask, bool IgnoreErrors);
	void		GetFilesOnAllCards(char* Filename, char* SearchMask, bool IgnoreErrors);
	void		HandleResult();

	//util funcs
	void		FillOutFileInfo(int FileNum, char* Filename, char* Directory,
									 int Port, int ControllerPort, char* MemCardName,
									 int SizeInBytes, ts_CMemCardDate Date, ts_CMemCardTime Time);
	void		WaitUntilIdle();
	bool		CompressData(int Type = 0);
	bool		UncompressData(int Type = 0);
	bool		EncryptData(int Type = MC_ENCRYPT_HELIX);
	bool		UnencryptData(int Type = MC_ENCRYPT_HELIX);
	void		CreateLoadSaveBuffer(int Filesize);
	void		ResetLoadSaveBuffer();
	void		CreateEncryptBuffer(int Filesize);
	void		ResetEncryptBuffer();
	void		SetPopupDisplayTime(float time);
	float		GetPopupDisplayTime();
	void		ResetPopupDisplayTime();
	void		SetPopupTitle(char* Title);
	void		TempDisableAutosave( bool Flag );
	void		RecopyDataBuffer(void);

	//functions that need to be supplied by app
	void		RegisterPopupFunction(int FunctionType, void* Function);
	void		DisplayPopup(	int MsgIndex, int PassThruChoice, int Default = 0,
									int Choice1 = -1, int Choice2 = -1, int Choice3 = -1, int Choice4 = -1);
	void		RemovePopup(void);
	int		PopupResult(void);

	// optional
	void		RegisterWaitUntilIdleUpdate(void* Function);

	//exit
	void		Exit() { m_ExitThread = true; };
	bool		ExitThread()  { return (m_ExitThread); };

	// network configuration error codes
	void		SetErrorCode( int error )	{ m_errorCode = error; }
	int		GetErrorCode( void )			{ return m_errorCode; }

public:
	ts_PopupInfo	PopupInfo;

protected:
	CMemCard_VoidFunc*	m_ShowPopup;
	CMemCard_VoidFunc*	m_HidePopup;
	CMemCard_IntFunc*		m_PopupResult;
	CMemCard_VoidFunc*	m_WaitUntilIdleUpdate;
	CMemCard_VoidFunc*	m_ClearPopupResult;

	CThread*		m_MemCardThread;
	int			m_State;
	int			m_LastState;
	bool			m_ExitThread;
	char*			m_Filename;
	char			m_Comment[64];
	int			m_Port;
	int			m_Controller;
	int			m_Filesize;
	char*			m_Buffer;
	char*			m_SearchMask;
	char*			m_SaveGameExt;
	int			m_Result;
	bool			m_ContinueWithoutSaving;
	int			m_NumFilesOnCard;
	bool			m_Compress;
	bool			m_Encrypt;
	int			m_MaxSlots;
	int			m_FixedSlotSize;
	char			m_TitleName[64];
	bool			m_AreYouSure;
	int			m_SaveType;
	int			m_BlocksToDelete;
	bool			m_UsePopups;
	int			m_CorrectChoice;
	char*			m_LoadSaveBuffer;
	char*			m_Name;
	float			m_PopupTime;
	char			m_PopupTitle[64];
	char*			m_EncryptBuffer;
	bool			m_FirstSave;
	char*			m_TitleID;
	int			m_PopupExitResult;
	bool			m_UseAutoSave;
	int			m_MainState;
	int			m_RoundBuffer;
	int			m_EncryptionType;
	int			m_CompressionType;
	bool			m_CardChanged[MAX_MEMCARDS];
	bool			m_CardSlotsActive[MAX_MEMCARDS];
	bool			m_CardPollingOn;
	int			m_NumCardsToPoll;
	int			m_CompressedSize;
	int			m_EncryptedSize;
	int			m_UncompressedSize;
	bool			m_StartupCheckDone;
	bool			m_PopupRequest;
	bool			m_PopupClearRequest;
	bool			m_LoadHeader;
	char			m_InfoString[64];
	char			m_DateString[64];
	bool			m_CardInserted[MAX_MEMCARDS];
	bool			m_TempAutosave;
	int			m_errorCode;
	int			m_appVersion1;			// app defined fields
	int			m_appVersion2;
	int			m_LoadedappVersion1;
	int			m_LoadedappVersion2;
	bool			m_AutoSaveIcon;
	char			m_DefaultFileName[64];
	int			m_InProgress;
	bool			m_IgnoreErrors;
	int			m_NumNetworkProfiles;
	bool			m_OverwriteConfirm;
	bool			m_AreYouReallySure;
	bool			m_NoOkConfirmation;
	bool			m_UseActualFileSize;
	char*			m_PassedInSaveBuffer;
	bool			m_CancelOperation;

	#if defined (PS2)
	PS2ProfileList		m_NetworkProfileList;
	#endif

	ts_CMemCardFiles			m_FilesOnCard[MAX_FILES_ON_CARD];
};

extern	CMemoryCard				*g_MemCardManager;
extern	char*	MC_SaveTypes[];

#endif	// _MEMORYCARD_H
