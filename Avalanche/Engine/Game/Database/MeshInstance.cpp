/*
**
**  File:   MeshInstance.cpp
**  Date:   April 25, 2002
**  By:     Bryant Collard
**  Desc:   An instance of a mesh.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshInstance.cpp $
**  $Revision: 31 $
**      $Date: 8/22/03 5:57p $
**    $Author: Bcollard $
**
*/

/******************* includes ****************************/

#include "Game/GamePCH.h"

#include "CollideHelper/ObjectList.h"


#include "collide/Capsule.h"
#include "geomUtil/LineToLine.h"

#if 0

// My header (always first).
#include "enginehelper/meshinstance.h"

// System headers.
#include <string.h>

// Engine includes.
#define VTXBUFFER_NEED_INLINES
#include "Display/Common/VtxBuffer.h"
#include "Display/Common/StripBuffer.h"
#include "Game/Database/Instance.h"
#include "layers/som.h"
#include "Game/Animation/Bone.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Game/Managers/DistortionMgr.h"

// GameLib includes.
#include "platform/BaseType.h"
#include "Game/Database/MeshSOM.h"
#include "Game/Database/MeshParam.h"
#include "Game/Database/MeshLOD.h"

#endif

// Constants
static const uint MAX_LAYERS     = 8;
static const float MIN_THICKNESS = 0.005f;

// Request flag bits.
static const uint8 REQUEST_NONE        = 0x00;
static const uint8 REQUEST_RESET       = 0x01;
static const uint8 REQUEST_CONSTRAINT  = 0x02;
static const uint8 REQUEST_REACTIVATE  = 0x02;

/***************************************************************************/
/***************************************************************************/
CMeshInstance::CMeshInstance(SMeshSOM &i_som, CInstance &i_refInstance,
		CMeshLOD* i_lod, const char* i_varPath) :
	d_som(i_som), d_refInstance(i_refInstance)
{
	uint i, j;
/*
 *	Allocate the vertex and tristrip buffers.
 */
	d_vertexBuffer = new VtxBuffer[d_som.d_numSubMesh];
	d_stripBuffer = new StripBuffer[d_som.d_numSubMesh];
/*
 *	Initialize the buffers, counting the total number of vertices along the way.
 */
	uint totalVertex = 0;
	for (i = 0; i < d_som.d_numSubMesh; i++)
	{
		uint numVertex = d_som.d_subMesh[i].d_numVertex;
		totalVertex += numVertex;
/*
 *		Setup the texture flags and initialize the vertex buffer.
 */
		d_som.d_subMesh[i].d_textureContext->TextureLayer->u8Filter = TS_FILTER_BILINEAR;
		uint numLayers = d_som.d_subMesh[i].d_textureContext->TextureCount;
		u32 textureFlags[MAX_LAYERS];
		for (j = 0; j < MAX_LAYERS; j++)
			textureFlags[j] = (j < numLayers) ? VtxBuffer::VERT_TEX_DIM2 : 0;
#ifdef VTX_BUFFER_NORMAL
		d_vertexBuffer[i].Init(numVertex, VtxBuffer::VERT_HAS_DIFFUSE |
				VtxBuffer::VERT_HAS_NORMAL, textureFlags);
#else
		d_vertexBuffer[i].Init(numVertex, VtxBuffer::VERT_HAS_DIFFUSE,
				textureFlags);
#endif
/*
 *		Set the diffuse colors and texture coordinates.
 */
		d_vertexBuffer[i].Lock();
		for (j = 0; j < numVertex; j++)
		{
			d_vertexBuffer[i].Diffuse(j) = 0xFFFFFFFF;  // ????
			for (uint k = 0; k < numLayers; k++)
			{
				d_vertexBuffer[i].TexCoordv2(k, j) =
						d_som.d_subMesh[i].d_textureCoord[j * numLayers + k];
			}
		}
		d_vertexBuffer[i].UnLock();
/*
 *		Initialize the strip buffer and setup textures.
 */
		d_stripBuffer[i].Init();
		d_stripBuffer[i].SetTextureArray(d_som.d_textureArray,
				d_som.d_textureSubSetIndex);

		for (j = 0; j < numLayers; j++)
		{
			ts_TextureLayer &layer =
					d_som.d_subMesh[i].d_textureContext->TextureLayer[j];

			// again, icky, but compatible.
			TextureDatabase* pDatabase = layer.textureDatabase;
			Texture* pTexture = pDatabase->GetTexture(layer.s16TextureIndex);
			d_stripBuffer[i].SetPassTexture(j, pTexture,
					layer.u8Method, layer.u8WrapU, layer.u8WrapV, layer.u8Filter,
					layer.u8Blend);
		}
		d_stripBuffer[i].SetVtxBuffer(&d_vertexBuffer[i]);
/*
 *		Add the strips.
 */
		uint start = 0;
		for (j = 0; j < d_som.d_subMesh[i].d_numTriStrip; j++)
		{
			uint end = d_som.d_subMesh[i].d_triStripHead[j];
			d_stripBuffer[i].AddStrip(&(d_som.d_subMesh[i].d_triStrip[start]),
					end - start);
			start = end;
		}
	}
/*
 *	Allocate the vertex instances and nodes.
 */
	d_vertexInstance = new SVertexInstance[totalVertex];
	d_node = new SNodeInstance[d_som.d_numNode];
/*
 *	Setup each node.
 */
	uint currVertex = 0;
	for (i = 0; i < d_som.d_numNode; i++)
	{
/*
 *		Set the pointer into the node's section of the vertex instance array.
 */
		d_node[i].d_vertex = &(d_vertexInstance[currVertex]);
		currVertex += d_som.d_node[i].d_numFrontSide +
				d_som.d_node[i].d_numBackSide;
/*
 *		If the node is attached to a bone in the reference instance, point the
 *		master to the bone.
 */
//		Until multi-component instances work.
//		if ((d_som.d_node[i].d_flag & NODE_ATTACHED) &&
//				(d_som.d_node[i].d_masterBoneID != INVALID_BONEID))
		if ((d_som.d_node[i].d_flag & NODE_ATTACHED) &&
				(d_som.d_node[i].d_masterBoneID != INVALID_BONEID) &&
				(d_refInstance.Body() != NULL))
		{
			d_node[i].d_master = boneGetBonePtr(d_refInstance.Body(),
					d_som.d_node[i].d_masterBoneID);
		}
/*
 *		If the node is not attached or is attached directly to the reference
 *		instance, set the master to NULL.
 */
		else
			d_node[i].d_master = NULL;
	}
/*
 *	Go through each vertex in each sub-mesh and and fill in the appropriate
 *	vertex instance array entries for the front side vertices.
 */
	for (i = 0; i < d_som.d_numSubMesh; i++)
	{
		for (j = 0; j < d_som.d_subMesh[i].d_numVertex; j++)
		{
/*
 *			Skip this vertex if it belongs to the back side of a node.
 */
			uintCount nodeIndex = d_som.d_subMesh[i].d_nodeIndex[j];
			if (nodeIndex & MESH_COUNT_FLAG)
				continue;
/*
 *			Fill in the vertex instance.
 */
			d_node[nodeIndex].d_vertex->d_subMesh = &d_vertexBuffer[i];
			d_node[nodeIndex].d_vertex->d_index = j;
/*
 *			Point the node to its next vertex instance.
 */
			d_node[nodeIndex].d_vertex++;
		}
	}
/*
 *	Go through each vertex in each sub-mesh and and fill in the appropriate
 *	vertex instance array entries for the back side vertices.
 */
	for (i = 0; i < d_som.d_numSubMesh; i++)
	{
		for (j = 0; j < d_som.d_subMesh[i].d_numVertex; j++)
		{
/*
 *			Skip this vertex if it belongs to the front side of a node.
 */
			uintCount nodeIndex = d_som.d_subMesh[i].d_nodeIndex[j];
			if (!(nodeIndex & MESH_COUNT_FLAG))
				continue;
			nodeIndex &= ~MESH_COUNT_FLAG;
/*
 *			Fill in the vertex instance.
 */
			d_node[nodeIndex].d_vertex->d_subMesh = &d_vertexBuffer[i];
			d_node[nodeIndex].d_vertex->d_index = j;
/*
 *			Point the node to its next vertex instance.
 */
			d_node[nodeIndex].d_vertex++;
		}
	}
/*
 *	Reset the pointer into each node's section of the vertex instance array.
 */
	for (i = 0; i < d_som.d_numNode; i++)
	{
		d_node[i].d_vertex -= (d_som.d_node[i].d_numFrontSide +
				d_som.d_node[i].d_numBackSide);
	}
/*
 *	Allocate space for the constraint info.
 */
	if (d_som.d_numConstraint != 0)
		d_constraintLengthSquared = new float[d_som.d_numConstraint];
	else
		d_constraintLengthSquared = NULL;
	if (d_som.d_numCollisionFace != 0)
		d_collisionFaceThickness = new float[d_som.d_numCollisionFace];
	else
		d_collisionFaceThickness = NULL;
	if (d_som.d_numCollisionEdge != 0)
		d_collisionEdgeThickness = new float[d_som.d_numCollisionEdge];
	else
		d_collisionEdgeThickness = NULL;
/*
 *	Finish initialization at next update.
 */
	d_requestFlag = REQUEST_CONSTRAINT | REQUEST_RESET;
/*
 *	Get parameters.
 */
	d_param = CMeshParam::Register(this, d_som.d_name, i_varPath);
	ASSERT(d_param);
/*
 *	Init the extents.
 */
	UpdateExtents(ModelVertex(0), false);
	for (i = 0; i < d_som.d_numNode; i++)
	{
		UpdateExtents(ModelVertex(i), true);
	}
/*
 *	Setup the LOD.
 */
	if (i_lod != NULL)
		d_lod = i_lod;
	else
		d_lod = &g_defaultMeshLOD;
}

