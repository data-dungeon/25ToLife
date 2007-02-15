//--------------------------------------------------------------//
// MemoryCard_Xbox.cpp
//
// Memory Card Handler for (platform specific)
// Author: Paul Blagay
//
//--------------------------------------------------------------//

#include "platform/PlatformPCH.h"

#if defined (_XBOX)

#include "platform/MemoryCard/MemoryCard.h"
#include "platform/MemoryCard/MemoryCard_Xbox.h"
#include "Layers/thread/thread.h"
#include "GameHelper/StringHelper.h"
#include "EngineHelper/Hash.h"
#include "GameHelper/encryption/Helix/Helix.h"

// memcard text files language specific
#include "memcard_english_xbox.h"
#include "Memcard_english_common.h"

//--------------------------------------------------------------//
// message formatting

ts_CMemCardMsg	CMemCardMsg[] = {
	{	MC_NO_VALID_SAVE_GAME,	CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },							
	{	MC_MEMCARD_FULL,			CMEMORYCARD_SAVE_TYPE,		CMEMORYCARD_BLOCKS_NEEDED,	-1, 							-1 },
	{	MC_MEMCARD_FULL_MULTIPLE, CMEMORYCARD_SAVE_TYPE,	CMEMORYCARD_BLOCKS_NEEDED,	CMEMORYCARD_BLOCKS_ALL, -1 },
	{	MC_NO_CARD_SAVE,			CMEMORYCARD_CONTROL_PORT,	CMEMORYCARD_MEMCARD_PORT,	-1, 							-1 },								
	{	MC_NO_CARD_LOAD,			CMEMORYCARD_CONTROL_PORT,	CMEMORYCARD_MEMCARD_PORT,	-1, 							-1 },								
	{	MC_AUTOSAVE_REMOVED,		CMEMORYCARD_CONTROL_PORT,	CMEMORYCARD_MEMCARD_PORT,	-1, 							-1 },								
	{	MC_SURE_LOAD,				CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_SURE_SAVE,				CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_SURE_DELETE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_SURE_OVERWRITE,		CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_LOADING_FILE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_SAVING_FILE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_OVERWRITING_FILE,		CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_DELETE_FILE	,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },
	{	MC_LOADING_CANCELLED,	-1,								-1,								-1, 							-1 },
	{	MC_SAVING_CANCELLED,		-1,								-1,								-1, 							-1 },
	{	MC_DELETE_CANCELLED,		-1,								-1,								-1, 							-1 },
	{	NULL,							-1,								-1,								-1, 							-1 },			// format cancelled
	{	NULL,							-1,								-1,								-1, 							-1 },			// format
	{	NULL,							NULL,								-1,								-1, 							-1 },			// sure format
	{	MC_ACCESSING_CARD,		-1,								-1,								-1, 							-1 },
	{	MC_DETECT_CARD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_OTHER_MARKET,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	NULL,							-1,								-1,								-1, 							-1 },			// unformatted
	{	MC_DAMAGED_CARD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },								
	{	MC_CORRUPT_CARD,			CMEMORYCARD_SAVE_TYPE,		-1,								-1, 							-1 },								
	{	MC_COMPLETED_OK,			-1,								-1,								-1, 							-1 },
	{	NULL,							-1,								-1,								-1, 							-1 },			// command failed
	{	NULL,							-1,								-1,								-1, 							-1 },			// format failed
	{	MC_AUTOSAVE_NO_CARD,		CMEMORYCARD_CONTROL_PORT,	CMEMORYCARD_MEMCARD_PORT,	-1, 							-1 },			
	{	NULL,							-1,								-1,								-1, 							-1 },			// unformatted card ok
	{	MC_FORMAT_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_DELETE_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_LOAD_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_SAVE_COMPLETED_OK,	-1,								-1,								-1, 							-1 },		
	{	MC_CARD_CHANGED,			CMEMORYCARD_CONTROL_PORT,	CMEMORYCARD_MEMCARD_PORT,	-1, 							-1 },			// card changed since last save
	{  NULL,							-1,								-1,								-1, 							-1 },
	{  NULL,							-1,							   -1,								-1, 							-1 },
	{  NULL,							-1,							   -1,								-1, 							-1 },
	{  MC_AUTOSAVING,				-1,								-1,								-1, 							-1 },			// autosaving
	{  MC_SAVE_FAILED,			-1,								-1,								-1, 							-1 },			// failed.
	{  NULL,							-1,								-1, 								-1,							-1 },			// PS2 
	{  MC_MEMCARD_FULL_FILES,	CMEMORYCARD_TITLE_NAME,		-1,								-1, 							-1 },			// max files reached
	{  NULL,							-1,								-1,								-1, 							-1 },			// PS2
	{	MC_MEMCARD_FULL_STARTUP, CMEMORYCARD_SAVE_TYPE,	CMEMORYCARD_BLOCKS_NEEDED, CMEMORYCARD_SAVE_TYPE,		-1 },			// startup memcard full
	{  MC_OVERWRITE_FAILED,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1, 							-1 },			// overwrite failed.
	{	MC_SURE_OVERWRITE,		CMEMORYCARD_SAVE_TYPE,		CMEMORYCARD_MEMCARD_PORT,	-1, 							-1 },
	{	MC_OVERWRITING_FILE,		CMEMORYCARD_SAVE_TYPE,								-1,								-1, 							-1 },
	{  NULL,							-1,								-1,								-1, 							-1 },
	{  NULL,							-1,								-1,								-1, 							-1 },
	{  NULL,							-1,								-1,								-1, 							-1 },
	{  NULL,							-1,								-1,								-1, 							-1 },
	{  NULL,							-1,								-1,								-1, 							-1 },
	{	MC_ARE_YOU_SURE,			-1,								-1,								-1, 							-1 },			// double confirm

};

