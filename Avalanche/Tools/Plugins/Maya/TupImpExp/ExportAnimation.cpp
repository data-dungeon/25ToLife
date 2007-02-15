///////////////////////////////////////////////////////////////////////////
//
// ExportAnimation
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupExport.h"
#include "TupAnimExport.h"
#include "IntHash.h"

void TupAnimExport::AddAnimation(const MDagPath &rootDagPath,TupAnimSetPool *pAnimSetPool,Keeper<MDagPath> *pNodePoolDagPaths)
{
   AddAnimationNode(rootDagPath,pAnimSetPool);
	AddCharacterSetAnimationData(pAnimSetPool,pNodePoolDagPaths);
}

bool TupAnimExport::AddClipAnimationAsGlobal(MObject clipobject,TupAnimSetPool *pAnimSetPool)
{
   MStatus status;

   MFnClip fnClip(clipobject);
   MObjectArray sourcecurves;
   MPlugArray associatedAttrs;
   fnClip.getMemberAnimCurves(sourcecurves,associatedAttrs);
   int nIndex;
   IntHash bonemap;
   int nNumSourceClips = sourcecurves.length();
   for (int nSourceClipIndex = 0; nSourceClipIndex < nNumSourceClips; nSourceClipIndex++)
   {
      MObject object = associatedAttrs[nSourceClipIndex].node();
      MFnDependencyNode depNode(object);
      if (!bonemap.FindKey((int)MTools::GetMPtrBase(depNode),nIndex))
      {
         nIndex = AddAnimSet(associatedAttrs[nSourceClipIndex],pAnimSetPool);
         bonemap.AddKey((int)MTools::GetMPtrBase(depNode),nIndex);
      }
      TupperwareAggregate *pAnimSetAgg = pAnimSetPool->GetAnimSet(nIndex);
      TupAnimSet tupAnimSet(pAnimSetAgg);

      AddAnimChannel(associatedAttrs[nSourceClipIndex],sourcecurves[nSourceClipIndex],pAnimSetAgg);
   }

   return true;
}

int TupAnimExport::AddAnimSet(const MPlug &attributePlug,TupAnimSetPool *pAnimSetPool)
{
   MString attributeplugname = attributePlug.name();
   const char *debug = attributeplugname.asChar();

   MDagPath nodeDagPath;
   MFnDependencyNode depNode(attributePlug.node());
   MFnDagNode dagNode(attributePlug.node());
   dagNode.getPath(nodeDagPath);

   int animSetIndex = pAnimSetPool->AddAnimSet();
   TupperwareAggregate *pAnimSetAgg = pAnimSetPool->GetAnimSet(animSetIndex);
   TupAnimSet tupAnimSet(pAnimSetAgg);
   tupAnimSet.SetAnimSetType(TupAnimSet::GLOBAL);

   tupAnimSet.AddName(nodeDagPath.fullPathName().asChar());

   AddReusableActionInfo(pAnimSetAgg,nodeDagPath);

   AddLocalToParentBindMatrix(pAnimSetAgg,nodeDagPath);

   AddJointOrientationQuaternion(pAnimSetAgg,nodeDagPath);

   TupperwareAggregate *pAnimChannelPoolAgg = tupAnimSet.AddChannels();
   
   // Maya always uses Euler rotation
   int nAnimSetFlags=0;
   nAnimSetFlags |= TupAnimSet::FLAGS_EULER_ROTATION;
   return animSetIndex;

}

void TupAnimExport::AddAnimChannel(const MPlug &attributePlug,MObject sourcecurve,TupperwareAggregate *pAnimSetAgg)
{
   TupAnimSet tupAnimSet(pAnimSetAgg);
   TupperwareAggregate *pAnimChannelPoolAgg = tupAnimSet.GetChannels();
   TupAnimChannelPool tupAnimChannelPool(pAnimChannelPoolAgg);
   int nChannelRef = tupAnimChannelPool.AddAnimChannel();
   TupperwareAggregate *pAnimChannelAgg = tupAnimChannelPool.GetAnimChannel(nChannelRef);
   TupAnimChannel tupAnimChannel(pAnimChannelAgg);

   MFnDependencyNode depNode(attributePlug.node());

   MString objectname = depNode.name();
   MString fullAttrName = attributePlug.name();
   tupAnimChannel.AddName(objectname.asChar());
   tupAnimChannel.AddAttribute(fullAttrName.asChar());
   tupAnimChannel.AddReference(-1);

   tupAnimChannel.SetAnimChannelType(TupAnimChannel::NODE);
   tupAnimChannel.AddFlags(0);

   TupperwareAggregate *pTupAnimCurvesAgg = tupAnimChannel.AddCurves();
   TupAnimCurvePool tupAnimCurvePool(pTupAnimCurvesAgg);

   int nAnimCurveIndex = tupAnimCurvePool.AddAnimCurve();
   TupperwareAggregate *pAnimCurveAgg = tupAnimCurvePool.GetAnimCurve(nAnimCurveIndex);

   AddAnimCurve(pAnimCurveAgg,&sourcecurve);
}

