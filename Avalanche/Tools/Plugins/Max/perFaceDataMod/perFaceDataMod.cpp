#include "pfdModPCH.h"
#include "perFaceDataMod.h"
#include "modData.h"
#include "FaceAttributes.h"

// -----------------------------------------------------------------------
// ***				Alternate "SetSwatch" Colorswatch Handler		   ***
// -----------------------------------------------------------------------
// ---subclassed colorswatch handler for set colorswatches (i.e. no colorpicker!)
LRESULT APIENTRY colorSwatchSubclassWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

// -----------------------------------------------------------------------
// ***				Instantiate Sub-Object ImageHandler Object		   ***
// -----------------------------------------------------------------------
static	SOSelImageHandler theSOImageHandler;

// -----------------------------------------------------------------------
// ***				Instantiate Class Description Object			   ***
// -----------------------------------------------------------------------
static	CpfdClassDesc pfdDesc;

// -----------------------------------------------------------------------
// ***				Class Desc. Function Defn						   ***
// -----------------------------------------------------------------------
extern	ClassDesc2*	get_pfdModDesc() 
{ 
// ---return modifier description information
	return &pfdDesc; 
}

// -----------------------------------------------------------------------
// ***				Inline Function Defn's							   ***
// -----------------------------------------------------------------------
inline BOOL uInt2bool( UINT bstat )
{ 
	// ---convert return value of IsDlgButtonChecked() from UINT to BOOL
	return ( (bstat == BST_CHECKED ) ? TRUE : FALSE ); 
}

// -----------------------------------------------------------------------
// ***				Static Variable Defn's							   ***
// -----------------------------------------------------------------------
static	GenSubObjType	SOT_Vertex	(1);	// ---vertex-level subobject
static	GenSubObjType	SOT_Edge	(2);	// ---edge-level subobject
static	GenSubObjType	SOT_Face	(3);	// ---face-level subobject
static	GenSubObjType	SOT_Poly	(4);	// ---polygon-level subobject
static	GenSubObjType	SOT_Element	(5);	// ---element-level subobject

static	HWND	hWnd;						// ---general purpose window handle
static	BOOL	updateNumSel	= TRUE;		// ---flag: number of sub-object elements selected
											// ---needs to be updated in the interface
static	int   currentVersion	= 1;		// ---MAX version
static	BOOL	oldShowEnd;					// ---previous state of "show end result"

