#include "Referee.h"
#include "modstack.h"


// icon identifiers
enum
{
  IMAGE_BASEOBJ,
  IMAGE_MODIFIER,
  IMAGE_XREFOBJ,
  IMAGE_NODE,
  IMAGE_REFOBJ
};


#define SPLITTER_WIDTH 6


/****************************** Helper functions ******************************/
 
/*******************************************************************************
 NotifySelSetChanged()

 Called whenever the selection set is changed.
*******************************************************************************/
void NotifySelSetChanged(void* param, NotifyInfo* info)
{
  theReferee.SelectionSetChanged();
} // end NotifySelSetChanged()


/*******************************************************************************
 FindTreeItemByObject()

 Helper function to recursively determine whether or not an object already
 exists in the tree. If the object is found, a pointer to its TreeItem is
 returned. Returns NULL otherwise.
*******************************************************************************/
TreeItem* FindTreeItemByObject(TreeItem* pBase, Object* pObject)
{
  if (pBase->pObject == pObject)
    return pBase;

  for (int i = 0; i < pBase->childList.Count(); ++i)
  {
    TreeItem* pResult = FindTreeItemByObject(pBase->childList[i], pObject);

    if (pResult)
      return pResult;
  }
  return NULL;
} // end FindTreeItemByObject()


/*******************************************************************************
 GetTreeItemFromHandle()

 Returns a pointer to a tree item, given a handle in the treeview
*******************************************************************************/
TreeItem* GetTreeItemFromHandle(HTREEITEM hti)
{
  TVITEM tvi;
  tvi.mask = TVIF_PARAM;
  tvi.hItem = hti;
  TreeView_GetItem(theReferee.m_hTree, &tvi);

  return (TreeItem*) tvi.lParam;
} // end GetTreeItemFromHandle()


/*******************************************************************************
 IsDerivedObject()

 Determines whether or not the object is a derived object.
*******************************************************************************/
BOOL IsDerivedObject(Object* pObject)
{
  if (pObject && pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    return TRUE;
  else
    return FALSE;
} // end IsDerivedObject()


/*******************************************************************************
 IsModifier()

 Determines whether or not the object is a derived object.
*******************************************************************************/
BOOL IsModifier(IDerivedObject* pObject)
{
  if (pObject->NumModifiers())
    return TRUE;
  else
    return FALSE;
} // end IsModifier()


/*******************************************************************************
 IsLegalMove()

 Determines whether or not dragging a source object onto the target object is
 allowed.
*******************************************************************************/
BOOL IsLegalMove(TreeItem* pTarget, TreeItem* pSource)
{
  // don't allow to drag to the same place
  if (pSource->pParent == pTarget)
    return FALSE;

  // determine if we're dragging an object onto itself, or onto one of its children
  while (pTarget)
  {
    if (pTarget->pObject == pSource->pObject)
      return FALSE;
    pTarget = pTarget->pParent;
  }
  return TRUE;
} // end IsLegalMove()


/*******************************************************************************
 IsLegalMove()

 Determines whether or not dragging a source node onto the target object is
 allowed.
*******************************************************************************/
BOOL IsLegalMove(TreeItem* pTarget, vector<NodeInfo*> sourceArray)
{
  // every node/object has to be legal for the move to be legal
  for (int i = 0; i < sourceArray.size(); ++i)
  {
    // potentially illegal if the source "node" is actually an object
    if (sourceArray[i]->id == REFID_OBJ)
    {
      if (!IsLegalMove(pTarget, (TreeItem*) sourceArray[i]))
        return FALSE;
    }
    // don't allow to drag a node to the original parent
    else if (sourceArray[i]->pParent == pTarget)
    {
      return FALSE;
    }
  }

  return TRUE;
} // end IsLegalMove()


/*******************************************************************************
 IsValidTarget()

 Determines whether or not the current list target is a valid object to drag
 the source node to.
*******************************************************************************/
BOOL IsValidTarget(HWND hList, int target, vector<NodeInfo*> sourceArray)
{
  LVITEM lvi;
  lvi.iItem = target;
  lvi.mask = LVIF_PARAM;
  ListView_GetItem(hList, &lvi);

  NodeInfo* pTarget = (NodeInfo*) lvi.lParam;

  // dragging onto an object
  if (pTarget->id == REFID_OBJ)
  {
    return IsLegalMove((TreeItem*) pTarget, sourceArray);
  }
  // dragging onto a node: never legal
  else
  {
    return FALSE;
  }
} // end IsValidTarget


/*******************************************************************************
 RemoveChildObject()

 Removes an object from its parent's child list.
*******************************************************************************/
void RemoveChildObject(TreeItem* pParent, TreeItem* pChild)
{
  if (!pParent || !pChild)
    return;

  // find the matching object
  for (int i = 0; i < pParent->childList.Count(); ++i)
  {
    if (pParent->childList[i] == pChild)
    {
      pParent->childList.Delete(i, 1);
      break;
    }
  }

  // if this object no longer has any child objects or nodes, we can remove it
  if (pParent->childList.Count() == 0 && pParent->nodeList.Count() == 0)
  {
    TreeView_DeleteItem(theReferee.m_hTree, pParent->hItem);
    RemoveChildObject(pParent->pParent, pParent);
    delete pParent;
  }
} // end RemoveChildObject()


/*******************************************************************************
 RemoveChildNode()

 Removes an object from its parent's node list.
*******************************************************************************/
void RemoveChildNode(TreeItem* pParent, NodeInfo* pChild)
{
  if (!pParent || !pChild)
    return;

  // find the matching node
  for (int i = 0; i < pParent->nodeList.Count(); ++i)
  {
    if (pParent->nodeList[i] == pChild)
    {
      pParent->nodeList.Delete(i, 1);
      break;
    }
  }

  // if this object no longer has any child objects or nodes, we can remove it
  if (pParent->childList.Count() == 0 && pParent->nodeList.Count() == 0)
  {
    TreeView_DeleteItem(theReferee.m_hTree, pParent->hItem);
    RemoveChildObject(pParent->pParent, pParent);
    delete pParent;
  }
}


/*******************************************************************************
 GetScreenCoords()

 Convenience function to get the screen coordinates from hWnd's client coords.
*******************************************************************************/
POINT GetScreenCoords(HWND hWnd, LPARAM lParam)
{
  static POINT pt;
  pt.x = GET_X_LPARAM(lParam);
  pt.y = GET_Y_LPARAM(lParam);
  ClientToScreen(hWnd, &pt);

  return pt;
} // end GetScreenCoords()


/*******************************************************************************
 DrawXorBar()

 Draws an XORed bar, for when the splitter window is dragging.
*******************************************************************************/
void DrawXorBar(HDC hdc, int xOrigin, int yOrigin, int width, int height)
{
	static WORD dotPatternBmp[8] = 
	{ 
		0x00aa, 0x0055, 0x00aa, 0x0055, 
		0x00aa, 0x0055, 0x00aa, 0x0055
	};

	HBITMAP hbm;
	HBRUSH  hbr, hbrushOld;

	hbm = CreateBitmap(8, 8, 1, 1, dotPatternBmp);
	hbr = CreatePatternBrush(hbm);
	
	SetBrushOrgEx(hdc, xOrigin, yOrigin, NULL);

	hbrushOld = (HBRUSH)SelectObject(hdc, hbr);
	
	PatBlt(hdc, xOrigin, yOrigin, width, height, PATINVERT);
	
	SelectObject(hdc, hbrushOld);
	
	DeleteObject(hbr);
	DeleteObject(hbm);
} // end DrawXorBar()


/****************************** Window callback *******************************/


static BOOL CALLBACK RelWindowDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
      {
        theReferee.InitializeRelationshipWindow(hWnd);
//        HICON hIcon = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_REFICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED | LR_VGACOLOR);
//        HICON hIconSm = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_REFICONSM), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED | LR_VGACOLOR);
//        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
//        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
      } break;

		case WM_CLOSE:
      {
        DestroyWindow(hWnd);
      } break;

    case WM_DESTROY:
      {
        theReferee.DestroyRelationshipWindow(hWnd);
      } break;

    case WM_SIZE:
      {
        theReferee.ResizeWindows(LOWORD(lParam), HIWORD(lParam));
      } break;

    case WM_NOTIFY:
      {
        LPNMHDR pnmh = (LPNMHDR) lParam;
        switch (pnmh->code)
        {
        case TVN_SELCHANGED:
          {
            // update the right list if the selected object in the treeview changes
            LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;

            // get the pointer to the currently selected object
            TreeItem* pSelectedItem = (TreeItem*) pnmtv->itemNew.lParam;
            theReferee.DisplayRightList(pSelectedItem);
          } break;

        case NM_DBLCLK:
          {
            // process doubleclick depending on the panel it happened in
            switch (pnmh->idFrom)
            {
            case IDC_RIGHT_LIST:
              theReferee.SelectNodeFromList();
              break;

            case IDC_BOTTOM_LIST:
              theReferee.ProcessBottomListDblclk();
              break;

            case IDC_TREE:
              theReferee.SelectObjectNodes(GetTreeItemFromHandle(TreeView_GetSelection(theReferee.m_hTree)));
              break;
            }
          } break;

        case TVN_BEGINDRAG:
          {
            theReferee.OnBeginDrag(theReferee.m_hTree, lParam);
          } break;

        case LVN_BEGINDRAG:
          {
            LPNMLISTVIEW pnmlv = (LPNMLISTVIEW) lParam;
            theReferee.OnBeginDrag(pnmlv->hdr.hwndFrom, lParam);
          } break;
        }
      } break;

    case WM_LBUTTONDOWN:
      {
        theReferee.OnLButtonDown(hWnd, GetScreenCoords(hWnd, lParam));
      } break;

    case WM_MOUSEMOVE:
      {
        theReferee.OnMouseMove(hWnd, GetScreenCoords(hWnd, lParam));
      } break;

    case WM_LBUTTONUP:
      {
        theReferee.OnLButtonUp(hWnd, GetScreenCoords(hWnd, lParam));
      } break;

		default:
			return FALSE;
	}
	return TRUE;
}