void TupAnimExport::AddAnimationNode(const MDagPath &nodeDagPath,TupAnimSetPool *pAnimSetPool)
{
   MString nodename = nodeDagPath.fullPathName();
   const char *debug = nodename.asChar();
   if (IsAnimated(nodeDagPath))
   {
      int animSetIndex = pAnimSetPool->AddAnimSet();
      TupperwareAggregate *pAnimSetAgg = pAnimSetPool->GetAnimSet(animSetIndex);
      TupAnimSet *pTupAnimSet = new TupAnimSet(pAnimSetAgg);
      pTupAnimSet->SetAnimSetType(TupAnimSet::GLOBAL);

      TupString bonename = nodeDagPath.fullPathName().asChar();
      TupExport::FixBoneName(bonename);
      pTupAnimSet->AddName(bonename);

      MString debugstr = nodeDagPath.fullPathName();
      const char *debug = debugstr.asChar();

      AddReusableActionInfo(pAnimSetAgg,nodeDagPath);

      AddLocalToParentBindMatrix(pAnimSetAgg,nodeDagPath);

      AddJointOrientationQuaternion(pAnimSetAgg,nodeDagPath);

      TupperwareAggregate *pAnimChannelPoolAgg = pTupAnimSet->AddChannels();
      TupAnimChannelPool *pAnimChannelPool = new TupAnimChannelPool(pAnimChannelPoolAgg);
      TupAnimChannelPool tupAnimChannelPool(pAnimChannelPoolAgg);
      AddGlobalAnimationSetData(nodeDagPath,pAnimChannelPool);
      delete pAnimChannelPool;
      pAnimChannelPool = NULL;
   }

   int numChildren = nodeDagPath.childCount();
   int childIndex;
   for (childIndex = 0; childIndex < numChildren; childIndex++)
   {
      MFnDagNode childNode(nodeDagPath.child(childIndex));
      MDagPath childPath;
      childNode.getPath(childPath);
      if (ValidNode(childPath) && TupExport::ValidNodeName(childNode.name().asChar()))
      {
         AddAnimationNode(childPath,pAnimSetPool);
      }
   }
}

bool TupAnimExport::IsAnimated(const MDagPath &nodeDagPath)
{
	if (MAnimUtil::isAnimated(nodeDagPath))
	{
		MPlugArray animatedPlugs;
		MAnimUtil::findAnimatedPlugs (nodeDagPath, animatedPlugs);
		int numPlugs = (int)animatedPlugs.length();
		
		// loop through the plugs
		for (int plugIndex = 0; plugIndex < numPlugs; plugIndex++) 
		{
			MPlug plug = animatedPlugs[plugIndex];
			MObject character;
			MFnCharacter fnCharacter;
			if (fnCharacter.getCharacterThatOwnsPlug(plug,character))
			{
				continue;
			}
			
			MObjectArray animation;
			
			// Find the animation curve(s) that animate this plug
			if (!MAnimUtil::findAnimation (plug, animation)) 
			{
				continue;
			}
			
			unsigned int numCurves = animation.length();
			for (unsigned int curveIndex = 0; curveIndex < numCurves; curveIndex++) 
			{
				MObject animCurveNode = animation[curveIndex];
				if (!animCurveNode.hasFn (MFn::kAnimCurve)) 
				{
					continue;
				}
				MFnAnimCurve animCurve (animCurveNode);
				unsigned int numKeys = animCurve.numKeys();
				if (numKeys == 0) 
				{
					continue;
				}

				return true;
			}
		}
	}

	// check the children (if a transform node)
	if (nodeDagPath.apiType() == MFn::kTransform)
	{
		int numChildren = nodeDagPath.childCount();
		int childIndex;

		for (childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MObject childObject = nodeDagPath.child(childIndex);
			MDagPath childPath = nodeDagPath;
			childPath.push(childObject);
			
         if (TupExport::ValidObject(childPath))
			{
				if (IsAnimated(childPath))
					return true;
			}
		}
	}
	return false;
}

