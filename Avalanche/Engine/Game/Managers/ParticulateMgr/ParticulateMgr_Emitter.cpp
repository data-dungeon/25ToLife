#include "Game/GamePCH.h"

namespace ParticulateMgr
{
	Emitter::Emitter(System* system, Effect* effect, Vector3CRef position, u32 flags, float absTime, float emitDelay) :
		mSystem(system),
		mEffect(effect),
		mBirthTime(absTime),
		mLastSpawnTime(absTime),
		mPosition(position),
		mFlags(flags)
	{
		mBirthTime = absTime + emitDelay;

		if(effect->mEmitterLifetimeRange.AlwaysZero())
			mDeathTime = 0.0f;
		else
			mDeathTime = mBirthTime + effect->mEmitterLifetimeRange.Random() + 0.0001f; // must not ever be 0.0f

		mSystem->AddEmitter(this);
		mEffect->mEmitterCount++;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	Emitter::~Emitter()
	{
		mSystem->RemoveEmitter(this);
		mEffect->mEmitterCount--;

		if(mFlags & EMITTER_OWNS_EFFECT)
		{
			if(mFlags & EMITTER_BELONGS_TO_SYSTEM)
				mSystem->mOwnedEffects.Delete(mEffect);
			else if(mFlags & EMITTER_DELETES_ON_DEATH)
				delete mEffect;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void Emitter::SpawnNewParticulates(float absTime)
	{
		int numToSpawn = mEffect->mSpawnsThisCycle;
		while(numToSpawn--)
		{
			int index = AllocNewParticulate(absTime);
			if(index == -1)
				return;
			Particulate* part = &mSystem->mBuffer[index];

			mEffect->InitializeParticulate(part, absTime, mPosition);

			mSystem->mTotalParticulatesSpawned++;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void Emitter::Relocate(const ParticulateSet& set)
	{
		int gapsize = set.GetSize();
		int oursize = Count();
		int remainder = gapsize - oursize;
		int halfremainder = remainder / 2;
		ParticulateSet newSet;
		newSet.mFirstIndex = set.mFirstIndex + halfremainder;
		newSet.mIndexAfterLast = newSet.mFirstIndex + oursize;
		memcpy(&mSystem->mBuffer[newSet.mFirstIndex], &mSystem->mBuffer[mParticulates.mFirstIndex], oursize * sizeof(Particulate));
		mParticulates = newSet;
		mSystem->ResortEmitter(this);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	int Emitter::AllocNewParticulate(float absTime)
	{
		// if we are empty, start by moving us somewhere valid.
		if(IsEmpty())
		{
			ParticulateSet set = mSystem->GetLargestGap();

			s32 size = set.GetSize();
			if(size == 0)
			{
				// TODO: resolve this
				mParticulates = set;
				return -1;
			}
			else if(size == 1)
			{
				mParticulates = set;
			}
			else
			{
				mParticulates.mIndexAfterLast = set.mFirstIndex + (set.mIndexAfterLast - set.mFirstIndex) / 2;
				mParticulates.mFirstIndex = mParticulates.mIndexAfterLast-1;

			}
			mSystem->ResortEmitter(this);
			return mParticulates.mFirstIndex;
		}

		// first try tacking it on the beginning
		if(CanMoveDown())
			return --mParticulates.mFirstIndex;

		// next try tacking it on the end of the particulate list
		if(CanMoveUp())
			return mParticulates.mIndexAfterLast++;

		// then see if we can move part of a neighbor
		if(!IsLast() && next->CanMoveUp())
		{
			next->ShiftUp();
			return mParticulates.mIndexAfterLast++;
		}
		if(!IsFirst() && prev->CanMoveDown())
		{
			prev->ShiftDown();
			return --mParticulates.mFirstIndex;
		}

		// then see if we should move ourselves or an entire neighbor
		s32 ourSize = Count();
		s32 upSize = 0x7FFFFFFF;
		s32 downSize = 0x7FFFFFFF;
		if(!IsLast())
			upSize = next->Count();
		if(!IsFirst())
			downSize = prev->Count();
		ParticulateSet set = mSystem->GetLargestGap();
		s32 setSize = set.GetSize();
		if(ourSize < upSize && ourSize < downSize && ourSize < setSize)
		{
			// move us...
			Relocate(set);
			// we should now safely find one of the easy outs above.
			return AllocNewParticulate(absTime);
		}
		else if(upSize < downSize && upSize < setSize)
		{
			// move next
			next->Relocate(set);
			// we should now safely find one of the easy outs above.
			return AllocNewParticulate(absTime);
		}
		else if(downSize < setSize)
		{
			// move prev
			prev->Relocate(set);
			return AllocNewParticulate(absTime);
		}

		// then see if we can pack the entire buffer tighter
		// search up first...
		Emitter* currEmitter = next;
		while(currEmitter)
		{
			int moveSize = currEmitter->CanMoveUp();
			if(moveSize)
			{
				moveSize = Math::Min(moveSize, NUMBER_TO_TRY_TO_MOVE);
				while(currEmitter != this)
				{
					currEmitter->ShiftUp(moveSize);
					currEmitter = currEmitter->prev;
				}
				return mParticulates.mIndexAfterLast++;
			}
			currEmitter = currEmitter->next;
		}
		// now search down...
		currEmitter = prev;
		while(currEmitter)
		{
			int moveSize = currEmitter->CanMoveDown();
			if(moveSize)
			{
				moveSize = Math::Min(moveSize, NUMBER_TO_TRY_TO_MOVE);
				while(currEmitter != this)
				{
					currEmitter->ShiftDown(moveSize);
					currEmitter = currEmitter->next;
				}
				return --mParticulates.mFirstIndex;
			}
			currEmitter = currEmitter->prev;
		}

		// give up.
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	void Emitter::KillOldParticulates(float absTime)
	{
		for(int i = mParticulates.mFirstIndex; i < mParticulates.mIndexAfterLast; i++)
		{
			if(mSystem->mBuffer[i].mDeathTime < absTime)
			{
				// move last over this ones spot and reduce count by one.
				mSystem->mBuffer[i] = mSystem->mBuffer[--mParticulates.mIndexAfterLast];

				// reprocess at this value of 'i'.
				i--;

				mSystem->mTotalParticulatesDied++;
			}
		}
	}

};

