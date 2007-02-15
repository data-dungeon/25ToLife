/***********************************/
/*                                 */
/* Doc.h                           */
/* Pigpen particle tool  12/07/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle tool document defn     */
/*                                 */
/***********************************/

#ifndef __PIGPENDOC_H
#define __PIGPENDOC_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "Effect.h"
#include "Particle.h"
#include "Graphic.h"
#include "Image.h"
#include "Emitter.h"
#include "Swoosh.h"
#include "SwooshEmitter.h"
#include "Envelope.h"
#include "View.h"
#include "Surface.h"
#include "Spawn.h"

/******************* defines *****************************/

/* define overall doc version to be sum of all other data versions. that way, when any version increments, the
overall doc version will, too. the doc version should always increment when any data component version increments
so that an old executable can recognize a version it can't handle (and bail) before getting down to parsing the
lower-level data items. all version numbers are integers, for serialization */

#define DOC_VERSION	3	// .pig doc contents
#define PIGPEN_VERSION	(DOC_VERSION + EFFECT_VERSION + PARTICLE_VERSION + GRAPHIC_VERSION + IMAGE_VERSION + ENVELOPE_VERSION + EMITTER_VERSION + SURFACE_VERSION + SPAWN_VERSION + SWOOSH_VERSION + SWOOSH_EMITTER_VERSION)

/* data types. this is a CObArray for each data type, and an associated view */

enum	{	EFFECTS = 0,
			PARTICLES,
			GRAPHICS,
			EMITTERS,
			SWOOSHES,
			SWOOSH_EMITTERS,

			/* additional types must always come at end of list for earlier version compatibility */

			MAX_DOC_DATA_TYPES		// must be last
		};

/* popup menus use the above enum for selecting the correct sub-menu. there are also some other popup menus */

enum	{	GRAPHIC_DIALOG_POPUP_MENU = MAX_DOC_DATA_TYPES,
			EFFECT_DIALOG_POPUP_MENU,
		};

/* flags for UpdateAllViews hint. default hint of 0 should always rebuild all views */

#define NO_CHANGE_EFFECTS_VIEW				0x00000001
#define NO_REORDER_EFFECTS_VIEW				0x00000002
#define NO_CHANGE_PARTICLES_VIEW				0x00000004
#define NO_REORDER_PARTICLES_VIEW			0x00000008
#define NO_CHANGE_GRAPHICS_VIEW				0x00000010
#define NO_REORDER_GRAPHICS_VIEW				0x00000020
#define NO_CHANGE_EMITTERS_VIEW				0x00000040
#define NO_REORDER_EMITTERS_VIEW				0x00000080
#define NO_CHANGE_SWOOSHES_VIEW				0x00000100
#define NO_REORDER_SWOOSHES_VIEW				0x00000200
#define NO_CHANGE_SWOOSH_EMITTERS_VIEW		0x00000400
#define NO_REORDER_SWOOSH_EMITTERS_VIEW	0x00000800

#define NO_REORDER_ALL_VIEWS					(NO_REORDER_EFFECTS_VIEW | NO_REORDER_PARTICLES_VIEW | NO_REORDER_GRAPHICS_VIEW | NO_REORDER_EMITTERS_VIEW | NO_REORDER_SWOOSHES_VIEW | NO_REORDER_SWOOSH_EMITTERS_VIEW)

/* flag value to be used with SetModifiedFlag. typically TRUE, but for development it is sometimes handy
to not have the "save document" prompt appear. always TRUE for a release build */

#ifdef _DEBUG
//#define MODIFIED_FLAG	TRUE
#define MODIFIED_FLAG	FALSE
#else //_DEBUG
#define MODIFIED_FLAG	TRUE
#endif //_DEBUG

/* export status values. before export, database is analyzed to weed out unused/empty data items */

#define EXPORT_STATUS_UNKNOWN					0
#define EXPORT_STATUS_VALID					1
#define EXPORT_STATUS_INVALID					2

/* return values from TestAssociate for better log reporting */

