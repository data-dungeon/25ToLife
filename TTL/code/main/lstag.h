///////////////////////////////////////////////////////////////////////////////
// A custom loading screen that uses Tags!!!!
///////////////////////////////////////////////////////////////////////////////
#ifndef LSTAG_H
#define LSTAG_H

#include "enginehelper/loadscreen.h"
#include "effects/decal/decalset.h"
#include "effects/decal/quaddecal.h"
#include "effects/decal/fadedecal.h"
#include "effects/tizag.h"

#define LOADSCREEN_TAGS					32

#define REGIONS_X							5
#define REGIONS_Y                   5

class LoadScreenTag : public LoadScreenModule
{
public:
	// All roations are in degrees
	// The creator (pass this into the LoadScreen setup call
	static LoadScreenModule *Create();

	// o way to handle errors here
	LoadScreenTag();
	virtual ~LoadScreenTag();

	// Do you want the pretty loading screen text?
	bool WantLoadingText()
		{ return false; }

	// We write these
	void Startup();
	void Render();

private:
	DecalSet						*m_wallDecalSet;
	DecalSet						*m_tagDecalSet;
	int							m_tagTextures;
	SimpleQuadDecalEmitter	*m_wall;
	Tizag							*m_logo;
	Tizag							*m_dvd;
	Tizag							*m_tizag[LOADSCREEN_TAGS];

	int							m_lowestTag, m_currentTag;
	float							m_zHeight;

	// Aspect ratio stuff
	float							m_aspectMultiply;
	bool							m_hdtv;

	// Do a tag
	void TagIt();

	// Fit a point into a box
	struct Region
	{
		int				x, y;
		Vector2			center;
		int 				useTime;
		bool				forbidden;
	};
	Region 	m_region[REGIONS_X][REGIONS_Y];
	int		m_useTime;
	Region	*m_lastRegion;

	Vector3 FitToWall(float width, float height, float rotate);
	Vector2 FindBestRegion(float maxWidth, float maxHeight);
	Vector2 MakeFit(const Region &region, float maxWidth, float maxHeight);
	void InitRegions();

	void Draw(Tizag *tizag);

	void GetTimeAlpha(float &time, float &alpha);

	static int s_useWallDBL;
};

#endif
