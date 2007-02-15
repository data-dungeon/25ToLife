//------------------------------------------------------------------------------
// Patcher.h
//
// Source for game patching library.
//
// Author: Dan Stanfill, Pinniped Software. http://pinniped.com
//         Under contract to Eidos, Inc.
// 
// Copyright (c) 2004-2005, Eidos, Inc. All rights reserved.
//------------------------------------------------------------------------------

#ifndef __PATCHER_H__
#define __PATCHER_H__ 1

#include "PatcherCommon/patchplatform.h"
#include "Patcher_platform.h"
#include "PatchCommandThread.h"
#include "PatchError.h"

// Host name max string length
#define PATCHER_HOST_MAX 64

// Resource or path max string length
#define PATCHER_PATH_MAX 128

class PATCHER_EXPORT Patcher {
public:
	// If this engine/platform requires it, do its per-class preamble
	PLATFORM_PREAMBLE;

	// Patcher status types, used for function returns
	enum Status {
		PATCH_STATUS_OK = 0,		// No errors, all is groovy
		PATCH_STATUS_YES,			// The answer is yes. :-)
		PATCH_STATUS_NO,			// The answer is no.
		PATCH_STATUS_NOT_FOUND,		// The given item is not in the patch, or a description is not available
		PATCH_STATUS_INIT_FAILED,	// The work thread does not exist
		PATCH_STATUS_BUSY,			// The work thread is currently busy
		PATCH_STATUS_BAD_REQUEST,	// The request does not make sense given the current state of the patcher.
		PATCH_STATUS_NET_ERROR,		// An error occurred in the work thread. Query network status for more info
		PATCH_STATUS_LOAD_ERROR,	// Module's code could not be relocated. Bad DLL?
		PATCH_STATUS_PATCH_ERROR,	// The patch file is no good; possibly hacked?
		PATCH_STATUS_MEM			// Out of memory, unable to continue
	};

	enum ThreadStatus {
		THREAD_STATUS_NOT_READY,	// The thread has not been started yet
		THREAD_STATUS_READY,		// The thread is ready and waiting, no other info
		THREAD_STATUS_NEED_UPDATE,	// The thread is ready; we need an update
		THREAD_STATUS_HOSTLOOKUP,	// The work thread is looking up the patch host name
		THREAD_STATUS_REQUESTING,	// An http request is being sent
		THREAD_STATUS_RECEIVING,	// The work thread is receiving data
		THREAD_STATUS_ENCRYPTING,	// The data buffer is being personalized or encrypted for the local machine
		THREAD_STATUS_DECRYPTING,	// The data buffer is being decrypted for use

		THREAD_STATUS_ERROR = 1024,	// Any status greater than this one indicates an error
		THREAD_STATUS_ENCRYPTERROR,	// An error occurred during encryption
		THREAD_STATUS_DECRYPTERROR,	// An error occurred during decryption
		THREAD_STATUS_VERIFYERROR,	// An error occurred verifying the patch integrity
		THREAD_STATUS_CONFIGERR,	// There was an error setting up the configuration from the version file
		THREAD_STATUS_NETWORKERR	// A network error occurred in the work thread
	};

	// Patcher states. We are just a simple state machine.
	// We use the higher order bits to indicate the readiness state of the patcher
	// thread.
	enum PatchState {
		PATCH_STATE_INIT = 0x0000,		// We need to initialize

		// States which are ready for commands
		PATCH_STATE_READY_FLAG = 0x0100,
		PATCH_STATE_READY,				// We are initialized, but have done nothing. Waiting for a command. Buffer is null.
		PATCH_STATE_NEED_UPDATE,		// We need to fetch an update
		PATCH_STATE_UPDATE_READY,		// The update has been downloaded, ready to decode and apply
		PATCH_STATE_NO_UPDATES,			// Up do date; no updates needed or updating complete
		PATCH_STATE_CLEAR_PATCH,		// The patch data is in the clear (decrypted) and ready in the buffer

		// States which indicate we are busy doing something
		PATCH_STATE_BUSY_FLAG = 0x0200,
		PATCH_STATE_START_VERSION,		// A version check is requested and will be started. 
		PATCH_STATE_CHECKING_VERSION,	// A version check has been initiated
		PATCH_STATE_START_UPDATE,		// An update has been requested, but the thread doesn't know yet
		PATCH_STATE_UPDATING,			// We are fetching an update
		PATCH_STATE_START_DECRYPT,		// Decrypt of buffer has been requested, notify the thread
		PATCH_STATE_DECRYPTING,			// Decrypt of buffer currently under way
		PATCH_STATE_START_DESCRIPTION,	// A description file is requested and will be downloaded
		PATCH_STATE_GETTING_DESCRIPTION,// The description file is being downloaded

