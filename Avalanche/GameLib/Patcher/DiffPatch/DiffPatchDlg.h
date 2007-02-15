// DiffPatchDlg.h : header file
//

#pragma once

#include <iostream>
#include <fstream>
#include <ios>
#include <algorithm>
#include <string>
#include <vector>
#include <deque>
#include "afxcmn.h"
#include "afxwin.h"
#include "RPatchCommon/RPatchCompiler.h"


// CDiffPatchDlg dialog
class CDiffPatchDlg : public CDialog
{
// Construction
public:
	CDiffPatchDlg(CWnd* pParent = NULL);	// standard constructor
	~CDiffPatchDlg();

// Dialog Data
	enum { IDD = IDD_DIFFPATCH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	RPatchCompiler *patchCompiler;

	void GenericFileOpen(CString &path);
	void ReadDefaults();
	bool BrowseForFolder(CString& outputStr);
	void PutStatus(const CString& str);

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
	afx_msg void OnBnClickedBrowse1();
	afx_msg void OnBnClickedBrowse2();
	afx_msg void OnBnClickedBrowse3();
	afx_msg void OnBnClickedGeneratepatch();
	CProgressCtrl m_progressBar;
	CString m_statusBox;
	CString m_originalDir;
	CString m_modifiedDir;
	CListCtrl m_listCtrl;
	afx_msg void OnBnClickedDiffbutton();
	CString m_patchPath;
	afx_msg void OnBnClickedSelall();
	afx_msg void OnBnClickedSelnone();
	CString m_finalPatchSize;
	CString m_versionString;
	UINT	m_versionNumber;
	afx_msg void OnEnUpdateVersionEdit();
};
