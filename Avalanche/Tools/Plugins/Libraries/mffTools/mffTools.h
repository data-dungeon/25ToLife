#include <vector>
#include <string>

#define VERSION 2.2f

#define MAX_MFFSTRING_SIZE 4096
#define MAX_LINE_SIZE 256

inline void GetLine(FILE *fp,int maxlength,char *line);

enum mffNodeType { kNODE, kNULL, kNULLINSTANCE, kMODEL, kMODELINSTANCE, kLIGHT, kLIGHTINSTANCE, kBONE, kBONEINSTANCE, kGROUPTRANSFORM, kGROUPTRANSFORMINSTANCE, kVERTEXCOLORNODE, kSHADERTABLENODE, kMAPTABLENODE, kDISPLAYLAYERTABLENODE };
enum mffModelType { kUNKNOWN, kPOLYGONMODEL, kPOLYGONMODELINSTANCE, kCURVEMODEL, kCURVEMODELINSTANCE  };

class mffAttribute
{
public:
	mffAttribute() { m_type = kUNKNOWN; strcpy(m_name,"Unknown"); strcpy(m_shortname,"Unknown"); };

	mffAttribute(const mffAttribute &other)
	{
		m_type = other.m_type;
		strcpy(m_name,other.m_name);
		strcpy(m_shortname,other.m_shortname);
	}

	enum mffAttrType { kINVALID, kBOOLEAN, kINTEGER, kFLOAT, kDOUBLE, kSTRING, kVECTOR, kVECTOR2, kCOLOR, kENUM, kINTARRAY, kDBLINTARRAY,
		kVECTORARRAY, kVECTOR2ARRAY, kDOUBLEARRAY, kCOLORARRAY, kSTRINGARRAY };

	enum mffAttrType2 { kUNKNOWN, kOBJECT, kFACE } m_type;

	char m_name[MAX_LINE_SIZE];
	char m_shortname[32];

	virtual mffAttrType GetAttrType() { return kINVALID; }
	virtual char *GetAttrTypeStr() { return "Invalid"; }

	virtual void import(FILE *fp)
	{
	}

	virtual void export(FILE *fp)
	{
	}

	virtual void importName(FILE *fp)
	{
	}

	virtual void exportName(FILE *fp)
	{
		const char *type; 
		
		switch (m_type)
		{
			case kUNKNOWN :
					type = "[UNKNOWN]";
					break;
			case kFACE :
					type = "[FACE]";
					break;
			case kOBJECT :
					type = "[OBJECT]";
					break;
		}

		fprintf(fp,"%s %s %s ",type,m_name,m_shortname);
	}

	virtual mffAttribute &operator = ( const mffAttribute &other )
	{
		m_type = other.m_type;
		strcpy(m_name,other.m_name);
		strcpy(m_shortname,other.m_shortname);
		return *this;
	}
};

class mffVector : public mffAttribute
{
public:
	mffVector() { x = 0.0; y = 0.0; z = 0.0; }
	mffVector(double X, double Y, double Z) { x = X; y = Y; z = Z; }
	struct { double x, y, z; };

	mffAttribute::mffAttrType GetAttrType() { return mffAttribute::kVECTOR; }
	char * GetAttrTypeStr() { return "Vector"; }

	void import(FILE *fp)
	{
		char line[MAX_LINE_SIZE];
		float fx,fy,fz;
		fscanf(fp,"[%s %f %f %f]\n",line,&fx,&fy,&fz);
		x = fx;
		y = fy;
		z = fz;
	}

	void export(FILE *fp)
	{
		float fx,fy,fz;
		fx = (float)x;
		fy = (float)y;
		fz = (float)z;
		fprintf(fp,"[xyz %f %f %f]\n",fx,fy,fz);
	}
};

class mffVector2 : public mffAttribute
{
public:
	mffVector2() { x = 0; y = 0; }
	mffVector2(double X, double Y) { x = X; y = Y; }
	struct { double x,y; };

	mffAttribute::mffAttrType GetAttrType() { return mffAttribute::kVECTOR2; }
	char * GetAttrTypeStr() { return "Vector2"; }

	void import(FILE *fp)
	{
		char line[MAX_LINE_SIZE];
		float fx,fy;
		fscanf(fp,"[%s %f %f]\n",line,&fx,&fy);
		x = fx;
		y = fy;
	}

	void export(FILE *fp)
	{
		float fx,fy;
		fx = (float)x;
		fy = (float)y;
		fprintf(fp,"[xy %f %f]\n",fx,fy);
	}
};

