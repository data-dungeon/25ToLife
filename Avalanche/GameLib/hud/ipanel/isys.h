///////////////////////////////////////////////////////////////////////////////
// Simple system for laying out an "Instrument Panel" i.e. HUD. Let's
// each instrument on a panel to be written and maintained separately.
///////////////////////////////////////////////////////////////////////////////
#ifndef ISYS_H
#define ISYS_H

#include "EngineHelper\Singleton.h"
#include "hud\ipanel\ipanel.h"

///////////////////////////////////////
///////////////////////////////////////
class InstrumentSystem
{
public:
	// Setup the basic system spects
	static void SetScreenSize(int x, int y)
		{ s_screenSize.Set((float)x, (float)y); }
	static void SetOverdrawPercentage(float overdrawPercentageX, float overdrawPercentageY)
		{ s_overdrawX = overdrawPercentageX * 0.01f; s_overdrawY = overdrawPercentageY * 0.01f; }
	static void SetWidescreenOffset(float offset)
		{ s_wideScreenOffset = offset; }

	InstrumentSystem();
	~InstrumentSystem() {}

	// Create a panel
	InstrumentPanel *Create(const char *panelName);
	InstrumentPanel *Find(const char *panelName, bool create = false);
	bool Remove(const char *panelName);

	// How/hide a wholepanel
	bool Show(const char *panelName, bool show = true, float transitionTime = -1.0f);
	bool Hidden(const char *panelName);
	bool Activate(const char *panelName, bool activate = true);

	// Call this once per frame (updates all panels)
	void Update();

	// Globalized screen
	static Vector2 ScreenSize();
	Vector2 TopLeft();
	Vector2 BottomRight();

private:
	DoublyLinkedList<InstrumentPanel>	m_panels;

	static Vector2								s_screenSize;
	static float								s_overdrawX, s_overdrawY;
	static float								s_wideScreenOffset;
};

// There can be only one!
DefineSingleton(InstrumentSystem)
#define g_instruments GetSingleton(InstrumentSystem)

#endif
