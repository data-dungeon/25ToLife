//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _bakeAnimation
#define _bakeAnimation

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>

class bakeAnimation : public MPxCommand
{
public:
	bakeAnimation ();
	~bakeAnimation (); 

	MStatus			doIt( const MArgList& args );
    MStatus			redoIt();
    MStatus			undoIt();
    bool			isUndoable() const;

	static void*	creator();
};

#endif