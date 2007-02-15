//--------------------------------------------------------------//
// MemoryCard_PS2.cpp
//
// Memory Card Handler (platform specific)
// Author: Paul Blagay
//
//--------------------------------------------------------------//

#include "platform/PlatformPCH.h"

#if defined (PS2)

#include <libmc.h>

#include "platform/MemoryCard/MemoryCard.h"
#include "platform/MemoryCard/MemoryCard_PS2.h"
#include "Layers/thread/thread.h"
#include "EngineHelper/Hash.h"
#include "GameHelper/encryption/Helix/Helix.h"

// memcard text files language specific
#include "memcard_english_ps2.h"
#include "Memcard_english_common.h"

#include "wantpal.h"

//JJS#include <netcnf.h>



//--------------------------------------------------------------//
// message formatting

ts_CMemCardMsg	CMemCardMsg[] = {
	{	MC_NO_VALID_SAVE_GAME,	CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_SAVE_TYPE,		-1,							-1 },
	{	MC_MEMCARD_FULL,			CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_BLOCKS_NEEDED, -1,							-1 },
	{	MC_MEMCARD_FULL_MULTIPLE, CMEMORYCARD_MEMCARD_PORT,CMEMORYCARD_BLOCKS_NEEDED,	CMEMORYCARD_BLOCKS_ALL, -1 },
	{	MC_NO_CARD_SAVE,			CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_BLOCKS_NEEDED,	-1,							-1 },								
	{	MC_NO_CARD_LOAD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_AUTOSAVE_NO_CARD,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_SURE_LOAD,				CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_SURE_SAVE,				CMEMORYCARD_SAVE_TYPE,		CMEMORYCARD_MEMCARD_PORT,	-1, 							-1 },
	{	MC_SURE_DELETE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_SURE_OVERWRITE,		CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_LOADING_FILE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_SAVING_FILE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_OVERWRITE_FILE,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },
	{	MC_DELETE_FILE	,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_LOADING_CANCELLED,	-1,								-1,								-1, 							-1 },
	{	MC_SAVING_CANCELLED,		-1,								-1,								-1, 							-1 },
	{	MC_DELETE_CANCELLED,		-1,								-1,								-1, 							-1 },
	{	MC_FORMAT_CANCELLED,		-1,								-1,								-1, 							-1 },								
	{	MC_FORMATTING,				CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_SURE_FORMAT,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_ACCESSING_CARD,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },
	{	MC_DETECT_CARD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_OTHER_MARKET,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_UNFORMATTED_CARD,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_DAMAGED_CARD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_CORRUPT_CARD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_COMPLETED_OK,			-1,								-1,								-1, 							-1 },
	{	NULL,							-1,								-1,								-1, 							-1 },			// command failed
	{	MC_FORMAT_FAILED,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },		
	{	MC_AUTOSAVE_NO_CARD,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },			
	{	MC_UNFORMATTED_CARD_OK,	CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },		
	{	MC_FORMAT_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_DELETE_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_LOAD_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_SAVE_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_CARD_CHANGED,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },			// card changed since last save
	{  MC_NETWORK_CONFIG_FAIL, CMEMORYCARD_MEMCARD_PORT,  -1,								-1, 							-1 },
	{  MC_NETWORK_MAGIC_FAIL,  CMEMORYCARD_MEMCARD_PORT,  -1,								-1, 							-1 },
	{  MC_NETWORK_CONFIG_BAD,  CMEMORYCARD_MEMCARD_PORT,  -1,								-1, 							-1 },
	{  MC_AUTOSAVING,				-1,								-1,								-1, 							-1 },			// autosaving
	{  MC_SAVE_FAILED,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },			// failed.
	{  MC_MEMCARD_FULL_STARTUP_PAL,CMEMORYCARD_MEMCARD_PORT, CMEMORYCARD_BLOCKS_NEEDED,-1, 						-1 },			// PAL full message
	{  MC_MEMCARD_FULL_FILES,	CMEMORYCARD_TITLE_NAME,		CMEMORYCARD_SAVE_TYPE,		-1, 							-1 },			// max files reached
	{  MC_MEMCARD_CONTINUE_SURE, CMEMORYCARD_MEMCARD_PORT,-1,								-1, 							-1 },			// sure continue no save?
	{	MC_MEMCARD_FULL_STARTUP, CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_BLOCKS_NEEDED, -1,							-1 },			// startup new wording
	{  MC_OVERWRITE_FAILED,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },			// overwrite failed.
	{	MC_SURE_OVERWRITE,		CMEMORYCARD_SAVE_TYPE,		CMEMORYCARD_MEMCARD_PORT,	-1, 							-1 },
	{	MC_OVERWRITE_FILE,		CMEMORYCARD_MEMCARD_PORT,								-1,								-1, 							-1 },
	{  MC_MEMCARD_CONTINUE_SURE_NOCARD, CMEMORYCARD_MEMCARD_PORT,CMEMORYCARD_BLOCKS_NEEDED,-1, 							-1 },			// sure continue no save?
	{	MC_OVERWRITE_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_NO_CARD_STARTUP_1,	CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_BLOCKS_NEEDED,	-1,							-1 },								
	{	MC_NO_CARD_STARTUP_2,	-1,								-1,									-1,							-1 },								
	{	MC_BAD_CONTROLLER,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },			// controller isnt supported
	{	MC_ARE_YOU_SURE,			-1,								-1,								-1, 							-1 },			// double confirm

};

static	int	PollCount = 0;
static	bool	CardsActive[MAX_MEMCARDS] = {false};
static	bool	FormatDone = false;
static	bool	OverWrite = false;


//--------------------------------------------------------------//