static	int	PollCount = 0;
static	bool	CardsActive[MAX_MEMCARDS] = {false};
static	bool	OverWrite = false;
//MEMORYSTATUS XboxDXMem;

//--------------------------------------------------------------//

int	MemCardThreadFunc()
{
	int					i;
	MEMCARD_RESULT		result;
	MEMCARD_RESULT		last_result[MAX_MEMCARDS];

	g_MemCardManager->GetThread()->YieldThread(1);

	ASSERTS (g_MemCardManager->GetThread(), "MemCard thread is not ready.");

	// get latest active counts
	for ( i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
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
		CMemoryCard_Xbox::Detect();
		if (g_MemCardManager->GetResult() == MEMCARD_RESULT_OK)
			g_MemCardManager->SetCardInserted(i, true);
	}

	while (1)
	{
//		GlobalMemoryStatus(&XboxDXMem);

		switch (g_MemCardManager->GetState())
		{
		case	g_MemCardManager->CMEMORYCARD_IDLE:
			break;
		case	g_MemCardManager->CMEMORYCARD_DETECT:
			CMemoryCard_Xbox::Detect();
			break;
		case	g_MemCardManager->CMEMORYCARD_LOAD:
			CMemoryCard_Xbox::Load();
			break;
		case	g_MemCardManager->CMEMORYCARD_SAVE:
			CMemoryCard_Xbox::Save();
			break;
		case	g_MemCardManager->CMEMORYCARD_DELETE:
			CMemoryCard_Xbox::Delete();
			break;
		case	g_MemCardManager->CMEMORYCARD_FILE_EXISTS:
			CMemoryCard_Xbox::Exists();
			break;
		case	g_MemCardManager->CMEMORYCARD_GET_FILES_ON_CARD:
			CMemoryCard_Xbox::GetFilesOnCard();
			break;
		case	g_MemCardManager->CMEMORYCARD_GET_FILES_ON_ALL_CARDS:
			CMemoryCard_Xbox::GetFilesOnAllCards();
			break;
		case	g_MemCardManager->CMEMORYCARD_HANDLE_RESULT:
			CMemoryCard_Xbox::HandleResult();
			break;
		case	g_MemCardManager->CMEMORYCARD_STARTUP:
			CMemoryCard_Xbox::StartupCheck();
			break;
		default:
			break;
		}

		// sleep a while
		g_MemCardManager->GetThread()->YieldThread(1);

		// check for change of card, and set changed flag
		for (i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
		{
			result = g_MemCardMgr.Detect(i);

			if (result == MEMCARD_RESULT_OK)
			{
				if (last_result[i] == MEMCARD_RESULT_NOCARD)
				{
					if (g_MemCardManager->GetCardChanged(i) == false)
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
					if (g_MemCardManager->GetCardChanged(i) == false)
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

	return 0;
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::StartupCheck()
{
	u32					BlocksNeeded;
	MEMCARD_RESULT		space_result;
	MEMCARD_RESULT		result;

	OverWrite = false;

	g_MemCardManager->SetPopupTitle("");
	g_MemCardManager->SetNoSave(false);

	BlocksNeeded = BlocksNeededForSave();
	space_result = g_MemCardMgr.CheckSpace( g_MemCardManager->GetPort(), g_MemCardManager->GetFilesize() , BlocksNeeded);

	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	// detect first
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// if not enough space, put up a message about continuing without save etc..
	if (space_result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL_STARTUP, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_MANAGE_FILES);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_CONTINUE_WITHOUT_SAVING)
		{
//pab			g_MemCardManager->SetNoSave(true);
			g_MemCardManager->SetAutosave(false);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
			HandleResult();
			return;
		}
		else
		{
			// call dashboard
			g_MemCardManager->RemovePopup();
			Dashboard();
		}
	}
	else
	{
		g_MemCardManager->SetNoSave(false);
		g_MemCardManager->SetResult(result);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
	}
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::PollCards()
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
			g_MemCardManager->SetCardChanged(i, true);
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
void	CMemoryCard_Xbox::Load()
{
	MEMCARD_RESULT	result;

	OverWrite = false;

	g_MemCardManager->SetPopupTitle("Load");

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	// detect
	if (result != MEMCARD_RESULT_OK)
	{
		if (result == MEMCARD_RESULT_NOSPACE)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_MANAGE_FILES);
			WaitOnResult();

			if (g_MemCardManager->PopupResult() == MC_CONTINUE_WITHOUT_SAVING)
			{
//pab				g_MemCardManager->SetNoSave(true);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
				HandleResult();
				return;
			}
			else
			{
				// call dashboard
				g_MemCardManager->RemovePopup();
				Dashboard();
			}
		}
		else
		{
			g_MemCardManager->SetResult(result);
			HandleResult();
			return;
		}
	}

	// set filename
	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());

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
	}

	// check file exists first.. if not then dont bother displaying loading message
	result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_NODATA);
		HandleResult();
		return;
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
				bool result= g_MemCardManager->UncompressData(hdr->compression_type);
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
		memcpy(g_MemCardManager->GetBuffer(), g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), g_MemCardManager->GetFilesize() );
	else if (g_MemCardManager->GetLoadHeader() == true)
		memcpy(g_MemCardManager->GetBuffer(), g_MemCardManager->GetLoadSaveBuffer(), g_MemCardManager->GetFilesize());

	// set the versions we loaded...
	g_MemCardManager->SetLoadedAppVersion1(hdr->appVersion1);
	g_MemCardManager->SetLoadedAppVersion2(hdr->appVersion2);

	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::Save()
{
	MEMCARD_RESULT	result;
	u32				BlocksNeeded;

	OverWrite = false;

	g_MemCardManager->SetPopupTitle("Save");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

#if defined TURN_ON_CARD_CHANGED
	// did card change??
	if (g_MemCardManager->GetCardChanged(g_MemCardManager->GetPort()) == true && g_MemCardManager->GetAutosave() == true)
	{
		g_MemCardManager->SetCardChanged(g_MemCardManager->GetPort(), false);

		// display message about changing card etc
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_CARD_CHANGED, MC_NO, MC_NO, MC_YES, MC_NO);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_NO)
		{
			g_MemCardManager->SetAutosave(false);
//pab			g_MemCardManager->SetNoSave(true);
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
#endif

	// are you sure
	if (g_MemCardManager->GetAreYouSure() == true)
	{
		while (1)
		{
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
					WaitOnResult();
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
	
	if (result != MEMCARD_RESULT_OK)
	{
		// card in slot?
		if (result == 	MEMCARD_RESULT_NOCARD)
		{
			// autosave on?
			if (g_MemCardManager->GetAutosave() == true)
			{
				// autosave needs to be turned off here if they say continue without saving
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_AUTOSAVE_REMOVED, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
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

				// continue without saving?
				if (g_MemCardManager->PopupResult() == MC_CONTINUE_WITHOUT_SAVING)
				{
					g_MemCardManager->SetAutosave(false);
//pab					g_MemCardManager->SetNoSave(true);
					g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
					g_MemCardManager->RemovePopup();
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
							if (result == MEMCARD_RESULT_OK)					// good to go
							{
								g_MemCardManager->SetAutosave(true);
								g_MemCardManager->SetNoSave(false);
								g_MemCardManager->RemovePopup();
								break;
							}
							else														// fall thru and catch error
								g_MemCardManager->RemovePopup();
						}
					}
				}
			}
		}
	}

//	int	NumFiles = 0;

	// set filename
	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());

	// do we have existing save game?
	result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());

	if (result != MEMCARD_RESULT_OK)
	{
		BlocksNeeded = BlocksNeededForSave();
		result = g_MemCardMgr.CheckSpace( g_MemCardManager->GetPort(), g_MemCardManager->GetFilesize(), BlocksNeeded);

		// detect
		if (result != MEMCARD_RESULT_OK)
		{
			// space check
			if (result == MEMCARD_RESULT_NOSPACE)
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_MANAGE_FILES);
				result =	WaitOnResult();

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
//pab					g_MemCardManager->SetNoSave(true);
					g_MemCardManager->RemovePopup();
					g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
					g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
					HandleResult();
					return;
				}
				else
				{
					// call dashboard
					g_MemCardManager->RemovePopup();
					Dashboard();
				}
			}
			else
			{
				g_MemCardManager->SetResult(result);
				HandleResult();
				return;
			}
		}
	}

	// enough files in directory left to create a new file??
	// PAB - dont really need this on Xbox, but may implement later for other games.. 