class mffColor : public mffAttribute
{
public:
	mffColor() { r = 0.0; g = 0.0; b = 0.0; a = 0.0; }
	mffColor(float RED,float GREEN,float BLUE,float ALPHA) { r = RED; g = GREEN; b = BLUE; a = ALPHA; }

	struct { float r, g, b, a; };

	mffAttribute::mffAttrType GetAttrType() { return mffAttribute::kCOLOR; }
	char * GetAttrTypeStr() { return "Color"; }

	void import(FILE *fp)
	{
		float fr,fg,fb,fa;
		fscanf(fp,"[rgba %f %f %f %f]\n",&fr,&fg,&fb,&fa);
		r = fr;
		g = fg;
		b = fb;
		a = fa;
	}

	void export(FILE *fp)
	{
		float fr,fg,fb,fa;
		fr = (float)r;
		fg = (float)g;
		fb = (float)b;
		fa = (float)a;
		fprintf(fp,"[rgba %f %f %f %f]\n",fr,fg,fb,fa);
	}
};

class mffMap
{
public:
	mffMap()
	{
		strcpy(m_filename,"");
		m_wrapu = 0;
		m_wrapv = 0;
		m_mirroru = 0;
		m_mirrorv = 0;
		m_stagger = 0;
		m_rotation = 0.0;
		m_startframe = 0;
		m_stopframe = 0;
		m_stepframe = 0;
		strcpy(m_uvsetname,"map1");
		m_alphagain = 0.0;
		m_alphaoffset = 0.0;
		m_repeat.x = 1.0;
		m_repeat.y = 1.0;
	}

public:
	char m_filename[512];

//	boolean
	int m_mirroru,m_mirrorv;
	int m_wrapu,m_wrapv;
	int m_stagger;

//	offsets
	mffVector2 m_repeat;
	mffVector2 m_translation;
	float m_rotation;

//	color balance
	mffColor m_defaultcolor;
	mffColor m_colorgain;
	mffColor m_coloroffset;
	float m_alphagain;
	float m_alphaoffset;

//	texture animation
	int m_startframe, m_stopframe, m_stepframe;	

	char m_uvsetname[MAX_LINE_SIZE];
};

class mffIntArray : public mffAttribute
{
public:

	void import(FILE *fp)
	{
		char line[MAX_LINE_SIZE];
		int count = 0,value;
		fscanf(fp,"[ %s ",line);

		fscanf(fp,"(%i)",&count);
		m_values.resize(count);
		for (int i = 0; i < count; i++)
		{
			fscanf(fp,"%i",&value);
			m_values[i] = value;
		}
		GetLine(fp,MAX_LINE_SIZE,line);
	}
	void export(FILE *fp)
	{
		int count = m_values.size();
		fprintf(fp,"[ intarray (%i)",count);
		for (int i = 0; i < m_values.size(); i++)
			fprintf(fp," %i",m_values[i]);
		fprintf(fp," ]\n");
	}

	void clear() { m_values.clear(); }
	void push_back(int value) { m_values.push_back(value); }
	int operator[]( int index ) const { return m_values[index]; }
 	int &operator[]( int index ) { return m_values[index]; }
	int size() { return m_values.size(); }
	void resize(int newsize) { m_values.resize(newsize); }

	mffAttribute::mffAttrType GetAttrType() { return kINTARRAY; }
	char * GetAttrTypeStr() { return "INTARRAY"; }

	std::vector<int> m_values;
};

class mffMapObject
{
public:
	enum mffMapObjectType { kMAPOBJECT, kSTANDARD, kBLENDER, kLAYEREDTEXTURE };

	mffMapObject(){};

	mffIntArray m_mapindicies;

	virtual const char *GetTypeStr() { return "MapObject"; }
	virtual int GetType() { return kMAPOBJECT; }
};

class mffStd : public mffMapObject
{
public:
	mffStd() {};

	const char *GetTypeStr() { return "Standard"; }
	virtual int GetType() { return kSTANDARD; }

	mffMap m_map;
};

class mffBlender : public mffMapObject
{
public:
	mffBlender() {};

	const char *GetTypeStr() { return "Blender"; }
	virtual int GetType() { return kBLENDER; }

	mffMap m_map0,m_map1,m_map2;
};

