#include "StdAfx.h"
#include "OrientLikeNode.h"

MTypeId  OrientLikeNode::id( 0x00363 );

MObject  OrientLikeNode::target;
MObject  OrientLikeNode::targetRotate;
MObject  OrientLikeNode::targetRotateX;
MObject  OrientLikeNode::targetRotateY;
MObject  OrientLikeNode::targetRotateZ;
MObject  OrientLikeNode::targetRotateOrder;
MObject  OrientLikeNode::targetPercent;

MObject  OrientLikeNode::constraintRotate;
MObject  OrientLikeNode::constraintRotateX;
MObject  OrientLikeNode::constraintRotateY;
MObject  OrientLikeNode::constraintRotateZ;
MObject  OrientLikeNode::constraintRotateOrder;
MObject  OrientLikeNode::constraintInitialRotate;
MObject  OrientLikeNode::constraintInitialRotateX;
MObject  OrientLikeNode::constraintInitialRotateY;
MObject  OrientLikeNode::constraintInitialRotateZ;

const double PI = 3.1415926535;
const double TWOPI = 2.0 * PI;
const double HALFPI = PI * 0.5;
const double EPSILON = 0.0000001;
#define ROTATE_ORDER_XYZ	0
#define ROTATE_ORDER_YZX	1
#define ROTATE_ORDER_ZXY	2
#define ROTATE_ORDER_XZY	3
#define ROTATE_ORDER_YXZ	4
#define ROTATE_ORDER_ZYX	5

// Error macro: if not successful, print error message and return failure.
// Assumes that "stat" contains the error value
// Note that if (!stat) is the most efficient way to test for failure
#define PERRORfail(s) \
	if (!stat) { \
	stat.perror(s); \
	return stat; \
	}

// Error macro: if not successful, print error message and return a NULL object
// Assumes that "stat" contains the error value
#define PERRORnull(s) \
	if (!stat) { \
	stat.perror(s); \
	return MObject::kNullObj; \
	}


MStatus OrientLikeNode::targetRotation(const MPlug &targetsPlug,MArrayDataHandle &inputTargetArrayData,const MQuaternion &initialQuat, MQuaternion &targetQuat )
{
	MStatus stat;

	targetQuat = initialQuat;

	int numTargets = inputTargetArrayData.elementCount ();

	for (int elementIndex = 0; elementIndex < numTargets; elementIndex++)
	{
		MPlug targetPlug = targetsPlug.elementByPhysicalIndex(elementIndex,&stat);
		if (stat && !targetPlug.isNull())
		{
			MPlug targetRotatePlug = targetPlug.child(targetRotate,&stat);
			if (stat && !targetRotatePlug.isNull())
			{
				if (targetRotatePlug.isConnected())
				{
					// array element
					inputTargetArrayData.jumpToElement(elementIndex);
					MDataHandle inputTargetData = inputTargetArrayData.inputValue(&stat);
					PERRORfail("OrientLikeNode::targetRotation inputTargetData");
					// target rotate
					MDataHandle inputTargetRotateData = inputTargetData.child(targetRotate);
					double3 &inputTargetRotateValue = inputTargetRotateData.asDouble3();
					// target rotate order
					MDataHandle inputTargetRotateOrderData = inputTargetData.child(targetRotateOrder);
					short rotateOrder = inputTargetRotateOrderData.asShort();
					MEulerRotation::RotationOrder inputTargetRotateOrderValue;
					switch ( rotateOrder ) 
					{
					case ROTATE_ORDER_XYZ:
						inputTargetRotateOrderValue = MEulerRotation::kXYZ; break;
					case ROTATE_ORDER_YZX:
						inputTargetRotateOrderValue = MEulerRotation::kYZX; break;
					case ROTATE_ORDER_ZXY:
						inputTargetRotateOrderValue = MEulerRotation::kZXY; break;
					case ROTATE_ORDER_XZY:
						inputTargetRotateOrderValue = MEulerRotation::kXZY; break;
					case ROTATE_ORDER_YXZ:
						inputTargetRotateOrderValue = MEulerRotation::kYXZ; break;
					case ROTATE_ORDER_ZYX:
						inputTargetRotateOrderValue = MEulerRotation::kZYX; break;
					default:
						inputTargetRotateOrderValue = MEulerRotation::kXYZ; break;
					}
					// target percent
					MDataHandle inputTargetPercent = inputTargetData.child(targetPercent);
					double targetPercentData = inputTargetPercent.asDouble();
					if (targetPercentData < 0.0)
						targetPercentData = 0.0;
					if (targetPercentData > 1.0)
						targetPercentData = 1.0;

					MEulerRotation targetRotationEuler(inputTargetRotateValue[0],inputTargetRotateValue[1],inputTargetRotateValue[2],
						inputTargetRotateOrderValue);
					MQuaternion targetRotationQuat;
					targetRotationQuat = targetRotationEuler;

					MQuaternion quatFrom,quatOut;

					//reduce them by the given percentage
					QatSlerpBetter(quatFrom, targetRotationQuat, targetPercentData, quatOut);
					targetQuat *= quatOut;
				}
			}
		}
	}

	return MS::kSuccess;
}

