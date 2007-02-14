///////////////////////////////////////////////////////////////////////////////
// Material spy instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef IMATSPY_H
#define IMATSPY_H

#include "hud\ipanel\itext.h"

///////////////////////////////////////
///////////////////////////////////////
class MaterialSpyInstrument : public TextInstrument
{
public:
	// Constructor
	MaterialSpyInstrument();
	~MaterialSpyInstrument() {}

	// The update (called once per frame)
	void Update(float dt);

	// Wee!
	static void Enable(bool enable)
		{ s_enable = enable; }
	static bool Enabled()
		{ return s_enable; }
	static void Toggle()
		{ Enable(!Enabled()); }

private:
	static bool	s_enable;
};

extern bool g_materialSpyEnable;

#endif