void TupAnimExport::AddGlobalAnimationSetData(const MDagPath &nodeDagPath,TupAnimChannelPool *pAnimChannelPool)
{
	if (MAnimUtil::isAnimated(nodeDagPath))
	{
		MPlugArray animatedPlugs;
		MAnimUtil::findAnimatedPlugs (nodeDagPath, animatedPlugs);
		int numPlugs = (int)animatedPlugs.length();

		// loop through the plugs
		for (int plugIndex = 0; plugIndex < numPlugs; plugIndex++) 
		{
			MPlug plug = animatedPlugs[plugIndex];
			MObject character;
			MFnCharacter fnCharacter;
			if (fnCharacter.getCharacterThatOwnsPlug(plug,character))
				continue;

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
				if (!TestAnimCurveNode(animCurveNode))
					continue;

            MString debugstr;

				if (!pTupAnimChannel)
				{
					int channelIndex = pAnimChannelPool->AddAnimChannel();
					TupperwareAggregate *pAnimChannelAgg = pAnimChannelPool->GetAnimChannel(channelIndex);
					pTupAnimChannel = new TupAnimChannel(pAnimChannelAgg);

					MPlug attrPlug (plug);
					MObject attrObj = attrPlug.attribute();
					MFnAttribute fnAttr (attrObj);
					MString fullAttrName (fnAttr.name());
               debugstr = fnAttr.name();
					attrPlug = attrPlug.parent();
					while (!attrPlug.isNull()) 
					{
						attrObj = attrPlug.attribute();
						MFnAttribute fnAttr (attrObj);
						fullAttrName = fnAttr.name() + "." + fullAttrName;
						attrPlug = attrPlug.parent();
					}

					pTupAnimChannel->AddName(nodeDagPath.fullPathName().asChar());
					pTupAnimChannel->AddAttribute(fullAttrName.asChar());
					pTupAnimChannel->AddReference(-1);

					pTupAnimChannel->SetAnimChannelType(TupAnimChannel::NODE);
					pTupAnimChannel->AddFlags(0);

					TupperwareAggregate *pTupAnimCurvesAgg = pTupAnimChannel->AddCurves();
					pTupAnimCurvePool = new TupAnimCurvePool(pTupAnimCurvesAgg);
				}
            const char *debug = debugstr.asChar();

				int curvePoolIndex = pTupAnimCurvePool->AddAnimCurve();
				TupperwareAggregate *pCurveAgg = pTupAnimCurvePool->GetAnimCurve(curvePoolIndex);
				AddAnimCurve(pCurveAgg,&animCurveNode);
			}
			delete pTupAnimChannel;
			delete pTupAnimCurvePool;
		}
	}
	// check the children (if a transform node)
	if (nodeDagPath.apiType() == MFn::kTransform)
	{
		int numChildren = nodeDagPath.childCount();
		int childIndex;
		
		for (childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MObject childObject = nodeDagPath.child(childIndex);
			MDagPath childPath = nodeDagPath;
			childPath.push(childObject);
			
         if (TupExport::ValidObject(childPath))
			{
				AddGlobalAnimationSetData(childPath,pAnimChannelPool);
			}
		}
	}
}

static bool IsChildBone(MDagPath startPath)
{
//	return false;

	if (startPath.apiType()==MFn::kJoint)
	{
		startPath.pop();
		if (startPath.apiType()==MFn::kJoint)
		{
			return true;
		}
		return false;
	}
	return false;
}

