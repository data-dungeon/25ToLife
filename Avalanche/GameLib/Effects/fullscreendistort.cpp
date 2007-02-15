//
// Author: Tyler Colbert
// Purpose: perform full screen pixel manipulation to distort the current frame in various ways.
//

#include "Effects/EffectsPCH.h"


#include "fullscreendistort.h"
#include "Display/Common/RenderState.h"
#include "EngineHelper/timer.h"
#include "Game/Managers/TextureMgr/PlatformImage.h"

#if defined(PS2) || defined(GCN) || defined(DIRECTX)
#define SUPPORT_FEEDBACK
#endif

namespace FullScreenDistort
{
//	u32 copyHandle;

	// This internal class represents a single distortion (feedback or non, X or Y)
	class Distortion
	{
		float attack, sustain, decay;
		bool	loopedSustain;
		float invattack, invdecay;
		float age, totalAge;

		float strength;

		// area of screen to affect
		float centerx, centery, innerradius, outerradius;
		bool inverseArea;

		SigGen* generator;
		bool useOmega;
		bool infinite;

	public:
		Distortion(float _centerx, float _centery, float _innerradius, float _outerradius, bool inversearea, float attacktime, float sustaintime, float decaytime, bool useOmegaFlag, SigGen* signalgenerator)
			: centerx(_centerx), centery(_centery), innerradius(_innerradius), outerradius(1.0f / _outerradius), inverseArea(inversearea), attack(attacktime), sustain(sustaintime), decay(decaytime), useOmega(useOmegaFlag), generator(signalgenerator)
		{
			ASSERT(attack >= 0.0f && sustain >= 0.0f && decay >= 0.0f);
			ASSERT(signalgenerator);

			if(attack == 0.0f && sustain == 0.0f && decay == 0.0f) infinite = true;
			else infinite = false;

			loopedSustain = false;

			strength = 1.0f;
			age = 0.0f;
			totalAge = 0.0f;
		}

		~Distortion()
		{
			delete generator;
		}

		float Strength(void) { return strength; }

		// Set the sustain flag
		void SetLoopedSustain(bool loop)
			{ loopedSustain = loop; }
		bool GetLoopedSustain()
			{ return loopedSustain; }

		bool Update(float deltaT)
		{
			age += deltaT;
			totalAge += deltaT;

			if(infinite)
				return true;

			if(age > attack && attack > 0.0f)
			{
				age -= attack;
				attack = 0.0f;
			}
			else if(attack == 0.0f && sustain > 0.0f)
			{
				if (loopedSustain)
					sustain += deltaT;
				else if (age > sustain)
				{
					age = 0.0f;
					sustain = 0.0f;
				}
			}

			if(attack > 0.0f)
				strength = 1.0f - ((attack - age) / attack);
			else if(sustain > 0.0f)
				strength = 1.0f;
			else
			{
				strength = (decay - age) / decay;
				if(age > decay)
					return false;
			}

			return true;
		}

		float Evaluate(float pos, float omega, float x, float y)
		{
			float xdiff = Math::Abs(x - centerx);
			float ydiff = Math::Abs(y - centery);
			float xscale = 1.0f;
			float yscale = 1.0f;
			if(xdiff > innerradius)
			{
				xdiff -= innerradius;
				xscale = 1.0f - (xdiff  * outerradius);
				if(xscale < 0.0f)
					xscale = 0.0f;
			}
			if(ydiff > innerradius)
			{
				ydiff -= innerradius;
				yscale = 1.0f - (ydiff  * outerradius);
				if(yscale < 0.0f)
					yscale = 0.0f;
			}

			float scale = xscale * yscale;
			scale = Math::Clamp(scale, 0.0f, 1.0f);

			if(inverseArea)
				scale = 1.0f - scale;

			if(useOmega)
				return scale * strength * generator->Generate(totalAge, omega);
			else
				return scale * strength * generator->Generate(totalAge, pos*Math::TwoPi);
		}
	};


	// here are the global variables that we use to track this whole mess...
#ifdef SUPPORT_FEEDBACK
	static DynList<Distortion> distortions[2][2];
#else
	static DynList<Distortion> distortions[1][2];
#endif

	static StripBuffer* stripbuffer;
	static VtxBuffer* vtxbuffer;
	static int gridX;
	static int gridY;
	static Graphics4x4 identity;
	static float* omegas;
	static float feedbackOpacity;
	static void RenderSet(bool feedback, DynList<Distortion>* set);
	static bool inited;
	static u32 filterColor;
	static MODE mode;
	static bool useOSTimer;