class mffLayeredTexture : public mffMapObject
{
public:
	mffLayeredTexture() {};

//	based on maya's enumeration type
//	enum mffLayerType { kNONE, kOVER, kIN, kOUT, kADD, kSUBTRACT, kMULTIPLY, kDIFFERENCE, kLIGHTEN, kDARKEN, kSATURATE, kDESATURATE, kILLUMINATE };

	const char *GetTypeStr() { return "LayeredTexture"; }
	virtual int GetType() { return kLAYEREDTEXTURE; }

	std::vector<mffMap> m_maps;
	mffIntArray m_types;
	mffIntArray m_mapsconnected;
//	mffIntArray m_alphasconnected;
//	mffDoubleArray m_alphavalues;
};

class mffShader
{
public:
	mffShader() { m_colorMap = NULL; m_reflectMap = NULL; }
	~mffShader() {}

public:
	char m_name[MAX_LINE_SIZE];
	mffColor m_color;
	mffMapObject *m_colorMap;
	float m_reflect;
	mffMapObject *m_reflectMap;
};
class mffString : public mffAttribute
{
public:
	char m_value[MAX_MFFSTRING_SIZE];

	mffString() {};

	mffString(const char *c_str)
	{
		strcpy(m_value,c_str);
	}

	const char *mffString::c_str()
	{
		return m_value;
	}

	void import(FILE *fp)
	{
		// scan for the first quote (") char.
		int c;
		do
		{
			c = fgetc(fp);
		} 
		while (c != '"');

		char value[1280];

		// keep getting chars until we get to the next quote (").
		int i = 0;
		do
		{
			// check the index to be sure it is in range.
			//ASSERT(i + 1 < 1280);
			c = fgetc(fp);
			if (c == '\n')
			{
				value[i++] = '\r';
				continue;
			}
			else if (c == '\r')
			{
				continue;
			}
			value[i++] = (char)c;
		} 
		while (c != '"');

		// terminate the string (overwrite the quote (")).
		value[i-1] = '\0';

		strcpy(m_value,value);

		while (c!='\n') { c = fgetc(fp); } 
	}

	void export(const char*label,FILE *fp)
	{
		char temp[MAX_MFFSTRING_SIZE];

		if (label)
			fprintf(fp,"%s \"",label);
		
		char *dest = temp;

		for (int i = 0; i < strlen(m_value); i++)
		{
			if (m_value[i] == '\n')
			{
//				*dest = '\n';
//				dest++;
				continue;
			}
			else if (m_value[i] == '\r')
			{
				*dest = '\n';
				dest++;
				continue;
			}
			
			*dest = m_value[i];
			dest++;
		}
		*dest = '\0';

		fprintf(fp,"%s\"\n",temp);
	}
	mffAttribute::mffAttrType GetAttrType() { return mffAttribute::kSTRING; }
	char * GetAttrTypeStr() { return "string"; }

	mffString &operator = ( const char *c_str )
	{
		strcpy(m_value,c_str);
		return *this;
	}
	mffString &operator = ( const mffString &other )
	{
		*(mffAttribute*)this = *(mffAttribute*)&other;
		strcpy(m_value,other.m_value);
		return *this;
	}
};

class mffStringArray : public mffAttribute
{
public:
	std::vector<mffString> m_values;

	void import(FILE *fp)
	{
		char line[MAX_LINE_SIZE];
		int count = 0;
		fscanf(fp,"[ %s ",line);

		fscanf(fp,"(%i)",&count);
		m_values.resize(count);
		for (int i = 0; i < count; i++)
		{
			fscanf(fp,"%s",line);
			mffString value(line);
			m_values[i] = value;
		}
		GetLine(fp,MAX_LINE_SIZE,line);
	}
	void export(FILE *fp)
	{
		int count = m_values.size();
		fprintf(fp,"[ stringarray (%i)",count);
		for (int i = 0; i < m_values.size(); i++)
			fprintf(fp," %s",m_values[i].c_str());
		fprintf(fp," ]\n");
	}

	void push_back(mffString value) { m_values.push_back(value); }
	mffString operator[]( int index ) const { return m_values[index]; }
 	mffString &operator[]( int index ) { return m_values[index]; }
	int size() { return m_values.size(); }
	void resize(int newsize) { m_values.resize(newsize); }

	mffAttribute::mffAttrType GetAttrType() { return kSTRINGARRAY; }
	char * GetAttrTypeStr() { return "STRINGARRAY"; }
};

class mffEnum : public mffAttribute
{
public:
	int m_value;
	std::vector<mffString> m_names;

