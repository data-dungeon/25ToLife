//--------------------------------------------------------------//
// MemoryCard.cpp
//
// Memory Card Handler (Thread based)
// Author: Paul Blagay
//
//--------------------------------------------------------------//

#include "platform/PlatformPCH.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "platform/MemoryCard/Memcard_english_common.h"
#include "platform/memcard.h"
#include "EngineHelper/timer.h"
#include "EngineHelper/LoadScreen.h"
#include "hud/hudlib.h"
#include "Game/Managers/RenderMgr.h" 

#include "GameHelper/encryption/Rijndael/rijndael.h"
#include "GameHelper/encryption/Helix/Helix.h"
#include "GameHelper/compression/compress2.h"
#include "GameHelper/Translator.h"
#include "Layers/thread/thread.h"

#if defined (PS2)
#include "platform/PS2/ps2card.h"
#elif defined (_XBOX)
#include "platform/Xbox/xboxcard.h"
#elif defined (WIN32)
#include "platform/Win32/wincard.h"
#endif

//--------------------------------------------------------------//
// externs

#if defined (WIN32)
extern	int	MemCardThreadFunc();
#elif defined (PS2)
extern	void	MemCardThreadFunc(void* data);
#elif defined (GCN)
#endif

extern bool g_regionUSA;

//--------------------------------------------------------------//
// prototypes

void CMemoryCard_AsyncUpdate(void);

//--------------------------------------------------------------//
// externs
extern	ts_CMemCardMsg	CMemCardMsg[];

//--------------------------------------------------------------//
// defines

//#define	USE_RIJNDAEL
#define	USE_HELIX

//--------------------------------------------------------------//
// vars
CMemoryCard					*g_MemCardManager;

// strings for popups based on content (default is game)
char*	MC_SaveTypes[] = {	"game",
									"player",
									"settings",
									"profile",
									"patch",
                           "account",
};

// popup choices
char*	MC_Choices[] = {	MC_OK_TEXT,
								MC_CANCEL_TEXT,
								MC_YES_TEXT,
								MC_NO_TEXT,
								MC_CONTINUE_TEXT,
								MC_CONTINUE_WITHOUT_SAVING_TEXT,
								MC_CONTINUE_WITHOUT_LOADING_TEXT,
								MC_RETRY_TEXT,
								MC_QUIT_TEXT,
								MC_MANAGE_FILES_TEXT,
								MC_DELETE_TEXT,
								MC_ENABLE_AUTOSAVE_TEXT,
								MC_CHOOSE_EXISTING_TEXT,
};


//--------------------------------------------------------------//
// methods
CMemoryCard::CMemoryCard()
{
	m_State = CMEMORYCARD_IDLE;
	m_MemCardThread = NULL;
	m_Buffer = NULL;
	m_Filename = NULL;
	m_Comment[0] = 0;
	m_InfoString[0] = 0;
	m_DateString[0] = 0;
	m_Port = 0;
	m_Filesize = 0;
	m_Result = MEMCARD_RESULT_OK;
	m_ContinueWithoutSaving = false;
	m_NumFilesOnCard = 0;
	m_Compress = false;
	m_Encrypt = false;
	m_MaxSlots = 0;
	m_FixedSlotSize = 0;
	m_BlocksToDelete = 0;
	m_AreYouSure = true;
	m_SaveType = MC_SAVED_GAME;
	m_FirstSave = true;
	m_PopupExitResult = PR_COMPLETED_OK;
	m_UseAutoSave = false;

	m_ShowPopup = NULL;
	m_HidePopup = NULL;
	m_ClearPopupResult = NULL;
	m_PopupResult = NULL;
	m_UsePopups = true;
	m_SearchMask = NULL;
	m_WaitUntilIdleUpdate = NULL;
	m_LoadSaveBuffer = NULL;
	m_PopupTime = 0.0f;
	m_RoundBuffer = 0;
	m_EncryptionType = 0;
	m_CompressionType = 0;
	m_StartupCheckDone = false;
	m_TempAutosave = false;
	m_CardPollingOn = false;
	m_NumCardsToPoll = 1;
	m_appVersion1 = 0;
	m_appVersion2 = 0;
	m_EncryptedSize = 0;
	m_CompressedSize = 0;
	m_UncompressedSize = 0;
	m_AutoSaveIcon = false;
	m_IgnoreErrors = false;
	m_OverwriteConfirm = true;
	m_AreYouReallySure = false;
	m_NoOkConfirmation = false;
	m_UseActualFileSize = false;
	m_PassedInSaveBuffer = NULL;
	m_CancelOperation = false;

	m_InProgress = CMEMORYCARD_NOTHING_IN_PROGRESS;

	for (int i = 0; i < MAX_MEMCARDS; i++)
		m_CardInserted[i] = false;

	memset(m_FilesOnCard, 0, sizeof(m_FilesOnCard));
	memset(&PopupInfo, 0, sizeof(PopupInfo));
}

