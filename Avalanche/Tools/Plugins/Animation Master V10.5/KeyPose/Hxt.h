// Dan  7/12/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\KeyPose\Hxt.h
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __HXT_H
#define __HXT_H

#include "resource.h"		// main symbols
#include "KeyPose.h"

class HRelation;
class HHashObject;
class ModelExt;
class HDataPropertyInfo;
class HPropertyInfo;
class HModel;
class HCategoryProperty;
class HDataProperty;

/////////////////////////////////////////////////////////////////////////////
// CHxtApp
// See Hxt.cpp for the implementation of this class
class CHxtApp : public CWinApp
{
public:
   CKeyPoseWnd m_wndKeyPose;
   HPropertyInfo *m_avalanchepropertyinfo;
   HDataPropertyInfo *m_datapropertyinfo;

   int
      m_avalanchepropertyindex,
         m_datapropertyindex;

   CHxtApp();

   void AllocPropertyInfos();
   void FreePropertyInfos();
   BOOL AddModelProperties(HModel *modelcache, HCategoryProperty *pluginproperty);
   HDataProperty *GetDataProperty(HModel *model);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHxtApp)
	//}}AFX_VIRTUAL
	virtual BOOL InitInstance();
   virtual int ExitInstance();

	//{{AFX_MSG(CHxtApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif __HXT_H