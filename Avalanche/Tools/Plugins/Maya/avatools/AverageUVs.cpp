#include "stdafx.h"
#include "AverageUVs.h"

AverageUVs::AverageUVs() {}

AverageUVs::~AverageUVs() {}

void* AverageUVs::creator()
{
	return new AverageUVs();
}

bool AverageUVs::isUndoable() const
{
    return false;
}

MStatus AverageUVs::undoIt()
{
    return MS::kSuccess;
}

MStatus AverageUVs::doIt( const MArgList& args )
{
	MStatus status;

	status = redoIt();	

	return status;
}

MStatus AverageUVs::redoIt()
{
	int index;
	MDGModifier dgModifier;
	MStatus status;
	MSelectionList selection;

	TupArray<Vector2>	uvArray; // pool of uv's
	TupArray<int> vertexIndexArray; // index into selected verts array
	TupArray<PolyInfo> polyInfoArray; // data for each face (uvid's)
	TupArray<SelectedVertexInfo> selectedVertsArray; // data for each selected vertex
	TupArray<int> selectedFaceArray;
	
	status = MGlobal::getActiveSelectionList(selection);
	int numSelections = selection.length();
	
	if (selection.length()==0)
	{
		DISPLAY_ERROR("Nothing selected.");
		return MS::kFailure;
	}

	MDagPath dagpath;
	MObject component;
	
	selection.getDagPath(0,dagpath,component);

	MObject shape = dagpath.node();
	
	if (dagpath.hasFn(MFn::kTransform))
		shape = dagpath.child(0);
	
	MFnMesh fnMesh(shape);

	MString selUVSet;
	fnMesh.getCurrentUVSetName( selUVSet );

	MFnDependencyNode shapedepnode(shape);
	
	MString shapename = shapedepnode.name();

	int numPolygons = fnMesh.numPolygons();
	int numVertices = fnMesh.numVertices();

	selectedFaceArray.SetSize(numPolygons);
	for (index=0;index<numPolygons;index++)
	{
		selectedFaceArray[index] = 0; // default to not selected
	}

	MFloatArray uarray,varray;
	MIntArray polyUVCounts;
	MIntArray polyUVIds;
	
	fnMesh.getUVs(uarray,varray,&selUVSet);
	fnMesh.getAssignedUVs(polyUVCounts,polyUVIds,&selUVSet);

	int numUVs = uarray.length();
	uvArray.SetSize(numUVs);
	for (index = 0; index < numUVs; index++)
		uvArray[index].Set(uarray[index],varray[index]);

	polyInfoArray.SetSize(numPolygons);
	vertexIndexArray.SetSize(numVertices);
	for (index=0;index<numVertices;index++)
		vertexIndexArray[index] = -1; // init to -1 (not selected)

	// get the selected vertices
	for ( MItSelectionList selectionIter( selection ); !selectionIter.isDone(); selectionIter.next() )
	{
		MDagPath selectionDagPath;
		selectionIter.getDagPath( selectionDagPath, component );
		MFn::Type componentType = component.apiType();
		if (componentType == MFn::kMeshVertComponent) // component is vertices
		{
			MObject vertShape = selectionDagPath.node();
		
			if (selectionDagPath.hasFn(MFn::kTransform))
				vertShape = selectionDagPath.child(0);
		
			if (vertShape!=shape)
				continue;
		
			MItMeshVertex vertexIter(dagpath,component);
			for (;!vertexIter.isDone(); vertexIter.next())
			{
				int vertexIndex = vertexIter.index();
				// if not set yet
				if (vertexIndexArray[vertexIndex]==-1)
				{
					vertexIndexArray[vertexIndex] = selectedVertsArray.GetSize();
					SelectedVertexInfo newSelectedVert;
					newSelectedVert.m_vertexIndex = vertexIndex;
					selectedVertsArray.Add(newSelectedVert);

					MIntArray connectedFaces;
					vertexIter.getConnectedFaces(connectedFaces);
					int numConnectedFaces = connectedFaces.length();
					for (index=0;index<numConnectedFaces;index++)
					{
						selectedFaceArray[connectedFaces[index]] = 1; // mark faces to be changed
					}
				}
			}
		}
		else if (componentType == MFn::kMeshEdgeComponent) // component is edges
		{
			MObject edgeShape = selectionDagPath.node();
			
			if (selectionDagPath.hasFn(MFn::kTransform))
				edgeShape = selectionDagPath.child(0);
			
			if (edgeShape!=shape)
				continue;
			
			MItMeshEdge edgeIter(dagpath,component);
			for (;!edgeIter.isDone(); edgeIter.next())
			{
				for (int edgePoint=0;edgePoint<2;edgePoint++)
				{
					int vertexIndex = edgeIter.index(edgePoint);
					// if not set yet
					if (vertexIndexArray[vertexIndex]==-1)
					{
						vertexIndexArray[vertexIndex] = selectedVertsArray.GetSize();
						SelectedVertexInfo newSelectedVert;
						newSelectedVert.m_vertexIndex = vertexIndex;
						selectedVertsArray.Add(newSelectedVert);
					}
				}
			
				MIntArray connectedFaces;
				edgeIter.getConnectedFaces(connectedFaces);
				int numConnectedFaces = connectedFaces.length();
				for (index=0;index<numConnectedFaces;index++)
				{
					selectedFaceArray[connectedFaces[index]] = 1; // mark faces to be changed
				}
			}
		}
		else if (componentType == MFn::kMeshPolygonComponent) // component is edges
		{
			MObject polyShape = selectionDagPath.node();
			
			if (selectionDagPath.hasFn(MFn::kTransform))
				polyShape = selectionDagPath.child(0);
			
			if (polyShape!=shape)
				continue;
			
			MItMeshPolygon polyIter(dagpath,component);
			for (;!polyIter.isDone(); polyIter.next())
			{
				int numVerts = polyIter.polygonVertexCount();
				int faceIndex = polyIter.index();
				
				for (index = 0; index < numVerts; index++)
				{
					int vertexIndex = polyIter.vertexIndex(index);
					if (vertexIndexArray[vertexIndex]==-1)
					{
						vertexIndexArray[vertexIndex] = selectedVertsArray.GetSize();
						SelectedVertexInfo newSelectedVert;
						newSelectedVert.m_vertexIndex = vertexIndex;
						selectedVertsArray.Add(newSelectedVert);
					}
				}
				selectedFaceArray[faceIndex] = 1; // mark faces to be changed
			}
		}
	}

	// make sure we have some
	if (selectedVertsArray.GetSize()==0)
	{
		DISPLAY_ERROR("Nothing selected.");
		return MS::kFailure;
	}

	MItMeshPolygon polyIter(dagpath);
	int currentUVId = 0;

	// go throught the polys on the mesh
	for (;!polyIter.isDone(); polyIter.next())
	{
		int faceIndex = polyIter.index();

		int numVerts = polyIter.polygonVertexCount(&status);
		if (!status)
			continue;

		PolyInfo &polyInfo = polyInfoArray[faceIndex];
		polyInfo.m_uvids.SetSize(numVerts);
		polyInfo.m_vertexIds.SetSize(numVerts);

		if (polyUVCounts[faceIndex])
		{
			for (index = 0; index < numVerts; index++)
			{
				int vertexIndex = polyInfo.m_vertexIds[index] = polyIter.vertexIndex(index);
				int selectedVertexIndex = vertexIndexArray[vertexIndex];
				// is this vertex selected and a face we will change?
				if (selectedVertexIndex!=-1 && selectedFaceArray[faceIndex])
				{
					SelectedVertexInfo &selectedVertexInfo = selectedVertsArray[selectedVertexIndex];
					
					selectedVertexInfo.m_faceIds.Add(faceIndex);
					selectedVertexInfo.m_facePointIds.Add(index);
				}
				polyInfo.m_uvids[index] = polyUVIds[currentUVId]; 
				currentUVId++;
			}
		}
		else // no id for face
		{
			for (index = 0; index < numVerts; index++)
			{
				polyInfo.m_vertexIds[index] = polyIter.vertexIndex(index);
				polyInfo.m_uvids[index] = -1;
			}
		}
	}

	// figure out the new uv's
	int numSelectedVerts = selectedVertsArray.GetSize();
	for (int selectedVertexIndex=0;selectedVertexIndex<numSelectedVerts;selectedVertexIndex++)
	{
		SelectedVertexInfo &selectedVertexInfo = selectedVertsArray[selectedVertexIndex];
		int numFacesForVert = selectedVertexInfo.m_faceIds.GetSize();
		if (numFacesForVert<2) // if not enough to worry about
			continue;

		TupArray<int> uvIds;
		uvIds.SetSize(numFacesForVert);

		Keeper<int> uniqueUVIds;

		// build the uvids as well as check if they are all using the same one
		for (index=0;index<numFacesForVert;index++)
		{
			int faceId = selectedVertexInfo.m_faceIds[index];
			int facePointId = selectedVertexInfo.m_facePointIds[index];
			int uvid = polyInfoArray[faceId].m_uvids[facePointId];
			uvIds[index] = uvid;
			uniqueUVIds.Add(uvid);
		}

		// if all are using the same id (don't bother)
		int numUniqueUVIds = uniqueUVIds.GetSize();
		if (numUniqueUVIds==1)
			continue;

		// start with first uv
		Vector2 startingUV = uvArray[uvIds[0]];
		startingUV.x = startingUV.x-((double)((int)startingUV.x));
		if (startingUV.x<0.0) startingUV.y+=1.0; // if minus put in 0-1 range
		startingUV.y = startingUV.y-((double)((int)startingUV.y));
		if (startingUV.y<0.0) startingUV.y+=1.0; // if minus put in 0-1 range

		Vector2 averageUV = startingUV;

		TupArray<Vector2> sampleUVs;
		sampleUVs.SetSize(numUniqueUVIds);
		sampleUVs[0] = startingUV;

		// add in the other uvs for getting the average
		for (index=1;index<numFacesForVert;index++)
		{
			Vector2 sampleUV = uvArray[uvIds[index]];
			sampleUV.x = sampleUV.x-((double)((int)sampleUV.x));
			if (sampleUV.x<0.0) sampleUV.y+=1.0; // if minus put in 0-1 range
			sampleUV.y = sampleUV.y-((double)((int)sampleUV.y));
			if (sampleUV.y<0.0) sampleUV.y+=1.0; // if minus put in 0-1 range

			// bring into within .5 of starting uv
			if (sampleUV.x-startingUV.x < -0.5) sampleUV.x += 1.0; 
			if (sampleUV.x-startingUV.x > 0.5) sampleUV.x -= 1.0; 
			if (sampleUV.y-startingUV.y < -0.5) sampleUV.y += 1.0; 
			if (sampleUV.y-startingUV.y > 0.5) sampleUV.y -= 1.0; 

			int uniqueUVIndex = uniqueUVIds.GetIndex(uvIds[index]);
			sampleUVs[uniqueUVIndex] = sampleUV;

			averageUV += sampleUV;
		}
		averageUV/=numFacesForVert;

		TupArray<int> newUVIds;
		newUVIds.SetSize(numUniqueUVIds);
		// calculate new uvs based on average (average is within -.5->.5 of starting uv brought to within 0-1 range)
		for (index=0;index<numUniqueUVIds;index++)
		{
			Vector2 newUV = uvArray[uniqueUVIds[index]] + (averageUV - sampleUVs[index]);
			newUVIds[index] = uvArray.GetSize();
			uvArray.Add(newUV);
		}

		// set up the new uvid on the face points
		for (index=0;index<numFacesForVert;index++)
		{
			int faceId = selectedVertexInfo.m_faceIds[index];
			int facePointId = selectedVertexInfo.m_facePointIds[index];
			int uniqueUVIndex = uniqueUVIds.GetIndex(uvIds[index]);
			polyInfoArray[faceId].m_uvids[facePointId] = newUVIds[uniqueUVIndex];
		}
	}

	uarray.clear();
	varray.clear();
	MIntArray uvconnects;
	MIntArray polycounts;

	Keeper<int> uniqueUVIds;

	for (int polyIndex = 0; polyIndex < numPolygons; polyIndex++)
	{
		PolyInfo &polyInfo = polyInfoArray[polyIndex];
		int numVerts = polyInfo.m_uvids.GetSize();

		if (polyUVCounts[polyIndex])
		{
			polycounts.append(numVerts); // update number of points for this poly (used for setting uvs)
			
			for (index = 0; index < numVerts; index++)
			{
				int uvid = polyInfo.m_uvids[index];
				int numUniqueUVIds = uniqueUVIds.GetSize();
				int uniqueIDIndex = uniqueUVIds.Add(uvid);
				// if we just added it
				if (uniqueIDIndex==numUniqueUVIds)
				{
					uarray.append((float)uvArray[uvid].x);
					varray.append((float)uvArray[uvid].y);
				}
				uvconnects.append(uniqueIDIndex);
			}
		}
		else
		{
			polycounts.append(0); // no uvs for this face
		}
	}

	fnMesh.clearUVs(&selUVSet);
	
	status = fnMesh.setUVs(uarray,varray,&selUVSet);
	if (!status)
		MGlobal::displayError("Couldn't apply uv.(1)");
	
	fnMesh.updateSurface();
	
	status = fnMesh.assignUVs(polycounts,uvconnects,&selUVSet);
	if (!status)
		MGlobal::displayError("Couldn't apply uv.(2)");
	
	fnMesh.updateSurface();
	
	fnMesh.syncObject();
	
	MGlobal::executeCommand(MString("delete -ch ") + shapename);
	dgModifier.doIt();
	
	status = MGlobal::setActiveSelectionList(selection);
		
	return MS::kSuccess;
}

