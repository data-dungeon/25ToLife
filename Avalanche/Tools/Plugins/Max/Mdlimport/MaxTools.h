////////////////////////////////////////////////////////////////////////////
//
// MaxTools
//
// Handle converting between mdllib and 3dsMax
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MAXTOOLS__H
#define __MAXTOOLS__H

#include "hspline.h"
#include "hpatch.h"
#include "hstatus.h"
#include "hvector.h"
#include "hintarray.h"
#include "hpoly.h"
#include "hgroup.h"
#include "hshader.h"
#include "hchorchannel.h"

#include "uiflags.h"

#include <ikctrl.h>
#include "maxscrpt/MAXScrpt.h"
#include "maxmodelinfo.h"
#include <modstack.h>

class Mesh;
class INode;
class Interface;
class mdlObject;

class MaxTools
{
public:
	MaxTools(mdlObject &m);
	virtual ~MaxTools();

	HStatus				CreatePolygons(Mesh *mesh);

	INode				*CreateSplinePatchModel(Interface *mainI,HStatus &status);

	HStatus				CreateSplineCage(BezierShape *beziershape);

	HStatus				CreateSkeleton(Interface *mainI,maxModelInfo &modelinfo);
	HStatus				SetLinkSizes(Interface *mainI,maxModelInfo &modelinfo);
	
	HStatus				CreateMaterials(Interface *mainI);
	HStatus				CreateMaterial(Interface *mainI,int index);
	HStatus				AssignMaterials(Interface *mainI,INode *node,Mesh *mesh);

	static void			ConvertHSplineToSpline3D(HSpline *from,Spline3D *to,double scaleratio);
	static Matrix3		GetTransformMatrixFromHash(const HVector &translate,const HVector &rotate,const HVector &scale);
	static void			SetTransformMatrixFromHash(INode *node, TimeValue t,const HVector &translate,const HVector &rotate,const HVector &scale);
	static void			SetAssignDfltMatrixControllerFromHash(INode *node, TimeValue t,const HVector &translate,const HVector &rotate,const HVector &scale);
	static bool			isReferenceOfType(RefTargetHandle ref,TSTR type);
	static double		GetUnitRatio();

	MtlList loadMtls; // the materials that have been created

	mdlObject *theModel;

	UIFlags ui_flags;
	unsigned ui_numsubdivisions;
	
	double unitratio;

	CharStream* out;

private:
	INode				*CreateBone(HBone *bone,Interface *mainI,bool endbone,HStatus &status,float &boneLengthFill);
	HStatus				AssignController(INode *node,IKMasterControl *master,bool makeEE);

	void				SimHashShaderNetwork(HShader *shader,StdMat2 *m);

	BitmapTex			*MakeTextureMap(HMap *map);
	MultiTex			*MakeCompositeTexture(int count,HMap::mType mapType,HShader *shader);	

	MultiTex			*MakeMixTexture(int count,int index,std::vector<int> &indicies,HShader *shader);
	MultiTex			*MakeMixTextureChain(std::vector<int> &indicies,HShader *shader);

	MultiTex			*MakeRGBMultiplyTexture(int count,int index,std::vector<int> &indicies,HShader *shader);
	MultiTex			*MakeRGBMultiplyTextureChain(std::vector<int> &indicies,HShader *shader);

	void				MakeCookieCutTexture(int count,StdMat2 *m,HShader *shader);

	HMap				*GrabFirstMap(HMap::mType mapType,HShader *shader);

	TSTR				GetRelativePath(const char *mapPath);

	HStatus				AssignWeights(INode *meshNode,Interface *mainI);
	
public:
	Modifier	*skinModifier;
	IDerivedObject *derivedObject;

	std::vector<INode *> maxbonelist;
	std::vector<HBone *> mybonelist;

};

#endif