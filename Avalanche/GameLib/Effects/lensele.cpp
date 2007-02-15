//***************************************************************************
// The lens flare!  Cheesy and simple.
//***************************************************************************
#include "Effects/EffectsPCH.h"
#include "EngineHelper/timer.h"

#define LENS_SORT 				SPRITE_DEFAULT_SORT
#define LENS_FLAGS				(VERTEX_BY_REFERENCE | UV_BY_REFERENCE | SPRITE_ALL_ONE_COLOR | SHADE_SPRITE)
#define MAX_ALPHACHANGEPERSEC	7.0f

Vector2 LensFlareElement::baseUV[4] =
{
	Vector2( 0.0f, 0.0f ),
	Vector2( 1.0f, 0.0f ),
	Vector2( 1.0f, 1.0f ),
	Vector2( 0.0f, 1.0f )
};

//***************************************************************************
//***************************************************************************
LensFlareElement::LensFlareElement(void)
{
	myHandle = INVALID_HANDLE;
	for (uint i = 0; i < 4; i++)
		uv[i] = baseUV[i];
}

//***************************************************************************
//***************************************************************************
LensFlareElement::~LensFlareElement(void)
{
	// Anything to clean up?
}

//***************************************************************************
// Setup the sprite
//***************************************************************************
void LensFlareElement::Setup(
LENSELEMENTINIT &init,
float aspect)
{
	texture = init.textureIndex;

	color[0].R = init.r;
	color[0].G = init.g;
	color[0].B = init.b;
	color[0].A = init.a;
	color[1] = color[0];
	color[2] = color[0];
	color[3] = color[0];

	halfSize.Set( init.size * 0.5f,
					  init.size * 0.5f * aspect);
	t = init.linePosition;
	rotatable = init.rotatable;

	minSizeFactor = init.minSizeFactor;
	maxSizeFactor = init.maxSizeFactor;
	sizeAble = (minSizeFactor != maxSizeFactor);

	minAlpha = init.minAlpha;
	maxAlpha = init.maxAlpha;
	baseAlpha = (float)init.a / 255.0f;
	currentAlpha = 0.0f;
	alphaThreshold = init.alphaThreshold;

	// Register us!
	myHandle = SpriteMgr__RegisterSprite(texture, LENS_GROUP, LENS_SORT, LENS_FLAGS,
				  										v, uv, color);
}

//***************************************************************************
// Update
//***************************************************************************
void LensFlareElement::SetPositionRotation(
float x,
float y,
float cosTheta,
float sinTheta,
float sizeFactor,
float alpha)
{
	// Update the position
	Vector2 size = halfSize * sizeFactor;
	v[0].Set(x - size.x(), y - size.y(), 0.0f);
	v[1].Set(x + size.x(), y - size.y(), 0.0f);
	v[2].Set(x + size.x(), y + size.y(), 0.0f);
	v[3].Set(x - size.x(), y + size.y(), 0.0f);

	// Do the rotation?
	if (rotatable)
	{
		float a = (sinTheta * 0.5f - cosTheta * 0.5f);
		float b = -1.0f * (sinTheta * 0.5f + cosTheta * 0.5f);

		for (uint i = 0; i < 4; i++)
		{
			uv[i].Set(cosTheta * baseUV[i].x() - sinTheta * baseUV[i].y() + a + 0.5f,
						 sinTheta * baseUV[i].x() + cosTheta * baseUV[i].y() + b + 0.5f);
		}
	}

	// Draw this sprite?
	if (alpha < alphaThreshold)
		alpha = 0.0f;

	// Compute alpha
	if (currentAlpha != alpha)
	{
		// Interpolate alpha
		float deltaAlpha = alpha - currentAlpha;
		float deltaT = g_timer.GetOSFrameSec();
		float delta = deltaAlpha / deltaT;

		if (delta > MAX_ALPHACHANGEPERSEC)
			deltaAlpha = MAX_ALPHACHANGEPERSEC * deltaT;
		else if (delta < -MAX_ALPHACHANGEPERSEC)
			deltaAlpha = -MAX_ALPHACHANGEPERSEC * deltaT;
		currentAlpha += deltaAlpha;

		int a = (int)(currentAlpha * 255.0f);
		color[0].A = color[1].A = color[2].A = color[3].A = a;
	}

	// Cause an update!
	SpriteMgr__UpdateSprite(myHandle, LENS_GROUP, LENS_SORT, LENS_FLAGS,	0, 0, color);
}

//***************************************************************************
// Reset the alpha settings
//***************************************************************************
void LensFlareElement::Reset(void)
{
	currentAlpha = 0.0f;
	color[0].A = color[1].A = color[2].A = color[3].A = 0;
	SpriteMgr__UpdateSprite(myHandle, LENS_GROUP, LENS_SORT, LENS_FLAGS,	0, 0, color);
}







































































































































































































































































