	void export(FILE *fp)
	{
		fprintf(fp,"[enum %i %s",m_value,m_names[0].c_str());
		for (unsigned i = 1; i < m_names.size(); i++)
		{
			fprintf(fp,",%s",m_names[i].c_str());
		}
		fprintf(fp,"]\n");
	}
	
	mffAttribute::mffAttrType GetAttrType() { return mffAttribute::kENUM; }
	char * GetAttrTypeStr() { return "ENUM"; }
};

class mffBoolean : public mffAttribute
{
public:
	bool m_value;

	void export(FILE *fp)
	{
		fprintf(fp,"[bool %i]\n",(int)m_value);
	}
	
	mffAttribute::mffAttrType GetAttrType() { return mffAttribute::kBOOLEAN; }
	char * GetAttrTypeStr() { return "BOOLEAN"; }
};

class mffInt : public mffAttribute
{
public:
	int m_value;

	void export(FILE *fp)
	{
		fprintf(fp,"[int %i]\n",m_value);
	}
	mffAttribute::mffAttrType GetAttrType() { return kINTEGER; }
	char * GetAttrTypeStr() { return "INTEGER"; }
};


class mffDoubleArray : public mffAttribute
{
public:

	void import(FILE *fp)
	{
		char line[MAX_LINE_SIZE];
		int count = 0;
		float value;
		fscanf(fp,"[ %s ",line);

		fscanf(fp,"(%i)",&count);
		m_values.resize(count);
		for (int i = 0; i < count; i++)
		{
			fscanf(fp,"%f",&value);
			m_values[i] = value;
		}
		GetLine(fp,MAX_LINE_SIZE,line);
	}
	void export(FILE *fp)
	{
		int count = m_values.size();
		fprintf(fp,"[ doublearray (%i)",count);
		for (int i = 0; i < m_values.size(); i++)
			fprintf(fp," %f",m_values[i]);
		fprintf(fp," ]\n");
	}
	mffAttribute::mffAttrType GetAttrType() { return kDOUBLEARRAY; }
	char * GetAttrTypeStr() { return "DOUBLEARRAY"; }

	void push_back(double value) { m_values.push_back(value); }
	double operator[]( int index ) const { return m_values[index]; }
 	double &operator[]( int index ) { return m_values[index]; }
	int size() { return m_values.size(); }
	void resize(int newsize) { m_values.resize(newsize); }

	std::vector<double> m_values;

};

class mffVectorArray : public mffAttribute
{
public:

	void import(FILE *fp)
	{
		char line[MAX_LINE_SIZE];
		int count = 0;
		mffVector vect;
		float x,y,z;
		fscanf(fp,"[ %s ",line);
		fscanf(fp,"(%i)",&count);
		m_values.resize(count/3);
		for (int i = 0; i < (count/3); i++)
		{
			fscanf(fp,"%f %f %f",&x,&y,&z);
			m_values[i] = mffVector(x,y,z);
		}
		GetLine(fp,MAX_LINE_SIZE,line);
	}
	void export(FILE *fp)
	{
		int count = m_values.size();
		fprintf(fp,"[ vectorarray (%i)",count*3);
		float x,y,z;
		for (int i = 0; i < m_values.size(); i++)
		{
			x = (float)m_values[i].x;
			y = (float)m_values[i].y;
			z = (float)m_values[i].z;
			fprintf(fp," %f %f %f",x,y,z);
		}
		fprintf(fp," ]\n");
	}
	mffAttribute::mffAttrType GetAttrType() { return kVECTORARRAY; }
	char * GetAttrTypeStr() { return "VECTORARRAY"; }

	void push_back(mffVector value) { m_values.push_back(value); }
	mffVector operator[]( int index ) const { return m_values[index]; }
 	mffVector &operator[]( int index ) { return m_values[index]; }
	int size() { return m_values.size(); }
	void resize(int newsize) { m_values.resize(newsize); }

	std::vector<mffVector> m_values;
};

class mffVector2Array : public mffAttribute
{
public:

