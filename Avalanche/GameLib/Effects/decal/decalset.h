///////////////////////////////////////////////////////////////////////////////
//	A "DecalSet" is a .dbl that is loaded that contains a texture list
//	(generated from an lst file). Alternatively it can be created on the
// fly in a variety of ways.
///////////////////////////////////////////////////////////////////////////////
#ifndef DECALSET_H
#define DECALSET_H

#include "Layers/LayersStruct.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Display/Common/TextureState.h"

class DecalSet
{
public:
	// A decal set just specifies a texture dbl
	DecalSet();
	DecalSet(const char *textureDBL, bool markPermanent = false);
	~DecalSet();

	// Is everything okay?
	bool Load(const char *textureDBL, bool markPermanent = false);

	// Other ways to create a decal set
	bool Create(t_Handle fileDBL, bool markPermanent = false);
	bool Create(TextureDatabase *textureDB);
	bool Create(int count, int width, int height, int format);

	// Cleanup
	void Destroy(bool forceUnload = false);

	// Are we loaded & operational
	bool Okay()
		{ return (m_textureArray != NULL); }

	// Access our information
	inline Texture *GetTexture(int index)
		{ return (m_textureDB ? m_textureDB->GetTexture(index) : NULL); }
	inline int Textures()
		{ return m_textures; }
	inline TextureArray *GetTextureArray()
		{ return m_textureArray; }

protected:
	// Our texture database
	t_Handle							m_texture;
	TextureDatabase				*m_textureDB;
	TextureArray					*m_textureArray;
	int								m_textures;
};

#endif
