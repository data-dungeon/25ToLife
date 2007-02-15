/********************************************************************
	created:	2003/12/04
	created:	4:12:2003   11:35
	filename: 	C:\dev\avalanche\engine\Game\Database\NavMesh.h
	file path:	C:\dev\avalanche\engine\Game\Database\
	file base:	NavMesh
	file ext:	h
	author:		Shon C. Love
	
	purpose:	In-game navigation mesh
*********************************************************************/

#ifndef NAVMESH_H
#define NAVMESH_H

#include "Math/Vector.h"
#include "collide/cube.h"

namespace Navigate
{
	class TreeNode;

	typedef unsigned short NodeIndexType;

	//! A node in the Navigate::Mesh data structure.
	/*!	Represents a particular triangle in the navigation mesh.  Stores neighbor, point, and flag data about the node.
	*/
#pragma pack( push, navigate_align, 1 )
	class Node
	{
	public:
		enum EdgeFlags
		{
			// the first 2 bits are dedicated to cover information
			EF_COVER_NONE			= 0x00,			//!< The edge offers no cover.
			EF_COVER_CROUCHING	= 0x01,			//!< The edge offers crouching cover - clear shot over cover while crouching.
			EF_COVER_STANDING		= 0x02,			//!< The edge offers standing cover - clear shot over cover while standing.
			EF_COVER_FULL			= 0x03,			//!< The edge offers full cover.

			// a bit-flag indicating that the edge is occluded
			EF_OCCLUDED				= 0x04,			//!< The edge is occluded by a dynamic object.
		};

		/*!@name Public Data Flags */
		//@{
		unsigned int	occlusionCount		: 5;	//! The number of times this node has been occluded by a dynamic object.
		unsigned int	edgeFlags_0			: 3;	//! Any flags (EdgeFlags) associated with edge 0.
		unsigned int	edgeFlags_1			: 3;	//! Any flags (EdgeFlags) associated with edge 1.
		unsigned int	edgeFlags_2			: 3;	//! Any flags (EdgeFlags) associated with edge 2.
		unsigned int	collisionTest		: 1;	//! Node collision test flag.
		unsigned int	pathingAttributes	: 8;	//! Face attributes for path searching.
		unsigned int	externalLinkIndex	: 9;	//! Place to record any external linkage information.
		//@}

		/*!@name Public Data Members */
		//@{
		NodeIndexType	neighbors[ 3 ];	//!< Index of our neighbor Nodes.
		NodeIndexType	points[ 3 ];	//!< Index into the Mesh's points array for the points of our triangle.
		//@}

#ifdef GCN
		void EndianSwap( )
		{
			/* flags */
			int *pFlags = (int *)this;

			ByteSwap( *pFlags);

			/* neighbors and points. NodeIndexType is currently typedef'd as an unsigned short  */

			ByteSwap( neighbors[0]);
			ByteSwap( neighbors[1]);
			ByteSwap( neighbors[2]);
			ByteSwap( points[0]);
			ByteSwap( points[1]);
			ByteSwap( points[2]);
		}
#endif //GCN
	};

	//! A navigation mesh.
	/*! This is a triangle mesh within which paths may be searched for.  There are several important properties about how the mesh is constructed to consider:
		-#	The mesh must be a triangle mesh - no arbitrary polygons.
		-#	The faces of all triangles in the mesh are defined Counter-Clockwise. (CCW)
		-#	The triangle mesh may be much more coarse than the terrain that it covers.
		-#	Try to keep the mesh roughly one meter over the terrain in Y.
		-#	The normals of all triangle faces must share a common 'direction'.  In this implementation, all normals must be y-up (positive y-values).
		-#	The mesh must be <i>contiguous</i>; meaning that there may be no T-junctions.
		-#	Triangles in the mesh may overlap in the XZ plane (since we're using y-up normals).
			-	Adjacent triangles may NOT overlap in XZ.
			-	Care should be taken to avoid overlaps that are too close in Y.  Give overlapping triangles a little room, vertically (0.5+).
		-#	A <i>Hard Edge</i> is a triangle edge that has only one triangle associated with it (i.e. there is no neighbor triangle through that edge).  All other edges are termed <i>Soft Edges</i>.
			-	Our system currently supports 4 types of cover flags that can be applied to the edges (<i>Hard/Soft</i>) of the triangles in the navigation mesh.
				-	<i>No Cover</i>
					-	The flagged edge offers no immediate (close) cover.  <i>Soft Cover</i> defaults to this type of cover.
				-	<i>Crouching Cover</i>
					-	The flagged edge offers 'firing' cover only to crouching people.  It offers little cover to a standing person.  A crouching person can peek over the crouching cover and fire a weapon, or duck down to hide.
				-	<i>Standing Cover</i>
					-	The flagged edge offers 'firing' cover to standing people.  A person can crouch behind this cover for full protection.  A standing person can peek over the standing cover and fire a weapon, or duck down to hide.
				-	<i>Full Cover</i>
					-	The flagged edge offers full protection.  The cover can not be fired/seen over.  To effectively use the cover to fire a weapon, one must move to the end of the cover edge and peek around.  <i>Hard Cover</i> defaults to this type of cover.
			-	All <i>Hard Edges</i> are assumed to be <i>Full Cover</i>, unless flagged otherwise in Maya.
			-	All <i>Soft Edges</i> are assumed to be <i>No Cover</i>, unless flagged otherwise in Maya.
			-	A node has only two cover flags.  One flag applies to all of the <i>Hard Edges</i> and the other flag applies to all of the <i>Soft Edges</i>.
				-	All <i>Hard Edges</i> on the same node share the same cover flag (<i>None, Crouching, Standing, or Full cover</i>).
				-	All <i>Soft Edges</i> on the same node share the same cover flag (<i>None, Crouching, Standing, or Full cover</i>).
				- <i>Hint:</i> If you have a node that has two <i>Hard Edges</i>, and they need different flags, split the node.  Same for <i>Soft Edges</i>.
	*/
	class Mesh
	{
	public:
		//! Fixup the data pointed to by base after loading it from disk.
		/*! This method is used mostly for the GameCube implementation (byte-swapping).
			@param	base A pointer to the data for the Mesh.
			@return	The data fixed up and stuffed into a Mesh object.  The returned Mesh object encompasses the memory pointed to by base, so don't delete base after this call.
		*/
		static Mesh		*DiskFixup( void *base );

		Node				*nodes;		//!< A Node array, representing all the Nodes in this Mesh.
		Vector3Packed	*points;		//!< A Vertex array, representing all the points in this Mesh.
		TreeNode			*root;		//!< This is the root of an octree database describing this Mesh.

		NodeIndexType	nodeCount;	//!< The number of Nodes in this Mesh.
		NodeIndexType	pointCount;	//!< The number of Points used by the Nodes in this Mesh.

		Octant			extents;		//!< The extents of the Mesh.
	};
#pragma pack( pop, navigate_align )
}

#endif