#include "scaleToFit.h"
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MDGModifier.h>
#include <maya/MString.h>
#include <maya/MFloatArray.h>
#include <maya/MItSelectionList.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>

#include <iostream.h>
#include "macros.h"
#include "neighbortree.h"

scaleToFit::scaleToFit() {}

scaleToFit::~scaleToFit() {}

void* scaleToFit::creator()
{
	return new scaleToFit();
}

bool scaleToFit::isUndoable() const
{
    return false;
}

MStatus scaleToFit::undoIt()
{
    return MS::kSuccess;
}

MStatus scaleToFit::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus scaleToFit::redoIt()
{
	MStatus status;
	MSelectionList selection;
	neighborTree ntree;
	MDGModifier dgModifier;
	neighborTree::topology_flags flags;

//	if ((ui_proportional)&&(ui_welduvs)) {
//		DISPLAY_INFO("[tileUVs] Proportional mapping on. Weld uvs on.");
//	}
//	else if ((ui_proportional)&&(!ui_welduvs)) {
//		DISPLAY_INFO("[tileUVs] Proportional mapping on.");
//	}
//	else if ((!ui_proportional)&&(ui_welduvs)) {
//		DISPLAY_INFO("[tileUVs] Weld uvs on.");
//	}

	MStringArray selFaces;
	MString command;

	command = MString("filterExpand -sm 34 -ex true");
	MGlobal::executeCommand(command,selFaces);
	dgModifier.doIt();

	if (selFaces.length() == 0) {
		FATAL_ERROR(status,"scaleToFit: No polygons selected.");
	}

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"scaleToFit: Couldn't get active selection list.");

	memset(&flags,0,sizeof(neighborTree::topology_flags));
	flags.kPolygons = 1;
//	if (ui_proportional)
		flags.kVertices = 1;
	flags.kNormals = 0;
//	if (ui_welduvs)
		flags.kEdges = 1;
	flags.kUVs = 1;

	status = ntree.create(selection,flags);
	FATAL_ERROR(status,"tileUVs: Couldn't calculate neighbors.");

	MDagPath mdagpath;
	MObject component;
	unsigned numVerts;
	unsigned col,row;
	MVector uv0,uv1,uv2,uv3;

	MObject object;
	selection.getDependNode(0,object);
	MFnMesh fnMesh( object, &status );

	MFnDagNode dagNode(object);
	MObject parentObject = dagNode.parent(0,&status);
	MFnDependencyNode fnNode(parentObject);
	MString objectName = fnNode.name();

	unsigned maxcol = 0,maxrow = 0;
	ntree.getPolyMaxColAndRow(maxcol,maxrow);

//	if (ui_proportional) {
//		ntree.getDistanceLists(ulist,vlist);
//	}

	HIntArray uvidx;
	MFloatArray uidx,vidx;

	MItSelectionList iter(selection);

	for (; !iter.isDone(); iter.next() ) {
		iter.getDagPath(mdagpath,component);
		FATAL_ERROR(status,"tileUVs: Couldn't get dagpath.");

		MItMeshPolygon polyIter(mdagpath,component);

		for (;!polyIter.isDone(); polyIter.next()) {
			numVerts = polyIter.polygonVertexCount(&status);

			ntree.getPolyColAndRow(polyIter.index(),col,row);

//			if (ui_proportional) {
//				uv0 = calcPolyUVProportional(0,col,row,maxcol,maxrow);
//				uv1 = calcPolyUVProportional(1,col,row,maxcol,maxrow);
//				uv2 = calcPolyUVProportional(2,col,row,maxcol,maxrow);
//				uv3 = calcPolyUVProportional(3,col,row,maxcol,maxrow);
//			}
//			else {
				uv0 = calcPolyUV(0,col,row,maxcol,maxrow);
				uv1 = calcPolyUV(1,col,row,maxcol,maxrow);
				uv2 = calcPolyUV(2,col,row,maxcol,maxrow);
				uv3 = calcPolyUV(3,col,row,maxcol,maxrow);
//			}

			uidx.clear(); vidx.clear();

			uidx.append(uv0.x); vidx.append(uv0.y);
			uidx.append(uv1.x); vidx.append(uv1.y);
			uidx.append(uv2.x); vidx.append(uv2.y);
			uidx.append(uv3.x); vidx.append(uv3.y);

			uvidx.clear();
			uvidx.append(ntree.getUVColAndRow(0,col,row,status));
			uvidx.append(ntree.getUVColAndRow(1,col,row,status));
			uvidx.append(ntree.getUVColAndRow(2,col,row,status));
			uvidx.append(ntree.getUVColAndRow(3,col,row,status));

			for (unsigned i = 0; i < 4; i++) {
				command = MString("select -r ") + objectName + MString(".map[") + uvidx[i] + MString("]");
				MGlobal::executeCommand(command);
				dgModifier.doIt();
				command = MString("polyEditUV -relative false -uValue ") + uidx[i] + MString(" -vValue ") + vidx[i];
				MGlobal::executeCommand(command);
				dgModifier.doIt();
			}
		}
	}
	
	// collect interior edges and weld

//	if (ui_welduvs) {
//		HIntArray edges;
//		ntree.getInteriorEdges(edges);

//		command = MString("polyMapSew -ch 1 ");

//		for (unsigned i = 0; i < edges.length(); i++)
//			command += objectName + MString(".e[") + edges[i] + MString("] ");

//		MGlobal::executeCommand(command);

//		dgModifier.doIt();

//	}

	status = MGlobal::setActiveSelectionList(selection);
	FATAL_ERROR(status,"scaleToFit: Couldn't set active selection list.");
	dgModifier.doIt();

    return MS::kSuccess;
}

MVector scaleToFit::calcPolyUV(unsigned vert,unsigned col,unsigned row,unsigned maxcol,unsigned maxrow) {
	MVector uv_topleft(0,maxrow,0);
	MVector uv_lowerleft(0,0,0);
	MVector uv_topright(maxcol,maxrow,0);
	MVector uv_lowerright(maxcol,0,0);

	MVector temp;

	if (vert == 1)
		temp = uv_topleft;
	else if (vert == 0)
		temp = uv_lowerleft;
	else if (vert == 3)
		temp = uv_lowerright;
	else if (vert == 2)
		temp = uv_topright;

	float ratioU = (1.0/maxcol);
	float ratioV = (1.0/maxrow);

	float u = ((temp.x * ratioU)+ (col * ratioU));
	float v = 1-((temp.y * ratioV)+ (row * ratioV));
		
	return MVector(u,v,0);
}