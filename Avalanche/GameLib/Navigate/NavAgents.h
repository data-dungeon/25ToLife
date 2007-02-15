/********************************************************************
	created:	2003/12/23
	created:	23:12:2003   10:28
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavAgents.h
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavAgents
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Implement some A* Agents.
*********************************************************************/

#if !defined( NAV_AGENTS_H )
#define NAV_AGENTS_H

#include "container/pqueue.h"
#include "container/skiplist.h"
#include "container/pool.h"

namespace Navigate
{
	class Mesh;
	class AStarSearchNode;
	class PQSearchNode;

	//! An A* Agent base class.
	/*!	This class is the base class for 'agents' used to perform the A* algorithm.
	*/
	class AStarAgent
	{
	public:
		/*!@name A* Agent Methods */
		//@{
		//!	Estimate the path cost between two Node objects.
		/*!	Estimates the cost of the path from the current Node (iFrom) to the Node (iTo).
			@param	mesh The Mesh to analyze.
			@param	iFrom The ID of the Node we're at in the search.
			@param	iTo The ID of the Node we're trying to reach.
			@return	An approximate cost of traversing from the current Node to the target Node.
		*/
		virtual float	PathCostEstimate( Mesh *mesh, NodeIndexType iFrom, NodeIndexType iTo ) = 0;

		//!	Estimate the cost of traversing a pair of neighboring Nodes.
		/*!	sGiven the Mesh pointer, this function will provide a traversal cost estimate for traveling from the iFrom Node to the iTo Node.
			@param	mesh The Mesh to analyze.
			@param	iParent The ID of the current Node's parent Node.
			@param	iFrom The ID of the current Node.
			@param	iTo The ID of the neighbor Node we're trying to traverse to.
			@return	A cost rating for making the suggested traversal.
		*/
		virtual float	TraverseCost( Mesh *mesh, NodeIndexType iParent, NodeIndexType iFrom, NodeIndexType iTo ) = 0;
		//@}

		//!@name Life-time management methods.
		//@{
		virtual void Reset( void ) = 0;
		//@}
	};

	//!	Implementation of AStarAgent.
	/*!	This A* Agent uses ( length_squared * 2 ) distances from a triangle's Centroid to the middle of any triangle edge to calculate traversal cost.
		It also uses straight-line distances to calculate the PathCostEstimate.
	*/
	class AStarNodeAgent : public AStarAgent
	{
	public:
		/*!@name Constructors */
		//@{
		AStarNodeAgent( void );
		//@}

		/*!@name Destructors */
		//@{
		virtual ~AStarNodeAgent( void );
		//@}

		/*!@name AStartAgent Methods */
		//@{
		virtual float	PathCostEstimate( Mesh *mesh, NodeIndexType iFrom, NodeIndexType iTo );
		virtual float	TraverseCost( Mesh* mesh, NodeIndexType iParent, NodeIndexType iFrom, NodeIndexType iTo );
		virtual void Reset( void );
		//@}

		/*!@name Implementation Data Members */
		//@{
		bool				bStarted;
		NodeIndexType	iFrom;
		NodeIndexType	iTo;

		DynamicArray< NodeIndexType >					nodesInPath;
		MinQueue< float, PQSearchNode* >				openList;
		SkipList< NodeIndexType, PQSearchNode*, 7 >	masterList;
		Pool< PQSearchNode >								nodePool;
		//@}
	};
}

#endif