// -----------------------------------------------------------------------
// ***				Static Dialog Processor Callback Defn's			   ***
// -----------------------------------------------------------------------
// ---prototypes for dialog processor callbacks
static BOOL CALLBACK SelTypeProc		(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK InfoProc			(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// -----------------------------------------------------------------------
// ***				Display Flag Array Defn							   ***
// -----------------------------------------------------------------------
// ---get display flags based on selLevel
const DWORD levelDispFlags[] = { 0, DISP_VERTTICKS|DISP_SELVERTS, DISP_SELEDGES, DISP_SELFACES, DISP_SELPOLYS, DISP_SELPOLYS };

// -----------------------------------------------------------------------
// ***				Hit Level Array Defn							   ***
// -----------------------------------------------------------------------
// ---for hit testing...
const int hitLevel[] = { 0, SUBHIT_VERTS, SUBHIT_EDGES, SUBHIT_FACES, SUBHIT_FACES, SUBHIT_FACES } ;

// -----------------------------------------------------------------------
// ***				CPerFaceDataMod Static Member Data Init			   ***
// -----------------------------------------------------------------------
IObjParam*			CPerFaceDataMod::ip						= NULL;	// ---MAX interface pointer
CPerFaceDataMod*	CPerFaceDataMod::m_pPFDMod				= NULL;	// ---pointer to main modifier class
SelectModBoxCMode*	CPerFaceDataMod::m_pSelectMode			= NULL;	// ---selection mode pointer
BOOL				CPerFaceDataMod::m_bUpdateCachePosted	= FALSE;// ---flag, inform pipeline to update any cached information

// -----------------------------------------------------------------------
// ***				CPerFaceDataMod Methods:General					   ***
// -----------------------------------------------------------------------
CPerFaceDataMod::CPerFaceDataMod()
{ 
	DebugPrint( "<> CPerFaceDataMod::Construct()\n" );

	// ---member data initial states
	m_dwSelLevel			= SELECT_OBJECT;
	m_bCollapsed			= FALSE;
	m_bIgnoreBackfaces		= FALSE;
	m_bSelByVert			= FALSE;
	m_bIgnoreVisEdge		= FALSE;
	m_bShowFNormals			= FALSE;
	m_bShowVNormals			= FALSE;
	m_bCollapsed			= FALSE; 
	m_bRSsoSel				= TRUE;
	m_bRSinfo				= TRUE;
	m_bRepaintControls		= false; 
	m_fNormScale			= 20.0f;
	m_fPlanarThresh			= 45.0f;
	m_nPickboxSize			= 4;

	m_hSubObjSel			= NULL;
	m_hInfo					= NULL;
//	m_pHotNode				= NULL;
}

CPerFaceDataMod::~CPerFaceDataMod()
{
}

void CPerFaceDataMod::DeleteThis()
{ 
	DebugPrint( "CPerFaceDataMod::DeleteThis()\n" );

	delete this;
}

// ---This method makes a new pfdMod and copies the state of this modifier
// ---to the new modifier.  This method is called when the user makes a copy of
// ---an object in the scene or when they press the make unique
// ---button in the modfier stack rollup page.
RefTargetHandle CPerFaceDataMod::Clone( RemapDir& remap )
{
	DebugPrint( "CPerFaceDataMod::Clone()\n" );

	CPerFaceDataMod* pNewPFDMod = new CPerFaceDataMod();

	pNewPFDMod->m_dwSelLevel			= m_dwSelLevel;
	pNewPFDMod->m_bCollapsed			= m_bCollapsed;
	pNewPFDMod->m_bIgnoreBackfaces		= m_bIgnoreBackfaces;
	pNewPFDMod->m_bSelByVert			= m_bSelByVert;
	pNewPFDMod->m_bIgnoreVisEdge		= m_bIgnoreVisEdge;
	pNewPFDMod->m_bShowFNormals			= m_bShowFNormals;
	pNewPFDMod->m_bShowVNormals			= m_bShowVNormals;
	pNewPFDMod->m_bCollapsed			= m_bCollapsed; 
	pNewPFDMod->m_bRepaintControls		= m_bRepaintControls; 
	pNewPFDMod->m_fNormScale			= m_fNormScale;
	pNewPFDMod->m_fPlanarThresh			= m_fPlanarThresh;
	pNewPFDMod->m_nPickboxSize			= m_nPickboxSize;
	
	pNewPFDMod->m_bRSsoSel				= m_bRSsoSel;
	pNewPFDMod->m_bRSinfo				= m_bRSinfo;

	// ---this method allows base classes to copy their data into a new 
	// ---object created by the clone operation. All overwrites of 
	// ---BaseClone() must call the base class implementation.
	BaseClone( this, pNewPFDMod, remap );
	
	return pNewPFDMod;
}

// ---returns the vality interval of the modifier.
Interval CPerFaceDataMod::GetValidity( TimeValue t ) 
{ 
	return FOREVER; 
}

void CPerFaceDataMod::ModifyObject( TimeValue t, ModContext& mc, ObjectState* os, INode* node ) 
{
	DebugPrint( "CPerFaceDataMod::ModifyObject()\n");
	
	// ---handle TriObjects
	if ( os->obj->IsSubClassOf( triObjectClassID ) ) 
	{
		TriObject *triOb = (TriObject*)os->obj;
		ModifyTriObject( triOb, t, mc, os );
	}
	// ---if possible, convert any non-mesh objects to a tri-mesh
	else if( os->obj->CanConvertToType( triObjectClassID ) ) 
	{
		TriObject  *triOb = ( TriObject * )os->obj->ConvertToType( t, triObjectClassID );
		ModifyTriObject( triOb, t, mc, os );
	}
	// ---don't know what the hell the object is, so punt!
	else return;
}

void CPerFaceDataMod::ModifyTriObject( TriObject* triOb, TimeValue t, ModContext& mc, ObjectState* os )
{		
	if ( m_bCollapsed ) return;
	if ( !ip )			return;

	INode*			pNode		= NULL;
	IDerivedObject* pDerivOb	= NULL;
	Modifier*		pModifier	= NULL;
	ModContext*		pModContext	= NULL;
	CModData*		pMDPipe		= NULL;
	TCHAR*			nodeName	= NULL;

	BOOL	bNewFaceData= FALSE;
	ULONG	handle		= 0;
	int		nMods		= 0;
	int		nNodeNameLen= 0;
	int		radix		= 10;

	// ---grab localData from the context
	CModData* pModData = (CModData*)mc.localData;

	// ---if there's no localData, create some!
	if ( !pModData )
	{
		mc.localData = pModData = new CModData( triOb->mesh );
		bNewFaceData = TRUE;
	}

	// ---find the node for this modifier and current modContext
	pNode = FindNodeReference( this, &mc );
	pModData->m_pNode = pNode;

	// ---if there's a pointer to a node, get number of mods on node's stack
	if ( pNode )
	{
		TSTR	modName;

		nodeName = pNode->GetName();		
		nNodeNameLen = sizeof( nodeName );
		
		DebugPrint( "Modifiers Applied to ");
		DebugPrint( nodeName );
		DebugPrint( ":\n");

		// ---get a pointer to the node's objectReference
		Object* pObjRef = pNode->GetObjectRef();

		// ---perFaceData modifier counter
		int nNumPFDMods = 0;
		
		// ---walk "down" the stack, looking at each modifier, until the
		// ---base object is reached (SuperClassID != GEN_DERIVOB_CLASS_ID)
		while ( pObjRef )
		{
			// ---if objectRef's superClass is GEN_DERIVOB, cast to an IDerivedObject
			if ( pObjRef->SuperClassID() == GEN_DERIVOB_CLASS_ID )
			{
				pDerivOb = (IDerivedObject*)pObjRef;

				// ---how many modifiers (ModApp's) are in this derivedObject?
				nMods = pDerivOb->NumModifiers();
				
				for ( int nModIdx = 0; nModIdx < nMods; nModIdx++ )
				{
					// ---grab a pointer to modifier at index nModIdx
					pModifier = pDerivOb->GetModifier( nModIdx );

					// ---get the modifier's name
					modName = pModifier->GetName();

					// ---is it a PerFaceData modifier?
					if ( pModifier->ClassID() == PERFACEDATAMOD_CLASS_ID )
					{
						DebugPrint( "*" );

						// ---if localData was just created for the current PerFaceData
						// ---modifier, and if there's another PFD mod down
						// ---the pipe, then copy its localData (i.e. faceData) into
						// ---the current PerFaceData mod's localData
						if ( bNewFaceData && nNumPFDMods == 1 )
						{
							// ---get pointer to the context of the nModIdx'th modifier
							pModContext = pDerivOb->GetModContext( nModIdx );

							// ---get pointer to localData for the context
							pMDPipe = (CModData*)pModContext->localData;
							if ( pMDPipe == NULL ) continue;

							// ---clone faceData
							pMDPipe->Clone( pModData );
							
							bNewFaceData = FALSE;
						}
						nNumPFDMods++;
					}					
					DebugPrint("---");
					DebugPrint( modName );
					DebugPrint("\n");
				}
				// ---get a pointer to the object referenced by the current derived onject
				// ---note that this may be another derived object, or the base object
				pObjRef = pDerivOb->GetObjRef();
			}
			else
			{
				break;
			}
		}
	}
	
	// ---sub-object selection bitarray management
	BitArray vertSel = pModData->m_baVertSel;
	BitArray faceSel = pModData->m_baFaceSel;
	BitArray edgeSel = pModData->m_baEdgeSel;
	vertSel.SetSize( triOb->GetMesh().getNumVerts(),	TRUE);
	faceSel.SetSize( triOb->GetMesh().getNumFaces(),	TRUE);
	edgeSel.SetSize( triOb->GetMesh().getNumFaces() * 3,TRUE);
	triOb->GetMesh().vertSel = vertSel;
	triOb->GetMesh().faceSel = faceSel;
	triOb->GetMesh().edgeSel = edgeSel;
	
	// ---set sub-object selection level of the mesh
	triOb->GetMesh().selLevel = meshLevel[m_dwSelLevel];

	// ---retrieve the current validity interval for the SELECT_CHAN_NUM
	// ---channel of the object
	Interval outValid = triOb->ChannelValidity( t, SELECT_CHAN_NUM );

	// ---set/update the cache used for display, hit-testing, etc.
	if ( !pModData->GetMesh() )	
	{
		pModData->SetCache( triOb->GetMesh() );
	}
	else
	{
		*( pModData->GetMesh() )= triOb->GetMesh();
	}

	// ---take care of normals display in the viewport
	if ( m_bShowVNormals )
		triOb->GetMesh().displayNormals( MESH_DISP_VERTEX_NORMALS, m_fNormScale );
	
	if ( m_bShowFNormals )
		triOb->GetMesh().displayNormals( MESH_DISP_FACE_NORMALS, m_fNormScale );
	
	// ---attend to the "special" display flags:
	// ---set display flags - but be sure to turn off vertex display in stack result if
	// ---"Show End Result" is turned on - we want the user to just see the Mesh Select
	// ---level vertices (from the Display method)	
	triOb->GetMesh().dispFlags	= 0;

	if ( ( m_dwSelLevel != SELECT_VERTEX ) || !ip || !ip->GetShowEndResult() )
	{
		triOb->GetMesh().SetDispFlag( levelDispFlags[m_dwSelLevel] );
	}

	// ---update the validity intervals of the object to which the modifier is applied
	triOb->SetChannelValidity( SELECT_CHAN_NUM, outValid );
}

// ---find the node corresponding to the current modifier and modcontext
INode* CPerFaceDataMod::FindNodeReference( ReferenceTarget* modifier, ModContext* mod_context ) 
{
	DependentIterator	di( modifier );
	ReferenceMaker*		reference_maker;
	
	while ( reference_maker = di.Next() ) 
	{ 
		if ( reference_maker->SuperClassID() == BASENODE_CLASS_ID ) 
		{
			INode*	node	= ( INode * )reference_maker;
			Object*	object	= node->GetObjectRef();

			//---get modifier and modcontext and see which one matches
			if ( object )
			{
				IDerivedObject* derived_object = ( IDerivedObject*)object;

				int modifier_index;
				
				for( modifier_index = 0; modifier_index < derived_object->NumModifiers(); modifier_index++ )
				{
					Modifier*	modifier		 = derived_object->GetModifier( modifier_index );
					ModContext* modifier_context = derived_object->GetModContext( modifier_index );

					if( modifier_context == mod_context )
					{
						return node;
					}
				} 
			} 
		}
		if( reference_maker->IsRefTarget() )
		{
			INode* node = FindNodeReference( (ReferenceTarget *)reference_maker, mod_context );

			if( node ) return node;
		}
	}
	return NULL;
}

void CPerFaceDataMod::UpdateCache( TimeValue t ) 
{
	NotifyDependents( Interval(t,t), PART_GEOM|SELECT_CHANNEL|PART_SUBSEL_TYPE|PART_DISPLAY|PART_TOPO, REFMSG_MOD_EVAL );
	m_bUpdateCachePosted = FALSE;
}

void CPerFaceDataMod::NotifyInputChanged( Interval changeInt, PartID partID, RefMessage message, ModContext *mc )
{
	// ---NotifyInputChanged() is called each time the input object is
	// ---changed in some way.  We can find out how it was
	// ---changed by checking partID and message
	if ( !mc->localData )			return;
	if ( partID == PART_SELECT )	return;
	
	// ---flush the data cache
	( (CModData*)mc->localData )->FreeCache();	
	if ( !ip || ( m_pPFDMod != this ) || m_bUpdateCachePosted  ) return;

	// ---the PostMessage() function places (posts) a message in the message
	// ---queue associated with the thread that created the specified window
	// ---and returns without waiting for the thread to process the message.
	PostMessage( m_hSubObjSel, WM_UPDATE_CACHE, (WPARAM)ip->GetTime(), 0 );
	m_bUpdateCachePosted = TRUE;
}

static int butIDs[] = { 0, IDC_SELVERTEX, IDC_SELEDGE, IDC_SELFACE, IDC_SELPOLY, IDC_SELELEMENT };
void CPerFaceDataMod::UpdateSelLevelDisplay() 
{
	if ( !m_hSubObjSel ) return;
	
	ICustToolbar*	iToolbar = GetICustToolbar( GetDlgItem( m_hSubObjSel, IDC_SELTYPE ) );
	ICustButton*	but;

	// ---update display state of the sub-object select custom toolbar
	for ( int i = 1; i < 6; i++ ) 
	{
		but = iToolbar->GetICustButton( butIDs[i] );
		but->SetCheck( (DWORD)i == m_dwSelLevel );
		ReleaseICustButton( but );
	}
	ReleaseICustToolbar( iToolbar );

	SetEnableStates();

	if ( IsDlgButtonChecked(  hWnd, IDC_SHOWNORM_CHK ) ) 
	{
		ShowNormals();
	}
	UpdateWindow( m_hSubObjSel );
}

// ---This method is called by the system when the user needs 
// ---to edit the modifier's parameters in the command panel.  
void CPerFaceDataMod::BeginEditParams( IObjParam* ip, ULONG flags, Animatable* prev )
{
	DebugPrint( "CPerFaceDataMod::BeginEditParams()\n");
	
	this->ip = ip;		// ---set interface pointer
	m_pPFDMod = this;	// ---set mod pointer

	theFaceAttributes.m_pCurrentModifier = this;

	// ---add rollups to modifier command panel interface...
	m_hSubObjSel = ip->AddRollupPage(
		hInstance,							// DLL instance handle
		MAKEINTRESOURCE(IDD_PANEL_SELECT),	// ID of the rollup
		SelTypeProc,						// Dialog procedure to process user input
		GetString(IDS_SUBOBJ_SEL),			// Rollup title text
		(LPARAM)this,						// Saves the this ptr of the item
		(DWORD)0 //APPENDROLL_CLOSED			// rollup initially in the rolled-up state
	);

	m_hInfo = ip->AddRollupPage(
		hInstance,							// DLL instance handle
		MAKEINTRESOURCE(IDD_PANEL_INFO),	// ID of the rollup
		InfoProc,							// Dialog procedure to process user input
		GetString(IDS_INFO),				// Rollup title text
		(LPARAM)this,						// Saves the this ptr of the item
		(DWORD)0 //APPENDROLL_CLOSED			// rollup initially in the rolled-up state
	);

	// ---establish a rollup window interface to the command panel;
	// ---this interface provides methods for showing and hiding rollups, adding and removing rollup pages, etc.
	// ---note: interface does not need to be released with ReleaseIRollup(), as MAX
	// ---takes care of this when it shuts down.
	m_pIrw = ip->GetCommandPanelRollup();
	if ( m_pIrw == NULL ) return;

	// ---get number of panels
	int m_nNumPanels = m_pIrw->GetNumPanels();	
	
	// ---restore the commandPanel rollup states, which were saved in CPerFaceDataMod::EndEditParams()
	if ( m_pIrw )
	{
		// ---iterate over rollup indicies, setting associated rolled-up panels'
		// ---open/closed states
		for (int idx = 0; idx < m_nNumPanels; idx++ )
		{	
			// ---rollup index legend:
			// ---	idx == 0: sub-object selection panel
			// ---	idx == 1: info/copyright panel
			switch ( idx ) 
			{
				case 0:		m_pIrw->SetPanelOpen( idx, m_bRSsoSel ); break;	
				case 1:		m_pIrw->SetPanelOpen( idx, m_bRSinfo );	break;
			}
		}
	}

	// ---instantiate two distinct sub-object selection modes, if not already in memory
	if ( !m_pSelectMode )
	{
		m_pSelectMode = new SelectModBoxCMode( this, ip );
	}

	// ---restore the selection level
	ip->SetSubObjectLevel( m_dwSelLevel );

	// ---initialize the UI
	UpdateSelLevelDisplay	();
	InvalidateNumberSelected();
	SetNumSelLabel			();

	// ---set "show end result" toggle button
	oldShowEnd = ip->GetShowEndResult() ? TRUE : FALSE;
	ip->SetShowEndResult( GetFlag( DISP_END_RESULT ) );

	TimeValue t = ip->GetTime();
	NotifyDependents( Interval( t,t ), PART_ALL, REFMSG_BEGIN_EDIT );
	NotifyDependents( Interval( t,t ), PART_ALL, REFMSG_MOD_DISPLAY_OFF );
	SetAFlag( A_MOD_BEING_EDITED );	

	theFaceAttributes.m_pCurrentModifier = this;
	theFaceAttributes.SetupAttributes();
}

// ---This is called by the system to terminate the editing of the
// ---parameters in the command panel.  
void CPerFaceDataMod::EndEditParams( IObjParam *ip, ULONG flags, Animatable *next )
{
	DebugPrint( "CPerFaceDataMod::EndEditParams()\n");

	// ---force object-level selection
	ip->SetSubObjectLevel( SELECT_OBJECT );

	// ---save rollup panel states, then remove rollups
	if ( m_hSubObjSel )
	{
		m_bRSsoSel = IsRollupPanelOpen( m_hSubObjSel );
		ip->DeleteRollupPage( m_hSubObjSel );
		m_hSubObjSel = NULL;
	}
	if ( m_hInfo ) 
	{
		m_bRSinfo = IsRollupPanelOpen( m_hInfo );
		ip->DeleteRollupPage( m_hInfo );
		m_hInfo = NULL;
	}

	// ---delete select mode
	ip->DeleteMode( m_pSelectMode );
	if ( m_pSelectMode ) delete m_pSelectMode;
	m_pSelectMode = NULL;

	// ---reset show end result
	SetFlag( DISP_END_RESULT, ip->GetShowEndResult() ? TRUE : FALSE );
	ip->SetShowEndResult( oldShowEnd );

	this->ip	= NULL;
	m_pPFDMod	= NULL;

	// ---notify dependents of completion of modifier edits
	TimeValue t = ip->GetTime();
	NotifyDependents( Interval(t,t), PART_ALL, REFMSG_END_EDIT );
	NotifyDependents( Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF );
	ClearAFlag( A_MOD_BEING_EDITED );	

	theFaceAttributes.m_pCurrentModifier = NULL;
	theFaceAttributes.CloseAttributes();

}

class SubObjHitListTemp 
{
public:
	MeshSubHitRec *first;
};

static void ReleaseHitList(SubObjHitList &hitList)
{
	// cast to another class to avoid the private memory problem
	SubObjHitListTemp *pHitListTemp = (SubObjHitListTemp *)&hitList;

	// call the max delete function to release this memory
	MeshSubHitRec *ptr = pHitListTemp->first,*fptr;
	while (ptr) 
	{
		fptr = ptr;
		ptr = ptr->Next();
//		delete fptr;
	}
	pHitListTemp->first = NULL;
}

int CPerFaceDataMod::HitTest( TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt, ModContext* mc ) 
{
	Interval		valid;
	HitRegion		hr;
	int				savedLimits;

	SubObjHitList	hitList;

	MeshSubHitRec*	rec		= NULL;		// ---hit record
	int				res		= FALSE;	// ---hit test flag

	// ---pointer to graphics window
	GraphicsWindow*	gw = vpt->getGW();

	// ---setup hit region
	MakeHitRegion( hr, type, crossing, m_nPickboxSize, p );
	gw->setHitRegion( &hr );

	Matrix3 mat = inode->GetObjectTM( t );
	gw->setTransform( mat );	

	// ---setup viewport render
	gw->setRndLimits( ( ( savedLimits = gw->getRndLimits() ) | GW_PICK ) & ~GW_ILLUM );
	if ( m_bIgnoreBackfaces )	
	{
		gw->setRndLimits( gw->getRndLimits() |  GW_BACKCULL );
	}
	else
	{
		gw->setRndLimits( gw->getRndLimits() & ~GW_BACKCULL );
	}	
	gw->clearHitCode();

	// ---check for context localData and mesh pointers
	if ( !mc->localData || !((CModData*)mc->localData)->GetMesh() ) return FALSE;

	// ---get a reference to the mesh
	Mesh& mesh = *( (CModData*)mc->localData )->GetMesh();

	if ( ( m_dwSelLevel > SELECT_VERTEX ) && m_bSelByVert ) 
	// ---select edge or face "by-vertex"
	{
		if ( m_dwSelLevel >= SELECT_FACE ) 
		{
			res =	mesh.SubObjectHitTest(gw, gw->getMaterial(), &hr, 
					flags | hitLevel[SELECT_VERTEX] | SUBHIT_USEFACESEL, hitList);
		}
		else 
		{
			res =	mesh.SubObjectHitTest(gw, gw->getMaterial(), &hr, 
					flags | hitLevel[SELECT_VERTEX], hitList);
		}
	} 
	// ---"normal" select
	else 
	{
		res =	mesh.SubObjectHitTest(gw, gw->getMaterial(), &hr, 
				flags | hitLevel[m_dwSelLevel], hitList);
	}

	// ---hitList is a linked list; rewind to first element
	rec = hitList.First();

	// ---iterate over the hitList, logging any "hits"
	while (rec) 
	{
		vpt->LogHit( inode, mc, rec->dist, rec->index, NULL );
		rec = rec->Next();
	}
	gw->setRndLimits( savedLimits );
	// we need to release this memory differently since the pointers were allocated in max but freed in the plugin.
	// If the destructor is called with pointers to be freed it will cause a crash
	ReleaseHitList(hitList);
	return res;
}

// ---perform a quick render of the modifier gizmo in viewport using the current TM
int CPerFaceDataMod::Display( TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext *mc ) 
{
	if ( !ip )						return FALSE;
	if ( !ip->GetShowEndResult () )	return FALSE;
	if ( !m_dwSelLevel )			return FALSE;
	if ( !mc->localData )			return FALSE;

	CModData*	mData	= (CModData*)mc->localData;
	Mesh*		mesh	= mData->GetMesh();

	if ( !mesh ) return FALSE;

	// ---setup graphics window interface
	GraphicsWindow*	gw = vpt->getGW();
	Matrix3			tm = inode->GetObjectTM( t );	// ---transformation matrix the object needs 
													// ---to be multiplied by to transform it into world space
	int savedLimits;

	gw->setRndLimits( ( savedLimits = gw->getRndLimits() ) & ~GW_ILLUM );
	gw->setTransform( tm );

	// ---draw a "gizmo" version of the mesh
	Point3 colSel	=	GetSubSelColor	();
	Point3 colTicks	=	GetUIColor		( COLOR_VERT_TICKS );
	Point3 colGiz	=	GetUIColor		( COLOR_GIZMOS );
	Point3 colGizSel=	GetUIColor		( COLOR_SEL_GIZMOS );
	gw->setColor( LINE_COLOR, colGiz );

	AdjEdgeList* ae = mData->GetAdjEdgeList();

	int		i;
	Point3	rp[3];
	int		es[3];
	int		ect = ae->edges.Count();

	for ( i = 0; i < ect; i++ ) 
	{
		MEdge& me = ae->edges[i];

		if ( me.Hidden( mesh->faces ) ) continue;
		
		if ( me.Visible( mesh->faces ) ) 
		{
			es[0] = GW_EDGE_VIS;
		}
		else 
		{
			if ( m_dwSelLevel < SELECT_EDGE ) continue;
			if ( m_dwSelLevel > SELECT_FACE ) continue;
			es[0] = GW_EDGE_INVIS;
		}

		if (m_dwSelLevel == SELECT_EDGE) 
		{
			if ( ae->edges[i].Selected( mesh->faces, mData->GetEdgeSel() ) )	
				gw->setColor( LINE_COLOR, colGizSel );
			else
				gw->setColor( LINE_COLOR, colGiz );
		}

		if (m_dwSelLevel >= SELECT_FACE) 
		{
			if ( ae->edges[i].AFaceSelected( mData->GetFaceSel() ) )
				gw->setColor( LINE_COLOR, colGizSel );
			else
				gw->setColor( LINE_COLOR, colGiz );
		}
		rp[0] = mesh->verts[me.v[0]];
		rp[1] = mesh->verts[me.v[1]];
		gw->polyline( 2, rp, NULL, NULL, FALSE, es );
	}
	
	if ( m_dwSelLevel == SELECT_VERTEX ) 
	{
		for ( i = 0; i < mesh->numVerts; i++ ) 
		{
			if ( mesh->vertHide[i] ) continue;

			if ( mData->GetVertSel()[i] )	gw->setColor( LINE_COLOR, colSel );
			else							gw->setColor( LINE_COLOR, colTicks );

			if ( getUseVertexDots() )	gw->marker( &( mesh->verts[i] ), getVertexDotType() ? DOT_MRKR : SM_DOT_MRKR );
			else						gw->marker( &( mesh->verts[i] ), PLUS_SIGN_MRKR );
		}
	}
	gw->setRndLimits( savedLimits );
	return FALSE;
}

void CPerFaceDataMod::GetWorldBoundBox( TimeValue t, INode* inode, ViewExp *vpt, Box3& box, ModContext *mc ) 
{
	if ( !ip->GetShowEndResult() || !mc->localData ) return;
	if ( !m_dwSelLevel ) return;
	
	CModData*	mData	= (CModData*)mc->localData;
	Mesh*		mesh	= mData->GetMesh();
	if ( !mesh ) return;

	Matrix3 tm	= inode->GetObjectTM( t );
	box			= mesh->getBoundingBox( &tm );
}

void CPerFaceDataMod::GetSubObjectCenters( SubObjAxisCallback *cb, TimeValue t, INode *node, ModContext *mc ) 
{
	if ( !mc->localData )				return;
	if ( m_dwSelLevel == SELECT_OBJECT )	return;

	CModData*	mcData	= (CModData *)mc->localData;
	Mesh*		mesh	= mcData->GetMesh();
	
	if ( !mesh ) return;

	Matrix3 tm = node->GetObjectTM( t );

	// ---return the center of the bounding box of the current selection
	BitArray sel = mesh->VertexTempSel();
	if ( sel.IsEmpty() ) return;

	Box3 box;
	for ( int i = 0; i < mesh->numVerts; i++ )
	{
		if ( sel[i] ) 
		{
			box += mesh->verts[i] * tm;
		}
	}	
	cb->Center( box.Center(), 0 );
}

void CPerFaceDataMod::GetSubObjectTMs( SubObjAxisCallback *cb, TimeValue t, INode *node,ModContext *mc ) 
{
	if ( !mc->localData )				return;
	if ( m_dwSelLevel == SELECT_OBJECT )	return;

	CModData* modData = (CModData *) mc->localData;

	Mesh* mesh	= modData->GetMesh();
	if ( !mesh ) return;
	
	Matrix3 tm = node->GetObjectTM( t );

	// ---return the center of the bounding box of the current selection
	BitArray sel = mesh->VertexTempSel();
	if ( !sel.NumberSet() ) return;
	Box3 box;
	for ( int i = 0; i < mesh->numVerts; i++)  
	{
		if ( sel[i] ) box += mesh->verts[i] * tm;
	}

	Matrix3 ctm( 1 );
	ctm.SetTrans( box.Center() );
	cb->TM( ctm, 0 );
}

void CPerFaceDataMod::ActivateSubobjSel( int level, XFormModes& modes ) 
{
	DebugPrint("CPerFaceDataMod::ActivateSubobjSel()\n");
	
	if ( !this ) return;

	// ---pointer to LocalModData
	CModData* pModData = NULL;

	// ---set the mesh selection level
	m_dwSelLevel = level;

	// ---integer node counter
	int nd;
	
	// ---fill in modes with our sub-object modes
	if ( ( level != SELECT_OBJECT ))
		modes = XFormModes( NULL, NULL, NULL, NULL, NULL, m_pSelectMode);
	
	// ---check for face selection flowing up the pipe; if there's faces
	// ---selected, then stuff their indicies into the face index vector
	ModContextList	mcList;
	INodeTab		nodes;		// ---a table of INode pointers (i.e. Tab<INode*>)
	ip->GetModContexts( mcList, nodes );

	for ( nd = 0; nd < mcList.Count(); nd++ ) 
	{
		pModData = (CModData*)mcList[nd]->localData;
		if ( pModData == NULL ) continue;

		if ( !pModData->m_baFaceSel.IsEmpty() )
		{
			// ---clear the index vector
			pModData->ClearCurrentSel();

			// ---iterate over the select bitarray, collecting the absolute
			// ---face indicies of the set (i.e. selected) faces
			for ( int fIdx = 0; fIdx < pModData->m_baFaceSel.GetSize(); fIdx++ )
			{
				if ( pModData->IsFaceSelected( fIdx ) )
				{
					pModData->AddSelectedFace( fIdx );
				}
			}
		}
	}
	
	// ---update UI
	UpdateSelLevelDisplay	();
	InvalidateNumberSelected();
	SetNumSelLabel			();

	NotifyDependents( FOREVER, PART_SUBSEL_TYPE|PART_DISPLAY, REFMSG_CHANGE );
	ip->PipeSelLevelChanged();
	NotifyDependents( FOREVER, SELECT_CHANNEL|DISP_ATTRIB_CHANNEL|SUBSEL_TYPE_CHANNEL, REFMSG_CHANGE );	
	ip->RedrawViews( ip->GetTime() );

	// update the window
	theFaceAttributes.RefreshAttributes();
}

DWORD CPerFaceDataMod::GetSelLevel() 
{
	switch ( m_dwSelLevel ) 
	{
		case SELECT_OBJECT:	return IMESHSEL_OBJECT;
		case SELECT_VERTEX:	return IMESHSEL_VERTEX;
		case SELECT_EDGE:		return IMESHSEL_EDGE;
	}
	return IMESHSEL_FACE;
}

void CPerFaceDataMod::SetSelLevel( DWORD sl ) 
{
	switch ( sl ) 
	{
		case IMESHSEL_OBJECT:
			m_dwSelLevel = SELECT_OBJECT;
		break;

		case IMESHSEL_VERTEX:
			m_dwSelLevel = SELECT_VERTEX;
		break;

		case IMESHSEL_EDGE:
			m_dwSelLevel = SELECT_EDGE;
		break;

		case IMESHSEL_FACE:
			// ---don't change if we're already in a face level:
			if ( GetSelLevel() == IMESHSEL_FACE ) break;
			m_dwSelLevel = SELECT_POLY;
		break;
	}

	if ( ip && m_pPFDMod == this )
	{
		ip->SetSubObjectLevel( m_dwSelLevel );
	}
}

// ---this method must be called when the LocalModData of the modifier is changed, e.g. subobject
// ---face, edge, etc. selection, faceData etc.
void CPerFaceDataMod::LocalDataChanged() 
{
	NotifyDependents( FOREVER, PART_SELECT, REFMSG_CHANGE );

	if ( !ip )	return;

	// ---update display elements:
	if ( ip && m_pPFDMod == this )
	{
		UpdateSelLevelDisplay	();
		InvalidateNumberSelected();
		if ( updateNumSel ) 
		{
			SetNumSelLabel();
		}

		// ---update the attribute window
		theFaceAttributes.RefreshAttributes();
	}
} 

// ---this method is called to change the selection state of the component identified by "hitRec"
void CPerFaceDataMod::SelectSubComponent( HitRecord *hitRec, BOOL selected, BOOL all, BOOL invert ) 
{
	DebugPrint( "CPerFaceDataMod::SelectSubComponent()\n" );

	if ( !ip ) return;
	TimeValue t = ip->GetTime();

	CModData*	pModData= NULL;
	Mesh*		pMesh	= NULL;	
	AdjFaceList* pAf	= NULL;
	AdjEdgeList* pAe	= NULL;

	BitArray nsel;

	ModContextList	mcList;
	INodeTab		nodes;	// ---a table of INode pointers (i.e. Tab<INode*>)

	ip->GetModContexts( mcList, nodes );

	int nd;
	for ( nd = 0; nd < mcList.Count(); nd++ ) 
	{
		pModData = (CModData*)mcList[nd]->localData;
		if ( pModData == NULL ) continue;

		HitRecord*	pHitRec = hitRec;

		if ( !all && ( pHitRec->modContext->localData != pModData ) ) continue;


		for ( ; pHitRec != NULL; pHitRec = pHitRec->Next() )
		{
			if ( pHitRec->modContext->localData == pModData ) break;
		}
		if ( pHitRec == NULL ) continue;

		pMesh = pModData->GetMesh();
		if ( !pMesh ) continue;

		if ( m_bSelByVert ) 
		{
			pAe = pModData->GetAdjEdgeList();
			if ( !pAe ) continue;
		}

		switch ( m_dwSelLevel )
		{
		case SELECT_VERTEX:
			nsel = pModData->m_baVertSel;
			
			for ( ; pHitRec != NULL; pHitRec = pHitRec->Next() ) 
			{
				if ( pModData != pHitRec->modContext->localData ) continue;
				nsel.Set( pHitRec->hitInfo, invert ? !nsel[pHitRec->hitInfo] : selected );
				if ( !all ) break;
			}
			pModData->SetVertSel( nsel, this, t );
		break;

		case SELECT_EDGE:
			nsel = pModData->m_baEdgeSel;
			for ( ; pHitRec != NULL; pHitRec=pHitRec->Next() ) 
			{
				if ( pModData != pHitRec->modContext->localData ) continue;
				if ( m_bSelByVert ) 
				{
					DWORDTab& list = pAe->list[pHitRec->hitInfo];
					for ( int i = 0; i < list.Count(); i++ ) 
					{
						MEdge& me = pAe->edges[ list[i] ];
						for ( int j = 0; j < 2; j++ ) 
						{
							if ( me.f[j] == UNDEFINED ) continue;

							DWORD ei = pMesh->faces[me.f[j]].GetEdgeIndex( me.v[0], me.v[1] );
							if ( ei > 2 ) continue;
							ei += me.f[j] * 3;
							nsel.Set( ei, invert ? !pModData->m_baEdgeSel[ei] : selected );
						}
					}
				} 
				else 
					nsel.Set( pHitRec->hitInfo, invert ? !pModData->m_baEdgeSel[pHitRec->hitInfo] : selected );

				if ( !all ) break;
			}
			pModData->SetEdgeSel( nsel, this, t );
		break;

		case SELECT_FACE:
			nsel = pModData->m_baFaceSel;
			for ( ; pHitRec != NULL; pHitRec = pHitRec->Next() ) 
			{
				if ( pHitRec->modContext->localData != pModData ) continue;
				if ( m_bSelByVert ) 
				{
					DWORDTab& list = pAe->list[pHitRec->hitInfo];

					for ( int i = 0; i < list.Count(); i++ ) 
					{
						MEdge& me = pAe->edges[list[i]];

						for ( int j = 0; j < 2; j++ ) 
						{
							if ( me.f[j] == UNDEFINED ) continue;
							nsel.Set( me.f[j], invert ? !pModData->m_baFaceSel[me.f[j]] : selected );
						}
					}
				} 
				else 
				{
					nsel.Set( pHitRec->hitInfo, invert ? !pModData->m_baFaceSel[pHitRec->hitInfo] : selected );
				}

				if (!all) break;
			}
			pModData->SetFaceSel( nsel, this, t );
			// ---clear the index vector
			pModData->ClearCurrentSel();
			
			//  ---update the selected-face-index vector
			if ( !nsel.IsEmpty() )
			{
				// ---iterate over the select bitarray, collecting the absolute
				// ---face indicies of the set (i.e. selected) faces
				for ( int fIdx = 0; fIdx < nsel.GetSize(); fIdx++ )
				{
					if ( nsel[fIdx] == 1 )
					{
						pModData->AddSelectedFace( fIdx );
					}
				}
			}
		break;

		case SELECT_POLY:
		case SELECT_ELEMENT:
			pAf = pModData->GetAdjFaceList ();
			nsel.SetSize( pMesh->numFaces );
			nsel.ClearAll();

			for ( ; pHitRec != NULL; pHitRec = pHitRec->Next() ) 
			{
				if ( pModData != pHitRec->modContext->localData  ) continue;
				if ( !m_bSelByVert ) 
				{
					if ( m_dwSelLevel == SELECT_ELEMENT)	
					{
						pMesh->ElementFromFace( pHitRec->hitInfo, nsel, pAf );
					}
					else
					{
						pMesh->PolyFromFace( pHitRec->hitInfo, nsel, pThreshDeg2Rad(), m_bIgnoreVisEdge, pAf );
					}
				} 
				else 
				{
					DWORDTab& list = pAe->list[pHitRec->hitInfo];
					for ( int i = 0; i < list.Count(); i++ ) 
					{
						MEdge& me = pAe->edges[list[i]];
						for ( int j = 0; j < 2; j++ ) 
						{
							if ( me.f[j] == UNDEFINED ) continue;

							if ( m_dwSelLevel == SELECT_ELEMENT )	
							{
								pMesh->ElementFromFace( me.f[j], nsel, pAf );
							}
							else
							{
								pMesh->PolyFromFace( me.f[j], nsel, pThreshDeg2Rad(), m_bIgnoreVisEdge, pAf );
							}
						}
					}
				}
				if (!all) break;
			}

			if ( invert )		
			{
				nsel ^= pModData->m_baFaceSel;
			}
			else if ( selected )
			{
				nsel |= pModData->m_baFaceSel;
			}
			else
			{
				nsel = pModData->m_baFaceSel & ~nsel;
			}

			pModData->SetFaceSel( nsel, this, t );

			// ---set this node as being 'hot'
//			m_pHotNode = nodes[ nd ];

			// ---clear the index vector
			pModData->ClearCurrentSel();

			//  ---update the selected-face-index vector
			if ( !nsel.IsEmpty() )
			{

				// ---iterate over the select bitarray, collecting the absolute
				// ---face indicies of the set (i.e. selected) faces
				for ( int fIdx = 0; fIdx < nsel.GetSize(); fIdx++ )
				{
					if ( nsel[fIdx] == 1 )
					{
						pModData->AddSelectedFace( fIdx );
					}
				}
			}
		break;
		}
	}
	nodes.DisposeTemporary();
	LocalDataChanged();
}

void CPerFaceDataMod::ClearSelection( int sl ) 
{	
	if ( !ip ) return;
	
	ModContextList	mcList;
	INodeTab		nodes;		// ---a table of INode pointers (i.e. Tab<INode*>)
	BitArray		nsel;
	CModData*		pModData;
	Mesh*			pMesh;
	BOOL			noneSel;

	ip->GetModContexts( mcList, nodes );
	for (int i = 0; i < mcList.Count(); i++ ) 
	{
		noneSel = FALSE;
		
		pModData = (CModData*)mcList[i]->localData;
		if ( !pModData ) continue;
		
		pMesh = pModData->GetMesh();
		switch( m_dwSelLevel)  
		{
			case SELECT_VERTEX:
				if ( pModData->m_baVertSel.IsEmpty() ) 
				{
					noneSel = TRUE; break; 
				}
				nsel.SetSize( pMesh->numVerts );
				nsel.ClearAll ();
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh verts
				pModData->SetVertSel( nsel, this, ip->GetTime() );
			break;

			case SELECT_FACE:
			case SELECT_POLY:
			case SELECT_ELEMENT:
				if ( pModData->m_baFaceSel.IsEmpty() ) 
				{
					noneSel = TRUE; break; 
				}
				nsel.SetSize( pMesh->getNumFaces() );
				nsel.ClearAll ();
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh faces
				pModData->SetFaceSel( nsel, this, ip->GetTime() );
				pModData->ClearCurrentSel();
			break;

			case SELECT_EDGE:
				if ( pModData->m_baEdgeSel.IsEmpty() )
				{
					noneSel = TRUE; break; 
				}
				nsel.SetSize( pMesh->getNumFaces() * 3 );
				nsel.ClearAll ();
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh edges
				pModData->SetEdgeSel( nsel, this, ip->GetTime() );
			break;
		}
	}
	
	if ( !noneSel ) 
	{ 
		LocalDataChanged();  
	}	
	nodes.DisposeTemporary();
}

void CPerFaceDataMod::SelectAll( int sl ) 
{
	if ( !ip ) return;	

	ModContextList	mcList;
	INodeTab		nodes;		// ---a table of INode pointers (i.e. Tab<INode*>)
	BitArray		nsel;
	CModData*		pModData;
	Mesh*			pMesh;

	ip->GetModContexts( mcList,nodes );
	for (int i = 0; i < mcList.Count(); i++ ) 
	{
		pModData = (CModData*)mcList[i]->localData;
		if ( !pModData ) continue;

		pMesh = pModData->GetMesh();

		switch ( m_dwSelLevel ) 
		{
			case SELECT_VERTEX:	
				nsel.SetSize( pMesh->numVerts );
				nsel.SetAll();
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh verts
				pModData->SetVertSel( nsel, this, ip->GetTime() );
			break;

			case SELECT_EDGE:
				nsel.SetSize( pMesh->numFaces * 3 );
				nsel.SetAll();
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh edges
				pModData->SetEdgeSel( nsel, this, ip->GetTime() );
			break;

			default:
				nsel.SetSize( pMesh->numFaces );
				nsel.SetAll();
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh faces
				pModData->SetFaceSel( nsel, this, ip->GetTime() );
			break;
		}
	}
	LocalDataChanged();
	nodes.DisposeTemporary();
}

void CPerFaceDataMod::InvertSelection( int sl ) 
{
	if ( !ip ) return;
	if ( m_dwSelLevel == SELECT_OBJECT ) return;

	ModContextList	mcList;
	INodeTab		nodes;		// ---a table of INode pointers (i.e. Tab<INode*>)
	BitArray		nsel;
	CModData*		pModData;	
	Mesh*			pMesh;

	ip->GetModContexts( mcList,nodes );

	for ( int i = 0; i < mcList.Count(); i++ ) 
	{
		pModData = (CModData*)mcList[i]->localData;
		if ( !pModData ) continue;	

		pMesh = pModData->GetMesh();

		switch ( m_dwSelLevel ) 
		{
			case SELECT_VERTEX:	
				nsel.SetSize( pMesh->numVerts );
				nsel = ~(pModData->m_baVertSel);
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh verts
				pModData->SetVertSel( nsel, this, ip->GetTime () );
			break;
		
			case SELECT_EDGE:
				nsel.SetSize( pMesh->numFaces * 3 );
				nsel = ~(pModData->m_baEdgeSel);
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh edges
				pModData->SetEdgeSel( nsel, this, ip->GetTime() );
			break;

			default:
				nsel.SetSize( pMesh->numFaces );
				nsel = ~(pModData->m_baFaceSel);
				pModData->SynchBitArrays();	// ---make sure size of the selection
											// ---bitarry matches the number of mesh faces
				pModData->SetFaceSel( nsel, this, ip->GetTime() );
			break;
		}
	}
	LocalDataChanged();
	nodes.DisposeTemporary();
}

IOResult CPerFaceDataMod::Save( ISave* isave )
{
	IOResult res;
	ULONG nb;

	// ---saves the modifier name
	Modifier::Save(isave);
	
	// ---save modifier data
	isave->BeginChunk( VERSION_CHUNKID );
	res = isave->Write( &currentVersion, sizeof(int), &nb );
	isave->EndChunk();

	DWORD flags = ExportFlags();
	isave->BeginChunk( FLAGS_CHUNKID );
	res = isave->Write( &flags, sizeof(DWORD), &nb );
	isave->EndChunk();

	return res;
}

// ---called by MAX when the modifier is loaded from disk
IOResult CPerFaceDataMod::Load( ILoad* iload ) 
{
	IOResult res;
	ULONG nb;
	int version = 1;
	DWORD flags;

	// ---load modifier name
	Modifier::Load(iload);

	// ---load modifier data
	while ( IO_OK == ( res = iload->OpenChunk() ) ) 
	{
		switch( iload->CurChunkID() )  
		{
			case VERSION_CHUNKID:
				iload->Read( &version, sizeof( version ), &nb );
			break;

			case FLAGS_CHUNKID:
				iload->Read( &flags, sizeof( DWORD ), &nb );
				ImportFlags( flags );
			break;
		}

		iload->CloseChunk();
		if ( res != IO_OK ) return res;
	}
	return IO_OK;
}

static IOResult WriteString(ISave *isave,const TupString &string)
{
	ULONG numBytesWritten;
	IOResult res;
	res = isave->Write((const char *)string,string.GetLength()+1,&numBytesWritten);
	if (res == IO_OK && numBytesWritten==string.GetLength()+1)
	{
		return res;
	}
	else
	{
		return IO_ERROR;
	}
}

static IOResult ReadString(ILoad *iload,TupString &string)
{
	TupArray<char> buffer;
	char value;
	ULONG numBytesRead;
	IOResult res;
	while (1)
	{
	   res = iload->Read(&value,1,&numBytesRead);
		if (res == IO_OK && numBytesRead==1)
		{
			buffer.Add(value);
			// if we read a null character we are done
			if (value==0)
				break;
		}
		else
		{
			string.Empty();
			return res;
		}
	}
	string = buffer.GetData();
	return IO_OK;
}


// ---save data associated with the modifier, such as subObj selections, key/val/fSel vectors, etc.
IOResult CPerFaceDataMod::SaveLocalData( ISave* isave, LocalModData* ld )
{	
	IOResult	res;
	ULONG		vOffset = 0;
	BitArray	fSel;
	ULONG numBytesWritten;
	CModData*	pModData = (CModData*)ld;
	
	isave->BeginChunk( VERTSEL_CHUNKID );
	pModData->m_baVertSel.Save( isave );
	isave->EndChunk();

	isave->BeginChunk( FACESEL_CHUNKID );
	pModData->m_baFaceSel.Save( isave );
	isave->EndChunk();

	isave->BeginChunk( EDGESEL_CHUNKID );
	pModData->m_baEdgeSel.Save( isave );
	isave->EndChunk();
	
	// ---save key/value vector and associated face selection vector
	isave->BeginChunk( KEYVAL_SETS_CHUNKID );
	int numKeyValsSets = pModData->m_keyValSets.GetSize();
	// write out the number of sets
	res = isave->Write(&numKeyValsSets,sizeof(int),&numBytesWritten);
	for (int setIndex=0;setIndex<numKeyValsSets;setIndex++)
	{
		CurrAttribData &currAttribData = pModData->m_keyValSets[setIndex];
		int numKeyValPairs = currAttribData.GetNumAttribs();
		// write out the number of key/val pairs
		res = isave->Write(&numKeyValPairs,sizeof(int),&numBytesWritten);
		for (int keyValIndex=0;keyValIndex<numKeyValPairs;keyValIndex++)
		{
			TupString key = currAttribData.GetKeyAsString(keyValIndex);
			res = WriteString(isave,key);
			TupString value = currAttribData.GetValueAsString(keyValIndex);
			res = WriteString(isave,value);
		}
	}
	isave->EndChunk();

	// save the set index for each face
	isave->BeginChunk(FACE_KEYVAL_INDICES_CHUNKID);
	int numFaces = pModData->m_faceKeyValIndices.GetSize();
	// write out the number of faces
	res = isave->Write(&numFaces,sizeof(int),&numBytesWritten);
	res = isave->Write(pModData->m_faceKeyValIndices.GetData(),numFaces*sizeof(int),&numBytesWritten);
	isave->EndChunk();
	
	return ( res != IO_OK ) ? res : IO_OK;
}

IOResult CPerFaceDataMod::LoadLocalData( ILoad* iload, LocalModData** pld )
{
	IOResult	res;
	BitArray	fSel;
	BOOL		keyValFacesLoaded = FALSE;
	ULONG    numBytesRead;
	
	CModData* pModData = new CModData;
	*pld = pModData;

	while ( IO_OK == ( res = iload->OpenChunk() ) ) 
	{
		switch( iload->CurChunkID() )  
		{	
			case VERTSEL_CHUNKID:
				pModData->m_baVertSel.Load( iload );
			break;

			case FACESEL_CHUNKID:
				pModData->m_baFaceSel.Load( iload );
			break;

			case EDGESEL_CHUNKID:
				pModData->m_baEdgeSel.Load( iload );
			break;

			case KEYVAL_SETS_CHUNKID:
				{
					pModData->m_keyValSets.RemoveAll();
					// ---read key/value vector and associated face selection vector
					int numKeyValsSets;
					// read in the number of sets
					res = iload->Read(&numKeyValsSets,sizeof(int),&numBytesRead);
					pModData->m_keyValSets.SetSize(numKeyValsSets);
					for (int setIndex=0;setIndex<numKeyValsSets;setIndex++)
					{
						CurrAttribData &currAttribData = pModData->m_keyValSets[setIndex];
						int numKeyValPairs;
						// write out the number of key/val pairs
						res = iload->Read(&numKeyValPairs,sizeof(int),&numBytesRead);
						for (int keyValIndex=0;keyValIndex<numKeyValPairs;keyValIndex++)
						{
							TupString key;
							res = ReadString(iload,key);
							TupString value;
							res = ReadString(iload,value);
							currAttribData.AddKeyValue(key,value);
						}
					}
				}
			break;

			case FACE_KEYVAL_INDICES_CHUNKID:
				{
					int numFaces;
					// write out the number of faces
					res = iload->Read(&numFaces,sizeof(int),&numBytesRead);
					pModData->m_faceKeyValIndices.SetSize(numFaces);
					res = iload->Read(pModData->m_faceKeyValIndices.GetData(),numFaces*sizeof(int),&numBytesRead);
				}
			break;
		}
		
		iload->CloseChunk();
		if ( res != IO_OK ) return res;
	}
	// ---reload the currently selected face index vector
	if ( !pModData->m_baFaceSel.IsEmpty() )
	{
		for (int fIdx = 0; fIdx < pModData->m_baFaceSel.GetSize(); fIdx++ )
		{
			if ( pModData->m_baFaceSel[ fIdx ] ) 
			{
				pModData->AddSelectedFace( fIdx );
			}
		}
	}
	
	return IO_OK;
}

void CPerFaceDataMod::NotifyPreCollapse( INode *node, IDerivedObject *derObj, int index )
{
// ---between NotifyPreCollapse and NotifyPostCollapse, Modify is
// ---called by the system.  NotifyPreCollapse can be used to save any
// ---plugin dependant data, e.g LocalModData.

	// ---copy our LocalModData
	m_bCollapsed = TRUE;
	ModContext* mc = derObj->GetModContext( index );
	m_pClonedLD	= (CModData*)mc->localData->Clone();

	TimeValue t = GetCOREInterface()->GetTime();
	NotifyDependents( Interval(t,t), PART_ALL, REFMSG_CHANGE );
}

void CPerFaceDataMod::NotifyPostCollapse( INode *node, Object *obj, IDerivedObject *derObj, int index )
{
//--- NotifyPostCollapse can be used to apply the modifier back onto
//--- to the stack, copying over the stored data from the temporary
//--- storage.  We want to survive a collapsed stack, so we reapply
//--- ourselves here.

	Object* bo				= node->GetObjectRef();	// ---returns the object that this node references 
	IDerivedObject* derob	= NULL;
	
	if( bo->SuperClassID() != GEN_DERIVOB_CLASS_ID )
	{
		derob = CreateDerivedObject( obj );	// ---creates an object space derived object
		node->SetObjectRef( derob );		// ---sets the object that this node references
	}
	else
		derob = (IDerivedObject*) bo;

	// ---add ourselves to the top of the stack
	derob->AddModifier( this, NULL, 0 );	// ---adds a modifier to the derived object
	
	// ---re-insert our local mod data
	int nNumModifiers = derob->NumModifiers();
	ModContext* mc = derob->GetModContext( nNumModifiers - 1 );	// ---returns the ModContext of the modifier
	mc->localData = m_pClonedLD;								// ---inserts the local data
	
	m_bCollapsed = FALSE;
	theFaceAttributes.RefreshAttributes();
}

int CPerFaceDataMod::NumSubObjTypes() 
{ 
	return 5;
}

ISubObjType *CPerFaceDataMod::GetSubObjType( int i ) 
{
	static bool initialized = false;
	if( !initialized )
	{
		initialized = true;
		SOT_Vertex.SetName( GetString( IDS_VERTEX ) );
		SOT_Edge.SetName( GetString( IDS_EDGE ) );
		SOT_Face.SetName( GetString( IDS_FACE ) );
		SOT_Poly.SetName( GetString( IDS_POLY ) );
		SOT_Element.SetName( GetString( IDS_ELEMENT ) );
	}
	switch( i )
	{
		case -1:	// ---selection flows up stack	
			if( GetSubObjectLevel() > 0 )
				return GetSubObjType( GetSubObjectLevel() - 1 );
			break;
		case 0:
			return &SOT_Vertex;
		case 1:
			return &SOT_Edge;
		case 2:
			return &SOT_Face;
		case 3:
			return &SOT_Poly;
		case 4:
			return &SOT_Element;
	}
	return NULL;
}

void CPerFaceDataMod::SetNumSelLabel() 
{	
	static TSTR		buf;
	ModContextList	mcList;	
	INodeTab		nodes;		// ---a table of INode pointers (i.e. Tab<INode*>)
	int				i;
	int				thissel;
	int				num		= 0;
	int				where	= 0;
	CModData*		pModData;	

	if ( !m_hSubObjSel ) return;
	if ( !updateNumSel ) 
	{
		SetDlgItemText( m_hSubObjSel, IDC_NUMSEL_NFO, buf );
		return;
	}

	updateNumSel = FALSE;
	if ( m_dwSelLevel == SELECT_OBJECT ) 
	{
		buf.printf( GetString( IDS_EM_OBJECT_SEL ) );
		SetDlgItemText( m_hSubObjSel, IDC_NUMSEL_NFO, buf );
		return;
	}

	ip->GetModContexts( mcList,nodes );
	for ( i = 0; i < mcList.Count(); i++ ) 
	{
		pModData = (CModData*)mcList[i]->localData;
		if ( !pModData ) continue;

		switch ( m_dwSelLevel ) 
		{ 
			case SELECT_VERTEX:
				thissel = pModData->m_baVertSel.NumberSet();
			break;

			case SELECT_FACE:
			case SELECT_POLY:
			case SELECT_ELEMENT:
				thissel = pModData->m_baFaceSel.NumberSet();
			break;

			case SELECT_EDGE:
				thissel= pModData->m_baEdgeSel.NumberSet();
			break;
		}

		if ( thissel ) 
		{ 
			where = i; 
			num += thissel; 
		}
	}

	// ---if we have only one element selected, which one is it?
	int which;
	if ( num == 1) 
	{
		CModData *pModData = (CModData*) mcList[where]->localData;
		switch (m_dwSelLevel) 
		{
			case SELECT_VERTEX:
				for ( which = 0; which < pModData->m_baVertSel.GetSize(); which++ ) 
					if ( pModData->m_baVertSel[which] ) break;
			break;

			case SELECT_FACE:
			case SELECT_POLY:
			case SELECT_ELEMENT:
				for ( which = 0; which < pModData->m_baFaceSel.GetSize(); which++ ) 
					if ( pModData->m_baFaceSel[which] ) break;
			break;

			case SELECT_EDGE:
				for ( which = 0; which < pModData->m_baEdgeSel.GetSize(); which++ ) 
					if ( pModData->m_baEdgeSel[which] ) break;
			break;
		}
	}

	switch ( m_dwSelLevel ) 
	{
		case SELECT_VERTEX:			
			if ( num == 1 ) 
				buf.printf( GetString( IDS_PFD_WHICHVERTSEL ), which + 1 );
			else 
				buf.printf( GetString( IDS_PFD_NUMVERTSELP ), num );
		break;

		case SELECT_FACE:
		case SELECT_POLY:
		case SELECT_ELEMENT:
			if ( num == 1 ) 
				buf.printf( GetString( IDS_PFD_WHICHFACESEL ), which + 1 );
			else 
				buf.printf( GetString( IDS_PFD_NUMFACESELP ), num );
		break;

		case SELECT_EDGE:
			if ( num == 1) 
				buf.printf( GetString( IDS_PFD_WHICHEDGESEL ), which + 1 );
			else 
				buf.printf( GetString( IDS_PFD_NUMEDGESELP ), num );
		break;
	}
	SetDlgItemText( m_hSubObjSel, IDC_NUMSEL_NFO, buf );
}

void CPerFaceDataMod::ShowNormals( DWORD vpFlags ) 
{
	DWORD			normalFlags = 0;
	ModContextList	mcList;
	INodeTab		nodes;	// ---a table of INode pointers (i.e. Tab<INode*>)

	switch ( m_dwSelLevel ) 
	{
		case SELECT_FACE:
		case SELECT_POLY:
		case SELECT_ELEMENT:
			if ( m_bShowFNormals ) normalFlags = MESH_DISP_FACE_NORMALS;
		break;
		case SELECT_VERTEX:
			if ( m_bShowVNormals ) normalFlags = MESH_DISP_VERTEX_NORMALS;
		break;
	}
	if ( !ip ) return;
	ip->GetModContexts( mcList, nodes );

	for ( int i = 0; i < mcList.Count(); i++ ) 
	{
		if ( normalFlags )
		{
			CModData* mData = (CModData*)mcList[i]->localData;
			if (!mData) continue;
			
			Mesh *mesh = mData->GetMesh();
			if ( mesh )
			{
				mesh->displayNormals( normalFlags, m_fNormScale );
				mesh->buildBoundingBox();
			}
		}
	}
	NotifyDependents( FOREVER, PART_DISPLAY, REFMSG_CHANGE );
	ip->RedrawViews( ip->GetTime(), vpFlags );
}

void CPerFaceDataMod::InvalidateNumberSelected () 
{
	if ( !m_hSubObjSel ) return;

	InvalidateRect( m_hSubObjSel, NULL, FALSE );
	updateNumSel = TRUE;
}

void CPerFaceDataMod::SetEnableStates() 
{
	ISpinnerControl*	spin;
	BOOL				enableMe = FALSE;
	
	bool obj	= ( m_dwSelLevel == SELECT_OBJECT )	? true : false;
	bool vert	= ( m_dwSelLevel == SELECT_VERTEX )	? true : false;
	bool edge	= ( m_dwSelLevel == SELECT_EDGE )		? true : false;
	bool face	= ( m_dwSelLevel >= SELECT_FACE )		? true : false;
	bool poly	= ( m_dwSelLevel == SELECT_POLY )		? true : false;

	// ---enable sub-object selection control only if a single node is selected!
	if ( m_hSubObjSel )
	{
		// ---selection controls: all checkboxes
		EnableWindow( GetDlgItem( m_hSubObjSel, IDC_SEL_BYVERT_CHK ),		edge || face	);
		EnableWindow( GetDlgItem( m_hSubObjSel, IDC_IGNORE_BACKFACES_CHK ),	!obj			);
		EnableWindow( GetDlgItem( m_hSubObjSel, IDC_IGNORE_VISEDGE_CHK ),	poly 			);
		EnableWindow( GetDlgItem( m_hSubObjSel, IDC_SHOWNORM_CHK ),			vert || face	);

		// ---selection controls: planar threshold options
		enableMe =	poly && uInt2bool( IsDlgButtonChecked( m_hSubObjSel, IDC_IGNORE_VISEDGE_CHK ) );
//					&& ( ip->GetSelNodeCount() == 1 );
		EnableWindow(	GetDlgItem( m_hSubObjSel, IDC_PLANTHRESH_LBL ), enableMe);
		EnableWindow( GetDlgItem ( m_hSubObjSel, IDC_PLANTHRESH_BOX ), poly );
		spin = GetISpinner( GetDlgItem( m_hSubObjSel, IDC_PLANTHRESH_SPIN ) );
		spin->Enable( enableMe );
		ReleaseISpinner( spin );

		// ---selection controls: normals options
		enableMe =	( vert || face ) && uInt2bool( IsDlgButtonChecked( m_hSubObjSel, IDC_SHOWNORM_CHK ) );
//					&& ( ip->GetSelNodeCount() == 1 );
		CheckDlgButton( m_hSubObjSel, IDC_SHOWNORM_CHK, m_bShowFNormals );
		EnableWindow( GetDlgItem( m_hSubObjSel, IDC_NORMSCALE_LBL ), enableMe);
		spin = GetISpinner( GetDlgItem( m_hSubObjSel, IDC_NORMSCALE_SPIN ) );
		spin->Enable( enableMe );
		ReleaseISpinner( spin );

		// ---selected vert/edge/face count 
		EnableWindow( GetDlgItem( m_hSubObjSel, IDC_NUMSEL_NFO ), vert || edge || face );

		// ---Sets the on/off (pressed/unpressed) state of the Show End Result button.  
		// ---Note that calling this method generates a redraw.
		ip->SetShowEndResult( GetFlag( DISP_END_RESULT ) );
	}
}

CModData* CPerFaceDataMod::GetMDFromNode( INode *node )
{
	SClass_ID		sc;
	IDerivedObject* dobj;

	BOOL found = FALSE;

	Object* obj = node->GetObjectRef();
	int ct = 0;

	if ( (sc = obj->SuperClassID() ) == GEN_DERIVOB_CLASS_ID )
	{
		dobj = (IDerivedObject*)obj;
		while ( sc == GEN_DERIVOB_CLASS_ID )
		{
			for ( int j = 0; j < dobj->NumModifiers(); j++ )
			{
				if ( dobj->GetModifier(j) == this )
					{
						ModContext *mc = dobj->GetModContext(j);
						if ( ( mc ) && ( mc->localData ) ) return (CModData*)mc->localData;
					}
			}
			dobj = (IDerivedObject*)dobj->GetObjRef();
			sc = dobj->SuperClassID();
		}
	}
	return NULL;
}

void CFaceAttributesCallBack::proc( Interface* ip )
{ ; }


// ---this method will initialize the face data control panel dialog.  Really,
// ---the only thing done here is the registering of the callback.
void CPerFaceDataMod::Init()
{
    ip->RegisterRedrawViewsCallback( &m_FaceAttribCallBack );
}


// ---this method will be called when the dialog box is closed.  It unregisters
// ---the callback for the dialog.
void CPerFaceDataMod::End()
{
    ip->UnRegisterRedrawViewsCallback( &m_FaceAttribCallBack );
}



/*************************************************************************
 CPerFaceDataMod::GetMeshFromNode

 Tries to extract a mesh from the node
*************************************************************************/
BOOL CPerFaceDataMod::GetMeshFromNode(INode* pNode, Mesh& mesh)
{
  if (!pNode)
    return FALSE;

  // make sure we can get a valid object
  ObjectState os = pNode->EvalWorldState(ip->GetTime());
  if (!os.obj)
    return FALSE;

  // don't process this node if it's not a mesh
  if (os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
    return FALSE;

  // handle TriObjects
  if (os.obj->IsSubClassOf(triObjectClassID))
  {
    TriObject *pTriObj = (TriObject*)os.obj;
    mesh = pTriObj->GetMesh();
  }
/*
  // handle PolyObjects
  else if (os.obj->IsSubClassOf(polyObjectClassID))
  {
    PolyObject *pPolyObj = (PolyObject*)os.obj;
    MNMesh& polyMesh = pPolyObj->GetMesh();
    polyMesh.OutToTri(mesh);
  }
*/
  // Convert to a tri mesh if possible
  else if(os.obj->CanConvertToType(triObjectClassID))
  {
    TriObject  *pTriObj = (TriObject *)os.obj->ConvertToType(ip->GetTime(), triObjectClassID);
    os.obj = pTriObj;
    mesh = pTriObj->GetMesh();
  }
  else
  {
    return FALSE;
  }
  return TRUE;
} // end HoldVCUtil::GetMesh


void CPerFaceDataMod::GetContextData(TupArray<CModData *> &contextPtrs)
{
	contextPtrs.RemoveAll();

	ModContextList	mcList;		// ---a list of modifier contexts
	INodeTab		nodes;		// ---a table of INode pointers (i.e. Tab<INode*>)
	ip->GetModContexts( mcList, nodes );

	// ---get the number of modifier contexts
	int nNumContexts = mcList.Count();

	// ---iterate thru context list
	for (int nContext = 0; nContext < nNumContexts; nContext++ ) 
	{
		// ---get pointer to localData for the current context
		CModData *pModData = (CModData*)mcList[nContext]->localData;
		if ( pModData == NULL ) 
			continue;

		if (pModData->m_pMesh == NULL)
			continue;

		if (pModData->m_pNode == NULL)
			continue;

		contextPtrs.Add(pModData);
	}
}


// -----------------------------------------------------------------------
// ***				Dialog Handler Proc Defn's						   ***
// -----------------------------------------------------------------------
// ---SelTypeProc() manages input to the "Sub-Object Selection" rollout
BOOL CALLBACK SelTypeProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
{

	CPerFaceDataMod* pPFDMod = (CPerFaceDataMod*)GetWindowLongPtr( hWnd, GWLP_USERDATA );

	int id = LOWORD( wParam );
	int code = HIWORD( wParam );

	ICustToolbar*		iToolbar;
	ISpinnerControl*	spin_pThresh;
	ISpinnerControl*	spin_nScale;
	ISpinnerControl*	spin;
	ICustEdit*			edit_pThresh;
	ICustEdit*			edit_nScale;

	ULONG nNodeCnt;

	switch (msg) 
	{
		// ---this message is sent to the dialog box procedure immediately before a dialog box is displayed
		case WM_INITDIALOG:
			pPFDMod = (CPerFaceDataMod*)lParam;
			pPFDMod->m_hSubObjSel = hWnd;
			SetWindowLongPtr( hWnd, GWLP_USERDATA, lParam );

			if ( pPFDMod )
			{
				// ---get number of selected nodes
				nNodeCnt = pPFDMod->ip->GetSelNodeCount();
			}

			// ---subObject toolbar
			iToolbar = GetICustToolbar( GetDlgItem( hWnd,IDC_SELTYPE ) );
			iToolbar->SetImage( theSOImageHandler.LoadImages() );
			iToolbar->AddTool( ToolButtonItem( CTB_CHECKBUTTON,0,5,10,5,24,23,24,23,IDC_SELVERTEX ) );
			iToolbar->AddTool( ToolButtonItem( CTB_CHECKBUTTON,1,6,11,6,24,23,24,23,IDC_SELEDGE ) );
			iToolbar->AddTool( ToolButtonItem( CTB_CHECKBUTTON,2,7,12,7,24,23,24,23,IDC_SELFACE ) );
			iToolbar->AddTool( ToolButtonItem( CTB_CHECKBUTTON,3,8,13,8,24,23,24,23,IDC_SELPOLY ) );
			iToolbar->AddTool( ToolButtonItem( CTB_CHECKBUTTON,4,9,14,9,24,23,24,23,IDC_SELELEMENT ) );
/*
			// ---if there's more than one node in the object selection set, 
			// ---disable the ability to drop into sub-object mode
			if ( nNodeCnt > 1 )
			{
				iToolbar->Enable( FALSE );

				// ---update display state of the sub-object select custom toolbar
				for ( int i = 1; i < 6; i++ ) 
				{
					but = iToolbar->GetICustButton( butIDs[i] );
					but->Disable();
					ReleaseICustButton( but );
				}
			}
			ReleaseICustToolbar( iToolbar );
*/
			// ---planar threshold spinner and editBox
			spin_pThresh = GetISpinner( GetDlgItem( hWnd, IDC_PLANTHRESH_SPIN ) );
			edit_pThresh = GetICustEdit( GetDlgItem( hWnd, IDC_PLANTHRESH_BOX ) );
			spin_pThresh->SetLimits( 0.0f, 89.5f, FALSE );
			spin_pThresh->SetValue( 45.0f, FALSE );
			spin_pThresh->SetScale( 0.1f );
			spin_pThresh->Enable( FALSE );
			spin_pThresh->LinkToEdit( GetDlgItem( hWnd,IDC_PLANTHRESH_BOX ), EDITTYPE_FLOAT );
			edit_pThresh->Enable( FALSE );
			ReleaseISpinner( spin_pThresh );
			ReleaseICustEdit( edit_pThresh );

			// ---normal scale spinner and editBox
			spin_nScale	= GetISpinner( GetDlgItem( hWnd, IDC_NORMSCALE_SPIN ) );
			edit_nScale	= GetICustEdit( GetDlgItem( hWnd, IDC_NORMSCALE_BOX ) );
			spin_nScale->SetLimits( 0.1f, 99999.9f, FALSE );
			spin_nScale->SetValue( 20.0f, FALSE );
			spin_nScale->SetScale( 0.1f );
			spin_nScale->Enable( FALSE );
			spin_nScale->LinkToEdit( GetDlgItem( hWnd,IDC_NORMSCALE_BOX ), EDITTYPE_FLOAT);
			edit_nScale->Enable( FALSE );
			ReleaseISpinner( spin_nScale );
			ReleaseICustEdit( edit_nScale );

			switch ( pPFDMod->m_dwSelLevel ) 
			{
				case SELECT_VERTEX:
					CheckDlgButton( hWnd, IDC_SHOWNORM_CHK, pPFDMod->m_bShowVNormals );
					if ( IsDlgButtonChecked(  hWnd, IDC_SHOWNORM_CHK ) ) 
					{
						pPFDMod->ShowNormals();
					}
				break;

				case SELECT_FACE:
				case SELECT_POLY:
				case SELECT_ELEMENT:
					CheckDlgButton( hWnd, IDC_SHOWNORM_CHK, pPFDMod->m_bShowFNormals );
					if ( IsDlgButtonChecked(  hWnd, IDC_SHOWNORM_CHK ) ) 
					{
						pPFDMod->ShowNormals();
					}
				break;
			}
			CheckDlgButton( hWnd, IDC_SEL_BYVERT_CHK,		pPFDMod->m_bSelByVert);
			CheckDlgButton( hWnd, IDC_IGNORE_BACKFACES_CHK,	pPFDMod->m_bIgnoreBackfaces);
			CheckDlgButton( hWnd, IDC_IGNORE_VISEDGE_CHK,	pPFDMod->m_bIgnoreVisEdge);

			pPFDMod->UpdateSelLevelDisplay();
			pPFDMod->ip->SetSubObjectLevel( SELECT_OBJECT );	// ---force object-level
		break;

		case CC_SPINNER_CHANGE:
			spin = (ISpinnerControl*)lParam;
			switch ( id ) 
			{
				case IDC_PLANTHRESH_SPIN:
					pPFDMod->m_fPlanarThresh = spin->GetFVal();
				break;

				case IDC_NORMSCALE_SPIN:
					pPFDMod->m_fNormScale = spin->GetFVal();
					if ( !IsDlgButtonChecked(  hWnd, IDC_SHOWNORM_CHK ) ) break;
					pPFDMod->ShowNormals( REDRAW_INTERACTIVE );
			}
		 break;

		case CC_SPINNER_BUTTONUP:
		// ---user is done twiddling spinner control...
		case WM_CUSTEDIT_ENTER: 
		// ---This message is sent when the user presses ENTER on
		// ---a custom edit control...
			spin = (ISpinnerControl*)lParam;
			switch ( id ) 
			{
				case IDC_PLANTHRESH_SPIN:
					// has user confirmed (HIWORD - i.e. code - is TRUE) or cancelled (HIWORD is FALSE) action
					if ( code ) pPFDMod->m_fPlanarThresh = spin->GetFVal();
				break;

				case IDC_NORMSCALE_SPIN:
					if ( code )
					{
						pPFDMod->m_fNormScale = spin->GetFVal();
						if ( IsDlgButtonChecked(  hWnd, IDC_SHOWNORM_CHK ) )
						{
							pPFDMod->ShowNormals( REDRAW_END );
						}
					}
				break;
			}
		break;

		case CC_SPINNER_BUTTONDOWN:
		// ---indicate the down state of the spinner button.
		// ---Note this message is ALSO sent when the user enters a 
		// ---value by typing and then presses ENTER.  You will
		// ---see the DN indicator come up in the Spin MSG: line
		// ---if you do press ENTER.
			switch ( id ) 
			{
				case IDC_PLANTHRESH_SPIN:
				break;

				case IDC_NORMSCALE_SPIN:
				break;
			}
		break;

		case WM_UPDATE_CACHE:
			pPFDMod->UpdateCache( (TimeValue)wParam );
 		break;

		case WM_COMMAND:
			switch ( id ) 
			{
				case IDC_SELVERTEX:
					if ( pPFDMod->m_dwSelLevel == SELECT_VERTEX ) 
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_OBJECT );
					}
					else 
					{ 
						pPFDMod->ip->SetSubObjectLevel( SELECT_VERTEX ); 
					}
				break;

				case IDC_SELEDGE:
					if ( pPFDMod->m_dwSelLevel == SELECT_EDGE )	
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_OBJECT );

					}
					else
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_EDGE );
					}
				break;

				case IDC_SELFACE:
					if ( pPFDMod->m_dwSelLevel == SELECT_FACE ) 
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_OBJECT );
					}
					else 
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_FACE );
					}
				break;

				case IDC_SELPOLY:
					if ( pPFDMod->m_dwSelLevel == SELECT_POLY ) 
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_OBJECT );
					}
					else
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_POLY );
					}
				break;

				case IDC_SELELEMENT:
					if (pPFDMod->m_dwSelLevel == SELECT_ELEMENT) 
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_OBJECT );
					}
					else
					{
						pPFDMod->ip->SetSubObjectLevel( SELECT_ELEMENT ); 
					}
				break;
			
				case IDC_SEL_BYVERT_CHK:
					pPFDMod->m_bSelByVert = IsDlgButtonChecked( hWnd, IDC_SEL_BYVERT_CHK );
				break;

				case IDC_IGNORE_BACKFACES_CHK:
					pPFDMod->m_bIgnoreBackfaces = IsDlgButtonChecked( hWnd, IDC_IGNORE_BACKFACES_CHK );
				break;

				case IDC_IGNORE_VISEDGE_CHK:
					pPFDMod->m_bIgnoreVisEdge = IsDlgButtonChecked( hWnd, IDC_IGNORE_VISEDGE_CHK );
				break;

				case IDC_SHOWNORM_CHK:
					switch ( pPFDMod->m_dwSelLevel ) 
					{
						case SELECT_VERTEX: 
							pPFDMod->m_bShowVNormals = IsDlgButtonChecked( hWnd, IDC_SHOWNORM_CHK ) ? TRUE : FALSE;
							pPFDMod->SetEnableStates();
							pPFDMod->ShowNormals();
						break;

						case SELECT_FACE:
						case SELECT_POLY:
						case SELECT_ELEMENT:
							pPFDMod->m_bShowFNormals = IsDlgButtonChecked( hWnd, IDC_SHOWNORM_CHK ) ? TRUE : FALSE;
							pPFDMod->SetEnableStates();
							pPFDMod->ShowNormals();
						break;
					}

			}
		break;

		case WM_DESTROY:
			pPFDMod->ShowNormals();

			// ---release all the custom controls in the rollup
			iToolbar		=	GetICustToolbar( GetDlgItem ( hWnd, IDC_SELTYPE ) );

			spin_pThresh	=	GetISpinner( GetDlgItem( hWnd, IDC_PLANTHRESH_SPIN ) );
			spin_nScale		=	GetISpinner( GetDlgItem( hWnd, IDC_NORMSCALE_SPIN ) );

			edit_pThresh	=	GetICustEdit( GetDlgItem( hWnd, IDC_PLANTHRESH_BOX ) );
			edit_nScale		=	GetICustEdit( GetDlgItem( hWnd, IDC_NORMSCALE_BOX ) );


			ReleaseICustToolbar	( iToolbar );
			ReleaseISpinner		( spin_pThresh );
			ReleaseICustEdit	( edit_pThresh );
			ReleaseISpinner		( spin_nScale );
			ReleaseICustEdit	( edit_nScale );
		break;

		case WM_NOTIFY:
		// ---this is where we provide the tooltip text for the toolbar buttons... 
			if ( ( ( LPNMHDR )lParam )->code != TTN_NEEDTEXT ) break;
			LPTOOLTIPTEXT lpttt;
			lpttt = (LPTOOLTIPTEXT)lParam;				
			switch (lpttt->hdr.idFrom) 
			{
				case IDC_SELVERTEX:
					lpttt->lpszText = GetString( IDS_VERTEX );
				break;

				case IDC_SELEDGE:
					lpttt->lpszText = GetString( IDS_EDGE );
				break;

				case IDC_SELFACE:
					lpttt->lpszText = GetString( IDS_FACE );
				break;

				case IDC_SELPOLY:
					lpttt->lpszText = GetString( IDS_POLY );
				break;
				
				case IDC_SELELEMENT:
					lpttt->lpszText = GetString( IDS_ELEMENT );
				break;	
			}
		break;
		default: return FAIL;
	}
	return SUCCESS;
}