	// init the system...
	void Init(int _gridX, int _gridY, float _feedbackOpacity, MODE _mode)
	{
		// did you call Init() twice without an intervening Shutdown()?
		ASSERT(!inited);

		// requirement
		ASSERT(_gridX >= 1 && _gridY >= 1);

		// sanity check (realistic values are like 20 & 15 or something...
		ASSERT(_gridX < 100 && _gridY < 100);

		gridX = _gridX;
		gridY = _gridY;
		feedbackOpacity = _feedbackOpacity;
		mode = _mode;
		useOSTimer = false;

		// we need an identity for rendering
		identity.Identity();

		// create a stripbuffer and vertex buffer big enough for our needs
		bool success;

		// the strip buffer
		stripbuffer = new StripBuffer();
		success = stripbuffer->Init();
		ASSERT(success == true);

		// flags for vtxbuffer
		u32 texFlags[VTX_BUFFER_TEXTURE_LAYERS];
		memset(texFlags, 0, sizeof(texFlags));
		texFlags[0] = VtxBuffer::VERT_TEX_DIM2;

		// now the vtxbuffer
		vtxbuffer = new VtxBuffer();
		success = vtxbuffer->Init(
			(gridX + 1) * (gridY + 1),
			VtxBuffer::VERT_HAS_DIFFUSE | VtxBuffer::VERT_WRITEONLY,
			texFlags);
		ASSERT(success == true);

		// bind the two together
		stripbuffer->SetVtxBuffer(vtxbuffer);

		// now add the strips to the strip buffer...
		// these are vertical strips.
		u16 *indices = new u16[(gridY + 1)*2];
		int base[2];
		base[1] = 0;
		int x, y;
		for(x = 0; x < gridX; x++)
		{
			base[0] = base[1];
			base[1] += gridY + 1;

			for(y = 0; y < gridY + 1; y++)
			{
				ASSERT(y*2+1 < (gridY+1)*2);
				indices[y*2] = base[0] + y;
				indices[y*2+1] = base[1] + y;
			}

			stripbuffer->AddStrip(indices, (gridY + 1) * 2);
		}
		delete[] indices;

		// initialize the parts of the vertices that are constant...
		vtxbuffer->Lock();
		for(x = 0; x < gridX + 1; x++)
		{
			for(y = 0; y < gridY + 1; y++)
			{
				Vector3 p;
				p.Set(x / (float)(gridX),
						1.0f - (y / (float)(gridY)),
						0.0f);
				int index = y + x * (gridY + 1);
				vtxbuffer->Vertex(index) = p;
#ifdef PS2
				vtxbuffer->Diffuse(index) = 0x80808080;
#else
				vtxbuffer->Diffuse(index) = 0xFFFFFFFF;
#endif
			}
		}

		// allocate and init the omegas array
		omegas = new float[(gridX+1)*(gridY+1)];
		for(x = 0; x < gridX + 1; x++)
		{
			for(y = 0 ; y < gridY + 1; y++)
			{
				omegas[y + x * gridY] = g_random.Unit() * Math::TwoPi;
			}
		}
		vtxbuffer->UnLock( );

		filterColor = 0xFFFFFFFF;

		// remember we're done
		inited = true;
	}

	void Shutdown(void)
	{
		// did you call Init() first?
		if(!inited) return;

		inited = false;

#ifdef SUPPORT_FEEDBACK
		distortions[true][X].Reset();
		distortions[true][Y].Reset();
#endif
		distortions[false][X].Reset();
		distortions[false][Y].Reset();

		if(stripbuffer) delete stripbuffer;
		stripbuffer = NULL;

		if(vtxbuffer) delete vtxbuffer;
		vtxbuffer = NULL;

		if(omegas) delete[] omegas;
		omegas = NULL;
	}

	void SetMode(MODE _mode)
	{
		mode = _mode;
	}

	void SetOpacity(float opacity)
	{
		feedbackOpacity = opacity;
	}

