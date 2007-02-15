
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AfxTempl.h"
#ifndef __STRCLASS_H
#include "StrClass.h"
#endif

class HActionCache;
class HActionCacheContainer;

class CExportActionsDialog : public CDialog
{
// Construction
public:
	virtual  ~CExportActionsDialog();
	CExportActionsDialog(HActionCacheContainer *hacc);

	enum { IDD = IDD_EXPORTACTIONS };
	CCheckListBox	m_checklistbox;

   CList<HActionCache *, HActionCache *> m_selected;
   HActionCacheContainer *m_hacc;
   float m_errortolerance;
   String m_dir;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBrowse();
	DECLARE_MESSAGE_MAP()
};