/*******************************************************************************
 Referee::LaunchRelationshipWindow()

 Creates a modeless dialog and displays it.
*******************************************************************************/
HWND Referee::LaunchRelationshipWindow()
{
  HWND hDialog = NULL;

  if (!m_isDialogUp)
  {
    hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_TREEVIEW), NULL, RelWindowDlgProc);
    ShowWindow(hDialog, SW_SHOW);
  }
  return hDialog;
} // end Referee::LaunchRelationshipWindow()


/*******************************************************************************
 Referee::InitializeRelationshipWindow()

 Creates a modeless dialog and displays it.
*******************************************************************************/
void Referee::InitializeRelationshipWindow(HWND hWnd)
{
  // save the handles to the tree and two lists
  m_hTree = GetDlgItem(hWnd, IDC_TREE);
  m_hBottomList = GetDlgItem(hWnd, IDC_BOTTOM_LIST);
  m_hRightList = GetDlgItem(hWnd, IDC_RIGHT_LIST);

  TreeView_SetBkColor(m_hTree, RGB(255, 255, 255));

  // set up a column for the bottom list
  RECT rc;
  GetClientRect(theReferee.m_hBottomList, &rc);
  LVCOLUMN lvc;
  lvc.mask = LVCF_WIDTH;
  lvc.cx = rc.right - rc.left - 20;
  ListView_InsertColumn(theReferee.m_hBottomList, 0, &lvc);

  InitializeImageList();

  PopulateTreeview();
  DisplayBottomList();

  RestoreWindowPositions(hWnd);

  m_isDialogUp = TRUE;
} // end Referee::InitializeRelationshipWindow()


/*******************************************************************************
 Referee::DestroyRelationshipWindow()

 Free all dynamically allocated memory, and cleanup.
*******************************************************************************/
void Referee::DestroyRelationshipWindow(HWND hWnd)
{
  ImageList_Destroy(m_hIcons);
  ClearNodeDatabase();
  DeleteTree(m_pBase);
  SaveWindowPositions(hWnd);
  m_isDialogUp = FALSE;
} // end Referee::DestroyRelationshipWindow()


/*******************************************************************************
 Referee::SelectionSetChanged()

 Called whenever the selection set is changed.
*******************************************************************************/
void Referee::SelectionSetChanged()
{
  // see if the change was caused by the relationship window
  if (m_changedSelSet)
  {
    m_changedSelSet = FALSE;
      return;
  }

  if (m_isDialogUp)
  {
    // save the current selection
    HTREEITEM hti = TreeView_GetSelection(m_hTree);
    Object* pSelObj = NULL;
    TreeItem* pSelItem;

    if (hti && (pSelItem = GetTreeItemFromHandle(hti)))
    {
      pSelObj = pSelItem->pObject;
    }

    PopulateTreeview();
    DisplayBottomList();

    if (pSelItem = FindTreeItemByObject(m_pBase, pSelObj))
    {
      TreeView_SelectItem(m_hTree, pSelItem->hItem);
    }
    else
    {
      TreeView_SelectItem(m_hTree, TreeView_GetFirstVisible(m_hTree));
    }
  }
} // end Referee::SelectionSetChanged()


/*******************************************************************************
 Referee::PopulateTreeview()

 Builds the treeview and displays it.
*******************************************************************************/
void Referee::PopulateTreeview()
{
  // build node database
  ClearNodeDatabase();
  BuildNodeDatabase(m_pInterface->GetRootNode());

  BuildTree();

  DisplayTreeview(m_pBase, TVI_ROOT);
} // end Referee::PopulateTreeview()


