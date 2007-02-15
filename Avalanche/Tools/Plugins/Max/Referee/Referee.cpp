////////////////////////////////////////////////////////////////////////////
//
// Referee
//
// Xref, reference, instance manager plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Referee.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Dave Astle   Date: 5/01/02    Time: 3:51p
 * Updated in $/Avalanche/tools/Plugins/Max/Referee
 * 
 * *****************  Version 6  *****************
 * User: Dave Astle   Date: 4/18/02    Time: 10:46a
 * Updated in $/Avalanche/tools/Plugins/Max/Referee
 * Relationship window in beta stage; undo added for all referee
 * operations *except* collapse to mesh
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/30/01   Time: 2:00p
 * Updated in $/Avalanche/tools/Plugins/Max/Referee
 * copy materials from picked objects
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/28/01   Time: 3:25p
 * Updated in $/Avalanche/tools/Plugins/Max/Referee
 * Collapse to Base
 * Collapse one level
 * Link to picked object
 * Link to picked base
 * Add a level
 */

#include "Referee.h"
#include "pick.h"
#include "modstack.h"
#include <string>

Referee theReferee;
RefereeClassDesc RefereeDesc;

ClassDesc2* GetRefereeDesc() 
{ 
	return &RefereeDesc; 
}

void *RefereeClassDesc::Create(BOOL loading)
{
	return &theReferee;
}



class RefereeViewWindow : public ViewWindow
{
public:
  TCHAR* GetName() { return _T("Relationship Window"); }
  HWND CreateViewWindow(HWND hParent, int x, int y, int w, int h)
  {
    theReferee.m_isDialogUp = TRUE;
    return theReferee.LaunchRelationshipWindow();
  }

  void DestroyViewWindow(HWND hWnd) { DestroyWindow(hWnd); }

  BOOL CanCreate() { return !theReferee.m_isDialogUp; }
  int NumberCanCreate() { return 1; }
};




static BOOL CALLBACK RefereeDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
			theReferee.Init(hWnd);
			break;

		case WM_DESTROY:
			theReferee.Destroy(hWnd);
			break;

		case WM_COMMAND:

			switch (LOWORD(wParam)) 
			{
				case IDC_CMD_CONVERT_TO_XREF:
					// -- Convert selected object into an XRef Object --
					theReferee.ConvertSelectedToXrefObject(hWnd);
					break;
				case IDC_CMD_COLLAPSE_ONE_LEVEL:
					theReferee.CollapseOneLevel();
					break;
				case IDC_CMD_COLLAPSE_TO_BASE:
					theReferee.CollapseToBase();
					break;
				case IDC_CMD_SELECT_SHARE_BASE:
					theReferee.SelectNodesSameObject(hWnd,true);
					break;
				case IDC_CMD_SELECT_SHARE_OBJECT:
					theReferee.SelectNodesSameObject(hWnd,false);
					break;
				case IDC_CMD_COLLAPSE_TO_MESH:
					theReferee.CollapseToMesh();
					break;
				case IDC_CMD_ADD_ONE_LEVEL:
					theReferee.AddLevel();
					break;
				case IDC_CMD_LINK_TO_PICKED_OBJECT:
					thePickMode.m_buttonIndex = LINKED_PICKED_OBJECT;
					theReferee.m_pInterface->SetPickMode(&thePickMode); 
					break;
				case IDC_CMD_LINK_TO_PICKED_BASE:
					thePickMode.m_buttonIndex = LINKED_PICKED_BASE;
					theReferee.m_pInterface->SetPickMode(&thePickMode); 
					break;
				case IDC_CMD_RELATIONSHIP_WINDOW:
          theReferee.LaunchRelationshipWindow();
					break;
			}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theReferee.m_pInterface->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}
	return TRUE;
}


RefereeViewWindow* pRefViewWnd = NULL;

