
#include "StdAfx.h"
#include "OrientLikeCmd.h"
#include "OrientLikeNode.h"

#define kNameFlag									"-n"
#define kNameFlagLong							"-name"

#define kPercentFlag								"-p"
#define kPercentFlagLong						"-percent"

#define kRemoveFlag								"-rm"
#define kRemoveFlagLong							"-remove"

#define kTargetListFlag							"-tl"
#define kTargetListFlagLong					"-targetList"

#define kPercentAliasListFlag					"-pal"
#define kPercentAliasListFlagLong			"-percentAliasList"

#define kInitialRotationFlag					"-ir"
#define kInitialRotationFlagLong				"-initialRotation"

MSyntax OrientLikeCmd::m_cachedSyntax;
bool OrientLikeCmd::m_bCachedSyntax = false;

MString itoa (int n)
{
	char buffer [256];
	sprintf (buffer, "%d", n);
	return MString (buffer);
}

MString dtoa (double d)
{
	char buffer [256];
	sprintf (buffer, "%f", d);
	return MString (buffer);
}

OrientLikeCmd::OrientLikeCmd()
{
}

OrientLikeCmd::~OrientLikeCmd()
{
}

void *OrientLikeCmd::creator(void)
{
	return new OrientLikeCmd;
}

// this function trys to get the orient like constraint object based on the selection set
MObject OrientLikeCmd::GetOrientLikeConstraintObject(const MSelectionList &selectionList,MStatus *pStatus) const
{
	if (selectionList.length())
	{
		MStatus status;
		MDagPath constrainDagPath;
		status = selectionList.getDagPath(selectionList.length()-1,constrainDagPath);
		if (status)
		{
			MFnTransform constrainFn;
			status = constrainFn.setObject(constrainDagPath);
			if (status)
			{
				// if last object is the constraint itself then return it
				if (strcmp(constrainFn.typeName().asChar(),"orientLikeConstraint")==0)
				{
					if (pStatus)
					{
						*pStatus = MS::kSuccess;
					}
					return constrainFn.object();
				}
				// the last object should be the constrained node (find the constraint connectected to it)
				else
				{
					int numChildren = constrainFn.childCount();
					for (int childIndex = 0; childIndex < numChildren; childIndex++)
					{
						MObject childObj = constrainFn.child(childIndex,&status);
						if (status && !childObj.isNull())
						{
							MFnDependencyNode childFn(childObj);
							if (strcmp(childFn.typeName().asChar(),"orientLikeConstraint")==0)
							{
								if (pStatus)
								{
									*pStatus = MS::kSuccess;
								}
								return childObj;
							}
						}
					}
				}
			}
		}
	}
	if (pStatus)
	{
		*pStatus = MS::kFailure;
	}
	return MObject::kNullObj;
}

// search to find testTarget among the current connected targets
// if found it will return a logical index >=0
// if not found it will return -1
int OrientLikeCmd::GetTargetLogicalIndex(const MObject &testTarget, const MPlug &targetsPlug, MStatus *pStatus) const
{
	MStatus status;
	unsigned numTargets = targetsPlug.numElements(&status);
	if (status)
	{
		for (unsigned targetIndex = 0; targetIndex < numTargets; targetIndex++)
		{
			MPlug targetPlug = targetsPlug.elementByPhysicalIndex(targetIndex,&status);
			if( status && !targetPlug.isNull() )
			{
				MPlug targetRotatePlug = targetPlug.child(OrientLikeNode::targetRotate, &status);
				if (status && !targetRotatePlug.isNull())
				{
					if (targetRotatePlug.isConnected())
					{
						MPlugArray connections;
						targetRotatePlug.connectedTo(connections,true,false,&status);
						if (status && connections.length())
						{
							MObject targetNode = connections[0].node();
							if (targetNode == testTarget)
							{
								if (pStatus)
								{
									*pStatus = MS::kSuccess;
								}
								return (int)targetPlug.logicalIndex();
							}
						}
					}
				}
			}
		}
	}
	if (pStatus)
	{
		*pStatus = MS::kFailure;
	}
	return -1;
}

