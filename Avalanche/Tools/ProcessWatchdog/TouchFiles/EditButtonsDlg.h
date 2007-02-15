#pragma once


// EditButtonsDlg dialog

class EditButtonsDlg : public CDialog
{
	DECLARE_DYNAMIC(EditButtonsDlg)

public:
	EditButtonsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~EditButtonsDlg();

// Dialog Data
	enum { IDD = IDD_EDIT_BUTTONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_name1;
	CString m_name2;
	CString m_name3;
	CString m_name4;
	CString m_path1;
	CString m_path2;
	CString m_path3;
	CString m_path4;
	afx_msg void OnBnClickedButtonBrowse1();
	afx_msg void OnBnClickedButtonBrowse2();
	afx_msg void OnBnClickedButtonBrowse3();
	afx_msg void OnBnClickedButtonBrowse4();
};
