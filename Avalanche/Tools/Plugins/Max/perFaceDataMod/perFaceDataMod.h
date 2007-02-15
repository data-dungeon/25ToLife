/*=============================================================================*\
	FILE:			CPerFaceDataMod.h

	DESCRIPTION:	CPerFaceDataMod(ifier) class definition.
					The CPerFaceDataMod class defines the actual MAX modifier
					data and methods.  The context of the derived object holds
					LocalData specific to the face data attributes.

	CREATED BY:		Bryan Safarik, for Avalanche Software SLC

	HISTORY:		Created:											13-Nov-01
					Level A Completed:									13-Jan-02

	Copyright (c) 2001-2002, All Rights Reserved.
\*=============================================================================*/
#ifndef __PERFACEDATAMOD__H
#define __PERFACEDATAMOD__H


class CModData;
class CFaceDataController;

extern TCHAR *GetString(int id);

extern HINSTANCE	hInstance;

// ---FaceAttributesCallBack class declaration
class CFaceAttributesCallBack : public RedrawViewsCallback
{
    void proc(Interface* ip);
};

// ---table to convert SelLevel values to mesh SelLevel flags
const int meshLevel[] = { MESH_OBJECT, MESH_VERTEX, MESH_EDGE, MESH_FACE, MESH_FACE, MESH_FACE };

// -----------------------------------------------------------------------
// ***					Class Defn: Per Face Data Modifier			   ***
// -----------------------------------------------------------------------
class CPerFaceDataMod : public Modifier, public IMeshSelect, public FlagUser 
{
public:
	// ---member data...
	// ---NOTE: any static declarations indicate that one copy of the member is 
	// ---shared by _ALL_ the instances of the class!!
	static IObjParam*			ip;						// ---interface pointer: access to the interface
	static CPerFaceDataMod*		m_pPFDMod;				// ---pointer to main modifier class
	static SelectModBoxCMode*	m_pSelectMode;			// ---pointer to selectMode instance
	static BOOL					m_bUpdateCachePosted;	// ---inform pipeline to update any cached information
	
	CModData*	m_pClonedLD;	// ---cloned localData
//	INode*		m_pHotNode;		// ---for modifier applied to multiple nodes, this points to the node
								// ---that is currently being operated on (e.g. subobject selection, etc)
	DWORD		m_dwSelLevel;	// ---current selection level
	bool m_bRepaintControls;	// ---flag to trigger repaint of dynamicUI controls

	BOOL m_bRSsoSel;	// ---rollup state of sub-object select panel (FALSE = rolled up)
	BOOL m_bRSinfo;		// ---rollup state of info/copyright panel

	HWND m_hSubObjSel;			// ---sub-object selection panel handle
	HWND m_hInfo;				// ---plugin info panel handle
	
	IRollupWindow*	m_pIrw;	// ---pointer to command panel rollup window interface 

	int m_nNumPanels;		// ---total number of panels (in command panel)

	int m_nPickboxSize;			// ---pickbox size for calculating hit region

	BOOL m_bCollapsed;			// ---stack collapse state			

	BOOL m_bSelByVert;			// ---checkbox, select faces/polys/elements by vertex state
	BOOL m_bIgnoreBackfaces;		// ---checkbox, ignore backfaces state
	BOOL m_bIgnoreVisEdge;		// ---checkbox, ignore visible edges, select faces based on planar threshold
	float	m_fPlanarThresh;		// ---spinner, planar threshold value for IgnoreVisibleEdges option
	float	m_fNormScale;			// ---spinner, size of normal line displayed in viewport			

	BOOL m_bShowFNormals;		// ---toggle, show face normals on/off
	BOOL m_bShowVNormals;		// ---toggle, show vertex normals on/off

	// ---constructor/~destructor
	CPerFaceDataMod();
	~CPerFaceDataMod();

