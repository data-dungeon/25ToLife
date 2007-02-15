#ifndef LENSELE_H
#define LENSELE_H

#include "Game/Managers/SpriteMgr.h"
#include "Math/Matrix.h"

// Our sprite group
#define LENS_GROUP	68

// Here is how we init a lens flare element
struct LENSELEMENTINIT
{
	int	textureIndex;
	float linePosition;
	float	size;
	u8		r, g, b, a;
	bool	rotatable;
	bool	overBright;
	float	minSizeFactor;
	float	maxSizeFactor;
	float	minAlpha;
	float maxAlpha;
	float alphaThreshold;
};

// Here is the actual element
class LensFlareElement
{
public:
	LensFlareElement(void);
	~LensFlareElement(void);

	// Setup the sprite
	void Setup(LENSELEMENTINIT &init, float aspect);

	// Update
	void SetPositionRotation(float x, float y, 
										float cosTheta, float sinTheta, 
										float sizeFactor = 1.0f, float alpha = 1.0f);
	
	// Hide and show us
	inline void Show()
		{ SpriteMgr__EnableSprite(myHandle); }
	inline void Hide()
		{ SpriteMgr__DisableSprite(myHandle); }

	// Reset us (for when we go off screen)
	void Reset(void);

	// What is our position along the flare line?
	inline float LinePosition(void)
		{ return(t); }

	// What is our size stuff
	inline bool SizeAble(void)
		{ return(sizeAble); }
	inline float MinSizeFactor(void)
		{ return(minSizeFactor); }
	inline float MaxSizeFactor(void)
		{ return(maxSizeFactor); }
	inline float SizeFactor(float t)
		{ return(minSizeFactor + (maxSizeFactor - minSizeFactor) * t); }
	inline float ClampSizeFactor(float sizeFactor)
		{ return(Clamp(sizeFactor, minSizeFactor, maxSizeFactor)); }

	// Alpha
	inline float Alpha(float t)
		{ return(Clamp(minAlpha + (maxAlpha - minAlpha) * t, minAlpha, maxAlpha)); }
	
private:
	// setup stuff
	int				texture;
	Vector2			halfSize;
	float				t;
	bool				rotatable;
		
	// Sizing stuff
	bool				sizeAble;
	float				minSizeFactor;
	float				maxSizeFactor;
		
	// Our alpha
	float				baseAlpha;
	float				minAlpha;
	float				maxAlpha;
	float				currentAlpha;
	float				alphaThreshold;

	// Stuff for the spritemgr
	t_Handle			myHandle;
	Vector3			v[4];
	Vector2			uv[4];
	ts_bRGBA			color[4];

	static Vector2	baseUV[4];

	static inline float Clamp(float t, float min, float max)
		{ if (t < min) return(min); else if (t > max) return(max); else return(t); }
};

#endif
