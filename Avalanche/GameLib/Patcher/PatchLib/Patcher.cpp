//------------------------------------------------------------------------------
// patcher.cpp
//
// Source for game patching library.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enable/disable debug printing
//#define PATCH_PRINT_ENABLED

#include "Patcher.h"
#include "PatchError.h"
#include "HttpMgr.h"
#include "PatcherCommon/PatchHelper.h"

// Define this to disable dnas so we can test with unauthored/unencrypted data
#define DISABLE_DNAS		// JJS

// Temp hack: Disable dnas for PC build until we wrap it in an interface
#ifndef PATCH_COMPILER_SN_PS2
	#define DISABLE_DNAS
#endif

// Make sure we have the include if we are using dnas
#ifndef DISABLE_DNAS
	#include <dnas2_inst_mc.h>
#endif

Patcher Patcher::s_PatcherInstance;

//------------------------------------------------------------------------------
// Get a pointer to our static Patcher. Result undefined if Create has not
// been called, because we are unable to initialize the static pointer to NULL
// in a DLL!!
Patcher* Patcher::GetPatcher(void)
{
	return &s_PatcherInstance;
}

//------------------------------------------------------------------------------
// Initialize the patch object. Does not create any resources
// or allocate memory.
void Patcher::_Init(void)
{
	m_state = PATCH_STATE_INIT;
	m_threadPriority = 0;
	m_bufState = BUFF_STATE_NULL;

	m_message = PATCH_MSG_NOOP;
	m_threadStatus = THREAD_STATUS_NOT_READY;
	m_threadError = PATCH_ERROR_NONE;
	m_version = 0;
	m_threadRunState = THREAD_STATE_BUSY;

	m_bytesReceived = 0;			// Number of data bytes received
	m_bufferSize = 0;				// Size of data buffer
	m_downloadSize = 0;
	m_storeSize = 0;
	m_clearSize = 0;
	m_mandatory = 1;
	m_host[0] = 0;
	m_resource[0] = 0;
	m_description[0] = 0;
	m_buffer = NULL;					// Data buffer
}

//------------------------------------------------------------------------------
// One-time initialization function to be called at engine
// startup time or when the game determines that it may need
// to check for patches. Creates the static singleton Patcher instance.
// Note: Our static pointer cannot be reliably initialized to NULL with
// the SN DLL system, so Create MUST be called before any other Patcher
// functions, and may not be called a second time unless Destroy() is called
// first.
Patcher::Status Patcher::Create(int threadPriority)
{
	// Make sure we haven't already started up a patcher
	if (s_PatcherInstance.m_threadPriority != 0)
		return PATCH_STATUS_BAD_REQUEST;

	s_PatcherInstance.m_threadPriority = threadPriority;
	return PATCH_STATUS_OK;
}

Patcher::Patcher()
{
	_Init();
}

//------------------------------------------------------------------------------
// Destroy frees all allocated resources in the singleton Patcher, and indicates
// that no further Patcher calls will be made. The Patcher DLL can be removed
// from memory at this point.
void Patcher::Destroy(void)
{
	// Shut down our static instance
	s_PatcherInstance._Shutdown();

	// Reinitialize so we can be created again
	s_PatcherInstance._Init();
}

//------------------------------------------------------------------------------
// Between the Init and the Shutdown functions, the Update
// function should be called frequently, such as once per frame,
// to provide a synchronization point with the game engine and to
// check for completion of asynchronous operations.
//
// Parameters:	dt		-- seconds since the last time we were called
void Patcher::Update(float dt)
{
	//PATCH_PRINT("Inside Patcher::Update\n");
	switch (m_state)
	{
		case PATCH_STATE_INIT:				// We are initialized, but have done nothing
			_InitStateWork();
			break;
		case PATCH_STATE_START_VERSION:		// We want to check the version
			_StartVersionWork();
			break;
		case PATCH_STATE_CHECKING_VERSION:	// A version check has been initiated
			_CheckingVersionWork();
			break;
		case PATCH_STATE_START_DESCRIPTION:	// Requesting a patch description
			_StartDescriptionWork();
			break;
		case PATCH_STATE_GETTING_DESCRIPTION:	// Downloading a description
			_GetDescriptionWork();
			break;
		case PATCH_STATE_START_UPDATE:		// We want to fetch an update
			_StartUpdateWork();
			break;
		case PATCH_STATE_UPDATING:			// We are fetching an update
			_UpdatingWork();
			break;
		case PATCH_STATE_START_DECRYPT:		// We want to start decrypting
			_StartDecryptWork();
			break;
		case PATCH_STATE_DECRYPTING:
			_DecryptingWork();
			break;
		case PATCH_STATE_CLEAR_PATCH:
		case PATCH_STATE_UPDATE_READY:		// Update has been fetched, waiting for app to request something
		case PATCH_STATE_ERROR:				// An error occurred with the last request, but we are not cleaned up
		case PATCH_STATE_NO_UPDATES:		// Up do date; no updates needed or updating complete
		case PATCH_STATE_READY:				// Initialized, waiting for check to be requested
		case PATCH_STATE_NEED_UPDATE:		// We need to fetch an update, but wait to be started
		case PATCH_STATE_HALTED:			// We are stopped; do nothing
		default:
			break;
	}
}

