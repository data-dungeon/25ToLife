//--------------------------------------------------------------//
// MemoryCard_win32.cpp
//
// Memory Card Handler (platform specific)
// Author: Paul Blagay
//
//--------------------------------------------------------------//

#include "platform/PlatformPCH.h"

#if defined (WIN32) && !defined (_XBOX)

#include "platform/MemoryCard/MemoryCard.h"
#include "platform/MemoryCard/MemoryCard_Win32.h"
#include "Layers/thread/thread.h"
#include "EngineHelper/Hash.h"
#include "GameHelper/encryption/Helix/Helix.h"

// memcard text files language specific
#include "Memcard_english_common.h"
#include "memcard_english_win32.h"

#ifdef DIRECTX_PC
#include <shlobj.h>

#define SAVE_GAME_LOCATION "\\25 to life"

#endif

//--------------------------------------------------------------//
// message formatting

ts_CMemCardMsg	CMemCardMsg[] = {
	{	MC_NO_VALID_SAVE_GAME,	CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},							
	{	MC_MEMCARD_FULL,			CMEMORYCARD_SAVE_TYPE,		CMEMORYCARD_BLOCKS_NEEDED, -1 							-1},
	{	MC_MEMCARD_FULL_MULTIPLE, CMEMORYCARD_SAVE_TYPE,	CMEMORYCARD_BLOCKS_NEEDED,	CMEMORYCARD_BLOCKS_ALL,	-1},
	{	MC_NO_CARD_SAVE,			CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_BLOCKS_NEEDED,	-1 							-1},								
	{	MC_NO_CARD_LOAD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1 							-1},								
	{	MC_AUTOSAVE_NO_CARD,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1 							-1},								
	{	MC_SURE_LOAD,				CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},
	{	MC_SURE_SAVE,				CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},
	{	MC_SURE_DELETE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},
	{	MC_SURE_OVERWRITE,		CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},
	{	MC_LOADING_FILE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},
	{	MC_SAVING_FILE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},
	{	MC_OVERWRITING_FILE,		CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},
	{	MC_DELETE_FILE	,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},
	{	MC_LOADING_CANCELLED,	-1,								-1,								-1 							-1},
	{	MC_SAVING_CANCELLED,		-1,								-1,								-1 							-1},
	{	MC_DELETE_CANCELLED,		-1,								-1,								-1 							-1},
	{	NULL,							-1,								-1,								-1 							-1},								// format cancelled
	{	NULL,							-1,								-1,								-1 							-1},								// formatting
	{	NULL,							-1,								-1,								-1 							-1},								// sure format	
	{	MC_ACCESSING_CARD,		-1,								-1,								-1 							-1},
	{	NULL,							-1,								-1,								-1 							-1},								// detect card
	{	NULL,							-1,								-1,								-1 							-1},								// other market
	{	NULL,							-1,								-1,								-1 							-1},								// unformatted
	{	NULL,							-1,								-1,								-1 							-1},								// damaged card
	{	MC_CORRUPT_CARD,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 							-1},								
	{	MC_COMPLETED_OK,			-1,								-1,								-1 							-1},
	{	MC_COMMAND_FAILED,		-1,								-1,								-1 							-1},
	{	NULL,							-1,								-1,								-1 							-1},								// format failed
	{	MC_AUTOSAVE_NO_CARD,		CMEMORYCARD_CONTROL_PORT,	CMEMORYCARD_MEMCARD_PORT,	-1 							-1},			
	{	NULL,							-1,								-1,								-1 							-1},								// unformatted card ok
	{	MC_FORMAT_COMPLETED_OK,	-1,								-1,								-1 							-1},		
	{	MC_DELETE_COMPLETED_OK,	-1,								-1,								-1 							-1},		
	{	MC_LOAD_COMPLETED_OK,	-1,								-1,								-1 							-1},		
	{	MC_SAVE_COMPLETED_OK,	-1,								-1,								-1 							-1},		
	{	NULL,							-1,								-1,								-1 							-1},								// card changed since last save
	{  NULL,							-1,								-1,								-1 							-1},
	{  NULL,							-1,							   -1,								-1 							-1},
	{  NULL,							-1,							   -1,								-1 							-1},
	{  MC_AUTOSAVING,				-1,								-1,								-1								-1},								// autosaving
	{  MC_SAVE_FAILED,			-1,								-1,								-1								-1},								// failed.
	{  NULL,							-1,								-1, 								-1,							-1 },			// PS2 
	{  NULL,							-1,								-1,								-1, 							-1 },			// PS2
	{  NULL,							-1,								-1,								-1, 							-1 },			// PS2
	{	NULL,							-1,								-1,								-1,							-1 },			// PS2
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

//--------------------------------------------------------------//

int	MemCardThreadFunc()
{
	MEMCARD_RESULT		result;

	g_MemCardManager->GetThread()->YieldThread(1);

	ASSERTS (g_MemCardManager->GetThread(), "MemCard thread is not ready.");

	// get latest active counts
	for (int i = 0; i < g_MemCardManager->GetNumCardsToPoll(); i++)
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

	while (1)
	{
		switch (g_MemCardManager->GetState())
		{
		case	g_MemCardManager->CMEMORYCARD_IDLE:
			break;
		case	g_MemCardManager->CMEMORYCARD_LOAD:
			CMemoryCard_Win32::Load();
			break;
		case	g_MemCardManager->CMEMORYCARD_SAVE:
			CMemoryCard_Win32::Save();
			break;
		case	g_MemCardManager->CMEMORYCARD_DELETE:
			CMemoryCard_Win32::Delete();
			break;
		case	g_MemCardManager->CMEMORYCARD_GET_FILES_ON_CARD:
			CMemoryCard_Win32::GetFilesOnCard();
			break;
		case	g_MemCardManager->CMEMORYCARD_GET_FILES_ON_ALL_CARDS:
			CMemoryCard_Win32::GetFilesOnAllCards();
			break;
		case	g_MemCardManager->CMEMORYCARD_FILE_EXISTS:
			CMemoryCard_Win32::Exists();
			break;
		case	g_MemCardManager->CMEMORYCARD_DETECT:
			CMemoryCard_Win32::Detect();
			break;
		case	g_MemCardManager->CMEMORYCARD_HANDLE_RESULT:
			CMemoryCard_Win32::HandleResult();
			break;
		case	g_MemCardManager->CMEMORYCARD_STARTUP:
			CMemoryCard_Win32::StartupCheck();
			break;
		default:
			break;
		}

		// sleep a while
		g_MemCardManager->GetThread()->YieldThread(1);

		// exit thread?
		if (g_MemCardManager->ExitThread())
			break;
	}

	// kill thread
	g_MemCardManager->GetThread()->ExitDelete();

	return 0;
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::StartupCheck()
{
	u32					BlocksNeeded;
	MEMCARD_RESULT		result;

	OverWrite = false;

	g_MemCardManager->SetNoSave(false);

	BlocksNeeded = g_MemCardMgr.GetBlocksForFileSize( g_MemCardManager->GetFilesize());
	result = g_MemCardMgr.CheckSpace( g_MemCardManager->GetPort(), g_MemCardManager->GetFilesize() , BlocksNeeded);

	g_MemCardManager->SetPopupTitle("Checking Space");
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
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING);
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
		}
	}
	else
	{
		g_MemCardManager->SetResult(result);
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
	}
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::PollCards()
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
void	CMemoryCard_Win32::Load()
{
	MEMCARD_RESULT	result;

	OverWrite = false;

	// set filename
	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());
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
	if (result == MEMCARD_RESULT_OK)
	{
		ts_MemcardHeader*	hdr = (ts_MemcardHeader*) g_MemCardManager->GetLoadSaveBuffer();

		if (g_MemCardManager->GetLoadHeader() == false)
		{
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
	else
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
	}
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::Save()
{
	MEMCARD_RESULT	result;
	u32				BlocksNeeded;
	bool				rr;

	OverWrite = false;

	// set filename
	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());
	g_MemCardManager->SetPopupTitle("Save");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetCancelOperation(false);

	// are you sure
	if (g_MemCardManager->GetAreYouSure() == true)
	{
		while (1)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_SAVE, MC_YES, MC_NO, MC_YES, MC_NO);
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
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// do we have enough space on the device..
	BlocksNeeded = g_MemCardMgr.GetBlocksForFileSize( g_MemCardManager->GetFilesize());
	result = g_MemCardMgr.CheckSpace( g_MemCardManager->GetPort(), g_MemCardManager->GetFilesize(), BlocksNeeded);

	// call failed - bad drive?
	if (result == MEMCARD_RESULT_BADPARAM)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_COMMAND_FAILED, MC_OK, MC_OK, MC_OK);
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

		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetResult(result);
		g_MemCardManager->SetPopupExitResult(PR_BADPARAM);
		HandleResult();
	}

	// not enough space
	if (result == MEMCARD_RESULT_NOSPACE)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING, MC_CONTINUE_WITHOUT_SAVING);
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
//pab			g_MemCardManager->SetNoSave(true);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetResult(result);
			g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
			HandleResult();
			return;
		}
	}

	// autosave, dont say overwriting
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

					g_MemCardManager->RemovePopup();
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
			}
		}
	}

	// compress data?
	if (g_MemCardManager->GetCompress() == true)
		rr = g_MemCardManager->CompressData(g_MemCardManager->GetCompressionType());

	if (g_MemCardManager->GetEncrypt() == true)
		rr = g_MemCardManager->EncryptData(g_MemCardManager->GetEncryptionType());

	// make a new buffer with our header in
	ts_MemcardHeader*	hdr = (ts_MemcardHeader*) g_MemCardManager->GetLoadSaveBuffer();
	hdr->version = MEMCARD_VERSION;
	hdr->compressed = g_MemCardManager->GetCompress();
	hdr->encrypted = g_MemCardManager->GetEncrypt();
	hdr->encryption_type = g_MemCardManager->GetEncryptionType();					// always helix for now.. todo
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
#ifdef DIRECTX_PC
	if( g_MemCardManager->GetUsePopups() )
	{
#endif
		g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
		if (OverWrite == false)
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVING_FILE, MC_NONE);
		else
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_OVERWRITING_FILE, MC_NONE);
#ifdef DIRECTX_PC
	}
