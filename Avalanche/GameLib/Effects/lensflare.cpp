//***************************************************************************w
// The lens flare!  Cheesy and simple.
//***************************************************************************
#include "Effects/EffectsPCH.h"

#include "EngineHelper/timer.h"
#include "cmdcon/cmdcon.h"

#define SUN_SIZE			0.4f
#define CONVERT_SIZE(s)	((float)s * SUN_SIZE / 100.0f)
#define CONVERT_T(t)		((float)t / 100.0f)
#define FLARE_ALPHA		254

// Generic lensflare by joe
LENSELEMENTINIT genericLensFlare[] =
{
	{ 0, CONVERT_T(100), CONVERT_SIZE(200), 255, 255, 255, FLARE_ALPHA, false, false, 0.5f, 0.6f, 1.0f, 1.0f, 0.0f },
	{ 5, CONVERT_T(100), CONVERT_SIZE(300), 255, 255, 255, FLARE_ALPHA, true, false, 0.5f, 1.0f, 0.0f, 0.9f, 0.0f },
	{ 1, CONVERT_T(60), CONVERT_SIZE(30), 255, 255, 255, FLARE_ALPHA, false, false, 1.0f, 1.0f, 0.6f, 1.0f, 0.8f },
	{ 2, CONVERT_T(40), CONVERT_SIZE(30), 255, 255, 255, FLARE_ALPHA, false, false, 1.0f, 1.0f, 0.6f, 1.0f, 0.8f },
	{ 3, CONVERT_T(20), CONVERT_SIZE(20), 255, 255, 255, FLARE_ALPHA, false, false, 1.0f, 1.0f, 0.6f, 1.0f, 0.8f },
	{ 4, CONVERT_T(-50), CONVERT_SIZE(30), 255, 255, 255, FLARE_ALPHA, false, false, 1.0f, 1.0f, 0.6f, 1.0f, 0.8f }
};
uint genericLensFlareElements = sizeof(genericLensFlare) / sizeof(LENSELEMENTINIT);

// Our singleton
LensFlare *g_lensFlarePtr = NULL;

//***************************************************************************
//***************************************************************************
LensFlare::LensFlare(void)
{
	hTexture = INVALID_HANDLE;
	haveLensFlare = false;
	element = NULL;
	elements = 0;
	enabled = true;
	overBrightSprite = INVALID_HANDLE;
	clipFactor.Set(1.1f,
						1.1f);
	blindCone = 0.97f;

	hidden = false;

	velocityNoise = false;
	noiseFrequency = 25.0f;
	noiseAmp = 4.0f;
	noiseMaxVel = 25.0f;
	lastCam.Clear();
	lastCamTime = g_timer.GetEndSec();
}

//***************************************************************************
//***************************************************************************
LensFlare::~LensFlare(void)
{
	delete [] element;
	if (overBrightSprite != INVALID_HANDLE)
		SpriteMgr__KillSprite(overBrightSprite);

	if (hTexture != INVALID_HANDLE)
		SpriteMgr__KillSpriteGroup(LENS_GROUP);
}

//***************************************************************************
// Initialize/terminate the system
//***************************************************************************
void LensFlare::Initialize()
{
	ASSERT(!g_lensFlarePtr);
	g_lensFlarePtr = new LensFlare;
	ASSERT(g_lensFlarePtr);

	g_ssEffects.AddTail(g_lensFlarePtr);
}

//***************************************************************************
//***************************************************************************
void LensFlare::Terminate()
{
	if (!g_lensFlarePtr)
		return;

	g_ssEffects.Remove(g_lensFlarePtr);
	delete g_lensFlarePtr;
	g_lensFlarePtr = NULL;
}

//***************************************************************************
// Init
//***************************************************************************
bool LensFlare::SetSource(
Vector3 &sourcePosition)
{
	source = sourcePosition;
	return(true);
}

//***************************************************************************
// Init
//***************************************************************************
bool LensFlare::SetClipFactor(
float xClipFactorValue,
float yClipFactorValue)
{
	clipFactor.Set(xClipFactorValue,
						yClipFactorValue);

	return(true);
}

//***************************************************************************
// Load the sprite DBL
//***************************************************************************
bool LensFlare::LoadSprites(
char *spriteDBL)
{
	// duh
	if (spriteDBL == NULL)
		return(false);

	// Load the dataset
	hTexture = dblLoad(spriteDBL);
	if (hTexture == INVALID_HANDLE)
		return(false);

	return(true);
}

