//
// Author: Tyler Colbert
// Purpose: manages a set of related Water squares.
//

#ifndef __WATERPOOL_H__
#define __WATERPOOL_H__

#include "container/dynlist.h"

#include "Effects/Water/Water.h"

#ifdef ENABLE_WATCHCLIENT
#include "WatchClient/WatchClient.h"
#endif

class Graphics4x4;
struct WaterParams;
typedef DynList<Water> WaterArray;

class WaterEquation;
class WaterAffector;
class WaterSprite;
typedef DynList<WaterEquation> WaterEquationArray;
typedef DynList<WaterAffector> WaterAffectorArray;
typedef DynList<WaterSprite> WaterSpriteArray;

class WaterPool : public WaterArray
{
public:
	// must be public for WaterMgr to work... dont call this yourself
	// you'll leave a dangling pointer.
	~WaterPool(void);

	////////////////////////////////////////////////////////////////////////////
	// Adding and removing sub-modules
	////////////////////////////////////////////////////////////////////////////

	// when you add an affector, you KEEP ownership of the affector.
	// ie. when ~Water() happens, it WILL NOT DELETE its affectors.
	void AddAffector(WaterAffector* affector);
	// when you remove an affector, you re-gain ownership of it.  ie,
	// Water does not delete it, you must.  It is uncommon to remove an
	// affector.
	void RemoveAffector(WaterAffector* affector);

	// adding new water sprites...
	void AddWaterSprite(WaterSprite* sprite);

	// rings
	void AddRippleRing(Vector3 pos);

	// wakes
	void AddWakePoint(Vector3 pos, Vector3 vel, float scale);

	// Surface Height (used for general changes in entire surface height over time..
	void ChangeSurfaceHeight(float targetHeight, float metersPerSecond) { mSurfaceHeightTarget = targetHeight; mSurfaceHeightVelocity = metersPerSecond; }
	float CurrentSurfaceHeight() { return mSurfaceHeight; }

	// TODO: create a RemoveWaterSprite()

	// add bumps to the water...
	void AddSegmentBump(float x, float z, float range, float height, float endx, float endz);
	void AddSemiCircleBump(float x, float z, float range, float height, float radius, float startAngle, float endAngle);


	// Insert a new equation into the evaluator like an affector, when
	// you add it to the water, you loose ownership of it.  ~Water()
	// will delete it for you...
	void AddEquation(WaterEquation *eq, bool Dampen = true);

	// remove an equation from the evaluator, like affectors, when
	// removed you regain ownership of the equation and the water
	// reliquishes all responsibility for it. This call should be
	// uncommon.
	void RemoveEquation(WaterEquation* eq);

	// apply affectors motion to the cells
	void ApplyAffectorsForce(void);

	// some other internal routines
	void UpdateAffectorStates(void);

	// test against all our water cells
	// return the first hit..
	bool Test(Segment &seg, Vector3* point);

	// get to the various arrays...
	WaterEquationArray& Equations(void) { return mEquations; }
	WaterSpriteArray& Sprites(void) { return mSprites; }
	WaterAffectorArray& Affectors(void) { return mAffectors; }

	static void PathFile(const char *path,const char *file,char *pathFile);

	float XScale(void) { return mXScale; }
	float ZScale(void) { return mZScale; }
	float InvXScale(void) { return mInvXScale; }
	float InvZScale(void) { return mInvZScale; }
	const Vector3& Position(void) { return mPosition; }
	float Angle(void) { return mAngle; }
	const Graphics4x4& WorldToLocalBasis(void) { return mWorldToLocalBasis; }
	const Graphics4x4& WorldToLocal(void) { return mWorldToLocal; }
	const Graphics4x4& LocalToWorld(void) { return mLocalToWorld; }

	float T(void) { return mT; }

	int	mPoolNumber;			// pool index
	int	mWaterLayersUsed;		// texture layers needed without sprites

	// store sprites that we have...
	WaterSpriteArray mSprites;			// PAB moved out to access from ANI

protected:
	friend class Water;
	friend class WaterMgr;

	WaterPool(void);

