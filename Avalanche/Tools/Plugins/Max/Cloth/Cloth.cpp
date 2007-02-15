/****************************************************************************
 Cloth.cpp

 DESCRIPTION:	Appwizard generated plugin

 CREATED BY: Dave Astle

 HISTORY: 

 Copyright (c) 2002, Avalanche Software LC, All Rights Reserved.
****************************************************************************/

#include "ClothPCH.h"
#include "irollupsettings.h"
#include <string>

/** Constants **************************************************************/
const int VERSION_1_0 = 0x0001;  // initial release

const DWORD VERSION_CHUNK = 0x1000;
const DWORD CLOTH_VERSION = VERSION_1_0;
const DWORD ID_CHUNK      = 0x1010;
const DWORD CLOTH_CONNECTION_CHUNK = 0x1030;
const DWORD CLOTH_POINT_CHUNK      = 0x1040;


const Point3 POINTCOLOR_FRONT(0.75, 0.4, 0.0);
const Point3 POINTCOLOR_BACK(0.0, 0.75, 0.0);
const Point3 POINTCOLOR_INACTIVE(0.5, 0.5, 0.5);
const Point3 POINTCOLOR_ATTACHED(0.75, 0.75, 0.0);
const Point3 POINTCOLOR_SELECTED(0.25, 0.25, 0.25);
const Point3 POINTCOLOR_HIGHLIGHT(1.0, 1.0, 1.0);

const Point3 LINECOLOR_FRONTFRONT(0.0, 0.0, 0.75);
const Point3 LINECOLOR_FRONTBACK(0.5, 0.1, 0.75);
const Point3 LINECOLOR_BAD(0.75, 0.0, 0.0);
const Point3 LINECOLOR_SELECTED(0.25, 0.25, 0.25);
const Point3 LINECOLOR_HIGHLIGHT(1.0, 1.0, 1.0);


static ClothClassDesc ClothDesc;
ClassDesc2* GetClothDesc() { return &ClothDesc; }



// initialize static variables
SelectMode* Cloth::m_pSelectMode = NULL;
IObjParam *Cloth::m_pInterface  = NULL;
HWND Cloth::m_hSelection = NULL;
HWND Cloth::m_hPoints = NULL;
HWND Cloth::m_hConnections = NULL;


// registry key names
#define EXTEND_LENGTH_NAME "extendlength"
#define SELECT_PANEL_OPEN_NAME "selpanelopen"
#define POINT_PANEL_OPEN_NAME "pointpanelopen"
#define CONN_PANEL_OPEN_NAME "connpanelopen"
#define BRUSH_SIZE_NAME "brushsize"
#define LINK_DISTANCE_NAME "linkdistance"
#define FLTPNT_FRONT_NAME "fltpnt_front"
#define FLTPNT_BACK_NAME "fltpnt_back"
#define FLTPNT_ATTACHED_NAME "fltpnt_attached"
#define FLTPNT_INACTIVE_NAME "fltpnt_inactive"
#define FLTCNN_FRONT_NAME "fltcnn_front"
#define FLTCNN_FRONTBACK_NAME "fltcnn_frontback"
#define FLTCNN_INVALID_NAME "fltcnn_invalid"


GenSubObjType SOT_Points(0);
GenSubObjType SOT_Connections(1);

ISubObjType* Cloth::GetSubObjType(int i)
{
	static bool initialized = false;
	if(!initialized)
	{
		initialized = true;
		SOT_Points.SetName(_T("Points"));
		SOT_Connections.SetName(_T("Connections"));
	}

	switch(i)
	{
	case 0:
		return &SOT_Points;
	case 1:
		return &SOT_Connections;
	}
	return NULL;
}


/****************************************************************************
 CompMatrix()

 Gets the nodes transformation matrix at time t.
****************************************************************************/
Matrix3 CompMatrix(TimeValue t, INode *inode)
{
	Interval iv;
	Matrix3 tm(1);	
	if (inode) 
		tm = tm * inode->GetObjTMBeforeWSM(t,&iv);
	return tm;
}


/****************************************************************************
 SelectDlgProc()

 Callback for the selection rollup panel
****************************************************************************/
static BOOL CALLBACK SelectDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Cloth* pCloth = (Cloth*) GetWindowLong(hWnd, GWL_USERDATA);
	if (!pCloth && msg!=WM_INITDIALOG) return FALSE;

	switch (msg)
	{
	case WM_INITDIALOG:
		{
			pCloth = (Cloth*)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, lParam);

			// initialize custom controls
			pCloth->m_pBrushSizeEditBox = GetICustEdit(GetDlgItem(hWnd, IDC_BRUSH_SIZE));
			pCloth->m_pBrushSizeSpinner = SetupFloatSpinner(hWnd, IDC_BRUSH_SIZE_SPINNER, IDC_BRUSH_SIZE, 0.0, 100.0, 1.0);

			Registry reg(CLOTH_KEY);
			DWORD dist;

			if (reg.Get(BRUSH_SIZE_NAME, &dist))
			{
				pCloth->m_pBrushSizeSpinner->SetValue(dist/1000.0f, FALSE);
			}

			// set the filters based on registry settings
			DWORD filter;
			if (reg.Get(FLTPNT_FRONT_NAME, &filter))
				CheckDlgButton(hWnd, IDC_FLTPNT_FRONT, (filter == 1) ? BST_CHECKED : BST_UNCHECKED);
			else
				CheckDlgButton(hWnd, IDC_FLTPNT_FRONT, BST_CHECKED);

			if (reg.Get(FLTPNT_BACK_NAME, &filter))
				CheckDlgButton(hWnd, IDC_FLTPNT_BACK, (filter == 1) ? BST_CHECKED : BST_UNCHECKED);
			else
				CheckDlgButton(hWnd, IDC_FLTPNT_BACK, BST_CHECKED);

			if (reg.Get(FLTPNT_ATTACHED_NAME, &filter))
				CheckDlgButton(hWnd, IDC_FLTPNT_ATTACHED, (filter == 1) ? BST_CHECKED : BST_UNCHECKED);
			else
				CheckDlgButton(hWnd, IDC_FLTPNT_ATTACHED, BST_CHECKED);

			if (reg.Get(FLTPNT_INACTIVE_NAME, &filter))
				CheckDlgButton(hWnd, IDC_FLTPNT_INACTIVE, (filter == 1) ? BST_CHECKED : BST_UNCHECKED);
			else
				CheckDlgButton(hWnd, IDC_FLTPNT_INACTIVE, BST_CHECKED);

			if (reg.Get(FLTCNN_FRONT_NAME, &filter))
				CheckDlgButton(hWnd, IDC_FLTCNN_FRONT, (filter == 1) ? BST_CHECKED : BST_UNCHECKED);
			else
				CheckDlgButton(hWnd, IDC_FLTCNN_FRONT, BST_CHECKED);

			if (reg.Get(FLTCNN_FRONTBACK_NAME, &filter))
				CheckDlgButton(hWnd, IDC_FLTCNN_FRONTBACK, (filter == 1) ? BST_CHECKED : BST_UNCHECKED);
			else
				CheckDlgButton(hWnd, IDC_FLTCNN_FRONTBACK, BST_CHECKED);

			if (reg.Get(FLTCNN_INVALID_NAME, &filter))
				CheckDlgButton(hWnd, IDC_FLTCNN_INVALID, (filter == 1) ? BST_CHECKED : BST_UNCHECKED);
			else
				CheckDlgButton(hWnd, IDC_FLTCNN_INVALID, BST_CHECKED);

			// set up the toolbar
			pCloth->m_pSelToolbar = GetICustToolbar(GetDlgItem(hWnd, IDC_SEL_TOOLBAR));
			pCloth->m_pSelToolbar->SetBottomBorder(FALSE);
			pCloth->m_pSelToolbar->SetTopBorder(FALSE);
			pCloth->m_hSelImages = ImageList_Create(16, 15, TRUE, 6, 0);

			HBITMAP hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SEL_TOOLBAR));
			HBITMAP	hMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SEL_TOOLBAR_MASK));
			ImageList_Add(pCloth->m_hSelImages, hBitmap, hMask);
			DeleteObject(hBitmap);
			DeleteObject(hMask);

			pCloth->m_pSelToolbar->SetImage(pCloth->m_hSelImages);

			// Add the point selection button
			pCloth->m_pSelToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,
																	   0, 1, 0, 1, 16, 15, 
																	   22, 20, ID_POINT_SEL));
			// Add the one-to-many replace button:
			pCloth->m_pSelToolbar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,
																	   2, 3, 2, 3, 16, 15, 
																	   22, 20, ID_CONN_SEL));

			// Diable button highlighting.
			pCloth->m_pSelPointsButton = pCloth->m_pSelToolbar->GetICustButton(ID_POINT_SEL);
			pCloth->m_pSelPointsButton->SetCheckHighlight(FALSE);
			pCloth->m_pSelConnsButton = pCloth->m_pSelToolbar->GetICustButton(ID_CONN_SEL);
			pCloth->m_pSelConnsButton->SetCheckHighlight(FALSE);
		} break;

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case ID_CONN_SEL:
			case ID_POINT_SEL:
				{
					pCloth->BeginSelect(LOWORD(wParam));
				} break;
			}
		} break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		pCloth->m_pInterface->RollupMouseMessage(hWnd,msg,wParam,lParam); 
		break;

	default:
		return FALSE;
	}
	return TRUE;
}


