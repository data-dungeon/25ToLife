///////////////////////////////////////////////////////////////////////////////
// Sniper scope instrument
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/ttlweapinv.h"
#include "weapons/ttlweapon.h"
#include "hud/iscope.h"

#define HATCH_ALPHA		0.5f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ScopeInstrument::ScopeInstrument() :
SpriteInstrument("scope")
{
	// We want to be behind all other intstruments
	m_sprite.SetPriority(0);

	// Load our shit
	t_Handle handle = dblLoad("newhud/scope.dbl");
	ASSERT(handle != INVALID_HANDLE);

	const Vector2 offset[4] =
	{
		Vector2(-160.0f, -120.0f),
		Vector2(160.0f, -120.0f),
		Vector2(160.0f, 120.0f),
		Vector2(-160.0f, 120.0f)
	};
	Vector2 uv[4][4] =
	{
		{ Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f) },
		{ Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f) },
		{ Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f) },
		{ Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 0.0f) }
	};

	for (int m = 0; m < 4; m++)
	{
		m_overlay[m].SetTextureSet(handle, 0);
		m_overlay[m].Reset();
		m_overlay[m].SetSize(320.0f, 240.0f);
		m_overlay[m].SetOrigin(0.5f, 0.5f);

		for (int t = 0; t < 4; t++)
			m_overlay[m].SetTextureCoordinates(t, uv[m][t].X(), uv[m][t].Y());

		m_sprite.AddChild(&m_overlay[m]);
		m_overlay[m].SetPosition(offset[m].X(), offset[m].Y());
 		m_overlay[m].SetPriority(10);
	}

	for (int h = 0; h < 10; h++)
	{
		m_hHatch[h].SetTextureSet(handle, 1);
		m_hHatch[h].Reset();
		m_hHatch[h].SetOrigin(0.0f, 0.5f);
		m_sprite.AddChild(&m_hHatch[h]);
		m_hHatch[h].SetPosition((float)h * 64.0f - 320.0f, 0.0f);
 		m_hHatch[h].SetPriority(5);
 		m_hHatch[h].SetAlpha(HATCH_ALPHA);
	}

	for (int v = 0; v < 8; v++)
	{
		m_vHatch[v].SetTextureSet(handle, 1);
		m_vHatch[v].Reset();
		m_vHatch[v].SetOrigin(0.5f, 0.5f);
		m_vHatch[v].SetRotation(90.0f);
		m_sprite.AddChild(&m_vHatch[v]);
		m_vHatch[v].SetPosition(0.0f, (float)v * 64.0f - 160.0f);
 		m_vHatch[v].SetPriority(5);
 		m_vHatch[v].SetAlpha(HATCH_ALPHA);
	}

	// The text
	m_sprite.AddChild(&m_range);
	m_range.SetPosition(160.0f, 0.0f);
 	m_range.SetPriority(15);

	// Default to off
	m_on = false;
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void ScopeInstrument::Update(float dt)
{
	// Anything to do?
	if (!m_hidden && m_active)
	{
		// Are we on?
		CCompActor *who = g_hud.GetSubject();
		if (Dead(who))
			m_on = false;

		if (!m_on)
			m_sprite.HideAll();
		else
		{
			m_sprite.ShowAll();

			// Who are we looking at
			float range = Range(who);
			if (range <= 0.0f)
				m_range.SetText(NULL);
			else
			{
				char rangeStr[64];
				sprintf(rangeStr, "%.1fm", range);
				m_range.SetText(rangeStr);
			}
		}
	}

	// Do the rest of the wrok
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float ScopeInstrument::Range(CCompActor *who)
{
	// Duh?
	if (!who)
		return -1.0f;

	// Get weapon information
	TTLWeaponInventory *inventory = (TTLWeaponInventory *)who->GetComponentByName("WeaponInventory");
	if (!inventory)
		return -1.0f;

	TTLWeapon *weapon = (TTLWeapon *)inventory->GetSelected();
 	AimingSimulation &aimSim = weapon->GetAimSim();

	return aimSim.TargetRange();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScopeInstrument::Dead(
CCompActor *who)
{
	// Who's on first
	if (!who)
		return true;

	// Get the health interface
	HealthIF *healthIF = (HealthIF *)who->GetInterface(HealthIF::GetID());
	if (!healthIF)
		return true;
	else
		return !healthIF->IsAlive();
}

