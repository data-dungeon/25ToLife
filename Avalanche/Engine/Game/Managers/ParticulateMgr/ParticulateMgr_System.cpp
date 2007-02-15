#include "Game/GamePCH.h"

//#define showRejectionReason(x) dbgPrint x
#define showRejectionReason(x)

#define RESERVE_SPACE_FOR_EMITTERS_FROM_PARTICLE_MGR 64

namespace ParticulateMgr
{

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	//// System:: routines
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////

	System::System() :
		mEmitters(128, 64),
		mOwnedEmitters(RESERVE_SPACE_FOR_EMITTERS_FROM_PARTICLE_MGR, 0),
		mOwnedEffects(RESERVE_SPACE_FOR_EMITTERS_FROM_PARTICLE_MGR, 0)
	{
		sSystems.AddHead(this);
		mBuffer = (Particulate*)memAlloc(sizeof(Particulate)*MAX_PARTICULATES_PER_SYSTEM, 128);
		ASSERT(mBuffer);

		Reset();
	}

	System::~System()
	{
		Reset();
		memFree(mBuffer);
		sSystems.Remove(this);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void System::Reset()
	{
		DeleteSelfDeletingEmitters();

		// you didnt delete all the emitters first!!!!!!!!
		ASSERT(mEmitters.Count() == 0);
		mEmitters.Reset();

// must be commented out with auto-grow set to 0.
//		mOwnedEmitters.Clear();
//		mOwnedEffects.Clear();

		mSortedEmitters.UnlinkAll();
		mTotalParticulatesSpawned = 0;
		mTotalParticulatesDied = 0;
	}

   ////////////////////////////////////////////////////////////////////////////////////////////////

	void System::HandleDeletingEmitter(Emitter* emitter, int i)
	{
		if(emitter->next != NULL || emitter->prev != NULL || mSortedEmitters.Head() == emitter)
			mSortedEmitters.Unlink(emitter);

		mEmitters.Delete(i);

		if(emitter->mFlags & Emitter::EMITTER_DELETES_ON_DEATH)
			delete emitter;
		else if(emitter->mFlags & Emitter::EMITTER_BELONGS_TO_SYSTEM)
			mOwnedEmitters.Delete(emitter);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void System::DeleteSelfDeletingEmitters()
	{
		while(mEmitters.Count() > 0)
		{
			HandleDeletingEmitter(mEmitters[0], 0);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	ParticulateSet System::GetLargestGap()
	{
		ParticulateSet set;
		set.mIndexAfterLast = MAX_PARTICULATES_PER_SYSTEM;
	
		Emitter* emitter = mSortedEmitters.Head();
		if(!emitter)
		{
			set.mFirstIndex = 0;
			return set;
		}
	
		set.mFirstIndex = mSortedEmitters.Tail()->mParticulates.mIndexAfterLast;
		int bestgap = set.GetSize();
		ParticulateSet currset;
		currset.mFirstIndex = 0;
		while(emitter)
		{
			currset.mIndexAfterLast = emitter->mParticulates.mFirstIndex;
			if(currset.GetSize() > bestgap)
			{
				bestgap = currset.GetSize();
				set = currset;
			}
			currset.mFirstIndex = emitter->mParticulates.mIndexAfterLast;
			emitter = emitter->next;
		}
		return set;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	// this is just to kill old emitters, and spawn/kill particulates.  Nothing more.
	void System::Update(float absTime)
	{
		for(int i = 0; i < mEmitters.Count(); i++)
		{
			Emitter* emitter = mEmitters[i];

			// ingore emitters that cant emit yet.
			if(emitter->mBirthTime > absTime)
				continue;

			bool emitterDead = false;
			if(emitter->mDeathTime < emitter->mBirthTime)
			{
				if(emitter->mDeathTime >= emitter->mBirthTime)
				{
					emitterDead = true;
					emitter->mDeathTime = absTime;
				}
				else
					emitter->mDeathTime += 1.0f;
			}
			else if(emitter->mDeathTime <= absTime && emitter->mDeathTime != 0.0f)
			{
				emitterDead = true;
			}

			if(emitterDead)
			{
				if(emitter->IsEmpty())
				{
					HandleDeletingEmitter(emitter, i--);
					continue;
				}
				emitter->KillOldParticulates(absTime);
				// dont spawn if we are after our death.
				// we arent deleted yet because we still contain particulates
			}
			else
			{
				emitter->KillOldParticulates(absTime);
				emitter->SpawnNewParticulates(absTime);
			}
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////////////

	void System::ResortEmitterList()
	{
		mSortedEmitters.BubbleSortAscending();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void System::AddEmitter(Emitter* emitter)
	{
		mEmitters.Add(emitter);
		mSortedEmitters.InsertAscending(emitter);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void System::RemoveEmitter(Emitter* emitter)
	{
		for(int i = 0; i < mEmitters.Count(); i++)
		{
			if(mEmitters[i] == emitter)
			{
				mEmitters.Delete(i);
				break;
			}
		}

		// is it already out?
		if(emitter->next == NULL && emitter->prev == NULL && mSortedEmitters.Head() != emitter)
			return;

		mSortedEmitters.Unlink(emitter);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void System::ResortEmitter(Emitter* emitter)
	{
		mSortedEmitters.Unlink(emitter);
		mSortedEmitters.InsertAscending(emitter);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	// From here down, the routines are for compatibility with the existing particle manager data
	// and function calls
	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////

	bool System::IsCompatible(ts_EmitterSource* source, int nDataIndex)
	{
		// unsupported flags
		if(source->u8Flags & (EMITTER_SOURCE_SCALE_CONT_RANDOM|EMITTER_SOURCE_DELAY_CONT_RANDOM|EMITTER_SOURCE_ACTIVE_MOVE_ONLY))
			return false;
		if((source->u8Type & EMITTER_TYPE_MASK) != EMITTER_STANDARD)
			return false;

		// we use u16GoRadius as a culling sphere radius for the emitter.  Hopefully this looks
		// better than stopping emitting, and doesnt hit us too badly for speed.
		//u16GoRadius

		// lod should be made to work, but right now it doesnt
		//u16LOD1Radius
		//u16LOD2Radius

		ts_Effect* pEffect = (ts_Effect *)ParticleMgr__GetEffectSource(nDataIndex, source->u16EffectID);
		if(pEffect == NULL)
			return false;

		return IsCompatible(pEffect);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	bool System::IsCompatible(ts_Effect* source)
	{
		for(u32 i = 0; i < source->u32ChannelCount; i++)
		{
			if(!IsCompatible(&source->ChannelData[i]))
				return false;
		}
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	bool System::IsCompatible(ts_Channel* source)
	{
		// pointer valid after patching by FixParticleEffect
		ASSERT_PTR(source->pParticleSource);

		// flags allowed (CHANNEL_EMIT_TIME_CONT_RANDOM doesnt appear to be supported by particle mgr, so we ignore it)
		if(source->u8Flags & CHANNEL_IS_SOUND)
			return false;

		// check particle source
		return IsCompatible(source->pParticleSource);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	bool System::IsCompatible(ts_ParticleEnvelope* colorenvelope)
	{
		if(colorenvelope->fSplinePoint0Y > colorenvelope->fCeilingY)
			return false;
		if(colorenvelope->fSplinePoint0Y < colorenvelope->fFloorY)
			return false;

		if((colorenvelope->u8Type & ENVELOPE_TYPE_MASK) == STRAIGHT_ENVELOPE)
		{
			if(colorenvelope->fSplinePoint3Y > colorenvelope->fCeilingY)
			{
				showRejectionReason(("PM: envelope ceiling violation, rejecting\n"));
				return false;
			}
			if(colorenvelope->fSplinePoint3Y < colorenvelope->fFloorY)
			{
				showRejectionReason(("PM: envelope floor violation, rejecting\n"));
				return false;
			}
		}
		else if((colorenvelope->u8Type & ENVELOPE_TYPE_MASK) == FLAT_ENVELOPE)
		{
			; // nothing to check here.
		}
		else if((colorenvelope->u8Type & ENVELOPE_TYPE_MASK) == SPLINE_ENVELOPE)
		{
			; // nothing to check here.
		}
		else
		{
			showRejectionReason(("PM: ADSR envelope, rejecting\n"));
			return false;
		}
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	bool System::IsCompatible(ts_ParticleSource* source)
	{
		// only a single texture, no animation or any such thing
		if(source->u16FirstIndex != source->u16LastIndex)
		{
			showRejectionReason(("PM: texanim, rejecting\n"));
			return false;
		}

		// X & Y must be uniform size
		if(source->u16XSize != source->u16YSize)
		{
			showRejectionReason(("PM: XY mismatch, rejecting\n"));
			return false;
		}
		if(source->u8XSizeRandomPct != source->u8YSizeRandomPct)
		{
			showRejectionReason(("PM: XY mismatch, rejecting\n"));
			return false;
		}

		// cant be set to collide with any thing
		if(source->u8CollisionFlags != 0)
		{
			showRejectionReason(("PM: collision, rejecting\n"));
			return false;
		}

		// z bias is unsupported
		if(source->s16ZBias != 0)
		{
			showRejectionReason(("PM: zbias, rejecting\n"));
			return false;
		}

		// rotational velocity in z only.
		// TJC - dont check this because the particle mgr sometimes sets these but then just ignores
		// them.  the PARTICLE_ORIENT_* checks below will resolve it.
		//if(source->vInitialRotVelRadians.x() != 0.0f || source->vInitialRotVelRadians.y() != 0.0f)
		//	return false;

		// these flags cant be set
		u32 notreqflags = 
			PARTICLE_PERPENDICULAR_TO_VELOCITY |
			PARTICLE_BULLET |
			PARTICLE_SOURCE_ALIGN_TO_VELOCITY |
			PARTICLE_ZBUFFER_WRITE |
			PARTICLE_LIGHTED |
			PARTICLE_FORCEFIELDS_AFFECT |
			PARTICLE_ALPHA_REF |
			PARTICLE_RANDOM_POS_PLANE |
			PARTICLE_ORIENT_FREE |		/* have to specify these because PARTICLE_ORIENT_SCREEN is 0 */
			PARTICLE_ORIENT_GROUND |	/* have to specify these because PARTICLE_ORIENT_SCREEN is 0 */
			PARTICLE_ORIENT_WALL |		/* have to specify these because PARTICLE_ORIENT_SCREEN is 0 */
			PARTICLE_RENDER_FOR_DISTORT;
		if(source->u32Flags & notreqflags)
		{
			showRejectionReason(("PM: bad flags, rejecting  %x %x\n", notreqflags, source->u32Flags));
			return false;
		}

		// these flags must be set
		u32 reqflags = 
			PARTICLE_RANDOM_SIZE_MAINTAIN_ASPECT_RATIO |
			PARTICLE_NO_DISTORTION |
			PARTICLE_NO_VIEW_CULL |
			PARTICLE_ORIENT_SCREEN;
		if((source->u32Flags & reqflags) != reqflags)
		{
			showRejectionReason(("PM: bad flags2, rejecting\n"));
			return false;
		}

		// envelopes must be synthisizable
		if(source->Envelope[X_SIZE_ENVELOPE].u8Type != source->Envelope[source->u8YSizeEnvelope].u8Type ||
			source->Envelope[X_SIZE_ENVELOPE].fSplinePoint0Y != source->Envelope[source->u8YSizeEnvelope].fSplinePoint0Y)
		{
			showRejectionReason(("PM: X and Y envelope dont match, rejecting\n"));
			return false;
		}
		if(source->Envelope[X_SIZE_ENVELOPE].u8Type == STRAIGHT_ENVELOPE &&
			source->Envelope[X_SIZE_ENVELOPE].fSplinePoint3Y != source->Envelope[source->u8YSizeEnvelope].fSplinePoint3Y)
		{
			showRejectionReason(("PM: X and Y envelope dont match, rejecting\n"));
			return false;
		}

		if(!IsCompatible(&source->Envelope[X_SIZE_ENVELOPE]))
			return false;

		if(!IsCompatible(&source->Envelope[RED_ENVELOPE]))
			return false;

		if(source->u32Flags & PARTICLE_COLOR_RGB)
		{
			if(!IsCompatible(&source->Envelope[GREEN_ENVELOPE]))
				return false;
			if(!IsCompatible(&source->Envelope[BLUE_ENVELOPE]))
				return false;
		}
		if(!IsCompatible(&source->Envelope[ALPHA_ENVELOPE]))
			return false;

		// no piggyback emitters
		if(source->u16PiggybackEmitterIndex != NO_PIGGYBACK_EMITTER)
   	{
			showRejectionReason(("PM: piggyback, rejecting\n"));
			return false;
		}

		// no orbit is allowed
		if(source->u8OrbitRadius != 0 || source->s8OrbitRadiusVel != 0)
		{
			showRejectionReason(("PM: orbit, rejecting\n"));
			return false;
		}

		// no leashing
		if(source->u16Leash != 0)
		{
			showRejectionReason(("PM: leash, rejecting\n"));
			return false;
		}

		// no spawn effects may be active
		for(int i = 0; i < source->u16SpawnEffectCount; i++)
		{
			if(source->SpawnEffect[i].u8Flags & SPAWN_ACTIVE)
			{
				showRejectionReason(("PM: spawn, rejecting\n"));
				return false;
			}
		}

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	FloatRange System::FromParticleMgrRandomToOurs(float value, int percent)
	{
		ASSERT((percent <= 100) || (percent == RANDOM_CENTER_ON_ZERO));
		if (percent != 0)
		{
			if (percent == RANDOM_CENTER_ON_ZERO)
				return FloatRange(-value, value);

			float fPercent = percent / 100.0f;

			float lowvalue = value - value * fPercent;
			float highvalue = value + value * fPercent;
			if(value >= 0.0f)
				return FloatRange(Math::Max(0.001f, lowvalue), Math::Max(0.001f, highvalue));
			else
				return FloatRange(Math::Min(-0.001f, lowvalue), Math::Min(-0.001f, highvalue));
		}
		else
			return FloatRange(value, value);
	}
	void System::FromParticleMgrEnvelopeToOurSpline(ts_ParticleEnvelope* env, float& p0, float& p1, float& p2, float& p3)
	{
		if(env->u8Type == FLAT_ENVELOPE)
		{
			p0 = env->fSplinePoint0Y;
			p1 = env->fSplinePoint0Y;
			p2 = env->fSplinePoint0Y;
			p3 = env->fSplinePoint0Y;
		}
		else if(env->u8Type == STRAIGHT_ENVELOPE)
		{
			p0 = env->fSplinePoint0Y;
			p1 = env->fSplinePoint0Y * 0.666666f + env->fSplinePoint3Y * 0.333333f;
			p2 = env->fSplinePoint0Y * 0.333333f + env->fSplinePoint3Y * 0.666666f;
			p3 = env->fSplinePoint3Y;
		}
		else if(env->u8Type == ADSR_ENVELOPE)
		{
			p0 = env->fSplinePoint0Y;
			p3 = env->fSplinePoint3Y;

			float avg = (p0 + p3) * 0.5f;

			p1 = avg + (env->SplinePoint1.Y() - avg) * 2.0f;
			p2 = avg + (env->SplinePoint2.Y() - avg) * 2.0f;
		}
		else
		{
			p0 = env->fSplinePoint0Y;
			p1 = env->SplinePoint1.Y();
			p2 = env->SplinePoint2.Y();
			p3 = env->fSplinePoint3Y;
		}
	}

	void System::CreateEffect(ts_Channel* pChannel, Vector3CRef pos, Vector3CRef vel, float velmag, Vector3CRef veladd, float fScale, float fConstantAlpha, float fColorMult)
	{
		ts_ParticleSource* source = pChannel->pParticleSource;
		Vector3 vVel = vel;

		// some work in advance of creation...
		Cone position(
			Vector3(1.0f, 0.0f, 0.0f), 
			0.0f,
			Math::Pi, 
			FloatRange(0.0f, CONVERT_U16_TO_FLOAT(source->u16PosRandom, MAX_P_POS_RANDOM)));

		float inner = CONVERT_U8_TO_FLOAT(source->u8VelConeStart, MAX_P_VEL_CONE);
		float outer = CONVERT_U8_TO_FLOAT(source->u8VelConeDelta, MAX_P_VEL_CONE) + inner;
		Cone velocity(vel,
						  inner,
						  outer,
						  FromParticleMgrRandomToOurs(velmag, source->u8VelRandomPct));

		// our scale is different that the particle managers... ours is from center out
		// while I think the particle managers is from one side to opposite side.
		fScale *= 0.5f; 

		FloatRange angVelocity = FromParticleMgrRandomToOurs(source->vInitialRotVelRadians.z(),source->u8RotVelRandomPct);
		FloatRange scale = FromParticleMgrRandomToOurs(CONVERT_U16_TO_FLOAT(source->u16XSize, MAX_P_SIZE) * fScale, source->u8XSizeRandomPct);
		FloatRange lifetime = FromParticleMgrRandomToOurs(CONVERT_U8_TO_FLOAT(source->u8Lifetime, MAX_P_LIFETIME), source->u8LifetimeRandomPct);
		float emitterlife = pChannel->fEmitTime;
		float spawnrate = pChannel->u8EmitCount;
		if(emitterlife < g_timer.GetFrameSec()*1.5f)
			emitterlife = -1.0f;
		else
			spawnrate /= emitterlife;
		Vector3 acceleration = vVel;
		acceleration = acceleration * source->fAccel + Vector3(0.0f, source->fGravity, 0.0f);
		FloatRange angPosition;
		if(source->u32Flags & PARTICLE_INIT_ROT_RANDOM)
			angPosition.Set(0.0f, Math::TwoPi);
		else
			angPosition.Set(0.0f, 0.0f);

		ColorEnvelope color;
		if(source->u32Flags & PARTICLE_COLOR_RGB)
		{
			float r0,r1,r2,r3;
			float g0,g1,g2,g3;
			float b0,b1,b2,b3;
			FromParticleMgrEnvelopeToOurSpline(&source->Envelope[RED_ENVELOPE], r0, r1, r2, r3);
			FromParticleMgrEnvelopeToOurSpline(&source->Envelope[GREEN_ENVELOPE], g0, g1, g2, g3);
			FromParticleMgrEnvelopeToOurSpline(&source->Envelope[BLUE_ENVELOPE], b0, b1, b2, b3);
			color.mSplinePoints[0].Set(r0, g0, b0, 0.0f);
			color.mSplinePoints[1].Set(r1, g1, b1, 0.0f);
			color.mSplinePoints[2].Set(r2, g2, b2, 0.0f);
			color.mSplinePoints[3].Set(r3, g3, b3, 0.0f);
		}
		else
		{
			float c0,c1,c2,c3;
			Vector4 color0(source->Color[0].R, source->Color[0].G, source->Color[0].B, 1.0f);
			Vector4 color1(source->Color[1].R, source->Color[1].G, source->Color[1].B, 1.0f);
			FromParticleMgrEnvelopeToOurSpline(&source->Envelope[RED_ENVELOPE], c0, c1, c2, c3);
			color.mSplinePoints[0] = color0 * c0 + color1 * (1.0f - c0);
			color.mSplinePoints[1] = color0 * c1 + color1 * (1.0f - c1);
			color.mSplinePoints[2] = color0 * c2 + color1 * (1.0f - c2);
			color.mSplinePoints[3] = color0 * c3 + color1 * (1.0f - c3);
		}

		// deal with alpha
		float a0,a1,a2,a3;
		FromParticleMgrEnvelopeToOurSpline(&source->Envelope[ALPHA_ENVELOPE], a0, a1, a2, a3);
		a0 *= 255.0f;
		a1 *= 255.0f;
		a2 *= 255.0f;
		a3 *= 255.0f;
		color.mSplinePoints[0].W(a0);
		color.mSplinePoints[1].W(a1);
		color.mSplinePoints[2].W(a2);
		color.mSplinePoints[3].W(a3);

		FloatSpline scaleEnvelope;
		FromParticleMgrEnvelopeToOurSpline(&source->Envelope[X_SIZE_ENVELOPE], 
													  scaleEnvelope.mPoints[0], 
													  scaleEnvelope.mPoints[1],
													  scaleEnvelope.mPoints[2],
													  scaleEnvelope.mPoints[3]);

		// create a new effect for this ... umm.. effect... :)
		void* effectspace = mOwnedEffects.NewPtr();
		if(!effectspace)
			return;
		void* emitterspace = mOwnedEmitters.NewPtr();
		if(!emitterspace)
			return;

		Effect* effect = new (effectspace) Effect(
			position,
			velocity,
			angPosition, angVelocity,
			scale,
			lifetime,
			FloatRange(emitterlife, emitterlife),
			color,
			source->pTextureDatabase->GetTexture(source->u16FirstIndex),
			0.0f, // drag coefficient
			acceleration,
			spawnrate,
			scaleEnvelope,
			source->fRotAccelRadians); 

		// create a new emitter using this effect
		Emitter* emitter = new (emitterspace) Emitter(
			this, 
			effect, 
			pos, 
			Emitter::EMITTER_OWNS_EFFECT | Emitter::EMITTER_BELONGS_TO_SYSTEM,
			pChannel->fEmitDelay);
	}

};

