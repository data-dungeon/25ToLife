///////////////////////////////////////////////////////////////////////////
//
// ExportAnimCurves
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupAnimExport.h"

void TupAnimExport::AddAnimCurveForDepNode(TupperwareAggregate *pChannelsAgg,const MFnDependencyNode &depNode)
{
	MObject node = depNode.object();

	TupAnimChannelPool animChannelPool(pChannelsAgg);

	if (MAnimUtil::isAnimated(node))
	{
		MPlugArray animatedPlugs;
		MAnimUtil::findAnimatedPlugs (node, animatedPlugs);
		int numPlugs = (int)animatedPlugs.length();
		
		// loop through the plugs
		for (int plugIndex = 0; plugIndex < numPlugs; plugIndex++) 
		{
			MPlug plug = animatedPlugs[plugIndex];
			MObjectArray animation;
			
			// Find the animation curve(s) that animate this plug
			if (!MAnimUtil::findAnimation (plug, animation)) 
			{
				continue;
			}
			
			TupAnimChannel *pTupAnimChannel = NULL;
			TupAnimCurvePool *pTupAnimCurvePool = NULL;
			
			int numCurves = (int)animation.length();
			for (int curveIndex = 0; curveIndex < numCurves; curveIndex++) 
			{
				MObject animCurveNode = animation[curveIndex];

				MStatus status;
				if (!animCurveNode.hasFn (MFn::kAnimCurve)) 
					continue;

				MFnAnimCurve animCurve (animCurveNode);
				unsigned int numKeys = animCurve.numKeys();
				if (numKeys == 0) 
					continue;
				
				if (!pTupAnimChannel)
				{
					int channelIndex = animChannelPool.AddAnimChannel();
					TupperwareAggregate *pChannelAgg = animChannelPool.GetAnimChannel(channelIndex);
					pTupAnimChannel = new TupAnimChannel(pChannelAgg);
					
					MPlug attrPlug (plug);
					MObject attrObj = attrPlug.attribute();
					MFnAttribute fnAttr (attrObj);
					MString fullAttrName (fnAttr.name());
					attrPlug = attrPlug.parent();
					while (!attrPlug.isNull()) 
					{
						attrObj = attrPlug.attribute();
						MFnAttribute fnAttr (attrObj);
						fullAttrName = fnAttr.name() + "." + fullAttrName;
						attrPlug = attrPlug.parent();
					}
					
					pTupAnimChannel->AddName(depNode.name().asChar());
					pTupAnimChannel->AddAttribute(fullAttrName.asChar());
					pTupAnimChannel->AddReference(-1);//this shouldn't be used
					pTupAnimChannel->SetAnimChannelType(TupAnimChannel::NODE);
					pTupAnimChannel->AddFlags(0);
					TupperwareAggregate *pTupAnimCurvesAgg = pTupAnimChannel->AddCurves();
					pTupAnimCurvePool = new TupAnimCurvePool(pTupAnimCurvesAgg);
				}
				int curvePoolIndex = pTupAnimCurvePool->AddAnimCurve();
				TupperwareAggregate *pCurveAgg = pTupAnimCurvePool->GetAnimCurve(curvePoolIndex);
				AddAnimCurve(pCurveAgg,&animCurveNode);
			}
		}
	}
}

