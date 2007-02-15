inline ifstream &operator >> (ifstream &fin, std::string &line)
{
	char fline[256];
	fin.getline(fline,255,'\n');
	line = fline;
	return fin;
}


inline int PropertyTree::asInt(const std::string line) const {
	return atoi(line.c_str());
}

inline float PropertyTree::asFloat(const std::string line) const {
	return atof(line.c_str());
}

inline float PropertyTree::ReplaceChar(char find,char replace,std::string &line) {
	std::string str;
	for (int i = 0; i < line.size(); i++)
	{
		if (line[i] == find)
			str+=replace;
		else
			str+=line[i];
	}
	line = str;
}

inline void PropertyTree::MakeStrUpper(std::string &line) {
	CString temp = line.c_str();
	temp.MakeUpper();
	line = LPCTSTR(temp);
}

inline void PropertyTree::removeTabs(std::string &line)
{
	StringArray sarray;

	if (line != "") {
		split(line,'\t',sarray);

		if (sarray.size()>1) {
			line = sarray[sarray.size()-1];
			return;
		}

		line = sarray[0];
	}
}

inline void PropertyTree::split(const std::string line,char character,StringArray &strarray)
{
	std::string str;
	strarray.clear();
	for (int i = 0; i < line.size(); i++) {
		if (line[i] == character){
			if (str != "")
				strarray.push_back(str);
			str = "";
		}
		else
			str += line[i];
	}
	if (str.size()>0)
		strarray.push_back(str);
}

inline void PropertyTree::killspaces(std::string &line) {
	StringArray sarray;
	split(line,' ',sarray);
	combineStrArray(line,NULL,sarray);	
}

inline bool PropertyTree::parseLine(std::string fline, std::string &label, StringArray &args)
{
	StringArray sarray,sarray2;
	MakeStrUpper(fline);
	if (fline!="") {
		removeTabs(fline);
		args.clear();
		split(fline,'=',sarray);
		if (sarray.size() < 2){
			label = sarray[0];
			return false;
		}
		label = sarray[0];
		killspaces(label);
		split(sarray[1],' ',sarray2);
		for (int i = 0; i < sarray2.size(); i++)
			args.push_back(sarray2[i]);
	}
	return true;
}

inline void PropertyTree::parseLine(std::string fline, StringArray &args)
{
	StringArray sarray;
	MakeStrUpper(fline);
	removeTabs(fline);
	args.clear();
	split(fline,' ',sarray);
	for (int i = 0; i < sarray.size(); i++)
		args.push_back(sarray[i]);
}

inline int PropertyTree::AddPropertyNode(PropertyNode &node,const int parent)
{
	if (parent >= 0)
		node._parent = parent;
	_properties.push_back(node);
	if (parent >= 0)
		_properties[parent]._children.push_back(_properties.size()-1);
	return _properties.size()-1;
}

inline void PropertyTree::combineStrArray(std::string &line,char character,StringArray &strarray){
	if (strarray.size()>0) {
		line = strarray[0];
		for (int i = 1; i < strarray.size(); i++) {
			if (character != NULL)
				line += character + strarray[i];
			else
				line += strarray[i];
		}
	}
}

inline PropertyNode	*PropertyTree::GetRoot()
{
		if (_properties.size() == 0)
			return NULL;
		return &_properties[0];
}

inline PropertyNode	*PropertyTree::GetParentPropertyNode(PropertyNode *node)
{
		if (node->_parent == -1)
			return NULL;
		return &_properties[node->_parent];
}

inline PropertyNode	*PropertyTree::GetChildPropertyNode(PropertyNode *node,const int index)
{
		if ((index >= node->_children.size())||(index == -1))
			return NULL;
		return &_properties[node->_children[index]];
	}

inline int PropertyTree::FindPropertyNodeIndexInChildren(PropertyNode *node,PropertyNode *InNode)
{
		for (int i = 0; i < InNode->_children.size(); i++)
			if (&_properties[InNode->_children[i]] == node)
				return i;
		return -1;
}

inline PropertyNode	*PropertyTree::GetNextChildFromParent(PropertyNode *node,int &col)
{
		PropertyNode *parentnode = GetParentPropertyNode(node);
		if (!parentnode)
			return NULL;
		int index = FindPropertyNodeIndexInChildren(node,parentnode);
		if ((index== -1)||((index+1) > parentnode->_children.size()-1)) {
			parentnode = GetNextChildFromParent(parentnode,col);
			col--;
			return parentnode;
		}
		if (!parentnode)
			return NULL;
		return GetChildPropertyNode(parentnode,index+1);
}

inline PropertyNode *PropertyTree::FindProperty(const char *matchname) {
	return _FindProperty(matchname,GetRoot());
}

inline PropertyNode *PropertyTree::_FindProperty(const char *matchname,PropertyNode *node) {

	CString temp;
	temp = matchname;
	temp.MakeUpper();
	if (strcmp(temp,node->_prop->_name.c_str()) == 0)
		return node;

	PropertyNode *nextnode;
	for (int i = 0; i < node->_children.size(); i++) {
		nextnode = _FindProperty(matchname,&_properties[node->_children[i]]);
		if (nextnode)
			return nextnode;
	}
	return NULL;
}