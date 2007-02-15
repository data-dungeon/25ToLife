#ifndef ORIENT_LIKE_NODE_H
#define ORIENT_LIKE_NODE_H

class OrientLikeNode : public MPxTransform //Node
{
public:
	virtual MStatus compute( const MPlug& plug, MDataBlock& data );
	virtual MStatus setDependentsDirty( const MPlug &plugBeingDirtied, MPlugArray &affectedPlugs );

	static  void *creator();
	static  MStatus initialize();

	MStatus targetRotation(const MPlug &targetsPlug,MArrayDataHandle &inputTargetArrayData,const MQuaternion &initialQuat, MQuaternion &targetQuat );

	static void QatSlerpBetter(const MQuaternion &from,const MQuaternion &to, double t, MQuaternion &result);
	static void QatSlerp(const MQuaternion &from,const MQuaternion &to, double t, MQuaternion &result);


public:
	static MObject target;
	static MObject   targetRotate;
	static MObject     targetRotateX;
	static MObject     targetRotateY;
	static MObject     targetRotateZ;
	static MObject   targetRotateOrder;
	static MObject   targetPercent;

	static MObject constraintRotate;
	static MObject   constraintRotateX;
	static MObject   constraintRotateY;
	static MObject   constraintRotateZ;
	static MObject constraintRotateOrder;
	static MObject constraintInitialRotate;
	static MObject   constraintInitialRotateX;
	static MObject   constraintInitialRotateY;
	static MObject   constraintInitialRotateZ;

	static	MTypeId		id;
};

#endif
