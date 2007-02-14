///////////////////////////////////////////////////////////////////////////////
// Streamed sprite support
///////////////////////////////////////////////////////////////////////////////
#ifndef SSPRITE_H
#define SSPRITE_H

#include "hud\sprite.h"
#include "enginehelper\texlib.h"

// 20 years and templates still sort of suck
enum StreamedSpriteState
{
	SSS_NOTEXTURE,     		// Nothing assinged
	SSS_WAITINGFORLOAD,		// Waiting for a load
	SSS_INITIALIZE,	  		// Initialize the sprite (nexttime returns ready)
	SSS_READY,					// Ready to draw
	SSS_BADMOJO					// Something is wrong
};

template <unsigned int count> class StreamedSpriteArray
{
public:
	// Constructor
	inline StreamedSpriteArray();
	inline virtual ~StreamedSpriteArray();

	// Setup the texture
	inline bool SetTextureDB(const char *textureDB);
	inline bool SetTexture(unsigned int sprite, int textureIndex);

	// Call this once per frame (does not call sprite update)
	inline StreamedSpriteState Process();

	// Check the state without advancing anything
	inline StreamedSpriteState PeekState() const
		{ return m_state; }

	// Get at the sprite!
	inline Sprite &GetSprite(unsigned int index)
		{ ASSERT(index < count); return m_sprite[index]; }

	// Get the texture handle....be extremely careful with this
	// because it gets invaid when you do a SetTextureDB or
	// delete this object
	inline t_Handle GetTextureDB()
		{ return m_texLib ? m_texLib->GetHandle() : INVALID_HANDLE; }

	inline void ShowAll(bool show)
		{ for (unsigned int c = 0; c < count; c++) m_sprite[c].Show(show); }

protected:
	TextureLibrary			*m_texLib;
	StreamedSpriteState	m_state;
	int						m_textureIndex[count];
	bool						m_textureAssigned[count];
	Sprite					m_sprite[count];

	inline void BindSprite(unsigned int which, int textureIndex);
	inline void BindSprites();
	inline void UnbindSprite(unsigned int which, bool force);
	inline void UnbindSprites(bool force);
};

class StreamedSprite : public StreamedSpriteArray<1>
{
public:
	StreamedSprite() {}
	~StreamedSprite() {}

	// Some shortcuts
	bool SetTexture(int textureIndex)
		{ return StreamedSpriteArray<1>::SetTexture(0, textureIndex); }

	// Get at the sprite!
	Sprite &GetSprite()
		{ return m_sprite[0]; }
};

// The inlines!
#include "hud/ssprite.hpp"

#endif
