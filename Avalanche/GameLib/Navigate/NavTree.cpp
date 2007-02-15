/********************************************************************
	created:	2003/12/11
	created:	11:12:2003   14:44
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavTree.cpp
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavTree
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement the OctTree for navigaion
*********************************************************************/

#include "Navigate/NavigatePCH.h"

static void FixTreeNode( Navigate::TreeNode *root, unsigned int baseOffset )
{
#ifdef GCN
	ByteSwap( root->flags);
#endif //GCN

	if( root->flags & Navigate::TreeNode::LEAF)
	{
#ifdef GCN
		ByteSwap( root->data.leaf.nodeCount);
		ByteSwap( root->data.leaf.softEdgeCount);
		ByteSwap( root->data.leaf.hardEdgeCount);
		ByteSwap( root->data.leaf.extraCoverCount);
		ByteSwap( root->data.leaf.nodes);
		ByteSwap( root->data.leaf.edgeNodes);
		ByteSwap( root->data.leaf.edgeData);
#endif //GCN

		root->data.leaf.nodes			= ( Navigate::NodeIndexType* )( ( unsigned int )root->data.leaf.nodes + baseOffset );
		root->data.leaf.edgeNodes		= ( unsigned char* )( ( unsigned int )root->data.leaf.edgeNodes + baseOffset );
		root->data.leaf.edgeData		= ( Navigate::TreeNode::EdgeData* )( ( unsigned int )root->data.leaf.edgeData + baseOffset );

#ifdef GCN
		for( unsigned short iNode = 0; iNode < root->data.leaf.nodeCount; iNode++)
		{
			ByteSwap( root->data.leaf.nodes[iNode]);
		}
#endif //GCN
	}
	else
	{
		for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
		{
#ifdef GCN
			ByteSwap( root->data.branch.children[iCurrChild]);
#endif //GCN
			if ( root->data.branch.children[ iCurrChild ] == ( Navigate::TreeNode* )0xffffffff )
			{
				root->data.branch.children[ iCurrChild ] = NULL;
			}
			else
			{
				root->data.branch.children[ iCurrChild ] = ( Navigate::TreeNode* )( ( unsigned int )root->data.branch.children[ iCurrChild ] + baseOffset );
				FixTreeNode( root->data.branch.children[ iCurrChild ], baseOffset );
			}
		}
	}
}

Navigate::TreeNode *Navigate::TreeNode::DiskFixup( void *base, unsigned int baseOffset )
{
	Navigate::TreeNode *root = ( Navigate::TreeNode* )base;

	FixTreeNode( root, baseOffset );

	return( root );
}

