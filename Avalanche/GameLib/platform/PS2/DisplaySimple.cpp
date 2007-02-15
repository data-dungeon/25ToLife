//============================================================================
//= DisplaySimple.cpp - a simple interface for very basic ps2 display
//============================================================================

#include "platform/PlatformPCH.h"
#include "platform/PS2/DisplaySimple.h"

//============================================================================

bool InterruptsEnabled()
{
	unsigned long retval;
	asm __volatile__ (".set noreorder");
	asm __volatile__ ("sync.p");
	asm __volatile__ ("mfc0 %0, $12" : "=r" (retval));
	asm __volatile__ (".set reorder");
	return (retval & ((1<<16) | 1)) == ((1<<16) | 1);
}

//============================================================================

void PS2DisplaySimple::Init(int width, int height, bool pal)
{
	d_width  = width;
	d_height = height;
	d_pal    = pal;
	d_currentFrame = 0;

	sceGsResetPath();
//	sceGsResetGraph(0, SCE_GS_INTERLACE, pal ? SCE_GS_PAL : SCE_GS_NTSC, SCE_GS_FIELD);

	// frame buffer addresses
	d_frameBufferAddr[0] = 0;
	d_frameBufferAddr[1] = FrameBufferSize(SCE_GS_PSMCT32, d_width, d_height);

	// clear vram
	ClearFrameBuffer(false);
	ClearFrameBuffer(true);

	// set display params for each buffer
	for (int i = 0; i < 2; i++)
	{
		sceGsDispEnv dispEnv;
		sceGsSetDefDispEnv(&dispEnv, SCE_GS_PSMCT32, d_width, d_height, 0, 0);

		d_dispEnv[i].pmode    = dispEnv.pmode;
		d_dispEnv[i].smode2   = dispEnv.smode2;
		d_dispEnv[i].dispfb2  = dispEnv.dispfb;
		d_dispEnv[i].display2 = dispEnv.display;
		d_dispEnv[i].bgcolor  = dispEnv.bgcolor;

		memcpy(&d_dispEnv[i].dispfb1, &d_dispEnv[i].dispfb2, sizeof(tGS_DISPFB1));
		memcpy(&d_dispEnv[i].display1, &d_dispEnv[i].display2, sizeof(tGS_DISPLAY1));

		d_dispEnv[i].dispfb1.FBP = d_frameBufferAddr[i] / (8192/256);	// blocks to pages
		d_dispEnv[i].dispfb2.FBP = d_frameBufferAddr[i] / (8192/256);
		d_dispEnv[i].dispfb2.DBY = 0;
		#ifdef AA_ON
		d_dispEnv[i].dispfb2.DBY = 1;
		d_dispEnv[i].display2.DH -= 1;
		#endif

		d_dispEnv[i].pmode.MMOD = 1;
		d_dispEnv[i].pmode.EN1 = 1;
		d_dispEnv[i].pmode.EN2 = 1;
		d_dispEnv[i].pmode.SLBG = 0;
		d_dispEnv[i].pmode.ALP = 0x80;
	}

	asm("sync.l");

	if (InterruptsEnabled())
		FlushCache(0);
	else
		iFlushCache(0);

	sceGsSyncV(0);
	PutDispEnv();
}

//============================================================================

int PS2DisplaySimple::Width()
{
	return d_width;
}

//============================================================================

int PS2DisplaySimple::Height()
{
	return d_height;
}

//============================================================================

void PS2DisplaySimple::Flip()
{
//		d_dispEnv[d_currentFrame].dispfb1.DBY += 1;	// DEBUG, scroll vram
//		d_dispEnv[d_currentFrame].dispfb2.DBY += 1;

	d_currentFrame ^= 1;
	sceGsSyncV(0);
	PutDispEnv();
}

//============================================================================

void PS2DisplaySimple::PutDispEnv()
{
	DPUT_GS_PMODE(*(u_long *)&d_dispEnv[d_currentFrame].pmode);
	DPUT_GS_SMODE2(*(u_long *)&d_dispEnv[d_currentFrame].smode2);
	DPUT_GS_DISPFB1(*(u_long *)&d_dispEnv[d_currentFrame].dispfb1);
	DPUT_GS_DISPFB2(*(u_long *)&d_dispEnv[d_currentFrame].dispfb2);
	DPUT_GS_DISPLAY1(*(u_long *)&d_dispEnv[d_currentFrame].display1);
	DPUT_GS_DISPLAY2(*(u_long *)&d_dispEnv[d_currentFrame].display2);
	DPUT_GS_BGCOLOR(*(u_long *)&d_dispEnv[d_currentFrame].bgcolor);
}

//============================================================================

int PS2DisplaySimple::FrameBufferSize(int pixelMode, int width, int height)
{
	sceGsGParam* gp = sceGsGetGParam();
	int nw = (width + 63) / 64;
	int nh = (pixelMode & 2) ? ((height + 63) / 64) : ((height + 31) / 32);
	if ((gp->sceGsInterMode == SCE_GS_INTERLACE) && (gp->sceGsFFMode == SCE_GS_FIELD))
		return nw * nh * (8192/256);	// pages to blocks
	else
		return nw * nh * 2 * (8192/256);
}

//============================================================================

uint s_frameDataBuffer[ 32 * 32 ];

void PS2DisplaySimple::ClearFrameBuffer(bool front)
{
	for ( int i = 0 ; i < 32 * 32 ; i++ )
		s_frameDataBuffer[ i ] = 0x80000000;
	for (int y = 0; y < d_height; y += 32)
	{
		for (int x = 0; x < d_width; x += 32)
		{
			int w = d_width - x;		if (w > 32) w = 32;
			int h = d_height - y;	if (h > 32) h = 32;
			Blit(s_frameDataBuffer, w, h, x, y, front);
		}
	}
}

//============================================================================

void PS2DisplaySimple::Blit(void* data, int width, int height, int x, int y, bool front)
{
	int blocks = (width * height > 81920) ? 4 : 1;
	int blockHeight = height / blocks;
	int frameAddr = (front) ? d_frameBufferAddr[d_currentFrame] : d_frameBufferAddr[d_currentFrame ^ 1];
	for (int i = 0; i < blocks; i++)
	{
		sceGsLoadImage im;
		sceGsSetDefLoadImage(
			&im,
			frameAddr,
			d_width / 64,
			SCE_GS_PSMCT32,
			x,
			y + i * blockHeight,
			width,
			blockHeight
		);
		if (InterruptsEnabled())
			FlushCache( WRITEBACK_DCACHE );
		else
			iFlushCache( WRITEBACK_DCACHE );

		sceGsExecLoadImage(&im, (u_long128*) ((uint)data + i * width * blockHeight * 4));

		// interrupt friendly "sceGsSyncPath(0, 0)", doesn't sync all paths
//		while(DGET_D1_CHCR() & 0x0100);
//		while(DGET_D2_CHCR() & 0x0100);
//		while(DGET_VIF1_STAT() & 0x1f000003);
	}
}

//============================================================================