MStatus OrientLikeNode::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus stat;

	MObject thisNode = thisMObject();
	MFnDagNode fnThisNode( thisNode );
	MPlug targetsPlug = fnThisNode.findPlug(target,&stat);
	if (!stat || targetsPlug.isNull())
	{
		return stat;
	}
 
	TupString plugName = MString(plug.name()).asChar();
	if( plug == constraintRotate )
	{
		MArrayDataHandle inputTargetArrayData = data.inputArrayValue( target, &stat );
		PERRORfail("OrientLikeNode::compute getting target input array data");

		// constraint rotate
		MDataHandle outputConstraintRotateData = data.outputValue(constraintRotate);
		// constraint rotate order
		MDataHandle inputConstraintRotateOrderData = data.inputValue(constraintRotateOrder);
		short rotateOrder = inputConstraintRotateOrderData.asShort();
		MEulerRotation::RotationOrder constraintRotateOrderEulerValue;
		switch ( rotateOrder ) 
		{
			case ROTATE_ORDER_XYZ:
				constraintRotateOrderEulerValue = MEulerRotation::kXYZ; 	break;
			case ROTATE_ORDER_YZX:
				constraintRotateOrderEulerValue = MEulerRotation::kYZX; 	break;
			case ROTATE_ORDER_ZXY:
				constraintRotateOrderEulerValue = MEulerRotation::kZXY; 	break;
			case ROTATE_ORDER_XZY:
				constraintRotateOrderEulerValue = MEulerRotation::kXZY; 	break;
			case ROTATE_ORDER_YXZ:
				constraintRotateOrderEulerValue = MEulerRotation::kYXZ; 	break;
			case ROTATE_ORDER_ZYX:
				constraintRotateOrderEulerValue = MEulerRotation::kZYX; 	break;
			default:
				constraintRotateOrderEulerValue = MEulerRotation::kXYZ; 	break;
		}
		// constraint initial rotate
		MDataHandle inputConstraintInitialRotateData = data.inputValue(constraintInitialRotate);
		double3 &constraintInitialRotateData = inputConstraintInitialRotateData.asDouble3();

		MEulerRotation initialRotationEuler(constraintInitialRotateData[0],constraintInitialRotateData[1],constraintInitialRotateData[2],
			constraintRotateOrderEulerValue);
		MQuaternion initialRotationQuat;
		initialRotationQuat = initialRotationEuler;

		MQuaternion targetRotationQuat;
		stat = targetRotation(targetsPlug,inputTargetArrayData,initialRotationQuat,targetRotationQuat);
		PERRORfail("OrientLikeNode::compute target rotation value");

		MEulerRotation targetRotationEuler = targetRotationQuat.asEulerRotation();
		targetRotationEuler.reorder(constraintRotateOrderEulerValue);
		outputConstraintRotateData.set(targetRotationEuler[0],targetRotationEuler[1],targetRotationEuler[2]);
		data.setClean( plug );
	} 
	else 
	{
		stat  = MS::kUnknownParameter;
	}
	return stat;
}

