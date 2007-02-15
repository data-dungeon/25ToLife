#include "camera/camera.h"
#include "camera/director.h"

#include "Game/GamePCH.h"

// use the generic renderer?
#if 0

#include "ParticulateMgr_generic.cpp"

#else

// see microcode for reasons for these values.
#define PARTICULATE_VU_BASE 0x000
#define PARTICULATE_VU_INPUT_START 0x171
#define PARTICULATE_BUFFER_START 0x187
#define PARTICULATE_VU_OFFSET 0x2000
#define PARTICULATE_MAX_PER_BUFFER 39

extern unsigned int	Particulate_DmaTag		__attribute__( ( section( ".vudata" )));
extern unsigned int	Particulate_CodeStart	__attribute__( ( section( ".vudata" )));
extern unsigned int	Particulate_CodeEnd		__attribute__( ( section( ".vudata" )));


namespace ParticulateMgr
{
	static bool sIsFirstEmitter;

	static void RenderBunch(Effect* pEffect, Particulate* part, s32 count, bool firstBunch, float absTime)
	{
		ASSERT(count <= PARTICULATE_MAX_PER_BUFFER);

		MasterList::AppendDMATAG(23);
		if(firstBunch)
			MasterList::Append(VIFInline::createMSCALF((u_int)&Particulate_CodeStart));
		else
			MasterList::Append(VIFInline::createMSCNT());

		// temp solution to avoid screen corruption.  Appearently I have an overwrite somewhere
		// in the microcode... I knew it went together too easily.
		MasterList::Append(VIFInline::createFLUSH());
		MasterList::Append(VIFInline::createNOP());
		MasterList::Append(VIFInline::createNOP());
		MasterList::Append(VIFInline::createNOP());

		MasterList::Append(VIFInline::createUNPACK(PARTICULATE_VU_INPUT_START, V4_32, 22, true));
		MasterList::Append(GIFInline::createGIFTAG(6, 1, 0xE, false));

		PlatformImage* pimage = pEffect->mUseTexture->GetImage()->GetPlatformImage();
		u64 tex0, tex1, clamp, miptbp1, miptbp2;
		pimage->GetRegisterValues(&tex0, &tex1, &clamp, &miptbp1, &miptbp2);

		MasterList::Append(GIFInline::createAD(SCE_GS_TEX0_1, tex0));
		MasterList::Append(GIFInline::createAD(SCE_GS_TEX1_1, tex1));
		MasterList::Append(GIFInline::createAD(SCE_GS_ALPHA_1, 0x44));
		MasterList::Append(GIFInline::createAD(SCE_GS_CLAMP_1, clamp));
		MasterList::Append(GIFInline::createAD(SCE_GS_MIPTBP1_1, miptbp1));
		MasterList::Append(GIFInline::createAD(SCE_GS_MIPTBP2_1, miptbp2));

		MasterList::Append(
			GIFInline::createGIFTAG(count, 
											9, 
											0x525252512l, 
											true, 
											GIFTag::FLG_PACKED, 
											(PRIM_TRIANGLE_STRIP|0x58)));

		Camera& camera = g_cameraDirector.CurrentCamera();
		Matrix4x4 worldToCull = camera.WorldToView() * *(const Matrix4x4*)&PS2Renderer::GetCameraToCull();
		MasterList::AppendMtx(&worldToCull);
		Matrix4x4 cullToScreen = *(const Matrix4x4*)&PS2Renderer::GetCullToScreen();
		MasterList::Append4xF(cullToScreen.e00, cullToScreen.e01, cullToScreen.e02, DisplayState.u32FrameBufferWidth);
		MasterList::Append4xF(cullToScreen.e10, cullToScreen.e11, cullToScreen.e12, DisplayState.u32FrameBufferHeight);
		MasterList::Append4xF(cullToScreen.e20, cullToScreen.e21, cullToScreen.e22, pEffect->mDragCoefficient);
		MasterList::Append4xF(cullToScreen.e30, cullToScreen.e31, cullToScreen.e32, absTime);
		MasterList::Append(pEffect->mColorEnvelope.mSplinePoints[0].vec());
		MasterList::Append(pEffect->mColorEnvelope.mSplinePoints[1].vec());
		MasterList::Append(pEffect->mColorEnvelope.mSplinePoints[2].vec());
		MasterList::Append(pEffect->mColorEnvelope.mSplinePoints[3].vec());
		MasterList::Append4xF(pEffect->mScaleEnvelope.mPoints[2],
									 pEffect->mScaleEnvelope.mPoints[3],
									 pEffect->mScaleEnvelope.mPoints[1],
									 pEffect->mScaleEnvelope.mPoints[0]);
		MasterList::Append4xF(pEffect->mAccelerationVector.x(),
									 pEffect->mAccelerationVector.y(),
									 pEffect->mAccelerationVector.z(),
									 pEffect->mAngluarAcceleration);


		MasterList::AppendDMATAG(count*3);
		MasterList::Append(VIFInline::createNOP());
		MasterList::Append(VIFInline::createUNPACK(PARTICULATE_BUFFER_START, V4_32, count*3, true));

		MasterList::CopyToMasterList(part, count*3);
	}

	// render this emitter at this point in time...
	void Emitter::Render(float absTime)
	{
		int count = Count();
		if(count == 0)
			return;

		mEffect->mUseTexture->Lock();
		RenderState__Default();
		RenderState__Request(RS_ZWRITE, RS_FALSE);
		GSState__Send();


		for(int i = 0; i < count; i+=PARTICULATE_MAX_PER_BUFFER)
		{
			RenderBunch(mEffect, 
							&mSystem->mBuffer[mParticulates.mFirstIndex + i], 
							Math::Min(count-i,PARTICULATE_MAX_PER_BUFFER),
							sIsFirstEmitter, 
							absTime);
			sIsFirstEmitter = false;
		}

		mEffect->mUseTexture->Unlock();
		RenderState__Request(RS_ZWRITE, RS_TRUE);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void System::Render(float absTime)
	{
		int count = mEmitters.Count();
		if(count == 0)
			return;

		// send up the microcode
		MasterList::AppendDMATAG(0, TAGID_CALL, &Particulate_DmaTag);
		MasterList::Append(createBASE(PARTICULATE_VU_BASE));
		MasterList::Append(createOFFSET(PARTICULATE_VU_OFFSET));
		PS2Renderer::JustTrashedVUInstructionMem((u_int)&Particulate_CodeStart, (u_int)&Particulate_CodeEnd - (u_int)&Particulate_CodeStart);
		MasterList::SetBase(PARTICULATE_VU_BASE);
		MasterList::SetOffset(PARTICULATE_VU_OFFSET);

		sIsFirstEmitter = true;
      int totalParticulates = 0;
		for(int i = 0; i < count; i++)
		{
			Emitter* emitter = mEmitters[i];
			emitter->Render(absTime);
			totalParticulates += emitter->Count();
		}
		if(totalParticulates != 0)
		{
			MasterList::AppendDMATAG();
			MasterList::Append(VIFInline::createMSCNT());
//			MasterList::Append(VIFInline::createNOP());
			MasterList::Append(VIFInline::createFLUSH());
//			dbgPrint("%f Particulates %d\n", absTime, totalParticulates);
		}
	}

};


#endif

