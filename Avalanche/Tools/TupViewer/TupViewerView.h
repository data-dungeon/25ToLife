// TupViewerView.h : interface of the CTupViewerView class
//


#pragma once


class CTupViewerView : public CView
{
protected: // create from serialization only
	CTupViewerView();
	DECLARE_DYNCREATE(CTupViewerView)

// Attributes
public:
	CTupViewerDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CTupViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TupViewerView.cpp
inline CTupViewerDoc* CTupViewerView::GetDocument() const
   { return reinterpret_cast<CTupViewerDoc*>(m_pDocument); }
#endif