void	MemCardThreadFunc(void* data)
{
	MEMCARD_RESULT		result;
	MEMCARD_RESULT		last_result[MAX_MEMCARDS];
	int					i;

	g_MemCardManager->GetThread()->YieldThread(100);

	ASSERTS (g_MemCardManager->GetThread(), "MemCard thread is not ready.");

	// get latest active counts
	for (i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
	{
		g_MemCardManager->SetPort(i);
		result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
		if (result == MEMCARD_RESULT_OK)
			g_MemCardManager->SetCardActive(i, true);
		else
			g_MemCardManager->SetCardActive(i, false);
	}

	// set last states
	for (i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
		CardsActive[i] = g_MemCardManager->GetCardActive(i);

	// set inital vals for inserted...
	for (i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
	{
		last_result[i] = MEMCARD_RESULT_OK;
		g_MemCardManager->SetPort(i);
		CMemoryCard_PS2::Detect();
		if (g_MemCardManager->GetResult() == MEMCARD_RESULT_OK)
			g_MemCardManager->SetCardInserted(i, true);
	}

	// set SCEE strings if necessary....
	if (!g_regionUSA)
	{
		CMemCardMsg[CMEMCARDMSG_MEMCARD_FULL].String				= MC_MEMCARD_FULL_PAL;
		CMemCardMsg[CMEMCARDMSG_MEMCARD_FULL_MULTIPLE].String = MC_MEMCARD_FULL_MULTIPLE_PAL;
		CMemCardMsg[CMEMCARDMSG_FORMATTING].String				= MC_FORMATTING_PAL;
		CMemCardMsg[CMEMCARDMSG_NETWORK_CONFIG_FAIL].String	= MC_NETWORK_CONFIG_FAIL_PAL;

	}

	while (1)
	{
		switch (g_MemCardManager->GetState())
		{
		case	g_MemCardManager->CMEMORYCARD_IDLE:
			if (FormatDone)
				FormatDone = false;
			if (OverWrite)
				OverWrite = false;
			break;
		case	g_MemCardManager->CMEMORYCARD_DETECT:
			CMemoryCard_PS2::Detect();
			break;
		case	g_MemCardManager->CMEMORYCARD_DETECT_WITH_RESULT:
			CMemoryCard_PS2::DetectWithResult();
			break;
		case	g_MemCardManager->CMEMORYCARD_DETECT_ON_START:
			CMemoryCard_PS2::DetectOnStart();
			break;
		case	g_MemCardManager->CMEMORYCARD_LOAD:
			CMemoryCard_PS2::Load();
			break;
		case	g_MemCardManager->CMEMORYCARD_SAVE:
			CMemoryCard_PS2::Save();
			break;
		case	g_MemCardManager->CMEMORYCARD_DELETE:
			CMemoryCard_PS2::Delete();
			break;
		case	g_MemCardManager->CMEMORYCARD_FORMAT:
			CMemoryCard_PS2::Format();
			break;
		case	g_MemCardManager->CMEMORYCARD_FILE_EXISTS:
			CMemoryCard_PS2::Exists();
			break;
		case	g_MemCardManager->CMEMORYCARD_GET_FILES_ON_CARD:
			CMemoryCard_PS2::GetFilesOnCard();
			break;
		case	g_MemCardManager->CMEMORYCARD_GET_FILES_ON_ALL_CARDS:
			CMemoryCard_PS2::GetFilesOnAllCards();
			break;
		case	g_MemCardManager->CMEMORYCARD_HANDLE_RESULT:
			CMemoryCard_PS2::HandleResult();
			break;
		case	g_MemCardManager->CMEMORYCARD_STARTUP:
			CMemoryCard_PS2::StartupCheck();
			break;
		case	g_MemCardManager->CMEMORYCARD_LOAD_NETWORK:
			CMemoryCard_PS2::LoadNetwork();
			break;
		case	g_MemCardManager->CMEMORYCARD_CONTROLLER_STARTUP:
			CMemoryCard_PS2::StartupControllerCheck();
			break;
		default:
			break;
		}

		// sleep a while
		g_MemCardManager->GetThread()->YieldThread(200);

		// check for change of card, and set changed flag
		for (i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
		{
			result = g_MemCardMgr.Detect(i);

			if (result == MEMCARD_RESULT_OK)
			{
				if (last_result[i] == MEMCARD_RESULT_NOCARD)
				{
//pab					if (g_MemCardManager->GetCardChanged(i) == false)
					{
						g_MemCardManager->SetCardChanged(i, true);
						g_MemCardManager->SetCardInserted(i, true);
					}
				}
				last_result[i] = MEMCARD_RESULT_OK;
			}
			else if (result == MEMCARD_RESULT_NOCARD)
			{
				if (last_result[i] == MEMCARD_RESULT_OK)
				{
//pab					if (g_MemCardManager->GetCardChanged(i) == false)
					{
						g_MemCardManager->SetCardChanged(i, true);
						g_MemCardManager->SetCardInserted(i, false);
					}
				}
				last_result[i] = MEMCARD_RESULT_NOCARD;
			}
		}

		// exit thread?
		if (g_MemCardManager->ExitThread())
			break;
	}

	// kill thread
	g_MemCardManager->GetThread()->ExitDelete();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::StartupControllerCheck()
{
	ControllerInputDriver* pDriver = NULL;

	pDriver = g_input.GetController(g_MemCardManager->GetPort());

	// no device.. 
	if (pDriver->DeviceName() == NULL)
	{
				// we dont have a Dualshock - display message.
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_BAD_CONTROLLER, MC_OK, MC_OK, MC_OK);
		WaitOnResult();

		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
		HandleResult();
		return;
	}

	// we have something.. make sure its Dualshock 2
	if (strcmp (pDriver->DeviceName(), "Dualshock 2")) 
	{
		// we dont have a Dualshock - display message.
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_BAD_CONTROLLER, MC_OK, MC_OK, MC_OK);
		WaitOnResult();

		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
		HandleResult();
		return;
	}
	else
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
		HandleResult();
		return;
	}

}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::StartupCheck()
{
	u32					BlocksNeeded;
	MEMCARD_RESULT		result;

	OverWrite = false;
	g_MemCardManager->SetNoSave(false);

	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	// if unformatted ignore startup check
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());

	if (result == MEMCARD_RESULT_UNFORMATTED)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
		HandleResult();
		return;
	}

	// no card inserted (or unformatted according to Eidos TCR)
	if (result == MEMCARD_RESULT_NOCARD || result == MEMCARD_RESULT_WRONGTYPE)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_CARD_STARTUP_1,  MC_YES, MC_NO, MC_YES, MC_NO);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_YES)
		{
			if (g_MemCardManager->PopupResult() == MC_YES)
			{
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab				g_MemCardManager->SetNoSave(true);
				HandleResult();
				return;
			}
			else
			{
				g_MemCardManager->RemovePopup();
				return;
			}
		}
		else		// try startup check again
		{
			g_MemCardManager->RemovePopup();
			return;
		}
	}

	// check space
	BlocksNeeded = g_MemCardMgr.GetBlocksForFileSize( g_MemCardManager->GetFilesize());
	result = g_MemCardMgr.CheckSpace( g_MemCardManager->GetPort(), g_MemCardManager->GetFilesize() , BlocksNeeded);

	g_MemCardManager->SetPopupTitle("");

	// if not enough space, put up a message about continuing without save etc..
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL_STARTUP, MC_NO, MC_NO, MC_YES, MC_NO);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_YES)
		{
			g_MemCardManager->RemovePopup();
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_CONTINUE_SURE, MC_YES, MC_NO, MC_YES, MC_NO);
			WaitOnResult();
			
			if (g_MemCardManager->PopupResult() == MC_YES)
			{
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab				g_MemCardManager->SetNoSave(true);
				HandleResult();
				return;
			}
			else
			{
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				HandleResult();
				return;
			}
		}
		else		// try startup check again
		{
			g_MemCardManager->RemovePopup();
			return;
		}
	}
	else
	{
		g_MemCardManager->SetNoSave(false);
		g_MemCardManager->SetResult(result);
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
	}
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::PollCards()
{
	int					i;
	MEMCARD_RESULT		result;

	// get latest active counts
	for (i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
	{
		g_MemCardManager->SetPort(i);
		result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
		if (result == MEMCARD_RESULT_OK)
			g_MemCardManager->SetCardActive(i, true);
		else
			g_MemCardManager->SetCardActive(i, false);
	}

	// check against previous
	for (i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
	{
		if (g_MemCardManager->GetCardActive(i) != CardsActive[i])
		{
			g_MemCardManager->SetCardChanged(g_MemCardManager->GetPort(), true);
			break;
		}
	}

	// copy over to last
	for (i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
		CardsActive[i] = g_MemCardManager->GetCardActive(i);

	// make sure it never overflows.. not sure if necessary but might as well
	if (PollCount > 100000)
		PollCount = 0;
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::Load()
{
	MEMCARD_RESULT	result;

	OverWrite = false;

	g_MemCardManager->SetPopupTitle("Load");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// display popup
	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_LOADING_FILE, MC_NONE);

	// check for icon.sys.. Sony QA tool removes this to damage a save game
	g_MemCardMgr.SetFileName("icon.sys");
	result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());

	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
		HandleResult();
		return;
	}

	// set filename
	char ProfName[48];
	sprintf(ProfName, "%s%s", g_MemCardManager->GetComment(), g_MemCardManager->GetExt());
	g_MemCardMgr.SetFileName(ProfName);

	// are you sure
	if (g_MemCardManager->GetAreYouSure() == true)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_LOAD, MC_YES, MC_NO, MC_YES, MC_NO);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_NO)
		{
			g_MemCardManager->RemovePopup();
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_LOADING_CANCELLED, MC_OK, MC_OK, MC_OK);
			WaitOnResult();
			
			g_MemCardManager->SetPopupExitResult(PR_LOAD_NO);
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			HandleResult();
			return;
		}
		else
			g_MemCardManager->RemovePopup();

		// detect card
		result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
		if (result != MEMCARD_RESULT_OK)
		{
			g_MemCardManager->SetResult(result);
			HandleResult();
			return;
		}
	}

	// display popup
	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_LOADING_FILE, MC_NONE);

	// start load
	int	Type = READ_ALL;
	int	Size = g_MemCardManager->GetFilesize() + sizeof(ts_MemcardHeader) + g_helix.GetEncryptionHeaderSize();

	if (g_MemCardManager->GetLoadHeader() == true)
	{
		Type = HEADER_READ_ONLY;
		Size = 1024;
	}

	result = g_MemCardMgr.Load( g_MemCardManager->GetPort(), g_MemCardManager->GetLoadSaveBuffer(), Size, Type);
	g_MemCardManager->RemovePopup();

	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// is our header valid??
	ts_MemcardHeader*	hdr = (ts_MemcardHeader*) g_MemCardManager->GetLoadSaveBuffer();
	if (hdr->version != MEMCARD_VERSION)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
		HandleResult();
		return;
	}

	// check checksum
	int	Checksum = Hash::CalcChecksum( (g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader)),hdr->checksumsize);
	if (hdr->checksum != Checksum && g_MemCardManager->GetLoadHeader() == false)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
		HandleResult();
		return;
	}

	g_MemCardManager->SetInfoString(hdr->InfoString);
	g_MemCardManager->SetDateString(hdr->DateString);

	// uncompress data?
	if (g_MemCardManager->GetLoadHeader() == false)
	{
		if (g_MemCardManager->GetCompress() == true)
		{
			if (hdr->compressed)
			{
				bool result = g_MemCardManager->UncompressData(hdr->compression_type);
				if (result == false)
				{
					g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
					HandleResult();
					return;
				}
			}
			else
			{
				g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
				HandleResult();
				return;
			}
		}

		if (g_MemCardManager->GetEncrypt() == true)
		{
			if (hdr->encrypted)
			{
				bool result = g_MemCardManager->UnencryptData(hdr->encryption_type);
				if (result == false)
				{
					g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
					HandleResult();
					return;
				}
			}
			else
			{
				g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
				HandleResult();
				return;
			}
		}
	}

	if ( g_MemCardManager->GetCompress() == false && g_MemCardManager->GetEncrypt() == false) 
		memcpy(g_MemCardManager->GetBuffer(), g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), g_MemCardManager->GetFilesize());
	else if (g_MemCardManager->GetLoadHeader() == true)
		memcpy(g_MemCardManager->GetBuffer(), g_MemCardManager->GetLoadSaveBuffer(), g_MemCardManager->GetFilesize());

	// set the versions we loaded...
	g_MemCardManager->SetLoadedAppVersion1(hdr->appVersion1);
	g_MemCardManager->SetLoadedAppVersion2(hdr->appVersion2);

	g_MemCardManager->SetResult(result);