//***************************************************************************
// Create the elements and variables from a script file
// The script should have:
// boolean --> enabled
// Vector3 --> source
// float -> blindcone
// if elementInit is NULL, a generic set of elements is used (which assumes 3 sprites in the table)
//***************************************************************************
bool LensFlare::Create(
char *script,
LENSELEMENTINIT *elementInit,
uint elementInitCount)
{
	char *context = "flare";
	g_console.BindScript(context, script);
	g_console.CreateVar(context, "enabled", haveLensFlare);
	g_console.CreateVar(context, "Source", (Vector3Packed*)&source);
	g_console.CreateVar(context, "blindCone", blindCone);

	// Load the script
	if (script && *script)
	{
		g_console.ExecuteContext(context);
#ifndef INTERACTIVE_CONSOLE
		g_console.DeleteContext(context);
#endif //INTERACTIVE_CONSOLE
	}

	// Do they want a lens flare?
	if (!haveLensFlare)
		return(true);

	hidden = false;

	// Init it!
	if (!elementInit)
	{
		elementInit = genericLensFlare;
		elementInitCount = genericLensFlareElements;
	}
	return(InitElements(elementInit, elementInitCount));
}

//***************************************************************************
// create without using a script
// if elementInit is NULL, a generic set of elements is used (which assumes 3
// sprites in the table)
//***************************************************************************
bool LensFlare::Create(
Vector3 &source,
float blindCone,
LENSELEMENTINIT *elementInit,
uint elementInitCount)
{
	// Setup the locals
	haveLensFlare = true;
	this->source = source;
	this->blindCone = blindCone;

	// Init it!
	if (!elementInit)
	{
		elementInit = genericLensFlare;
		elementInitCount = genericLensFlareElements;
	}
	return(InitElements(elementInit, elementInitCount));
}

//***************************************************************************
// Draw
//***************************************************************************
void LensFlare::Update(
CameraScene *scene)
{
	// Anything to do?
	if (hTexture == INVALID_HANDLE || elements == 0)
		return;

	// Hide it?
	if (hidden || RenderMgr__IsFading())
	{
		Enable(false);
		return;
	}

	// what is our composite matrix
	Camera &camera = g_cameraDirector.CurrentCamera(scene);

	// Is the lens flare visible?
	Vector2 s;
	float w;
	bool onScreen = camera.ToScreen(source, s, w, clipFactor);
	if (!onScreen)
	{
		Enable(false);
		return;
	}

	// turn us on!
	Enable(true);

	// In shadow?
	CLineEntity detectEntity;
	detectEntity.Line().SetOriginEnd(camera.Eye(), source);
	detectEntity.Line().SetFindMode(CLineCollisionObject::FIND_ANY);
	detectEntity.SetIgnoreEntity(ignoreEntity);
	g_collisionSystem.ResetStack();
	bool haveHit = (g_collisionSystem.Test(&detectEntity) != NULL);

	// Figure out diff stuff
	float dx = s.x() - 0.5f;
	float dy = s.y() - 0.5f;

	// Figure out angle stuff
	float theta = Math::ArcTan2(dy, dx);
	float cosTheta = Math::Cos(theta * 0.5f);
	float sinTheta = Math::Sin(theta * 0.5f);

	// setup this stuff
  	Vector3 lookAt = source - camera.Eye();
	lookAt.Normalize();
	float dot = Vector3::Dot(camera.Dir(), lookAt);

	float dotFactor = Math::Abs(Math::ArcCos(dot)) / Math::Pi;
	float sizeDot = 1.0f - dotFactor * 4.0f;

	// Apply noise?
	float noiseMod;
	if (!velocityNoise)
		noiseMod = 1.0f;
	else
	{
		// figure out the camera velocity
		Vector3 delta = camera.Eye()- lastCam;
		float speed = delta.Length() / (g_timer.GetEndSec() - lastCamTime);
		float speedRatio = speed / noiseMaxVel;
		if (speedRatio > 1.0f)
			speedRatio = 1.0f;

		float t = g_timer.GetEndSec() * noiseFrequency;
		noiseMod = Math::Abs(g_perlinNoise.Noise(t)) * noiseAmp;

		noiseMod += (1.0f - speedRatio);

		// Clamp it
		if (noiseMod > 1.0f)
			noiseMod = 1.0f;
	}

	// Draw each element
	for (uint i = 0; i < elements; i++)
	{
		// Figure out the sprite's "position"
		float t = element[i].LinePosition();
		float x = 0.5f + dx * t;
		float y = 0.5f + dy * t;

		// do it!
		float sizeFactor;
		if (!element[i].SizeAble())
			sizeFactor = 1.0f;
		else
			sizeFactor = element[i].ClampSizeFactor(element[i].SizeFactor(sizeDot * sizeDot * sizeDot));

		// Compute alpha
		float alpha = haveHit ? 0.0f : (element[i].Alpha(sizeDot) * noiseMod);

		// Draw this bad boy
		element[i].SetPositionRotation(x, y, cosTheta, sinTheta, sizeFactor, alpha);
	}

	// Okay update this bad boy
	if (haveHit)
		HideOverBrightSprite();
	else
		ShowOverBrightSprite(dot, 1.0f - dotFactor);

	// save the last camera pos
	lastCam = camera.Eye();
	lastCamTime = g_timer.GetEndSec();
}