#endif
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
			// recopy the data..
			g_MemCardManager->RecopyDataBuffer();
			return;
		}
	}

	g_MemCardManager->RemovePopup();

	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::Exists()
{
	MEMCARD_RESULT	result = MEMCARD_RESULT_OK;

	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());

		// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// exists
	result = g_MemCardMgr.Exists(g_MemCardManager->GetPort()); 
	g_MemCardManager->SetResult(result);
	g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::Detect()
{
	MEMCARD_RESULT	result;

	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());		
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);
	g_MemCardManager->SetResult(result);
	g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
	g_MemCardManager->RemovePopup();
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::Delete()
{
	MEMCARD_RESULT	result = MEMCARD_RESULT_OK;

	OverWrite = false;

	g_MemCardMgr.SetFileName(g_MemCardManager->GetFilename());
	g_MemCardMgr.UpdateFileComment(g_MemCardManager->GetComment());
	g_MemCardManager->SetPopupTitle("Delete");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

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
			g_MemCardManager->RemovePopup();
			HandleResult();
			return;
		}
		else
			g_MemCardManager->RemovePopup();
	}

	// delete
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_DELETE_FILE, MC_NONE);
	result = g_MemCardMgr.Delete(g_MemCardManager->GetPort());
	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::GetFilesOnAllCards()
{
	HANDLE				Handle;
	WIN32_FIND_DATA	FindData;
	int					FileNum = 0;
	ts_CMemCardDate	Date;
	ts_CMemCardTime	Time;
	SYSTEMTIME			SysTime;
	MEMCARD_RESULT		result;

	g_MemCardManager->SetPopupTitle("Finding Files");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	// enumerate save games
	char	Path[256];
	
#ifdef DIRECTX_PC
	char myDocumentsLocation[MAX_PATH];
	HRESULT res = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
	ASSERT(res == S_OK);
	strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
	sprintf(Path, "%s\\Savegames\\*", myDocumentsLocation );
#else
	sprintf(Path, "%s\\SaveGames\\*", getenv(WIN32_PLATFORM_ENV));
#endif
	Handle = FindFirstFile(Path, &FindData);						// gets .
	FindNextFile(Handle, &FindData);									// get ..

	// re-detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// no save games?
	if (FindNextFile(Handle, &FindData) == false)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_NOFILES);
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_HANDLE_RESULT);
		g_MemCardManager->SetNumFilesOnCard(0);
		return; 
	}

	// setup date and time
	BOOL	rr = FileTimeToSystemTime( &FindData.ftLastWriteTime, &SysTime);
	Time.Hour    = SysTime.wHour;
	Time.Minutes = SysTime.wMinute;
	Time.Seconds = SysTime.wSecond;
	Date.Day     = SysTime.wDay;
	Date.Month   = SysTime.wMonth;
	Date.Year    = SysTime.wYear;

	// we have a save game, save it off and search for any more
	g_MemCardManager->FillOutFileInfo(FileNum, g_MemCardManager->GetFilename(), FindData.cFileName, g_MemCardManager->GetPort(),
												 0, "none", FindData.nFileSizeLow, Date, Time);
	FileNum++;

	// loop through all other save games
	while ( FindNextFile(Handle, &FindData) != false)
	{
		// make sure we dont go off the end of the array
		if (FileNum >= MAX_FILES_ON_CARD)
			break;

		g_MemCardManager->FillOutFileInfo(FileNum, g_MemCardManager->GetFilename(), FindData.cFileName, g_MemCardManager->GetPort(), 
													 0, "none", FindData.nFileSizeLow, Date, Time);
		FileNum++;
	}

	g_MemCardManager->SetNumFilesOnCard(FileNum);
	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::GetFilesOnCard()
{
	HANDLE				Handle;
	WIN32_FIND_DATA	FindData;
	int					FileNum = 0;
	ts_CMemCardDate	Date;
	ts_CMemCardTime	Time;
	SYSTEMTIME			SysTime;
	MEMCARD_RESULT		result;

	g_MemCardManager->SetPopupTitle("Finding Files");
	g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_IN_PROGRESS);

	// enumerate save games
	char	Path[256];
	