bool TupAnimExport::AddAnimCurve(TupperwareAggregate *pCurveAgg,const MObject *pAnimCurveObj,bool bFirstKeyOnly)
{
	if (pAnimCurveObj == NULL || pAnimCurveObj->isNull())
		return false; // problem with curve

	MStatus status = MS::kSuccess;
	MFnAnimCurve animCurve(*pAnimCurveObj, &status);
	if (MS::kSuccess != status) 
	{
		return false;
	}

	TupAnimCurve tupAnimCurve(pCurveAgg);

	int flags = 0;

	// input and output type
	switch (animCurve.animCurveType())
	{
		case MFnAnimCurve::kAnimCurveTL:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_TIME|TupAnimCurve::FLAGS_OUTPUT_TYPE_LINEAR;
			break;
		case MFnAnimCurve::kAnimCurveUL:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_UNITLESS|TupAnimCurve::FLAGS_OUTPUT_TYPE_LINEAR;
			break;
		case MFnAnimCurve::kAnimCurveTA:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_TIME|TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR;
			break;
		case MFnAnimCurve::kAnimCurveUA:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_UNITLESS|TupAnimCurve::FLAGS_OUTPUT_TYPE_ANGULAR;
			break;
		case MFnAnimCurve::kAnimCurveTT:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_TIME|TupAnimCurve::FLAGS_OUTPUT_TYPE_TIME;
			break;
		case MFnAnimCurve::kAnimCurveUT:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_UNITLESS|TupAnimCurve::FLAGS_OUTPUT_TYPE_TIME;
			break;
		case MFnAnimCurve::kAnimCurveTU:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_TIME|TupAnimCurve::FLAGS_OUTPUT_TYPE_UNITLESS;
			break;
		case MFnAnimCurve::kAnimCurveUU:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_UNITLESS|TupAnimCurve::FLAGS_OUTPUT_TYPE_UNITLESS;
			break;
		case MFnAnimCurve::kAnimCurveUnknown:
		default:
			flags |= TupAnimCurve::FLAGS_INPUT_TYPE_UNKNOWN|TupAnimCurve::FLAGS_OUTPUT_TYPE_UNKNOWN;
			break;
	}

	if (animCurve.isWeighted())
		flags |= TupAnimCurve::FLAGS_WEIGHTED_TANGENTS;

	MFnDependencyNode curveNode(*pAnimCurveObj);
	MPlug rotPlug = curveNode.findPlug(MString("roti"),&status);
	// use independent euler as default
	if (status==MS::kSuccess)
	{
		int rotValue;
		status = rotPlug.getValue(rotValue);
		if (status==MS::kSuccess)
		{
			switch (rotValue)
			{
				case 0:
				case 1:
					flags |= TupAnimCurve::FLAGS_ROTATION_INDEPENDENT_EULER;
					break;
				case 2:
					flags |= TupAnimCurve::FLAGS_ROTATION_SYNCRONIZED_EULER;
					break;
				case 3:
					flags |= TupAnimCurve::FLAGS_ROTATION_SYNCRONIZED_QUATERNION;
					break;
			}
		}
	}

	switch (animCurve.preInfinityType())
	{
		case MFnAnimCurve::kConstant:
			flags |= TupAnimCurve::FLAGS_PRE_INFINITY_CONSTANT;
			break;
		case MFnAnimCurve::kLinear:
			flags |= TupAnimCurve::FLAGS_PRE_INFINITY_LINEAR;
			break;
		case MFnAnimCurve::kCycle:
			flags |= TupAnimCurve::FLAGS_PRE_INFINITY_CYCLE;
			break;
		case MFnAnimCurve::kCycleRelative:
			flags |= TupAnimCurve::FLAGS_PRE_INFINITY_CYCLE_RELATIVE;
			break;
		case MFnAnimCurve::kOscillate:
			flags |= TupAnimCurve::FLAGS_PRE_INFINITY_OSCILLATE;
			break;
	}

	switch (animCurve.postInfinityType())
	{
		case MFnAnimCurve::kConstant:
			flags |= TupAnimCurve::FLAGS_POST_INFINITY_CONSTANT;
			break;
		case MFnAnimCurve::kLinear:
			flags |= TupAnimCurve::FLAGS_POST_INFINITY_LINEAR;
			break;
		case MFnAnimCurve::kCycle:
			flags |= TupAnimCurve::FLAGS_POST_INFINITY_CYCLE;
			break;
		case MFnAnimCurve::kCycleRelative:
			flags |= TupAnimCurve::FLAGS_POST_INFINITY_CYCLE_RELATIVE;
			break;
		case MFnAnimCurve::kOscillate:
			flags |= TupAnimCurve::FLAGS_POST_INFINITY_OSCILLATE;
			break;
	}

	tupAnimCurve.AddFlags(flags);

	unsigned numKeys = animCurve.numKeyframes();
	if (numKeys && bFirstKeyOnly)
		numKeys = 1;

	TupArray<float> inputValues; inputValues.SetSize(numKeys);
	TupArray<float> outputValues; outputValues.SetSize(numKeys);
	TupArray<int> keyFlags; keyFlags.SetSize(numKeys);

   TupArray<float> inTangentX; inTangentX.SetSize(numKeys);
   TupArray<float> inTangentY; inTangentY.SetSize(numKeys);
   TupArray<float> outTangentX; outTangentX.SetSize(numKeys);
   TupArray<float> outTangentY; outTangentY.SetSize(numKeys);

	for (unsigned keyIndex=0;keyIndex<numKeys;keyIndex++)
	{
      switch (animCurve.animCurveType())
      {
      case MFnAnimCurve::kAnimCurveTL:
         {
            inputValues[keyIndex] = (float)animCurve.time(keyIndex).as(MTime::kSeconds);// convert to seconds
            outputValues[keyIndex] = (float)animCurve.value(keyIndex)*0.01f;// convert to meters
         }
         break;
      case MFnAnimCurve::kAnimCurveTA:
         {
            inputValues[keyIndex] = (float)animCurve.time(keyIndex).as(MTime::kSeconds);// convert to seconds
            outputValues[keyIndex] = (float)(animCurve.value(keyIndex));// already in radians
         }
         break;
      case MFnAnimCurve::kAnimCurveTU:
         {
            inputValues[keyIndex] = (float)animCurve.time(keyIndex).as(MTime::kSeconds);// convert to seconds
            outputValues[keyIndex] = (float)animCurve.value(keyIndex);
         }
         break;
      case MFnAnimCurve::kAnimCurveTT:
         {
            inputValues[keyIndex] = (float)animCurve.time(keyIndex).as(MTime::kSeconds);// convert to seconds
            outputValues[keyIndex] = (float)MTime(animCurve.value(keyIndex)).as(MTime::kSeconds);// convert to seconds
         }
         break;
      case MFnAnimCurve::kAnimCurveUL:
         {
            inputValues[keyIndex] = (float)animCurve.unitlessInput(keyIndex);
            outputValues[keyIndex] = (float)(animCurve.value(keyIndex) * 0.01f);
         }
         break;
      case MFnAnimCurve::kAnimCurveUA:
         {
            inputValues[keyIndex] = (float)animCurve.unitlessInput(keyIndex);
            outputValues[keyIndex] = (float)(animCurve.value(keyIndex));// already in radians
         }
         break;
      case MFnAnimCurve::kAnimCurveUU:
         {
            inputValues[keyIndex] = (float)animCurve.unitlessInput(keyIndex);
            outputValues[keyIndex] = (float)animCurve.value(keyIndex);
         }
         break;
      case MFnAnimCurve::kAnimCurveUT:
         {
            inputValues[keyIndex] = (float)animCurve.unitlessInput(keyIndex);
            outputValues[keyIndex] = (float)MTime(animCurve.value(keyIndex)).as(MTime::kSeconds);// convert to seconds
         }
         break;
      }

		int keyFlag = 0;

		switch (animCurve.inTangentType(keyIndex))
		{
         case MFnAnimCurve::kTangentSmooth:
            keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_SMOOTH;
            break;
			case MFnAnimCurve::kTangentFixed:
				keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FIXED;
				break;
			case MFnAnimCurve::kTangentLinear:
				keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_LINEAR;
				break;
			case MFnAnimCurve::kTangentFlat:
				keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FLAT;
				break;
         case MFnAnimCurve::kTangentStep:
            keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_STEP;
            break;
//			case MFnAnimCurve::kTangentGlobal:
//				keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_GLOBAL;
//				break;

//			case MFnAnimCurve::kTangentSlow:
//				keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_SLOW;
//				break;
//			case MFnAnimCurve::kTangentFast:
//				keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_FAST;
//				break;
			case MFnAnimCurve::kTangentClamped:
			default:
				keyFlag |= TupAnimCurve::FLAGS_IN_TANGENT_TYPE_CLAMPED;
				break;
		}

		switch (animCurve.outTangentType(keyIndex))
		{
         case MFnAnimCurve::kTangentSmooth:
            keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_SMOOTH;
            break;
			case MFnAnimCurve::kTangentFixed:
				keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FIXED;
				break;
			case MFnAnimCurve::kTangentLinear:
				keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_LINEAR;
				break;
			case MFnAnimCurve::kTangentFlat:
				keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FLAT;
				break;
			case MFnAnimCurve::kTangentStep:
				keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_STEP;
				break;
//			case MFnAnimCurve::kTangentGlobal:
//				keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_GLOBAL;
//				break;
//			case MFnAnimCurve::kTangentSlow:
//				keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_SLOW;
//				break;
//			case MFnAnimCurve::kTangentFast:
//				keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_FAST;
//				break;
			case MFnAnimCurve::kTangentClamped:
			default:
				keyFlag |= TupAnimCurve::FLAGS_OUT_TANGENT_TYPE_CLAMPED;
				break;
		}

		if (animCurve.tangentsLocked(keyIndex))
			keyFlag |= TupAnimCurve::FLAGS_TANGENTS_LOCKED;

		if (animCurve.weightsLocked(keyIndex))
			keyFlag |= TupAnimCurve::FLAGS_WEIGHTS_LOCKED;

		if (animCurve.isBreakdown(keyIndex))
			keyFlag |= TupAnimCurve::FLAGS_IS_BREAKDOWN;

		keyFlags[keyIndex] = keyFlag;

      // Maya stores data internally as centimeters, radians, and seconds

      // this keeps handing me bogus data, BEWARE!
//      animCurve.getTangent(keyIndex,inTangentX[keyIndex],inTangentY[keyIndex],true);
//      animCurve.getTangent(keyIndex,outTangentX[keyIndex],outTangentY[keyIndex],false);

      MAngle angle;
      double weight;

      animCurve.getTangent(keyIndex,angle,weight,true);
      inTangentX[keyIndex] = 3.0f * (float)weight * Math::Cos((float)angle.asRadians());
      inTangentY[keyIndex] = 3.0f * (float)weight * Math::Sin((float)angle.asRadians());
      animCurve.getTangent(keyIndex,angle,weight,false);
      outTangentX[keyIndex] = 3.0f * (float)weight * Math::Cos((float)angle.asRadians());
      outTangentY[keyIndex] = 3.0f * (float)weight * Math::Sin((float)angle.asRadians());
	}

	if (numKeys)
	{
		float startTime = inputValues[0];
		float endTime = inputValues[numKeys-1];
		tupAnimCurve.AddStartTime(startTime);
		tupAnimCurve.AddEndTime(endTime);

		tupAnimCurve.AddInputValues(inputValues.GetData(),numKeys);
		tupAnimCurve.AddOutputValues(outputValues.GetData(),numKeys);
		tupAnimCurve.AddKeyFlags(keyFlags.GetData(),numKeys);

      tupAnimCurve.AddInTangentX(inTangentX.GetData(),numKeys);
      tupAnimCurve.AddInTangentY(inTangentY.GetData(),numKeys);
      tupAnimCurve.AddOutTangentX(outTangentX.GetData(),numKeys);
      tupAnimCurve.AddOutTangentY(outTangentY.GetData(),numKeys);
	}
	return true;
}