//***************************************************************************
// Init the elements from an elements array
//***************************************************************************
bool LensFlare::InitElements(
LENSELEMENTINIT *elementInit,
uint howManyElements)
{
	// duh
	if (howManyElements == 0 || elementInit == NULL)
		return(false);

	// Do we have a database
	if (hTexture == INVALID_HANDLE)
		return(false);

	// Setup the array
	elements = howManyElements;
	element = new LensFlareElement[elements];
	ASSERT(element);

	// Set the texture set
	SpriteMgr__SetTextureDbl(hTexture);
	for (uint i = 0; i < elements; i++)
	{
		LENSELEMENTINIT e;
		e = elementInit[i];
#ifdef PS2
		if (!e.overBright)
		{
			e.r /= 2;
			e.g /= 2;
			e.b /= 2;
			e.a /= 2;
		}
#else
		if (e.overBright)
		{
			e.r = max(255, e.r * 2);
			e.g = max(255, e.g * 2);
			e.b = max(255, e.b * 2);
			e.a = max(255, e.a * 2);
		}
#endif
		element[i].Setup(e, aspect);
	}

	// setup the overbright sprite
	SetupOverBrightSprite(0);

	return(true);
}


//***************************************************************************
//***************************************************************************
bool LensFlare::Enable(
bool newEnable)
{
	// Toggle enable
	if (enabled != newEnable)
	{
		for (uint i = 0; i < elements; i++)
		{
			if (newEnable)
				element[i].Show();
			else
			{
				element[i].Hide();
				element[i].Reset();
			}
		}
		enabled = newEnable;
	}

	if (!enabled)
		SpriteMgr__DisableSprite(overBrightSprite);

	return(enabled);
}

//***************************************************************************
//***************************************************************************
void LensFlare::SetupOverBrightSprite(
int textureIndex)
{
	ts_bRGBA	color[4];
	memset(color, '\0', sizeof(ts_bRGBA) * 4);

	Vector3 v[4];
	v[0].Set(0.0f, 0.0f, 0.0f);
	v[1].Set(1.0f, 0.0f, 0.0f);
	v[2].Set(1.0f, 1.0f, 0.0f);
	v[3].Set(0.0f, 1.0f, 0.0f);

	Vector2 center(0.5f, 0.5f);
	Vector2 halfPixel(0.5f / 32.0f, 0.5f / 32.0f);
	Vector2 uv[4];
	uv[0].Set(center.x() - halfPixel.x(), center.y() - halfPixel.y());
	uv[1].Set(center.x() + halfPixel.x(), center.y() - halfPixel.y());
	uv[2].Set(center.x() + halfPixel.x(), center.y() + halfPixel.y());
	uv[3].Set(center.x() - halfPixel.x(), center.y() + halfPixel.y());

	// Register us!
	overBrightSprite = SpriteMgr__RegisterSprite(textureIndex, 70, SPRITE_DEFAULT_SORT,
																SPRITE_ALL_ONE_COLOR | SHADE_SPRITE,
						  										v, uv, color);
	SpriteMgr__DisableSprite(overBrightSprite);
}

//***************************************************************************
//***************************************************************************
void LensFlare::ShowOverBrightSprite(
float dot,
float dotFactor)
{
	// Show it?
	if (dot < 0.0f || dotFactor < blindCone)
	{
		SpriteMgr__DisableSprite(overBrightSprite);
		return;
	}

	// Figure out our color
	ts_bRGBA	color[4];
#ifdef PS2
	color[0].R = 127;
	color[0].G = 127;
	color[0].B = 127;
#else
	color[0].R = 255;
	color[0].G = 255;
	color[0].B = 255;
#endif
	float ratio = (dotFactor - blindCone) / (1.0f - blindCone);
	uint alpha = (uint)(ratio * 255.0f);
	color[0].A = min(255, alpha);

	color[1] = color[0]; color[2] = color[0]; color[3] = color[0];

	// Cause an update!
	SpriteMgr__EnableSprite(overBrightSprite);
	SpriteMgr__UpdateSprite(overBrightSprite, 70, SPRITE_DEFAULT_SORT,
									SPRITE_ALL_ONE_COLOR | SHADE_SPRITE,
									0, 0, color);
}

//***************************************************************************
//***************************************************************************
void LensFlare::HideOverBrightSprite(void)
{
	SpriteMgr__DisableSprite(overBrightSprite);
}
