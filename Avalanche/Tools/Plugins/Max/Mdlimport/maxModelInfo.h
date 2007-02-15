
#ifndef __maxModelInfo_H
#define __maxModelInfo_H

#include <vector>

class maxModelInfo
{
public:
	maxModelInfo() { m_node = NULL; m_pXRef = NULL; };

	INode *m_node;
	INode *m_noBonesNode;
	TSTR m_fileName;
	TSTR m_objectName;
	IXRefObject *m_pXRef;

	std::vector<INode*> m_bones;
	std::vector<INode*> m_links;
	std::vector<float> m_linkSizes;
	std::vector<IXRefObject*> m_bonesxref;
	std::vector<IXRefObject*> m_linksxref;
};

#endif
