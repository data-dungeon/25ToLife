#pragma once


// CViewHeaderWnd

class CViewHeaderWnd : public CWnd
{
	DECLARE_DYNAMIC(CViewHeaderWnd)

   enum { HEIGHT=20 };

public:
   CComboBox m_wndViewCombo;

   CViewHeaderWnd();
	virtual ~CViewHeaderWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
   BOOL Create(CWnd * pParent);
   afx_msg void OnPaint();
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


