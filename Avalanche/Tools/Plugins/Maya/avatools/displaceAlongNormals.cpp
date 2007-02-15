#include "stdafx.h"
#include "displaceAlongNormals.h"
#include "neighborTree.h"

displaceAlongNormals::displaceAlongNormals() {}

displaceAlongNormals::~displaceAlongNormals() {}

void* displaceAlongNormals::creator()
{
	return new displaceAlongNormals();
}

bool displaceAlongNormals::isUndoable() const
{
    return false;
}

MStatus displaceAlongNormals::undoIt()
{
    return MS::kSuccess;
}

MStatus displaceAlongNormals::doIt( const MArgList& args )
{
	MStatus status;

	m_distance = 0.01f;

	if (args.length() > 0)
		args.get(0,m_distance);

	status = redoIt();	

	return status;
}

MStatus displaceAlongNormals::redoIt()
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

		std::vector<Vector> *pVertices = ntree.GetVertices();
		std::vector<Vector> *pVertexNormals = ntree.GetVertexNormals();

		MIntArray polycounts,polyconnects;

		ntree.GetPolyCounts(polycounts,false);
		ntree.GetPolyConnects(polyconnects);


		// displace vertices
		int numNormals = pVertexNormals->size();
		if (numNormals>0)
		{
			IntMap visited;
			int count = 0,vidx;
			MVector vertexnormal;
			for (int i = 0; i < (int)polycounts.length(); i++)
			{
				int numVerts = polycounts[i];
				for (unsigned j = 0; j < (unsigned)numVerts; j++)
				{
					if (!visited.HasKey(polyconnects[count+j]))
					{
						vidx = polyconnects[count+j];

						Vector *vect = &(*pVertexNormals)[vidx];
						vertexnormal = MVector(vect->x,vect->y,vect->z);
						vertexnormal.normalize();
						vertexnormal *= m_distance;
						Vector *vect2 = &(*pVertices)[vidx];
						MVector newpos = vertexnormal + MVector(vect2->x,vect2->y,vect2->z);
						
						status = fnMesh.setPoint(vidx,newpos,MSpace::kObject);
						if (!status)
						{
							MGlobal::displayError("Failed to set vertices.");
							return MS::kFailure;
						}

						visited.SetValue(polyconnects[count+j],1);
					}
				}
				count += numVerts;
			}

			dgModifier.doIt();

			fnMesh.updateSurface();
		}
	}
	return MS::kSuccess;
}