//------------------------------------------------------------------------------
// BeginVersionCheck() will initialize an asynchronous
// check with the patch server to determine whether an
// update is needed or available. The status returned will
// indicate whether a check was initiated, the check was
// already completed, or an error occurred. Once a check
// has been initiated, use TestCompletion() or WaitCompletion()
// to get back the update information in the CompletionInfo structure.
// The update information will indicate whether a patch is available, and if
// so how large it is (and any other information needed).
//
// Parameters:	host    -- Host name of computer hosting version information; e.g., "patch.eidos.com"
//				path    -- Name of version file on host; e.g. "/TTL/patch/version.txt"
//				version -- a version indicator stored in the local unsigned int format
//
// Return values:
//		PATCH_STATUS_OK			-- A version check has started
//		anything else			-- an error occurred
Patcher::Status Patcher::BeginVersionCheck(const char* host, const char* path, unsigned int version)
{
	// If we are still in the INIT state, that means that the caller is calling
	// us immediately after startup, without giving the update function a chance
	// to work. Since all the INIT state does is transition anyway, go ahead and
	// do the work here.
	if (m_state == PATCH_STATE_INIT)
	{
		_InitStateWork();
		if (m_state != PATCH_STATE_READY)
			return PATCH_STATUS_INIT_FAILED;
	}

	// If we are in an error state, the caller is probably retrying with
	// different parameters, so let it go through. Otherwise, we need to be
	// in a ready state.
	if ((m_state != PATCH_STATE_ERROR) && !(m_state & PATCH_STATE_READY_FLAG))
		return PATCH_STATUS_BAD_REQUEST;

	// Make sure they have provided host and path names
	if (!host || !path)
		return PATCH_STATUS_BAD_REQUEST;

	// Save the host and path
	if ((safestrlcpy(m_host, host, PATCHER_HOST_MAX) >= PATCHER_HOST_MAX) ||
		(safestrlcpy(m_resource, path, PATCHER_PATH_MAX) >= PATCHER_PATH_MAX))
		return PATCH_STATUS_BAD_REQUEST;

	// Save the version information for later
	m_version = version;

	// Ready to initiate
	m_state = PATCH_STATE_START_VERSION;

	return PATCH_STATUS_OK;
}

//------------------------------------------------------------------------------
// DownloadDescription begins downloading the description associated with the patch.
// If PATCH_STATUS_OK is returned, used TestCompletion to wait for your buffer
// to be filled. If the patch description is larger than the buffer size,
// the description is truncated to the buffer. As a defense against attacks,
// the last byte is always set to zero, so a truncated description will actually
// be truncated to one byte less than bufferSize.
Patcher::Status Patcher::DownloadDescription(char* buffer, int bufferSize)
{
	if (m_state != PATCH_STATE_NEED_UPDATE)
	{
		PATCH_PRINT("Description requested when not ready for update\n");
		return PATCH_STATUS_BAD_REQUEST;
	}

	if (!buffer || (bufferSize <= 0))
	{
		PATCH_PRINT("Description requested without a proper buffer!\n");
		return PATCH_STATUS_BAD_REQUEST;
	}

	// If we don't have a resource for the description to download, we are done
	if (m_description[0] == 0)
	{
		PATCH_PRINT("Description requested when none available");
		return PATCH_STATUS_NOT_FOUND;
	}

	// Save any necessary information for passing to thread
	m_buffer = buffer;
	m_bufferSize = bufferSize;
	m_bufState = BUFF_STATE_FILLING;

	// Transition to the start-getting-description state, where the real work is done.
	m_state = PATCH_STATE_START_DESCRIPTION;

	return PATCH_STATUS_OK;
}

//------------------------------------------------------------------------------
// TestCompletion() will poll to see if any asynchronous operations have completed.
// It will return immediately whether or not an operation is complete. The returned
// status will indicate one of
//	PATCH_STATUS_OK		– Any operations are complete, and CompletionInfo contains relevant information.
//	PATCH_STATUS_BUSY	– There are outstanding asynchronous operations which have not completed. CompletionInfo is untouched.
//	Anything else		- An error condition exists, you should abort.
Patcher::Status Patcher::TestCompletion(CompletionInfo& info)
{
	switch (m_state & PATCH_STATE_READY_MASK)
	{
		case 0:
			return PATCH_STATUS_INIT_FAILED;

		case PATCH_STATE_BUSY_FLAG:
			return PATCH_STATUS_BUSY;

		case PATCH_STATE_ERROR_FLAG:
			return PATCH_STATUS_NET_ERROR;
	}

	// OK, we are in a ready state.
	if (m_state == PATCH_STATE_NO_UPDATES)
	{
		// The size fields must be zero if we don't need an update
		info.reqBufSize = 0;
		info.storeSize = 0;
		info.clearSize = 0;
		info.mandatory = false;
	}
	else
	{
		info.reqBufSize = m_downloadSize;
		info.storeSize = m_storeSize;
		info.clearSize = m_clearSize;
		info.mandatory = (m_mandatory != 0);
	}
	return PATCH_STATUS_OK;
}

