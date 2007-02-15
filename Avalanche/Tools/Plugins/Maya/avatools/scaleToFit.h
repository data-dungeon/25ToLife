//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _scaleToFit
#define _scaleToFit

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>

class scaleToFit : public MPxCommand {
public:
	scaleToFit ();
	~scaleToFit (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

	MVector calcPolyUV(unsigned vert,unsigned col,unsigned row,unsigned maxcol,unsigned maxrow);
};

#endif