//--- Referee -------------------------------------------------------
Referee::Referee()
{
  m_isDialogUp = FALSE;
  m_changedSelSet = FALSE;
	m_pUtil = NULL;
	m_pInterface = NULL;	
	m_hPanel = NULL;
	m_linkedPickedObjectButton = NULL;
	m_linkedPickedBaseButton = NULL;
  m_pBase = NULL;
  m_rightListNodeArray = NULL;
  m_dragState = DRAG_NONE;
  m_isTreeDragging = FALSE;
  m_isListDragging = FALSE;
  m_treeDragSource = NULL;
  m_listDropTarget = NULL;
  m_splitState = SPLIT_NODRAG;
  m_lastTarget = -1;

	RegisterNotification(NotifySelSetChanged, NULL, NOTIFY_SELECTIONSET_CHANGED);

}

Referee::~Referee()
{
	UnRegisterNotification(NotifySelSetChanged, NULL, NOTIFY_SELECTIONSET_CHANGED);
}

void Referee::BeginEditParams(Interface *m_pInterface,IUtil *iu) 
{
	this->m_pUtil = iu;
	this->m_pInterface = m_pInterface;
	m_hPanel = m_pInterface->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		RefereeDlgProc,
		GetString(IDS_PARAMS),
		0);

  if (!pRefViewWnd)
  {
    pRefViewWnd = new RefereeViewWindow();
    m_pInterface->RegisterViewWindow(pRefViewWnd);
  }
}
	
void Referee::EndEditParams(Interface *m_pInterface,IUtil *iu) 
{
	this->m_pUtil = NULL;
	this->m_pInterface = NULL;
	m_pInterface->DeleteRollupPage(m_hPanel);
	m_hPanel = NULL;

  if (!pRefViewWnd)
  {
    m_pInterface->UnRegisterViewWindow(pRefViewWnd);
    delete pRefViewWnd;
    pRefViewWnd = NULL;
  }
}

void Referee::Init(HWND hWnd)
{
	m_linkedPickedObjectButton = GetICustButton(GetDlgItem(hWnd,IDC_CMD_LINK_TO_PICKED_OBJECT));
	m_linkedPickedObjectButton->SetType(CBT_CHECK);
	m_linkedPickedObjectButton->SetHighlightColor(GREEN_WASH);

	m_linkedPickedBaseButton = GetICustButton(GetDlgItem(hWnd,IDC_CMD_LINK_TO_PICKED_BASE));
	m_linkedPickedBaseButton->SetType(CBT_CHECK);
	m_linkedPickedBaseButton->SetHighlightColor(GREEN_WASH);
}

void Referee::Destroy(HWND hWnd)
{

}

// Function name	: Referee::ConvertSelectedToXrefObject
// Description	    : ...please read below...
// Return type		: void 
// Argument         : HWND hWnd
void Referee::ConvertSelectedToXrefObject(HWND hWnd)
{
    // *** ConvertSelectedToXrefObject ***
    //
    // Takes single selection of scene object and
    // "converts" it to XRef Object with default settings.  This is 
    // done via the following steps:
    //
    // 1) Save selected object into new MAX file of user's specification
    // 2) Create a new XRefObject
    // 3) XRefObject makes reference to the objectref of the selected node (OSM-level only)
    // 4) INode object reference changed to XRefObject
    //
    // converting multiple selected objects is left as an exercise
    // but should be quite straight-forward...
	
    INode * pNode = NULL;
    TSTR filename = "";
	
	int nodeCount = m_pInterface->GetSelNodeCount();
	if (nodeCount==0)
	{
        ::MessageBox(hWnd, "Select at least 1 node", GetString(IDS_ERROR_TITLE), MB_ICONSTOP | MB_OK);
        return;
	}

  // create undo object
  RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

	for (int i=0;i<nodeCount;i++)
	{
		pNode = m_pInterface->GetSelNode(i);
		if (!pNode) 
			continue;
		filename = pNode->GetName();
		if (!DoOpenSaveDialog(filename)) 
			continue;

		INodeTab nodeTab;
		nodeTab.Append(1,&pNode);
		m_pInterface->FileSaveNodes(&nodeTab,filename);
		
		// One caveat : If the object (not node) has any ReferenceMakers watching it
		// that are expecting geom pipeline msgs, you may need to remove/reset these
		// references.  After the conversion, the original object is effectively
		// hidden from the pipeline.
		
		NodeToXref(pNode, filename, false, pRestoreObj);
	}

  // perform conversion and register undo
  pRestoreObj->Execute();
  theHold.Begin();
  if ( theHold.Holding() )
    theHold.Put(pRestoreObj);
  theHold.Accept(_T("Convert to XRef"));

  m_pInterface->RedrawViews(m_pInterface->GetTime());
}