#ifdef DIRECTX_PC
	char myDocumentsLocation[MAX_PATH];
	HRESULT res = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, myDocumentsLocation);
	ASSERT(res == S_OK);
	strcat(myDocumentsLocation, SAVE_GAME_LOCATION);
	sprintf(Path, "%s\\Savegames\\*", myDocumentsLocation);
#else
	sprintf(Path, "%s\\SaveGames\\*", getenv(WIN32_PLATFORM_ENV));
#endif
	Handle = FindFirstFile(Path, &FindData);						// gets .
	FindNextFile(Handle, &FindData);									// get ..

	// re-detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// no save games?
	if (FindNextFile(Handle, &FindData) == false)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_NOFILES);
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_HANDLE_RESULT);
		return;
	}

	// setup date and time
	BOOL rr = FileTimeToSystemTime( &FindData.ftLastWriteTime, &SysTime);
	Time.Hour    = SysTime.wHour;
	Time.Minutes = SysTime.wMinute;
	Time.Seconds = SysTime.wSecond;
	Date.Day     = SysTime.wDay;
	Date.Month   = SysTime.wMonth;
	Date.Year    = SysTime.wYear;

	// we have a save game, save it off and search for any more
	g_MemCardManager->FillOutFileInfo(FileNum, g_MemCardManager->GetFilename(), FindData.cFileName, g_MemCardManager->GetPort(),
												 0, "none", FindData.nFileSizeLow, Date, Time);
	FileNum++;

	// loop through all other save games
	while ( FindNextFile(Handle, &FindData) != false)
	{
		// make sure we dont go off the end of the array
		if (FileNum >= MAX_FILES_ON_CARD)
			break;

		g_MemCardManager->FillOutFileInfo(FileNum, g_MemCardManager->GetFilename(), FindData.cFileName, g_MemCardManager->GetPort(),
													 0, "none", FindData.nFileSizeLow, Date, Time);
		FileNum++;
	}

	g_MemCardManager->SetNumFilesOnCard(FileNum);
	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_Win32::HandleResult()
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
#ifdef DIRECTX_PC
			if( g_MemCardManager->GetUsePopups() )
			{
#endif
				if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVE_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
				else if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_LOAD)
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_LOAD_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
				else
					g_MemCardManager->DisplayPopup(CMEMCARDMSG_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
#ifdef DIRECTX_PC
			}
