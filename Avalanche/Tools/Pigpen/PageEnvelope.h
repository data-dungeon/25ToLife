/***********************************/
/*                                 */
/* PageEnvelope.h                  */
/* Pigpen particle tool  12/15/00  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* particle envelope page          */
/*                                 */
/***********************************/

#ifndef __PAGEENVELOPE_H
#define __PAGEENVELOPE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/******************* includes ****************************/

#include "resource.h"
#include "Envelope.h"
#include "ParticleSwoosh.h"

/* game engine includes */

/******************* defines *****************************/

#define NO_CONTROL_POINT			MAX_CONTROL_POINTS

/* graph dimensions */

#define GRAPH_X_OFFSET				32		// pixels from left edge of dialog's client area
#define GRAPH_Y_OFFSET				14		// pixels from top edge of dialog's client area
#define GRAPH_WIDTH					250	// width of graph area, in pixels
#define GRAPH_HEIGHT					220	// height of graph area, in pixels, including extra space above and below
#define GRAPH_PERCENT				50		// percent of GRAPH_HEIGHT given to 0..100% range. rest is divided into extra space above and below graph
#define GRAPH_ONE_Y					(GRAPH_HEIGHT * GRAPH_PERCENT / 100)	// number of pixels in 0..100% graph range
#define GRAPH_X_ORIGIN				GRAPH_X_OFFSET
#define GRAPH_Y_ORIGIN				GRAPH_Y_OFFSET + ((GRAPH_HEIGHT - GRAPH_ONE_Y) / 2) + GRAPH_ONE_Y
#define GRAPH_HIGH_Y					1.0f + (((100.0f - GRAPH_PERCENT) / 2.0f) / GRAPH_PERCENT)	// highest allowed y-value
#define GRAPH_LOW_Y					0.0f - (((100.0f - GRAPH_PERCENT) / 2.0f) / GRAPH_PERCENT)	// lowest allowed y-value
#define GRAPH_BORDER					9		// border around actual graph since drawn control points extend outside graph borders
#define GRAPH_RIGHT_BORDER			13		// right border is bigger because of clip line control points hanging off right of graph
#define GRAPH_CLIP_EXTEND_RIGHT	8		// ceiling clip line extends off right of graph a bit
#define GRAPH_CLIP_EXTEND_LEFT	6		// floor clip line extends off left of graph a bit

/* for drawing spline */

#define SPLINE_SEGMENTS				20
#define SPLINE_INTERVAL				(1.0f / (float) SPLINE_SEGMENTS)

/******************* forward declarations ****************/

class CDocView;
class CParticleSheet;
class CSwooshSheet;

/******************* class *******************************/

class CEnvelopePage : public CPropertyPage
{
public:

	CEnvelopePage(CDocView *pParentView, CParticleSheet *pParentParticleSheet, CSwooshSheet *pParentSwooshSheet);	// constructor

// Dialog Data-- helps class wizard know what controls are available
	//{{AFX_DATA(CEnvelopePage)
	enum { IDD = IDD_PAGE_ENVELOPE };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnvelopePage)
	protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

// Implementation

public:

	void ShowData(void);
	void DrawEnvelope(HDC hDC, CEnvelope *pEnvelope, HPEN hPen1, HPEN hPen2, int nLoopCount);
	void DrawClippedLine(HDC hDC, int nIndex);

protected:
	
	CDocView* m_pParentView;

	/* this page can handle ownership by a particle OR a swoosh */

	CParticleSheet *m_pParentParticleSheet;
	CSwooshSheet *m_pParentSwooshSheet;
	CParticleSwoosh m_ParticleSwoosh;	// combined particle/swoosh for coding simplicity

	/* graph area for invalidating */

	CRect m_GraphRect;
	
	/* for moving control points around on screen */

	int m_nSelectedPoint;
	int m_bGraphChanged;
	int m_nPointOffsetX;
	int m_nPointOffsetY;

	/* for use during OnPaint */

	int m_nFloorY;				// in screen y-coords
	int m_nCeilingY;			// in screen y-coords
	int m_nTopClipY;			// in screen y-coords
	int m_nBottomClipY;		// in screen y-coords
	CPoint m_GraphPoint[SPLINE_SEGMENTS + 1];
	CPoint m_ClippedPoint[4];
	
	/* dialog control processing */

//	void UpdateEditBox(int nID, int nDelta, float *pfDirectValue, bool bCheckSpecialCases);
	void UpdateCheckBox(int nID, bool bToggle);
	void UpdateRadioButton(int nID, bool bUpdate);

	/* higher-level message handlers, called from OnNotify and OnCommand overrides */

	bool OnButtonClicked(int nID);
	bool OnButtonUnpushed(int nID);

	//{{AFX_MSG(CEnvelopePage)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);		// right mouse button brings up popup menu
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //__PAGEENVELOPE_H