//------------------------------------------------------------------------------
// WaitCompletion() is the same as TestCompletion(), except that if there are
// asynchronous operations which have not completed, it waits until one completes.
// This function would be used in a multithreaded game, where it is desired that
// the testing thread not use up unnecessary CPU time spinning.
Patcher::Status Patcher::WaitCompletion(CompletionInfo& info)
{
	// For now, this function won't be used, so rather than create another
	// semaphore or other synchronization object on which we can wait, we
	// commit the cardinal sin of spinning...
	while (m_state & PATCH_STATE_BUSY_FLAG) {
		m_thread.Delay(10);
		Update(0.01f);
	}

	return TestCompletion(info);
}

//------------------------------------------------------------------------------
// DownloadPatch() begins the process of downloading and patching executable
// data. After DownloadPatch is called, a patch which has been previously
// determined to be available is downloaded into the given buffer, and "personalized"
// (encrypted using parameters specific to the currently running computer). Call
// TestCompletion() to determine when the download is complete. When the download
// is complete, the buffer will contain the encrypted data ready to be stored by
// the application on the storage device (memory card, hard disk, etc.).
Patcher::Status Patcher::DownloadPatch(char* buffer, int bufferSize)
{
	if (m_state != PATCH_STATE_NEED_UPDATE)
	{
		PATCH_PRINT("Update requested when not ready for update\n");
		return PATCH_STATUS_BAD_REQUEST;
	}

	if (!buffer || (bufferSize <= 0))
	{
		PATCH_PRINT("Update requested without a proper buffer!\n");
		return PATCH_STATUS_BAD_REQUEST;
	}

	// Save any necessary information for passing to thread
	m_buffer = buffer;
	m_bufferSize = bufferSize;
	m_bufState = BUFF_STATE_FILLING;

	// Make sure we start with the bytes received as zero, in case our caller
	// checks before the thread has a chance to set it
	m_bytesReceived = 0;

	// Transition to the start-update state, where the real work is done.
	m_state = PATCH_STATE_START_UPDATE;

	return PATCH_STATUS_OK;
}

//------------------------------------------------------------------------------
// Start preparing the current patch for use. Assuming an encrypted (personalized)
// patch is already stored in memory, this function will initiate an asynchronous 
// decryption of the patch into clear data. Use TestCompletion or WaitCompletion
// to wait for actual completion.
Patcher::Status Patcher::PreparePatch(void)
{
	// We must have an encrypted patch in memory
	if (m_state != PATCH_STATE_UPDATE_READY)
		return PATCH_STATUS_BAD_REQUEST;

	// Make sure the buffer is in a good state
	if ((m_bufState != BUFF_STATE_ENCRYPTED) || !m_buffer || (m_bufferSize <= 0))
		return PATCH_STATUS_BAD_REQUEST;

	// Transition to the start-decrypt state
	m_state = PATCH_STATE_START_DECRYPT;

	return PATCH_STATUS_OK;
}

//------------------------------------------------------------------------------
// When a patch is read off the memory card or hard drive by the application,
// this function sets the current patch into the patcher so it can be applied.
// The data in the buffer must be in the encrypted state used to store on
// the memory card (DNAS personalized). As long as the patch was downloaded
// properly in a previous run, it will be in that state.
Patcher::Status Patcher::SetPatch(char* buffer, int bufferSize)
{
	// If we are still in the INIT state, that means that the caller is calling
	// us immediately after startup, without giving the update function a chance
	// to work. Since all the INIT state does is transition anyway, go ahead and
	// do the work here.
	if (m_state == PATCH_STATE_INIT)
	{
		_InitStateWork();
		if (m_state != PATCH_STATE_READY)
			return PATCH_STATUS_INIT_FAILED;
	}

	// Make sure we are in a ready state
	if (!(m_state & PATCH_STATE_READY_FLAG))
		return PATCH_STATUS_BAD_REQUEST;

	// Save the buffer information. Also set default decrypted size, just in case
	m_buffer = buffer;
	m_bufferSize = m_downloadSize = m_storeSize = m_clearSize = bufferSize;
	m_bufState = BUFF_STATE_ENCRYPTED;

	// Set the patcher state so we know we have an encrypted patch in memory.
	m_state = PATCH_STATE_UPDATE_READY;

	return PATCH_STATUS_OK;
}

//------------------------------------------------------------------------------
// Get the number of bytes downloaded so far during a lengthy transfer. Value
// is undefined if we are not downloading anything.
int Patcher::GetBytesDownloaded(void)
{
	return m_bytesReceived;
}