void TupAnimExport::AddCharacterSetAnimationData(TupAnimSetPool *pAnimSetPool,Keeper<MDagPath> *pNodePoolDagPaths)
{
	MStatus status;
	MItDependencyNodes depIterator(MFn::kCharacter,&status);

	if (!status)
		return;
	
	for ( ; !depIterator.isDone(); depIterator.next() )
	{
		MDagPath dagPath;
		MObject  component = MObject::kNullObj;
		
		MFnCharacter mfnCharacter(depIterator.item());

		// set up the anim set
		int animSetIndex = pAnimSetPool->AddAnimSet();
		TupperwareAggregate *pAnimSetAgg = pAnimSetPool->GetAnimSet(animSetIndex);
		TupAnimSet tupAnimSet(pAnimSetAgg);
		tupAnimSet.SetAnimSetType(TupAnimSet::CHARACTER);
		tupAnimSet.AddName(mfnCharacter.name().asChar());
		TupperwareAggregate *pAnimChannelPoolAgg = tupAnimSet.AddChannels();
		TupAnimChannelPool characterAnimChannelPool(pAnimChannelPoolAgg);

		MPlugArray plugArray;
		status = mfnCharacter.getMemberPlugs(plugArray);

		Keeper<TupString> channelDagAttribs;

		if (!status)
			continue;

		int numPlugs = plugArray.length();

		if (numPlugs==0)
			continue;

		for (int plugIndex=0;plugIndex<numPlugs;plugIndex++)
		{
			// get the name of the attribute
			MPlug myPlug(plugArray[plugIndex]);
			MObject plugObj = myPlug.node();
			TupString plugApiType = plugObj.apiTypeStr();
			if (plugObj.hasFn(MFn::kDagNode))
			{
				MPlug attrPlug(myPlug);
				MObject plugAttr = myPlug.attribute();
				MFnAttribute fnAttr (plugAttr);
				MString fullAttrName (fnAttr.name());
				attrPlug = attrPlug.parent();
				while (!attrPlug.isNull()) 
				{
					plugAttr = attrPlug.attribute();
					MFnAttribute fnAttr (plugAttr);
					fullAttrName = fnAttr.name() + "." + fullAttrName;
					attrPlug = attrPlug.parent();
				}

				MFnDagNode mDagNode(plugObj);
				MDagPathArray paths;
				mDagNode.getAllPaths(paths);
				int numPaths = paths.length();
				for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
				{
					MDagPath myPath(paths[pathIndex]);
					while (!myPath.hasFn(MFn::kTransform))
					{
						myPath.pop(); // go up one
					}

					int channelIndex = characterAnimChannelPool.AddAnimChannel();
					TupperwareAggregate *pAnimChannelAgg = characterAnimChannelPool.GetAnimChannel(channelIndex);
					TupAnimChannel tupAnimChannel(pAnimChannelAgg);

					int nodeIndex;
               if (pNodePoolDagPaths)
                  nodeIndex = pNodePoolDagPaths->GetIndex(myPath);

					tupAnimChannel.AddName(myPath.fullPathName().asChar());
					tupAnimChannel.AddAttribute(fullAttrName.asChar());
					tupAnimChannel.AddReference(nodeIndex);// should it be used?!

					tupAnimChannel.SetAnimChannelType(TupAnimChannel::NODE);
					tupAnimChannel.AddFlags(0);
               
					TupString channelDagAttrib = myPath.fullPathName().asChar();
					channelDagAttrib += ":";
					channelDagAttrib += fullAttrName.asChar();
					channelDagAttribs.Add(channelDagAttrib); // add to keeper

					MObjectArray animation;

					if (!MAnimUtil::findAnimation (myPlug, animation)) 
					{
						continue;
					}
					
					TupAnimCurvePool *pTupAnimCurvePool = NULL;
					
					int numCurves = (int)animation.length();
					for (int curveIndex = 0; curveIndex < numCurves; curveIndex++) 
					{
						MObject animCurveNode = animation[curveIndex];
						if (!animCurveNode.hasFn (MFn::kAnimCurve)) 
						{
							continue;
						}
						MFnAnimCurve animCurve (animCurveNode);
						unsigned int numKeys = animCurve.numKeys();
						if (numKeys == 0) 
						{
							continue;
						}

						if (!pTupAnimCurvePool)
						{
							TupperwareAggregate *pTupAnimCurvesAgg = tupAnimChannel.AddCurves();
							pTupAnimCurvePool = new TupAnimCurvePool(pTupAnimCurvesAgg);
						}
						int curvePoolIndex = pTupAnimCurvePool->AddAnimCurve();
						TupperwareAggregate *pCurveAgg = pTupAnimCurvePool->GetAnimCurve(curvePoolIndex);
						AddAnimCurve(pCurveAgg,&animCurveNode);
					}
					delete pTupAnimCurvePool;
				}
			}
		}
		// do the source clips
		Keeper<int> uniqueSourceClipPtrs; // keep track of the dag ptr for instanced clip use
		int numSourceClips = mfnCharacter.getSourceClipCount();
		if (numSourceClips)
		{
			TupperwareAggregate *pSourceClipsAgg = tupAnimSet.AddSourceClips();
			TupAnimClipPool sourceClipPool(pSourceClipsAgg);

			for (int sourceClipIndex=0;sourceClipIndex<numSourceClips;sourceClipIndex++)
			{
				MObject sourceClipObj = mfnCharacter.getSourceClip(sourceClipIndex);
				MFnDependencyNode sourceClipDepNode(sourceClipObj);
				int previousSize = uniqueSourceClipPtrs.GetSize();
				int sourceClipPtr = (int)MayaHelper::GetMPtrBase(sourceClipDepNode);
				int sourceClipPtrIndex = uniqueSourceClipPtrs.Add(sourceClipPtr);
				// if we added this one (should always happen)
				if (sourceClipPtrIndex==previousSize)
				{
					int poolIndex = sourceClipPool.AddAnimClip();
					TupperwareAggregate *pSourceClipAgg = sourceClipPool.GetAnimClip(poolIndex);
					TupAnimSourceClip tupAnimSourceClip(pSourceClipAgg);
					MFnClip mfnClip(sourceClipObj);
					tupAnimSourceClip.AddName(mfnClip.name().asChar());
					tupAnimSourceClip.AddStartTime((float)mfnClip.getSourceStart().value());
					tupAnimSourceClip.AddDuration((float)mfnClip.getSourceDuration().value());
					int flags = 0;
					if (mfnClip.isPose())
						flags |= TupAnimSourceClip::FLAGS_POSE;
					tupAnimSourceClip.AddFlags(flags);

					// user properties
					TupString noteString;
					MPlug notesPlug = sourceClipDepNode.findPlug("notes",&status);
					if (status)
					{
						MString tempString;
						notesPlug.getValue(tempString);	
						noteString = tempString.asChar();
						noteString.Replace("\r\n","\n");
						noteString.Replace("\r","\n");
					}
					if (noteString.GetLength())
					{
						tupAnimSourceClip.AddUserProperties(noteString);
					}

					TupperwareAggregate *pSourceClipChannelsAgg = tupAnimSourceClip.AddChannels();
					TupAnimChannelPool tupSourceClipChannelPool(pSourceClipChannelsAgg);

					MObjectArray curves;
					MPlugArray assocAttribs;
					mfnClip.getMemberAnimCurves(curves,assocAttribs);

					int numPlugs = curves.length();

					for (int plugIndex=0;plugIndex<numPlugs;plugIndex++)
					{
						// get the name of the attribute
						MPlug myPlug(assocAttribs[plugIndex]);
						MObject plugObj = myPlug.node();
						TupString plugApiType = plugObj.apiTypeStr();
						if (plugObj.hasFn(MFn::kDagNode))
						{
							MPlug attrPlug(myPlug);
							MObject plugAttr = myPlug.attribute();
							MFnAttribute fnAttr (plugAttr);
							MString fullAttrName (fnAttr.name());
							attrPlug = attrPlug.parent();
							while (!attrPlug.isNull()) 
							{
								plugAttr = attrPlug.attribute();
								MFnAttribute fnAttr (plugAttr);
								fullAttrName = fnAttr.name() + "." + fullAttrName;
								attrPlug = attrPlug.parent();
							}
							
							MFnDagNode mDagNode(plugObj);
							MDagPathArray paths;
							mDagNode.getAllPaths(paths);
							int numPaths = paths.length();
							for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
							{
								MDagPath myPath(paths[pathIndex]);
								while (!myPath.hasFn(MFn::kTransform))
								{
									myPath.pop(); // go up one
								}
								bool bChildBone = IsChildBone(myPath);
								
								int channelIndex = tupSourceClipChannelPool.AddAnimChannel();
								TupperwareAggregate *pAnimChannelAgg = tupSourceClipChannelPool.GetAnimChannel(channelIndex);
								TupAnimChannel tupAnimChannel(pAnimChannelAgg);
								
                        int nodeIndex;
                        if (pNodePoolDagPaths)
                           nodeIndex = pNodePoolDagPaths->GetIndex(myPath);
								
								tupAnimChannel.AddName(myPath.fullPathName().asChar());
								tupAnimChannel.AddAttribute(fullAttrName.asChar());
								tupAnimChannel.AddReference(nodeIndex);// should it be used?!
								tupAnimChannel.SetAnimChannelType(TupAnimChannel::NODE);
								tupAnimChannel.AddFlags(0);

								TupString channelDagAttrib = myPath.fullPathName().asChar();
								channelDagAttrib += ":";
								channelDagAttrib += fullAttrName.asChar();
								int characterIndex = channelDagAttribs.GetIndex(channelDagAttrib); // get index from keeper
								tupAnimChannel.AddCharacterIndex(characterIndex);

								MObject animCurveNode = curves[plugIndex];
								if (animCurveNode.hasFn (MFn::kAnimCurve)) 
								{
									MFnAnimCurve animCurve (animCurveNode);
									unsigned int numKeys = animCurve.numKeys();
									if (numKeys != 0) 
									{
										TupperwareAggregate *pTupAnimCurvesAgg = tupAnimChannel.AddCurves();
										TupAnimCurvePool tupAnimCurvePool(pTupAnimCurvesAgg);

										TupperwareAggregate *pCurveAgg = tupAnimCurvePool.AddAnimCurve(0);
                              AddAnimCurve(pCurveAgg,&animCurveNode);
									}
								}
							}
						}
					}
				}
			}
		}

		// do the instanced clips
		Keeper<int> uniqueInstancedClipPtrs; // keep track of the dag ptr for clip blends
		int numInstancedClips = mfnCharacter.getScheduledClipCount();
		if (numInstancedClips)
		{
			TupperwareAggregate *pInstancedClipsAgg = tupAnimSet.AddInstancedClips();
			TupAnimClipPool instancedClipPool(pInstancedClipsAgg);

			for (int instancedClipIndex = 0;instancedClipIndex<numInstancedClips;instancedClipIndex++)
			{
				MObject instanceClipObj = mfnCharacter.getScheduledClip(instancedClipIndex);
				MFnDependencyNode instancedClipDepNode(instanceClipObj);

				int previousSize = uniqueInstancedClipPtrs.GetSize();
				int instancedClipPtr = (int)MayaHelper::GetMPtrBase(instancedClipDepNode);
				int instancedClipPtrIndex = uniqueInstancedClipPtrs.Add(instancedClipPtr);

				// if we added this one (should always happen)
				if (instancedClipPtrIndex==previousSize)
				{
					int poolIndex = instancedClipPool.AddAnimClip();
					TupperwareAggregate *pInstancedClipAgg = instancedClipPool.GetAnimClip(poolIndex);
					TupAnimInstancedClip tupAnimInstancedClip(pInstancedClipAgg);

					MFnClip mfnClip(instanceClipObj);

					tupAnimInstancedClip.AddName(mfnClip.name().asChar());
					// source clip reference
					MObject sourceClipObj = mfnClip.sourceClip();
					MFnDependencyNode sourceClipNode(sourceClipObj);
					int sourceClipPtr = (int) MayaHelper::GetMPtrBase(sourceClipNode);
					int sourceClipIndex = uniqueSourceClipPtrs.GetIndex(sourceClipPtr);
					tupAnimInstancedClip.AddSourceClipReference(sourceClipIndex);

					tupAnimInstancedClip.AddStartTime((float)mfnClip.getStartFrame().value());
					tupAnimInstancedClip.AddCycle((float)mfnClip.getCycle());
					tupAnimInstancedClip.AddWeight((float)mfnClip.getWeight());
					tupAnimInstancedClip.AddScale((float)mfnClip.getScale());
					TupperwareAggregate *pAnimatedChannelsAgg = tupAnimInstancedClip.AddAnimagedChannels();
					AddAnimCurveForDepNode(pAnimatedChannelsAgg,instancedClipDepNode);

					// user properties
					TupString noteString;
					MPlug notesPlug = mfnClip.findPlug("notes",&status);
					if (status)
					{
						MString tempString;
						notesPlug.getValue(tempString);	
						noteString = tempString.asChar();
						noteString.Replace("\r\n","\n");
						noteString.Replace("\r","\n");
					}
					if (noteString.GetLength())
					{
						tupAnimInstancedClip.AddUserProperties(noteString);
					}
					int flags = 0;
					if (mfnClip.getEnabled())
					{
						flags |= TupAnimInstancedClip::FLAGS_ENABLE;
					}
					if (mfnClip.getAbsolute())
					{
						flags |= TupAnimInstancedClip::FLAGS_ABSOLUTE;
					}
					tupAnimInstancedClip.AddFlags(flags);
				}
			}
		}
		// handle the clip blends
		int numBlends = mfnCharacter.getBlendCount();
		if (numBlends)
		{
			TupperwareAggregate *pClipBlendPoolAgg = tupAnimSet.AddClipBlends();
			TupAnimClipBlendPool	tupAnimClipBlendPool(pClipBlendPoolAgg);
			for (int blendIndex=0;blendIndex<numBlends;blendIndex++)
			{
				TupperwareAggregate *pAnimClipBlendAgg = tupAnimClipBlendPool.AddAnimClipBlend(blendIndex);
				TupAnimClipBlend tupAnimClipBlend(pAnimClipBlendAgg);

				MObject animBlendObj = mfnCharacter.getBlend(blendIndex);
				MFnDependencyNode animBlendNode(animBlendObj);

				tupAnimClipBlend.AddName(animBlendNode.name().asChar());

				// get refs for instanced clips
				MObject clip1,clip2;
				mfnCharacter.getBlendClips(blendIndex,clip1,clip2);
				MFnDependencyNode clip1Node(clip1);
				MFnDependencyNode clip2Node(clip2);
				int clip1NodePtr = (int)MayaHelper::GetMPtrBase(clip1Node);
				int clip2NodePtr = (int)MayaHelper::GetMPtrBase(clip2Node);
				int clip1Ref = uniqueInstancedClipPtrs.GetIndex(clip1NodePtr);
				int clip2Ref = uniqueInstancedClipPtrs.GetIndex(clip2NodePtr);
				tupAnimClipBlend.AddClipInstance1Reference(clip1Ref);
				tupAnimClipBlend.AddClipInstance2Reference(clip2Ref);

				// weight curve
				MPlug weightPlug = animBlendNode.findPlug("weight",&status);
				if (status)
				{
					MPlugArray conns;
					if (weightPlug.connectedTo(conns,true,false) && conns.length())
					{
						MObject blendCurveNode = conns[0].node();
						if (blendCurveNode.hasFn (MFn::kAnimCurve)) 
						{
							MFnAnimCurve animCurve(blendCurveNode);
							unsigned int numKeys = animCurve.numKeys();
							if (numKeys != 0) 
							{
								TupperwareAggregate *pTupAnimCurvesAgg = tupAnimClipBlend.AddCurves();
								TupAnimCurvePool tupAnimCurvePool(pTupAnimCurvesAgg);
								
								TupperwareAggregate *pCurveAgg = tupAnimCurvePool.AddAnimCurve(0);
								AddAnimCurve(pCurveAgg,&blendCurveNode);
							}
						}
					}
				}

				// get the flags
				int flags = 0;
				MPlug rotationBlendPlug = animBlendNode.findPlug("rotationBlend",&status);
				if (status)
				{
					int rotationBlendValue;
					rotationBlendPlug.getValue(rotationBlendValue);
					switch (rotationBlendValue)
					{
						case 0: // QUATERNION_SHORT
							flags |= TupAnimClipBlend::FLAGS_ROTATION_QUATERNION_SHORT;
							break;
						case 1: // QUATERNION_LONG
							flags |= TupAnimClipBlend::FLAGS_ROTATION_QUATERNION_LONG;
							break;
						case 2: // LINEAR
							flags |= TupAnimClipBlend::FLAGS_ROTATION_LINEAR;
							break;
					}
				}
				tupAnimClipBlend.AddFlags(flags);
			}
		}
	}
}

