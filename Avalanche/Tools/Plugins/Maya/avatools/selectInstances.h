//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		0 - select by using maya instancing
//		1 - select by comparing meshes (ignoring vertex colors)
//		2 - select by comparing meshes (as well as vertex colors)
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _selectInstances
#define _selectInstances

class CompareMeshesManager;

class selectInstances : public MPxCommand 
{
protected:
	enum SelectionTypes
	{
		MAYA_INSTANCES,
		IDENTICAL_MESH,
		IDENTICAL_MESH_VC,
	};

	int m_selectionType;

public:
	selectInstances ();
	~selectInstances ();

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
	void CheckNode(MDagPath &dagPath,MDagPathArray &dagPathArray,CompareMeshesManager &compareMeshes);
	bool ValidNodeName(const char *pName);
	bool ValidNode(MDagPath &mDagPath);
	bool ValidMesh(MDagPath &mDagPath);



};

#endif