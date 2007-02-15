#ifndef __PARTICULATE_MGR_H__
#define __PARTICULATE_MGR_H__

#include "Math/fastrand.h"
#include "Display/Common/StripBuffer.h"
#include "Display/Common/VtxBuffer.h"
#include "Game/Managers/ParticleMgr.h"

// The particulate manager is a stripped down particle manager designed for
// thousands of particles per frame, rather than the hundreds that the
// ParticleMgr is capable of.  However, it has a couple of restrictions. :)
// Both therefore are good to have around.
//
// Restrictions
//   1) Particulates always face the screen
//   2) Particulates always have a width == height
//   3) Particulate emitters emit only one type of particulate
//   4) Particulates are always disposable.  The system is free to throw them
//      out at any time.  They are eye candy only.  The system will, from time
//      to time, ignore their lifetime in order to better maintain the effects.
//   5) The system only has the following parameters
//      Per Particulate:
//         a) creation time
//         b) death time
//         c) position
//         d) velocity
//         e) color
//         f) scale
//         g) rotation angle
//         h) angular velocity
//      Per Emitter:
//         a) creation time
//         b) death time
//         c) position
//         d) effect data pointer
//         e) particulate list
//      Per Effect:
//         a) initial particulate position offset range
//         b) initial particulate velocity range
//         c) particulate color envelope
//               initial color
//               mid color
//               final color
//               mid lifetime percentage (0-1)
//         d) initial particulate scale range
//         e) particulate lifetime range
//         f) emitter lifetime range
//         g) Texture
//         h) particulate drag coefficient
//         i) particulate acceleration vector
//         k) particulate spawn rate
//         l) particulate scale rate
//         m) particulate angular acceleration rate
//         n) particulate initial angular velocity range
// 
// Notes:
// * For emitters to change position, the code must update them regularly.
// * Code outside this system should not deal with individual particles.
// * A namespace is used to help me keep names of classes short.

#define MAX_PARTICULATES_PER_SYSTEM 2048
#define NUMBER_TO_TRY_TO_MOVE 4 // this is just an optimization tuning number

namespace ParticulateMgr
{
	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	// initialization ranged values & envelopes
	class FloatRange;
	class Cone;
	class ColorEnvelope;
	// a single System has a fixed memory overhead.
	class System;
	// a single Emitter emits particulates of on style from it.
	class Emitter;
	// a single Effect represents one style of particulates.
	class Effect;
	// internal Particulate representation
	class Particulate;


	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	// The two main entry points (called each frame).
	void Update(float absTime = g_timer.GetStartSec());
	void Render(float absTime = g_timer.GetStartSec());
	#ifndef CDROM
	void Test(void);
	#endif
	void RenderDisable();
	void RenderEnable();
	bool RenderEnabled();

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

   class FloatRange {
	public:
		float mMin;
		float mMax;

		inline FloatRange();
		inline FloatRange(float daMin, float daMax);
		inline float Random();
		inline bool AlwaysZero();
		inline void Set(float val);
		inline void Set(float minimum, float maximum);
	};

	class FloatSpline {
	public:
		float mPoints[4];

		inline FloatSpline();
		inline FloatSpline(float constant);
		inline FloatSpline(float initial, float final);
		inline FloatSpline(float point0, float point1, float point2, float point3);
		inline float GetValue(float fT);
	};

	//////////////////////////////////////////////////////////////////////////////////

	class Cone {
	public:
		Vector3 mDirection;
		float mOuterAngle;
		float mInnerAngle;
		FloatRange mConeScale;

		inline Cone();
		inline Cone(Vector3CRef daDirection, float daInnerAngle, float daOuterAngle, const FloatRange& daConeScale);
		inline Vector3 Random();
	};

	//////////////////////////////////////////////////////////////////////////////////

	class ColorEnvelope {
	public:
		Vector4 mSplinePoints[4];