MStatus OrientLikeCmd::doIt ( const MArgList &args )
{ 
	m_bUndoable = false; // start with no undo

	// make sure the syntax is built
	if (!m_bCachedSyntax)
		newSyntax();

	MStatus status;
	MArgDatabase argDatabase(m_cachedSyntax,args,&status);
	if (!status)
	{
		MGlobal::displayError( MString("Unable to parse command: ") + status.errorString());
		return status;
	}

	MSelectionList selectionList;
	status = argDatabase.getObjects(selectionList);
	if (!status)
	{
		DISPLAY_ERROR( MString("Error parsing command line objects: ") + status.errorString());
		return status;
	}

	// if no objects specified on the command line then get the current selected objects
	if (selectionList.length()==0)
	{
		status = MGlobal::getActiveSelectionList(selectionList);
		if (!status)
		{
			DISPLAY_ERROR( MString("Unable to get active selection list: ") + status.errorString());
			return status;
		}
	}

	// query mode
	if (argDatabase.isQuery())
	{
		if (selectionList.length()==0)
		{
			DISPLAY_ERROR("Invalid Object.");
			return MS::kFailure;
		}

		if (argDatabase.isFlagSet(kNameFlag))
		{
			if (selectionList.length()<1)
			{
				DISPLAY_ERROR("Need at least 1 node specified.");
				return MS::kFailure;
			}

			MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
			if (status)
			{
				MFnDagNode orientLikeNodeFn(orientLikeConstraintObj);
				setResult(orientLikeNodeFn.partialPathName());
			}
			else
			{
				DISPLAY_ERROR("Could not find orient like contraint node.");
				return MS::kFailure;
			}
		}
		if (argDatabase.isFlagSet(kTargetListFlag))
		{
			if (selectionList.length()>1)
			{
				DISPLAY_ERROR("Cannot specify targets for this query.");
				return MS::kFailure;
			}

			MStringArray resultStrings;

			MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
			if (status)
			{
				MFnDagNode orientLikeNodeFn(orientLikeConstraintObj);
				MPlug targetsPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target, &status);
				if ( status && !targetsPlug.isNull())
				{
					unsigned numTargets = targetsPlug.numElements();
					for( unsigned targetIndex = 0; targetIndex < numTargets; targetIndex++)
					{
						MPlug targetPlug = targetsPlug.elementByPhysicalIndex( targetIndex, &status );
						if( status && !targetPlug.isNull() )
						{
							MPlug targetRotatePlug = targetPlug.child(OrientLikeNode::targetRotate, &status);
							if (status && !targetRotatePlug.isNull())
							{
								if (targetRotatePlug.isConnected())
								{
									MPlugArray connections;
									targetRotatePlug.connectedTo(connections,true,false,&status);
									if (status && connections.length())
									{
										MObject targetObj = connections[0].node();
										MFnDagNode targetFn(targetObj);
										resultStrings.append(targetFn.partialPathName());
									}
								}
							}
						}
					}
				}
				setResult(resultStrings);
			}
			else
			{
				DISPLAY_ERROR("Could not find contraint node.");
				return MS::kFailure;
			}
		}
		if (argDatabase.isFlagSet(kPercentFlag))
		{
			if (selectionList.length()<2)
			{
				DISPLAY_ERROR("Target list was empty or contained no valid targets.");
				return MS::kFailure;
			}

			MStringArray resultStrings;

			MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
			if (status && !orientLikeConstraintObj.isNull())
			{
				MFnDagNode orientLikeNodeFn(orientLikeConstraintObj);
				MPlug targetsPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target, &status);

				// count how many we are needing to do
				unsigned int selectionIndex;
				for (selectionIndex = 0; selectionIndex < selectionList.length()-1; selectionIndex++)
				{
					MDagPath targetPath;
					selectionList.getDagPath(selectionIndex,targetPath);

					MFnDagNode targetFn;
					targetFn.setObject(targetPath);

					int targetLogicalIndex = GetTargetLogicalIndex(targetFn.object(),targetsPlug,&status);
					if (status)
					{
						MPlug targetPlug = targetsPlug.elementByLogicalIndex(targetLogicalIndex,&status);

						if( status && !targetPlug.isNull() )
						{
							MPlug targetPercentPlug = targetPlug.child(OrientLikeNode::targetPercent, &status);
							if (status && !targetPercentPlug.isNull())
							{
								double value;
								status = targetPercentPlug.getValue(value);
								resultStrings.append(dtoa(value));
							}
						}
					}
				}
				if (resultStrings.length())
				{
					setResult(resultStrings);
				}
				else
				{
					DISPLAY_ERROR("Target list was empty or contained no valid targets.");
					return MS::kFailure;
				}
			}
			else
			{
				DISPLAY_ERROR("Could not find contraint node.");
				return MS::kFailure;
			}
		}
		if (argDatabase.isFlagSet(kPercentAliasListFlag))
		{
			if (selectionList.length()>1)
			{
				DISPLAY_ERROR("Cannot specify targets for this query.");
				return MS::kFailure;
			}

			MStringArray resultStrings;

			MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
			if (status)
			{
				MFnDagNode orientLikeNodeFn(orientLikeConstraintObj);
				MPlug targetsPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target, &status);
				if ( status && !targetsPlug.isNull())
				{
					unsigned numTargets = targetsPlug.numElements();
					for( unsigned targetIndex = 0; targetIndex < numTargets; targetIndex++)
					{
						MPlug targetPlug = targetsPlug.elementByPhysicalIndex( targetIndex, &status );
						if( status && !targetPlug.isNull() )
						{
							MPlug targetPercentPlug = targetPlug.child(OrientLikeNode::targetPercent, &status);
							if (status && !targetPercentPlug.isNull())
							{
								if (targetPercentPlug.isConnected())
								{
									MPlugArray connections;
									targetPercentPlug.connectedTo(connections,true,false,&status);
									if (status && connections.length())
									{
										resultStrings.append(connections[0].partialName(false,false,false,false,false,true));
									}
								}
							}
						}
					}
				}
				setResult(resultStrings);
			}
			else
			{
				DISPLAY_ERROR("Could not find contraint node.");
				return MS::kFailure;
			}
		}
		if (argDatabase.isFlagSet(kInitialRotationFlag))
		{
			if (selectionList.length()>1)
			{
				DISPLAY_ERROR("Cannot specify targets for this query.");
				return MS::kFailure;
			}

			MStringArray resultStrings;

			MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
			if (status)
			{
				MFnDependencyNode orientLikeNodeFn(orientLikeConstraintObj);
				MPlug initialRotationXPlug = orientLikeNodeFn.findPlug(OrientLikeNode::constraintInitialRotateX, &status);
				if ( status && !initialRotationXPlug.isNull())
				{
					MAngle value;
					initialRotationXPlug.getValue(value);
					resultStrings.append(dtoa(value.asDegrees()));
				}
				MPlug initialRotationYPlug = orientLikeNodeFn.findPlug(OrientLikeNode::constraintInitialRotateY, &status);
				if ( status && !initialRotationYPlug.isNull())
				{
					MAngle value;
					initialRotationYPlug.getValue(value);
					resultStrings.append(dtoa(value.asDegrees()));
				}
				MPlug initialRotationZPlug = orientLikeNodeFn.findPlug(OrientLikeNode::constraintInitialRotateZ, &status);
				if ( status && !initialRotationZPlug.isNull())
				{
					MAngle value;
					initialRotationZPlug.getValue(value);
					resultStrings.append(dtoa(value.asDegrees()));
				}
				if (resultStrings.length()==3)
				{
					setResult(resultStrings);
				}
			}
			else
			{
				DISPLAY_ERROR("Could not find contraint node.");
				return MS::kFailure;
			}
		}

		return MS::kSuccess;
	}
	// edit mode
	else if (argDatabase.isEdit())
	{
		if (selectionList.length()<1)
		{
			DISPLAY_ERROR("Need at least 1 node specified.");
			return MS::kFailure;
		}
		MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
		if (!status || orientLikeConstraintObj.isNull())
		{
			DISPLAY_ERROR("Could not find orient like contraint node.");
			return MS::kFailure;
		}
		MFnDagNode orientLikeNodeFn(orientLikeConstraintObj);

		if (argDatabase.isFlagSet(kNameFlag))
		{
			MString newNodeName;
			argDatabase.getFlagArgument(kNameFlag,0,newNodeName);
			m_dagMod[0].renameNode(orientLikeConstraintObj,newNodeName);
			m_bUndoable = true;
		}
		if (argDatabase.isFlagSet(kPercentFlag))
		{
			if (selectionList.length()<2)
			{
				DISPLAY_ERROR("Target list was empty or contained no valid targets.");
				return MS::kFailure;
			}
			// this is the number of percent values given in the command
			const unsigned int nPercentValues = argDatabase.numberOfFlagUses( kPercentFlag );

			if (selectionList.length()-1 != nPercentValues)
			{
				DISPLAY_ERROR("Number of percent values need to match the number of targets.");
				return MS::kFailure;
			}

			MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
			if (status && !orientLikeConstraintObj.isNull())
			{
				MFnDagNode orientLikeNodeFn(orientLikeConstraintObj);
				MPlug targetsPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target, &status);

				// count how many we are needing to do
				unsigned int selectionIndex;
				for (selectionIndex = 0; selectionIndex < selectionList.length()-1; selectionIndex++)
				{
					MDagPath targetPath;
					selectionList.getDagPath(selectionIndex,targetPath);

					MFnDagNode targetFn;
					targetFn.setObject(targetPath);

					int targetLogicalIndex = GetTargetLogicalIndex(targetFn.object(),targetsPlug,&status);
					if (status)
					{
						MPlug targetPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target).elementByLogicalIndex(targetLogicalIndex);

						MString percentLongName = targetFn.name()+MString("P")+itoa(targetLogicalIndex);

						MArgList targetPercentArgs;
						argDatabase.getFlagArgumentList( kPercentFlag, selectionIndex, targetPercentArgs );
						if (targetPercentArgs.length()>=1)
						{
							double percentValue;
							targetPercentArgs.get(0,percentValue);
							m_dagMod[0].commandToExecute( MString("setAttr ")+orientLikeNodeFn.partialPathName() + MString(".") + percentLongName + MString(" ") + percentValue);
							m_bUndoable = true;
						}
					}
				}
			}
			else
			{
				DISPLAY_ERROR("Could not find contraint node.");
				return MS::kFailure;
			}
		}
		if (argDatabase.isFlagSet(kInitialRotationFlag))
		{
			MAngle initialRotate[3];
			argDatabase.getFlagArgument(kInitialRotationFlag,0,initialRotate[0]); 
			argDatabase.getFlagArgument(kInitialRotationFlag,1,initialRotate[1]); 
			argDatabase.getFlagArgument(kInitialRotationFlag,2,initialRotate[2]); 

			m_dagMod[0].commandToExecute( MString("setAttr ")+orientLikeNodeFn.partialPathName() + ".constraintInitialRotateX " + initialRotate[0].asDegrees());
			m_dagMod[0].commandToExecute( MString("setAttr ")+orientLikeNodeFn.partialPathName() + ".constraintInitialRotateY " + initialRotate[1].asDegrees());
			m_dagMod[0].commandToExecute( MString("setAttr ")+orientLikeNodeFn.partialPathName() + ".constraintInitialRotateZ " + initialRotate[2].asDegrees());
			m_bUndoable = true;
		}
		if (argDatabase.isFlagSet(kRemoveFlag))
		{
			if (selectionList.length()<2)
			{
				DISPLAY_ERROR("Target list was empty or contained no valid targets.");
				return MS::kFailure;
			}

			MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
			if (status && !orientLikeConstraintObj.isNull())
			{
				MFnDagNode orientLikeNodeFn(orientLikeConstraintObj);
				MPlug targetsPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target, &status);

				// count how many we are needing to do
				unsigned int selectionIndex;
				for (selectionIndex = 0; selectionIndex < selectionList.length()-1; selectionIndex++)
				{
					MDagPath targetPath;
					selectionList.getDagPath(selectionIndex,targetPath);

					MFnDagNode targetFn;
					targetFn.setObject(targetPath);

					int targetLogicalIndex = GetTargetLogicalIndex(targetFn.object(),targetsPlug,&status);
					if (status)
					{
						MPlug targetPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target).elementByLogicalIndex(targetLogicalIndex);

						m_dagMod[0].disconnect(targetFn.findPlug(OrientLikeNode::rotate), targetPlug.child(OrientLikeNode::targetRotate));
						m_dagMod[0].disconnect(targetFn.findPlug(OrientLikeNode::rotateOrder), targetPlug.child(OrientLikeNode::targetRotateOrder));

						MString percentLongName = targetFn.name()+MString("P")+itoa(targetLogicalIndex);
						MString percentShortName = "p"+itoa(targetLogicalIndex);

						MPlug targetPercentPlug = orientLikeNodeFn.findPlug(percentLongName,&status);
						if (!status || targetPercentPlug.isNull())
						{
							DISPLAY_ERROR(MString("Error finding new percent plug: - ") + status.errorString());
							return status;
						}
						m_dagMod[0].disconnect(targetPercentPlug,targetPlug.child(OrientLikeNode::targetPercent));
						m_dagMod[0].removeAttribute(orientLikeConstraintObj,targetPercentPlug.attribute(),MFnDependencyNode::kLocalDynamicAttr);
						m_bUndoable = true;
					}
				}
			}
			else
			{
				DISPLAY_ERROR("Could not find contraint node.");
				return MS::kFailure;
			}
		}



		if (m_bUndoable)
		{
			status = m_dagMod[0].doIt();
			if (!status)
			{
				DISPLAY_ERROR(MString("Error processing command: - ") + status.errorString());
				return status;
			}
			// if we tried to remove some targets (see if the node needs to go away)
			if (argDatabase.isFlagSet(kRemoveFlag))
			{
				MObject orientLikeConstraintObj = GetOrientLikeConstraintObject(selectionList,&status);
				if (status && !orientLikeConstraintObj.isNull())
				{
					MFnDagNode orientLikeNodeFn(orientLikeConstraintObj);
					MPlug targetsPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target, &status);

					unsigned numTargets = targetsPlug.numElements(&status);
					int numValidTargets = 0;
					for (unsigned targetIndex = 0; targetIndex < numTargets; targetIndex++)
					{
						MPlug targetPlug = targetsPlug.elementByPhysicalIndex(targetIndex,&status);
						if( status && !targetPlug.isNull() )
						{
							MPlug targetRotatePlug = targetPlug.child(OrientLikeNode::targetRotate, &status);
							if (status && !targetRotatePlug.isNull())
							{
								if (targetRotatePlug.isConnected())
								{
									numValidTargets++;
								}
							}
						}
					}
					// delete node if no valid targets
					if (numValidTargets==0)
					{
						m_dagMod[1].deleteNode(orientLikeConstraintObj);
						status = m_dagMod[1].doIt();
						if (!status)
						{
							DISPLAY_ERROR(MString("Error processing command to remove constraint node: - ") + status.errorString());
							return status;
						}
						return MS::kSuccess; // don't return the name if it is deleted
					}
				}
			}
			setResult(orientLikeNodeFn.partialPathName());
		}
		return MS::kSuccess;
	}
	// creation mode
	else
	{
		if (selectionList.length()<2)
		{
			DISPLAY_ERROR("Target list was empty or contained no valid targets.");
			return MS::kFailure;
		}

		MDagPath constrainDagPath;
		selectionList.getDagPath(selectionList.length()-1,constrainDagPath);
		MFnTransform constrainFn;
		constrainFn.setObject(constrainDagPath);

		MString newNodeName;
		bool bRenameNode = false;
		bool bCreatedNew = false;

		MFnDagNode orientLikeNodeFn;

		MObject orientLikeNodeObj = GetOrientLikeConstraintObject(selectionList,&status);
		if (status || !orientLikeNodeObj.isNull())
		{
			orientLikeNodeFn.setObject( orientLikeNodeObj );
		}
		else // need to create a new node
		{
			MObject parentObject = constrainFn.object();
			orientLikeNodeObj = m_dagMod[0].createNode(OrientLikeNode::id,parentObject,&status);
			if (!status || orientLikeNodeObj.isNull())
			{
				DISPLAY_ERROR(MString("Unable to create orient like constraint node - ")+status.errorString());
				return MS::kFailure;
			}

			orientLikeNodeFn.setObject( orientLikeNodeObj );
			newNodeName = constrainFn.name() + MString("_orientLikeConstraint1");
			bRenameNode = true;
			bCreatedNew = true;
		}
		if (argDatabase.isFlagSet(kNameFlag))
		{
			argDatabase.getFlagArgument(kNameFlag,0,newNodeName);
			bRenameNode = true;
		}
		if (bRenameNode)
		{
			m_dagMod[0].renameNode(orientLikeNodeObj,newNodeName);
		}
		status = m_dagMod[0].doIt();
		if (!status)
		{
			DISPLAY_ERROR(MString("Error processing first phase of command: - ") + status.errorString());
			return status;
		}

		MAngle initialRotate[3];

		// if offset set from command
		bool bSetInitialRotation = false;
		if (argDatabase.isFlagSet(kInitialRotationFlag))
		{
			argDatabase.getFlagArgument(kInitialRotationFlag,0,initialRotate[0]); 
			argDatabase.getFlagArgument(kInitialRotationFlag,1,initialRotate[1]); 
			argDatabase.getFlagArgument(kInitialRotationFlag,2,initialRotate[2]); 
			bSetInitialRotation = true;
		}
		// if offset set from initial rotation of constrained node
		else if (bCreatedNew)
		{
			constrainFn.findPlug("rotateX").getValue(initialRotate[0]);
			constrainFn.findPlug("rotateY").getValue(initialRotate[1]);
			constrainFn.findPlug("rotateZ").getValue(initialRotate[2]);
			bSetInitialRotation = true;
		}
		if (bSetInitialRotation)
		{
			m_dagMod[1].commandToExecute( MString("setAttr ")+orientLikeNodeFn.partialPathName() + ".constraintInitialRotateX " + initialRotate[0].asDegrees());
			m_dagMod[1].commandToExecute( MString("setAttr ")+orientLikeNodeFn.partialPathName() + ".constraintInitialRotateY " + initialRotate[1].asDegrees());
			m_dagMod[1].commandToExecute( MString("setAttr ")+orientLikeNodeFn.partialPathName() + ".constraintInitialRotateZ " + initialRotate[2].asDegrees());
		}
		if (bCreatedNew)
		{
			m_dagMod[1].connect(orientLikeNodeFn.findPlug(OrientLikeNode::constraintRotate),constrainFn.findPlug("rotate"));
			m_dagMod[1].connect(constrainFn.findPlug("rotateOrder"),orientLikeNodeFn.findPlug(OrientLikeNode::constraintRotateOrder));
		}

		MFnNumericAttribute nAttr;

		// this is the number of percent values given in the command
		const unsigned int nPercentValues = argDatabase.numberOfFlagUses( kPercentFlag );

		if (nPercentValues && selectionList.length()-1 != nPercentValues)
		{
			DISPLAY_ERROR("Number of percent values need to match the number of targets.");
			return MS::kFailure;
		}

		// go though and find the max logicial index (this is where new targets will go)
		MPlug targetsPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target,&status);
		unsigned numInitialTargets = targetsPlug.numElements(&status);
		int maxLogicalIndex = -1;
		unsigned targetIndex;
		for (targetIndex = 0; targetIndex < numInitialTargets; targetIndex++)
		{
			MPlug targetPlug = targetsPlug.elementByPhysicalIndex(targetIndex,&status);
			if( status && !targetPlug.isNull() )
			{
				MPlug targetRotatePlug = targetPlug.child(OrientLikeNode::targetRotate, &status);
				if (status && !targetRotatePlug.isNull())
				{
					if (targetRotatePlug.isConnected())
					{
						int logicalIndex = (int)targetPlug.logicalIndex();
						if (logicalIndex > maxLogicalIndex)
						{
							maxLogicalIndex = (int)logicalIndex;
						}
					}
				}
			}
		}
		int currentAddTarget = maxLogicalIndex+1;

		TupArray<int> targetLogicalIndices;
		targetLogicalIndices.SetSize(selectionList.length()-1);
		TupArray<bool> targetNewConnections;
		targetNewConnections.SetSize(selectionList.length()-1);

		// count how many we are needing to do
		unsigned int selectionIndex;
		for (selectionIndex = 0; selectionIndex < selectionList.length()-1; selectionIndex++)
		{
			MDagPath targetPath;
			selectionList.getDagPath(selectionIndex,targetPath);

			MFnTransform targetFn;
			targetFn.setObject(targetPath);

			// see if target is already connected
			targetLogicalIndices[selectionIndex] = GetTargetLogicalIndex(targetFn.object(),targetsPlug,&status);
			if (status) // already there (logical index is correct)
			{
				targetNewConnections[selectionIndex] = false;
			}
			else // if not found then create one
			{
				targetNewConnections[selectionIndex] = true;
				// new logical index (after all the others)
				targetLogicalIndices[selectionIndex] = currentAddTarget++;
				// create new connection to target
				MPlug targetPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target).elementByLogicalIndex(targetLogicalIndices[selectionIndex]);
				m_dagMod[1].connect(targetFn.findPlug("rotate"), targetPlug.child(OrientLikeNode::targetRotate));
				m_dagMod[1].connect(targetFn.findPlug("rotateOrder"), targetPlug.child(OrientLikeNode::targetRotateOrder));
				MString percentLongName = targetFn.name()+MString("P")+itoa(targetLogicalIndices[selectionIndex]);
				MString percentShortName = "p"+itoa(targetLogicalIndices[selectionIndex]);

				MObject targetPercent = nAttr.create(percentLongName,percentShortName,MFnNumericData::kDouble,1.0);
				nAttr.setKeyable(true);
				m_dagMod[1].addAttribute(orientLikeNodeObj,targetPercent,MFnDependencyNode::kLocalDynamicAttr);
			}
		}

		status = m_dagMod[1].doIt();
		if (!status)
		{
			DISPLAY_ERROR(MString("Error processing second phase of command: - ") + status.errorString());
			return status;
		}

		// count how many we are needing to do
		for (selectionIndex = 0; selectionIndex < selectionList.length()-1; selectionIndex++)
		{
			MDagPath targetPath;
			selectionList.getDagPath(selectionIndex,targetPath);

			MFnDagNode targetFn;
			targetFn.setObject(targetPath);

			MPlug targetPlug = orientLikeNodeFn.findPlug(OrientLikeNode::target).elementByLogicalIndex(targetLogicalIndices[selectionIndex]);

			MString percentLongName = targetFn.name()+MString("P")+itoa(targetLogicalIndices[selectionIndex]);

			// if just created then hook up the two attributes
			if (targetNewConnections[selectionIndex])
			{
				MPlug targetPercentPlug = orientLikeNodeFn.findPlug(percentLongName,&status);
				if (!status)
				{
					DISPLAY_ERROR(MString("Error finding new percent plug: - ") + status.errorString());
					return status;
				}
				m_dagMod[2].connect(targetPercentPlug,targetPlug.child(OrientLikeNode::targetPercent));
			}

			// if there is a percent for this target
			if (nPercentValues>selectionIndex)
			{
				MArgList targetPercentArgs;
				argDatabase.getFlagArgumentList( kPercentFlag, selectionIndex, targetPercentArgs );
				if (targetPercentArgs.length()>=1)
				{
					double percentValue;
					targetPercentArgs.get(0,percentValue);
					m_dagMod[2].commandToExecute( MString("setAttr ")+orientLikeNodeFn.partialPathName() + MString(".") + percentLongName + MString(" ") + percentValue);
				}
			}
		}
		m_bUndoable = true;

		status = m_dagMod[2].doIt();
		if (!status)
		{
			DISPLAY_ERROR(MString("Error processing third phase of command: - ") + status.errorString());
			return status;
		}

		setResult(orientLikeNodeFn.partialPathName());

		return MS::kSuccess;
	}
}