/****************************************************************************
 PointDlgProc()

 Callback for the points rollup panel
****************************************************************************/
static BOOL CALLBACK PointDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Cloth* pCloth = (Cloth*) GetWindowLong(hWnd, GWL_USERDATA);
	if (!pCloth && msg!=WM_INITDIALOG) return FALSE;

	switch (msg)
	{
	case WM_INITDIALOG:
		{
			pCloth = (Cloth*)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, lParam);

			// initialize custom controls
			pCloth->m_pLinkDistEditBox = GetICustEdit(GetDlgItem(hWnd, IDC_LINK_DISTANCE));
			pCloth->m_pLinkDistSpinner = SetupIntSpinner(hWnd, IDC_LINKDIST_SPINNER, IDC_LINK_DISTANCE, 1, 100, 1);

			pCloth->m_pLinkButton = GetICustButton(GetDlgItem(hWnd, IDC_CMD_LINK));
			pCloth->m_pLinkButton->SetType(CBT_CHECK);
			pCloth->m_pLinkButton->SetHighlightColor(GREEN_WASH);

			Registry reg(CLOTH_KEY);
			DWORD dist;

			if (reg.Get(LINK_DISTANCE_NAME, &dist))
			{
				pCloth->m_pLinkDistSpinner->SetValue((int)dist, FALSE);
			}
		} break;

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_CMD_LINK:
				{
					pCloth->BeginSelect(LOWORD(wParam));
				} break;
			
			case IDC_CMD_LINK_EDGES:
				{
					pCloth->LinkEdges();
				} break;

			case IDC_POINTPROP_INACTIVE:
			case IDC_POINTPROP_FRONT:
			case IDC_POINTPROP_ATTACHED:
			case IDC_POINTPROP_BACK:
				pCloth->UpdatePointProperties(LOWORD(wParam));
				break;

			case IDC_B2F_CREATE:
				pCloth->CreateBackToFrontConnections();
				break;
			}
		} break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		pCloth->m_pInterface->RollupMouseMessage(hWnd,msg,wParam,lParam); 
		break;

	default:
		return FALSE;
	}
	return TRUE;
}


/****************************************************************************
 ConnDlgProc()

 Callback for the connections rollup panel
****************************************************************************/
static BOOL CALLBACK ConnDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Cloth* pCloth = (Cloth*) GetWindowLong(hWnd, GWL_USERDATA);
	if (!pCloth && msg!=WM_INITDIALOG) return FALSE;

	switch (msg)
	{
	case WM_INITDIALOG:
		{
			pCloth = (Cloth*)lParam;
			SetWindowLong(hWnd, GWL_USERDATA, lParam);
			
			// initialize custom controls
			pCloth->m_pExtendEditBox = GetICustEdit(GetDlgItem(hWnd, IDC_EXTEND));
			pCloth->m_pExtendSpinner = SetupFloatSpinner(hWnd, IDC_EXTEND_SPINNER, IDC_EXTEND, 0.0, 100.0, 0.0);

			Registry reg(CLOTH_KEY);
			DWORD dist;

			if (reg.Get(EXTEND_LENGTH_NAME, &dist))
			{
				pCloth->m_pExtendSpinner->SetValue((float)dist/1000.0f, FALSE);
			}
		} break;

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDC_CMD_DELETE_CONN:
				{
					pCloth->DeleteConnections(false);
				} break;

			case IDC_CMD_DELETE_INVALID:
				{
					pCloth->DeleteConnections(true);
				} break;
			}

		} break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		pCloth->m_pInterface->RollupMouseMessage(hWnd,msg,wParam,lParam); 
		break;

	case WM_CUSTEDIT_ENTER:
		{
			switch (wParam)
			{
			case IDC_EXTEND:
				pCloth->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
				pCloth->m_pInterface->RedrawViews(pCloth->m_pInterface->GetTime());
				break;
			}
		} break;

	case CC_SPINNER_CHANGE:
		{
			switch (LOWORD(wParam))
			{
			case IDC_EXTEND_SPINNER:
				pCloth->NotifyDependents(FOREVER, GEOM_CHANNEL, REFMSG_CHANGE);
				pCloth->m_pInterface->RedrawViews(pCloth->m_pInterface->GetTime());
				break;
			}
		} break;

	default:
		return FALSE;
	}
	return TRUE;
}



/****************************************************************************
 Cloth::Cloth()
****************************************************************************/
Cloth::Cloth()
{
	m_pExtendSpinner = NULL;
	m_pExtendEditBox = NULL;
	m_pLinkDistSpinner = NULL;
	m_pLinkDistEditBox = NULL;
	m_pBrushSizeSpinner = NULL;
	m_pBrushSizeEditBox = NULL;
	m_pSelToolbar = NULL;
	m_pSelPointsButton = NULL;
	m_pSelConnsButton = NULL;
}


/****************************************************************************
 Cloth::~Cloth()
****************************************************************************/
Cloth::~Cloth()
{
}


/****************************************************************************
 Cloth::LocalValidity()
****************************************************************************/
Interval Cloth::LocalValidity(TimeValue t)
{
	Interval iv = NEVER;
	iv.Set(t,t);
	return iv;
}


/****************************************************************************
 Cloth::NotifyPreCollapse()
****************************************************************************/
void Cloth::NotifyPreCollapse(INode *node, IDerivedObject *derObj, int index)
{
	//TODO:  Perform any Pre Stack Collapse methods here
}


/****************************************************************************
 Cloth::NotifyPostCollapse()
****************************************************************************/
void Cloth::NotifyPostCollapse(INode *node,Object *obj, IDerivedObject *derObj, int index)
{
	//TODO: Perform any Post Stack collapse methods here.
}


/****************************************************************************
 Cloth::ModifyObject()
****************************************************************************/
void Cloth::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
{
	// create the local mod data instance if it doesn't already exist
	if (!mc.localData)
	{
		// try to convert this to a triangle mesh
		TriObject* pTriObject;

		// handle TriObjects
		if (os->obj->IsSubClassOf(triObjectClassID))
		{
			pTriObject = (TriObject*)os->obj;
		}
		// Convert to a tri mesh if possible
		else if(os->obj->CanConvertToType(triObjectClassID))
		{
			pTriObject = (TriObject *)os->obj->ConvertToType(t, triObjectClassID);
			// Now stuff this into the pipeline!
			os->obj = pTriObject;
		}
		// otherwise, bail
		else
		{
			return;
		}
		
		ClothModData* pModData  = new ClothModData();
		mc.localData = pModData;

		// create a ClothPoint for every point in the mesh
		ClothPoint cp;
		for (int n = 0; n < pTriObject->NumPoints(); ++n)
			pModData->points.Append(1, &cp);

		NotifyDependents(FOREVER, OBJ_CHANNELS, REFMSG_CHANGE);
		Interval valid;
		valid.SetEmpty();
		os->obj->UpdateValidity(GEOM_CHAN_NUM, valid); // Have to do this to get it to evaluate

		return;
	}

	os->obj->UpdateValidity(GEOM_CHAN_NUM, LocalValidity(t));
}


