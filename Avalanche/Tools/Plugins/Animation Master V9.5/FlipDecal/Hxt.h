// KB  6/4/01  \Ken90\Plugin\HXT\Duplicator\Hxt.h
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __HXT_H
#define __HXT_H

#include "resource.h"		// main symbols
#include "Matrix34.h"
#include "SDK\HPropert.h"
#include "SDK\HPropDri.h"
#include "SDK\HPatch.h"
#include "SDK\HModel.h"
#include "SDK\HDecal.h"

#ifndef __HashTools_H
#include "../hashtools/hashtools.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CHxtApp
// See Hxt.cpp for the implementation of this class
//
class HModelCache;
class HGroupCP;
class HGroup;

class CHxtApp : public CWinApp
{
public:
	CHxtApp();

	BOOL		OnFlipDecal(HTreeObject *htreeobject);
	BOOL		Evaluate(HTreeObject *htreeobject);

	Vector2		FlipUV(Vector2 uv,const Vector2 midpoint);
	void		FlipUVs(Vector2 *uvs,const Vector2 midpoint);

	Vector2		CalcBoundingBoxMidpoint(HashBasePatchUVs &patchuvs);

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