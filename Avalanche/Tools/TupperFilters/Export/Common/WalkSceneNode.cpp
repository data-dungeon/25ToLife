////////////////////////////////////////////////////////////////////////////
//
// ExportTextureContext
//
// Export the texture contexts
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: WalkSceneNode.cpp $
 * 
 * *****************  Version 13  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:54p
 * Updated in $/Avalanche/tools/TupperFilters/ExportGCN
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:23p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * new bounding
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 3:24p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * render vs collision instances
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * hybrid configuration
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 12:33p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Abort checks within system
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:56p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed to use Annotate filter
 * Added boundign and annotate helper references
 * Removed export pass
 * Added Dependencies
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/07/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * UV Offsets and Tiling now altering the uvs within the model
 * Pushed pools up to top of export dbl 
 * Renamed all inserted tupperware atoms to ExportPC - 
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed Exception Problem
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

#include "ExportPch.h"
#include "WalkSceneNode.h"
// static member variables
const float WalkSceneNode::BOUNDING_NODE_EXPAND = (20.0f / 100.0f);
CSceneNode *WalkSceneNode::m_pBestBoundingParent = NULL;
int WalkSceneNode::m_nBestBoundingParentDepth;
ts_BoundingBox *WalkSceneNode::m_pBestBoundingInstBox = NULL;
bool WalkSceneNode::m_bBestBoundingExpand;
float WalkSceneNode::m_fDivideTargetSize;

/* swap for big endian machines */

/* set target bounding box size for WalkToDivideXZ or WalkToDivideXYZ */
void WalkSceneNode::SetDivideTargetSize(float fTargetSize)
{
	m_fDivideTargetSize = fTargetSize;	// in meters
}

/* find bounding node that best fits passed bounding box */
void WalkSceneNode::InitBestBoundingParent(CSceneNode &pParent, ts_BoundingBox *pInstBox, bool bExpand)
{
	m_pBestBoundingParent = &pParent;
	m_nBestBoundingParentDepth = pParent.GetDepth();
	m_pBestBoundingInstBox = pInstBox;
	m_bBestBoundingExpand = bExpand;
}

CSceneNode *WalkSceneNode::GetBestBoundingParent(void)
{
	return(m_pBestBoundingParent);
}

