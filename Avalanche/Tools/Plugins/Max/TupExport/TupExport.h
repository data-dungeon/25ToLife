////////////////////////////////////////////////////////////////////////////
//
// TupExport
//
// Class to handle exporting max data into the tupperware graphics format
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUP_EXPORT_H
#define TUP_EXPORT_H

#include "resource.h"

#define CFGFILENAME		_T("TUPEXPORT.CFG")	// Configuration file

#define TUPEXPORT_CLASS_ID	Class_ID(0x69dbf2c4, 0x3c2fa415)

class BoneObj;

class TupExport : public SceneExport 
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

	TupExport();
	~TupExport();		

private:
	int	DoExport(const char *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

	// ExportScene
	int ExportScene(const char *sourceFilename,const char *outputFilename,BOOL exportSelected=FALSE,int nameCode=0,BOOL nonCodedNodesOnly=FALSE);
	void AddNodesToPools(INode *pRootNode,BOOL nonCodedNodesOnly);
	void AddNodesToScene(INode *pRootNode,TupNode &tupRootNode,int rootIndex,BOOL nonCodedNodesOnly);

	// ExportNode
	void AddNode(int parentNodeReference,int nodeReference,TupperwareAggregate *pNodeAgg,INode *pNode,bool exportSelected=FALSE);
	void AddChildrenNodesToPools(INode *pNode,bool exportSelected=FALSE);

	// ExportTransform
	void AddTransform(TupNode &tupNode,INode *pNode);
	void ConvertMaxMatrix(const Matrix3 &srcMatrix,Graphics4x4 &dstMatrix);

	// ExportMaterial
	int AddMaterial(Mtl *pMaterial);
	void AddStandardMaterial(TupperwareAggregate *pMaterialAgg,StdMat2 *pMaterial);
	void AddMultiSubObjectMaterial(TupperwareAggregate *pMaterialAgg,MultiMtl *pMaterial);
	void AddBlendMaterial(TupperwareAggregate *pMaterialAgg,Mtl *pMaterial);
	void AddCompositeMaterial(TupperwareAggregate *pMaterialAgg,Mtl *pMaterial);
	void AddDoubleSidedMaterial(TupperwareAggregate *pMaterialAgg,Mtl *pMaterial);

	// ExportMap
	int AddMap(Texmap *pMap);
	void AddBitmapMap(TupperwareAggregate *pMapAgg,BitmapTex *pMap);
	void AddCompositeMap(TupperwareAggregate *pMapAgg,MultiTex *pMap);
	void AddMixMap(TupperwareAggregate *pMapAgg,MultiTex *pMap);
	int GetMapping(StdUVGen *pUVGen);

	// ExportImage
	int AddImage(const TupString &mapName);

	// ExportObject
	int AddObject(Object *pObject,const char *pNodePathName);
	void AddNonDerivedObject(Object *pObject,TupObject &tupObject,const char *pNodePathName);
	void AddMesh(TupperwareAggregate *objectAgg,TriObject *triObject,const char *pNodePathName);
	Point3 GetVertexNormal(Mesh *mesh, int faceNo,RVertex* rv);
	void AddCamera(TupperwareAggregate *pObjectAgg,GenCamera *pCamera,const char *pNodePathName);
	void AddLight(TupperwareAggregate *pObjectAgg,GenLight *pLight,const char *pNodePathName);
	void AddDerived(TupperwareAggregate *pObjectAgg,IDerivedObject *pDerivedObject,int numModifiers,bool bAddObjectReference,const char *pNodePathName);
	void AddXRef(TupperwareAggregate *pObjectAgg,IXRefObject *pXRefObject,const char *pNodePathName);
	void AddBezierShape(TupperwareAggregate *pObjectAgg,BezierShape &bezierShape,const char *pNodePathName);
	void AddBone(TupperwareAggregate *pObjectAgg,BoneObj *pBoneObject,const char *pNodePathName);
	void AddUnknownObject(TupperwareAggregate *pObjectAgg,Object *pObject,const char *pNodePathName);

	// ExportModifier
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
	BOOL m_xrefExport;

	Keeper<INode *> *m_pNodePoolPtrs;
	Keeper<Object *> *m_pObjectPoolPtrs;
	Keeper<Mtl *> *m_pMaterialPoolPtrs;
	Keeper<Modifier *> *m_pModifierPoolPtrs;
	Keeper<Texmap *> *m_pMapPoolPtrs;
	Keeper<TupString> *m_pImagePoolStrings;
	Keeper<TupString> *m_pXRefFilenameStrings;

};

extern char *GetString(int id);
extern HINSTANCE hInstance;
extern BOOL CALLBACK TupExportOptionsDlgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);



#endif // __TUPEXPORT__H
