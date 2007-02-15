//
// Author: Tyler Colbert
// Purpose: perform world space screen pixel manipulation to distort the current frame in various ways.
//

/*** pre-compiled headers through .h file (visual studio), can't be inside #if statement. ***/
/*** see .h file for more info.for codewarrior, pre-compiled header is handled through    ***/
/*** xcl compile/link interpreter, and doesn't need any explicit include                  ***/

#include "Effects/EffectsPCH.h"

// Define this if you want to allow backbuffer copies
//#define ALLOWBACKBUFFERCOPY


#include "worldspacedistort.h"
#include "Display/Common/RenderState.h"
#include "EngineHelper/timer.h"

namespace WorldSpaceDistort
{
 	u32 copyHandle;

	// This internal class represents a single distortion (feedback or non, X or Y)
	class Distortion : public InstancedDynamicObject
	{
		Vector3 pos;
		Vector3 normal;

		float attack, sustain, decay;
		float invattack, invdecay;
		float age;
		float realAge;
		bool autoDestruct;

		float strength;

		// area of circle
		int slices, rings;
		float aspectRatio, innerradius, outerradius;

		SigGen* generator;
		float signalScale;
		bool useOmega;
		bool infinite;

		// vtx buffer info..
		float* omegas;
		StripBuffer* stripBuffer;
		VtxBuffer* vtxBuffer;
		int numVerts;
		int numStrips;

		Vector4 insideColor;
		Vector4 midColor;
		Vector4 outerColor;

#if defined(GCN) || defined(DIRECTX)
		float		insideAlpha;
		float		midAlpha;
		float		outerAlpha;
#endif

		float spiralStrength;
		float spiralMaxAge;
		float spiralInitialAngle;
		float fixedSpiralAngle;

		float conicDepth;

		bool copyBackBuffer;

	public:
		Distortion(Vector3CRef _pos, Vector3CRef _normal,
						const DistortParams &_params,
						SigGen *generator,
						bool _copyBackBuffer, bool _autoDestruct);
		virtual ~Distortion();

		float Strength(void) { return strength; }
		bool Update(float deltaT, Graphics4x4* worldToCamera);
		void UpdateVerts(Graphics4x4* worldToCamera);
		void CreateStrips(void);
		void Render(Texture *texture, const Graphics4x4* worldToCamera);
		virtual void Render(const CInstance* pInstance, const Graphics4x4* pmLocalToWorld, float fZCamDist)
		{
			Texture *texture = NULL;
			int iZWrite = 0;

			if( true 
//#ifndef CONSUMER_BUILD
			&& !RenderMgr__WireframeOn()
//#endif
				)
			{
#if defined(ALLOWBACKBUFFERCOPY)
				if(copyBackBuffer)
					texture = TextureMgr::CopyBackBuffer(copyHandle);
				else
#endif
					texture = TextureMgr::GetBackBuffer();

				ASSERT_PTR(texture);
				if (!texture)
					return;

				//ts_bRGBA color;
				//gfxClearFrameBuffer(RS_DEPTH_BUFFER_BIT, &color);
				RenderState__Unlock( RS_ZWRITE);
				iZWrite = RenderState__Get( RS_ZWRITE);
				RenderState__Request(RS_ZWRITE, false);
				RenderState__Request(RS_ZTEST, true);
//				RenderState__Request(RS_ZFUNC, RS_ZFUNC_GEQUAL);
				RenderState__Request(RS_ALPHATEST, false);
			}
			else
			{
				texture = TextureMgr::GetWhiteTexture();
			}

#ifdef PS2
			RenderState__Request(RS_VU_CLIPPING, RS_CLIPPING_CULL);
#endif

#ifdef GCN
			RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( 1.0f));
#endif

			texture->Lock();
			Render(texture, pmLocalToWorld);
			texture->Unlock();

