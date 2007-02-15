//--------------------------------------------------------------//
// MemoryCard_GCN.cpp
//
// Memory Card Handler (platform specific)
// Author: Paul Blagay
//
//--------------------------------------------------------------//

#include "platform/PlatformPCH.h"

#if defined (GCN)

#include "platform/MemoryCard/MemoryCard.h"
#include "platform/MemoryCard/MemoryCard_gcn.h"
#include "Layers/thread/thread.h"
#include "EngineHelper/Hash.h"
#include "GameHelper/encryption/Helix/Helix.h"

// memcard text files language specific
#include "memcard_english_gcn.h"
#include "Memcard_english_common.h"

//--------------------------------------------------------------//
// message formatting

ts_CMemCardMsg	CMemCardMsg[] = {
	{	MC_NO_VALID_SAVE_GAME,	CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_SAVE_TYPE,		-1 },							
	{	MC_MEMCARD_FULL,			CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_BLOCKS_NEEDED, -1 },
	{	MC_MEMCARD_FULL_MULTIPLE, CMEMORYCARD_MEMCARD_PORT,CMEMORYCARD_BLOCKS_NEEDED,	CMEMORYCARD_BLOCKS_ALL },
	{	MC_NO_CARD_SAVE,			CMEMORYCARD_MEMCARD_PORT,	CMEMORYCARD_BLOCKS_NEEDED,	-1 },								
	{	MC_NO_CARD_LOAD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_AUTOSAVE_NO_CARD,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_SURE_LOAD,				CMEMORYCARD_SAVE_TYPE,		-1,								-1 },
	{	MC_SURE_SAVE,				CMEMORYCARD_SAVE_TYPE,		-1,								-1 },
	{	MC_SURE_DELETE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 },
	{	MC_SURE_OVERWRITE,		CMEMORYCARD_SAVE_TYPE,		-1,								-1 },
	{	MC_LOADING_FILE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 },
	{	MC_SAVING_FILE,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 },
	{	MC_OVERWRITE_FILE,		CMEMORYCARD_SAVE_TYPE,		CMEMORYCARD_MEMCARD_PORT,	-1 },
	{	MC_DELETE_FILE	,			CMEMORYCARD_SAVE_TYPE,		-1,								-1 },
	{	MC_LOADING_CANCELLED,	-1,								-1,								-1 },
	{	MC_SAVING_CANCELLED,		-1,								-1,								-1 },
	{	MC_DELETE_CANCELLED,		-1,								-1,								-1 },
	{	MC_FORMAT_CANCELLED,		-1,								-1,								-1 },								
	{	MC_FORMATTING,				CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_SURE_FORMAT,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_ACCESSING_CARD,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },
	{	MC_DETECT_CARD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_OTHER_MARKET,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_UNFORMATTED_CARD,		CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_DAMAGED_CARD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_CORRUPT_CARD,			CMEMORYCARD_MEMCARD_PORT,	-1,								-1 },								
	{	MC_COMPLETED_OK,			-1,								-1,								-1 },
	{	NULL,							-1,								-1,								-1 },			// command failed
	{	MC_FORMAT_FAILED,			-1,								-1,								-1 },		
	{	MC_AUTOSAVE_NO_CARD,		CMEMORYCARD_CONTROL_PORT,	CMEMORYCARD_MEMCARD_PORT,	-1 },			
	{	MC_UNFORMATTED_CARD_OK,	-1,								-1,								-1 },		
	{	MC_FORMAT_COMPLETED_OK,	-1,								-1,								-1 },		
	{	MC_DELETE_COMPLETED_OK,	-1,								-1,								-1 },		
	{	MC_LOAD_COMPLETED_OK,	-1,								-1,								-1 },		
	{	MC_SAVE_COMPLETED_OK,	-1,								-1,								-1 },		
};

static	int	PollCount = 0;
static	bool	CardsActive[MAX_MEMCARDS] = {false};
static	bool	FormatDone = false;

//--------------------------------------------------------------//