	// initialize ourselves off of this file.
	bool Create(const char *directory, const char *poolFileName);

	// set params to default
	static void Defaults(WaterParams &params);
	
	// update all our squares...
	void Update(float deltaTime, Graphics4x4& cameraMatrix);

	// initialize ourselves with this data
	// the name is for the watch client.
	bool Create(const WaterParams& params, const char* name);

	// GC only versions.. using old water system
	bool Create(int gc, const WaterParams& params, const char *name);		// pab
	bool Create(int gc, const WaterParams& params, const char *name, int x, int z);		// pab


	bool CreateWater(const char* poolFileName, const WaterParams& params, int x, int z);

	static bool LoadScript(const char *directory,const char *scriptName,WaterParams &params);
	static void SetupVariables(const char *context, WaterParams &params);
	static char* varName(char *base, int index, char* name);
	static char* varName(char *base, int index1, int index2, char* name);

	void UpdateWakePoints(float deltaTime);

	void ExpireEquations(float deltaTime);


	/////////////////////////////
	// OBB
	/////////////////////////////
	OBBInstance mObbWithoutSurfaceHeight;
	OBBInstance mObb;
	AABB mTrueAABB;

	/////////////////////////////
	// equations
	/////////////////////////////

	// store equations that have been added to us.
	WaterEquationArray mEquations;
	float mEquationFlowRateX;
	float mEquationFlowRateZ;

	/////////////////////////////
	// rings
	/////////////////////////////
	struct WaterRing
	{
		Vector2 mCenter;
		float mStartTime;
	};
	WaterRing mRings[MAX_RINGS];
	float mRingAttack;
	float mRingSustain;
	float mRingRelease;
	float mRingGrowRate;
	float mRingWaveLength;
	float mRingWaveStartPosition;
	float mRingWaveHeight;
	float mRingWaveCount;
	float mRingWavePhaseShift;
	float EvaluateWaterRings(float t, float x, float z);


	/////////////////////////////
	// surface height
	/////////////////////////////
	float mSurfaceHeight;
	float mSurfaceHeightTarget;
	float mSurfaceHeightVelocity;


	/////////////////////////////
	// affectors
	/////////////////////////////

	// store affectors that have been added to us.
	WaterAffectorArray mAffectors;


	/////////////////////////////
	// sprites
	/////////////////////////////


	float mSpriteFlowX;
	float mSpriteFlowZ;
	friend class WaterSprite;

	void ExpireWaterSprites(float deltaTime);

	/////////////////////////////
	// time
	/////////////////////////////
	float mT;
	float mDeltaT;

	/////////////////////////////
	// placement/size info
	/////////////////////////////
	float mXScale, mZScale;
	float mInvXScale, mInvZScale;
	Vector3 mPosition;
	float mAngle;

	Graphics4x4 mWorldToLocalBasis;
	Graphics4x4 mLocalToWorldBasis;
	Graphics4x4 mWorldToLocal;
	Graphics4x4 mLocalToWorld;

	/////////////////////////////
	// noise
	/////////////////////////////
	float EvaluateNoise(float t, float x, float z);
	Vector3 mNoiseScale;
	float mNoiseTScale;

	/////////////////////////////
	// Physics
	/////////////////////////////
	float mPhysicsForce;
	float mPhysicsTrailAffector;

	/////////////////////////////
	// wakes
	/////////////////////////////
	float EvaluateWakes(float t, float x, float z);
	struct WakePoint
	{
		Vector2 pos;
		Vector2 vel;
		float lifeLeft;
		float scale;
	};
	WakePoint* mWakePoints;
	int mWakeIndex;
	int mWakePointCount;
	float mWakeRadiusSquared, mInvWakeRadiusSquared;
	float mWakeHeight;
	float mWakeLifetime, mInvWakeLifetime;
	float mWakeForwardContribution;
	float mWakePerpendicularContribution;
	float mWakeMinSpeedRatio;
	float mWakeTrailAffector;
	float mWakeDelay;
	bool mHasSomeLiveWakes;

	/////////////////////////////
	// identification
	/////////////////////////////
	char* mName;
};

#endif //__WATERPOOL_H__