/****************************************************************************
 Cloth::BeginEditParams()
****************************************************************************/
void Cloth::BeginEditParams(IObjParam *ip, ULONG flags, Animatable *prev)
{
	m_pInterface = ip;

	ICatRegistry* pCatReg = GetIRollupSettings()->GetCatReg();
	pCatReg->Load();
	static TCHAR titleBuf[80];
	int bufLen = 80;
	int nErrorCat = 0;
	int nCat = 0;
	Registry reg(CLOTH_KEY);
	DWORD panelState;

	// initialize each rollup panel
	if (!m_hSelection)
	{
		LoadString(hInstance, IDS_SELECTION, titleBuf, bufLen);
		nCat = pCatReg->GetCat(ClothDesc.SuperClassID(), ClothDesc.ClassID(), titleBuf, nErrorCat);
		reg.Get(SELECT_PANEL_OPEN_NAME, &panelState);
		m_hSelection = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_SELECTION), SelectDlgProc, titleBuf, (LPARAM)this, panelState, nCat);
		ip->RegisterDlgWnd(m_hSelection);
	}
	else
	{
		SetWindowLong(m_hSelection, GWL_USERDATA, (LONG)this);
	}

	if (!m_hPoints)
	{
		LoadString(hInstance, IDS_POINTS, titleBuf, bufLen);
		nCat = pCatReg->GetCat(ClothDesc.SuperClassID(), ClothDesc.ClassID(), titleBuf, nErrorCat);
		reg.Get(POINT_PANEL_OPEN_NAME, &panelState);
		m_hPoints = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_POINTS), PointDlgProc, titleBuf, (LPARAM)this, panelState, nCat);
		ip->RegisterDlgWnd(m_hPoints);
	}
	else
	{
		SetWindowLong(m_hPoints, GWL_USERDATA, (LONG)this);
	}

	if (!m_hConnections)
	{
		LoadString(hInstance, IDS_CONNECTIONS, titleBuf, bufLen);
		nCat = pCatReg->GetCat(ClothDesc.SuperClassID(), ClothDesc.ClassID(), titleBuf, nErrorCat);
		reg.Get(CONN_PANEL_OPEN_NAME, &panelState);
		m_hConnections = ip->AddRollupPage(hInstance, MAKEINTRESOURCE(IDD_CONNECTIONS), ConnDlgProc, titleBuf, (LPARAM)this, panelState, nCat);
		ip->RegisterDlgWnd(m_hConnections);
	}
	else
	{
		SetWindowLong(m_hConnections, GWL_USERDATA, (LONG)this);
	}

	// create a new select mode
	m_pSelectMode = new SelectMode(this,ip);
	m_selectionState = SEL_NONE;

	// grey out controls based on the current selection setting
	SetControlStates();

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);
}


/****************************************************************************
 Cloth::EndEditParams()
****************************************************************************/
void Cloth::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{
	// store all settings off to the registry
	Registry reg(CLOTH_KEY);
	DWORD dist = (DWORD) (1000 * m_pExtendSpinner->GetFVal());
	reg.Set(EXTEND_LENGTH_NAME, dist);

	dist = m_pLinkDistSpinner->GetIVal();
	reg.Set(LINK_DISTANCE_NAME, dist);

	dist = (DWORD)(1000 * m_pBrushSizeSpinner->GetFVal());
	reg.Set(BRUSH_SIZE_NAME, dist);

	DWORD filter = (IsDlgButtonChecked(m_hSelection, IDC_FLTPNT_FRONT)) == BST_CHECKED ? 1 : 0;
	reg.Set(FLTPNT_FRONT_NAME, filter);

	filter = (IsDlgButtonChecked(m_hSelection, IDC_FLTPNT_BACK)) == BST_CHECKED ? 1 : 0;
	reg.Set(FLTPNT_BACK_NAME, filter);

	filter = (IsDlgButtonChecked(m_hSelection, IDC_FLTPNT_ATTACHED)) == BST_CHECKED ? 1 : 0;
	reg.Set(FLTPNT_ATTACHED_NAME, filter);

	filter = (IsDlgButtonChecked(m_hSelection, IDC_FLTPNT_INACTIVE)) == BST_CHECKED ? 1 : 0;
	reg.Set(FLTPNT_INACTIVE_NAME, filter);

	filter = (IsDlgButtonChecked(m_hSelection, IDC_FLTCNN_FRONT)) == BST_CHECKED ? 1 : 0;
	reg.Set(FLTCNN_FRONT_NAME, filter);

	filter = (IsDlgButtonChecked(m_hSelection, IDC_FLTCNN_FRONTBACK)) == BST_CHECKED ? 1 : 0;
	reg.Set(FLTCNN_FRONTBACK_NAME, filter);

	filter = (IsDlgButtonChecked(m_hSelection, IDC_FLTCNN_INVALID)) == BST_CHECKED ? 1 : 0;
	reg.Set(FLTCNN_INVALID_NAME, filter);

	DWORD panelState;

	if (m_hSelection)
	{
		if (IsRollupPanelOpen(m_hSelection))
			panelState = 0;
		else
			panelState = APPENDROLL_CLOSED;

		reg.Set(SELECT_PANEL_OPEN_NAME, panelState);
	}

	if (m_hPoints)
	{
		if (IsRollupPanelOpen(m_hPoints))
			panelState = 0;
		else
			panelState = APPENDROLL_CLOSED;

		reg.Set(POINT_PANEL_OPEN_NAME, panelState);
	}

	if (m_hConnections)
	{
		if (IsRollupPanelOpen(m_hConnections))
			panelState = 0;
		else
			panelState = APPENDROLL_CLOSED;

		reg.Set(CONN_PANEL_OPEN_NAME, panelState);
	}

	ReleaseISpinner(m_pExtendSpinner);
	ReleaseICustEdit(m_pExtendEditBox);
	ReleaseISpinner(m_pLinkDistSpinner);
	ReleaseICustEdit(m_pLinkDistEditBox);
	ReleaseISpinner(m_pBrushSizeSpinner);
	ReleaseICustEdit(m_pBrushSizeEditBox);

	m_pExtendSpinner = NULL;
	m_pExtendEditBox = NULL;
	m_pLinkDistSpinner = NULL;
	m_pLinkDistEditBox = NULL;
	m_pBrushSizeSpinner = NULL;
	m_pBrushSizeEditBox = NULL;

	ip->UnRegisterDlgWnd(m_hSelection);
	ip->DeleteRollupPage(m_hSelection);
	m_hSelection = NULL;

	ip->UnRegisterDlgWnd(m_hPoints);
	ip->DeleteRollupPage(m_hPoints);
	m_hPoints = NULL;

	ip->UnRegisterDlgWnd(m_hConnections);
	ip->DeleteRollupPage(m_hConnections);
	m_hConnections = NULL;

	m_pInterface = NULL;

	TimeValue t = ip->GetTime();

	// NOTE: This flag must be cleared before sending the REFMSG_END_EDIT
	ClearAFlag(A_MOD_BEING_EDITED);

	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);

	ip->DeleteMode(m_pSelectMode);
	delete m_pSelectMode;
	m_pSelectMode = NULL;

	ip->ClearPickMode();
}


/****************************************************************************
 Cloth::Clone()
****************************************************************************/
RefTargetHandle Cloth::Clone(RemapDir& remap)
{
	Cloth* newmod = new Cloth();	
	//TODO: Add the cloning code here
	BaseClone(this, newmod, remap);
	return(newmod);
}


/****************************************************************************
 Cloth::NotifyRefChanged()
****************************************************************************/
RefResult Cloth::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message) 
{
	//TODO: Add code to handle the various reference changed messages
	return REF_SUCCEED;
}


/****************************************************************************
 Cloth::NotifyInputChanged()
****************************************************************************/
void Cloth::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
{
}


/****************************************************************************
 Cloth::HasUVW()
****************************************************************************/
BOOL Cloth::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	return TRUE; 
}


/****************************************************************************
 Cloth::SetGenUVW()
****************************************************************************/
void Cloth::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
}


/****************************************************************************
 Cloth::Load()
****************************************************************************/
IOResult Cloth::Load(ILoad *iLoad)
{
	//TODO: Add code to allow plugin to load its data
	
	return IO_OK;
}

/****************************************************************************
 Cloth::Save()
****************************************************************************/
IOResult Cloth::Save(ISave *iSave)
{
	//TODO: Add code to allow plugin to save its data
	
	return IO_OK;
}


