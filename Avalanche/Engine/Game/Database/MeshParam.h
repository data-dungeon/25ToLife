/*
**
**  File:   MeshParam.h
**  Date:   May 6, 2002
**  By:     Bryant Collard
**  Desc:   Interactive mesh parameters.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshParam.h $
**  $Revision: 8 $
**      $Date: 8/22/03 5:57p $
**    $Author: Bcollard $
**
*/

#ifndef MESH_PARAM_H
#define MESH_PARAM_H

// Engine includes.
#include "Math/Vector.h"

// GameLib includes.
#include "platform/BaseType.h"
#include "GameHelper/lod.h"

// Forward declarations.
class CMeshInstance;
class CMeshParam;

#define CONTEXT_NAME_LENGTH    9

class CMeshParamInstance
{
  public:
	CMeshParamInstance();
  private:
	CMeshInstance* d_mesh;
	CMeshParamInstance* d_instancePrev;
	CMeshParamInstance* d_instanceNext;

	friend class CMeshParam;
};

class CMeshParam
{
  public:
	CMeshParam();
	~CMeshParam();
	static CMeshParam* Register(CMeshInstance* i_mesh, const char* i_context,
			const char* i_varPath);
	static void Unregister(CMeshInstance* i_mesh);
  private:
	void CreateContext(const char* i_context, const char* i_varPath);
	bool HasMesh(CMeshInstance* i_mesh);
	void AddMesh(CMeshInstance* i_mesh);
	void RemoveMesh(CMeshInstance* i_mesh);
/*
 *	Control flag stuff.
 */
	typedef uint8 TControlFlag;
	static const TControlFlag MESH_NO_UPDATE;
	static const TControlFlag MESH_UPDATE;
	static const TControlFlag MESH_DYNAMIC;
	static const TControlFlag MESH_SNAP_TO_INIT;
	static const TControlFlag MESH_DECAY_TO_INIT;
	static const TControlFlag MESH_APPLY_GRAVITY;
	static const TControlFlag MESH_APPLY_DRAG;
	static const TControlFlag MESH_CHECK_OBJECT;
	static const TControlFlag MESH_CONSTRAINT_DIR;
	static const TControlFlag MESH_COMPUTE_NORMALS;
	static const TControlFlag MESH_UPDATE_SUB_MESH;

	static CMeshParam* d_paramHead;
	CMeshParam* d_paramPrev;
	CMeshParam* d_paramNext;
	CMeshParamInstance* d_instanceHead;
	char d_context[CONTEXT_NAME_LENGTH];
	int d_refCount;

	TControlFlag d_controlFlag[CBaseLOD::NUM_LODS];
	float d_iterationRate;
	float d_globalDamping;
	float d_decayTimeConstant;
	Vector3Packed d_gravity;
	float d_dragCoefficient;
	Vector3Packed d_constraintDir;

	friend class CMeshInstance;
};

#endif // MESH_PARAM_H