/*******************************************************************************
 Referee::BuildNodeDatabase()

 Recursively steps through all the nodes in the world and stores relevant
 information about each.
*******************************************************************************/
void Referee::BuildNodeDatabase(INode* pNode)
{
  if (!pNode)
    return;

  // rather than processing the current node, information about its children is
  // stored. This avoids special case code dealing with the world root.
  for (int i = 0; i < pNode->NumberOfChildren(); ++i)
	{
		INode *pChild = pNode->GetChildNode(i);

		if (!pChild)
			continue;

    NodeInfo* pInfo = new NodeInfo();
    pInfo->pNode = pChild;

    BuildObjectList(pChild->GetObjectRef(), pInfo->objectList);
    m_nodeDatabase.Append(1, &pInfo);

    BuildNodeDatabase(pChild);
	}
} // end Referee::BuildNodeDatabase()


/*******************************************************************************
 Referee::ClearNodeDatabase()

 Removes all entries from the node database, freeing allocated memory.
*******************************************************************************/
void Referee::ClearNodeDatabase()
{
  for (int i = 0; i < m_nodeDatabase.Count(); ++i)
  {
    delete m_nodeDatabase[i];
    m_nodeDatabase[i] = NULL;
  }

  m_nodeDatabase.Shrink();
  m_nodeDatabase.ZeroCount();
} // end Referee::ClearNodeDatabase()


/*******************************************************************************
 Referee::BuildObjectList()

 Recursively builds a list of all objects pointing to a node.
*******************************************************************************/
void Referee::BuildObjectList(Object* pObject, vector<Object*> &objectList)
{
  // Check for NULL
  if (!pObject)
    return;

  // Skip bones
  if (pObject->ClassID() == Class_ID(BONE_CLASS_ID, 0))
    return;		

  // may want to skip other object types here

  objectList.push_back(pObject);

  // if this is a derived object continue down the stack
  if (IsDerivedObject(pObject))
  {
    IDerivedObject* pDerivedObj = (IDerivedObject*)pObject;
    BuildObjectList(pDerivedObj->GetObjRef(), objectList);
  }
} // end Referee::BuildObjectList()


/*******************************************************************************
 Referee::BuildTree()

 Builds a tree of all the objects in the world.
*******************************************************************************/
void Referee::BuildTree()
{
  // clear out anything previously stored
  DeleteTree(m_pBase);

  // set up the base object
  m_pBase = new TreeItem();
  m_pBase->pObject = NULL;
  m_pBase->pParent = NULL;

  // loop through every node
  for (int n = 0; n < m_nodeDatabase.Count(); ++n)
  {
    // start with the parent being the world base
    TreeItem* pParent = m_pBase;

    // loop through the (derived) object chain
    vector<Object*> objectList = m_nodeDatabase[n]->objectList;
    for (int i = objectList.size() - 1; i >= 0; --i)
    {
      // see if the object already exists
      TreeItem* pTreeItem = FindTreeItemByObject(m_pBase, objectList[i]);

      // create a new item only if we haven't seen this object before
      if (!pTreeItem)
      {
        pTreeItem = new TreeItem();
        pTreeItem->pObject = objectList[i];
        pTreeItem->pParent = pParent;

        // add this object to the current parent's child list
        pParent->childList.Append(1, &pTreeItem);
      }
      // make the current object the parent
      pParent = pTreeItem;
    }

    // add the node to the current parent's node list
    m_nodeDatabase[n]->pParent = pParent;
    pParent->nodeList.Append(1, &m_nodeDatabase[n]);
  }
} // end Referee::BuildTree()


/*******************************************************************************
 Referee::DisplayTreeview()

 Recursively populates the treeview control using the internal tree structure
*******************************************************************************/
void Referee::DisplayTreeview(TreeItem* pRoot, HTREEITEM hParent)
{
  if (!pRoot)
    return;

  if (pRoot == m_pBase)
  {
    // clear out anything that may be in the control already
    TreeView_SelectItem(m_hTree, NULL);
    TreeView_DeleteAllItems(m_hTree);
  }

  TVINSERTSTRUCT tvis;
  tvis.hParent = hParent;
  tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

  // Again, the children of the current node are processed to avoid special case
  // code for the root (which is not displayed)
  for (int i = 0; i < pRoot->childList.Count(); ++i)
  {
    tvis.hInsertAfter = TVI_ROOT;
    Object* pObject = pRoot->childList[i]->pObject;

    tvis.item.pszText = GetObjectName(pObject);
    tvis.item.lParam = (LPARAM) pRoot->childList[i];
    tvis.item.iImage = tvis.item.iSelectedImage = GetIconType(pObject);

    HTREEITEM hItem = TreeView_InsertItem(m_hTree, &tvis);
    pRoot->childList[i]->hItem = hItem;

    DisplayTreeview(pRoot->childList[i], hItem);
  }
} // end Referee::DisplayTreeview()


/*******************************************************************************
 Referee::DeleteTree()

 Recursively clears out the internal representation of the tree
*******************************************************************************/
void Referee::DeleteTree(TreeItem*& pRoot)
{
  if (!pRoot)
    return;

  // first remove the children
  for (int i = 0; i < pRoot->childList.Count(); ++i)
  {
    DeleteTree(pRoot->childList[i]);
  }

  // then delete the root
  delete pRoot;
  pRoot = NULL;
} // end Referee::DeleteTree()


/*******************************************************************************
 Referee::DisplayRightList()

 Displays the right list based on the selected item in the treeview. If
 pSelectedNode is non-NULL, then display the node as selected.
*******************************************************************************/
void Referee::DisplayRightList(TreeItem* pSelectedItem, NodeInfo* pSelectedNode)
{
  if (!pSelectedItem)
    return;

  // clear out previous contents
  ListView_DeleteAllItems(m_hRightList);

  int i = 0;
  int index = 0;
  int selectedIndex = 0;

  LVITEM item;
  ZeroMemory(&item, sizeof(LVITEM));
  item.mask = LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;

  // first, display all the child objects
  for (i = 0; i < pSelectedItem->childList.Count(); ++i)
  {
    item.pszText = GetObjectName(pSelectedItem->childList[i]->pObject);
    item.lParam = (LPARAM) pSelectedItem->childList[i];
    item.iItem = index++;
    item.iImage = GetIconType(pSelectedItem->childList[i]->pObject);

    ListView_InsertItem(m_hRightList, &item);
  }

  item.iImage = IMAGE_NODE;

  // then list all of the child nodes
  for (i = 0; i < pSelectedItem->nodeList.Count(); ++i)
  {
    item.lParam = (LPARAM) pSelectedItem->nodeList[i];
    item.pszText = pSelectedItem->nodeList[i]->pNode->GetName();
    item.iItem = index++;
    ListView_InsertItem(m_hRightList, &item);

    if (pSelectedNode == pSelectedItem->nodeList[i])
      ListView_SetItemState(m_hRightList, item.iItem, LVIS_SELECTED, LVIS_SELECTED);
  }
} // end Referee::DisplayRightList()


