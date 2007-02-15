/**********************************************************************
 *<
	FILE: mffio.h

	DESCRIPTION:	MFF "EXPORT" PLUGIN

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
#include <stdmat.h>

#include "intmap.h"

#pragma warning (disable : 4786)
#pragma warning (disable : 4530)

#include "../../libraries/mfftools/mfftools.h"


class tPoly
{
public:
	tPoly()
	{
		shaderindex = -1;
		visited = 0;
		facenormalindex = -1;
	}

	std::vector<int> vindicies;
	std::vector<int> uvindicies;
	std::vector<int> nindicies;
	std::vector<int> eindicies;
	int facenormalindex;
	int shaderindex;
	int visited;
};

class tEdge
{
public:
	tEdge(){};

	int v0,v1;
	int vis;

	int polyindex;
	int reledgeindex;
	int adjedgeindex;
};

#define MFFIO_CLASS_ID	Class_ID(0x7a7c6082, 0x5c3708a7)

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

class Mffio : public SceneExport {
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

	BOOL			SupportsOptions(int ext, DWORD options);
	int				DoExport(const TCHAR *filename,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

	//Constructor/Destructor

	Mffio();
	~Mffio();
		
protected:
	int				ExportNode(INode *pNode,Interface *pInterface);
	int				ExportPolygonMesh(Mesh *mesh,Interface *pInterface,mffPolyModel *polymodel);
	int				ExportPolyInstance(int refidx,INode *pNode);
	int				ExportLightInstance(int refidx,INode *pNode);
	Point3			GetVertexNormal(Mesh *mesh, int faceNo,RVertex* rv);

	int				ExportMaterial(Mtl *pMaterial,mffModel *model);
	int				GetStandardMaterial(StdMat2 *mat,mffModel *model);
	int				GetMultiSubMaterial(MultiMtl *mat,mffModel *model);
	int				GetMap(Texmap *pMap,mffShader &shader);
	int				GetBitmapMap(BitmapTex *pBitmapTex,mffShader &shader);
	int				GetCompositeMap(MultiTex *pMultiTex,mffShader &shader);
	int				GetMixMap(MultiTex *pMultiTex,mffShader &shader);

	double			GetEdgeLength(tEdge *e,std::vector<mffVector> &positions);
	int				LegalPoly(tPoly *poly,std::vector<tEdge> &edges,std::vector<mffVector> &positions);

	int				EvalPoly(tPoly *poly,tPoly &newpoly);
	int				CombinePolyAtEdge(tEdge *edge,tPoly *poly0,tPoly &newpoly);
	int				EdgesEqual(tEdge *edge0, tEdge *edge1);

	void			ExportTransform(INode *pNode,mffTransform &transform,int flag = 0);

protected:
	mffTools *m_mff;
	IntMap m_matmap;
	std::vector<tPoly> polys;
	std::vector<tEdge> edges;

};


class MffioClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new Mffio(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return MFFIO_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("Mffio"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

#endif // __MFFIO__H