//		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::LoadNetwork()
{
	MEMCARD_RESULT	result;
	int				SN_result;

	OverWrite = false;

	g_MemCardManager->SetPopupTitle("Load");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// display popup
	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_ACCESSING_CARD, MC_NONE);

	// start load
	SN_result = PS2Networking::ReadProfileListFromMC( g_MemCardManager->GetNetworkProfileList(), 0 );

	if (SN_result > 0)
	{
		SN_result = g_MemCardManager->GetNetworkProfileList()->GetNbProfiles();
		g_MemCardManager->SetNumNetworkProfiles(SN_result);
		g_MemCardManager->SetResult(MEMCARD_RESULT_OK);
		HandleResult();
		return;
	}
	else
	{
		g_MemCardManager->SetNumNetworkProfiles(0);
		g_MemCardManager->SetErrorCode( SN_result );
		g_MemCardManager->SetResult( MEMCARD_RESULT_NETWORK_CONFIGURATION_FAILED );
		HandleResult();
		return;
	}
	g_MemCardManager->RemovePopup();
}

//--------------------------------------------------------------//
MEMCARD_RESULT CMemoryCard_PS2::WillSaveFit()
{
	// get size of file
	// check if baslus file is there.. if so, then we only need to calculate
	// actual save game size, since icon has been saved before.. on delete we
	// only delete the save game portion no icons etc.. because a patch might need
	// it for example
	u32 Size = 0;
	MEMCARD_RESULT result;

	result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask() );
	int NumFiles = g_MemCardMgr.GetNumFilesFound();

	// file is already there
	if (g_MemCardManager->GetUseActualFileSize())
	{
		Size = g_MemCardMgr.GetBlocksForActualFileSize( NumFiles, g_MemCardManager->GetFilesize() );
	}
	else if (NumFiles > 0)
	{
		Size = g_MemCardMgr.GetBlocksForActualFileSize( NumFiles, g_MemCardManager->GetFilesize() );
	}
	else
	{
		// do we have icon.sys, baslus, directory, etc.. if we do.. then we can just do the filesize..
		// this means someone deleted the game save file
		g_MemCardMgr.SetFileName(g_MemCardManager->GetTitleID());
		result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());

		if (result == MEMCARD_RESULT_OK)
			Size = g_MemCardMgr.GetBlocksForActualFileSize( NumFiles, g_MemCardManager->GetFilesize() );
		else
			Size = g_MemCardMgr.GetBlocksForFileSize( g_MemCardManager->GetFilesize() );
	}

	// do we have enough space on the device..
	result = g_MemCardMgr.CheckSpace( g_MemCardManager->GetPort(), Size * MEMCARD_BLOCK_SIZE, Size );

	return result;
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::Save()
{
	MEMCARD_RESULT	result;
	u32				BlocksNeeded;
	int				NumFiles;
	int				MaxFiles;
	int				temp;

	OverWrite = false;

	g_MemCardManager->SetPopupTitle("Save");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	// set filename
	char ProfName[48];
	sprintf(ProfName, "%s%s", g_MemCardManager->GetComment(), g_MemCardManager->GetExt());
	g_MemCardMgr.SetFileName(ProfName);

#if defined TURN_ON_CARD_CHANGE
	// did card change??
	if (g_MemCardManager->GetCardChanged(g_MemCardManager->GetPort()) == true && g_MemCardManager->GetAutosave() == true)
	{
		g_MemCardManager->SetCardChanged(g_MemCardManager->GetPort(), false);

		// if card is inserted, tell them it changed.. else we let it run through to detect, which 
		// will say there is no card..
		if (g_MemCardManager->GetCardInserted(g_MemCardManager->GetPort()))
		{
			// display message about changing card etc
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CARD_CHANGED, MC_NO, MC_NO, MC_YES, MC_NO);
			WaitOnResult();

			if (g_MemCardManager->PopupResult() == MC_NO)
			{
				g_MemCardManager->SetAutosave(false);
//pab				g_MemCardManager->SetNoSave(true);
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				g_MemCardManager->RemovePopup();
				HandleResult();
				return;
			}
			else
			{
				g_MemCardManager->SetAutosave(true);
				g_MemCardManager->SetNoSave(false);
			}
		}
	}
