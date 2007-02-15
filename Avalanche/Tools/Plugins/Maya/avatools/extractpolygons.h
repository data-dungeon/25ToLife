//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _extractPolygons
#define _extractPolygons

class MString;
class MStringArray;

class extractPolygons : public MPxCommand {
public:
	extractPolygons ();
	~extractPolygons (); 

	MStatus	doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();

private:
	MStringArray origObjs, splitstring, dupObjs;
};

#endif