//
// Author: Tyler Colbert
// Purpose: This is a part of the Water class.  Please see the Water.h header for details.  This
//    module represents the interactive part of the water calculations (internally called the water's
//    physics, although that might be a bit of a misnomer).
//

#include "Effects/EffectsPCH.h"


#include "Effects/Water/Water.h"


///////////////////////////////////////////////////////////////////////////////////////
// Water - Physics
///////////////////////////////////////////////////////////////////////////////////////

void Water::EnablePhysics(bool enabled)
{
	// cant turn on physics if its not initialized!
	mPhysicsEnabled = (mPsi != NULL) && enabled;
}

bool Water::InitializePhysics(int subdivs, float physicsWaveSpeed, float physicsElasticity)
{
	// should we turn on?
	if(subdivs == 0) return false;

	mElasticity = physicsElasticity;

	// calculate grid sizes
	MEM_SET_ALLOC_NAME("Water physics");
	mPhysicsGrid.Init(subdivs, sizeof(float)*4, true);

	// allocate memory
	mPsi = (float *)mPhysicsGrid.mDataBlock;
	mPsiA = mPsi + mPhysicsGrid.mGridNodes;
	mPsiV = mPsiA + mPhysicsGrid.mGridNodes;
	mWaveSpeedSquared = mPsiV + mPhysicsGrid.mGridNodes;

	// set wavespeed & clear other physics data...
	physicsWaveSpeed *= physicsWaveSpeed;
	for(int i = 0; i < mPhysicsGrid.mGridNodes; i++)
	{
		mPsi[i] = 0.0f;
		mPsiA[i] = 0.0f;
		mPsiV[i] = 0.0f;
		mWaveSpeedSquared[i] = physicsWaveSpeed;
	}
	
	return true;
}

void Water::ApplyForce(const Vector3& point, float force)
{
	// Duh
	if(!mPhysicsEnabled)
		return;

	// Are they even inside of us?
	AABB &aabb = (AABB &)mInstance->BoundingBox();
	if (!aabb.PointIn(point))
		return;

	// Put the point into model space
	Vector3 modelPoint = point * mWorldToLocal;

	// Apply the force
	ApplyDisplacement(modelPoint, force);
}

void Water::ApplyDisplacement(Vector3& at, float amount)
{
	// find 4 closest points...
	int gridxl = (int)(at.x() * (mPhysicsGrid.mGridSizeMinus1));
	int gridzl = (int)(at.z() * (mPhysicsGrid.mGridSizeMinus1));
	int gridxh = gridxl + 1;
	int gridzh = gridzl + 1;

	// determine weights...
	float weightxh = (at.x() * (mPhysicsGrid.mGridSizeMinus1)) - (float)gridxl;
	float weightzh = (at.z() * (mPhysicsGrid.mGridSizeMinus1)) - (float)gridzl;
	float weightxl = 1.0f - weightxh;
	float weightzl = 1.0f - weightzh;

	ApplyDisplacement(gridxl, gridzl, amount * weightxl * weightzl);
	ApplyDisplacement(gridxh, gridzl, amount * weightxh * weightzl);
	ApplyDisplacement(gridxl, gridzh, amount * weightxl * weightzh);
	ApplyDisplacement(gridxh, gridzh, amount * weightxh * weightzh);
}

// test doink the water..
void Water::Doink(float force)
{
	Vector3 modelPoint;
	modelPoint.Set(0.5f, 0.5f, 0.5f);
	ApplyDisplacement(modelPoint, force);
}

void Water::Doink(const Vector3& point, float force)
{
	Vector3 modelPoint = point * mWorldToLocal;
	ApplyDisplacement(modelPoint, force);
}

void Water::ApplyDisplacement(int x, int z, float amount)
{
	// skip outside of bounds & edges as well...
	if(x < 1 || x >= (mPhysicsGrid.mGridSizeMinus1) || z < 1 || z >= (mPhysicsGrid.mGridSizeMinus1))
		return;

	mPsi[x + z * mPhysicsGrid.mGridSize] = amount;
}

void Water::CleanupPhysics(void)
{
	mWaveSpeedSquared = mPsiA = mPsiV = mPsi = NULL;
	mPhysicsEnabled = false;
}

float Water::PhysicsPsi(int x, int y, float& samples)
{
	if(x < 0 || x >= mPhysicsGrid.mGridSize || y < 0 || y >= mPhysicsGrid.mGridSize)
		return(0.0f);

	samples += 1.0f;
	
	return mPhysicsGrid(x, y);
}

float Water::PhysicsHeight(float x, float z)
{
	return mPhysicsGrid(x, z);
}

void Water::RunPhysics(float deltaT)
{
	// Code stolen from Food's anihf.cpp file...
	// and then a little modification to make it fit into place...

	// loop over all nodes and
	// Integrate acceleration from forces
	int x, y, index;
	for(y = 0, index = 0; y < mPhysicsGrid.mGridSize; y++)
	{
		for(x = 0; x < mPhysicsGrid.mGridSize; x++, index++)
		{
			// What is our multiplier?
			float samples = 0;

			// Apply forces
			mPsiA[index] += (PhysicsPsi(x - 2, y, samples) +
								  PhysicsPsi(x, y - 2, samples) +
								  PhysicsPsi(x + 2, y, samples) +
								  PhysicsPsi(x, y + 2, samples) +
								  PhysicsPsi(x - 1, y, samples) +
								  PhysicsPsi(x, y - 1, samples) +
								  PhysicsPsi(x + 1, y, samples) +
								  PhysicsPsi(x, y + 1, samples)) - samples * mPsi[index];
			mPsiA[index] *= mElasticity;
			mPsiV[index] = mPsiA[index] * deltaT;
		}
	}

	// Integrate height from acceleration
	for(y = 0, index = 0; y < mPhysicsGrid.mGridSize; y++)
	{
		for(x = 0; x < mPhysicsGrid.mGridSize; x++, index++)
		{
#ifdef DEBUG_WATER
			mPsi[index] += (Water_Dbg.mPhysicsScaleWaveSpeed + 1.0f) * mWaveSpeedSquared[index] * mPsiA[index] * deltaT;
			mPsi[index] *= Water_Dbg.mPhysicsDampeningValue + 1.0f;
#else
			mPsi[index] += mWaveSpeedSquared[index] * mPsiA[index] * deltaT;
#endif
		}
	}
}

