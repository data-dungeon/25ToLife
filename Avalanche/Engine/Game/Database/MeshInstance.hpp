/*
**
**  File:   MeshInstance.hpp
**  Date:   April 25, 2002
**  By:     Bryant Collard
**  Desc:   An instance of a mesh.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshInstance.hpp $
**  $Revision: 5 $
**      $Date: 6/11/03 2:31p $
**    $Author: Tyler Colbert $
**
*/

#ifdef MESH_INSTANCE_HPP
#undef MESH_INSTANCE_HPP

/***************************************************************************/
/***************************************************************************/
inline Vector3Packed& CMeshInstance::ModelVertex(uint i_node)
{
	return(d_som.d_node[i_node].d_baseVertexInModel);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3Packed& CMeshInstance::NodeVertex(uint i_node)
{
	return(d_node[i_node].d_vertex->d_subMesh->Vertex(
			d_node[i_node].d_vertex->d_index));
}

/***************************************************************************/
/***************************************************************************/
inline Vector3& CMeshInstance::NodeNormal(uint i_node)
{
#ifdef VTX_BUFFER_NORMAL
	return(d_node[i_node].d_vertex->d_subMesh->Normal(
			d_node[i_node].d_vertex->d_index));
#else
	return(d_node[i_node].d_normalInWorld);
#endif
}

/***************************************************************************/
/***************************************************************************/
inline Vector3& CMeshInstance::NewVertex(uint i_node)
{
	return(d_node[i_node].d_newNodeInWorld);
}

/***************************************************************************/
/***************************************************************************/
inline Vector3& CMeshInstance::Velocity(uint i_node)
{
	return(d_node[i_node].d_velocityInWorld);
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::NewVertexToOBB(uint i_node, OBB* i_obb,
		Vector3 &o_vertexInOBB)
{
	o_vertexInOBB = (NewVertex(i_node) - (*(i_obb->Center()))) /
			(*(i_obb->Matrix()));
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::NewVertexFromOBB(uint i_node, OBB* i_obb,
		Vector3 &i_vertexInOBB)
{
	NewVertex(i_node) = i_vertexInOBB * (*(i_obb->Matrix())) +
			(*(i_obb->Center()));
}

/***************************************************************************/
/***************************************************************************/
inline bool CMeshInstance::TestPosFace(float i_vertex0, float i_vertex1,
		float i_vertex2, float i_face, float &o_depth)
{
	if (i_vertex1 < i_vertex0)
	{
		if (i_vertex2 < i_vertex1)
		{
			if (i_vertex2 >= i_face)
				return(true);
			o_depth = i_face - i_vertex2;
			return(false);
		}
		else
		{
			if (i_vertex1 >= i_face)
				return(true);
			o_depth = i_face - i_vertex1;
			return(false);
		}
	}
	else if (i_vertex2 < i_vertex0)
	{
		if (i_vertex2 >= i_face)
			return(true);
		o_depth = i_face - i_vertex2;
		return(false);
	}
	else
	{
		if (i_vertex0 >= i_face)
			return(true);
		o_depth = i_face - i_vertex0;
		return(false);
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool CMeshInstance::TestNegFace(float i_vertex0, float i_vertex1,
		float i_vertex2, float i_face, float &o_depth)
{
	if (i_vertex1 > i_vertex0)
	{
		if (i_vertex2 > i_vertex1)
		{
			if (i_vertex2 <= -i_face)
				return(true);
			o_depth = i_face + i_vertex2;
			return(false);
		}
		else
		{
			if (i_vertex1 <= -i_face)
				return(true);
			o_depth = i_face + i_vertex1;
			return(false);
		}
	}
	else if (i_vertex2 > i_vertex0)
	{
		if (i_vertex2 <= -i_face)
			return(true);
		o_depth = i_face + i_vertex2;
		return(false);
	}
	else
	{
		if (i_vertex0 <= -i_face)
			return(true);
		o_depth = i_face + i_vertex0;
		return(false);
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool CMeshInstance::TestCrossPlane(float i_planeDist,
		float i_vertexDist, float i_edgeDist, float &o_posDepth,
		float &o_negDepth)
{
	if (i_vertexDist < i_edgeDist)
	{
		o_posDepth = i_planeDist - i_vertexDist;
		if (o_posDepth <= 0.0f)
			return (true);
		o_negDepth = i_edgeDist + i_planeDist;
		if (o_negDepth <= 0.0f)
			return (true);
	}
	else
	{
		o_posDepth = i_planeDist - i_edgeDist;
		if (o_posDepth <= 0.0f)
			return (true);
		o_negDepth = i_vertexDist + i_planeDist;
		if (o_negDepth <= 0.0f)
			return (true);
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::CompareDepthSquared(float i_scaledDepth,
		float i_normalSqInv, float &io_minDepthSq, int i_index, int &o_minIndex)
{
	float depthSq = i_scaledDepth * i_scaledDepth * i_normalSqInv;
	if (depthSq < io_minDepthSq)
	{
		io_minDepthSq = depthSq;
		o_minIndex = i_index;
	}
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::ClampMin3(float i_min, float &io_val0,
		float &io_val1, float &io_val2)
{
	if (io_val0 < i_min)
		io_val0 = i_min;
	if (io_val1 < i_min)
		io_val1 = i_min;
	if (io_val2 < i_min)
		io_val2 = i_min;
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::ClampMax3(float i_max, float &io_val0,
		float &io_val1, float &io_val2)
{
	if (io_val0 > i_max)
		io_val0 = i_max;
	if (io_val1 > i_max)
		io_val1 = i_max;
	if (io_val2 > i_max)
		io_val2 = i_max;
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::ClampFaceToCrossPlane(float i_vertexDist,
		float i_edgeDist, float i_planeDist, float i_normalSqInv, float i_edgeA,
		float i_edgeB, float &io_vertex0A, float &io_vertex0B,
		float &io_vertex1A, float &io_vertex1B,
		float &io_vertex2A, float &io_vertex2B)
{
	if (i_edgeDist < i_planeDist)
	{
		float edgeDepth = (i_planeDist - i_edgeDist) * i_normalSqInv;
		float componentDepth = edgeDepth * i_edgeB;
		io_vertex1A -= componentDepth;
		io_vertex2A -= componentDepth;
		componentDepth = edgeDepth * i_edgeA;
		io_vertex1B += componentDepth;
		io_vertex2B += componentDepth;
	}
	if (i_vertexDist < i_planeDist)
	{
		float vertexDepth = (i_planeDist - i_vertexDist) * i_normalSqInv;
		io_vertex0A -= vertexDepth * i_edgeB;
		io_vertex0B += vertexDepth * i_edgeA;
	}
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::ClampToFace(bool i_posFace, float i_face,
		float &io_val0, float &io_val1)
{
	if (i_posFace)
	{
		if (io_val0 < i_face)
			io_val0 = i_face;
		if (io_val1 < i_face)
			io_val1 = i_face;
	}
	else
	{
		if (io_val0 > -i_face)
			io_val0 = -i_face;
		if (io_val1 > -i_face)
			io_val1 = -i_face;
	}
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::ClampEdgeToCrossPlane(bool i_posPlane,
		float i_scaledOverlap, float i_sizeA, float i_sizeB,
		float &io_startA, float &io_startB, float &io_endA, float &io_endB)
{
	float offsetA, offsetB;
	if (i_posPlane)
	{
		offsetA = -i_scaledOverlap * i_sizeB;
		offsetB = i_scaledOverlap * i_sizeA;
	}
	else
	{
		offsetA = i_scaledOverlap * i_sizeB;
		offsetB = -i_scaledOverlap * i_sizeA;
	}
	io_startA += offsetA;
	io_endA += offsetA;
	io_startB += offsetB;
	io_endB += offsetB;
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::FindConstraintCorner(OBB* i_obb,
		Vector3 &i_thickCorner, Vector3 &o_constraintCorner)
{
/*
 *	Transform the constraint direction into obb coordinates.
 */
	Vector3 dirInOBB = d_param->d_constraintDir / (*(i_obb->Matrix()));
/*
 *	Find the OBB corner in the direction of the constraint direction.
 */
	o_constraintCorner = i_thickCorner;
	if (dirInOBB.x() < 0.0f)
		o_constraintCorner.x( -o_constraintCorner.x());
	if (dirInOBB.y() < 0.0f)
		o_constraintCorner.y( -o_constraintCorner.y());
	if (dirInOBB.z() < 0.0f)
		o_constraintCorner.z( -o_constraintCorner.z());
/*
 *	Transform the corner into world coordinates.
 */
	o_constraintCorner = o_constraintCorner * (*(i_obb->Matrix())) +
			(*(i_obb->Center()));
}

/***************************************************************************/
/***************************************************************************/
inline void CMeshInstance::MoveToConstraintPlane(uint i_node,
		Vector3 &i_constraintCorner)
{
	float dist = d_param->d_constraintDir.Dot(i_constraintCorner -
			NewVertex(i_node));
	if (dist > 0.0f)
		NewVertex(i_node) += dist * d_param->d_constraintDir;
}

#endif // MESH_INSTANCE_HPP
