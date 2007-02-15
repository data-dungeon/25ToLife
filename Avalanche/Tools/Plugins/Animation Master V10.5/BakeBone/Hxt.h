// Dan  6/23/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\MergeDown\Hxt.h
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __HXT_H
#define __HXT_H

#include "resource.h"		// main symbols
#include "HashTime.h"

class HObject;
class HAnimObject;

/////////////////////////////////////////////////////////////////////////////
// CHxtApp
// See Hxt.cpp for the implementation of this class
class CHxtApp : public CWinApp
{
public:
   BOOL m_abort;

	CHxtApp();

   void BakeFrame(HObject *srcobj, HObject *dstobj, Time time);
   BOOL BakeBone(HObject *srcobj);
   BOOL AbortBake();
   void ReduceBranch( HAnimObject *srcobj, float tolerance );
   BOOL ReplaceDrivers(HObject *srcobj, HObject *dstbj);

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