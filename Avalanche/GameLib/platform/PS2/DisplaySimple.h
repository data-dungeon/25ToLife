//============================================================================
//= DisplaySimple.h - a simple interface for very basic ps2 display
//============================================================================

#ifndef _PS2_DISPLAY_SIMPLE
#define _PS2_DISPLAY_SIMPLE

#include <eeregs.h>

class PS2DisplaySimple
{
public:
	void Init(int width, int height, bool pal);
	void Flip();
	void ClearFrameBuffer(bool front);
	void Blit(void* data, int width, int height, int x, int y, bool front);
	int Width();
	int Height();

private:
	void PutDispEnv();
	int FrameBufferSize(int pixelMode, int width, int height);

	struct DISPENV 
	{
		tGS_PMODE		pmode;
		tGS_SMODE2		smode2;
		tGS_DISPFB1		dispfb1;
		tGS_DISPFB2		dispfb2;
		tGS_DISPLAY1	display1;
		tGS_DISPLAY2	display2;
		tGS_BGCOLOR		bgcolor;
	};

	int		d_frameBufferAddr[ 2 ];
	DISPENV	d_dispEnv[ 2 ];
	int		d_width;
	int		d_height;
	int		d_currentFrame;
	bool		d_pal;
};

#endif //_PS2_DISPLAY_SIMPLE
