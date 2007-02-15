/*
**
**  File:   MeshInstance.h
**  Date:   April 25, 2002
**  By:     Bryant Collard
**  Desc:   An instance of a mesh.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshInstance.h $
**  $Revision: 14 $
**      $Date: 8/22/03 5:57p $
**    $Author: Bcollard $
**
*/

#ifndef MESH_INSTANCE_H
#define MESH_INSTANCE_H

// Engine includes.
#include "Game/Database/DynamicObject.h"
#include "Game/Database/MeshSOM.h"
#include "Game/Database/MeshParam.h"
#include "Display/Common/VtxBuffer.h"
#include "Math/Vector.h"
#include "Game/Misc/Bounds.h"

// GameLib includes.
#include "platform/BaseType.h"
class OBB;
class CObjectList;
class Capsule;

// Forward declarations.
class CInstance;
struct ts_Bone;
class Graphics4x4;
class StripBuffer;
class CMeshLOD;

#ifndef PS2
#define VTX_BUFFER_NORMAL
#endif

class CMeshInstance : public DynamicObject
{
  public:
	CMeshInstance(SMeshSOM &i_som, CInstance &i_refInstance,
			CMeshLOD* i_lod, const char* i_varPath = NULL);
	~CMeshInstance();
	void SetLOD(CMeshLOD* i_lod) {if (i_lod) d_lod = i_lod;}
	void SetObjectList(CObjectList* i_objectList)
		{d_objectList = i_objectList;}
	void RecomputeConstraints(void);
	void Reset(void);
	bool Update(float i_deltaSec, bool i_inScene);
	virtual void Render(const CInstance* i_instance,
			const Graphics4x4* i_localToCamera, float i_zCamDist);
	virtual void RenderShadow(const CInstance* i_instance,
			const Graphics4x4* i_localToCamera, float i_zCamDist) {}
	virtual void RenderOffScreen(const CInstance* i_instance) {}
	void GetExtents(Vector3 &io_minExtent, Vector3 &io_maxExtent,
			bool i_compare = false);

	virtual void UpdateName(CInstance* pInstance);
	virtual void UpdateBoundingVolume(CInstance* pInstance);
	virtual void UpdateBoundingVolume(CInstance* pInstance, float scale);
	virtual void UpdateBoundingVolumeAccurate(CInstance* pInstance);
	virtual ts_BoundingBox& GetBoundingBox(void) {return(d_extents);}

	void SetDiffuseColor(uint32 i_diffuseColor);

	void DrawConstraints(void);
	static void DumpMeshSOM(SMeshSOM &i_meshSOM);
  private:
/*
 *	Prevent use of the default constructor.
 */
	CMeshInstance();
/*
 *	Define the structures used in the instance.
 *
 *	An instance of a particular vertex is stored in a vertex buffer.
 */
	struct SVertexInstance
	{
		VtxBuffer* d_subMesh;
		uint32 d_index;
	};
/*
 *	The dynamically changing node data.
 */
	struct SNodeInstance
	{
/*
 *		A pointer into the list of vertices. All vertices associated with a node
 *		are grouped together. The front side vertices are first then the back
 *		side vertices. This member points to the first vertex associated with
 *		the node. The first member of the array is used to store the current
 *		position of the node.
 */
		SVertexInstance* d_vertex;
/*
 *		The position of the node in the world at the end of the current
 *		time step.
 */
		Vector3 d_newNodeInWorld;
/*
 *		The velocity of the node.
 */
		Vector3 d_velocityInWorld;
/*
 *		The node's normal (temp until PS2 VtxBuffer can handle normals)
 */
#ifndef VTX_BUFFER_NORMAL
		Vector3 d_normalInWorld;
#endif
/*
 *		The thickness of the node in world coordinates. Thickness is a function
 *		of scaling in the reference instance to world matrix. It needs to be
 *		computed at when the mesh instance is created and whenever scaling of
 *		the reference instance is changed (not a good idea).
 */
		float d_thickness;
/*
 *		If the node is rigidly attached, a pointer to the bone in the reference
 *		instance that the node is rigidly attached to. NULL if the node is not
 *		attached to a bone but to the reference instance directly. This
 *		information could actually be determined from the master bone ID in the
 *		SOM. It is stored here to save a level of indirection and for
 *		convenience.
 */
		ts_Bone* d_master;
	};
/*
 *	A typedef for functions that get vectors from nodes.
 */
	typedef Vector3Packed& (CMeshInstance::*GetVectorFn)(uint i_node);
/*
 *	Private member functions.
 *
 *	Update node positions.
 */
	void UpdateSnapToInit(void);
	void UpdateDecayToInit(float i_deltaSec);
	void UpdateDynamic(float i_deltaSec,
			CMeshParam::TControlFlag i_controlFlag);
/*
 *	Determine new extents from new node positions.
 */
	void UpdateExtents(Vector3CRef i_point, bool i_compare);
/*
 *	Compute mesh info.
 */
	void ComputeConstraints(void);
	bool ComputeNormal(uint i_node, Vector3 &o_normal,
			GetVectorFn i_getVertexFn);
/*
 *	Propogate information to all sub-meshes.
 */
	void UpdateSubMesh(uint i_node);
/*
 *	Functions to enforce OBB constraints.
 */
	void EnforceOBBFaceConstraints(OBB* i_obb, uint i_face, bool i_inDir);
	void EnforceOBBEdgeConstraints(OBB* i_obb, uint i_edge, bool i_inDir);
	void EnforceOBBPointConstraints(OBB* i_obb, uint i_point, bool i_inDir);

