#include "stdafx.h"
#include "setCameraToView.h"

setCameraToView::setCameraToView() {}

setCameraToView::~setCameraToView() {}

void* setCameraToView::creator()
{
	return new setCameraToView();
}

bool setCameraToView::isUndoable() const
{
    return false;
}

MStatus setCameraToView::undoIt()
{
    return MS::kSuccess;
}

MStatus setCameraToView::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length()==0)
		return MS::kFailure;

	args.get(0,m_cameraview);

	status = redoIt();	

	return status;
}

#define DEGTORAD  (3.14 / 180.0)
#define RADTODEG  (180.0 / 3.14)

MStatus setCameraToView::redoIt()
{
	MStatus status;

	MVector newcameravect;
	MVector yup;

	MEulerRotation euler;

	euler = MEulerRotation::identity;

	switch(m_cameraview)
	{
	case 0://front
		{
			newcameravect = MVector(0,0,-1);
			euler.x = 0.0;
			euler.y = 0.0;
			euler.z = 0.0;
		}
		break;
	case 1://back
		{
			newcameravect = MVector(0,0,1);
			euler.x = 0.0;
			euler.y = 180.0 * DEGTORAD;
			euler.z = 0.0;
		}
		break;
	case 2://left
		{
			newcameravect = MVector(1,0,0);
			euler.x = 0.0;
			euler.y = -90.0 * DEGTORAD;
			euler.z = 0.0;
		}
		break;
	case 3://right
		{
			newcameravect = MVector(-1,0,0);
			euler.x = 0.0;
			euler.y = 90.0 * DEGTORAD;
			euler.z = 0.0;
		}
		break;
	case 4://top
		{
			newcameravect = MVector(0,-1,0);
			euler.x = -90.0 * DEGTORAD;
			euler.y = 0.0;
			euler.z = 0.0;
		}
		break;
	case 5://bottom
		{
			newcameravect = MVector(0,1,0);
			euler.x = 90.0 * DEGTORAD;
			euler.y = 0.0;
			euler.z = 0.0;
		}
		break;
	default:
		{
			return MS::kFailure;
		}
	}

	M3dView view = M3dView::active3dView(&status);
	if (!status)
		return MS::kFailure;
	
	MDagPath cameradagpath;
	status = view.getCamera(cameradagpath);
	if (!status)
		return MS::kFailure;

	MFnCamera camera(cameradagpath,&status);
	if (!status)
		return MS::kFailure;

	MPoint target = camera.centerOfInterestPoint(MSpace::kWorld,&status);
	if (!status)
		return MS::kFailure;

	MPoint pos = camera.eyePoint(MSpace::kWorld,&status);
	if (!status)
		return MS::kFailure;

	// get new eye point position

	MVector oldcameravect = target-pos;

	MVector newcameravect2 = newcameravect * oldcameravect.length();
	
	newcameravect2 = target - newcameravect2;

	status = camera.setEyePoint(MPoint(newcameravect2),MSpace::kWorld);
	if (!status)
		return MS::kFailure;

	// set appropriate rotation

	MFnTransform cameratransform(cameradagpath.transform());

	cameratransform.setRotation(euler);


	camera.setParallelView(false);
	
	return MS::kSuccess;
}