void	MemCardThreadFunc(void* data)
{
	MEMCARD_RESULT		result;
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

	while (1)
	{
		switch (g_MemCardManager->GetState())
		{
		case	g_MemCardManager->CMEMORYCARD_IDLE:
			if (FormatDone)
				FormatDone = false;
			break;
		case	g_MemCardManager->CMEMORYCARD_DETECT:
			CMemoryCard_GCN::Detect();
			break;
		case	g_MemCardManager->CMEMORYCARD_LOAD:
			CMemoryCard_GCN::Load();
			break;
		case	g_MemCardManager->CMEMORYCARD_SAVE:
			CMemoryCard_GCN::Save();
			break;
		case	g_MemCardManager->CMEMORYCARD_DELETE:
			CMemoryCard_GCN::Delete();
			break;
		case	g_MemCardManager->CMEMORYCARD_FORMAT:
			CMemoryCard_GCN::Format();
			break;
		case	g_MemCardManager->CMEMORYCARD_FILE_EXISTS:
			CMemoryCard_GCN::Exists();
			break;
		case	g_MemCardManager->CMEMORYCARD_GET_FILES_ON_CARD:
			CMemoryCard_GCN::GetFilesOnCard();
			break;
		case	g_MemCardManager->CMEMORYCARD_GET_FILES_ON_ALL_CARDS:
			CMemoryCard_GCN::GetFilesOnAllCards();
			break;
		case	g_MemCardManager->CMEMORYCARD_HANDLE_RESULT:
			CMemoryCard_GCN::HandleResult();
			break;
		case	g_MemCardManager->CMEMORYCARD_STARTUP:
			CMemoryCard_GCN::StartupCheck();
			break;
		default:
			break;
		}

		// sleep a while
		g_MemCardManager->GetThread()->YieldThread(100);

		// if card polling is on, poll them every once in a while
		if (g_MemCardManager->GetCardPollingOn())
		{
			if ( (PollCount % 2) == 0)
				CMemoryCard_GCN::PollCards();

			PollCount++;
		}

		// exit thread?
		if (g_MemCardManager->ExitThread())
			break;
	}

	// kill thread
	g_MemCardManager->GetThread()->ExitDelete();
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::StartupCheck()
{
	u32					BlocksNeeded;
	MEMCARD_RESULT		result;

	BlocksNeeded = g_MemCardMgr.GetBlocksForFileSize(0, g_MemCardManager->GetFilesize());
	result = g_MemCardMgr.CheckSpace( g_MemCardManager->GetPort(), g_MemCardManager->GetFilesize() , BlocksNeeded);

	g_MemCardManager->SetPopupTitle("Checking Space");

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
	}
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::PollCards()
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
			g_MemCardManager->SetCardChanged(true);
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
void	CMemoryCard_GCN::Load()
{
	MEMCARD_RESULT	result;
	char				ProfName[48];

	g_MemCardManager->SetPopupTitle("Load");

	// detect card
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	
	if (result != MEMCARD_RESULT_OK)
	{
		g_MemCardManager->SetResult(result);
		HandleResult();
		return;
	}

	// set filename
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
	result = g_MemCardMgr.Load( g_MemCardManager->GetPort(), g_MemCardManager->GetLoadSaveBuffer(),
										 g_MemCardManager->GetFilesize() + sizeof(ts_MemcardHeader) + g_MemCardManager->GetHeaderSize() + g_helix.GetEncryptionHeaderSize(),
										 READ_ALL);
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
	if (hdr->checksum != Checksum)
	{
		g_MemCardManager->SetResult(MEMCARD_RESULT_BADDATA);
		HandleResult();
		return;
	}

	// uncompress data?
	if (result == MEMCARD_RESULT_OK)
	{
		ts_MemcardHeader*	hdr = (ts_MemcardHeader*) g_MemCardManager->GetLoadSaveBuffer();

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

		if (g_MemCardManager->GetCompress() == false && g_MemCardManager->GetEncrypt() == false)
			memcpy(g_MemCardManager->GetBuffer(), g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), g_MemCardManager->GetFilesize() + g_MemCardManager->GetHeaderSize() );

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
void	CMemoryCard_GCN::Save()
{
	MEMCARD_RESULT	result;
	u32				BlocksNeeded;
	bool				OverWrite = false;
	char				ProfName[48];
	int				NumFiles;
	int				MaxFiles;

	g_MemCardManager->SetPopupTitle("Save");

	// set filename
	sprintf(ProfName, "%s%s", g_MemCardManager->GetComment(), g_MemCardManager->GetExt());
	g_MemCardMgr.SetFileName(ProfName);

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
				// autosave needs to be turned off here if they say continue without saving
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_AUTOSAVE_REMOVED, MC_CONTINUE_WITHOUT_SAVING, MC_ENABLE_AUTOSAVE, MC_ENABLE_AUTOSAVE, MC_CONTINUE_WITHOUT_SAVING);
				WaitOnResult();

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

	// get size of file + dummy file
	u32 Size = 0;
	if (g_MemCardManager->GetFirstSave() == true)
		 Size = g_MemCardMgr.GetBlocksForFileSize(0, g_MemCardManager->GetFilesize());
	else
		 Size = g_MemCardMgr.GetBlocksForFileSize(1, g_MemCardManager->GetFilesize());

	// do we have enough space on the device..
	result = g_MemCardMgr.CheckSpace( g_MemCardManager->GetPort(), Size *  MEMCARD_BLOCK_SIZE	 , Size);

	// not enough space
	if (result == MEMCARD_RESULT_NOSPACE)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_CONTINUE_WITHOUT_SAVING)
		{
//pab			g_MemCardManager->SetNoSave(true);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
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
//	result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask());
//	NumFiles = g_MemCardMgr.GetNumFilesFound();

	if (g_MemCardManager->GetFirstSave() == true)
		MaxFiles = MC_MAX_FILES_DIR - 1;
	else
		MaxFiles = MC_MAX_FILES_DIR;

	if (NumFiles >= MaxFiles)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_MEMCARD_FULL, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_CONTINUE_WITHOUT_SAVING)
		{
//pab			g_MemCardManager->SetNoSave(true);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
			g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
			HandleResult();
			return;
		}
		else
		{			// try again homes
			g_MemCardManager->RemovePopup();
			return;
		}
	}

	// are you sure
	if (g_MemCardManager->GetAreYouSure() == true && FormatDone == false)
	{
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_SAVE, MC_YES, MC_NO, MC_YES, MC_NO);
		WaitOnResult();

		if (g_MemCardManager->PopupResult() == MC_NO)
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


	if (g_MemCardManager->GetAutosave() == false)
	{
		// does file exists already
		result = g_MemCardMgr.Exists(g_MemCardManager->GetPort());

		// overwrite?
		if (result == MEMCARD_RESULT_OK)
		{	
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_OVERWRITE, MC_YES, MC_NO, MC_YES, MC_NO);
			WaitOnResult();

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

	// create checksum
	int	Checksum = Hash::CalcChecksum( (g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader)),g_MemCardManager->GetFilesize());
	hdr->checksum = Checksum;

	if (g_MemCardManager->GetCompress() == false && g_MemCardManager->GetEncrypt() == false)
		memcpy(g_MemCardManager->GetLoadSaveBuffer() + sizeof(ts_MemcardHeader), g_MemCardManager->GetBuffer(), g_MemCardManager->GetFilesize() + g_MemCardManager->GetHeaderSize());

	// display popup
	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	if (OverWrite == false)
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_SAVING_FILE, MC_NONE);
	else
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_OVERWRITING_FILE, MC_NONE);

	// start save
	if (g_MemCardManager->GetCompress())
		result = g_MemCardMgr.Save( g_MemCardManager->GetPort(), g_MemCardManager->GetLoadSaveBuffer(), g_MemCardManager->GetCompressedSize() + sizeof(ts_MemcardHeader) + g_MemCardManager->GetHeaderSize());
	else
		result = g_MemCardMgr.Save( g_MemCardManager->GetPort(), g_MemCardManager->GetLoadSaveBuffer(), g_MemCardManager->GetFilesize() + sizeof(ts_MemcardHeader) + g_MemCardManager->GetHeaderSize() + g_helix.GetEncryptionHeaderSize());

	// if this is first save, save the dummy baslus file
	if (g_MemCardManager->GetFirstSave() == true)
	{
		sprintf(ProfName, "%s", g_MemCardManager->GetTitleID());
		g_MemCardMgr.SetFileName(ProfName);
		result = g_MemCardMgr.Save( g_MemCardManager->GetPort(), g_MemCardManager->GetBuffer(), 32);
		g_MemCardManager->SetFirstSave(false);
	}

	g_MemCardManager->RemovePopup();

	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::Exists()
{
	MEMCARD_RESULT	result;
	char				ProfName[48];

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
void	CMemoryCard_GCN::Delete()
{
	MEMCARD_RESULT	result;
	char				ProfName[48];

	g_MemCardManager->SetPopupTitle("Delete");

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
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_SURE_DELETE, MC_YES, MC_NO, MC_YES, MC_NO);
	WaitOnResult();

	if (g_MemCardManager->PopupResult() == MC_NO)
	{
		g_MemCardManager->RemovePopup();
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_DELETE_CANCELLED, MC_OK, MC_OK, MC_OK);
		WaitOnResult();

		g_MemCardManager->RemovePopup();
		g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
		g_MemCardManager->SetPopupExitResult(PR_DELETE_NO);
		g_MemCardManager->RemovePopup();
		return;
	}
	else
		g_MemCardManager->RemovePopup();

	// display popup
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_DELETE_FILE, MC_NONE);

	// delete
	result = g_MemCardMgr.Delete(g_MemCardManager->GetPort());
	g_MemCardManager->RemovePopup();

	g_MemCardManager->SetResult(result);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::Format()
{
	MEMCARD_RESULT	result;

	g_MemCardManager->SetPopupTitle("Format");

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
int	CMemoryCard_GCN::BlocksNeededForSave()
{
	int	BlocksNeeded = 0;

	BlocksNeeded = g_MemCardMgr.GetBlocksForFileSize(0, g_MemCardManager->GetFilesize());
	return (BlocksNeeded);
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::Detect()
{
	MEMCARD_RESULT	result;

	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	g_MemCardManager->SetResult(result);
	g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::GetFilesOnCard()
{
	MEMCARD_RESULT		result;
	int					FileNum = 0;

	g_MemCardManager->SetPopupTitle("Finding Files");

	// detect first
	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_DETECT_CARD, MC_NONE);
	result = g_MemCardMgr.Detect(g_MemCardManager->GetPort());
	g_MemCardManager->RemovePopup();
	g_MemCardManager->SetResult(result);
	HandleResult();

	// get directory listing
	g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
	g_MemCardManager->DisplayPopup(CMEMCARDMSG_ACCESSING_CARD, MC_NONE);
//	result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask());
	g_MemCardManager->RemovePopup();
	g_MemCardManager->SetResult(result);
	HandleResult();

	// get num
//	FileNum = g_MemCardMgr.GetNumFilesFound();
//	DirTable = g_MemCardMgr.GetDirTable();

	// fill out the info
	for ( int i = 0 ; i < FileNum ; i++)
	{
//		g_MemCardManager->FillOutFileInfo(i, (char*)&DirTable[i].EntryName, ".", g_MemCardManager->GetPort(),
//													 0, "none", (int)DirTable[i].FileSizeByte, Date, Time );
	}

	// ok done
	g_MemCardManager->SetNumFilesOnCard(FileNum);
	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::GetFilesOnAllCards()
{
	MEMCARD_RESULT		result;
	int					FileNum = 0;
	int					i, j;
	int					Port = 0;

	g_MemCardManager->SetPopupTitle("Finding Files");

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
			g_MemCardManager->SetPopupDisplayTime( POPUP_DISPLAY_TIME );
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_ACCESSING_CARD, MC_NONE);
//			result = g_MemCardMgr.GetDirectory( g_MemCardManager->GetPort(), g_MemCardManager->GetSearchMask());

			if (result == MEMCARD_RESULT_OK)
			{
				// get num
//				FileNum = g_MemCardMgr.GetNumFilesFound();
//				DirTable = g_MemCardMgr.GetDirTable();

				// fill out the info
				for ( j = 0 ; j < FileNum ; j++)
				{
//					g_MemCardManager->FillOutFileInfo(j, (char*)&DirTable[j].EntryName, ".", g_MemCardManager->GetPort(),
//																 0, "none", (int)DirTable[j].FileSizeByte, Date, Time);
				}
			}
			g_MemCardManager->RemovePopup();
		}
	}

	// ok done
	g_MemCardManager->SetNumFilesOnCard(FileNum);
	g_MemCardManager->SetResult(MEMCARD_RESULT_NONE);
	HandleResult();
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::HandleResult()
{
	int				temp;

	MEMCARD_RESULT	result = (MEMCARD_RESULT)g_MemCardManager->GetResult();
	MEMCARD_RESULT	old_result = result;

	switch(result)
	{
	case	MEMCARD_RESULT_NONE:
		g_MemCardManager->SetResult(MEMCARD_RESULT_OK);
		g_MemCardManager->SetPopupExitResult(PR_COMPLETED_OK);
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

			WaitOnResult();
			g_MemCardManager->RemovePopup();
		}
		g_MemCardManager->SetPopupExitResult(PR_COMPLETED_OK);
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
		g_MemCardManager->RemovePopup();

		if (g_MemCardManager->PopupResult() == temp)
		{
			g_MemCardManager->SetPopupExitResult(PR_NOCARD_CANCEL);
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_UNFORMATTED:

		// startup.. dont allow format, but allow a new card.. otherwise continue without saving
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_UNFORMATTED_CARD_OK, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
			WaitOnResult();
			g_MemCardManager->RemovePopup();

			if (g_MemCardManager->PopupResult() == MC_RETRY)
				return;
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab				g_MemCardManager->SetNoSave(true);
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				return;
			}
		}

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_UNFORMATTED_CARD, MC_YES, MC_NO, MC_YES, MC_NO);
			WaitOnResult();
			g_MemCardManager->RemovePopup();

			if (g_MemCardManager->PopupResult() == MC_NO)
			{
				g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMAT_CANCELLED, MC_OK, MC_OK, MC_OK);
				WaitOnResult();
				g_MemCardManager->SetPopupExitResult(PR_FORMAT_NO);
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				break;
			}
		}
		else		// no option to format on load
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_UNFORMATTED_CARD_OK, MC_OK, MC_OK, MC_OK);
			WaitOnResult();
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetPopupExitResult(PR_FORMAT_OK);
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

		// start format
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMATTING, MC_NONE);
		result = g_MemCardMgr.Format(g_MemCardManager->GetPort());

		// did it fail??
		if (result != MEMCARD_RESULT_OK)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMAT_FAILED, MC_OK, MC_OK, MC_OK);
			g_MemCardManager->RemovePopup();
			g_MemCardManager->SetResult(result);
			g_MemCardManager->SetPopupExitResult(PR_FORMAT_FAILED);
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}

		FormatDone = true;

		// completed ok
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_FORMAT_COMPLETED_OK, MC_OK, MC_OK, MC_OK);
		WaitOnResult();
		g_MemCardManager->RemovePopup();
		return;

	case	MEMCARD_RESULT_NOFILES:
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		// display appropriate message
		break;

	case	MEMCARD_RESULT_BADCARD:

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_DAMAGED_CARD, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
			WaitOnResult();
			g_MemCardManager->RemovePopup();

			if (g_MemCardManager->PopupResult() == MC_RETRY)
				return;
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab				g_MemCardManager->SetNoSave(true);
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				return;
			}
		}

		g_MemCardManager->DisplayPopup(CMEMCARDMSG_DAMAGED_CARD, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
		WaitOnResult();
		g_MemCardManager->RemovePopup();

		if (g_MemCardManager->PopupResult() == MC_CANCEL)
		{
			g_MemCardManager->SetPopupExitResult(PR_DAMAGED_CANCEL);
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_WRONGTYPE:

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
		g_MemCardManager->RemovePopup();

		if (g_MemCardManager->PopupResult() == temp)
		{
			g_MemCardManager->SetPopupExitResult(PR_NOCARD_CANCEL);
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_OTHERMARKET:

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_STARTUP)
		{
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_OTHER_MARKET, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY, MC_CONTINUE_WITHOUT_SAVING, MC_RETRY);
			WaitOnResult();
			g_MemCardManager->RemovePopup();

			if (g_MemCardManager->PopupResult() == MC_RETRY)
				return;
			else
			{
				g_MemCardManager->SetPopupExitResult(PR_CONTINUE_WITHOUT_SAVING);
//pab				g_MemCardManager->SetNoSave(true);
				g_MemCardManager->SetAutosave(false);
				g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
				return;
			}
		}

		g_MemCardManager->DisplayPopup(CMEMCARDMSG_OTHER_MARKET, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
		WaitOnResult();
		g_MemCardManager->RemovePopup();

		if (g_MemCardManager->PopupResult() == MC_CANCEL)
		{
			g_MemCardManager->SetPopupExitResult(PR_OTHER_MARKET_CANCEL);
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_NODATA:
		g_MemCardManager->DisplayPopup(CMEMCARDMSG_NO_VALID_SAVE, MC_CANCEL, MC_RETRY, MC_RETRY, MC_CANCEL);
		WaitOnResult();
		g_MemCardManager->RemovePopup();

		if (g_MemCardManager->PopupResult() == MC_CANCEL)
		{
			g_MemCardManager->SetPopupExitResult(PR_NO_VALID_SAVE_CANCEL);
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
			break;
		}
		break;

	case	MEMCARD_RESULT_BADDATA:
		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CORRUPT_CARD, MC_CANCEL, MC_CANCEL, MC_CANCEL, MC_DELETE);
		else
			g_MemCardManager->DisplayPopup(CMEMCARDMSG_CORRUPT_CARD, MC_CANCEL, MC_CANCEL, MC_CANCEL);

		WaitOnResult();
		g_MemCardManager->RemovePopup();

		if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_SAVE)
		{
			if (g_MemCardManager->PopupResult() == MC_CANCEL)
			{
				g_MemCardManager->SetPopupExitResult(PR_CORRUPT_CANCEL);
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
			g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		}
	
	default:
		// set back to idle
		g_MemCardManager->SetState(g_MemCardManager->CMEMORYCARD_IDLE);
		break;
	}
}

//--------------------------------------------------------------//
void	CMemoryCard_GCN::WaitOnResult()
{
	while (g_MemCardManager->PopupResult() == MC_NONE )
		g_MemCardManager->GetThread()->YieldThread(100);
}


#endif // GCN
