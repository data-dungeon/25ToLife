////////////////////////////////////////////////////////////////////////////
//
// TupAnimExport
//
// Tupperware animation importer class
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUP_ANIM_IMPORT
#define TUP_ANIM_IMPORT

class JointCurve
{
public:
   MObject m_attribute;
   MObject m_curveobject;
   MTime m_starttime;
   MTime m_stoptime;
};

class JointCurves
{
public:
   JointCurves() { m_object = MObject::kNullObj; }
   MObject m_object;
   TupArray<JointCurve> m_curves;

   MTime GetStartTime()
   {
      MTime mintime = m_curves[0].m_starttime;
      for (int i = 1; i < m_curves.GetSize(); i++)
      {
         if (m_curves[i].m_starttime < mintime)
            mintime = m_curves[i].m_starttime;
      }
      return mintime;
   }
   MTime GetStopTime()
   {
      MTime maxtime = m_curves[m_curves.GetSize()-1].m_stoptime;
      for (int i = 1; i < m_curves.GetSize(); i++)
      {
         if (m_curves[i].m_stoptime > maxtime)
            maxtime = m_curves[i].m_stoptime;
      }
      return maxtime;
   }
};

class TupAnimImport
{
   TupperwareAggregate *m_pSceneAgg;
   TupScene *m_pTupScene;

public:
   TupAnimImport();

   MStatus ImportAnimation(const TCHAR *inputFilename,bool bImportAsClip);

private:
   void BuildAnimationData(const MDagPath &rootDagPath,TupperwareAggregate *pAnimSetPoolAgg,TupArray<JointCurves> &jointCurveArray,bool bHookUpCurves=TRUE);
   void BuildJointCurves(const MDagPath &rootDagPath,TupperwareAggregate *pAnimSetAgg,JointCurves *pJointCurves);
   MObject BuildAnimCurve(MDagPath dagPath,MObject attributeobject,TupperwareAggregate *pAnimCurveAgg,bool bHookUpCurves=TRUE);

   void HookUpAnimCurvesToSourceClip(MObject characterobject,MObject sourceclipobject,TupArray<JointCurves> &jointCurveArray);
   MStatus HookUpCharacter(
      MObject object,
      MObject &characterobject,
      MString sourceclipname,
      MObject &sourceclipobject,
      TupArray<JointCurves> &jointCurveArray);
};

#endif