MStatus OrientLikeCmd::undoIt()
{
	MStatus status;
	status = m_dagMod[2].undoIt();
	if (!status)
	{
		DISPLAY_ERROR(MString("Error processing third phase of command: - ") + status.errorString());
		return status;
	}
	status = m_dagMod[1].undoIt();
	if (!status)
	{
		DISPLAY_ERROR(MString("Error processing second phase of command: - ") + status.errorString());
		return status;
	}
	status = m_dagMod[0].undoIt();
	if (!status)
	{
		DISPLAY_ERROR(MString("Error processing first phase of command: - ") + status.errorString());
		return status;
	}
	return MS::kSuccess;
}

MStatus OrientLikeCmd::redoIt()
{
	MStatus status;
	status = m_dagMod[0].doIt();
	if (!status)
	{
		DISPLAY_ERROR(MString("Error processing first phase of command: - ") + status.errorString());
		return status;
	}
	status = m_dagMod[1].doIt();
	if (!status)
	{
		DISPLAY_ERROR(MString("Error processing second phase of command: - ") + status.errorString());
		return status;
	}
	status = m_dagMod[2].doIt();
	if (!status)
	{
		DISPLAY_ERROR(MString("Error processing third phase of command: - ") + status.errorString());
		return status;
	}
	return MS::kSuccess;
}

bool OrientLikeCmd::isUndoable() const
{
	return m_bUndoable;
}

// static
MSyntax OrientLikeCmd::newSyntax()
{
	if (m_bCachedSyntax)
		return m_cachedSyntax;

	m_cachedSyntax.enableEdit(true);
	m_cachedSyntax.enableQuery(true);
	m_cachedSyntax.addFlag( kNameFlag, kNameFlagLong, MSyntax::kString );
	m_cachedSyntax.addFlag( kPercentFlag, kPercentFlagLong, MSyntax::kDouble );
	m_cachedSyntax.makeFlagMultiUse( kPercentFlag );
	m_cachedSyntax.addFlag( kRemoveFlag, kRemoveFlagLong );
	m_cachedSyntax.addFlag( kTargetListFlag, kTargetListFlagLong );
	m_cachedSyntax.addFlag( kPercentAliasListFlag, kPercentAliasListFlagLong );
	m_cachedSyntax.addFlag( kInitialRotationFlag, kInitialRotationFlagLong, MSyntax::kAngle, MSyntax::kAngle, MSyntax::kAngle );

	m_cachedSyntax.setObjectType(MSyntax::kSelectionList, 0);

	m_bCachedSyntax = true;

	return m_cachedSyntax;
}

