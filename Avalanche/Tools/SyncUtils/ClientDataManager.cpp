////////////////////////////////////////////////////////////////////////////
//
// ClientDataManager
//
// Class used for creating and maintaining the sync tree
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ClientDataManager.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/SyncUtils
 * auto freeze list
 * sync styles
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/20/03    Time: 2:48p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data sync bugs
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/16/03    Time: 4:29p
 * Updated in $/Avalanche/tools/SyncUtils
 * More work on Phase 2 - Getting the folder sets dialog working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/10/03    Time: 1:05p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data manager foundataion
 * exclude / frozen lock stuff
 */

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "ClientDataManager.h"
#include "TreeNodeFolder.h"
#include "TreeNodeFile.h"
#include "TreeIterator.h"
#include "SyncTreeNodeFolder.h"
#include "SyncTreeNodeFile.h"
#include "SyncTreeNodeFlags.h"
#include "ClientTreeNodeFolder.h"
#include "ClientTreeNodeFile.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

ClientTreeNodeFolder *m_pClientTreeRoot;
TupString m_leftFolderPath;
TupString m_rightFolderPath;

ClientDataManager::ClientDataManager(void) : m_pClientTreeRoot(NULL)
{
}

ClientDataManager::~ClientDataManager()
{
	delete m_pClientTreeRoot;
}

ClientTreeNodeFolder *ClientDataManager::GetTreeRoot(void)
{
	return m_pClientTreeRoot;
}

const char *ClientDataManager::GetLeftFolderPath(void) const
{
	return m_leftFolderPath;
}

const char *ClientDataManager::GetRightFolderPath(void) const
{
	return m_rightFolderPath;
}

// this method will create a client tree node and its children from tupperware data
// the format of the tupperware data is as follows:
//
//  LeftFolderPath = "pathstring" (string scalar)
//  RightFolderPath = "pathstring" (string scalar)
//  AutoFrozenPaths = [ "pathstring" , "pathstring" ] (string list)
//  FileTree = (aggregate)
//  {
//		File "name" = (aggregate)
//		{
//			Flags = 12345 (int scalar)
//		}
//		Folder "name" (aggregate)
//		{
//			Flags = 12345 (int scalar)
//			Children = (aggregate)
//			{
//				File "name" ...
//				Folder "name" ...
//			}
//		}
//  }
//
void ClientDataManager::LoadData(const char *pFilename)
{
	Clear();
	TupperwareAggregate *pTupRoot;
	int result = false;
	try
	{
		result = TupperwareLoadSaveManager::Read(pFilename,&pTupRoot);
	}
	catch(...)
	{
	}
	if (result)
	{
		assert(pTupRoot);
		TupperwareScalar *pLeftFolderPathScalar = pTupRoot->FindScalarByKey("LeftFolderPath");
		if (pLeftFolderPathScalar && pLeftFolderPathScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			m_leftFolderPath = pLeftFolderPathScalar->GetAsString();
		}
		TupperwareScalar *pRightFolderPathScalar = pTupRoot->FindScalarByKey("RightFolderPath");
		if (pRightFolderPathScalar && pRightFolderPathScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			m_rightFolderPath = pRightFolderPathScalar->GetAsString();
		}
		TupperwareList *pAutoFrozenPathsList = pTupRoot->FindListByKey("AutoFrozenPaths");
		if (pAutoFrozenPathsList && pAutoFrozenPathsList->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			pAutoFrozenPathsList->GetAsString(m_autoFrozenPaths);
		}
		TupperwareAggregate *pFileTreeAgg = pTupRoot->FindAggregateByKey("FileTree");
		if (pFileTreeAgg)
		{
			m_pClientTreeRoot = new ClientTreeNodeFolder;
			m_pClientTreeRoot->SetFileName("Root");
			for (TupperwareIterator i = pFileTreeAgg->FindAll(); !i.Finished(); i.Next())
			{
				TupperwareAggregate *pChildAggregate = i.GetAsAggregate();
				TreeNodeBase *pChild = BuildClientTreeNode(pChildAggregate);
				if (pChild)
				{
					m_pClientTreeRoot->AddChild(pChild);
				}
			}
		}
		delete pTupRoot;
	}
}