// The setDependentsDirty() method allows attributeAffects relationships
// in a much more general way than via MPxNode::attributeAffects
// which is limited to static attributes only.
// The setDependentsDirty() method allows relationships to be established
// between any combination of dynamic and static attributes.
//
// Within a setDependentsDirty() implementation you get passed in the
// plug which is being set dirty, and then, based upon which plug it is,
// you may choose to dirty any other plugs by adding them to the
// affectedPlugs list.
//
// In almost all cases, the relationships you set up will be fixed for
// the duration of Maya, such as "A affects B". However, you can also
// set up relationships which depend upon some external factor, such
// as the current frame number, the time of day, if maya was invoked in
// batch mode, etc. These sorts of relationships are straightforward to
// implement in your setDependentsDirty() method.
//
// There may also be situations where you need to look at values in the
// dependency graph. It is VERY IMPORTANT that when accessing DG values
// you do not cause a DG evaluation. This is because your setDependentsDirty()
// method is called during dirty processing and causing an evalutaion could
// put Maya into an infinite loop. The only safe way to look at values
// on plugs is via the MDataBlock::outputValue() which does not trigger
// an evaluation. It is recommeneded that you only look at plugs whose
// values are constant or you know have already been computed.
//
// For this example routine, we will only implement the simplest case
// of a relationship.
//
MStatus OrientLikeNode::setDependentsDirty( const MPlug &plugBeingDirtied, MPlugArray &affectedPlugs )
{
	MStatus	status;
	MObject thisNode = thisMObject();
	MFnDependencyNode fnThisNode( thisNode );

	if (plugBeingDirtied.isConnected(&status))
	{
		MPlugArray connectedTo;
		if (plugBeingDirtied.connectedTo(connectedTo,false,true,&status))
		{
			for (unsigned connectionIndex = 0; connectionIndex < connectedTo.length(); connectionIndex++)
			{
				if (connectedTo[connectionIndex].node() == thisNode)
				{
					affectedPlugs.append( connectedTo[connectionIndex] );
				}
			}
		}
	}
	return( MS::kSuccess );
}


void *OrientLikeNode::creator()
{
return new OrientLikeNode();
}

MStatus OrientLikeNode::initialize()
{
	MStatus stat;

	MFnCompoundAttribute cAttr;
	MFnNumericAttribute nAttr;
	MFnMatrixAttribute mAttr;
	MFnEnumAttribute eAttr;
	MFnUnitAttribute uAttr;

	targetRotateX = uAttr.create("targetRotateX","trx",MFnUnitAttribute::kAngle,0.0,&stat);
	PERRORfail("OrientLikeNode::initialize error creating targetRotateX attribute");
	targetRotateY = uAttr.create("targetRotateY","try",MFnUnitAttribute::kAngle,0.0,&stat);
	PERRORfail("OrientLikeNode::initialize error creating targetRotateY attribute");
	targetRotateZ = uAttr.create("targetRotateZ","trz",MFnUnitAttribute::kAngle,0.0,&stat);
	PERRORfail("OrientLikeNode::initialize error creating targetRotateZ attribute");
	targetRotate = nAttr.create("targetRotate","tr",targetRotateX,targetRotateY,targetRotateZ,&stat);
	PERRORfail("OrientLikeNode::initialize error creating targetRotate attribute");

	targetRotateOrder = eAttr.create( "targetRotateOrder", "tro", ROTATE_ORDER_XYZ,&stat);
	PERRORfail("OrientLikeNode::initialize error creating targetRotateOrder attribute");
	eAttr.addField("xyz", ROTATE_ORDER_XYZ);
	eAttr.addField("yzx", ROTATE_ORDER_YZX);
	eAttr.addField("zxy", ROTATE_ORDER_ZXY);
	eAttr.addField("xzy", ROTATE_ORDER_XZY);
	eAttr.addField("yxz", ROTATE_ORDER_YXZ);
	eAttr.addField("zyx", ROTATE_ORDER_ZYX);

	targetPercent = nAttr.create("targetPercent","tp",MFnNumericData::kDouble,1.0);

	target = cAttr.create("target","tg");
//	cAttr.setHidden(true);
	cAttr.setArray(true);

	cAttr.addChild(targetRotate);
	cAttr.addChild(targetRotateOrder);
	cAttr.addChild(targetPercent);

	constraintRotateX = uAttr.create("constraintRotateX","crx",MFnUnitAttribute::kAngle,0.0);
	constraintRotateY = uAttr.create("constraintRotateY","cry",MFnUnitAttribute::kAngle,0.0);
	constraintRotateZ = uAttr.create("constraintRotateZ","crz",MFnUnitAttribute::kAngle,0.0);
	constraintRotate = nAttr.create("constraintRotate","cr",constraintRotateX,constraintRotateY,constraintRotateZ);

	constraintRotateOrder = eAttr.create( "constraintRotateOrder", "cro", ROTATE_ORDER_XYZ);
	eAttr.addField("xyz", ROTATE_ORDER_XYZ);
	eAttr.addField("yzx", ROTATE_ORDER_YZX);
	eAttr.addField("zxy", ROTATE_ORDER_ZXY);
	eAttr.addField("xzy", ROTATE_ORDER_XZY);
	eAttr.addField("yxz", ROTATE_ORDER_YXZ);
	eAttr.addField("zyx", ROTATE_ORDER_ZYX);
	eAttr.setHidden(true);

	constraintInitialRotateX = uAttr.create("constraintInitialRotateX","cirx",MFnUnitAttribute::kAngle,0.0);
	constraintInitialRotateY = uAttr.create("constraintInitialRotateY","ciry",MFnUnitAttribute::kAngle,0.0);
	constraintInitialRotateZ = uAttr.create("constraintInitialRotateZ","cirz",MFnUnitAttribute::kAngle,0.0);
	constraintInitialRotate = nAttr.create("constraintInitialRotate","cir",constraintInitialRotateX,constraintInitialRotateY,constraintInitialRotateZ);
	nAttr.setHidden(false);

	addAttribute( target );
	addAttribute( constraintRotate );
	addAttribute( constraintRotateOrder );
	addAttribute( constraintInitialRotate );

	attributeAffects( target, constraintRotate );
	attributeAffects( constraintRotateOrder, constraintRotate );
	attributeAffects( constraintInitialRotate, constraintRotate );

	return MS::kSuccess;
}

