#include "Game/GamePCH.h"
#include "camera/camera.h"
#include "camera/director.h"
#define VTXBUFFER_NEED_INLINES
#include "Display/Common/VtxBuffer.h"

namespace ParticulateMgr
{

	// render this emitter at this point in time...
	void Emitter::Render(float absTime)
	{
		if(Count() == 0)
			return;

		if(!sb.IsInited())
		{
			sb.Init();
			u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS] = {
				VtxBuffer::VERT_TEX_DIM2
			};
			vb.Init(MAX_PARTICULATES_PER_SYSTEM*4, VtxBuffer::VERT_HAS_DIFFUSE, texFlags);
			sb.SetVtxBuffer(&vb);
			sb.SetPassTexture(0, mEffect->mUseTexture, TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR, RS_BLENDFUNC_DEFAULT);
		}

		u16 indices[] = { 0,1,2,3 };
		vb.Lock();
		sb.Reset();

		Particulate* part = &mSystem->mBuffer[mParticulates.mFirstIndex];
		for(int i = mParticulates.mIndexAfterLast - mParticulates.mFirstIndex, vtx = 0; i > 0; i--, part++)
		{
			sb.AddStrip(indices, 4);
			indices[0]+=4;
			indices[1]+=4;
			indices[2]+=4;
			indices[3]+=4;

			float time = absTime - part->mBirthTime;
			float lifepercent = time / (part->mDeathTime - part->mBirthTime);

			Vector4 color = mEffect->mColorEnvelope.GetColor(lifepercent);
			#ifdef PS2
			color /= 128.0f;
			#else
			color /= 255.0f;
			#endif
			u32 byteColor = VtxBuffer::cvtRGBA(
				(RGBF_TO_B(color.X()) << 0) |
				(RGBF_TO_B(color.Y()) << 8) |
				(RGBF_TO_B(color.Z()) << 16) |
				(RGBF_TO_B(color.W()) << 24));


			Vector3 position = 
				part->mInitialPosition +
				((part->mInitialVelocity +
				  mEffect->mAccelerationVector * time) /
				 (1 + mEffect->mDragCoefficient * time)) * time;

			float scale = part->mInitialScale * mEffect->mScaleEnvelope.GetValue(lifepercent);

			float angle = 
				part->mInitialAngularPosition +
				part->mInitialAngularVelocity * time +
				mEffect->mAngluarAcceleration * time * time;

			Vector3 offsets[4] = {
				Vector3(+scale, +scale, 0.0f),
				Vector3(+scale, -scale, 0.0f),
				Vector3(-scale, +scale, 0.0f),
				Vector3(-scale, -scale, 0.0f)
			};
         Vector3 worldoffsets[4];

			Graphics4x4 rotmtx;
			rotmtx.SetZRotation(angle);
			Camera& camera = g_cameraDirector.CurrentCamera();
			Graphics4x4 xformmtx = rotmtx * camera.ViewToWorld();
			xformmtx.BasisTransformArray(offsets, worldoffsets, 4);

			//byteColor = 0xFFFFFFFF;
			vb.TexCoordv2(0, vtx).Set(1.0f, 1.0f);
			vb.Diffuse(vtx) = byteColor;
			vb.Vertex(vtx++) = position + worldoffsets[0];
			vb.TexCoordv2(0, vtx).Set(1.0f, 0.0f);
			vb.Diffuse(vtx) = byteColor;
			vb.Vertex(vtx++) = position + worldoffsets[1];
			vb.TexCoordv2(0, vtx).Set(0.0f, 1.0f);
			vb.Diffuse(vtx) = byteColor;
			vb.Vertex(vtx++) = position + worldoffsets[2];
			vb.TexCoordv2(0, vtx).Set(0.0f, 0.0f);
			vb.Diffuse(vtx) = byteColor;
			vb.Vertex(vtx++) = position + worldoffsets[3];
		}

		vb.UnLock();
		Matrix4x4 ident;
		ident.Identity();
		RenderState__Request(RS_ZWRITE, RS_FALSE);
		mEffect->mUseTexture->Lock();
		sb.Render(ident, &ident, 0);
		mEffect->mUseTexture->Unlock();
		RenderState__Request(RS_ZWRITE, RS_TRUE);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void System::Render(float absTime)
	{
		int totalParticulates = 0;
		for(int i = 0; i < mEmitters.Count(); i++)
		{
			Emitter* emitter = mEmitters[i];
			emitter->Render(absTime);
			totalParticulates += emitter->Count();
		}
#ifdef TYLER
		if(totalParticulates != 0)
			dbgPrint("%f Particulates %d\n", absTime, totalParticulates);
#endif
	}

};

