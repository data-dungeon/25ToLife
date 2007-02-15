////////////////////////////////////////////////////////////////////////////
//
// ClientDataManger
//
// Class used for keeping track of the client data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ClientDataManager.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/SyncUtils
 * auto freeze list
 * sync styles
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/10/03    Time: 1:05p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data manager foundataion
 * exclude / frozen lock stuff
 */

#ifndef SYNC_UTILS_DLL_CLIENT_DATA_MANAGER_H
#define SYNC_UTILS_DLL_CLIENT_DATA_MANAGER_H

class ClientTreeNodeFolder;
class TreeNodeBase;
class SyncTreeNodeFolder;

class AFX_EXT_CLASS ClientDataManager
{
	ClientTreeNodeFolder *m_pClientTreeRoot;
	TupString m_leftFolderPath;
	TupString m_rightFolderPath;
	TupArray<TupString> m_autoFrozenPaths;

public:
	ClientDataManager(void);
	virtual ~ClientDataManager();

	ClientTreeNodeFolder *GetTreeRoot(void);
	const char *GetLeftFolderPath(void) const;
	const char *GetRightFolderPath(void) const;
	int GetNumAutoFrozenPaths(void) const;
	void GetAutoFrozenPath(TupString &path,int index);

	void LoadData(const char *pFilename);

	void BuildTree(SyncTreeNodeFolder *pSyncTreeNodeRoot);
	void UpdateSyncTree(SyncTreeNodeFolder *pSyncTreeNodeRoot);
	bool SaveData(const char *pFilename);
	void SetLeftFolderPath(const char *pPath);
	void SetRightFolderPath(const char *pPath);
	void AddAutoFrozenPath(const char *path);
	
	void Clear(void);

private:
	TreeNodeBase *BuildClientTreeNode(TreeNodeBase *pNode);
	TreeNodeBase *BuildClientTreeNode(TupperwareAggregate *pAggregate);
	void BuildTreeNodeAgg(TupperwareAggregate *pParentAgg,TreeNodeBase *pNode);
	void UpdateSyncNodeChildren(ClientTreeNodeFolder *pClientParentNode,SyncTreeNodeFolder *pSyncParentNode);
	int CompareNodes(TreeNodeBase *pNode1,TreeNodeBase *pNode2);
		
};

//-----------------------------------------------------------------------------

#endif






















