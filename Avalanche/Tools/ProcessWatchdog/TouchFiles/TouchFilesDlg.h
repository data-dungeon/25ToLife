// TouchFilesDlg.h : header file
//

#pragma once


// CTouchFilesDlg dialog
class CTouchFilesDlg : public CDialog
{
	class TouchButton
	{
	public:
		CString m_name;
		CString m_path;
	};

	TouchButton m_touchButtons[4];

	bool m_bClosing;

// Construction
public:
	CTouchFilesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TOUCHFILES_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void LoadFromRegistry(void);
	void SaveToRegistry(void);
	bool TouchFile(CString pathToFile);
	void UpdateButtons(void);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnEditEditbuttons();
	afx_msg void OnClose();

	afx_msg void OnOK();
	afx_msg void OnCancel();

};
