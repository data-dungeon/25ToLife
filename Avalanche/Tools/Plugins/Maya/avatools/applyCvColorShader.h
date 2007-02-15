//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _applyCvColorShader
#define _applyCvColorShader

#include <maya/MPxCommand.h>
#include <maya/MArgList.h>

class applyCvColorShader : public MPxCommand
{
public:
	applyCvColorShader ();
	~applyCvColorShader (); 

	MStatus			doIt( const MArgList& args );
    MStatus			redoIt();
    MStatus			undoIt();
    bool			isUndoable() const;

	static void*	creator();
};

#endif