///////////////////////////////////////////////////////////////////////////////
// Network status instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef INETSTAT_H
#define INETSTAT_H

#include "hud\ipanel\isprite.h"

///////////////////////////////////////
///////////////////////////////////////
class NetStatInstrument : public SpriteInstrument
{
public:
	// Constructor
	NetStatInstrument();
	~NetStatInstrument();

	// our extents
	Vector2 Extents();

	// The update (called once per frame)
	void Update(float dt);

private:
	Sprite		m_host;
	Sprite		m_problem;
	float			m_problemAlpha;
};

#endif
