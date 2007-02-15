#ifndef __PSDRENDERSTATE_H_INCLUDED__
#define __PSDRENDERSTATE_H_INCLUDED__

// TJC - have to do this because I need the defines on the ps2...
#ifdef PS2
#include <eetypes.h> // for u_int, u_long, and u_long128
#endif

/*-- vertex info. reg--*/
#define RSREGS_PRIM     	0x00
#define RSREGS_RGBAQ    	0x01
#define RSREGS_ST    		0x02
#define RSREGS_UV    		0x03
#define RSREGS_XYZF2    	0x04
#define RSREGS_XYZ2     	0x05
#define RSREGS_FOG      	0x0a
#define RSREGS_XYZF3    	0x0c
#define RSREGS_XYZ3     	0x0d
#define RSREGS_XYOFFSET_1  0x18
#define RSREGS_XYOFFSET_2  0x19
#define RSREGS_PRMODECONT  0x1a
#define RSREGS_A_D			0x0e
/*-- drawing attribute reg. --*/
#define RSREGS_PRMODE      0x1b
#define RSREGS_TEX0_1      0x06
#define RSREGS_TEX0_2      0x07
#define RSREGS_TEX1_1      0x14
#define RSREGS_TEX1_2      0x15
#define RSREGS_TEX2_1      0x16
#define RSREGS_TEX2_2      0x17
#define RSREGS_TEXCLUT     0x1c
#define RSREGS_SCANMSK     0x22
#define RSREGS_MIPTBP1_1   0x34
#define RSREGS_MIPTBP1_2   0x35
#define RSREGS_MIPTBP2_1   0x36
#define RSREGS_MIPTBP2_2   0x37
#define RSREGS_CLAMP_1     0x08
#define RSREGS_CLAMP_2     0x09
#define RSREGS_TEXA     	0x3b
#define RSREGS_FOGCOL      0x3d
#define RSREGS_TEXFLUSH    0x3f
/*-- pixel operation reg. --*/
#define RSREGS_SCISSOR_1   0x40
#define RSREGS_SCISSOR_2   0x41
#define RSREGS_ALPHA_1     0x42
#define RSREGS_ALPHA_2     0x43
#define RSREGS_DIMX     	0x44
#define RSREGS_DTHE     	0x45
#define RSREGS_COLCLAMP    0x46
#define RSREGS_TEST_1      0x47
#define RSREGS_TEST_2      0x48
#define RSREGS_PABE     	0x49
#define RSREGS_FBA_1    	0x4a
#define RSREGS_FBA_2    	0x4b
/*-- buffer reg. --*/
#define RSREGS_FRAME_1     0x4c
#define RSREGS_FRAME_2     0x4d
#define RSREGS_ZBUF_1      0x4e
#define RSREGS_ZBUF_2      0x4f
/*-- inter-buffer transfer reg. --*/
#define RSREGS_BITBLTBUF   0x50
#define RSREGS_TRXPOS      0x51
#define RSREGS_TRXREG      0x52
#define RSREGS_TRXDIR      0x53
#define RSREGS_HWREG    	0x54
/*-- other reg. --*/
#define RSREGS_SIGNAL      0x60
#define RSREGS_FINISH      0x61
#define RSREGS_LABEL    	0x62
#define RSREGS_NOP      	0x7f


// PRIM register bit settings
#define RS_PRIM_POINT 		0
#define RS_PRIM_LINE       1
#define RS_PRIM_LINESTRIP  2
#define RS_PRIM_TRI        3
#define RS_PRIM_TRISTRIP   4
#define RS_PRIM_TRIFAN     5
#define RS_PRIM_SPRITE     6
#define RS_PRIM_PROHIBITED 7

#define RS_PRIM_FLATSHADE	0
#define RS_PRIM_GOURAUD		1

#define RS_PRIM_STQ			0
#define RS_PRIM_UV			1

#define RS_PRIM_CONTEXT1	0
#define RS_PRIM_CONTEXT2	1

