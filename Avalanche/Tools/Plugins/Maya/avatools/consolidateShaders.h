//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _consolidateShaders
#define _consolidateShaders

#define MAXSTRINGSIZE 512

class MObject;
class MObjectArray;

#include "crc.h"


///////////////////////////////////////////////////////////////////////////////////////////
// delete these
#include "maya/mdagpath.h"
#include "maya/mintarray.h"
#include "maya/mpxcommand.h"
#include "maya/mdgmodifier.h"
#include "maya/mitdag.h"
#include "maya/mglobal.h"
#include "maya/mobjectarray.h"
#include "maya/mfndagnode.h"
#include "maya/mplug.h"
#include "maya/MFnBlinnShader.h"
#include "maya/MFnPhongShader.h"
#include "maya/MFnLambertShader.h"
#include "maya/mplugarray.h"
#include "maya/mfnset.h"
#include "maya/mselectionlist.h"

#include <vector>

#include "../../libraries/MTools/MTools.h"
#include "intmap.h"

///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////

class myColor
{
public:
	myColor() { r = 0.0; g = 0.0; b = 0.0; }
	myColor(float RED,float GREEN,float BLUE) { r = RED; g = GREEN; b = BLUE; }

	int	GetHashValue()
	{
		return HashValue::GetHashValue(this,sizeof(this));
	}

	bool operator == ( const myColor &other ) const
	{
		if ((r!=other.r)&&(g!=other.g)&&(b!=other.b))
			return false;
		return true;
	}

	bool operator != (const myColor &other) const
	{
		return !(*this == other);
	}

public:
	struct { float r, g, b; };
};

///////////////////////////////////////////////////////////////////////////////////////////

class myMap
{
public:
	myMap()
	{
		m_ru = 0.0;
		m_rv = 0.0;
		m_wrapu = 1;
		m_wrapv = 1;
		memset(m_pathname,0,sizeof(char));
		strcpy(m_pathname,"unused");
	}

	int	GetHashValue()
	{
		CRC crc;
		crc.AnalyzeData(m_pathname);
		crc.AnalyzeData(m_ru);
		crc.AnalyzeData(m_rv);
		crc.AnalyzeData(m_wrapu);
		crc.AnalyzeData(m_wrapv);
		return crc.GetResult();
	}

public:
	char m_pathname[MAXSTRINGSIZE];
	float m_ru,m_rv;
	int m_wrapu,m_wrapv;
};

///////////////////////////////////////////////////////////////////////////////////////////

class ShaderChannel
{
public:
	ShaderChannel()
	{
		m_type = kNULL;
	}

	int	GetHashValue()
	{
		CRC crc;
		crc.AnalyzeData((int)m_type);
		crc.AnalyzeData(m_rgb.GetHashValue());
		crc.AnalyzeData(m_maps[0].GetHashValue());
		crc.AnalyzeData(m_maps[1].GetHashValue());
		crc.AnalyzeData(m_maps[2].GetHashValue());
		return crc.GetResult();
	}

	enum channelType { kNULL, kCOLOR, kFILE, kBLENDER, kENVBALL };

public:
	channelType m_type;
	myColor m_rgb;
	myMap m_maps[3];
};

///////////////////////////////////////////////////////////////////////////////////////////

class HashShader
{
public:
	HashShader()
	{
		m_type = kUNKNOWN;
		strcpy(m_name,"noname");
	}

	int	GetHashValue()
	{
		int hashvalue = m_color.GetHashValue();
		return hashvalue;
	}

	enum shaderType { kUNKNOWN, kLAMBERT, kPHONG, kBLINN };

public:
	char m_name[MAXSTRINGSIZE];
	shaderType m_type;
	ShaderChannel m_color,m_transparency,m_reflectivity;	
};


///////////////////////////////////////////////////////////////////////////////////////////

class ObjectInfo
{
public:
	MDagPath mdagpath;
	MIntArray keys;
};

class consolidateShaders : public MPxCommand
{
public:
	consolidateShaders ();
	~consolidateShaders ();

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

public:
	void CreateShaderTable(MObjectArray &SGSets,std::vector<HashShader> &shaders,MObjectArray &nodetrash);
	void RemoveShadingGroups(MObjectArray &SGSets);
	int GetMap(MObject filenode,myMap *pMap,MObjectArray &nodetrash);
	int GetPlacementNodeInfo(MObject filenode,myMap *pMap,MObjectArray &nodetrash);
	int CreateShadingGroup(HashShader &shader,MObject &newShadingGroup);
	int LinkUpMapNode(MString shaderNode,MString attrName,MString mapName,double ru,double rv,int wrapu,int wrapv,int useAlpha);
	int CreatePlacementNode(MString filenode,double ru,double rv,int wrapu,int wrapv);
	void RemoveMap(MObject filenode);

	MStatus ConnectShadingGroups(MDagPath dagpath,const MObjectArray &SGList,const std::vector<int> &shaderindicies);
	void CreateHashShader(MObject shadingGroup,HashShader &hashShader,MObjectArray &nodetrash);	
};

#endif
