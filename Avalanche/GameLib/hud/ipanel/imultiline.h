///////////////////////////////////////////////////////////////////////////////
// Simple multiline based instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef IMULTILINE_H
#define IMULTILINE_H

#include "hud\text.h"
#include "hud\ipanel\instrument.h"

///////////////////////////////////////
///////////////////////////////////////
class MultilineTextInstrument : public Instrument
{
public:
	// Constructor
	MultilineTextInstrument(const char *name, int lines, float timeOut, const char *fontName = NULL, float textHeight = -1.0f, bool shadow = false);
	virtual ~MultilineTextInstrument();

	// This is called by the group to set the overall parent
	virtual void SetRoot(Sprite &root);

	// For fading in and out
	virtual void SetBaseAlpha(float alpha);
	virtual void SetSpritePriority(int priority);

	// Set the color of everytihng
	virtual void SetColor(const Vector4 &color);

	// Return the XY extent of this instrument (you write this!)
	virtual Vector2 Extents();

	// The parent calls this to set your position
	virtual void Place(float x, float y);

	// The update (called once per frame)
	virtual void Update(float dt);

	// Set the text
	bool AddText(const char *newText);

protected:
	Vector2		m_pos;

	class TextLine
	{
	public:
		TextLine() { m_born = 0.0f; }
		~TextLine() {}

		float			m_born;
		Text			m_text;
		TextLine 	*prev, *next;
	};
	DoublyLinkedList<TextLine> 	m_lines;
	int 									m_maxLines;
	float									m_timeOut;

	// Get the line height
	float GetLineHeight();
};

#endif