#define ASSOCIATE_NONE										0
#define ASSOCIATE_EMITTER_USES_EFFECT					1
#define ASSOCIATE_EFFECT_USED_BY_EMITTER				2
#define ASSOCIATE_EFFECT_USES_PARTICLE					3
#define ASSOCIATE_PARTICLE_USED_BY_EFFECT				4
#define ASSOCIATE_PARTICLE_PIGGYBACKS_EMITTER		5
#define ASSOCIATE_EMITTER_PIGGYBACKED_BY_PARTICLE	6
#define ASSOCIATE_PARTICLE_SPAWNS_EFFECT				7
#define ASSOCIATE_EFFECT_SPAWNED_BY_PARTICLE			8
#define ASSOCIATE_PARTICLE_USES_GRAPHIC				9
#define ASSOCIATE_GRAPHIC_USED_BY_PARTICLE			10
#define ASSOCIATE_SWOOSH_USES_GRAPHIC					11
#define ASSOCIATE_GRAPHIC_USED_BY_SWOOSH				12
#define ASSOCIATE_SWOOSH_EMITTER_USES_SWOOSH			13
#define ASSOCIATE_SWOOSH_USED_BY_SWOOSH_EMITTER		14

/******************* forward declarations ****************/

class CDocView;

/******************* class *******************************/

class CPigpenDoc : public CDocument
{
protected: // create from serialization only
	CPigpenDoc();
	DECLARE_DYNCREATE(CPigpenDoc)

// Attributes
public:

// Operations
public:

	/* basic CObArray processing */

	CObArray *GetArrays(void) { return(m_Arrays); }
	CObject *GetArrayItem(int nType, int nIndex) { return(m_Arrays[nType].GetAt(nIndex)); }
	int GetArraySize(int nType) { return(m_Arrays[nType].GetSize()); }
	CObject *NewItem(int nType);
	void RenameItem(int nType, int nIndex, char *pName);

	/* simple effect routines that can be defined here */

	CEffect *GetEffect(int nIndex) { return((CEffect *) GetArrayItem(EFFECTS, nIndex)); }
	int GetEffectCount(void) { return(GetArraySize(EFFECTS)); }

	/* simple particle routines that can be defined here */

	CParticle *GetParticle(int nIndex) { return((CParticle *) GetArrayItem(PARTICLES, nIndex)); }
	int GetParticleCount(void) { return(GetArraySize(PARTICLES)); }

	/* simple graphic routines that can be defined here */

	CGraphic *GetGraphic(int nIndex) { return((CGraphic *) GetArrayItem(GRAPHICS, nIndex)); }
	int GetGraphicCount(void) { return(GetArraySize(GRAPHICS)); }

	/* simple emitter routines that can be defined here */

	CEmitter *GetEmitter(int nIndex) { return((CEmitter *) GetArrayItem(EMITTERS, nIndex)); }
	int GetEmitterCount(void) { return(GetArraySize(EMITTERS)); }

	/* simple swoosh routines that can be defined here */

	CSwoosh *GetSwoosh(int nIndex) { return((CSwoosh *) GetArrayItem(SWOOSHES, nIndex)); }
	int GetSwooshCount(void) { return(GetArraySize(SWOOSHES)); }

	/* simple swoosh emitter routines that can be defined here */

	CPigpenSwooshEmitter *GetSwooshEmitter(int nIndex) { return((CPigpenSwooshEmitter *) GetArrayItem(SWOOSH_EMITTERS, nIndex)); }
	int GetSwooshEmitterCount(void) { return(GetArraySize(SWOOSH_EMITTERS)); }

	/* generic list handling routines-- generally use a CObArray/CListCtrl pair */

	int *CollectSelectedItems(int nType, CListCtrl& List, int& nIndexCount);
	bool DeleteSelectedItems(int nType, CListCtrl& List);
	void MoveSelectedItemsToTop(int nType, CListCtrl& List);
	void MoveSelectedItemsToBottom(int nType, CListCtrl& List);
	void SortArray(int nType, int nColumn);

	/* list special processing */

