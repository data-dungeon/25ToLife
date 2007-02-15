///////////////////////////////////////////////////////////////////////////
//
// ExportConnections
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "TupExport.h"
#include "TupAnimExport.h"

void TupExport::AddConnections(void)
{
	AddMotionPathConnections();
}

void TupExport::AddMotionPathConnections(void)
{
	MStatus status;

	// find all motion paths
	MItDependencyNodes iter( MFn::kMotionPath);
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObject object = iter.item(&status);
		if (!status)
			continue;

		MFn::Type apiType = object.apiType();
		if (apiType != MFn::kMotionPath)
			continue;
		MFnDependencyNode motionPathNode(object);

		int connectionIndex = m_pConnectionPool->AddConnection();
		TupperwareAggregate *pConnectionAgg = m_pConnectionPool->GetConnection(connectionIndex);
		TupConnection tupConnection(pConnectionAgg);
		tupConnection.SetConnectionType(TupConnection::MOTION_PATH);
		TupConnectionMotionPath tupMotionPath(pConnectionAgg);

		MString name = motionPathNode.name();
		tupMotionPath.AddName(name.asChar());

		MPlug geometryPathPlug = motionPathNode.findPlug("geometryPath");
		MPlugArray connections;
		geometryPathPlug.connectedTo(connections,true,false,&status);
		if (status==MStatus::kSuccess && connections.length())
		{
			MObject curveObject = connections[0].node();
			MFnDagNode curveDagNode(curveObject);
			MDagPath curveDagPath;
			curveDagNode.getPath(curveDagPath);
			curveDagPath.pop(); // up to transform
			int curvePathRef = m_pNodePoolDagPaths->GetIndex(curveDagPath);
			if (curvePathRef!=-1)
			{
				tupMotionPath.AddPathNodeReference(curvePathRef);
			}
		}

		Keeper<int> animatedObjectNodeRefs;

		MPlug messagePlug = motionPathNode.findPlug("message");
		messagePlug.connectedTo(connections,false,true,&status);
		for (connectionIndex=0;connectionIndex<(int)connections.length();connectionIndex++)
		{
			MFnDagNode dagNode(connections[connectionIndex].node());
			MDagPathArray dagPaths;
			status = dagNode.getAllPaths(dagPaths);
			if (!status)
				continue;
			int numDagPaths = dagPaths.length();
			for (int pathIndex=0;pathIndex<numDagPaths;pathIndex++)
			{
				MDagPath dagPath = dagPaths[pathIndex];
				int nodeRef = m_pNodePoolDagPaths->GetIndex(dagPath);
				if (nodeRef!=-1)
					animatedObjectNodeRefs.Add(nodeRef);
			}
		}
		if (animatedObjectNodeRefs.GetSize())
		{
			tupMotionPath.AddAnimatedNodeReferences(animatedObjectNodeRefs.GetData(),animatedObjectNodeRefs.GetSize());
		}

		tupMotionPath.AddFrontTwist(MayaHelper::GetFloatAttrib(motionPathNode,"frontTwist"));
		tupMotionPath.AddUpTwist(MayaHelper::GetFloatAttrib(motionPathNode,"upTwist"));
		tupMotionPath.AddSideTwist(MayaHelper::GetFloatAttrib(motionPathNode,"sideTwist"));
		float tempVector[3];
		tempVector[0] = MayaHelper::GetFloatAttrib(motionPathNode,"worldUpVectorX");
		tempVector[1] = MayaHelper::GetFloatAttrib(motionPathNode,"worldUpVectorY");
		tempVector[2] = MayaHelper::GetFloatAttrib(motionPathNode,"worldUpVectorZ");
		tupMotionPath.AddWorldUpVector(tempVector);
		tupMotionPath.AddBankScale(MayaHelper::GetFloatAttrib(motionPathNode,"bankScale"));
		tupMotionPath.AddBankLimit(MayaHelper::GetFloatAttrib(motionPathNode,"bankLimit"));
		tupMotionPath.AddUValue(MayaHelper::GetFloatAttrib(motionPathNode,"uValue"));

		int worldUpRef = -1;
		MPlug worldUpObjectPlug = motionPathNode.findPlug("worldUpMatrix");
		worldUpObjectPlug.connectedTo(connections,true,false,&status);
		if (status==MStatus::kSuccess && connections.length())
		{
			MObject worldUpObject = connections[0].node();
			MFn::Type worldUpObjectType = worldUpObject.apiType();
			MFnDagNode worldUpDagNode(worldUpObject);
			MDagPath worldUpDagPath;
			worldUpDagNode.getPath(worldUpDagPath);
			TupString worldUpDatPathName = worldUpDagPath.fullPathName().asChar();
			worldUpRef = m_pNodePoolDagPaths->GetIndex(worldUpDagPath);
		}
		tupMotionPath.AddWorldUpNodeReference(worldUpRef);

		int flags = 0;
		
		if (MayaHelper::GetBoolAttrib(motionPathNode,"follow"))
		{
			flags |= TupConnectionMotionPath::FLAGS_FOLLOW;
		}
		if (MayaHelper::GetBoolAttrib(motionPathNode,"bank"))
		{
			flags |= TupConnectionMotionPath::FLAGS_BANK;
		}
		if (MayaHelper::GetBoolAttrib(motionPathNode,"fractionMode"))
		{
			flags |= TupConnectionMotionPath::FLAGS_PARAMETRIC_LENGTH;
		}
		int upAxis = MayaHelper::GetIntAttrib(motionPathNode,"upAxis");
		switch (upAxis)
		{
			case 0:
				flags |= TupConnectionMotionPath::FLAGS_UP_AXIS_X;
				break;
			case 1:
				flags |= TupConnectionMotionPath::FLAGS_UP_AXIS_Y;
				break;
			case 2:
				flags |= TupConnectionMotionPath::FLAGS_UP_AXIS_Z;
				break;
		}
		int frontxis = MayaHelper::GetIntAttrib(motionPathNode,"frontAxis");
		switch (frontxis)
		{
			case 0:
				flags |= TupConnectionMotionPath::FLAGS_FRONT_AXIS_X;
				break;
			case 1:
				flags |= TupConnectionMotionPath::FLAGS_FRONT_AXIS_Y;
				break;
			case 2:
				flags |= TupConnectionMotionPath::FLAGS_FRONT_AXIS_Z;
				break;
		}
		int worldUpType = MayaHelper::GetIntAttrib(motionPathNode,"worldUpType");
		switch(worldUpType) 
		{
			case 0:
				flags |= TupConnectionMotionPath::FLAGS_WORLD_UP_TYPE_SCENE_UP;
				break;
			case 1:
				flags |= TupConnectionMotionPath::FLAGS_WORLD_UP_TYPE_OBJECT_UP;
				break;
			case 2:
				flags |= TupConnectionMotionPath::FLAGS_WORLD_UP_TYPE_OBJECT_ROTATION_UP;
				break;
			case 3:
				flags |= TupConnectionMotionPath::FLAGS_WORLD_UP_TYPE_VECTOR;
				break;
			case 4:
				flags |= TupConnectionMotionPath::FLAGS_WORLD_UP_TYPE_NORMAL;
				break;
		}
		if (MayaHelper::GetBoolAttrib(motionPathNode,"inverseUp"))
		{
			flags |= TupConnectionMotionPath::FLAGS_INVERSE_UP;
		}
		if (MayaHelper::GetBoolAttrib(motionPathNode,"inverseFront"))
		{
			flags |= TupConnectionMotionPath::FLAGS_INVERSE_FRONT;
		}
		tupMotionPath.AddFlags(flags);
		TupperwareAggregate *pAnimatedChannelsAgg = tupMotionPath.AddAnimatedChannels();
      TupAnimExport::AddAnimCurveForDepNode(pAnimatedChannelsAgg,motionPathNode);
	}
}
