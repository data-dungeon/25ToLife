#pragma once

#include "HModelTranslatorPlugin.h" 
#include "HModelTranslatorPluginHelper.h" 
#include "HSceneTranslatorPlugin.h" 
#include "HSceneTranslatorPluginHelper.h"
#include "HMotionTranslatorPlugin.h" 
#include "HMotionTranslatorPluginHelper.h"
#include "HMotionSetTranslatorPluginHelper.h"
#include "IntHash.h"
#include "CRC.h"

class HPolyModel;
class HPoly;
class HUV;
class HSurface;
class HBone;
class HTransformableObject;
class HSurfaceDriver;
class HUVMapSurfaceDriver;
class HMotionChannel;
class HFloatMotionCurve;

class TestMaterial
{
public:
   TestMaterial() { m_name = "Untitled"; }
   TestMaterial(const TestMaterial &other) { (*this) = other; }
   String m_name;
   float m_DiffuseColor[3];
   float m_AmbientColor[3];
   float m_SpecularColor[3];
   float m_Transprency;
   CArray<HSurfaceDriver*> m_surfacedrivers;
   TestMaterial &operator = ( const TestMaterial &other );
   BOOL operator == ( const TestMaterial &other ) const;
};

class MaterialMapInfo
{
public:
   MaterialMapInfo() { m_nSubMapIndex = 0; }
   TupperwareAggregate *m_pStandardMaterialMapAgg;
   TupperwareAggregate *m_pCompositeMapAgg;
   int m_nSubMapIndex;
   MaterialMapInfo &operator = ( const MaterialMapInfo &other )
   {
      m_nSubMapIndex = other.m_nSubMapIndex;
      m_pStandardMaterialMapAgg = other.m_pStandardMaterialMapAgg;
      m_pCompositeMapAgg = other.m_pCompositeMapAgg;
      return *this;
   }
};

class HVertex;

class VertexBoneInfo
{
public:
   int m_nModelRelativeVertexIndex;
   HVertex *m_pVertex;
   CArray<int> m_bones;
   CArray<float> m_weights; 

   VertexBoneInfo &operator = ( const VertexBoneInfo &other )
   {
      m_nModelRelativeVertexIndex = other.m_nModelRelativeVertexIndex;
      m_pVertex = other.m_pVertex;
      m_bones.SetSize(other.m_bones.GetSize());
      for (int i = 0; i < other.m_bones.GetSize(); i++)
         m_bones[i] = other.m_bones[i];
      m_weights.SetSize(other.m_weights.GetSize());
      for (i = 0; i < other.m_weights.GetSize(); i++)
         m_weights[i] = other.m_weights[i];
      return *this;
   }

};

template<class ARG_KEY>
AFX_INLINE UINT AFXAPI HashKey( const TestMaterial &key )
{
   CRC crc;
   for (int i = 0; i < key.m_surfacedrivers.GetSize(); i++)
      crc.AnalyzeData((unsigned long)key.m_surfacedrivers.GetAt(i));
   return (UINT)crc.GetResult();
}

class MaterialUVSetKey
{
public:
   TupperwareAggregate *m_pMaterialAgg;
   UINT m_nUVSetChannel;

public:
   MaterialUVSetKey()
   {
      m_pMaterialAgg = NULL;
      m_nUVSetChannel = 0;
   }
   MaterialUVSetKey(const MaterialUVSetKey &other)
   {
      m_pMaterialAgg = other.m_pMaterialAgg;
      m_nUVSetChannel = other.m_nUVSetChannel;
   }
   MaterialUVSetKey(TupperwareAggregate *pMaterialAgg,UINT nUVSetChannel)
   {
      m_pMaterialAgg = pMaterialAgg;
      m_nUVSetChannel = nUVSetChannel;
   }
   MaterialUVSetKey &operator = ( const MaterialUVSetKey &other )
   {
      m_pMaterialAgg = other.m_pMaterialAgg;
      m_nUVSetChannel = other.m_nUVSetChannel;
      return *this;
   }
   BOOL operator == ( const MaterialUVSetKey &other ) const
   {
      if ((m_pMaterialAgg == other.m_pMaterialAgg)&&(m_nUVSetChannel == other.m_nUVSetChannel))
         return TRUE;
      return FALSE;
   }
};

class HTexCoords;

