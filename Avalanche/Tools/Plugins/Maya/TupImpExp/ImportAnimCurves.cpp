///////////////////////////////////////////////////////////////////////////
//
// ImportAnimCurves
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupAnimImport.h"

MObject TupAnimImport::BuildAnimCurve(MDagPath dagPath,MObject attributeobject,TupperwareAggregate *pAnimCurveAgg,bool bHookUpCurves)
{
   MStatus status;

   MPlug attributeplug(dagPath.node(),attributeobject);
   MString attributename = attributeplug.name();
   const char *debug = attributename.asChar();

   TupAnimCurve tupAnimCurve(pAnimCurveAgg);

   float *pOutputValues;
   int nNumOutputValues;
   tupAnimCurve.GetOutputValues(&pOutputValues,nNumOutputValues);// value

   float *pInputValues;
   int nNumInputValues;
   tupAnimCurve.GetInputValues(&pInputValues,nNumInputValues);// time

   int *pKeyFlags;
   int nNumKeyFlags;
   tupAnimCurve.GetKeyFlags(&pKeyFlags,nNumKeyFlags);

   float *pInXTangents;
   int nNumInXTangents;
   tupAnimCurve.GetInTangentX(&pInXTangents,nNumInXTangents);

   float *pInYTangents;
   int nNumInYTangents;
   tupAnimCurve.GetInTangentY(&pInYTangents,nNumInYTangents);

   float *pOutXTangents;
   int nNumOutXTangents;
   tupAnimCurve.GetOutTangentX(&pOutXTangents,nNumOutXTangents);

   float *pOutYTangents;
   int nNumOutYTangents;
   tupAnimCurve.GetOutTangentY(&pOutYTangents,nNumOutYTangents);

   int nFlags;
   tupAnimCurve.GetFlags(nFlags);

   MFnAnimCurve fnCurve;

   MObject curveobject;

   MFnAnimCurve::AnimCurveType animcurvetype = MFnAnimCurve::kAnimCurveTL;

   if ((nFlags & TupAnimCurve::FLAGS_INPUT_TYPE_MASK)==TupAnimCurve::FLAGS_INPUT_TYPE_TIME)
   {
      if ((nFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_MASK)==TupAnimCurve::FLAGS_OUTPUT_TYPE_LINEAR)
         animcurvetype = MFnAnimCurve::kAnimCurveTL;
      else if ((nFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_MASK)==TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR)
         animcurvetype = MFnAnimCurve::kAnimCurveTA;
      else if ((nFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_MASK)==TupAnimCurve::FLAGS_OUTPUT_TYPE_TIME)
         animcurvetype = MFnAnimCurve::kAnimCurveTT;
      else if ((nFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_MASK)==TupAnimCurve::FLAGS_OUTPUT_TYPE_UNITLESS)
         animcurvetype = MFnAnimCurve::kAnimCurveTU;
   }
   if ((nFlags & TupAnimCurve::FLAGS_INPUT_TYPE_MASK)==TupAnimCurve::FLAGS_INPUT_TYPE_UNITLESS)
   {  
      if ((nFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_MASK)==TupAnimCurve::FLAGS_OUTPUT_TYPE_LINEAR)
         animcurvetype = MFnAnimCurve::kAnimCurveUL;
      else if ((nFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_MASK)==TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR)
         animcurvetype = MFnAnimCurve::kAnimCurveUA;
      else if ((nFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_MASK)==TupAnimCurve::FLAGS_OUTPUT_TYPE_TIME)
         animcurvetype = MFnAnimCurve::kAnimCurveUT;
      else if ((nFlags & TupAnimCurve::FLAGS_OUTPUT_TYPE_MASK)==TupAnimCurve::FLAGS_OUTPUT_TYPE_UNITLESS)
         animcurvetype = MFnAnimCurve::kAnimCurveUU;
   }

   if (bHookUpCurves)
      curveobject = fnCurve.create(attributeplug,animcurvetype,NULL,&status);
   else
      curveobject = fnCurve.create(animcurvetype,NULL,&status);

   if (!status)
   {
      MGlobal::displayWarning("Couldn't create AnimCurve.\n");
      return MObject::kNullObj;
   }

   if (nFlags & TupAnimCurve::FLAGS_WEIGHTED_TANGENTS)
      fnCurve.setIsWeighted(true);
   else
      fnCurve.setIsWeighted(false);

   for (int i = 0; i < nNumOutputValues; i++)
   {
      MFnAnimCurve::TangentType intangenttype;

      int nKeyFlags = pKeyFlags[i];

      if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_SMOOTH)
         intangenttype = MFnAnimCurve::kTangentSmooth;     
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FIXED)
         intangenttype = MFnAnimCurve::kTangentFixed;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FLAT)
         intangenttype = MFnAnimCurve::kTangentFlat;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_LINEAR)
         intangenttype = MFnAnimCurve::kTangentLinear;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_STEP)
         intangenttype = MFnAnimCurve::kTangentStep;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_CLAMPED)
         intangenttype = MFnAnimCurve::kTangentClamped;
