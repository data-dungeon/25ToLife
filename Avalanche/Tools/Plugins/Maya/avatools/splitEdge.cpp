#include "stdafx.h"
#include "splitEdge.h"
#include "mergeEdge.h"

splitEdge::splitEdge() {}

splitEdge::~splitEdge() {}

void* splitEdge::creator()
{
	return new splitEdge();
}

bool splitEdge::isUndoable() const
{
    return false;
}

MStatus splitEdge::undoIt()
{
    return MS::kSuccess;
}

MStatus splitEdge::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus splitEdge::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;
	MSelectionList selection;

	MStringArray verts, edges, faces;

	MString objectName;
	
	// store edges
	MGlobal::executeCommand("ls -sl",edges);
	dgModifier.doIt();

	if (edges.length() == 0) {
		FATAL_ERROR(status,"edgeSplit: Nothing selected.");
	}

	MStringArray temp;
	edges[0].split('.',temp);
	objectName = temp[0];

	// get vertex table
	MObject object;
	MPointArray positionsA,positionsB;
	std::vector<myEdge> edgesA,edgesB;
	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[edgeSplit] Couldn't get active selection list.");
	selection.getDependNode(0,object);
	MFnMesh fnMesh(object, &status);
	FATAL_ERROR(status,"[edgeSplit] Failure in MFnMesh initialization.");
	fnMesh.getPoints(positionsA,MSpace::kWorld);
	
	// convert selection to faces
	MGlobal::executeCommand("PolySelectConvert 1");
	dgModifier.doIt();

	// store faces selected
	MGlobal::executeCommand("ls -sl",faces);
	dgModifier.doIt();
	
	// reselect edges
	command = "select -r ";
	for (unsigned i = 0; (unsigned)i < edges.length(); i++)
	{
		command += edges[i] + " ";
	}
	MGlobal::executeCommand(command);
	dgModifier.doIt();

	GetEdges(edgesA,positionsA);

	// convert selection to vertices
	MGlobal::executeCommand("PolySelectConvert 3");
	dgModifier.doIt();

	// store vertices selected
	MGlobal::executeCommand("ls -sl",verts);
	dgModifier.doIt();

	// split vertices
	command = "polySplitVertex -ch 1 ";
	for (i = 0; i < (unsigned)verts.length(); i++)
	{
		command += verts[i] + " ";
	}
	MGlobal::executeCommand(command);
	dgModifier.doIt();
	
	// get vertex table
	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[splitEdge] Couldn't get active selection list.");
	selection.getDependNode(0,object);
	MFnMesh fnMesh2(object, &status);
	FATAL_ERROR(status,"[splitEdge] Failure in MFnMesh initialization.");
	fnMesh2.getPoints(positionsB,MSpace::kWorld);

	// convert selection to edges
	MGlobal::executeCommand("PolySelectConvert 2");
	dgModifier.doIt();

	GetEdges(edgesB,positionsB);

	int debug0 = edgesA.size();
	int debug1 = edgesB.size();


	// GET INNER VERTICES OF EDGE FOR MOVING

	MIntArray moveVerts;
	int *hits = new int[positionsA.length()];
	memset(hits,0,positionsA.length());
	for (unsigned j = 0; j < edgesA.size(); j++)
	{
		hits[edgesA[j].v0] = 1;
		hits[edgesA[j].v1] = 1;
	}
	moveVerts.clear();
	for (j = 0; j < positionsA.length(); j++)
	{
		if (hits[j]>1)
			moveVerts.append(j);
	}
	
	for (j = 0; j < (unsigned)moveVerts[j]; j++)
	{
		std::cerr << j << ") " << moveVerts[j] << std::endl;
	}


	MString commandA,commandB,commandC = "select -r ";
	commandA = "polyMergeVertex -ch 0 -tx 1 -d 0.01 ";
	commandB = "";

	for (unsigned a = 0; a < edgesB.size(); a++)
	{
		int debug0 = edgesB[a].v0;
		int debug1 = edgesB[a].v1;

		if (!DoesEdgeExist(edgesB[a],edgesA))
		{
			int debug0 = edgesB[a].v0;
			int debug1 = edgesB[a].v1;

//			commandB = "polyMergeVertex -ch 0 -tx 1 -d 0.01 ";
							
			commandB += objectName + MString(".vtx[") + edgesB[a].v0 + MString("] ");
			commandB += objectName + MString(".vtx[") + edgesB[a].v1 + MString("] ");
	
			for (unsigned b = 0; b < edgesB.size(); b++)
			{
				if (b != a)
				{
					if ((!edgesB[a].flag)&&(!edgesB[b].flag))
					{
						int debug2 = edgesB[b].v0;
						int debug3 = edgesB[b].v1;

						if (EdgesAreEqual(edgesB[a],edgesB[b]))
						{
							commandB += objectName + MString(".vtx[") + edgesB[b].v0 + MString("] ");
							commandB += objectName + MString(".vtx[") + edgesB[b].v1 + MString("] ");

							edgesB[a].flag = 1;
							edgesB[b].flag = 1;

//							MGlobal::executeCommand(commandB);
//							std::cerr << commandB << std::endl;


						}
					}
				}
			}

		}
		else
		{
			commandC += objectName + MString(".vtx[") + edgesB[a].v0 + MString("] ");
			commandC += objectName + MString(".vtx[") + edgesB[a].v1 + MString("] ");
		}
	}

//	std::cerr << "COMMAND A & COMMAND B " << commandA + commandB << std::endl;

//	MGlobal::executeCommand(commandA+commandB);
//	dgModifier.doIt();

	// select edges

	std::cerr << "COMMAND C " << commandC.asChar() << std::endl;

//	MGlobal::executeCommand(commandC);
//	dgModifier.doIt();

	MGlobal::executeCommand("select -cl");
	dgModifier.doIt();

	// convert selection to edges
	MGlobal::executeCommand("PolySelectConvert 2");
	dgModifier.doIt();


    return MS::kSuccess;
}

int splitEdge::DoesEdgeExist(myEdge e,std::vector<myEdge> &edges)
{
	for (unsigned i = 0; i < edges.size(); i++)
	{
		if (EdgesAreEqual(e,edges[i]))
		{
			return 1;
			break;
		}
	}
	return 0;
}

int splitEdge::EdgesAreEqual(myEdge a,myEdge b)
{
	if (((a.p0.distanceTo(b.p0) < 0.001)&&(a.p1.distanceTo(b.p1) < 0.001))||
		((a.p0.distanceTo(b.p1) < 0.001)&&(a.p1.distanceTo(b.p0) < 0.001)))
		return 1;
	return 0;
}

MStatus splitEdge::GetEdges(std::vector<myEdge> &edges,MPointArray &positions)
{
	MSelectionList selection;
	MStatus status;

	edges.clear();

	MObject object,component;
	MDagPath mdagpath;
	
	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[splitEdge] Couldn't get active selection list.");

	selection.getDagPath(0,mdagpath,component);

	MItMeshEdge edgeIter(mdagpath,component);

	for (;!edgeIter.isDone(); edgeIter.next())
	{
		int v0 = edgeIter.index(0);
		int v1 = edgeIter.index(1);

		myEdge e;
		e.v0 = v0;
		e.v1 = v1;
		e.p0 = positions[v0];
		e.p1 = positions[v1];
		edges.push_back(e);	
	}

	return MS::kSuccess;
}