/*******************************************************************************
 Referee::DisplayBottomList()

 Displays a list of all nodes in the current selection set.
*******************************************************************************/
void Referee::DisplayBottomList()
{
  // clear out previous contents
  ListView_DeleteAllItems(m_hBottomList);

  LVITEM item;
  ZeroMemory(&item, sizeof(LVITEM));
  item.mask = LVIF_PARAM | LVIF_TEXT;

  int index = 0;

	for (int i = 0; i < m_pInterface->GetSelNodeCount(); ++i) 
	{
		// Get a selected node
		INode* pNode = m_pInterface->GetSelNode(i);

    // find the node in the node database
    NodeInfo* pInfo = NULL;
    for (int j = 0; j < m_nodeDatabase.Count(); ++j)
    {
      if (pNode == m_nodeDatabase[j]->pNode)
      {
        pInfo = m_nodeDatabase[j];
        break;
      }
    }

    // if it was found, build the "path" to it
    if (pInfo)
    {
      TSTR path = pInfo->pNode->GetName();
      TSTR temp;

      for (TreeItem* pParent = pInfo->pParent; pParent != m_pBase; pParent = pParent->pParent)
      {
        temp = GetObjectName(pParent->pObject);
        temp += "->";
        temp += path;
        path = temp;
      }

      item.pszText = path.data();
      item.lParam = (LPARAM) pInfo;
      item.iItem = index++;
      ListView_InsertItem(m_hBottomList, &item);
    }
  }
} // end Referee::DisplayBottomList()


/*******************************************************************************
 Referee::GetObjectName()

 Determines the displayed "name" of an object, based on what type of object it
 is.
*******************************************************************************/
TCHAR* Referee::GetObjectName(Object* pObject)
{
  static TSTR objName;
  
  pObject->InitNodeName(objName);

  if (pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
  {
    IDerivedObject* pDerivedObj = (IDerivedObject*) (pObject);

    if (pDerivedObj->NumModifiers())
    {
      Modifier* pModifier = pDerivedObj->GetModifier(0);
      objName = pModifier->GetName();
    }
  }
  
  return objName.data();
} // end Referee::GetObjectName()


/*******************************************************************************
 Referee::InitializeImageList()

 Creates the image list that will be used for all of the views.
*******************************************************************************/
void Referee::InitializeImageList()
{
  m_hIcons = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 4, 0);
  HBITMAP hbmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_OBJECT_ICONS));
  HBITMAP hmask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_OBJECT_MASK));
  ImageList_Add(m_hIcons, hbmp, hmask);

  ListView_SetImageList(m_hRightList, m_hIcons, LVSIL_SMALL);
  TreeView_SetImageList(m_hTree, m_hIcons, TVSIL_NORMAL);
} // end Referee::InitializeImageList()


