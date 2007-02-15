//***************************************************************************
// Halo's
//***************************************************************************
#include "Effects/EffectsPCH.h"
#include "Math/sfilter.h"

#define HALO_SORT 	(SPRITE_DEFAULT_SORT)
#define HALO_FLAGS   (SPRITE_ALL_ONE_COLOR | SPRITE_BLENDMODE_ENABLED)
#define HALO_BLEND	(SPRITEBLEND_ADD)

Vector2 HaloElement::baseUV[4] =
{
	Vector2(0.0f, 0.0f),
	Vector2(1.0f, 0.0f),
	Vector2(1.0f, 1.0f),
	Vector2(0.0f, 1.0f)
};
float HaloElement::fadeInT = 0.08f;
float HaloElement::fadeOutT = 0.04f;

//***************************************************************************
//***************************************************************************
HaloElement::HaloElement()
{
	myHandle = INVALID_HANDLE;
	isParticle = false;

	for (int i = 0; i < 4; i++)
	{
		v[i].Clear();
		uv[i] = baseUV[i];
	}
	haveAngle = false;

	pos.Clear();
	angle = 0.0f;
	scale = 1.0f;

	color.R = color.G = color.B = color.A = 255;

	currentAlpha = 0.0f;
	targetAlpha = 0.0f;

	state = HIDDEN;
}

//***************************************************************************
//***************************************************************************
HaloElement::~HaloElement()
{
	Destroy();
}

//***************************************************************************
// Set some filter constants
//***************************************************************************
void HaloElement::SetFilterConstants(
float fadeIn,
float fadeOut)
{
	fadeInT = fadeIn;
	fadeOutT = fadeOut;
}

//***************************************************************************
// Setup the sprite
//***************************************************************************
void HaloElement::Setup(
int textureIndex,
ts_bRGBA &baseColor,
const Vector3 *posInWorld)
{
	isParticle = (posInWorld != NULL);
	SetColor(baseColor);

	if (!isParticle)
	{
		// Register us!
		myHandle = SpriteMgr__RegisterSprite( textureIndex, HALO_GROUP, HALO_SORT, HALO_FLAGS, v, uv, &color);

		if( myHandle != INVALID_HANDLE )
			SpriteMgr__SetBlendMode( myHandle, HALO_BLEND );

		state = VISIBLE;
		SetState(HIDDEN);
	}
	else
	{
		pos = *posInWorld;
		state = HIDDEN;
		SetState(VISIBLE);
	}
}

//***************************************************************************
//***************************************************************************
void HaloElement::Destroy()
{
	if (myHandle != INVALID_HANDLE)
	{
		if (isParticle)
			Particle__Kill(myHandle, false);
		else
			SpriteMgr__KillSprite(myHandle);

		myHandle = INVALID_HANDLE;
	}
}

//***************************************************************************
//***************************************************************************
void HaloElement::SetState(
State newState)
{
	if (state == newState)
		return;

	switch (newState)
	{
		case HIDDEN:
			currentAlpha = targetAlpha = 0.0f;
			filterT = 0.0f;
			Hide();
			break;
		case FADEIN:
			targetAlpha = 1.0f;
			filterT = fadeInT;
			if (state != FADEOUT)
				currentAlpha = 0.0f;
			if (state == HIDDEN)
				Show();
			break;
		case FADEOUT:
			targetAlpha = 0.0f;
			filterT = fadeInT;
			if (state != FADEIN)
				currentAlpha = 1.0f;
			if (state == HIDDEN)
				Show();
			break;
		case VISIBLE:
			currentAlpha = targetAlpha = 1.0f;
			filterT = 0.0f;
			Show();
			break;
	}

	// new state!
	state = newState;
}

//***************************************************************************
//***************************************************************************
void HaloElement::SetStateSmart(
State newState)
{
	if (newState == FADEOUT && state == HIDDEN)
		SetState(HIDDEN);
	else if (newState == FADEIN && state == VISIBLE)
		SetState(VISIBLE);
	else
		SetState(newState);
}

//***************************************************************************
// Update
//***************************************************************************
void HaloElement::SetPosition(
float x,
float y,
float size,
float aspect)
{
	// Update the position
	if (isParticle)
		scale = size;
	else
	{
		Vector2 halfSize(size * 0.5f, size * 0.5f * aspect);
		v[0].Set(x - halfSize.x(), y - halfSize.y(), 0.0f);
		v[1].Set(x + halfSize.x(), y - halfSize.y(), 0.0f);
		v[2].Set(x + halfSize.x(), y + halfSize.y(), 0.0f);
		v[3].Set(x - halfSize.x(), y + halfSize.y(), 0.0f);
	}
}

