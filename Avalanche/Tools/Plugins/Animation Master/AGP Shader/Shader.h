// KB  4/26/00  \Ken85\Plugin\Shaders\Anime\Shader.h
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __SHADER_H
#define __SHADER_H

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CShaderApp
// See Shader.cpp for the implementation of this class
//
class CShaderApp : public CWinApp
{
public:
	CShaderApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShaderApp)
	//}}AFX_VIRTUAL
	virtual BOOL InitInstance();

	//{{AFX_MSG(CShaderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
#endif __SHADER_H