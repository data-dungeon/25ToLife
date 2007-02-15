/////////////////////////////////////////////////////////////////////////////
// Here is the class the manages lensflares
/////////////////////////////////////////////////////////////////////////////
#ifndef LENSFLARE_H
#define LENSFLARE_H

#include "Effects/sseffect.h"
#include "Game/Managers/SpriteMgr.h"
#include "Effects/lensele.h"

class LensFlare : public ScreenSpaceEffect
{
public:
	LensFlare(void);
	~LensFlare(void);

	// Initialize/terminate the system
	static void Initialize();
	static void Terminate();

	// Init the main lens flare
	bool SetSource(Vector3 &sourcePosition);
	bool SetClipFactor(float xClipFactorValue, float yClipFactorValue);

	// Setup the database
	bool LoadSprites(char *spriteDBL);

	// Create the elements and variables from a script file
	// The script should have:
	// boolean --> enabled
	// Vector3 --> source
	// float -> blindcone
	// if elementInit is NULL, a generic set of elements is used (which assumes
	//																					3 sprites in the table)
	bool Create(char *script, LENSELEMENTINIT *elementInit = NULL, uint elementsInitCount = 0);

	// create without using a script
	// if elementInit is NULL, a generic set of elements is used (which assumes
	//																					3 sprites in the table)
	bool Create(Vector3 &source, float blindCone,
					LENSELEMENTINIT *elementInit = NULL, uint elementInitCount = 0);

	// Set an ignore entity for collisions
	void SetIgnoreEntity(CStageEntity *entity)
		{ ignoreEntity = entity; }

	// Draw
	void Update(CameraScene *scene);

	// Set a noise frequency
	void SetupVelocityNoise(float freq, float amp, float maxVel)
		{ noiseFrequency = freq; noiseAmp = amp; noiseMaxVel = maxVel; }
	void EnableVelocityNoise(bool enable)
		{ velocityNoise = enable; }

	// Hide/show the lens flare
	void Hide(void)
		{ hidden = true; }
	void Show(void)
		{ hidden = false; }

private:
	t_Handle					hTexture;
	bool						haveLensFlare;
	Vector3					source;
	Vector2					clipFactor;

	// The ignore entity
	CStageEntity			*ignoreEntity;

	LensFlareElement		*element;
	uint						elements;
	bool						enabled;
	t_Handle					overBrightSprite;
	float						blindCone;

	// Are we visible?
	// THIS IS A HACK FOR SPLIT SCREEN
	// MAKE THIS NICER EVENTUALLY
	bool						hidden;

	// Stuff for noise
	bool						velocityNoise;
	float						noiseFrequency;
	float						noiseAmp;
	float						noiseMaxVel;
	Vector3					lastCam;
	float						lastCamTime;

	// Init the elements from a structure
	bool InitElements(LENSELEMENTINIT *elementInit, uint howManyElements);

	// Toggle the enabled state
	bool Enable(bool newEnable);

	// Overbright sprite internals
	void SetupOverBrightSprite(int textureID);
	void ShowOverBrightSprite(float dot, float dotFactor);
	void HideOverBrightSprite(void);
};

// There can be only one!
extern LensFlare *g_lensFlarePtr;
inline LensFlare &SafePtr(LensFlare *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_lensFlare SafePtr(g_lensFlarePtr)

#endif
