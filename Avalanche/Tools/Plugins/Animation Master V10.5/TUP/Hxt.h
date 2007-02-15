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

#define VERSION 2L

#include <vector>

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

class KeeperIEMapNode
{
public:
	KeeperIEMapNode()
	{
		m_mapid = -1;
	}
	KeeperIEMapNode(IEMapNode *mapnode)
	{
		m_mapid = mapnode->m_mapid;
		m_repeat = mapnode->m_repeat;
		m_percent = mapnode->m_percent;
		m_flags = mapnode->m_flags;
	}

	LONG    m_mapid;
	float   m_percent;
	Vector2 m_repeat;
	DWORD  m_flags;
   
	bool operator == ( const KeeperIEMapNode &other )
	{
		if (m_mapid != other.m_mapid)
			return false;
		if (m_repeat != other.m_repeat)
			return false;
		if (m_percent != other.m_percent)
			return false;
		if (m_flags != other.m_flags)
			return false;
		return true;
	}
};

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
	
	BOOL ExportTUP(IEPolyModel *model, const String &filename);
	int ExportModel(const char *mayaname,TupperwareAggregate *pObjectAgg,IEPolyModel *model);
	int ExportImages(IEPolyModel *model);


	LONG CountBones(IEPolyModel *model);
	void SaveAttributes(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void SaveTextureMaps(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void SaveVertexList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void SaveNormalList(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void SavePolys(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void SaveBones(FileStream &fs, IEPolyModel *model, LONG &totalitems, LONG &curritem);
	void SaveBoneBranch(FileStream &fs, IEBone *bone, LONG &totalitems, LONG &curritem);

	BOOL OnImportModelCache(HModelCache *hmc);
	BOOL OnImportObjectCacheContainer(HObjectCacheContainer *hocc);
	
	IEPolyModel *ImportTUP(const String &filename);
	
	void AddTriMeshObject(TupperwareAggregate *pObjectAgg,TupperwareAggregate *pTransformAgg,int matref,IEPolyModel *model);
	int ConvertTupMatrixToMatrix44(TupperwareAggregate *pTransformAgg,Matrix44 &matrix);

	
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
public:
	TupperwareAggregate *m_pSceneAgg;
	TupScene			*m_pTupScene;
	TupNodePool			*m_pTupNodePool;
	TupObjectPool		*m_pTupObjectPool;
	TupMapPool			*m_pTupMapPool;
	TupImagePool		*m_pTupImagePool;
	TupMaterialPool		*m_pTupMaterialPool;
	TupModifierPool		*m_pTupModifierPool;
	float				m_unitratio;
	CString				m_modelname;
	Keeper<KeeperIEMapNode>	m_mapnodes;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IE_H__58C0982D_D8D2_11D3_83A5_00E0811049D9__INCLUDED_)
