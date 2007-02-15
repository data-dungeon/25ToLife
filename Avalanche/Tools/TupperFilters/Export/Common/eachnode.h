////////////////////////////////////////////////////////////////////////////
//
// EachNode
//
// Utility class to go through each node
//
////////////////////////////////////////////////////////////////////////////
#ifndef EACHNODE_H
#define EACHNODE_H

class TupScene;
class TupNodePool;

class EachNode
{
public:
	// Pass in NULL for key if you want all of them
	EachNode(TupConfigRoot &tupConfigRoot,
				TupNodePool &tupNodePool,
				const char *key,					// "instance_type" is useful
				const char *value);
	~EachNode();

	// Cout the nodes
	int Count();

	// Get the first node of type
	// Resets the enumeration
	TupNode *First();

	// Get the next node, keep calling until NULL
	// is returned.
	TupNode *Next(TupNode *current);

	// Abort the enumeration
	void Abort(TupNode *current);

private:
	TupConfigRoot 			&m_tupConfigRoot;
	TupNodePool 			&m_tupNodePool;
	char 						*m_key;
	char 						*m_value;
	int						m_totalNodes;
	int						m_onNode;

	char *CopyInputString(const char *s);
};

#endif