// this method will build the client tree nodes from the sync tree nodes
void ClientDataManager::BuildTree(SyncTreeNodeFolder *pSyncTreeNodeRoot)
{
	delete m_pClientTreeRoot;
	m_pClientTreeRoot = NULL;
	if (pSyncTreeNodeRoot)
	{
		m_pClientTreeRoot = new ClientTreeNodeFolder;
		m_pClientTreeRoot->SetFileName("Root");

		TreeNodeBase *pChildNode = pSyncTreeNodeRoot->GetFirstChildNode();
		while(pChildNode)
		{
			m_pClientTreeRoot->AddChild(BuildClientTreeNode(pChildNode));
			pChildNode = pChildNode->GetNext();
		}
	}
}

// this method will take the data contained within the client nodes and update
// the nodes within the sync tree
void ClientDataManager::UpdateSyncTree(SyncTreeNodeFolder *pSyncTreeNodeRoot)
{
	if (m_pClientTreeRoot && pSyncTreeNodeRoot)
	{
		UpdateSyncNodeChildren(m_pClientTreeRoot,pSyncTreeNodeRoot);
	}
}

// this method will construct the tupperware data from the client tree nodes
// and save it to disk
bool ClientDataManager::SaveData(const char *pFilename)
{
	if (pFilename && m_pClientTreeRoot)
	{
		TupperwareAggregate *pTupRoot = new TupperwareAggregate;

		pTupRoot->AddScalarString("LeftFolderPath",NULL,m_leftFolderPath);
		pTupRoot->AddScalarString("RightFolderPath",NULL,m_rightFolderPath);
		pTupRoot->AddListString("AutoFrozenPaths",NULL,m_autoFrozenPaths);
		
		TupperwareAggregate *pFileTreeAgg = pTupRoot->AddAggregate("FileTree");
		TreeNodeBase *pChildNode = m_pClientTreeRoot->GetFirstChildNode();
		while(pChildNode)
		{
			BuildTreeNodeAgg(pFileTreeAgg,pChildNode);
			pChildNode = pChildNode->GetNext();
		}
		int result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,pFilename,pTupRoot);
		delete pTupRoot;
		return result!=0;
	}
	return false;
}

void ClientDataManager::SetLeftFolderPath(const char *pPath)
{
	m_leftFolderPath = pPath;
}

void ClientDataManager::SetRightFolderPath(const char *pPath)
{
	m_rightFolderPath = pPath;
}

void ClientDataManager::Clear(void)
{
	delete m_pClientTreeRoot;
	m_pClientTreeRoot = NULL;
	m_leftFolderPath.Empty();
	m_rightFolderPath.Empty();
	m_autoFrozenPaths.RemoveAll();
}

// this method will create the client tree nodes from the sync tree nodes
TreeNodeBase *ClientDataManager::BuildClientTreeNode(TreeNodeBase *pNode)
{
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this child is a folder
	if (pSyncNodeFolder)
	{
		ClientTreeNodeFolder *pClientTreeNodeFolder = new ClientTreeNodeFolder;
		pClientTreeNodeFolder->SetFileName(pSyncNodeFolder->GetFileName());
		pClientTreeNodeFolder->SetFlags(pSyncNodeFolder->GetFlags());
		TreeNodeBase *pChildNode = pSyncNodeFolder->GetFirstChildNode();
		while(pChildNode)
		{
			pClientTreeNodeFolder->AddChild(BuildClientTreeNode(pChildNode));
			pChildNode = pChildNode->GetNext();
		}
		return pClientTreeNodeFolder;
	}
	
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this child is a folder
	if (pSyncNodeFile)
	{
		ClientTreeNodeFile *pClientTreeNodeFile = new ClientTreeNodeFile;
		pClientTreeNodeFile->SetFileName(pSyncNodeFile->GetFileName());
		pClientTreeNodeFile->SetFlags(pSyncNodeFile->GetFlags());
		pClientTreeNodeFile->SetLeftSyncDate(pSyncNodeFile->GetLeftSyncDate());
		pClientTreeNodeFile->SetRightSyncDate(pSyncNodeFile->GetRightSyncDate());
		return pClientTreeNodeFile;
	}
	return NULL;	
}