	void import(FILE *fp)
	{
		char line[MAX_LINE_SIZE];
		int count = 0;
		mffVector vect;
		float x,y;
		fscanf(fp,"[ %s ",line);
		fscanf(fp,"(%i)",&count);
		m_values.resize(count/2);
		for (int i = 0; i < count/2; i++)
		{
			fscanf(fp,"%f %f",&x,&y);
			m_values[i] = mffVector2(x,y);
		}
		GetLine(fp,MAX_LINE_SIZE,line);
	}
	void export(FILE *fp)
	{
		int count = m_values.size() * 2;
		fprintf(fp,"[ vector2array (%i)",count);
		float x,y;
		for (int i = 0; i < m_values.size(); i++)
		{
			x = (float)m_values[i].x;
			y = (float)m_values[i].y;
			fprintf(fp," %f %f",x,y);
		}
		fprintf(fp," ]\n");
	}
	mffAttribute::mffAttrType GetAttrType() { return kVECTOR2ARRAY; }
	char * GetAttrTypeStr() { return "VECTOR2ARRAY"; }

	void push_back(mffVector2 value) { m_values.push_back(value); }
	mffVector2 operator[]( int index ) const { return m_values[index]; }
 	mffVector2 &operator[]( int index ) { return m_values[index]; }
	int size() { return m_values.size(); }
	void resize(int newsize) { m_values.resize(newsize); }

	std::vector<mffVector2> m_values;

};

class mffColorArray : public mffAttribute
{
public:
	void import(FILE *fp)
	{
		char line[MAX_LINE_SIZE];
		int count = 0;
		float r,g,b,a;
		fscanf(fp,"[ %s ",line);
		fscanf(fp,"(%i)",&count);
		m_values.resize(count/3);
		for (int i = 0; i < (count/3); i++)
		{
			fscanf(fp,"%f %f %f %f",&r,&g,&b,&a);
			m_values[i] = mffColor(r,g,b,a);
		}
		GetLine(fp,MAX_LINE_SIZE,line);
	}
	void export(FILE *fp)
	{
		int count = m_values.size();
		fprintf(fp,"[ colorarray (%i)",count*3);
		float r,g,b,a;
		for (int i = 0; i < m_values.size(); i++)
		{
			r = (float)m_values[i].r;
			g = (float)m_values[i].g;
			b = (float)m_values[i].b;
			a = (float)m_values[i].a;
			fprintf(fp," %f %f %f %f",r,g,b,a);
		}
		fprintf(fp," ]\n");
	}
	mffAttribute::mffAttrType GetAttrType() { return kCOLORARRAY; }
	char * GetAttrTypeStr() { return "COLORARRAY"; }

	void push_back(mffColor value) { m_values.push_back(value); }
	mffColor operator[]( int index ) const { return m_values[index]; }
 	mffColor &operator[]( int index ) { return m_values[index]; }
	void resize(int newsize) { m_values.resize(newsize); }
	int size() { return m_values.size(); }

	std::vector<mffColor> m_values;
	
};

class mffDblIntArray : public mffAttribute
{
public:
	int m_id;
	std::vector<int> m_values1,m_values2;

	void export(FILE *fp)
	{
		int count = m_values1.size();
		fprintf(fp,"[ dblintarray id:%i (%i)",m_id,count);
		for (int i = 0; i < m_values1.size(); i++)
			fprintf(fp," %i",m_values1[i]);
		count = m_values2.size();
		fprintf(fp," (%i)",count);
		for (i = 0; i < m_values2.size(); i++)
			fprintf(fp," %i",m_values2[i]);
		fprintf(fp," ]\n");
	}
	mffAttribute::mffAttrType GetAttrType() { return kDBLINTARRAY; }
	char * GetAttrTypeStr() { return "DBLINTARRAY"; }
};

class mffFloat: public mffAttribute
{
public:
	float m_value;

	void export(FILE *fp)
	{
		fprintf(fp,"[float %f]\n",m_value);
	}

	mffAttribute::mffAttrType GetAttrType() { return mffAttribute::kFLOAT; }
	char * GetAttrTypeStr() { return "FLOAT"; }
};

class mffDouble: public mffAttribute
{
public:
	double m_value;

	void export(FILE *fp)
	{
		float f;
		f = (float)m_value;
		fprintf(fp,"[double %f]\n",(float)f);
	}

	mffAttribute::mffAttrType GetAttrType() { return mffAttribute::kDOUBLE; }
	char * GetAttrTypeStr() { return "DOUBLE"; }
};

class mffNode
{
public:
	mffNode()
	{
		m_index = -1;
		m_parentindex = -1;
		m_refidx = -1;
		m_layerindex = -1;
		m_isexternalreference = 0;
		m_filename = "";
	}
	virtual ~mffNode() {}

	mffString m_name;
	int m_index;
	int m_refidx;
	int m_parentindex;