/***************************************************************************/
/***************************************************************************/
CMeshInstance::~CMeshInstance()
{
	CMeshParam::Unregister(this);
	delete[] d_collisionEdgeThickness;
	delete[] d_collisionFaceThickness;
	delete[] d_constraintLengthSquared;
	delete[] d_node;
	delete[] d_vertexInstance;
	delete[] d_stripBuffer;
	delete[] d_vertexBuffer;
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::Reset(void)
{
	d_requestFlag |= REQUEST_RESET;
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::RecomputeConstraints(void)
{
	d_requestFlag |= REQUEST_CONSTRAINT;
}

/***************************************************************************/
/***************************************************************************/
bool CMeshInstance::Update(float i_deltaSec, bool i_inScene)
{
/*
 *	Lock the vertex buffers.
 */
	uint i;
	for (i = 0; i < d_som.d_numSubMesh; i++)
		d_vertexBuffer[i].Lock();
/*
 *	Get the new control flag.
 */
	CMeshParam::TControlFlag controlFlag;
	if (d_requestFlag & REQUEST_RESET)
	{
		controlFlag = CMeshParam::MESH_SNAP_TO_INIT |
				CMeshParam::MESH_COMPUTE_NORMALS |
				CMeshParam::MESH_UPDATE_SUB_MESH;
	}
	else
	{
		controlFlag = d_param->d_controlFlag[
				d_lod->GetMeshLOD(NodeVertex(d_som.d_positionNode), i_inScene)];
		if ((controlFlag & CMeshParam::MESH_UPDATE) &&
				(d_requestFlag & REQUEST_REACTIVATE))
		{
			controlFlag &= ~CMeshParam::MESH_DYNAMIC;
			controlFlag |= CMeshParam::MESH_SNAP_TO_INIT |
					CMeshParam::MESH_COMPUTE_NORMALS;
		}
	}
/*
 *	 Update if wanted.
 */
	if (controlFlag & CMeshParam::MESH_UPDATE)
	{
/*
 *		Check for a request to compute constraints.
 */
		if (d_requestFlag & REQUEST_CONSTRAINT)
			ComputeConstraints();
/*
 *		Perform full dynamic modeling.
 */
		if (controlFlag & CMeshParam::MESH_DYNAMIC)
			UpdateDynamic(i_deltaSec, controlFlag);
/*
 *		Instantly move to modeled positions.
 */
		else if (controlFlag & CMeshParam::MESH_SNAP_TO_INIT)
			UpdateSnapToInit();
/*
 *		Smoothly move to modeled positions.
 */
		else
			UpdateDecayToInit(i_deltaSec);
/*
 *		Clear the request flag.
 */
		d_requestFlag = REQUEST_NONE;
	}
/*
 *	If the mesh is not being updated, request a reset the next time it is
 *	updated.
 */
	else
		d_requestFlag |= REQUEST_REACTIVATE;
/*
 *	Compute normals.
 */
	if (controlFlag & CMeshParam::MESH_COMPUTE_NORMALS)
	{
		for (i = 0; i < d_som.d_numNode; i++)
			ComputeNormal(i, NodeNormal(i), &CMeshInstance::NodeVertex);
	}
/*
 *	Propogate updates to all sub-mesh vertices.
 */
	if (controlFlag & CMeshParam::MESH_UPDATE_SUB_MESH)
	{
		for (i = 0; i < d_som.d_numNode; i++)
			UpdateSubMesh(i);
	}
/*
 *	Unlock the vertex buffers.
 */
	for (i = 0; i < d_som.d_numSubMesh; i++)
		d_vertexBuffer[i].UnLock();

	return((controlFlag & CMeshParam::MESH_DYNAMIC) &&
			(controlFlag & CMeshParam::MESH_CHECK_OBJECT));
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::Render(const CInstance* i_instance,
		const Graphics4x4* i_localToCamera, float i_zCamDist)
{
	#ifdef _DEBUG
	DrawUtility::Sphere( ((CInstance*)i_instance)->Matrix().GetTranslation(), DrawUtility::GREEN );
	#endif

	TextureMgr::Cache().Lock( d_som.d_textureArray );

	for (uint i = 0; i < d_som.d_numSubMesh; i++)
		d_stripBuffer[i].Render(i_instance, i_localToCamera, i_zCamDist);
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateSnapToInit(void)
{
/*
 *	Transform each node into world space.
 */
	uint i;
	for (i = 0; i < d_som.d_numNode; i++)
	{
/*
 *		Alias the vertex.
 */
		Vector3Packed &vertex = NodeVertex(i);
/*
 *		If the node is attached to a bone, apply the bone transformation to
 *		the vertex in model space then transform into world space.
 */
		if (d_node[i].d_master)
		{
			vertex = (ModelVertex(i) * (*boneRunBonesToBone(
					d_node[i].d_master))) *	d_refInstance.Matrix();
		}
/*
 *		Otherwise, just transform the vertex into into world space and zero
 *		out the velocity.
 */
		else
		{
			vertex = ModelVertex(i) * d_refInstance.Matrix();
			Velocity(i).Clear();
		}
/*
 *		Compute new extents.
 */
		UpdateExtents(vertex, (i != 0));
	}
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateDecayToInit(float i_deltaSec)
{
/*
 *	Compute the denominator term for the implicit Euler integrator.
 */
	float timeTermInv = 0.0f;
	float dampedDeltaSecInv = 0.0f;
	bool nonZeroTime = !Math::DivByZero(1.0f, i_deltaSec);
	if (nonZeroTime)
	{
		timeTermInv = 1.0f / (i_deltaSec + d_param->d_decayTimeConstant);
		dampedDeltaSecInv = d_param->d_globalDamping / i_deltaSec;
	}
/*
 *	Update positions of each node.
 */
	uint i;
	for (i = 0; i < d_som.d_numNode; i++)
	{
/*
 *		Alias the vertex.
 */
		Vector3Packed &vertex = NodeVertex(i);
/*
 *		If the node is attached to a bone, apply the bone transformation to
 *		the vertex in model space then transform into world space.
 */
		if (d_node[i].d_master)
		{
			vertex = (ModelVertex(i) * (*boneRunBonesToBone(
					d_node[i].d_master))) *	d_refInstance.Matrix();
		}
/*
 *		If the node is attached directly to the reference instance, just
 *		transform the vertex into into world space.
 */
		else if (d_som.d_node[i].d_flag & NODE_ATTACHED)
			vertex = ModelVertex(i) * d_refInstance.Matrix();
/*
 *		The node is not rigidly attached.
 */
		else if (nonZeroTime)
		{
			NewVertex(i) = timeTermInv * (d_param->d_decayTimeConstant * vertex +
					i_deltaSec * (ModelVertex(i) * d_refInstance.Matrix()));
			Velocity(i) = dampedDeltaSecInv * (NewVertex(i) - vertex);
			vertex = NewVertex(i);
		}
/*
 *		Update the extents.
 */
		UpdateExtents(vertex, (i != 0));
	}
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateDynamic(float i_deltaSec,
		CMeshParam::TControlFlag i_controlFlag)
{
/*
 *	Compute a typical position and velocity for all particles affected by
 *	aerodynamic drag.
 */
	Vector3 windVelocity(0.0f, 0.0f, 0.0f);
	float dragFactor = 0.0f;
	bool nonZeroTime = !Math::DivByZero(1.0f, i_deltaSec);
	float deltaSecInv = 0.0f;
	if (nonZeroTime)
	{
		deltaSecInv = 1.0f / i_deltaSec;
		if (i_controlFlag & CMeshParam::MESH_APPLY_DRAG)
		{
			Vector3 temp = NodeVertex(d_som.d_dragNode);
			DistortionMgr__ApplyDistortions(&temp,
					&windVelocity, DISTORTION_AFFECTS_MESHES);
			NodeVertex(d_som.d_dragNode) = temp;
			windVelocity *= deltaSecInv;
			dragFactor = d_param->d_dragCoefficient *
					(windVelocity - Velocity(d_som.d_dragNode)).Length();
		}
	}
/*
 *	Update positions of each node.
 */
	uint i;
	for (i = 0; i < d_som.d_numNode; i++)
	{
/*
 *		The particle is attached to a bone.
 */
		if (d_node[i].d_master)
		{
			NodeVertex(i) = (ModelVertex(i) * (*boneRunBonesToBone(
					d_node[i].d_master))) *	d_refInstance.Matrix();
		}
/*
 *		The particle is attached directly to the reference instance.
 */
		else if (d_som.d_node[i].d_flag & NODE_ATTACHED)
		{
			NodeVertex(i) = ModelVertex(i) * d_refInstance.Matrix();
		}
/*
 *		The particle is not rigidly attached.
 */
		else
		{
			NewVertex(i).Clear();
			if (i_controlFlag & CMeshParam::MESH_APPLY_DRAG)
				NewVertex(i) = dragFactor * (windVelocity - Velocity(i));
			if (i_controlFlag & CMeshParam::MESH_APPLY_GRAVITY)
				NewVertex(i) += d_param->d_gravity;
			NewVertex(i) = Velocity(i) + NewVertex(i) * i_deltaSec;
			NewVertex(i) *= i_deltaSec;
			NewVertex(i) += NodeVertex(i);
		}
	}
/*
 *	Enforce constraints.
 */
	Vector3 delta;
	float lengthSquared;
	int iterations = (int)(d_param->d_iterationRate * i_deltaSec) + 1;
	for (int j = 0; j < iterations; j++)
	{
/*
 *		Enforce length constraints.
 */
		for (i = 0; i < d_som.d_numConstraint; i++)
		{
			uint start = d_som.d_constraint[i].d_index[0];
			uint end = d_som.d_constraint[i].d_index[1];
			if (d_som.d_node[start].d_flag & NODE_ATTACHED)
			{
				delta = NewVertex(end) - NodeVertex(start);
				lengthSquared = delta.LengthSquared();
				delta *= (lengthSquared - d_constraintLengthSquared[i]) /
						(lengthSquared + d_constraintLengthSquared[i]);
				NewVertex(end) -= delta;
			}
			else if (d_som.d_node[end].d_flag & NODE_ATTACHED)
			{
				delta = NodeVertex(end) - NewVertex(start);
				lengthSquared = delta.LengthSquared();
				delta *= (lengthSquared - d_constraintLengthSquared[i]) /
						(lengthSquared + d_constraintLengthSquared[i]);
				NewVertex(start) += delta;
			}
			else
			{
				delta = NewVertex(end) - NewVertex(start);
				lengthSquared = delta.LengthSquared();
				delta *= 0.5f * (lengthSquared - d_constraintLengthSquared[i]) /
						(lengthSquared + d_constraintLengthSquared[i]);
				NewVertex(start) += delta;
				NewVertex(end) -= delta;
			}
		}
/*
 *		Enforce collision constraints.
 */
		if (i_controlFlag & CMeshParam::MESH_CHECK_OBJECT)
		{
			bool inDir = ((i_controlFlag & CMeshParam::MESH_CONSTRAINT_DIR) != 0);
			if (d_objectList)
			{
				CCollisionObject *cObj = d_objectList->GetHead();
				while( cObj )
				{
					if( cObj->GetObjectType() == CCollisionObject::OBB_OBJECT )
					{
						OBB* obb = (OBB*)cObj;
						for (i = 0; i < d_som.d_numCollisionFace; i++)
							EnforceOBBFaceConstraints(obb, i, inDir);
						for (i = 0; i < d_som.d_numCollisionEdge; i++)
							EnforceOBBEdgeConstraints(obb, i, inDir);
						for (i = 0; i < d_som.d_numCollisionPoint; i++)
							EnforceOBBPointConstraints(obb, i, inDir);
					}
					else if( cObj->GetObjectType() == CCollisionObject::CAPSULE )
					{
						Capsule *capsule = (Capsule *)cObj;
						for (i = 0; i < d_som.d_numCollisionFace; i++)
							EnforceCapsuleFaceConstraints(capsule, i, inDir);
						for (i = 0; i < d_som.d_numCollisionEdge; i++)
							EnforceCapsuleEdgeConstraints(capsule, i, inDir);
						for (i = 0; i < d_som.d_numCollisionPoint; i++)
							EnforceCapsulePointConstraints(capsule, i, inDir);
					}
					cObj = d_objectList->GetNext();
				}
			}
		}
	}
/*
 *	Compute velocity and record new positions.
 */
	if (nonZeroTime)
	{
		float dampedDeltaSecInv = deltaSecInv * d_param->d_globalDamping;
		for (i = 0; i < d_som.d_numNode; i++)
		{
			Vector3Packed &vertex = NodeVertex(i);
			if (!(d_som.d_node[i].d_flag & NODE_ATTACHED))
			{
				Velocity(i) = dampedDeltaSecInv * (NewVertex(i) - vertex);
				vertex = NewVertex(i);
			}
			UpdateExtents(vertex, (i != 0));
		}
	}
	else
	{
		UpdateExtents(NodeVertex(0), false);
		for (i = 1; i < d_som.d_numNode; i++)
		{
			UpdateExtents(NodeVertex(i), true);
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateExtents(Vector3CRef i_point, bool i_compare)
{
	if (i_compare)
	{
		d_extents.Min = Vector3::Min(d_extents.Min, i_point);
		d_extents.Max = Vector3::Max(d_extents.Max, i_point);
	}
	else
	{
		d_extents.Min = i_point;
		d_extents.Max = i_point;
	}
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::GetExtents(Vector3 &io_minExtent, Vector3 &io_maxExtent,
		bool i_compare)
{
	if (i_compare)
	{
		io_minExtent = Vector3::Min(io_minExtent, d_extents.Min);
		io_maxExtent = Vector3::Max(io_maxExtent, d_extents.Max);
	}
	else
	{
		io_minExtent = d_extents.Min;
		io_maxExtent = d_extents.Max;
	}
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateName(CInstance* pInstance)
{
	pInstance->SetName( d_som.d_name );
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateBoundingVolume(CInstance* pInstance)
{
	pInstance->BoundingSphere().Center = 0.5f * (d_extents.Min + d_extents.Max);
	pInstance->BoundingSphere().Radius =
			(d_extents.Max - pInstance->BoundingSphere().Center).Length();
	pInstance->BoundingBox() = d_extents;
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateBoundingVolume(CInstance* pInstance,
		float scale)
{
	UpdateBoundingVolume(pInstance);
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateBoundingVolumeAccurate(CInstance* pInstance)
{
	UpdateBoundingVolume(pInstance);
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::ComputeConstraints(void)
{
/*
 *	Constraints are not affected by translation so just deal with
 *	"orientation" (it's actually the scaling that is important).
 */
	Matrix3x3 modelToWorld;
	d_refInstance.Matrix().GetOrientation(modelToWorld);
/*
 *	Compute the constraint lengths from the base model vertices transformed
 *	into world coordinates.
 */
	uint i;
	for (i = 0; i < d_som.d_numConstraint; i++)
	{
		d_constraintLengthSquared[i] =
				((ModelVertex(d_som.d_constraint[i].d_index[0]) -
				ModelVertex(d_som.d_constraint[i].d_index[1])) *
				modelToWorld).LengthSquared();
	}
/*
 *	Find the thickness of each node by computing the vector from the front
 *	side to the back side in model coordinates, transforming it into world
 *	coordinates, and using its length as the thickness.
 */
	Vector3 normal;
	for (i = 0; i < d_som.d_numNode; i++)
	{
		if (d_som.d_node[i].d_flag & NODE_HAS_THICKNESS)
		{
			if (ComputeNormal(i, normal, &CMeshInstance::ModelVertex))
			{
				d_node[i].d_thickness = ((d_som.d_node[i].d_thickness * normal) *
						modelToWorld).Length();
			}
			else
				d_node[i].d_thickness = d_som.d_node[i].d_thickness;
		}
		else
			d_node[i].d_thickness = MIN_THICKNESS;
	}
/*
 *	The thickness of each collision face is the average of its vertex
 *	thicknesses.
 */
	for (i = 0; i < d_som.d_numCollisionFace; i++)
	{
		d_collisionFaceThickness[i] =
				(d_node[d_som.d_collisionFace[i].d_index[0]].d_thickness +
				d_node[d_som.d_collisionFace[i].d_index[1]].d_thickness +
				d_node[d_som.d_collisionFace[i].d_index[2]].d_thickness) / 3.0f;
	}
/*
 *	The thickness of each collision edge is the average of its endpoint
 *	thicknesses.
 */
	for (i = 0; i < d_som.d_numCollisionEdge; i++)
	{
		d_collisionEdgeThickness[i] =
				(d_node[d_som.d_collisionEdge[i].d_index[0]].d_thickness +
				d_node[d_som.d_collisionEdge[i].d_index[1]].d_thickness) * 0.5f;
	}
}

/***************************************************************************/
/***************************************************************************/
bool CMeshInstance::ComputeNormal(uint i_node, Vector3 &o_normal,
		GetVectorFn i_getVertexFn)
{
/*
 *	Alias the node's vertex.
 */
	Vector3Packed &center = (this->*i_getVertexFn)(i_node);
/*
 *	Clear the normal and mark it as invalid.
 */
	Vector3 normal;
	normal.Clear();
	bool normalValid = false;
/*
 *	Step through each neighbor.
 */
	uint j;
	Vector3Packed* neighbor[3];
	Vector3 vec1, vec2;
	uint firstNeighbor =
			(i_node == 0) ? 0 : d_som.d_node[i_node - 1].d_endNeighbor;
	uint neighborCount = 0;
	for (j = firstNeighbor; j < d_som.d_node[i_node].d_endNeighbor; j++)
	{
/*
 *		Alias the neighbor's vertex.
 */
		neighbor[j % 3] = &((this->*i_getVertexFn)(
				(uint)(d_som.d_neighbor[j] & ~MESH_COUNT_FLAG)));
/*
 *		See if two adjacent triangles were found then add the sum of their
 *		cross products using a single cross product.
 */
		if (neighborCount == 2)
		{
			vec1 = *(neighbor[(j - 2) % 3]) - *(neighbor[j % 3]);
			vec2 = *(neighbor[(j - 1) % 3]) - center;
			normal += vec1.Cross(vec2);
			normalValid = true;
/*
 *			If the current edge does not form part of a new triangle, restart
 *			the neighbor count; otherwise, count the current edge as the next
 *			neighbor.
 */
			neighborCount = (d_som.d_neighbor[j] & MESH_COUNT_FLAG) ? 0 : 1;
		}
/*
 *		If the current edge does not form part of a new triangle, add in the
 *		current triangle and restart the neighbor count.
 */
		else if (d_som.d_neighbor[j] & MESH_COUNT_FLAG)
		{
			if (j != firstNeighbor)
			{
				vec1 = *(neighbor[(j - 1) % 3]) - center;
				vec2 = *(neighbor[j % 3]) - center;
				normal += vec1.Cross(vec2);
				normalValid = true;
			}
			neighborCount = 0;
		}
/*
 *		Still trying to process as many triangles at once as possible.
 */
		else
			neighborCount++;
	}
/*
 *	Add in the triangle formed by the last neighbor and the first neighbor.
 */
	switch (neighborCount)
	{
	 case 1:
		vec1 = *(neighbor[(j - 1) % 3]) - center;
		vec2 = (this->*i_getVertexFn)(d_som.d_neighbor[firstNeighbor] &
				~MESH_COUNT_FLAG) - center;
		normal += vec1.Cross(vec2);
		normalValid = true;
		break;
	 case 2:
		vec1 = *(neighbor[(j - 2) % 3]) - (this->*i_getVertexFn)
				(d_som.d_neighbor[firstNeighbor] & ~MESH_COUNT_FLAG);
		vec2 = *(neighbor[(j - 1) % 3]) - center;
		normal += vec1.Cross(vec2);
		normalValid = true;
		break;
	}
/*
 *	If a normal was computed, normalize and export it.
 */
	if (normalValid)
	{
		o_normal = normal.Normalized();
		return(true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::UpdateSubMesh(uint i_node)
{
/*
 *	Alias the vertex and normal.
 */
	Vector3Packed &vertex = NodeVertex(i_node);
	Vector3 &normal = NodeNormal(i_node);
/*
 *	Update the front side vertices.
 */
	uint i;
	SVertexInstance* curr = d_node[i_node].d_vertex + 1;
	for (i = 1; i < d_som.d_node[i_node].d_numFrontSide; i++)
	{
		curr->d_subMesh->Vertex(curr->d_index) = vertex;
#ifdef VTX_BUFFER_NORMAL
		curr->d_subMesh->Normal(curr->d_index) = normal;
#endif
		curr++;
	}
/*
 *	Update the back side vertices.
 */
	if (d_som.d_node[i_node].d_numBackSide > 0)
	{
		Vector3 backNormal = -normal;
		Vector3 backVertex = vertex;
		if (d_som.d_node[i_node].d_flag & NODE_HAS_THICKNESS)
			backVertex += d_node[i_node].d_thickness * backNormal;
		for (i = 0; i < d_som.d_node[i_node].d_numBackSide; i++)
		{
			curr->d_subMesh->Vertex(curr->d_index) = backVertex;
#ifdef VTX_BUFFER_NORMAL
			curr->d_subMesh->Normal(curr->d_index) = backNormal;
#endif
			curr++;
		}
	}
}

void CMeshInstance::EnforceCapsuleFaceConstraints(Capsule* i_capsule, uint i_face, bool i_inDir)
{
	/*
	*	Get the vertex node indices.
	*/
	uint face0 = d_som.d_collisionFace[i_face].d_index[0];
	uint face1 = d_som.d_collisionFace[i_face].d_index[1];
	uint face2 = d_som.d_collisionFace[i_face].d_index[2];

	Vector3 &v0 = NewVertex( face0 );
	Vector3 &v1 = NewVertex( face1 );
	Vector3 &v2 = NewVertex( face2 );

	Vector3 e0 = v1 - v0;
	Vector3 e1 = v2 - v0;
	Vector3 normal = e0.Cross( e1 );
	normal.SafeNormalize();

	Vector3 pointOnLine, pointOnTri;
	float dSquared = SegmentToTriangleDistanceSquared( i_capsule->Origin(), i_capsule->Axis(),
		0.0f, i_capsule->AxisLength(), normal, v0, e0, e1, pointOnLine, pointOnTri );

	float rSquared = i_capsule->Radius() + d_collisionFaceThickness[i_face];
	rSquared *= rSquared;
	if( dSquared > rSquared )
		return; // no intersection

	// which direction do we need to move to escape the capsule?
	Vector3 displacement = pointOnTri - pointOnLine;
	if( !displacement.SafeNormalize() )
	{
		// capsule axis intersects the triangle directly.  figure out a good displacement axis
		// use the max triangle distance away from the line as a guide
		float test;
		displacement = v0;
		float dist = PointToSegmentDistanceSquared( i_capsule->Origin(), i_capsule->Axis(),
			0.0f, i_capsule->AxisLength(), v0 );
		test = PointToSegmentDistanceSquared( i_capsule->Origin(), i_capsule->Axis(),
			0.0f, i_capsule->AxisLength(), v1 );
		if( test > dist )
		{
			dist = test;
			displacement = v1;
		}
		test = PointToSegmentDistanceSquared( i_capsule->Origin(), i_capsule->Axis(),
			0.0f, i_capsule->AxisLength(), v2 );
		if( test > dist )
		{
			dist = test;
			displacement = v2;
		}
		displacement.SafeNormalize();
	}

	float d = i_capsule->Radius() + d_collisionFaceThickness[i_face] - Math::Sqrt( dSquared );
	displacement *= d;

	v0 += displacement;
	v1 += displacement;
	v2 += displacement;
}

void CMeshInstance::EnforceCapsuleEdgeConstraints(Capsule* i_capsule, uint i_edge, bool i_inDir)
{
	/*
	*	Get the endpoint node indices.
	*/
	uint start = d_som.d_collisionEdge[i_edge].d_index[0];
	uint end = d_som.d_collisionEdge[i_edge].d_index[1];

	Vector3 &v0 = NewVertex( start );
	Vector3 &v1 = NewVertex( end );

	float t1, t2;
	Vector3 pointOnCapsuleAxis, pointOnEdge;

	float dSquared = SegmentToSegmentDistanceSquared( i_capsule->Origin(), i_capsule->Axis(),
		0.0f, i_capsule->AxisLength(), v0, v1, 0.0f, 1.0f, t1, t2, pointOnCapsuleAxis, pointOnEdge );

	float rSquared = i_capsule->Radius() + d_collisionEdgeThickness[i_edge];
	rSquared *= rSquared;
	if( dSquared > rSquared )
		return; // no intersection

	Vector3 displacement = pointOnEdge - pointOnCapsuleAxis;
	if( !displacement.SafeNormalize() )
	{
		displacement = i_capsule->Axis().Cross( v1 - v0 );
		if( !displacement.SafeNormalize() )
		{
			// the planets are aligned against you, frustrating your efforts.
			// do nothing, and hope that next frame we can really figure out what to do
			return;
		}
	}

	float d = i_capsule->Radius() + d_collisionEdgeThickness[i_edge] - Math::Sqrt( dSquared );
	displacement *= d;

	v0 += displacement;
	v1 += displacement;
}

void CMeshInstance::EnforceCapsulePointConstraints(Capsule* i_capsule, uint i_point, bool i_inDir)
{
	/*
	*	Get the point node index.
	*/
	uint point = d_som.d_collisionPoint[i_point];

	Vector3 &v0 = NewVertex( point );

	float t;
	Vector3 p;
	float dSquared = PointToSegmentDistanceSquared( i_capsule->Origin(), i_capsule->Axis(),
		0.0f, i_capsule->AxisLength(), v0, t, p );

	float rSquared = i_capsule->Radius() + d_node[i_point].d_thickness;
	rSquared *= rSquared;
	if( dSquared > rSquared )
		return; // no intersection

	Vector3 displacement = v0 - p;
	if( !displacement.SafeNormalize() )
	{
		// the planets are aligned against you, frustrating your efforts.
		// do nothing, and hope that next frame we can really figure out what to do
		return;
	}

	float d = i_capsule->Radius() + d_node[i_point].d_thickness - Math::Sqrt( dSquared );
	displacement *= d;

	v0 += displacement;
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::EnforceOBBFaceConstraints(OBB* i_obb, uint i_face,
		bool i_inDir)
{
/*
 *	Get the vertex node indices.
 */
	uint face0 = d_som.d_collisionFace[i_face].d_index[0];
	uint face1 = d_som.d_collisionFace[i_face].d_index[1];
	uint face2 = d_som.d_collisionFace[i_face].d_index[2];
/*
 *	Transform the face into OBB coordinates.
 */
	Vector3 vertex0, vertex1, vertex2;
	NewVertexToOBB(face0, i_obb, vertex0);
	NewVertexToOBB(face1, i_obb, vertex1);
	NewVertexToOBB(face2, i_obb, vertex2);
/*
 *	Expand the obb to allow for thickness.
 */
	Vector3 corner = i_obb->GetAABB().maxV;
	corner += d_collisionFaceThickness[i_face];
/*
 *	Find the distance (possibly scaled) required to move the deepest
 *	penetrating face vertex to each separating plane. If any value is
 *	negative, a separating plane exists and there is no interpenetration.
 */
	float depth[25];
/*
 *	Test the OBB faces; +x, -x, +y, -y, +z, -z.
 */
	if (TestPosFace(vertex0.x(), vertex1.x(), vertex2.x(), corner.x(), depth[0]))
		return;
	if (TestNegFace(vertex0.x(), vertex1.x(), vertex2.x(), corner.x(), depth[1]))
		return;
	if (TestPosFace(vertex0.y(), vertex1.y(), vertex2.y(), corner.y(), depth[2]))
		return;
	if (TestNegFace(vertex0.y(), vertex1.y(), vertex2.y(), corner.y(), depth[3]))
		return;
	if (TestPosFace(vertex0.z(), vertex1.z(), vertex2.z(), corner.z(), depth[4]))
		return;
	if (TestNegFace(vertex0.z(), vertex1.z(), vertex2.z(), corner.z(), depth[5]))
		return;
/*
 *	The test face.
 *
 *	Find the normal to the face and the scaled distance from the OBB center to
 *	the face in the direction of the face normal. Make sure the normal points
 *	from the OBB center towards the face.
 */
	Vector3 edge[3];
	edge[0] = vertex2 - vertex1;
	edge[1] = vertex0 - vertex2;
	Vector3 faceNormal = edge[0].Cross(edge[1]);
	float faceDistance = vertex0.Dot(faceNormal);
	if (faceDistance < 0.0f)
	{
		faceNormal.Negate();
		faceDistance = -faceDistance;
	}
/*
 *	Find the distance from the face to the separating plane scaled by the
 *	magnitude of the normal.
 */
	Vector3 absFaceNormal = Vector3::Abs(faceNormal);
	depth[6] = Vector3::SumXYZ(corner.Diag(absFaceNormal)) - faceDistance;
	if (depth[6] <= 0.0f)
		return;
/*
 *	Prepare for the planes normal to the cross products of the face and OBB
 *	edges.
 */
	Vector3* vertex[5] = {&vertex0, &vertex1, &vertex2, &vertex0, &vertex1};
	edge[2] = vertex1 - vertex0;
	Vector3 absEdge[3];
	int i;
	for (i = 0; i < 3; i++)
	{
		absEdge[i] = Vector3::Abs(edge[i]);
	}
/*
 *	Go through each edge.
 */
	Vector3 planeDist[3];
	Vector3 edgeDist[3];
	Vector3 vertexDist[3];
	int index = 7;
	for (i = 0; i < 3; i++)
	{
/*
 *		Edge cross x-axis.
 */
		planeDist[i].x( absEdge[i].y() * corner.z() + absEdge[i].z() * corner.y() );
		edgeDist[i].x(
				edge[i].y() * vertex[i + 1]->z() - edge[i].z() * vertex[i + 1]->y() );
		vertexDist[i].x( edge[i].y() * vertex[i]->z() - edge[i].z() * vertex[i]->y() );
		if (TestCrossPlane(planeDist[i].x(), vertexDist[i].x(), edgeDist[i].x(),
				depth[index], depth[index + 1]))
			return;
		index += 2;
/*
 *		Edge cross y-axis.
 */
		planeDist[i].y( absEdge[i].z() * corner.x() + absEdge[i].x() * corner.z() );
		edgeDist[i].y(
				edge[i].z() * vertex[i + 1]->x() - edge[i].x() * vertex[i + 1]->z() );
		vertexDist[i].y( edge[i].z() * vertex[i]->x() - edge[i].x() * vertex[i]->z() );
		if (TestCrossPlane(planeDist[i].y(), vertexDist[i].y(), edgeDist[i].y(),
				depth[index], depth[index + 1]))
			return;
		index += 2;
/*
 *		Edge cross z-axis.
 */
		planeDist[i].z( absEdge[i].x() * corner.y() + absEdge[i].y() * corner.x() );
		edgeDist[i].z(
			edge[i].x() * vertex[i + 1]->y() - edge[i].y() * vertex[i + 1]->x());
		vertexDist[i].z( edge[i].x() * vertex[i]->y() - edge[i].y() * vertex[i]->x());
		if (TestCrossPlane(planeDist[i].z(), vertexDist[i].z(), edgeDist[i].z(),
				depth[index], depth[index + 1]))
			return;
		index += 2;
	}
/*
 *	If this point is reached, the face penetrates the OBB. If a constraint
 *	direction has been specified, move the points in that direction
 */
	if (i_inDir)
	{
		Vector3 constraintCorner;
		FindConstraintCorner(i_obb, corner, constraintCorner);
		MoveToConstraintPlane(face0, constraintCorner);
		MoveToConstraintPlane(face1, constraintCorner);
		MoveToConstraintPlane(face2, constraintCorner);
		return;
	}
/*
 *	Find the separating plane that requires the least amount of displacment of
 *	face verticies to fix the penetration.
 *
 *	The depths to the OBB faces are true distances. Find the closest OBB face.
 */
	int minIndex = 0;
	for (i = 1; i < 6; i++)
	{
		if (depth[i] < depth[minIndex])
			minIndex = i;
	}
/*
 *	The remaining depths are scaled distances. Find the minimum by comparing
 *	squares of true distances.
 */
	float minDepthSq = depth[minIndex] * depth[minIndex];
/*
 *	Do the face normal.
 */
	float depthSq = depth[6];
	depth[6] /= faceNormal.LengthSquared();
	depthSq *= depth[6];
	if (depthSq < minDepthSq)
	{
		minDepthSq = depthSq;
		minIndex = 6;
	}
/*
 *	Do the edge cross products.
 */
	Vector3 edgeSq;
	Vector3 normalSqInv[3];
	index = 6;
	for (i = 0; i < 3; i++)
	{
		edgeSq = edge[i].Diag(edge[i]);
		normalSqInv[i].Set(1.0f / (edgeSq.y() + edgeSq.z()),
								 1.0f / (edgeSq.x() + edgeSq.z()),
								 1.0f / (edgeSq.x() + edgeSq.y()));
/*
 *		Edge cross x-axis.
 */
		index++;
		CompareDepthSquared(depth[index], normalSqInv[i].x(), minDepthSq, index,
				minIndex);
		index++;
		CompareDepthSquared(depth[index], normalSqInv[i].x(), minDepthSq, index,
				minIndex);
/*
 *		Edge cross y-axis.
 */
		index++;
		CompareDepthSquared(depth[index], normalSqInv[i].y(), minDepthSq, index,
				minIndex);
		index++;
		CompareDepthSquared(depth[index], normalSqInv[i].y(), minDepthSq, index,
				minIndex);
/*
 *		Edge cross z-axis.
 */
		index++;
		CompareDepthSquared(depth[index], normalSqInv[i].z(), minDepthSq, index,
				minIndex);
		index++;
		CompareDepthSquared(depth[index], normalSqInv[i].z(), minDepthSq, index,
				minIndex);
	}
/*
 *	Now move the vertices.
 */
	float f0, f1, f2, f3, f4, f5;
	switch(minIndex)
	{
	case 0:
		f0 = vertex0.x();
		f1 = vertex1.x();
		f2 = vertex2.x();
		ClampMin3(corner.x(), f0, f1, f2);
		vertex0.x(f0);
		vertex1.x(f1);
		vertex2.x(f2);
		break;
	case 1:
		f0 = vertex0.x();
		f1 = vertex1.x();
		f2 = vertex2.x();
		ClampMax3(-corner.x(), f0, f1, f2);
		vertex0.x(f0);
		vertex1.x(f1);
		vertex2.x(f2);
		break;
	case 2:
		f0 = vertex0.y();
		f1 = vertex1.y();
		f2 = vertex2.y();
		ClampMin3(corner.y(), f0, f1, f2);
		vertex0.y(f0);
		vertex1.y(f1);
		vertex2.y(f2);
		break;
	case 3:
		f0 = vertex0.y();
		f1 = vertex1.y();
		f2 = vertex2.y();
		ClampMax3(-corner.y(), f0, f1, f2);
		vertex0.y(f0);
		vertex1.y(f1);
		vertex2.y(f2);
		break;
	case 4:
		f0 = vertex0.z();
		f1 = vertex1.z();
		f2 = vertex2.z();
		ClampMin3(corner.z(), f0, f1, f2);
		vertex0.z(f0);
		vertex1.z(f1);
		vertex2.z(f2);
		break;
	case 5:
		f0 = vertex0.z();
		f1 = vertex1.z();
		f2 = vertex2.z();
		ClampMax3(-corner.z(), f0, f1, f2);
		vertex0.z(f0);
		vertex1.z(f1);
		vertex2.z(f2);
		break;
	case 6:
		faceNormal *= depth[6];
		vertex0 += faceNormal;
		vertex1 += faceNormal;
		vertex2 += faceNormal;
		break;
	default:
		i = minIndex - 7;
		index = i / 6;
		switch(i % 6)
		{
		 case 0:
			 f0 = vertex[index]->y();
			 f1 = vertex[index]->z();
			 f2 = vertex[index + 1]->y();
			 f3 = vertex[index + 1]->z();
			 f4 = vertex[index + 2]->y();
			 f5 = vertex[index + 2]->z();
			 ClampFaceToCrossPlane(vertexDist[index].x(), edgeDist[index].x(),
										  planeDist[index].x(), normalSqInv[index].x(), edge[index].y(),
										  edge[index].z(), 
										  f0, f1, f2,f3,f4,f5);
			 vertex[index]->y(f0);
			 vertex[index]->z(f1);
			 vertex[index + 1]->y(f2);
			 vertex[index + 1]->z(f3);
			 vertex[index + 2]->y(f4);
			 vertex[index + 2]->z(f5);
			break;
		 case 1:
			 f0 = vertex[index]->y();
			 f1 = vertex[index]->z();
			 f2 = vertex[index + 1]->y();
			 f3 = vertex[index + 1]->z();
			 f4 = vertex[index + 2]->y();
			 f5 = vertex[index + 2]->z();
			 ClampFaceToCrossPlane(-vertexDist[index].x(), -edgeDist[index].x(),
					planeDist[index].x(), normalSqInv[index].x(), -edge[index].y(),
										  -edge[index].z(), 
										  f0, f1, f2,f3,f4,f5);
			 vertex[index]->y(f0);
			 vertex[index]->z(f1);
			 vertex[index + 1]->y(f2);
			 vertex[index + 1]->z(f3);
			 vertex[index + 2]->y(f4);
			 vertex[index + 2]->z(f5);
			break;
		 case 2:
			 f0 = vertex[index]->z();
			 f1 = vertex[index]->x();
			 f2 = vertex[index + 1]->z();
			 f3 = vertex[index + 1]->x();
			 f4 = vertex[index + 2]->z();
			 f5 = vertex[index + 2]->x();
			 ClampFaceToCrossPlane(vertexDist[index].y(), edgeDist[index].y(),
					planeDist[index].y(), normalSqInv[index].y(), edge[index].z(),
										  edge[index].x(), 
										  f0, f1, f2,f3,f4,f5);
			 vertex[index]->z(f0);
			 vertex[index]->x(f1);
			 vertex[index + 1]->z(f2);
			 vertex[index + 1]->x(f3);
			 vertex[index + 2]->z(f4);
			 vertex[index + 2]->x(f5);
			break;
		 case 3:
			 f0 = vertex[index]->z();
			 f1 = vertex[index]->x();
			 f2 = vertex[index + 1]->z();
			 f3 = vertex[index + 1]->x();
			 f4 = vertex[index + 2]->z();
			 f5 = vertex[index + 2]->x();
			 ClampFaceToCrossPlane(-vertexDist[index].y(), -edgeDist[index].y(),
					planeDist[index].y(), normalSqInv[index].y(), -edge[index].z(),
										  -edge[index].x(), 
										  f0, f1, f2,f3,f4,f5);
			 vertex[index]->z(f0);
			 vertex[index]->x(f1);
			 vertex[index + 1]->z(f2);
			 vertex[index + 1]->x(f3);
			 vertex[index + 2]->z(f4);
			 vertex[index + 2]->x(f5);
			break;
		 case 4:
			 f0 = vertex[index]->x();
			 f1 = vertex[index]->y();
			 f2 = vertex[index + 1]->x();
			 f3 = vertex[index + 1]->y();
			 f4 = vertex[index + 2]->x();
			 f5 = vertex[index + 2]->y();
			 ClampFaceToCrossPlane(vertexDist[index].z(), edgeDist[index].z(),
					planeDist[index].z(), normalSqInv[index].z(), edge[index].x(),
										  edge[index].y(), 
										  f0, f1, f2,f3,f4,f5);
			 vertex[index]->x(f0);
			 vertex[index]->y(f1);
			 vertex[index + 1]->x(f2);
			 vertex[index + 1]->y(f3);
			 vertex[index + 2]->x(f4);
			 vertex[index + 2]->y(f5);
			break;
		 default: // 5
			 f0 = vertex[index]->x();
			 f1 = vertex[index]->y();
			 f2 = vertex[index + 1]->x();
			 f3 = vertex[index + 1]->y();
			 f4 = vertex[index + 2]->x();
			 f5 = vertex[index + 2]->y();
			 ClampFaceToCrossPlane(-vertexDist[index].z(), -edgeDist[index].z(),
					planeDist[index].z(), normalSqInv[index].z(), -edge[index].x(),
										  -edge[index].y(), 
										  f0, f1, f2,f3,f4,f5);
			 vertex[index]->x(f0);
			 vertex[index]->y(f1);
			 vertex[index + 1]->x(f2);
			 vertex[index + 1]->y(f3);
			 vertex[index + 2]->x(f4);
			 vertex[index + 2]->y(f5);
			break;
		}
		break;
	}
/*
 *	Transform back into world space.
 */
	NewVertexFromOBB(face0, i_obb, vertex0);
	NewVertexFromOBB(face1, i_obb, vertex1);
	NewVertexFromOBB(face2, i_obb, vertex2);
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::EnforceOBBEdgeConstraints(OBB* i_obb, uint i_edge,
		bool i_inDir)
{
/*
 *	Get the endpoint node indices.
 */
	uint start = d_som.d_collisionEdge[i_edge].d_index[0];
	uint end = d_som.d_collisionEdge[i_edge].d_index[1];
/*
 *	Transform the edge into OBB coordinates.
 */
	Vector3 startInOBB, endInOBB;
	NewVertexToOBB(start, i_obb, startInOBB);
	NewVertexToOBB(end, i_obb, endInOBB);
/*
 *	Find the center and size.
 */
	Vector3 center = 0.5f * (startInOBB + endInOBB);
	Vector3 size = endInOBB - center;
/*
 *	Expand the obb to allow for thickness.
 */
	Vector3 corner = i_obb->GetAABB().maxV;
	corner += d_collisionEdgeThickness[i_edge];
/*
 *	Check against the separating planes formed by the faces of the OBB.
 *
 *	X faces.
 */
	float overlap[6];
	Vector3 absSize = Vector3::Abs(size);
	Vector3 absCenter = Vector3::Abs(center);
	overlap[0] = corner.x() + absSize.x() - absCenter.x();
	if (overlap[0] < 0.0f)
		return;
/*
 *	Y faces.
 */
	overlap[1] = corner.y() + absSize.y() - absCenter.y();
	if (overlap[1] < 0.0f)
		return;
/*
 *	Z faces.
 */
	overlap[2] = corner.z() + absSize.z() - absCenter.z();
	if (overlap[2] < 0.0f)
		return;
/*
 *	Check against the separating planes formed by the line and the OBB edges.
 *
 *	X axis.
 */
	Vector3 crossAxis;
	crossAxis.x(center.z() * size.y() - center.y() * size.z());
	overlap[3] = corner.y() * absSize.z() + corner.z() * absSize.y() -
			Math::Abs(crossAxis.x());
	if (overlap[3] < 0.0f)
		return;
/*
 *	Y axis.
 */
	crossAxis.y(center.x() * size.z() - center.z() * size.x());
	overlap[4] = corner.x() * absSize.z() + corner.z() * absSize.x() -
			Math::Abs(crossAxis.y());
	if (overlap[4] < 0.0f)
		return;
/*
 *	Z axis.
 */
	crossAxis.z(center.y() * size.x() - center.x() * size.y());
	overlap[5] = corner.x() * absSize.y() + corner.y() * absSize.x() -
			Math::Abs(crossAxis.z());
	if (overlap[5] < 0.0f)
		return;
/*
 *	If this point is reached, the edge penetrates the OBB. If a constraint
 *	direction has been specified, move the points in that direction
 */
	if (i_inDir)
	{
		Vector3 constraintCorner;
		FindConstraintCorner(i_obb, corner, constraintCorner);
		MoveToConstraintPlane(start, constraintCorner);
		MoveToConstraintPlane(end, constraintCorner);
		return;
	}
/*
 *	Determine which of the face planes has the smallest overlap.
 */
	int minIndex;
	if (overlap[0] < overlap[1])
	{
		if (overlap[0] < overlap[2])
			minIndex = 0;
		else
			minIndex = 2;
	}
	else if (overlap[1] < overlap[2])
		minIndex = 1;
	else
		minIndex = 2;
/*
 *	The computed overlap values for the line-edge planes are squared values.
 *	Square the smallest face overlap to use for comparisons.
 */
	float minDistSq = overlap[minIndex] * overlap[minIndex];
/*
 *	See if any of the line-edge plane overlaps are the smallest.
 *
 *	X axis.
 */
	Vector3 scaledOverlap;
	scaledOverlap.x(overlap[3] / (size.y() * size.y() + size.z() * size.z()));
	float distSq = overlap[3] * scaledOverlap.x();
	if (distSq < minDistSq)
	{
		minDistSq = distSq;
		minIndex = 3;
	}
/*
 *	Y axis.
 */
	scaledOverlap.y( overlap[4] / (size.x() * size.x() + size.z() * size.z()));
	distSq = overlap[4] * scaledOverlap.y();
	if (distSq < minDistSq)
	{
		minDistSq = distSq;
		minIndex = 4;
	}
/*
 *	Z axis.
 */
	scaledOverlap.z( overlap[5] / (size.x() * size.x() + size.y() * size.y()));
	distSq = overlap[5] * scaledOverlap.z();
	if (distSq < minDistSq)
		minIndex = 5;
/*
 *	Move the endpoints.
 */
	float f0, f1, f2, f3;
	switch (minIndex)
	{
	case 0:
		f0 = startInOBB.x();
		f1 = endInOBB.x();
		ClampToFace((center.x() >= 0.0f), corner.x(), f0, f1);
		startInOBB.x(f0);
		endInOBB.x(f1);
		break;
	case 1:
		f0 = startInOBB.y();
		f1 = endInOBB.y();
		ClampToFace((center.y() >= 0.0f), corner.y(), f0, f1);
		startInOBB.y(f0);
		endInOBB.y(f1);
		break;
	case 2:
		f0 = startInOBB.z();
		f1 = endInOBB.z();
		ClampToFace((center.z() >= 0.0f), corner.z(), f0, f1);
		startInOBB.z(f0);
		endInOBB.z(f1);
		break;
	case 3:
		f0 = startInOBB.y();
		f1 = startInOBB.z();
		f2 = endInOBB.y();
		f3 = endInOBB.z();
		ClampEdgeToCrossPlane((crossAxis.x() >= 0.0f), scaledOverlap.x(), size.y(),
				size.z(), f0, f1, f2, f3);
		startInOBB.y(f0);
		startInOBB.z(f1);
		endInOBB.y(f2);
		endInOBB.z(f3);
		break;
	case 4:
		f0 = startInOBB.z();
		f1 = startInOBB.x();
		f2 = endInOBB.z();
		f3 = endInOBB.x();
		ClampEdgeToCrossPlane((crossAxis.y() >= 0.0f), scaledOverlap.y(), size.z(),
									 size.x(), f0, f1, f2, f3);
		startInOBB.z(f0);
		startInOBB.x(f1);
		endInOBB.z(f2);
		endInOBB.x(f3);
		break;
	default:  // case 5:
		f0 = startInOBB.x();
		f1 = startInOBB.y();
		f2 = endInOBB.x();
		f3 = endInOBB.y();
		ClampEdgeToCrossPlane((crossAxis.z() >= 0.0f), scaledOverlap.z(), size.x(),
									 size.y(), f0, f1, f2, f3);
		startInOBB.x(f0);
		startInOBB.y(f1);
		endInOBB.x(f2);
		endInOBB.y(f3);
		break;
	}
/*
 *	Transform back into world space.
 */
	NewVertexFromOBB(start, i_obb, startInOBB);
	NewVertexFromOBB(end, i_obb, endInOBB);
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::EnforceOBBPointConstraints(OBB* i_obb, uint i_point,
		bool i_inDir)
{
/*
 *	Get the point node index.
 */
	uint point = d_som.d_collisionPoint[i_point];
/*
 *	Transform the point into OBB coordinates.
 */
	Vector3 pointInOBB;
	NewVertexToOBB(point, i_obb, pointInOBB);
/*
 *	Expand the obb to allow for thickness.
 */
	Vector3 extent = i_obb->GetAABB().maxV;
	extent += d_node[point].d_thickness;
/*
 *	See if the point is inside the expanded OBB.
 */
	if ((pointInOBB.x() > -extent.x()) && (pointInOBB.x() < extent.x()) &&
			(pointInOBB.z() > -extent.z()) && (pointInOBB.z() < extent.z()) &&
			(pointInOBB.y() > -extent.y()) && (pointInOBB.y() < extent.y()))
	{
/*
 *		If a constraint direction has been specified, move the point in that
 *		direction.
 */
		if (i_inDir)
		{
			Vector3 constraintCorner;
			FindConstraintCorner(i_obb, extent, constraintCorner);
			MoveToConstraintPlane(point, constraintCorner);
		}
/*
 *		Move the point to the closest face.
 */
		else
		{
/*
 *			Find the distances to each face.
 */
			float dist[6];
			dist[0] = pointInOBB.x() + extent.x();
			dist[1] = pointInOBB.y() + extent.y();
			dist[2] = pointInOBB.z() + extent.z();
			dist[3] = extent.x() - pointInOBB.x();
			dist[4] = extent.y() - pointInOBB.y();
			dist[5] = extent.z() - pointInOBB.z();
/*
 *			Find the closest face.
 */
			int closestFace = 0;
			for (int i = 1; i < 6; i++)
			{
				if (dist[i] < dist[closestFace])
					closestFace = i;
			}
/*
 *			Move the vertex to the closest face.
 */
			switch (closestFace)
			{
			 case 0:
				pointInOBB.x( -extent.x());
				break;
			 case 1:
				pointInOBB.y( -extent.y());
				break;
			 case 2:
				pointInOBB.z( -extent.z());
				break;
			 case 3:
				pointInOBB.x( extent.x());
				break;
			 case 4:
				pointInOBB.y( extent.y());
				break;
			 default:
				pointInOBB.z( extent.z());
			}
/*
 *			Transform back into world space.
 */
			NewVertexFromOBB(point, i_obb, pointInOBB);
		}
	}
}

/***************************************************************************/
/***************************************************************************/
void CMeshInstance::SetDiffuseColor(uint32 i_diffuseColor)
{
	for (uint i = 0; i < d_som.d_numSubMesh; i++)
	{
		uint numVertex = d_som.d_subMesh[i].d_numVertex;
		d_vertexBuffer[i].Lock();
		for (uint j = 0; j < numVertex; j++)
			d_vertexBuffer[i].Diffuse(j) = VtxBuffer::cvtRGBA(i_diffuseColor);
		d_vertexBuffer[i].UnLock();
	}
}

/***************************************************************************/
/***************************************************************************/
#include "EngineHelper/drawutil.h"
void CMeshInstance::DrawConstraints(void)
{
/*
 *	Draw the attached nodes.
 */
	uint i;
	for (i = 0; i < d_som.d_numNode; i++)
	{
/*
 *		The particle is attached to a bone.
 */
		if (d_node[i].d_master)
		{
			DrawUtility::Sphere(NodeVertex(i), DrawUtility::MAGENTA, 0.025f);
		}
/*
 *		The particle is attached directly to the reference instance.
 */
		else if (d_som.d_node[i].d_flag & NODE_ATTACHED)
		{
			DrawUtility::Sphere(NodeVertex(i), DrawUtility::RED, 0.025f);
		}
	}
/*
 *	Draw face constraints.
 */
	for (i = 0; i < d_som.d_numCollisionFace; i++)
	{
		DrawUtility::TriangleFace(
				NewVertex(d_som.d_collisionFace[i].d_index[0]),
				NewVertex(d_som.d_collisionFace[i].d_index[1]),
				NewVertex(d_som.d_collisionFace[i].d_index[2]),
				DrawUtility::WHITE, 0.025f, 0.5f);
	}
/*
 *	Draw edge constraints.
 */
	for (i = 0; i < d_som.d_numCollisionEdge; i++)
	{
		DrawUtility::Line(
				NewVertex(d_som.d_collisionEdge[i].d_index[0]),
				NewVertex(d_som.d_collisionEdge[i].d_index[1]),
				DrawUtility::GREEN);
	}
/*
 *	Draw point constraints.
 */
	for (i = 0; i < d_som.d_numCollisionPoint; i++)
	{
		DrawUtility::Sphere(NewVertex(d_som.d_collisionPoint[i]),
				DrawUtility::BLUE, 0.025f);
	}
}

/***************************************************************************/
/***************************************************************************/
#include <stdio.h>
void CMeshInstance::DumpMeshSOM(SMeshSOM &i_meshSOM)
{
	int i, j, k;
	int start, end;
	FILE* fp = fopen("mesh.dat", "w");

	fprintf(fp, "%s: %d nodes in %d sub-meshes.\n", i_meshSOM.d_name,
			i_meshSOM.d_numNode, i_meshSOM.d_numSubMesh);
	fprintf(fp, "   Position Node: %d, Drag Node: %d\n",
			i_meshSOM.d_positionNode, i_meshSOM.d_dragNode);

	fprintf(fp, "\nNodes (%d)\n", i_meshSOM.d_numNode);
	for (i = 0, start = 0; i < i_meshSOM.d_numNode; i++)
	{
		fprintf(fp, "   Node: %d\n", i);
		if (i_meshSOM.d_node[i].d_flag & NODE_ATTACHED)
		{
			if (i_meshSOM.d_node[i].d_masterBoneID == INVALID_BONEID)
				fprintf(fp, "      Attached to Model\n");
			else
				fprintf(fp, "      Attached to Bone (%d)\n",
						i_meshSOM.d_node[i].d_masterBoneID);
		}
		else
			fprintf(fp, "      Not Attached\n");

		fprintf(fp, "      Loc: [%f, %f, %f]\n",
				i_meshSOM.d_node[i].d_baseVertexInModel.x(),
				i_meshSOM.d_node[i].d_baseVertexInModel.y(),
				i_meshSOM.d_node[i].d_baseVertexInModel.z());

		fprintf(fp, "      Front Side Vertices: %d, Back Side Vertices: %d\n",
				i_meshSOM.d_node[i].d_numFrontSide,
				i_meshSOM.d_node[i].d_numBackSide);

		fprintf(fp, "      Thickness %s (%f)\n",
				(i_meshSOM.d_node[i].d_flag & NODE_HAS_THICKNESS) ? "ON" : "OFF",
				i_meshSOM.d_node[i].d_thickness);

		end = i_meshSOM.d_node[i].d_endNeighbor;
		fprintf(fp, "      Neighbors (%d)\n", end - start);
		for (j = start; j < end; j++)
		{
			fprintf(fp, "         Neighbor %d: Node %d%s\n", j - start,
					i_meshSOM.d_neighbor[j] & ~MESH_COUNT_FLAG,
					(i_meshSOM.d_neighbor[j] & MESH_COUNT_FLAG) ?
					"(End Neighbor)" : "");
		}
		start = end;
	}

	for (i = 0; i < i_meshSOM.d_numSubMesh; i++)
	{
		fprintf(fp, "\nSub-Mesh: %d\n", i);
		int numLayers;
		if (i_meshSOM.d_subMesh[i].d_textureContext)
		{
			numLayers = i_meshSOM.d_subMesh[i].d_textureContext->TextureCount;
			fprintf(fp, "   Texture Context layers (%d)\n", numLayers);
			for (j = 0; j < numLayers; j++)
			{
				fprintf(fp, "      Layer %d: %s\n", j, i_meshSOM.d_subMesh[
						i].d_textureContext->TextureLayer[j].TextureName);
			}
		}
		else
		{
			numLayers = 0;
			fprintf(fp, "   NULL Texture Context\n");
		}

		fprintf(fp, "   Vertices (%d)\n", i_meshSOM.d_subMesh[i].d_numVertex);
		for (j = 0; j < i_meshSOM.d_subMesh[i].d_numVertex; j++)
		{
			fprintf(fp, "      Vertex %d: Node %d (%s)\n", j,
					i_meshSOM.d_subMesh[i].d_nodeIndex[j] & ~MESH_COUNT_FLAG,
					(i_meshSOM.d_subMesh[i].d_nodeIndex[j] & MESH_COUNT_FLAG) ?
					"back" : "front");
			for (k = 0; k < numLayers; k++)
			{
				fprintf(fp, "         Layer %d Texture Coordinate: [%f %f]\n", k,
						i_meshSOM.d_subMesh[i].d_textureCoord[j * numLayers + k].x(),
						i_meshSOM.d_subMesh[i].d_textureCoord[j * numLayers + k].y());
			}
		}

		fprintf(fp, "   Tri-Strips (%d)\n",
				i_meshSOM.d_subMesh[i].d_numTriStrip);
		for (j = 0, start = 0; j < i_meshSOM.d_subMesh[i].d_numTriStrip; j++)
		{
			end =  i_meshSOM.d_subMesh[i].d_triStripHead[j];
			fprintf(fp, "      Tri-Strip %d: %d Points\n", j, end - start);
			for (k = start; k < end; k++)
			{
				fprintf(fp, "         Point %d: Vertex %d\n", k - start,
						i_meshSOM.d_subMesh[i].d_triStrip[k]);
			}
			start = end;
		}
	}

	fprintf(fp, "\nConstraints (%d)\n", i_meshSOM.d_numConstraint);
	for (i = 0; i < i_meshSOM.d_numConstraint; i++)
	{
		fprintf(fp, "   Constraint %d: [%d %d]\n", i,
				i_meshSOM.d_constraint[i].d_index[0],
				i_meshSOM.d_constraint[i].d_index[1]);
	}

	fprintf(fp, "\nCollision Faces (%d)\n", i_meshSOM.d_numCollisionFace);
	for (i = 0; i < i_meshSOM.d_numCollisionFace; i++)
	{
		fprintf(fp, "   Face %d: [%d %d %d]\n", i,
				i_meshSOM.d_collisionFace[i].d_index[0],
				i_meshSOM.d_collisionFace[i].d_index[1],
				i_meshSOM.d_collisionFace[i].d_index[2]);
	}

	fprintf(fp, "\nCollision Edges (%d)\n", i_meshSOM.d_numCollisionEdge);
	for (i = 0; i < i_meshSOM.d_numCollisionEdge; i++)
	{
		fprintf(fp, "   Edge %d: [%d %d]\n", i,
				i_meshSOM.d_collisionEdge[i].d_index[0],
				i_meshSOM.d_collisionEdge[i].d_index[1]);
	}

	fprintf(fp, "\nCollision Points (%d)\n", i_meshSOM.d_numCollisionPoint);
	for (i = 0; i < i_meshSOM.d_numCollisionPoint; i++)
	{
		fprintf(fp, "   Point %d: %d\n", i, i_meshSOM.d_collisionPoint[i]);
	}

	fclose(fp);
}
