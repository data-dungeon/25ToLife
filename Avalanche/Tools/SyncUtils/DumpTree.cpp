////////////////////////////////////////////////////////////////////////////
//
// DumpTree
//
// Class for printing out the tree nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DumpTree.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 10/30/02   Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * tree stuff
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync tree first pass complete
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/08/02   Time: 9:39a
 * Updated in $/Avalanche/tools/SyncUtils
 * pulled back to simple tree structure
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/07/02   Time: 1:14p
 * Updated in $/Avalanche/tools/SyncUtils
 * file length
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:45p
 * Updated in $/Avalanche/tools/SyncUtils
 * trace dump
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

#include "stdafx.h"
#include "SyncUtils.h"
#include "DumpTree.h"
#include "TreeNodeFolder.h"
#include "TreeNodeFile.h"
#include "TreeIterator.h"
#include "SyncTreeNodeFolder.h"
#include "SyncTreeNodeFile.h"
#include "SyncTreeNodeFlags.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void DumpTreeDumperTrace::Dump(const char *pString,int indent)
{
	TupString string;
	for (int indentIndex=0;indentIndex<indent;indentIndex++)
	{
		string += "\t";
	}
	string += pString;
	TRACE(_T(string));
	TRACE(_T("\n"));
}

void DumpTree::DoDump(TreeNodeBase *pNode,DumpTreeDumperBase &dumper,int indent)
{
	TupString string;

	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		string += "[ ";
		string += pNode->GetFileName();
		string += "\t";
		switch (pSyncNodeFolder->GetFlags()&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
				{
					string += "Both Inactive";
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
				{
					string += "Left Only";
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
				{
					string += "Right Only";
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				{
					string += "Both";
				}
				break;
		}
		string += " ]";
		dumper.Dump(string,indent);
		TreeIterator first(pSyncNodeFolder);
		TreeIterator last(pSyncNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			DoDump((*first),dumper,indent+1);
		}
		return;
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		string += pNode->GetFileName();
		string += "\t";
		switch (pSyncNodeFile->GetFlags()&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
				{
					string += "Both Inactive";
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
				{
					string += "Left Only\t";
					CString lengthStr;
					TreeNodeFile *pNodeFile = pSyncNodeFile->GetLeftFileNode();
					ASSERT(pNodeFile); // better be a file node

					lengthStr.Format("%u bytes\t",pNodeFile->GetFileLength());
					string += lengthStr;
					CTime tempTime(pNodeFile->GetModifiedTime());
					CString timeStr = tempTime.Format("%c");
					string += timeStr;
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
				{
					string += "Right Only\t";
					CString lengthStr;
					TreeNodeFile *pNodeFile = pSyncNodeFile->GetRightFileNode();
					ASSERT(pNodeFile); // better be a file node

					lengthStr.Format("%u bytes\t",pNodeFile->GetFileLength());
					string += lengthStr;
					CTime tempTime(pNodeFile->GetModifiedTime());
					CString timeStr = tempTime.Format("%c");
					string += timeStr;
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
				{
					string += "Left Newer\t";
					CString lengthStr;
					TreeNodeFile *pLeftNodeFile = pSyncNodeFile->GetLeftFileNode();
					TreeNodeFile *pRightNodeFile = pSyncNodeFile->GetRightFileNode();
					ASSERT(pLeftNodeFile); // better be a file node
					ASSERT(pRightNodeFile); // better be a file node

					lengthStr.Format("%u bytes\t",pLeftNodeFile->GetFileLength());
					string += lengthStr;
					CTime tempLeftTime(pLeftNodeFile->GetModifiedTime());
					CString timeStr = tempLeftTime.Format("%c");
					string += timeStr;
					string += "\t";
					lengthStr.Format("%u bytes\t",pRightNodeFile->GetFileLength());
					string += lengthStr;
					CTime tempRightTime(pRightNodeFile->GetModifiedTime());
					timeStr = tempRightTime.Format("%c");
					string += timeStr;
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
				{
					string += "Right Newer\t";
					CString lengthStr;
					TreeNodeFile *pLeftNodeFile = pSyncNodeFile->GetLeftFileNode();
					TreeNodeFile *pRightNodeFile = pSyncNodeFile->GetRightFileNode();
					ASSERT(pLeftNodeFile); // better be a file node
					ASSERT(pRightNodeFile); // better be a file node

					lengthStr.Format("%u bytes\t",pLeftNodeFile->GetFileLength());
					string += lengthStr;
					CTime tempLeftTime(pLeftNodeFile->GetModifiedTime());
					CString timeStr = tempLeftTime.Format("%c");
					string += timeStr;
					string += "\t";
					lengthStr.Format("%u bytes\t",pRightNodeFile->GetFileLength());
					string += lengthStr;
					CTime tempTime(pRightNodeFile->GetModifiedTime());
					timeStr = tempTime.Format("%c");
					string += timeStr;
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				{
					string += "Both Equal\t";
					CString lengthStr;
					TreeNodeFile *pLeftNodeFile = pSyncNodeFile->GetLeftFileNode();
					TreeNodeFile *pRightNodeFile = pSyncNodeFile->GetRightFileNode();
					ASSERT(pLeftNodeFile); // better be a file node
					ASSERT(pRightNodeFile); // better be a file node

					lengthStr.Format("%u bytes\t",pLeftNodeFile->GetFileLength());
					string += lengthStr;
					CTime tempLeftTime(pLeftNodeFile->GetModifiedTime());
					CString timeStr = tempLeftTime.Format("%c");
					string += timeStr;
					string += "\t";
					lengthStr.Format("%u bytes\t",pRightNodeFile->GetFileLength());
					string += lengthStr;
					CTime tempTime(pRightNodeFile->GetModifiedTime());
					timeStr = tempTime.Format("%c");
					string += timeStr;
				}
				break;
		}
		dumper.Dump(string,indent);
		return;
	}


	// check and see if this node is a folder
	TreeNodeFolder *pNodeFolder = dynamic_cast<TreeNodeFolder *>(pNode);
	// if this is not null then it is a folder
	if (pNodeFolder)
	{
		string += "[ ";
		string += pNode->GetFileName();
		string += " ]";
		dumper.Dump(string,indent);
		TreeIterator first(pNodeFolder);
		TreeIterator last(pNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			DoDump((*first),dumper,indent+1);
		}
		return;
	}
	TreeNodeFile *pNodeFile = dynamic_cast<TreeNodeFile *>(pNode);
	if (pNodeFile)
	{
		string += pNode->GetFileName();
		string += "\t";
		CString lengthStr;
		TreeNodeFile *pNodeFile = dynamic_cast<TreeNodeFile *>(pNode);
		ASSERT(pNodeFile); // better be a file node
		lengthStr.Format("%u bytes",pNodeFile->GetFileLength());
		string += lengthStr;
		string += "\t";
		CTime tempTime(pNodeFile->GetModifiedTime());
		CString timeStr = tempTime.Format("%c");
		string += timeStr;
		dumper.Dump(string,indent);
		return;
	}
	string += "Unknown Node Type ";
	string += pNode->GetFileName();
	dumper.Dump(string,indent);
}


