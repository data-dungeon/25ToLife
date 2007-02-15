//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _goToBindPose
#define _goToBindPose

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>

class goToBindPose : public MPxCommand
{
public:
	goToBindPose ();
	~goToBindPose (); 

	MStatus			doIt( const MArgList& args );
    MStatus			redoIt();
    MStatus			undoIt();
    bool			isUndoable() const;

	static void*	creator();
};

#endif