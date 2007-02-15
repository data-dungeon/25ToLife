////////////////////////////////////////////////////////////////////////////
//
// TupAnimExport
//
// Tupperware animation exporter class
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUP_ANIM_EXPORT
#define TUP_ANIM_EXPORT

class TupAnimExport
{
private:
   bool m_bBinaryOutput;
   bool m_bExportSelected;

   TupperwareAggregate *m_pSceneAgg;
   TupScene *m_pTupScene;
	TupAnimSetPool *m_pAnimSetPool;

   Keeper<MDagPath> *m_pExportDagPaths; // keep track of all the dag paths that will be exported (when exporting selections)

public:
   TupAnimExport(bool bExportSelected,Keeper<MDagPath> *pExportDagPaths);
   TupAnimExport(bool bBinaryOutput,bool bExportSelected);

   bool ValidNode(MDagPath &mDagPath);

   MStatus ExportAnimation(const TCHAR *outputFilename);

	// ExportAnimation
   void AddAnimation(const MDagPath &rootDagPath,TupAnimSetPool *pAnimSetPool,Keeper<MDagPath> *pNodePoolDagPaths);
   void AddAnimationNode(const MDagPath &nodeDagPath,TupAnimSetPool *pAnimSetPool);
   static bool IsAnimated(const MDagPath &nodeDagPath);
   static void AddGlobalAnimationSetData(const MDagPath &nodeDagPath,TupAnimChannelPool *pAnimChannelPool);
   static void AddCharacterSetAnimationData(TupAnimSetPool *pAnimSetPool,Keeper<MDagPath> *pNodePoolDagPaths);
   static void AddAnimationGlobals(TupScene *pTupScene);
   static bool TestAnimCurveNode(MObject &animCurveNode);
   static bool AddClipAnimationAsGlobal(MObject clipobject,TupAnimSetPool *pAnimSetPool);
   static int AddAnimSet(const MPlug &attributePlug,TupAnimSetPool *pAnimSetPool);
   static void AddAnimChannel(const MPlug &attributePlug,MObject sourcecurve,TupperwareAggregate *pAnimSetAgg);

   // ExportAnimCurves
   static void AddAnimCurveForDepNode(TupperwareAggregate *pChannelsAgg,const MFnDependencyNode &depNode);
   static bool AddAnimCurve(TupperwareAggregate *pCurveAgg,const MObject *pAnimCurveObj,bool bFirstKeyOnly=false);

private:
   static void AddReusableActionInfo(TupperwareAggregate *pAnimChannelAgg,MDagPath dagPath);
   static void AddLocalToParentBindMatrix(TupperwareAggregate *pAnimSetAgg,MDagPath dagPath);
   static void AddJointOrientationQuaternion(TupperwareAggregate *pAnimSetAgg,MDagPath dagPath);
};

#endif