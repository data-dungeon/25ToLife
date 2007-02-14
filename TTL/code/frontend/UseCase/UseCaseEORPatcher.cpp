///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEORPatcher.cpp
///
///  \brief Implementation of the EORPatcher class, which is responsible 
/// for handling the patcher flow.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseEORPatcher.hpp"         // This use case
#include "UseCaseDisplayDialog.hpp"      // The OK dialog
#include "UseCaseYesNoDialog.hpp"        // The Yes/No dialog
#include "UseCaseEORLogin.hpp"           // Login use case
#include "UseCaseUtility.hpp"            // ShowWaitScreen
#include "platform/PS2/CDllLoader.h"     // DLL loading routines
#include "Patcher/PatchLib/Patcher.h"          // Patcher
#include "Patcher/PatchExt/PatchExtractor.h"  // Patch extractor
#include "platform/wantpal.h"            // Pal Flag
#include "frontend/screens/screen_title_only.h"
#ifdef NETZ
#include "Network/NetZ/NetzConnection.h"
#endif

// The names of the patcher DLL
#define PATCHER_NAME (g_regionUSA ? "PatchLib.rel" : "PatchLibPAL.rel")

///////////////////////////////////////////////////////////////////////////////////
// Patch servers
///////////////////////////////////////////////////////////////////////////////////
static const char* s_hosts_USA[] =
{
	"ttl1.patch.eidos.com",
	"ttl2.patch.eidos.com",
	"ttl3.patch.eidos.com",
};
static const char* s_hosts_Europe[] =
{
	"ttl3.patch.eidos.com",
	"ttl2.patch.eidos.com",
	"ttl1.patch.eidos.com",
};
#if defined(CONSUMER_BUILD)
	#define PATCH_VERSION_PATH (g_regionUSA ? "/TTL/final-version.txt" : "/TTL/finalUK-version.txt")
#else
	#define PATCH_VERSION_PATH (g_regionUSA ? "/TTL/debug-version.txt" : "/TTL/debugUK-version.txt")
#endif

static const char** s_hosts = NULL;
static int s_numHosts = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Simple static function to determine whether another server should be tried
// in response to an error.
///////////////////////////////////////////////////////////////////////////////////////////////////
static bool ShouldRetry(int err)
{
	switch (err)
	{
		// Errors specific to one host; it would be worthwhile trying an alternate host
		case PATCH_ERROR_HOSTNOTFOUND:		// Unable to find requested host (name server doesn't recognize host name)
		case PATCH_ERROR_NOCONNECT:			// connect failed; host may not be currently available
		case PATCH_ERROR_SENDFAIL:			// send failed; host may be having problems
		case PATCH_ERROR_TIMEOUT:			// Network timeout
			return true;

		// Errors indicating general network problems; don't try another host
		case PATCH_ERROR_POLLERROR:			// Error occurred while polling socket (should save more info)
		case PATCH_ERROR_RECVERROR:			// Error occurred while reading on the socket (should save more info)
		case PATCH_ERROR_NAMESERVER_ERROR:	// Unable to resolve a host name because some error (e.g., network is down) prevented lookup
			return false;
	}

	// Unknown error (shouldn't get here if this file is up to date)
	return true;
}

