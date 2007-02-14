//////////////////////////////////////////////////////
// Pain Body effects - Pain_Decal.h
//
// Paul Blagay  9/8/04
// Avalanche Software 2004
//////////////////////////////////////////////////////
#ifndef _PAIN_DECAL_H
#define _PAIN_DECAL_H

class CPainDecal
{
public:
	// The parts we can apply pain to
	enum Zone
	{
		PAINZONE_HEAD = 0,
		PAINZONE_BACK,
		PAINZONE_CHEST,
		PAINZONE_SHOULDER_L,
		PAINZONE_SHOULDER_R,
		PAINZONE_LEG_L,
		PAINZONE_LEG_R,

		PAINZONES
	};

	// Initialize & load the testures
	static bool	LoadPainTextures(char* fileName);
	static bool	UnloadPainTextures();

	// Actor support

	// Reset all textures
	static void ResetAll(CCompActor &actor);

	// Apply a pain stage
	static int GetStages(Zone zone);
	static bool	ApplyPain(CCompActor &actor, Zone zone, int stage);
	static bool	RemovePain(CCompActor &actor, Zone zone);
	static void UpdateChanges(CCompActor &actor);

private:
	// The pain zones
	struct PainZone
	{
		char			*name;
		int			offset;
		int			stages;
	};
	static PainZone s_painZone[PAINZONES];

	// Our texture DB
	static TextureDatabase *s_textureDB;
};

class Stumpy
{
public:
	static bool	CanDecapitate(CCompActor &actor);

	static bool	Decapitate(CCompActor &actor);
	static bool	Recapitate(CCompActor &actor);

	static bool	Decapitated(CCompActor &actor);

private:
	static const char *s_stumpName;
};


#endif  // _PAIN_DECAL_H
