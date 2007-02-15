//
// Author: Tyler Colbert
// Purpose: manages multiple pools of water where a pool of water is a set of related Water squares.
//

#include "Effects/EffectsPCH.h"


#include "WaterMgr.h"
#include "WaterPool.h"
#include "Water.h"
#include "WaterAffector.h"
#include "WaterDBL.h"


#include "EngineHelper/timer.h"
#include "cmdcon/cmdcon.h"

DeclareSingleton(WaterMgr);

WaterMgr::WaterMgr()
	: mWaterSpriteEffects(1, 1, true) // start expecting 1 and grow 1 at a time..
{
}

void WaterMgr::Defaults(WaterParams& params)
{
	WaterPool::Defaults(params);
}

bool WaterMgr::LoadScript(const char *directory,const char *scriptName,WaterParams &params)
{
    return WaterPool::LoadScript(directory, scriptName, params);
}

WaterPool* WaterMgr::Create(const WaterParams& params, const char* name)
{
	WaterPool* pool = new WaterPool();
	if(!pool->Create(params, name))
	{
		delete pool;
		return NULL;
	}

	if(!Add(pool))
	{
		ASSERT(false);
		delete pool;
		return NULL;
	}

	#ifdef USE_ANIHF_WATER
	// store the name for later lookup.		mName already allocated
	strcpy( pool->mName, name );
	int	poolnum = g_waterMgr.PoolNumberFromName((char*)name);

	// find the animated height list version and plug in the pool number
	AnimatedHeightField *A = g_aniHFMgr.Find ((char*)name);
	if (A != NULL)
		A->mPoolNumber = poolnum;

	pool->mPoolNumber = poolnum;
	#endif

	return pool;
}

WaterPool* WaterMgr::Create(char *directory, char *poolFileName)
{
	// just pass it down...
	WaterPool* pool = new WaterPool();
	ASSERT(pool);

	if(!pool->Create(directory, poolFileName))
		return NULL;

	if(!Add(pool))
	{
		ASSERT(false);
		delete pool;
		return NULL;
	}

	#ifdef USE_ANIHF_WATER
	// store the name for later lookup.
	pool->mName = new char[strlen(poolFileName) + 1];
	strcpy( pool->mName, poolFileName );
	int	poolnum = g_waterMgr.PoolNumberFromName((char*)poolFileName);

	// find the animated height list version and plug in the pool number
	AnimatedHeightField *A = g_aniHFMgr.Find (poolFileName);

	if (A != NULL)
		A->mPoolNumber = poolnum;

	pool->mPoolNumber = poolnum;
	#endif

	return pool;
}

bool WaterMgr::CreateAll(char *directory)
{

	int i;

	// create all the effects..
	for(i = 0; i < 16; i++)
	{
		char script[32];
		sprintf(script, "watere%d", i);
		
		// does the script exist?
		char pathFile[256];
		WaterPool::PathFile("var", script, pathFile);
		if(!g_console.ScriptExists(pathFile))
			continue;

		// create the effect...
		WaterSpriteEffect* effect = new WaterSpriteEffect(script, "var");
		mWaterSpriteEffects.Add(effect);
	}

	// Create from DBL chunks.
	ts_RecordList *recordList = (ts_RecordList *)dblGetChunkData(DB_WATER, DEFAULT_CHUNK_KEY_STRING, g_loadSOM.GetWorldHandle());
	if (recordList)
	{
		for (uint i = 0; i < recordList->usRecordCount; i++)
		{
			ts_Water* water = (ts_Water*)GetRecordAddress(recordList, i);
			ASSERT(water);

			if (water)
			{
				// setup the water defaults
				WaterParams wparams;
				WaterMgr::Defaults(wparams);

				if (!WaterMgr::LoadScript(directory, water->varFile, wparams))
				{
					ASSERTF(0, ("Unable to load water var file: %s", water->varFile));
					continue;
				}

				// overwrite var file data with info from the instance
				wparams.pos = water->pos;
				wparams.cellSizeX = water->size.x();
				wparams.cellSizeZ = water->size.z();
				wparams.angle = water->angle;
				wparams.disableVisReg = true;

				float scaleU = wparams.cellSizeX / water->wrap;		// divide by wrap: wrap = wrap texture every x meters.
				float scaleV = wparams.cellSizeZ / water->wrap;

				for (int j = 0; j < MAX_LAYERS; j++)
				{
					if (strcmp(wparams.layers[j].mLayerType, "DISTORT") != 0)
					{
						wparams.layers[j].mScaleU *= scaleU;
						wparams.layers[j].mScaleV *= scaleV;
					}
				}

				// create the pool
				WaterPool* pool = g_waterMgr.Create(wparams, water->name);
				ASSERT(pool != 0);
			}
		}
	}
	else
	{
		// Check for scripts
		for (i = 0; i < 16; i++)
		{
			char script[32];
			sprintf(script, "waterp%d", i);
			
			// does the script exist?
			char pathFile[256];
			WaterPool::PathFile(directory, script, pathFile);
			if (!g_console.ScriptExists(pathFile))
				continue;
			
			// We should probably care about this return,
			// but what can we do?
			Create(directory, script);
		}
	}


	return(true);
}