//***************************************************************************
// Update
//***************************************************************************
void HaloElement::SetRotation(
float rotation)
{
	if (isParticle)
		angle = rotation;
	else
	{
 		// Do the rotation?
		if (Math::Zero(angle))
			haveAngle = false;
		else
		{
			float cosTheta, sinTheta;
			Math::SinCos(angle, sinTheta, cosTheta);
			float a = (sinTheta * 0.5f - cosTheta * 0.5f);
			float b = -1.0f * (sinTheta * 0.5f + cosTheta * 0.5f);

			for (uint i = 0; i < 4; i++)
			{
				uv[i].Set(cosTheta * baseUV[i].x() - sinTheta * baseUV[i].y() + a + 0.5f,
							 sinTheta * baseUV[i].x() + cosTheta * baseUV[i].y() + b + 0.5f);
			}
			haveAngle = true;
		}
	}
}

//***************************************************************************
// Update
//***************************************************************************
void HaloElement::SetColor(
const ts_bRGBA &newColor)
{
	color = newColor;
}

//***************************************************************************
// Call this once per frame regardless
//***************************************************************************
void HaloElement::Update(
const Vector3 &eye,
float deltaT)
{
	switch (state)
	{
		case HIDDEN:
			return;

		case VISIBLE:
			break;

		case FADEIN:
			// Compute the alpha
			currentAlpha = FOFilterImplicit(currentAlpha, targetAlpha, deltaT, filterT);
			break;

		case FADEOUT:
			currentAlpha = FOFilterImplicit(currentAlpha, targetAlpha, deltaT, filterT);
			if (currentAlpha < HALO_NOSHOWTHRESHOLD)
				SetState(HIDDEN);
			break;
	}

	// Shouldn't have to do this
	ts_bRGBA c[4];
	c[0] = color;
	c[0].A = (unsigned char)Math::Clamp((float)color.A * currentAlpha, 0.0f, 255.0f);

	// Cause an update!
	if (!isParticle)
	{
		// this is a sprite
		c[1] = c[0];
		c[2] = c[0];
		c[3] = c[0];

		SpriteMgr__UpdateSprite(myHandle, HALO_GROUP, HALO_SORT, HALO_FLAGS, v, haveAngle ? uv : NULL, c);
		SpriteMgr__SetBlendMode(myHandle, HALO_BLEND );
	}
	else if (myHandle != INVALID_HANDLE)
	{
		// This is a particle
		Particle__SetColor(myHandle, c[0]);
		Particle__SetScale(myHandle, scale);

		// Figure out the camera based stuff
		float d = (eye - pos).Length();
		Vector3 normal;
		if (!Math::Zero(d))
			normal = (pos - eye) / d;
		else
			normal.Set(0.0f, 0.0f, 1.0f); // today I like Z
#if 0
		Particle__SetNormal(myHandle, &normal);
#endif

		Vector3 rotation(0.0f, 0.0f, angle);
		Particle__SetRotation(myHandle, &rotation);

		static float distanceTweak = -0.0025f;
		const float offsetHalo = d * distanceTweak;
		Vector3 tweak = pos + normal * offsetHalo;
		Particle__SetPosition(myHandle, &tweak);
	}
}

//***************************************************************************
// Show and Hide
//***************************************************************************
void HaloElement::Show()
{
	if (!isParticle)
		SpriteMgr__EnableSprite(myHandle);
	else if (myHandle == INVALID_HANDLE)
	{
		char particleName[64];
		myHandle = ParticleMgr__CreateOwnedParticle(ParticleName(particleName),	//	const char *pEffectName,
	 			  													&pos,				//	const Vector3 *pvpos,
 																	NULL,				//	const Vector3 *pvVelUnitVector,
				 													NULL,				//	const float *pfVelMagnitude,
 																	NULL,				//	const Vector3 *pvAdditiveVel,
 																	NULL,				//	const t_ParticleDeathCallback DeathCallback,
 																	this,				//	void *context,
 																	scale); 			//	const float fScale = 1.0f);
	}
}

//***************************************************************************
//***************************************************************************
void HaloElement::Hide()
{
	if (!isParticle)
		SpriteMgr__DisableSprite(myHandle);
	else if (myHandle != INVALID_HANDLE)
	{
		Particle__Kill(myHandle, false);
		myHandle = INVALID_HANDLE;
	}
}

//***************************************************************************
// Particle name from texture name
//***************************************************************************
const char *HaloElement::ParticleName(
char *buffer)
{
#if 1
	sprintf(buffer, "halo%d", textureIndex);
#else
	strcpy(buffer, "BulletHole");
#endif
	return buffer;
}