	void ClearEffectReferences(int *pIndexArray, int nIndexCount);
	void ClearParticleReferences(int *pIndexArray, int nIndexCount);
	void ClearGraphicReferences(int *pIndexArray, int nIndexCount);
	void ClearSwooshReferences(int *pIndexArray, int nIndexCount);
	CEffect *AddDummyEffect(void);
	void DeleteDummyEffect(void);
	CEmitter *AddDummyEmitter(void);
	void DeleteDummyEmitter(void);
	CPigpenSwooshEmitter *AddDummySwooshEmitter(void);
	void DeleteDummySwooshEmitter(void);
	void SetParticleSource(void *pData) { m_pParticleSource = pData; }
	void EnforceUniqueName(CObject *pObject, int nType);
	void FindAssociates(CObject *pSeedObject, int nType);
	void TestAssociate(CObject *pTestObject, int nType);

	/* view tracking */

	void ViewOpen(CDocView *pView, int nType) { if (m_pView[nType] == NULL) m_pView[nType] = pView; }
	void ViewClosed(int nType) { m_pView[nType] = NULL; }
	CDocView *GetView(int nType) { return(m_pView[nType]); }
	void OnView(int nType);

	/* window restoring on file load */

	void OpenLoadedWindows(void);

	/* collision surface type list */

	int GetSurfaceIndex()					{ return(m_nSurfaceTypeIndex); }
	void SetSurfaceIndex(int nIndex)		{ ASSERT(nIndex < GetSurfaceCount()); m_nSurfaceTypeIndex = nIndex; }
	int GetPredefinedSurfaceCount()		{ return(m_nPredefinedSurfaceCount); }
	int GetEndOfLifeSurfaceCount()		{ return(m_nEndOfLifeSurfaceCount); }
	int GetSurfaceCount()					{ return(m_SurfaceTypes.GetSize()); }
	CSurface *GetSurface(int nIndex)		{ return((CSurface *) m_SurfaceTypes.GetAt(nIndex)); }
	int SurfaceExists(CString &Name);
	CSurface *CurrentSurface()				{ return((CSurface *) m_SurfaceTypes.GetAt(m_nSurfaceTypeIndex)); }
	void AddSurface(CSurface *pSurface);
	void DeleteSurface();

	/* export */

	void ExportDBLFile(int nMode, char *pExportFileName = NULL);

	/* access routines */

