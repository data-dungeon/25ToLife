///////////////////////////////////////////////////////////////////////////////
// Load screen!
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Display/Common/DisplayState.h"
#include "Display/Common/GfxFunc.h"
#include "Game/Managers/RenderMgr.h"
#include "main/lstag.h"

int LoadScreenTag::s_useWallDBL = 0;

#define LOADSCREEN_WALL_DBLS 	5
#define LOADSCREEN_TAG_DBLS 	7

#define TIZAG_XPOINTS	12
#define TIZAG_YPOINTS	8
#define SPRAY_TIME_MIN 	1.0f
#define SPRAY_TIME_MAX 	6.0f
#define LOGO_SPRAY_TIME	1.0f
#define SPRAY_ALPHA_MAX	1.0f
#define SPRAY_ALPHA_MIN	0.3f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoadScreenModule *LoadScreenTag::Create()
{
	return new LoadScreenTag;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoadScreenTag::LoadScreenTag()
{
	if (s_useWallDBL == 0)
		s_useWallDBL = g_random.InRange(1, LOADSCREEN_WALL_DBLS);

	m_aspectMultiply = 1.0f;
	m_hdtv = false;
#if defined (_XBOX)
	if ((DirectX_PresentParams.Flags & D3DPRESENTFLAG_WIDESCREEN) != 0)
	{
		m_aspectMultiply = 0.75f;
		if (DirectX_PresentParams.BackBufferWidth > 640)
			m_hdtv = true;
	}
#endif

	// Setup the wall/logo first
	char dblName[64];
	sprintf(dblName, "tag/lswall%d.dbl", s_useWallDBL);
	if (++s_useWallDBL > LOADSCREEN_WALL_DBLS)
		s_useWallDBL = 1;

	m_wallDecalSet = new DecalSet(dblName);
	ASSERT(m_wallDecalSet && m_wallDecalSet->Okay());

	m_wall = NULL;
	m_logo = NULL;
	m_dvd = NULL;
	if (m_wallDecalSet && m_wallDecalSet->Okay())
	{
		DecalTextureLayer wall(0);
		wall.m_clampU = wall.m_clampV = false;
		m_wall = new SimpleQuadDecalEmitter(*m_wallDecalSet, &wall, 1);
		ASSERT(m_wall);
		if (m_wall)
			m_wall->Setup(1, -1);

	  	m_logo = new Tizag(*m_wallDecalSet, 1, TIZAG_XPOINTS >> 1, TIZAG_YPOINTS >> 1, false);
		ASSERT(m_logo);

	  	m_dvd = new Tizag(*m_wallDecalSet, 2, 2, 2, false);
		ASSERT(m_dvd);
	}

	// Now do the tags
	sprintf(dblName, "tag/lstag%d.dbl", g_random.InRange(1, LOADSCREEN_TAG_DBLS));

	m_tagDecalSet = new DecalSet(dblName);
	ASSERT(m_tagDecalSet && m_tagDecalSet->Okay());
	m_tagTextures = m_tagDecalSet->Textures();
	for (int t = 0; t < LOADSCREEN_TAGS; t++)
	{
		if (!m_tagDecalSet || !m_tagDecalSet->Okay())
			m_tizag[t] = NULL;
		else
		{
			m_tizag[t] = new Tizag(*m_tagDecalSet, t % m_tagTextures, TIZAG_XPOINTS, TIZAG_YPOINTS, false);
			ASSERT(m_tizag[t]);
		}
	}

	m_lowestTag = m_currentTag = 0;

	// Init the regions
	InitRegions();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoadScreenTag::~LoadScreenTag()
{
	for (int t = 0; t < LOADSCREEN_TAGS; t++)
	{
		delete m_tizag[t];
		m_tizag[t] = NULL;
	}

	delete m_dvd;
	m_dvd = NULL;

	delete m_logo;
	m_logo = NULL;

	delete m_wall;
	m_wall = NULL;

	if (m_wallDecalSet)
	{
		m_wallDecalSet->Destroy(true);
		delete m_wallDecalSet;
	}

	if (m_tagDecalSet)
	{
		m_tagDecalSet->Destroy(true);
		delete m_tagDecalSet;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenTag::Startup()
{
	MatrixStack__Mode(MS_PROJECTION);
	MatrixStack__Push();
	MatrixStack__Identity();
	MatrixStack__Orthographic(-4.05f, 4.05f, -3.05f, 3.05f, -1.0f, 1.0f);

	g4WorldToCamera.Identity();

	// Setup the wall
	const Vector3 origin(0.0f, 0.0f, 0.0f);
	const Vector3 right(1.0f, 0.0f, 0.0f);
	const Vector3 up(0.0f, 1.0f, 0.0f);

	ts_bRGBA color[4];
	const u8 bright = 220;
	const u8 dark = 96;
	color[0].R = bright; color[0].G = bright; color[0].B = bright; color[0].A = 255;
	color[1].R = bright; color[1].G = bright; color[1].B = bright; color[1].A = 255;
	color[2].R = dark; color[2].G = dark; color[2].B = dark; color[2].A = 255;
	color[3].R = dark; color[3].G = dark; color[3].B = dark; color[3].A = 255;

	Vector2 uv[4];
	const float uWrap = 2.0f;
	const float vWrap = 1.5f;
	uv[0].Set(0.0f, 0.0f);
	uv[1].Set(uWrap, 0.0f);
	uv[2].Set(0.0f, vWrap);
	uv[3].Set(uWrap, vWrap);

	m_wall->Create(origin, right, up, 4.0f, 3.0f, color, uv);

	// Setup the logo
	if (m_logo)
	{
		const Vector3 normal(0.0f, 0.0f, 1.0f);
		const Vector3 up(0.0f, 1.0f, 0.0f);
		Vector3 pos;
		if (!m_hdtv)
			pos.Set(1.5f, 1.8f, 0.01f);
		else
			pos.Set(1.4f, 1.6f, 0.01f);


		// Create a new one!
		m_logo->Setup(pos, normal, up, 4.0f * m_aspectMultiply, 2.0f, LOGO_SPRAY_TIME, 0.75f);
	}

	if (m_dvd)
	{
		const Vector3 normal(0.0f, 0.0f, 1.0f);
		const Vector3 up(0.0f, 1.0f, 0.0f);
		const Vector3 pos(0.0f, 0.0f, 0.02f);

		m_dvd->Setup(pos, normal, up, 0.5f * m_aspectMultiply, 0.5f, 0.0f, 1.0f);
	}

	// When do we make the next tag?
	g_timer.SetSpeed(CMasterTimer::ONE_X);
	g_random.Seed(g_timer.GetOSTime());

	m_lowestTag = m_currentTag = g_random.UpToExcludeHigh(LOADSCREEN_TAGS);
	ASSERT(m_currentTag >= 0 && m_currentTag < LOADSCREEN_TAGS);
	m_zHeight = 0.015f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenTag::Render()
{
	g_input.BeginInput();

	ts_bRGBA clearColor = { 0, 0, 0, 255 };
	gfxClearFrameBuffer(RS_COLOR_BUFFER_BIT | RS_DEPTH_BUFFER_BIT, &clearColor);

	Graphics4x4 localToCamera;
	localToCamera.Identity();

	// Make a new tag?
	g_timer.NewFrame();
	bool logoDone = !m_logo || m_logo->DoneFading();
	Tizag *tag = m_tizag[m_currentTag];
	if (logoDone && (tag && (!tag->Visible() || tag->DoneFading())))
		TagIt();

	if (tag && tag->Visible() && !tag->DoneFading())
	{
		float spray, alpha;
		GetTimeAlpha(spray, alpha);
		tag->ChangePaintTime(spray);
		tag->ChangeAlphaTarget(alpha);
	}

	// Render it!
	if (m_wall)
		m_wall->Render(NULL, &localToCamera, 0.5f);

	Draw(m_logo);

	for (int t = m_lowestTag; t < LOADSCREEN_TAGS; t++)
		Draw(m_tizag[t]);
	for (int t = 0; t < m_lowestTag; t++)
		Draw(m_tizag[t]);

	if (m_dvd)
	{
		CInstance *instance = m_dvd->GetInstance();
		ASSERT(instance);
		instance->Matrix().SetScale(0.5f, 0.5f, 1.0f);
		instance->Matrix().CatYRotation(g_timer.GetOSEndSec() * 3.0f);
		Vector3 pos;
		if (!m_hdtv)
			pos.Set(3.1f, -2.15f, 0.52f);
		else
			pos.Set(3.0f, -2.1f, 0.52f);

		instance->Matrix().PokeTranslation(pos);
		m_dvd->Render(instance, &instance->Matrix(), 0.5f);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenTag::TagIt()
{
	// randomly select a tag
	ASSERT(m_currentTag >= 0 && m_currentTag < LOADSCREEN_TAGS);

	++m_currentTag;
	if (m_currentTag == LOADSCREEN_TAGS)
		m_currentTag = 0;
	Tizag *tag = m_tizag[m_currentTag];

	if (m_currentTag == m_lowestTag)
	{
		if (++m_lowestTag == LOADSCREEN_TAGS)
			m_lowestTag = 0;
	}

	// Make the corners
	const Vector3 normal(0.0f, 0.0f, 1.0f);
	const Vector3 realUp(0.0f, 1.0f, 0.0f);
	const float maxRot = Math::Deg2Rad(30.0f);
	float rotation = g_random.UpTo(3) == 3 ? 0.0f : g_random.InRange(-maxRot, maxRot);
	Vector3 up = RotatePointAroundAxis(realUp, normal, rotation);

	const float width = g_random.InRange(1.5f, 2.5f);
	const float height = width * g_random.InRange(0.4f, 0.6f);
	Vector3 pos = FitToWall(width, height, rotation);

	// Create a new one!
	float spray, alpha;
	GetTimeAlpha(spray, alpha);
	tag->Setup(pos, normal, up, width * m_aspectMultiply, height, spray, alpha);

	// Sort correctly
	m_zHeight += 0.0005f;
}

///////////////////////////////////////////////////////////////////////////////
// Fit a point into a box
///////////////////////////////////////////////////////////////////////////////
Vector3 LoadScreenTag::FitToWall(
float width,
float height,
float rotate)
{
	// Rotate the points
	Vector3 p[4];
	p[0].Set(width * 0.5f, height * 0.5f, 0.0f);
	p[1].Set(width * 0.5f, height * -0.5f, 0.0f);
	p[2].Set(width * -0.5f, height * -0.5f, 0.0f);
	p[3].Set(width * -0.5f, height * 0.5f, 0.0f);
	float maxHalfWidth = 0.0f, maxHalfHeight = 0.0f;
	const Vector3 axis(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 4; i++)
	{
		const Vector3 r = RotatePointAroundAxis(p[i], axis, rotate);
		if (i == 0 || Math::Abs(r.X()) > maxHalfWidth)
			maxHalfWidth = Math::Abs(r.X());
		if (i == 0 || Math::Abs(r.Y()) > maxHalfHeight)
			maxHalfHeight = Math::Abs(r.Y());
	}

	// Find the best place to put it
	Vector2 center = FindBestRegion(maxHalfWidth, maxHalfHeight);
	return Vector3(center.X(), center.Y(), m_zHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 LoadScreenTag::FindBestRegion(
float halfWidth,
float halfHeight)
{
	Region *best = NULL;
	if (m_lastRegion)
	{
		// Find the farthest away one that will fit
		int x, loopXDone;
		x = loopXDone = g_random.UpToExcludeHigh(REGIONS_X);
		int incX = g_random.CoinToss() ? 1 : -1;

		int y, loopYDone;
		y = loopYDone = g_random.UpToExcludeHigh(REGIONS_Y);
		int incY = g_random.CoinToss() ? 1 : -1;

		do {
			do {
				if (!m_region[x][y].forbidden)
				{
					// What is the distance to this region
					int d = Math::Abs(m_lastRegion->x - x) + Math::Abs(m_lastRegion->y - y);
					if (d > 0 && d < 4)
					{
						if (!best || m_region[x][y].useTime < best->useTime)
						best = &m_region[x][y];
					}
				}

				y += incY;
				if (y < 0)
					y = REGIONS_Y - 1;
				else if (y == REGIONS_Y)
					y = 0;

			} while (y != loopYDone);

			x += incX;
			if (x < 0)
				x = REGIONS_X - 1;
			else if (x == REGIONS_X)
				x = 0;

		} while (x != loopXDone);
	}

	// Did we find one?
	if (!best)
	{
		// Pick something in the middle
		float halfX = (float)REGIONS_X * 0.5f;
		int x = (int)(g_random.InRange(halfX - 1.0f, halfX + 1.0f) + 0.5f);
		float halfY = (float)REGIONS_Y * 0.5f;
		int y = (int)(g_random.InRange(halfY - 1.0f, halfY + 1.0f) + 0.5f);
		best = &m_region[x][y];
	}

	// Track it
	m_lastRegion = best;
	m_lastRegion->useTime = ++m_useTime;

	// Make it fit
	return MakeFit(*m_lastRegion, halfWidth, halfHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 LoadScreenTag::MakeFit(
const Region &region,
float halfWidth,
float halfHeight)
{
	const float xDwell = 0.16f;
	const float yDwell = 0.12f;

	Vector2 center;
	center.X(region.center.X() + g_random.InRange(-xDwell, xDwell));
	center.Y(region.center.Y() + g_random.InRange(-xDwell, xDwell));

	const float minX = -4.0f;
	const float maxX = 4.0f;
	const float minY = -3.0f;
	const float maxY = 3.0f;

	if (center.X() - halfWidth < minX)
		center.X(minX + halfWidth);
	else if (center.X() + halfWidth > maxX)
		center.X(maxX - halfWidth);

	if (center.Y() - halfWidth < minY)
		center.Y(minY + halfWidth);
	else if (center.Y() + halfWidth > maxY)
		center.Y(maxY - halfWidth);

	return center;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenTag::InitRegions()
{
	const float xRegion = 8.0f / (float)REGIONS_X;
	const float yRegion = 6.0f / (float)REGIONS_Y;
	const float xOff = -4.0f + xRegion * 0.5f;
	const float yOff = -3.0f + yRegion * 0.5f;

	for (int x = 0; x < REGIONS_X; x++)
	{
		for (int y = 0; y < REGIONS_Y; y++)
		{
			m_region[x][y].x = x;
			m_region[x][y].y = y;
			m_region[x][y].center.Set(xOff + (float)x * xRegion, yOff + (float)(REGIONS_Y - 1 - y) * yRegion);
			m_region[x][y].useTime = 0;
			m_region[x][y].forbidden = false;
		}
	}

	// Don't tag on logo
	m_region[2][0].forbidden = true;
	m_region[3][0].forbidden = true;
	m_region[4][0].forbidden = true;

	m_lastRegion = NULL;
	m_useTime = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenTag::Draw(
Tizag *tizag)
{
	if (tizag)
	{
		CInstance *instance = tizag->GetInstance();
		if (instance)
			tizag->Render(instance, &instance->Matrix(), 0.5f);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenTag::GetTimeAlpha(
float &time,
float &alpha)
{
	float sprayAxis = 0.0f;
	float alphaAxis = 0.0f;

	ControllerInputDriver *controller = NULL;
	if (g_input.ControllerLocked(0))
	{
 		controller = g_input.GetController(0);
		if (controller)
		{
			sprayAxis = controller->StickPosition(AA_X2);
			alphaAxis = controller->StickPosition(AA_Y1);
		}
	}
	else
	{
		int controllers = g_input.Controllers();
		for (int c = 0; c < controllers; c++)
		{
			controller = g_input.GetUnlockedController(c);
			if (controller)
			{
				sprayAxis += controller->StickPosition(AA_X2);
				alphaAxis += controller->StickPosition(AA_Y1);
			}
		}
	}

	time = Math::LinearMap(sprayAxis, -1.0f, 1.0f, SPRAY_TIME_MIN, SPRAY_TIME_MAX);
	alpha = Math::LinearMap(alphaAxis, -1.0f, 1.0f, SPRAY_ALPHA_MIN, SPRAY_ALPHA_MAX);
}