bool Referee::DoOpenSaveDialog(TSTR &fileName,bool bOpen)
{
    // Does a standard Win32 CommonDlg Save-As or Open dialog
    // for a .MAX file
    //
    // (doesn't use registered custom dlg in MAX, as
    // in truth, only MAX can access this cache currently,
    // although you'll get any registered dialog if
    // you call Interface::FileSave or something similar,
    // but we just want to get a filename, not save yet)
	
    OPENFILENAME    ofn;
    TCHAR           szFilter[]=__TEXT("3D Studio MAX (*.MAX)\0*.MAX\0\0");     
    TCHAR           fname[512];
	
    _tcscpy(fname,fileName);
    
    // set up that OPENFILENAME struct
    ::memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize      = sizeof(OPENFILENAME);
    ofn.hwndOwner        = m_hPanel;
    ofn.nFilterIndex     = 1L;
    ofn.lpstrFilter      = szFilter;
    ofn.lpstrCustomFilter = (LPTSTR)NULL;
    ofn.lpstrFile        = fname;
    ofn.nMaxFile         = sizeof(fname) / sizeof(TCHAR);
    ofn.lpstrFileTitle   = NULL;
    ofn.nMaxFileTitle    = 0;
    ofn.lpstrInitialDir  = m_pInterface->GetDir(APP_SCENE_DIR);
    ofn.lpstrTitle       = (LPCSTR)NULL;
    if (bOpen) {
        ofn.Flags            = OFN_HIDEREADONLY | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES;
    } else {
        ofn.Flags            = OFN_HIDEREADONLY | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_OVERWRITEPROMPT;
    }
    ofn.lpstrDefExt      = _TEXT("MAX");
	
    if (bOpen) {
        if (GetOpenFileName(&ofn)) {
			
            // NOTE: More error checking needs to be done for this
            // to be practical -- e.g. we shouldn't allow the user to
            // select the currently open file.  
			
            fileName = TSTR(ofn.lpstrFile); // full path and file 
            return true; // success
			
        } else {
            // user canceled
            return false;
        }
		
    } else {   
        if (GetSaveFileName(&ofn)) {
			
            // NOTE: More error checking needs to be done for this
            // to be practical -- e.g. we shouldn't allow the user to
            // select the currently open file.  
			
            fileName = TSTR(ofn.lpstrFile); // full path and file 
            return true; // success
			
        } else {
            // user canceled
            return false;
        }
    }
    return false; // failure
}

// Function name	: Xrefutil::NodeToXref
// Description	    : This method does a straightforward conversion of a Node to an XRef
// Return type		: void 
// Argument         : INode * pNode
// Argument         : TSTR &filename
// Argument         : bool bProxy
// Argument         : bool bIgnoreAnim
void Referee::NodeToXref(INode * pNode, TSTR &filename, bool bProxy, RefereeRestoreObj* pRestoreObj, bool bIgnoreAnim)
{
    IXRefObject * pXRef = (IXRefObject *)m_pInterface->CreateInstance(SYSTEM_CLASS_ID, 
        Class_ID(XREFOBJ_CLASS_ID,0));
    TSTR obName = TSTR(pNode->GetName());
    pXRef->Init(filename, obName, pNode->GetObjectRef(), bProxy);

//    pNode->SetObjectRef(pXRef);
    pRestoreObj->AddNode(pNode->GetObjectRef(), pXRef, pNode);

    // also, set visual queue that we're ignoring anim if we did
    if (bIgnoreAnim)
        pXRef->SetIgnoreAnim(TRUE,FALSE);
}

