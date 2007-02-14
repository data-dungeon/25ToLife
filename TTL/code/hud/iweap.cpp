///////////////////////////////////////////////////////////////////////////////
// The Weapon instrument!
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/ttlweapinv.h"
#include "weapons/firearm.h"
#include "hud/iweap.h"

#define WEAPON_IMAGE_FORCEHEIGHT	48.0f
#define BULLET_MARGIN				5.0f
#define WEAPON_ALPHA					0.5f
#define TEXT_ALPHA					0.6f
#define LOWAMMO_ALPHA				0.25f
#define LOWAMMO_RATIO				0.25f
#define TRAY_PUSHDOWN 				8.0f
#define TRAY_PUSHLEFT 				8.0f
#define DUAL_OFFSETX					16.0f
#define DUAL_OFFSETY					4.0f
#define BASE_PRIORITY				50

// The trays!
FirearmWeaponSubInstrument::TrayDescription FirearmWeaponSubInstrument::s_trayDescription[] =
{  //                                                 Xsz   Ysz   eox     oox  oy
	{ "newhud/tray1.dbl", TRAYTYPE_CLIP,  	   1,	   1, 25.0f, 25.0f, 12.5f, 0.0f, 12.5f },
	{ "newhud/tray5fmj.dbl", TRAYTYPE_CLIP,  	5, 	1, 13.0f, 11.0f, 8.0f, 0.0f, 8.0f },
	{ "newhud/tray7fmj.dbl", TRAYTYPE_CLIP,  	7, 	1, 13.0f, 11.0f, 8.0f, 0.0f, 8.0f },
	{ "newhud/tray8fmj.dbl", TRAYTYPE_CLIP,  	8, 	1, 13.0f, 11.0f, 8.0f, 0.0f, 8.0f },

	{ "newhud/tray11fmj.dbl", TRAYTYPE_CLIP, 	11, 	2, 13.0f, 11.0f, 8.0f, 14.0f, 8.0f },
	{ "newhud/tray15fmj.dbl", TRAYTYPE_CLIP, 	15, 	2, 13.0f, 11.0f, 8.0f, 14.0f, 8.0f },
	{ "newhud/tray16fmj.dbl", TRAYTYPE_CLIP, 	16, 	2, 13.0f, 11.0f, 8.0f, 14.0f, 8.0f },

	{ "newhud/tray20fmj.dbl", TRAYTYPE_CLIP, 	20, 	2, 13.0f, 11.0f, 14.0f, 8.0f, 8.0f },

	{ "newhud/tray30fmj.dbl", TRAYTYPE_CLIP, 	30, 	3, 13.0f, 11.0f, 14.0f, 8.0f, 8.0f },

	{ "newhud/tray100fmj.dbl", TRAYTYPE_CLIP,	100, 	5, 6.0f, 5.0f, 6.0f, 9.0f, 6.0f },

	{ "newhud/tray2sg.dbl", TRAYTYPE_SHOTGUN, 2, 1, 22.0f, 18.0f, 12.0f, 22.0f, 12.0f },
	{ "newhud/tray6sg.dbl", TRAYTYPE_SHOTGUN, 6, 2, 22.0f, 18.0f, 12.0f, 22.0f, 12.0f },
	{ "newhud/tray7sg.dbl", TRAYTYPE_SHOTGUN, 7, 2, 22.0f, 18.0f, 12.0f, 22.0f, 12.0f },
	{ "newhud/tray8sg.dbl", TRAYTYPE_SHOTGUN, 8, 2, 22.0f, 18.0f, 12.0f, 22.0f, 12.0f },

	{ "newhud/tray6rev.dbl", TRAYTYPE_REVOLVER, 6, 0, 20.5f, 20.5f, 14.0f, 22.0f, 11.0f }
};
int FirearmWeaponSubInstrument::s_trayDescriptions = sizeof(FirearmWeaponSubInstrument::s_trayDescription) / sizeof(FirearmWeaponSubInstrument::TrayDescription);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WeaponInstrument::WeaponInstrument() :
SpriteInstrument("weapon")
{
	for (int w = 0; w < HUD_WEAPONS; w++)
		m_sub[w] = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Set the font!
///////////////////////////////////////////////////////////////////////////////
void WeaponInstrument::SetFont(
const char *font,
float fontHeight)
{
	if (font)
		strcpy(m_font, font);
	else
		m_font[0] = '\0';
	m_fontHeight = fontHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WeaponInstrument::~WeaponInstrument()
{
	DestroyAll();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 WeaponInstrument::Extents()
{
	Vector2 extents;
	WeaponSubInstrument *sub = GetSelected();
	if (!sub)
		extents.Set(0.0f, 0.0f);
	else
		extents = sub->Extents();

	return extents;
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void WeaponInstrument::Update(float dt)
{
	if (m_hidden || !m_active)
		ShowAll(false);
	else
	{
		CCompActor *actor = g_hud.GetSubject();
		TTLWeaponInventory *inventory = actor ? (TTLWeaponInventory *)actor->GetComponentByName("WeaponInventory") : NULL;
		if (inventory)
			UpdateWeapons(inventory, dt);
		else
			ShowAll(false);
	}

	// Do the rest of the wrok
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
// Upadte everything
///////////////////////////////////////////////////////////////////////////////
void WeaponInstrument::UpdateWeapons(
TTLWeaponInventory *inventory,
float dt)
{
	// Duh
	ASSERT(inventory);
	if (!inventory)
		return;

	// Enumerate the invetory
	const Lobbed *lobbed = (Lobbed *)inventory->GetLobbed();
	const TTLWeapon *selected = (TTLWeapon *)inventory->GetSelected();
	for (int w = 0; w < HUD_WEAPONS; w++)
	{
		// Figure out the weapon in question (ignore lobbed)
		const TTLWeapon *weapon = (TTLWeapon *)inventory->Get(w);
		if (weapon == (TTLWeapon *)lobbed)
			weapon = NULL;

		// Handle changes
		if (!weapon)
			Destroy(w);
		else if (!m_sub[w] || (m_sub[w] && !m_sub[w]->SameWeapon(weapon)))
		{
			Destroy(w);
			m_sub[w] = Create(weapon);
		}

		// Do it!
		if (m_sub[w])
		{
			ASSERT(weapon);
			bool isSelected = (weapon == selected);
			m_sub[w]->Update(*weapon, isSelected, dt);
			m_sub[w]->Show(isSelected);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WeaponInstrument::IsFirearm(
const TTLWeapon *weapon)
{
	return (weapon && weapon->GetWeaponClass() == WEAPONCLASS_FIREARM);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WeaponSubInstrument *WeaponInstrument::Create(
const TTLWeapon *weapon)
{
	if (!weapon)
		return NULL;
	else if (IsFirearm(weapon))
		return new FirearmWeaponSubInstrument(*weapon, m_sprite, m_font, m_fontHeight);
	else
		return new SimpleWeaponSubInstrument(*weapon, m_sprite, m_font, m_fontHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WeaponInstrument::Destroy(int instrument)
{
	if (m_sub[instrument])
	{
		delete m_sub[instrument];
		m_sub[instrument] = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WeaponInstrument::DestroyAll()
{
	for (int w = 0; w < HUD_WEAPONS; w++)
		Destroy(w);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WeaponInstrument::ShowAll(bool show)
{
	for (int w = 0; w < HUD_WEAPONS; w++)
	{
		if (m_sub[w])
			m_sub[w]->Show(show);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WeaponSubInstrument *WeaponInstrument::GetSelected()
{
  	CCompActor *actor = g_hud.GetSubject();
	if (!actor)
		return NULL;

	TTLWeaponInventory *inventory = (TTLWeaponInventory *)actor->GetComponentByName("WeaponInventory");
	if (!inventory)
		return NULL;

	const TTLWeapon *lobbed = (TTLWeapon *)inventory->GetLobbed();
	const TTLWeapon *selected = (TTLWeapon *)inventory->GetSelected();
	if (lobbed == selected)
		return NULL;

	int index = inventory->GetSelectedIndex();
	if (index < 0)
		return NULL;

	return m_sub[index];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WeaponSubInstrument::WeaponSubInstrument(
const TTLWeapon &weapon,
Sprite &parent,
const char *font,
float fontHeight)
{
	m_weaponIndex = weapon.GetProperties().GetIndex();
}

///////////////////////////////////////////////////////////////////////////////
// Get the dbl name for the weapon
///////////////////////////////////////////////////////////////////////////////
void WeaponSubInstrument::GetWeaponDBL(
const TTLWeapon &weapon,
char *weaponDBL)
{
	sprintf(weaponDBL, "newhud/%s.dbl", weapon.Icon());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WeaponSubInstrument::ForceHeight(
Sprite &sprite,
float forceHeight,
bool onlyIfBigger)
{
	float width, height;
	sprite.GetSize(width, height);
	if (Math::Zero(width) || Math::Zero(height))
		return;

	if (onlyIfBigger && height <= forceHeight)
		return;

	float ratio = forceHeight / height;
	sprite.SetSize(width * ratio, forceHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SimpleWeaponSubInstrument::SimpleWeaponSubInstrument(
const TTLWeapon &weapon,
Sprite &parent,
const char *font,
float fontHeight) :
WeaponSubInstrument(weapon, parent, font, fontHeight)
{
	parent.AddChild(&m_name);
	parent.AddChild(&m_weapon.GetSprite());

	if (font && font[0])
		m_name.SetFont((char *)font);
	if (fontHeight > 0.0f)
		m_name.SetTextHeight(fontHeight);
	m_name.SetAlpha(TEXT_ALPHA);

	char textureDBL[128];
	GetWeaponDBL(weapon, textureDBL);
	m_weapon.SetTextureDB(textureDBL);
}

///////////////////////////////////////////////////////////////////////////////
// Get the extents
///////////////////////////////////////////////////////////////////////////////
Vector2 SimpleWeaponSubInstrument::Extents()
{
	Vector2 extents;
	extents.Set(0.0f, m_weapon.GetSprite().GetHeight() + m_name.GetTextHeight());
	return extents;
}

///////////////////////////////////////////////////////////////////////////////
// Update
///////////////////////////////////////////////////////////////////////////////
void SimpleWeaponSubInstrument::Update(
const TTLWeapon &weapon,
bool selected,
float dt)
{
	if (m_weapon.Process() == SSS_INITIALIZE)
	{
		Sprite &sprite = m_weapon.GetSprite();
		sprite.SetOrigin(1.0f, 0.0f);
		sprite.SetPosition(0.0f, 0.0f);
		sprite.SetAlpha(WEAPON_ALPHA);
		ForceHeight(sprite, WEAPON_IMAGE_FORCEHEIGHT, true);
	}

	float height = m_weapon.GetSprite().GetHeight();
	m_name.SetText(weapon.DisplayName());
 	float textWidth = m_name.ComputeTextLength();
 	m_name.SetPosition(-textWidth, height);
}

///////////////////////////////////////////////////////////////////////////////
// Show/hide
///////////////////////////////////////////////////////////////////////////////
void SimpleWeaponSubInstrument::Show(
bool show)
{
	if (show)
		m_name.ShowAll();
	else
		m_name.HideAll();
	m_weapon.ShowAll(show);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FirearmWeaponSubInstrument::FirearmWeaponSubInstrument(
const TTLWeapon &weapon,
Sprite &parent,
const char *font,
float fontHeight) :
WeaponSubInstrument(weapon, parent, font, fontHeight)
{
	// Add the text
	parent.AddChild(&m_name);
	SetupFont(font, fontHeight, m_name);
	parent.AddChild(&m_inClip);
	SetupFont(font, fontHeight * 2.0f, m_inClip);
	parent.AddChild(&m_totalRounds);
	SetupFont(font, fontHeight, m_totalRounds);
	parent.AddChild(&m_bulletName);
	SetupFont(font, fontHeight, m_bulletName);

	// Do the sprites
	for (unsigned int w = 0; w < WEAPONLAYERS; w++)
		parent.AddChild(&m_weapon.GetSprite(w));

	for (unsigned int t = 0; t < TRAYLAYERS; t++)
		parent.AddChild(&m_tray.GetSprite(t));

	// Start the dbl loading w/the weapon shiznol
	char textureDBL[128];
	GetWeaponDBL(weapon, textureDBL);
	m_weapon.SetTextureDB(textureDBL);
	m_weapon.SetTexture(WEAPON_IMAGE, 0);
	m_weapon.SetTexture(WEAPON_IMAGEDUAL, 0);
	m_weapon.SetTexture(BULLET_IMAGE, 1);

	// Figure out the tray to load...
	m_trayDescription = BestTray((const Firearm &)weapon);
	ASSERT(m_trayDescription);

	// load the tray
	m_tray.SetTextureDB(m_trayDescription->dblName);

	// No rounds yet
	m_round = NULL;
	m_rounds = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FirearmWeaponSubInstrument::~FirearmWeaponSubInstrument()
{
	// Free!
	DestroyRounds();
}

///////////////////////////////////////////////////////////////////////////////
// Get the extents
///////////////////////////////////////////////////////////////////////////////
Vector2 FirearmWeaponSubInstrument::Extents()
{
	Vector2 extents;
	extents.Set(0.0f,
					m_weapon.GetSprite(WEAPON_IMAGE).GetHeight() +
					m_bulletName.GetTextHeight() +
					TRAY_PUSHDOWN + TrayHeight(*m_trayDescription));
	return extents;
}

///////////////////////////////////////////////////////////////////////////////
// Update
///////////////////////////////////////////////////////////////////////////////
void FirearmWeaponSubInstrument::Update(
const TTLWeapon &weapon,
bool selected,
float dt)
{
	const Firearm &firearm = (const Firearm &)weapon;

	m_name.SetText(weapon.DisplayName());
 	float nameWidth = m_name.ComputeTextLength();

	m_bulletName.SetText(firearm.DisplayProjectile());

	char num[64];
	sprintf(num, "%d", firearm.TotalRounds());
	m_totalRounds.SetText(num);
 	sprintf(num, "%d", firearm.RoundsLoaded());
	m_inClip.SetText(num);

	if (m_weapon.Process() == SSS_INITIALIZE)
	{
		Sprite &weaponSprite = m_weapon.GetSprite(WEAPON_IMAGE);
		weaponSprite.SetOrigin(1.0f, 0.0f);
		weaponSprite.SetPosition(-nameWidth, 0.0f);
		weaponSprite.SetAlpha(WEAPON_ALPHA);
		ForceHeight(weaponSprite, WEAPON_IMAGE_FORCEHEIGHT, true);

		Sprite &dualSprite = m_weapon.GetSprite(WEAPON_IMAGEDUAL);
		if (firearm.IsDualWield())
		{
			dualSprite.SetOrigin(1.0f, 0.0f);
			dualSprite.SetPosition(-nameWidth - DUAL_OFFSETX, DUAL_OFFSETY);
			ForceHeight(dualSprite, WEAPON_IMAGE_FORCEHEIGHT, true);
		}

		Sprite &bullet = m_weapon.GetSprite(BULLET_IMAGE);
		ForceHeight(bullet, m_bulletName.ComputeTextHeight() * 0.5f, true);
		bullet.SetOrigin(0.0f, 0.5f);
		bullet.SetAlpha(WEAPON_ALPHA);
	}

	// Handle dual wielding
	Sprite &dualSprite = m_weapon.GetSprite(WEAPON_IMAGEDUAL);
	if (!IsDualWielding(firearm))
	 	dualSprite.SetAlpha(0.0f);
	else
		dualSprite.SetAlpha(WEAPON_ALPHA);

	float weaponHeight = m_weapon.GetSprite(WEAPON_IMAGE).GetHeight();
 	m_name.SetPosition(-nameWidth, weaponHeight - m_name.ComputeTextHeight());

	float textHeight = m_name.ComputeTextHeight();
 	float bulletNameWidth = m_bulletName.ComputeTextLength();
	m_bulletName.SetPosition(-bulletNameWidth, weaponHeight);

	Sprite &bullet = m_weapon.GetSprite(BULLET_IMAGE);
	float bulletWidth = bullet.GetWidth() + BULLET_MARGIN * 2.0f;
	bullet.SetPosition(-bulletWidth - bulletNameWidth + BULLET_MARGIN, weaponHeight + textHeight * 0.5f);

 	float totalRoundsWidth = m_totalRounds.ComputeTextLength();
	m_totalRounds.SetPosition(-totalRoundsWidth - bulletWidth - bulletNameWidth, weaponHeight);

	if (m_tray.Process() == SSS_INITIALIZE)
	{
		for (unsigned int t = 0; t < TRAYLAYERS; t++)
		{
			Sprite &tray = m_tray.GetSprite(t);
			tray.SetOrigin(0.0f, 0.0f);
		}
		m_tray.GetSprite(TRAY_FRAME).SetAlpha(WEAPON_ALPHA);
		m_tray.GetSprite(TRAY_FRAME).SetPriority(BASE_PRIORITY + 10);
		m_tray.GetSprite(TRAY_BACKGROUND).SetPriority(BASE_PRIORITY);
	}

	// Make sure this is all in the right spot
	for (unsigned int t = 0; t < TRAYLAYERS; t++)
	{
		Sprite &tray = m_tray.GetSprite(t);
		float trayWidth = TrayWidth(*m_trayDescription);
		tray.SetPosition(-trayWidth - TRAY_PUSHLEFT, textHeight + weaponHeight + TRAY_PUSHDOWN);
	}

	// setup the rounds!
	if (m_weapon.GetTextureDB() != INVALID_HANDLE && !m_round)
	{
		CreateRounds((const Firearm &)weapon);
		SetupRounds(m_weapon.GetTextureDB());
	}

	float inClipWidth = m_inClip.ComputeTextLength();
	float trayWidth = TrayWidth(*m_trayDescription);
	float trayHeight = TrayHeight(*m_trayDescription);
	float myHeight = m_inClip.ComputeTextHeight();
	m_inClip.SetPosition(-inClipWidth - trayWidth - TRAY_PUSHLEFT - 8.0f, textHeight + weaponHeight + TRAY_PUSHDOWN + ((trayHeight - myHeight) * 0.5f));

	// Update the roudn coutner
	if (!selected)
		HideRounds();
	else
	{
		float reloadRatio = firearm.ReloadedRatio();
		Sprite &tray = m_tray.GetSprite(TRAY_BACKGROUND);
		bool showReload = firearm.ClipBased() || (!firearm.ClipBased() && firearm.MagazineCapacity() == 1);
		if (reloadRatio >= 0.0f && showReload)
		{
			HideRounds();
			float maxU = trayWidth / tray.GetWidth();
			tray.SetColor(1.0f, 1.0f, 0.0f, WEAPON_ALPHA);
			tray.CropRect(0.0f, 0.0f, Math::Clamp(reloadRatio * maxU), 1.0f);
		}
		else
		{
			int rounds = RoundsLoaded(firearm);
			float roundRatio = (float)rounds / (float)Capacity(firearm);
			float alpha = (roundRatio > LOWAMMO_RATIO) ? 0.0f : Math::LinearMap(roundRatio, 0.0f, LOWAMMO_RATIO, LOWAMMO_ALPHA, 0.0f);
			tray.SetColor(1.0f, 0.0f, 0.0f, alpha);
			tray.CropRect(0.0f, 0.0f, 1.0f, 1.0f);
			DrawRounds(rounds);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Show/hide
///////////////////////////////////////////////////////////////////////////////
void FirearmWeaponSubInstrument::Show(
bool show)
{
	if (show)
	{
		m_name.ShowAll();
		m_inClip.ShowAll();
		m_totalRounds.ShowAll();
		m_bulletName.ShowAll();
	}
	else
	{
		m_name.HideAll();
		m_inClip.HideAll();
		m_totalRounds.HideAll();
		m_bulletName.HideAll();
	}

	m_weapon.ShowAll(show);
	m_tray.GetSprite(TRAY_BACKGROUND).Show(show);
	m_tray.GetSprite(TRAY_FRAME).Show(show);

	if (!show)
		HideRounds();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FirearmWeaponSubInstrument::SetupFont(
const char *font,
float fontHeight,
Text &text)
{
  if (font && font[0])
		text.SetFont((char *)font);
	if (fontHeight > 0.0f)
		text.SetTextHeight(fontHeight);
	text.SetOrigin(0.0f, 0.0f);
 	text.SetAlpha(TEXT_ALPHA);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FirearmWeaponSubInstrument::CreateRounds(
const Firearm &firearm)
{
	m_rounds = 0;
	m_round = NULL;

	int capacity = firearm.ClipBased() ? firearm.ClipCapacity() : firearm.MagazineCapacity();
	if (!m_trayDescription || capacity == 0)
		return false;

	if (m_trayDescription->rounds < capacity)
		capacity = m_trayDescription->rounds;

	m_round = new Sprite[capacity];
	ASSERT(m_round);
	if (!m_round)
		return false;
	m_rounds = capacity;

	Sprite &tray = m_tray.GetSprite(TRAY_FRAME);
	for (int r = 0; r < m_rounds; r++)
		tray.AddChild(&m_round[r]);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FirearmWeaponSubInstrument::SetupRounds(
t_Handle textureDB)
{
	if (textureDB == INVALID_HANDLE || !m_trayDescription)
		return;

	for (int r = 0; r < m_rounds; r++)
	{
		m_round[r].SetTextureSet(textureDB, 2);
		m_round[r].Reset();
		m_round[r].SetOrigin(0.5f, 0.5f);
		Vector2 offset = RoundOffset(r, *m_trayDescription);
		m_round[r].SetPosition(offset.X(), offset.Y());
		m_round[r].SetPriority(BASE_PRIORITY + 5);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FirearmWeaponSubInstrument::DrawRounds(
int inGun)
{
	if (!m_round)
		return;

	int show = Math::Min(inGun, m_rounds);
	for (int r = 0; r < show; r++)
		m_round[r].Show();
	for (int h = show; h < m_rounds; h++)
		m_round[h].Hide();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FirearmWeaponSubInstrument::HideRounds()
{
	for (int h = 0; h < m_rounds; h++)
		m_round[h].Hide();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FirearmWeaponSubInstrument::DestroyRounds()
{
	if (m_round)
	{
		for (int r = 0; r < m_rounds; r++)
			m_round[r].ClearTextureSet(true);
		delete [] m_round;
		m_round = NULL;
	}
	m_rounds = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const FirearmWeaponSubInstrument::TrayDescription *FirearmWeaponSubInstrument::BestTray(
const Firearm &firearm)
{
	// What are we looking for?
	TrayType lookingFor;
	if (IsShotgun(firearm))
		lookingFor = TRAYTYPE_SHOTGUN;
	else if (IsRevolver(firearm))
		lookingFor = TRAYTYPE_REVOLVER;
	else
		lookingFor = TRAYTYPE_CLIP;
	int capacity = Capacity(firearm);

	// Assume the first one
	const TrayDescription *best = &s_trayDescription[0];
	for (int d = 0; d < s_trayDescriptions; d++)
	{
		// Check this one
		const TrayDescription *check = &s_trayDescription[d];

		// Always get the type right
		if (lookingFor == check->type)
		{
			if (best->type != lookingFor)
				best = check;
			if (capacity <= check->rounds)
				return check;
		}
	}

	return best;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float FirearmWeaponSubInstrument::TrayHeight(
const TrayDescription &desc)
{
	switch (desc.type)
	{
		case TRAYTYPE_REVOLVER:
			return desc.oddOffsetX * 2.0f;
			break;

		default:
		case TRAYTYPE_CLIP:
		case TRAYTYPE_SHOTGUN:
			return (float)desc.rows * desc.sizeInY;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float FirearmWeaponSubInstrument::TrayWidth(
const TrayDescription &desc)
{
	switch (desc.type)
	{
		case TRAYTYPE_REVOLVER:
			return desc.oddOffsetX * 2.0f;
			break;

		default:
		case TRAYTYPE_CLIP:
		case TRAYTYPE_SHOTGUN:
			return (float)((desc.rounds + 1) / desc.rows) * desc.sizeInX + (Math::Max(desc.oddOffsetX, desc.evenOffsetX) - desc.sizeInX * 0.5f) * 2.0f;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 FirearmWeaponSubInstrument::RoundOffset(
int index,
const TrayDescription &desc)
{
	float x = 0.0f, y = 0.0f;

	index = desc.rounds - index - 1;
	if (desc.type == TRAYTYPE_CLIP || desc.type == TRAYTYPE_SHOTGUN)
	{
	 	int rowSize = desc.rounds / desc.rows;
	 	int row = index % desc.rows;
	 	int column = index / desc.rows;
		bool evenRow = ((row & 1) == 0);

		x = (float)column * desc.sizeInX;
		y = desc.baseOffsetY + (float)row * desc.sizeInY;
		if (evenRow)
			x += desc.evenOffsetX;
		else
			x += desc.oddOffsetX;
	}
	else if (desc.type == TRAYTYPE_REVOLVER)
	{
		float degreesPerBullet = 360.0f / (float)desc.rounds;
		float s,c;
		Math::SinCos(Math::Deg2Rad(240.0f + degreesPerBullet * (float)index), s, c);
		x = desc.sizeInX + s * desc.evenOffsetX;
		y = desc.sizeInY + c * desc.evenOffsetX;
	}
	else
	{
		ASSERTS(false, "unknown tray type");
	}

	return Vector2(x, y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FirearmWeaponSubInstrument::Capacity(const Firearm &firearm)
{
	int capacity = firearm.ClipBased() ? firearm.ClipCapacity() : firearm.MagazineCapacity();
	if (firearm.IsDualWield())
		return capacity / 2;
	else
		return capacity;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FirearmWeaponSubInstrument::RoundsLoaded(const Firearm &firearm)
{
	if (IsDualWielding(firearm))
		return firearm.RoundsLoaded() / 2;
	else
		return firearm.RoundsLoaded();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FirearmWeaponSubInstrument::IsShotgun(
const Firearm &firearm)
{
	return (firearm.GetWeaponType() == WEAPONTYPE_SHOTGUN);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FirearmWeaponSubInstrument::IsRevolver(
const Firearm &firearm)
{
	return (firearm.GetWeaponReload() == WEAPONRELOAD_REVOLVER);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FirearmWeaponSubInstrument::IsDualWielding(
const Firearm &firearm)
{
	return (firearm.IsDualWield() && firearm.OffHandEnabled());
}

//#undef so the SN pch won't take a shit
#undef WEAPON_IMAGE_FORCEHEIGHT
#undef BULLET_MARGIN
#undef WEAPON_ALPHA
#undef TEXT_ALPHA
#undef LOWAMMO_ALPHA
#undef LOWAMMO_RATIO
#undef TRAY_PUSHDOWN
#undef TRAY_PUSHLEFT
#undef DUAL_OFFSETX
#undef DUAL_OFFSETY
#undef BASE_PRIORITY

