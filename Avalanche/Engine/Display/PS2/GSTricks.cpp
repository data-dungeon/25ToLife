//
// GSTricks
// this is a class that is used to perform some of the low level GS manipulation
// used to do things like ZBuffer based fog, Depth of Field, etc..
//

/******************* includes ****************************/

#include "Display/DisplayPCH.h"

#include "Display/PS2/GSTricks.h"

#include <libgifpk.h>

#define OFFX (((4096-DisplayState.nScreenWidth)/2)<<4)
#define OFFY (((4096-DisplayState.nScreenHeight)/2)<<4)

#define MAX_MARKS 3

namespace GSTricks
{
	static u64 sRGBA;
	static u32 sMask;
	static u32 sZValue;

	static u32 sMemPage;
	static u32 sMemMarks[MAX_MARKS];
	static u32 sMemCurrMark;
	static u32 sMemHighWater;

	static bool sWePausedTextureMgr;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Prep/cleanup
	//
	/////////////////////////////////////////////////////////////////////////////////////////////

	void Prep(void)
	{
		if(MasterList::CurrentlyActive())
		{
			MasterList::AppendDMATAG(0);
			MasterList::Append(VIFInline::createNOP());
			MasterList::Append(VIFInline::createFLUSHA());
		}
		// these are defaults for any functions in this class.
		// if you change any of them in your function, you should
		// set them back afterwards...
		GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0x80));
		GSState__Set(SCE_GS_ZBUF_1, SCE_GS_SET_ZBUF_1(DisplayState.u32ZBufferAddr/32, DisplayState.u32ZPixelFormat, 1));
		GSState__Set(SCE_GS_TEST_1, SCE_GS_SET_TEST_1(false, 0, 0x00, 0, false, false, true, SCE_GS_ZALWAYS));
		GSState__Set(SCE_GS_XYOFFSET_1, SCE_GS_SET_XYOFFSET_1(OFFX, OFFY));
		GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR_1(
			0, DisplayState.u32FrameBufferWidth - 1, 
			0, DisplayState.u32FrameBufferHeight - 1));
		GSState__Set(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1( 0, 0, 0, 0, 0, 0, 0 ));
		GSState__Set(SCE_GS_TEXA, SCE_GS_SET_TEXA( 0x80, 0, 0x80 ));
		GSState__Set(SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0));
		GSState__Set(SCE_GS_DTHE, false);

		Color(0x80, 0x80, 0x80, 0x80);

		// clear variables..
		sMask = 0x00000000;
		sZValue = 0;

		sMemCurrMark = 0;
		sMemPage = 0x200;
		sMemHighWater = sMemPage;

		sWePausedTextureMgr = false;
	}

	void Cleanup(void)
	{
		GSState__Set(SCE_GS_ALPHA_1, GSState.ALPHA_1);
		GSState__Set(SCE_GS_COLCLAMP, GSState.COLCLAMP);
		GSState__Set(SCE_GS_DIMX, GSState.DIMX);
		GSState__Set(SCE_GS_DTHE, GSState.DTHE);
		GSState__Set(SCE_GS_FRAME_1, GSState.FRAME_1);
		GSState__Set(SCE_GS_PRMODECONT, GSState.PRMODECONT);
		GSState__Set(SCE_GS_SCISSOR_1, GSState.SCISSOR_1);
		GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);
		GSState__Set(SCE_GS_TEXA, GSState.TEXA);
		GSState__Set(SCE_GS_XYOFFSET_1, GSState.XYOFFSET_1);
		GSState__Set(SCE_GS_ZBUF_1, GSState.ZBUF_1);
		GSState__Send();

		if(sWePausedTextureMgr)
			TextureMgr::Resume();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	// GS mem management for tricks...
	//
	/////////////////////////////////////////////////////////////////////////////////////////////

	void PushMark()
	{
		sMemMarks[sMemCurrMark++] = sMemPage;

		ASSERT(sMemCurrMark <= MAX_MARKS);
	}

	void PopMark()
	{
		sMemPage = sMemMarks[--sMemCurrMark];

		ASSERT(sMemCurrMark >= 0);
	}

	u32 GetBuff(u32 pages)
	{
		if(!TextureMgr::IsPaused())
		{
			TextureMgr::Pause();
			sWePausedTextureMgr = true;
		}

		sMemPage -= pages;

		if(sMemPage < sMemHighWater)
		{
			sMemHighWater = sMemPage;
			ASSERTS((sMemHighWater * GS_MEM_BLOCKS_PER_PAGE) > GSMemManager::GetHighestBlockUsed(), "Not enough free texture memory for cool GS tricks..");
		}

		return sMemPage;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Behaviour control
	//
	/////////////////////////////////////////////////////////////////////////////////////////////
	
	void Mask(u32 mask)
	{
		sMask = mask;
	}

	void Color(u8 r, u8 g, u8 b, u8 a)
	{
		sRGBA = SCE_GS_SET_RGBAQ(r, g, b, a, 1.0f);
		GSState__Set(SCE_GS_RGBAQ, sRGBA);
	}

	void ZValue(u32 value)
	{
		sZValue = value;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Below here are the real workhorse routines... the others above are for controlling the 
	// behaviour of the ones below here...
	//
	/////////////////////////////////////////////////////////////////////////////////////////////

	void InvertBuffer(u32 srcAddr, u32 srcWidth, u32 srcHeight, u8 srcFormat)
	{
		switch(srcFormat)
		{
		case SCE_GS_PSMZ32:
			srcFormat = SCE_GS_PSMCT32;
			break;
		case SCE_GS_PSMZ24:
			srcFormat = SCE_GS_PSMCT32;
			break;
		case SCE_GS_PSMZ16S:
		case SCE_GS_PSMZ16:
			ASSERT(FALSE);
			break;
		default:
			break;
		}
			
		GSState__Set(SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(0x80, 0x80, 0x80, 0x80, 1.0f));
		GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 2, 2, 0x80));

		SolidPoly(srcAddr, srcWidth, srcHeight, srcFormat, true);

		GSState__Set(SCE_GS_TEXFLUSH, 0);
		GSState__Set(SCE_GS_RGBAQ, sRGBA);
		GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0x80));
	}

	void SolidPoly(u32 buffAddr, u32 width, u32 height, u8 format, bool alphaBlend)
	{
		GSState__Set(SCE_GS_TEXFLUSH, 0);
		GSState__Set(SCE_GS_FRAME_1, SCE_GS_SET_FRAME_1(
			buffAddr/32,
			width/64, 
			format, 
			sMask));
		GSState__Set(SCE_GS_PRIM, createPRIM(
			SCE_GS_PRIM_SPRITE, 
			false,	// iip
			false,	// tme
			false,	// fge
			alphaBlend,	// abe
			false,	// aa1
			true,		// fst
			false,	// ctxt
			false));// fix

		int numStrips = width / 32;

		LotsOfStrips(
			numStrips, 
			0, 0, 32, height, 0, 
			0, 0, 32, height, 0);
	}

	void ConvolveBuffer11(u32 buffAddr, u32 width, u32 height, u8 format)
	{
		// copy without the centering...
		CopyBuffer(buffAddr, width, height, format,
			buffAddr, width, height, format, false, 0, 0);
	}

	void ConvolveBuffer3(float *weights, u32 bufferAddr, u32 width, u32 height, u8 format)
	{
		float bilinearOffset = -2.0f + weights[1] / (weights[1] + weights[2]);
		float alpha = (weights[1] + weights[2]) / (weights[0] + weights[1] + weights[2]);

		bilinearOffset *= 16.0f;
		alpha *= (float)0x80;

		// turn on 50 % alpha blend...
		GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1( 0, 1, 2, 1, (int)alpha));

		// copy without the centering...
		CopyBuffer(bufferAddr, width, height, format,
			bufferAddr, width, height, format, true, (u32)bilinearOffset, 0);

		// copy without the centering...
		CopyBuffer(bufferAddr, width, height, format,
			bufferAddr, width, height, format, true, 0, (u32)bilinearOffset);

		GSState__Set(SCE_GS_TEXFLUSH, 0);
		// reset alpha
		GSState__Set(SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA_1(0, 1, 0, 1, 0x80));
	}