	void SetStageFile(CString &File);
	CString &GetStageFile(void) { return(m_StageFile); }
	void SetEmitterModelFile(CString &File) { m_EmitterModelFile = File; SetModifiedFlag(MODIFIED_FLAG); }
	CString &GetEmitterModelFile(void) { return(m_EmitterModelFile); }
	void SetBackgroundColor(COLORREF Color) { m_BackgroundColor = Color; SetModifiedFlag(MODIFIED_FLAG); }
	COLORREF GetBackgroundColor(void) { return(m_BackgroundColor); }
	void SetCameraPos(Vector3CRef vCameraPos, Vector3CRef vCameraRot);
	void GetCameraPos(Vector3 *pvCameraPos, Vector3 *pvCameraRot);
	void SetEmitterModelXPos(float fXPos) { m_fEmitterModelXPos = fXPos; SetModifiedFlag(MODIFIED_FLAG); }
	float GetEmitterModelXPos(void) { return(m_fEmitterModelXPos); }
	void SetEmitterModelYPos(float fYPos) { m_fEmitterModelYPos = fYPos; SetModifiedFlag(MODIFIED_FLAG); }
	float GetEmitterModelYPos(void) { return(m_fEmitterModelYPos); }
	void SetEmitterModelZPos(float fZPos) { m_fEmitterModelZPos = fZPos; SetModifiedFlag(MODIFIED_FLAG); }
	float GetEmitterModelZPos(void) { return(m_fEmitterModelZPos); }
	void SetEmitterModelYRot(float fYRot) { m_fEmitterModelYRot = fYRot; SetModifiedFlag(MODIFIED_FLAG); }
	float GetEmitterModelYRot(void) { return(m_fEmitterModelYRot); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPigpenDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument( LPCTSTR lpszPathName );
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPigpenDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	unsigned long m_ulPigpenVersion;
	unsigned long m_ulDocVersion;
	CObArray m_Arrays[MAX_DOC_DATA_TYPES];	// effects, particles, etc. each list has an associated view type

	/* document views, one for each data type */

	CDocView *m_pView[MAX_DOC_DATA_TYPES];

	/* for restoring window position/status when opening a file. for example, referenced by CDocView::OnInitialUpdate */

	int m_nViewOpen[MAX_DOC_DATA_TYPES];
	int m_nEditOpen[MAX_DOC_DATA_TYPES][MAX_EDIT_WINDOWS_PER_VIEW];
	CRect m_ViewRect[MAX_DOC_DATA_TYPES];
	CRect m_EditRect[MAX_DOC_DATA_TYPES][MAX_EDIT_WINDOWS_PER_VIEW];
	SCROLLINFO m_ViewHorzScroll[MAX_DOC_DATA_TYPES];
	SCROLLINFO m_ViewVertScroll[MAX_DOC_DATA_TYPES];

	/* collision surface type list */

	int m_nSurfaceTypeIndex;
	CObArray m_SurfaceTypes;

	/* preview prefs */

	CString m_StageFile;							// where stage .dbl can be found. default is <exe dir>\stage.dbl
	int m_bDefaultStageFile;
	CString m_EmitterModelFile;				// where emitter model .dbl can be found. default is ""-- none
	float m_fEmitterModelXPos;
	float m_fEmitterModelYPos;
	float m_fEmitterModelZPos;
	float m_fEmitterModelYRot;
	COLORREF m_BackgroundColor;
	float m_fInitCameraPosX;
	float m_fInitCameraPosY;
	float m_fInitCameraPosZ;
	float m_fInitCameraRotX;
	float m_fInitCameraRotY;

	/* not serialized */

	void *m_pParticleSource;			// pointer to single particle's source data for particle preview mode
	int *m_pnSurfaceTranslation;		// pointer to a translation table that tells how to map file spawn records to surface table
	int m_nPredefinedSurfaceCount;	// number of pre-defined surfaces in CPigpenDoc::CPigpenDoc()
	int m_nEndOfLifeSurfaceCount;		// number of pre-defined end-of-life surfaces in CPigpenDoc::CPigpenDoc()

	/* for associate code */

	FILE *m_pAssociateLog;

// Generated message map functions
protected:
	//{{AFX_MSG(CPigpenDoc)
	afx_msg void OnViewEffects() {OnView(EFFECTS);}
	afx_msg void OnUpdateViewEffects(CCmdUI* pCmdUI);
	afx_msg void OnViewParticles() {OnView(PARTICLES);}
	afx_msg void OnUpdateViewParticles(CCmdUI* pCmdUI);
	afx_msg void OnViewGraphics() {OnView(GRAPHICS);}
	afx_msg void OnUpdateViewGraphics(CCmdUI* pCmdUI);
	afx_msg void OnViewEmitters() {OnView(EMITTERS);}
	afx_msg void OnUpdateViewEmitters(CCmdUI* pCmdUI);
	afx_msg void OnViewSwooshes() {OnView(SWOOSHES);}
	afx_msg void OnUpdateViewSwooshes(CCmdUI* pCmdUI);
	afx_msg void OnViewSwooshEmitters() {OnView(SWOOSH_EMITTERS);}
	afx_msg void OnUpdateViewSwooshEmitters(CCmdUI* pCmdUI);
	afx_msg void OnViewExportLog();
	afx_msg void OnUpdateViewExportLog(CCmdUI* pCmdUI);
	afx_msg void OnFileExportPC();
	afx_msg void OnFileExportPS2();
	afx_msg void OnFileExportGCN();
	afx_msg void OnFileAnalyze();
	afx_msg void OnTextureAnalyze();
	afx_msg void OnFileMerge();
	afx_msg void OnSetAssociateLogFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PIGPENDOC_H
