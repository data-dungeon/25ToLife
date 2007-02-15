#include "stdafx.h"
#include "smoothNormals.h"
#include "neighborTree.h"

smoothNormals::smoothNormals() {}

smoothNormals::~smoothNormals() {}

void* smoothNormals::creator()
{
	return new smoothNormals();
}

bool smoothNormals::isUndoable() const
{
    return false;
}

MStatus smoothNormals::undoIt()
{
    return MS::kSuccess;
}

MStatus smoothNormals::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus smoothNormals::redoIt()
{
	MStatus status;
	MSelectionList selection;
	MDGModifier dgModifier;
	MString command;

	status = MGlobal::getActiveSelectionList(selection);

	if (selection.length()==0)
	{
		DISPLAY_ERROR("Nothing selected.");
		return MS::kFailure;
	}

	for (int i = 0; i < (int)selection.length(); i++)
	{
		NeighborTree ntree;

		MDagPath dagpath;
		MObject component;

		selection.getDagPath(i,dagpath,component);

		ntree.GenerateNeighborInfo(i);
		
		int numPolygons = ntree.NumNeighborFaces();

		MObject object = dagpath.node();

		if (dagpath.hasFn(MFn::kTransform))
			object = dagpath.child(0);

		if (!object.hasFn((MFn::kMesh)))
			return MS::kFailure;

		MFnDependencyNode depnode(object);
		const char*debug = depnode.name().asChar();
		
		MFnMesh fnMesh( object, &status );
		
		int numVertices = ntree.NumVertices();

		ntree.CalcNormals(true);

		std::vector<Vector> *pVertexNormals = ntree.GetVertexNormals();

		MIntArray polycounts,polyconnects;

		ntree.GetPolyCounts(polycounts,false);
		ntree.GetPolyConnects(polyconnects);

	//	Assign Normals
		int numNormals = pVertexNormals->size();
		if (numNormals>0)
		{

			MVectorArray normals;
			MIntArray vids;

			IntMap visited;
			int count = 0,vidx;
			MVector normal;
			for (int i = 0; i < (int)polycounts.length(); i++)
			{
				int numVerts = polycounts[i];
				for (unsigned j = 0; j < (unsigned)numVerts; j++)
				{
					if (!visited.HasKey(polyconnects[count+j]))
					{
						vidx = polyconnects[count+j];

						Vector *vect = &(*pVertexNormals)[vidx];
						normal = MVector(vect->x,vect->y,vect->z);

						vids.append(vidx);
						normals.append(normal);

						visited.SetValue(polyconnects[count+j],1);
					}
				}
				count += numVerts;
			}

			status = fnMesh.setVertexNormals(normals,vids,MSpace::kObject);
			if (!status)
				MGlobal::displayError("Failed to assign normals.");
		
			dgModifier.doIt();

			fnMesh.updateSurface();
		}
	}
	return MS::kSuccess;
}