static BOOL CALLBACK InfoProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
{	

	CPerFaceDataMod *pPFDMod = (CPerFaceDataMod*)GetWindowLongPtr(hWnd,GWLP_USERDATA);


	switch (msg) 
	{
		case WM_INITDIALOG:
			pPFDMod = (CPerFaceDataMod*)lParam;
			pPFDMod->m_hInfo = hWnd;
			SetWindowLongPtr( hWnd, GWLP_USERDATA, lParam );

			if ( pPFDMod )
			{
				// ---update the rollup state
//				pPFDMod->m_bRSinfo = IsRollupPanelOpen( pPFDMod->m_hInfo );
			}
			return TRUE;

		case WM_PAINT:
			return FALSE;

		case WM_DESTROY:
			break;

		case WM_COMMAND:
			break;

		case WM_NOTIFY:
			break;
	}
	return TRUE;
}

// -----------------------------------------------------------------------
// ***				SOSImageHandler Method Defn's					   ***
// -----------------------------------------------------------------------
HIMAGELIST SOSelImageHandler::LoadImages() 
{
	// ---manages bitmap resources for the sub-object type custom toolbar
	if ( images ) return images;

	HBITMAP hBitmap, hMask;

	// ---create a new image list
	images = ImageList_Create(24, 23, ILC_COLOR|ILC_MASK, 15, 0);

	// ---load the specified bitmap and mask resources from a module’s executable file
	hBitmap	= LoadBitmap( hInstance, MAKEINTRESOURCE( IDB_SELTYPES ) );
	hMask	= LoadBitmap ( hInstance, MAKEINTRESOURCE( IDB_SELMASK ) );

	// add images to the image list
	ImageList_Add (images, hBitmap, hMask);

	DeleteObject (hBitmap);
	DeleteObject (hMask);

	return images;
}




















