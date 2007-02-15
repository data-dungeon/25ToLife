//============================================================================
//=
//= OccluderMgr - contains all visual occluder planes in the world.
//=               used for trimming instances out of the display pipeline
//=               which are obscured.
//=
//============================================================================

#ifndef _OCCLUDER_MGR_H
#define _OCCLUDER_MGR_H

#include "Math/plane.h"

//============================================================================

class COccluder
{
public:
	COccluder( Vector3CRef min, Vector3CRef max, Graphics4x4& orient );
	void Setup( Vector3CRef cam );
	bool IsOccluded( Vector3CRef point ) const;
	bool IsOccluded( CInstance* instance ) const;
	void Draw();

	const CPlane& GetPlane(int index) const;
private:
	CPlane	d_plane[ 5 ];
	Vector3	d_point[ 4 ];
	//Vector3 d_cameraPos;
};

//============================================================================

class COccluderMgr
{
public:
	COccluderMgr();
	void Reset();
	void Add( const COccluder& occluder );
	void Draw();
	void Setup( Vector3CRef cameraPos );
	bool IsOccluded( CInstance* instance );

private:
	DynamicArray<COccluder> d_occluders;

	#ifdef PS2
	bool PS2OptimizedIsOccluded( const ts_BoundingSphere& sphere );
	void SetupPS2Optimization(void);
	Vector4 dat[256];
	#endif
};

//============================================================================

#endif //_OCCLUDER_MGR_H
