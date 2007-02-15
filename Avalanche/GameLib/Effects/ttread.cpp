/***************************************************************************/
/***************************************************************************/

/******************* includes ****************************/

#include "Effects/EffectsPCH.h"


#include "Effects/ttrack.h"


/***************************************************************************/
// Create stuff
/***************************************************************************/
TireTreadType::TireTreadType(
TireTrackSystem &theSystem,
TREADTEXTUREID theTexture,
float baseAlpha) :
system(theSystem)
{
	// Save this off
	texture = new char[( strlen( theTexture) + 1)];
	strcpy(texture, theTexture);

	// Load the dataset
	hTexture = dblLoad(theTexture);

	// Build the texture context
	BuildTextureContext(hTexture, baseAlpha);

	list.CallDestructors(false);
}

/***************************************************************************/
// Clean up
/***************************************************************************/
TireTreadType::~TireTreadType(void)
{
	delete[] texture;
}

/***************************************************************************/
// Get a new tire track
/***************************************************************************/
TireTrack *TireTreadType::New(void)
{
	// Ask the system for a new track
	TireTrack *track = system.New(*this);
	if (track)
	{
		track->SetTextureContext(textureArray, &textureContext);
		list.AddTail(track);
	}

	return(track);
}

/***************************************************************************/
// Remove this guy
/***************************************************************************/
void TireTreadType::Delete(
TireTrack *killMe)
{
	if (killMe)
 	{
		list.Remove(killMe);
		system.Delete(killMe);
 	}
}

/***************************************************************************/
// Render the whole group
/***************************************************************************/
//  void TireTreadType::Render(
//  TireTrackDisplay &displayTrack,
//  Graphics4x4 &camera,
//  float maxDrawDistance)
//  {
//  	// Get the root pointer to the head
//  	TireTrack *track = list.Head();
//  	if (!track)
//  		return;

//  	// do the list
//  	do {
//  		bool draw = !CullTireTrack(camera, *track, maxDrawDistance);
//  		track->SetDrawnLastFrame(draw);
//  		if (draw)
//  			track->Render(displayTrack);
//  		track = track->next;
//  		}
//  	while (track);
//  }

/***************************************************************************/
/***************************************************************************/
void TireTreadType::Clear(void)
{
	TireTrack *track = list.Head();
	while (track)
	{
		Delete(track);
		track = list.Head();
	}
}
/***************************************************************************/
/***************************************************************************/
void TireTreadType::BuildTextureContext(
t_Handle hTexture,
float baseAlpha)
{
	// Someday we have to actually load in the texture
	memset(&textureContext, '\0', sizeof(textureContext));

	// Set a base color, even with no texture
	textureContext.BaseColor.R = 255;
	textureContext.BaseColor.G = 255;
	textureContext.BaseColor.B = 255;
	textureContext.BaseColor.A = (int)(baseAlpha * 255.0f);

	if (hTexture != INVALID_HANDLE)
	{
		// Set the bind number!!!!!!
		textureContext.TextureCount = 1;

		TextureDatabase* textureDatabase = (TextureDatabase*)dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, hTexture);
		if(!textureDatabase)
			return;

		textureContext.TextureLayer[0].textureDatabase = textureDatabase;
		// All of your base are belong to us
		textureContext.TextureLayer[0].s16TextureIndex = 0;

		// Hack up a bunch of this shizit
		textureContext.TextureLayer[0].u8Method = TS_METHOD_MODULATE;
		textureContext.TextureLayer[0].u8Blend = RS_BLENDFUNC_DEFAULT;
		textureContext.TextureLayer[0].u8WrapU = TS_WRAP_REPEAT;
		textureContext.TextureLayer[0].u8WrapV = TS_WRAP_REPEAT;
		textureContext.TextureLayer[0].u8Flags = 0; //TEXTURE_IS_DIFFUSE_MAP;
	}
}