	void SetFilterColorPoint(float xpos, float ypos, float innerradius, float outerradius, u32 color)
	{
		float xdist, ydist;
		int x,y;
		union {
			u32 c;
			struct {
				u8 r, g, b, a;
			};
		} c1, c2;

		c2.c = color;

#ifndef PS2
		// no overbrighting on platforms except PS2...
		c2.r = Math::Max((short)c2.r << 1, 0xFF);
		c2.g = Math::Max((short)c2.g << 1, 0xFF);
		c2.b = Math::Max((short)c2.b << 1, 0xFF);
		c2.a = Math::Max((short)c2.a << 1, 0xFF);

#ifdef DIRECTX
		Math::Swap(c2.r, c2.b);
#endif //DIRECTX
#ifdef GCN
		Math::Swap( c2.r, c2.a);
		Math::Swap( c2.g, c2.b);
#endif //GCN

#endif //PS2

		//innerradius = 1.0f / innerradius;
		outerradius = 1.0f / outerradius;
		ASSERT(outerradius > 0.0f);

		// did you forget to Init()?
		ASSERT(inited);

		vtxbuffer->Lock( );
		for(x = 0, xdist = 0.0f; x < gridX + 1; x++, xdist += (1.0f / (gridX+1)))
		{
			float xdiff = Math::Abs(xdist - xpos);
			float xscale = 1.0f;
			if(xdiff > innerradius)
			{
				xdiff -= innerradius;
				xscale = 1.0f - (xdiff * outerradius);
				if(xscale < 0.0f)
					xscale = 0.0f;
			}
			for(y = 0, ydist = 0.0f; y < gridY + 1; y++, ydist += (1.0f / gridY))
			{
				float ydiff = Math::Abs(ydist - ypos);
				float yscale = 1.0f;
				if(ydiff > innerradius)
				{
					ydiff -= innerradius;
					yscale = 1.0f - (ydiff * outerradius);
					if(yscale < 0.0f)
						yscale = 0.0f;
				}
				int index = y + x * (gridY + 1);

				float scale = xscale * yscale;
				scale = Math::Clamp(scale, 0.0f, 1.0f);

				c1.c = vtxbuffer->Diffuse(index);

				c1.r = (u8)(c1.r + (c2.r - c1.r) * scale);
				c1.g = (u8)(c1.g + (c2.g - c1.g) * scale);
				c1.b = (u8)(c1.b + (c2.b - c1.b) * scale);
				c1.a = (u8)(c1.a + (c2.a - c1.a) * scale);

				vtxbuffer->Diffuse(index) = c1.c;
			}
		}
		vtxbuffer->UnLock( );

	}

	void SetFilterColor(u32 color)
	{
		int x,y;


		union {
			u32 c;
			struct {
				u8 r, g, b, a;
			};
		} c2;

		c2.c = color;

#ifndef PS2
		// no overbrighting on platforms except PS2...
		c2.r = Math::Min(((short)c2.r) << 1, 0xFF);
		c2.g = Math::Min(((short)c2.g) << 1, 0xFF);
		c2.b = Math::Min(((short)c2.b) << 1, 0xFF);
		c2.a = Math::Min(((short)c2.a) << 1, 0xFF);

		// reorder to match ps2...
#ifdef DIRECTX
  				Math::Swap(c2.r, c2.b);
#endif //DIRECTX
#ifdef GCN
		Math::Swap( c2.r, c2.a);
		Math::Swap( c2.g, c2.b);
#endif //GCN

#endif //PS2

		// did you forget to Init()?
		ASSERT(inited);

		vtxbuffer->Lock( );

		filterColor = color;
		for(x = 0; x < gridX + 1; x++)
		{
			for(y = 0; y < gridY + 1; y++)
			{
				int index = y + x * (gridY + 1);
				vtxbuffer->Diffuse(index) = c2.c;
			}
		}
		vtxbuffer->UnLock( );
	}

	u32 Add(DIMENSION dim, float centerx, float centery, float innerradius, float outerradius, bool inversearea, float attacktime, float sustaintime, float decaytime, bool useOmega, bool feedback, SigGen* generator)
	{
		Distortion* distortion = NULL;

		// did you forget to Init()?
		ASSERT(inited);

#ifndef SUPPORT_FEEDBACK
		if(!feedback)
#endif
		{
			distortion = new Distortion(centerx, centery, innerradius, outerradius, inversearea, attacktime, sustaintime, decaytime, useOmega, generator);
			distortions[(int)feedback][(int)dim].Add(distortion);
		}

		if(!feedback)
		{
			// setup for copying the back buffer
//			copyHandle = TextureMgr::RequireBackBufferCopy(1.0f, true);
//			ASSERT(copyHandle != 0);
		}

#ifdef SUPPORT_FEEDBACK
#if defined(GCN) || defined(DIRECTX)
		if(feedback)
		{
			// setup for copying the back buffer
//			copyHandle = TextureMgr::RequireFrontBufferCopy();
//			ASSERT(copyHandle != 0);
		}
#endif //GCN
#endif //SUPPORT_FEEDBACK

		return (u32)distortion;
	}

