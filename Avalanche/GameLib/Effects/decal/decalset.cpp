///////////////////////////////////////////////////////////////////////////////
// decal system!
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DecalSet::DecalSet()
{
	// Assume the worst
	m_texture = INVALID_HANDLE;
	m_textureDB = NULL;
	m_textureArray = NULL;
	m_textures = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DecalSet::DecalSet(
const char *textureDBL,
bool markPermanent)
{
	// Assume the worst
	m_texture = INVALID_HANDLE;
	m_textureDB = NULL;
	m_textureArray = NULL;
	m_textures = 0;

	// Load it
	Load(textureDBL, markPermanent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DecalSet::~DecalSet()
{
	Destroy();
}

///////////////////////////////////////////////////////////////////////////////
// Is everything okay?
///////////////////////////////////////////////////////////////////////////////
bool DecalSet::Load(
const char *textureDBL,
bool markPermanent)
{
	ASSERT(textureDBL);

	return Create(dblLoad(textureDBL), markPermanent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool DecalSet::Create(
t_Handle fileDBL,
bool markPermanent)
{
	if (fileDBL == INVALID_HANDLE)
		return false;

	dblLock(fileDBL);

	// Make sure to set this
	if (markPermanent)
		dblSetPermanent(fileDBL, true);

	// Ooops
	ASSERT(m_texture == INVALID_HANDLE);
	ASSERT(fileDBL);

	m_texture = fileDBL;

	// Create using a texture database
	return Create(dblGetTextures(m_texture, DEFAULT_CHUNK_KEY_STRING));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool DecalSet::Create(TextureDatabase *textureDB)
{
	if (!textureDB)
		return false;

	m_textureDB = textureDB;
	m_textureArray = new TextureArray();
	m_textureArray->AddEntireDatabase(m_textureDB);
	m_textures = m_textureDB->TextureCount();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool DecalSet::Create(
int count,
int width,
int height,
int format)
{
	ASSERT(m_texture == INVALID_HANDLE );
	ASSERT(m_textureDB == NULL);
	ASSERT(m_textureArray == NULL);

	m_textureArray = new TextureArray(count);
	ASSERT(m_textureArray);

	for (int i = 0; i < count; i++)
	{
		Texture *texture = TextureMgr::CreateSpecialTexture(0, 0, format, width, height, "DecalSet");

		if (texture)
			m_textureArray->AddTexture(texture);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalSet::Destroy(
bool forceUnload)
{
	delete m_textureArray;

	if (m_texture != INVALID_HANDLE)
	{
		dblUnlock(m_texture);
		if (forceUnload && dblRemoveIfUnused(m_texture))
			TextureMgr::Cache().Reset();
	}

	m_textureDB = NULL;
	m_textureArray = NULL;
	m_textures = 0;
	m_texture = INVALID_HANDLE;
}
