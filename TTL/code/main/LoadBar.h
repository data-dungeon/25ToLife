//============================================================================
//=
//= LoadBar.h - handle loading bar updates
//=
//============================================================================

#ifndef _LOADBAR_H
#define _LOADBAR_H

class LoadBar
{
public:
	void Start();
	void Update();

private:
	void DrawColoredBlock( int x, int y, DrawUtility::COLOR color );
	void NewLine();

	#ifdef PS2
	void DMAImageToVRAM( u32 u32VRAMAddr, u32 u32Width, u32 u32Height, void *pImage, u32 u32Format );
	#endif //PS2

private:
	int						d_x;
	int						d_y;
	DrawUtility::COLOR	d_color;
};

extern LoadBar g_LoadBar;

#endif //_LOADBAR_H