	// Turn on looped sustain
	void SetLoopedSustain(u32 handle, bool loop)
	{
#ifdef SUPPORT_FEEDBACK
		for(int feedback = 0; feedback <= 1; feedback++)
#else
		for(int feedback = 0; feedback <= 0; feedback++)
#endif
		{
			for(int dimension = X; dimension <= Y; dimension++)
			{
				for(int i = distortions[feedback][dimension].Count()-1; i>= 0; i--)
				{
					if((u32)distortions[feedback][dimension][i] == handle)
						distortions[feedback][dimension][i]->SetLoopedSustain(loop);
				}
			}
		}
	}

	// Turn on looped sustain
	bool GetLoopedSustain(u32 handle)
	{
#ifdef SUPPORT_FEEDBACK
		for(int feedback = 0; feedback <= 1; feedback++)
#else
		for(int feedback = 0; feedback <= 0; feedback++)
#endif
		{
			for(int dimension = X; dimension <= Y; dimension++)
			{
				for(int i = distortions[feedback][dimension].Count()-1; i>= 0; i--)
				{
					if((u32)distortions[feedback][dimension][i] == handle)
						return distortions[feedback][dimension][i]->GetLoopedSustain();
				}
			}
		}

		return false;
	}

	bool IsPlaying(u32 handle)
	{
#ifdef SUPPORT_FEEDBACK
		for(int feedback = 0; feedback <= 1; feedback++)
#else
		for(int feedback = 0; feedback <= 0; feedback++)
#endif
		{
			for(int dimension = X; dimension <= Y; dimension++)
			{
				for(int i = distortions[feedback][dimension].Count()-1; i>= 0; i--)
				{
					if((u32)distortions[feedback][dimension][i] == handle)
						return true;
				}
			}
		}

		return false;
	}

	void Remove(u32 handle)
	{
		// distortion handles are really pointers to the distortion to remove... we just search for it and if we find
		// it then we know its valid...
		int i;
		int feedback;
		int dimension;

#ifdef SUPPORT_FEEDBACK
		for(feedback = 0; feedback <= 1; feedback++)
#else
		for(feedback = 0; feedback <= 0; feedback++)
#endif
		{
			for(dimension = X; dimension <= Y; dimension++)
			{
				for(i = distortions[feedback][dimension].Count()-1; i>= 0; i--)
				{
					if((u32)distortions[feedback][dimension][i] == handle)
					{
						distortions[feedback][dimension].Delete(i);
						// it will only ever be in the list once, so just return when we find it.
						return;
					}
				}
			}
		}
	}

	void UseOSTimer( bool flag )
	{
		useOSTimer = flag;
	}

