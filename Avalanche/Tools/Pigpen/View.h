/************************************/
/*                                  */
/* View.h                           */
/* Pigpen particle tool  12/07/99   */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* particle list view               */
/*                                  */
/************************************/

#ifndef __VIEW_H
#define __VIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include <Afxcview.h>

#include "Doc.h"

/******************* defines *****************************/

/* effect view columns */

#define EFFECT_NAME_COLUMN						0
#define EFFECT_PART1_COLUMN					1
#define EFFECT_PART2_COLUMN					2
#define EFFECT_PART3_COLUMN					3
#define EFFECT_PART4_COLUMN					4
#define EFFECT_PART5_COLUMN					5
#define EFFECT_ASSOCIATED_COLUMN				6
#define EFFECT_USED_COUNT_COLUMN				7

/* particle view columns */

#define PARTICLE_NAME_COLUMN					0
#define PARTICLE_GRAPHIC_COLUMN				1
#define PARTICLE_ASSOCIATED_COLUMN			2
#define PARTICLE_USED_COUNT_COLUMN			3

/* graphic view columns */

#define GRAPHIC_NAME_COLUMN					0
#define GRAPHIC_FRAMES_COLUMN					1
#define GRAPHIC_SIZE_COLUMN					2
#define GRAPHIC_IMPORT_PATH_COLUMN			3
#define GRAPHIC_USED_COUNT_COLUMN			4
#define GRAPHIC_EXPORT_SIZE_COLUMN			5

/* emitter view columns */

#define EMITTER_NAME_COLUMN					0
#define EMITTER_EFFECT_COLUMN					1
#define EMITTER_ASSOCIATED_COLUMN			2
#define EMITTER_USED_COUNT_COLUMN			3

/* swoosh view columns */

#define SWOOSH_NAME_COLUMN						0
#define SWOOSH_GRAPHIC_COLUMN					1
#define SWOOSH_ASSOCIATED_COLUMN				2
#define SWOOSH_USED_COUNT_COLUMN				3

/* swoosh emitter view columns */

#define SWOOSH_EMITTER_NAME_COLUMN			0
#define SWOOSH_EMITTER_SWOOSH_COLUMN		1
#define SWOOSH_EMITTER_ASSOCIATED_COLUMN	2
#define SWOOSH_EMITTER_USED_COUNT_COLUMN	3

/* max edit windows per view (for example, particle view has particle sheet and envelope dialog windows) */

#define EDIT_WINDOW0								0
#define EDIT_WINDOW1								1
#define MAX_EDIT_WINDOWS_PER_VIEW			2

/* edit window types */

#define NO_DIALOG									-1
#define EFFECT_DIALOG							0
#define PARTICLE_DIALOG							1
#define ENVELOPE_DIALOG							2	//obsolete
#define GRAPHIC_DIALOG							3
#define EMITTER_DIALOG							4
#define SWOOSH_DIALOG							5
#define SWOOSH_EMITTER_DIALOG					6

/******************* class *******************************/

class CDocView : public CListView
{
protected: // create from serialization only
	CDocView();
	DECLARE_DYNCREATE(CDocView)

// Attributes
public:
	CPigpenDoc* GetDocument();

// Operations
public:

	void ItemToView(int nIndex, bool bAdd);	// add or refresh item from doc to view
	int GetSelectedItemIndex(void);				// index of first selected item, -1 if none selected
	void UpdateEditWindow(void);					// update dialog/property sheet when selection changes
	void EditWindowClosed(int nEditWindow);	// communication from edit window
	CWnd *GetEditWindow(int nEditWindow) {return((CWnd *) m_pEditWindow[nEditWindow]);}
	int EditWindowOpenCount();
	void OnEdit0();
	void OnEdit1();
	int GetEditWindowType(int nEditWindow);
	void FindAndSelectItem(CObject *pItemRecord);
	void CloseAllBut(int nEditWindowType);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocView)
	public:
	virtual void OnInitialUpdate();	// add columns to list view
	protected:
	virtual void PostNcDestroy();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~CDocView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	int m_nType;						// view type, e.g. effect view, particle view
	int m_nClipboardFormat;			// format for clipboard data
	int m_nClipboardRecordSize;	// size of a single item (e.g. sizeof(CParticle))
	void *m_pEditWindow[MAX_EDIT_WINDOWS_PER_VIEW];	// modeless editing window, may be a dialog or a property sheet, depending on view type
	bool m_labelEditActive;			// for text cut and paste

// Generated message map functions
protected:
	//{{AFX_MSG(CDocView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);		// right mouse button brings up popup menu
	afx_msg void OnNew();
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnDelete();
	afx_msg void OnRename();
	afx_msg void OnMoveToTop();
	afx_msg void OnMoveToBottom();
	afx_msg void OnPreview();
	afx_msg void OnFindAssociates();
	afx_msg void OnRepairClearStage();
	afx_msg void OnRepairClearEmitterModel();
	afx_msg void OnCloseAllButEffects();
	afx_msg void OnCloseAllButGraphics();
	afx_msg void OnCloseAllButParticles();
	afx_msg void OnCloseAllButEmitters();
	afx_msg void OnCloseAllButSwooshes();
	afx_msg void OnCloseAllButSwooshEmitters();
	afx_msg void OnUpdateCloseAllButEffects(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCloseAllButGraphics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCloseAllButParticles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCloseAllButEmitters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCloseAllButSwooshes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCloseAllButSwooshEmitters(CCmdUI* pCmdUI);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(UINT id, NMHDR* pNotifyStruct, LRESULT* pResult);	// clicking on column headers
public:
	afx_msg void OnDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdit(int nEditWindow);
	//}}AFX_MSG

	BOOL DoCut();
	BOOL DoCopy();
	BOOL DoPaste();
	BOOL CopyText();
	BOOL CutText();
	BOOL PasteText();
	CObject *NewDocument();

protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version
inline CPigpenDoc* CDocView::GetDocument()
   { return (CPigpenDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif //__VIEW_H
