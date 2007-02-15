//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _duplicatePolygons
#define _duplicatePolygons

class MString;
class MStringArray;

class duplicatePolygons : public MPxCommand {
public:
	duplicatePolygons ();
	~duplicatePolygons (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
	MStringArray origObjs, splitstring, dupObjs;
};

#endif