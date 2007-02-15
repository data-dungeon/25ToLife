#include "stdafx.h"
#include "extractPolygons.h"
#include "invertSelect.h"

#ifndef TEMPLATE_SPECIALIZATION
unsigned int GetHashValue(const MDagPath& entry)
#else
template<> unsigned int GetHashValue<MDagPath> (const MDagPath& entry)
#endif
{
	TupString tempString = entry.fullPathName().asChar();
	return GetHashValue(tempString);
}

extractPolygons::extractPolygons() {}

extractPolygons::~extractPolygons() {}

void* extractPolygons::creator()
{
	return new extractPolygons();
}

bool extractPolygons::isUndoable() const
{
    return true;
}

MStatus extractPolygons::undoIt()
{
	MDGModifier dgModifier;

    return MS::kSuccess;
}

MStatus extractPolygons::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus extractPolygons::redoIt()
{
	MStatus status;
	MDGModifier dgModifier;
	MString command;

	MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList iter( slist );

	Keeper<MDagPath> meshesToDo;
	TupArray<TupArray<bool> > facesToDo;

	bool bFacesSelected = false;

	// Selection list loop
	for ( ; !iter.isDone(); iter.next() )
	{	 
		// get the selected node
		MObject mObject;
		iter.getDependNode(mObject);
		MFn::Type type = mObject.apiType();

		if (type == MFn::kMesh)
		{
			MDagPath objectPath;
			MObject component;
			iter.getDagPath(objectPath,component);

			MFn::Type componentType = component.apiType();
			if (componentType!=MFn::kMeshPolygonComponent)
				continue;

			int oldKeeperSize = meshesToDo.GetSize();
			int meshIndex = meshesToDo.Add(objectPath);

			// just added?
			if (meshIndex==oldKeeperSize)
			{
				facesToDo.SetSize(meshesToDo.GetSize());

				MFnMesh fnMesh(objectPath);
				int numPolygons = fnMesh.numPolygons();
				facesToDo[meshIndex].SetSize(numPolygons);
				for (int faceIndex=0;faceIndex<numPolygons;faceIndex++)
				{
					facesToDo[meshIndex][faceIndex] = false;
				}
			}

			MItMeshPolygon polyIter(objectPath,component);
			for (;!polyIter.isDone();polyIter.next())
			{
				facesToDo[meshIndex][polyIter.index()] = true;
				bFacesSelected = true;
			}
		}
	}

	MSelectionList finalSelectionList;

	if (!bFacesSelected)
	{
		DISPLAY_ERROR("extractPolygons: Nothing selected.");
		return MS::kFailure;
	}

	int numMeshes = meshesToDo.GetSize();
	for (int meshIndex=0;meshIndex<numMeshes;meshIndex++)
	{
		int numFacesToExtract = 0;

		int faceIndex;
		int numPolysInMesh = facesToDo[meshIndex].GetSize();

		for (faceIndex=0;faceIndex<numPolysInMesh;faceIndex++)
		{
			if (facesToDo[meshIndex][faceIndex])
			{
				numFacesToExtract++;
			}
		}
		if (numFacesToExtract == numPolysInMesh)
		{
			DISPLAY_INFO(MString("extractPolygons: All polygons selected in ") + meshesToDo[meshIndex].fullPathName() + " - skipping");
			continue;
		}
		else if (!numFacesToExtract)
		{
			DISPLAY_ERROR(MString("extractPolygons: No polygons selected in ") + meshesToDo[meshIndex].fullPathName() + " - skipping");
			continue;
		}

		MStringArray newTransformString;
		MDagPath sourceTransformPath = meshesToDo[meshIndex];
		sourceTransformPath.pop();

		// make a copy of the mesh and transform
		MGlobal::executeCommand(MString("duplicate ") + sourceTransformPath.fullPathName(), newTransformString);
		dgModifier.doIt();

		TupString transFormString(newTransformString[0].asChar());
		MDagPath newTransformPath;
		MTools::GetMDagPath(newTransformString[0],newTransformPath);
		TupString transFormPath(newTransformPath.fullPathName().asChar());

		// place just below the source in the outliner
		int sourceTransformPos = MTools::GetParentOrderIndex(sourceTransformPath);
		int newTransformPos = MTools::GetParentOrderIndex(newTransformPath);
		int moveRel = (sourceTransformPos-newTransformPos)+1;
		if (sourceTransformPos!=-1 && newTransformPos!=-1 && moveRel!=0)
		{
			MGlobal::executeCommand(MString("reorder -r ") + moveRel + MString(" ") + newTransformPath.fullPathName());
			dgModifier.doIt();
		}

		MSelectionList sourceSelectList;

		// DELETE SELECTED FACES ON ORIGINAL OBJECT
		// build a selection list for the source faces
		MItMeshPolygon polyIter(meshesToDo[meshIndex]);
		for (faceIndex=0;faceIndex<numPolysInMesh;faceIndex++)
		{
			if (facesToDo[meshIndex][faceIndex])
			{
				int prevIndex;
				polyIter.setIndex(faceIndex,prevIndex);
				sourceSelectList.add(meshesToDo[meshIndex],polyIter.polygon());
			}
		}
		// select
		status = MGlobal::setActiveSelectionList(sourceSelectList);
		FATAL_ERROR(status,"Error seting active selection list");

		MGlobal::executeCommand(MString("delete"));
		dgModifier.doIt();

		MSelectionList destSelectList;

		// build a selection list for the dest faces (faces that were not selected)
		polyIter.reset(newTransformPath);
		for (faceIndex=0;faceIndex<numPolysInMesh;faceIndex++)
		{
			if (!facesToDo[meshIndex][faceIndex])
			{
				int prevIndex;
				polyIter.setIndex(faceIndex,prevIndex);
				destSelectList.add(newTransformPath,polyIter.polygon());
			}
		}

		// select
		status = MGlobal::setActiveSelectionList(destSelectList);
		FATAL_ERROR(status,"Error seting active selection list");

		MGlobal::executeCommand(MString("delete"));
		dgModifier.doIt();

		// add the new faces to our final selection
		polyIter.reset(newTransformPath);
		MFnMesh fnMesh(newTransformPath);
		int numPolygons = fnMesh.numPolygons();
		for (faceIndex=0;faceIndex<numPolygons;faceIndex++)
		{
			int prevIndex;
			polyIter.setIndex(faceIndex,prevIndex);
			finalSelectionList.add(newTransformPath,polyIter.polygon());
		}
	}

	// select all the new faces
	status = MGlobal::setActiveSelectionList(finalSelectionList);
	FATAL_ERROR(status,"Error seting active selection list");

	return MS::kSuccess;
}