#endif

	// are you sure
	if (g_MemCardManager->GetAreYouSure() == true &&  FormatDone == false )
	{
		while (1)
		{
			g_MemCardManager->SetPopupTitle("Save");
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_SAVE, MC_YES, MC_YES, MC_YES, MC_NO);
			result = WaitOnResult();

			// bail out if we need to
			if (result == MEMCARD_RESULT_CANCEL_OPERATION)
			{
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
				g_MemCardManager->ResetLoadSaveBuffer();
				g_MemCardManager->ResetEncryptBuffer();
				HandleResult();
				return;
			}

			if (g_MemCardManager->PopupResult() == MC_NO)
			{
				// Are you really sure?????? Come on now!!!!!!!
				if (g_MemCardManager->GetAreYouReallySure() == true)
				{
					g_MemCardManager->SetPopupTitle("Abandon Save");
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_ARE_YOU_SURE, MC_YES, MC_NO, MC_YES, MC_NO);
					result = WaitOnResult();

					// bail out if we need to
					if (result == MEMCARD_RESULT_CANCEL_OPERATION)
					{
						g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
						g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
						g_MemCardManager->ResetLoadSaveBuffer();
						g_MemCardManager->ResetEncryptBuffer();
						HandleResult();
						return;
					}

					if (g_MemCardManager->PopupResult() == MC_YES)
					{
						g_MemCardManager->RemovePopup();
						g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVING_CANCELLED, MC_OK, MC_OK, MC_OK);
						WaitOnResult();

						g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
						g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
						HandleResult();
						return;
					}
					else
					{
						g_MemCardManager->RemovePopup();
					}
				}
				else
				{
					g_MemCardManager->RemovePopup();
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVING_CANCELLED, MC_OK, MC_OK, MC_OK);
					result = WaitOnResult();

					g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
					g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
					HandleResult();
					return;
				}
			}
			else
			{
				g_MemCardManager->RemovePopup();
				break;
			}
		}
	}

	g_MemCardManager->SetPopupTitle("Save");

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());

	// card in slot?
	if (result != MEMCARD_RESULT_OK)
	{
		if (result == MEMCARD_RESULT_NOCARD)
		{
			// autosave on?
			if (g_MemCardManager->GetAutosave() == true)
			{

				if (!strcmp(g_MemCardManager->GetExt(), ".pat"))
				{
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_AUTOSAVE_REMOVED, MC_CANCEL, MC_RETRY, MC_CANCEL, MC_RETRY);
					temp = MC_CANCEL;
				}
				else
				{
					// autosave needs to be turned off here if they say continue without saving
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_AUTOSAVE_REMOVED, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
					temp = MC_CONTINUE_WITHOUT_SAVING;
				}

				WaitOnResult();

				// continue without saving?
				if (g_MemCardManager->PopupResult() == temp)
				{
					g_MemCardManager->SetPopupExitResult(PR_NOCARD_CANCEL);
					g_MemCardManager->SetAutosave(false);
					g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
					g_MemCardManager->RemovePopup();
//pab					g_MemCardManager->SetNoSave(true);
					HandleResult();
					return;
				}
				else		// try to enable autosave
				{
					g_MemCardManager->RemovePopup();

					while (1)
					{
						result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());

						if (result == MEMCARD_RESULT_NOCARD)
						{
							g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_CARD_SAVE, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
							WaitOnResult();

							// cancel so dump out, turn off autosave
							if (g_MemCardManager->PopupResult() == MC_CANCEL)
							{
								g_MemCardManager->SetPopupExitResult(PR_NOCARD_CANCEL);
								g_MemCardManager->SetAutosave(false);
//pab								g_MemCardManager->SetNoSave(true);
								g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
								g_MemCardManager->RemovePopup();
								HandleResult();
								return;
							}
							else
								g_MemCardManager->RemovePopup();
						}
						else		// card was found, re-enable autosave and let it save from here.
						{
							if (result == MEMCARD_RESULT_OK)			// good to go
							{
								g_MemCardManager->SetAutosave(true);
								g_MemCardManager->SetNoSave(false);
								g_MemCardManager->RemovePopup();
								break;
							}	
							else												// fall thru and catch error
								g_MemCardManager->RemovePopup();
						}
					}
				}
			}
		}
	}

	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// do we have enough space?
	result = WillSaveFit();

	// reset filename
	g_MemCardMgr.SetFileName(ProfName);

	// not enough space
	if (result == MEMCARD_RESULT_NOSPACE)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL, MC_NO, MC_NO, MC_YES, MC_NO);
		result = WaitOnResult();

		// bail out if we need to
		if (result == MEMCARD_RESULT_CANCEL_OPERATION)
		{
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
			g_MemCardManager->ResetLoadSaveBuffer();
			g_MemCardManager->ResetEncryptBuffer();
			HandleResult();
			return;
		}

		if (g_MemCardManager->PopupResult() == MC_NO)
		{
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab			g_MemCardManager->SetNoSave(true);
			HandleResult();
			return;
		}
		else
		{			// try again homes
			g_MemCardManager->RemovePopup();
			return;
		}
	}

	// enough files in directory left to create a new file??
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask());
	NumFiles = g_MemCardMgr.GetNumFilesFound();

	if (g_MemCardManager->GetFirstSave() == true)
		MaxFiles = MC_MAX_FILES_DIR - 1;
	else
		MaxFiles = MC_MAX_FILES_DIR;

	MaxFiles -= MC_RESERVED_FILES;			// on PS2, 3 files are taken up with system related info (icon.sys, logo.sys, <dirname>)

	if (NumFiles >= MaxFiles)
	{
		// check to see if this file already exists, if so, carry on and let them overwrite..
		result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());

		if (result != MEMCARD_RESULT_OK)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL_FILES, MC_CONTINUE_WITHOUT_SAVING, MC_CHOOSE_EXISTING, MC_CHOOSE_EXISTING, MC_CONTINUE_WITHOUT_SAVING);
			result = WaitOnResult();

			// bail out if we need to
			if (result == MEMCARD_RESULT_CANCEL_OPERATION)
			{
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
				g_MemCardManager->ResetLoadSaveBuffer();
				g_MemCardManager->ResetEncryptBuffer();
				HandleResult();
				return;
			}

			if (g_MemCardManager->PopupResult() == MC_CONTINUE_WITHOUT_SAVING)
			{
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab				g_MemCardManager->SetNoSave(true);
			}
			else
				g_MemCardManager->SetPopupExitResult(PR_CHOOSE_EXISTING);

			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			HandleResult();
			return;
		}
	}

	if (g_MemCardManager->GetAutosave() == false)
	{
		// does file exists already
		result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());

		if ( g_MemCardManager->GetOverwriteConfirm() )
		{
			// overwrite?
			if (result == MEMCARD_RESULT_OK)
			{	
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_OVERWRITE, MC_YES, MC_NO, MC_YES, MC_NO);
				result = WaitOnResult();

				// bail out if we need to
				if (result == MEMCARD_RESULT_CANCEL_OPERATION)
				{
					g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
					g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
					g_MemCardManager->ResetLoadSaveBuffer();
					g_MemCardManager->ResetEncryptBuffer();
					HandleResult();
					return;
				}

				if (g_MemCardManager->PopupResult() == MC_NO)
				{
					g_MemCardManager->RemovePopup();
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVING_CANCELLED, MC_OK, MC_OK, MC_OK);
					WaitOnResult();

					g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
					g_MemCardManager->SetPopupExitResult(PR_OVERWRITE_NO);
					HandleResult();
					return;
				}
				else
				{
					OverWrite = true;
					g_MemCardManager->RemovePopup();
				}

				// detect card
				result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
				if (result != MEMCARD_RESULT_OK)
				{
					g_MemCardManager->SetResult(result);
					HandleResult();
					return;
				}

				// User may have swapped cards on us, recheck free space...

				// do we have enough space?
				result = WillSaveFit();

				// reset filename
				g_MemCardMgr.SetFileName(ProfName);

				// not enough space
				if (result == MEMCARD_RESULT_NOSPACE)
				{
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL, MC_NO, MC_NO, MC_YES, MC_NO);
					result = WaitOnResult();

					// bail out if we need to
					if (result == MEMCARD_RESULT_CANCEL_OPERATION)
					{
						g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
						g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
						g_MemCardManager->ResetLoadSaveBuffer();
						g_MemCardManager->ResetEncryptBuffer();
						HandleResult();
						return;
					}

					if (g_MemCardManager->PopupResult() == MC_NO)
					{
						g_MemCardManager->RemovePopup();
						g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
						g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
			//pab			g_MemCardManager->SetNoSave(true);
						HandleResult();
						return;
					}
					else
					{			// try again homes
						g_MemCardManager->RemovePopup();
						return;
					}
				}
			}
		}
	}

	// compress data?
	if (g_MemCardManager->GetCompress() == true)
		g_MemCardManager->CompressData(g_MemCardManager->GetCompressionType());

	if (g_MemCardManager->GetEncrypt() == true)
		g_MemCardManager->EncryptData(g_MemCardManager->GetEncryptionType());

	// make a new buffer with our header in
	ts_MemcardHeader*	hdr = (ts_MemcardHeader*) g_MemCardManager->GetLoadSaveBuffer();
	hdr->version = MEMCARD_VERSION;
	hdr->compressed = g_MemCardManager->GetCompress();
	hdr->encrypted = g_MemCardManager->GetEncrypt();
	hdr->encryption_type = g_MemCardManager->GetEncryptionType();
	hdr->compression_type = g_MemCardManager->GetCompressionType();
	hdr->checksumsize = g_MemCardManager->GetFilesize();
	hdr->appVersion1 = g_MemCardManager->GetAppVersion1();
	hdr->appVersion2 = g_MemCardManager->GetAppVersion2();
	hdr->compressedSize = g_MemCardManager->GetCompressedSize();
	hdr->encryptedSize = g_MemCardManager->GetEncryptedSize();
	hdr->uncompressedSize = g_MemCardManager->GetUncompressedSize();

	strcpy (hdr->InfoString, g_MemCardManager->GetInfoString());
	strcpy (hdr->DateString, g_MemCardManager->GetDateString());

	// create checksum
	int	Checksum = Hash::CalcChecksum( (g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader)),g_MemCardManager->GetFilesize());
	hdr->checksum = Checksum;

	if (g_MemCardManager->GetCompress() == false && g_MemCardManager->GetEncrypt() == false)
		memcpy(g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), g_MemCardManager->GetBuffer(), g_MemCardManager->GetFilesize());

	// display warning if autosave is active
	if (g_MemCardManager->GetAutosave())
	{
		g_MemCardManager->SetPopupTitle("Autosaving...");
		g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_AUTOSAVING, MC_NONE);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetAutoSaveIcon(true);
	}

	// display popup
	if( g_MemCardManager->GetUsePopups() )
	{
		g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
		if (OverWrite == false)
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVING_FILE, MC_NONE);
		else
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_OVERWRITING_FILE, MC_NONE);
	}

	// start save
	int	size = 0; 

	if (g_MemCardManager->GetCompress())
		size = g_MemCardManager->GetCompressedSize() + sizeof(ts_MemcardHeader);

	if (g_MemCardManager->GetEncrypt() && g_MemCardManager->GetCompress() == false)
		size = g_MemCardManager->GetFilesize() + sizeof(ts_MemcardHeader) + g_helix.GetEncryptionHeaderSize();

	if (g_MemCardManager->GetEncrypt() == false && g_MemCardManager->GetCompress() == false)
		size =  g_MemCardManager->GetFilesize() + sizeof(ts_MemcardHeader);

	hdr->datasize = size;

	g_MemCardManager->SetNoSave(false);
	result = g_MemCardMgr.Save( g_MemCardManager->GetPort(), g_MemCardManager->GetLoadSaveBuffer(), size);

		// save failed??
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetAutoSaveIcon(false);

		if (OverWrite == false)
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVE_FAILED, MC_NO, MC_NO, MC_YES, MC_NO);
		else
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_OVERWRITE_FAILED,MC_NO, MC_NO, MC_YES, MC_NO );
		result = WaitOnResult();

		// bail out if we need to
		if (result == MEMCARD_RESULT_CANCEL_OPERATION)
		{
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			g_MemCardManager->SetPopupExitResult(PR_SAVE_NO);
			g_MemCardManager->ResetLoadSaveBuffer();
			g_MemCardManager->ResetEncryptBuffer();
			HandleResult();
			return;
		}

		if (g_MemCardManager->PopupResult() == MC_NO)
		{
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			g_MemCardManager->SetPopupExitResult( PR_SAVE_CANCEL );
			HandleResult();
			return;
		}
		else
		{
			// need to recopy the original buffer back in since its been compresssed / encrypted at this
			// point
			g_MemCardManager->RecopyDataBuffer();
			return;
		}
	}

	// save out the dummy file
	{
		g_MemCardMgr.SetFileName(g_MemCardManager->GetTitleID());
		result = g_MemCardMgr.Save( g_MemCardManager->GetPort(), g_MemCardManager->GetBuffer(), 32);
		g_MemCardManager->SetFirstSave(false);
	}

	g_MemCardManager->SetAutoSaveIcon(false);

	g_MemCardManager->RemovePopup();

	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::Exists()
{
	MEMCARD_RESULT	result;

	char ProfName[48];
	sprintf(ProfName, "%s%s", g_MemCardManager->GetComment(),g_MemCardManager->GetExt());
	g_MemCardMgr.SetFileName(ProfName);

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// exists
	result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());		//todo
	g_MemCardManager->SetResult(result);
	g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::Delete()
{
	MEMCARD_RESULT	result;
	int				NumFiles = 0;
	bool				CardChanged = false;

	OverWrite = false;

	g_MemCardManager->SetPopupTitle("Delete");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	char ProfName[48];
	sprintf(ProfName, "%s%s", g_MemCardManager->GetComment(), g_MemCardManager->GetExt());
	g_MemCardMgr.SetFileName(ProfName);

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// are you sure
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_DELETE, MC_NO, MC_NO, MC_YES, MC_NO);
	CardChanged = WaitOnResult(true);

	if (g_MemCardManager->PopupResult() == MC_NO || CardChanged != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->RemovePopup();
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_DELETE_CANCELLED, MC_OK, MC_OK, MC_OK);
		WaitOnResult();

		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
		g_MemCardManager->SetPopupExitResult(PR_DELETE_NO);
		g_MemCardManager->RemovePopup();
		HandleResult();
		return;
	}
	else
		g_MemCardManager->RemovePopup();

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// display popup
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_DELETE_FILE, MC_NONE);

	result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask());
	NumFiles = g_MemCardMgr.GetNumFilesFound();

	// delete main file
	if (NumFiles == 1)
	{
		result = g_MemCardMgr.DeleteAll(g_MemCardManager->GetPort(), g_MemCardManager->GetTitleID());
		g_MemCardManager->SetFirstSave(false);
	}
	else
		result = g_MemCardMgr.Delete(g_MemCardManager->GetPort());

	g_MemCardManager->RemovePopup();

	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::Format()
{
	MEMCARD_RESULT	result;

	g_MemCardManager->SetPopupTitle("Format");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

		// are you sure
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_FORMAT, MC_YES, MC_NO, MC_YES, MC_NO);
	WaitOnResult();

	if (g_MemCardManager->PopupResult() == MC_NO)
	{
		g_MemCardManager->RemovePopup();
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMAT_CANCELLED, MC_NONE);
		WaitOnResult();
		
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
		g_MemCardManager->SetPopupExitResult(PR_FORMAT_NO);
		HandleResult();
		return;
	}
	else
		g_MemCardManager->RemovePopup();

	// re-detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// display popup
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMATTING, MC_NONE);

	// format
	result = g_MemCardMgr.Format(g_MemCardManager->GetPort());
	g_MemCardManager->RemovePopup();

	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
