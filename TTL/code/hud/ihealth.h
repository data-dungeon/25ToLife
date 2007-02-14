///////////////////////////////////////////////////////////////////////////////
// Health instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef IHEALTH_H
#define IHEALTH_H

#include "hud\ipanel\isprite.h"
#include "hud\ssprite.h"

///////////////////////////////////////
///////////////////////////////////////
class HealthInstrument : public SpriteInstrument
{
public:
	// Constructor
	HealthInstrument();
	~HealthInstrument() {}

	// our extents
	Vector2 Extents();

	// The update (called once per frame)
	void Update(float dt);

	// Wee!
	void Pulse(float strength, float duration)
		{	m_pulse = strength; m_pulseRate = strength / duration; }

private:
	// Get at an actors health! returns false
	// if this actor does not have health
	bool GetHealth(CCompActor *who, float &health, bool &alive);

	// Our player type
	enum PlayerType
	{
		NONE,
		COP,
		GANGSTER
	};
	PlayerType GetPlayerType(CCompActor *who);
	PlayerType m_currentPlayerType;

	// Update type
	bool UpdateType(CCompActor *who);

	// Load the right sprite up
	enum Layer
	{
		BAR_BASECOLOR,
		BAR_FILL,
		BAR_OUTLINE,
		BAR_LAYERS
	};
	StreamedSpriteArray<BAR_LAYERS> m_layer;
	void SetupSprites();

	// Update the indicator!
	void UpdateIndicator(float health, bool alive);

	// Get the right dbl name based on player type
	const char *TextureDBName(PlayerType playerType);

	// Pulsing support
	float				m_pulse;
	float				m_pulseRate;
};

#endif
