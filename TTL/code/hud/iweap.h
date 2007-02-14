///////////////////////////////////////////////////////////////////////////////
// Weapon instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef IWEAPON_H
#define IWEAPON_H

#include "hud\text.h"
#include "hud\ipanel\isprite.h"
#include "hud\ssprite.h"
#include "weapons\ttlweapon.h"
#include "weapons\ttlweapinv.h"

class Firearm;
class WeaponSubInstrument;

#define HUD_WEAPONS 	4

///////////////////////////////////////
///////////////////////////////////////
class WeaponInstrument : public SpriteInstrument
{
public:
	// Constructor
	WeaponInstrument();
	~WeaponInstrument();

	// Set the font!
	void SetFont(const char *font, float fontHeight = -1.0f);

	// Fix our extents
	Vector2 Extents();

	// The update (called once per frame)
	void Update(float dt);

public:
	char							m_font[32];
	float							m_fontHeight;
	WeaponSubInstrument		*m_sub[HUD_WEAPONS];

	void UpdateWeapons(TTLWeaponInventory *inventory, float dt);

	bool IsFirearm(const TTLWeapon *weapon);

	WeaponSubInstrument *Create(const TTLWeapon *weapon);
	void Destroy(int instrument);
	void DestroyAll();
	void ShowAll(bool show);

	WeaponSubInstrument *GetSelected();
};

///////////////////////////////////////
///////////////////////////////////////
class WeaponSubInstrument
{
public:
	WeaponSubInstrument(const TTLWeapon &weapon, Sprite &parent, const char *font, float fontHeight = -1.0f);
	virtual ~WeaponSubInstrument() {}

	// Doh
	bool SameWeapon(const TTLWeapon *weapon)
		{ return (weapon && weapon->GetProperties().GetIndex() == m_weaponIndex); }

	// Get the extents
	virtual Vector2 Extents() = 0;

	// Update
	virtual void Update(const TTLWeapon &weapon, bool selected, float dt) = 0;

	// Show/hide
  	virtual void Show(bool show) = 0;

protected:
	int				m_weaponIndex;

	// Get the dbl name for the weapon
	static void GetWeaponDBL(const TTLWeapon &weapon, char *weaponDBL);
	static void ForceHeight(Sprite &sprite, float forceHeight, bool onlyIfBigger = false);
};

///////////////////////////////////////
///////////////////////////////////////
class SimpleWeaponSubInstrument : public WeaponSubInstrument
{
public:
	SimpleWeaponSubInstrument(const TTLWeapon &weapon, Sprite &parent, const char *font, float fontHeight = -1.0f);
	~SimpleWeaponSubInstrument() {}

	// Get the extents
	Vector2 Extents();

	// Update
	void Update(const TTLWeapon &weapon, bool selected, float dt);

	// Show/hide
  	void Show(bool show);

private:
	Text					m_name;
	StreamedSprite    m_weapon;
};

///////////////////////////////////////
///////////////////////////////////////
class FirearmWeaponSubInstrument : public WeaponSubInstrument
{
public:
	FirearmWeaponSubInstrument(const TTLWeapon &weapon, Sprite &parent, const char *font, float fontHeight = -1.0f);
	~FirearmWeaponSubInstrument();

	// Get the extents
	Vector2 Extents();

	// Update
	void Update(const TTLWeapon &weapon, bool selected, float dt);

	// Show/hide
  	void Show(bool show);

private:
	enum WeaponLayer
	{
		WEAPON_IMAGE,
		WEAPON_IMAGEDUAL,
		BULLET_IMAGE,
		WEAPONLAYERS
	};
	StreamedSpriteArray<WEAPONLAYERS>	m_weapon;

	enum TrayLayer
	{
		TRAY_BACKGROUND,
		TRAY_FRAME,
		TRAYLAYERS
	};
	StreamedSpriteArray<TRAYLAYERS>		m_tray;

	// The text
	Text							m_name;
	Text							m_inClip;
	Text							m_totalRounds;
	Text							m_bulletName;
	void SetupFont(const char *font, float fontHeight, Text &text);

	// All the shit for the tray
	enum TrayType
	{
		TRAYTYPE_CLIP,
		TRAYTYPE_SHOTGUN,
		TRAYTYPE_REVOLVER
	};
	struct TrayDescription
	{
		const char *dblName;
		TrayType		type;
		int			rounds;
		int			rows;
		float			sizeInX, sizeInY;
		float			evenOffsetX, oddOffsetX; // These are the inner/outter radius for revlover
		float			baseOffsetY;
	};
	const TrayDescription 	*m_trayDescription;
	Sprite						*m_round;
	int							m_rounds;
	bool CreateRounds(const Firearm &firearm);
	void SetupRounds(t_Handle textureDB);
	void DrawRounds(int inGun);
	void HideRounds();
	void DestroyRounds();

	// The global descriptions
	static TrayDescription 	s_trayDescription[];
	static int 					s_trayDescriptions;

	static const TrayDescription *BestTray(const Firearm &firearm);
	static float TrayHeight(const TrayDescription &desc);
	static float TrayWidth(const TrayDescription &desc);
	static Vector2 RoundOffset(int index, const TrayDescription &desc);

	static int Capacity(const Firearm &firearm);
	static int RoundsLoaded(const Firearm &firearm);
	static bool IsShotgun(const Firearm &firearm);
	static bool IsRevolver(const Firearm &firearm);
	static bool IsDualWielding(const Firearm &firearm);
};


#endif