		// States which indicate an error has occurred
		PATCH_STATE_ERROR_FLAG = 0x0400,
		PATCH_STATE_ERROR,				// An error has occurred, but we have not shut down (you can try again)
		PATCH_STATE_HALTED,				// We halted because of an error

		// Mask to just check ready state
		PATCH_STATE_READY_MASK = 0x0700
	};

	// Buffer states. We need to track the state of the data in the buffer
	enum BufferState {
		BUFF_STATE_NULL,				// Initial state. The buffer pointer should be null
		BUFF_STATE_TEMP,				// Buffer pointer is temporarily overridden. Do not touch!
		BUFF_STATE_FILLING,				// The buffer is currently being filled from the network
		BUFF_STATE_DOWNLOADED,			// The buffer contains unencrypted data from a successful download
		BUFF_STATE_ENCRYPTED,			// The buffer contains encrypted data from a successful update
		BUFF_STATE_CLEAR_DATA			// The buffer contains clear decrypted data
	};

	// Thread run states
	enum TRunState {
		THREAD_STATE_READY,				// Thread is waiting for a message from main thread
		THREAD_STATE_BUSY,				// Thread is busy running (won't take messages). See thread status for details
		THREAD_STATE_QUIT				// Thread is exiting or has exited (cannot accept messages)
	};

	// Patcher messages. Used to send messages from the main thread to the worker thread
	enum Message {
		PATCH_MSG_NOOP,					// Do nothing (for debugging)
		PATCH_MSG_CHECK_VERSION,		// Perform version check
		PATCH_MSG_FETCH_UPDATE,			// Fetch the current update
		PATCH_MSG_DECRYPT,				// Decrypt the current buffer
		PATCH_MSG_GET_DESCRIPTION,		// Download the description file from the server
		PATCH_MSG_EXIT					// Please exit the thread
	};

	// CompletionInfo -- Indicates the current state of the Patcher. If the Patcher
	// has determined that a patch is needed, the size of the buffer required to
	// get the patch is set.
	struct CompletionInfo {
		int			reqBufSize;		// If non-zero, an update is needed and this is how big a buffer is required to get it
		int			storeSize;		// Size required on memory card to save the encrypted update
		int			clearSize;		// Size of patch when fully decrypted
		bool		mandatory;		// Patch is tagged as a mandatory patch
	};

	// One-time initialization function to be called at engine
	// startup time or when the game determines that it may need
	// to check for patches. Creates the static singleton Patcher instance.
	// Note: Our static pointer cannot be reliably initialized to NULL with
	// the SN DLL system, so Create MUST be called before any other Patcher
	// functions.
	static Status		Create(int threadPriority);

	// Destroy frees all allocated resources in the singleton Patcher, and indicates
	// that no further Patcher calls will be made. The Patcher DLL can be removed
	// from memory at this point.
	static void			Destroy(void);

	// Get a pointer to our static Patcher. Result undefined if Create has not
	// been called, because we are unable to initialize the static pointer to NULL
	// in a DLL!!
	static Patcher*		GetPatcher(void);

	// Between the Create and the Destroy functions, the Update
	// function should be called frequently, such as once per frame,
	// to provide a synchronization point with the game engine and to
	// check for completion of asynchronous operations.
    void Update(float dt);

	// BeginVersionCheck() will begin the check to see whether a patch is available
	// or not, and collect information (such as size, etc.) about the patch. This
	// function begins an asynchronous operation, so use TestCompletion to determine
	// when the check is finished.
    Status BeginVersionCheck(const char* host, const char* path, unsigned int version);

	// DownloadDescription begins downloading the description associated with the patch.
	// If PATCH_STATUS_OK is returned, used TestCompletion to wait for your buffer
	// to be filled. If the patch description is larger than the buffer size,
	// the description is truncated to the buffer. As a defense against attacks,
	// the last byte is always set to zero, so a truncated description will actually
	// be truncated to one byte less than bufferSize.
	Status DownloadDescription(char* buffer, int bufferSize);

	// DownloadPatch() begins the process of downloading and patching executable
	// data. After DownloadPatch is called, a patch which has been previously
	// determined to be available is downloaded into the given buffer, and "personalized"
	// (encrypted using parameters specific to the currently running computer). Call
	// TestCompletion() to determine when the download is complete. When the download
	// is complete, the buffer will contain the encrypted data ready to be stored by
	// the application on the storage device (memory card, hard disk, etc.).
	Status DownloadPatch(char* buffer, int bufferSize);	// Begin the patching operation

	// Start preparing the current patch for use. Assuming an encrypted (personalized)
	// patch is already stored in memory, this function will initiate an asynchronous 
	// decryption of the patch into clear data. Use TestCompletion or WaitCompletion
	// to wait for actual completion.
	Status PreparePatch(void);

