// Dan  6/18/2004  \DanLP115\Plugin\HXT\Avalanche\AnimationMaster\MirrorBones\Hxt.h
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef __HXT_H
#define __HXT_H

#ifndef __HASHFILE_H
#include "HashFile.h"
#endif

#include "resource.h"		// main symbols
#include "Matrix34.h"
#include "SDK\HPropert.h"
#include "SDK\HPropDri.h"
#include "SDK\HPatch.h"
#include "SDK\HModel.h"
#include "SDK\HDecal.h"
#include "sdk\HProgres.h"
#include "../../libraries/hashtools/hashtools.h"

#include "intmap.h"

#include <vector>

#ifndef __STRCLASS_H
#include "strclass.h"
#endif


class HModelCache;

/////////////////////////////////////////////////////////////////////////////
// CHxtApp
// See Hxt.cpp for the implementation of this class
//
class HGroupCP;
class HGroup;
class HProperyDriver;

class BonePair
{
public:
	BonePair() { right = NULL; left = NULL; rightparent = NULL; leftparent = NULL; }
	String name;
	HTreeObject *right;
	HTreeObject *left;
	HTreeObject *rightparent;
	HTreeObject *leftparent;
};

class CHxtApp : public CWinApp
{
public:
	CHxtApp();

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

public:
	BOOL OnMirrorBones(HTreeObject *htreeobject);

   void GenerateLogFile();
   void LogRelationships(BonePair *pair);
   
   void RebuildRelationshipList();

	int EvaluateBoneChildren(HTreeObject *htreeobject,HTreeObject *htreeobjectparent = NULL);
	bool IsLeftSide(HTreeObject *htreeobject);
	bool IsRightSide(HTreeObject *htreeobject);
	String GetBoneName(HTreeObject *htreeobject);
	int  CopyBoneAttributes(HBoneCache *boneDst,HBoneCache *boneSrc,bool invertX = true);
   void CopyPropertyDrivers(HHashObject *srcparent, HHashObject *dstparent, HBoneCache *srcbone, HBoneCache *dstbone);
   void MirrorConstraintTargets(HConstraint *src, HConstraint *dst);
   void MirrorExpressionTargets(HExpression *dst);
   BOOL HandleSpecial(HPropertyDriver *src, HPropertyDriver *dst, HBoneCache *srcbone, HBoneCache *dstbone);
   
	String Replace(String fullstring,String replace,String replacewith);

	HBoneCache *DuplicateBone(HBoneCache *boneSrc, HBoneCache *boneParent,String name, bool invertX = true);
	HBoneCache *CreateRightBone(BonePair *bonepair);
	HBoneCache *CreateLeftBone(BonePair *bonepair);

public:
	bool m_adjustright;
	bool m_assignvertices;
	bool m_createbones;
   bool m_generatelogfile;
	std::vector<BonePair> m_bonepairs;
	std::vector<HTreeObject*> m_bonesingles;
	IntMap m_bonessinglemap,m_parentmap;
   HashTreeObjects m_relationships;
	double m_tolerance;
	HModelCache *m_hmc;
   int m_currprogress, m_totalprogresssize;
   FileStream m_fs;
   HProgressBar *m_hprogress;
};


/////////////////////////////////////////////////////////////////////////////
#endif __HXT_H