//      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_SLOW)
//         intangenttype = MFnAnimCurve::kTangentSlow;
 //     else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_IN_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FAST)
 //        intangenttype = MFnAnimCurve::kTangentFast;

      MFnAnimCurve::TangentType outtangenttype = MFnAnimCurve::kTangentGlobal;

      if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_SMOOTH)
         outtangenttype = MFnAnimCurve::kTangentSmooth;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FIXED)
         outtangenttype = MFnAnimCurve::kTangentFixed;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FLAT)
         outtangenttype = MFnAnimCurve::kTangentFlat;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_LINEAR)
         outtangenttype = MFnAnimCurve::kTangentLinear;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_STEP)
         outtangenttype = MFnAnimCurve::kTangentStep;
      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_CLAMPED)
         outtangenttype = MFnAnimCurve::kTangentClamped;
//      else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_SLOW)
//         outtangenttype = MFnAnimCurve::kTangentSlow;
 //     else if ((pKeyFlags[i] & TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_MASK)==TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FAST)
 //        outtangenttype = MFnAnimCurve::kTangentFast;
 
      switch (animcurvetype)
      {
      case MFnAnimCurve::kAnimCurveTL:
         {
            MTime mtime(pInputValues[i],MTime::kSeconds);
            fnCurve.addKey(mtime,pOutputValues[i]*100.0f,intangenttype,outtangenttype,NULL,&status);
         }
         break;
      case MFnAnimCurve::kAnimCurveTA:
         {
            MTime mtime(pInputValues[i],MTime::kSeconds);
            fnCurve.addKey(mtime,pOutputValues[i],intangenttype,outtangenttype,NULL,&status);
         }
         break;
      case MFnAnimCurve::kAnimCurveTU:
         {
            MTime mtime(pInputValues[i],MTime::kSeconds);
            fnCurve.addKey(mtime,pOutputValues[i],intangenttype,outtangenttype,NULL,&status);
         }
         break;
      case MFnAnimCurve::kAnimCurveTT:
         {
            MTime mtime1(pInputValues[i],MTime::kSeconds);
            MTime mtime2(pOutputValues[i],MTime::kSeconds);
            fnCurve.addKey(mtime1,mtime2,intangenttype,outtangenttype,NULL,&status);
          }
         break;
      case MFnAnimCurve::kAnimCurveUL:
         {
            fnCurve.addKey(pInputValues[i],pOutputValues[i]*100.0f,intangenttype,outtangenttype,NULL,&status);
         }
         break;
      case MFnAnimCurve::kAnimCurveUA:
         {
            fnCurve.addKey(pInputValues[i],pOutputValues[i],intangenttype,outtangenttype,NULL,&status);
         }
         break;
      case MFnAnimCurve::kAnimCurveUU:
         {
            fnCurve.addKey(pInputValues[i],pOutputValues[i],intangenttype,outtangenttype,NULL,&status);
         }
         break;
      case MFnAnimCurve::kAnimCurveUT:
         {
            MTime mtime(pOutputValues[i],MTime::kSeconds);
            fnCurve.addKey(pInputValues[i],mtime,intangenttype,outtangenttype,NULL,&status);
         }
         break;
      }

      fnCurve.setTangentsLocked(i,false);
      fnCurve.setWeightsLocked(i,false);


      MAngle angle(Math::ArcTan2(pInYTangents[i],pInXTangents[i]),MAngle::kRadians);
      double weight = pInXTangents[i]/(3.0f * Math::Cos((float)angle.asRadians()));

      if (nKeyFlags&TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FIXED)
//         fnCurve.setTangent(i,pInXTangents[i],pInYTangents[i],true);
         fnCurve.setTangent(i,angle,weight,true);

      angle = MAngle(Math::ArcTan2(pOutYTangents[i],pOutXTangents[i]),MAngle::kRadians);
      weight = pOutXTangents[i]/(3.0f * Math::Cos((float)angle.asRadians()));

      if (nKeyFlags&TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FIXED)
//         fnCurve.setTangent(i,pOutXTangents[i],pOutYTangents[i],false);
         fnCurve.setTangent(i,angle,weight,false);

      fnCurve.setWeightsLocked(i,true);
      fnCurve.setTangentsLocked(i,true);

      if (!status)
         MGlobal::displayWarning("Couldn't add key.\n");
   }

   return curveobject;
}