/* divide a bounding box into 2 or 4 child bounding boxes if it is larger than the target size */
void WalkSceneNode::WalkToDivideXZ(CSceneNode &sceneNode)
{
	// if a main node then do my children
	if (sceneNode.GetType() == SCENE_NODE_ALWAYS_PASS)
	{
		CSceneNode *pChild = sceneNode.GetFirstChild();
		while (pChild)
		{
			WalkToDivideXZ(*pChild);
			pChild = pChild->GetNextSibling();
		}
	}
	else
	{
		assert(sceneNode.GetType() == SCENE_NODE_BOUNDING_BOX);	// only for use in building initial bounding boxes for tree
		ts_BoundingBox *pBox = (ts_BoundingBox *) sceneNode.GetData();
		float fXChildSize = ( pBox->Max.x( ) - pBox->Min.x( ) ) / 2.0f;
		float fZChildSize = ( pBox->Max.z( ) - pBox->Min.z( ) ) / 2.0f;
		if ((fXChildSize < m_fDivideTargetSize) && (fZChildSize < m_fDivideTargetSize))
			return;	// done-- no more children
		
		/* how to divide? may want to divide in both x and y, or in x only, or in z only */
		
		CSceneNode *pChild;
		ts_BoundingBox *pChildBox;
		
		/* first child is from min x to center x, min z to center z. if current x is already < target size, */
		/* don't divide in x (that is, only divide in z). same for z. dividing into 4 will use all 4 children. */
		/* dividing only in x will use children 1 and 2. dividing only in z will use children 1 and 3 */
		
		pChildBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
		*pChildBox = *pBox;
		if (fXChildSize >= m_fDivideTargetSize)	// may not need to divide in x
			pChildBox->Max.x( pChildBox->Min.x( ) + fXChildSize );	// max x = center x
		if (fZChildSize >= m_fDivideTargetSize)	// may not need to divide in z
			pChildBox->Max.z( pChildBox->Min.z( ) + fZChildSize );	// max z = center z
		pChild = new CSceneNode;
		pChild->Init();
		pChild->SetType(SCENE_NODE_BOUNDING_BOX);
		pChild->SetData(pChildBox);
		sceneNode.AddChild(pChild);
		WalkToDivideXZ(*pChild);	// recursively divide some more
		
		/* 2nd child node-- center x to max x, min z to center z. only need if dividing in x */
		
		if (fXChildSize >= m_fDivideTargetSize)
		{
			pChildBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
			*pChildBox = *pBox;
			pChildBox->Min.x( pChildBox->Min.x( ) + fXChildSize );							// min x = center x
			if (fZChildSize >= m_fDivideTargetSize)	// may not need to divide in z
				pChildBox->Max.z( pChildBox->Min.z( ) + fZChildSize );	// max z = center z
			pChild = new CSceneNode;
			pChild->Init();
			pChild->SetType(SCENE_NODE_BOUNDING_BOX);
			pChild->SetData(pChildBox);
			sceneNode.AddChild(pChild);
			WalkToDivideXZ(*pChild);	// recursively divide some more
		}
		
		/* 3rd child node-- min x to center x, center z to max z. only need if dividing in z */
		
		if (fZChildSize >= m_fDivideTargetSize)
		{
			pChildBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
			*pChildBox = *pBox;
			if (fXChildSize >= m_fDivideTargetSize)	// may not need to divide in x
				pChildBox->Max.x( pChildBox->Min.x( ) + fXChildSize );	// max x = center x
			pChildBox->Min.z( pChildBox->Min.z( ) + fZChildSize );							// min z = center z
			pChild = new CSceneNode;
			pChild->Init();
			pChild->SetType(SCENE_NODE_BOUNDING_BOX);
			pChild->SetData(pChildBox);
			sceneNode.AddChild(pChild);
			WalkToDivideXZ(*pChild);	// recursively divide some more
		}
		
		/* 4th child node-- center x to max x, center z to max z. only need if dividing into 4 sub-nodes */
		
		if ((fXChildSize >= m_fDivideTargetSize) && (fZChildSize >= m_fDivideTargetSize))
		{
			pChildBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
			*pChildBox = *pBox;
			pChildBox->Min.x( pChildBox->Min.x( ) + fXChildSize );							// min x = center x
			pChildBox->Min.z( pChildBox->Min.z( ) + fZChildSize );							// min z = center z
			pChild = new CSceneNode;
			pChild->Init();
			pChild->SetType(SCENE_NODE_BOUNDING_BOX);
			pChild->SetData(pChildBox);
			sceneNode.AddChild(pChild);
			WalkToDivideXZ(*pChild);	// recursively divide some more
		}
	}
}

/* find the smallest bounding box that contains instance. call InitBestBoundingParent first, call GetBestBoundingParent
to retrieve results */

void WalkSceneNode::WalkToFindBestBoundingParentXZ(CSceneNode &sceneNode,int nDepth)
{
	/* instance nodes are not under consideration */
	if (sceneNode.GetType() == SCENE_NODE_INSTANCE)
	{
		assert(sceneNode.GetFirstChild() == NULL);	// instances shouldn't have any child nodes
		return;
	}

	/* this routine is originally passed in the tree root, not a bounding box */

	if (sceneNode.GetType() == SCENE_NODE_BOUNDING_BOX)
	{
		/* we allow the bounding node boxes to expand a bit to include instances. this is because when large models */
		/* are broken up along grid bounds, we don't cut polys-- they extend out of the grid bounds. but the broken */
		/* up pieces should fit in a grid-sized bounding node */

		static ts_BoundingBox NodeBox;	// static so it won't take up stack space during recursion
		NodeBox = *((ts_BoundingBox *) sceneNode.GetData());
		if (m_bBestBoundingExpand)
		{
			static float fXSize;				// static so it won't take up stack space during recursion
			static float fZSize;				// static so it won't take up stack space during recursion
			fXSize = NodeBox.Max.x( ) - NodeBox.Min.x( );
			fZSize = NodeBox.Max.z( ) - NodeBox.Min.z( );
			NodeBox.Min.x( NodeBox.Min.x( ) - fXSize * BOUNDING_NODE_EXPAND );
			NodeBox.Min.z( NodeBox.Min.z( ) - fZSize * BOUNDING_NODE_EXPAND );
			NodeBox.Max.x( NodeBox.Max.x( ) + fXSize * BOUNDING_NODE_EXPAND );
			NodeBox.Max.z( NodeBox.Max.z( ) + fZSize * BOUNDING_NODE_EXPAND );
		}

		/* does instance fit? */

		if (Bounding::ContainsPointXZ(NodeBox.Min,NodeBox.Max,m_pBestBoundingInstBox->Min) == FALSE)
			return;	// doesn't fit, no point in recursing through children
		if (Bounding::ContainsPointXZ(NodeBox.Min,NodeBox.Max,m_pBestBoundingInstBox->Max) == FALSE)
			return;	// doesn't fit, no point in recursing through children
	}

	/* instance fits. is this parent better than current best? */

	if (nDepth > m_nBestBoundingParentDepth)
	{
		assert(nDepth == sceneNode.GetDepth());
		m_pBestBoundingParent = &sceneNode;
		m_nBestBoundingParentDepth = nDepth;
	}

	/* go recursive to keep looking */

	CSceneNode *pChild = sceneNode.GetFirstChild();
	while (pChild)
	{
		WalkToFindBestBoundingParentXZ(*pChild,nDepth + 1);
		pChild = pChild->GetNextSibling();
	}
}