/****************************************************************************
 Cloth::SaveLocalData()
****************************************************************************/
IOResult Cloth::SaveLocalData(ISave* iSave, LocalModData* pld)
{
	ClothModData* pModData = (ClothModData*) pld;
	ULONG nb;

	iSave->BeginChunk(VERSION_CHUNK);
	iSave->Write(&CLOTH_VERSION, sizeof(CLOTH_VERSION), &nb);
	iSave->EndChunk();

	// not used anymore, but kept to avoid version conflicts
	iSave->BeginChunk(ID_CHUNK);
	iSave->Write(&pModData->id, sizeof(int), &nb);
	iSave->EndChunk();

	int count = pModData->points.Count();

	iSave->BeginChunk(CLOTH_POINT_CHUNK);
	iSave->Write(&count, sizeof(count), &nb);

	for (int i = 0; i < count; ++i)
	{
		iSave->Write(&pModData->points[i], sizeof(ClothPoint), &nb);
	}
	iSave->EndChunk();

	count = pModData->connections.Count();
	if (count > 0)
	{
		iSave->BeginChunk(CLOTH_CONNECTION_CHUNK);
		iSave->Write(&count, sizeof(count), &nb);
		for (int i = 0; i < count; ++i)
		{
			iSave->Write(&pModData->connections[i], sizeof(ClothConnection), &nb);
		}
		iSave->EndChunk();
	}

	return IO_OK;
}


/****************************************************************************
 Cloth::LoadLocalData()
****************************************************************************/
IOResult Cloth::LoadLocalData(ILoad* iLoad, LocalModData** pld)
{
	IOResult	res;
	ULONG		nb;
	ClothModData* pModData = new ClothModData();
	*pld = pModData;

	DWORD version = 0;
	int count = 0;

	while (IO_OK == (res = iLoad->OpenChunk()))
	{
		switch(iLoad->CurChunkID())
		{
		case VERSION_CHUNK:
			{
				iLoad->Read(&version, sizeof(DWORD), &nb);
			} break;

		case CLOTH_CONNECTION_CHUNK:
			{
				iLoad->Read(&count, sizeof(int), &nb);
				for (int i = 0; i < count; ++i)
				{
					ClothConnection conn;
					iLoad->Read(&conn, sizeof(ClothConnection), &nb);
					pModData->connections.Append(1, &conn);
				}
			} break;

		// not used anymore, but kept to avoid version conflicts
		case ID_CHUNK:
			iLoad->Read(&pModData->id, sizeof(int), &nb);
			break;

		case CLOTH_POINT_CHUNK:
			//read in data
			iLoad->Read(&count, sizeof(count), &nb);

			for (int i = 0; i < count; ++i)
			{
				ClothPoint point;
				iLoad->Read(&point, sizeof(ClothPoint), &nb);
				pModData->points.Append(1, &point);
			}
			break;
		} // end switch

		iLoad->CloseChunk();
		if (res != IO_OK) return res;
	} // end while

	return IO_OK;
}


/****************************************************************************
 Cloth::BeginSelect()

 Begins or ends the selection process, based on which button was pressed.
****************************************************************************/
void Cloth::BeginSelect(int button)
{
	// if entering the select mode for the first time, save the old command mode
	if (m_pInterface && m_selectionState == SEL_NONE)
	{
		m_pOldCmdMode = m_pInterface->GetCommandMode();
	}

	switch (button)
	{
	case ID_POINT_SEL:
		{
			switch (m_selectionState)
			{
			case SEL_LINK:
				m_pLinkButton->SetCheck(FALSE);
				m_selectionState = SEL_POINTS;
				break;

			case SEL_CONNECTIONS:
				m_pSelConnsButton->SetCheck(FALSE);
				m_pSelPointsButton->SetCheck(TRUE);
				m_selectionState = SEL_POINTS;
				m_pInterface->SetSubObjectLevel(SEL_POINTS);
				break;

			case SEL_NONE:
				m_pSelPointsButton->SetCheck(TRUE);
				m_selectionState = SEL_POINTS;
				m_pInterface->SetSubObjectLevel(SEL_POINTS);
				break;

			case SEL_POINTS:
				m_pSelPointsButton->SetCheck(FALSE);
				m_pInterface->SetSubObjectLevel(SEL_NONE);
				m_selectionState = SEL_NONE;
				break;
			}
		} break;

	case IDC_CMD_LINK:
		{
			switch (m_selectionState)
			{
			case SEL_POINTS:
				m_pLinkButton->SetCheck(TRUE);
				m_selectionState = SEL_LINK;
				break;

			case SEL_LINK:
				m_pLinkButton->SetCheck(FALSE);
				m_selectionState = SEL_POINTS;
				break;
			}
		} break;

	case ID_CONN_SEL:
		{
			switch (m_selectionState)
			{
			case SEL_POINTS:
				m_pSelPointsButton->SetCheck(FALSE);
				m_pSelConnsButton->SetCheck(TRUE);
				m_pInterface->SetSubObjectLevel(SEL_CONNECTIONS);
				m_selectionState = SEL_CONNECTIONS;
				break;

			case SEL_LINK:
				m_pSelPointsButton->SetCheck(FALSE);
				m_pLinkButton->SetCheck(FALSE);
				m_pSelConnsButton->SetCheck(TRUE);
				m_pInterface->SetSubObjectLevel(SEL_CONNECTIONS);
				m_selectionState = SEL_CONNECTIONS;
				break;

			case SEL_NONE:
				m_pSelConnsButton->SetCheck(TRUE);
				m_pInterface->SetSubObjectLevel(SEL_CONNECTIONS);
				m_selectionState = SEL_CONNECTIONS;
				break;

			case SEL_CONNECTIONS:
				m_pSelConnsButton->SetCheck(FALSE);
				m_pInterface->SetSubObjectLevel(SEL_NONE);
				m_selectionState = SEL_NONE;
				break;
			}
		} break;

	case SEL_NONE:
		{
			switch (m_selectionState)
			{
			case SEL_POINTS:
				m_pSelPointsButton->SetCheck(FALSE);
				m_selectionState = SEL_NONE;
				break;

			case SEL_LINK:
				m_pSelPointsButton->SetCheck(FALSE);
				m_selectionState = SEL_NONE;
				break;

			case SEL_NONE:
				break;

			case SEL_CONNECTIONS:
				m_pSelConnsButton->SetCheck(FALSE);
				m_selectionState = SEL_NONE;
				break;
			}
		}
	}

	SetControlStates();

	if (m_pInterface)
	{
		// see if selection is complete and restore the old command mode if it is
		if (m_selectionState == SEL_NONE)
		{
			m_pInterface->SetCommandMode(m_pOldCmdMode);
		}
		// otherwise, if the select mode hasn't been changed, do so now
		else if (m_pSelectMode != m_pInterface->GetCommandMode())
		{
			m_pInterface->SetCommandMode(m_pSelectMode);
		}
	}
}


