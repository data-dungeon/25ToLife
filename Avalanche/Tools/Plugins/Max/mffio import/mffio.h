/**********************************************************************
 *<
	FILE: mffio.h

	DESCRIPTION:	MFF "IMPORT" PLUGIN

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __MFFIO__H
#define __MFFIO__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "../../libraries/mfftools/mfftools.h"
#include "stdmat.h"
#include "modstack.h"
#include "applyvc.h"

#include <direct.h>
#include <commdlg.h>
#include <vector>

#include "intmap.h"

class String;
class StringArray;

#define MFFIO_CLASS_ID	Class_ID(0x7714c533, 0x566fc810)

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

class Mffio : public SceneImport
{
public:
	static HWND hParams;



	int				ExtCount();					// Number of extensions supported
	const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const TCHAR *	AuthorName();				// ASCII Author name
	const TCHAR *	CopyrightMessage();			// ASCII Copyright message
	const TCHAR *	OtherMessage1();			// Other message #1
	const TCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	int				DoImport(const TCHAR *name,ImpInterface *i,Interface *gi, BOOL suppressPrompts=FALSE);	// Import file
		
	//Constructor/Destructor

	Mffio();
	~Mffio();

protected:
	INode *CreatePolygonModel(mffPolyModel *polymodel,mffPolyModelInstance *polymodelinstance,Interface *mainI);
	int CreatePolygonMesh(Mesh *mesh,mffPolyModel &model);
	void FillVertexColors(Mesh &mesh,ApplyVCMod &mod,mffVertexColorNode &vcnode,mffPolyModel &model);
	void SetFaceColorPoint(FaceColor &faceCol,mffVertexColorNode &vcnode,int vcIndex,int pointIndex);
	int CreateMaterials(Interface *mainI,std::vector<mffShader> &Shaders,MtlList &loadMtls);
	int CreateMaterial(Interface *mainI,mffShader *shader,MtlList &loadMtls);
	void CreateShaderNetwork(mffShader *shader,StdMat2 *m);
	BitmapTex *MakeTextureMap(mffMap *pMap,bool invert = false);
	int AssignMaterials(Interface *mainI,INode *node,Mesh *mesh,MtlList &loadMtls);
	MultiTex *MakeMixTexture(mffBlender *blender);
	Matrix3 GetTransformMatrix(const mffTransform &transform,int type = 0);
	void AddAttribute(mffAttribute *attr,INode *node);

	INode *CreateCurve(mffCurveModel *curvemodel,Interface *mainI);

	void CreateBranch(Interface *mainI,mffNode *node,mffNode *parent,std::vector <INode*> &nodes,std::vector<int> attrindicies);
	void AddKeyValue(String &mybuffer,String key,String value);

	int FindKey(String key,StringArray &notelines,String &value);
	int FindLine(String line,StringArray &notelines);

protected:
	mffTools *m_mff;

	std::vector<INode *> m_refNodes;
	std::vector<Object *> m_refObjects;

};


class MffioClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new Mffio(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_IMPORT_CLASS_ID; }
	Class_ID		ClassID() { return MFFIO_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("Mffio"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

#endif