/* remove bounding nodes that have 0 or 1 children. if they have 0 children, obviously they're not needed. if they have
one child, it may be an instance or a bounding box. in the case of the instance, the bounding node's box will just match
the instance's bounding box, so it's redundant. in the case of a single bounding box node, it's clear that the parent
is redundant */

void WalkSceneNode::WalkToRemoveUnneededBoundingNodes(CSceneNode &sceneNode)
{
	/* first process children, so that we have valid status when we look at current node */

	CSceneNode *pChild;
	CSceneNode *pNextChild = sceneNode.GetFirstChild();
	while (pNextChild)
	{
		pChild = pNextChild;
		pNextChild = pNextChild->GetNextSibling();	// child may get removed, so get next pointer before processing
		WalkToRemoveUnneededBoundingNodes(*pChild);
	}

	/* now look at this node */

	if (sceneNode.GetType() == SCENE_NODE_BOUNDING_BOX)
	{
		static int nChildCount;	// static so it won't take up stack space during recursion
		nChildCount = sceneNode.ChildCount();

		/* 1 child? remove this node and move child up */

		if (nChildCount == 1)
		{
			pChild = sceneNode.GetFirstChild();
			pChild->Remove();
			sceneNode.GetParent()->AddChild(pChild);
		}

		/* 0 or 1 children? delete */

		if (nChildCount <= 1)
		{
			sceneNode.Remove();

			/* free memory */

			free(sceneNode.GetData());
			delete &sceneNode;
		}
	}
}

/* resize the bounding nodes so that they contain all their children */

