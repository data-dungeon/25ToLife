///////////////////////////////////////////////////////////////////////////////
// Lobbed instrument!
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/ttlweapinv.h"
#include "weapons/lobbed.h"
#include "hud/ilobbed.h"

#define LOBBED_WIDTH			32.0f
#define LOBBED_HEIGHT		32.0f
#define LOBBED_SPACING		16.0f
#define LOBBED_TEXTMARGIN	20.0f
#define TEXT_ALPHA			0.6f
#define WEAPON_ALPHA			0.6f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbedInstrument::LobbedInstrument() :
SpriteInstrument("lobbed")
{
	m_lastLobbed = -1;

	for (int l = 0; l < HUD_SHOW_LOBBED; l++)
	{
		m_lobbed.SetTexture(l, 0);
		m_sprite.AddChild(&m_lobbed.GetSprite(l));
	}
	m_sprite.AddChild(&m_text);

	m_text.SetAlpha(TEXT_ALPHA);

	m_throwStrength.SetTextureSet(dblLoad("newhud/throw.dbl"), 0);
	m_throwStrength.Reset();
	m_throwStrength.SetOrigin(0.5f, 1.0f);
	m_throwStrength.SetAlpha(0.4f);
	m_throwStrength.SetScale(2.0f, 1.0f );
	m_sprite.AddChild(&m_throwStrength);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 LobbedInstrument::Extents()
{
	Vector2 extents(0.0f, LOBBED_HEIGHT);
	return SpriteInstrument::Extents();
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void LobbedInstrument::Update(float dt)
{
	// Anything to do?
	if (!m_hidden && m_active)
	{
		// Who are we looking at
		CCompActor *who = g_hud.GetSubject();
		Lobbed *lobbed = GetLobbed(who);

		// Has the wepaon changed?
		HandleWeaponChange(lobbed);

		if (!lobbed || !lobbed->HasAmmo())
			m_sprite.HideAll();
		else
		{
			m_sprite.ShowAll();

			// Do this!
			m_text.SetText(lobbed->DisplayName());
			float width = m_text.ComputeTextLength();
			m_text.SetPosition(-width, -LOBBED_HEIGHT);

			// Sprite is ready?
			if (m_lobbed.Process() == SSS_INITIALIZE)
				Setup(width);

			// Show the right stuff here
			unsigned int l, show = Math::Min(HUD_SHOW_LOBBED, lobbed->Ammo());
			for (l = 0; l < show; l++)
				m_lobbed.GetSprite(l).Show();
			for (l = show; l < HUD_SHOW_LOBBED; l++)
				m_lobbed.GetSprite(l).Hide();
		}

		UpdateThrowStrength(lobbed);
	}

	// Do the rest of the wrok
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Lobbed *LobbedInstrument::GetLobbed(CCompActor *who)
{
	if (!who)
		return NULL;

	// Get the inventory
	TTLWeaponInventory *inv = (TTLWeaponInventory *)who->GetComponentByName("WeaponInventory");
	if (!inv)
		return false;

	// Give it to them!
	return inv->GetLobbed();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbedInstrument::HandleWeaponChange(
Lobbed *lobbed)
{
	int index = lobbed ? lobbed->GetProperties().GetIndex() : -1;
	if (m_lastLobbed == index)
		return;

	// Save it
	m_lastLobbed = index;

	// Start the load
	char textureDB[256];
	if (!GetDBLName(lobbed, textureDB))
		m_lobbed.SetTextureDB(NULL);
	else
		m_lobbed.SetTextureDB(textureDB);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbedInstrument::UpdateThrowStrength(
Lobbed *lobbed)
{
	if (!lobbed)
	{
		m_throwStrength.CropRect(0.0f, 0.0f, 0.0f, 0.0f);
		m_throwStrength.Hide();
	}
	else
	{
		float strength = Math::Clamp(lobbed->GetThrowStrength());
		m_throwStrength.Show();
		m_throwStrength.CropRect(0.0f, 0.0f, strength, 1.0f);
		m_throwStrength.SetScreenPosition(320.0f, 240.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbedInstrument::Setup(
float textWidth)
{
	textWidth += LOBBED_TEXTMARGIN;
	for (unsigned int l = 0; l < HUD_SHOW_LOBBED; l++)
	{
		Sprite &sprite = m_lobbed.GetSprite(l);
		sprite.SetSize(LOBBED_WIDTH, LOBBED_HEIGHT);
		sprite.SetOrigin(0.0f, 0.0f);
		sprite.SetPosition((float)(l + 1) * -LOBBED_SPACING - textWidth, -LOBBED_HEIGHT);
		sprite.SetAlpha(WEAPON_ALPHA);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbedInstrument::GetDBLName(
Lobbed *lobbed,
char *dblName)
{
	// Who's on first
	if (!lobbed)
		return false;

	sprintf(dblName, "newhud/%s.dbl", lobbed->Name());
	return true;
}

//#undef so the SN pch won't take a shit
#undef LOBBED_WIDTH
#undef LOBBED_HEIGHT
#undef LOBBED_SPACING
#undef LOBBED_TEXTMARGIN
#undef TEXT_ALPHA
#undef WEAPON_ALPHA

