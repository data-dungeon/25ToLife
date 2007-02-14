///////////////////////////////////////////////////////////////////////////////
// 3D player names instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef IPLAYERNAMES_H
#define IPLAYERNAMES_H

#include "hud/ipanel/isprite.h"
#include "hud/text3d.h"

#define MAX_PLAYER_NAMES	16
#define MAX_AI_NAMES			32

///////////////////////////////////////
///////////////////////////////////////
class PlayerNamesInstrument : public SpriteInstrument
{
public:
  // Constructor
	PlayerNamesInstrument();
	~PlayerNamesInstrument() {}

	// Return the XY extent of this instrument (you write this!)
	Vector2 Extents()
		{ return Vector2(0.0f, 0.0f); }

	// The update (called once per frame)
	void Update(float dt);

private:
	Text3D 		m_nameTag[MAX_PLAYER_NAMES];
};

///////////////////////////////////////
///////////////////////////////////////
class AISpyInstrument : public SpriteInstrument
{
public:
  // Constructor
	AISpyInstrument();
	~AISpyInstrument() {}

	// Return the XY extent of this instrument (you write this!)
	Vector2 Extents()
		{ return Vector2(0.0f, 0.0f); }

	// The update (called once per frame)
	void Update(float dt);

private:
	Text3D 		m_nameTag[MAX_AI_NAMES];

	void MakeAIName(CCompActor *aiActor, char *name);
	bool HasContact(CCompActor &who);
	void GetPsyche(CCompActor &who, char *psyche);
	float GetHP(CCompActor &who);
};

#endif
