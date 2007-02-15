#if !defined(AFX_WATCHSLIDER_H__4632D31E_8ACD_411A_85F5_E54AAA2492BD__INCLUDED_)
#define AFX_WATCHSLIDER_H__4632D31E_8ACD_411A_85F5_E54AAA2492BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WatchSlider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWatchSlider window

#include "WatchInfo.h"
#include "WatchVar.h"
#include "Context.h"
#include "Table.h"
#include "WatchEdit.h"

class CWatchSlider : public CSliderCtrl, public ContextElement
{
// Construction
public:
	CWatchSlider();

// Operations
public:
	bool Init(CWnd*, int tab, CFont*, VAR_INFO&);
	int Tab() {return tab;}
	void MoveWindow(LPRECT);
	void ShowWindow(int nCmdShow);
	void UpdateVar();
	void UpdateSlider();
	void UpdateVar(VAR_VAL&);
	WatchVar* Variable() {return variable;}
	void SetTable(Table* t) {fTable = t;}
	void Enable(bool = true);
	void EditOnChange(const CString& str);
	void SendToApp();

	// Following is interface functions inherited from ContextElement.
	void Show(int cmdShow) {ShowWindow(cmdShow);}
	void Move(CRect& rect) {MoveWindow(rect);}
	void Refresh(VAR_VAL& val) {UpdateVar(val);}
	bool operator==(void* address) {return Variable()->GetPtr() == address;}
	int  Size() {return 25;}

	

// Attributes
protected:
	WatchVar*	variable;
	bool			textOnly;

	CStatic		namectrl;
	CWatchEdit	valctrl;

	int			tab;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWatchSlider();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWatchSlider)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	Table* fTable;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHSLIDER_H__4632D31E_8ACD_411A_85F5_E54AAA2492BD__INCLUDED_)