void TupAnimExport::AddLocalToParentBindMatrix(TupperwareAggregate *pAnimSetAgg,MDagPath dagPath)
{
   if (dagPath.apiType() != MFn::kJoint)
      return;

  TupAnimSet tupAnimSet(pAnimSetAgg);

  TupperwareAggregate *pLocalToParentBindMatrixAgg = tupAnimSet.AddLocalToParentBindMatrix();

  TupTransform tupTransform(pLocalToParentBindMatrixAgg);

  tupTransform.SetTransformType(TupTransform::STATIC_MATRIX);
  TupStaticMatrix tupStaticMatrix(pLocalToParentBindMatrixAgg);

  MMatrix inclusive,exclusive,localtoparent;
  MTools::GetInclusiveBindPoseMatrix(dagPath,inclusive);
  MTools::GetExclusiveBindPoseMatrix(dagPath,exclusive);
  localtoparent = inclusive * (exclusive.inverse());

  float fMatrix[4][4];
  localtoparent.get(fMatrix);

  fMatrix[3][0] = fMatrix[3][0] * 0.01f;//force matrix to meters
  fMatrix[3][1] = fMatrix[3][1] * 0.01f;
  fMatrix[3][2] = fMatrix[3][2] * 0.01f;

  tupStaticMatrix.AddMatrix((float*)fMatrix);
}