	// ---from Animatable
	void DeleteThis();
	void GetClassName(TSTR& s)	{ s = GetString(IDS_CLASS_NAME); }  
	Class_ID ClassID() { return Class_ID(PERFACEDATAMOD_CLASS_ID); }
	TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }
	int NumSubs() { return 1; }
	Animatable *SubAnim(int i)	{ return GetReference(i); }
	TSTR SubAnimName(int i) { return GetString(IDS_PARAMS); }
	RefTargetHandle Clone(RemapDir &remap = NoRemap());


	// ---from Modifier
	//
	// ---inform MAX what channels are required by this modifier:
	// ---	PART_SELECT: selection level
	// ---	PART_SUBSEL_TYPE: sub-object selection channel
	// ---	PART_TOPO: topology, i.e. polygons or faces
	// ---  PART_GEOM: geometry, i.e. points
	// ---	DISP_ATTRIB_CHANNEL:	miscellaneous bits controlling the item's display.  
	// ---							These bits are specific to the type of object.  For the Mesh object 
	// ---							these are the surface normal scale, display of surface normals, edge
	// ---							visibility and display flags.
	// ---
	ChannelMask ChannelsUsed() { return PART_GEOM | PART_TOPO; }
	ChannelMask ChannelsChanged() { return PART_SELECT | PART_SUBSEL_TYPE | PART_TOPO | DISP_ATTRIB_CHANNEL; }
	Class_ID InputType() { return triObjectClassID; }
	void ModifyObject(TimeValue t, ModContext &mc, ObjectState* os, INode *node);
	Interval GetValidity(TimeValue t);
	Interval LocalValidity(TimeValue t) { return GetValidity(t); }
	BOOL DependOnTopology(ModContext &mc) {return TRUE;}
	IOResult Save(ISave* isave);
	IOResult Load(ILoad* iload);
	IOResult LoadLocalData(ILoad* iload, LocalModData** pld);
	IOResult SaveLocalData(ISave* isave, LocalModData* ld);

	// ---from BaseObject
	void BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev);
	void EndEditParams(IObjParam *ip, ULONG flags, Animatable* next);
	void Init();
	void End();
	int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc);
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext* mc);
	void GetWorldBoundBox(TimeValue t, INode* inode, ViewExp *vpt, Box3& box, ModContext* mc);
	void GetSubObjectCenters(SubObjAxisCallback* cb, TimeValue t, INode* node, ModContext* mc);
	void GetSubObjectTMs(SubObjAxisCallback* cb, TimeValue t, INode* node, ModContext* mc);
	void ActivateSubobjSel(int level, XFormModes& modes);
	void SelectSubComponent(HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert = FALSE);
	void ClearSelection(int m_dwSelLevel);
	void SelectAll(int m_dwSelLevel);
	void InvertSelection	(int m_dwSelLevel);
	void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext* mc);
	void ShowEndResultChanged(BOOL showEndResult) { NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE); }
	void NotifyPreCollapse(INode* node, IDerivedObject* derObj, int index);
	void NotifyPostCollapse(INode* node,Object* obj, IDerivedObject* derObj, int index);
	int NumSubObjTypes();
	ISubObjType* GetSubObjType(int i);
	BOOL SupportsNamedSubSels() {return FALSE;}
	CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

	// ---from IMeshSelect
	void LocalDataChanged();
	DWORD GetSelLevel();
	void SetSelLevel(DWORD level);

	// ---from ReferenceMaker
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message)	{ return REF_SUCCEED; }
	// ---custom methods
	void	UpdateSelLevelDisplay();
	void	SetNumSelLabel	();
	void	UpdateCache		(TimeValue t);
	void	ShowNormals		(DWORD vpFlags = REDRAW_NORMAL);
	void	InvalidateNumberSelected();
	void	SetEnableStates	();

	void	ModifyTriObject	(TriObject* tobj, TimeValue t, ModContext &mc, ObjectState * os);
	float	pThreshDeg2Rad	() { return DegToRad(m_fPlanarThresh); }

	CModData*	GetMDFromNode(INode *node);
	INode*		FindNodeReference(ReferenceTarget* modifier, ModContext* mod_context);

	BOOL	GetMeshFromNode	(INode* pNode, Mesh& mesh);
	void GetContextData(TupArray<CModData *> &contextPtrs);
	
private:

protected:
	CFaceAttributesCallBack	m_FaceAttribCallBack;
};

// -----------------------------------------------------------------------
// ***					Class Defn: Modifier Descriptor					   ***
// -----------------------------------------------------------------------
class CpfdClassDesc : public ClassDesc2 
{
public:
	// The IsPublic() method should return TRUE if the plug-in can be picked
	// and assigned by the user. Some plug-ins may be used privately by other
	// plug-ins implemented in the same DLL and should not appear in lists for
	// user to choose from -- these plug-ins would return FALSE.
	int 			IsPublic()							{ return TRUE; }

	// 3DSMAX calls this method when it needs a pointer to a new instance of 
	// the plug-in class.  The system calls the correspoding 
	// DeleteThis() method of the plug-in to free the memory.  Our 
	// implementations use 'new' and 'delete'.
	void*			Create(BOOL loading = FALSE)	{ return new CPerFaceDataMod; }
	
	// This is the name that appears on the button
	// in the MAX user interface.
	const TCHAR *	ClassName()							{ return GetString(IDS_CLASS_NAME); }

	// The system calls this method at startup to determine the type of object
	// this is.  In our case, we're a object-space modifier, so we return 
	// OSM_CLASS_ID.  The possible options are defined in PLUGAPI.H
	SClass_ID		SuperClassID()							{ return OSM_CLASS_ID; }

	// The system calls this method to retrieve the unique
	// class id for this item.
	Class_ID		ClassID()							{ return PERFACEDATAMOD_CLASS_ID; }

	// The modifier category under which the plugin will appear.
	// If this is set to be an exiting category (e.g. "Parametric Modifiers", ...)
	// then the plug-in will appear in that category. If the category doesn't
	// yet exists then it is created.
	const TCHAR* 	Category()							{ return GetString(IDS_CATEGORY); }

	// The following are used by MAX Script and the Schematic View
	const TCHAR*	InternalName()							{ return _T("Face Attribute Modifier"); }

	// returns owning module handle
	HINSTANCE		HInstance()							{ return hInstance; }
};

// ---declares 'get_pfdModDesc()' and specifies that it has external linkage 
// ---(i.e., its name is visible from files other than this one). 
extern ClassDesc2*	get_pfdModDesc();

// -----------------------------------------------------------------------
// ***					Class Defn: Image Handler					   ***
// -----------------------------------------------------------------------
// ---image handler class for sub-object selection toolbar
class SOSelImageHandler 
{
public:
	HIMAGELIST images;

	SOSelImageHandler()	{ images = NULL; }
	~SOSelImageHandler()	{ if (images) ImageList_Destroy(images); }

	HIMAGELIST	LoadImages();
};
#endif // ---__PERFACEDATAMOD__H