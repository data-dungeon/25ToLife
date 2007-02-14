//////////////////////////////////////////////////////
// Pain Body effects - Pain_Decal.cpp
//
// Paul Blagay  9/8/04
// Avalanche Software 2004
//////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "effects/pain_decal.h"
#include "GameHelper/SharedBody.h"
#include "main/PlayerDB.h"
#include "components/painstats.h"

// Statics
// name, offset, num_textures
CPainDecal::PainZone CPainDecal::s_painZone[PAINZONES] =
{
	{ "headshot_a", 10, 3 },				// head
	{ "backshot_a", 0, 3 },				// torso
	{ "chestshot_a", 3, 3 },
	{ "shouldershot_l_a", 6, 2},		// arms
	{ "shouldershot_r_a", 8, 2},
	{ "legshot_l_a", 6, 2},			// legs..
	{ "legshot_r_a", 8, 2},
};

TextureDatabase *CPainDecal::s_textureDB = NULL;

const char *Stumpy::s_stumpName = "avatars\\pieces\\p30.dbl";	// stump w/out head

//=================================================================
bool CPainDecal::LoadPainTextures(char * fileName)
{
	t_Handle handle = dblLoad(fileName);

	if (handle == INVALID_HANDLE)
	{
		ASSERTF(handle != 0, ("Pain Textures %s could not be found", fileName));
		return false;
	}

	if (!s_textureDB)
		s_textureDB = dblGetTextures(handle, DEFAULT_CHUNK_KEY_STRING);
	return true;
}

//=================================================================
bool CPainDecal::UnloadPainTextures()
{
	s_textureDB = NULL;
	return true;
}

//=================================================================
void CPainDecal::ResetAll(CCompActor &actor)
{
	if (!s_textureDB)
		return;

	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return;

	// Reset all the parts
	for (int i = 0; i < PAINZONES; i++)
		body->RemoveTextureSwap(s_painZone[i].name);
	body->RefreshTextureSwaps();
}

//=================================================================
int CPainDecal::GetStages(Zone zone)
{
	if (!s_textureDB)
		return 0;

	return s_painZone[zone].stages;
}

//=================================================================
bool CPainDecal::ApplyPain(CCompActor &actor, Zone zone, int stage)
{
	if (!s_textureDB)
		return false;

	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return false;

	// Make sure we are unbound
	body->RemoveTextureSwap(s_painZone[zone].name);
	if (stage >= 0)
	{
		// make sure our pain is within bounds
		if (stage >= s_painZone[zone].stages)
		stage = s_painZone[zone].stages - 1;

		stage += s_painZone[zone].offset;

		// fix up painstage...
		int index = Math::Min(stage, s_textureDB->TextureCount() - 1);
		Texture *texture = s_textureDB->GetTexture(index);
		if (texture)
			body->TextureSwap(s_painZone[zone].name, texture, INVALID_HANDLE);
	}

	return true;
}

//=================================================================
bool CPainDecal::RemovePain(
CCompActor &actor,
Zone zone)
{
	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return false;

	body->RemoveTextureSwap(s_painZone[zone].name);
	return true;
}

//=================================================================
void CPainDecal::UpdateChanges(CCompActor &actor)
{
	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (body)
		body->RefreshTextureSwaps();
}

//=================================================================
bool Stumpy::CanDecapitate(CCompActor &actor)
{
	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return false;

	// Is there a head?
	if (body->FindSOMIndexByName("head") < 0)
		return false;

	return true;
}

//=================================================================
bool Stumpy::Decapitate(CCompActor &actor)
{
	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return false;

	// Is there a head?
	if (body->FindSOMIndexByName("head") < 0)
		return false;

	if (body->FindSOMIndexByName("stump") < 0)
	{
		ts_Geometry *geom = g_loadSOM.Geometry(s_stumpName);
		if (geom && geom->pSOM)
			body->AddSOM(geom->pSOM, INVALID_HANDLE, "stump");
	}

	// switch out his head to a stump
	body->Hide("head", true);
	body->Hide("headwear", true);
	body->Hide("facialhair", true);
	body->Hide("stump", false);

	return true;
}

//=================================================================
bool Stumpy::Recapitate(CCompActor &actor)
{
	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return false;

	// switch out his head to a stump
	body->Hide("stump", true);
	body->Hide("facialhair", false);
	body->Hide("headwear", false);
	body->Hide("head", false);

	return true;
}

//=================================================================
bool Stumpy::Decapitated(CCompActor &actor)
{
	SharedBody *body = (SharedBody *) actor.Instance()->DynamicModel();
	if (!body)
		return false;

	return !body->Hidden("stump");
}
