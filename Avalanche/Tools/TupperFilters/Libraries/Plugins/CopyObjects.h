////////////////////////////////////////////////////////////////////////////
//
// CopyObjects
//
// Class to handle the copying of objects down to and including the base mesh
//
////////////////////////////////////////////////////////////////////////////

#ifndef COPY_OBJECTS_H
#define COPY_OBJECTS_H

class CopyObjectsManager
{
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;

	int m_tupRootNodeRef;

public:
	CopyObjectsManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,int tupRootNodeRef);

	int CopyNode(int nodeRef); // copy the nodes and return the new reference

private:
	int CopyObject(int objectRef);
	int CopyModifier(int modifierRef);
};

#endif
