////////////////////////////////////////////////////////////////////////////
//
// TupImport
//
// Class to handle importing max data into the tupperware graphics format
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TupImport.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUP_IMPORT_H
#define TUP_IMPORT_H

#include "resource.h"

#define CFGFILENAME		_T("TUPIMPORT.CFG")	// Configuration file

#define TUPIMPORT_CLASS_ID	Class_ID(0x3f3c7a07, 0x670f7cae)

class BoneObj;
class StdUV;

class TupImport : public SceneImport 
{
	// Dialog Options
	BOOL m_outputASCII;

public:

	enum ModifierType
	{
		MODIFIER_APPLY_VC,
		MODIFIER_VERTEX_PAINT,
		MODIFIER_HOLD_VC,
		MODIFIER_FACE_ATTRIBUTE,
		MODIFIER_SKIN,
		MODIFIER_CLOTH,
		MODIFIER_UNKNOWN
	};

	static HWND hParams;

	int				ExtCount();					// Number of extensions supported
	const char *	Ext(int n);					// Extension #n (i.e. "tup")
	const char *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const char *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const char *	AuthorName();				// ASCII Author name
	const char *	CopyrightMessage();			// ASCII Copyright message
	const char *	OtherMessage1();			// Other message #1
	const char *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void ShowAbout(HWND wnd);

	BOOL SupportsOptions(int ext, DWORD options);

	// Dialog Options
	void	SetOutputASCII(BOOL val) { m_outputASCII = val; }
	BOOL	GetOutputASCII(void) { return m_outputASCII; }

	//Constructor/Destructor

	Interface *m_pInterface;

	TupImport();
	~TupImport();		

private:
	int	DoImport(const char *name,ImpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE);

	// ImportScene
	int ImportScene(const char *sourceFilename,const char *outputFilename=NULL,int nameCode=0,BOOL nonCodedNodesOnly=FALSE);
	void AddNodesToPools(INode *pRootNode,BOOL nonCodedNodesOnly);
	void AddNodesToScene(INode *pRootNode,TupNode &tupRootNode,int rootIndex,BOOL nonCodedNodesOnly);

	// ImportNode
	void AddNode(int parentNodeReference,int nodeReference,TupperwareAggregate *pNodeAgg,INode *pNode,bool importSelected=FALSE);
	void AddChildrenNodesToPools(INode *pNode,bool importSelected=FALSE);

	// ImportTransform
	void AddTransform(TupNode &tupNode,INode *pNode);
	void ConvertMaxMatrix(const Matrix3 &srcMatrix,Graphics4x4 &dstMatrix);

	// ImportMaterial
	Mtl *TupImport::AddMaterial(int materialRef);
	StdMat2 *AddStandardMaterial(TupperwareAggregate *pMaterialAgg);
	MultiMtl *AddMultiSubObjectMaterial(TupperwareAggregate *pMaterialAgg);
	Mtl *AddBlendMaterial(TupperwareAggregate *pMaterialAgg);
	Mtl *AddCompositeMaterial(TupperwareAggregate *pMaterialAgg);
	Mtl *AddDoubleSidedMaterial(TupperwareAggregate *pMaterialAgg);

	// ImportMap
	Texmap *AddMap(int mapRef);
	BitmapTex *AddBitmapMap(TupperwareAggregate *pMapAgg);
	MultiTex *AddCompositeMap(TupperwareAggregate *pMapAgg);
	MultiTex *AddMixMap(TupperwareAggregate *pMapAgg);
	void SetMapping(StdUV *pUVGen,int mappingType);

	// ImportImage
	char *GetImagePathName(int imageRef);

	// ImportObject
	int AddObject(Object *pObject);
	void AddNonDerivedObject(Object *pObject,TupObject &tupObject);
	void AddMesh(TupperwareAggregate *objectAgg,TriObject *triObject);
	Point3 GetVertexNormal(Mesh *mesh, int faceNo,RVertex* rv);
	void AddCamera(TupperwareAggregate *pObjectAgg,GenCamera *pCamera);
	void AddLight(TupperwareAggregate *pObjectAgg,GenLight *pLight);
	void AddDerived(TupperwareAggregate *pObjectAgg,IDerivedObject *pDerivedObject,int numModifiers,bool bAddObjectReference);
	void AddXRef(TupperwareAggregate *pObjectAgg,IXRefObject *pXRefObject);
	void AddBezierShape(TupperwareAggregate *pObjectAgg,BezierShape &bezierShape);
	void AddBone(TupperwareAggregate *pObjectAgg,BoneObj *pBoneObject);
	void AddUnknownObject(TupperwareAggregate *pObjectAgg,Object *pObject);

	// ImportModifier
	int AddModifier(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext);
	ModifierType GetModifierType(Modifier *pModifier);
	void AddApplyVCModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifier);
	void AddApplyVCModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext);
	void AddVertexPaintModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifier);
	void AddVertexPaintModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext);
	void AddHoldVCModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifier);
	void AddHoldVCModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext);
	void AddFaceAttributesModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase);
	void AddFaceAttributesModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext);
	void AddSkinModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase);
	void AddSkinModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext);
	void AddClothModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifierBase);
	void AddClothModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext);
	void AddUnknownModifier(TupperwareAggregate *pModifierAgg,Modifier *pModifier);
	void AddUnknownModifierContext(Modifier *pModifier,TupperwareAggregate *pModifierContextAgg,ModContext *pModifierContext);

	// Configuration Settings
	TupString GetCfgFilename(void);
	BOOL ReadConfig(void);
	void WriteConfig(void);

	TupperwareAggregate *m_pSceneAgg;
	TupNodePool *m_pNodePool;
	TupObjectPool *m_pObjectPool;
	TupModifierPool *m_pModifierPool;
	TupMaterialPool *m_pMaterialPool;
	TupMapPool *m_pMapPool;
	TupImagePool *m_pImagePool;
	TupString m_sourceFilename;
	TupString m_outputFilename;
	BOOL m_xrefImport;

	TupArray<INode *> *m_pNodePoolPtrs;
	TupArray<Object *> *m_pObjectPoolPtrs;
	TupArray<Mtl *> *m_pMaterialPoolPtrs;
	TupArray<Modifier *> *m_pModifierPoolPtrs;
	TupArray<Texmap *> *m_pMapPoolPtrs;
	Keeper<TupString> *m_pXRefFilenameStrings;

};

extern char *GetString(int id);
extern HINSTANCE hInstance;
extern BOOL CALLBACK TupImportOptionsDlgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);



#endif // __TUPIMPORT__H
