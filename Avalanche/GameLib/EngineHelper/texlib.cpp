///////////////////////////////////////////////////////////////////////////////
// Simple class to manage texture libraries
///////////////////////////////////////////////////////////////////////////////
#include "EngineHelper/EngineHelperPCH.h"

#include "game/managers/resourcemanager.h"
#include "EngineHelper/texlib.h"
#include "Game\Managers\SpriteMgr.h"

#ifdef PS2
#include "Display/PS2/Renderer/Code/MasterList.h"
#endif

// We can only have one file open at a time
TextureLibrary *TextureLibrary::s_processingFile = NULL;

///////////////////////////////////////////////////////////////////////////////
// Load a dbl of textures!
///////////////////////////////////////////////////////////////////////////////
TextureLibrary::TextureLibrary(
const char *dbl,
bool loadAsync)
{
	// Make the real name
	char realFile[128];
	strcpy(realFile, dbl);
	if (strlen(dbl) > 4 && strcmp(&dbl[strlen(dbl) - 4], ".dbl") != 0)
		strcat(realFile, ".dbl");

	// Duh
	ASSERT(strlen(realFile) < sizeof(m_dbl));
	strcpy(m_dbl, realFile);

	// Set up everything
	m_state = TEXLIB_IDLE;

	m_fileHandle = NULL;
	m_fileBuffer = NULL;
	m_fileSize = 0;

	m_handle = INVALID_HANDLE;
	m_textureDB = NULL;
	m_textureArray = NULL;

	Reload(loadAsync);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextureLibrary::~TextureLibrary()
{
	Unload();
}

///////////////////////////////////////////////////////////////////////////////
// Process & garbage collect
///////////////////////////////////////////////////////////////////////////////
TextureLibrary::State TextureLibrary::Process()
{
	// Process pedning loads
	if (m_state == TEXLIB_PENDINGLOAD)
		ProcessPendingLoad();

	// Auto advance states
	if (m_state == TEXLIB_LOADDONE)
		m_state = TEXLIB_FIRSTREADY;
	else if (m_state == TEXLIB_FIRSTREADY)
		m_state = TEXLIB_READY;

	return m_state;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextureArray *TextureLibrary::GetArray()
{
	if (!m_textureArray && m_textureDB)
	{
		// Create this only if they want it
		m_textureArray = new TextureArray;
		if (m_textureArray)
			m_textureArray->AddEntireDatabase(m_textureDB);
	}

	return m_textureArray;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextureLibrary::Unload()
{
	// Load in progress?
	if (m_fileHandle)
	{
		m_fileHandle->AwaitIO(-1);	// block until complete (we can't release the buffer with a async load in process)
		UnlockFileProcessing();
		m_fileHandle->Close();
		m_fileHandle = NULL;
	}

	// File buffer around?
	if (m_fileBuffer)
	{
		memFree(m_fileBuffer);
		m_fileBuffer = NULL;
	}
	m_fileSize = 0;

	// Did they request a texture array?
	if (m_textureArray)
	{
		delete m_textureArray;
		m_textureArray = NULL;
	}

	// Dbl owns these
	m_textureDB = NULL;
	m_textures = 0;

	// Do the big daddy
	if (m_handle != INVALID_HANDLE)
	{
		dblUnlock(m_handle);
#ifdef PS2
		// Tyler said this shouldn't hurt....
		MasterList::Flush();
#endif
		if (dblRemoveIfUnused(m_handle))
			TextureMgr::Cache().Reset();
 		m_handle = INVALID_HANDLE;
	}

	m_state = TEXLIB_IDLE;
}

///////////////////////////////////////////////////////////////////////////////
// Reload
///////////////////////////////////////////////////////////////////////////////
void TextureLibrary::Reload(bool async)
{
	// Make sure nothing is in progress
	Unload();

	m_handle = dblFindFile(m_dbl);
	if (m_handle != INVALID_HANDLE)
	{
		m_state = TEXLIB_IDLE;
		FinishLoad(false);
	}
	else if (async)
		 m_state = TEXLIB_PENDINGLOAD;
	else
	{
		// Just load it now!
		m_handle = dblLoad(m_dbl);
		if (m_handle == INVALID_HANDLE)
			m_state = TEXLIB_ERROR;
		else
		{
			m_state = TEXLIB_IDLE;
			FinishLoad();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextureLibrary::ProcessPendingLoad()
{
	// Bail if this guy is working
	if (g_resourceManager.LoadsPending())
		return;

	// We better be waiting for load
	ASSERT(m_state == TEXLIB_PENDINGLOAD);

	// start load
	if (!m_fileHandle && !FileProcessing())
	{
		LockFileProcessing();
		m_fileHandle = Media::StartAsyncLoad(m_dbl, &m_fileBuffer, &m_fileSize);
		if (!m_fileHandle)
		{
			m_state = TEXLIB_ERROR;
			UnlockFileProcessing();
		}
	}

	// Make sure this is a very true statement
	ASSERT((s_processingFile != this && m_fileHandle == NULL) || (s_processingFile == this && m_fileHandle != NULL));

	t_Error error;
	if (m_fileHandle && m_fileHandle->AwaitIO(0, NULL, NULL, &error))
		FinishLoad();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextureLibrary::FinishLoad(
bool owner)
{
	ASSERT(m_state == TEXLIB_IDLE || m_state == TEXLIB_PENDINGLOAD);

	// Close the file, if open
	if (m_fileHandle)
	{
		UnlockFileProcessing();
		m_fileHandle->Close();
		m_fileHandle = NULL;
	}

	// Was this an async?
	if (m_fileBuffer)
	{
		// Did the file get loaded directly outside of us?
		m_handle = dblFindFile(m_dbl);
		if (m_handle == INVALID_HANDLE)
		{
			m_handle = dblLoad(m_dbl, m_fileBuffer, m_fileSize);
			ASSERT(m_handle != INVALID_HANDLE);
			m_fileBuffer = NULL; // dbl owns this now
			m_fileSize = 0;
		}
		else
		{
			// Someone loaded it while we were streaming it!
			memFree(m_fileBuffer);
			m_fileBuffer = NULL;
			m_fileSize = 0;
		}
	}

	// Do we have a valid handle?
	if (m_handle == INVALID_HANDLE)
		m_state = TEXLIB_ERROR;
	else
	{
		// Finish stetting up everything
 		m_textureDB = dblGetTextures(m_handle, DEFAULT_CHUNK_KEY_STRING);
		if (!m_textureDB)
			m_state = TEXLIB_ERROR;
		else
		{
			m_textures = m_textureDB->TextureCount();
			m_state = TEXLIB_LOADDONE;
		}
		dblLock(m_handle);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextureLibrary::LockFileProcessing()
{
	ASSERT(s_processingFile == NULL);
 	s_processingFile = this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextureLibrary::UnlockFileProcessing()
{
	ASSERT(s_processingFile == this);
	s_processingFile = NULL;
}


