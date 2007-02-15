#ifndef __PropertyTree_H
#define __PropertyTree_H

#include <string>
#include <vector>
#include <fstream.h>

#pragma warning (disable : 4786)

typedef std::vector<std::string> StringArray;

class PropContainer {
public:
	enum PropTypes { EMPTY,CONTAINER_PROP, COLOR_PROP, PERCENT_PROP, INT_PROP, FLOAT_PROP,
						BOOL_PROP, ENUM_PROP, TIME_PROP, TIME_RANGE_PROP, STRING_PROP,
						FILENAME_LOAD_PROP, FILENAME_SAVE_NAME, VECTOR_PROP, VECTOR2_PROP,
						TRANSLATE_PROP, ROTATE_PROP, SCALE_PROP, TRANSFORM_PROP, IMAGE_PROP };
	
	virtual PropTypes GetType() { return CONTAINER_PROP; };
	virtual char *GetTypeStr() { return "CONTAINER"; };

public:
	std::string _name;
};

class PropColor : public PropContainer {
public:
	float _red,_green,_blue;
	PropTypes GetType() { return COLOR_PROP; };
	char *GetTypeStr() { return "COLOR"; };
};

class PropPercent : public PropContainer {
public:
	float _default;
	PropTypes GetType() { return PERCENT_PROP; };
	char *GetTypeStr() { return "PERCENT"; };
};

class PropBool : public PropContainer {
public:
	bool _default;
	PropTypes GetType() { return BOOL_PROP; };
	char *GetTypeStr() { return "BOOL"; };

};

class PropInt : public PropContainer {
public:
	int _default;
	int _min,_max;
	PropTypes GetType() { return INT_PROP; };
	char *GetTypeStr() { return "INT"; };
};

class PropFloat : public PropContainer {
public:
	float _default;
	float _min,_max;
	PropTypes GetType() { return FLOAT_PROP; };
	char *GetTypeStr() { return "FLOAT"; };
};

class PropEnum : public PropContainer {
public:
	int _default;
	std::vector<std::string> _types;
	PropTypes GetType() { return ENUM_PROP; };
	char *GetTypeStr() { return "ENUM"; };
};

class PropImage : public PropContainer {
public:
	PropTypes GetType() { return IMAGE_PROP; };
	char *GetTypeStr() { return "IMAGE"; };
};

class PropString : public PropContainer {
public:
	std::string _default;
	PropTypes GetType() { return STRING_PROP; };
	char *GetTypeStr() { return "STRING"; };
};

class PropertyNode
{
public:
	PropertyNode() {
		_parent = -1;
		_prop = NULL;
	};
	PropertyNode(const PropertyNode &other) {
		_parent = other._parent;
		_children = other._children;
		_prop = other._prop;
	}
	~PropertyNode() {}

	void	clear() {
		_parent = -1;
		_children.clear();
	};
	void	purge() {
		if (_prop)
			delete _prop;
	};

	PropContainer *GetProperty() { return _prop; };

	bool	hasParent() { return (_parent >= 0); };

public:
	PropContainer *_prop;
	int _parent;
	std::vector<int> _children;
};

typedef std::vector<PropertyNode> PropertyNodeArray;

class PropertyTree {
public:
	PropertyTree(){ cached = false; };
	~PropertyTree(){
		for (int i = 0; i < _properties.size(); i++)
			_properties[i].purge();
	}
	static void		split(const std::string line,char character,StringArray &strarray);
	void			removeTabs(std::string &line);
	bool			parseLine(std::string fline, std::string &label, StringArray &args);
	void			parseLine(std::string fline, StringArray &args);
	void			combineStrArray(std::string &line,char character,StringArray &strarray);
	void			killspaces(std::string &line);
	static void		MakeStrUpper(std::string &line);
	int				asInt(const std::string line) const;
	float			asFloat(const std::string line) const;
	static float	ReplaceChar(char find,char replace,std::string &line);
	
	PropertyNode	*GetRoot();
	PropertyNode	*GetParentPropertyNode(PropertyNode *node);
	PropertyNode	*GetChildPropertyNode(PropertyNode *node,const int index);
	PropertyNode	*GetNextChildFromParent(PropertyNode *node,int &col);
	int				FindPropertyNodeIndexInChildren(PropertyNode *node,PropertyNode *InNode);

	int				AddPropertyNode(PropertyNode &node,const int parentidx);

	bool			ReadPropertyTree(const char *filename);

	bool			ReadName(ifstream &fin,PropertyNode &node);
	bool			ReadContainer(ifstream &fin,PropertyNode &node);
	bool			ReadEnum(ifstream &fin,PropertyNode &node);
	bool			ReadBool(ifstream &fin,PropertyNode &node);
	bool			ReadInt(ifstream &fin,PropertyNode &node);
	bool			ReadFloat(ifstream &fin,PropertyNode &node);
	bool			ReadImage(ifstream &fin,PropertyNode &node);
	bool			ReadString(ifstream &fin,PropertyNode &node);

	PropertyNode	*_FindProperty(const char *matchname,PropertyNode *node);
	PropertyNode	*FindProperty(const char *matchname);

	bool			OutputToTextFile();

	friend ostream &operator << (ostream &os,PropertyTree &other);

public:
	bool cached;
	PropertyNodeArray _properties;
};

#include "propertytree.hpp"

#endif
