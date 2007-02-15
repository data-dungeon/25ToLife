#include "Game/GamePCH.h"
#include "Layers/ttyColors.h"

#define TEST_PASS NULL
#define RUN_TEST(xyz) \
do{\
	StartTest(); \
	char*results = xyz(); \
	StopTest(); \
	if(results == TEST_PASS) \
	{ \
		goodcount++; \
		dbgPrint("ParticulateMgr Test " TTY_GREEN "passed!"  TTY_RESET " %s\n", #xyz); \
	} \
	else \
	{ \
		badcount++; \
		dbgPrint("ParticulateMgr Test " TTY_RED "failed!" TTY_RESET " %s %s\n", #xyz, results); \
	}\
}while(0)

#define TEST_RESULTS(xyz) \
do{\
	char* result = xyz;\
	if(result != TEST_PASS)\
		return result;\
}while(0)

#define TEST_ASSERTION(xyz)\
do{\
	assertioncount++;\
	if(!(xyz))\
	{\
		sprintf(test_msg_buffer, "(%s) (LINE:%d)", #xyz, __LINE__);\
		return test_msg_buffer;\
	}\
}while(0)

#define TEST_ASSERTION2(xyz, value)\
do{\
	assertioncount++;\
	if(!(xyz))\
	{\
		sprintf(test_msg_buffer, "(%s) (value:%d) (LINE:%d)", #xyz, value, __LINE__);\
		return test_msg_buffer;\
	}\
}while(0)

#define TEST_ASSERTION3(xyz, value1, value2)\
do{\
	assertioncount++;\
	if(!(xyz))\
	{\
		sprintf(test_msg_buffer, "(%s) (value1:%d) (value2:%d) (LINE:%d)", #xyz, value1, value2, __LINE__);\
		return test_msg_buffer;\
	}\
}while(0)



namespace ParticulateMgr
{
	static int assertioncount = 0;
	static char test_msg_buffer[1000];
	void TestClass::StartTest()
	{
		testTexture = new Texture(NULL, "Empty junk");
		
		testEffect = new Effect(
			Cone(Vector3(0.0f, 0.0f, 0.0f), 0.0f, Math::Pi, FloatRange(0.0f, 1.0f)), // position offset cone
			Cone(Vector3(0.0f, 0.0f, 0.0f), 0.0f, Math::Pi, FloatRange(0.0f, 1.0f)), // velocity cone
			FloatRange(0.0f, 0.0f), // angular position
			FloatRange(0.0f, 0.0f), // angular velocity
			FloatRange(1.0f, 1.0f), // scale
			FloatRange(1.0f, 2.0f), // lifetime
			FloatRange(5.0f, 6.0f), // emitter lifetime
			ColorEnvelope(
				Vector4(1.0f, 1.0f, 1.0f, 1.0f),  // start
				Vector4(1.0f, 1.0f, 1.0f, 1.0f)),  // end
			testTexture, // texture
			0.0f,	//drag
			Vector3(0.0f, -9.8f, 0.0f), // acceleration
			1.0f, // spawnrate
			0.0f, // scalerate
			0.0f); // angluaraccel
	}
	void TestClass::StopTest()
	{
		delete testTexture;
		delete testEffect;
		testTexture = NULL;
		testEffect = NULL;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// test subroutines
	//////////////////////////////////////////////////////////////////////////////////////////////////

	char* TestClass::VerifyEmitterIsValid(System& system,Emitter* a)
	{
		TEST_ASSERTION(((Emitter*)a) != NULL);
		TEST_ASSERTION(a->mParticulates.mIndexAfterLast >= a->mParticulates.mFirstIndex);
		TEST_ASSERTION(a->mSystem == &system);
		TEST_ASSERTION(a->mDeathTime == 0.0f || a->mBirthTime < a->mDeathTime);
		return TEST_PASS;
	}
	char* TestClass::VerifyEmitterCount(System& system, int expectedCount)
	{
		TEST_ASSERTION3(system.mEmitters.Count() == expectedCount, system.mEmitters.Count(), expectedCount);
		return TEST_PASS;
	}
	char* TestClass::VerifyEmitterParticulateCount(Emitter* a, int count)
	{
		TEST_ASSERTION3(a->Count() == count, a->Count(), count);
		return TEST_PASS;
	}
	char* TestClass::VerifyEmitterIsEmpty(Emitter* a)
	{
		TEST_ASSERTION(a->IsEmpty());
		return TEST_PASS;
	}
	char* TestClass::VerifyEmittersAreSequential(Emitter* a, Emitter* b)
	{
		if(a->mParticulates.mFirstIndex == a->mParticulates.mIndexAfterLast)
			return TEST_PASS;
		if(b->mParticulates.mFirstIndex == b->mParticulates.mIndexAfterLast)
			return TEST_PASS;
		TEST_ASSERTION(a->mParticulates.mIndexAfterLast <= b->mParticulates.mFirstIndex);
		return TEST_PASS;
	}
	char* TestClass::VerifyEmittersDontCollide(Emitter* a, Emitter* b)
	{
		int afirst = a->mParticulates.mFirstIndex;
		int alast = a->mParticulates.mIndexAfterLast;
		if(afirst == alast)
			return TEST_PASS;
		int bfirst = b->mParticulates.mFirstIndex;
		int blast = b->mParticulates.mIndexAfterLast;
		if(bfirst == blast)
			return TEST_PASS;
		TEST_ASSERTION(afirst < bfirst || afirst >= blast);
		if(afirst < bfirst)
			TEST_ASSERTION(alast <= bfirst);
		else
			TEST_ASSERTION(alast >= blast);
		return TEST_PASS;
	}
	char* TestClass::VerifyEmitterListValid(System& system)
	{
		for(int i = 0; i < system.mEmitters.Count(); i++)
		{
			TEST_RESULTS(VerifyEmitterIsValid(system, system.mEmitters[i]));
			if(i > 0)
			{
				TEST_RESULTS(VerifyEmittersDontCollide(system.mEmitters[i-1], system.mEmitters[i]));
			}
		}
		Emitter* curremitter = system.mSortedEmitters.Head();
		while(curremitter)
		{
			if(!curremitter->IsEmpty())
			{
				Emitter* nextemitter = curremitter->next;
				while(nextemitter && nextemitter->IsEmpty())
					nextemitter = nextemitter->next;

				if(nextemitter)
				{
					TEST_RESULTS(VerifyEmittersAreSequential(curremitter, nextemitter));
				}
				curremitter = nextemitter;
			}
			else
				curremitter = curremitter->next;
		}
		return TEST_PASS;
	}

	Emitter* TestClass::CreateTestEmitter(System& system, s32 start, s32 end)
	{
		Emitter* emitter = new Emitter(&system, testEffect, Vector3(0.0f, 0.0f, 0.0f), 0, 0.0f);
		emitter->mParticulates.mFirstIndex = start;
		emitter->mParticulates.mIndexAfterLast = end;
		for(int i = start; i < end; i++)
		{
			testEffect->InitializeParticulate(&system.mBuffer[i], 0.0f, Vector3(0.0f, 0.0f, 0.0f));
		}
		system.ResortEmitter(emitter);
		return emitter;
	}
	int TestClass::ForceParticulateAllocation(Emitter* emitter, float time)
	{
		int index = emitter->AllocNewParticulate(time);
		if(index == -1)
			return index;
		testEffect->InitializeParticulate(&emitter->mSystem->mBuffer[index], 0.0f, Vector3(0.0f, 0.0f, 0.0f));
		return index;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// test code
	//////////////////////////////////////////////////////////////////////////////////////////////////

	char* TestClass::TestAlwaysZero()
	{
		FloatRange fr(0.0f, 0.0f);
		TEST_ASSERTION(fr.AlwaysZero());
		FloatRange fr2(0.0f, 1.0f);
		TEST_ASSERTION(!fr2.AlwaysZero());
		FloatRange fr3(-1.0f, 0.0f);
		TEST_ASSERTION(!fr3.AlwaysZero());
		return TEST_PASS;
	}
	char* TestClass::TestRandomRange()
	{
		FloatRange fr(2.0f, 3.0f);
		float last = 0.0f;
		for(int i = 0; i < 100; i++)
		{
			float t = fr.Random();
			TEST_ASSERTION(t >= 2.0f);
			TEST_ASSERTION(t < 3.0f);
			TEST_ASSERTION(t != last);
			last = t;
		}
		return TEST_PASS;
	}
	char* TestClass::TestEmitterCreation()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 0);
		TEST_ASSERTION(((Emitter*)emitter) == mySystem.mEmitters[0]);
		TEST_RESULTS(VerifyEmitterIsEmpty(emitter));
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_RESULTS(VerifyEmitterCount(mySystem, 1));
		delete emitter;
		return TEST_PASS;
	}
	char* TestClass::TestEmitterCreation2()
	{
		System mySystem;
		Emitter* emitters[100];
		for(int i = 0; i < 100; i++)
		{
			emitters[i] = CreateTestEmitter(mySystem, 0, 0);
			TEST_RESULTS(VerifyEmitterIsEmpty(emitters[i]));
			TEST_RESULTS(VerifyEmitterListValid(mySystem));
			TEST_RESULTS(VerifyEmitterCount(mySystem, i+1));
		}
		for(int i = 0; i < 100; i++)
			delete emitters[i];

		TEST_RESULTS(VerifyEmitterCount(mySystem, 0));

		return TEST_PASS;
	}

	char* TestClass::TestEmitterDeath()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 0);
		Emitter* emitter2 = CreateTestEmitter(mySystem, 0, 0);

		TEST_RESULTS(VerifyEmitterIsEmpty(emitter));
		TEST_RESULTS(VerifyEmitterIsEmpty(emitter2));
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_RESULTS(VerifyEmitterCount(mySystem, 2));

		float t1 = Math::Min(emitter->mDeathTime, emitter2->mDeathTime);
		float t2 = Math::Max(emitter->mDeathTime, emitter2->mDeathTime);
		Update(t1);
		TEST_RESULTS(VerifyEmitterCount(mySystem, 1));
		Update(t2);
		TEST_RESULTS(VerifyEmitterCount(mySystem, 0));

		delete emitter;
		delete emitter2;

		return TEST_PASS;
	}

	char* TestClass::TestParticulateSpawning()
	{
		System mySystem;
		testEffect->mEmitterLifetimeRange.Set(3.5f, 3.5f);
		testEffect->mLifetimeRange.Set(2.0f, 2.0f);
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 0);
		TEST_RESULTS(VerifyEmitterIsEmpty(emitter));
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_RESULTS(VerifyEmitterCount(mySystem, 1));
		float time;
		int partcount = 0;
		Update(time = 0.001f);
		int dir = 1;
		while(mySystem.mEmitters.Count() == 1)
		{
			int count = emitter->Count();
			if(count > partcount)
				TEST_ASSERTION3(dir == 1, count, partcount);
			if(count < partcount && dir == 1)
			{
//				dbgPrint("Reversing on %d < %d\n", count, partcount);
				dir = -1;
			}
			partcount = count;

//			dbgPrint("ParticulateCount: %d\n", partcount);
			TEST_RESULTS(VerifyEmitterListValid(mySystem));
			Update(time += 1.001f);
		}
		delete emitter;
		return TEST_PASS;
	}

	char* TestClass::TestParticulateAllocationUpwards()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) == 1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) == 2);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) == 3);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) == 4);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		return TEST_PASS;
	}

	char* TestClass::TestParticulateAllocationDownwards()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-1, MAX_PARTICULATES_PER_SYSTEM);
		int index;
		TEST_ASSERTION2((index = ForceParticulateAllocation(emitter)) == MAX_PARTICULATES_PER_SYSTEM-2, index);
		TEST_ASSERTION2((index = ForceParticulateAllocation(emitter)) == MAX_PARTICULATES_PER_SYSTEM-3, index);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) == MAX_PARTICULATES_PER_SYSTEM-4);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) == MAX_PARTICULATES_PER_SYSTEM-5);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationMoveDown()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-100, MAX_PARTICULATES_PER_SYSTEM);
		Emitter* emitter2 = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM, MAX_PARTICULATES_PER_SYSTEM);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationMoveDown2()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-100, MAX_PARTICULATES_PER_SYSTEM-1);
		Emitter* emitter2 = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-1, MAX_PARTICULATES_PER_SYSTEM);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationMoveDown3()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-100, MAX_PARTICULATES_PER_SYSTEM-50);
		Emitter* emitter2 = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-50, MAX_PARTICULATES_PER_SYSTEM-1);
		Emitter* emitter3 = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-1, MAX_PARTICULATES_PER_SYSTEM);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter3) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter3) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter3) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter3) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		delete emitter3;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationMoveUp()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 0);
		Emitter* emitter2 = CreateTestEmitter(mySystem, 0, 100);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationMoveUp2()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 1);
		Emitter* emitter2 = CreateTestEmitter(mySystem, 1, 100);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationMoveUp3()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 1);
		Emitter* emitter2 = CreateTestEmitter(mySystem, 1, 100);
		Emitter* emitter3 = CreateTestEmitter(mySystem, 100, 200);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		delete emitter3;
		return TEST_PASS;
	}
	char* TestClass::TestEmitterRelocation()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 100);
		s32 oursize = emitter->Count();
		ParticulateSet gap1, gap2, gap3;
		gap1.mFirstIndex = 100;
		gap1.mIndexAfterLast = 200;
		gap2.mFirstIndex = 300;
		gap2.mIndexAfterLast = 401;
		gap3.mFirstIndex = 501;
		gap3.mIndexAfterLast = 602;
		emitter->Relocate(gap1);
		TEST_ASSERTION(emitter->mParticulates.mFirstIndex == gap1.mFirstIndex);
		TEST_ASSERTION(emitter->mParticulates.mIndexAfterLast == gap1.mIndexAfterLast);
		TEST_ASSERTION(emitter->Count() == oursize);
		emitter->Relocate(gap2);
		TEST_ASSERTION(emitter->mParticulates.mFirstIndex >= gap2.mFirstIndex);
		TEST_ASSERTION(emitter->mParticulates.mIndexAfterLast <= gap2.mIndexAfterLast);
		TEST_ASSERTION(emitter->Count() == oursize);
		emitter->Relocate(gap3);
		TEST_ASSERTION(emitter->mParticulates.mFirstIndex >= gap3.mFirstIndex);
		TEST_ASSERTION(emitter->mParticulates.mIndexAfterLast <= gap3.mIndexAfterLast);
		TEST_ASSERTION(emitter->Count() == oursize);
		delete emitter;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationPushDown()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-101, MAX_PARTICULATES_PER_SYSTEM-100);
		Emitter* emitter2 = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-100, MAX_PARTICULATES_PER_SYSTEM);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationPushUp()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 100);
		Emitter* emitter2 = CreateTestEmitter(mySystem, 100, 101);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationVeryFull1()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, 0, 1);
		Emitter* emitter2 = CreateTestEmitter(mySystem, 1, MAX_PARTICULATES_PER_SYSTEM/3);
		Emitter* emitter3 = CreateTestEmitter(mySystem, emitter2->mParticulates.mIndexAfterLast, emitter2->mParticulates.mIndexAfterLast + MAX_PARTICULATES_PER_SYSTEM/3);
		Emitter* emitter4 = CreateTestEmitter(mySystem, emitter3->mParticulates.mIndexAfterLast, MAX_PARTICULATES_PER_SYSTEM-4);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		delete emitter3;
		delete emitter4;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationVeryFull2()
	{
		System mySystem;
		Emitter* emitter = CreateTestEmitter(mySystem, MAX_PARTICULATES_PER_SYSTEM-1, MAX_PARTICULATES_PER_SYSTEM);
		Emitter* emitter2 = CreateTestEmitter(mySystem, 4, MAX_PARTICULATES_PER_SYSTEM/3);
		Emitter* emitter3 = CreateTestEmitter(mySystem, emitter2->mParticulates.mIndexAfterLast, emitter2->mParticulates.mIndexAfterLast + MAX_PARTICULATES_PER_SYSTEM/3);
		Emitter* emitter4 = CreateTestEmitter(mySystem, emitter3->mParticulates.mIndexAfterLast, MAX_PARTICULATES_PER_SYSTEM-1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_ASSERTION(ForceParticulateAllocation(emitter2) != -1);
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		delete emitter;
		delete emitter2;
		delete emitter3;
		delete emitter4;
		return TEST_PASS;
	}
	char* TestClass::TestParticulateAllocationFull()
	{
		System mySystem;
		const int testcount = 30;
		Emitter* emitter[testcount];
		int i;

		for(i = 0; i < testcount; i++)
		{
			emitter[i] = CreateTestEmitter(mySystem, 0, 0);
			TEST_RESULTS(VerifyEmitterIsValid(mySystem, emitter[i]));
		}
		TEST_RESULTS(VerifyEmitterListValid(mySystem));
		for(i = 0; i < MAX_PARTICULATES_PER_SYSTEM; i++)
		{
			TEST_ASSERTION(ForceParticulateAllocation(emitter[i % testcount]) != -1);
		}
		TEST_ASSERTION(ForceParticulateAllocation(emitter[0]) == -1);

		for(i = 0; i < testcount; i++)
		{
			delete emitter[i];
		}
		return TEST_PASS;
	}
	char* TestClass::TestLargestGap()
	{
		System mySystem1;
		Emitter *emitter1 = CreateTestEmitter(mySystem1, 300,	MAX_PARTICULATES_PER_SYSTEM-1);
		Emitter *emitter2 = CreateTestEmitter(mySystem1, 1,	100);
		Emitter *emitter3 = CreateTestEmitter(mySystem1, 105,	200);
		Emitter *emitter4 = CreateTestEmitter(mySystem1, 202,	300);
		ParticulateSet set = mySystem1.GetLargestGap();
		TEST_ASSERTION3(set.mFirstIndex == 100 && set.mIndexAfterLast == 105, set.mFirstIndex, set.mIndexAfterLast);
		delete emitter1;
		delete emitter2;
		delete emitter3;
		delete emitter4;


		System mySystem2;
		emitter1 = CreateTestEmitter(mySystem2, 300,	MAX_PARTICULATES_PER_SYSTEM-1);
		emitter2 = CreateTestEmitter(mySystem2, 1,	100);
		emitter3 = CreateTestEmitter(mySystem2, 102,	200);
		emitter4 = CreateTestEmitter(mySystem2, 205,	300);
		set = mySystem2.GetLargestGap();
		TEST_ASSERTION3(set.mFirstIndex == 200 && set.mIndexAfterLast == 205, set.mFirstIndex, set.mIndexAfterLast);
		delete emitter1;
		delete emitter2;
		delete emitter3;
		delete emitter4;


		System mySystem3;
		emitter1 = CreateTestEmitter(mySystem3, 300,	MAX_PARTICULATES_PER_SYSTEM-5);
		emitter2 = CreateTestEmitter(mySystem3, 1,	100);
		emitter3 = CreateTestEmitter(mySystem3, 102,	200);
		emitter4 = CreateTestEmitter(mySystem3, 202,	300);
		set = mySystem3.GetLargestGap();
		TEST_ASSERTION3(set.mFirstIndex == MAX_PARTICULATES_PER_SYSTEM-5 && set.mIndexAfterLast == MAX_PARTICULATES_PER_SYSTEM, set.mFirstIndex, set.mIndexAfterLast);
		delete emitter1;
		delete emitter2;
		delete emitter3;
		delete emitter4;


		System mySystem4;
		emitter1 = CreateTestEmitter(mySystem4, 300,	MAX_PARTICULATES_PER_SYSTEM-1);
		emitter2 = CreateTestEmitter(mySystem4, 5,	100);
		emitter3 = CreateTestEmitter(mySystem4, 102,	200);
		emitter4 = CreateTestEmitter(mySystem4, 202,	300);
		set = mySystem4.GetLargestGap();
		TEST_ASSERTION3(set.mFirstIndex == 0 && set.mIndexAfterLast == 5, set.mFirstIndex, set.mIndexAfterLast);
		delete emitter1;
		delete emitter2;
		delete emitter3;
		delete emitter4;
		return TEST_PASS;
	}

	void TestClass::Run()
	{
		int goodcount = 0;
		int badcount = 0;

		// The tests assume at least this.
		ASSERT(MAX_PARTICULATES_PER_SYSTEM >= 602);

		RUN_TEST(TestRandomRange);
		RUN_TEST(TestAlwaysZero);
		RUN_TEST(TestEmitterCreation);
		RUN_TEST(TestEmitterCreation2);
		RUN_TEST(TestEmitterDeath);
		RUN_TEST(TestParticulateSpawning);
		RUN_TEST(TestLargestGap);
		RUN_TEST(TestParticulateAllocationUpwards);
		RUN_TEST(TestParticulateAllocationDownwards);
		RUN_TEST(TestParticulateAllocationMoveDown);
		RUN_TEST(TestParticulateAllocationMoveUp);
		RUN_TEST(TestEmitterRelocation);
		RUN_TEST(TestParticulateAllocationMoveDown2);
		RUN_TEST(TestParticulateAllocationMoveUp2);
		RUN_TEST(TestParticulateAllocationMoveDown3);
		RUN_TEST(TestParticulateAllocationMoveUp3);
		RUN_TEST(TestParticulateAllocationPushDown);
		RUN_TEST(TestParticulateAllocationPushUp);
		RUN_TEST(TestParticulateAllocationFull);
		RUN_TEST(TestParticulateAllocationVeryFull1);
		RUN_TEST(TestParticulateAllocationVeryFull2);

		dbgPrint("Ran %d tests.  %d passed, %d failed.  %d assertions checked\n", goodcount+badcount, goodcount, badcount, assertioncount);
	}
};