// this method will create a client tree node and its children from tupperware data
TreeNodeBase *ClientDataManager::BuildClientTreeNode(TupperwareAggregate *pAggregate)
{
	const char *pKey = pAggregate->GetKey();
	// if this is a folder
	if (!strcmp(pKey,"Folder"))
	{
		ClientTreeNodeFolder *pFolder = new ClientTreeNodeFolder;
		pFolder->SetFileName(pAggregate->GetName());
		TupperwareScalar *pFlagsScalar = pAggregate->FindScalarByKey("Flags");
		if (pFlagsScalar && pFlagsScalar->GetDataType() == TupperwareScalar::DATA_INT)
		{
			pFolder->SetFlags(pFlagsScalar->GetAsInt());
		}
		TupperwareAggregate *pChildrenAgg = pAggregate->FindAggregateByKey("Children");
		if (pChildrenAgg)
		{
			for (TupperwareIterator i = pChildrenAgg->FindAll(); !i.Finished(); i.Next())
			{
				TupperwareAggregate *pChildAgg = i.GetAsAggregate();
				TreeNodeBase *pChild = BuildClientTreeNode(pChildAgg);
				if (pChild)
				{
					pFolder->AddChild(pChild);
				}
			}
		}
		return pFolder;
	}
	else if (!strcmp(pKey,"File"))
	{
		ClientTreeNodeFile *pFile = new ClientTreeNodeFile;
		pFile->SetFileName(pAggregate->GetName());
		TupperwareScalar *pFlagsScalar = pAggregate->FindScalarByKey("Flags");
		if (pFlagsScalar && pFlagsScalar->GetDataType() == TupperwareScalar::DATA_INT)
		{
			int flags = pFlagsScalar->GetAsInt();
			pFile->SetFlags(flags);

			if (flags & SyncTreeNodeFlags::FLAGS_HAS_SYNC_DATE)
			{
				TupperwareList *pDatesList = pAggregate->FindListByKey("SyncDates");
				ASSERT(pDatesList);
				ASSERT(pDatesList->GetDataType() == TupperwareList::DATA_INT);
				ASSERT(pDatesList->GetLength()==4);
				FILETIME *pDates = (FILETIME *)pDatesList->GetAsInt();
				pFile->SetLeftSyncDate(pDates[0]);
				pFile->SetRightSyncDate(pDates[1]);
			}
		}
		return pFile;
	}
	return NULL;
}

// this method will construct the tupperware data from the client tree nodes
void ClientDataManager::BuildTreeNodeAgg(TupperwareAggregate *pParentAgg,TreeNodeBase *pNode)
{
	ClientTreeNodeFolder *pClientTreeFolder = dynamic_cast<ClientTreeNodeFolder *>(pNode);
	// if this child is a folder
	if (pClientTreeFolder)
	{
		TupperwareAggregate *pFolderAgg = pParentAgg->AddAggregate("Folder",pClientTreeFolder->GetFileName());
		pFolderAgg->AddScalarInt("Flags",NULL,pClientTreeFolder->GetFlags());

		TreeNodeBase *pChildNode = pClientTreeFolder->GetFirstChildNode();
		TupperwareAggregate *pChildrenAgg = NULL;
		if (pChildNode)
		{
			pChildrenAgg = pFolderAgg->AddAggregate("Children");
		}
		while(pChildNode)
		{
			BuildTreeNodeAgg(pChildrenAgg,pChildNode);
			pChildNode = pChildNode->GetNext();
		}
		return;
	}
	
	ClientTreeNodeFile *pClientTreeFile = dynamic_cast<ClientTreeNodeFile *>(pNode);
	// if this child is a folder
	if (pClientTreeFile)
	{
		TupperwareAggregate *pFileAgg = pParentAgg->AddAggregate("File",pClientTreeFile->GetFileName());
		int flags = pClientTreeFile->GetFlags();
		pFileAgg->AddScalarInt("Flags",NULL,flags);
		// if sync date is there then save out
		if (flags & SyncTreeNodeFlags::FLAGS_HAS_SYNC_DATE)
		{
			FILETIME dates[2];
			dates[0] = pClientTreeFile->GetLeftSyncDate();
			dates[1] = pClientTreeFile->GetRightSyncDate();
			pFileAgg->AddListInt("SyncDates",NULL,(int *)dates,4);
		}
		return;
	}		
}