		inline ColorEnvelope();
		inline ColorEnvelope(Vector4CRef color);
		inline ColorEnvelope(Vector4CRef initial, Vector4CRef final);
		inline ColorEnvelope(Vector4CRef splinepoint1, Vector4CRef splinepoint2, Vector4CRef splinepoint3, Vector4CRef splinepoint4);
		inline Vector4 GetColor(float fT);
	};

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	class ParticulateSet {
	public:
		s32 mFirstIndex;
		s32 mIndexAfterLast;

		inline ParticulateSet();
		inline s32 GetSize() const;
	};

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	class Effect {
		friend class TestClass;
		friend class Emitter;
		friend class DoublyLinkedList<ParticulateMgr::Effect>;
		friend void Update(float absTime);

		// tracking info used internally
		Effect *next;
		Effect *prev;
		u32 mEmitterCount; // just used to make sure we clean up properly
		float mLastABSTime;
		s32 mSpawnsThisCycle;
		float mSpawnTimeRemaining;


		void InitializeParticulate(Particulate* particulate, float absTime, Vector3CRef position);
		void Update(float absTime);

	public:
		Effect(const Cone& initialPositionOffset,
				 const Cone& initialVelocity,
				 const FloatRange& initialAngularPositionRange,
				 const FloatRange& initialAngularVelocityRange,
				 const FloatRange& initialScaleRange,
				 const FloatRange& lifetimeRange,
				 const FloatRange& emitterLifetimeRange, // negative values are frame counts
				 const ColorEnvelope& colorEnvelope,
				 Texture* useTexture,
				 float dragCoefficient,
				 Vector3CRef accelerationVector,
				 float spawnRate,
				 const FloatSpline& scaleEnvelope,
				 float angluarAcceleration);
		Effect(const Effect& baseEffect);
		~Effect();
		Effect* Clone();

		// initial particulate values
		Cone mInitialPositionOffset;
		Cone mInitialVelocity;
		FloatRange mInitialAngularPositionRange;
		FloatRange mInitialAngularVelocityRange;
		FloatRange mInitialScaleRange;
		FloatRange mLifetimeRange;
		FloatSpline mScaleEnvelope;

		// how fast to spawn
		float mSpawnRate;

		// initial emitter values
		FloatRange mEmitterLifetimeRange;

		// effect wide values
		ColorEnvelope mColorEnvelope;
		Texture* mUseTexture;
		Vector3 mAccelerationVector;
		float mDragCoefficient;
		float mScaleRate;
		float mAngluarAcceleration;
	};

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	class Emitter {
		friend class TestClass;
		friend class System;
		friend class DoublyLinkedList<ParticulateMgr::Emitter>;

		System* mSystem;
		Effect* mEffect;
		Emitter* next;
		Emitter* prev;
		ParticulateSet mParticulates;

		float mBirthTime;
		float mLastSpawnTime;

		u32 mFlags;

		void SpawnNewParticulates(float absTime);
		int AllocNewParticulate(float absTime);
		void KillOldParticulates(float absTime);

		inline bool IsFirst() const;
		inline bool IsLast() const;
		inline s32 CanMoveUp() const;
		inline s32 CanMoveDown() const;
		inline void ShiftUp(s32 count = 1);
		inline void ShiftDown(s32 count = 1);
		inline bool operator> (const Emitter& rhs) const;
		inline bool operator< (const Emitter& rhs) const;

		void Relocate(const ParticulateSet& set);
		void Render(float absTime);

		// rendering helpers
//		#ifndef PS2
		StripBuffer sb;
		VtxBuffer vb;
//		#endif
	public:
		// flags for this emitter
		enum {
			EMITTER_OWNS_EFFECT			= 0x00000001, // if set, emitter deletes effect once empty and dead
			EMITTER_DELETES_ON_DEATH	= 0x00000002, // if set, emitter deletes self once empty and dead

			// if set, this emitter gets put back into system free list on death
			// this is basically just to optimize the particle manager tie-ins.  It
			// sucks, and I feel like its a hack into the particulate manager.
			// to make it a bit more hacky, the EMITTER_OWNS_EFFECT should also be
			// set, and the emitter will just hold on to its effect memory forever.
			EMITTER_BELONGS_TO_SYSTEM	= 0x00000004, 

		};

		Vector3 mPosition;
		float mDeathTime;

		inline bool IsEmpty() const;
		inline int Count() const;

		Emitter(System* system, Effect* effect, Vector3CRef position, u32 flags = 0, float absTime = g_timer.GetStartSec(), float emitDelay = 0.0f);
		~Emitter();
	};

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	class System {
		friend class TestClass;
		friend class Emitter;
		friend class DoublyLinkedList<ParticulateMgr::System>;
		friend void Update(float absTime);
		friend void Render(float absTime);

		void AddEmitter(Emitter* emitter);
		void RemoveEmitter(Emitter* emitter);
		void ResortEmitter(Emitter* emitter);
		void Render(float absTime);
		void Update(float absTime);

		ParticulateSet GetLargestGap();
		void ResortEmitterList();
		void DeleteSelfDeletingEmitters();
		void HandleDeletingEmitter(Emitter* emitter,  int i );
		System *next;
		System *prev;
		Particulate *mBuffer;

		DoublyLinkedList<Emitter> mSortedEmitters;

	public:
		// these are for particle manager tie-in use only.. HACK..
		// they exist to accelerate the connection, and thats it.
		Pool<Emitter> mOwnedEmitters; 
		Pool<Effect> mOwnedEffects;

		s32 mTotalParticulatesSpawned;
		s32 mTotalParticulatesDied;

		DynamicArray<Emitter*> mEmitters;

		System();
		~System();

		void Reset();

		// ParticleMgr Compatibility routines.  These are not necessary to use the
		// ParticulateMgr... They just help glue the two systems together.
		bool IsCompatible(ts_EmitterSource* source, int nDataIndex);
		bool IsCompatible(ts_Effect* source);
		bool IsCompatible(ts_Channel* source);
		bool IsCompatible(ts_ParticleSource* source);
		bool IsCompatible(ts_ParticleEnvelope* colorenvelope);
		void CreateEffect(ts_Channel* pChannel, Vector3CRef pos, Vector3CRef vel, float velmag, Vector3CRef veladd, float fScale, float fConstantAlpha, float fColorMult);
	private:
		FloatRange FromParticleMgrRandomToOurs(float value, int percent);
		void FromParticleMgrEnvelopeToOurSpline(ts_ParticleEnvelope* env, float& p0, float& p1, float& p2, float& p3);
	};

	#include "Game/Managers/ParticulateMgr/ParticulateMgr_Internal.h"
	#include "Game/Managers/ParticulateMgr/ParticulateMgr.hpp"
};


#endif // __PARTICULATE_MGR_H__


