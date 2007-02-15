//============================================================================
//= DisplaySimpleText.h
//============================================================================

#ifndef _DISPLAY_SIMPLE_TEXT_H
#define _DISPLAY_SIMPLE_TEXT_H

#include <stdarg.h>
#include "platform/PS2/DisplaySimple.h"

#define DST_MAX_COLS 60
#define DST_MAX_ROWS 30
#define DST_BUFFER	(4096 + 16)

//============================================================================

class PS2DisplaySimpleText : public PS2DisplaySimple
{
public:
	struct FontMetrics
	{
		short x1;
		short y1;
		short x2;
		short y2;
		short cx;
		short cy;
	};

	void	Init(int width, int height, bool pal, const unsigned char* fontData, const FontMetrics* fontMetrics);
	void  Clear();
	void  SetBackgroundColor(int r, int g, int b);
	void  SetForegroundColor(int r, int g, int b);
	int	MaxRows() const;
	int	MaxCols() const;
	int	CurRow() const;
	int	CurCol() const;
	void	SetPosition(int x, int y);
	void	Printf(const char* fmt, ...);
	void	Flip();

private:
	const unsigned char* d_fontData;
	const FontMetrics*   d_fontMetrics;

	unsigned char d_buffer[DST_BUFFER];
	unsigned char d_screen[DST_MAX_ROWS][DST_MAX_COLS];
	unsigned int  d_backgroundColor;
	unsigned int  d_foregroundColor;
	int			  d_maxRows;
	int			  d_maxCols;
	int			  d_leftMargin;
	int			  d_topMargin;
	int			  d_fontW;
	int			  d_fontH;
	int			  d_x;
	int			  d_y;
};

#endif //_DISPLAY_SIMPLE_TEXT_H

//============================================================================
