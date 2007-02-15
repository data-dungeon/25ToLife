// Dan  12/27/00  \Dan90\Plugin\HXT\AV2\Hxt.h
//

#if !defined(AFX_IE_H__58C0982D_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
#define AFX_IE_H__58C0982D_D8D2_11D3_83A5_00E0811049D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "IEModel.h"

#define VERSION 2L

/////////////////////////////////////////////////////////////////////////////
// CHxtApp
// See IE.cpp for the implementation of this class
//

class HModelCache;
class HObjectCacheContainer;
class FileStream;
class String;
class HProgressBar;
class HActionCache;
class HActionCacheContainer;
class HChor;

class CHxtApp : public CWinApp
{
public:
	CHxtApp();
   IEPolyModelParms m_iemp;
   HProgressBar *m_hprogressbar;

 	void ReportImportError(const String &filename);
 	void ReportExportError(const String &filename);

	BOOL OnExportModelCache(HModelCache *hmc);
	BOOL OnExportObjectCacheContainer(HObjectCacheContainer *hocc);
	BOOL OnExportActionCache(HActionCache *hac);
	BOOL OnExportActionCacheContainer(HActionCacheContainer *hocc);
	BOOL OnExportChor(HChor *hchor);

	
	BOOL	ExportOBJ(IEPolyModel *model, const String &filename);

//	void	SaveAttributes(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void	SaveTextureMaps(FileStream &fs, FileStream &fsmat, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void	SaveVertexList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void	SaveNormalList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void	SaveUVList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void	SavePolyList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);

//	long	CountBones(IEPolyModel *model);
//	void	SaveBones(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
//	void	SaveBoneBranch(FileStream &fs, IEBone *bone, LONG &totalitems, LONG &curritem);

	BOOL OnImportModelCache(HModelCache *hmc);
	BOOL OnImportObjectCacheContainer(HObjectCacheContainer *hocc);
	IEPolyModel *ImportOBJ(const String &filename);
	void LoadBones(FileStream &fs, IEBone **parentuplink, IEBone *parent);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHxtApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHxtApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IE_H__58C0982D_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
