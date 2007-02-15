///////////////////////////////////////////////////////////////////////////////
// Simple class to manage texture libraries
///////////////////////////////////////////////////////////////////////////////
#ifndef TEXLIB_H
#define TEXLIB_H

#include "game\managers\texturemgr\texturemgr.h"

class TextureLibrary
{
public:
	TextureLibrary(const char *name, bool loadAsync);
	~TextureLibrary();

	// Check & allow the state to process
	enum State
	{
		TEXLIB_ERROR,
		TEXLIB_IDLE,
		TEXLIB_PENDINGLOAD,
		TEXLIB_LOADDONE,
		TEXLIB_FIRSTREADY, // Returned once b4 ready so you can see the edge
		TEXLIB_READY
	};
	State Process();

	// Access stuff
	const char *GetName()
		{ return m_dbl; }

	// Get the dbl handle
	t_Handle GetHandle()
		{ return m_handle; }

	// Get at the texture database
	TextureDatabase *GetTextureDB()
		{ return m_textureDB; }

	// How many textures are there?
	int Textures()
		{ return m_textures; }

	// Get at the texture array
	// (won't new this by default unless you ask for it)
	TextureArray *GetArray();

	// Force an unload
	void Unload();

	// Reload (make sure you unload)
	void Reload(bool async);

private:
	// The dbl in question
	char					m_dbl[64];
	bool					m_owner;

	// Our current state
	State					m_state;

	// The async info
	MediaFile			*m_fileHandle;
	void					*m_fileBuffer;
	int					m_fileSize;

	// The loaded dbl
	t_Handle				m_handle;
	int					m_textures;
	TextureDatabase	*m_textureDB;
	TextureArray		*m_textureArray;

	void ProcessPendingLoad();
	void FinishLoad(bool owner = true);

	// We can only have one file open at a time
	static TextureLibrary *s_processingFile;
	void LockFileProcessing();
	void UnlockFileProcessing();
	bool FileProcessing()
		{ return (s_processingFile != NULL); }
};

#endif