			RenderState__Request(RS_ZWRITE, iZWrite);
			RenderState__Request(RS_ZTEST, true);
			RenderState__Request(RS_ALPHATEST, true);
			RenderState__Request(RS_BLENDFUNC, RS_BLENDFUNC_DEFAULT);
#ifdef PS2
			GSState__Set(SCE_GS_TEXA, SCE_GS_SET_TEXA(0x00, 0, 0x80));
	//		GSState__Set(SCE_GS_COLCLAMP, 1);
#endif
		}

		void Move(Vector3CRef _pos, Vector3CRef _normal)
		{
			pos = _pos;
			normal = _normal;
		}
	};

	// here are the global variables that we use to track this whole mess...
	static DynList<Distortion> distortions;

	static Graphics4x4 identity;
	static bool inited;

	// init the system...
	void Init()
	{
		// did you call Init() twice without an intervening Shutdown()?
		ASSERT(!inited);

		// we need an identity for rendering
		identity.Identity();

		// remember we're done
		inited = true;
	}

	void Shutdown(void)
	{
		// did you call Init() first?
		if(!inited) return;

		inited = false;

		distortions.Reset();
	}

	u32 AddCircle(	Vector3CRef pos,
						Vector3CRef normal,
						const DistortParams &params,
						SigGen* generator,
						bool copyBackBuffer,
						bool autoDestruct)
	{
		Distortion* distortion = NULL;

		// did you forget to Init()?
		ASSERT(inited);

		distortion = new Distortion(pos, normal, params, generator, copyBackBuffer, autoDestruct);
		distortions.Add(distortion);

		// setup for copying the back buffer
#if defined(ALLOWBACKBUFFERCOPY)
		if(copyBackBuffer)
			copyHandle = TextureMgr::RequireBackBufferCopy(1.0f, false);

		// possibly out of texmem?
		ASSERT(copyHandle != 0);
#endif

		return (u32)distortion;
	}

	void Remove(u32 handle)
	{
		// distortion handles are really pointers to the distortion to remove... we just search for it and if we find
		// it then we know its valid...
		int i;

		for(i = distortions.Count()-1; i>= 0; i--)
		{
			if((u32)distortions[i] == handle)
			{
				distortions.Delete(i);
				// it will only ever be in the list once, so just return when we find it.
				return;
			}
		}
	}

	void UpdateAll(Graphics4x4* worldToCamera)
	{
		if(!inited || distortions.Count() == 0) return;

		float deltaT = g_timer.GetFrameSec( );

		int i;
		for(i = distortions.Count() - 1; i >= 0; i--)
		{
			if(!distortions[i]->Update(deltaT, worldToCamera))
			{
				// time to die...
				distortions.Delete(i);
			}
		}
	}

	Distortion* HandleToDistortion(u32 handle)
	{
		// flat conversion, but it's only valid if it's in the list.
		Distortion* d = (Distortion*) handle;
		for (int i = distortions.Count() - 1; i >= 0; i--)
			if (distortions[i] == d)
				return d;
		return NULL;
	}

	void MoveDistortion(u32 handle,
								Vector3CRef pos,
								Vector3CRef normal)
	{
		if(!inited) return;
		Distortion* distortion = HandleToDistortion(handle);
		ASSERT_PTR(distortion);
		if (distortion)
		{
			distortion->Move(pos, normal);
		}
	}


	// This internal class represents a single distortion (feedback or non, X or Y)

	Distortion::Distortion(Vector3CRef _pos, Vector3CRef _normal, const DistortParams &_params, SigGen* _signalgenerator, bool _copyBackBuffer, bool _autoDestruct)
		:	InstancedDynamicObject("WSDistort"),
			pos(_pos),
			normal(_normal),
			slices(_params.slices),
			rings(_params.rings),
			aspectRatio(_params.aspectRatio),
			innerradius(_params.innerradius),
			outerradius(_params.outerradius),
			attack(_params.attacktime),
			sustain(_params.sustaintime),
			decay(_params.decaytime),
			useOmega(_params.useOmega),
			generator(_signalgenerator),
			signalScale(_params.signalScale),
			insideColor(_params.insideColor),
			midColor(_params.midColor),
			outerColor(_params.outerColor),
			spiralStrength(_params.spiralStrength),
			spiralMaxAge(_params.spiralMaxAge),
			spiralInitialAngle(_params.spiralInitialAngle),
			copyBackBuffer(_copyBackBuffer),
			fixedSpiralAngle(_params.fixedSpiralAngle),
			conicDepth(_params.conicDepth),
			autoDestruct(_autoDestruct)
	{
		ASSERT(attack >= 0.0f && sustain >= 0.0f && decay >= 0.0f);
		ASSERT(_signalgenerator);

#if defined(GCN) || defined(DIRECTX)
		insideAlpha = insideColor.W( );
		midAlpha = midColor.W( );
		outerAlpha = outerColor.W( );
#endif
		if(attack == 0.0f && sustain == 0.0f && decay == 0.0f) infinite = true;
		else infinite = false;

		strength = 1.0f;
		age = 0.0f;
		realAge = 0.0f;

		numVerts = slices * (rings + 1);

		// the strip buffer
		stripBuffer = new StripBuffer();
		bool success = stripBuffer->Init();
		ASSERT(success == true);

		// flags for vtxbuffer
		u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS];
		memset(texFlags, 0, sizeof(texFlags));
		texFlags[0] = VtxBuffer::VERT_TEX_DIM2;

		vtxBuffer = new VtxBuffer();
		success = vtxBuffer->Init(
			numVerts,
			VtxBuffer::VERT_HAS_DIFFUSE | VtxBuffer::VERT_WRITEONLY,
			texFlags);
		ASSERT(success == true);

		// bind the two together
		stripBuffer->SetVtxBuffer(vtxBuffer);

