//////////////////////////////////////////////////////////////////////////////////////
//
//	ARGUMENTS:
//
//		NO ARGUMENTS
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _snapVerticesToClosest
#define _snapVerticesToClosest

class snapVerticesToClosest : public MPxCommand
{
public:
	snapVerticesToClosest ();
	~snapVerticesToClosest (); 

	MStatus		doIt( const MArgList& args );
    MStatus     redoIt();
    MStatus     undoIt();
    bool        isUndoable() const;

	static void*	creator();
public:
	double m_tolerance;
};

#endif