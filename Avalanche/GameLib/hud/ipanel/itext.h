///////////////////////////////////////////////////////////////////////////////
// Simple text instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef ITEXT_H
#define ITEXT_H

#include "hud\text.h"
#include "hud\ipanel\instrument.h"

///////////////////////////////////////
///////////////////////////////////////
class TextInstrument : public Instrument
{
public:
	// Constructor
	TextInstrument(const char *name);
	virtual ~TextInstrument() {}

	// This is called by the group to set the overall parent
	virtual void SetRoot(Sprite &root)
		{ root.AddChild(&m_text); }

	// Set the font
	virtual void SetFont(const char *font, float height = -1.0f);

	// Show/hide the instrument
	virtual bool Show(bool show = true);
	virtual bool Activate(bool activate = true);

	// For fading in and out
	virtual void SetBaseAlpha(float alpha)
		{ m_text.SetFamilyColor(1.0f, 1.0f, 1.0f, alpha); }
	virtual void SetBaseScale(float scale)
		{ m_text.SetScale(scale, scale); }
	virtual void SetSpritePriority(int priority)
		{ m_text.SetPriority(priority); }

	// Return the XY extent of this instrument (you write this!)
	virtual Vector2 Extents();

	// The parent calls this to set your position
	virtual void Place(float x, float y);

	// The update (called once per frame)
	virtual void Update(float dt);

	// Set the text
	bool SetText(const char *newText, const char *font = NULL, bool shadow = false);

	float ComputeTextLength() {return m_text.ComputeTextLength();}

protected:
	Text			m_text;
	Vector2		m_pos;
};

#endif
