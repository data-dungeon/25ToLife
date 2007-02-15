#pragma once

class SnipeObject;

class SnipeObjectNode
{
private:
   SnipeObject *m_pObject;
   SnipeObject *m_pOwner;

public:
   SnipeObjectNode()
   {
      m_pObject = NULL;
      m_pOwner = NULL;
   }

   SnipeObjectNode( SnipeObject *pObject, SnipeObject *pOwner=NULL )
   {
      m_pObject = pObject;

      if (m_pOwner==NULL)
         m_pOwner = m_pObject;
      else
         m_pOwner = m_pOwner;
   }
};

class SnipeObjectNodeList : public CList<SnipeObjectNode>
{
public:
};