#if 0
	result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask());
	NumFiles = g_MemCardMgr.GetNumFilesFound();

	if (NumFiles >= MC_MAX_FILES_DIR)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL_FILES, MC_CONTINUE_WITHOUT_SAVING, MC_CHOOSE_EXISTING, MC_CHOOSE_EXISTING, MC_CONTINUE_WITHOUT_SAVING);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_CONTINUE_WITHOUT_SAVING)
		{
			g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab			g_MemCardManager->SetNoSave(true);
		}
		else
			g_MemCardManager->SetPopupExitResult(PR_CHOOSE_EXISTING);

		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
		HandleResult();
		return;
	}
#endif


	// does file exists already
	result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());

	if (g_MemCardManager->GetAutosave() == false)
	{
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
			
					g_MemCardManager->RemovePopup();
					g_MemCardManager->SetPopupExitResult(PR_OVERWRITE_NO);
					g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
					HandleResult();
					return;
				}
				else
				{
					// delete original container
					g_MemCardMgr.DeleteContainer(g_MemCardManager->GetPort());
					OverWrite = true;
					g_MemCardManager->RemovePopup();
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

	// display popup
	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	if (OverWrite == false)
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVING_FILE, MC_NONE);
	else
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_OVERWRITING_FILE, MC_NONE);

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
		if (result == MEMCARD_RESULT_NOFILES)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL_FILES, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_MANAGE_FILES, MC_CONTINUE_WITHOUT_SAVING);
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
//pab				g_MemCardManager->SetNoSave(true);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
				HandleResult();
				return;
			}
			else
			{
				Dashboard();
			}
		}

		if (OverWrite == false)
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVE_FAILED, MC_CANCEL, MC_RETRY, MC_CANCEL, MC_RETRY);
		else
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_OVERWRITE_FAILED, MC_CANCEL, MC_RETRY, MC_CANCEL, MC_RETRY);
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

		if (g_MemCardManager->PopupResult() == MC_CANCEL)
		{
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
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

	g_MemCardManager->RemovePopup();

	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::Exists()
{
	MEMCARD_RESULT	result;

	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());

	// exists
	result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());
	g_MemCardManager->SetResult(result);
	g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::Delete()
{
	MEMCARD_RESULT	result;

	OverWrite = false;

	g_MemCardManager->SetPopupTitle("Delete");

	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	// are you sure
	if (g_MemCardManager->GetAreYouSure() == true)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_DELETE, MC_YES, MC_NO, MC_YES, MC_NO);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_NO)
		{
			g_MemCardManager->RemovePopup();
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_DELETE_CANCELLED, MC_OK, MC_OK, MC_OK);
			WaitOnResult();

			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			g_MemCardManager->SetPopupExitResult(PR_DELETE_NO);
			HandleResult();
			return;
		}
		else
			g_MemCardManager->RemovePopup();
	}

	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_DELETE_FILE, MC_NONE);

	// delete
	result = g_MemCardMgr.DeleteContainer(g_MemCardManager->GetPort());
	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::GetFilesOnCard()
{
	int	NumFiles = 0;

	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	// hard drive??
	if (g_MemCardManager->GetPort() == 0)
		NumFiles = EnumerateSaveGames("U:\\", NumFiles);
	else			// enumerate the memory units
	{
		char	drive;
		int	result; 

		result = XMountMU( (g_MemCardManager->GetPort()/2)-1, g_MemCardManager->GetPort()-1, &drive);

		// found one.. so go through and enumerate save games
		if (result == ERROR_SUCCESS)
		{
			char	DrivePath[16];
			sprintf(DrivePath, "%s:\\", &drive);
			NumFiles = EnumerateSaveGames(DrivePath, NumFiles);
			XUnmountMU((g_MemCardManager->GetPort()/2)-1, g_MemCardManager->GetPort()-1);
		}
	}

	g_MemCardManager->SetNumFilesOnCard(NumFiles);
	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::GetFilesOnAllCards()
{
	int	NumFiles = 0;

	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	// enumerate the harddrive
	NumFiles = EnumerateSaveGames("U:\\", 0);

	int	NumCards = g_MemCardManager->GetNumCardsToPoll() - 1;
	NumCards /= 2;
	NumCards += 1;

	if (NumCards > 1)
	{
		// try and mount a memory unit
		for (int i = 0; i < NumCards; i++)
		{
			char	drive;

			for (int j = 0; j < 2; j++)			// check both slots
			{
				int	result; 
				result = XMountMU(i, j, &drive); 

				// found one.. so go through and enumerate save games
				if	(result == ERROR_SUCCESS)
				{
					char	DrivePath[16];
					sprintf(DrivePath, "%s:\\", &drive);
					NumFiles += EnumerateSaveGames(DrivePath, NumFiles);
					XUnmountMU(i, j);
				}
			}
		}
	}	

	g_MemCardManager->SetNumFilesOnCard(NumFiles);
	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
int	CMemoryCard_Xbox::EnumerateSaveGames(char* Drive, int StartIndex)
{
	HANDLE				Handle;
	XGAME_FIND_DATA	FindData;
	int					FileNum = StartIndex;
	ts_CMemCardDate	Date;
	ts_CMemCardTime	Time;
	SYSTEMTIME			SysTime;

	// enumerate save games
	Handle = XFindFirstSaveGame(Drive, &FindData);

	// no save games?
	if (Handle == INVALID_HANDLE_VALUE)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_NOFILES);
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_HANDLE_RESULT);
		return 0;
	}

	// we have a save game, save it off and search for any more
	int	Port;
	int	ControllerPort = 0;
	char*	MU_Name = NULL;
	int	MU_NameSize = 0;

	if (g_MemCardManager->GetPort() == 0)
	{
		Port = g_MemCardManager->GetPort();
		MU_Name = "";
	}
	else
	{
		if (g_MemCardManager->GetPort() == 1 || g_MemCardManager->GetPort() == 2)
			ControllerPort = 1;
		if (g_MemCardManager->GetPort() == 3 || g_MemCardManager->GetPort() == 4)
			ControllerPort = 2;
		if (g_MemCardManager->GetPort() == 5 || g_MemCardManager->GetPort() == 6)
			ControllerPort = 3;
		if (g_MemCardManager->GetPort() == 7 || g_MemCardManager->GetPort() == 8)
			ControllerPort = 4;

		XMUNameFromDriveLetter( *Drive, (LPWSTR)MU_Name, MU_NameSize);
		Port = XMUPortFromDriveLetter(*Drive);
	}

	// setup date and time
	BOOL	result = FileTimeToSystemTime( &FindData.wfd.ftLastWriteTime, &SysTime);
	Time.Hour    = SysTime.wHour;
	Time.Minutes = SysTime.wMinute;
	Time.Seconds = SysTime.wSecond;
	Date.Day     = SysTime.wDay;
	Date.Month   = SysTime.wMonth;
	Date.Year    = SysTime.wYear;

	g_MemCardManager->FillOutFileInfo(FileNum, CStringHelper::UnicodeToAscii((short*)&FindData.szSaveGameName), FindData.szSaveGameDirectory, 
												 Port,ControllerPort,MU_Name, 0, Date, Time);
	FileNum++;

	// loop through all other save games
	while ( XFindNextSaveGame(Handle, &FindData) != false)
	{
		// make sure we dont go off the end of the array
		if (FileNum >= MAX_FILES_ON_CARD)
			break;

		g_MemCardManager->FillOutFileInfo(FileNum, CStringHelper::UnicodeToAscii((short*)&FindData.szSaveGameName), FindData.szSaveGameDirectory,
													 Port, ControllerPort, MU_Name, 0, Date, Time);
		FileNum++;
	}
	return (FileNum);
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::Dashboard()
{
	LD_LAUNCH_DASHBOARD	ld;
	u32						BlocksFree;
	u32						FreeFiles;
	int						BlocksNeeded;

	BlocksNeeded = BlocksNeededForSave();
	g_MemCardMgr.GetCardFreeSpace( g_MemCardManager->GetPort(), FreeFiles, BlocksFree);

	// calc blocks needed
	int	deleteblocks =  BlocksNeeded - BlocksFree;
	g_MemCardManager->SetBlocksToDelete( deleteblocks );

	ld.dwReason = XLD_LAUNCH_DASHBOARD_MEMORY;
	ld.dwContext = 0;
	ld.dwParameter1 = 'U';
	ld.dwParameter2 = g_MemCardManager->GetBlocksToDelete();
	DWORD	result = XLaunchNewImage(NULL, (PLAUNCH_DATA)&ld);
}