int	CMemoryCard_PS2::BlocksNeededForSave()
{
	int	BlocksNeeded = 0;

	BlocksNeeded = g_MemCardMgr.GetBlocksForFileSize(g_MemCardManager->GetFilesize());
	return (BlocksNeeded);
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::Detect()
{
	MEMCARD_RESULT	result;

	g_MemCardManager->SetPopupTitle("");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	g_MemCardManager->SetResult(result);
	g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
	g_MemCardManager->RemovePopup();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::DetectWithResult()
{
	MEMCARD_RESULT	result;

	g_MemCardManager->SetPopupTitle("");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::DetectOnStart()
{
	MEMCARD_RESULT	result;

	g_MemCardManager->SetPopupTitle("");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::GetFilesOnCard()
{
	MEMCARD_RESULT		result;
	int					FileNum = 0;
	sceMcTblGetDir*	DirTable;
	int					i;
	ts_CMemCardDate	Date;
	ts_CMemCardTime	Time;

	g_MemCardManager->SetPopupTitle("Finding Files");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	// detect first
	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_DETECT_CARD, MC_NONE);
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	g_MemCardManager->RemovePopup();

	if(result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// get directory listing
//pab	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );			// told not needed
//pab	g_MemCardManager->DisplayPopup(CMEMCARDMSG_ACCESSING_CARD, MC_NONE);
	result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask());
	g_MemCardManager->RemovePopup();

	if(result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// get num
	FileNum = g_MemCardMgr.GetNumFilesFound();
	DirTable = g_MemCardMgr.GetDirTable();

	// fill out the info
	for ( i = 0 ; i < FileNum ; i++)
	{
		// get date and time
		Time.Hour = DirTable[i]._Create.Hour;
		Time.Minutes = DirTable[i]._Create.Min;
		Time.Seconds = DirTable[i]._Create.Sec;
		Date.Day = DirTable[i]._Create.Day;
		Date.Month = DirTable[i]._Create.Month;
		Date.Year = DirTable[i]._Create.Year;

		g_MemCardManager->FillOutFileInfo(i, (char*)&DirTable[i].EntryName, ".", g_MemCardManager->GetPort(),
													 0, "none", (int)DirTable[i].FileSizeByte, Date, Time );
	}

	// detect again
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());

	if(result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// ok done
	g_MemCardManager->SetNumFilesOnCard(FileNum);
	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::GetFilesOnAllCards()
{
	MEMCARD_RESULT		result;
	int					FileNum = 0;
	sceMcTblGetDir*	DirTable;
	int					i, j;
	int					Port = 0;
	ts_CMemCardDate	Date;
	ts_CMemCardTime	Time;
	int					change = 0;

	g_MemCardManager->SetCardChanged(g_MemCardManager->GetPort(), false);
	g_MemCardManager->SetPopupTitle("Finding Files");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	// 2 ports, not supporting multitap (for now)
	for ( i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
	{
		g_MemCardManager->SetPort(i);

		// detect first
		g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_DETECT_CARD, MC_NONE);
		result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
		g_MemCardManager->RemovePopup();

		if (result == MEMCARD_RESULT_OK)
		{
			// get directory listing
			result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask());

			if (result == MEMCARD_RESULT_OK)
			{
				// get num
				FileNum = g_MemCardMgr.GetNumFilesFound();
				DirTable = g_MemCardMgr.GetDirTable();

				// fill out the info
				for ( j = 0 ; j < FileNum ; j++)
				{
					// get date and time
					Time.Hour = DirTable[j]._Create.Hour;
					Time.Minutes = DirTable[j]._Create.Min;
					Time.Seconds = DirTable[j]._Create.Sec;
					Date.Day = DirTable[j]._Create.Day;
					Date.Month = DirTable[j]._Create.Month;
					Date.Year = DirTable[j]._Create.Year;

					g_MemCardManager->FillOutFileInfo(j, (char*)&DirTable[j].EntryName, ".", g_MemCardManager->GetPort(),
																 0, "none", (int)DirTable[j].FileSizeByte, Date, Time);
				}
			}
			else
			{
				g_MemCardManager->SetNumFilesOnCard(0);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetResult(result);
				HandleResult();
				return;
			}
			g_MemCardManager->RemovePopup();
		}
		else
		{
			if (g_MemCardManager->GetIgnoreErrors() == false)
			{
				g_MemCardManager->SetNumFilesOnCard(0);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetResult(result);
				HandleResult();
				return;
			}
			else
			{
				g_MemCardManager->SetNumFilesOnCard(0);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				HandleResult();
			}
		}
	}
	
	// cycle thru all files and see if icon.sys is valid
	// TODO - change name to *corrupt* if it is missing




	// out of here
	g_MemCardManager->RemovePopup();

	// ok done
	g_MemCardManager->SetNumFilesOnCard(FileNum);
	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_PS2::HandleResult()
{
	int				temp;

	MEMCARD_RESULT	result = (MEMCARD_RESULT)g_MemCardManager->GetResult();
	MEMCARD_RESULT	old_result = result;

	// special case this.. we dont want to detect other issues here
	// only detect card in or not
	if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_DETECT_ON_START)
	{
		if (result != MEMCARD_RESULT_OK && result != MEMCARD_RESULT_NOCARD)
			result = MEMCARD_RESULT_OK;
	}

	switch(result)
	{
	case	MEMCARD_RESULT_NONE:
		g_MemCardManager->SetResult(MEMCARD_RESULT_OK);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;

	case	MEMCARD_RESULT_OK:
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_LOAD_NETWORK ||
			 g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_DETECT_ON_START ||
			 g_MemCardManager->GetNoOkConfirmation() == true)
		{
			g_MemCardManager->SetPopupExitResult(PR_COMPLETED_OK);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}

		// display appropriate message
		if (g_MemCardManager->GetAutosave() == false)
		{
			if (OverWrite == true)
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_OVERWRITE_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
				WaitOnResult();
			}
			else
			{
				if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
				{
					if( g_MemCardManager->GetUsePopups() )
					{
						g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVE_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
						WaitOnResult();
					}
				}
				else if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_LOAD ||
							g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_HANDLE_RESULT)
				{
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_LOAD_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
					WaitOnResult();
				}
				else if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_DELETE)
				{
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_DELETE_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
					WaitOnResult();
				}
				else
				{
					if (g_MemCardManager->GetState() != g_MemCardManager->CMEMORYCARD_DETECT_WITH_RESULT)
					{
						g_MemCardManager->DisplayPopup(CMEMCARDMSG_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
						WaitOnResult();
					}
				}
			}

		}
		g_MemCardManager->SetPopupExitResult(PR_COMPLETED_OK);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;

	case	MEMCARD_RESULT_NOCARD:

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_DETECT_ON_START)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_CONTINUE_SURE_NOCARD, MC_YES, MC_NO, MC_YES, MC_NO);
			result = WaitOnResult(true);
			if (result != old_result)
			{
				g_MemCardManager->SetCardChanged(g_MemCardManager->GetPort(), true);
			}

			if (g_MemCardManager->PopupResult() == MC_YES)
			{
				g_MemCardManager->SetPopupExitResult(PR_NOCARD_YES);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				break;
			}
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_NOCARD_NO);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				break;
			}
		}

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE || g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			if (!strcmp(g_MemCardManager->GetExt(), ".pat"))
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_CARD_SAVE, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
				temp = MC_CANCEL;
			}
			else
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_CARD_SAVE, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING);
				temp = MC_CONTINUE_WITHOUT_SAVING;
			}
		}
		else
		{
			if (!strcmp(g_MemCardManager->GetExt(), ".pat"))
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_CARD_LOAD, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
				temp = MC_CANCEL;
			}
			else
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_CARD_LOAD, MC_CONTINUE_WITHOUT_LOADING, MC_RETRY, MC_RETRY, MC_CONTINUE_WITHOUT_LOADING);
				temp = MC_CONTINUE_WITHOUT_LOADING;
			}
		}

		WaitOnResult();

		if (g_MemCardManager->PopupResult() == temp)
		{
			g_MemCardManager->SetPopupExitResult(PR_NOCARD_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);

//pab			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE || g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
//pab				g_MemCardManager->SetNoSave(true);

			break;
		}

		g_MemCardManager->RemovePopup();
		break;

	case	MEMCARD_RESULT_UNFORMATTED:

		g_MemCardManager->SetPopupTitle("Unformatted Card");

		// startup.. dont allow format, but allow a new card.. otherwise continue without saving
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_UNFORMATTED_CARD_OK, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
			WaitOnResult();

			if (g_MemCardManager->PopupResult() == MC_RETRY)
				return;
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
//pab				g_MemCardManager->SetNoSave(true);
				return;
			}

		g_MemCardManager->RemovePopup();
		}

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_UNFORMATTED_CARD, MC_YES, MC_NO, MC_YES, MC_NO);
			WaitOnResult();

			if (g_MemCardManager->PopupResult() == MC_NO)
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMAT_CANCELLED, MC_OK, MC_OK, MC_OK);
				WaitOnResult();