#define MAX_REPS 3
	u32 BlurBuffer(u32 buffAddr, u32 width, u32 height, u8 format, u8 repetitions, int modeDown, int modeUp, float strength)
	{
		ASSERT(repetitions <= MAX_REPS && repetitions > 0);
		if(repetitions > MAX_REPS || repetitions <= 0) return 0;

		PushMark();

		// calc amount of temp space we need.. we need temp space for one size lower per rep..
		u32 w = width;
		u32 h = height;
		u32 buffers[MAX_REPS+1];
		u32 bytes = 0;
		u32 f = format;
		u32 blocks;
		int i;

		buffers[0] = buffAddr;
		for(i = 0; i < repetitions; i++)
		{
			w >>= 1;
			h >>= 1;

			// how many bytes does that take (at 32 bit)
			bytes += w * h * 4;

			// how many GS pages is that
			int pages = (bytes+4095) / 4096;

			// rebuild bytes... :)
			bytes = pages * 4096;

			// remember where the buffer is
			buffers[i+1] = GetBuff(pages) * 32;

			if(i == 0) PushMark();
		}
		
		w = width;
		h = height;

		float weights[3];
		ASSERT(strength >= 0.0f && strength <= 1.0f);
		weights[1] = 1.0f - strength;
		weights[0] = strength * 0.5f;
		weights[2] = weights[0];

		for(i = 0; i < repetitions; i++)
		{
			switch(modeDown)
			{
			case CONVOLVE_121:
				// blur buffer i
				ConvolveBuffer3(weights, buffers[i], w, h, f);
				break;
			case CONVOLVE_11:
				// blur buffer i
				ConvolveBuffer11(buffers[i], w, h, f);
				break;
			default:
				// no convolution
				break;
			}

			// scale down
			CopyBuffer(buffers[i], w, h, f,
				buffers[i+1], w>>1, h>>1, SCE_GS_PSMCT32);

			w >>= 1;
			h >>= 1;
			f = SCE_GS_PSMCT32;
		}

		// now work the other direction...
		for(i = repetitions; i > 0; i--)
		{
			if(i == 1)
				f = format;

			switch(modeUp)
			{
			case CONVOLVE_121:
				// blur buffer i
				ConvolveBuffer3(weights, buffers[i], w, h, SCE_GS_PSMCT32);
				break;
			case CONVOLVE_11:
				// blur buffer i
				ConvolveBuffer11(buffers[i], w, h, SCE_GS_PSMCT32);
				break;
			default:
				// no convolution
				break;
			}

			if(i > 1)
			{
				// scale up
				CopyBuffer(buffers[i], w, h, SCE_GS_PSMCT32,
					buffers[i-1], w<<1, h<<1, f);
			}

			w <<= 1;
			h <<= 1;
		}
		PopMark();
		return buffers[1];
	}

	void RGToBA(u32 srcAddr, u32 srcWidth, u32 srcHeight, u8 srcFormat, u32 destAddr, u8 destFormat)
	{
		switch(destFormat)
		{
		case SCE_GS_PSMCT32:
			destFormat = SCE_GS_PSMCT16;
			break;
		default:
			ASSERTS(false, "RGToBA doesnt support this dest buffer format");
			return;
		}
      switch(srcFormat)
		{
		case SCE_GS_PSMCT32:
			srcFormat = SCE_GS_PSMCT16;
			break;
		case SCE_GS_PSMZ32:
		case SCE_GS_PSMZ24:
			srcFormat = SCE_GS_PSMZ16;
			break;
		default:
			ASSERTS(false, "RGToBA doesnt support this source buffer format");
			return;
		}


		GSState__Set(SCE_GS_TEXFLUSH, 0);
		GSState__Set(SCE_GS_FRAME_1, SCE_GS_SET_FRAME_1(
			destAddr/32, 
			srcWidth/64, 
			destFormat, 
			sMask));
		GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR_1(0, srcWidth - 1, 0, (srcHeight * 2) - 1));
		GSState__Set(SCE_GS_TEXA, SCE_GS_SET_TEXA( 0x00, 0, 0x80 ));
		GSState__Set(SCE_GS_TEX0_1, SCE_GS_SET_TEX0_1(
			srcAddr,
			srcWidth/64,
			srcFormat,
			10, 10, 1, SCE_GS_MODULATE,
			0, 0, 0, 0, 0));
		GSState__Set(SCE_GS_PRIM, createPRIM(
			SCE_GS_PRIM_SPRITE, 
			false,	// iip
			true,		// tme
			false,	// fge
			false,	// abe
			false,	// aa1
			true,		// fst
			false,	// ctxt
			false));// fix

		int numStrips = srcWidth / 8;

		LotsOfStrips(
			numStrips, 
			0, 0, 8, srcHeight*2, 8, 
			8, 0, 8, srcHeight*2, 8);

		GSState__Set(SCE_GS_TEXFLUSH, 0);
		GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR_1(
			0, DisplayState.u32FrameBufferWidth - 1, 
			0, DisplayState.u32FrameBufferHeight - 1));
		GSState__Set(SCE_GS_TEXA, SCE_GS_SET_TEXA( 0x80, 0, 0x80 ));
	}

	void CopyBuffer(u32 srcAddr, u32 srcWidth, u32 srcHeight, u8 srcFormat, u32 destAddr, u32 destWidth, u32 destHeight, u8 destFormat, bool alphaBlend, u32 offsetU, u32 offsetV)
	{
		ASSERT((destAddr & 31) == 0);
		GSState__Set(SCE_GS_TEXFLUSH, 0);
		GSState__Set(SCE_GS_FRAME_1, SCE_GS_SET_FRAME_1(
			destAddr/32, 
			(destWidth+63)/64, 
			destFormat, 
			sMask));
		GSState__Set(SCE_GS_TEX0_1, SCE_GS_SET_TEX0_1(
			srcAddr,
			(srcWidth+63)/64,
			srcFormat,
			10, 10, 1, SCE_GS_MODULATE,
			0, 0, 0, 0, 0));
		GSState__Set(SCE_GS_PRIM, createPRIM(
			SCE_GS_PRIM_SPRITE, 
			false,	// iip
			true,		// tme
			false,	// fge
			alphaBlend,	// abe
			false,	// aa1
			true,		// fst
			false,	// ctxt
			false));// fix
		
		int numStrips = (srcWidth+31) / 32;

		LotsOfStrips(
			numStrips, 
			0, 0, 32, srcHeight, 0, 
			0, 0, (destWidth+numStrips-1) / numStrips, destHeight, 0,
			offsetU, offsetV);
		GSState__Set(SCE_GS_TEXFLUSH, 0);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Helper routines
	// 
	/////////////////////////////////////////////////////////////////////////////////////////////

	void LotsOfStrips(int num, int srcX, int srcY, int srcWidth, int srcHeight, int srcGap, int destX, int destY, int destWidth, int destHeight, int destGap, u32 offsetU, u32 offsetV)
	{
		// put into GS precision...
		destWidth <<= 4;
		destHeight <<= 4;
		srcWidth <<= 4;
		srcHeight <<= 4;
		srcGap <<= 4;
		destGap <<= 4;
		srcX <<= 4;
		destX <<= 4;
		destY <<= 4;

		int srcXPos = srcX;
		int destXPos = destX;

		int cnt;
		for( cnt=0; cnt < num; cnt++ )
		{
			// source
			int u0 = srcXPos;
			int v0 = srcY;

			// center
			// shift by 0.5
			u0 += offsetU;
			v0 += offsetV;

			int u1 = u0 + srcWidth;
			int v1 = v0 + srcHeight;

			int x0 = OFFX + destXPos;
			int y0 = OFFY + destY;
			int x1 = x0 + destWidth;
			int y1 = y0 + destHeight;

			// draw sprite
			GSState__Set(SCE_GS_UV, SCE_GS_SET_UV(u0, v0));
			GSState__Set(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(x0, y0, sZValue));
			GSState__Set(SCE_GS_UV, SCE_GS_SET_UV(u1, v1));
			GSState__Set(SCE_GS_XYZ2, SCE_GS_SET_XYZ2(x1, y1, sZValue));

			srcXPos += srcWidth + srcGap;
			destXPos += destWidth + destGap;
		}
	}


}



