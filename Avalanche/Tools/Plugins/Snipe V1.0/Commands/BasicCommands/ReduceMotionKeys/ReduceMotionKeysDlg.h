#pragma once

#include "../resource.h"
#include "afxwin.h"
#include "HSnipeObject.h"

// ReduceMotionKeysDlg dialog

class ReduceMotionKeysDlg : public CDialog
{
	DECLARE_DYNAMIC(ReduceMotionKeysDlg)

public:
   float m_fTolerance;

            ReduceMotionKeysDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ReduceMotionKeysDlg();

// Dialog Data
	enum { IDD = IDD_REDUCEMOTIONKEYS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

//	DECLARE_MESSAGE_MAP()
public:
 
};