void Referee::SelectNodesSameObject(HWND hWnd,bool findBase)
{
	int nodeCount = m_pInterface->GetSelNodeCount();
	if (nodeCount==0)
	{
        ::MessageBox(hWnd, "Select at least 1 node", GetString(IDS_ERROR_TITLE), MB_ICONSTOP | MB_OK);
        return;
	}
	INode *pNode;
	Tab<Object *> objectPtrArray;
	INodeTab selectedNodeArray;

	// build an array of objects that the selected nodes point to
	for (int i=0;i<nodeCount;i++)
	{
		pNode = m_pInterface->GetSelNode(i);
		if (!pNode) 
			continue;
		selectedNodeArray.Append(1,&pNode);
		Object *object = pNode->GetObjectRef();
		if (findBase)
			object = object->FindBaseObject();

		for (int j=0;j<objectPtrArray.Count();j++)
		{
			if (objectPtrArray[j]==object)
				break;
		}
		if (j==objectPtrArray.Count())
		{
			objectPtrArray.Append(1,&object);
		}
	}
	INodeTab nodePtrArray;
	pNode = m_pInterface->GetRootNode();
	AddReferenceNodes(pNode,nodePtrArray,objectPtrArray,selectedNodeArray,findBase);

	theHold.Begin();
	m_pInterface->SelectNodeTab(nodePtrArray,TRUE);
	TSTR undostr; undostr.printf("Select");
	theHold.Accept(undostr);
}


void Referee::AddReferenceNodes(INode *parentNode,INodeTab &nodePtrArray,const Tab<Object *> &objectPtrArray,const INodeTab &selectedNodeArray,bool findBase)
{
	int numChildren = parentNode->NumberOfChildren();
	// go through the node's children
	for (int i=0;i<numChildren;i++)
	{
		INode *node = parentNode->GetChildNode(i);
		if (!node)
			continue;
		// see if already selected
		for (int j=0;j<selectedNodeArray.Count();j++)
		{
			if (selectedNodeArray[j]==node)
				break;
		}
		// if not selected then check if it is referencing one of the objects in the array
		if (j==selectedNodeArray.Count())
		{
			Object *object = node->GetObjectRef();
			if (findBase)
				object = object->FindBaseObject();
			for (j=0;j<objectPtrArray.Count();j++)
			{
				if (objectPtrArray[j]==object)
				{
					nodePtrArray.Append(1,&node); // add to the array of nodes
					break;
				}
			}
		}
		AddReferenceNodes(node,nodePtrArray,objectPtrArray,selectedNodeArray,findBase);
	}
}

void Referee::CollapseToMesh(void)
{
	INode *node;
	TriObject *tobj = NULL;
	INodeTab flash;
  RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

	for (int i=0; i<m_pInterface->GetSelNodeCount(); i++) 
	{
		// Get a selected node
		node = m_pInterface->GetSelNode(i);
		flash.Append(1,&node,10);

		// Eval the node's object (exclude WSMs)
		Object *oldObj = node->GetObjectRef();
		
		// Check for NULL
		if (!oldObj) continue;

		// Skip bones
		if (oldObj->ClassID()==Class_ID(BONE_CLASS_ID,0)) continue;		

		// RB 6/14/99: Skip system nodes too
		Control *tmCont = node->GetTMController();
		if (tmCont && GetMasterController(tmCont)) continue;

		NotifyCollapseEnumProc PreNCEP(true,node);
		EnumGeomPipeline(&PreNCEP,node);

		ObjectState os = oldObj->Eval(m_pInterface->GetTime());

		Object *obj = (Object*)os.obj->CollapseObject();

		if(obj == os.obj)
			obj = (Object*)obj->Clone();

		if (os.obj->CanConvertToType(triObjectClassID)) 
		{
			// Convert it to a TriObject and make that the new object
			tobj = (TriObject*)obj->ConvertToType(m_pInterface->GetTime(),triObjectClassID);
			oldObj->SetAFlag(A_LOCK_TARGET);
			node->SetObjectRef(tobj);		
			oldObj->ClearAFlag(A_LOCK_TARGET);

			// NS: 4/6/00 Notify all mods and objs in the pipleine, that they have been collapsed
			NotifyCollapseEnumProc PostNCEP(false,node,tobj);
			EnumGeomPipeline(&PostNCEP,oldObj);
			
			if (obj!=tobj) obj->AutoDelete();
		}
	}
  // perform collapse and register undo
  pRestoreObj->Execute();
  theHold.Begin();
  if ( theHold.Holding() )
    theHold.Put(pRestoreObj);
  theHold.Accept(_T("Collapse to mesh"));

	// Flash nodes
	m_pInterface->FlashNodes(&flash);

	m_pInterface->RedrawViews(m_pInterface->GetTime());
//	GetSystemSetting(SYSSET_CLEAR_UNDO);
//	SetSaveRequiredFlag(TRUE);
}

