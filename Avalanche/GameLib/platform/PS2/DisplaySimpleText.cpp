//============================================================================
//= DisplaySimpleText.cpp
//============================================================================

#include "platform/PlatformPCH.h"
#include "DisplaySimpleText.h"

//============================================================================

void PS2DisplaySimpleText::Init(int width, int height, bool pal, const unsigned char* fontData, const FontMetrics* fontMetrics)
{
	PS2DisplaySimple::Init(width, height, pal);

	d_fontData    = fontData;
	d_fontMetrics = fontMetrics;

	d_topMargin   = 4;
	d_leftMargin  = 4;

	d_fontW = 0;
	d_fontH = 0;
	for (int i = 32; i <= 126; i++)
	{
		const FontMetrics* m = &d_fontMetrics[i - 32];
		int w = m->x2 - m->x1;
		int h = m->y2 - m->y1;
		if (w > d_fontW) d_fontW = w;
		if (h > d_fontH) d_fontH = h;
	}

	d_maxCols = (width - d_leftMargin) / d_fontW;
	if (d_maxCols > DST_MAX_COLS) d_maxCols = DST_MAX_COLS;
	d_maxRows = (height - d_topMargin) / d_fontH;
	if (d_maxRows > DST_MAX_ROWS) d_maxRows = DST_MAX_ROWS;

	SetBackgroundColor(0x30, 0x30, 0x30);
	SetForegroundColor(0xa0, 0xa0, 0xa0);

	Clear();
}

//============================================================================

void PS2DisplaySimpleText::Clear()
{
	memset(d_screen, 0, sizeof(d_screen));
	d_x = 0;
	d_y = 0;	
}

//============================================================================

void PS2DisplaySimpleText::SetBackgroundColor(int r, int g, int b)
{
	if (r < 0)   r = 0;
	if (r > 255) r = 255;
	if (g < 0)   g = 0;
	if (g > 255) g = 255;
	if (b < 0)   b = 0;
	if (b > 255) b = 255;
	d_backgroundColor = (0x80 << 24) | (r << 0) | (g << 8) | (b << 16);
}

//============================================================================

void PS2DisplaySimpleText::SetForegroundColor(int r, int g, int b)
{
	if (r < 0)   r = 0;
	if (r > 255) r = 255;
	if (g < 0)   g = 0;
	if (g > 255) g = 255;
	if (b < 0)   b = 0;
	if (b > 255) b = 255;
	d_foregroundColor = (0x80 << 24) | (r << 0) | (g << 8) | (b << 16);
}

//============================================================================

int PS2DisplaySimpleText::MaxRows() const
{
	return d_maxRows;
}

//============================================================================

int PS2DisplaySimpleText::MaxCols() const
{
	return d_maxCols;
}

//============================================================================

int PS2DisplaySimpleText::CurRow() const
{
	return d_y;
}

//============================================================================

int PS2DisplaySimpleText::CurCol() const
{
	return d_x;
}

//============================================================================

void PS2DisplaySimpleText::SetPosition(int x, int y)
{
	d_x = x;
	d_y = y;
	if (d_x < 0)          d_x = 0;
	if (d_x >= d_maxCols) d_x = d_maxCols - 1;
	if (d_y < 0)          d_y = 0;
	if (d_y >= d_maxRows) d_y = d_maxRows - 1;
}

//============================================================================

void PS2DisplaySimpleText::Printf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf((char*)d_buffer, fmt, args);
	va_end(args);

	const unsigned char* p;
	for (p = d_buffer; *p && p < &d_buffer[sizeof(d_buffer)]; p++)
	{
		bool newline = false;
		int c = *(unsigned char*)p;
		if (c >= 32 && c <= 126)
		{
			d_screen[d_y][d_x] = c;
			d_x++;
			if (d_x >= d_maxCols)
				newline = true;
		}
		else if (c == '\n')
		{
			newline = true;
		}

		if (newline)
		{
			d_x = 0;
			d_y++;
			if (d_y >= d_maxRows)
				d_y = d_maxRows - 1;
		}
	}
}

//============================================================================

void PS2DisplaySimpleText::Flip()
{
	// render the screen block per block.  speed isn't important here so
	// we can use a hacky approach to make this easier to write.
	unsigned int* block = (unsigned int*)(((unsigned int)d_buffer + 15) & ~15);
	const int blockWidth = 64;
	const int blockHeight = ((unsigned int)&d_buffer[DST_BUFFER] - (unsigned int)block) / (blockWidth * 4);

	int  fontWidth   = *(int*)&d_fontData[0];
	int  fontHeight  = *(int*)&d_fontData[4];
	int  fontColors  = *(int*)&d_fontData[8];
	int* fontPalette = (int*)&d_fontData[12];
	unsigned char* fontData = (unsigned char*)&fontPalette[fontColors];

	for (int by = 0; by < Height(); by += blockHeight)
	{
		for (int bx = 0; bx < Width(); bx += blockWidth)
		{
			for (int i = 0; i < blockWidth * blockHeight; i++)
				block[i] = d_backgroundColor;

			for (int y = ((by - d_topMargin) / d_fontH) - 1; y <= ((by - d_topMargin + blockHeight) / d_fontH) + 1; y++)
			{
				for (int x = ((bx - d_leftMargin) / d_fontW) - 1; x <= ((bx - d_leftMargin + blockWidth) / d_fontW) + 1; x++)
				{
					if (x >= 0 && x < d_maxCols && y >= 0 && y < d_maxRows)
					{
						int c = d_screen[y][x];
						if (c >= 33 && c <= 126)
						{
							const FontMetrics* m = &d_fontMetrics[c - 32];
							int letterX1 = d_leftMargin + (x * d_fontW) + m->cx;
							int letterY1 = d_topMargin +  (y * d_fontH) + m->cy;
							int letterW  = m->x2 - m->x1;
							int letterH  = m->y2 - m->y1;
							if ((letterX1 + letterW >= bx && letterX1 <= bx + blockWidth) &&
								(letterY1 + letterH >= by && letterY1 <= by + blockHeight))
							{
								for (int ly = 0; ly < letterH; ly++)
								{
									int lby = letterY1 + ly - by;
									if (lby >= 0 && lby < blockHeight)
									{
										for (int lx = 0; lx < letterW; lx++)
										{
											int lbx = letterX1 + lx - bx;
											if (lbx >= 0 && lbx < blockWidth)
											{
												if (fontData[(fontHeight - 1 - (ly + m->y1)) * fontWidth + (lx + m->x1)])
												{
													block[lby * blockWidth + lbx] = d_foregroundColor;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			Blit(block, blockWidth, blockHeight, bx, by, false);
		}
	}
	PS2DisplaySimple::Flip();
}

//============================================================================
