// Dan  7/7/2004  \DanLP110\Plugin\HXT\Avalanche\AnimationMaster\KeyPose\KeyPose.h

#define __KEYPOSE_H

class HModel;
class ModelExt;

#ifndef __HASHTAB_H
#include "HashTab.h"
#endif
#ifndef __POSETREE_H
#include "PoseTree.h"
#endif

class ModelExt;

class CKeyPoseWnd : public CWnd
{
   DECLARE_DYNAMIC(CKeyPoseWnd);
public:
   HModel *m_model;
   ModelExt *m_modelext;
   CHashTabCtrl m_tab;
   CPoseTree  m_tree;
   BOOL m_isinsetmodel;

   CKeyPoseWnd();

   void SetModel( HModel *model );
   ModelExt *GetModelExt(HModel *model);
   BOOL SetModelExt(HModel *model, ModelExt *modelext);

protected:
   afx_msg void OnTabChange(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
//   afx_msg void OnPaint();
   afx_msg void OnSize( UINT nType, int cx, int cy );
   DECLARE_MESSAGE_MAP()
};

HProperty *GetFirstTreeProperty( HTreeObject *obj );