/*******************************************************************************
 Referee::GetIconType()

 Returns the id of the image to be used for this object type
*******************************************************************************/
int Referee::GetIconType(Object* pObject)
{
  int iconID = IMAGE_BASEOBJ;

  if (pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
  {
    if (IsModifier((IDerivedObject*)pObject))
      iconID = IMAGE_MODIFIER;
    else
      iconID = IMAGE_REFOBJ;
  }
  else if (pObject->ClassID() == Class_ID(XREFOBJ_CLASS_ID,0))
  {
    iconID = IMAGE_XREFOBJ;
  }

  return iconID;
} // end Referee::GetIconType()


/*******************************************************************************
 Referee::SelectObjectNodes()

 Sets the selection set to all of the child nodes of the currently selected
 object.
*******************************************************************************/
void Referee::SelectObjectNodes(TreeItem* pTreeItem)
{
  if (!pTreeItem)
    return;

  INodeTab selectedNodes;

  GetChildNodes(pTreeItem, selectedNodes);

	theHold.Begin();
  m_pInterface->ClearNodeSelection(FALSE);
	m_pInterface->SelectNodeTab(selectedNodes, TRUE);
	TSTR undostr; undostr.printf("Select");
	theHold.Accept(undostr);

  m_pInterface->RedrawViews(m_pInterface->GetTime());

  m_changedSelSet = TRUE;

  DisplayBottomList();
} // end Referee::SelectObjectNodes()


/*******************************************************************************
 Referee::GetChildNodes()

 Recursively builds a list of all of the child nodes of a treeitem.
*******************************************************************************/
void Referee::GetChildNodes(TreeItem* pTreeItem, INodeTab& nodeList)
{
  int i;
  for (i = 0; i < pTreeItem->nodeList.Count(); ++i)
  {
    nodeList.Append(1, &pTreeItem->nodeList[i]->pNode);
  }

  for (i = 0; i < pTreeItem->childList.Count(); ++i)
  {
    GetChildNodes(pTreeItem->childList[i], nodeList);
  }
} // end Referee::GetChildNodes()


/*******************************************************************************
 Referee::SelectNodeFromList()

 Updates the selection set based on a doubleclick in the right panel. If the
 selected item was a node, then only that node is selected. If it was an object,
 then all of the object's child nodes are selected.
*******************************************************************************/
void Referee::SelectNodeFromList()
{
  int selection = ListView_GetSelectionMark(m_hRightList);

  if (selection == -1)
    return;

  LVITEM lvi;
  lvi.mask = LVIF_PARAM;
  lvi.iItem = selection;

  ListView_GetItem(m_hRightList, &lvi);

  // selected item was a node
  if (((NodeInfo*)lvi.lParam)->id == REFID_NODE)
  {
	  theHold.Begin();
    m_pInterface->ClearNodeSelection(FALSE);
	  m_pInterface->SelectNode(((NodeInfo*) lvi.lParam)->pNode, TRUE);
	  TSTR undostr; undostr.printf("Select");
	  theHold.Accept(undostr);

    m_pInterface->RedrawViews(m_pInterface->GetTime());

    DisplayBottomList();
  }
  // selected item was an object
  else
  {
    SelectObjectNodes((TreeItem*) lvi.lParam);
  }
} // end Referee::SelectNodeFromList()


/*******************************************************************************
 Referee::ProcessBottomListDblclk()

 When an item in the bottom list is doubleclicked, the treeview is expanded to
 the node's parent object, and the node is selected in the right list. The
 selection set is not modified.
*******************************************************************************/
void Referee::ProcessBottomListDblclk()
{
  int selection = ListView_GetSelectionMark(m_hBottomList);

  if (selection == -1)
    return;

  LVITEM lvi;
  lvi.mask = LVIF_PARAM;
  lvi.iItem = selection;

  ListView_GetItem(m_hBottomList, &lvi);

  NodeInfo* pSelected = (NodeInfo*) lvi.lParam;

  TreeView_SelectItem(m_hTree, pSelected->pParent->hItem);

  DisplayRightList(pSelected->pParent, pSelected);
} // end Referee::ProcessBottomListDblclk()


/*******************************************************************************
 Referee::CreateMultipleDragImage()

 Creates an image to be used when dragging multiple selections from the right
 list.
*******************************************************************************/
HIMAGELIST Referee::CreateMultipleDragImage(HWND hWnd)
{
  RECT  singleRect;	
  RECT  completeRect;
  int   index;
  BOOL  isFirst = TRUE;

  // Determine the size of the drag image
  index = ListView_GetNextItem(m_hRightList, -1, LVNI_SELECTED);
  while (index != -1)
  {
    ListView_GetItemRect(m_hRightList, index, &singleRect, LVIR_BOUNDS);
    if (isFirst)
    {
      // Initialize the CompleteRect
      ListView_GetItemRect(m_hRightList, index, &completeRect, LVIR_BOUNDS);
      isFirst = FALSE;
    }
    else
    {
      UnionRect(&completeRect, &completeRect, &singleRect);
    }
    index = ListView_GetNextItem(m_hRightList, index, LVNI_SELECTED);
  }

  // Create bitmap in memory DC
  HDC hdc = GetWindowDC(hWnd);
  HDC hMemDC = CreateCompatibleDC(hdc);

  if (!hMemDC)
    return NULL;

  HBITMAP hbmp = CreateCompatibleBitmap(hdc, completeRect.right - completeRect.left, completeRect.bottom - completeRect.top);
  HBITMAP hOldBmp = (HBITMAP) SelectObject(hMemDC, hbmp);

  // Here green is used as mask color
  LOGBRUSH lbr;
  lbr.lbColor = RGB(0, 255, 0);
  lbr.lbStyle = BS_SOLID;
  HBRUSH hbr = CreateBrushIndirect(&lbr);
  RECT rc;
  SetRect(&rc, 0, 0, completeRect.right - completeRect.left, completeRect.bottom - completeRect.top);
  FillRect(hMemDC, &rc, hbr);

  DeleteObject(hbr);

  // copy each selected item's image into the DC
  HIMAGELIST hSingleImageList;
  POINT pt;

  index = ListView_GetNextItem(m_hRightList, -1, LVNI_SELECTED);
  while (index != -1)
  {
    ListView_GetItemRect(m_hRightList, index, &singleRect, LVIR_BOUNDS);
    hSingleImageList = ListView_CreateDragImage(m_hRightList, index, &pt);

    if (hSingleImageList)
    {
      IMAGELISTDRAWPARAMS ildp;
      ildp.cbSize = sizeof(IMAGELISTDRAWPARAMS);
      ildp.himl = hSingleImageList;
      ildp.i = 0;
      ildp.hdcDst = hMemDC;
      ildp.x = singleRect.left - completeRect.left;
      ildp.y = singleRect.top - completeRect.top;
      ildp.cx = singleRect.right - singleRect.left;
      ildp.cy = singleRect.bottom - singleRect.top;
      ildp.xBitmap = ildp.yBitmap = 0;
      ildp.rgbBk = CLR_DEFAULT;
      ildp.rgbFg = CLR_DEFAULT;
      ildp.fStyle = ILD_NORMAL;
      ildp.dwRop = SRCCOPY;
      
      ImageList_DrawIndirect(&ildp);
      ImageList_Destroy(hSingleImageList);
    }
    index = ListView_GetNextItem(m_hRightList, index, LVNI_SELECTED);
  }

  SelectObject(hMemDC, hOldBmp);

  // Create the imagelist	with the merged drag images
  HIMAGELIST hCompleteImageList;
  hCompleteImageList = ImageList_Create(completeRect.right - completeRect.left,
                           completeRect.bottom - completeRect.top,
                           ILC_COLOR | ILC_MASK, 1, 0);

  ImageList_AddMasked(hCompleteImageList, hbmp, RGB(0, 255, 0));

  // cleanup
  DeleteObject(hbmp);
  DeleteDC(hMemDC);
  ReleaseDC(hWnd, hdc);

  return hCompleteImageList;
} // end Referee::CreateMultipleDragImage()


/*******************************************************************************
 Referee::OnBeginDrag()

 Determines what's being dragged, and stores information to complete the drag
 later.
*******************************************************************************/
void Referee::OnBeginDrag(HWND hWnd, LPARAM lParam)
{
  RECT rcItem;

  // drag source is the treeview
  if (hWnd == m_hTree)
  {
    LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;

    // Tell the tree view control to create an image to use for dragging.
    m_dragImage = TreeView_CreateDragImage(m_hTree, pnmtv->itemNew.hItem);

    // save the source for later
    m_treeDragSource = GetTreeItemFromHandle(pnmtv->itemNew.hItem);

    // Get the bounding rectangle of the item being dragged.
    TreeView_GetItemRect(m_hTree, pnmtv->itemNew.hItem, &rcItem, TRUE);

    // Start the drag operation.
    ImageList_BeginDrag(m_dragImage, 0, 0, rcItem.bottom - rcItem.top);
    ImageList_DragEnter(m_hTree, 100, 100);

    // Direct mouse input to the parent window.
    SetCapture(GetParent(m_hTree));
    m_dragState = DRAG_TREE;
  }
  // drag source is one of the listviews
  else if (hWnd == m_hRightList || hWnd == m_hBottomList)
  {
    LPNMLISTVIEW pnmlv = (LPNMLISTVIEW) lParam;

    // Tell the tree view control to create an image to use for dragging.
    if (hWnd == m_hRightList)
    {
      m_dragImage = CreateMultipleDragImage(GetParent(hWnd));
    }
    else
    {
      POINT pt;
      pt.x = pt.y = 0;
      m_dragImage = ListView_CreateDragImage(hWnd, pnmlv->iItem, &pt);
    }

    m_listDragSources.clear();

    // save the source(s) for later
    int index = ListView_GetNextItem(hWnd, -1, LVNI_SELECTED);
    while (index != -1)
    {
      LVITEM lvi;
      lvi.iItem = index;
      lvi.mask = LVIF_PARAM;
      ListView_GetItem(hWnd, &lvi);
      m_listDragSources.push_back((NodeInfo*) lvi.lParam);

      index = ListView_GetNextItem(m_hRightList, index, LVNI_SELECTED);
    }

    // Start the drag operation.
    ImageList_BeginDrag(m_dragImage, 0, 0, 0);
    ImageList_DragEnter(GetParent(hWnd), pnmlv->ptAction.x, pnmlv->ptAction.y);

    // Direct mouse input to the parent window.
    SetCapture(GetParent(hWnd));
    m_dragState = DRAG_LIST;
  }
  m_defaultCursor = GetCursor();
} // end Referee::OnBeginDrag()


/*******************************************************************************
 Referee::OnLButtonDown()

 Begins dragging one of the splitter bars.
*******************************************************************************/
void Referee::OnLButtonDown(HWND hWnd, POINT cursorScreenPos)
{
  RECT rect;

  GetWindowRect(hWnd, &rect);

  //convert the mouse coordinates relative to the top-left of the window
  cursorScreenPos.x -= rect.left;
  cursorScreenPos.y -= rect.top;

  //do the same conversion on the top of the horizontal splitter
  RECT clientRect;
  GetClientRect(hWnd, &clientRect);
  POINT pt;
  pt.x = 0;
  pt.y = m_hSplitterRatio * (clientRect.bottom - clientRect.top);
  ClientToScreen(hWnd, &pt);
  pt.y -= rect.top;

  //same for the window coordinates - make them relative to 0,0
  OffsetRect(&rect, -rect.left, -rect.top);

  if(cursorScreenPos.y < 0)
    cursorScreenPos.y = 0;
  if(cursorScreenPos.y > rect.bottom - SPLITTER_WIDTH)
    cursorScreenPos.y = rect.bottom - SPLITTER_WIDTH;

  SetCapture(hWnd);

  HDC hdc = GetWindowDC(hWnd);

  if (cursorScreenPos.y >= pt.y)
  {
    m_splitState = SPLIT_HDRAG;
    DrawXorBar(hdc, 1, cursorScreenPos.y - SPLITTER_WIDTH/2, rect.right - 2, SPLITTER_WIDTH);
    m_oldPos = cursorScreenPos.y;
  }
  else
  {
    m_splitState = SPLIT_VDRAG;
    DrawXorBar(hdc, cursorScreenPos.x - SPLITTER_WIDTH/2, 1, SPLITTER_WIDTH, rect.bottom - 2);
    m_oldPos = cursorScreenPos.x;
  }

  ReleaseDC(hWnd, hdc);
} // end Referee::OnLButtonDown()


/*******************************************************************************
 Referee::OnMouseMove()

 Handles mouse movements for drop & drag and splitter operations
*******************************************************************************/
void Referee::OnMouseMove(HWND hWnd, POINT cursorScreenPos)
{ 
  bool isTargetValid = false;

  POINT windowPos = cursorScreenPos;
  ScreenToClient(GetParent(m_hTree), &windowPos);

  if (m_dragState != DRAG_NONE)
  {
    POINT treePos = cursorScreenPos;
    ScreenToClient(m_hTree, &treePos);
    POINT listPos = cursorScreenPos;
    ScreenToClient(m_hRightList, &listPos);

    if (m_dragState == DRAG_TREE)
    {
      // Drag the item to the current position of the mouse pointer. 
      ImageList_DragMove(treePos.x, treePos.y);
      ImageList_DragShowNolock(FALSE);

      // Find out if the pointer is on the item. If it is, highlight the item as a drop target.
      HTREEITEM htiTarget;  // handle to target item 
      TVHITTESTINFO tvht;   // hit test information 
      tvht.pt = treePos;

      if ((htiTarget = TreeView_HitTest(m_hTree, &tvht)) != NULL &&
          IsLegalMove(GetTreeItemFromHandle(htiTarget), m_treeDragSource))
      {
        TreeView_SelectDropTarget(m_hTree, htiTarget);
        isTargetValid = true;
      }
      else
      {
        TreeView_SelectDropTarget(m_hTree, NULL);
      }
      ImageList_DragShowNolock(TRUE);
      UpdateWindow(m_hTree);
    }

    // a list is dragging
    else
    {
      // Drag the item to the current position of the mouse pointer.
      ImageList_DragMove(windowPos.x, windowPos.y);
      ImageList_DragShowNolock(FALSE);

      // see if the cursor is over the treeview
      HTREEITEM htiTarget;  // handle to target item 
      TVHITTESTINFO tvht;   // hit test information 
      tvht.pt = treePos;

      if ((htiTarget = TreeView_HitTest(m_hTree, &tvht)) != NULL &&
          IsLegalMove(GetTreeItemFromHandle(htiTarget), m_listDragSources))
      {
        TreeView_SelectDropTarget(m_hTree, htiTarget);
        m_listDropTarget = GetTreeItemFromHandle(htiTarget);
        isTargetValid = true;

        // make sure the right list is not highlighted
        if (m_lastTarget != -1)
        {
          ListView_SetItemState(m_hRightList, m_lastTarget, 0, LVIS_DROPHILITED);
          ListView_RedrawItems(m_hRightList, m_lastTarget, m_lastTarget);
          UpdateWindow(m_hRightList);
          m_lastTarget = -1;
        }
      }
      else
      {
        // clear out any treeview highlights
        TreeView_SelectDropTarget(m_hTree, NULL);

        // see if the cursor is over the right listview
        LVHITTESTINFO lvht;
        lvht.pt = listPos;
  
        if (ListView_HitTest(m_hRightList, &lvht) != -1 &&
            IsValidTarget(m_hRightList, lvht.iItem, m_listDragSources))
        {
          ListView_SetItemState(m_hRightList, lvht.iItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
          ListView_RedrawItems(m_hRightList, lvht.iItem, lvht.iItem);

          if (m_lastTarget != -1 && m_lastTarget != lvht.iItem)
          {
            ListView_SetItemState(m_hRightList, m_lastTarget, 0, LVIS_DROPHILITED);
            ListView_RedrawItems(m_hRightList, m_lastTarget, m_lastTarget);
          }
          UpdateWindow(m_hRightList);

          m_lastTarget = lvht.iItem;

          LVITEM lvi;
          lvi.iItem = lvht.iItem;
          lvi.mask = TVIF_PARAM;
          ListView_GetItem(m_hRightList, &lvi);
          m_listDropTarget = (TreeItem*) lvi.lParam;

          isTargetValid = true;
        }
        else
        {
          // make sure the right list is not highlighted
          if (m_lastTarget != -1)
          {
            ListView_SetItemState(m_hRightList, m_lastTarget, 0, LVIS_DROPHILITED);
            ListView_RedrawItems(m_hRightList, m_lastTarget, m_lastTarget);
            UpdateWindow(m_hRightList);
            m_lastTarget = -1;
          }
          m_listDropTarget = NULL;
        }
      }
      ImageList_DragShowNolock(TRUE);
    } // end if

  }

  else if (m_splitState != SPLIT_NODRAG)
  {
    RECT rect;

    GetWindowRect(hWnd, &rect);

    cursorScreenPos.x -= rect.left;
    cursorScreenPos.y -= rect.top;

    OffsetRect(&rect, -rect.left, -rect.top);

    if (cursorScreenPos.y < 0)
      cursorScreenPos.y = 0;
    if (cursorScreenPos.y > rect.bottom - SPLITTER_WIDTH)
      cursorScreenPos.y = rect.bottom - SPLITTER_WIDTH;
    
    if(cursorScreenPos.x < 0)
      cursorScreenPos.x = 0;

    if(cursorScreenPos.x > rect.right - SPLITTER_WIDTH) 
	    cursorScreenPos.x = rect.right - SPLITTER_WIDTH;

    if (m_splitState == SPLIT_HDRAG)
    {
      if (cursorScreenPos.y != m_oldPos)
      {
        HDC hdc = GetWindowDC(hWnd);
        DrawXorBar(hdc, 1, m_oldPos - SPLITTER_WIDTH/2, rect.right - 2, SPLITTER_WIDTH);
        DrawXorBar(hdc, 1, cursorScreenPos.y - SPLITTER_WIDTH/2, rect.right - 2, SPLITTER_WIDTH);
	        
        ReleaseDC(hWnd, hdc);

        m_oldPos = cursorScreenPos.y;
      }
    }
    else
    {
      if (cursorScreenPos.x != m_oldPos)
      {
        HDC hdc = GetWindowDC(hWnd);
        DrawXorBar(hdc, m_oldPos - SPLITTER_WIDTH/2, 1, SPLITTER_WIDTH, rect.bottom - 2);
        DrawXorBar(hdc, cursorScreenPos.x - SPLITTER_WIDTH/2, 1, SPLITTER_WIDTH, rect.bottom - 2);
	        
        ReleaseDC(hWnd, hdc);

        m_oldPos = cursorScreenPos.x;
      }
    }
  }

  // if necessary, change the cursor to indicate that the current drag operation
  // is not legal
  if (m_dragState != DRAG_NONE)
  {
    if (isTargetValid)
    {
      SetCursor(m_defaultCursor);
    }
    else
    {
      SetCursor(LoadCursor(NULL, IDC_NO));
    }
  }

  // or, change the cursor if it is over one of the splitter windows.
  else if (m_splitState == SPLIT_NODRAG)
  {
    RECT rect;
    GetClientRect(hWnd, &rect);

    if (windowPos.y >= m_hSplitterRatio * (rect.bottom - rect.top) - 1)
    {
      SetCursor(LoadCursor(NULL, IDC_SIZENS));
    }
    else
    {
      SetCursor(LoadCursor(NULL, IDC_SIZEWE));
    }
  }
} // end Referee::OnMouseMove()


/*******************************************************************************
 Referee::OnLButtonUp()

 Handles mouse movements for drop & drag and splitter operations
*******************************************************************************/
void Referee::OnLButtonUp(HWND hWnd, POINT cursorScreenPos)
{
  if (m_dragState != DRAG_NONE)
  {
    // see if we were dragging from the tree
    if (m_dragState == DRAG_TREE)
    {
      HTREEITEM htiTarget = TreeView_GetDropHilight(m_hTree);

      if (htiTarget)
      {
        RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

        ModifyObjectRelationship(m_treeDragSource, GetTreeItemFromHandle(htiTarget), pRestoreObj);

        pRestoreObj->Execute();
        theHold.Begin();
        if ( theHold.Holding() )
          theHold.Put(pRestoreObj);
        theHold.Accept(_T("Change object relationships"));

        DisplayTreeview(m_pBase, TVI_ROOT);
      }
    }

    // otherwise, the source was a list, so see if we have a valid target
    else if (m_listDropTarget)
    {
      RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

      HTREEITEM hOldSelection = TreeView_GetSelection(m_hTree);
      TreeView_SelectItem(m_hTree, NULL);
      for (int i = 0; i < m_listDragSources.size(); ++i)
      {
        // source is a node
        if (m_listDragSources[i]->id == REFID_NODE)
        {
          ModifyNodeRelationship(m_listDragSources[i], m_listDropTarget, pRestoreObj);
        }
        // source is an object
        else
        {
          ModifyObjectRelationship((TreeItem*) m_listDragSources[i], m_listDropTarget, pRestoreObj);
        }
      }
      pRestoreObj->Execute();
      theHold.Begin();
      if ( theHold.Holding() )
        theHold.Put(pRestoreObj);
      theHold.Accept(_T("Change object relationships"));

      if (!TreeView_SelectItem(m_hTree, hOldSelection))
      {
        TreeView_SelectItem(m_hTree, m_listDropTarget->hItem);
      }

      DisplayBottomList();
    }

    m_treeDragSource = NULL;
    m_lastTarget = -1;
    m_listDropTarget = NULL;
    TreeView_SelectDropTarget(m_hTree, NULL);
    ListView_SetItemState(m_hRightList, -1, 0, LVIS_DROPHILITED);
    ImageList_DragLeave(GetParent(m_hTree));
    ImageList_EndDrag();
    ImageList_Destroy(m_dragImage);
    ReleaseCapture();
    m_dragState = DRAG_NONE;

    InvalidateRect(m_hTree, NULL, TRUE);
    UpdateWindow(m_hTree);

    m_pInterface->ForceCompleteRedraw(m_pInterface->GetTime());
  }

  // see if this is the completion of a split drag
  else if (m_splitState != SPLIT_NODRAG)
  {
    RECT rect;

    GetWindowRect(hWnd, &rect);

    cursorScreenPos.x -= rect.left;
    cursorScreenPos.y -= rect.top;

    OffsetRect(&rect, -rect.left, -rect.top);

    if(cursorScreenPos.y < 0)
      cursorScreenPos.y = 0;

    if(cursorScreenPos.y > rect.bottom - SPLITTER_WIDTH) 
	    cursorScreenPos.y = rect.bottom - SPLITTER_WIDTH;

    if(cursorScreenPos.x < 0)
      cursorScreenPos.x = 0;

    if(cursorScreenPos.x > rect.right - SPLITTER_WIDTH) 
	    cursorScreenPos.x = rect.right - SPLITTER_WIDTH;

    HDC hdc = GetWindowDC(hWnd);

    if (m_splitState == SPLIT_HDRAG)
    {
      DrawXorBar(hdc, 1, m_oldPos - SPLITTER_WIDTH/2, rect.right - 2, SPLITTER_WIDTH);
    }
    else
    {
      DrawXorBar(hdc, m_oldPos - SPLITTER_WIDTH/2, 1, SPLITTER_WIDTH, rect.bottom - 2);
    }
    ReleaseDC(hWnd, hdc);

    //convert the splitter position back to screen coords.
    GetWindowRect(hWnd, &rect);
    cursorScreenPos.x += rect.left;
    cursorScreenPos.y += rect.top;

    //now convert into CLIENT coordinates
    ScreenToClient(hWnd, &cursorScreenPos);
    GetClientRect(hWnd, &rect);

    if (m_splitState == SPLIT_HDRAG)
    {
      m_hSplitterRatio = (float)cursorScreenPos.y / (float)(rect.bottom - rect.top);
    }
    else
    {
      m_vSplitterRatio = (float)cursorScreenPos.x / (float)(rect.right - rect.left);
    }

    //position the child controls
    ResizeWindows(rect.right, rect.bottom);

    ReleaseCapture();
    m_splitState = SPLIT_NODRAG;
  }
  else
  {
    return;
  }
} // end Referee::OnLButtonUp()


/*******************************************************************************
 Referee::ModifyObjectRelationship()

 Changes the object wrapped by pSource to reference the object wrapped by
 pTarget
*******************************************************************************/
void Referee::ModifyObjectRelationship(TreeItem* pSource, TreeItem* pTarget, RefereeRestoreObj* pRestoreObj)
{
  if (pSource == pTarget || !pSource || !pTarget)
    return;

  if (IsDerivedObject(pSource->pObject))
  {
    // update the relationship in Max
    IDerivedObject* pDerivedObj = (IDerivedObject*)pSource->pObject;
    pRestoreObj->AddObject(pSource->pParent->pObject, pTarget->pObject, pDerivedObj);

    // update the relationship in the internal representation
    RemoveChildObject(pSource->pParent, pSource);
    pTarget->childList.Append(1, &pSource);
    pSource->pParent = pTarget;

    if (IsDerivedObject(pTarget->pObject))
    {
      // Derived Object to Derived Object - Move the source object to be a child
      // of the target object.  Hold a keyboard modifier (Ctrl, Shift, etc) to
      // have the target object replace the source object.

    }
    else
    {
      // Derived Object to Base Object - Move the derived object to be a child of the target object.

    }
  }
  else
  {
    // Base Object to Derived Object or Base Object to Base Object - Target object replaces the source object.
    Object* pOldParent = pSource->pObject;

    // update internal representation
    RemoveChildObject(pSource->pParent, pSource);  // remove the source from its parent's child list

    // set all child objects of the source to be children of the target
    int i;

    for (i = 0; i < pSource->childList.Count(); ++i)
    {
      pSource->childList[i]->pParent = pTarget;
      pTarget->childList.Append(1, &pSource->childList[i]);
      IDerivedObject* pDerivedObj = (IDerivedObject*)pSource->childList[i]->pObject;
      pRestoreObj->AddObject(pOldParent, pTarget->pObject, pDerivedObj);
    }

    // set all child nodes of the source to be children of the target
    for (i = 0; i < pSource->nodeList.Count(); ++i)
    {
      pSource->nodeList[i]->pParent = pTarget;
      pTarget->nodeList.Append(1, &pSource->nodeList[i]);
      pRestoreObj->AddNode(pOldParent, pTarget->pObject, pSource->nodeList[i]->pNode);
    }
  }
} // end Referee::ModifyObjectRelationship()


/*******************************************************************************
 Referee::ModifyNodeRelationship()

 Changes the node wrapped by pSource to reference the object wrapped by
 pTarget
*******************************************************************************/
void Referee::ModifyNodeRelationship(NodeInfo* pSource, TreeItem* pTarget, RefereeRestoreObj* pRestoreObj)
{
  if (!pSource || !pTarget)
    return;

  // update the relationship in Max
  pRestoreObj->AddNode(pSource->pParent->pObject, pTarget->pObject, pSource->pNode);

  // update the relationship in the internal representation
  RemoveChildNode(pSource->pParent, pSource);
  pTarget->nodeList.Append(1, &pSource);
  pSource->pParent = pTarget;
} // end Referee::ModifyNodeRelationship()


/*******************************************************************************
 Referee::ResizeWindows()

 Adjust all windows based on the size of the parent's client area, as well as
 the relative position of the sliders.
*******************************************************************************/
void Referee::ResizeWindows(int width, int height)
{
  if (m_vSplitterRatio < 0.05)
    m_vSplitterRatio = 0.05f;
  if (m_vSplitterRatio > 0.95)
    m_vSplitterRatio = 0.95f;
  if (m_hSplitterRatio < 0.05)
    m_hSplitterRatio = 0.05f;
  if (m_hSplitterRatio > 0.95)
    m_hSplitterRatio = 0.95f;

  int vSplitterPos = m_vSplitterRatio * width;
  int hSplitterPos = m_hSplitterRatio * height;

  MoveWindow(m_hTree, 0, 0, vSplitterPos, hSplitterPos, TRUE);
  MoveWindow(m_hRightList, vSplitterPos + SPLITTER_WIDTH, 0, width - vSplitterPos - SPLITTER_WIDTH, hSplitterPos, TRUE);
  MoveWindow(m_hBottomList, 0, hSplitterPos + SPLITTER_WIDTH, width, height - hSplitterPos - SPLITTER_WIDTH, TRUE);

  RECT rect;
  GetClientRect(m_hBottomList, &rect);
  ListView_SetColumnWidth(m_hBottomList, 0, rect.right - rect.left);

  UpdateWindow(GetParent(m_hTree));
} // Referee::ResizeWindows()


/*******************************************************************************
 Referee::SaveWindowPositions()

 Save window position settings to the registry
*******************************************************************************/
void Referee::SaveWindowPositions(HWND hWnd)
{
  Registry reg(REFEREE_KEY);

  RECT rect;
  GetWindowRect(hWnd, &rect);

  DWORD vSplitter = 1000 * m_vSplitterRatio;
  DWORD hSplitter = 1000 * m_hSplitterRatio;

  reg.Set("xpos", rect.left);
  reg.Set("ypos", rect.top);
  reg.Set("width", rect.right - rect.left);
  reg.Set("height", rect.bottom - rect.top);
  reg.Set("vSplitterPos", vSplitter);
  reg.Set("hSplitterPos", hSplitter);
} // end Referee::SaveWindowPositions()


/*******************************************************************************
 Referee::SaveWindowPositions()

 Restore window position settings from the registry
*******************************************************************************/
void Referee::RestoreWindowPositions(HWND hWnd)
{
  Registry reg(REFEREE_KEY);

  DWORD x, y, width, height;

  // use defaults if no registry key exists
  if (reg.KeyWasNew())
  {
    x = 50;
    y = 50;
    width = 500;
    height = 400;
    m_vSplitterRatio = 0.50;
    m_hSplitterRatio = 0.75;
  }
  else
  {
    DWORD vSplitter, hSplitter;
    reg.Get("xpos", &x);
    reg.Get("ypos", &y);
    reg.Get("width", &width);
    reg.Get("height", &height);
    reg.Get("vSplitterPos", &vSplitter);
    reg.Get("hSplitterPos", &hSplitter);
    m_vSplitterRatio = (float)vSplitter / 1000;
    m_hSplitterRatio = (float)hSplitter / 1000;
  }

  MoveWindow(hWnd, x, y, width, height, TRUE);
} // end Referee::SaveWindowPositions()