	mffIntArray m_childindicies;
	mffIntArray m_attrindicies;

	int m_layerindex;
	int m_isexternalreference;
	mffString m_filename;

	virtual const char *GetNodeTypeStr() { return "Node"; }
	virtual mffNodeType GetNodeType() { return kNODE; }
};

class mffModel : public mffNode
{
public:
	mffModel(){}
	virtual ~mffModel(){}

	std::vector<mffShader> m_shaders;

	const char *GetNodeTypeStr() { return "Model"; }
	mffNodeType GetNodeType() { return kMODEL; }
	virtual mffModelType GetModelType() { return kUNKNOWN; };
	virtual const char *GetModelTypeStr() { return "Unknown"; }
};

class mffUVMap 
{
public:
	mffString m_name;
	mffVector2Array m_UVArray;
	mffIntArray m_PolyUVIndicies;
	mffIntArray m_UVCounts;
};

class mffPolyModel : public mffModel
{
public:
	mffPolyModel(){}
	~mffPolyModel(){}

	mffVectorArray m_VertexArray;
	mffVectorArray m_VertexNormalArray;
	
	mffIntArray m_VertexCounts;
	mffIntArray m_PolyVertexIndicies;
	mffIntArray m_VertexNormalIndicies;

	mffIntArray m_ShaderIndicies;

	std::vector<mffUVMap> m_UVMaps;

	const char *GetModelTypeStr() { return "PolygonModel"; }
	mffModelType GetModelType() { return kPOLYGONMODEL; }
};

class mffCurveModel : public mffModel
{
public:
	mffCurveModel(){};

	int m_degree,m_spans,m_form;

	mffVectorArray m_CVArray;
	mffVectorArray m_KnotArray;
	mffIntArray m_CVCounts;
	mffIntArray m_KnotCounts;
	mffIntArray m_CVConnects;
	mffIntArray m_KnotConnects;
	mffDoubleArray m_KnotParams;

	const char *GetModelTypeStr() { return "CurveModel"; }
	mffModelType GetModelType() { return kCURVEMODEL; }
};

class mffTransform
{
public:
	mffTransform(){}

	mffVector translate;
	mffVector rotate;
	mffVector scale;
	mffVector rotatepivot;
	mffVector rotatepivottranslation;
	mffVector scalepivot;
	mffVector scalepivottranslation;
};

class mffGroupTransform : public mffNode
{
public:
	mffTransform m_transform;
	const char *GetNodeTypeStr() { return "GroupTransform"; }
	mffNodeType GetNodeType() { return kGROUPTRANSFORM; }
};

class mffGroupTransformInstance : public mffNode
{
public:
	const char *GetNodeTypeStr() { return "GroupTransformInstance"; }
	mffNodeType GetNodeType() { return kGROUPTRANSFORMINSTANCE; }
};

class mffLight : public mffNode
{
public:
	enum spotType { omni, spot, directional } m_type;
	float m_intensity;
	mffColor m_color;
	mffLight() { m_type = omni; }
	const char *GetNodeTypeStr() { return "Light"; }
	mffNodeType GetNodeType() { return kLIGHT; }
};

class mffLightInstance : public mffNode
{
public:
	const char *GetNodeTypeStr() { return "LightInstance"; }
	mffNodeType GetNodeType() { return kLIGHTINSTANCE; }
};

class mffNULL : public mffNode
{
public:
	const char *GetNodeTypeStr() { return "Null"; }
	mffNodeType GetNodeType() { return kNULL; }
};

class mffNULLInstance : public mffNode
{
public:
	const char *GetNodeTypeStr() { return "NullInstance"; }
	mffNodeType GetNodeType() { return kNULLINSTANCE; }
};

class mffBone : public mffNode
{
public:
	const char *GetNodeTypeStr() { return "Bone"; }
	mffNodeType GetNodeType() { return kBONE; }
};

class mffBoneInstance : public mffNode
{
public:
	const char *GetNodeTypeStr() { return "BoneInstance"; }
	mffNodeType GetNodeType() { return kBONEINSTANCE; }
};

class mffModelInstance : public mffNode
{
public:
	mffModelInstance()
	{
		m_vcid = -1;
	}
	const char *GetNodeTypeStr() { return "ModelInstance"; }
	mffNodeType GetNodeType() { return kMODELINSTANCE; }
	virtual mffModelType GetModelType() { return kUNKNOWN; };
	virtual const char *GetModelTypeStr() { return "Unknown"; }

