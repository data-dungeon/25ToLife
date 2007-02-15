#ifndef HALOELE_H
#define HALOELE_H

#include "Game/Managers/SpriteMgr.h"
#include "Math/Vector.h"

// Our sprite group
#define HALO_GROUP	69

#define HALO_NOSHOWTHRESHOLD	0.01f

// Here is a halo element
class HaloElement
{
public:
	HaloElement();
	~HaloElement();

	// Init! (giving a posinworld makes this a non-screenspace effect)
	void Setup(int textureIndex, ts_bRGBA &baseColor, const Vector3 *posInWorld = NULL);

	// Set some filter constants
	static void SetFilterConstants(float fadeIn, float fadeOut);

	// Destroy!
	void Destroy();

	// We have distinct states now
	enum State
	{
		HIDDEN,
		FADEIN,
		FADEOUT,
		VISIBLE
	};
	State GetState()
		{ return state; }
	void SetState(State newState);
	void SetStateSmart(State newState);

	// Update!
	void SetPosition(float x, float y, float size, float aspect);
	void SetRotation(float angle);
	void SetColor(const ts_bRGBA &newColor);

	// Call this once per frame regardless
	void Update(const Vector3 &eye, float deltaT);

private:
	// Stuff for the spritemgr or particle manager
	t_Handle			myHandle;
	bool				isParticle;

	// Sprite information
	Vector3 			v[4];
	Vector2 			uv[4];
	bool				haveAngle;

	// Particle info
	Vector3Padded	pos;
	float				angle;
	float				scale;

	// Common information
	int				textureIndex;
	ts_bRGBA 		color;

	// our current state
	State				state;
	float				filterT;
	float				targetAlpha, currentAlpha;

	static Vector2	baseUV[4];
	static float	fadeInT, fadeOutT;

	// Show and Hide
	void Show();
	void Hide();

	// Particle name from texture name
	const char *ParticleName(char *buffer);
};


#endif