	static void RenderSet(bool feedback, DynList<Distortion>* set)
	{
		int x,y,i;
		float maxStrength = 0.0f;
		float distx, disty;

		if(set[X].Count() == 0 && set[Y].Count() == 0)
			return;

		float fUScale, fVScale;

#if defined(_XBOX) || defined(PS2)
		if( feedback )
			TextureMgr::GetFrontBufferTextureScaling( fUScale, fVScale );
		else
			TextureMgr::GetBackBufferTextureScaling( fUScale, fVScale );
#elif defined(DIRECTX_PC)
		fUScale = RenderToTexture::renderSurface->GetImage()->GetImageUScale();
		fVScale = RenderToTexture::renderSurface->GetImage()->GetImageVScale();
#else
		fUScale = 1.0f;
		fVScale = 1.0f;
#endif

		vtxbuffer->Lock( );

		// loop through the vertices and generate the tex coords for them.
		for(x = 0, distx = 0.0f; x < gridX + 1; x++, distx += (1.0f / (gridX+1)))
		{
			float basex = x / (float)(gridX);
			for(y = 0, disty = 0.0f; y < gridY + 1; y++, disty += (1.0f / (gridY+1)))
			{
				float basey = y / (float)(gridY);
				Vector2 p;
				p.Set(basex, basey);

				float omega = omegas[y + x * gridY];

				for(i = set[X].Count() - 1; i >= 0; i--)
				{
					p.x( p.x() + set[X][i]->Evaluate(basex, omega, distx, disty));
					maxStrength = Math::Max(maxStrength, set[X][i]->Strength());
				}
				for(i = set[Y].Count() - 1; i >= 0; i--)
				{
					p.y( p.y() + set[Y][i]->Evaluate(basey, omega, distx, disty));
					maxStrength = Math::Max(maxStrength, set[Y][i]->Strength());
				}

				p.x( Math::Clamp( p.x( ) * fUScale, 0.0f, fUScale) );
				p.y( Math::Clamp( p.y( ) * fVScale, 0.0f, fVScale) );

				int index = y + x * (gridY + 1);
				vtxbuffer->TexCoordv2(0, index) = p;
			}
		}

		vtxbuffer->UnLock( );

#ifdef PS2
		stripbuffer->Overbrighting(true);
//		GSState__Set(SCE_GS_COLCLAMP, 0);
#endif

#ifdef GCN
		RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( 1.0f));
#endif

		Graphics4x4 cameraToWorld;
		g4WorldToCamera.Invert(cameraToWorld);

		// invert source buffer
		if(mode == NEGATIVE && !feedback)
		{
			stripbuffer->SetPassTexture(0, TextureMgr::GetWhiteTexture(), TS_METHOD_MODULATE, 0, 0, TS_FILTER_BILINEAR, RS_BLENDFUNC_NEGATIVE);
			stripbuffer->Render(cameraToWorld, &identity);
		}


		// get the correct bind
		Texture* texture = NULL;
		bool ownedTexture = false;
		if(feedback)
		{
			texture = TextureMgr::GetFrontBuffer();
#ifdef GCN
			if( !TextureMgr::IsFrontBufferValid( ))
			{
				texture = NULL;
			}
#endif
			if(texture)
				texture->Lock();
		}
		else
		{
#ifdef GCN
			ASSERT( mode != GHOSTLY);	//if this assert hits, we need to implement GHOSTLY on the GCN
#endif
#if defined(PS2)  ||  defined(_XBOX)
			texture = TextureMgr::CopyBackBuffer(
				1.0f,
				((mode == GREYSCALE)?
					TextureMgr::COPY_GREYSCALE:
					TextureMgr::COPY_NORMAL),
				true);
			ownedTexture = true;
#else
#ifdef DIRECTX_PC
			if (RenderToTexture::isRenderingToTextureOn)
				texture = RenderToTexture::renderSurface;
			else
#endif
			texture = TextureMgr::GetBackBuffer();
			if (texture)
				texture->Lock();
#endif
//			texture = TextureMgr::GetBackBuffer();
//			texture = TextureMgr::GetWhiteTexture();
//			texture->Lock();
		}
#if defined(GCN) || defined(DIRECTX)
		if( !texture )
			return;
#else
		ASSERT_PTR(texture);
#endif


		// invert source buffer
		if(mode == EMBOSS && !feedback)
		{
			stripbuffer->SetPassTexture(0, TextureMgr::GetWhiteTexture(), TS_METHOD_MODULATE, 0, 0, TS_FILTER_BILINEAR, RS_BLENDFUNC_NEGATIVE);
			stripbuffer->Render(cameraToWorld, &identity);
		}

		// TJC - TODO: make this use vertex alpha as well as a fixed alpha value.. probably have to bake
		// the fixed alpha into the vertices... but thats not hard.
#if 0
		if(feedback)
		{
#ifndef GCN
			RenderState__Request(RS_FIXEDALPHA, RenderState__ftoi(feedbackOpacity * maxStrength));
			RenderState__Request(RS_TEXTURE_CONSTANTALPHA, RenderState__ftoi(feedbackOpacity * maxStrength));
			stripbuffer->SetPassTexture(0, texture, TS_METHOD_CONSTANTALPHA, 0, 0, TS_FILTER_BILINEAR, RS_BLENDFUNC_FIXEDALPHA);
#else
			RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( feedbackOpacity * maxStrength));
			stripbuffer->SetPassTexture(0, texture, TS_METHOD_MODULATE, 0, 0, TS_FILTER_BILINEAR, RS_BLENDFUNC_DEFAULT);
#endif //GCN
		}
		else
