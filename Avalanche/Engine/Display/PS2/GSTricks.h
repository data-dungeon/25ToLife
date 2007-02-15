//
// GSTricks
// this is a class that is used to perform some of the low level GS manipulation
// used to do things like ZBuffer based fog, Depth of Field, etc..
//

#ifndef __GSTRICKS_H__
#define __GSTRICKS_H__

namespace GSTricks
{
	// prep/cleanup
	void Prep();
	void Cleanup();

	// behaviour control
	void Mask(u32 mask);
	void Color(u8 r, u8 g, u8 b, u8 a);
	void ZValue(u32 value);

	// blurring a buffer (note that strength is used to generate weights for convolve121 mode only)
	// the BlurBuffer method leaves a single buffer pushed on the mark stack which is 1/2 resolution
	// and contains the result of the blur.  The final resample up is skipped.  The block address of
	// the buffer is returned...  These all expect you to turn on the bilinear filter before calling them:
	//GSState__Set(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1( 0, 0, 1, 1, 0, 0, 0 ));
	// and turn it off when your done...
	//GSState__Set(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1( 0, 0, 0, 0, 0, 0, 0 ));

	static const int CONVOLVE_NONE = 0;
	static const int CONVOLVE_11   = 1;
	static const int CONVOLVE_121  = 2;
	u32 BlurBuffer(u32 buffAddr, u32 width, u32 height, u8 format, u8 repetitions, int modeDown, int modeUp, float strength = 0.5f);
	void ConvolveBuffer11(u32 buffAddr, u32 width, u32 height, u8 format);
	void ConvolveBuffer3(float *weights, u32 bufferAddr, u32 width, u32 height, u8 format);

	// copy/fill functions
	void RGToBA(u32 srcAddr, u32 srcWidth, u32 srcHeight, u8 srcFormat, u32 destAddr, u8 destFormat);
	void CopyBuffer(u32 srcAddr, u32 srcWidth, u32 srcHeight, u8 srcFormat, u32 destAddr, u32 destWidth, u32 destHeight, u8 destFormat, bool alphaBlend = false, u32 offsetU = 1 << 3, u32 offsetV = 1 << 3);
	void SolidPoly(u32 buffAddr, u32 width, u32 height, u8 format, bool alphaBlend = false);

	// inversion
	void InvertBuffer(u32 srcAddr, u32 srcWidth, u32 srcHeight, u8 srcFormat);

	// helpers
	void LotsOfStrips(int num, int srcX, int srcY, int srcWidth, int srcHeight, int srcGap, int destX, int destY, int destWidth, int destHeight, int destGap, u32 offsetU = 1 << 3, u32 offsetV = 1 << 3);

	// gs mem mangement
	void PushMark();
	void PopMark();
	u32 GetBuff(u32 pages);
};

#endif // __GSTRICKS_H__
