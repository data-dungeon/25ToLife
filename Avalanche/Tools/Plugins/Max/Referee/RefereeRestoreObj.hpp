

inline void RefereeRestoreObj::Restore(int isUndo)
{
  int i;
  Object** parentArray = m_parentArray.GetData();
  Mtl** mtlArray = m_mtlArray.GetData();

  for (i = 0; i < m_nodeArray.size(); ++i)
  {
    m_nodeArray[i].pNode->SetObjectRef(parentArray[m_nodeArray[i].oldParent]);
    if (m_nodeArray[i].oldMtl != -1)
      m_nodeArray[i].pNode->SetMtl(mtlArray[m_nodeArray[i].oldMtl]);
  }
  for (i = 0; i < m_objectArray.size(); ++i)
  {
    m_objectArray[i].pObject->ReferenceObject(parentArray[m_objectArray[i].oldParent]);
  }
  NotifySelSetChanged(NULL, NULL);
}

inline void RefereeRestoreObj::Redo()
{
  int i;
  Object** parentArray = m_parentArray.GetData();
  Mtl** mtlArray = m_mtlArray.GetData();

  for (i = 0; i < m_nodeArray.size(); ++i)
  {
    m_nodeArray[i].pNode->SetObjectRef(parentArray[m_nodeArray[i].newParent]);
    if (m_nodeArray[i].newMtl != -1)
      m_nodeArray[i].pNode->SetMtl(mtlArray[m_nodeArray[i].newMtl]);
  }
  for (i = 0; i < m_objectArray.size(); ++i)
  {
    m_objectArray[i].pObject->ReferenceObject(parentArray[m_objectArray[i].newParent]);
  }
  NotifySelSetChanged(NULL, NULL);
}

inline int RefereeRestoreObj::Size()
{
  return 4 * sizeof(Object*);
//    return (m_objectList.size() + m_nodeList.size() + 2) * sizeof(Object*);
}


inline void RefereeRestoreObj::EndHold()
{
}


inline void RefereeRestoreObj::AddNode(Object* pOldParent, Object* pNewParent, INode* pNode, Mtl* pOldMtl, Mtl* pNewMtl)
{
  NodeData nd;
  nd.pNode = pNode;
  nd.newParent = m_parentArray.Add(pNewParent);
  nd.oldParent = m_parentArray.Add(pOldParent);
  if (pOldMtl)
    nd.newMtl = m_mtlArray.Add(pOldMtl);
  else
    nd.oldMtl = -1;

  if (pNewMtl)
    nd.newMtl = m_mtlArray.Add(pNewMtl);
  else
    nd.newMtl = -1;

  m_nodeArray.push_back(nd);
}

inline void RefereeRestoreObj::AddObject(Object* pOldParent, Object* pNewParent, IDerivedObject* pObject)
{
  ObjData od;
  od.pObject = pObject;
  od.newParent = m_parentArray.Add(pNewParent);
  od.oldParent = m_parentArray.Add(pOldParent);
  m_objectArray.push_back(od);
}

inline void RefereeRestoreObj::Execute()
{
  Object** parentArray = m_parentArray.GetData();
  for (int i = 0; i < m_parentArray.GetSize(); ++i)
  {
    ObjectRefMaker* pRef = new ObjectRefMaker();
    pRef->SetRef(parentArray[i]);
    m_dummyRefs.push_back(pRef);
  }

  Redo();
}