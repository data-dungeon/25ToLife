#pragma warning (disable : 4786)

#include "stdafx.h"
#include "propertytree.h"

bool PropertyTree::ReadPropertyTree(const char *filename) {
	if (!this->cached)
	{
		ifstream fin;
		std::string line2;

		fin.open(filename, (ios::nocreate));

		if (!fin)
			return false;//Couldn't open file
		
		std::string line,label;
		StringArray args;

		bool open = false;
		bool finished = false;
		bool hasParent = false;

		PropertyNode node;

		int idx;

		fin >> line;

		while ( !finished ) {
			if (line.substr(0,2)=="//") {
				fin >> line;
				continue;
			}

			parseLine(line,label,args);

			if (label =="{")
			{
				if (open) {
					if (hasParent) {
						idx = AddPropertyNode(node,idx);	
					}
					else {
						idx = AddPropertyNode(node,-1);
						hasParent = true;
					}
					node.clear();
				}
				open = true;
			}
			else if (label == "NODETYPE") {
				if (args[0] == "CONTAINER")
					ReadContainer(fin,node);
				else if (args[0] == "ENUMERATOR")
					ReadEnum(fin,node);
				else if (args[0] == "BOOL")
					ReadBool(fin,node);
				else if (args[0] == "INT")
					ReadInt(fin,node);
				else if (args[0] == "FLOAT")
					ReadFloat(fin,node);
				else if (args[0] == "IMAGE")
					ReadImage(fin,node);
				else if (args[0] == "STRING")
					ReadString(fin,node);
			}
			else if (label == "}") {
				if (open) {
					if (hasParent) {
						idx = AddPropertyNode(node,idx);	
					}
					else {
						idx = AddPropertyNode(node,-1);
					}
					node.clear();
					open = false;
				}			
				if (_properties[idx].hasParent()) {
					idx = _properties[idx]._parent;
					hasParent = true;
				}
				else {
					finished = true;
					continue;
				}
			}
			fin >> line;

		}

		fin.close();

//		OutputToTextFile();

		node._prop = NULL;

		this->cached = true;
	}
	return true;
}

bool PropertyTree::ReadName(ifstream &fin,PropertyNode &node) {
	StringArray args;
	std::string line,label;
	fin >> line;
	parseLine(line,label,args);
	if (label != "NAME")
		return false;
	combineStrArray(line,' ',args);
	node._prop->_name = line;
	return true;
}

bool PropertyTree::ReadContainer(ifstream &fin,PropertyNode &node) {
	PropContainer *container = new PropContainer;
	node._prop = container;
	ReadName(fin,node);
	return true;
}

bool PropertyTree::ReadString(ifstream &fin,PropertyNode &node) {
	PropString *str = new PropString;
	node._prop = str;
	ReadName(fin,node);

	StringArray args;
	std::string line,label;

	fin >> line;
	parseLine(line,label,args);
	if (label != "DEFAULT")
		return false;
	combineStrArray(line,' ',args);
	str->_default = line;

	return true;
}

bool PropertyTree::ReadEnum(ifstream &fin,PropertyNode &node) {
	StringArray args;
	std::string line,label;

	PropEnum *enumerator = new PropEnum;
	node._prop = enumerator;

	ReadName(fin,node);
	fin >> line;
	parseLine(line,label,args);
	if (label != "DEFAULT")
		return false;
	enumerator->_default = asInt(args[0]);
	fin >> line;
	parseLine(line,label,args);
	if (label != "COUNT")
		return false;
	int count = asInt(args[0]);

	for (int i = 0; i < count; i++) {
		fin >> line;
		removeTabs(line);
		enumerator->_types.push_back(line);
	}
	return true;
}

bool PropertyTree::ReadBool(ifstream &fin,PropertyNode &node) {
	StringArray args;
	std::string line,label;

	PropBool *prop = new PropBool;
	node._prop = prop;

	ReadName(fin,node);
	fin >> line;
	parseLine(line,label,args);
	if (label != "DEFAULT")
		return false;
	if (args[0] == "TRUE")
		prop->_default = true;
	else
		prop->_default = false;
	return true;
}

bool PropertyTree::ReadInt(ifstream &fin,PropertyNode &node) {
	StringArray args;
	std::string line,label;

	PropInt *prop = new PropInt;
	node._prop = prop;

	ReadName(fin,node);

	fin >> line;
	parseLine(line,label,args);
	if (label != "DEFAULT")
		return false;
	prop->_default = asInt(args[0]);
	
	fin >> line;
	parseLine(line,label,args);
	if (label != "MIN")
		return false;
	prop->_min = asInt(args[0]);
	fin >> line;
	parseLine(line,label,args);
	if (label != "MAX")
		return false;
	prop->_max = asInt(args[0]);
	return true;
}

bool PropertyTree::ReadFloat(ifstream &fin,PropertyNode &node) {
	StringArray args;
	std::string line,label;

	PropFloat *prop = new PropFloat;
	node._prop = prop;

	ReadName(fin,node);

	fin >> line;
	parseLine(line,label,args);
	if (label != "DEFAULT")
		return false;
	prop->_default = asFloat(args[0]);
	
	fin >> line;
	parseLine(line,label,args);
	if (label != "MIN")
		return false;
	prop->_min = asFloat(args[0]);

	fin >> line;
	parseLine(line,label,args);
	if (label != "MAX")
		return false;
	prop->_max = asFloat(args[0]);
	return true;
}

bool PropertyTree::ReadImage(ifstream &fin,PropertyNode &node) {
	PropImage *prop = new PropImage;
	node._prop = prop;

	ReadName(fin,node);

	return true;
}

bool PropertyTree::OutputToTextFile() {
	ofstream fout;

	fout.open("e:\\temp\\log.txt");

	if (!fout)
		return false;//Couldn't open file

	bool finished = false;
	int col = 0;
	int idx = 0,cidx = 0;

	PropertyNode *prop = GetRoot(),*nextprop = NULL;

	fout << prop->_prop->_name.c_str() << endl;

	while (!finished) {
		nextprop = GetChildPropertyNode(prop,0);
		if (!nextprop) {
			nextprop = GetNextChildFromParent(prop,col);
			if (!nextprop) {
				finished = true;
				break;
			}
		}
		else
			col++;

		if (finished)
			continue;

		prop = nextprop;
		
		for (int i = 0; i < col; i++)
			fout << "\t";
		fout << prop->_prop->_name.c_str();
		fout << "[ " << prop->_prop->GetTypeStr() << ": ";
		if (prop->_prop->GetType() == PropContainer::ENUM_PROP) {
			PropEnum *enumerator = (PropEnum *)prop->_prop;
			fout << enumerator->_types.size();
		}
		fout << "]" << endl;
	}

	fout.close();
	return true;
}

