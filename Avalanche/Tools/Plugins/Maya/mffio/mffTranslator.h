#include <maya/MPxFileTranslator.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MObjectArray.h>
#include <maya/MStringArray.h>
#include <maya/MDoubleArray.h>

#include "../../libraries/mtools/intmap.h"
#include "../../libraries/mfftools/mfftools.h"
#include "../../libraries/mtools/mtools.h"
#include <tupperware\hash.h>

class MeshHash
{
public:
	MeshHash() {};
	MeshHash(MDagPath dagpath)
	{
		MStatus status;
		MFnMesh fnmesh(dagpath);
		vertexpoollength = fnmesh.numVertices();
		MPointArray vertices;
		fnmesh.getPoints(vertices,MSpace::kObject);
		firstposition = vertices[0];
		lastposition = vertices[vertices.length()-1];
		midposition = vertices[vertices.length()*0.5f];
	}

	int	GetHashValue() { return HashValue::GetHashValue(this,sizeof(this)); }

public:
	int vertexpoollength;
	MVector firstposition;
	MVector midposition;
	MVector lastposition;	
};

class MeshTest
{
public:
	MeshTest()
	{
		pVertices = NULL;
		numVerts = -1;
		hashValue = -1;
	}
	MeshTest(MDagPath dagpath)
	{
		MStatus status;

		MeshHash hash(dagpath);
		hashValue = hash.GetHashValue();

		MFnMesh fnmesh(dagpath);
		numVerts = fnmesh.numVertices();
		MPointArray vertices;
		fnmesh.getPoints(vertices,MSpace::kObject);
		pVertices = new MPoint[numVerts];
		for (int i = 0; i < numVerts; i++)
			pVertices[i] = vertices[i];
	}
	MeshTest(const MeshTest &other)
	{
		numVerts = other.numVerts;
		hashValue = other.hashValue;
		pVertices = new MPoint[numVerts];
		memcpy(pVertices,other.pVertices,sizeof(MPoint)*numVerts);
	}
	~MeshTest()
	{
		if (pVertices)
			delete [] pVertices;
	}

	bool operator == ( MeshTest &other )
	{
		if (hashValue == other.hashValue)
			return true;
		if (memcmp(pVertices,other.pVertices,sizeof(MPoint)*numVerts)==0)
			return true;
		return false;
	}

	int numVerts;
	int hashValue;
	MPoint *pVertices;
};

class mffTools;

typedef std::vector<int> IntArray;

class mffTranslator : public MPxFileTranslator {
public:
                    mffTranslator () {};
    virtual         ~mffTranslator () {};


	static void*	creator();

    MStatus			reader ( const MFileObject& file,
                             const MString& optionsString,
                             FileAccessMode mode);

    MStatus			writer ( const MFileObject& file,
                             const MString& optionsString,
                             FileAccessMode mode );

    bool			haveReadMethod () const;
    bool			haveWriteMethod () const;
    MString			defaultExtension () const;
    MFileKind		identifyFile ( const MFileObject& fileName,
                                   const char* buffer,
                                   short size) const;


	MStatus		CreatePolygonMesh(mffPolyModel *polymodel,MDagPath &result,mffPolyModelInstance *polymodelinstance);
	MStatus		CreatePolygonMesh2(mffPolyModel *polymodel,MDagPath &result,mffPolyModelInstance *polymodelinstance);

	MStatus		CreateCurve(mffCurveModel *curvemodel,MDagPath &dagpath,mffCurveModelInstance *curvemodelinstance);

	MStatus		ExportSelected();
	MStatus		ExportAll();

	int			ExportPolygonMesh(MDagPath mdagPath,int &vcid,MStatus &status);
	int			ExportPolygonMeshInstance(int parentidx,int refidx,int layerindex,MDagPath &mdagPath,mffIntArray *attrindicies);

	int			ExportGroupTransformInstance(int parentidx,int layerindex,MDagPath &mdagPath,mffIntArray *attrindicies);