	/*
	*	Functions to enforce Capsule constraints.
	*/
	void EnforceCapsuleFaceConstraints(Capsule* i_capsule, uint i_face, bool i_inDir);
	void EnforceCapsuleEdgeConstraints(Capsule* i_capsule, uint i_edge, bool i_inDir);
	void EnforceCapsulePointConstraints(Capsule* i_capsule, uint i_point, bool i_inDir);

/*
 *	Inlined functions.
 *
 *	Provide access to useful vectors.
 */
	Vector3Packed& ModelVertex(uint i_node);
	Vector3Packed& NodeVertex(uint i_node);
	Vector3& NodeNormal(uint i_node);
	Vector3& NewVertex(uint i_node);
	Vector3& Velocity(uint i_node);
/*
 *	Transform into and out of OBB space.
 */
	void NewVertexToOBB(uint i_node, OBB* i_obb, Vector3 &o_vertexInOBB);
	void NewVertexFromOBB(uint i_node, OBB* i_obb, Vector3 &i_vertexInOBB);
/*
 *	Perform constraint tests.
 */
	bool TestPosFace(float i_vertex0, float i_vertex1, float i_vertex2,
			float i_face, float &o_depth);
	bool TestNegFace(float i_vertex0, float i_vertex1, float i_vertex2,
			float i_face, float &o_depth);
	bool TestCrossPlane(float i_planeDist, float i_vertexDist,
			float i_edgeDist, float &o_posDepth, float &o_negDepth);
	void CompareDepthSquared(float i_scaledDepth, float i_normalSqInv,
			float &io_minDepthSq, int i_index, int &o_minIndex);
/*
 *	Move points in OBB space.
 */
	void ClampMin3(float i_min, float &io_val0, float &io_val1, float &io_val2);
	void ClampMax3(float i_max, float &io_val0, float &io_val1, float &io_val2);
	void ClampFaceToCrossPlane(float i_vertexDist, float i_edgeDist,
			float i_planeDist, float i_normalSqInv, float i_edgeA, float i_edgeB,
			float &io_vertex0A, float &io_vertex0B, float &io_vertex1A,
			float &io_vertex1B, float &io_vertex2A, float &io_vertex2B);
	void ClampToFace(bool i_posFace, float i_face, float &io_val0,
			float &io_val1);
	void ClampEdgeToCrossPlane(bool i_posPlane, float i_scaledOverlap,
			float i_sizeA, float i_sizeB, float &io_startA, float &io_startB,
			float &io_endA, float &io_endB);
/*
 *	Move points in constraint direction.
 */
	void FindConstraintCorner(OBB* i_obb, Vector3 &i_thickCorner,
			Vector3 &o_constraintCorner);
	void MoveToConstraintPlane(uint i_node, Vector3 &i_constraintCorner);
/*
 *	The node data.
 *
 *	The SOM for the instance.
 */
	SMeshSOM &d_som;
/*
 *	The reference instance.
 */
	CInstance &d_refInstance;
/*
 *	A an array of node instances.
 */
	SNodeInstance* d_node;
/*
 *	A an array of vertex instances. The mesh is made up of one or more
 *	sub-meshes. A sub-mesh is a group of verticies and associated tri-strips
 *	that have a common texture context. A sub-mesh is stored in a vertex buffer
 *	and a tri-strip buffer. If there are multiple sub-meshes, a single node
 *	could control multiple vertices. This array provides a mechanism for a node
 *	to access all its associated vertices.
 */
	SVertexInstance* d_vertexInstance;
/*
 *	An array of vertex buffers, one for each sub-mesh;
 */
	VtxBuffer* d_vertexBuffer;
/*
 *	An array of strip buffers, one for each sub-mesh;
 */
	StripBuffer* d_stripBuffer;
/*
 *	An array of the square of the lengths of the distance constraints. They are
 *	a function of scaling in the reference instance to world matrix. They need
 *	to be computed when the instance is created and whenever scaling of the
 *	reference instance is changed (not a good idea). To compute them, transform
 *	the endpoint vertices from model coordinates into world coordinates and
 *	measure the square of the distance between them.
 */
	float* d_constraintLengthSquared;
/*
 *	Collision thicknesses are averages of the thicknesses of the associated
 *	nodes. They must be computed when the instance is created and whenever
 *	scaling of the reference instance is changed.
 */
	float* d_collisionFaceThickness;
	float* d_collisionEdgeThickness;
/*
 *	The corners of a AABB surrounding the mesh.
 */
	ts_BoundingBox d_extents;
/*
 *	A list of collision events to check for collisions.
 */
	CObjectList* d_objectList;
/*
 *	Parameters.
 */
	CMeshParam* d_param;
/*
 *	Update management.
 */
	CMeshLOD* d_lod;
	uint8 d_requestFlag;
};

#define MESH_INSTANCE_HPP
#include "Game/Database/MeshInstance.hpp"

#endif  // MESH_INSTANCE_H
