///////////////////////////////////////////////////////////////////////////////
// Simple text based instrument
///////////////////////////////////////////////////////////////////////////////
#include "hud/HudPCH.h"

#include "hud/text.h"
#include "enginehelper/timer.h"
#include "hud/ipanel/imultiline.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultilineTextInstrument::MultilineTextInstrument(
const char *name,
int lines,
float timeOut,
const char *fontName,
float textHeight,
bool shadow) :
Instrument(name),
m_lines(true)
{
	m_pos.Clear();

	ASSERT(lines > 0);
	ASSERT(timeOut > 0.0f);

	m_maxLines = lines;
	m_timeOut = timeOut;
	for (int l = 0; l < lines; l++)
	{
		TextLine *textLine = new TextLine;
		ASSERT(textLine);
		if (textLine)
		{
			if (fontName)
				textLine->m_text.SetFont((char *)fontName);
			if (textHeight > 0.0f)
				textLine->m_text.SetTextHeight(textHeight);
			textLine->m_text.ShadowSettings(shadow);
			textLine->m_text.SetPriority(50);
			m_lines.AddTail(textLine);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultilineTextInstrument::~MultilineTextInstrument()
{
}

///////////////////////////////////////////////////////////////////////////////
// This is called by the group to set the overall parent
///////////////////////////////////////////////////////////////////////////////
void MultilineTextInstrument::SetRoot(
Sprite &root)
{
	TextLine *line = m_lines.Head();
	while (line)
	{
		root.AddChild(&line->m_text);
		line = line->next;
	}
}

///////////////////////////////////////////////////////////////////////////////
// For fading in and out
///////////////////////////////////////////////////////////////////////////////
void MultilineTextInstrument::SetBaseAlpha(
float alpha)
{
	TextLine *line = m_lines.Head();
	while (line)
	{
		line->m_text.SetFamilyColor(1.0f, 1.0f, 1.0f, alpha);
		line = line->next;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultilineTextInstrument::SetSpritePriority(int priority)
{
	TextLine *line = m_lines.Head();
	while (line)
	{
		line->m_text.SetPriority(priority);
		line = line->next;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Set the color of everytihng
///////////////////////////////////////////////////////////////////////////////
void MultilineTextInstrument::SetColor(const Vector4 &color)
{
	TextLine *line = m_lines.Head();
	while (line)
	{
		line->m_text.SetColor(color.X(), color.Y(), color.Z(), color.W());
		line = line->next;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Return the XY extent of this instrument (you write this!)
///////////////////////////////////////////////////////////////////////////////
Vector2 MultilineTextInstrument::Extents()
{
	Vector2 extents(0.0f, GetLineHeight() * (float)m_maxLines);

	TextLine *line = m_lines.Head();
	while (line)
	{
		float width = line->m_text.ComputeTextLength();
		if (width > extents.X())
			extents.X(width);

		line = line->next;
	}

	return extents;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultilineTextInstrument::Place(
float x,
float y)
{
	m_pos.Set(x, y);
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void MultilineTextInstrument::Update(float dt)
{
	// Update the lines
	float lineHeight = GetLineHeight();
	float y = m_pos.Y() + lineHeight * (float)(m_maxLines - 1);
	TextLine *line = m_lines.Head();
	while (line)
	{
		// Doh!
		if (Hidden() || !Active())
			line->m_text.Hide();
		else
			line->m_text.Show();

		// Time out?
		if (line->m_born + m_timeOut < g_timer.GetEndSec())
			line->m_text.ClearText();

		// Setup the positions
		line->m_text.SetPosition(m_pos.X(), y);

		y -= lineHeight;
		line = line->next;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Set the text
///////////////////////////////////////////////////////////////////////////////
bool MultilineTextInstrument::AddText(
const char *newText)
{
	if (!newText)
		return false;

	// Tail is always the oldest
	TextLine *line = m_lines.Tail();
	if (!line)
		return false;

	m_lines.Unlink(line);
	line->m_text.SetText(newText);
	line->m_born = g_timer.GetEndSec();
	m_lines.AddHead(line);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float MultilineTextInstrument::GetLineHeight()
{
	TextLine *line = m_lines.Head();
	if (!line)
		return 0.0f;

	return line->m_text.GetTextHeight();
}