/****************************************************************************
 Cloth::Display()
****************************************************************************/
int Cloth::Display(TimeValue t, INode* iNode, ViewExp* pViewport, int flagst, ModContext* pModContext)
{
	// get the node display is being called for and see if it is selected
	if (iNode)
	{
		if (m_pInterface)
		{
			int nodeCount = m_pInterface->GetSelNodeCount();
			BOOL isNodeSelected = FALSE;
			for (int curNode = 0; curNode < nodeCount; ++curNode)
			{
				if (iNode == m_pInterface->GetSelNode(curNode))
				{
					isNodeSelected = TRUE;
					break;
				}
			}
			if (!isNodeSelected)
				return 0;
		}
	}

	// set up the graphics window
	GraphicsWindow* pGraphicsWnd = pViewport->getGW();
	Matrix3 tm = CompMatrix(t, iNode);

	ClothModData* pModData = (ClothModData *) pModContext->localData;

	if (pModData == NULL)
		return 0;

	// store the current render states, then change them to not z-test
	int savedLimits = pGraphicsWnd->getRndLimits();
	pGraphicsWnd->setRndLimits(savedLimits & ~GW_Z_BUFFER);
	pGraphicsWnd->setTransform(tm);

	ObjectState os = iNode->EvalWorldState(t);
	int curConnection;

	// mark all connections
	for (curConnection = 0; curConnection < pModData->connections.Count(); ++curConnection)
	{
		Point3 line[3]; // third 'empty' element required by Max
		line[0] = os.obj->GetPoint(pModData->connections[curConnection].v1);
		line[1] = os.obj->GetPoint(pModData->connections[curConnection].v2);

		// extend the lines to make them more easily seen
		AdjustLineExtents(line);

		// set the line color based on the connection type and whether the line is selected
		if (pModData->connections[curConnection].selected)
			pGraphicsWnd->setColor(LINE_COLOR,
			GetLineColor(pModData->connections[curConnection].GetConnectionType(pModData)) + LINECOLOR_SELECTED);
		else
			pGraphicsWnd->setColor(LINE_COLOR, GetLineColor(pModData->connections[curConnection].GetConnectionType(pModData)));

		// draw the line
		pGraphicsWnd->polyline(2, line, NULL, NULL, 0, NULL);
	}

	// mark all highlighted connections
	for (curConnection = 0; curConnection < pModData->hilightedConnections.Count(); ++curConnection)
	{
		Point3 line[3]; // third 'empty' element required by Max
		line[0] = os.obj->GetPoint(pModData->connections[pModData->hilightedConnections[curConnection]].v1);
		line[1] = os.obj->GetPoint(pModData->connections[pModData->hilightedConnections[curConnection]].v2);

		// extend the lines to make them more easily seen
		AdjustLineExtents(line);

		pGraphicsWnd->setColor(LINE_COLOR, LINECOLOR_HIGHLIGHT);
		pGraphicsWnd->polyline(2, line, NULL, NULL, 0, NULL);
	}

	Point3 point;
	int curPoint;

	// color each point based on its current state
	for (curPoint = 0; curPoint < os.obj->NumPoints(); ++curPoint)
	{
		point = os.obj->GetPoint(curPoint);
		pGraphicsWnd->setColor(LINE_COLOR, GetStateColor(pModData->points[curPoint].state));
		pGraphicsWnd->marker(&point, SM_DOT_MRKR);
	}

	// mark all currently selected points with a larger dot and brigher color
	for (curPoint = 0; curPoint < pModData->selectedPoints.Count(); ++curPoint)
	{
		point = os.obj->GetPoint(pModData->selectedPoints[curPoint]);
		pGraphicsWnd->setColor(LINE_COLOR,
			GetStateColor(pModData->points[pModData->selectedPoints[curPoint]].state) + POINTCOLOR_SELECTED);
		pGraphicsWnd->marker(&point, DOT_MRKR);
	}

	// mark the currenly highlighted points with a circle
	for (curPoint = 0; curPoint < pModData->hilightedPoints.Count(); ++curPoint)
	{
		point = os.obj->GetPoint(pModData->hilightedPoints[curPoint]);
		pGraphicsWnd->setColor(LINE_COLOR, POINTCOLOR_HIGHLIGHT);
		pGraphicsWnd->marker(&point, CIRCLE_MRKR);
	}

	// restore the old render states
	pGraphicsWnd->setRndLimits(savedLimits);

	if (m_selectionState != SEL_NONE && pModData->isHit) 
	{
		//draw 3d cursor
		Point3 x(1.0f,0.0f,0.0f),y(0.0f,1.0f,0.0f),z(0.0f,0.0f,1.0f);
		pGraphicsWnd->setColor(LINE_COLOR, 1.0f,1.0f,0.0f);
		float radius = m_pBrushSizeSpinner->GetFVal();

		DrawCrossSectionNoMarkers(pModData->hitPoint, x, radius, pGraphicsWnd);
		DrawCrossSectionNoMarkers(pModData->hitPoint, y, radius, pGraphicsWnd);
		DrawCrossSectionNoMarkers(pModData->hitPoint, z, radius, pGraphicsWnd);
	}

	return 0;
}


/****************************************************************************
 Cloth::CreateConnection()

 Creates a new connection between v1 and v2 iff one doesn't already exist.
****************************************************************************/
void Cloth::CreateConnection(ClothModData* pModData, int v1, int v2)
{
	ClothConnection conn;
	conn.selected = false;

	if (v1 < v2)
	{
		conn.v1 = v1;
		conn.v2 = v2;
	}
	else
	{
		conn.v1 = v2;
		conn.v2 = v1;
	}

	// check to see if the connection already exists, and if it does, don't
	// create a new one
	for (int i = 0; i < pModData->connections.Count(); ++i)
	{
		if (pModData->connections[i].v1 == conn.v1 && pModData->connections[i].v2 == conn.v2)
			return;
	}

	pModData->connections.Append(1, &conn);
}


/****************************************************************************
 Cloth::CreateConnections()

 Called when "Link to" is selected. Connects all the currently selected points
 to the highlighted point. No validation is performed.
****************************************************************************/
void Cloth::CreateConnections(ClothModData* pModData)
{
	if (pModData->selectedPoints.Count() == 0 || pModData->hilightedPoints.Count() == 0)
		return;

	int v1 = pModData->hilightedPoints[0];

	for (int i = 0; i < pModData->selectedPoints.Count(); ++i)
	{
		CreateConnection(pModData, v1, pModData->selectedPoints[i]);
	}

	pModData->selectedPoints.ZeroCount();
}


/****************************************************************************
 Cloth::AdjustLineExtents()

 Extends the line to make it more easily viewable in the interface. Does not
 modify the data at all.
****************************************************************************/
void Cloth::AdjustLineExtents(Point3 line[3])
{
	float length = m_pExtendSpinner->GetFVal();

	Point3 vec = line[1] - line[0];
	vec = vec.Normalize();

	line[1] += vec * length;
	line[0] += -vec * length;
}


