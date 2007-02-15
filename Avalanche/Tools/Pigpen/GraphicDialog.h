/***********************************/
/*                                 */
/* GraphicDialog.h                 */
/* Pigpen particle tool  12/20/99  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* dialog box for editing graphics */
/*                                 */
/***********************************/

#ifndef __GRAPHICDIALOG_H
#define __GRAPHICDIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"
#include "Graphic.h"

/******************* defines *****************************/

/******************* forward declarations ****************/

class CDocView;

/******************* class *******************************/

class CGraphicDialog : public CDialog
{
public:

	CGraphicDialog(CDocView *pParentView, CWnd* pParentWnd);	// constructor
	~CGraphicDialog();	// destructor
	BOOL Create();

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CGraphicDialog)
	enum { IDD = IDD_GRAPHIC_DIALOG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphicDialog)
	protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(CGraphic *pGraphic);
	void SetHighlightIndex(int nIndex);
	int GetHighlightIndex(void);
	CImage *GetHighlightedImage(void);
	void ShowHighlightedImageInfo(void);

protected:
	
	CDocView* m_pParentView;
	CWnd* m_pParentWnd;
	CGraphic *m_pGraphic;			// current graphic being edited
	CGraphic m_OriginalGraphic;	// current graphic before editing, for reverting
	int m_nHighlightIndex;			// which of image frames is highlighted

	/* display */

	void UpdateImagesAndIndices(void);

	/* import/export */

	void ImportImage(bool bReplace, bool bUpdate);

	/* dialog control processing */

	void UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases);
	void UpdateCheckBox(int nID, bool bToggle);
//	void UpdateRadioButton(int nID, bool bUpdate);

	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

	bool OnCustomdrawSlider(NMHDR* pNMHDR, LRESULT* pResult); 
	bool OnSpinner(NMHDR* pNMHDR, LRESULT* pResult);
	bool OnKillFocusEdit(int nID);
	bool OnButtonClicked(int nID);

	//{{AFX_MSG(CGraphicDialog)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);		// right mouse button brings up popup menu
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMarkIcon();
	afx_msg void OnDelete();
	afx_msg void OnDeleteAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__GRAPHICDIALOG_H