	// When a patch is read off the memory card or hard drive by the application,
	// this function sets the current patch into the patcher so it can be applied.
	// The data in the buffer must be in the encrypted state used to store on
	// the memory card (DNAS personalized). As long as the patch was downloaded
	// properly in a previous run, it will be in that state.
	Status SetPatch(char* buffer, int bufferSize);

	// TestCompletion() will poll to see if any asynchronous operations have completed.
	// It will return immediately whether or not an operation is complete. The returned
	// status will indicate one of
	//	PATCH_STATUS_OK		– Any operations are complete, and CompletionInfo contains relevant information.
	//	PATCH_STATUS_BUSY	– There are outstanding asynchronous operations which have not completed. CompletionInfo is untouched.
	//	Anything else		- An error condition exists, you should abort.
    Status TestCompletion(CompletionInfo& info);

	// WaitCompletion() is the same as TestCompletion(), except that if there are
	// asynchronous operations which have not completed, it waits until one completes.
	// This function would be used in a multi-threaded game, where it is desired that
	// the testing thread not use up unnecessary CPU time spinning.
    Status WaitCompletion(CompletionInfo& info);

	// When the status from one of our other functions indicates a problem, get the
	// last error from the work thread, which is generally a network error or a dnas error.
	PatchError GetPatchError(void) {return (PatchError)m_threadError;}

	// When the patcher is busy, use GetProgress to find out what it is busy doing
	ThreadStatus GetProgress(void) {return m_threadStatus;}

	// Get the number of bytes downloaded so far during a lengthy transfer. Value
	// is undefined if we are not downloading anything.
	int GetBytesDownloaded(void);

	// Get the buffer size required to hold the patch in all its forms
	int GetBufferSize(void) const;

private:
	// Don't allow anyone outside of the Patcher itself to create or destroy a Patcher
	Patcher();
	//~Patcher() {}

	// Initialize the patch object. Does not create any resources
	// or allocate memory.
	void				_Init(void);

	// Send a message to the worker thread. Returns true if the message was sent, false otherwise
	bool				_SendMessage(Message msg);

	// Clean up in preparation for destroying (since destructor does nothing)
	void				_Shutdown(void);	

	// Private work functions, main thread
	void				_InitStateWork(void);			// Perform work for the INIT state
	void				_StartVersionWork(void);		// Work for the START_VERSION state
	void				_CheckingVersionWork(void);		// Work for the CHECKING_VERSION state
	void				_StartUpdateWork(void);			// Work for the START_UPDATE state
	void				_UpdatingWork(void);
	void				_StartDecryptWork(void);
	void				_DecryptingWork(void);
	void				_ApplyUpdateWork(void);
	void				_StartDescriptionWork(void);	// Request patch description from server
	void				_GetDescriptionWork(void);		// Downloading the patch description

	// Private work functions, network work thread
	void				_ThreadMain(void);		// Main function for Patcher thread
	void				_ThreadFetchHTTP(const char* host, const char* file, bool truncateOK);
	void				_ThreadEncryptLocal(void);
	void				_ThreadDecryptLocal(void);
	void				_ThreadParseVersion(void);

	// Static private member functions
	static void			S_PatcherThreadMain(void* param) {((Patcher*)param)->_ThreadMain();}

	// Private members, used only by main thread
	PatchState			m_state;						// Our current state
	PatchCommandThread	m_thread;						// Our command thread
	int					m_threadPriority;				// Priority of our thread
	BufferState			m_bufState;						// Current state or contents of our buffer

	// Private members, used by both threads
	Message				m_message;						// Current message from main to work thread
	ThreadStatus		m_threadStatus;					// Status of the work thread, or if thread is busy, current task (for tracking progress)
	TRunState			m_threadRunState;				// Whether the tread can accept messages or not
	int					m_threadError;					// If the thread status indicates an error, this is the error
	unsigned int		m_version;						// Version identifier provided by application

	// Private members, used by work thread
	int					m_bytesReceived;				// Number of data bytes received
	int					m_bufferSize;					// Size of data buffer pointed to by m_buffer
	int					m_downloadSize;					// Size of raw download
	int					m_storeSize;					// Size required for memory card storage of encrypted data
	int					m_clearSize;					// Size of fully decrypted patch
	int					m_mandatory;					// non-zero if this patch is mandatory
	char*				m_buffer;						// Data buffer
	char				m_host[PATCHER_HOST_MAX];		// Host name for download
	char				m_resource[PATCHER_PATH_MAX];	// Resource name for download
	char				m_description[PATCHER_PATH_MAX];// Resource name for description text

	// Static private members
	static Patcher		s_PatcherInstance;				// Our only allowed instance
};

#endif // __PATCHER_H__