void WaterMgr::Update(Graphics4x4& cameraMatrix)
{
	u32 count = Count();
	u32 idx;

	float deltaTime = g_timer.GetFrameSec( );

	for(idx = 0; idx < count; idx++)
	{
		Get(idx)->Update(deltaTime, cameraMatrix);
	}
}


int WaterMgr::Test(Segment &seg, Vector3* point)
{
	u32 count = Count();
	u32 idx;

	for(idx = 0; idx < count; idx++)
	{
		if(Get(idx)->Test(seg, point))
			return idx;
	}

	// we didnt collide
	return -1;
}

void WaterMgr::AddAffector(WaterAffector* affector)
{
	u32 count = Count();
	u32 idx;

	for(idx = 0; idx < count; idx++)
	{
		Get(idx)->AddAffector(affector);
	}
}

// add a new ripple ring to the water pool
void WaterMgr::AddRippleRing(int poolIdx, Vector3 pos)
{
	if(poolIdx < 0 || poolIdx >= g_waterMgr.Count())
	{
#ifndef USE_ANIHF_WATER
		ASSERTS(false, "poolIdx out of range");
#endif
		return;
	}
	Get(poolIdx)->AddRippleRing(pos);
}

// spawn the named effect at the specified place in the given pool
void WaterMgr::SpawnSpriteEffect(int effectNumber, int poolIdx, const Vector3& position, const Vector3& velocity)
{
	if(poolIdx < 0 || poolIdx >= g_waterMgr.Count())
	{
#ifndef USE_ANIHF_WATER
		ASSERTS(false, "poolIdx out of range");
#endif
		return;
	}
	if(effectNumber < 0 || effectNumber >= mWaterSpriteEffects.Count())
	{
		ASSERTS(false, "effectNumber out of range");
		return;
	}

	// create the effect
	mWaterSpriteEffects[effectNumber]->Trigger(poolIdx, position, velocity);

}


//Change the surface height of the specified pool
void WaterMgr::ChangeSurfaceHeight(int poolNumber, float targetHeight, float metersPerSecond)
{
	WaterPool* pool = Get(poolNumber);
	if(!pool)
	{
#ifndef USE_ANIHF_WATER
		ASSERTS(false, "poolNumber out of range");
#endif
		return;
	}

#ifndef USE_ANIHF_WATER
	ASSERTS_PTR(pool, "bad pool pointer");
#endif

	pool->ChangeSurfaceHeight(targetHeight, metersPerSecond);
}

int	WaterMgr::GetNumSprites (int poolNumber)
{
	WaterPool* pool = Get(poolNumber);
	if(!pool)
	{
#ifndef USE_ANIHF_WATER
		ASSERTS(false, "poolNumber out of range");
#endif
		return 0;
	}
#ifndef USE_ANIHF_WATER
	ASSERTS_PTR(pool, "bad pool pointer");
#endif
	
	return ( pool->Sprites().Count() );
}

WaterPool *WaterMgr::GetPool (int poolNumber)
{
	return ( Get(poolNumber) );
}


//Get the surface height of the specified
float WaterMgr::GetSurfaceHeight(int poolNumber)
{
	WaterPool* pool = Get(poolNumber);
	if(!pool)
	{
#ifndef USE_ANIHF_WATER
		ASSERTS(false, "poolNumber out of range");
#endif
		return -1.0f;
	}
#ifndef USE_ANIHF_WATER
	ASSERTS_PTR(pool, "bad pool pointer");
#endif

	return(pool->CurrentSurfaceHeight());
}

//Get a pool number from a pool name.
int WaterMgr::PoolNumberFromName(char* name)
{
	int count = Count();
	for (int i = 0; i < count; i++)
	{
		WaterPool* pool = Get(i);
		if (pool->mName && strcmp(pool->mName, name) == 0)
			return i;
	}

	return -1;
}