	int m_vcid;//vertex color reference index
};

class mffShaderTableNode : public mffNode
{
public:
	std::vector<mffShader> m_shaders;

	void push_back(mffShader value) { m_shaders.push_back(value); }
	mffShader operator[]( int index ) const { return m_shaders[index]; }
 	mffShader &operator[]( int index ) { return m_shaders[index]; }
	int size() { return m_shaders.size(); }
	void resize(int newsize) { m_shaders.resize(newsize); }
	void clear() { m_shaders.clear(); }

	const char *GetNodeTypeStr() { return "ShaderTableNode"; }
	mffNodeType GetNodeType() { return kSHADERTABLENODE; }
};

class mffMapTableNode : public mffNode
{
public:
	std::vector<mffMap> m_maps;

	int push_back(mffMap value) { m_maps.push_back(value); return m_maps.size()-1; }
	mffMap operator[]( int index ) const { return m_maps[index]; }
 	mffMap &operator[]( int index ) { return m_maps[index]; }
	int size() { return m_maps.size(); }
	void resize(int newsize) { m_maps.resize(newsize); }
	void clear() { m_maps.clear(); }

	const char *GetNodeTypeStr() { return "MapTableNode"; }
	mffNodeType GetNodeType() { return kMAPTABLENODE; }
};

class mffVertexColorNode : public mffNode
{
public:
	mffColorArray m_VertexColors;

	const char *GetNodeTypeStr() { return "VertexColorNode"; }
	mffNodeType GetNodeType() { return kVERTEXCOLORNODE; }
};

class mffDisplayLayerTableNode : public mffNode
{
public:
	mffIntArray m_layerflags;
	mffStringArray m_layernames;
	mffColorArray m_layercolors;

	const char *GetNodeTypeStr() { return "DisplayLayerTableNode"; }
	mffNodeType GetNodeType() { return kDISPLAYLAYERTABLENODE; }
};

class mffPolyModelInstance : public mffModelInstance
{
public:
	mffPolyModelInstance() {};
	const char *GetModelTypeStr() { return "PolygonModelInstance"; }
	mffModelType GetModelType() { return kPOLYGONMODELINSTANCE; }
};

class mffCurveModelInstance : public mffModelInstance
{
public:
	mffCurveModelInstance() {};
	const char *GetModelTypeStr() { return "CurveModelInstance"; }
	mffModelType GetModelType() { return kCURVEMODELINSTANCE; }
};

class mffTools
{
public:
	mffTools();
	~mffTools();

	int			ImportFile(const char *filename);
	int			ExportFile(const char *filename);

	unsigned		GetNumReferences() { return m_references.size(); }
	unsigned		GetNumInstances() { return m_instances.size(); }
	unsigned		GetNumAttributes() { return m_attributes.size(); }

	int			GetNumInstancesOfReference(mffNode *node);

	mffNode					*GetReferenceNode(const unsigned index) { return m_references[index]; }
	mffNode					*GetInstanceNode(const unsigned index) { return m_instances[index]; } 

	mffPolyModel			*GetPolyModel(const unsigned index);
	mffPolyModelInstance	*GetPolyModelInstance(const unsigned index);
	mffGroupTransform		*GetGroupTransform(const unsigned index);
	mffGroupTransformInstance	*GetGroupTransformInstance(const unsigned index);
	mffAttribute			*GetAttribute(const unsigned index) { return m_attributes[index]; }

	int			AddPolyModel(mffPolyModel &polymodel);
	int			AddPolyModelInstance(mffPolyModelInstance &polymodelinstance,mffIntArray &attrindicies);

	int			AddCurveModel(mffCurveModel &curvemodel);
	int			AddCurveModelInstance(mffCurveModelInstance &curvemodelinstance);

	int			AddGroupTransformInstance(int parentidx,int layerindex,mffGroupTransform &group,mffIntArray &attrindicies);

	int			AddVertexColorNode(mffVertexColorNode &vcnode);
	int			AddShaderTableNode(mffShaderTableNode &shadertablenode);
	int			AddMapTableNode(mffMapTableNode &maptablenode);

	int			AddLight(mffLight &light);
	int			AddLightInstance(int parentindex,mffLight &group);

	int			AddNULL(mffNULL &Null);
	int			AddNULLInstance(int parentindex,mffNULL &Null,mffIntArray &attrindicies);

	int			AddBone(mffBone &bone);
	int			AddBoneInstance(int parentindex,mffBone &bone);