#endif
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
//pab			g_MemCardManager->SetNoSave(true);
			g_MemCardManager->SetPopupExitResult(PR_NOCARD_CANCEL);
			g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
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

	case	MEMCARD_RESULT_FAILED:
		// display appropriate message
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_COMMAND_FAILED, MC_OK, MC_OK, MC_OK);
		WaitOnResult();
		g_MemCardManager->SetPopupExitResult(PR_FAILED);
		g_MemCardManager->SetInProgress(g_MemCardManager->CMEMORYCARD_NOTHING_IN_PROGRESS);
		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;

	case	MEMCARD_RESULT_BADDATA:
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CORRUPT_CARD, MC_CANCEL, MC_CANCEL, MC_CANCEL, MC_DELETE);
		else
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CORRUPT_CARD, MC_OK, MC_OK, MC_OK);

		WaitOnResult();

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
		else
		{
			if (g_MemCardManager->PopupResult() == MC_CANCEL ||
			    g_MemCardManager->PopupResult() == MC_NO || 
			    g_MemCardManager->PopupResult() == MC_OK )
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
MEMCARD_RESULT	CMemoryCard_Win32::WaitOnResult()
{
	MEMCARD_RESULT	CardChanged = MEMCARD_RESULT_OK;

	if (g_MemCardManager->GetUsePopups())
	{
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
	}
	return CardChanged;
}

#endif // XBOX