// TEX bit settings
#define RS_TEX_PSMCT32        0
#define RS_TEX_PSMCT24        1
#define RS_TEX_PSMCT16        2
#define RS_TEX_PSMCT16S       10
#define RS_TEX_PSMT8       	19
#define RS_TEX_PSMT4       	20
#define RS_TEX_PSMT8H         27
#define RS_TEX_PSMT4HL        36
#define RS_TEX_PSMT4HH        44
#define RS_TEX_PSMZ32         48
#define RS_TEX_PSMZ24         49
#define RS_TEX_PSMZ16         50
#define RS_TEX_PSMZ16S        58

#define RS_TEX_MODULATE       0
#define RS_TEX_DECAL          1
#define RS_TEX_HIGHLIGHT      2
#define RS_TEX_HIGHLIGHT2     3

#define RS_TEX_CLUT_CSM1 		0
#define RS_TEX_CLUT_CSM2      1

// CLAMP bit settings
#define RS_CLAMP_REPEAT				0
#define RS_CLAMP_CLAMP           1
#define RS_CLAMP_REGION_CLAMP    2
#define RS_CLAMP_REGION_REPEAT   3

#ifdef PS2

#ifdef NEED_GIFTAG_HERE
struct GIFTag
{
   unsigned long NLOOP:15;
   unsigned long EOP:1;
   unsigned long pad16:16;
   unsigned long id:14;
   unsigned long PRE:1;
   unsigned long PRIM:11;
   unsigned long FLG:2;
   unsigned long NREG:4;
	unsigned long REGSGLOB:64;
	inline void SetReg( int reg, int val )
	{
		REGSGLOB &= ~( ((u_long)0xf) << (reg * 4) );
		val &= 0xf;
		REGSGLOB |= ( ((u_long)val) << (reg * 4) );
	}
	inline int GetReg( int reg )
	{
		return ( (REGSGLOB >> (reg * 4)) & 0xf );
	}
};
#endif

class RSRegs
{


public:
	// 64 bit reg settings
	static inline u_long PRIM( u_int primtype, u_int shading, bool texmap, bool fog,
			bool alphablend, bool antialias, u_int texcoordmode, u_int context, bool fragment )
	{
		return ( (u_long)primtype | ((u_long)shading)<<3 | ((u_long)texmap)<<4 |
				((u_long)fog)<<5 | ((u_long)alphablend)<<6 | ((u_long)antialias)<<7 |
				((u_long)texcoordmode)<<8 | ((u_long)context)<<9 | ((u_long)fragment)<<10 );
	}
	static inline u_long FOG( unsigned char fogvalue )
	{
		return ( ((u_long)fogvalue)<<56);
	}
	static inline u_long TEX0( u_int texbase, u_int texbufwidth, u_int pixelformat,
			u_int texwidth, u_int texheight, bool texcolorcomponent, u_int function,
			u_int clutbase, u_int clutpixelformat, u_int clutstoragemode, u_int clutoffset,
			u_int clutcontrol )
	{
		u_long reg = 0;
		u_long expwidth = 0, expheight = 0;

		while( texwidth > 1 )
		{
			texwidth = texwidth >> 1;
			expwidth++;
		}
		while( texheight > 1 )
		{
			texheight = texheight >> 1;
			expheight++;
		}

		reg = ( ((u_long)texbase) | ((u_long)texbufwidth/64)<<14 | ((u_long)pixelformat)<<20 |
				((u_long)expwidth)<<26 | ((u_long)expheight)<<30 |
				((u_long)texcolorcomponent)<<34 | ((u_long)function)<<35 |
				((u_long)clutbase)<<37 | ((u_long)clutpixelformat)<<51 |
				((u_long)clutstoragemode)<<55 | ((u_long)clutoffset)<<56 |
				((u_long)clutcontrol)<<61 );

		return reg;
	}
	static inline u_long CLAMP( u_int uclamp, u_int vclamp, u_int minu, u_int maxu,
			u_int minv, u_int maxv )
	{
		return ( ((u_long)uclamp) | ((u_long)vclamp)<<2 | ((u_long)minu)<<4 |
				((u_long)maxu)<<14 | ((u_long)minv)<<24 | ((u_long)maxv)<<34 );
	}
};

#endif // PS2

#endif