//--------------------------------------------------------------//
int	CMemoryCard_Xbox::BlocksNeededForSave()
{
	int	BlocksNeeded = 0;

	BlocksNeeded = g_MemCardMgr.BytesToBlocks(g_MemCardManager->GetFilesize());
	BlocksNeeded += 3 + 1;				// dir, savemeta, saveimage, +1 for hack from MS.. bleh

	return (BlocksNeeded);
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::Detect()
{
	MEMCARD_RESULT	result;

	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());		// TODO (memory units need this)
	g_MemCardManager->SetResult(result);
	g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
}

//--------------------------------------------------------------//
void	CMemoryCard_Xbox::HandleResult()
{
	int				temp;
	MEMCARD_RESULT	result = (MEMCARD_RESULT)g_MemCardManager->GetResult();

	switch(result)
	{
	case	MEMCARD_RESULT_NONE:
		g_MemCardManager->SetResult(MEMCARD_RESULT_OK);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;

	case	MEMCARD_RESULT_OK:
		// display appropriate message
		if (g_MemCardManager->GetAutosave() == false)
		{
			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVE_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
			else if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_LOAD)
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_LOAD_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
			else
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_COMPLETED_OK, MC_OK, MC_OK, MC_OK);

			g_MemCardManager->DisplayPopup(CMEMCARDMSG_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
			WaitOnResult();
		}
		g_MemCardManager->SetPopupExitResult(PR_COMPLETED_OK);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;

	case	MEMCARD_RESULT_NOCARD:
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

//pab			g_MemCardManager->SetNoSave(true);
			break;
		}
		break;

	case	MEMCARD_RESULT_FAILED:
		// display appropriate message
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_COMMAND_FAILED, MC_OK, MC_OK, MC_OK);
		WaitOnResult();
		g_MemCardManager->SetPopupExitResult(PR_FAILED);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;

	case	MEMCARD_RESULT_NODATA:
		// display appropriate message
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_VALID_SAVE, MC_OK, MC_OK, MC_OK);
		WaitOnResult();
		g_MemCardManager->SetPopupExitResult(PR_NO_VALID_SAVE_CANCEL);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;

	case	MEMCARD_RESULT_BADDATA:

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CORRUPT_CARD, MC_CANCEL, MC_CANCEL, MC_CANCEL, MC_DELETE);
		else
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CORRUPT_CARD, MC_CANCEL, MC_CANCEL, MC_CANCEL);
		WaitOnResult();

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
		{
			if (g_MemCardManager->PopupResult() == MC_CANCEL)
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
		else
		{
			g_MemCardManager->SetPopupExitResult(PR_CORRUPT_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_BADCARD:
			if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_DAMAGED_CARD, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
			WaitOnResult();

			if (g_MemCardManager->PopupResult() == MC_RETRY)
				return;
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab				g_MemCardManager->SetNoSave(true);
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
				g_MemCardManager->RemovePopup();
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
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
		
	default:
		// set back to idle
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;
	}
}

//--------------------------------------------------------------//
MEMCARD_RESULT	CMemoryCard_Xbox::WaitOnResult()
{
	MEMCARD_RESULT	CardChanged = MEMCARD_RESULT_OK;

	while (g_MemCardManager->PopupResult() == MC_NONE )
	{
		g_MemCardManager->GetThread()->YieldThread(1);

		if (g_MemCardManager->GetCancelOperation())
		{
			CardChanged = MEMCARD_RESULT_CANCEL_OPERATION;
			g_MemCardManager->SetCancelOperation(false);
			break;
		}
	}

	return CardChanged;
}

#endif // XBOX


