/*
**
**  File:   MeshSOM.h
**  Date:   April 25, 2002
**  By:     Bryant Collard
**  Desc:   A model (or "SOM") for a mesh. A mesh could be a completely
**          self-contained model. More typically, it will be a component
**          of a larger model. Each component is treated as an independent
**          model when rendering. An instance of a mesh can, however, refer
**          to an instance of another model to affect its behavior. In
**          fact, the mesh model is geometry is expressed relative to the
**          reference model coordinates.
**          
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshSOM.h $
**  $Revision: 5 $
**      $Date: 6/11/03 2:31p $
**    $Author: Tyler Colbert $
**
*/

#ifndef MESH_SOM_H
#define MESH_SOM_H

// Engine includes.
#include "Math/Vector.h"
#include "Game/Animation/Bone.h"
#include "Game/Database/DataTypes.h"

// GameLib includes.
#include "platform/BaseType.h"

// Forward declarations.
struct ts_TextureContext;
struct ts_PatchSOM;
struct ts_LOD;
struct ts_TextureSet;
class TextureArray;

/*
 *	Make a type that is used for array sizes and indices into the arrays.
 *	The most significant bit (MSB) may be used as a flag. The type should be
 *	large enough to accomodate the largest possible number of nodes in a mesh.
 *	(It seems that 15 bits [32768 nodes] should be enough.)
 */
typedef uint16 uintCount;
#define MESH_COUNT_FLAG 0x8000

/*
 *	Make a type that is the same size as a platform's pointer that is used
 *	in unions for offsets into a file.
 */
typedef uint32 uintOffset;

/*
 *	Defines for the node SOM flag member.
 */
#define NODE_HAS_THICKNESS   0x01
#define NODE_ATTACHED        0x02

/*
 *	Name length.
 */
#define MESH_NAME_LENGTH     32

