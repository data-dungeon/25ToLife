/**********************************************************************
 *<
	FILE: Referee.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __REFEREE__H
#define __REFEREE__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "utilapi.h"
#include "notify.h"
#include "tupperware/keeper.h"
#include "registry.h"
#include <vector>
using std::vector;

void NotifySelSetChanged(void* param, NotifyInfo* info);

#include "RefereeRestoreObj.h"


#define REFEREE_CLASS_ID	Class_ID(0x25604d1b, 0x65d4a68d)

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

enum
{
  REFID_NODE,
  REFID_OBJ
};

enum
{
  SPLIT_NODRAG,
  SPLIT_VDRAG,
  SPLIT_HDRAG
};

enum
{
  DRAG_NONE,
  DRAG_TREE,
  DRAG_LIST
};

struct TreeItem;

struct NodeInfo
{
  int id;
  INode* pNode;
  TreeItem* pParent;
  vector<Object*> objectList;

  NodeInfo() : id(REFID_NODE), pNode(NULL), pParent(NULL) {}
};


struct TreeItem
{
  int id;
  Object* pObject;
  TreeItem* pParent;
  Tab<TreeItem*> childList;
  Tab<NodeInfo*> nodeList;
  HTREEITEM hItem;

  TreeItem() : id(REFID_OBJ), pObject(NULL), pParent(NULL) {}
};


class Referee : public UtilityObj
{
public:
	HWND			m_hPanel;
	IUtil			*m_pUtil;
	Interface		*m_pInterface;
	ICustButton		*m_linkedPickedObjectButton;
	ICustButton		*m_linkedPickedBaseButton;
	
  // relationship window
  BOOL m_isDialogUp;
  BOOL m_changedSelSet;
  Tab<NodeInfo*> m_nodeDatabase;
  NodeInfo* m_rightListNodeArray;
  TreeItem* m_pBase;
  HIMAGELIST m_hIcons;

  HWND m_hTree;
  HWND m_hBottomList;
  HWND m_hRightList;

  // dragging
  int m_dragState;
  BOOL m_isTreeDragging;
  BOOL m_isListDragging;
  TreeItem* m_treeDragSource;
  vector<NodeInfo*> m_listDragSources;
  int m_lastTarget;
  TreeItem* m_listDropTarget;
  HIMAGELIST m_dragImage;
  HCURSOR m_defaultCursor;

  // splitter
  int m_splitState;
  int m_oldPos;
  float m_hSplitterRatio;
  float m_vSplitterRatio;

	void BeginEditParams(Interface *ip,IUtil *iu);
	void EndEditParams(Interface *ip,IUtil *iu);

	void Init(HWND hWnd);
	void Destroy(HWND hWnd);
	void ConvertSelectedToXrefObject(HWND hWnd);
	bool DoOpenSaveDialog(TSTR &fileName, bool bOpen=false);
	void NodeToXref(INode * pNode, TSTR &filename, bool bProxy, RefereeRestoreObj* pRestoreObj, bool bIgnoreAnim=false);
	void SelectNodesSameObject(HWND hWnd,bool findBase);
	void AddReferenceNodes(INode *parentNode,INodeTab &nodePtrArray,const Tab<Object *> &objectPtrArray,const INodeTab &selectedNodeArray,bool findBase);
	void CollapseToMesh(void);
	void AddLevel(void);
	void CollapseOneLevel(void);
	void CollapseToBase(void);
	void LinkToPickedObject(INode *pickedNode);
	void LinkToPickedBase(INode *pickedNode);

  // Relationship window
  HWND LaunchRelationshipWindow();
  void InitializeRelationshipWindow(HWND hWnd);
  void SelectionSetChanged();
  void PopulateTreeview();
  void BuildNodeDatabase(INode* pNode);
  void ClearNodeDatabase();
  void BuildObjectList(Object* pObject, vector<Object*> &objectStack);
  void BuildTree();
  void DisplayTreeview(TreeItem* pRoot, HTREEITEM hParent);
  void DeleteTree(TreeItem*& pRoot);
  void DisplayRightList(TreeItem* pSelectedItem, NodeInfo* pSelectedNode = NULL);
  void DisplayBottomList();
  TCHAR* GetObjectName(Object* pObject);
  void InitializeImageList();
  int GetIconType(Object* pObject);
  void SaveWindowPositions(HWND hWnd);
  void RestoreWindowPositions(HWND hWnd);
  void ResizeWindows(int width, int height);
  void DestroyRelationshipWindow(HWND hWnd);

  // doubleclick
  void SelectObjectNodes(TreeItem* pTreeItem);
  void GetChildNodes(TreeItem* pTreeItem, INodeTab& nodeList);
  void SelectNodeFromList();
  void ProcessBottomListDblclk();

  // drag & drop and splitter
  HIMAGELIST CreateMultipleDragImage(HWND hWnd);
  void OnBeginDrag(HWND hWnd, LPARAM lParam);
  void OnLButtonDown(HWND hWnd, POINT cursorScreenPos);
  void OnMouseMove(HWND hWnd, POINT cursorScreenPos);
  void OnLButtonUp(HWND hWnd, POINT cursorScreenPos);

  void ModifyObjectRelationship(TreeItem* pSource, TreeItem* pTarget, RefereeRestoreObj* pRestoreObj);
  void ModifyNodeRelationship(NodeInfo* pSource, TreeItem* pTarget, RefereeRestoreObj* pRestoreObj);

	void DeleteThis() { }		
	//Constructor/Destructor

	Referee();
	~Referee();		

};

class RefereeClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE);
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return REFEREE_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("Referee"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

extern Referee theReferee;
extern RefereeClassDesc RefereeDesc;


#endif // __REFEREE__H