void ClientDataManager::UpdateSyncNodeChildren(ClientTreeNodeFolder *pClientParentNode,SyncTreeNodeFolder *pSyncParentNode)
{
	TreeNodeBase *pClientNode = pClientParentNode->GetFirstChildNode();
	TreeNodeBase *pSyncNode = pSyncParentNode->GetFirstChildNode();
	while(pClientNode && pSyncNode)
	{
		int stringCompare = CompareNodes(pClientNode,pSyncNode);
		// if they are equal
		if (stringCompare==0)
		{
			ClientTreeNodeFolder *pClientFolder = dynamic_cast<ClientTreeNodeFolder *>(pClientNode);
			SyncTreeNodeFolder *pSyncFolder = dynamic_cast<SyncTreeNodeFolder *>(pSyncNode);
			if (pClientFolder && pSyncFolder)
			{
				int clientFlags = pClientFolder->GetFlags();
				int syncFlags = pSyncFolder->GetFlags();
				int newSyncFlags = syncFlags | (clientFlags & SyncTreeNodeFlags::CLIENT_RESTORE_FLAGS); // or in
				pSyncFolder->SetFlags(newSyncFlags);
				UpdateSyncNodeChildren(pClientFolder,pSyncFolder); // keep doing our children together
			}

			ClientTreeNodeFile *pClientFile = dynamic_cast<ClientTreeNodeFile *>(pClientNode);
			SyncTreeNodeFile *pSyncFile = dynamic_cast<SyncTreeNodeFile *>(pSyncNode);
			if (pClientFile && pSyncFile)
			{
				int clientFlags = pClientFile->GetFlags();
				int syncFlags = pSyncFile->GetFlags();
				int newSyncFlags = syncFlags | (clientFlags & SyncTreeNodeFlags::CLIENT_RESTORE_FLAGS); // or in
				pSyncFile->SetFlags(newSyncFlags);
				pSyncFile->SetLeftSyncDate(pClientFile->GetLeftSyncDate());
				pSyncFile->SetRightSyncDate(pClientFile->GetRightSyncDate());
			}
			pClientNode=pClientNode->GetNext();
			pSyncNode=pSyncNode->GetNext();
		}
		// if client < sync
		else if (stringCompare<0)
		{
			pClientNode=pClientNode->GetNext();
		}
		// if client > sync
		else
		{
			pSyncNode=pSyncNode->GetNext();
		}
	}
}

// folders are less than files
// returns -1 if node1<node2
// returns 0 if node1==node2 (and same type)
// return 1 if node1>node2
int ClientDataManager::CompareNodes(TreeNodeBase *pNode1,TreeNodeBase *pNode2)
{
	TreeNodeFolder *pNodeFolder1 = dynamic_cast<TreeNodeFolder *>(pNode1);
	TreeNodeFolder *pNodeFolder2 = dynamic_cast<TreeNodeFolder *>(pNode2);
	
	// if both files or folders
	if ((pNodeFolder1 && pNodeFolder2)||(!pNodeFolder1 && !pNodeFolder2))
	{
		return stricmp(pNode1->GetFileName(),pNode2->GetFileName());
	}
	// if node1 is a folder, node2 is a file
	else if (pNodeFolder1)
	{
		return -1;
	}
	// if node2 is a folder, node1 is a file
	else 
	{
		return 1;
	}
}

int ClientDataManager::GetNumAutoFrozenPaths(void) const
{
	return m_autoFrozenPaths.GetSize();
}

void ClientDataManager::GetAutoFrozenPath(TupString &path,int index)
{
	path = m_autoFrozenPaths[index];
}

void ClientDataManager::AddAutoFrozenPath(const char *path)
{
	TupString newPath(path);
	m_autoFrozenPaths.Add(newPath);
}

