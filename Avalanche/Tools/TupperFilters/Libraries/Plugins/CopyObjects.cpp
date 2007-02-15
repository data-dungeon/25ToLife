////////////////////////////////////////////////////////////////////////////
//
// CopyObjects
//
// Class to handle the copying of objects down to and including the base mesh
//
////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "CopyObjects.h"

CopyObjectsManager::CopyObjectsManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,int tupRootNodeRef) :
m_tupNodePool(tupNodePool), m_tupObjectPool(tupObjectPool), m_tupModifierPool(tupModifierPool), m_tupRootNodeRef(tupRootNodeRef)
{
}

// copies a node along with the objects and modifiers needed.
// adds new node as a child of the same node parent
// new node will not have any children
int CopyObjectsManager::CopyNode(int nodeRef)
{
	TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeRef);
	assert(pNodeAgg);
	TupNode tupNode(pNodeAgg);
	int objectRef;
	tupNode.GetObjectReference(objectRef);
	assert(objectRef!=-1);

	int newNodeRef = m_tupNodePool.AddNode();

	// add new ref to parent
	int parentNodeRef;
	tupNode.GetParentNodeReference(parentNodeRef);
	TupperwareAggregate *pParentNodeAgg = m_tupNodePool.GetNode(parentNodeRef);
	assert(pParentNodeAgg);
	TupNode tupParentNode(pParentNodeAgg);
	tupParentNode.AddChildNodeReference(newNodeRef);

	TupperwareAggregate *pNewNodeAgg = m_tupNodePool.GetNode(newNodeRef);
	// copy stuff from my node into the new one
	for (TupperwareIterator iter = pNodeAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		pNewNodeAgg->Copy(iter.GetAsAtom());
	}
	pNewNodeAgg->DeleteAtomsByKey("Children"); // kill off the children nodes

	int newObjectRef = CopyObject(objectRef);
	TupNode tupNewNode(pNewNodeAgg);
	tupNewNode.AddObjectReference(newObjectRef);
	return newNodeRef;
}

// copy the current object and all it points to
int CopyObjectsManager::CopyObject(int objectRef)
{
	TupperwareAggregate *pObjectAgg = m_tupObjectPool.GetObjectAggregate(objectRef);

	int newObjectRef = m_tupObjectPool.AddObjectAggregate();
	TupperwareAggregate *pNewObjectAgg = m_tupObjectPool.GetObjectAggregate(newObjectRef);
	// copy stuff from my object into the new one
	for (TupperwareIterator iter = pObjectAgg->FindAll(); !iter.Finished(); iter.Next()) 
	{
		pNewObjectAgg->Copy(iter.GetAsAtom());
	}

	TupObject tupNewObject(pNewObjectAgg);
	switch (tupNewObject.GetObjectType())
	{
	case TupObject::DERIVED:
		{
			TupDerived tupDerived(pObjectAgg);
			TupDerived tupNewDerived(pNewObjectAgg);
			int numModifierSlots = tupDerived.GetNumModifierSlots();
			for (int modifierIndex=0;modifierIndex<numModifierSlots;modifierIndex++)
			{
				TupperwareAggregate *pDerivedModifierAgg = tupDerived.GetModifierBySlot(modifierIndex);
				if (!pDerivedModifierAgg)
					continue;
				TupDerivedModifier tupDerivedModifier(pDerivedModifierAgg);
				int modifierReference;
				tupDerivedModifier.GetModifierReference(modifierReference);
				int newModifierReference = CopyModifier(modifierReference);
				// if it created a new one
				if (newModifierReference!=modifierReference)
				{
					// link new derived modifier to new modifier reference
					TupperwareAggregate *pNewDerivedModifierAgg = tupNewDerived.GetModifierBySlot(modifierIndex);
					assert(pNewDerivedModifierAgg);
					TupDerivedModifier tupNewDerivedModifier(pNewDerivedModifierAgg);
					tupNewDerivedModifier.AddModifierReference(newModifierReference);
				}
			}
			// point to a copy as well
			int derivedObjectRef;
			tupDerived.GetObjectReference(derivedObjectRef);
			int newDerivedObjectRef = CopyObject(derivedObjectRef);
			tupNewDerived.AddObjectReference(newDerivedObjectRef);
		}
		break;
	}
	return newObjectRef;
}

int CopyObjectsManager::CopyModifier(int modifierRef)
{
	TupperwareAggregate *pModifierAgg = m_tupModifierPool.GetModifier(modifierRef);
	assert(pModifierAgg);
	TupModifier tupModifier(pModifierAgg);

	bool bCopyModifier = false;
	switch(tupModifier.GetModifierType())
	{
	case TupModifier::APPLY_VC_MODIFIER:
	case TupModifier::HOLD_VC_MODIFIER:
		bCopyModifier = true;
		break;
	case TupModifier::VERTEX_PAINT_MODIFIER:
	case TupModifier::FACE_ATTRIBUTE_MODIFIER:
	case TupModifier::SKIN_MODIFIER:
	case TupModifier::CLOTH_MODIFIER:
		break;
	default:
		assert(false);
	}
	if (bCopyModifier)
	{
		int newModifierRef = m_tupModifierPool.AddModifier();
		TupperwareAggregate *pNewModifierAgg = m_tupModifierPool.GetModifier(newModifierRef);
		// copy stuff from my modifier into the new one
		for (TupperwareIterator iter = pModifierAgg->FindAll(); !iter.Finished(); iter.Next()) 
		{
			pNewModifierAgg->Copy(iter.GetAsAtom());
		}
		return newModifierRef;
	}
	else
	{
		return modifierRef;
	}
}