	int			AddDisplayLayerTableNode(mffDisplayLayerTableNode &displaylayertablenode);

	int			AddBoolAttribute(mffBoolean &attribute);
	int			AddIntAttribute(mffInt &attribute);
	int			AddFloatAttribute(mffFloat &attribute);
	int			AddDoubleAttribute(mffDouble &attribute);
	int			AddStringAttribute(mffString &attribute);
	int			AddEnumAttribute(mffEnum &attribute);
	int			AddIntArrayAttribute(mffIntArray &attribute);
	int			AddDblIntArrayAttribute(mffDblIntArray &attribute);

protected:
	int			AddGroupTransform(mffGroupTransform &group);

	void		ImportShaderTable(std::vector<mffShader> &Shaders);
	void		ExportShaderTable(std::vector<mffShader> &shaders);

	void		ImportMapTable(std::vector<mffMap> &maps);
	void		ExportMapTable(std::vector<mffMap> &maps);

	void		ImportReferences();
	void		ImportInstances();

	void		ExportReferenceNode(mffNode *node);
	void		ImportReferenceNode(mffNode *node);

	void		ImportInstanceNode(mffNode *node);
	void		ExportInstanceNode(mffNode *node);

	void		ImportModelInstance(mffModelInstance *modelinstance);
	void		ExportModelInstance(mffModelInstance *modelinstance);

	void		ImportPolygonModel(mffPolyModel &model);
	void		ImportPolygonInstance(mffPolyModelInstance &instance);
	void		ExportPolygonModel(mffPolyModel &polymodel);
	void		ExportPolygonModelInstance(mffPolyModelInstance &polymodelinstance);
	
	void		ImportTransform(mffTransform &transform);
	void		ExportTransform(mffTransform &transform);

	void		ImportMap(mffMap &map);
	void		ExportMap(mffMap &map,int index);

	void		ImportGroupTransform(mffGroupTransform &group);
	void		ImportGroupTransformInstance(mffGroupTransformInstance &groupinstance);
	void		ExportGroupTransform(mffGroupTransform &group);
	void		ExportGroupTransformInstance(mffGroupTransformInstance &groupinstance);

	int			ImportAttributes();
	void		ExportAttributes();

	void		ImportVertexColors(mffVertexColorNode &vertexcolornode);
	void		ExportVertexColors(mffVertexColorNode &vertexcolornode);

	void		ImportDisplayLayers(mffDisplayLayerTableNode &displaylayertable);
	void		ExportDisplayLayers(mffDisplayLayerTableNode &displaylayertable);

	void		ImportLight(mffLight &light);
	void		ImportLightInstance(mffLightInstance &lightinstance);
	void		ExportLight(mffLight &light);
	void		ExportLightInstance(mffLightInstance &lightinstance);

	void		ImportCurveModel(mffCurveModel &curvemodel);
	void		ExportCurveModel(mffCurveModel &curvemodel);
	void		ImportCurveModelInstance(mffCurveModelInstance &curvemodelinstance);
	void		ExportCurveModelInstance(mffCurveModelInstance &curvemodelinstance);

	void		ImportNULL(mffNULL &Null);
	void		ExportNULL(mffNULL &Null);
	void		ImportNULLInstance(mffNULLInstance &nullinstance);
	void		ExportNULLInstance(mffNULLInstance &nullinstance);

	void		ImportBone(mffBone &bone);
	void		ExportBone(mffBone &bone);
	void		ImportBoneInstance(mffBoneInstance &boneinstance);
	void		ExportBoneInstance(mffBoneInstance &boneinstance);
	
	float		GetVersion();

	void		ExportUVMaps(std::vector<mffUVMap> &uvmaps);
	void		ExportUVMap(mffUVMap &uvmap);
	void		ImportUVMaps(std::vector<mffUVMap> &uvmaps);
	void		ImportUVMap(mffUVMap &uvmap);

public:
	std::vector<mffNode*> m_references;
	std::vector<mffNode*> m_instances;
	std::vector<mffAttribute*> m_attributes;
	FILE *m_fp;
};


inline void GetLine(FILE *fp,int maxlength,char *line)
{
	char c;
	int i = 0;

	while ((i < maxlength)||(!feof(fp)))
	{
		c = (char)fgetc(fp);

		if (c == '\r')
		{
			continue;
		}
		if (c == '\n')
		{
			line[i] = '\0';
			return;
		}
		line[i++] = c;

	}
}