void Referee::AddLevel(void)
{
	INode *node;
	INodeTab flash;
  RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

	for (int i=0; i<m_pInterface->GetSelNodeCount(); i++) 
	{
		// Get a selected node
		node = m_pInterface->GetSelNode(i);
		flash.Append(1,&node,10);

		// Eval the node's object (exclude WSMs)
		Object *oldObj = node->GetObjectRef();
		
		// Check for NULL
		if (!oldObj) continue;

		// Skip bones
		if (oldObj->ClassID()==Class_ID(BONE_CLASS_ID,0)) continue;		

		// RB 6/14/99: Skip system nodes too
		Control *tmCont = node->GetTMController();
		if (tmCont && GetMasterController(tmCont)) continue;

		Object *newObj = CreateDerivedObject(oldObj);
		if (!newObj)
			continue;
    pRestoreObj->AddNode(oldObj, newObj, node);
//		node->SetObjectRef(newObj);		
	}

  // perform add and register undo
  pRestoreObj->Execute();
  theHold.Begin();
  if ( theHold.Holding() )
    theHold.Put(pRestoreObj);
  theHold.Accept(_T("Add level"));

	// Flash nodes
	m_pInterface->FlashNodes(&flash);

	m_pInterface->RedrawViews(m_pInterface->GetTime());
//	GetSystemSetting(SYSSET_CLEAR_UNDO);
//	SetSaveRequiredFlag(TRUE);
}

void Referee::CollapseOneLevel(void)
{
	INode *node;

  RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

	for (int i=0; i<m_pInterface->GetSelNodeCount(); i++) 
	{
		// Get a selected node
		node = m_pInterface->GetSelNode(i);

		// Eval the node's object (exclude WSMs)
		Object *oldObj = node->GetObjectRef();
		
		// Check for NULL
		if (!oldObj) continue;

		// Skip bones
		if (oldObj->ClassID()==Class_ID(BONE_CLASS_ID,0)) continue;		

		// RB 6/14/99: Skip system nodes too
		Control *tmCont = node->GetTMController();
		if (tmCont && GetMasterController(tmCont)) continue;


		// if this is a derived class point to the object it is pointing to
		if (oldObj->SuperClassID()==GEN_DERIVOB_CLASS_ID)
		{
			IDerivedObject *dObj = (IDerivedObject*)oldObj;
			Object *nextObj = dObj->GetObjRef();
			if (!nextObj)
				continue;
      pRestoreObj->AddNode(oldObj, nextObj, node);
//			node->SetObjectRef(nextObj);		
		}
	}
  // perform collapse and register undo
  pRestoreObj->Execute();
  theHold.Begin();
  if ( theHold.Holding() )
    theHold.Put(pRestoreObj);
  theHold.Accept(_T("Collapse one level"));

	m_pInterface->ForceCompleteRedraw(FALSE);
//	GetSystemSetting(SYSSET_CLEAR_UNDO);
//  SetSaveRequiredFlag(TRUE);
}

void Referee::CollapseToBase(void)
{
	INode *node;
  RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

	for (int i=0; i<m_pInterface->GetSelNodeCount(); i++) 
	{
		// Get a selected node
		node = m_pInterface->GetSelNode(i);

		// Eval the node's object (exclude WSMs)
		Object *oldObj = node->GetObjectRef();
		
		// Check for NULL
		if (!oldObj) continue;

		// Skip bones
		if (oldObj->ClassID()==Class_ID(BONE_CLASS_ID,0)) continue;		

		// RB 6/14/99: Skip system nodes too
		Control *tmCont = node->GetTMController();
		if (tmCont && GetMasterController(tmCont)) continue;

		Object *baseObj = oldObj->FindBaseObject();
		if (!baseObj)
			continue;
//		node->SetObjectRef(baseObj);		
    pRestoreObj->AddNode(oldObj, baseObj, node);
	}
  // perform collapse and register undo
  pRestoreObj->Execute();
  theHold.Begin();
  if ( theHold.Holding() )
    theHold.Put(pRestoreObj);
  theHold.Accept(_T("Collapse to base"));

	m_pInterface->ForceCompleteRedraw(FALSE);
//	GetSystemSetting(SYSSET_CLEAR_UNDO);
//	SetSaveRequiredFlag(TRUE);
}