void TupAnimExport::AddJointOrientationQuaternion(TupperwareAggregate *pAnimSetAgg,MDagPath dagPath)
{
   if (dagPath.apiType() != MFn::kJoint)
      return;

   TupAnimSet tupAnimSet(pAnimSetAgg);

   MFnIkJoint fnjoint(dagPath);
   MQuaternion quat;
   fnjoint.getOrientation(quat);

   float fValues[4];
   fValues[0] = (float)quat.x;
   fValues[1] = (float)quat.y;
   fValues[2] = (float)quat.z;
   fValues[3] = (float)quat.w;
   tupAnimSet.AddJointOrientationQuaternion(fValues);
}

void TupAnimExport::AddReusableActionInfo(TupperwareAggregate *pAnimSetAgg,MDagPath dagPath)
{
   if (dagPath.apiType() != MFn::kJoint)
      return;

   TupAnimSet tupAnimSet(pAnimSetAgg);

   Vector3 bonedirection,rolldirection,parentbonedirection,parentrolldirection;

   MDagPath parentdagpath(dagPath);
   parentdagpath.pop();

   MQuaternion quat;
   MTools::GetInclusiveOrientation(parentdagpath,quat);

   MVector Zidentity(0,0,1);
   MVector Yidentity(0,1,0);

   MVector parentorientationvector = Zidentity.rotateBy(quat);
   parentorientationvector.normalize();

   MVector parentrollvector = Yidentity.rotateBy(quat);
   parentrollvector.normalize();

   parentbonedirection = Vector3((float)parentorientationvector.x,(float)parentorientationvector.y,(float)parentorientationvector.z);
   parentrolldirection = Vector3((float)parentrollvector.x,(float)parentrollvector.y,(float)parentrollvector.z);

   Zidentity = MVector(0,0,1);
   Yidentity = MVector(0,1,0);

   MQuaternion quat2;
   MTools::GetInclusiveOrientation(dagPath,quat2);

   MVector orientationvector = Zidentity.rotateBy(quat2);
   orientationvector.normalize();

   MVector rollvector = Yidentity.rotateBy(quat2);
   rollvector.normalize();

   bonedirection = Vector3((float)orientationvector.x,(float)orientationvector.y,(float)orientationvector.z);
   rolldirection = Vector3((float)rollvector.x,(float)rollvector.y,(float)rollvector.z);

   tupAnimSet.AddBoneDirection((float*)&bonedirection);
   tupAnimSet.AddRollDirection((float*)&rolldirection);
   tupAnimSet.AddParentBoneDirection((float*)&parentbonedirection);
   tupAnimSet.AddParentRollDirection((float*)&parentrolldirection);
}