/****************************************************************************
 Cloth::GetMesh()

 Gets a mesh from an object.
****************************************************************************/
bool Cloth::GetMesh(Object* pObject, Mesh& mesh)
{
	if (!pObject)
		return false;

	// don't process this node if it's not a mesh
	if (pObject->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return false;

	// handle TriObjects
	if (pObject->IsSubClassOf(triObjectClassID))
	{
		TriObject* pTriObj = (TriObject*)pObject;
		mesh = pTriObj->GetMesh();
	}
	// handle PolyObjects
	else if (pObject->IsSubClassOf(polyObjectClassID))
	{
		PolyObject* pPolyObj = (PolyObject*)pObject;
		MNMesh& polyMesh = pPolyObj->GetMesh();
		polyMesh.OutToTri(mesh);
	}
	// Convert to a tri mesh if possible
	else if(pObject->CanConvertToType(triObjectClassID))
	{
		TriObject* pTriObj = (TriObject *)pObject->ConvertToType(m_pInterface->GetTime(), triObjectClassID);
		pObject = pTriObj;
		mesh = pTriObj->GetMesh();
	}
	else
	{
		return false;
	}
	return true;
}


/****************************************************************************
 Cloth::LinkEdges()

 Called when the "link edges" button is selected.
****************************************************************************/
void Cloth::LinkEdges()
{
	if (!m_pInterface)
		return;

	// get all mod contexts
	ModContextList mcList;
	INodeTab nodes;

	m_pInterface->GetModContexts(mcList, nodes);
	int numObjects = mcList.Count();

	for (int k = 0; k < numObjects; ++k) 
	{
		ClothModData* pModData = (ClothModData*) mcList[k]->localData;

		if (pModData)
		{
			ObjectState os = nodes[k]->EvalWorldState(m_pInterface->GetTime());
			Object* pObject = os.obj;
			Mesh mesh;

			if (GetMesh(pObject, mesh))
			{
				for (int j = 0; j < pModData->selectedPoints.Count(); ++j)
				{
					bool* alreadyChecked = new bool[mesh.getNumVerts()];
					memset(alreadyChecked, 0, mesh.getNumVerts());
					AdjEdgeList adjacentEdges(mesh);
					RecursivelyAddConnections(pModData, pModData->selectedPoints[j], pModData->selectedPoints[j],
													  m_pLinkDistSpinner->GetIVal(), adjacentEdges, alreadyChecked);
					delete [] alreadyChecked;
				}
			}
		} // end if
	} // end for
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	m_pInterface->RedrawViews(m_pInterface->GetTime());
}


/****************************************************************************
 Cloth::RecursivelyAddConnections()

 Called as part of the link edges process
****************************************************************************/
void Cloth::RecursivelyAddConnections(ClothModData* pModData, int centralPoint, int edgePoint, int level, const AdjEdgeList& adjacentEdges, bool* alreadyChecked)
{
	if (level > 0)
	{
		alreadyChecked[edgePoint] = true;
		DWORDTab edgeIndices = adjacentEdges.list[edgePoint];

		for (int i = 0; i < edgeIndices.Count(); ++i)
		{
			MEdge currentEdge = adjacentEdges.edges[edgeIndices[i]];
			int v2;
			if (currentEdge.v[0] == edgePoint)
				v2 = currentEdge.v[1];
			else
				v2 = currentEdge.v[0];

			// only create connections between two fronts or a front and attached
			if ((pModData->points[centralPoint].state == POINTSTATE_FRONT && pModData->points[v2].state == POINTSTATE_FRONT) ||
				(pModData->points[centralPoint].state == POINTSTATE_FRONT && pModData->points[v2].state == POINTSTATE_ATTACHED) ||
				(pModData->points[centralPoint].state == POINTSTATE_ATTACHED && pModData->points[v2].state == POINTSTATE_FRONT))
			{
				CreateConnection(pModData, centralPoint, v2);
			}

			if (!alreadyChecked[v2])
				RecursivelyAddConnections(pModData, centralPoint, v2, level - 1, adjacentEdges, alreadyChecked);
		}
	}
}


/****************************************************************************
 Cloth::ClearSelected()

 Clears the current selection, including highlights
****************************************************************************/
void Cloth::ClearSelected()
{
	if (!m_pInterface)
		return;

	ModContextList mcList;
	INodeTab nodes;

	m_pInterface->GetModContexts(mcList, nodes);
	int numObjects = mcList.Count();

	for (int k = 0; k < numObjects; ++k) 
	{
		ClothModData* pModData = (ClothModData*) mcList[k]->localData;
		if (pModData)
		{
			for (int i = 0; i < pModData->connections.Count(); ++i)
			{
				pModData->connections[i].selected = false;
			}
			pModData->selectedPoints.ZeroCount();
			pModData->hilightedConnections.ZeroCount();
			pModData->hilightedConnections.Shrink();
			pModData->hilightedPoints.ZeroCount();
			pModData->hilightedPoints.Shrink();
		} // end if
	} // end for
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	m_pInterface->RedrawViews(m_pInterface->GetTime());
}


/****************************************************************************
 Cloth::DeleteConnections()

 Removes connections. If onlyDeleteInvalid is true, then all invalid
 connections are deleted. Otherwise, all currently selected connections are
 deleted.
****************************************************************************/
void Cloth::DeleteConnections(bool onlyDeleteInvalid)
{
	if (!m_pInterface)
		return;

	ModContextList mcList;
	INodeTab nodes;

	m_pInterface->GetModContexts(mcList, nodes);
	int numObjects = mcList.Count();

	for (int k = 0; k < numObjects; ++k) 
	{
		ClothModData* pModData = (ClothModData*) mcList[k]->localData;
		if (pModData)
		{
			for (int i = pModData->connections.Count() - 1; i >= 0 ; --i)
			{
				if (onlyDeleteInvalid)
				{
					if (pModData->connections[i].GetConnectionType(pModData) == CT_BAD)
						pModData->connections.Delete(i, 1);
				}
				else if (pModData->connections[i].selected)
					pModData->connections.Delete(i, 1);
			}
		} // end if
	} // end for
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	m_pInterface->RedrawViews(m_pInterface->GetTime());
}


/****************************************************************************
 Cloth::DisplayProperties()

 Updates the panels displaying the currently selected point and connection
 properties.
****************************************************************************/
void Cloth::DisplayProperties()
{
	if (!m_pInterface)
		return;

	ModContextList mcList;
	INodeTab nodes;

	m_pInterface->GetModContexts(mcList, nodes);
	int numObjects = mcList.Count();

	for (int k = 0; k < numObjects; ++k) 
	{
		ClothModData* pModData = (ClothModData*) mcList[k]->localData;
		if (pModData)
		{
			bool atLeastOneSelected = false;
			bool arePropertiesEqual = true;
			ClothPoint point;

			// check point properties
			if (pModData->selectedPoints.Count())
			{
				atLeastOneSelected = true;
				point = pModData->points[pModData->selectedPoints[0]];
				for (int i = 1; i < pModData->selectedPoints.Count(); ++i)
				{
					if (pModData->points[pModData->selectedPoints[i]].state != point.state)
					{
						arePropertiesEqual = false;
					}
				}
			}

			// update the point properties display
			if (atLeastOneSelected && arePropertiesEqual)
			{
				int buttonID;
				switch (point.state)
				{
				case POINTSTATE_INACTIVE:
					buttonID = IDC_POINTPROP_INACTIVE;
					break;
				case POINTSTATE_FRONT:
					buttonID = IDC_POINTPROP_FRONT;
					break;
				case POINTSTATE_ATTACHED:
					buttonID = IDC_POINTPROP_ATTACHED;
					break;
				case POINTSTATE_BACK:
					buttonID = IDC_POINTPROP_BACK;
					break;
				}
				CheckRadioButton(m_hPoints, IDC_POINTPROP_INACTIVE, IDC_POINTPROP_BACK, buttonID);
			}
			else
			{
				// if nothing is selected, or if the items in the selection have
				// different properties, don't show anything
				CheckDlgButton(m_hPoints, IDC_POINTPROP_INACTIVE, BST_UNCHECKED);
				CheckDlgButton(m_hPoints, IDC_POINTPROP_FRONT, BST_UNCHECKED);
				CheckDlgButton(m_hPoints, IDC_POINTPROP_ATTACHED, BST_UNCHECKED);
				CheckDlgButton(m_hPoints, IDC_POINTPROP_BACK, BST_UNCHECKED);
			}
		} // end if
	} // end for
}


/****************************************************************************
 Cloth::UpdatePointProperties()

 Changes the properties of the currently selected points
****************************************************************************/
void Cloth::UpdatePointProperties(int button)
{
	if (!m_pInterface)
		return;

	// determine which radio button was selected
	PointState newState = POINTSTATE_INACTIVE;
	switch(button)
	{
	case IDC_POINTPROP_FRONT:
		newState = POINTSTATE_FRONT;
		break;
	case IDC_POINTPROP_ATTACHED:
		newState = POINTSTATE_ATTACHED;
		break;
	case IDC_POINTPROP_BACK:
		newState = POINTSTATE_BACK;
		break;
	default:
		newState = POINTSTATE_INACTIVE;
	}

	// go through all currently selected points, and update them to the new state
	ModContextList mcList;
	INodeTab nodes;

	m_pInterface->GetModContexts(mcList, nodes);
	int numObjects = mcList.Count();

	for (int k = 0; k < numObjects; ++k) 
	{
		ClothModData* pModData = (ClothModData*) mcList[k]->localData;
		if (pModData)
		{
			for (int i = 0; i < pModData->selectedPoints.Count(); ++i)
			{
				pModData->points[pModData->selectedPoints[i]].state = newState;
			}
		} // end if
	} // end for

	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	m_pInterface->RedrawViews(m_pInterface->GetTime());
}


/****************************************************************************
 Cloth::GetStateColor()

 Get the color that should be assigned to a point based on it's state.
****************************************************************************/
Point3 Cloth::GetStateColor(PointState state)
{
	Point3 color;

	switch(state)
	{
	case POINTSTATE_INACTIVE:
		color = POINTCOLOR_INACTIVE;
		break;
	case POINTSTATE_FRONT:
		color = POINTCOLOR_FRONT;
		break;
	case POINTSTATE_BACK:
		color = POINTCOLOR_BACK;
		break;
	case POINTSTATE_ATTACHED:
		color = POINTCOLOR_ATTACHED;
		break;
	}

	return color;
}


/****************************************************************************
 Cloth::GetLineColor()

 This method will return the appropriate color for a line based on the 
 connection type.
****************************************************************************/
Point3 Cloth::GetLineColor(ConnectionType type)
{
	Point3 color;

	switch (type)
	{
	case CT_FRONTFRONT:
		color = LINECOLOR_FRONTFRONT;
		break;

	case CT_FRONTBACK:
		color = LINECOLOR_FRONTBACK;
		break;

	default:
		color = LINECOLOR_BAD;
		break;
	}
	return color;
}


/****************************************************************************
 Cloth::CreateBackToFrontConnections()

 Automatically connects every back point to the front point closest to its
 normal vector.
****************************************************************************/
void Cloth::CreateBackToFrontConnections()
{
	ModContextList mcList;
	INodeTab nodes;

	m_pInterface->GetModContexts(mcList, nodes);
	int numObjects = mcList.Count();

	// step through all objects this modifier applies to
	for (int k = 0; k < numObjects; ++k) 
	{
		// get the local mod data
		ClothModData* pModData = (ClothModData*) mcList[k]->localData;
		ObjectState os = nodes[k]->EvalWorldState(m_pInterface->GetTime());

		if (pModData && os.obj)
		{
			// cache the point normals if needed
			if (pModData->pointNormals.Count() == 0)
			{
				CachePointNormals(pModData, os.obj);
			}

			// first, clear out any existing back to front connections
			for (int curConn = pModData->connections.Count() - 1; curConn >= 0; --curConn)
			{
				if (pModData->connections[curConn].GetConnectionType(pModData) == CT_FRONTBACK)
					pModData->connections.Delete(curConn, 1);
			}

			// loop through all points, and create connections for the back points
			for (int i = 0; i < pModData->points.Count(); ++i)
			{
				if (POINTSTATE_BACK == pModData->points[i].state)
				{
					int frontPoint = FindClosestFrontPoint(pModData, pModData->pointNormals[i], i, os.obj);
					if (frontPoint != -1)
						CreateConnection(pModData, i, frontPoint);
				}
			}
		} // end if
	} // end for

	// update the display
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	m_pInterface->RedrawViews(m_pInterface->GetTime());
}


/****************************************************************************
 Cloth::FindClosestFrontPoint()

 Finds the front point closest to backPoint based on the distance to the
 normal at the back point
****************************************************************************/
int Cloth::FindClosestFrontPoint(ClothModData* pModData, Point3 normal, int backPoint, Object* pObject)
{
	int closestFront = -1;

	float closestDistance = 1000000.0f;
	Point3 lineBegin = pObject->GetPoint(backPoint);
	Point3 lineEnd = lineBegin - normal;

	for (int i = 0; i < pModData->points.Count(); ++i)
	{
		if (POINTSTATE_FRONT == pModData->points[i].state)
		{
			Point3 frontPoint = pObject->GetPoint(i);
			float dist = SquaredDistanceToLine(lineBegin, lineEnd, frontPoint, false);

			if (dist <= closestDistance && NumConnectedBacks(pModData, i) == 0)
			{
				closestDistance = dist;
				closestFront = i;
			}
		}
	}

	return closestFront;
}


/****************************************************************************
 Cloth::NumConnectedBacks()

 Returns the number of back points connected to a given front (more than one
 is a problem).
****************************************************************************/
int Cloth::NumConnectedBacks(ClothModData* pModData, int frontPointID)
{
	int numConnections = 0;
	for (int curConn = 0; curConn < pModData->connections.Count(); ++curConn)
	{
		ClothConnection conn = pModData->connections[curConn];
		if ((conn.v1 == frontPointID || conn.v2 == frontPointID) &&
			 (conn.GetConnectionType(pModData) == CT_FRONTBACK))
		{
			 numConnections++;
		}
	}

	return numConnections;
}


/****************************************************************************
 Cloth::GetVertexNormal

 Determines the normal of the given vertex.
****************************************************************************/
Point3 Cloth::GetNormal(Mesh *mesh, int faceNo, RVertex* rv)
{
  Face &face = mesh->faces[faceNo];
  DWORD smGroup = face.smGroup;
  int numNormals;
  Point3 vertexNormal;

  // Is normal specified?
  // SPCIFIED is not currently used, but may be used in future versions.
  if (rv->rFlags & SPECIFIED_NORMAL) 
  {
    vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
	{
    // If there is only one vertex is found in the rn member.
    if (numNormals == 1) 
    {
      vertexNormal = rv->rn.getNormal();
    }
    else 
    {
      // If two or more vertices are there you need to step through them
      // and find the vertex with the same smoothing group as the current face.
      // You will find multiple normals in the ern member.
      for (int i = 0; i < numNormals; i++) 
      {
        if (rv->ern[i].getSmGroup() & smGroup) 
        {
          vertexNormal = rv->ern[i].getNormal();
        }
      }
    }
  }
  else 
  {
    // Get the normal from the Face if no smoothing groups are there
    vertexNormal = mesh->getFaceNormal(faceNo);
  }

  return vertexNormal;
} // end Cloth::GetNormal


/****************************************************************************
 Cloth::CachePointNormals()

 Determines the normal for each point in the mesh and caches it for future
 reference.
****************************************************************************/
void Cloth::CachePointNormals(ClothModData* pModData, Object* pObject)
{
  if (!pObject || !pModData)
    return;

  Mesh mesh;

  GetMesh(pObject, mesh);

  mesh.buildNormals();

  int numFaces = mesh.getNumFaces();

  pModData->pointNormals.SetCount(pModData->points.Count());

  // even though this plugin doesn't deal with faces at all, we need to step
  // through the faces to get the correct normals. Each point may get processed
  // more than once, and the normals calulated at each face will possibly be
  // different, but for now, the last one calculated will be the one used.
  for (int i = 0; i < numFaces; ++i)
  {
    int     vert;
    Point3  vertexNormal;
    Face&   face = mesh.faces[i];

    vert = face.getVert(0);
    vertexNormal = GetNormal(&mesh, i, mesh.getRVertPtr(vert));
    pModData->pointNormals[vert] = vertexNormal;

    vert = face.getVert(1);
    vertexNormal = GetNormal(&mesh, i, mesh.getRVertPtr(vert));
    pModData->pointNormals[vert] = vertexNormal;

    vert = face.getVert(2);
    vertexNormal = GetNormal(&mesh, i, mesh.getRVertPtr(vert));
    pModData->pointNormals[vert] = vertexNormal;
  }
}


/****************************************************************************
 Cloth::SetControlStates()

 Enables/disables controls based on the current selection state.
****************************************************************************/
void Cloth::SetControlStates()
{
	switch (m_selectionState)
	{
	case SEL_NONE:
		{
			// selection panel
			m_pBrushSizeSpinner->Disable();
			m_pBrushSizeEditBox->Disable();
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_FRONT), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_BACK), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_ATTACHED), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_INACTIVE), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_FRONT), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_FRONTBACK), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_INVALID), FALSE);

			// Points panel
			m_pLinkButton->Disable();
			m_pLinkDistSpinner->Disable();
			m_pLinkDistEditBox->Disable();
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_FRONT), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_BACK), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_ATTACHED), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_INACTIVE), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_CMD_LINK_EDGES), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_B2F_CREATE), FALSE);

			// Connections panel
			EnableWindow(GetDlgItem(m_hConnections, IDC_CMD_DELETE_CONN), FALSE);
			EnableWindow(GetDlgItem(m_hConnections, IDC_CMD_DELETE_INVALID), FALSE);
		} break;

	case SEL_POINTS:
	case SEL_LINK:
		{
			// selection panel
			m_pBrushSizeSpinner->Enable();
			m_pBrushSizeEditBox->Enable();
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_FRONT), TRUE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_BACK), TRUE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_ATTACHED), TRUE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_INACTIVE), TRUE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_FRONT), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_FRONTBACK), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_INVALID), FALSE);

			// Points panel
			m_pLinkButton->Enable();
			m_pLinkDistSpinner->Enable();
			m_pLinkDistEditBox->Enable();
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_FRONT), TRUE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_BACK), TRUE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_ATTACHED), TRUE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_INACTIVE), TRUE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_CMD_LINK_EDGES), TRUE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_B2F_CREATE), TRUE);

			// Connections panel
			EnableWindow(GetDlgItem(m_hConnections, IDC_CMD_DELETE_CONN), FALSE);
			EnableWindow(GetDlgItem(m_hConnections, IDC_CMD_DELETE_INVALID), FALSE);

		} break;

	case SEL_CONNECTIONS:
		{
			// selection panel
			m_pBrushSizeSpinner->Enable();
			m_pBrushSizeEditBox->Enable();
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_FRONT), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_BACK), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_ATTACHED), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTPNT_INACTIVE), FALSE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_FRONT), TRUE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_FRONTBACK), TRUE);
			EnableWindow(GetDlgItem(m_hSelection, IDC_FLTCNN_INVALID), TRUE);

			// Points panel
			m_pLinkButton->Disable();
			m_pLinkDistSpinner->Disable();
			m_pLinkDistEditBox->Disable();
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_FRONT), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_BACK), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_ATTACHED), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_POINTPROP_INACTIVE), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_CMD_LINK_EDGES), FALSE);
			EnableWindow(GetDlgItem(m_hPoints, IDC_B2F_CREATE), FALSE);

			// Connections panel
			EnableWindow(GetDlgItem(m_hConnections, IDC_CMD_DELETE_CONN), TRUE);
			EnableWindow(GetDlgItem(m_hConnections, IDC_CMD_DELETE_INVALID), TRUE);
		} break;

	}
}


