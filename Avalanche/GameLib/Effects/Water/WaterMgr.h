//
// Author: Tyler Colbert
// Purpose: manages multiple pools of water where a pool of water is a set of related Water squares.
//

#ifndef __WATERMGR_H__
#define __WATERMGR_H__

#include "container/dynlist.h"

#include "Effects/Water/WaterPool.h"
#include "Effects/Water/WaterSpriteEffect.h"

class Graphics4x4;
class Segment;
typedef DynList<WaterPool> WaterPoolArray;
typedef DynList<WaterSpriteEffect> WaterSpriteEffectArray;

class WaterMgr : public WaterPoolArray
{
public:
	WaterMgr(void);

	// create a new water pool based on the information in the given file/structure.
	WaterPool* Create(char *directory, char *poolFileName);
	WaterPool* Create(const WaterParams& params, const char* name);

	//Change the surface height of a specific pool
	void ChangeSurfaceHeight(int poolNumber, float targetHeight, float metersPerSecond);

	//Get the surface height of a specific pool
	float GetSurfaceHeight(int poolNumber);

	int GetNumSprites (int poolNumber);
	WaterPool *GetPool (int poolNumber);

	// default a params struct
	static void Defaults(WaterParams& params);
	static bool LoadScript(const char *directory,const char *scriptName,WaterParams &params);

	// call create for each waterpool[0-9]*.var files in a directory
	bool CreateAll(char *directory);

	// update all the pools
	void Update(Graphics4x4& cameraMatrix);

	// test against all the pools
	// just returns the first collision!
	// usually, for water, this is PLEANTY...
	// returns pool# we hit or -1 if no hit!
	int Test(Segment &seg, Vector3* point = NULL);

	// add an affector to all water pools
	void AddAffector(WaterAffector* affector);

	// spawn the named effect at the specified place in the given pool
	void SpawnSpriteEffect(int effectNumber, int poolNumber, const Vector3& position, const Vector3& velocity);

	// add a new ripple ring to the water pool
	void AddRippleRing(int poolNumber, Vector3 pos);

	// get a pool number from a name
	int PoolNumberFromName(char* name);

protected:
	WaterSpriteEffectArray mWaterSpriteEffects;
};

// There can be only one!
DefineSingleton(WaterMgr)
#define g_waterMgr GetSingleton(WaterMgr)


#endif // __WATERMGR_H__