void WalkSceneNode::WalkToResizeBoundingNodes(CSceneNode &sceneNode)
{
	/* first process children, so that we have valid status when we look at current node */

	CSceneNode *pChild = sceneNode.GetFirstChild();
	while (pChild)
	{
		WalkToResizeBoundingNodes(*pChild);
		pChild = pChild->GetNextSibling();
	}

	/* now look at this node */

	if (sceneNode.GetType() == SCENE_NODE_BOUNDING_BOX)
	{
		/* first clear it to get best results-- contents may be smaller than original quad tree */

		static ts_BoundingBox *pBox;	// static so it won't take up stack space during recursion
		pBox = (ts_BoundingBox *) sceneNode.GetData();
		Bounding::Clear(pBox->Min,pBox->Max);

		/* and include children */
		pChild = sceneNode.GetFirstChild();
		while (pChild)
		{
			switch(pChild->GetType())
			{
			case SCENE_NODE_INSTANCE:
				static TupperwareAggregate *pNodeAgg;	// static so it won't take up stack space during recursion
				pNodeAgg = (TupperwareAggregate *) pChild->GetData();
				Bounding::AddToBoundingBox(pBox->Min,pBox->Max,*(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat());
				Bounding::AddToBoundingBox(pBox->Min,pBox->Max,*(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat());
				break;
			case SCENE_NODE_ALWAYS_PASS:
				break;
			case SCENE_NODE_BOUNDING_BOX:
				static ts_BoundingBox *pChildBox;	// static so it won't take up stack space during recursion
				pChildBox = (ts_BoundingBox *) pChild->GetData();
				Bounding::AddToBoundingBox(pBox->Min,pBox->Max,pChildBox->Min);
				Bounding::AddToBoundingBox(pBox->Min,pBox->Max,pChildBox->Max);
				break;
			default:
				assert(FALSE);	// unknown node type
			}
			pChild = pChild->GetNextSibling();
		}

		/* should never end up empty-- means an error in the tree */

		assert(Bounding::IsEmpty(pBox->Min,pBox->Max) == FALSE);
	}
}

/* walk the tree and write the nodes out to the .dbl file -- ImportExport only. this is kind of a hack, */
/* but this is a two stage process-- walk the tree once to write the records out so that we know where */
/* they end up in the file. then walk again to set the file offset relationships between the nodes. for */
/* example, since a node may have children, it's hard to predict where they will end up in the file. by */
/* writing all the nodes out and recording where each one is in the file, i can go back and fix up offsets */
/* in the second pass (WalkToFixExportData). the only real hack part of this is that i use the m_pNextRender */
/* field to record the file position during the first pass, since i know that is a run-time field only */

void WalkSceneNode::WalkToExportData(CSceneNode &sceneNode)
{
	if (PluginLog::GetAbortFlag()) throw(PluginException(ERROR_FATAL,"Processing Stopped By User"));

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	/* record file offset in m_pNextRender field. m_pNextRender is a run-time only field, it does not */
	/* contain any data in the database. this is used by WalkToFixExportData */

	sceneNode.SetNextRender( (CSceneNode *) (ftell(pDBLFile) - DBLUtil::GetChunkDataStart()) );	// offset from start of chunk

	/* what kind of node? this must match corresponding section in WalkToFixExportData */

	switch(sceneNode.GetType())
	{
	case SCENE_NODE_INSTANCE:
	case SCENE_NODE_ALWAYS_PASS:
		try
		{
			DBLWrite::WriteRecord(&sceneNode, pDBLFile);
		}
		catch (PluginException e)
		{
			throw (PluginException(ERROR_FATAL,"WalkToExportData - Write scene node record for Instance or always pass failed\r\n  %s",(const char *)(e)));
		}
		break;
	case SCENE_NODE_BOUNDING_BOX:
		/* for bounding box node, write out node and then immediately follow with bounding box record */
		try
		{
			DBLWrite::WriteRecord(&sceneNode, pDBLFile);
			DBLWrite::WriteRecord((ts_BoundingBox *) sceneNode.GetData(), pDBLFile);
		}
		catch (PluginException e)
		{
			throw (PluginException(ERROR_FATAL,"WalkToExportData - Write scene node record for bounding box failed\r\n  %s",(const char *)(e)));
		}
		break;
	default:
		throw (PluginException(ERROR_FATAL,"WalkToExportData - Unknown node type"));
	}

	/* process children (recursive) */

	CSceneNode *pChild = sceneNode.GetFirstChild();
	while (pChild)
	{
		WalkToExportData(*pChild);
		pChild = pChild->GetNextSibling();
	}
}

/* this is the second pass of the two-pass data export process. when this is called, all the nodes in the */
/* scene tree have their file position stored in the m_pNextRender field (as a u32), so i can calculate */
/* the file offsets necessary to link them back together at run-time */
#define SCENE_NODE_DISK_NULL		(CSceneNode *) 0xffffffff

void WalkSceneNode::WalkToFixExportData(CSceneNode &sceneNode)
{
	static FILE *pDBLFile = DBLUtil::GetDBLFile();

	/* copy contents into a node i can edit, then set the pointers to offsets. each node's m_pNextRender */
	/* field contains offset from the start of the chunk data. 0 is a valid value (it is the file offset */
	/* of the first node in the tree), so flag NULL pointers with another value */

	static CSceneNode CopyNode;	// static so it won't take up stack space during recursion
	CopyNode = sceneNode;
	if (sceneNode.GetParent())
		CopyNode.SetParent(sceneNode.GetParent()->GetNextRender());
	else
		CopyNode.SetParent(SCENE_NODE_DISK_NULL);
	if (sceneNode.GetFirstChild())
		CopyNode.SetFirstChild(sceneNode.GetFirstChild()->GetNextRender());
	else
		CopyNode.SetFirstChild(SCENE_NODE_DISK_NULL);
	if (sceneNode.GetNextSibling())
		CopyNode.SetNextSibling(sceneNode.GetNextSibling()->GetNextRender());
	else
		CopyNode.SetNextSibling(SCENE_NODE_DISK_NULL);
	CopyNode.SetNextRender(NULL);	// just to be clean, doesn't need fix up at run-time load

	/* fix up m_pData pointer contents. should be ok for most things already */

	switch(sceneNode.GetType())
	{
	case SCENE_NODE_INSTANCE:
		/* convert m_pData from CInstanceInfo pointer to export index */

		static TupperwareAggregate *pNodeAgg;	// static so it won't take up stack space during recursion
		pNodeAgg = (TupperwareAggregate *) sceneNode.GetData();
//		assert(pInstanceInfo->IsValidForSceneTree());
		CopyNode.SetData((void *) pNodeAgg->FindScalarByKey("Annotate_RenderInstanceIndex")->GetAsInt());	// will get fixed up at game load time
		break;
	case SCENE_NODE_ALWAYS_PASS:
		CopyNode.SetData(NULL);
		break;
	case SCENE_NODE_BOUNDING_BOX:
		/* currently points to a valid ts_BoundingBox record, but for disk purposes this will always be fixed */
		/* up at load time to point to bounding record that immediately follows node record, so clear it just */
		/* to be clean */

		CopyNode.SetData(NULL);
		break;
	default:
		throw (PluginException(ERROR_FATAL,"WalkToFixExportData - Unknown node type"));
	}

	/* what kind of node? this must match corresponding section in WalkToExportData, except it writes out */
	/* CopyNode instead of current node */

	switch(sceneNode.GetType())
	{
	case SCENE_NODE_INSTANCE:
	case SCENE_NODE_ALWAYS_PASS:
		try
		{
			DBLWrite::WriteRecord(&CopyNode, pDBLFile);
		}
		catch (PluginException e)
		{
			throw (PluginException(ERROR_FATAL,"WalkToFixExportData - Write scene node record for Instance or always pass failed\r\n  %s",(const char *)(e)));
		}
		break;
	case SCENE_NODE_BOUNDING_BOX:
		/* for bounding box node, write out node and then immediately follow with bounding box record */
		try
		{
			DBLWrite::WriteRecord(&CopyNode, pDBLFile);
			DBLWrite::WriteRecord((ts_BoundingBox *) sceneNode.GetData(), pDBLFile);
		}
		catch (PluginException e)
		{
			throw (PluginException(ERROR_FATAL,"WalkToFixExportData - Write scene node record for Bounding box failed\r\n  %s",(const char *)(e)));
		}
		break;
	default:
		throw (PluginException(ERROR_FATAL,"WalkToFixExportData - Unknown node type"));
	}

	/* process children (recursive) */

	CSceneNode *pChild = sceneNode.GetFirstChild();
	while (pChild)
	{
		WalkToFixExportData(*pChild);
		pChild = pChild->GetNextSibling();
	}
}

/* recursively free a scene tree. assumes all nodes were created via new, and that all bounding boxes were malloc'd */

void WalkSceneNode::WalkToFree(CSceneNode &sceneNode)
{
	/* first process children-- tree frees itself from leaves to the root */

	CSceneNode *pNextChild = sceneNode.GetFirstChild();
	while (pNextChild)
	{
		CSceneNode *pChild = pNextChild;
		pNextChild = pNextChild->GetNextSibling();	// child may get removed, so get next pointer before processing
		WalkToFree(*pChild);
	}

	/* now look at this node-- data to free? */

	switch(sceneNode.GetType())
	{
	case SCENE_NODE_INSTANCE:
	case SCENE_NODE_ALWAYS_PASS:
		break;
	case SCENE_NODE_BOUNDING_BOX:
		free(sceneNode.GetData());	// bounding box data is malloc'd
		break;
	default:
		throw (PluginException(ERROR_FATAL,"WalkToFree - Unknown node type"));
	}
	/* and then the node itself */
	delete &sceneNode;
}