	int			ExportCurve(MDagPath mdagPath,MObject mComponent,int &vcid,MStatus &status);
	int			ExportCurveInstance(int parentidx,int refidx,MDagPath &mdagPath);

	int			ExportLightInstance(int parentidx,MDagPath &mdagPath);

	int			ExportLocatorInstance(int parentidx,MDagPath &mdagPath,mffIntArray *attrindicies);

	int			ExportJointInstance(int parentidx,MDagPath &mdagPath);

protected:

	void		mffVectorToMVector(std::vector <mffVector> vectorArray, MVectorArray &mvectorArray);
	void		mffVectorToMPoint(std::vector <mffVector> vectorArray, MPointArray &mpointArray);
	void		intToMInt(std::vector <int> intArray, MIntArray &mintArray);
	void		doubleToMDouble(std::vector <double> doublearray, MDoubleArray &mdoublearray);

	void		CreateShaderTable(MObjectArray &SGSets,std::vector<mffShader> &shaders);

	int			GetMap(MObject filenode,mffMap &mffmap,mffMapObject *mapobject);
	int			GetPlacementNodeInfo(MObject filenode,mffMap &mffmap);
	int			GetUVChooserInfo(MObject node,mffMap &mffmap);

	MStatus		CreateShadingGroups(std::vector<mffShader> &shaders,MObjectArray &sgsets);
	int			CreateShadingGroup(std::vector<mffShader> &shaders,unsigned index,MObjectArray &sgsets);

	int			LinkUpMapNode(MString shadernode,MString attrname,mffMap *mffmap,bool asAlpha=false);

	int			CreatePlacementNode(MString filenode,mffMap *mffmap,MString &placementnode);
	int			CreateUVChooserNode(MString placementnode,mffMap *mffmap,MString &uvchoosernode);
	int			HookUpChooser(MDagPath dagpath,mffMap *mffmap);

	MString		FixFileName(const char *filename);

	int			ExportGroupTransform(MDagPath &mdagPath);
	MStatus		GetTransform(mffTransform &transform,MDagPath &mdagPath);
	void		SetTransform(mffTransform *transform,MDagPath dagpath);

	int			ExportDagPaths(std::vector<MDagPath> &mdagPathArray);
	int			ExportBranch(MDagPath mdagPath,std::vector<MDagPath> mdagPathArray);

	void		AddAttribute(mffAttribute *attr,MDagPath dagpath);

	void		CreateBranch(MObject parentobject,mffNode *node,mffNode *parent,std::vector<int> &flags);

	int			ExportPolygonVertexColors(MDagPath dagPath);
	void		AddAttributesForExport(MObject object,mffIntArray &attrindicies);

	void		SetNameFromHierarchy(mffNode *node,MObject object);
	
	int			CreateMeshTestTable();

protected:
	mffTools *m_mff;

	std::vector<MDagPath> m_references;
	
	std::vector<mffIntArray> m_attributestacks;

	MObjectArray m_layers;
	IntMap m_layermap;
	MStringArray m_layernames;

	IntMap m_instmap;
	IntMap m_attrmap;

	MObjectArray m_attributes;


	int m_count,m_oldcount;
	
	mffShaderTableNode m_shadertable;
	mffShaderTableNode *m_pShaderTable;
	mffMapTableNode m_maptable;
	mffMapTableNode *m_pMapTable;

	IntMap m_sgsetmap;
	MObjectArray m_sgsets;
	MObjectArray m_filenodes;
	MObjectArray m_uvchoosernodes;

	IntMap m_shadermap;
	IntMap m_shadinggroupmap;
	IntMap m_filenodemap;
	IntMap m_uvchoosernodemap;

	std::vector<MeshTest> m_meshtesttable;
	IntMap m_meshtestmap,m_meshtesthash;


	bool m_consolshapes;
	bool m_shadertableperobject;
	bool m_oneshaderperobject;

};

extern const char *const mffOptionScript;
extern const char *const mffDefaultOptions;