void OrientLikeNode::QatSlerpBetter(const MQuaternion &from,const MQuaternion &to, double t, MQuaternion &result)
{
	MQuaternion qTemp;
	double mag1, mag2;

	mag1 = ((from.x + to.x) * (from.x + to.x)) +
			 ((from.y + to.y) * (from.y + to.y)) +
			 ((from.z + to.z) * (from.z + to.z)) +
			 ((from.w + to.w) * (from.w + to.w));

	mag2 = ((from.x - to.x) * (from.x - to.x)) +
			 ((from.y - to.y) * (from.y - to.y)) +
			 ((from.z - to.z) * (from.z - to.z)) +
			 ((from.w - to.w) * (from.w - to.w));

	if (mag1 > mag2)
	{
		QatSlerp(from, to, t, result);
	}
	else
	{
		qTemp.x = -to.x;
		qTemp.y = -to.y;
		qTemp.z = -to.z;
		qTemp.w = -to.w;
		QatSlerp(from, qTemp, t, result);
	}
}


void OrientLikeNode::QatSlerp(const MQuaternion &from,const MQuaternion &to, double t, MQuaternion &result)
{
	double omega, cosom, sinom, scaleFrom, scaleTo;

	cosom = (from.x * from.x) + (from.y * from.y) + (from.z * from.z) + (from.w * to.w);

	if ((1.0 + cosom) > EPSILON)
	{
		if ((1.0 - cosom) > EPSILON)
		{
			omega = acos(cosom);
			sinom = sin(omega);
			scaleFrom = sin((1.0 - t) * omega) / sinom;
			scaleTo = sin(t * omega) / sinom;
		}
		else
		{
			scaleFrom = 1.0 - t;
			scaleTo = t;
		}
		result.x = (scaleFrom * from.x) + (scaleTo * to.x);
		result.y = (scaleFrom * from.y) + (scaleTo * to.y);
		result.z = (scaleFrom * from.z) + (scaleTo * to.z);
		result.w = (scaleFrom * from.w) + (scaleTo * to.w);
	}
	else
	{
		result.x = -from.y;
		result.y = from.x;
		result.z = -from.w;
		result.w = from.z;

		scaleFrom = sin((1.0 - t) * HALFPI);
		scaleTo = sin(t * HALFPI);

		result.x = (scaleFrom * from.x) + (scaleTo * result.x);
		result.y = (scaleFrom * from.y) + (scaleTo * result.y);
		result.z = (scaleFrom * from.z) + (scaleTo * result.z);
		//yuch-- loop in textbook is i = 0; i < 3, though it seems hard to believe
		//		pResult->W   = (float) ((fScaleFrom * pFrom->W  ) + (fScaleTo * pResult->W  ));
	}
}
