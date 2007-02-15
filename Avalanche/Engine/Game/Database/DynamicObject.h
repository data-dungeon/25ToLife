#ifndef __DYNAMIC_OBJECT_H__
#define __DYNAMIC_OBJECT_H__

#include <string.h>
#include "Game/Misc/Bounds.h"
#include "Game/Database/Instance.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Memory.h"
#include "Game/Managers/SceneMgr.h"

#define DYNAMIC_NAME_LENGTH 20
#define VERYLARGENUMBER 100000.0f

// pure virtual dynamic object class... used for wrapping an instance around a dynamic object.
class DynamicObject
{
public:
	// virtual destructor is a requirement..
	virtual ~DynamicObject() {}

	// called by Instance__Render()
	virtual void Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist) = 0L;
	virtual void RenderShadow(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist) = 0L;
	virtual void RenderOffScreen(const CInstance* pInstance) = 0L;

	virtual void UpdateName(CInstance* pInstance) { pInstance->SetName( "" ); }
	virtual void UpdateBoundingVolume(CInstance* pInstance) = 0L;
	virtual void UpdateBoundingVolume(CInstance* pInstance, float scale) = 0L;
	virtual void UpdateBoundingVolumeAccurate(CInstance* pInstance) = 0L;
	virtual ts_BoundingBox& GetBoundingBox(void) = 0L;
};

// A class that handles bounding volumes.
class GenericDynamicObject : public DynamicObject
{
public:
	GenericDynamicObject() {Name[0] = '\0';}
	virtual void UpdateName(CInstance* pInstance);
	virtual void UpdateBoundingVolume(CInstance* pInstance);
	virtual void UpdateBoundingVolume(CInstance* pInstance, float scale);
	virtual void UpdateBoundingVolumeAccurate(CInstance* pInstance);
	virtual ts_BoundingBox& GetBoundingBox(void) {return(BoundingBox);}

	virtual void RenderShadow(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist) {}
	virtual void RenderOffScreen(const CInstance* pInstance) {}

	ts_BoundingSphere BoundingSphere;
	ts_BoundingBox BoundingBox;

	char Name[DYNAMIC_NAME_LENGTH];
};

// a class that handles bounding volume and has some support for 
// handling the instance for you.
class InstancedDynamicObject : public GenericDynamicObject
{
public:
	// assumes identity if you pass NULL for mLocalToWorld
	// assumes VERY large radius sphere if you dont pass a sphere...
	// has some default flags.
	// name is for debugging...
	InstancedDynamicObject(
		char* name,
		ts_BoundingSphere* boundingSphere = NULL, 
		const Graphics4x4 *LocalToWorld = NULL,
		bool disableVis = true);

	virtual ~InstancedDynamicObject();

public:
	CInstance* mInstance;
};

inline void GenericDynamicObject::UpdateName(CInstance* pInstance)
{
	pInstance->SetName( Name );
}

inline void GenericDynamicObject::UpdateBoundingVolume(
CInstance* pInstance)
{
	// Xform the sphere center into world coords.
	pInstance->BoundingSphere().Center = BoundingSphere.Center *
			pInstance->Matrix();

	// Make sure the radius has been initialized.
	ASSERT((pInstance->BoundingSphere().Radius > 0.0f) &&
			(pInstance->BoundingSphere().Radius < 10000.0f));

	// Assume that instance's radius is set correctly and use it to construct
	// a box that encloses the sphere. The model's radius cannot be used since
	// it does not account for scaling.
	pInstance->BoundingBox().Min = 
		pInstance->BoundingSphere().Center -
		pInstance->BoundingSphere().Radius;

	pInstance->BoundingBox().Max = 
		pInstance->BoundingSphere().Center +
		pInstance->BoundingSphere().Radius;
}

inline void GenericDynamicObject::UpdateBoundingVolume(
CInstance* pInstance,
float scale)
{
	// Compute the instance's radius
	pInstance->BoundingSphere().Radius = BoundingSphere.Radius * scale;
	UpdateBoundingVolume(pInstance);
}

inline void GenericDynamicObject::UpdateBoundingVolumeAccurate(
CInstance* pInstance)
{
	// transform 8 corners of model's bounding box into world space
	BoundingBox__Transform(&BoundingBox, &(pInstance->BoundingBox()),
			&(pInstance->Matrix()));

	// calculate bounding sphere that encloses this box
	pInstance->BoundingSphere().Center = BoundingBox__GetCenter(&(pInstance->BoundingBox()));

	// radius of instance bounding sphere is distance from center to one
	// corner of bounding box
	pInstance->BoundingSphere().Radius = (pInstance->BoundingSphere().Center -
			pInstance->BoundingBox().Min).Length();
}

inline InstancedDynamicObject::InstancedDynamicObject(
	char* name, 
	ts_BoundingSphere* boundingSphere, // = NULL, 
	const Graphics4x4 *LocalToWorld, // = NULL,
	bool disableVis) // = false)
{
	// Allocate all the data
	MEM_SET_ALLOC_NAME(name);
	mInstance = new CInstance;
	ASSERT(mInstance);

	MEM_SET_ALLOC_NAME(name);

	strcpy(Name, name);
	mInstance->SetName( name );

	if(boundingSphere)
		BoundingSphere = *boundingSphere;
	else
	{
		BoundingSphere.Center.Clear();
		BoundingSphere.Radius = VERYLARGENUMBER;
	}
	
	BoundingBox.Min = BoundingSphere.Center - BoundingSphere.Radius;
	BoundingBox.Max = BoundingSphere.Center + BoundingSphere.Radius;

	mInstance->InitFromDynamicObject( this );
	mInstance->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 1);

	// Assume alpha
	mInstance->SetAlphaSort( false );
	mInstance->SetZWriteDisabled( true );
	mInstance->SetReceiveShadows( false );
	mInstance->SetDynamicallyLit( false );
	mInstance->SetFogDisabled( true );
	
	// matrix
	if(LocalToWorld)
		mInstance->Matrix() = *LocalToWorld;

	// calculate initial bounding data
	mInstance->UpdateBoundingVolumesAccurate();
	SceneMgr::AddInstance(mInstance);
	SceneMgr::SetInstanceVisRegionStatic(mInstance, true);
}

// ok, so it will never inline... it avoids the need for a source file.. :)
inline InstancedDynamicObject::~InstancedDynamicObject()
{
	SceneMgr::RemoveInstance(mInstance);
	delete mInstance;
}

#endif //__DYNAMIC_OBJECT_H__
