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

#define VERSION 2L

#ifndef __HPROPERT_H
#include "SDK\HPropert.h"
#endif

#ifndef __PropertyTree_H
#include "propertytree.h"
#endif

#include "hashfile.h"

#ifndef __CollectData_H
#include "collectdata.h"
#endif

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
class HGroup;
class HDecal;
class HDecalStamp;
class HBasePatchUV;
class HCP;

//#define KEEPLOG

class CHxtApp : public CWinApp
{
public:

	CHxtApp();
	~CHxtApp()
	{
		for (int i = 0; i < m_typeinfoarray2.size(); i++)
			delete m_typeinfoarray2[i];
	};

	BOOL AddModelProperties(HModelCache *modelcache, HDynamicCategoryProperty *pluginproperty);
	BOOL AddGroupProperties(HGroup *group, HDynamicCategoryProperty *pluginproperty);

	void AddProperty(PropertyNode *node,PropertyTree *tree,HDynamicCategoryProperty *pluginproperties);
	void AddProperties(PropertyNode *node,PropertyTree *tree,HDynamicCategoryProperty *pluginproperties);

//	int AddVertexToPool(Vector &v,std::vector<Vector> &vpool);

 	void ReportImportError(const String &filename);
 	void ReportExportError(const String &filename);

	BOOL OnExportModelCache(HModelCache *hmc);
	BOOL OnExportObjectCacheContainer(HObjectCacheContainer *hocc);
	BOOL OnExportActionCache(HActionCache *hac);
	BOOL OnExportActionCacheContainer(HActionCacheContainer *hocc);
	BOOL OnExportChor(HChor *hchor);

	LONG CountBones(IEPatchModel *model);
	BOOL ExportAV4(IEPatchModel *model, HModelCache *hmc, const String &filename);
	void SaveMaterials(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
	void SaveShaders(FileStream &fs, IEPatchModel *model, HModelCache *hmc, LONG &totalitems, LONG &curritem);
	void SaveAttributes(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
	void SaveTextureMaps(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
	void SaveVertexList(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
	void SaveControlVertexList(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);
	void SavePatches(FileStream &fs, IEPatchModel *model, LONG &totalitems, LONG &curritem);

	void SaveBones(FileStream &fs,LONG &totalitems, LONG &curritem);
	void SaveBoneBranch(FileStream &fs,myBone *mybone,LONG &totalitems, LONG &curritem);

	BOOL OnImportModelCache(HModelCache *hmc);
	BOOL OnImportObjectCacheContainer(HObjectCacheContainer *hocc);
	IEPatchModel *ImportMPF(const String &filename);
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

#ifdef KEEPLOG
	public:
		ofstream log;
#endif

public:
	IEModelParms m_iemp;
	HProgressBar *m_hprogressbar;

//	these should stick around
	PropertyTree m_GroupProperties,m_ModelProperties;
	std::vector<TypeInfoArray*> m_typeinfoarray2;

	// this is for keeping track of vertices showing up on multiple bones
	IntMap m_vbonemap;

	CollectData m_collectdata;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IE_H__58C0982D_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