/****************************************************************************
 Cloth::IsPointFiltered()

 Returns true if the point is not included in the currently enabled filters
****************************************************************************/
bool Cloth::IsPointFiltered(ClothModData* pModData, int pointID)
{
	ClothPoint point = pModData->points[pointID];

	if ((point.state == POINTSTATE_FRONT && IsDlgButtonChecked(m_hSelection, IDC_FLTPNT_FRONT) == BST_CHECKED) ||
		 (point.state == POINTSTATE_BACK && IsDlgButtonChecked(m_hSelection, IDC_FLTPNT_BACK) == BST_CHECKED) ||
		 (point.state == POINTSTATE_ATTACHED && IsDlgButtonChecked(m_hSelection, IDC_FLTPNT_ATTACHED) == BST_CHECKED) ||
		 (point.state == POINTSTATE_INACTIVE && IsDlgButtonChecked(m_hSelection, IDC_FLTPNT_INACTIVE) == BST_CHECKED))
		 return false;
	else
		return true;
}


/****************************************************************************
 Cloth::IsConnectionFiltered()

 Returns true if the connection is not included in the currently enabled
 filters
****************************************************************************/
bool Cloth::IsConnectionFiltered(ClothModData* pModData, int connID)
{
	ClothConnection conn = pModData->connections[connID];
	ConnectionType type = conn.GetConnectionType(pModData);

	if ((type == CT_FRONTFRONT && IsDlgButtonChecked(m_hSelection, IDC_FLTCNN_FRONT) == BST_CHECKED) ||
		 (type == CT_FRONTBACK && IsDlgButtonChecked(m_hSelection, IDC_FLTCNN_FRONTBACK) == BST_CHECKED) ||
		 (type == CT_BAD && IsDlgButtonChecked(m_hSelection, IDC_FLTCNN_INVALID) == BST_CHECKED))
		 return false;
	else
		return true;
}

