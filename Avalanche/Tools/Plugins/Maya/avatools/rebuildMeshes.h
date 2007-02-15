//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _rebuildMeshes
#define _rebuildMeshes

class rebuildMeshes : public MPxCommand
{
public:
	rebuildMeshes ();
	~rebuildMeshes (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif