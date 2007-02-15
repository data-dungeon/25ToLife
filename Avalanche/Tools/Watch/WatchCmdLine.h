// WatchCmdLine.h: interface for the CWatchCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATCHCMDLINE_H__CCA7BEE5_DBAF_41C9_A0DA_C816EC9F8A36__INCLUDED_)
#define AFX_WATCHCMDLINE_H__CCA7BEE5_DBAF_41C9_A0DA_C816EC9F8A36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWatchCmdLine : public CCommandLineInfo  
{
public:
	CWatchCmdLine();
	virtual ~CWatchCmdLine();

	void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
};

#endif // !defined(AFX_WATCHCMDLINE_H__CCA7BEE5_DBAF_41C9_A0DA_C816EC9F8A36__INCLUDED_)
