// CmdLine.h: interface for the CCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDLINE_H__83E63D40_9E8B_406D_965D_F1237FC6464C__INCLUDED_)
#define AFX_CMDLINE_H__83E63D40_9E8B_406D_965D_F1237FC6464C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCmdLine  
{
public:
	bool m_Verbose;
	bool Initialize( int argc, char *argv[]);
	CCmdLine( void);
	virtual ~CCmdLine( void);

	char			*m_pOutputFile;
	char			*m_pInputFile;

private:
	bool IsArg( const char *pArgLabel, const char *pArgValue, int iMinMatchLength) const;
};

extern CCmdLine g_CmdLine;

#endif // !defined(AFX_CMDLINE_H__83E63D40_9E8B_406D_965D_F1237FC6464C__INCLUDED_)
