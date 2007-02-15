//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _setFarClipToBBox
#define _setFarClipToBBox

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>

class setFarClipToBBox : public MPxCommand {
public:
	setFarClipToBBox ();
	~setFarClipToBBox (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
};

#endif