struct SMeshSOM
{
/*
 *	Define the structures used in the model.
 *
 *	A node is a particle in the mesh.
 */
	struct SNodeSOM
	{
/*
 *		The location of the front side of the node in reference model
 *		coordinates.
 */
		Vector3Packed d_baseVertexInModel;
/*
 *		The number of sub-mesh vertices associated with the front side of the
 *		node.
 */
		uintCount d_numFrontSide;
/*
 *		The number of sub-mesh vertices associated with the back side of the
 *		node.
 */
		uintCount d_numBackSide;
/*
 *		The vertex normal is computed from the face normals of the triangles
 *		formed by the node and those surrounding it. An array of neighbor
 *		node indices is built that holds the concatination of the neighbors
 *		for each node. A node's neighbors are arranged in counter-clockwise
 *		order. If an edge defined by a neighbor finishes a triangle but does
 *		not start one, the MSB of the neighbor node index is set. The following
 *		variable contains the index into the neighbor node array after the node's
 *		last neighbor.
 */
		uintCount d_endNeighbor;
/*
 *		If the node has thickness, indicated in the flag member, the distance
 *		from the front side to the back side. Computed from the length of a bone
 *		in model coordinates.
 */
		float d_thickness;
/*
 *		The node may be rigidly attached to the reference model. If it is,
 *		the flag member will indicated it and the current position of the node
 *		may be determined by assuming that the node vertex is associated with the
 *		master bone and it is placed just like the other vertices associated with
 *		the bone. If the node is rigidly attached but to a bone, the master bone
 *		ID should be INVALID_BONEID. In that case, the current position of the
 *		node is determined by simply transforming the base position of the vertex
 *		in the model into world coordinates using the reference instance model
 *		to world matrix.
 */
		BoneID d_masterBoneID;
/*
 *		A bit field for flags (integrate with padding?).
 */
		uint8 d_flag;
	};
/*
 *	A sub-mesh is a group of verticies and associated tri-strips that have
 *	a common texture context. In an instance, a sub-mesh is stored in a
 *	vertex buffer and a tri-strip buffer. The following structure contains all
 *	the information needed to create the buffers.
 */
	struct SSubMesh
	{
/*
 *		Put the pointer sized things first.
 *
 *		The texture context used by the sub-mesh.
 */
		union
		{
			uintOffset d_textureContextIndex;
			ts_TextureContext* d_textureContext;
		};
/*
 *		An array of indices into the mesh node array. Each entry corresponds to
 *		a vertex in the sub-mesh and the order (hopefully) results in efficient
 *		display of the tri-strips. The position of the vertex is obtained from
 *		the corresponding node in the mesh node array. If the MSB of the entry
 *		is not set, the vertex corresponds to the front side of the node. If the
 *		MSB is set, the vertex corresponds to the back side of the node. The size
 *		of the node index array is stored in d_numVertex;
 */
		union
		{
			uintOffset d_nodeIndexOffset;
			uintCount* d_nodeIndex;
		};
/*
 *		An array of texture coordinates. Entries are grouped by vertex. The size
 *		of each group is determined by the number of texture layers specified
 *		in the texture context.
 */
		union
		{
			uintOffset d_textureCoordOffset;
			Vector2* d_textureCoord;
		};
/*
 *		An array of indices into the tri-strip array. Each entry indicates the
 *		the beginning of the next tri-strip. So, entry 0 is the index of the
 *		beginning of tri-strip 1, entry 1 is the index of the beginning of
 *		tri-strip 2, etc. The last entry ends up being the total number of
 *		tri-strip vertex references. Its (the tri-strip head array) size is
 *		stored in d_numTriStrips.
 */
		union
		{
			uintOffset d_triStripHeadOffset;
			uintCount* d_triStripHead;
		};
/*
 *		An array of indices into the node index array. A tri-strip is simply an
 *		ordered list of vertices. Inside this sub-mesh structure, a vertex is
 *		represented by an entry in the node index array. The sub-mesh can be
 *		formed by a number of tri-strips. This array is a concatination of all
 *		of the tri-strip lists. Its size is stored in the last entry of the
 *		tri-strip head array.
 */
		union
		{
			uintOffset d_triStripOffset;
			uint16* d_triStrip;
		};
/*
 *		The number of verticies and tri-strips in the sub-mesh.
 */
		uintCount d_numVertex;
		uintCount d_numTriStrip;
	};
/*
 *	Some structs for sets of indices into the node array.
 */
	struct SIndexPair
	{
		uintCount d_index[2];
	};
	struct SIndexTrio
	{
		uintCount d_index[3];
	};
/*
 *	The data header which includes type, sub type, flags, priority.
 *	-- MUST BE FIRST-- so the beginning of all SOMs match. See comments in
 *	datatypes.h
 */
	ts_FlagsDataHeader dh;
/*
 *	Now for the data starting with pointer sized things.
 *
 *	The list of nodes in the mesh.
 */
	union
	{
		uintOffset d_nodeOffset;
		SNodeSOM* d_node;
	};
/*
 *	The mesh is updated by integrating each node forward in time then
 *	repeatedly moving the nodes in an attempt to keep specified pairs of
 *	nodes some distance apart. The list of constraint pairs.
 */
	union
	{
		uintOffset d_constraintOffset;
		SIndexPair* d_constraint;
	};
/*
 *	In addition to the attempt to meet the length constraint, the nodes are
 *	are moved to prevent the mesh from passing through objects. For the
 *	most part, the goal to to prevent penetration of a collection of
 *	triangle faces specified by the triangle vertices.
 */
	union
	{
		uintOffset d_collisionFaceOffset;
		SIndexTrio* d_collisionFace;
	};
/*
 *	Sometimes all of the vertices of a triangle cannot be moved to prevent
 *	penetration. Sometimes nodes can, however, be move to prevent penetration
 *	of a triangle edge.
 */
	union
	{
		uintOffset d_collisionEdgeOffset;
		SIndexPair* d_collisionEdge;
	};
/*
 *	Finally, it may be possible to move individual nodes.
 */
	union
	{
		uintOffset d_collisionPointOffset;
		uintCount* d_collisionPoint;
	};
/*
 *	The list of sub-meshes.
 */
	union
	{
		uintOffset d_subMeshOffset;
		SSubMesh* d_subMesh;
	};
/*
 *	The aggregate neighbor array.
 */
	union
	{
		uintOffset d_neighborOffset;
		uintCount* d_neighbor;
	};
/*
 *	The texture set and sub set index for the SOM. (Union needed?)
 */
	TextureArray* d_textureArray;
	uint32 d_textureSubSetIndex;
/*
 *	The sizes of the preceding arrays.
 */
	uintCount d_numNode;
	uintCount d_numConstraint;
	uintCount d_numCollisionFace;
	uintCount d_numCollisionEdge;
	uintCount d_numCollisionPoint;
	uintCount d_numSubMesh;
/*
 *	Identify a node whose position can be used as the position of the
 *	entire mesh.
 */
	uintCount d_positionNode;
/*
 *	Only nodes that are not rigidly attached to something can be affected
 *	by drag. Identify the node closest to the center of those that can
 *	be affected.
 */
	uintCount d_dragNode;
/*
 *	Provide a name for debugging and to hook up to parameters.
 */
	char d_name[MESH_NAME_LENGTH];
/*
 *	Unfinished business:
 *		Attachment to bones requires a reference to a specific instance. For now,
 *			that association is made when the multi-SOM is built. A more
 *			general and robust solution should be found.
 *		It seems that the artist should be able to specify level-of-detail
 *			behavior. For now, that comes in through as a parameter.
 */
};

#endif MESH_SOM_H
