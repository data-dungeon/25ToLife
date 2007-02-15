#include "stdafx.h"
#include "selectAllByType.h"
#include "CompareMeshes.h"

selectAllByType::selectAllByType() {}

selectAllByType::~selectAllByType() {}

void* selectAllByType::creator()
{
	return new selectAllByType();
}

bool selectAllByType::isUndoable() const
{
    return false;
}

MStatus selectAllByType::undoIt()
{
    return MS::kSuccess;
}

MStatus selectAllByType::doIt( const MArgList& args )
{
	MStatus status;

	m_selectionFlags = 0;

	int numArgs = args.length();
	for (int argIndex=0;argIndex<numArgs;argIndex++)
	{
		MString string;
		args.get(argIndex,string);
		if (!stricmp(string.asChar(),"vc"))
			m_selectionFlags |= VC;
		if (!stricmp(string.asChar(),"novc"))
			m_selectionFlags |= NO_VC;
	}

	if (m_selectionFlags==0)
	{
		DISPLAY_INFO("No selection types specified. Use vc,novc");
		return MS::kFailure;
	}

	status = redoIt();	

	return status;
}

MStatus selectAllByType::redoIt()
{
	MStatus status;

	MDagPathArray dagpatharray;
	dagpatharray.clear();

	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid); // make an iterator (to get the first node)
	MFnDagNode rootNode(dagIterator.item());
	
	int numChildren = rootNode.childCount();
	if (numChildren)
	{
		for (int childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MFnDagNode childNode(rootNode.child(childIndex));
			MDagPath childPath;
			childNode.getPath(childPath);
			if (ValidNode(childPath) && ValidNodeName(childNode.name().asChar()))
			{
				CheckNode(childPath,dagpatharray);
			}
		}
	}

	MDGModifier dgModifier;
	MSelectionList selection;

	for (unsigned i = 0; i < dagpatharray.length(); i++)
	{
		selection.add(dagpatharray[i]);
	}

	MGlobal::setActiveSelectionList(selection);
	FATAL_ERROR(status,"scaleToFit: Couldn't set active selection list.");
	dgModifier.doIt();

	return MS::kSuccess;
}

void selectAllByType::CheckNode(MDagPath &dagPath,MDagPathArray &dagPathArray)
{
	MFnDagNode parentNode(dagPath.node());
	
	int numChildren = parentNode.childCount();
	if (numChildren)
	{
		for (int childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MFnDagNode childNode(parentNode.child(childIndex));
			MDagPath childPath;
			childNode.getPath(childPath);
			// if this is a mesh child
			if (ValidMesh(childPath))
			{
				if (CheckMesh(childPath))
				{
					dagPathArray.append(childPath);
				}
			}
			else if (ValidNode(childPath))
			{
				CheckNode(childPath,dagPathArray);
			}
		}
	}
}

static const char *invalidNodeNamesFull[] = 
{
	"groundPlane_transform",
	"persp",
	"top",
	"front",
	"side",
	NULL
};

static const char *invalidNodeNamesPart[] = 
{
	"References",
	NULL
};

bool selectAllByType::ValidNodeName(const char *pName)
{
	int index=0;
	while (invalidNodeNamesFull[index])
	{
		if (!strcmp(pName,invalidNodeNamesFull[index]))
		{
			return false;
		}
		index++;
	}
	index=0;
	while (invalidNodeNamesPart[index])
	{
		// if a portion of the name is found
		if (strstr(pName,invalidNodeNamesPart[index]))
		{
			return false;
		}
		index++;
	}
	return true;
}

bool selectAllByType::ValidNode(MDagPath &mDagPath)
{
	// make sure it is not an intermediate object
	MFnDagNode mDagNode(mDagPath);
	if (mDagNode.isIntermediateObject())
		return false;
	
	MFn::Type type = mDagPath.apiType();
	if (type == MFn::kTransform)
	{
		return true;
	}
	else if (type == MFn::kJoint)
	{
		int numChildren = mDagPath.childCount();
		// find an child joint (if there is one we are a valid joint)
		for (int childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MObject childObject = mDagPath.child(childIndex);
			MDagPath childPath = mDagPath;
			childPath.push(childObject);
			// found a child joint
			if (childPath.apiType()==MFn::kJoint)
			{
				return true;
			}
		}
	}
	return false;
}

bool selectAllByType::ValidMesh(MDagPath &mDagPath)
{
	// make sure it is not an intermediate object
	MFnDagNode mDagNode(mDagPath);
	if (mDagNode.isIntermediateObject())
		return false;
	
	MFn::Type type = mDagPath.apiType();
	if (type == MFn::kMesh)
	{
		return true;
	}
	return false;
}

bool selectAllByType::CheckMesh(MDagPath &mDagPath)
{
	MStatus status;
	MObject mComponent = MObject::kNullObj;

	MItMeshPolygon polyIter(mDagPath, mComponent, &status);
	
	if (status != MS::kSuccess) 
	{
		fprintf(stderr,"Failure in MItMeshPolygon initialization.\n");
		return false;
	}

	bool bHasVertexColors = true;
	
	// determine if this mesh has vertex colors for each point on each face or not
	int numFaces = 0;
	for ( ; !polyIter.isDone(); polyIter.next() )
	{
		int polyVertexCount = polyIter.polygonVertexCount();

		int numVertexColors;
		polyIter.numColors(numVertexColors);
		if (numVertexColors > 0)
		{
			MColorArray colors;
			polyIter.getColors(colors);
			if (colors.length() != polyVertexCount)
			{
				bHasVertexColors = false;
				break;
			}
		}
		else
		{
			bHasVertexColors = false;
			break;
		}
	}

	// return true based on the flags
	if (bHasVertexColors && (m_selectionFlags&VC))
		return true;
	if (!bHasVertexColors && (m_selectionFlags&NO_VC))
		return true;

	return false;
}
