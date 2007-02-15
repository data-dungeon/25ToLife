///////////////////////////////////////////////////////////////////////////////
// Simple system for laying out an "Instrument Panel" i.e. HUD. Let's
// each instrument on a panel to be written and maintained separately.
///////////////////////////////////////////////////////////////////////////////
#ifndef INSTRUMENT_H
#define INSTRUMENT_H

class Sprite;

///////////////////////////////////////
///////////////////////////////////////
class Instrument
{
public:
	// Constructor
	Instrument(const char *name);
	virtual ~Instrument() {}

	// This is called by the group to set the overall parent
	virtual void SetRoot(Sprite &root) {}

	// Get at our name
	const char *Name() const
		{ return m_name; }

	// Show/hide the instrument (pass negative time to use default, 0.0f to be instant)
	virtual bool Show(bool show = true);
	inline bool Hide()
		{ return Show(false); }
	inline bool Hidden() const
		{ return m_hidden; }

	// Activate/deactivate (hides and remains hidden)
	virtual bool Activate(bool activate = true);
	inline bool Deactivate()
		{ return Activate(false); }
	inline bool Active() const
		{ return m_active; }

	// For fading in and out
	virtual void SetBaseAlpha(float alpha) = 0;
	virtual void SetBaseScale(float scale) {}
	virtual void SetSpritePriority(int priority) {}

	// Return the XY extent of this instrument (you write this!)
	virtual Vector2 Extents() = 0;

	// The parent calls this to set your position
	virtual void Place(float x, float y) = 0;

	// The update (called once per frame)
	virtual void Update(float dt) = 0;

private:
	char							 				m_name[32];

protected:
	// Let subclasses manipulate directly
	bool											m_hidden, m_active;

public:
	// We appear in a list
	Instrument									*prev, *next;
};

#endif
