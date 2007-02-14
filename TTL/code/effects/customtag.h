///////////////////////////////////////////////////////////////////////////////
// The class to load custom tags...
///////////////////////////////////////////////////////////////////////////////
#ifndef CUSTOM_TAG_H
#define CUSTOM_TAG_H

#include "EngineHelper/Singleton.h"
#include "Math/MathClass.h"
#include "Math/fastrand.h"
#include "Effects/tizag.h"
#include "Effects/decal/staticdecal.h"
#include "Interfaces/Interface.h"

struct ts_TagParams
{
	char Text[6];
	ts_bRGBA Color[3];
};

struct CustomTagInfo
{
	Vector3		pos;
	Vector3		normal;
	Vector3		up;
	u16			color;
};

class CustomTag
{
public:
	CustomTag();
	~CustomTag();

	// Initialize everything
	void Initialize();

	static void PreloadFonts();

	Tizag* SetupEmitter(uint8 index);

private:
	// Tag source
	enum Source
	{
		TAG_NONE,
		TAG_DBL,
		TAG_CUSTOM
	};
	Source m_source;

	// The tag decal set!
#if defined(PS2) || defined(_XBOX)
	static TextureDatabase *s_textureDB;
#endif
	static DecalSet *s_decalSet;
	static int s_decalSetReferences;
	static void AcquireReference();
	static void ReleaseReference();
};

// There can be only one!
DefineSingleton(CustomTag)
#define g_customTag GetSingleton(CustomTag)

#endif
