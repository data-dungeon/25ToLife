#ifndef __MEDIALOG_H
#define __MEDIALOG_H

#include "SDK\Misc.h"
#include "Resource.h"

class TClampParameters;
class CTClampDialog : public CDialog
{
// Construction
public:
	TClampParameters *m_parameters;

	CTClampDialog(){};   // standard constructor
	CTClampDialog(TClampParameters *parameters);

// Dialog Data
	//{{AFX_DATA(CDupDialog)
	enum { IDD = IDD_TCLAMP };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDupDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