class MaterialUVSetValue
{
public:
   CMap < UINT, UINT, HUV*, HUV* > m_uvmap;
   HTexCoords *m_pTexCoords;

public:
   MaterialUVSetValue()
   {
      m_pTexCoords = NULL;
   }
   MaterialUVSetValue(HTexCoords *pTexCoords)
   {
      m_pTexCoords = pTexCoords;
   }
   MaterialUVSetValue(const MaterialUVSetValue &other)
   {
      m_pTexCoords = other.m_pTexCoords;
      for (POSITION pos = other.m_uvmap.GetStartPosition(); pos;)
      {
         UINT nIndex;
         HUV *pUV;
         other.m_uvmap.GetNextAssoc(pos,nIndex,pUV); 
         m_uvmap.SetAt(nIndex,pUV);
      }
   }
   MaterialUVSetValue &operator = ( const MaterialUVSetValue &other )
   {
      m_pTexCoords = other.m_pTexCoords;
      for (POSITION pos = other.m_uvmap.GetStartPosition(); pos;)
      {
         UINT nIndex;
         HUV *pUV;
         other.m_uvmap.GetNextAssoc(pos,nIndex,pUV); 
         m_uvmap.SetAt(nIndex,pUV);
      }
      return *this;
   }
};

template<class ARG_KEY>
AFX_INLINE UINT AFXAPI HashKey( const MaterialUVSetKey &key )
{
   CRC crc;
   crc.AnalyzeData((int)key.m_pMaterialAgg);
   crc.AnalyzeData((int)key.m_nUVSetChannel);
   return (UINT)crc.GetResult();
}

class TupModelTranslator : public HModelTranslatorPluginHelper
{
public:
   DECLARE_MODELTRANSLATOR_PLUGINHELPER( TupModelTranslator, HModelTranslatorPluginHelper )

   TupperwareAggregate  *m_pSceneAgg;
   TupScene             *m_pTupScene;
   TupNodePool          *m_pTupNodePool;
   TupObjectPool        *m_pTupObjectPool;
   TupMapPool           *m_pTupMapPool;
   TupImagePool         *m_pTupImagePool;
   TupMaterialPool      *m_pTupMaterialPool;
   TupModifierPool      *m_pTupModifierPool;
 
   IntHash m_imagelookup;
   IntHash m_surfacelookup;
   IntHash m_texcoordslookup;
   IntHash m_maplookup;
   IntHash m_bonelookup;
   IntHash m_vmap;//export
   IntHash m_vertexboneinfomap;// export

   CMap < UINT, UINT, HVertex*,HVertex*> m_vertexmap;
   CMap < TestMaterial, TestMaterial, int, int > m_materiallookup;//for exporting only
   CMap < MaterialUVSetKey, MaterialUVSetKey, MaterialUVSetValue*, MaterialUVSetValue* > m_texcoordsmap;

   HHierObject *m_pModel;
   TupperwareAggregate *m_pModelNodeAgg;
   TupperwareAggregate *m_pWorldNodeAgg;
   int m_nSkinModifierBoneCount;
   CArray<VertexBoneInfo> m_vertexboneinfoarray;//export

public:
   TupModelTranslator();
   ~TupModelTranslator(){}

   // HModelTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "TUP Model"; }
   virtual const char *GetFilterExtension() { return "tup"; }