//--------------------------------------------------------------//
CMemoryCard::~CMemoryCard()
{
}

//--------------------------------------------------------------//
bool CMemoryCard::Init(bool Compress, int CompressionType, bool Encrypt, int EncryptionType, int MaxSlots, 
							  int FixedSlotSize, char* TitleName, char* TitleID, int NumCardsToPoll, int RoundBuffer,
							  int	AppVersion1, int AppVersion2)
{
	m_ExitThread = false;
	m_MaxSlots = MaxSlots;
	m_FixedSlotSize = FixedSlotSize;
	strcpy(m_TitleName, TitleName);
	strcpy(m_DefaultFileName, TitleName);
	m_TitleID = TitleID;
	m_RoundBuffer = RoundBuffer;
	m_NumCardsToPoll = NumCardsToPoll;

	if (m_NumCardsToPoll > MAX_MEMCARDS)
		m_NumCardsToPoll = MAX_MEMCARDS;

	// set up compress and encrypt types
	m_Compress = Compress;
	m_Encrypt = Encrypt;
	m_CompressionType = CompressionType;
	m_EncryptionType = EncryptionType;

	// set up version ID's
	m_appVersion1 = AppVersion1;
	m_appVersion2 = AppVersion2;
	m_LoadedappVersion1 = 0;
	m_LoadedappVersion2 = 0;

	// setup active flags
	for (int i = 0; i < MAX_MEMCARDS; i++)
	{
		m_CardSlotsActive[i] = false;
		m_CardChanged[i] = false;
	}

	m_CardPollingOn = false;			

	// set up compress and encrypt types
	m_Compress = Compress;
	m_Encrypt = Encrypt;
	m_CompressionType = CompressionType;
	m_EncryptionType = EncryptionType;

	// start up the memory card thread
	m_MemCardThread = new CThread;

	if (!m_MemCardThread)
		return false;

	#if defined (WIN32)
	m_MemCardThread->Create( (int*)&MemCardThreadFunc );		// PC & XBox
	#elif defined (PS2)
	m_MemCardThread->Create( MemCardThreadFunc );				// PS2
	#endif

	// init callback for lowlevel updating
	g_MemCardMgr.RegisterUpdate( (void*)&CMemoryCard_AsyncUpdate );

	m_MemCardThread->Start();
	SetIgnoreErrors(false);

	// detect cards
	return true;
}

//--------------------------------------------------------------//
void CMemoryCard_AsyncUpdate(void)
{
	#if defined (PS2)
	g_MemCardManager->GetThread()->YieldThread(100);
	#else
	g_MemCardManager->GetThread()->YieldThread(1);
	#endif
}

//--------------------------------------------------------------//
void CMemoryCard::Update()
{
	/*if (m_MemCardThread)
		m_MemCardThread->Execute();*/
}

//--------------------------------------------------------------//
void CMemoryCard::DeInit(void)
{
	if (m_MemCardThread)
	{
		delete m_MemCardThread;
		m_MemCardThread = NULL;
	}
}

