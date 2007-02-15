#include "stdafx.h"
#include "selectInstances.h"
#include "CompareMeshes.h"

selectInstances::selectInstances() {}

selectInstances::~selectInstances() {}

void* selectInstances::creator()
{
	return new selectInstances();
}

bool selectInstances::isUndoable() const
{
    return false;
}

MStatus selectInstances::undoIt()
{
    return MS::kSuccess;
}

MStatus selectInstances::doIt( const MArgList& args )
{
	MStatus status;

	if (args.length() > 0)
		args.get(0,m_selectionType);
	else
		m_selectionType = 0;

	status = redoIt();	

	return status;
}

MStatus selectInstances::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MSelectionList selection;

	status = MGlobal::getActiveSelectionList(selection);
	FATAL_ERROR(status,"[updateInstances] Couldn't get active selection list.");

	if (selection.length()<=0)
	{
		DISPLAY_ERROR("No objects selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;
	
	selection.getDagPath(0,dagpath);

	MDagPathArray dagpatharray;
	dagpatharray.clear();

	MDagPath firstshapedagpath;

	if(!MTools::GetFirstShape(dagpath,firstshapedagpath,true))
		return MS::kFailure;

	if (m_selectionType==SelectionTypes::MAYA_INSTANCES)
	{
		dagpath.getAllPathsTo(firstshapedagpath.node(),dagpatharray);
	}
	else if (m_selectionType==SelectionTypes::IDENTICAL_MESH || m_selectionType==SelectionTypes::IDENTICAL_MESH_VC)
	{
		CompareMeshesManager compareMeshes;
		compareMeshes.AddSourceMesh(firstshapedagpath);

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
					CheckNode(childPath,dagpatharray,compareMeshes);
				}
			}
		}
	}

	MStringArray selection0;

	for (unsigned i = 0; i < dagpatharray.length(); i++)
	{
		MFnDependencyNode depnode(dagpatharray[i].node());		
		if (depnode.name() != "References")
			selection0.append(dagpatharray[i].fullPathName());
	}

	MString command = MString("select -r ") + selection0[0];
	for (i = 0; i < selection0.length(); i++)
		command += MString(" ") + selection0[i];
	MGlobal::executeCommand(command);
	dgModifier.doIt();

	return MS::kSuccess;
}

void selectInstances::CheckNode(MDagPath &dagPath,MDagPathArray &dagPathArray,CompareMeshesManager &compareMeshes)
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
				if (compareMeshes.CheckMesh(childPath,m_selectionType==SelectionTypes::IDENTICAL_MESH_VC))
				{
					dagPathArray.append(childPath);
				}
			}
			else if (ValidNode(childPath))
			{
				CheckNode(childPath,dagPathArray,compareMeshes);
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

bool selectInstances::ValidNodeName(const char *pName)
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

bool selectInstances::ValidNode(MDagPath &mDagPath)
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

bool selectInstances::ValidMesh(MDagPath &mDagPath)
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