#define NNUM_SEGS	16

/****************************************************************************
 Cloth::DrawCrossSectionNoMarkers()

 Draw a 3D paintbrush for selection. Taken from the Flex modifier.
****************************************************************************/
void Cloth::DrawCrossSectionNoMarkers(Point3 a, Point3 align, float length, GraphicsWindow *gw)
{
	Point3 plist[NNUM_SEGS+1];
	Point3 mka,mkb,mkc,mkd;

	align = Normalize(align);
	{
		int ct = 0;
		float angle = TWOPI/float(NNUM_SEGS) ;
		Matrix3 rtm = RotAngleAxisMatrix(align, angle);
		Point3 p(0.0f,0.0f,0.0f);
		if (align.x == 1.0f)
		{
			p.z = length;
		}
		else if (align.y == 1.0f)
		{
			p.x = length;
		}
		else if (align.z == 1.0f)
		{
			p.y = length;
		}
		else if (align.x == -1.0f)
		{
			p.z = -length;
		}
		else if (align.y == -1.0f)
		{
			p.x = -length;
		}
		else if (align.z == -1.0f)
		{
			p.y = -length;
		}
		else 
		{
			p = Normalize(align^Point3(1.0f,0.0f,0.0f))*length;
		}

		for (int i=0; i<NNUM_SEGS; i++)
		{
			p = p * rtm;
			plist[ct++] = p;
		}

		p = p * rtm;
		plist[ct++] = p;


		for (i=0; i<NNUM_SEGS+1; i++) 
		{
			plist[i].x += a.x;
			plist[i].y += a.y;
			plist[i].z += a.z;
		}
	}
	mka = plist[15];
	mkb = plist[3];
	mkc = plist[7];
	mkd = plist[11];

	gw->polyline(NNUM_SEGS+1, plist, NULL, NULL, 0);
}


/****************************************************************************
 Cloth::SelectAll()

 Called when Select All is chosen from the Max Edit menu.
****************************************************************************/
void Cloth::SelectAll(int selLevel)
{
	switch (m_selectionState)
	{
	// select all points
	case SEL_POINTS:
		{
			ModContextList mcList;
			INodeTab nodes;

			m_pInterface->GetModContexts(mcList, nodes);
			int numObjects = mcList.Count();

			// step through all objects this modifier applies to
			for (int curObject = 0; curObject < numObjects; ++curObject) 
			{
				// get the local mod data
				ClothModData* pModData = (ClothModData*) mcList[curObject]->localData;
				ObjectState os = nodes[curObject]->EvalWorldState(m_pInterface->GetTime());

				if (pModData && os.obj)
				{
					// clear all previously selected points
					pModData->selectedPoints.ZeroCount();

					// select points based on the current filter
					for (int curPoint = 0; curPoint < pModData->points.Count(); ++curPoint)
					{
						if (!IsPointFiltered(pModData, curPoint))
							pModData->selectedPoints.Append(1, &curPoint);
					}
				} // end if
			} // end for
		} break;

	case SEL_CONNECTIONS:
		{
			ModContextList mcList;
			INodeTab nodes;

			m_pInterface->GetModContexts(mcList, nodes);
			int numObjects = mcList.Count();

			// step through all objects this modifier applies to
			for (int curObject = 0; curObject < numObjects; ++curObject) 
			{
				// get the local mod data
				ClothModData* pModData = (ClothModData*) mcList[curObject]->localData;
				ObjectState os = nodes[curObject]->EvalWorldState(m_pInterface->GetTime());

				if (pModData && os.obj)
				{
					for (int curConn = 0; curConn < pModData->connections.Count(); ++curConn)
					{
						if (!IsConnectionFiltered(pModData, curConn))
							pModData->connections[curConn].selected = true;
					}
				} // end if
			} // end for
		} break;
	}
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	m_pInterface->RedrawViews(m_pInterface->GetTime());
}


/****************************************************************************
 Cloth::ActivateSubobjSel()

 Called when a change is made to the subobject selection menu.
****************************************************************************/
void Cloth::ActivateSubobjSel(int level, XFormModes& modes)
{
	ModContextList mcList;
	INodeTab nodes;
	int old = m_selectionState;

	if (!m_pInterface)
		return;

	switch (level)
	{
	// object selection level
	case 0:
		switch (m_selectionState)
		{
		case SEL_NONE:
			// do nothing
			break;

		case SEL_POINTS:
		case SEL_LINK:
		case SEL_CONNECTIONS:
			// we were in a select mode, and now we're not, so update
			BeginSelect(SEL_NONE);
			break;
		}
		break;

	// select points
	case 1:
		switch (m_selectionState)
		{
		case SEL_NONE:
		case SEL_CONNECTIONS:
			BeginSelect(ID_POINT_SEL);
			break;

		case SEL_POINTS:
		case SEL_LINK:
			// do nothing
			break;
		}
		break;

	// select connections
	case 2:
		switch (m_selectionState)
		{
		case SEL_NONE:
		case SEL_POINTS:
		case SEL_LINK:
			BeginSelect(ID_CONN_SEL);
			break;

		case SEL_CONNECTIONS:
			// do nothing
			break;
		}
		break;
	}

/*
	// if we're in sub object selection then we want del key input.
	if (selLevel==0 && level!=0)
	{
		delEvent.SetEditMeshMod(this);
		ip->RegisterDeleteUser(&delEvent);
	}
	if (selLevel!=0 && level==0)
	{
		ip->UnRegisterDeleteUser(&delEvent);
	}
	if ((selLevel != level) && ((level<SL_FACE) || (selLevel<SL_FACE))) {
		ExitAllCommandModes (level == SL_OBJECT);
	}
*/	

	if (level != SEL_NONE)
	{
		modes = XFormModes(NULL,NULL,NULL,NULL,NULL,NULL);
	}

	// Setup named selection sets	
//	SetupNamedSelDropDown();
//	UpdateNamedSelDropDown();

//	nodes.DisposeTemporary();
//	if (hSel) RefreshSelType ();
	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	m_pInterface->RedrawViews(m_pInterface->GetTime());
}

