// TupViewerDoc.h : interface of the CTupViewerDoc class
//


#pragma once

class CTupViewerDoc : public CDocument
{
protected: // create from serialization only
	CTupViewerDoc();
	DECLARE_DYNCREATE(CTupViewerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CTupViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void TriggerTotalRefresh(void);
	TupperwareAggregate *GetTupData(void);

protected:
	TupperwareAggregate *m_pAgg;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

};