//--------------------------------------------------------------//
void CMemoryCard::DetectCard(int Port)
{
	ASSERTS(m_State == CMEMORYCARD_IDLE, "Memory card in progress already..");
	SetIgnoreErrors(false);
	SetPort(Port);
	SetState(CMEMORYCARD_DETECT);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::DetectCardWithResult(int Port)
{
	ASSERTS(m_State == CMEMORYCARD_IDLE, "Memory card in progress already..");
	SetIgnoreErrors(false);
	SetPort(Port);
	SetState(CMEMORYCARD_DETECT_WITH_RESULT);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::DetectCardOnStart(int Port)
{
	ASSERTS(m_State == CMEMORYCARD_IDLE, "Memory card in progress already..");
	SetIgnoreErrors(false);
	SetPort(Port);
	SetState(CMEMORYCARD_DETECT_ON_START);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::SaveFile(char *Filename, char* Comment, int Port, char* Buffer,
									int Filesize, bool AreYouSure, int SaveType )
{

	SetIgnoreErrors(false);

	// round up file size if necessary
	if (m_RoundBuffer)
	{
		Filesize += m_RoundBuffer - 1;
		Filesize /= m_RoundBuffer;
		Filesize *= m_RoundBuffer;
	}

#if defined(DONT_USE_POPUPS) //|| defined(DIRECTX_PC)
	SetUsePopups(false);
#endif

	if (GetAutosave() == false)
		SetMainState(CMEMORYCARD_STATE_SAVE);
	else
		SetMainState(CMEMORYCARD_STATE_AUTOSAVE);

	SetPopupExitResult(PR_COMPLETED_OK);

	// do we have a filename??
	#if defined (PS2)
	if (Comment[0] == 0)
		SetComment(m_DefaultFileName);
	else
		SetComment(Comment);
	#else
	SetComment(Comment);
	#endif

	SetFilename(Filename);
	SetPort(Port);
	SetFilesize(Filesize);
	SetAreYouSure(AreYouSure);
	SetSaveType(SaveType);
	SetState(CMEMORYCARD_SAVE);
	SetPassedInSaveBuffer(Buffer);

	CreateLoadSaveBuffer(Filesize);
	memcpy(m_LoadSaveBuffer, Buffer, Filesize);
	SetBuffer(m_LoadSaveBuffer);

	Update();
}

//--------------------------------------------------------------//	
void CMemoryCard::RecopyDataBuffer(void)
{
	memcpy(GetLoadSaveBuffer(), GetPassedInSaveBuffer(), GetFilesize());
}

//--------------------------------------------------------------//	
void CMemoryCard::StartupControllerCheck(int Port)
{
	SetIgnoreErrors(false);
	SetMainState(CMEMORYCARD_STATE_CONTROLLER_STARTUP);
	SetPopupExitResult(PR_COMPLETED_OK);
	SetPort(Port);
	SetState(CMEMORYCARD_CONTROLLER_STARTUP);
	Update();
}

//--------------------------------------------------------------//	
void CMemoryCard::StartupCheck( int Port, int Filesize, int SaveType )
{
	SetIgnoreErrors(false);
	SetMainState(CMEMORYCARD_STATE_STARTUP);
	SetPopupExitResult(PR_COMPLETED_OK);
	SetPort(Port);
	SetFilesize(Filesize);
	SetAreYouSure(true);
	SetAutosave(false);
	SetSaveType(SaveType);
	SetState(CMEMORYCARD_STARTUP);
	Update();
}

//--------------------------------------------------------------//	
// This will temporarily turn off autosave but remember its state
// This has to be called again to set things back to normal after
void CMemoryCard::TempDisableAutosave( bool Flag )
{
	if (Flag == true)
	{
		m_TempAutosave = GetAutosave();
		SetAutosave(false);
	}
	else
		SetAutosave(m_TempAutosave);
}

//--------------------------------------------------------------//	
void CMemoryCard::LoadFile(char *Filename, char* Comment, int Port, char* Buffer,
									int Filesize, bool AreYouSure, int SaveType )
{
	SetIgnoreErrors(false);

	// round up file size if necessary
	if (m_RoundBuffer)
	{
		Filesize += m_RoundBuffer - 1;
		Filesize /= m_RoundBuffer;
		Filesize *= m_RoundBuffer;
	}

	SetMainState(CMEMORYCARD_STATE_LOAD);
	SetPopupExitResult(PR_COMPLETED_OK);

	// do we have a filename??
	#if defined (PS2)
	if (Comment[0] == 0)
		SetComment(m_DefaultFileName);
	else
		SetComment(Comment);
	#else
	SetComment(Comment);
	#endif

	SetFilename(Filename);
	SetPort(Port);
	SetFilesize(Filesize);
	SetBuffer(Buffer);
	SetAreYouSure(AreYouSure);
	SetSaveType(SaveType);
	SetState(CMEMORYCARD_LOAD);
	CreateLoadSaveBuffer(Filesize);
	SetLoadHeader(false);
	Update();
}

//--------------------------------------------------------------//	
void CMemoryCard::LoadNetwork(int Port)
{
	SetIgnoreErrors(false);
	SetMainState(CMEMORYCARD_STATE_LOAD_NETWORK);
	SetPopupExitResult(PR_COMPLETED_OK);

	SetNumNetworkProfiles(0);
	SetPort(Port);
	SetSaveType(MC_SAVED_ACCOUNT);
	SetState(CMEMORYCARD_LOAD_NETWORK);
	SetLoadHeader(false);
	Update();
}

//--------------------------------------------------------------//	
void CMemoryCard::LoadHeader(char *Filename, char* Comment, int Port, char* Buffer,
									  bool AreYouSure, int SaveType )
{
	SetIgnoreErrors(false);
	SetMainState(CMEMORYCARD_STATE_LOAD);
	SetPopupExitResult(PR_COMPLETED_OK);

	// do we have a filename??
	#if defined (PS2)
	if (Comment[0] == 0)
		SetComment(m_DefaultFileName);
	else
		SetComment(Comment);
	#else
	SetComment(Comment);
	#endif

	SetFilename(Filename);
	SetPort(Port);
	SetFilesize(sizeof(ts_MemcardHeader));
	SetBuffer(Buffer);
	SetAreYouSure(AreYouSure);
	SetSaveType(SaveType);
	SetState(CMEMORYCARD_LOAD);
	CreateLoadSaveBuffer(MEMCARD_BLOCK_SIZE);
	SetLoadHeader(true);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::FileExists(char *Filename, char* Comment, int Port)
{
	SetIgnoreErrors(false);
	SetMainState(CMEMORYCARD_STATE_EXISTS);
	SetPopupExitResult(PR_COMPLETED_OK);
	SetFilename(Filename);
	SetComment(Comment);
	SetPort(Port);
	SetState(CMEMORYCARD_FILE_EXISTS);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::DeleteFile(char *Filename, char* Comment, int Port, int SaveType )
{
	SetIgnoreErrors(false);
	SetMainState(CMEMORYCARD_STATE_DELETE);
	SetPopupExitResult(PR_COMPLETED_OK);
	SetFilename(Filename);
	SetComment(Comment);
	SetPort(Port);
	SetSaveType(SaveType);
	SetState(CMEMORYCARD_DELETE);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::FormatCard( int Port )
{
	SetIgnoreErrors(false);
	SetMainState(CMEMORYCARD_STATE_FORMAT);
	SetPopupExitResult(PR_COMPLETED_OK);
	SetPort(Port);
	SetState(CMEMORYCARD_FORMAT);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::HandleResult()
{
	SetState(CMEMORYCARD_HANDLE_RESULT);
}

//--------------------------------------------------------------//
void CMemoryCard::GetFilesOnCard( int Port, char* Filename, char* SearchMask, bool IgnoreErrors )
{
	ASSERTS(m_State == CMEMORYCARD_IDLE, "Memory card in progress already..");

	// clear out list of files
	memset(m_FilesOnCard, 0, sizeof(m_FilesOnCard));
	SetMainState(CMEMORYCARD_STATE_DIR);
	SetPopupExitResult(PR_COMPLETED_OK);
	SetFilename(Filename);
	SetSearchMask(SearchMask);
	SetPort(Port);
	SetState(CMEMORYCARD_GET_FILES_ON_CARD);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::GetFilesOnAllCards(char* Filename, char* SearchMask,bool IgnoreErrors)
{
	ASSERTS(m_State == CMEMORYCARD_IDLE, "Memory card in progress already..");

	// clear out list of files
	memset(m_FilesOnCard, 0, sizeof(m_FilesOnCard));
	SetMainState(CMEMORYCARD_STATE_DIR);
	SetPopupExitResult(PR_COMPLETED_OK);
	SetFilename(Filename);
	SetSearchMask(SearchMask);
	SetPort(0);
	SetState(CMEMORYCARD_GET_FILES_ON_ALL_CARDS);
	Update();
}

//--------------------------------------------------------------//
void CMemoryCard::GetFileInfo(int FileNum, ts_CMemCardFiles &memCardFile )
{
	memcpy( &memCardFile, ( const void* )&( m_FilesOnCard[ FileNum ] ), sizeof( ts_CMemCardFiles ) );
}

//--------------------------------------------------------------//
void CMemoryCard::FillOutFileInfo(int FileNum, char* Filename, char* Directory,
											 int Port, int ControllerPort, char* MemCardName,
											 int SizeInBytes, ts_CMemCardDate Date, ts_CMemCardTime Time)
{
	strcpy(m_FilesOnCard[FileNum].Filename, Filename);
	strcpy(m_FilesOnCard[FileNum].Directory, Directory);
	strcpy(m_FilesOnCard[FileNum].MemcardName, MemCardName);
	m_FilesOnCard[FileNum].SizeInBytes = SizeInBytes;
	m_FilesOnCard[FileNum].Port = Port;
	m_FilesOnCard[FileNum].SizeInBlocks = g_MemCardMgr.BytesToBlocks(SizeInBytes);
	m_FilesOnCard[FileNum].ControllerPort = ControllerPort;
	m_FilesOnCard[FileNum].Date = Date;
	m_FilesOnCard[FileNum].Time = Time;

	#if defined (_XBOX)
	strcpy(m_FilesOnCard[FileNum].DisplayName, Filename);
	strcpy(m_FilesOnCard[FileNum].LoadFilename, Filename);
	#elif defined (PS2)
	// remove extensions from PS2 save
	strcpy(m_FilesOnCard[FileNum].DisplayName, Filename);
	m_FilesOnCard[FileNum].DisplayName[strlen(m_FilesOnCard[FileNum].DisplayName) - 4] = 0;
	strcpy(m_FilesOnCard[FileNum].LoadFilename, Filename);
	m_FilesOnCard[FileNum].LoadFilename[strlen(m_FilesOnCard[FileNum].LoadFilename) - 4] = 0;
	#else
	strcpy(m_FilesOnCard[FileNum].DisplayName, Directory);
	strcpy(m_FilesOnCard[FileNum].LoadFilename, Directory);
	#endif
};

//--------------------------------------------------------------//

void CMemoryCard::RegisterPopupFunction( int FunctionType, void* Function )
{
	switch( FunctionType )
	{
	case SHOWPOPUP:
		m_ShowPopup = (CMemCard_VoidFunc*)Function;
		break;
	case HIDEPOPUP:
		m_HidePopup = (CMemCard_VoidFunc*)Function;
		break;
	case POPUPRESULT:
		m_PopupResult = (CMemCard_IntFunc*)Function;
		break;
	case CLEARRESULT:
		m_ClearPopupResult = (CMemCard_VoidFunc*)Function;
		break;
	}

	return;
}

//--------------------------------------------------------------//
void CMemoryCard::SetPopupDisplayTime ( float time )
{
	m_PopupTime = time;
}

//--------------------------------------------------------------//
float CMemoryCard::GetPopupDisplayTime (  )
{
	return (m_PopupTime);
}

//--------------------------------------------------------------//
void CMemoryCard::ResetPopupDisplayTime ()
{
	m_PopupTime = 0.0f;
}

//--------------------------------------------------------------//
void CMemoryCard::SetPopupTitle(char* Title)
{
	strcpy (m_PopupTitle, Title);
}

//--------------------------------------------------------------//
void CMemoryCard::DisplayPopup( int MsgIndex, int PassThruChoice, int Default, int Choice1, int Choice2, int Choice3, int Choice4)
{
	memset(&PopupInfo, 0, sizeof(PopupInfo));
	PopupInfo.NumChoices = 0;

	m_CorrectChoice = PassThruChoice;
	PopupInfo.PassThrough = PassThruChoice;

	if (m_ClearPopupResult)
		m_ClearPopupResult();
	// setup choice strings
	if (Choice1 != -1)
	{
		PopupInfo.Choices[PopupInfo.NumChoices].ChoiceText = MC_Choices[Choice1];
		PopupInfo.Choices[PopupInfo.NumChoices].ChoiceResult = Choice1;
		PopupInfo.NumChoices++;
	}
	if (Choice2 != -1)
	{
		PopupInfo.Choices[PopupInfo.NumChoices].ChoiceText = MC_Choices[Choice2];
		PopupInfo.Choices[PopupInfo.NumChoices].ChoiceResult = Choice2;
		PopupInfo.NumChoices++;
	}
	if (Choice3 != -1)
	{
		PopupInfo.Choices[PopupInfo.NumChoices].ChoiceText = MC_Choices[Choice3];
		PopupInfo.Choices[PopupInfo.NumChoices].ChoiceResult = Choice3;
		PopupInfo.NumChoices++;
	}
	if (Choice4 != -1)
	{
		PopupInfo.Choices[PopupInfo.NumChoices].ChoiceText = MC_Choices[Choice4];
		PopupInfo.Choices[PopupInfo.NumChoices].ChoiceResult = Choice4;
		PopupInfo.NumChoices++;
	}

	// create popup string
	int	NumParams = 0;
	int	Params[4];
	char	SubString[4][12] = { 0, 0, 0, 0 };

	if (CMemCardMsg[MsgIndex].SubString1 != -1)
	{
		Params[NumParams] = CMemCardMsg[MsgIndex].SubString1;
		NumParams++;
	}
	if (CMemCardMsg[MsgIndex].SubString2 != -1)
	{
		Params[NumParams] = CMemCardMsg[MsgIndex].SubString2;
		NumParams++;
	}
	if (CMemCardMsg[MsgIndex].SubString3 != -1)
	{
		Params[NumParams] = CMemCardMsg[MsgIndex].SubString3;
		NumParams++;
	}

	if (CMemCardMsg[MsgIndex].SubString4 != -1)
	{
		Params[NumParams] = CMemCardMsg[MsgIndex].SubString4;
		NumParams++;
	}

	for (int i = 0; i < NumParams; i++)
	{
		switch (Params[i])
		{
		case	CMEMORYCARD_TITLE_NAME:
			strcpy (SubString[i],GetTitleName());
			break;

		case	CMEMORYCARD_SAVE_TYPE:
			strcpy (SubString[i],MC_SaveTypes[GetSaveType()]);
			break;

		case	CMEMORYCARD_MEMCARD_NAME:
			strcpy (SubString[i],GetName());
			break;

		case	CMEMORYCARD_MEMCARD_PORT:
			sprintf (SubString[i], "%d", (GetPort() + 1));
			break;

		case	CMEMORYCARD_CONTROL_PORT:
			sprintf (SubString[i], "%d", (GetControllerPort() + 1));
			break;

		case	CMEMORYCARD_BLOCKS_FREE:
			u32	files;
			u32	blocks;
			int	Free;
			Free = g_MemCardMgr.GetCardFreeSpace(GetPort(), files, blocks);
			sprintf (SubString[i], "%d", Free );
			break;

		case	CMEMORYCARD_BLOCKS_NEEDED:
		{
			int	Needed;

		#if defined (PS2)
			Needed = g_MemCardMgr.GetBlocksForFileSize( GetFilesize() );
		#elif defined (_XBOX)
			u32	FreeFiles;
			u32	FreeBlocks;

			Needed = g_MemCardMgr.BytesToBlocks(GetFilesize()) + 3 + 1;  // for icons etc		+1 is a hack because MS cant figure out why Im having
																							 // problems, it require +1 sometimes
			g_MemCardMgr.GetCardFreeSpace( GetPort(), FreeFiles, FreeBlocks);
			
			Needed = Needed - FreeBlocks;

		#else
			Needed = g_MemCardMgr.BytesToBlocks(GetFilesize());
		#endif
			sprintf (SubString[i], "%d", Needed );
			break;
		}

		case	CMEMORYCARD_BLOCKS_ALL:
			break;
		}
	}

	// create master string
	char	PopupString[256];
	sprintf( PopupString, g_translator.Translate( CMemCardMsg[MsgIndex].String ),
		                   g_translator.Translate( SubString[0] ),
								 g_translator.Translate( SubString[1] ),
								 g_translator.Translate( SubString[2] ),
 								 g_translator.Translate( SubString[3] ) );



	strcpy( PopupInfo.String, PopupString );
	PopupInfo.Result = MC_NONE;
	PopupInfo.Default = Default;
	strcpy(PopupInfo.Title, m_PopupTitle);

	// setup popup if we are using them
	m_PopupRequest = true;

	// do we have a time to display??
	if (m_PopupTime != 0.0f)
	{
		while (g_MemCardManager->GetPopupRequest() != false)
		{
			#if defined (WIN32)
			g_MemCardManager->GetThread()->YieldThread(1);
			#else
			g_MemCardManager->GetThread()->YieldThread(10);
			#endif
		}

		// wait in here for a bit
		while(m_PopupTime > 0.0f )
		{
			#if defined (WIN32) && !defined (_XBOX)
			g_MemCardManager->GetThread()->YieldThread(1);
			#elif defined (_XBOX)
			g_MemCardManager->GetThread()->YieldThread(10);
			#else
			g_MemCardManager->GetThread()->YieldThread(10);
			#endif
			m_PopupTime -= g_timer.GetOSFrameSec( );
		}
		ResetPopupDisplayTime();
	}
	else		// just make sure the request gets thru..
	{
		#if defined (WIN32)
		g_MemCardManager->GetThread()->YieldThread(1);
		#else
		g_MemCardManager->GetThread()->YieldThread(10);
		#endif
	}
}

//--------------------------------------------------------------//
void CMemoryCard::RemovePopup(void)
{
	m_PopupClearRequest = true;

	while (g_MemCardManager->GetPopupClearRequest() != false)
	{
			#if defined (WIN32)
			g_MemCardManager->GetThread()->YieldThread(1);
			#else
			g_MemCardManager->GetThread()->YieldThread(10);
			#endif
	}
}

//--------------------------------------------------------------//
int CMemoryCard::PopupResult(void)
{
	if (m_PopupResult)
	{
		int result = m_PopupResult();

		if (result == 0xFF)
			return (m_CorrectChoice);
		else
			return(result);
	}
	else
	{
		#if !defined(CDROM)

		#if !defined (PS2)
		dbgPrint("Popup Result Polled\n");
		dbgPrint(Printf("Popup Result = %s\n", MC_Choices[MC_YES]));
		#endif

		#endif
	}

	return (m_CorrectChoice);
}

//--------------------------------------------------------------//
void CMemoryCard::RegisterWaitUntilIdleUpdate(void* Function)
{
	m_WaitUntilIdleUpdate = (CMemCard_VoidFunc*)Function;
}

//--------------------------------------------------------------//
void CMemoryCard::WaitUntilIdle()
{
	while (GetState() != CMEMORYCARD_IDLE)
	{
		// update thread
		Update();

		// update our user defined function if it exists
		if (m_WaitUntilIdleUpdate)
			m_WaitUntilIdleUpdate();

		// update engine/render
		if (!LoadScreen::IsRunning()
#if !defined(DISABLE_ASSERTS)
			  && !Assert::InAssert()
#endif
		)
		{
			g_timer.NewFrame();

#ifdef DIRECTX_PC
			GameEngine__Advance(true);
#else
			GameEngine__Advance();
#endif
			Sprite::DrawAll();
			RenderMgr__Render();
			g_audio.Daemon();
		}
	}
	ResetLoadSaveBuffer();
}

//--------------------------------------------------------------//
void CMemoryCard::CreateLoadSaveBuffer(int Filesize)
{
	// add in size of header
	ASSERT(!m_LoadSaveBuffer);
	m_LoadSaveBuffer = g_MemCardMgr.CreateLoadSaveBuffer(Filesize + sizeof(ts_MemcardHeader) + g_helix.GetEncryptionHeaderSize());
	if (!m_LoadSaveBuffer)
		ASSERTS(0, "Load/Save Buffer alloc failed");
}

//--------------------------------------------------------------//
void CMemoryCard::ResetLoadSaveBuffer()
{
	if (m_LoadSaveBuffer)
	{
		g_MemCardMgr.DestroyLoadSaveBuffer(m_LoadSaveBuffer);
		m_LoadSaveBuffer = NULL;
	}
}

//--------------------------------------------------------------//
void CMemoryCard::CreateEncryptBuffer(int Filesize)
{
	int ClusterSize = MEMCARD_CLUSTER_SIZE;
	int Blocks = (Filesize + (ClusterSize - 1)) / ClusterSize;
	int encryptsize = Blocks * MEMCARD_CLUSTER_SIZE;

	m_EncryptBuffer = new char[encryptsize];
	if (!m_EncryptBuffer)
		ASSERTS(0, "Encrypt Buffer alloc failed");
}

//--------------------------------------------------------------//
void CMemoryCard::ResetEncryptBuffer()
{
	if (m_EncryptBuffer)
	{
		delete m_EncryptBuffer;
		m_EncryptBuffer = NULL;
	}
}

//--------------------------------------------------------------//

//S32 COMPRESS_CompressBuffer( U8* srcData, S32 srcSize, U8* compBuffer, S32 compBufferSize );
//U8* COMPRESS_DecompressBuffer( U8* compBuffer, U8 *decompBuffer );
//BOOL COMPRESS_IsCompressedFile( U8* compBuffer );
//S32 COMPRESS_GetUncompressedSize( U8* compBuffer );

bool CMemoryCard::CompressData(int Type)			// todo
{
	s32	CompSize;

	switch (Type)
	{
		case	MC_COMPRESS_0:
			CreateEncryptBuffer(GetFilesize());
			CompSize = COMPRESS_CompressBuffer (  (U8*)g_MemCardManager->GetBuffer(), GetFilesize(),
															  (U8*)m_EncryptBuffer,GetFilesize());

			m_CompressedSize = CompSize;
			memcpy( g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), m_EncryptBuffer, GetFilesize());

			m_CompressedSize   = CompSize;
			m_UncompressedSize = GetFilesize();

			ResetEncryptBuffer();
			break;

		default:
			break;
	}

	return true;
}

//--------------------------------------------------------------//
bool CMemoryCard::UncompressData(int Type)		// todo
{
	s32	UncompSize;
	BOOL	IsComp;

	switch (Type)
	{
		case	MC_COMPRESS_0:
			 IsComp = COMPRESS_IsCompressedFile ((U8*) (g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader)));

			if (IsComp == false)
				return	false;

			UncompSize = COMPRESS_GetUncompressedSize ((U8*)(g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader)));
			CreateEncryptBuffer(UncompSize);
			COMPRESS_DecompressBuffer((U8*)g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), (U8*)m_EncryptBuffer );
			memcpy(g_MemCardManager->GetBuffer(), m_EncryptBuffer, UncompSize);
			ResetEncryptBuffer();
			break;

		default:
			break;
	}

	return true;
}

//--------------------------------------------------------------//
bool CMemoryCard::EncryptData(int Type)
{
	int	encryptsize = 0;
	char*	Buff;

	if (g_MemCardManager->GetCompress())
	{
		Buff = g_MemCardManager->GetLoadSaveBuffer();
		encryptsize = g_helix.GetEncryptionSize((BYTE*)Buff + sizeof(ts_MemcardHeader), m_CompressedSize);
		CreateEncryptBuffer(encryptsize);
	}
	else
	{
		Buff =  g_MemCardManager->GetBuffer();
		encryptsize = g_helix.GetEncryptionSize((BYTE*)Buff, GetFilesize());
		CreateEncryptBuffer(encryptsize);
	}

	switch(Type)
	{
		case	MC_ENCRYPT_HELIX:
		{
			if (g_MemCardManager->GetCompress())
			{
				g_helix.Encrypt( (BYTE*)(Buff + sizeof(ts_MemcardHeader)), m_CompressedSize, (BYTE*) m_EncryptBuffer);
				memcpy( g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), m_EncryptBuffer,  m_CompressedSize);
			}
			else
			{
				g_helix.Encrypt( (BYTE*)Buff, GetFilesize(), (BYTE*) m_EncryptBuffer);
				memcpy( g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), m_EncryptBuffer, encryptsize);
			}

			m_EncryptedSize    = encryptsize;
			m_UncompressedSize = GetFilesize();
			ResetEncryptBuffer();
			break;
		}
		default:
		{
			ResetEncryptBuffer();
			break;
		}
	}

	return true;
}

//--------------------------------------------------------------//
bool CMemoryCard::UnencryptData(int Type)
{
	int					encryptsize = 0;
	int					fullsize = 0;

	switch(Type)
	{
		case	MC_ENCRYPT_HELIX:
		{
			ts_MemcardHeader* MemHdr = (ts_MemcardHeader*)g_MemCardManager->GetLoadSaveBuffer();
			encryptsize = MemHdr->encryptedSize;
			fullsize    = MemHdr->uncompressedSize;

			CreateEncryptBuffer(fullsize);
			bool result = g_helix.Decrypt((BYTE*)g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), encryptsize, (BYTE*)m_EncryptBuffer);

			if (result == false)
			{
				ResetEncryptBuffer();
				return false;
			}

			if (g_MemCardManager->GetCompress())
				memcpy(g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), m_EncryptBuffer, fullsize);
			else
				memcpy(g_MemCardManager->GetBuffer(), m_EncryptBuffer, fullsize);
			ResetEncryptBuffer();
		}

		default:
			break;
	}

	return true;
}
