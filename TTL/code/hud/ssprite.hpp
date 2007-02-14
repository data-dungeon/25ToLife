///////////////////////////////////////////////////////////////////////////////
// Simple text based instrument
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline StreamedSpriteArray<count>::StreamedSpriteArray()
{
	m_texLib = NULL;
	m_state = SSS_NOTEXTURE;
	for (unsigned int c = 0; c < count; c++)
	{
		m_textureIndex[c] = c;
		m_textureAssigned[c] = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline StreamedSpriteArray<count>::~StreamedSpriteArray()
{
	UnbindSprites(true);
	delete m_texLib;
}

///////////////////////////////////////////////////////////////////////////////
// Setup the texture
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline bool StreamedSpriteArray<count>::SetTextureDB(
const char *textureDB)
{
	// Is everything already kosher?
	if (m_texLib && textureDB && strcmpi(m_texLib->GetName(), textureDB) == 0)
		return true;

	// Make sure the sprite is unbound
	UnbindSprites(true);

	// Delete the old one
	if (m_texLib)
	{
		delete m_texLib;
		m_texLib = NULL;
		m_state = SSS_NOTEXTURE;
	}

	// Create the new one
	if (textureDB)
	{
		m_texLib = new TextureLibrary(textureDB, true);
		if (!m_texLib)
			m_state = SSS_BADMOJO;
		else
			m_state = SSS_WAITINGFORLOAD;
	}

	// Wee!
	return (m_texLib != NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline bool StreamedSpriteArray<count>::SetTexture(
unsigned int which,
int textureIndex)
{
	ASSERT(which < count);
	if (m_textureIndex[which] != textureIndex)
		BindSprite(which, textureIndex);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Call this once per frame (does not call sprite update)
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline StreamedSpriteState StreamedSpriteArray<count>::Process()
{
	if (m_state == SSS_WAITINGFORLOAD)
	{
		// Are we still waiting for load?
		ASSERT(m_texLib);
		if (m_texLib && m_texLib->Process() == TextureLibrary::TEXLIB_FIRSTREADY)
		{
			m_state = SSS_INITIALIZE;
			BindSprites();
		}
	}
	else if (m_state == SSS_INITIALIZE)
		m_state = SSS_READY;

	return m_state;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline void StreamedSpriteArray<count>::BindSprite(
unsigned int which,
int textureIndex)
{
	// Save it!
	m_textureIndex[which] = textureIndex;

	// If we are ready, do it!
	if (m_state == SSS_READY || m_state == SSS_INITIALIZE)
	{
		ASSERT(m_texLib);
		UnbindSprite(which, false);
		m_sprite[which].SetTextureSet(m_texLib->GetHandle(), textureIndex);
		m_sprite[which].Reset();
		m_textureAssigned[which] = true;

		// The now need re-setup this sprite
		m_state = SSS_INITIALIZE;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline void StreamedSpriteArray<count>::BindSprites()
{
	for (unsigned int c = 0; c < count; c++)
		BindSprite(c, m_textureIndex[c]);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline void StreamedSpriteArray<count>::UnbindSprite(
unsigned int which,
bool force)
{
	if (m_textureAssigned[which])
	{
		m_sprite[which].ClearTextureSet(force);
		m_textureAssigned[which] = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <unsigned int count>
inline void StreamedSpriteArray<count>::UnbindSprites(bool force)
{
	for (unsigned int c = 0; c < count; c++)
		UnbindSprite(c, force);
}

