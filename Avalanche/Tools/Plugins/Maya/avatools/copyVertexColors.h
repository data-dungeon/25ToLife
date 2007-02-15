//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		1 - select by name
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _copyVertexColors
#define _copyVertexColors

class CompareMeshesManager;

class copyVertexColors : public MPxCommand 
{
	
public:
	copyVertexColors ();
	~copyVertexColors ();
	
	MStatus		doIt( const MArgList& args );
	MStatus     redoIt();
	MStatus     undoIt();
	bool        isUndoable() const;
	
	static void*	creator();
	
};

#endif