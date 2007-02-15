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

#include <vector>

#ifndef __STRCLASS_H
#include "strclass.h"
#endif

class HModelCache;
class TClampParameters
{
public:
};

/////////////////////////////////////////////////////////////////////////////
// CHxtApp
// See Hxt.cpp for the implementation of this class
//
class HGroupCP;
class HGroup;
class CHxtApp : public CWinApp
{
public:
//	TClampParameters  m_parameters;

	bool SingleClamp;
	bool MultipleClampStretch;
	bool MultipleClampNoStretch;
	int BlendMapping;
	String PrimaryMapPath,SecondaryMapPath,AlphaMapPath;

	CHxtApp();

	BOOL OnTextureClamp(HTreeObject *htreeobject);
	BOOL Evaluate(HTreeObject *htreeobject);

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

	void		ClampUVToPatch(Vector2 *uvs,Vector2 upperleft,Vector2 lowerleft,Vector2 lowerright,
								Vector2 upperright);
	Vector2		calcUVWithInBoundary(double s, double t);
};


/////////////////////////////////////////////////////////////////////////////
#endif __HXT_H