#ifdef PS2
		stripBuffer->Overbrighting(true);
#endif

		if(useOmega)
		{
			// allocate and init the omegas array
			omegas = new float[numVerts];
			for(int x = 0; x < numVerts; x++)
			{
				omegas[x] = g_random.Unit() * Math::TwoPi;
			}
		}
		else
			omegas = NULL;

		CreateStrips();
	}

	Distortion::~Distortion()
	{
		delete generator;

		if(stripBuffer) delete stripBuffer;
		stripBuffer = NULL;

		if(vtxBuffer) delete vtxBuffer;
		vtxBuffer = NULL;

		if(omegas) delete[] omegas;
		omegas = NULL;
	}

	bool Distortion::Update(float deltaT, Graphics4x4* worldToCamera)
	{
		age += deltaT;
		realAge += deltaT;

		if(infinite)
		{
			UpdateVerts(worldToCamera);
			return true;
		}

		if(age > attack && attack > 0.0f)
		{
			age -= attack;
			attack = 0.0f;
		}
		else if(attack == 0.0f && age > sustain && sustain > 0.0f)
		{
			age -= sustain;
			sustain = 0.0f;
		}

		if(attack > 0.0f)
			strength = 1.0f - ((attack - age) / attack);
		else if(sustain > 0.0f)
			strength = 1.0f;
		else
		{
			strength = (decay - age) / decay;
			if(age > decay)
				return !autoDestruct;
		}

		UpdateVerts(worldToCamera);
		return true;
	}

	void Distortion::UpdateVerts(Graphics4x4* worldToCamera)
	{
		static const Vector4 fullon(255.0f, 255.0f, 255.0f, 255.0f);
		static const Vector4 zeros(0.0f, 0.0f, 0.0f, 0.0f);

		float fUScale;
		float fVScale;

#if defined(DIRECTX) || defined(PS2)
		TextureMgr::GetBackBufferTextureScaling( fUScale, fVScale );
#elif defined(DIRECTX_PC)
		fUScale = RenderToTexture::renderSurface->GetImage()->GetImageUScale();
		fVScale = RenderToTexture::renderSurface->GetImage()->GetImageVScale();
#else
		fUScale = 1.0f;
		fVScale = 1.0f;
#endif

		vtxBuffer->Lock( );

		float distPerRing = outerradius / rings;
		float distance = 0.0f;

		Matrix4x4 worldToScreen = *worldToCamera * *MatrixStack__Get(MS_PROJECTION);

		// loop through the vertices and generate the values for them.
		for(int r = 0; r < rings+1; r++)
		{
			float scale;
			Vector4 color;
#if defined(GCN) || defined(DIRECTX)
			float alphaHack;
#endif

			if(distance < innerradius)
			{
				scale = 1.0f;
				color = (midColor - insideColor) * (distance / innerradius) + insideColor;
#if defined(GCN) || defined(DIRECTX)
				alphaHack = (midAlpha - insideAlpha) * (distance / innerradius) + insideAlpha;
#endif
			}
			else
			{
				scale = ((distance - innerradius) / (outerradius - innerradius));
				// just in case
				scale = Math::Clamp(scale, 0.0f, 1.0f);
				color = (outerColor - midColor) * scale + midColor;
#if defined(GCN) || defined(DIRECTX)
				alphaHack = (outerAlpha - midAlpha) * scale + midAlpha;
#endif
				scale = 1.0f - scale;
			}
			color *= 128.0f;
#if defined(GCN) || defined(DIRECTX)
			alphaHack *= 128.0f;
#endif
			Vector4 white(128.0f, 128.0f, 128.0f, 0.0f);
			Vector4 postcolor = (color - white) * strength + white;
#if defined(GCN) || defined(DIRECTX)
			float postAlpha = ( alphaHack - 0.0f) * strength + 0.0f;
#endif
			color = Vector4::Min(Vector4::Max(postcolor, zeros), fullon);
#if defined(GCN) || defined(DIRECTX)
			alphaHack = Math::Min( Math::Max( postAlpha, 0.0f), 128.0f);
#endif
#ifdef DIRECTX
			u32 rgba = D3DCOLOR_RGBA(
					(unsigned char) _RenderState__Clampf( color.X( ) * 2.0f, 0.0f, 255.0f ),
					(unsigned char) _RenderState__Clampf( color.Y( ) * 2.0f, 0.0f, 255.0f ),
					(unsigned char) _RenderState__Clampf( color.Z( ) * 2.0f, 0.0f, 255.0f ),
					(unsigned char) _RenderState__Clampf(  alphaHack * 2.0f, 0.0f, 255.0f ) );
#elif defined(GCN)
			u32 rgba =
				( ( (int)Math::Clamp( color.X( ) * 2.0f,   0.0f, 255.0f)) << 24) |
				( ( (int)Math::Clamp( color.Y( ) * 2.0f,   0.0f, 255.0f)) << 16) |
				( ( (int)Math::Clamp( color.Z( ) * 2.0f,   0.0f, 255.0f)) <<  8) |
				( ( (int)Math::Clamp(  alphaHack * 2.0f,   0.0f, 255.0f)) <<  0);
#else
			u32 rgba =
				(((int)color.X()) << 0) |
				(((int)color.Y()) << 8) |
				(((int)color.Z()) << 16) |
				(((int)color.W()) << 24);
#endif
//#ifndef CONSUMER_BUILD
			if(RenderMgr__WireframeOn())
			{
				rgba = 0x80000080;
			}
//#endif // CONSUMER_BUILD

			float spiralAge;
			if(spiralMaxAge == 0.0f)
				spiralAge = Math::Sin(spiralInitialAngle);
			else
				spiralAge = Math::Sin(spiralInitialAngle + realAge / spiralMaxAge);
			float spiralScale = 1.0f - (distance / outerradius);
			spiralScale = spiralScale * spiralScale;
			float spiralStr = spiralStrength * strength * strength;
			float spinAngle = spiralStr * spiralAge * Math::TwoPi * spiralScale;
			Graphics4x4 spinMatrix;
			spinMatrix.SetZRotation(spinAngle + fixedSpiralAngle * spiralScale * Math::TwoPi);

			Vector3 coneoffset;
			coneoffset = -normal * conicDepth * (1.0f - (distance / outerradius));

			for(int s = 0; s < slices; s++)
			{
				float angle = (Math::TwoPi / slices) * s;

				int vtx = s + (r * slices);
				int omega = s + (r * slices);
				float om;

				if(!useOmega)
					om = distance;
				else
					om = omegas[omega];

				// face the plane towards the normal.
				float sin, cos;
				Math::SinCos(angle, sin, cos);
				Vector3 surfaceVector = Vector3(sin, cos, 0.0f) * distance;
				Vector3 right = Vector3(0.0f, 1.0f, 0.0f).Cross(normal);
				right.SafeNormalize();
				Vector3 up = normal.Cross(right);
				up.SafeNormalize();
				Graphics4x4 rotMat;
				rotMat.Identity();
				rotMat.SetRow0(right);
				rotMat.SetRow1(up);
				rotMat.SetRow2(normal);
				rotMat.CatYScale(aspectRatio);

				// build position taking spin into account.
				Vector3 position;
				if(r != rings)
				{
					position = pos + ((surfaceVector * spinMatrix) * rotMat) + coneoffset;
				}
				else
				{
					position = pos + (surfaceVector * rotMat) + coneoffset;
				}


				vtxBuffer->Vertex(vtx) = position +
					normal * scale * strength * generator->Generate(realAge, om * signalScale);

				vtxBuffer->Diffuse(vtx) = rgba;

				// rebuild position without spin
				position = pos + surfaceVector * rotMat;

				// Transform the raw vertex to the screen and see where it
				// falls.. use that for the texture coordinate.
  				Vector4 result = position.Transform3(worldToScreen);
				result *= 0.5f / result.W();
				result += Vector4(0.5f, 0.5f, 0.5f, 0.5f);

#ifdef PS2
				// TJC - when I originally wrote this, the PS2 did not Clamp() here...
				// anyone care to comment on why it does now?
  				result.x( Math::Clamp(result.x() * fUScale, 0.0f, fUScale) );
  				result.y( Math::Clamp(result.y() * fVScale, 0.0f, fVScale) );
#elif defined(DIRECTX)
				result.x( result.x( ) * fUScale );
				result.y( (1.0f - result.y( )) * fVScale );
#else
				// ps2 clamps to the correct range in hardware.  we need to clamp here
				// for the other platforms.
				result.x( Math::Clamp(result.x(), 0.0f, 1.0f));
				result.y( Math::Clamp(result.y(), 0.0f, 1.0f));
#endif
				vtxBuffer->TexCoordv2(0, vtx) = Vector2(result.X(), result.Y());
			}
			distance += distPerRing;
		}

		vtxBuffer->UnLock( );
	}

	void Distortion::Render(Texture *texture, const Graphics4x4 * pmLocalToWorld )
	{
		stripBuffer->SetPassTexture(0, texture, TS_METHOD_MODULATE, 0, 0, TS_FILTER_BILINEAR, RS_BLENDFUNC_DEFAULT);
		stripBuffer->Render(identity, &identity);
	}

	void Distortion::CreateStrips(void)
	{
		u16 *indices = new u16[slices*2 + 2];
		for(int i = 0; i < rings; i++)
		{
			int l = 0;
			for(int j = 0; j < slices; j++)
			{
				indices[l++] = j + (slices * (i+0));
				indices[l++] = j + (slices * (i+1));
			}
			// connect the circles.
			indices[l++] = 0 + (slices * (i+0));
			indices[l++] = 0 + (slices * (i+1));
			stripBuffer->AddStrip(indices, l);
		}
		delete[] indices;
	}
};