#else
		if(feedback)
		{
#ifdef PS2
			u8 opacity = (u8)(feedbackOpacity * maxStrength * 0x80);
			GSState__Set(SCE_GS_TEXA, SCE_GS_SET_TEXA(opacity, 0, opacity));
#endif //PS2
#ifdef GCN
			RenderState__Request( RS_OBJECT_CONSTANTALPHA, RenderState__ftoi( feedbackOpacity * maxStrength));
#else
			RenderState__Request(RS_FIXEDALPHA, RenderState__ftoi(feedbackOpacity * maxStrength));
			RenderState__Request(RS_TEXTURE_CONSTANTALPHA, RenderState__ftoi(feedbackOpacity * maxStrength));
#endif
			stripbuffer->SetPassTexture(0, texture, TS_METHOD_MODULATE, 0, 0, TS_FILTER_BILINEAR, RS_BLENDFUNC_DEFAULT);

		}
		else
#endif //0
		{

			u8 method = TS_METHOD_MODULATE;

#ifdef GCN
			if( mode == GHOSTLY)
			{
				method = TS_METHOD_INVERSE;
			}
#endif //GCN
			stripbuffer->SetPassTexture(0, texture, method, 0, 0, TS_FILTER_BILINEAR, RS_BLENDFUNC_DEFAULT);
		}
		stripbuffer->Render(cameraToWorld, &identity);


		if(texture)
		{
			texture->Unlock();
			if(ownedTexture)
				delete texture;
		}

		RenderState__Request(RS_BLENDFUNC, RS_BLENDFUNC_DEFAULT);
#ifdef PS2
		GSState__Set(SCE_GS_TEXA, SCE_GS_SET_TEXA(0x00, 0, 0x80));
//		GSState__Set(SCE_GS_COLCLAMP, 1);
#endif
	}

	void RenderAll(void)
	{
		if(!inited) return;

		// render it in the correct orthographic space
		MatrixStack__Mode(MS_PROJECTION);
		MatrixStack__Push();
		MatrixStack__Identity();

#if defined(GCN)
		MatrixStack__Orthographic( 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);
#else
		MatrixStack__Orthographic(0.0f, 1.0f, 0.0f, 1.0f, -0.01f, 1.0f);
#endif // GCN
		RenderState__Default();

		RenderState__Request(RS_ZWRITE, RS_FALSE);
		RenderState__Request(RS_ZTEST, RS_FALSE);
		RenderState__Request(RS_ALPHATEST, RS_FALSE);
		RenderState__Request(RS_FOG, RS_FALSE);
		RenderState__Request(RS_LIGHTING, RS_FALSE);

#ifdef PS2
		RenderState__Request(RS_VU_CLIPPING, RS_CLIPPING_NONE);
#endif

#ifdef SUPPORT_FEEDBACK
		RenderSet(true, &distortions[true][0]);
#endif
		RenderSet(false, &distortions[false][0]);

		MatrixStack__Mode(MS_PROJECTION);
		MatrixStack__Pop();
	}

	void UpdateAll(void)
	{
		if(!inited) return;

   	float deltaT = 0.0f;

		if ( g_timer.GetSpeed() != CMasterTimer::PAUSE )
			deltaT = (useOSTimer) ? g_timer.GetOSFrameSec() : g_timer.GetFrameSec();

		int i;
		for(i = distortions[false][X].Count() - 1; i >= 0; i--)
		{
			if(!distortions[false][X][i]->Update(deltaT))
			{
				// time to die...
				distortions[false][X].Delete(i);
			}
		}
		for(i = distortions[false][Y].Count() - 1; i >= 0; i--)
		{
			if(!distortions[false][Y][i]->Update(deltaT))
			{
				// time to die...
				distortions[false][Y].Delete(i);
			}
		}
#ifdef SUPPORT_FEEDBACK
		for(i = distortions[true][X].Count() - 1; i >= 0; i--)
		{
			if(!distortions[true][X][i]->Update(deltaT))
			{
				// time to die...
				distortions[true][X].Delete(i);
			}
		}
		for(i = distortions[true][Y].Count() - 1; i >= 0; i--)
		{
			if(!distortions[true][Y][i]->Update(deltaT))
			{
				// time to die...
				distortions[true][Y].Delete(i);
			}
		}
#endif
	}
};
