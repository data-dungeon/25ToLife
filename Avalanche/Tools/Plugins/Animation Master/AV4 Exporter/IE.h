// IE.h : main header file for the IE DLL
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

#define VERSION 4L

/////////////////////////////////////////////////////////////////////////////
// CIEApp
// See IE.cpp for the implementation of this class
//

class ModelCacheHandle;
class ObjectCacheContainerHandle;
class FileStream;
class String;
class ProgressBarHandle;
class ActionCacheHandle;
class ActionCacheContainerHandle;

class CIEApp : public CWinApp
{
public:
	CIEApp();
   IEModelParms m_iemp;
   ProgressBarHandle *m_progressbarhandle;

 	void ReportImportError(const String &filename);
 	void ReportExportError(const String &filename);

	BOOL OnExportModelCacheHandle(ModelCacheHandle *mch);
   BOOL OnExportObjectCacheContainerHandle(ObjectCacheContainerHandle *occh);
   BOOL OnExportActionCacheHandle(ActionCacheHandle *ach);
   BOOL OnExportActionCacheContainerHandle(ActionCacheContainerHandle *occh);

   LONG CountBones(IEPatchModel *model);
   BOOL ExportAV3(IEPatchModel *model, const String &filename);
   void SaveAttributes(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
   void SaveMaterials(FileStream &fs, IEPatchModel2 *model, LONG &totalitems, LONG &curritem);
   void SaveShaders(FileStream &fs, IEPatchModel2 *model, LONG &totalitems, LONG &curritem,String fname);
   void SaveTextureMaps(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
   void SaveVertexList(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
   void SaveControlVertexList(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
   void SavePatches(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
   void SaveBones(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
   void SaveBoneBranch(FileStream &fs, IEBone *bone, LONG &totalitems, LONG &curritem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIEApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CIEApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IE_H__58C0982D_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
