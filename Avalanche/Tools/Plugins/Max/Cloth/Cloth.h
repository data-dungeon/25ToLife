/**********************************************************************
*<
FILE: Cloth.h

DESCRIPTION:	Includes for Plugins
  
CREATED BY:
	
HISTORY:
	  
*>	Copyright (c) 2000, All Rights Reserved.
**********************************************************************/

#ifndef __CLOTH__H
#define __CLOTH__H


#define CLOTH_CLASS_ID	Class_ID(0x697e6651, 0xcdfbd77)

// Custom toolbar button id defines:
#define ID_POINT_SEL	7501
#define ID_CONN_SEL	7502


extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

class Cloth;

class SelectMode;

class Cloth : public Modifier
{
public:
	static HWND m_hSelection;
	static HWND m_hPoints;
	static HWND m_hConnections;

	static IObjParam* m_pInterface;   //Access to the interface

	ICustButton*		m_pLinkButton;
	ISpinnerControl*	m_pExtendSpinner;
	ICustEdit*			m_pExtendEditBox;
	ISpinnerControl*	m_pLinkDistSpinner;
	ICustEdit*			m_pLinkDistEditBox;
	ISpinnerControl*	m_pBrushSizeSpinner;
	ICustEdit*			m_pBrushSizeEditBox;
	ICustToolbar*		m_pSelToolbar;
	HIMAGELIST			m_hSelImages;
	ICustButton*		m_pSelPointsButton;
	ICustButton*		m_pSelConnsButton;

	enum SelectionStates
	{
		SEL_NONE,
		SEL_POINTS,
		SEL_CONNECTIONS,
		SEL_LINK
	};

	SelectionStates m_selectionState;
	static SelectMode* m_pSelectMode;
	CommandMode* m_pOldCmdMode;

	void BeginSelect(int selectionState);
	void CreateConnection(ClothModData* pModData, int v1, int v2);
	void CreateConnections(ClothModData* pModData);
	void AdjustLineExtents(Point3 line[3]);
	void LinkEdges();
	bool GetMesh(Object* pObject, Mesh& mesh);
	void ClearSelected();
	void RecursivelyAddConnections(ClothModData* pModData, int centralPoint, int edgePoint, int level, const AdjEdgeList& adjacentEdges, bool* alreadyChecked);
	void DeleteConnections(bool onlyDeleteInvalid = false);
	void DisplayProperties();
	void UpdateConnectionProperties();
	void UpdatePointProperties(int button);
	void CreateBackToFrontConnections();
	void CachePointNormals(ClothModData* pModData, Object* pObject);
	int FindClosestFrontPoint(ClothModData* pModData, Point3 normal, int backPoint, Object* pObject);
	Point3 GetNormal(Mesh *mesh, int faceNo, RVertex* rv);
	void SetControlStates();
	bool IsPointFiltered(ClothModData* pModData, int pointID);
	bool IsConnectionFiltered(ClothModData* pModData, int connID);
	void DrawCrossSectionNoMarkers(Point3 a, Point3 align, float length, GraphicsWindow *gw);
	void SelectAll(int selLevel);
	void ActivateSubobjSel(int level, XFormModes& modes);

	static int NumConnectedBacks(ClothModData* pModData, int frontPointID);

	Point3 GetStateColor(PointState state);
	Point3 GetLineColor(ConnectionType type);

	void HoldData(ClothModData* pModData);
	INode* GetNodeFromModContext(ModContext *smd, int &which);

	// may need to change these later
	int NumSubObjTypes() { return 2; }
	ISubObjType* GetSubObjType(int i);

	//From Modifier
	ChannelMask ChannelsUsed()  { return GEOM_CHANNEL | SELECT_CHANNEL | TOPO_CHANNEL; }
	ChannelMask ChannelsChanged() { return GEOM_CHANNEL; }
	Class_ID InputType() {return defObjectClassID;}

	void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
	void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);

	void NotifyPreCollapse(INode *node, IDerivedObject *derObj, int index);
	void NotifyPostCollapse(INode *node,Object *obj, IDerivedObject *derObj, int index);

	Interval LocalValidity(TimeValue t);

	// From BaseObject
	BOOL ChangeTopology() {return FALSE;}
	BOOL DependOnTopology(ModContext& mc) { return TRUE; }
	
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);

	CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

	BOOL HasUVW();
	void SetGenUVW(BOOL sw);

	void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

	// Loading/Saving
	IOResult Load(ILoad *iload);
	IOResult Save(ISave *isave);
	IOResult SaveLocalData(ISave *iSave, LocalModData *pld);
	IOResult LoadLocalData(ILoad *iLoad, LocalModData **pld);

	//From Animatable
	Class_ID ClassID() {return CLOTH_CLASS_ID;}		
	SClass_ID SuperClassID() { return OSM_CLASS_ID; }
	TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }
	void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

	RefTargetHandle Clone( RemapDir &remap );
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message);


	int NumSubs() { return 0; }
	TSTR SubAnimName(int i) { return _T(""); }
	Animatable* SubAnim(int i) { return NULL; }

	int NumRefs() { return 0; }
	RefTargetHandle GetReference(int i) { return NULL; }
	void SetReference(int i, RefTargetHandle rtarg) {}

	void DeleteThis() { delete this; }

	//Constructor/Destructor
	Cloth();
	~Cloth();
};


class ClothClassDesc : public ClassDesc2
{
public:
	int           IsPublic() { return TRUE; }
	void*         Create(BOOL loading = FALSE) { return new Cloth(); }
	const TCHAR*  ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID     SuperClassID() { return OSM_CLASS_ID; }
	Class_ID      ClassID() { return CLOTH_CLASS_ID; }
	const TCHAR*  Category() { return GetString(IDS_CATEGORY); }
	const TCHAR*  InternalName() { return _T("Cloth"); }  // returns fixed parsable name (scripter-visible name)
	HINSTANCE     HInstance() { return hInstance; }       // returns owning module handle
	TCHAR*        GetRsrcString(StringResID id) { return NULL; }
};


#endif // __CLOTH__H
