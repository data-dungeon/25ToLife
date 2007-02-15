//-----------------------------------------------------------------------------
//	File:		TreeNodeBase.h
//	Created:	Sep 26, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TreeNodeBase class acts as a base class for the folder tree
//			hierarchy.
//
//	History:	9/26/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: TreeNodeBase.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * CRC checks
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 11/05/02   Time: 10:02a
 * Updated in $/Avalanche/tools/SyncUtils
 * attributes added
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 10/11/02   Time: 4:38p
 * Updated in $/Avalanche/tools/SyncUtils
 * first draft of folder and file copies
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
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 9:03a
 * Updated in $/Avalanche/tools/SyncUtils
 * added treenode code
 * added tupperware & keeper to project and workspace
 */

#ifndef SYNC_UTILS_DLL_TREE_NODE_BASE_H
#define SYNC_UTILS_DLL_TREE_NODE_BASE_H

//-----------------------------------------------------------------------------
// CTreeBase class declaration:

typedef void* TREE_NODE_HANDLE;

class TreeIterator;
class TreeNodeFolder;
class TreeFactory;

class AFX_EXT_CLASS TreeNodeBase  
{
public:
	enum Attributes
	{
		ATTRIBUTE_NORMAL		= 1<<0,
		ATTRIBUTE_READ_ONLY		= 1<<1,
		ATTRIBUTE_HIDDEN		= 1<<2,
		ATTRIBUTE_SYSTEM		= 1<<3,
	};

private:
	friend class TreeNodeFolder;
	friend class TreeIterator;

	TreeNodeFolder* m_pParent;
	TreeNodeBase* m_pNext;
	TreeNodeBase* m_pPrev;

	TupString m_fileName;
	FILETIME m_modifiedTime;
	unsigned long m_attributes;
	mutable DWORD m_CRCValue;
	mutable bool m_bValidCRC;

public:
	// Constructors and destructors:
	TreeNodeBase(void);
	TreeNodeBase(const TreeNodeBase &s);
	virtual ~TreeNodeBase();

	// Public methods:
	TreeNodeBase* GetParent(void) const;
	TreeNodeBase* GetNext(void) const;
	TreeNodeBase* GetPrev(void) const;

	const char *GetFileName(void) const;
	void SetFileName(const char *pNewName);
	const FILETIME GetModifiedTime(void) const;
	void SetModifiedTime(const FILETIME &newModifiedTime);

	void SetAttributes(unsigned long attributes);
	unsigned long GetAttributes(void) const;

	DWORD GetCRC(void) const;
	bool ValidCRC(void) const;

	void SetCRC(DWORD newCRC) const;
	void SetValidCRC(bool bValidCRC) const;

	void Detatch(void);

	virtual TreeNodeBase *Duplicate(const TreeFactory &treeFactory) const = 0;
	virtual TreeNodeBase *Duplicate(void) const = 0;
	virtual TreeNodeBase *Create(const TreeFactory &treeFactory) const = 0;
	virtual TreeNodeBase *Create(void) const = 0;

	void CopyInfo(const TreeNodeBase &s);

	void GetPath(TupString &filePath) const;


};

//-----------------------------------------------------------------------------

#endif