void TupAnimExport::AddAnimationGlobals(TupScene *pTupScene)
{
	TupperwareAggregate *pAnimGlobalsAgg = pTupScene->AddAnimGlobals();
	TupAnimGlobals tupAnimGlobals(pAnimGlobalsAgg);

   tupAnimGlobals.AddCurrentTime((float)MTime(MAnimControl::currentTime().value(),MTime::uiUnit()).as(MTime::kSeconds));

	double dResult;
	MGlobal::executeCommand("playbackOptions -q -ast",dResult);
	tupAnimGlobals.AddStartTime((float)MTime(dResult,MTime::uiUnit()).as(MTime::kSeconds));

	MGlobal::executeCommand("playbackOptions -q -aet",dResult);
	tupAnimGlobals.AddEndTime((float)MTime(dResult,MTime::uiUnit()).as(MTime::kSeconds));

	tupAnimGlobals.AddStartPlaybackTime((float)MTime(MAnimControl::minTime().value(),MTime::uiUnit()).as(MTime::kSeconds));
	tupAnimGlobals.AddEndPlaybackTime((float)MTime(MAnimControl::maxTime().value(),MTime::uiUnit()).as(MTime::kSeconds));
	tupAnimGlobals.AddPlaybackBy((float)MAnimControl::playbackBy());
	tupAnimGlobals.AddPlaybackSpeed((float)MAnimControl::playbackSpeed());

	int flags = 0;
	switch (MAnimControl::playbackMode())
	{
		case MAnimControl::kPlaybackOnce:
			flags |= TupAnimGlobals::FLAGS_PLAYBACK_MODE_ONCE;
			break;
		case MAnimControl::kPlaybackLoop:
			flags |= TupAnimGlobals::FLAGS_PLAYBACK_MODE_LOOP;
			break;
		case MAnimControl::kPlaybackOscillate:
			flags |= TupAnimGlobals::FLAGS_PLAYBACK_MODE_OSCILLATE;
			break;
	}
	if (MAnimControl::viewMode() == MAnimControl::kPlaybackViewAll)
	{
		flags |= TupAnimGlobals::FLAGS_VIEW_MODE_ALL;
	}
	if (MAnimControl::autoKeyMode())
	{
		flags |= TupAnimGlobals::FLAGS_AUTO_KEY_MODE;
	}
   flags |= TupAnimGlobals::FLAGS_USE_MAYA_BONE_TRANSLATION_SPACE;

	tupAnimGlobals.AddFlags(flags);
}

bool TupAnimExport::TestAnimCurveNode(MObject &animCurveNode)
{
	MStatus status;
	if (!animCurveNode.hasFn (MFn::kAnimCurve)) 
	{
		return false;
	}
	MFnAnimCurve animCurve (animCurveNode);
	unsigned int numKeys = animCurve.numKeys();
	if (numKeys == 0) 
	{
		return false;
	}

	MPlug outputPlug = animCurve.findPlug("output",&status);
	if (!status)
		return false;

	MPlugArray conns;
	if (outputPlug.connectedTo(conns,false,true) && conns.length())
	{
		MObject connectedNode = conns[0].node();
		MFn::Type apiType = connectedNode.apiType();
		switch (apiType)
		{
			case MFn::kMotionPath:
				return false;
		}
	}
	return true;
}