//------------------------------------------------------------------------------
// Get the buffer size required to hold the patch in all its forms (this is for
// convenience; the same information is available from TestCompletion).
int Patcher::GetBufferSize(void) const
{
	int max1 = (m_downloadSize > m_storeSize) ? m_downloadSize : m_storeSize;
	return (m_clearSize > max1) ? m_clearSize : max1;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Private work functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void Patcher::_Shutdown(void)
{
	int i;
	if (m_threadRunState == THREAD_STATE_READY)
	{
		// Let the thread know we want to quit.
		_SendMessage(PATCH_MSG_EXIT);

		// Give the thread a chance to exit gracefully. This loop is not
		// necessary on all systems, only those that can't properly wait for
		// thread termination.
		for (i = 0; i < 10; i++)
		{
			m_thread.Delay(1);	// Delay one ms
			if (m_threadRunState == THREAD_STATE_QUIT)
				break;
		}
	}
	// Wait up to 10 more ms to make sure the thread has exited
	m_thread.WaitComplete(10);
	m_thread.Kill();
}

//------------------------------------------------------------------------------
// Send a message to the worker thread. Returns true if the message was sent, false otherwise
bool Patcher::_SendMessage(Message msg)
{
	if (m_threadRunState != THREAD_STATE_READY)
		return false;

	m_message = msg;
	m_threadRunState = THREAD_STATE_BUSY;
	m_thread.SignalMessage();
	return true;
}

//------------------------------------------------------------------------------
// Perform work for the INIT state
void Patcher::_InitStateWork(void)
{
	PATCH_PRINT("Patcher::_InitStateWork called\n");

	// Make sure we are ready for an error abort
	m_threadRunState = THREAD_STATE_BUSY;

	// Start up the worker thread
	if (!m_thread.Start(S_PatcherThreadMain, this, m_threadPriority))
	{
		// If we failed to start a worker thread, transition to our do-nothing state
		m_state = PATCH_STATE_HALTED;
		return;
	}

	// OK, the thread is ready and so are we...
	m_state = PATCH_STATE_READY;
	return;
}

//------------------------------------------------------------------------------
// Perform work for the CHECKING_UPDATE state
void Patcher::_CheckingVersionWork(void)
{
	// See if the version check is complete
	if (m_threadRunState == THREAD_STATE_BUSY) {
		m_thread.Delay(0);
		return;
	}

	// OK, the worker thread is done with what it was doing. Let's see how it did...
	switch (m_threadStatus) {
		case THREAD_STATUS_NEED_UPDATE:
			PATCH_PRINT("----- We need an update\n");
			m_state = PATCH_STATE_NEED_UPDATE;
			break;
		case THREAD_STATUS_READY:
			PATCH_PRINT("----- We DO NOT need an update\n");
			m_state = PATCH_STATE_NO_UPDATES;
			break;
		case THREAD_STATUS_NETWORKERR:
		default:
			PATCH_PRINT("----- We encountered an error, thread status %d!\n", m_threadStatus);
			m_state = PATCH_STATE_ERROR;
			break;
	}
}

//------------------------------------------------------------------------------
void Patcher::_StartVersionWork(void)
{
	PATCH_PRINT("Patcher::_StartVersionWork called\n");

	// Initiate a version check and transition states if the thread is ready for us
	if (_SendMessage(PATCH_MSG_CHECK_VERSION))
		m_state = PATCH_STATE_CHECKING_VERSION;
}

//------------------------------------------------------------------------------
// Try to start downloading a description file
void Patcher::_StartDescriptionWork(void)
{
	PATCH_PRINT("Patcher::_StartDescriptionWork called\n");

	// Initiate the download is we are ready
	if (_SendMessage(PATCH_MSG_GET_DESCRIPTION))
		m_state = PATCH_STATE_GETTING_DESCRIPTION;
}

//------------------------------------------------------------------------------
// We are currently downloading a patch description in our worker thread. Since threads
// on the PS2 are not preemptive, give up some time to allow the work thread to
// do its thing.
void Patcher::_GetDescriptionWork(void)
{
	// See if the thread is done yet
	if (m_threadRunState == THREAD_STATE_BUSY) {
		// Still busy; come back later
		m_thread.Delay(0);
		return;
	}

	// Worker thread is done; transition main thread state
	if (m_threadStatus == THREAD_STATUS_READY)
		m_state = PATCH_STATE_NEED_UPDATE;
	else
		m_state = PATCH_STATE_ERROR;
}


//------------------------------------------------------------------------------
// Try to initiate an update
void Patcher::_StartUpdateWork(void)
{
	PATCH_PRINT("Patcher::_StartUpdateWork called\n");

	// If we can sucessfully send the start message, we can transition to the next state
	if (_SendMessage(PATCH_MSG_FETCH_UPDATE))
		m_state = PATCH_STATE_UPDATING;
}

//------------------------------------------------------------------------------
// We are currently downloading an update in our worker thread. Since threads
// on the PS2 are not preemptive, give up some time to allow the work thread to
// do its thing.
void Patcher::_UpdatingWork(void)
{
	// See if the version check is complete
	if (m_threadRunState == THREAD_STATE_BUSY) {
		m_thread.Delay(0);
		return;
	}

	if (m_threadStatus == THREAD_STATUS_READY)
		m_state = PATCH_STATE_UPDATE_READY;
	else
		m_state = PATCH_STATE_ERROR;
}

//------------------------------------------------------------------------------
// Try to initiate decryption
void Patcher::_StartDecryptWork(void)
{
	PATCH_PRINT("Patcher::_StartDecryptWork called\n");

	// If we can sucessfully send the start message, we can transition to the next state
	if (_SendMessage(PATCH_MSG_DECRYPT))
		m_state = PATCH_STATE_DECRYPTING;
}

//------------------------------------------------------------------------------
void Patcher::_DecryptingWork(void)
{
	// See if the thread is still busy
	if (m_threadRunState == THREAD_STATE_BUSY) {
		m_thread.Delay(0);
		return;
	}

	// Thread is done. Transition state depending on thread status
	if (m_threadStatus == THREAD_STATUS_READY)
		m_state = PATCH_STATE_CLEAR_PATCH;
	else
		m_state = PATCH_STATE_ERROR;
}
//------------------------------------------------------------------------------
void Patcher::_ApplyUpdateWork(void)
{
	PATCH_PRINT("_ApplyUpdateWork called!\n");
}

//------------------------------------------------------------------------------
// Main function for the work thread
void Patcher::_ThreadMain(void)
{
	// Create the http manager
	HttpMgr::Create();
	if (!HttpMgr::GetPtr())
	{
		// Something is really wrong, just quit
		m_threadRunState = THREAD_STATE_QUIT;
		return;
	}

#ifndef DISABLE_DNAS
	// Initialized DNAS-inst
	int status = sceDNAS2InstInit_mc();
	if (status)
	{
		// This is a fatal error; abort the thread
		PATCH_PRINT("!!!! Warning: DNAS-inst init failed with status %d\n", status);
		m_threadRunState = THREAD_STATE_QUIT;
		m_threadStatus = THREAD_STATUS_DECRYPTERROR;
		m_threadError = PATCH_ERROR_CRYPT_INIT;
		return;
	}
#endif // DISABLE_DNAS

	// Just sit in a loop waiting for messages from the main thread
	bool running = true;
	m_threadRunState = THREAD_STATE_READY;
	m_threadStatus = THREAD_STATUS_READY;
	m_threadError = PATCH_ERROR_NONE;

	while (running)
	{
		PATCH_PRINT("======== Thread -- About to wait for message =============\n");
		if (m_thread.WaitMessage())
		{
			switch (m_message)
			{
				case PATCH_MSG_NOOP:
					PATCH_PRINT("============= Received NOOP message ===============\n");
					break;

				case PATCH_MSG_CHECK_VERSION:
				{
					PATCH_PRINT("============= Received CHECK_VERSION message ===============\n");

					// Just create a temp buffer on the stack
					char buf[512];
					char* saveBuf = m_buffer;
					int saveSize = m_bufferSize;
					BufferState saveState = m_bufState;

					m_buffer = buf;
					m_bufferSize = 512;
					m_bufState = BUFF_STATE_TEMP;

					// Download update availability information from the web
					_ThreadFetchHTTP(m_host, m_resource, false);

					// Figure out whether an update is actually needed or not.
					// Note: m_host and m_resource are changed here is an update is needed
					_ThreadParseVersion();

					// Set the buffer back
					m_buffer = saveBuf;
					m_bufferSize = saveSize;
					m_bufState = saveState;
					break;
				}

				case PATCH_MSG_GET_DESCRIPTION:
					PATCH_PRINT("=========== Thread received GET_DESCRIPTION message =================\n");
					// Zero out our buffer and allow truncation so we are null terminated
					memset(m_buffer, 0, m_bufferSize);
					_ThreadFetchHTTP(m_host, m_description, true);
					break;

				case PATCH_MSG_FETCH_UPDATE:
					PATCH_PRINT("=========== Thread received FETCH_UPDATE message =================\n");
					_ThreadFetchHTTP(m_host, m_resource, false);
					_ThreadEncryptLocal();
					break;

				case PATCH_MSG_DECRYPT:
					PATCH_PRINT("=========== Thread received DECRYPT message =================\n");
					_ThreadDecryptLocal();
					break;

				case PATCH_MSG_EXIT:
					PATCH_PRINT("============= Received EXIT message ===============\n");
					running = false;
					break;

				default:
					PATCH_PRINT("============= WARNING -- BAD MESSAGE RECEIVED: %d ===============\n", m_message);
					break;
			}
			// Let the main thread know we are ready to receive the next message
			m_threadRunState = THREAD_STATE_READY;
		}
		else
		{
			// If we get an error from WaitSema, something is really wrong so exit
			running = false;
		}

	}

	PATCH_PRINT("============= Thread finished with loop, destroying HTTP mgr ===============\n");

	HttpMgr::Destroy();

#ifndef DISABLE_DNAS
	sceDNAS2InstShutdown_mc();
#endif // DISABLE_DNAS

	////if ((status = sceInsockTerminate(0)) < 0)
	////	PATCH_PRINT("!!!!!!!!!! sceInsockTerminate failed with %d!!!!!!!!!!!\n", status);

	// The last thing -- Flag we are done (note: this does not mean the thread 
	// has actually terminated, just that it has no more to do)
	m_threadRunState = THREAD_STATE_QUIT;
}

//------------------------------------------------------------------------------
void Patcher::_ThreadFetchHTTP(const char* host, const char* file, bool truncateOK)
{
	m_threadStatus = THREAD_STATUS_HOSTLOOKUP;
	HttpMgr* pHttp = HttpMgr::GetPtr();
	int status = pHttp->FindHost(host);
	if (status != PATCH_ERROR_NONE)
	{
		PATCH_PRINT("===Thread: Error %d looking up host %s, aborting!\n", (int)status, host);
		m_threadStatus = THREAD_STATUS_NETWORKERR;
		m_threadError = status;
		return;
	}

	PATCH_PRINT("===Thread: Found host. Requesting URL...\n");
	m_threadStatus = THREAD_STATUS_REQUESTING;
	status = pHttp->RequestHTTP(host, file, 10);
	if (status != PATCH_ERROR_NONE)
	{
		PATCH_PRINT("===Thread: Error sending request, aborting!\n");
		m_threadStatus = THREAD_STATUS_NETWORKERR;
		m_threadError = status;
		return;
	}

	PATCH_PRINT("===Thread: Sent request, receiving...\n");

	m_threadStatus = THREAD_STATUS_RECEIVING;
	int reportedSize;
	status = pHttp->ReceiveAll(m_bytesReceived, reportedSize, m_buffer, m_bufferSize, truncateOK);
	if (status != PATCH_ERROR_NONE)
	{
		PATCH_PRINT("===Thread: Error %d receiving data!!!\n", (int)status);
		m_threadStatus = THREAD_STATUS_NETWORKERR;
		m_threadError = status;
		return;
	}

	PATCH_PRINT("===Thread: Update data received OK.\n");
	m_threadStatus = THREAD_STATUS_READY;
	m_threadError = PATCH_ERROR_NONE;
}

//------------------------------------------------------------------------------
// Once version information has been fetched from the web, figure out whether we
// need to actually update or not.
void Patcher::_ThreadParseVersion(void)
{
	PATCH_PRINT("== _ThreadParseVersion: ");
	// If a network error occurred, look to see what it was
	if (m_threadStatus == THREAD_STATUS_NETWORKERR)
	{
		PATCH_PRINT("Network error: %d\n", m_threadError);
		// If the resource was not found, then there is no update available
		if (m_threadError == PATCH_ERROR_FILE_NOT_FOUND)
		{
			PATCH_PRINT("OK, file not found\n");
			// A ready status means no update and no error
			m_threadError = PATCH_ERROR_NONE;
			m_threadStatus = THREAD_STATUS_READY;
		}
		// Let the main thread deal with any other errors; we don't have an update
		return;
	}

	// Make sure we have a version info file present (this should never trigger)
	if (!m_buffer || !m_bufferSize || !m_bytesReceived)
	{
		PATCH_PRINT("== _ThreadParseVersion: Shouldn't be here.\n");
		PATCH_PRINT("   m_buffer=0x%x, m_bufferSize= %d, m_bytesReceived=%d\n", (unsigned int)m_buffer, m_bufferSize, m_bytesReceived);
		// This should never happen; just pretend a network error occurred
		m_threadStatus = THREAD_STATUS_NETWORKERR;
		return;
	}

	// By default, we don't update
	m_threadStatus = THREAD_STATUS_READY;
	m_downloadSize = 0;
	m_storeSize = 0;
	m_clearSize = 0;
	m_mandatory = 1;
	m_host[0] = 0;
	m_resource[0] = 0;
	m_description[0] = 0;
	bool needUpdate = false;

	// Parse the buffer to see if we are up to date
	int i, j;
	for (i = 0, j = 0; i < m_bytesReceived; i = j)
	{
		// Skip j forward to the end of the line
		while ((j < m_bytesReceived) && (m_buffer[j] != '\r') && (m_buffer[j] != '\n')) j++;
		if (j == i)
			break;

		// Skip to the beginning of the next line
		while ((j < m_bytesReceived) && ((m_buffer[j] == '\r') || (m_buffer[j] == '\n'))) j++;

		// OK, i points to the start of this line, and j to the start of the next line
		// Note: nothing is null terminated

		//PATCH_PRINT("Parsing line: %s\n", &m_buffer[i]);

		// Check for comment line beginning with #
		if (m_buffer[i] == '#')
		{
			PATCH_PRINT("Found comment, skipping to next line\n");
			continue;
		}

		// Look for a command
		char cmd[256], val[256];
		if (sscanf(&m_buffer[i], "%[^:]: %[^\r\n\t #]", cmd, val) != 2)
		{
			PATCH_PRINT("No command found, skipping to next line\n");
			continue;	// No command; just go to next line
		}

		PATCH_PRINT("--- cmd: '%s'; val: '%s'\n", cmd, val);

		// Parse the command
		if (stricmp(cmd, "Host") == 0)
		{
			PATCH_PRINT("Found Host: '%s'\n", val);
			// Add this host to our host list
			safestrlcpy(m_host, val, PATCHER_HOST_MAX);
		}
		else if (stricmp(cmd, "Patch") == 0)
		{
			PATCH_PRINT("Found Patch: '%s'\n", val);
			// add this patch file to our list
			safestrlcpy(m_resource, val, PATCHER_PATH_MAX);
		}
		else if (stricmp(cmd, "Version") == 0)
		{
			// Version string. Compare to ours to see if we need an update
			unsigned int ver = atoi(val);
			if (ver != m_version)
			{
				PATCH_PRINT("--- Version from file %d does not match given version %d\n", ver, m_version);
				needUpdate = true;
			}
		}
		else if (stricmp(cmd, "RawSize") == 0)
		{
			m_downloadSize = atoi(val);
			PATCH_PRINT("Download size: %d\n", m_downloadSize);
		}
		else if (stricmp(cmd, "StoreSize") == 0)
		{
			m_storeSize = atoi(val);
			PATCH_PRINT("Storage size: %d\n", m_storeSize);
		}
		else if (stricmp(cmd, "ClearSize") == 0)
		{
			m_clearSize = atoi(val);
			PATCH_PRINT("Decrypted (clear) size: %d\n", m_clearSize);
		}
		else if (stricmp(cmd, "Description") == 0)
		{
			safestrlcpy(m_description, val, PATCHER_PATH_MAX);
		}
		else if (stricmp(cmd, "Mandatory") == 0)
		{
			if (!stricmp(val, "yes") || !stricmp(val, "true"))
				m_mandatory = 1;
			else if (!stricmp(val, "no") || !stricmp(val, "false"))
				m_mandatory = 0;
			else
				m_mandatory = atoi(val);
		}
		else
		{
			PATCH_PRINT("Unknown keyword, skipping: '%s'\n", val);
		}
	}

	// OK, if we need an update and everything was read OK from the version file, set the state.
	if (needUpdate)
	{
		// In order to update we need to have a server, a patch file name, and our sizes.
		if ((m_downloadSize > 0) && (m_storeSize > 0) && (m_host[0]) && (m_resource[0]))
		{
			m_threadStatus = THREAD_STATUS_NEED_UPDATE;
		}
		else
		{
			m_threadStatus = THREAD_STATUS_CONFIGERR;
		}
	}
}

//------------------------------------------------------------------------------
void Patcher::_ThreadEncryptLocal(void)
{
	PATCH_PRINT("Inside _ThreadEncryptLocal\n");
	// If we didn't have a good download just prior, do nothing
	if ((m_threadStatus != THREAD_STATUS_READY) || (m_threadError != PATCH_ERROR_NONE))
		return;

	PATCH_PRINT("Thread is ready...\n");

	// Likewise if the buffer is not in a good state
	if (!m_buffer || (m_bufferSize <= 0) || (m_bytesReceived <= 0))
		return;

	PATCH_PRINT("Buffer is good... about to get PersonalizeDataLength...\n");

	// Current thread status...
	m_threadStatus = THREAD_STATUS_ENCRYPTING;

	// Current buffer state...
	m_bufState = BUFF_STATE_DOWNLOADED;

#ifdef DISABLE_DNAS
	if (m_storeSize < m_bytesReceived)
		m_storeSize = m_bytesReceived;
#else
	// Make sure our buffer is big enough
	u_int storeSize = 0;
	int status = sceDNAS2InstPersonalizeDataLength_mc(m_bytesReceived, (u_char*)m_buffer, &storeSize);
	PATCH_PRINT("~~~~~~~~~ PersonalizeDataLength: returned %d, storeSize = %d\n", status, storeSize);
	if (status || (storeSize <= 0))
	{
		// Error getting the personalized size; the data is bad or the buffer too small
		m_threadStatus = THREAD_STATUS_ENCRYPTERROR;
		m_threadError = PATCH_ERROR_ENCRYPT_SIZE;
		return;
	}

	if (storeSize > (u_int)m_bufferSize)
	{
		PATCH_PRINT("PATCH ERROR: encrypted size %s is larger than buffer size %d!\n", storeSize, m_bufferSize);
		m_threadStatus = THREAD_STATUS_ENCRYPTERROR;
		m_threadError = PATCH_ERROR_BUF_TOO_SMALL;
		return;
	}

	// Make sure the store size is accurate.
	if (storeSize != (u_int)m_storeSize)
	{
		// Developer warning...
		PATCH_PRINT("Warning: encrypted size %d from DNAS is different from size %d from version file.\n", storeSize, m_storeSize);
		PATCH_PRINT("Please update version information file.\n");
		m_storeSize = storeSize;
	}

	// Encrypt the buffer for the local machine
	PATCH_PRINT("Personalizing...\n");
	status = sceDNAS2InstPersonalizeData_mc(m_bytesReceived, storeSize, (u_char*)m_buffer);
	PATCH_PRINT("...Done personalizing, status = %d\n", status);

	// Note: The documentation only allows for a return of zero on success, negative
	// on error. But we are getting back the storeSize for a status!
	if (status < 0)
	{
		// Error encrypting the data
		m_threadStatus = THREAD_STATUS_ENCRYPTERROR;
		m_threadError = PATCH_ERROR_ENCRYPT;
		return;
	}

#endif // DISABLE_DNAS

	// Done!
	m_bufState = BUFF_STATE_ENCRYPTED;
	m_threadStatus = THREAD_STATUS_READY;
	m_threadError = PATCH_ERROR_NONE;
}

//------------------------------------------------------------------------------
void Patcher::_ThreadDecryptLocal(void)
{
	PATCH_PRINT("Inside _ThreadDecryptLocal\n");
	// If we didn't have a good download just prior, do nothing
	if ((m_threadStatus != THREAD_STATUS_READY) || (m_threadError != PATCH_ERROR_NONE))
		return;

	// Likewise if the buffer is not in a good state
	if (!m_buffer || (m_bufferSize <= 0))
		return;

	// Make sure the contents are what we expect
	if (m_bufState != BUFF_STATE_ENCRYPTED)
		return;

	PATCH_PRINT("Buffer is good... about to decrypt...\n");

	// Current thread status...
	m_threadStatus = THREAD_STATUS_DECRYPTING;

#ifdef DISABLE_DNAS
	m_clearSize = m_bytesReceived;
#else
	PATCH_PRINT("Buffer is good... about to get ExtractDataLength...\n");
	// Make sure our buffer is big enough
	u_int newSize = 0;
	int status = sceDNAS2InstExtractDataLength_mc(m_storeSize, (u_char*)m_buffer, &newSize);
	PATCH_PRINT("~~~~~~~~~ ExtractDataLength: returned %d, newSize = %d\n", status, newSize);
	if (status || (newSize <= 0))
	{
		PATCH_PRINT("~~~~~~~~~~~ Aborting decrypt; buffersize=%d\n", m_bufferSize);
		// Error getting the decrypted size
		m_threadStatus = THREAD_STATUS_DECRYPTERROR;
		m_threadError = PATCH_ERROR_DECRYPT_SIZE;
		return;
	}

	if (newSize > (u_int)m_bufferSize)
	{
		PATCH_PRINT("~~~~~~~~~~~ Aborting decrypt; buffersize=%d\n", m_bufferSize);
		// Error getting the decrypted size; the data is bad or the buffer too small
		m_threadStatus = THREAD_STATUS_DECRYPTERROR;
		m_threadError = PATCH_ERROR_BUF_TOO_SMALL;
		return;
	}

	// Make sure the clear size is accurate.
	if (newSize != (u_int)m_clearSize)
	{
		// Developer warning...
		PATCH_PRINT("Warning: decrypted (clear) size %d from DNAS is different from size %d from version file\n", newSize, m_clearSize);
		PATCH_PRINT("Please update version information file.\n");

		m_clearSize = newSize;
	}

	// Decrypt the buffer for the local machine
	PATCH_PRINT("Decrypting...\n");
	status = sceDNAS2InstExtractData_mc(m_storeSize, newSize, (u_char*)m_buffer);
	PATCH_PRINT("...Done decrypting, status = %d\n", status);
	if (status)
	{
		PATCH_PRINT("~~~~~~~~ Aborting!\n");
		// Error encrypting the data
		m_threadStatus = THREAD_STATUS_DECRYPTERROR;
		m_threadError = PATCH_ERROR_DECRYPT;
		return;
	}

	// Print out the first bit for debugging
	PATCH_PRINT("------------- First 16 bytes ---------------------\n");
	int j;
	for (j = 0; j < 16; j++)
		PATCH_PRINT(" %02x", (int)((u_char)m_buffer[j]));
	PATCH_PRINT("  |\n");
	for (j = 0; j < 16; j++)
		PATCH_PRINT("  %c", ((m_buffer[j] > 32) && (m_buffer[j] < 127)) ? m_buffer[j] : '.');
	PATCH_PRINT("\n-------------------------------------------------\n");


#endif // DISABLE_DNAS

	// Done!
	m_bufState = BUFF_STATE_CLEAR_DATA;
	m_threadStatus = THREAD_STATUS_READY;
	m_threadError = PATCH_ERROR_NONE;
}