void Referee::LinkToPickedObject(INode *pickedNode)
{
	INode *node;
	INodeTab flash;

	if (!pickedNode)
		return;
	
  RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

	// get new object to reference
	Object *linkObj = pickedNode->GetObjectRef();

	for (int i=0; i<m_pInterface->GetSelNodeCount(); i++) 
	{
		// Get a selected node
		node = m_pInterface->GetSelNode(i);
		flash.Append(1,&node,10);

		// Eval the node's object (exclude WSMs)
		Object *oldObj = node->GetObjectRef();
		
		// Check for NULL
		if (!oldObj) continue;

		// Skip bones
		if (oldObj->ClassID()==Class_ID(BONE_CLASS_ID,0)) continue;		

		// RB 6/14/99: Skip system nodes too
		Control *tmCont = node->GetTMController();
		if (tmCont && GetMasterController(tmCont)) continue;

		if (oldObj==linkObj) // make sure we are not already linked
			continue;

    pRestoreObj->AddNode(oldObj, linkObj, node, node->GetMtl(), pickedNode->GetMtl());
//		node->SetObjectRef(linkObj);		
//		Mtl *pickedMaterial = pickedNode->GetMtl();
//		node->SetMtl(pickedMaterial);
	}

  // perform link and register undo
  pRestoreObj->Execute();
  theHold.Begin();
  if ( theHold.Holding() )
    theHold.Put(pRestoreObj);
  theHold.Accept(_T("Link to picked object"));

	// Flash nodes
	m_pInterface->FlashNodes(&flash);

	m_pInterface->ForceCompleteRedraw(FALSE);
//	m_pInterface->RedrawViews(m_pInterface->GetTime());
//	GetSystemSetting(SYSSET_CLEAR_UNDO);
//	SetSaveRequiredFlag(TRUE);
}

void Referee::LinkToPickedBase(INode *pickedNode)
{
	INode *node;
	INodeTab flash;

	if (!pickedNode)
		return;
	
  RefereeRestoreObj* pRestoreObj = new RefereeRestoreObj();

	// get new object to reference
	Object *linkObj = pickedNode->GetObjectRef();
	if (!linkObj)
		return;
	Object *baseObj = linkObj->FindBaseObject();
	if (!baseObj)
		return;

	for (int i=0; i<m_pInterface->GetSelNodeCount(); i++) 
	{
		// Get a selected node
		node = m_pInterface->GetSelNode(i);
		flash.Append(1,&node,10);

		// Eval the node's object (exclude WSMs)
		Object *oldObj = node->GetObjectRef();
		
		// Check for NULL
		if (!oldObj) continue;

		// Skip bones
		if (oldObj->ClassID()==Class_ID(BONE_CLASS_ID,0)) continue;		

		// RB 6/14/99: Skip system nodes too
		Control *tmCont = node->GetTMController();
		if (tmCont && GetMasterController(tmCont)) continue;

		if (oldObj==baseObj) // make sure we are not already linked
			continue;
    pRestoreObj->AddNode(oldObj, linkObj, node, node->GetMtl(), pickedNode->GetMtl());
//		node->SetObjectRef(baseObj);		
//		Mtl *pickedMaterial = pickedNode->GetMtl();
//		node->SetMtl(pickedMaterial);
	}

  // perform link and register undo
  pRestoreObj->Execute();
  theHold.Begin();
  if ( theHold.Holding() )
    theHold.Put(pRestoreObj);
  theHold.Accept(_T("Link to picked base"));

	// Flash nodes
	m_pInterface->FlashNodes(&flash);

	m_pInterface->ForceCompleteRedraw(FALSE);
//	m_pInterface->RedrawViews(m_pInterface->GetTime());
//	GetSystemSetting(SYSSET_CLEAR_UNDO);
//	SetSaveRequiredFlag(TRUE);
}