const char* PatchErrorString(int code)
{
	switch (code)
	{
		// Errors specific to one host; it would be worthwhile trying an alternate host
		case PATCH_ERROR_HOSTNOTFOUND:		// Unable to find requested host (name server doesn't recognize host name)
		case PATCH_ERROR_NOCONNECT:			// connect failed; host may not be currently available
		case PATCH_ERROR_SENDFAIL:			// send failed; host may be having problems
		case PATCH_ERROR_TIMEOUT:			// Network timeout
			return "The patching server was not available. Please try connecting again at a later time.";

			// Errors indicating general network problems; don't try another host
		case PATCH_ERROR_POLLERROR:			// Error occurred while polling socket (should save more info)
		case PATCH_ERROR_RECVERROR:			// Error occurred while reading on the socket (should save more info)
			return "A network error occurred. Please try connecting again at a later time.";

		case PATCH_ERROR_NAMESERVER_ERROR:	// Unable to resolve a host name because some error (e.g., network is down) prevented lookup
			return "Unable to look up address of patching server. Please check your network connection and try again.";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EORPatcher::EORPatcher() : 
    _stateMachine(INIT_PATCHER)
{
    _stateMachine.setStateUpdate(INIT_PATCHER,                 &OnlineUI::EORPatcher::_updateInitPatcher);

    _stateMachine.setStateEnter(MEMCARD_PATCH_CHECK,           &OnlineUI::EORPatcher::_enterMemcardPatchCheck);
    _stateMachine.setStateUpdate(MEMCARD_PATCH_CHECK,          &OnlineUI::EORPatcher::_updateMemcardPatchCheck);

    _stateMachine.setStateEnter(LOAD_PATCH_HEADER,             &OnlineUI::EORPatcher::_enterLoadPatchHeader);
    _stateMachine.setStateUpdate(LOAD_PATCH_HEADER,            &OnlineUI::EORPatcher::_updateLoadPatchHeader);

    _stateMachine.setStateEnter(LOAD_PATCH,                    &OnlineUI::EORPatcher::_enterLoadPatch);
    _stateMachine.setStateUpdate(LOAD_PATCH,                   &OnlineUI::EORPatcher::_updateLoadPatch);

    _stateMachine.setStateUpdate(CHECK_LOCAL_VERSION,          &OnlineUI::EORPatcher::_updateLocalVersionCheck);

    _stateMachine.setStateEnter(CHECK_SERVER_VERSION,          &OnlineUI::EORPatcher::_enterServerVersionCheck);
    _stateMachine.setStateUpdate(CHECK_SERVER_VERSION,         &OnlineUI::EORPatcher::_updateServerVersionCheck);

    _stateMachine.setStateEnter(QUERY_PATCH_DOWNLOAD,          &OnlineUI::EORPatcher::_queryPatchDownload);
    _stateMachine.setStateUpdate(QUERY_PATCH_DOWNLOAD,         &OnlineUI::EORPatcher::_updateQueryPatchDownload);

	_stateMachine.setStateEnter(QUERY_NO_TO_DOWNLOAD,          &OnlineUI::EORPatcher::_enterQueryNoDownload);
    _stateMachine.setStateUpdate(QUERY_NO_TO_DOWNLOAD,         &OnlineUI::EORPatcher::_updateQueryNoDownload);

    _stateMachine.setStateUpdate(PATCH_DOWNLOAD,               &OnlineUI::EORPatcher::_updatePatchDownload);

    _stateMachine.setStateEnter(SAVE_PATCH,                    &OnlineUI::EORPatcher::_enterPatchSave);
    _stateMachine.setStateUpdate(SAVE_PATCH,                   &OnlineUI::EORPatcher::_updatePatchSave);

    _stateMachine.setStateUpdate(WAIT_TERMINATE,               &OnlineUI::EORPatcher::_updateWaitTerminate);

    _stateMachine.setStateEnter(ERROR_TERMINATE,               &OnlineUI::EORPatcher::_enterErrorTerminate);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case and load the patcher.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORPatcher::_init( void )
{
	g_FrontEnd.GotoScreen( "SCREEN_ID_TITLE_ONLY" );

	_loadedPatcher = false;
	if( !_progressIndicator("Starting") ) return false;

    _stateMachine.setCurrentState(this, INIT_PATCHER);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_deinit()
{
    // Get rid of the screen
	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenTitleOnly >() )
	{
		g_FrontEnd.EndScreen( 0 );
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_TITLE_ONLY" );

    // Disable patching and clean up patch buffer
    if (_enabledPatching)
    {
        Media::DisablePatching();

        // The patch buffer is deleted by the media system, so all we have to do is zero it out
        _patchBufSize = 0;
        _patchBuffer = 0;
        _enabledPatching = false;
    }
    else
    {
        delete [] _patchBuffer;
        _patchBuffer = 0;
        _patchBufSize = 0;
    }

	// Need to destroy patcher here if the DLL is still loaded in case we are exiting from a fatal error
	if( _loadedPatcher )
	{
		Patcher::Destroy();
		g_DllLoader.Unload(PATCHER_NAME);
		_loadedPatcher = false;
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Pump the patcher every frame to get the number of patches.
/// Called when updating the INIT_PATCHER state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateInitPatcher()
{
	if (!_loadInitPatcher())
    {
        return true;
    }

	_stateMachine.setCurrentState(this, MEMCARD_PATCH_CHECK);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Handle a memory card result error during the patching process
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_handleMemcardPopupResult( int result )
{
	switch( result )
	{
	case PR_COMPLETED_OK:
	case PR_CHOOSE_EXISTING:
		// No error
		return true;
	case PR_LOAD_NO:
	case PR_LOAD_CANCEL:
	case PR_SAVE_NO:
	case PR_SAVE_CANCEL:
	case PR_OVERWRITE_NO:
	case PR_OVERWRITE_CANCEL:
	case PR_DELETE_NO:
	case PR_DELETE_CANCEL:
	case PR_FORMAT_NO:
	case PR_FORMAT_CANCEL:
	case PR_FORMAT_FAILED:
	case PR_FORMAT_OK:
	case PR_CORRUPT_CANCEL:
	case PR_DAMAGED_CANCEL:
	case PR_OTHER_MARKET_CANCEL:
	case PR_WRONG_TYPE_CANCEL:
	case PR_MEMCARD_FULL_CANCEL:
	case PR_CONTINUE_WITHOUT_SAVING:
	case PR_NOCARD_CANCEL:
	case PR_NO_VALID_SAVE_CANCEL:
	case PR_BADPARAM:
	case PR_FAILED:
	case PR_NOCARD_NO:
	case PR_NOCARD_YES:
	case PR_QUIT:
	case PR_CANCEL:
	default:
		_exitPatcher("The most recent patch must be downloaded and saved to a memory card (8MB) (for PlayStation® 2) in order to play online.");
		break;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Load the DLLs needed for the patcher and initialize the patcher.  Utility method called while
/// in the INIT_PATCHER state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_loadInitPatcher()
{
    _patchBuffer = 0;
	_patchBufSize = 0;
    _patchSize = 0;
    _nextHost = 0;

	 if (g_regionUSA)
	 {
		 s_hosts = s_hosts_USA;
		 s_numHosts = sizeof(s_hosts_USA) / sizeof(s_hosts_USA[0]);
	 }
	 else
	 {
		 s_hosts = s_hosts_Europe;
		 s_numHosts = sizeof(s_hosts_Europe) / sizeof(s_hosts_Europe[0]);
	 }

    _enabledPatching = false;

	// Load patcher DLL
    bool success = g_DllLoader.Load(PATCHER_NAME, false);
    if (!success)
    {
        // Load failure
        _stateMachine.setCurrentState(this, WAIT_TERMINATE);
        DisplayDialog::getSingletonPtr()->start("Patching Failed", "Unable to load patcher.");
        return false;
    }

    // Initialize patcher
    if (Patcher::Create(16) != Patcher::PATCH_STATUS_OK)
    {
        // Patcher creation failed, display error dialog
        _stateMachine.setCurrentState(this, WAIT_TERMINATE);
		g_DllLoader.Unload(PATCHER_NAME);
        DisplayDialog::getSingletonPtr()->start("Patching Failed", "Unable to start patcher.");
        return false;
    }

	_loadedPatcher = true;

    // Pump the patcher once to start the helper thread
    Patcher::GetPatcher()->Update(g_timer.GetOSFrameSec());
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Start the check to see how many patches (0 or 1) are on the memory card.  Called when entering
/// the MEMCARD_PATCH_CHECK state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_enterMemcardPatchCheck()
{
    g_MemCardManager->GetFilesOnAllCards("", "*.pat", false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Wait for a result on the memory card patch check code.  Called when updating the 
/// MEMCARD_PATCH_CHECK state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateMemcardPatchCheck()
{
	if( g_regionUSA )
		_progressIndicator("Checking memory card");
	else
		_progressIndicator("Checking");
    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
    {
		g_MemCardManager->ResetLoadSaveBuffer();

        int numPatches = g_MemCardManager->GetNumFilesOnCard();

		// Something bad might have happened, check the popup result
		if (!_handleMemcardPopupResult( g_MemCardManager->GetPopupExitResult() ))
		{
			return true;
		}

        if (0 == numPatches)
        {
            // No patches, on to next state
            _patchVersion = TTL_PATCH_VERSION;
            _stateMachine.setCurrentState(this, CHECK_SERVER_VERSION);
        }
        else
        {
            // There should only be one patch
            ASSERT(1 == numPatches);

            // Load the patch header
            _stateMachine.setCurrentState(this, LOAD_PATCH_HEADER);
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Start loading the patch header.  Called when entering the LOAD_PATCH_HEADER state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_enterLoadPatchHeader()
{
    _patchHeader.uncompressedSize = -1;

    // Get the information on the patch
    g_MemCardManager->GetFileInfo(0, _patchFile);

	g_MemCardManager->SetExt(PATCH_EXT);
	g_MemCardManager->SetUsePopups(true);
	g_MemCardManager->LoadHeader(SAVEGAME_FILENAME, _patchFile.LoadFilename, _patchFile.Port, (char *)&_patchHeader, false, MC_SAVED_PATCH);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Wait for the patch header to finish loading and then load the patch.  Called when
/// updating the LOAD_PATCH_HEADER state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateLoadPatchHeader()
{
	_progressIndicator("Reading saved update");
    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
    {
		g_MemCardManager->ResetLoadSaveBuffer();

        // Something bad might have happened, check the popup result
		if (!_handleMemcardPopupResult( g_MemCardManager->GetPopupExitResult() ))
		{
			return true;
		}

        // Something bad happened while loading the header, bail
        if (_patchHeader.uncompressedSize <= 0)
        {
            _exitPatcher("An error occurred while trying to verify the patch from the memory card (8MB) (for PlayStation® 2).");
            return true;
        }

        // Allocate patch
        _patchBufSize = _patchHeader.uncompressedSize;
        _patchBuffer = new char[_patchBufSize];

        // Load the patch
        _stateMachine.setCurrentState(this, LOAD_PATCH);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Start loading the patch from the memory card.  Called when entering the LOAD_PATCH state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_enterLoadPatch()
{
    // Load patch
	g_MemCardManager->SetExt(PATCH_EXT);
	g_MemCardManager->SetUsePopups(true);
	g_MemCardManager->LoadFile(SAVEGAME_FILENAME, _patchFile.LoadFilename, 0, _patchBuffer, _patchBufSize, false, MC_SAVED_PATCH);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Wait until the patch is loaded from the memory card, then hand it to the patcher.  Called
/// when updating the LOAD_PATCH state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateLoadPatch()
{
	_progressIndicator("Reading saved update");
    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
    {
		g_MemCardManager->ResetLoadSaveBuffer();

        // Something bad might have happened, check the popup result
		if (!_handleMemcardPopupResult( g_MemCardManager->GetPopupExitResult() ))
		{
			return true;
		}

        // Hand patch to patcher to decrypt
        if (Patcher::PATCH_STATUS_OK != Patcher::GetPatcher()->SetPatch(_patchBuffer, _patchBufSize))
        {
            _exitPatcher("An error occurred while trying to verify the patch from the memory card (8MB) (for PlayStation® 2).");
            return true;
        }

        // Tell the patcher to start decrypting the patch
        if (Patcher::PATCH_STATUS_OK != Patcher::GetPatcher()->PreparePatch())
        {
            _exitPatcher("An error occurred while trying to verify the patch from the memory card (8MB) (for PlayStation® 2).");
            return true;
        }

        _stateMachine.setCurrentState(this, CHECK_LOCAL_VERSION);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when updating the CHECK_LOCAL_VERSION state.  Wait until the patch is done decrypting,
/// get its version, and then go to the CHECK_SERVER_VERSION state.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateLocalVersionCheck()
{
    Patcher::CompletionInfo info;
    Patcher::Status status;

    // Pump the patcher
    Patcher::GetPatcher()->Update(g_timer.GetOSFrameSec());
	_progressIndicator("Reading saved update");

    // See if the patch is done decrypting
    status = Patcher::GetPatcher()->TestCompletion(info);
	if (Patcher::PATCH_STATUS_BUSY != status)
	{
		// It's done, get the patch version and go on to the next state
		bool isGood = false;
		if (Patcher::PATCH_STATUS_OK == status)
		{
			// DFS Check this
			// The patch extractor here is being used as a throwaway, but it is
			// computing checksums, etc. It would be good to keep it around and
			// use it later for actual extraction of patches, assuming we don't
			// get another patch to replace it.
			PatchExtractor p(_patchBuffer, info.clearSize);
			isGood = p.IsGood();
			if (isGood)
			{
				_patchVersion = p.QueryVersion();
				_patchSize = info.clearSize;
				_stateMachine.setCurrentState(this, CHECK_SERVER_VERSION);
			}
		}

		// Oops, we have an error reading the patch: re-download it
		if (!isGood)
		{
			delete [] _patchBuffer;
  			_patchBuffer = 0;
  			_patchBufSize = _patchSize = 0;
  			_patchVersion = TTL_PATCH_VERSION;
  
  			_stateMachine.setCurrentState(this, CHECK_SERVER_VERSION);

		}
	}
    return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the CHECK_SERVER_VERSION state.  Kick off a version check
/// for the current patch.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_enterServerVersionCheck()
{
    // Start the version check against our current patch (if any)
    _nextHost = 0;
    if (Patcher::PATCH_STATUS_OK !=
        Patcher::GetPatcher()->BeginVersionCheck(s_hosts[_nextHost++], PATCH_VERSION_PATH, _patchVersion))
    {
        _exitPatcher("There was an error checking the version of the patch.");
        return;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when in the CHECK_SERVER_VERSION state.  Wait for the version check to finish and
/// then go to the QUERY_PATCH_DOWNLOAD state if a patch download is needed.  If no patch is
/// needed, go to the logon screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateServerVersionCheck()
{
    Patcher::CompletionInfo info;
    Patcher::Status status;

    // Pump the patcher
    Patcher::GetPatcher()->Update(g_timer.GetOSFrameSec());
	if (_nextHost)
		_progressIndicator("Trying backup servers");
	else
		_progressIndicator("Checking server for updates");

    // See if the patch is done version checking
    status = Patcher::GetPatcher()->TestCompletion(info);

    // Yes
    if (Patcher::PATCH_STATUS_OK == status)
    {
        // There's a new version of the patch we need
        if (info.reqBufSize != 0)
        {
            // Allocate the patch
            if (_patchBuffer)
            {
                delete [] _patchBuffer;
                _patchBuffer = 0;
            }
            _patchBufSize = info.reqBufSize;
            _patchBuffer = static_cast<char *>(memAlloc(_patchBufSize));
            _stateMachine.setCurrentState(this, QUERY_PATCH_DOWNLOAD);
        }
        // No need to patch, go to the login screen
        else
        {
            // If a patch was already loaded from the memory card, tell the system about the patch
            if (_patchBuffer && _patchSize)
            {
                Media::EnablePatching(_patchBuffer, _patchSize);
                _enabledPatching = true;
#ifdef NETZ
                CNetZConnection::LoadNetworkConfigSettings();
#endif
            }

            // DFS TODO
            // Don't unload the DLLs if we're managing them elsewhere
            Patcher::Destroy();
            g_DllLoader.Unload(PATCHER_NAME);
			_loadedPatcher = false;

            _stateMachine.setCurrentState(this, WAIT_TERMINATE);

            // Start the login process
            EORLogin::getSingletonPtr()->start();
        }
    }
    // No, we have an error
    else if (Patcher::PATCH_STATUS_BUSY != status)
    {
		// See if the specific error allows us to retry
		int errCode = Patcher::GetPatcher()->GetPatchError();
		bool tryAgain = ShouldRetry(errCode);

 		// If we have not exhausted all of our potential servers, try the next
 		// one on the list.
 		if (tryAgain && (_nextHost < s_numHosts))
 		{
 			if (Patcher::PATCH_STATUS_OK != Patcher::GetPatcher()->BeginVersionCheck(s_hosts[_nextHost++], PATCH_VERSION_PATH, _patchVersion))
 			{
 				_exitPatcher("There was an error checking the version of the patch.");
 				return true;
 			}
 		}
 		else
 		{
 			// We have tried all of our servers. We might have an unrecoverable
 			// network error.
 			_exitPatcher(PatchErrorString(errCode));
 			return true;
 		}
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the QUERY_PATCH_DOWNLOAD state.  Ask the user if they want to download
/// a patch.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_queryPatchDownload()
{
    // Ask the user if they want to download the patch
    YesNoDialog::getSingletonPtr()->start("Update Available", "An update for Twenty-Five to Life is now "
                                                              "available for download. To continue to "
                                                              "Twenty-Five to Life Online, the update "
                                                              "must be downloaded and saved to the "
															  "memory card (8MB) (for PlayStation® 2).  "
                                                              "Would you like to download "
                                                              "the update now?", false);
                                                               
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when in the QUERY_PATCH_DOWNLOAD state.  If the user wants to download the patch,
/// start the download and go to the PATCH_DOWNLOAD state.  Otherwise, ask them if they are sure.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateQueryPatchDownload()
{
    // Check return result from dialog
    if (YesNoDialog::YES == YesNoDialog::getSingletonPtr()->GetAnswer())
    {
        // Start the patch download
        Patcher::GetPatcher()->DownloadPatch(_patchBuffer, _patchBufSize);
        _stateMachine.setCurrentState(this, PATCH_DOWNLOAD);
    }
    else
    {
        // Can't play online without patch, go back to the main menu
        _stateMachine.setCurrentState( this, QUERY_NO_TO_DOWNLOAD );
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the QUERY_NO_TO_DOWNLOAD state.  Ask the user if they want to download
/// a patch.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_enterQueryNoDownload()
{
    // Ask the user if they want to download the patch
    YesNoDialog::getSingletonPtr()->start("Are you sure?", "You have requested to cancel downloading the patch.  "
															"However you will not be able to play online without the patch.  "
															"Do you want to download the patch and play online?"
															, false);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when in the QUERY_NO_TO_DOWNLOAD state.  If the user wants to download the patch,
/// start the download and go to the PATCH_DOWNLOAD state.  Otherwise, return to the main menu, as
/// a user cannot play online if they're not fully patched up.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateQueryNoDownload()
{
    // Check return result from dialog
    if (YesNoDialog::YES == YesNoDialog::getSingletonPtr()->GetAnswer())
    {
        // Start the patch download
        Patcher::GetPatcher()->DownloadPatch(_patchBuffer, _patchBufSize);
        _stateMachine.setCurrentState(this, PATCH_DOWNLOAD);
    }
	else
    {
		// Can't play online without patch, go back to the main menu
        _exitPatcher(NULL);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when in the PATCH_DOWNLOAD state.  Wait for the patch to finish downloading and save
/// it to the memory card.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updatePatchDownload()
{
    Patcher::CompletionInfo info;
    Patcher::Status status;
	Patcher* patcher = Patcher::GetPatcher();

    // Pump the patcher
    patcher->Update(g_timer.GetOSFrameSec());
	_progressIndicator("Downloading update");

    // See if the patch is done downloading
    status = patcher->TestCompletion(info);

    // Yes, save the patch
    if (Patcher::PATCH_STATUS_OK == status)
    {
        _patchSize = info.storeSize;
        _stateMachine.setCurrentState(this, SAVE_PATCH);
    }
    // No, spit out an error
    else if (Patcher::PATCH_STATUS_BUSY != status)
    {
        _exitPatcher("There was an error downloading the patch.");
        return true;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Start the save operation to save the patch to the memory card
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_enterPatchSave()
{
    // Save to memory card
    g_MemCardManager->SetExt(PATCH_EXT);
    g_MemCardManager->SetUsePopups(true);
	g_MemCardManager->SetOverwriteConfirm(true);
	g_MemCardManager->SetAreYouReallySure(false);
	g_MemCardManager->SetUseActualFileSize(true);
	g_MemCardManager->SaveFile(SAVEGAME_FILENAME, "patch", 0, _patchBuffer, _patchSize, false, MC_SAVED_PATCH);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Wait until the patch is done saving to the memory card
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updatePatchSave()
{
	_progressIndicator("Saving patch");

    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
    {
		g_MemCardManager->ResetLoadSaveBuffer();

        Patcher::CompletionInfo info;
        Patcher* patcher = Patcher::GetPatcher();

		// Something bad might have happened, check the popup result
		if (!_handleMemcardPopupResult( g_MemCardManager->GetPopupExitResult() ))
		{
			return true;
		}
            
        // DFS -- temp hack; synchronously wait for decrypt (shouldn't take 
        // long anyway)
        _patchSize = 0;
        if ((patcher->PreparePatch() == Patcher::PATCH_STATUS_OK) &&
                (patcher->WaitCompletion(info) == Patcher::PATCH_STATUS_OK))
        {
	        _patchSize = info.clearSize;
        }
        else
        {
            _exitPatcher("There was an error decrypting the patch.");
            return true;
        }

        // Tell the system about the patch.
        // DFS TODO: if there was an error preparing the patch, we need to do something,
        // like prohibit entry into a network game
        if (_patchSize)
        {
            Media::EnablePatching(_patchBuffer, _patchSize);
            _enabledPatching = true;
#ifdef NETZ
            CNetZConnection::LoadNetworkConfigSettings();
#endif
        }

        Patcher::Destroy();
        g_DllLoader.Unload(PATCHER_NAME);
		_loadedPatcher = false;

        _stateMachine.setCurrentState(this, WAIT_TERMINATE);

        // Start the login process
        EORLogin::getSingletonPtr()->start();

        // Show status message
        DisplayDialog::getSingletonPtr()->start("Update Available", "Update successful.");
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// State machine waits in this state for the app to finish playing online so it can
/// stop processing and remove the patcher from memory.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORPatcher::_updateWaitTerminate()
{
    stop();
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Do everything needed to exit the patcher
///
/// \param pErrorString The error string to display in a dialog.  If NULL, don't display error dialog.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_exitPatcher(const char *pErrorString)
{
    // Unload patcher from memory
    Patcher::Destroy();
    g_DllLoader.Unload(PATCHER_NAME);
	_loadedPatcher = false;

    if (pErrorString)
    {
        OSIstrncpy(_errorString, pErrorString, OSI_STRING_LENGTH_HUGE);
        _errorString[OSI_STRING_LENGTH_HUGE - 1] = 0;
    }
    else
    {
        _errorString[0] = 0;
    }

    _stateMachine.setCurrentState(this, ERROR_TERMINATE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Do everything needed to exit the patcher in an error state
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORPatcher::_enterErrorTerminate()
{
	// Display error message if needed
    if (_errorString[0])
    {
        DisplayDialog::getSingletonPtr()->start("Patching Failed", _errorString);
    }
    // Stop use case after error is displayed
    _stateMachine.setCurrentState(this, WAIT_TERMINATE);
}

bool OnlineUI::EORPatcher::_progressIndicator( const char* defaultStr )
{
	ScreenTitleOnly* pScreen = UseCase::GetActiveScreen< ScreenTitleOnly >();
	if( pScreen == NULL ) return false;

	// Pick an appropriate message
	const char* titleStr = defaultStr;
	if (_loadedPatcher)
	{
		Patcher::ThreadStatus tstat = Patcher::GetPatcher()->GetProgress();
		switch (tstat)
		{
			case Patcher::THREAD_STATUS_HOSTLOOKUP:		// The work thread is looking up the patch host name
				titleStr = "Looking up host name";
				break;
			case Patcher::THREAD_STATUS_REQUESTING:		// An http request is being sent
				titleStr = "Sending request";
				break;
			case Patcher::THREAD_STATUS_RECEIVING:		// The work thread is receiving data
				titleStr = "Receiving data";
				break;
		}
	}

	char tempBuf[ 128 ];
	int slen = OSIsnprintf( tempBuf, 128, "Updater - %s", titleStr );
	tempBuf[127] = 0;

	if (slen > 0)
	{
		int dotcount = ( g_timer.GetOSTime() / 500 ) & 7;
		while (dotcount && (slen <= 125))
		{
			tempBuf[ slen++ ] = ' ';
			tempBuf[ slen++ ] = '.';
			dotcount--;
		}
		tempBuf[ slen ] = 0;
	}

	pScreen->SetTitle( tempBuf );
	return( true );
}
