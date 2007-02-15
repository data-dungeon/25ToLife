////////////////////////////////////////////////////////////////////////////
//
// EachNode
//
// Utility class to go through each node
//
////////////////////////////////////////////////////////////////////////////
#include "ExportPch.h"
#include "EachNode.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
EachNode::EachNode(
TupConfigRoot &tupConfigRoot,
TupNodePool &tupNodePool,
const char *key,
const char *value) :
m_tupConfigRoot(tupConfigRoot),
m_tupNodePool(tupNodePool)
{
	m_key = CopyInputString(key);
	m_value = CopyInputString(value);
	m_totalNodes = m_tupNodePool.GetNumNodes();
	m_onNode = 0;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
EachNode::~EachNode()
{
	delete [] m_value;
	delete [] m_key;
}

////////////////////////////////////////////////////////////////////////////
// Cout the nodes
////////////////////////////////////////////////////////////////////////////
int EachNode::Count()
{
	int count = 0;
	TupNode *node = First();
	while (node)
	{
		++count;
		node = Next(node);
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////
// Get the first node of type
// Resets the enumeration
////////////////////////////////////////////////////////////////////////////
TupNode *EachNode::First()
{
	m_onNode = 0;
	return Next(NULL);
}

////////////////////////////////////////////////////////////////////////////
// Get the next node, keep calling until NULL
// is returned
////////////////////////////////////////////////////////////////////////////
TupNode *EachNode::Next(
TupNode *current)
{
	// Delete the current node for them
	delete current;

	// Scan for a match
	while (m_onNode < m_totalNodes)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(m_onNode++);
		if (!pNodeAgg)
			continue;
		TupNode *tupNode = new TupNode(pNodeAgg);

		// This is easy
		if (!m_key)
			return tupNode;

		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode->GetUserProperties(&pNodeAttributesString);

		// Do this shiznit again...
		TupKeyValue tupKeyValue(pNodeAttributesString, BEGIN_BLOCK, END_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey(m_key);
		m_tupConfigRoot.GetKeyValueContexts(tupKeyValue, tupKeyValueContext, false);

		// Does this key exist?
		if (!tupKeyValueContext.ValueValidAt(0))
		{
			// This is what we are looking for if this is NULL
			if (!m_value)
				return tupNode;
		}
		else
		{
			// Is this kind we want?
			const char *value = tupKeyValueContext.GetValueAsString(0);
			if (strcmp(value, m_value) == 0)
				return tupNode;
		}

		// Free it and look again
		delete tupNode;
	}

	// Couldn't find one
	return NULL;
}

////////////////////////////////////////////////////////////////////////////
// Abort the enumeration
////////////////////////////////////////////////////////////////////////////
void EachNode::Abort(
TupNode *current)
{
	delete current;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
char *EachNode::CopyInputString(
const char *s)
{
	if (!s)
		return NULL;

	char *copy = new char[strlen(s) + 1];
	strcpy(copy, s);
	return copy;
}