   virtual HModelTranslatorPlugin::ModelType GetModelType() { return HModelTranslatorPlugin::MT_POLYMODEL; }
 //  virtual HModelTranslatorPlugin::ModelType GetModelType() { return HModelTranslatorPlugin::MT_PATCHMODEL; }

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "Tup Poly"; }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // TUP IMPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasImporter() { return TRUE; }
   virtual void OnImport( HPolyModel *pModel, const char *szFileName);

   // misc TUP helper functions
   TupObject::ObjectTypes GetTupObjectType(TupperwareAggregate *pNodeAgg);
   TupperwareAggregate *FindObject(TupperwareAggregate *pNodeAgg);
   TupperwareAggregate *FindSkinModifier(TupperwareAggregate *pObjectAgg,TupperwareAggregate **pModifierContextAgg);
   TupperwareAggregate *FindVertexColorModifier(TupperwareAggregate *pObjectAgg,TupperwareAggregate **pModifierContextAgg);
   TupperwareAggregate *FindRootBone(TupperwareAggregate *pNodeAgg);

   void InitTupSceneForImport(const char *szFileName);
   void DestroyTupScene();

   void ImportImages();
   void RecurseTupTree(TupperwareAggregate *pNodeAgg,HHierObject *pParent);
   void ImportPolyMesh(TupperwareAggregate *pNodeAgg,HPolyModel *model);
   void ImportMaterials(HPolyModel *pModel,int nMatRef);
   void ImportMultiSubObjectMaterial(TupperwareAggregate *pMaterialAgg,HPolyModel *pModel);
   void ImportMaterial(TupperwareAggregate *pMaterialAgg,HPolyModel *pModel);
   void ImportMap(TupperwareAggregate *pMaterialAgg,TupperwareAggregate *pStandardMaterialMapAgg,HSurface *pSurface);
   void ImportBitmap(TupperwareAggregate *pMaterialAgg,TupperwareAggregate *pMapAgg,HSurface *pSurface);

   void AddFaceToMutiSubObjectMaterial(HPoly *pPoly,TupperwareAggregate *pMaterialAgg,int nCount,int nSubMatIndex);
   void AddFaceToMaterial(HPoly *pPoly,TupperwareAggregate *pMaterialAgg,int nCount);
   void AddFaceToMap(HPoly *pPoly,TupperwareAggregate *pMaterialAgg,TupperwareAggregate *pSubMaterialMapAgg,int nCount);
   void AddFaceToTexCoords(HPoly *pPoly,TupperwareAggregate *pMaterialAgg,TupperwareAggregate *pMapAgg,int nCount);

   void ImportSkeleton(HPolyModel *pPolyModel,TupperwareAggregate *pNodeAgg);
   void ImportBone(HPolyModel *pModel,HBone *pParentBone,TupperwareAggregate *pNodeAgg,TupperwareAggregate *pParentNodeAgg,TupperwareAggregate *pModifierAgg,TupperwareAggregate *pModifierContextAgg);
 
   void ConvertTupMatrixToMatrix44(float *pMatrix,Matrix44 &matrix);

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // EXPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasExporter() { return TRUE; }
   virtual void OnExport( HPolyModel *pModel, const char *szFileName);

   void InitTupSceneForExport(const char *szFileName);

   void RecurseBranch(HHierObject *pHierObject,TupperwareAggregate *pParentAgg);
   TupperwareAggregate *ExportNode(HHierObject *pHierObject,TupperwareAggregate *pParentAgg);
   int ExportObject(HHierObject *pHierObject,TupperwareAggregate *pNodeAgg);
   void ExportTransform(HTransformableObject *pTransformableObject,TupperwareAggregate *pNodeAgg);
   int ExportPolyMesh(HPolyModel *pModel,TupperwareAggregate *pNodeAgg);
   void ExportDefaultMaterial();
   TupperwareAggregate *ExportMultiSubObjectMaterial(TupperwareAggregate *pNodeAgg);
   int GetMaterialIndex(HPoly *pPoly,TupperwareAggregate *pMultiSubObjectAgg);
   int ExportTestMaterial(const TestMaterial &testmaterial,TupperwareAggregate *pMultiSubObjectAgg);
   void BuildTestMaterial(HSurface *pSurface,TestMaterial &testmaterial);
   void ExportSubMap(HUVMapSurfaceDriver *pUVMapSurfaceDriver,const MaterialMapInfo &materialmapinfo);
   void ExportBitMap(HUVMapSurfaceDriver *pUVMapSurfaceDriver,TupperwareAggregate *pBitMapAgg);
   void ExportBone(HTransformableObject *pTransformableObject,TupperwareAggregate *pParentAgg,TupperwareAggregate *pSkinModifierAgg);
   int ExportSkinnedModel(HPolyModel *pModel,TupperwareAggregate *pNodeAgg);
   void ConvertMatrix44ToTupmatrix(Matrix44 matrix,float *pMatrix);
   TupperwareAggregate *FindSkinModifierBone(TupperwareAggregate *pNodeAgg,TupperwareAggregate *pModifierAgg);
   void ExportSkinModifierContext(TupperwareAggregate *pDerivedModifierAgg,HTransformableObject *pTransformableObject);
};

class TupSceneTranslator : public HSceneTranslatorPluginHelper
{
public:
   DECLARE_SCENETRANSLATOR_PLUGINHELPER( TupSceneTranslator, HSceneTranslatorPluginHelper )

   TupModelTranslator   *m_pTupModelTranslator;

   HScene *m_pScene;
   IntHash m_objecthash;

public:
   TupSceneTranslator();

   // HSceneTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "TUP Scene"; }
   virtual const char *GetFilterExtension() { return "tup"; }

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "Tup Scene"; }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // TUP IMPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasImporter() { return TRUE; }
   virtual void OnImport( HScene *pScene, const char *szFileName);

   TupObject::ObjectTypes GetTupObjectType(TupperwareAggregate *pNodeAgg);
   void RecurseTupTree(TupperwareAggregate *pNodeAgg,HHierObject *pParent);
   void ImportTransform(TupperwareAggregate *pNodeAgg,HTransformableObject *pTransformableObject);
   void ConvertTupMatrixToMatrix44(float *pMatrix,Matrix44 &matrix);

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // EXPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasExporter() { return TRUE; }
   virtual void OnExport( HScene *pScene, const char *szFileName);

   void BuildScene( HScene *pScene, const char *szFileName );
   void RecurseBranch(HHierObject *pHierObject,TupperwareAggregate *pParentAgg);
   TupperwareAggregate *ExportNode(HHierObject *pHierObject,TupperwareAggregate *pParentAgg);

};

class HFloatMotionCurveDriver;

class TupMotionTranslator : public HMotionTranslatorPluginHelper
{
public:
   DECLARE_MOTIONTRANSLATOR_PLUGINHELPER( TupMotionTranslator, HMotionTranslatorPluginHelper )

   TupperwareAggregate  *m_pSceneAgg;
   TupScene             *m_pTupScene;
   BOOL                 m_bUseMayaBoneTranslationSpace;
   BOOL                 m_bBinary;
public:
   TupMotionTranslator();

   // HMotionTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "TUP Motion"; }
   virtual const char *GetFilterExtension() { return "anm"; }

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "Tup Motion"; }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // TUP IMPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasImporter() { return TRUE; }
   virtual void OnImport( HMotion *pMotion, const char *szFileName);

   void ImportAnimation(HMotion *pMotion,TupperwareAggregate *pAnimSetPoolAgg);

   void FillOutFloatMotionCurveDriver(HFloatMotionCurve *pFloatMotionCurve,TupperwareAggregate *pAnimCurveAgg,float fMultiply=1.0f);
   TupperwareAggregate *FindCurveForChannelAttribute(const char *channelattributename,TupperwareAggregate *pAnimChannelPoolAgg);

   void AddTranslationCurvesConvertFromMayaBoneSpace(HMotionChannel *pMotionChannel,TupperwareAggregate *pAnimChannelPoolAgg,TupperwareAggregate *pAnimSetAgg);
   void AddRotationCurvesConvertFromXYZ(HMotionChannel *pMotionChannel,TupperwareAggregate *pAnimChannelPoolAgg,TupperwareAggregate *pAnimSetAgg);
   void AddAnimCurve(const char *attributename,HFloatMotionCurve *pFloatMotionCurve,TupperwareAggregate *pAnimChannelPoolAgg,float fMultiply=1.0f);

   BOOL IsChannelAnimated(const char *attributename,TupperwareAggregate *pAnimChannelPoolAgg);

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // EXPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasExporter() { return TRUE; }
   virtual void OnExport( HMotion *pMotion, const char *szFileName);

   void AddMotion(HMotion *pMotion,TupperwareAggregate *pAnimSetPoolAgg);
   void AddMotionChannel(HMotionChannel *pMotionChannel,TupperwareAggregate *pAnimSetPoolAgg);
   void AddMotionCurve(const char *attributename,HFloatMotionCurve *pFloatMotionCurve,TupAnimCurve::CurveFlags nCurveFlags,TupperwareAggregate *pAnimChannelPoolAgg);

};

class MSetTranslator : public HMotionSetTranslatorPluginHelper
{
public:
   DECLARE_MOTIONSETTRANSLATOR_PLUGINHELPER( MSetTranslator, HMotionSetTranslatorPluginHelper )

   BOOL                 m_bBinary;

public:
   MSetTranslator();

   // HMotionTranslatorPluginHelper overrides
   virtual const char *GetFilterName() { return "MSet File"; }
   virtual const char *GetFilterExtension() { return "mset"; }

   // HPluginHelper overrides
   virtual const char *GetTitleName() const { return "MSet File"; }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // TUP IMPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasImporter() { return TRUE; }
   virtual void OnImport( HMotionSet *pMotionSet, const char *szFileName);

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // EXPORTER
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   virtual BOOL HasExporter() { return TRUE; }
   virtual void OnExport( HMotionSet *pMotionSet, const char *szFileName);
};

extern const char *g_pExporterDate;
extern const char *g_pExporterTime;
