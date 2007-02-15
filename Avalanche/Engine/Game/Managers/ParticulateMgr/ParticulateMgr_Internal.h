// If you're not currently considering changing the ParticulateMgr,
// you shouldn't need any of the information in this file.
//
// THIS IS IN THE NAMESPACE 'ParticulateMgr'

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

extern DoublyLinkedList<System> sSystems;
extern DoublyLinkedList<Effect> sEffects;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

class Particulate
{
public:
	// Arranged for PS2 goodness... probably not a problem with the other platforms though.
	Vector3Packed mInitialPosition;
	float mInitialAngularPosition;

	Vector3Packed mInitialVelocity;
	float mInitialAngularVelocity;

	float mBirthTime;
	float mDeathTime;
	float mInitialScale;
	float UNUSED; // unsed padding for PS2 goodness...
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Good idea to create one of these and call Run() if you change the ParticulateMgr
// it tests pretty much all the code called by Update(), but none of Render()
class TestClass
{
	Effect *testEffect;
	Texture *testTexture;
	void StartTest();
	void StopTest();
	char* VerifyEmitterIsValid(System& system,Emitter* a);
	char* VerifyEmitterCount(System& system, int expectedCount);
	char* VerifyEmitterParticulateCount(Emitter* a, int count);
	char* VerifyEmitterIsEmpty(Emitter* a);
	char* VerifyEmittersAreSequential(Emitter* a, Emitter* b);
	char* VerifyEmittersDontCollide(Emitter* a, Emitter* b);
	char* VerifyEmitterListValid(System& system);
	Emitter* CreateTestEmitter(System& system, s32 start, s32 end);
	int ForceParticulateAllocation(Emitter* emitter, float time = 0.0f);
	char* TestAlwaysZero();
	char* TestRandomRange();
	char* TestEmitterCreation();
	char* TestEmitterCreation2();
	char* TestEmitterDeath();
	char* TestParticulateSpawning();
	char* TestParticulateAllocationUpwards();
	char* TestParticulateAllocationDownwards();
	char* TestParticulateAllocationMoveDown();
	char* TestParticulateAllocationMoveDown2();
	char* TestParticulateAllocationMoveDown3();
	char* TestParticulateAllocationMoveUp();
	char* TestParticulateAllocationMoveUp2();
	char* TestParticulateAllocationMoveUp3();
	char* TestParticulateAllocationPushDown();
	char* TestParticulateAllocationPushUp();
	char* TestParticulateAllocationVeryFull1();
	char* TestParticulateAllocationVeryFull2();
	char* TestParticulateAllocationFull();
	char* TestEmitterRelocation();
	char* TestLargestGap();
public:
	void Run();
};

