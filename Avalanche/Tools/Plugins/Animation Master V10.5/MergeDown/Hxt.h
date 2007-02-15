// Dan  6/23/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\MergeDown\Hxt.h
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __HXT_H
#define __HXT_H

#include "resource.h"		// main symbols
#include "HashTime.h"

class HAction;
class HHashObject;

/////////////////////////////////////////////////////////////////////////////
// CHxtApp
// See Hxt.cpp for the implementation of this class
class CHxtApp : public CWinApp
{
public:
	CHxtApp();

	BOOL MergeDown(HAction *haction);
   void CopyAction(HHashObject *srcparent, HHashObject *dstparent, HAction *action);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHxtApp)
	//}}AFX_VIRTUAL
	virtual BOOL InitInstance();

	//{{AFX_MSG(CHxtApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
#endif __HXT_H