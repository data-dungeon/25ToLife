#ifndef __REFEREERESTOREOBJ_H__
#define __REFEREERESTOREOBJ_H__

#include <vector>
using std::vector;
#include "tupperware/keeper.h"
#include "modstack.h"


#define  ORM_CLASS_ID 0xB8073421
class ObjectRefMaker: public ReferenceMaker
{
public:
  RefTargetHandle rtarget;
  ObjectRefMaker() { rtarget = NULL; }
  ~ObjectRefMaker() { DeleteAllRefsFromMe(); }

  void SetRef(RefTargetHandle rt)
  {
    theHold.Suspend();
    ReplaceReference(0,rt);
    theHold.Resume();
  }
  RefTargetHandle GetRef() { return rtarget; }

  // ReferenceMaker 
  RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message)
  {
    if (message==REFMSG_TARGET_DELETED)
    {
      if (hTarget==rtarget)
        rtarget = NULL;
    }
    return REF_SUCCEED;
  }
  
  void DeleteThis() { delete this; }
  SClass_ID  SuperClassID() { return ORM_CLASS_ID; }
  
  // From ref
  int NumRefs() { return 1; }
  RefTargetHandle GetReference(int i) { return rtarget; }
  void SetReference(int i, RefTargetHandle rtarg) { rtarget = rtarg; }
  BOOL CanTransferReference(int i) {return FALSE;}
};


class RefereeRestoreObj : public RestoreObj
{
public:
  struct NodeData
  {
    INode* pNode;
    int newParent;
    int oldParent;
    int newMtl;
    int oldMtl;
  };

  struct ObjData
  {
    IDerivedObject* pObject;
    int newParent;
    int oldParent;
  };

  RefereeRestoreObj() {}
  ~RefereeRestoreObj()
  {
    for (int i = 0; i < m_dummyRefs.size(); ++i)
    {
      delete m_dummyRefs[i];
      m_dummyRefs[i] = NULL;
    }
    m_dummyRefs.clear();
  }


  // from RestoreObj
  void Restore(int isUndo);
  void Redo();
  int Size();
  void EndHold();
  TSTR Description() { return TSTR(_T("RefereeRestoreObj")); }

  void AddNode(Object* pOldParent, Object* pNewParent, INode* pNode, Mtl* pOldMtl = NULL, Mtl* pNewMtl = NULL);
  void AddObject(Object* pOldParent, Object* pNewParent, IDerivedObject* pObject);
  void Execute();

private:
  Keeper<Object*> m_parentArray;
  Keeper<Mtl*> m_mtlArray;
  vector<ObjData> m_objectArray;
  vector<NodeData> m_nodeArray;
  vector<ObjectRefMaker*> m_dummyRefs;
  
};

#include "RefereeRestoreObj.hpp"

#endif // __REFEREERESTOREOBJ_H__