//				g_MemCardManager->SetPopupExitResult(PR_FORMAT_NO);
//				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
//				g_MemCardManager->RemovePopup();
//				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);	// retry save
				break;	// they declined format, so break out and try "save" state again.
			}

			g_MemCardManager->RemovePopup();
		}
		// no option to format on load
		else if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_LOAD || 
					g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_GET_FILES_ON_CARD ||
					g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_GET_FILES_ON_ALL_CARDS)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_UNFORMATTED_CARD_OK, MC_OK, MC_OK, MC_OK);
			WaitOnResult();
			g_MemCardManager->SetPopupExitResult(PR_FORMAT_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		else
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_UNFORMATTED_CARD_OK, MC_CANCEL, MC_CANCEL, MC_CANCEL);
			WaitOnResult();
			g_MemCardManager->SetPopupExitResult(PR_FORMAT_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}

		// re-detect card
		result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
		if (result != old_result)
		{
			g_MemCardManager->SetResult(result);
			return;
		}

		// if not unformatted card anymore.. bail and try save again.
		if (result != MEMCARD_RESULT_UNFORMATTED)
			break;

		// start format
		while (1)
		{
			result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());

			// if not unformatted card anymore.. bail and try save again.
			if (result != MEMCARD_RESULT_UNFORMATTED)
				return;

			g_MemCardManager->SetPopupTitle("Formatting Card");
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMATTING, MC_NONE);

			result = g_MemCardMgr.Format(g_MemCardManager->GetPort());

			// did it fail??
			if (result != MEMCARD_RESULT_OK)
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMAT_FAILED, MC_YES, MC_YES, MC_YES, MC_NO);
				WaitOnResult();

				if (g_MemCardManager->PopupResult() == MC_NO)
				{
					g_MemCardManager->SetResult(result);
					g_MemCardManager->SetPopupExitResult(PR_FORMAT_FAILED);
					g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
					g_MemCardManager->RemovePopup();
					g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
					return;
				}
			}

			else
				break;
		}

		FormatDone = true;

		// completed ok--- if done during a save, continue with save without saying format completed ok? for TCR
		if (g_MemCardManager->GetState() != g_MemCardManager->CMEMORYCARD_SAVE)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMAT_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
			WaitOnResult();
			g_MemCardManager->RemovePopup();
		}
		else		// save, just display format successful with no choices
		{
			g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMAT_COMPLETED_OK, MC_NONE);
			g_MemCardManager->RemovePopup();
		}
		return;

	case	MEMCARD_RESULT_NOFILES:
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		// display appropriate message
		break;

	case	MEMCARD_RESULT_BADCARD:

		g_MemCardManager->SetPopupTitle("");

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_DAMAGED_CARD, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
			WaitOnResult();

			if (g_MemCardManager->PopupResult() == MC_RETRY)
				return;
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
//pab				g_MemCardManager->SetNoSave(true);
				return;
			}
		}

		g_MemCardManager->DisplayPopup(CMEMCARDMSG_DAMAGED_CARD, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_CANCEL)
		{
			g_MemCardManager->SetPopupExitResult(PR_DAMAGED_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_WRONGTYPE:

		g_MemCardManager->SetPopupTitle("");

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE || g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_CARD_SAVE, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING);
			temp = MC_CONTINUE_WITHOUT_SAVING;
		}
		else
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_CARD_LOAD, MC_CONTINUE_WITHOUT_LOADING, MC_RETRY, MC_RETRY, MC_CONTINUE_WITHOUT_LOADING);
			temp = MC_CONTINUE_WITHOUT_LOADING;
		}

		WaitOnResult();

		if (g_MemCardManager->PopupResult() == temp)
		{
			g_MemCardManager->SetPopupExitResult(PR_NOCARD_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);

//pab			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE || g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
//pab				g_MemCardManager->SetNoSave(true);

			break;
		}
		break;

	case	MEMCARD_RESULT_OTHERMARKET:

		g_MemCardManager->SetPopupTitle("");

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_OTHER_MARKET, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
			WaitOnResult();

			if (g_MemCardManager->PopupResult() == MC_RETRY)
				return;
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
//pab				g_MemCardManager->SetNoSave(true);
				return;
			}
		}

		g_MemCardManager->DisplayPopup(CMEMCARDMSG_OTHER_MARKET, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_CANCEL)
		{
			g_MemCardManager->SetPopupExitResult(PR_OTHER_MARKET_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_NODATA:
		g_MemCardManager->SetPopupTitle("");

		g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_VALID_SAVE, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_CANCEL)
		{
			g_MemCardManager->SetPopupExitResult(PR_NO_VALID_SAVE_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_BADDATA:
		g_MemCardManager->SetPopupTitle("");

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CORRUPT_CARD, MC_CANCEL, MC_CANCEL, MC_CANCEL, MC_DELETE);
		else
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CORRUPT_CARD, MC_NO, MC_NO, MC_YES, MC_NO);

		result = WaitOnResult(true);
		if (result != old_result)
		{
			g_MemCardManager->SetCardChanged(g_MemCardManager->GetPort(), true);
		}

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
		{
			if (g_MemCardManager->PopupResult() == MC_CANCEL || g_MemCardManager->PopupResult() == MC_NO)
			{
				g_MemCardManager->SetPopupExitResult(PR_CORRUPT_CANCEL);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				break;
			}
			else
			{
				Delete();
				break;
			}
		}
		else if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_LOAD)
		{
			if (g_MemCardManager->PopupResult() == MC_CANCEL || g_MemCardManager->PopupResult() == MC_NO)
			{
				g_MemCardManager->SetPopupExitResult(PR_CORRUPT_CANCEL);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				break;
			}
			else
			{
				break;
			}
		}
		else
		{
			if (g_MemCardManager->PopupResult() == MC_CANCEL || g_MemCardManager->PopupResult() == MC_NO)
			{
				g_MemCardManager->SetPopupExitResult(PR_CORRUPT_CANCEL);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				break;
			}
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_RETRY_YES);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				break;
			}
		}
		break;
	
	case MEMCARD_RESULT_NETWORK_CONFIGURATION_FAILED:
	{
		int errorCode = g_MemCardManager->GetErrorCode();
#if defined(JJSFIX)
		switch( errorCode )
		{
			case sceNETCNF_NG:
				g_MemCardManager->DisplayPopup( CMEMCARDMSG_NETWORK_CONFIG_BAD, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL );
				break;

			case sceNETCNF_MAGIC_ERROR:
				g_MemCardManager->DisplayPopup( CMEMCARDMSG_NETWORK_MAGIC_FAIL, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL );
				break;

			default:
				g_MemCardManager->DisplayPopup( CMEMCARDMSG_NETWORK_CONFIG_FAIL, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL );
				break;
		}
#endif
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_CANCEL)
		{
			g_MemCardManager->SetPopupExitResult(PR_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
	}
	break;

	default:
		// set back to idle
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;
	}
}

//--------------------------------------------------------------//
MEMCARD_RESULT	CMemoryCard_PS2::WaitOnResult(bool Detect)
{
	MEMCARD_RESULT	CardChanged = MEMCARD_RESULT_OK;

	while (g_MemCardManager->PopupResult() == MC_NONE )
	{
		if (Detect)
		{
			// detect card
			int result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
			if (result != MEMCARD_RESULT_OK)
				CardChanged = (MEMCARD_RESULT) result;
		}

		// wait a while
		g_MemCardManager->GetThread()->YieldThread(100);

		if (g_MemCardManager->GetCancelOperation())
		{
			CardChanged = MEMCARD_RESULT_CANCEL_OPERATION;
			g_MemCardManager->SetCancelOperation(false);
			break;
		}
	}

	return CardChanged;
}


#endif // PS2
