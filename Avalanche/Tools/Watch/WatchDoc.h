// WatchDoc.h : interface of the CWatchDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATCHDOC_H__7A7682CE_BDD6_443E_9E57_B090351A01FA__INCLUDED_)
#define AFX_WATCHDOC_H__7A7682CE_BDD6_443E_9E57_B090351A01FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWatchDoc : public CDocument
{
protected: // create from serialization only
	CWatchDoc();
	DECLARE_DYNCREATE(CWatchDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWatchDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWatchDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHDOC_H__7A7682CE_BDD6_443E_9E57_B090351A01FA__INCLUDED_)
